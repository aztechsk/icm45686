/*
 * icm45686.c
 *
 * Copyright (c) 2025 Jan Rusnak <jan@rusnak.sk>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <string.h>
#include <gentyp.h>
#include "sysconf.h"
#include "msgconf.h"
#include "criterr.h"
#include "dlycnt.h"
#include "hwerr.h"
#include "icm45686.h"

#define IREG_ACCESS_MIN_DLY_US 4
#define IREG_ACCESS_MAX_TMO_US 25
#define FLUSH_FIFO_DLY_US 10
#define FLUSH_FIFO_STEP_DLY_US 100
#define ICM_RESET_TMO_US 50000
#define ICM_RESET_STAB_US 1000
#define ICM_45686_WHOAMI_ID 0xE9
#define SPI_SLEW_TM_US 2
#define SPI_HAL_SHORT_TRANS SPI_HAL_POLL
#define SPI_HAL_LONG_TRANS SPI_HAL_DMA

#define FORM_16_BITS_DATA(p_in8) \
	((uint16_t) (((uint16_t) (((const uint8_t *) (p_in8))[1]) << 8) | ((const uint8_t *) (p_in8))[0]))

static int ireg_done(icm_45686 icm);
static inline int32_t sign_extend_20_32(uint32_t u);

/**
 * init_icm_45686
 */
void init_icm_45686(icm_45686 icm)
{
	spi_hal_dev_init(&icm->spi);
}

/**
 * icm_45686_soft_reset
 */
int icm_45686_soft_reset(icm_45686 icm)
{
	int ret;

	uint8_t v = 0x02;
	if ((ret = icm_45686_bank0_write(icm, ICM_45686_REG_MISC2, &v, 1, TRUE))) {
		return (ret);
	}
	int w_us = 0;
	for (;;) {
		uint8_t st;
		if ((ret = icm_45686_bank0_read(icm, ICM_45686_INT1_STATUS0, &st, 1))) {
			return (ret);
		}
		if (st & 0x80) {
			break;
		}
		if (w_us >= ICM_RESET_TMO_US) {
			return (-EHW);
		}
		delay_us(50);
		w_us += 50;
	}
	uint8_t misc2 = 0;
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_REG_MISC2, &misc2, 1))) {
		return (ret);
	}
	if (misc2 & 0x02) {
		delay_us(100);
		if ((ret = icm_45686_bank0_read(icm, ICM_45686_REG_MISC2, &misc2, 1))) {
			return (ret);
		}
		if (misc2 & 0x02) {
			return (-EHW);
		}
	}
	delay_us(ICM_RESET_STAB_US);
	return (0);
}

/**
 * icm_45686_whoami_check
 */
int icm_45686_whoami_check(icm_45686 icm)
{
	int ret;
	uint8_t who;

	if ((ret = icm_45686_bank0_read(icm, ICM_45686_WHOAMI, &who, 1))) {
		return (ret);
	}
	return ((who == ICM_45686_WHOAMI_ID) ? 0 : -EHW);
}

/**
 * icm_45686_set_spi_pads_slew
 */
int icm_45686_set_spi_pads_slew(icm_45686 icm, enum icm_45686_drive_config0_pads_spi_slew slew)
{
	int ret;
	struct icm_45686_drive_config0 drive_config0;

	if ((unsigned int) slew > ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_0_5NS) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_DRIVE_CONFIG0, &drive_config0, 1))) {
		return (ret);
	}
	drive_config0.pads_spi_slew = slew;
	if ((ret = icm_45686_bank0_write(icm, ICM_45686_DRIVE_CONFIG0, &drive_config0, 1, TRUE))) {
		return (ret);
	}
	delay_us(SPI_SLEW_TM_US);
	return (ret);
}

/**
 * icm_45686_set_accel_mode
 */
int icm_45686_set_accel_mode(icm_45686 icm, enum icm_45686_pwr_mgmt0_accel_mode mode)
{
	int ret;
	struct icm_45686_pwr_mgmt0 pwr_mgmt0;

	if ((unsigned int) mode > ICM_45686_PWR_MGMT0_ACCEL_MODE_LN) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_PWR_MGMT0, &pwr_mgmt0, 1))) {
		return (ret);
	}
	pwr_mgmt0.accel_mode = mode;
	return (icm_45686_bank0_write(icm, ICM_45686_PWR_MGMT0, &pwr_mgmt0, 1, TRUE));
}

/**
 * icm_45686_set_gyro_mode
 */
