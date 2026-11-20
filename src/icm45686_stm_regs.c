/*
 * icm45686_stm_regs.c
 *
 * Autors: Jan Rusnak.
 * (c) 2025 AZTech.
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <gentyp.h>
#include "sysconf.h"
#include "board.h"
#include <mmio.h>
#include "msgconf.h"
#include "criterr.h"
#include "hwerr.h"
#include "dlycnt.h"
#include "cmdln.h"
#include "gpio_hal.h"
#include "pinmux_hal.h"
#include "icm45686.h"
#include "icm45686_stm_regs.h"

#ifndef ICM45686_SENSOR_MODE
#error "ICM45686_SENSOR_MODE not defined; define it in sysconf.h"
#endif
#ifndef ICM45686_REGS_MODE
#error "ICM45686_REGS_MODE not defined; define it in sysconf.h"
#endif

#if ICM45686_SENSOR_MODE == ICM45686_REGS_MODE

#define RESET_STM_TMO_MS 5000
#define WAIT_ISR_SIG_TMO_MS 50
#define ICM45686_REPORT_TO_MSG 1U
#define ICM45686_REPORT_TO_CALLBACK 2U

#if ICM45686_REPORT_MSG_NUM == 1
#define ICM45686_REPORT_NUM_FMT " [%03u]"
#define ICM45686_REPORT_NUM_LINE_ARGS(dest, fmt, ...) report_line((dest), (fmt), msg_num, __VA_ARGS__)
#else
#define ICM45686_REPORT_NUM_FMT ""
#define ICM45686_REPORT_NUM_LINE_ARGS(dest, fmt, ...) report_line((dest), (fmt), __VA_ARGS__)
#endif

#if ICM45686_REPORT_ACCEL_SI < 0 || ICM45686_REPORT_ACCEL_SI > 3 || \
    ICM45686_REPORT_GYRO_SI < 0 || ICM45686_REPORT_GYRO_SI > 3 || \
    ICM45686_REPORT_ACCEL_LSB < 0 || ICM45686_REPORT_ACCEL_LSB > 3 || \
    ICM45686_REPORT_GYRO_LSB < 0 || ICM45686_REPORT_GYRO_LSB > 3
#error "ICM45686_REPORT_* values must be 0 (off), 1 (msg), 2 (callback), or 3 (both)"
#endif
#if ICM45686_REPORT_FN == 0 && \
    ((ICM45686_REPORT_ACCEL_SI & 2) || (ICM45686_REPORT_GYRO_SI & 2) || \
     (ICM45686_REPORT_ACCEL_LSB & 2) || (ICM45686_REPORT_GYRO_LSB & 2))
#error "ICM45686_REPORT_FN must be 1 when a text report is routed to the callback"
#endif

#if ICM45686_REPORT_FN == 1
#define ICM45686_REPORT_BUF_SIZE 192U
#endif

static TaskHandle_t tsk_hndl;
static struct icm_45686_dsc icm_45686_dsc;
static icm_45686 icm = &icm_45686_dsc;
static p_stf_t stmf;
static SemaphoreHandle_t isr_sig;
#if ICM45686_REPORT_FN == 1
static boolean_t (*report_clbk)(const char *);
static char *report_buf;
#endif
#if ICM45686_REPORT_SAMPLE_FN == 1
static boolean_t (*sample_clbk)(const struct icm_45686_sensor_data *);
#endif

static struct {
	unsigned int read_data_reg_cnt;
	unsigned int unknown_intr_cnt;
	unsigned int drdy_timeout_cnt;
	unsigned int stm_rst_cnt;
#if ICM45686_REPORT_FN == 1
	unsigned int report_fn_err_cnt;
#endif
#if ICM45686_REPORT_SAMPLE_FN == 1
	unsigned int sample_fn_err_cnt;
#endif
} stats;

static gfp_t state_sw_reset(void);
static gfp_t state_whoami_check(void);
static gfp_t set_spi_slew(void);
static gfp_t intr_cfg(void);
static gfp_t accel_gyro_cfg(void);
static gfp_t wait_intr(void);
static gfp_t ping_icm(void);
static BaseType_t int1_isr_clbk(uint32_t status);
static gfp_t state_error(void);
static void tsk(void *p);
static void report_data(struct icm_45686_sensor_data *sd) __attribute__((unused));
static void cmd_icms(void);

/**
 * init_icm45686_stm_regs
 */
