/*
 * icm45686_stm_fifo.c
 *
 * Autors: Jan Rusnak.
 * (c) 2025 AZTech.
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
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
#include "icm45686_stm_fifo.h"

#if ICM45686_SENSOR_MODE == ICM45686_FIFO_MODE

#define RESET_STM_TMO_MS 5000
#define WAIT_ISR_SIG_TMO_MS 50

static TaskHandle_t tsk_hndl;
static struct icm_45686_dsc icm_45686_dsc;
static icm_45686 icm = &icm_45686_dsc;
static p_stf_t stmf;
static SemaphoreHandle_t isr_sig;

static struct {
	unsigned int read_frame_cnt;
	unsigned int unknown_intr_cnt;
	unsigned int fifo_empty_err_cnt;
	unsigned int stm_rst_cnt;
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
static void report_data(union icm_45686_fifo_frame *frame) __attribute__((unused));
static void cmd_icms(void);

/**
 * init_icm45686_stm_fifo
 */
void init_icm45686_stm_fifo(void)
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
	if (NULL == (isr_sig = xSemaphoreCreateBinary())) {
		crit_err_exit(MALLOC_ERROR);
	}
	add_command_noargs("icms", cmd_icms);
        if (pdPASS != xTaskCreate(tsk, "ICM45686", ICM45686_TASK_STACK_SIZE, NULL,
                                  ICM45686_TASK_PRIO, &tsk_hndl)) {
                crit_err_exit(MALLOC_ERROR);
        }
}

/**
 * state_sw_reset
 */