int icm_45686_set_gyro_mode(icm_45686 icm, enum icm_45686_pwr_mgmt0_gyro_mode mode)
{
	int ret;
	struct icm_45686_pwr_mgmt0 pwr_mgmt0;

	if ((unsigned int) mode > ICM_45686_PWR_MGMT0_GYRO_MODE_LN) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_PWR_MGMT0, &pwr_mgmt0, 1))) {
		return (ret);
	}
	pwr_mgmt0.gyro_mode = mode;
	return (icm_45686_bank0_write(icm, ICM_45686_PWR_MGMT0, &pwr_mgmt0, 1, TRUE));
}

/**
 * icm_45686_set_accel_freq
 */
int icm_45686_set_accel_freq(icm_45686 icm, enum icm_45686_accel_config0_odr freq)
{
	int ret;
	struct icm_45686_accel_config0 accel_config0;

	if (freq < ICM_45686_ACCEL_CONFIG0_ODR_6400_HZ || freq > ICM_45686_ACCEL_CONFIG0_ODR_1_5625_HZ) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_ACCEL_CONFIG0, &accel_config0, 1))) {
		return (ret);
	}
	accel_config0.accel_odr = freq;
	return (icm_45686_bank0_write(icm, ICM_45686_ACCEL_CONFIG0, &accel_config0, 1, TRUE));
}

/**
 * icm_45686_set_gyro_freq
 */
int icm_45686_set_gyro_freq(icm_45686 icm, enum icm_45686_gyro_config0_odr freq)
{
	int ret;
	struct icm_45686_gyro_config0 gyro_config0;

	if (freq < ICM_45686_GYRO_CONFIG0_ODR_6400_HZ || freq > ICM_45686_GYRO_CONFIG0_ODR_1_5625_HZ) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_GYRO_CONFIG0, &gyro_config0, 1))) {
		return (ret);
	}
	gyro_config0.gyro_odr = freq;
	return (icm_45686_bank0_write(icm, ICM_45686_GYRO_CONFIG0, &gyro_config0, 1, TRUE));
}

/**
 * icm_45686_set_accel_fsr
 */
int icm_45686_set_accel_fsr(icm_45686 icm, enum icm_45686_accel_config0_ui_fs_sel fsr)
{
	int ret;
	struct icm_45686_accel_config0 accel_config0;

	if ((unsigned int) fsr > ICM_45686_ACCEL_CONFIG0_UI_FS_SEL_2_G) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_ACCEL_CONFIG0, &accel_config0, 1))) {
		return (ret);
	}
	accel_config0.accel_ui_fs_sel = fsr;
	return (icm_45686_bank0_write(icm, ICM_45686_ACCEL_CONFIG0, &accel_config0, 1, TRUE));
}

/**
 * icm_45686_set_gyro_fsr
 */
int icm_45686_set_gyro_fsr(icm_45686 icm, enum icm_45686_gyro_config0_ui_fs_sel fsr)
{
	int ret;
	struct icm_45686_gyro_config0 gyro_config0;

	if ((unsigned int) fsr > ICM_45686_GYRO_CONFIG0_UI_FS_SEL_15_625_DPS) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_GYRO_CONFIG0, &gyro_config0, 1))) {
		return (ret);
	}
	gyro_config0.gyro_ui_fs_sel = fsr;
	return (icm_45686_bank0_write(icm, ICM_45686_GYRO_CONFIG0, &gyro_config0, 1, TRUE));
}

/**
 * icm_45686_set_accel_lp_avg
 */
int icm_45686_set_accel_lp_avg(icm_45686 icm, enum icm_45686_ipreg_sys2_reg_129_accel_lp_avg_sel avg)
{
	int ret;
	struct icm_45686_ipreg_sys2_reg_129 sys2_reg_129;

	if ((unsigned int) avg > ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_64) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_ireg_read(icm, ICM_45686_IPREG_SYS2_REG_129, &sys2_reg_129, 1))) {
		return (ret);
	}
	sys2_reg_129.accel_lp_avg_sel = avg;
	return (icm_45686_ireg_write(icm, ICM_45686_IPREG_SYS2_REG_129, &sys2_reg_129, 1));
}

/**
 * icm_45686_set_gyro_lp_avg
 */