void init_icm45686_stm_regs(void)
{
	icm->spi.cfg.spi_bus_id = 0;
	icm->spi.cfg.mode = SPI_HAL_MODE0;
	icm->spi.cfg.bits_trans = SPI_HAL_8_BIT_TRANS;
	icm->spi.cfg.dly_bct_ns = 0;
	icm->spi.cfg.dly_bcs_ns = 20;
	icm->spi.cfg.cs_rise = FALSE;
	icm->spi.cfg.sck_hz = 12000000;
	icm->spi.cfg.csel_num = SPI_HAL_CSEL2;
	icm->spi.cfg.csel_cont = gpio_hal_get_ctrl(ICM45686_CS_CONT_ID);
	icm->spi.cfg.csel_pin = ICM45686_CS_PIN_ID;
	init_icm_45686(icm);
#if ICM45686_REPORT_FN == 1
	if ((report_buf = pvPortMalloc(ICM45686_REPORT_BUF_SIZE)) == NULL) {
		crit_err_exit(MALLOC_ERROR);
	}
#endif
	if ((isr_sig = xSemaphoreCreateBinary()) == NULL) {
		crit_err_exit(MALLOC_ERROR);
	}
	add_command_noargs("icms", cmd_icms);
	if (xTaskCreate(tsk, "ICM45686", ICM45686_TASK_STACK_SIZE, NULL,
			ICM45686_TASK_PRIO, &tsk_hndl) != pdPASS) {
		crit_err_exit(MALLOC_ERROR);
	}
}

#if ICM45686_REPORT_FN == 1
/**
 * icm45686_stm_set_report_fn
 */
void icm45686_stm_set_report_fn(boolean_t (*report_fn)(const char *))
{
	report_clbk = report_fn;
}
#endif

#if ICM45686_REPORT_SAMPLE_FN == 1
/**
 * icm45686_stm_set_sample_fn
 */
void icm45686_stm_set_sample_fn(boolean_t (*sample_fn)(const struct icm_45686_sensor_data *))
{
	sample_clbk = sample_fn;
}
#endif

/**
 * state_sw_reset
 */
static gfp_t state_sw_reset(void)
{
	int ret;

	gpio_hal_intr_disable(ICM45686_INT1_CONT, ICM45686_INT1_PIN);
	ret = icm_45686_soft_reset(icm);
	if (ret) {
		msg(INF, "ICM45686: Soft reset error (%s)\n", hwerr_str(ret));
		return ((gfp_t) state_error);
	}
	msg(INF, "ICM45686: Soft reset done (ICM45686_REGS_MODE)\n");
	return ((gfp_t) set_spi_slew);
}

/**
 * state_whoami_check
 */
static gfp_t state_whoami_check(void)
{
	int ret;

	ret = icm_45686_whoami_check(icm);
	if (ret) {
		msg(INF, "ICM45686: whoami_check() error (%s)\n", hwerr_str(ret));
		return ((gfp_t) state_error);
	}
	return ((gfp_t) intr_cfg);
}

/**
 * set_spi_slew
 */
static gfp_t set_spi_slew(void)
{
	int ret;

	ret = icm_45686_set_spi_pads_slew(icm, ICM45686_PADS_SPI_SLEW);
	if (ret) {
		msg(INF, "ICM45686: set_spi_slew() error (%s)\n", hwerr_str(ret));
		return ((gfp_t) state_error);
	}
	return ((gfp_t) state_whoami_check);
}