static gfp_t state_sw_reset(void)
{
	int ret;

	ret = icm_45686_soft_reset(icm);
	if (ret) {
		msg(INF, "ICM45686: Soft reset error (%s)\n", hwerr_str(ret));
		return ((gfp_t) state_error);
	}
	msg(INF, "ICM45686: Soft reset done (ICM45686_FIFO_MODE)\n");
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
	int_cfg.fifo_ths = ICM_45686_ENABLE;
	if ((ret = icm_45686_set_config_int_source(icm, ICM_45686_INT1, &int_cfg))) {
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
	gpio_hal_intr_enable(ICM45686_INT1_CONT, ICM45686_INT1_PIN);
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
	struct icm_45686_fifo_config fifo_config;

	if ((ret = icm_45686_get_fifo_config(icm, &fifo_config))) {
		goto err;
	}
	fifo_config.gyro_en = ICM_45686_ENABLE;
	fifo_config.accel_en = ICM_45686_ENABLE;
#if ICM45686_FIFO_HIRES == 1
	fifo_config.hires_en = ICM_45686_ENABLE;
#else
	fifo_config.hires_en = ICM_45686_DISABLE;
#endif
	fifo_config.fifo_wm_th = 1;
	fifo_config.fifo_mode  = ICM_45686_FIFO_CONFIG0_MODE_SNAPSHOT;
	fifo_config.fifo_depth = ICM_45686_FIFO_CONFIG0_DEPTH_APEX;
	if ((ret = icm_45686_set_fifo_config(icm, &fifo_config))) {
		goto err;
	}
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
	struct icm_45686_int_bitmap is;
	union icm_45686_fifo_frame frame;

	if (pdFALSE == xSemaphoreTake(isr_sig, ms_to_os_ticks(WAIT_ISR_SIG_TMO_MS))) {
		return ((gfp_t) ping_icm);
	}
	if ((ret = icm_45686_get_int_status(icm, ICM_45686_INT1, &is))) {
		msg(INF, "ICM45686: get_int_status() error (%s)\n", hwerr_str(ret));
		return ((gfp_t) state_error);
	}
	if (is.fifo_ths) {
		int frame_count;
		if ((ret = icm_45686_get_frame_count(icm, &frame_count))) {
			msg(INF, "ICM45686: get_frame_count() error (%s)\n", hwerr_str(ret));
			return ((gfp_t) state_error);
		}
		if (!frame_count) {
			stats.fifo_empty_err_cnt++;
			gpio_hal_intr_enable(ICM45686_INT1_CONT, ICM45686_INT1_PIN);
			return ((gfp_t) wait_intr);
		}
		for (int i = 0; i < frame_count; i++) {
			if ((ret = icm_45686_get_fifo_frame(icm, &frame))) {
				msg(INF, "ICM45686: get_fifo_frame() error (%s)\n", hwerr_str(ret));
				return ((gfp_t) state_error);
			}
			stats.read_frame_cnt++;
			report_data(&frame);
		}
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
	if (status & ICM45686_INT1_PIN && gpio_hal_is_intr_enabled(ICM45686_INT1_CONT, ICM45686_INT1_PIN) &&
	    gpio_hal_get_input(ICM45686_INT1_CONT, ICM45686_INT1_PIN) == GPIO_HAL_LOW) {
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

/**
 * report_data
 */
static void report_data(union icm_45686_fifo_frame *frame)
{
	__attribute__((unused)) boolean_t accel_data_valid = FALSE;
	__attribute__((unused)) boolean_t gyro_data_valid = FALSE;
	__attribute__((unused)) boolean_t temp_data_valid = FALSE;
	int accel_raw[3] = {0};
	int gyro_raw[3] = {0};
	int temp_raw = 0;
	__attribute__((unused)) uint16_t fifo_timestamp = 0;
	__attribute__((unused)) float accel_g[3] = {0};
	__attribute__((unused)) float gyro_dps[3] = {0};
	__attribute__((unused)) float temp_degc = 0;
#if ICM45686_FIFO_HIRES == 1
	int max_lsb = 524288;
	int accel_fsr_g = 32;
	int gyro_fsr_dps = 4000;
	int temp_scale_factor = 128;
#else
	int max_lsb = 32768;
	int accel_fsr_g = 4;
	int gyro_fsr_dps = 2000;
	int temp_scale_factor = 2;
#endif
#if ICM45686_FIFO_HIRES == 1
	accel_raw[0] = frame->byte_20.accel_data[0];
	accel_raw[1] = frame->byte_20.accel_data[1];
	accel_raw[2] = frame->byte_20.accel_data[2];
	if ((accel_raw[0] >> 4 != ICM_45686_INVALID_VALUE_FIFO) &&
	    (accel_raw[1] >> 4 != ICM_45686_INVALID_VALUE_FIFO) &&
	    (accel_raw[2] >> 4 != ICM_45686_INVALID_VALUE_FIFO)) {
		accel_data_valid = TRUE;
	}
	gyro_raw[0] = frame->byte_20.gyro_data[0];
	gyro_raw[1] = frame->byte_20.gyro_data[1];
	gyro_raw[2] = frame->byte_20.gyro_data[2];
	if ((gyro_raw[0] >> 4 != ICM_45686_INVALID_VALUE_FIFO) &&
	    (gyro_raw[1] >> 4 != ICM_45686_INVALID_VALUE_FIFO) &&
	    (gyro_raw[2] >> 4 != ICM_45686_INVALID_VALUE_FIFO)) {
		gyro_data_valid = TRUE;
	}
	temp_raw = frame->byte_20.temp_data;
	if (temp_raw >> 4 != ICM_45686_INVALID_VALUE_FIFO_1B) {
		temp_data_valid = TRUE;
	}
	fifo_timestamp = frame->byte_20.timestamp;
#else
	accel_raw[0] = frame->byte_16.accel_data[0];
	accel_raw[1] = frame->byte_16.accel_data[1];
	accel_raw[2] = frame->byte_16.accel_data[2];
	if ((accel_raw[0] != ICM_45686_INVALID_VALUE_FIFO) &&
	    (accel_raw[1] != ICM_45686_INVALID_VALUE_FIFO) &&
	    (accel_raw[2] != ICM_45686_INVALID_VALUE_FIFO)) {
		accel_data_valid = TRUE;
	}
	gyro_raw[0] = frame->byte_16.gyro_data[0];
	gyro_raw[1] = frame->byte_16.gyro_data[1];
	gyro_raw[2] = frame->byte_16.gyro_data[2];
	if ((gyro_raw[0] != ICM_45686_INVALID_VALUE_FIFO) &&
	    (gyro_raw[1] != ICM_45686_INVALID_VALUE_FIFO) &&
	    (gyro_raw[2] != ICM_45686_INVALID_VALUE_FIFO)) {
		gyro_data_valid = TRUE;
	}
	temp_raw = frame->byte_16.temp_data;
	if (temp_raw != ICM_45686_INVALID_VALUE_FIFO_1B) {
		temp_data_valid = TRUE;
	}
	fifo_timestamp = frame->byte_16.timestamp;
#endif
	accel_g[0] = (float) (accel_raw[0] * accel_fsr_g) / max_lsb;
	accel_g[1] = (float) (accel_raw[1] * accel_fsr_g) / max_lsb;
	accel_g[2] = (float) (accel_raw[2] * accel_fsr_g) / max_lsb;
	gyro_dps[0] = (float) (gyro_raw[0] * gyro_fsr_dps) / max_lsb;
	gyro_dps[1] = (float) (gyro_raw[1] * gyro_fsr_dps) / max_lsb;
	gyro_dps[2] = (float) (gyro_raw[2] * gyro_fsr_dps) / max_lsb;
	temp_degc = (float) 25 + ((float) temp_raw / temp_scale_factor);
#if ICM45686_REPORT_FIFO_TMSTMP == 1
	if (temp_data_valid) {
		msg(INF, "ICM45686 FIFO Timestamp: %5hu us Temp: % 6.2f degC\n", fifo_timestamp, temp_degc);
	} else {
		msg(INF, "ICM45686 FIFO Timestamp: %5hu us\n", fifo_timestamp);
	}
#endif
#if ICM45686_REPORT_ACCEL_SI == 1
	if (accel_data_valid) {
		msg(INF, "ICM45686 Accel: % 8.2f % 8.2f % 8.2f g\n",
		    accel_g[0], accel_g[1], accel_g[2]);
	} else {
		msg(INF, "ICM45686 Accel: -- invalid data --\n");
	}
#endif
#if ICM45686_REPORT_GYRO_SI == 1
	if (gyro_data_valid) {
		msg(INF, "ICM45686  Gyro: % 8.2f % 8.2f % 8.2f dps\n",
		    gyro_dps[0], gyro_dps[1], gyro_dps[2]);
	} else {
		msg(INF, "ICM45686  Gyro: -- invalid data --\n");
	}
#endif
#if ICM45686_REPORT_ACCEL_LSB == 1
	if (accel_data_valid) {
		msg(INF, "ICM45686 Accel: %+09d %+09d %+09d\n",
		    accel_raw[0], accel_raw[1], accel_raw[2]);
	} else {
		msg(INF, "ICM45686 Accel: -- invalid data --\n");
	}
#endif
#if ICM45686_REPORT_GYRO_LSB == 1
	if (gyro_data_valid) {
		msg(INF, "ICM45686  Gyro: %+09d %+09d %+09d\n",
		    gyro_raw[0], gyro_raw[1], gyro_raw[2]);
	} else {
		msg(INF, "ICM45686  Gyro: -- invalid data --\n");
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
	msg(INF, "ICM45686 cnt: read_frame_cnt=%u unknown_intr=%u\n",
	    stats.read_frame_cnt, stats.unknown_intr_cnt);
	msg(INF, "ICM45686 cnt: fifo_empty_err=%u stm_rst=%u\n",
	    stats.fifo_empty_err_cnt, stats.stm_rst_cnt);
	vTaskPrioritySet(NULL, pr);
}
#endif