int icm_45686_set_gyro_lp_avg(icm_45686 icm, enum icm_45686_ipreg_sys1_reg_170_gyro_lp_avg_sel avg)
{
	int ret;
	struct icm_45686_ipreg_sys1_reg_170 sys1_reg_170;

	if ((unsigned int) avg > ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_64) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_ireg_read(icm, ICM_45686_IPREG_SYS1_REG_170, &sys1_reg_170, 1))) {
		return (ret);
	}
	sys1_reg_170.gyro_lp_avg_sel = avg;
	return (icm_45686_ireg_write(icm, ICM_45686_IPREG_SYS1_REG_170, &sys1_reg_170, 1));
}

/**
 * icm_45686_set_accel_ln_bw
 */
int icm_45686_set_accel_ln_bw(icm_45686 icm, enum icm_45686_ipreg_sys2_reg_131_accel_ui_lpfbw bw)
{
	int ret;
	struct icm_45686_ipreg_sys2_reg_131 sys2_reg_131;

	if ((unsigned int) bw > ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_128) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_ireg_read(icm, ICM_45686_IPREG_SYS2_REG_131, &sys2_reg_131, 1))) {
		return (ret);
	}
	sys2_reg_131.accel_ui_lpfbw_sel = bw;
	return (icm_45686_ireg_write(icm, ICM_45686_IPREG_SYS2_REG_131, &sys2_reg_131, 1));
}

/**
 * icm_45686_set_gyro_ln_bw
 */
int icm_45686_set_gyro_ln_bw(icm_45686 icm, enum icm_45686_ipreg_sys1_reg_172_gyro_ui_lpfbw_sel bw)
{
	int ret;
	struct icm_45686_ipreg_sys1_reg_172 sys1_reg_172;

	if ((unsigned int) bw > ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_128) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_ireg_read(icm, ICM_45686_IPREG_SYS1_REG_172, &sys1_reg_172, 1))) {
		return (ret);
	}
	sys1_reg_172.gyro_ui_lpfbw_sel = bw;
	return (icm_45686_ireg_write(icm, ICM_45686_IPREG_SYS1_REG_172, &sys1_reg_172, 1));
}

/**
 * icm_45686_get_reg_data
 */
int icm_45686_get_reg_data(icm_45686 icm, struct icm_45686_sensor_data *data)
{
	int ret;

	if ((ret = icm_45686_bank0_read(icm, ICM_45686_ACCEL_DATA_X1_UI, data, sizeof(struct icm_45686_sensor_data)))) {
		return (ret);
	}
	data->accel_data[0] = FORM_16_BITS_DATA(&data->accel_data[0]);
	data->accel_data[1] = FORM_16_BITS_DATA(&data->accel_data[1]);
	data->accel_data[2] = FORM_16_BITS_DATA(&data->accel_data[2]);
	data->gyro_data[0] = FORM_16_BITS_DATA(&data->gyro_data[0]);
	data->gyro_data[1] = FORM_16_BITS_DATA(&data->gyro_data[1]);
	data->gyro_data[2] = FORM_16_BITS_DATA(&data->gyro_data[2]);
	data->temp_data = FORM_16_BITS_DATA(&data->temp_data);
	return (ret);
}

/**
 * icm_45686_get_fifo_config
 */
int icm_45686_get_fifo_config(icm_45686 icm, struct icm_45686_fifo_config *cfg)
{
	int ret;
	struct icm_45686_fifo_config_regs cfg_r;

	if ((ret = icm_45686_bank0_read(icm, ICM_45686_FIFO_CONFIG0, &cfg_r,
					sizeof(struct icm_45686_fifo_config_regs)))) {
		return (ret);
	}
	cfg->fifo_mode = cfg_r.config0.fifo_mode;
	if (cfg_r.config0.fifo_depth == ICM_45686_FIFO_CONFIG0_DEPTH_MAX ||
	    cfg_r.config0.fifo_depth == ICM_45686_FIFO_CONFIG0_DEPTH_APEX ||
	    cfg_r.config0.fifo_depth == ICM_45686_FIFO_CONFIG0_DEPTH_GAF) {
		cfg->fifo_depth = cfg_r.config0.fifo_depth;
	} else {
		cfg->fifo_depth = ICM_45686_FIFO_CONFIG0_DEPTH_APEX;
	}
	cfg->fifo_wm_th = (cfg_r.config1_1 << 8) | cfg_r.config1_0;
	cfg->hires_en = cfg_r.config3.fifo_hires_en;
	cfg->gyro_en  = cfg_r.config3.fifo_gyro_en;
	cfg->accel_en = cfg_r.config3.fifo_accel_en;
	return (ret);
}