/**
 * intr_cfg
 */
static gfp_t intr_cfg(void)
{
	int ret;
	struct icm_45686_int_bitmap int_cfg = {0};
	struct icm_45686_int_pin_cfg pin_cfg = {0};

	if ((ret = icm_45686_set_config_int_source(icm, ICM_45686_INT1, &int_cfg))) {
		goto err;
	}
#if ICM45686_INTR_TYPE == LOW_LEVEL_INTR_TYPE
	pin_cfg.polarity = ICM_45686_INT1_CONFIG2_POLARITY_LOW;
	pin_cfg.mode = ICM_45686_INT1_CONFIG2_MODE_LATCH;
	pin_cfg.drive = ICM_45686_INT1_CONFIG2_DRIVE_PP;
#elif ICM45686_INTR_TYPE == FALL_EDGE_INTR_TYPE
	pin_cfg.polarity = ICM_45686_INT1_CONFIG2_POLARITY_LOW;
	pin_cfg.mode = ICM_45686_INT1_CONFIG2_MODE_PULSE;
	pin_cfg.drive = ICM_45686_INT1_CONFIG2_DRIVE_PP;
#else
#error "ICM45686_INTR_TYPE unknown"
#endif
	if ((ret = icm_45686_set_config_int_pin(icm, ICM_45686_INT1, &pin_cfg))) {
		goto err;
	}
	if ((ret = icm_45686_get_int_status(icm, ICM_45686_INT1, &int_cfg))) {
		goto err;
	}
	pinmux_hal_set_func(ICM45686_INT1_CONT, ICM45686_INT1_PIN, PINMUX_HAL_FUNC_GPIO_IN);
	gpio_hal_set_pull(ICM45686_INT1_CONT, ICM45686_INT1_PIN, GPIO_HAL_PULL_NONE);
	gpio_hal_set_filter(ICM45686_INT1_CONT, ICM45686_INT1_PIN, GPIO_HAL_FILTER_NONE);
	gpio_hal_set_schmitt(ICM45686_INT1_CONT, ICM45686_INT1_PIN, TRUE);
#if ICM45686_INTR_TYPE == LOW_LEVEL_INTR_TYPE
	gpio_hal_intr_config(ICM45686_INT1_CONT, ICM45686_INT1_PIN, GPIO_HAL_INTR_LEVEL_LOW);
#elif ICM45686_INTR_TYPE == FALL_EDGE_INTR_TYPE
	gpio_hal_intr_config(ICM45686_INT1_CONT, ICM45686_INT1_PIN, GPIO_HAL_INTR_FALLING);
#else
#error "ICM45686_INTR_TYPE unknown"
#endif
	if (!gpio_hal_isr_registered(ICM45686_INT1_CONT, int1_isr_clbk)) {
		gpio_hal_isr_register(ICM45686_INT1_CONT, int1_isr_clbk);
	}
	return ((gfp_t) accel_gyro_cfg);
err:
	msg(INF, "ICM45686: intr_cfg() error (%s)\n", hwerr_str(ret));
	return ((gfp_t) state_error);
}

/**
 * accel_gyro_cfg
 */
static gfp_t accel_gyro_cfg(void)
{
	int ret;
	struct icm_45686_int_bitmap int_cfg = {0};
	struct icm_45686_int_bitmap is;

	if ((ret = icm_45686_set_accel_fsr(icm, ICM_45686_ACCEL_CONFIG0_UI_FS_SEL_4_G))) {
		goto err;
	}
	if ((ret = icm_45686_set_gyro_fsr(icm, ICM_45686_GYRO_CONFIG0_UI_FS_SEL_2000_DPS))) {
		goto err;
	}
	if ((ret = icm_45686_set_accel_freq(icm, ICM_45686_ACCEL_CONFIG0_ODR_50_HZ))) {
		goto err;
	}
	if ((ret = icm_45686_set_gyro_freq(icm, ICM_45686_GYRO_CONFIG0_ODR_50_HZ))) {
		goto err;
	}
	if ((ret = icm_45686_set_accel_ln_bw(icm, ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4))) {
		goto err;
	}
	if ((ret = icm_45686_set_gyro_ln_bw(icm, ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4))) {
		goto err;
	}
	if ((ret = icm_45686_set_accel_mode(icm, ICM_45686_PWR_MGMT0_ACCEL_MODE_LN))) {
		goto err;
	}
	if ((ret = icm_45686_set_gyro_mode(icm, ICM_45686_PWR_MGMT0_GYRO_MODE_LN))) {
		goto err;
	}
	if ((ret = icm_45686_select_accel_lp_clk(icm, ICM_45686_SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC))) {
		goto err;
	}
	/* Route DATA_READY to INT1 only after the gyroscope has started up. */
	vTaskDelay(ms_to_os_ticks((ICM_45686_GYR_STARTUP_TIME_US + 999) / 1000) + 1);
	if ((ret = icm_45686_get_int_status(icm, ICM_45686_INT1, &is))) {
		goto err;
	}
	(void) xSemaphoreTake(isr_sig, 0);
	int_cfg.ui_drdy = ICM_45686_ENABLE;
	if ((ret = icm_45686_set_config_int_source(icm, ICM_45686_INT1, &int_cfg))) {
		goto err;
	}
	gpio_hal_intr_enable(ICM45686_INT1_CONT, ICM45686_INT1_PIN);
	return ((gfp_t) wait_intr);
err:
	msg(INF, "ICM45686: accel_gyro_cfg() error (%s)\n", hwerr_str(ret));
	return ((gfp_t) state_error);
}

/**
 * wait_intr
 */
static gfp_t wait_intr(void)
{
	int ret;
	struct icm_45686_sensor_data sd;
	struct icm_45686_int_bitmap is;

	if (xSemaphoreTake(isr_sig, ms_to_os_ticks(WAIT_ISR_SIG_TMO_MS)) == pdFALSE) {
		stats.drdy_timeout_cnt++;
		return ((gfp_t) ping_icm);
	}
	if ((ret = icm_45686_get_int_status(icm, ICM_45686_INT1, &is))) {
		msg(INF, "ICM45686: get_int_status() error (%s)\n", hwerr_str(ret));
		return ((gfp_t) state_error);
	}
	if (is.ui_drdy) {
		if ((ret = icm_45686_get_reg_data(icm, &sd))) {
			msg(INF, "ICM45686: get_reg_data() error (%s)\n", hwerr_str(ret));
			return ((gfp_t) state_error);
		}
		stats.read_data_reg_cnt++;
#if ICM45686_REPORT_SAMPLE_FN == 1 || ICM45686_REPORT_ACCEL_SI != 0 || \
    ICM45686_REPORT_GYRO_SI != 0 || ICM45686_REPORT_ACCEL_LSB != 0 || \
    ICM45686_REPORT_GYRO_LSB != 0
		report_data(&sd);
#endif
	} else {
		stats.unknown_intr_cnt++;
	}
	gpio_hal_intr_enable(ICM45686_INT1_CONT, ICM45686_INT1_PIN);
	return ((gfp_t) wait_intr);
}

/**
 * ping_icm
 */
static gfp_t ping_icm(void)
{
	int ret;

	ret = icm_45686_whoami_check(icm);
	if (ret) {
		msg(INF, "ICM45686: ping_icm() error (%s)\n", hwerr_str(ret));
		return ((gfp_t) state_error);
	}
	return ((gfp_t) wait_intr);
}

/**
 * int1_isr_clbk
 */