/**
 * icm_45686_set_fifo_config
 */
int icm_45686_set_fifo_config(icm_45686 icm, const struct icm_45686_fifo_config *cfg)
{
	int ret;
	struct icm_45686_fifo_config_regs cfg_r;
	struct icm_45686_smc_control_0 smc_control_0;
	uint8_t ff_size = 0;

	if (cfg->fifo_mode != ICM_45686_FIFO_CONFIG0_MODE_BYPASS &&
	    cfg->fifo_mode != ICM_45686_FIFO_CONFIG0_MODE_STREAM &&
	    cfg->fifo_mode != ICM_45686_FIFO_CONFIG0_MODE_SNAPSHOT) {
		crit_err_exit(BAD_PARAMETER);
	}
	if (cfg->fifo_depth != ICM_45686_FIFO_CONFIG0_DEPTH_APEX &&
	    cfg->fifo_depth != ICM_45686_FIFO_CONFIG0_DEPTH_GAF &&
	    cfg->fifo_depth != ICM_45686_FIFO_CONFIG0_DEPTH_MAX) {
		crit_err_exit(BAD_PARAMETER);
	}
	if (cfg->gyro_en > ICM_45686_ENABLE || cfg->accel_en > ICM_45686_ENABLE ||
	    cfg->hires_en > ICM_45686_ENABLE) {
		crit_err_exit(BAD_PARAMETER);
	}
	if (cfg->hires_en && (!cfg->gyro_en || !cfg->accel_en)) {
		crit_err_exit(BAD_PARAMETER);
	}
	if (cfg->fifo_mode != ICM_45686_FIFO_CONFIG0_MODE_BYPASS && !cfg->accel_en && !cfg->gyro_en) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_FIFO_CONFIG0, &cfg_r,
					sizeof(struct icm_45686_fifo_config_regs)))) {
		return (ret);
	}
	cfg_r.config3.fifo_if_en = ICM_45686_DISABLE;
	if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG3, &cfg_r.config3, 1, TRUE))) {
		return (ret);
	}
	cfg_r.config0.fifo_mode = ICM_45686_FIFO_CONFIG0_MODE_BYPASS;
	if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG0, &cfg_r.config0, 1, TRUE))) {
		return (ret);
	}
	cfg_r.config0.fifo_depth = cfg->fifo_depth;
	cfg_r.config1_0 = cfg->fifo_wm_th;
	cfg_r.config1_1 = cfg->fifo_wm_th >> 8;
	cfg_r.config2.fifo_wr_wm_gt_th = ICM_45686_FIFO_CONFIG2_WR_WM_EQ_OR_GT_TH;
	cfg_r.config3.fifo_hires_en = cfg->hires_en;
	cfg_r.config3.fifo_gyro_en = cfg->gyro_en;
	cfg_r.config3.fifo_accel_en = cfg->accel_en;
	if ((ret = icm_45686_ireg_read(icm, ICM_45686_SMC_CONTROL_0, &smc_control_0, 1))) {
		return (ret);
	}
	if ((cfg->fifo_mode != ICM_45686_FIFO_CONFIG0_MODE_BYPASS) &&
	    ((cfg->accel_en && cfg->gyro_en) || cfg->hires_en)) {
		cfg_r.config4.fifo_tmst_fsync_en = ICM_45686_ENABLE;
		smc_control_0.tmst_en = ICM_45686_ENABLE;
	} else {
		cfg_r.config4.fifo_tmst_fsync_en = ICM_45686_DISABLE;
		smc_control_0.tmst_en = ICM_45686_DISABLE;
	}
	if ((ret = icm_45686_ireg_write(icm, ICM_45686_SMC_CONTROL_0, &smc_control_0, 1))) {
		return (ret);
	}
	if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG0, &cfg_r,
					 sizeof(struct icm_45686_fifo_config_regs), TRUE))) {
		return (ret);
	}
	cfg_r.config0.fifo_mode = cfg->fifo_mode;
	if (cfg->fifo_mode == ICM_45686_FIFO_CONFIG0_MODE_BYPASS) {
		cfg_r.config3.fifo_if_en = ICM_45686_DISABLE;
		if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG3, &cfg_r.config3, 1, TRUE))) {
			return (ret);
		}
		if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG0, &cfg_r.config0, 1, TRUE))) {
			return (ret);
		}
	} else {
		if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG0, &cfg_r.config0, 1, TRUE))) {
			return (ret);
		}
		cfg_r.config3.fifo_if_en = ICM_45686_ENABLE;
		if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG3, &cfg_r.config3, 1, TRUE))) {
			return (ret);
		}
	}
	if (cfg->hires_en) {
		ff_size = 20;
	} else {
		if (cfg->accel_en) {
			ff_size += 8;
		}
		if (cfg->gyro_en) {
			ff_size += 8;
		}
	}
	icm->fifo_frame_size = ff_size;
	return (ret);
}