static BaseType_t int1_isr_clbk(uint32_t status)
{
	BaseType_t tsk_wkn = pdFALSE;

#if ICM45686_INTR_TYPE == LOW_LEVEL_INTR_TYPE
	if (status & ICM45686_INT1_PIN &&
	    gpio_hal_is_intr_enabled(ICM45686_INT1_CONT, ICM45686_INT1_PIN) &&
	    gpio_hal_get_input(ICM45686_INT1_CONT, ICM45686_INT1_PIN) == GPIO_HAL_LOW) {
		gpio_hal_intr_disable(ICM45686_INT1_CONT, ICM45686_INT1_PIN);
		xSemaphoreGiveFromISR(isr_sig, &tsk_wkn);
	}
#elif ICM45686_INTR_TYPE == FALL_EDGE_INTR_TYPE
	if (status & ICM45686_INT1_PIN && gpio_hal_is_intr_enabled(ICM45686_INT1_CONT, ICM45686_INT1_PIN)) {
		gpio_hal_intr_disable(ICM45686_INT1_CONT, ICM45686_INT1_PIN);
		xSemaphoreGiveFromISR(isr_sig, &tsk_wkn);
	}
#else
#error "ICM45686_INTR_TYPE unknown"
#endif
	return (tsk_wkn);
}

/**
 * state_error
 */
static gfp_t state_error(void)
{
	msg(INF, "ICM45686: resetting state machine after error\n");
	vTaskDelay(ms_to_os_ticks(RESET_STM_TMO_MS));
	stats.stm_rst_cnt++;
	return ((gfp_t) state_sw_reset);
}

/**
 * tsk
 */
static void tsk(void *p)
{
	stmf = state_sw_reset;
	while (TRUE) {
		stmf = (p_stf_t) (*stmf)();
	}
}

#if ICM45686_REPORT_FN == 1 && \
    (ICM45686_REPORT_ACCEL_SI != 0 || ICM45686_REPORT_GYRO_SI != 0 || \
     ICM45686_REPORT_ACCEL_LSB != 0 || ICM45686_REPORT_GYRO_LSB != 0)
/**
 * report_line
 */
static void report_line(unsigned int dest, const char *fmt, ...)
{
	va_list ap;
	int n;

	if (!(dest & ICM45686_REPORT_TO_MSG) && (!(dest & ICM45686_REPORT_TO_CALLBACK) || report_clbk == NULL)) {
		return;
	}
	va_start(ap, fmt);
	n = vsnprintf(report_buf, ICM45686_REPORT_BUF_SIZE, fmt, ap);
	va_end(ap);
	if (n < 0 || (size_t) n >= ICM45686_REPORT_BUF_SIZE) {
		msg(INF, "ICM45686: report_line() formatting error\n");
		return;
	}
	if (dest & ICM45686_REPORT_TO_MSG) {
		msg(INF, "%s", report_buf);
	}
	if ((dest & ICM45686_REPORT_TO_CALLBACK) && report_clbk != NULL && !report_clbk(report_buf)) {
		stats.report_fn_err_cnt++;
	}
}
#elif ICM45686_REPORT_FN == 0
#define report_line(dest, ...) msg(INF, __VA_ARGS__)
#endif

/**
 * report_data
 */