/**
 * icm_45686_flush_fifo
 */
int icm_45686_flush_fifo(icm_45686 icm)
{
	int ret;
	struct icm_45686_fifo_config2 fifo_config2;
	int tmo_us = 1000000;

	if ((ret = icm_45686_bank0_read(icm, ICM_45686_FIFO_CONFIG2, &fifo_config2, 1))) {
		return (ret);
	}
	fifo_config2.fifo_flush = ICM_45686_ENABLE;
	if ((ret = icm_45686_bank0_write(icm, ICM_45686_FIFO_CONFIG2, &fifo_config2, 1, TRUE))) {
		return (ret);
	}
	delay_us(FLUSH_FIFO_DLY_US);
	while (TRUE) {
		if ((ret = icm_45686_bank0_read(icm, ICM_45686_FIFO_CONFIG2, &fifo_config2, 1))) {
			return (ret);
		}
		if (fifo_config2.fifo_flush != ICM_45686_ENABLE) {
			break;
		}
		delay_us(FLUSH_FIFO_STEP_DLY_US);
		tmo_us -= FLUSH_FIFO_STEP_DLY_US;
		if (tmo_us <= 0) {
			return (-EHW);
		}
	}
	return (ret);
}

/**
 * icm_45686_get_frame_count
 */
int icm_45686_get_frame_count(icm_45686 icm, int *count)
{
	int ret;
	uint16_t fc;

	if ((ret = icm_45686_bank0_read(icm, ICM_45686_FIFO_COUNT_0, &fc, 2))) {
		return (ret);
	}
	// Errata AN-000364 (2.2) - Read FIFO_COUNT value twice.
	if ((ret = icm_45686_bank0_read(icm, ICM_45686_FIFO_COUNT_0, &fc, 2))) {
		return (ret);
	}
	fc = FORM_16_BITS_DATA(&fc);
	*count = fc;
	return (ret);
}

/**
 * icm_45686_get_fifo_frame
 */
int icm_45686_get_fifo_frame(icm_45686 icm, union icm_45686_fifo_frame *frame)
{
	int ret;
	uint8_t frm[20];
	int16_t r16[3];

	if ((ret = icm_45686_bank0_read(icm, ICM_45686_FIFO_DATA, frm, icm->fifo_frame_size))) {
		return (ret);
	}
	frame->header.byte = frm[0];
	switch (icm->fifo_frame_size) {
	case 8 :
		if (frame->header.bits.ext_header || frame->header.bits.timestamp_bit ||
		    frame->header.bits.twentybits_bit ||
		    (frame->header.bits.accel_bit == frame->header.bits.gyro_bit)) {
			return (-EHW);
		}
		frame->byte_8.sensor_data[0] = FORM_16_BITS_DATA(&frm[1]);
		frame->byte_8.sensor_data[1] = FORM_16_BITS_DATA(&frm[3]);
		frame->byte_8.sensor_data[2] = FORM_16_BITS_DATA(&frm[5]);
		frame->byte_8.temp_data = frm[7];
		break;
	case 16 :
		if (frame->header.bits.ext_header || frame->header.bits.twentybits_bit ||
		    !frame->header.bits.accel_bit || !frame->header.bits.gyro_bit ||
		    !frame->header.bits.timestamp_bit) {
			return (-EHW);
		}
		frame->byte_16.accel_data[0] = FORM_16_BITS_DATA(&frm[1]);
		frame->byte_16.accel_data[1] = FORM_16_BITS_DATA(&frm[3]);
		frame->byte_16.accel_data[2] = FORM_16_BITS_DATA(&frm[5]);
		frame->byte_16.gyro_data[0] = FORM_16_BITS_DATA(&frm[7]);
		frame->byte_16.gyro_data[1] = FORM_16_BITS_DATA(&frm[9]);
		frame->byte_16.gyro_data[2] = FORM_16_BITS_DATA(&frm[11]);
		frame->byte_16.temp_data = frm[13];
		frame->byte_16.timestamp = FORM_16_BITS_DATA(&frm[14]);
		break;
	case 20 :
		if (frame->header.bits.ext_header || !frame->header.bits.twentybits_bit ||
		    !frame->header.bits.accel_bit || !frame->header.bits.gyro_bit ||
		    !frame->header.bits.timestamp_bit) {
			return (-EHW);
		}
		r16[0] = FORM_16_BITS_DATA(&frm[1]);
		r16[1] = FORM_16_BITS_DATA(&frm[3]);
		r16[2] = FORM_16_BITS_DATA(&frm[5]);
		frame->byte_20.accel_data[0] = sign_extend_20_32(((uint32_t) (uint16_t) r16[0] << 4) | (frm[17] >> 4));
		frame->byte_20.accel_data[1] = sign_extend_20_32(((uint32_t) (uint16_t) r16[1] << 4) | (frm[18] >> 4));
		frame->byte_20.accel_data[2] = sign_extend_20_32(((uint32_t) (uint16_t) r16[2] << 4) | (frm[19] >> 4));
		r16[0] = FORM_16_BITS_DATA(&frm[7]);
		r16[1] = FORM_16_BITS_DATA(&frm[9]);
		r16[2] = FORM_16_BITS_DATA(&frm[11]);
		frame->byte_20.gyro_data[0] = sign_extend_20_32(((uint32_t) (uint16_t) r16[0] << 4) | (frm[17] & 0x0F));
		frame->byte_20.gyro_data[1] = sign_extend_20_32(((uint32_t) (uint16_t) r16[1] << 4) | (frm[18] & 0x0F));
		frame->byte_20.gyro_data[2] = sign_extend_20_32(((uint32_t) (uint16_t) r16[2] << 4) | (frm[19] & 0x0F));
		frame->byte_20.temp_data = FORM_16_BITS_DATA(&frm[13]);
		frame->byte_20.timestamp = FORM_16_BITS_DATA(&frm[15]);
		break;
	default :
		crit_err_exit(BAD_PARAMETER);
	}
	return (ret);
}

/**
 * icm_45686_set_config_int_source
 */
int icm_45686_set_config_int_source(icm_45686 icm, enum icm_45686_int_num num, const struct icm_45686_int_bitmap *cfg)
{
	struct icm_45686_int_cfg_01_regs regs = {0};
	enum icm_45686_bank0_adr adr = ICM_45686_INT1_CONFIG0;

	if (num == ICM_45686_INT2) {
		adr = ICM_45686_INT2_CONFIG0;
	} else if (num != ICM_45686_INT1) {
		crit_err_exit(BAD_PARAMETER);
	}
	if (cfg->fifo_full > ICM_45686_ENABLE || cfg->fifo_ths > ICM_45686_ENABLE ||
	    cfg->ui_drdy > ICM_45686_ENABLE || cfg->ois1 > ICM_45686_ENABLE ||
	    cfg->ui_fsync > ICM_45686_ENABLE || cfg->agc_rdy > ICM_45686_ENABLE ||
	    cfg->ois1_agc_rdy > ICM_45686_ENABLE || cfg->reset_done > ICM_45686_ENABLE ||
	    cfg->pll_rdy > ICM_45686_ENABLE || cfg->wom_x > ICM_45686_ENABLE ||
	    cfg->wom_y > ICM_45686_ENABLE || cfg->wom_z > ICM_45686_ENABLE ||
	    cfg->i3c_prot_err > ICM_45686_ENABLE || cfg->i2cm_done > ICM_45686_ENABLE ||
	    cfg->edmp_event > ICM_45686_ENABLE) {
		crit_err_exit(BAD_PARAMETER);
	}
	regs.config0.int1_status_en_fifo_full = cfg->fifo_full;
	regs.config0.int1_status_en_fifo_ths = cfg->fifo_ths;
	regs.config0.int1_status_en_drdy = cfg->ui_drdy;
	regs.config0.int1_status_en_aux1_drdy = cfg->ois1;
	regs.config0.int1_status_en_ap_fsync = cfg->ui_fsync;
	regs.config0.int1_status_en_ap_agc_rdy = cfg->agc_rdy;
	regs.config0.int1_status_en_aux1_agc_rdy = cfg->ois1_agc_rdy;
	regs.config0.int1_status_en_reset_done = cfg->reset_done;
	regs.config1.int1_status_en_pll_rdy = cfg->pll_rdy;
	regs.config1.int1_status_en_wom_x = cfg->wom_x;
	regs.config1.int1_status_en_wom_y = cfg->wom_y;
	regs.config1.int1_status_en_wom_z = cfg->wom_z;
	regs.config1.int1_status_en_i3c_protocol_err = cfg->i3c_prot_err;
	regs.config1.int1_status_en_i2cm_done = cfg->i2cm_done;
	regs.config1.int1_status_en_apex_event = cfg->edmp_event;
	return (icm_45686_bank0_write(icm, adr, &regs, sizeof(struct icm_45686_int_cfg_01_regs), TRUE));
}