static void report_data(struct icm_45686_sensor_data *sd)
{
#if ICM45686_REPORT_MSG_NUM == 1 && \
    (ICM45686_REPORT_ACCEL_SI != 0 || ICM45686_REPORT_GYRO_SI != 0 || \
     ICM45686_REPORT_ACCEL_LSB != 0 || ICM45686_REPORT_GYRO_LSB != 0)
	static unsigned int msg_num;
#endif
#if ICM45686_REPORT_SAMPLE_FN == 1
	if (sample_clbk != NULL && !sample_clbk(sd)) {
		stats.sample_fn_err_cnt++;
	}
#endif
#if ICM45686_REPORT_ACCEL_SI != 0 || ICM45686_REPORT_GYRO_SI != 0
	float temp_degc;

	/* Temperature: 25 degC + raw / 128 LSB/degC. */
	temp_degc = (float) 25 + ((float) sd->temp_data / 128);
#endif
#if ICM45686_REPORT_ACCEL_SI != 0
	/* Conversion assumes signed 16-bit samples at accelerometer FSR +/-4 g. */
	float accel_g[3];
	accel_g[0] = (float) (sd->accel_data[0] * 4) / 32768;
	accel_g[1] = (float) (sd->accel_data[1] * 4) / 32768;
	accel_g[2] = (float) (sd->accel_data[2] * 4) / 32768;
	ICM45686_REPORT_NUM_LINE_ARGS(ICM45686_REPORT_ACCEL_SI,
	    "ICM45686 Accel" ICM45686_REPORT_NUM_FMT ": % 8.2f % 8.2f % 8.2f g   % 6.2f degC\n",
	    accel_g[0], accel_g[1], accel_g[2], temp_degc);
#endif
#if ICM45686_REPORT_GYRO_SI != 0
	/* Conversion assumes signed 16-bit samples at gyroscope FSR +/-2000 dps. */
	float gyro_dps[3];
	gyro_dps[0] = (float) (sd->gyro_data[0] * 2000) / 32768;
	gyro_dps[1] = (float) (sd->gyro_data[1] * 2000) / 32768;
	gyro_dps[2] = (float) (sd->gyro_data[2] * 2000) / 32768;
	ICM45686_REPORT_NUM_LINE_ARGS(ICM45686_REPORT_GYRO_SI,
	    "ICM45686  Gyro" ICM45686_REPORT_NUM_FMT ": % 8.2f % 8.2f % 8.2f dps % 6.2f degC\n",
	    gyro_dps[0], gyro_dps[1], gyro_dps[2], temp_degc);
#endif
#if ICM45686_REPORT_ACCEL_LSB != 0
	ICM45686_REPORT_NUM_LINE_ARGS(ICM45686_REPORT_ACCEL_LSB,
	    "ICM45686 Accel" ICM45686_REPORT_NUM_FMT ": %+09d %+09d %+09d Temp: %+07d\n",
	    (int) sd->accel_data[0], (int) sd->accel_data[1], (int) sd->accel_data[2], (int) sd->temp_data);
#endif
#if ICM45686_REPORT_GYRO_LSB != 0
	ICM45686_REPORT_NUM_LINE_ARGS(ICM45686_REPORT_GYRO_LSB,
	    "ICM45686  Gyro" ICM45686_REPORT_NUM_FMT ": %+09d %+09d %+09d Temp: %+07d\n",
	    (int) sd->gyro_data[0], (int) sd->gyro_data[1], (int) sd->gyro_data[2], (int) sd->temp_data);
#endif
#if ICM45686_REPORT_MSG_NUM == 1 && \
    (ICM45686_REPORT_ACCEL_SI != 0 || ICM45686_REPORT_GYRO_SI != 0 || \
     ICM45686_REPORT_ACCEL_LSB != 0 || ICM45686_REPORT_GYRO_LSB != 0)
	if (++msg_num == 1000) {
		msg_num = 0;
	}
#endif
}

/**
 * cmd_icms
 */
static void cmd_icms(void)
{
	UBaseType_t pr;

	pr = uxTaskPriorityGet(NULL);
	vTaskPrioritySet(NULL, TASK_PRIO_HIGH);
	msg(INF, cmd_accp);
	msg(INF, "ICM45686 cnt: read_data_reg=%u unknown_intr=%u drdy_timeout=%u stm_rst=%u\n",
	    stats.read_data_reg_cnt, stats.unknown_intr_cnt, stats.drdy_timeout_cnt, stats.stm_rst_cnt);
#if ICM45686_REPORT_FN == 1
	msg(INF, "ICM45686 cnt: report_fn_err=%u\n", stats.report_fn_err_cnt);
#endif
#if ICM45686_REPORT_SAMPLE_FN == 1
	msg(INF, "ICM45686 cnt: sample_fn_err=%u\n", stats.sample_fn_err_cnt);
#endif
	vTaskPrioritySet(NULL, pr);
}
#endif