/**
 * icm_45686_set_config_int_pin
 */
int icm_45686_set_config_int_pin(icm_45686 icm, enum icm_45686_int_num num, const struct icm_45686_int_pin_cfg *cfg)
{
	struct icm_45686_int1_config2 int1_config2 = {0};
	enum icm_45686_bank0_adr adr = ICM_45686_INT1_CONFIG2;

	if (num == ICM_45686_INT2) {
		adr = ICM_45686_INT2_CONFIG2;
	} else if (num != ICM_45686_INT1) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((unsigned int) cfg->polarity > ICM_45686_INT1_CONFIG2_POLARITY_HIGH ||
	    (unsigned int) cfg->mode > ICM_45686_INT1_CONFIG2_MODE_LATCH ||
	    (unsigned int) cfg->drive > ICM_45686_INT1_CONFIG2_DRIVE_OD) {
		crit_err_exit(BAD_PARAMETER);
	}
	int1_config2.int1_polarity = cfg->polarity;
	int1_config2.int1_mode = cfg->mode;
	int1_config2.int1_drive = cfg->drive;
	return (icm_45686_bank0_write(icm, adr, &int1_config2, 1, TRUE));
}

/**
 * icm_45686_get_int_status
 */
int icm_45686_get_int_status(icm_45686 icm, enum icm_45686_int_num num, struct icm_45686_int_bitmap *stat)
{
	int ret;
	struct icm_45686_int_stat_01_regs regs;
	enum icm_45686_bank0_adr adr = ICM_45686_INT1_STATUS0;

	if (num == ICM_45686_INT2) {
		adr = ICM_45686_INT2_STATUS0;
	} else if (num != ICM_45686_INT1) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_bank0_read(icm, adr, &regs, sizeof(struct icm_45686_int_stat_01_regs)))) {
		return (ret);
	}
	stat->fifo_full = regs.status0.int1_status_fifo_full;
	stat->fifo_ths = regs.status0.int1_status_fifo_ths;
	stat->ui_drdy = regs.status0.int1_status_drdy;
	stat->ois1 = regs.status0.int1_status_aux1_drdy;
	stat->ui_fsync = regs.status0.int1_status_ap_fsync;
	stat->agc_rdy = regs.status0.int1_status_ap_agc_rdy;
	stat->ois1_agc_rdy = regs.status0.int1_status_aux1_agc_rdy;
	stat->reset_done = regs.status0.int1_status_reset_done;
	stat->pll_rdy = regs.status1.int1_status_pll_rdy;
	stat->wom_x = regs.status1.int1_status_wom_x;
	stat->wom_y = regs.status1.int1_status_wom_y;
	stat->wom_z = regs.status1.int1_status_wom_z;
	stat->i3c_prot_err = regs.status1.int1_status_i3c_protocol_err;
	stat->i2cm_done = regs.status1.int1_status_i2cm_done;
	stat->edmp_event = regs.status1.int1_status_apex_event;
	return (ret);
}

/**
 * icm_45686_select_accel_lp_clk
 */
int icm_45686_select_accel_lp_clk(icm_45686 icm, enum icm_45686_smc_control_0_accel_lp_clk_sel clk)
{
	int ret;
	struct icm_45686_smc_control_0 smc_control_0;

	if (clk != ICM_45686_SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC &&
	    clk != ICM_45686_SMC_CONTROL_0_ACCEL_LP_CLK_WUOSC) {
		crit_err_exit(BAD_PARAMETER);
	}
	if ((ret = icm_45686_ireg_read(icm, ICM_45686_SMC_CONTROL_0, &smc_control_0, 1))) {
		return (ret);
	}
	smc_control_0.accel_lp_clk_sel = clk;
	if ((ret = icm_45686_ireg_write(icm, ICM_45686_SMC_CONTROL_0, &smc_control_0, 1))) {
		return (ret);
	}
	return (ret);
}

/**
 * icm_45686_bank0_read
 */
int icm_45686_bank0_read(icm_45686 icm, enum icm_45686_bank0_adr adr, void *data, int n)
{
	enum spi_hal_xfer_type xt = (n > 1) ? SPI_HAL_LONG_TRANS : SPI_HAL_SHORT_TRANS;
	uint8_t hdr = 0x80 | (adr & 0x7F);
	uint8_t *rx = (uint8_t *) data;

	for (int i = 0; i < n; i++) {
		rx[i] = 0xFF;
	}
	return (spi_hal_xfer(&icm->spi, xt, &hdr, 1, data, n));
}

/**
 * icm_45686_bank0_write
 */
int icm_45686_bank0_write(icm_45686 icm, enum icm_45686_bank0_adr adr, void *data, int n, boolean_t ro_data)
{
	enum spi_hal_xfer_type xt = (n > 1) ? SPI_HAL_LONG_TRANS : SPI_HAL_SHORT_TRANS;
	uint8_t hdr = (adr & 0x7F);

	if (ro_data) {
		uint8_t l_data[n];
		memcpy(l_data, data, n);
		return (spi_hal_xfer(&icm->spi, xt, &hdr, 1, l_data, n));
	} else {
		return (spi_hal_xfer(&icm->spi, xt, &hdr, 1, data, n));
	}
}

/**
 * icm_45686_ireg_read
 */
int icm_45686_ireg_read(icm_45686 icm, int adr, void *data, int n)
{
	int ret;
	uint8_t wr[3] = {ICM_45686_IREG_ADDR_15_8, adr >> 8, adr};

	if ((ret = spi_hal_xfer(&icm->spi, SPI_HAL_SHORT_TRANS, wr, 1, wr + 1, 2))) {
		goto err;
	}
	if ((ret = ireg_done(icm))) {
		goto err;
	}
	for (int i = 0; i < n; i++) {
		wr[0] = ICM_45686_IREG_DATA | 0x80;
		*((uint8_t *) data + i) = 0xFF;
		if ((ret = spi_hal_xfer(&icm->spi, SPI_HAL_SHORT_TRANS, wr, 1, (uint8_t *) data + i, 1))) {
			goto err;
		}
		if ((ret = ireg_done(icm))) {
			goto err;
		}
	}
	return (ret);
err:
	delay_us(IREG_ACCESS_MIN_DLY_US);
	return (ret);
}

/**
 * icm_45686_ireg_write
 */
int icm_45686_ireg_write(icm_45686 icm, int adr, const void *data, int n)
{
	int ret;
	const uint8_t *p_data = data;
	uint8_t wr[4] = {ICM_45686_IREG_ADDR_15_8, adr >> 8, adr, p_data[0]};

	if ((ret = spi_hal_xfer(&icm->spi, SPI_HAL_SHORT_TRANS, wr, 1, wr + 1, 3))) {
		goto err;
	}
	if ((ret = ireg_done(icm))) {
		goto err;
	}
	if (n > 1) {
		for (int i = 1; i < n; i++) {
			wr[0] = ICM_45686_IREG_DATA;
			wr[1] = p_data[i];
			if ((ret = spi_hal_xfer(&icm->spi, SPI_HAL_SHORT_TRANS, wr, 1, wr + 1, 1))) {
				goto err;
			}
			if ((ret = ireg_done(icm))) {
				goto err;
			}
		}
	}
	return (ret);
err:
	delay_us(IREG_ACCESS_MIN_DLY_US);
	return (ret);
}

/**
 * ireg_done
 */
static int ireg_done(icm_45686 icm)
{
	uint8_t stat;
	int ret;
	int w_us = 0;

	for (;;) {
		delay_us(1);
		w_us++;
		ret = icm_45686_bank0_read(icm, ICM_45686_REG_MISC2, &stat, 1);
		if (ret) {
			return (ret);
		}
		if (stat & 0x01) {
			if (w_us < IREG_ACCESS_MIN_DLY_US) {
				delay_us(IREG_ACCESS_MIN_DLY_US - w_us);
			}
			return (0);
		}
		if (w_us > IREG_ACCESS_MAX_TMO_US) {
			return (-EHW);
		}
	}
}

/**
 * sign_extend_20_32
 */
static inline int32_t sign_extend_20_32(uint32_t u)
{
	return ((u & 0x80000U) ? (int32_t) (u | 0xFFF00000U) : (int32_t) u);
}
