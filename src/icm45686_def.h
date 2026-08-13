/*
 * icm45686_def.h
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

#ifndef ICM45686_DEF_H
#define ICM45686_DEF_H

enum icm_45686_bool {
	ICM_45686_DISABLE,
	ICM_45686_ENABLE
};

// Startup times
#define ICM_45686_ACC_STARTUP_TIME_US 10000
#define ICM_45686_GYR_STARTUP_TIME_US 70000

// Interrupt number
enum icm_45686_int_num {
	ICM_45686_INT1,
	ICM_45686_INT2
};

enum icm_45686_bank0_adr {
	ICM_45686_ACCEL_DATA_X1_UI = 0x00,
	ICM_45686_ACCEL_DATA_X0_UI = 0x01,
	ICM_45686_ACCEL_DATA_Y1_UI = 0x02,
	ICM_45686_ACCEL_DATA_Y0_UI = 0x03,
	ICM_45686_ACCEL_DATA_Z1_UI = 0x04,
	ICM_45686_ACCEL_DATA_Z0_UI = 0x05,
	ICM_45686_GYRO_DATA_X1_UI = 0x06,
	ICM_45686_GYRO_DATA_X0_UI = 0x07,
	ICM_45686_GYRO_DATA_Y1_UI = 0x08,
	ICM_45686_GYRO_DATA_Y0_UI = 0x09,
	ICM_45686_GYRO_DATA_Z1_UI = 0x0a,
	ICM_45686_GYRO_DATA_Z0_UI = 0x0b,
	ICM_45686_TEMP_DATA1_UI = 0x0c,
	ICM_45686_TEMP_DATA0_UI = 0x0d,
	ICM_45686_TMST_FSYNCH = 0x0e,
	ICM_45686_TMST_FSYNCL = 0x0f,
	ICM_45686_PWR_MGMT0 = 0x10,
	ICM_45686_FIFO_COUNT_0 = 0x12,
	ICM_45686_FIFO_COUNT_1 = 0x13,
	ICM_45686_FIFO_DATA = 0x14,
	ICM_45686_INT1_CONFIG0 = 0x16,
	ICM_45686_INT1_CONFIG1 = 0x17,
	ICM_45686_INT1_CONFIG2 = 0x18,
	ICM_45686_INT1_STATUS0 = 0x19,
	ICM_45686_INT1_STATUS1 = 0x1a,
	ICM_45686_ACCEL_CONFIG0 = 0x1b,
	ICM_45686_GYRO_CONFIG0 = 0x1c,
	ICM_45686_FIFO_CONFIG0 = 0x1d,
	ICM_45686_FIFO_CONFIG1_0 = 0x1e,
	ICM_45686_FIFO_CONFIG1_1 = 0x1f,
	ICM_45686_FIFO_CONFIG2 = 0x20,
	ICM_45686_FIFO_CONFIG3 = 0x21,
	ICM_45686_FIFO_CONFIG4 = 0x22,
	ICM_45686_TMST_WOM_CONFIG = 0x23,
	ICM_45686_FSYNC_CONFIG0 = 0x24,
	ICM_45686_FSYNC_CONFIG1 = 0x25,
	ICM_45686_RTC_CONFIG = 0x26,
	ICM_45686_DMP_EXT_SEN_ODR_CFG = 0x27,
	ICM_45686_ODR_DECIMATE_CONFIG = 0x28,
	ICM_45686_EDMP_APEX_EN0 = 0x29,
	ICM_45686_EDMP_APEX_EN1 = 0x2a,
	ICM_45686_APEX_BUFFER_MGMT = 0x2b,
	ICM_45686_INTF_CONFIG0 = 0x2c,
	ICM_45686_INTF_CONFIG1_OVRD = 0x2d,
	ICM_45686_INTF_AUX_CONFIG = 0x2e,
	ICM_45686_IOC_PAD_SCENARIO = 0x2f,
	ICM_45686_IOC_PAD_SCENARIO_AUX_OVRD = 0x30,
	ICM_45686_IOC_PAD_SCENARIO_OVRD = 0x31,
	ICM_45686_DRIVE_CONFIG0 = 0x32,
	ICM_45686_DRIVE_CONFIG1 = 0x33,
	ICM_45686_DRIVE_CONFIG2 = 0x34,
	ICM_45686_REG_MISC1 = 0x35,
	ICM_45686_INT_APEX_CONFIG0 = 0x39,
	ICM_45686_INT_APEX_CONFIG1 = 0x3a,
	ICM_45686_INT_APEX_STATUS0 = 0x3b,
	ICM_45686_INT_APEX_STATUS1 = 0x3c,
	ICM_45686_INTF_CONFIG_OVRD_AUX1 = 0x42,
	ICM_45686_ACCEL_DATA_X1_AUX1 = 0x44,
	ICM_45686_ACCEL_DATA_X0_AUX1 = 0x45,
	ICM_45686_ACCEL_DATA_Y1_AUX1 = 0x46,
	ICM_45686_ACCEL_DATA_Y0_AUX1 = 0x47,
	ICM_45686_ACCEL_DATA_Z1_AUX1 = 0x48,
	ICM_45686_ACCEL_DATA_Z0_AUX1 = 0x49,
	ICM_45686_GYRO_DATA_X1_AUX1 = 0x4a,
	ICM_45686_GYRO_DATA_X0_AUX1 = 0x4b,
	ICM_45686_GYRO_DATA_Y1_AUX1 = 0x4c,
	ICM_45686_GYRO_DATA_Y0_AUX1 = 0x4d,
	ICM_45686_GYRO_DATA_Z1_AUX1 = 0x4e,
	ICM_45686_GYRO_DATA_Z0_AUX1 = 0x4f,
	ICM_45686_TEMP_DATA1_AUX1 = 0x50,
	ICM_45686_TEMP_DATA0_AUX1 = 0x51,
	ICM_45686_TMST_FSYNCH_AUX1 = 0x52,
	ICM_45686_TMST_FSYNCL_AUX1 = 0x53,
	ICM_45686_PWR_MGMT_AUX1 = 0x54,
	ICM_45686_FS_SEL_AUX1 = 0x55,
	ICM_45686_INT2_CONFIG0 = 0x56,
	ICM_45686_INT2_CONFIG1 = 0x57,
	ICM_45686_INT2_CONFIG2 = 0x58,
	ICM_45686_INT2_STATUS0 = 0x59,
	ICM_45686_INT2_STATUS1 = 0x5a,
	ICM_45686_INTF_CONFIG_OVRD_AUX2 = 0x5c,
	ICM_45686_ACCEL_DATA_X1_AUX2 = 0x5e,
	ICM_45686_ACCEL_DATA_X0_AUX2 = 0x5f,
	ICM_45686_ACCEL_DATA_Y1_AUX2 = 0x60,
	ICM_45686_ACCEL_DATA_Y0_AUX2 = 0x61,
	ICM_45686_ACCEL_DATA_Z1_AUX2 = 0x62,
	ICM_45686_ACCEL_DATA_Z0_AUX2 = 0x63,
	ICM_45686_GYRO_DATA_X1_AUX2 = 0x64,
	ICM_45686_GYRO_DATA_X0_AUX2 = 0x65,
	ICM_45686_GYRO_DATA_Y1_AUX2 = 0x66,
	ICM_45686_GYRO_DATA_Y0_AUX2 = 0x67,
	ICM_45686_GYRO_DATA_Z1_AUX2 = 0x68,
	ICM_45686_GYRO_DATA_Z0_AUX2 = 0x69,
	ICM_45686_TEMP_DATA1_AUX2 = 0x6a,
	ICM_45686_TEMP_DATA0_AUX2 = 0x6b,
	ICM_45686_TMST_FSYNCH_AUX2 = 0x6c,
	ICM_45686_TMST_FSYNCL_AUX2 = 0x6d,
	ICM_45686_PWR_MGMT_AUX2 = 0x6e,
	ICM_45686_FS_SEL_AUX2 = 0x6f,
	ICM_45686_INT_AUX2_CONFIG = 0x70,
	ICM_45686_INT_AUX2_STATUS = 0x71,
	ICM_45686_WHOAMI = 0x72,
	ICM_45686_REG_HOST_MSG = 0x73,
	ICM_45686_IREG_ADDR_15_8 = 0x7c,
	ICM_45686_IREG_ADDR_7_0 = 0x7d,
	ICM_45686_IREG_DATA = 0x7e,
	ICM_45686_REG_MISC2 = 0x7f
};

// PWR_MGMT0
struct icm_45686_pwr_mgmt0 {
	uint8_t accel_mode : 2;
	uint8_t gyro_mode : 2;
	uint8_t resv_1 : 4;
};

/* gyro_mode */
enum icm_45686_pwr_mgmt0_gyro_mode {
	ICM_45686_PWR_MGMT0_GYRO_MODE_LN = 0x03,
	ICM_45686_PWR_MGMT0_GYRO_MODE_LP = 0x02,
	ICM_45686_PWR_MGMT0_GYRO_MODE_STANDBY = 0x01,
	ICM_45686_PWR_MGMT0_GYRO_MODE_OFF = 0x00
};

/* accel_mode */
enum icm_45686_pwr_mgmt0_accel_mode {
	ICM_45686_PWR_MGMT0_ACCEL_MODE_LN = 0x03,
	ICM_45686_PWR_MGMT0_ACCEL_MODE_LP = 0x02,
	ICM_45686_PWR_MGMT0_ACCEL_MODE_OFF_ALT = 0x01,
	ICM_45686_PWR_MGMT0_ACCEL_MODE_OFF = 0x00
};

// FIFO_DATA
struct icm_45686_fifo_data {
	uint8_t fifo_data : 8;
};

// INT1_CONFIG0
struct icm_45686_int1_config0 {
	uint8_t int1_status_en_fifo_full : 1;
	uint8_t int1_status_en_fifo_ths : 1;
	uint8_t int1_status_en_drdy : 1;
	uint8_t int1_status_en_aux1_drdy : 1;
	uint8_t int1_status_en_ap_fsync : 1;
	uint8_t int1_status_en_ap_agc_rdy : 1;
	uint8_t int1_status_en_aux1_agc_rdy : 1;
	uint8_t int1_status_en_reset_done : 1;
};

// INT1_CONFIG1
struct icm_45686_int1_config1 {
	uint8_t int1_status_en_pll_rdy : 1;
	uint8_t int1_status_en_wom_x : 1;
	uint8_t int1_status_en_wom_y : 1;
	uint8_t int1_status_en_wom_z : 1;
	uint8_t int1_status_en_i3c_protocol_err : 1;
	uint8_t int1_status_en_i2cm_done : 1;
	uint8_t int1_status_en_apex_event : 1;
	uint8_t resv_1 : 1;
};

// Interrupts configuration register 0 & 1
struct icm_45686_int_bitmap {
	// INT1_CONFIG0
	uint8_t fifo_full;
	uint8_t fifo_ths;
	uint8_t ui_drdy;
	uint8_t ois1;
	uint8_t ui_fsync;
	uint8_t agc_rdy;
	uint8_t ois1_agc_rdy;
	uint8_t reset_done;
	// INT1_CONFIG1
	uint8_t pll_rdy;
	uint8_t wom_x;
	uint8_t wom_y;
	uint8_t wom_z;
	uint8_t i3c_prot_err;
	uint8_t i2cm_done;
	uint8_t edmp_event;
};

// INT1_CONFIG2
struct icm_45686_int1_config2 {
	uint8_t int1_polarity : 1;
	uint8_t int1_mode : 1;
	uint8_t int1_drive : 1;
	uint8_t resv_1 : 5;
};

// int1_drive
enum icm_45686_int1_config2_drive {
	ICM_45686_INT1_CONFIG2_DRIVE_PP = 0x00,
	ICM_45686_INT1_CONFIG2_DRIVE_OD = 0x01
};

// int1_mode
enum icm_45686_int1_config2_mode {
	ICM_45686_INT1_CONFIG2_MODE_PULSE = 0x00,
	ICM_45686_INT1_CONFIG2_MODE_LATCH = 0x01
};

// int1_polarity
enum icm_45686_int1_config2_polarity {
	ICM_45686_INT1_CONFIG2_POLARITY_LOW = 0x00,
	ICM_45686_INT1_CONFIG2_POLARITY_HIGH = 0x01
};

// Interrupts configuration register 2
struct icm_45686_int_pin_cfg {
	enum icm_45686_int1_config2_polarity polarity;
	enum icm_45686_int1_config2_mode mode;
	enum icm_45686_int1_config2_drive drive;
};

// Required registers to configure interrupts
struct icm_45686_int_cfg_01_regs {
	struct icm_45686_int1_config0 config0;
	struct icm_45686_int1_config1 config1;
} __attribute__((__packed__));

// INT1_STATUS0
struct icm_45686_int1_status0 {
	uint8_t int1_status_fifo_full : 1;
	uint8_t int1_status_fifo_ths : 1;
	uint8_t int1_status_drdy : 1;
	uint8_t int1_status_aux1_drdy : 1;
	uint8_t int1_status_ap_fsync : 1;
	uint8_t int1_status_ap_agc_rdy : 1;
	uint8_t int1_status_aux1_agc_rdy : 1;
	uint8_t int1_status_reset_done : 1;
};

// INT1_STATUS1
struct icm_45686_int1_status1 {
	uint8_t int1_status_pll_rdy : 1;
	uint8_t int1_status_wom_x : 1;
	uint8_t int1_status_wom_y : 1;
	uint8_t int1_status_wom_z : 1;
	uint8_t int1_status_i3c_protocol_err : 1;
	uint8_t int1_status_i2cm_done : 1;
	uint8_t int1_status_apex_event : 1;
	uint8_t resv_1 : 1;
};

// Required registers to read interrupts status
struct icm_45686_int_stat_01_regs {
	struct icm_45686_int1_status0 status0;
	struct icm_45686_int1_status1 status1;
} __attribute__((__packed__));

// ACCEL_CONFIG0
struct icm_45686_accel_config0 {
	uint8_t accel_odr : 4;
	uint8_t accel_ui_fs_sel : 3;
	uint8_t resv_1 : 1;
};

/* accel_ui_fs_sel */
enum icm_45686_accel_config0_ui_fs_sel {
	ICM_45686_ACCEL_CONFIG0_UI_FS_SEL_2_G = 0x4,
	ICM_45686_ACCEL_CONFIG0_UI_FS_SEL_4_G = 0x3,
	ICM_45686_ACCEL_CONFIG0_UI_FS_SEL_8_G = 0x2,
	ICM_45686_ACCEL_CONFIG0_UI_FS_SEL_16_G = 0x1,
	ICM_45686_ACCEL_CONFIG0_UI_FS_SEL_32_G = 0x0
};

/* accel_odr */
enum icm_45686_accel_config0_odr {
	ICM_45686_ACCEL_CONFIG0_ODR_1_5625_HZ = 0xF,
	ICM_45686_ACCEL_CONFIG0_ODR_3_125_HZ = 0xE,
	ICM_45686_ACCEL_CONFIG0_ODR_6_25_HZ = 0xD,
	ICM_45686_ACCEL_CONFIG0_ODR_12_5_HZ = 0xC,
	ICM_45686_ACCEL_CONFIG0_ODR_25_HZ = 0xB,
	ICM_45686_ACCEL_CONFIG0_ODR_50_HZ = 0xA,
	ICM_45686_ACCEL_CONFIG0_ODR_100_HZ = 0x9,
	ICM_45686_ACCEL_CONFIG0_ODR_200_HZ = 0x8,
	ICM_45686_ACCEL_CONFIG0_ODR_400_HZ = 0x7,
	ICM_45686_ACCEL_CONFIG0_ODR_800_HZ = 0x6,
	ICM_45686_ACCEL_CONFIG0_ODR_1600_HZ = 0x5,
	ICM_45686_ACCEL_CONFIG0_ODR_3200_HZ = 0x4,
	ICM_45686_ACCEL_CONFIG0_ODR_6400_HZ = 0x3
};

// GYRO_CONFIG0
struct icm_45686_gyro_config0 {
	uint8_t gyro_odr : 4;
	uint8_t gyro_ui_fs_sel : 4;
};

/* gyro_ui_fs_sel */
enum icm_45686_gyro_config0_ui_fs_sel {
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_15_625_DPS = 8,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_31_25_DPS = 7,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_62_5_DPS = 6,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_125_DPS = 5,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_250_DPS = 4,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_500_DPS = 3,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_1000_DPS = 2,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_2000_DPS = 1,
	ICM_45686_GYRO_CONFIG0_UI_FS_SEL_4000_DPS = 0
};

/* gyro_odr */
enum icm_45686_gyro_config0_odr {
	ICM_45686_GYRO_CONFIG0_ODR_1_5625_HZ = 0xF,
	ICM_45686_GYRO_CONFIG0_ODR_3_125_HZ = 0xE,
	ICM_45686_GYRO_CONFIG0_ODR_6_25_HZ = 0xD,
	ICM_45686_GYRO_CONFIG0_ODR_12_5_HZ = 0xC,
	ICM_45686_GYRO_CONFIG0_ODR_25_HZ = 0xB,
	ICM_45686_GYRO_CONFIG0_ODR_50_HZ = 0xA,
	ICM_45686_GYRO_CONFIG0_ODR_100_HZ = 0x9,
	ICM_45686_GYRO_CONFIG0_ODR_200_HZ = 0x8,
	ICM_45686_GYRO_CONFIG0_ODR_400_HZ = 0x7,
	ICM_45686_GYRO_CONFIG0_ODR_800_HZ = 0x6,
	ICM_45686_GYRO_CONFIG0_ODR_1600_HZ = 0x5,
	ICM_45686_GYRO_CONFIG0_ODR_3200_HZ = 0x4,
	ICM_45686_GYRO_CONFIG0_ODR_6400_HZ = 0x3
};

// FIFO_CONFIG0
struct icm_45686_fifo_config0 {
	uint8_t fifo_depth : 6;
	uint8_t fifo_mode : 2;
};

/* fifo_mode */
enum icm_45686_fifo_config0_mode {
	ICM_45686_FIFO_CONFIG0_MODE_SNAPSHOT = 0x02,
	ICM_45686_FIFO_CONFIG0_MODE_STREAM = 0x01,
	ICM_45686_FIFO_CONFIG0_MODE_BYPASS = 0x00
};

/* fifo_depth */
enum icm_45686_fifo_config0_depth {
	ICM_45686_FIFO_CONFIG0_DEPTH_MAX = 0x1E, /* Errata AN-000364 */
	ICM_45686_FIFO_CONFIG0_DEPTH_APEX = 0x07,
	ICM_45686_FIFO_CONFIG0_DEPTH_GAF = 0x04
};

// FIFO_CONFIG2
struct icm_45686_fifo_config2 {
	uint8_t resv_1 : 3;
	uint8_t fifo_wr_wm_gt_th : 1;
	uint8_t resv_2 : 3;
	uint8_t fifo_flush : 1;
};

/* fifo_wr_wm_gt_th */
enum icm_45686_fifo_config2_wr_wm_gt_th {
	ICM_45686_FIFO_CONFIG2_WR_WM_EQ_OR_GT_TH = 0x1,
	ICM_45686_FIFO_CONFIG2_WR_WM_EQ_TH = 0x0
};

// FIFO_CONFIG3
struct icm_45686_fifo_config3 {
	uint8_t fifo_if_en : 1;
	uint8_t fifo_accel_en : 1;
	uint8_t fifo_gyro_en : 1;
	uint8_t fifo_hires_en : 1;
	uint8_t fifo_es0_en : 1;
	uint8_t fifo_es1_en : 1;
	uint8_t resv_1 : 2;
};

// FIFO_CONFIG4
struct icm_45686_fifo_config4 {
	uint8_t fifo_es0_6b_9b : 1;
	uint8_t fifo_tmst_fsync_en : 1;
	uint8_t fifo_comp_en : 1;
	uint8_t fifo_comp_nc_flow_cfg : 3;
	uint8_t resv_1 : 2;
};

/* fifo_comp_nc_flow_cfg */
enum icm_45686_fifo_config4_comp_nc_flow_cfg {
	ICM_45686_FIFO_CONFIG4_COMP_NC_FLOW_CFG_EVERY_128_FR = 0x5,
	ICM_45686_FIFO_CONFIG4_COMP_NC_FLOW_CFG_EVERY_64_FR = 0x4,
	ICM_45686_FIFO_CONFIG4_COMP_NC_FLOW_CFG_EVERY_32_FR = 0x3,
	ICM_45686_FIFO_CONFIG4_COMP_NC_FLOW_CFG_EVERY_16_FR = 0x2,
	ICM_45686_FIFO_CONFIG4_COMP_NC_FLOW_CFG_EVERY_8_FR = 0x1,
	ICM_45686_FIFO_CONFIG4_COMP_NC_FLOW_CFG_DIS = 0x0
};

/* fifo_es0_6b_9b */
enum icm_45686_fifo_config4_es0_6b_9b {
	ICM_45686_FIFO_CONFIG4_ES0_9B = 0x1,
	ICM_45686_FIFO_CONFIG4_ES0_6B = 0x0
};

// TMST_WOM_CONFIG
struct icm_45686_tmst_wom_config {
	uint8_t wom_int_dur : 2;
	uint8_t wom_int_mode : 1;
	uint8_t wom_mode : 1;
	uint8_t wom_en : 1;
	uint8_t tmst_resol : 1;
	uint8_t tmst_delta_en : 1;
	uint8_t resv_1 : 1;
};

/* tmst_resol */
enum icm_45686_tmst_wom_config_resol {
	ICM_45686_TMST_WOM_CONFIG_RESOL_16_US = 0x01,
	ICM_45686_TMST_WOM_CONFIG_RESOL_1_US = 0x00
};

/* wom_mode */
enum icm_45686_tmst_wom_config_mode {
	ICM_45686_TMST_WOM_CONFIG_MODE_CMP_PREV = 0x01,
	ICM_45686_TMST_WOM_CONFIG_MODE_CMP_INIT = 0x00
};

/* wom_int_mode*/
enum icm_45686_tmst_wom_config_int_mode {
	ICM_45686_TMST_WOM_CONFIG_INT_MODE_ANDED = 0x01,
	ICM_45686_TMST_WOM_CONFIG_INT_MODE_ORED = 0x00
};

/* wom_int_dur */
enum icm_45686_tmst_wom_config_int_dur {
	ICM_45686_TMST_WOM_CONFIG_INT_DUR_1_SMPL = 0x00,
	ICM_45686_TMST_WOM_CONFIG_INT_DUR_2_SMPL = 0x01,
	ICM_45686_TMST_WOM_CONFIG_INT_DUR_3_SMPL = 0x02,
	ICM_45686_TMST_WOM_CONFIG_INT_DUR_4_SMPL = 0x03
};

// FSYNC_CONFIG0
struct icm_45686_fsync_config0 {
	uint8_t ap_fsync_sel : 3;
	uint8_t ap_fsync_flag_clear_sel : 1;
	uint8_t resv_1 : 4;
};

/* ap_fsync_sel */
enum icm_45686_fsync_config0_ap_fsync_sel {
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_NO = 0x0,
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_TEMP = 0x1,
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_GYRO_X = 0x2,
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_GYRO_Y = 0x3,
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_GYRO_Z = 0x4,
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_ACCEL_X = 0x5,
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_ACCEL_Y = 0x6,
	ICM_45686_FSYNC_CONFIG0_AP_FSYNC_ACCEL_Z = 0x7
};

// FSYNC_CONFIG1
struct icm_45686_fsync_config1 {
	uint8_t aux1_fsync_sel : 3;
	uint8_t aux1_fsync_flag_clear_sel : 1;
	uint8_t resv_1 : 4;
};

// RTC_CONFIG
struct icm_45686_rtc_config {
	uint8_t resv_1 : 5;
	uint8_t rtc_mode : 1;
	uint8_t rtc_align : 1;
	uint8_t resv_2 : 1;
};

// DMP_EXT_SEN_ODR_CFG
struct icm_45686_dmp_ext_sen_odr_cfg {
	uint8_t apex_odr : 3;
	uint8_t ext_odr : 3;
	uint8_t ext_sensor_en : 1;
	uint8_t resv_1 : 1;
};

/* ext_odr */
enum icm_45686_dmp_ext_sen_odr_cfg_ext_odr {
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_3_25_HZ = 0x00,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_6_25_HZ = 0x01,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_12_5_HZ = 0x02,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_25_HZ = 0x03,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_50_HZ = 0x04,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_100_HZ = 0x05,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_200_HZ = 0x06,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_EXT_ODR_400_HZ = 0x07
};

/* apex_odr */
enum icm_45686_dmp_ext_sen_odr_cfg_apex_odr {
	ICM_45686_DMP_EXT_SEN_ODR_CFG_APEX_ODR_25_HZ = 0x00,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_APEX_ODR_50_HZ = 0x01,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_APEX_ODR_100_HZ = 0x02,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_APEX_ODR_200_HZ = 0x03,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_APEX_ODR_400_HZ = 0x04,
	ICM_45686_DMP_EXT_SEN_ODR_CFG_APEX_ODR_800_HZ = 0x05
};

// ODR_DECIMATE_CONFIG
struct icm_45686_odr_decimate_config {
	uint8_t accel_fifo_odr_dec : 4;
	uint8_t gyro_fifo_odr_dec : 4;
};

/* gyro_fifo_odr_dec */
enum icm_45686_odr_decimate_config_gyro_dec {
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_1 = 0x0,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_2 = 0x1,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_4 = 0x2,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_8 = 0x3,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_16 = 0x4,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_32 = 0x5,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_64 = 0x6,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_128 = 0x7,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_256 = 0x8,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_512 = 0x9,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_1024 = 0xA,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_2048 = 0xB,
	ICM_45686_ODR_DECIMATE_CONFIG_GYRO_DEC_4096 = 0xC
};

/* accel_fifo_odr_dec */
enum icm_45686_odr_decimate_config_accel_dec {
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_1 = 0x0,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_2 = 0x1,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_4 = 0x2,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_8 = 0x3,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_16 = 0x4,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_32 = 0x5,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_64 = 0x6,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_128 = 0x7,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_256 = 0x8,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_512 = 0x9,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_1024 = 0xA,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_2048 = 0xB,
	ICM_45686_ODR_DECIMATE_CONFIG_ACCEL_DEC_4096 = 0xC
};

// EDMP_APEX_EN0
struct icm_45686_edmp_apex_en0 {
	uint8_t tap_en : 1;
	uint8_t reserved0 : 1;
	uint8_t reserved1 : 1;
	uint8_t tilt_en : 1;
	uint8_t pedo_en : 1;
	uint8_t ff_en : 1;
	uint8_t r2w_en : 1;
	uint8_t smd_en : 1;
};

// EDMP_APEX_EN1
struct icm_45686_edmp_apex_en1 {
	uint8_t soft_hard_iron_corr_en : 1;
	uint8_t init_en : 1;
	uint8_t power_save_en : 1;
	uint8_t basic_smd_en : 1;
	uint8_t resv_1 : 1;
	uint8_t feature3_en : 1;
	uint8_t edmp_enable : 1;
	uint8_t resv_2 : 1;
};

// APEX_BUFFER_MGMT
struct icm_45686_apex_buffer_mgmt {
	uint8_t step_count_edmp_wptr : 2;
	uint8_t step_count_host_rptr : 2;
	uint8_t ff_duration_edmp_wptr : 2;
	uint8_t ff_duration_host_rptr : 2;
};

// INTF_CONFIG0
struct icm_45686_intf_config0 {
	uint8_t ap_spi_mode : 1;
	uint8_t ap_spi_34_mode : 1;
	uint8_t resv_1 : 3;
	uint8_t virtual_access_aux1_en : 1;
	uint8_t resv_2 : 2;
};

// INTF_CONFIG1_OVRD
struct icm_45686_intf_config1_ovrd {
	uint8_t ap_spi_mode_ovrd_val : 1;
	uint8_t ap_spi_mode_ovrd : 1;
	uint8_t ap_spi_34_mode_ovrd_val : 1;
	uint8_t ap_spi_34_mode_ovrd : 1;
	uint8_t resv_1 : 4;
};

/* ap_spi_34_mode_ovrd_val */
enum icm_45686_intf_config1_ovrd_34_ovrd_val {
	ICM_45686_INTF_CONFIG1_OVRD_34_OVRD_VAL_3_WIRE = 0x0,
	ICM_45686_INTF_CONFIG1_OVRD_34_OVRD_VAL_4_WIRE = 0x1
};

/* ap_spi_mode_ovrd_val */
enum icm_45686_intf_config1_ovrd_mode_ovrd_val {
	ICM_45686_INTF_CONFIG1_OVRD_MODE_OVRD_VAL_0_OR_3 = 0x0,
	ICM_45686_INTF_CONFIG1_OVRD_MODE_OVRD_VAL_1_OR_2 = 0x1
};

// INTF_AUX_CONFIG
struct icm_45686_intf_aux_config {
	uint8_t aux1_spi_mode : 1;
	uint8_t aux1_spi_34_mode : 1;
	uint8_t aux2_spi_mode : 1;
	uint8_t resv_1 : 5;
};

// IOC_PAD_SCENARIO
struct icm_45686_ioc_pad_scenario {
	uint8_t aux1_enable : 1;
	uint8_t aux1_mode : 2;
	uint8_t aux2_enable : 1;
	uint8_t pads_int2_cfg : 2;
	uint8_t resv_1 : 2;
};

// IOC_PAD_SCENARIO_AUX_OVRD
struct icm_45686_ioc_pad_scenario_aux_ovrd {
	uint8_t aux1_enable_ovrd_val : 1;
	uint8_t aux1_enable_ovrd : 1;
	uint8_t aux1_mode_ovrd_val : 2;
	uint8_t aux1_mode_ovrd : 1;
	uint8_t aux2_enable_ovrd_val : 1;
	uint8_t aux2_enable_ovrd : 1;
	uint8_t resv_1 : 1;
};

// IOC_PAD_SCENARIO_OVRD
struct icm_45686_ioc_pad_scenario_ovrd {
	uint8_t pads_int2_cfg_ovrd_val : 2;
	uint8_t pads_int2_cfg_ovrd : 1;
	uint8_t resv_1 : 5;
};

/* pads_int2_cfg_ovrd_val */
enum icm_45686_ioc_pad_scenario_ovrd_pads_int2_cfg_ovrd_val {
	ICM_45686_IOC_PAD_SCENARIO_OVRD_INT2_CFG_OVRD_VAL_INT2 = 0,
	ICM_45686_IOC_PAD_SCENARIO_OVRD_INT2_CFG_OVRD_VAL_FSYNC = 1,
	ICM_45686_IOC_PAD_SCENARIO_OVRD_INT2_CFG_OVRD_VAL_CLKIN = 2,
	ICM_45686_IOC_PAD_SCENARIO_OVRD_INT2_CFG_OVRD_VAL_DRDY_INTR = 3
};

// DRIVE_CONFIG0
struct icm_45686_drive_config0 {
	uint8_t virtual_access_aux2_en : 1;
	uint8_t pads_spi_slew : 3;
	uint8_t pads_i2c_slew : 3;
	uint8_t resv_1 : 1;
};

/* pads_i2c_slew */
enum icm_45686_drive_config0_pads_i2c_slew {
	ICM_45686_DRIVE_CONFIG0_PADS_I2C_SLEW_20NS = 0x0,
	ICM_45686_DRIVE_CONFIG0_PADS_I2C_SLEW_7NS = 0x2
};

/* pads_spi_slew */
enum icm_45686_drive_config0_pads_spi_slew {
	ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_38NS = 0x0,
	ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_14NS = 0x1,
	ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_10NS = 0x2,
	ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_7NS = 0x3,
	ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_5NS = 0x4,
	ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_4NS = 0x5,
	ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_0_5NS = 0x6
};

// DRIVE_CONFIG1
struct icm_45686_drive_config1 {
	uint8_t pads_i3c_sdr_slew : 3;
	uint8_t pads_i3c_ddr_slew : 3;
	uint8_t resv_1 : 2;
};

// DRIVE_CONFIG2
struct icm_45686_drive_config2 {
	uint8_t pads_slew : 3;
	uint8_t resv_1 : 5;
};

// REG_MISC1
struct icm_45686_reg_misc1 {
	uint8_t osc_id_ovrd : 4;
	uint8_t resv_1 : 4;
};

/* osc_id_ovrd */
enum icm_45686_reg_misc1_osc_id_ovrd {
	ICM_45686_REG_MISC1_OSC_ID_OVRD_OFF = 0x0,
	ICM_45686_REG_MISC1_OSC_ID_OVRD_EDOSC = 0x1,
	ICM_45686_REG_MISC1_OSC_ID_OVRD_RCOSC = 0x2,
	ICM_45686_REG_MISC1_OSC_ID_OVRD_PLL = 0x4,
	ICM_45686_REG_MISC1_OSC_ID_OVRD_EXT_CLK = 0x8
};

// INT_APEX_CONFIG0
struct icm_45686_int_apex_config0 {
	uint8_t int_status_mask_pin_tap_detect : 1;
	uint8_t int_status_mask_pin_high_g_det : 1;
	uint8_t int_status_mask_pin_low_g_det : 1;
	uint8_t int_status_mask_pin_tilt_det : 1;
	uint8_t int_status_mask_pin_step_cnt_ovfl : 1;
	uint8_t int_status_mask_pin_step_det : 1;
	uint8_t int_status_mask_pin_ff_det : 1;
	uint8_t int_status_mask_pin_r2w_wake_det : 1;
};

// INT_APEX_CONFIG1
struct icm_45686_int_apex_config1 {
	uint8_t int_status_mask_pin_r2w_sleep_det : 1;
	uint8_t int_status_mask_pin_smd_det : 1;
	uint8_t int_status_mask_pin_selftest_done : 1;
	uint8_t resv_1 : 1;
	uint8_t int_status_mask_pin_sa_done : 1;
	uint8_t int_status_mask_pin_basic_smd : 1;
	uint8_t resv_2 : 2;
};

// INT_APEX_STATUS0
struct icm_45686_int_apex_status0 {
	uint8_t int_status_tap_det : 1;
	uint8_t int_status_high_g_det : 1;
	uint8_t int_status_low_g_det : 1;
	uint8_t int_status_tilt_det : 1;
	uint8_t int_status_step_cnt_ovfl : 1;
	uint8_t int_status_step_det : 1;
	uint8_t int_status_ff_det : 1;
	uint8_t int_status_r2w_wake_det : 1;
};

// INT_APEX_STATUS1
struct icm_45686_int_apex_status1 {
	uint8_t int_status_r2w_sleep_det : 1;
	uint8_t int_status_smd_det : 1;
	uint8_t int_status_selftest_done : 1;
	uint8_t resv_1 : 1;
	uint8_t int_status_sa_done : 1;
	uint8_t int_status_basic_smd : 1;
	uint8_t resv_2 : 2;
};

// INTF_CONFIG_OVRD_AUX1
struct icm_45686_intf_config_ovrd_aux1 {
	uint8_t aux1_spi_mode_ovrd_val : 1;
	uint8_t aux1_spi_mode_ovrd : 1;
	uint8_t aux1_spi_34_mode_ovrd_val : 1;
	uint8_t aux1_spi_34_mode_ovrd : 1;
	uint8_t aux1_ireg_auto_addr_inc_dis : 1;
	uint8_t resv_1 : 3;
};

// PWR_MGMT_AUX1
struct icm_45686_pwr_mgmt_aux1 {
	uint8_t accel_aux1_en : 1;
	uint8_t gyro_aux1_en : 1;
	uint8_t resv_1 : 6;
};

// FS_SEL_AUX1
struct icm_45686_fs_sel_aux1 {
	uint8_t accel_aux1_fs_sel : 3;
	uint8_t gyro_aux1_fs_sel : 4;
	uint8_t resv_1 : 1;
};

/* aux_gyro_fs_sel */
enum icm_45686_fs_sel_aux1_gyro_fs_sel {
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_15_625_DPS = 8,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_31_25_DPS = 7,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_62_5_DPS = 6,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_125_DPS = 5,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_250_DPS = 4,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_500_DPS = 3,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_1000_DPS = 2,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_2000_DPS = 1,
	ICM_45686_FS_SEL_AUX1_GYRO_FS_SEL_4000_DPS = 0
};

/* aux_accel_fs_sel */
enum icm_45686_fs_sel_aux1_accel_fs_sel {
	ICM_45686_FS_SEL_AUX1_ACCEL_FS_SEL_2_G = 0x4,
	ICM_45686_FS_SEL_AUX1_ACCEL_FS_SEL_4_G = 0x3,
	ICM_45686_FS_SEL_AUX1_ACCEL_FS_SEL_8_G = 0x2,
	ICM_45686_FS_SEL_AUX1_ACCEL_FS_SEL_16_G = 0x1,
	ICM_45686_FS_SEL_AUX1_ACCEL_FS_SEL_32_G = 0x0
};

// INT2_CONFIG0
struct icm_45686_int2_config0 {
	uint8_t int2_status_en_fifo_full : 1;
	uint8_t int2_status_en_fifo_ths : 1;
	uint8_t int2_status_en_drdy : 1;
	uint8_t int2_status_en_aux1_drdy : 1;
	uint8_t int2_status_en_ap_fsync : 1;
	uint8_t int2_status_en_ap_agc_rdy : 1;
	uint8_t int2_status_en_aux1_agc_rdy : 1;
	uint8_t int2_status_en_reset_done : 1;
};

// INT2_CONFIG1
struct icm_45686_int2_config1 {
	uint8_t int2_status_en_pll_rdy : 1;
	uint8_t int2_status_en_wom_x : 1;
	uint8_t int2_status_en_wom_y : 1;
	uint8_t int2_status_en_wom_z : 1;
	uint8_t int2_status_en_i3c_protocol_err : 1;
	uint8_t int2_status_en_i2cm_done : 1;
	uint8_t int2_status_en_apex_event : 1;
	uint8_t resv_1 : 1;
};

// INT2_CONFIG2
struct icm_45686_int2_config2 {
	uint8_t int2_polarity : 1;
	uint8_t int2_mode : 1;
	uint8_t int2_drive : 1;
	uint8_t resv_1 : 5;
};

/* int2_drive */
enum icm_45686_int2_config2_drive {
	ICM_45686_INT2_CONFIG2_DRIVE_PP = 0x00,
	ICM_45686_INT2_CONFIG2_DRIVE_OD = 0x01
};

/* int2_mode */
enum icm_45686_int2_config2_mode {
	ICM_45686_INT2_CONFIG2_MODE_PULSE = 0x00,
	ICM_45686_INT2_CONFIG2_MODE_LATCH = 0x01
};

/* int2_polarity */
enum icm_45686_int2_config2_polarity {
	ICM_45686_INT2_CONFIG2_POLARITY_LOW = 0x00,
	ICM_45686_INT2_CONFIG2_POLARITY_HIGH = 0x01
};

// INT2_STATUS0
struct icm_45686_int2_status0 {
	uint8_t int2_status_fifo_full : 1;
	uint8_t int2_status_fifo_ths : 1;
	uint8_t int2_status_drdy : 1;
	uint8_t int2_status_aux1_drdy : 1;
	uint8_t int2_status_ap_fsync : 1;
	uint8_t int2_status_ap_agc_rdy : 1;
	uint8_t int2_status_aux1_agc_rdy : 1;
	uint8_t int2_status_reset_done : 1;
};

// INT2_STATUS1
struct icm_45686_int2_status1 {
	uint8_t int2_status_pll_rdy : 1;
	uint8_t int2_status_wom_x : 1;
	uint8_t int2_status_wom_y : 1;
	uint8_t int2_status_wom_z : 1;
	uint8_t int2_status_i3c_protocol_err : 1;
	uint8_t int2_status_i2cm_done : 1;
	uint8_t int2_status_apex_event : 1;
	uint8_t resv_1 : 1;
};

// INTF_CONFIG_OVRD_AUX2
struct icm_45686_intf_config_ovrd_aux2 {
	uint8_t aux2_spi_mode_ovrd_val : 1;
	uint8_t aux2_spi_mode_ovrd : 1;
	uint8_t aux2_ireg_auto_addr_inc_dis : 1;
	uint8_t resv_1 : 5;
};

// PWR_MGMT_AUX2
struct icm_45686_pwr_mgmt_aux2 {
	uint8_t accel_aux2_en : 1;
	uint8_t gyro_aux2_en : 1;
	uint8_t resv_1 : 6;
};

// FS_SEL_AUX2
struct icm_45686_fs_sel_aux2 {
	uint8_t accel_aux2_fs_sel : 3;
	uint8_t gyro_aux2_fs_sel : 4;
	uint8_t resv_1 : 1;
};

/* aux_gyro_fs_sel */
enum icm_45686_fs_sel_aux2_gyro_fs_sel {
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_15_625_DPS = 8,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_31_25_DPS = 7,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_62_5_DPS = 6,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_125_DPS = 5,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_250_DPS = 4,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_500_DPS = 3,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_1000_DPS = 2,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_2000_DPS = 1,
	ICM_45686_FS_SEL_AUX2_GYRO_FS_SEL_4000_DPS = 0
};

/* aux_accel_fs_sel */
enum icm_45686_fs_sel_aux2_accel_fs_sel {
	ICM_45686_FS_SEL_AUX2_ACCEL_FS_SEL_2_G = 0x4,
	ICM_45686_FS_SEL_AUX2_ACCEL_FS_SEL_4_G = 0x3,
	ICM_45686_FS_SEL_AUX2_ACCEL_FS_SEL_8_G = 0x2,
	ICM_45686_FS_SEL_AUX2_ACCEL_FS_SEL_16_G = 0x1,
	ICM_45686_FS_SEL_AUX2_ACCEL_FS_SEL_32_G = 0x0
};

// INT_AUX2_CONFIG
struct icm_45686_int_aux2_config {
	uint8_t int_en_aux2_agc_rdy : 1;
	uint8_t int_en_aux2_reset_done : 1;
	uint8_t int_en_aux2_pll_rdy : 1;
	uint8_t int_en_aux2_drdy : 1;
	uint8_t resv_1 : 4;
};

// INT_AUX2_STATUS
struct icm_45686_int_aux2_status {
	uint8_t int_status_aux2_agc_rdy : 1;
	uint8_t int_status_aux2_reset_done : 1;
	uint8_t int_status_aux2_pll_rdy : 1;
	uint8_t int_status_aux2_drdy : 1;
	uint8_t resv_1 : 4;
};

// WHO_AM_I
struct icm_45686_who_am_i {
	uint8_t who_am_i : 8;
};

// REG_HOST_MSG
struct icm_45686_reg_host_msg {
	uint8_t testopenable : 1;
	uint8_t resv_1 : 4;
	uint8_t edmp_on_demand_en : 1;
	uint8_t resv_2 : 2;
};

// IREG_ADDR_15_8
struct icm_45686_ireg_addr_15_8 {
	uint8_t ireg_addr_15_8 : 8;
};

// IREG_ADDR_7_0
struct icm_45686_ireg_addr_7_0 {
	uint8_t ireg_addr_7_0 : 8;
};

// IREG_DATA
struct icm_45686_ireg_data {
	uint8_t ireg_data : 8;
};

// REG_MISC2
struct icm_45686_reg_misc2 {
	uint8_t ireg_done : 1;
	uint8_t soft_rst : 1;
	uint8_t resv_1 : 6;
};

enum icm_45686_ip_reg_top1_adr {
	ICM_45686_SIFS_IXC_ERROR_STATUS = 0xa24b,
	ICM_45686_EDMP_PRGRM_IRQ0_0 = 0xa24f,
	ICM_45686_EDMP_PRGRM_IRQ0_1 = 0xa250,
	ICM_45686_EDMP_PRGRM_IRQ1_0 = 0xa251,
	ICM_45686_EDMP_PRGRM_IRQ1_1 = 0xa252,
	ICM_45686_EDMP_PRGRM_IRQ2_0 = 0xa253,
	ICM_45686_EDMP_PRGRM_IRQ2_1 = 0xa254,
	ICM_45686_EDMP_SP_START_ADDR = 0xa255,
	ICM_45686_SMC_CONTROL_0 = 0xa258,
	ICM_45686_SMC_CONTROL_1 = 0xa259,
	ICM_45686_STC_CONFIG = 0xa263,
	ICM_45686_SREG_CTRL = 0xa267,
	ICM_45686_SIFS_I3C_STC_CFG = 0xa268,
	ICM_45686_INT_PULSE_MIN_ON_INTF0 = 0xa269,
	ICM_45686_INT_PULSE_MIN_ON_INTF1 = 0xa26a,
	ICM_45686_INT_PULSE_MIN_OFF_INTF0 = 0xa26b,
	ICM_45686_INT_PULSE_MIN_OFF_INTF1 = 0xa26c,
	ICM_45686_ISR_0_7 = 0xa26e,
	ICM_45686_ISR_8_15 = 0xa26f,
	ICM_45686_ISR_16_23 = 0xa270,
	ICM_45686_STATUS_MASK_PIN_0_7 = 0xa271,
	ICM_45686_STATUS_MASK_PIN_8_15 = 0xa272,
	ICM_45686_STATUS_MASK_PIN_16_23 = 0xa273,
	ICM_45686_INT_I2CM_SOURCE = 0xa274,
	ICM_45686_ACCEL_WOM_X_THR = 0xa27e,
	ICM_45686_ACCEL_WOM_Y_THR = 0xa27f,
	ICM_45686_ACCEL_WOM_Z_THR = 0xa280,
	ICM_45686_SELFTEST = 0xa290,
	ICM_45686_IPREG_MISC = 0xa297,
	ICM_45686_SW_PLL1_TRIM = 0xa2a2,
	ICM_45686_FIFO_SRAM_SLEEP = 0xa2a7
};

// SIFS_IXC_ERROR_STATUS
struct icm_45686_sifs_ixc_error_status {
	uint8_t sifs_ixc_timeout_err : 1;
	uint8_t aux1_sifs_ixc_timeout_err : 1;
	uint8_t resv_1 : 6;
};

// EDMP_SP_START_ADDR
struct icm_45686_edmp_sp_start_addr {
	uint8_t edmp_sp_start_addr : 8;
};

// SMC_CONTROL_0
struct icm_45686_smc_control_0 {
	uint8_t tmst_en : 1;
	uint8_t tmst_fsync_en : 1;
	uint8_t tmst_force_aux_fine_en : 1;
	uint8_t temp_dis : 1;
	uint8_t accel_lp_clk_sel : 1;
	uint8_t resv_1 : 3;
};

/* accel_lp_clk_sel */
enum icm_45686_smc_control_0_accel_lp_clk_sel {
	ICM_45686_SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC = 0x01,
	ICM_45686_SMC_CONTROL_0_ACCEL_LP_CLK_WUOSC = 0x00
};

// SMC_CONTROL_1
struct icm_45686_smc_control_1 {
	uint8_t resv_1 : 3;
	uint8_t sreg_aux_accel_only_en : 1;
	uint8_t resv_2 : 4;
};

// STC_CONFIG
struct icm_45686_stc_config {
	uint8_t resv_1 : 2;
	uint8_t stc_sensor_sel : 2;
	uint8_t resv_2 : 4;
};

// SREG_CTRL
struct icm_45686_sreg_ctrl {
	uint8_t resv_1 : 1;
	uint8_t sreg_data_endian_sel : 1;
	uint8_t resv_2 : 6;
};

/* sreg_data_endian_sel */
enum icm_45686_sreg_ctrl_sreg_data_endian_sel {
	ICM_45686_SREG_CTRL_SREG_DATA_BIG_ENDIAN = 0x01,
	ICM_45686_SREG_CTRL_SREG_DATA_LITTLE_ENDIAN = 0x00
};

// SIFS_I3C_STC_CFG
struct icm_45686_sifs_i3c_stc_cfg {
	uint8_t resv_1 : 2;
	uint8_t i3c_stc_mode : 1;
	uint8_t resv_2 : 5;
};

// INT_PULSE_MIN_ON_INTF0
struct icm_45686_int_pulse_min_on_intf0 {
	uint8_t int0_tpulse_duration : 3;
	uint8_t resv_1 : 5;
};

// INT_PULSE_MIN_ON_INTF1
struct icm_45686_int_pulse_min_on_intf1 {
	uint8_t int1_tpulse_duration : 3;
	uint8_t resv_1 : 5;
};

// INT_PULSE_MIN_OFF_INTF0
struct icm_45686_int_pulse_min_off_intf0 {
	uint8_t int0_tdeassert_disable : 3;
	uint8_t resv_1 : 5;
};

// INT_PULSE_MIN_OFF_INTF1
struct icm_45686_int_pulse_min_off_intf1 {
	uint8_t int1_tdeassert_disable : 3;
	uint8_t resv_1 : 5;
};

// ISR_0_7
struct icm_45686_isr_0_7 {
	uint8_t int_status_accel_drdy_pin_0 : 1;
	uint8_t resv_1 : 2;
	uint8_t int_status_ext_odr_drdy_pin_0 : 1;
	uint8_t resv_2 : 1;
	uint8_t int_status_on_demand_pin_0 : 1;
	uint8_t resv_3 : 2;
};

// ISR_8_15
struct icm_45686_isr_8_15 {
	uint8_t int_status_accel_drdy_pin_1 : 1;
	uint8_t resv_1 : 2;
	uint8_t int_status_ext_odr_drdy_pin_1 : 1;
	uint8_t resv_2 : 1;
	uint8_t int_status_on_demand_pin_1  : 1;
	uint8_t resv_3 : 2;
};

// ISR_16_23
struct icm_45686_isr_16_23 {
	uint8_t int_status_accel_drdy_pin_2 : 1;
	uint8_t resv_1 : 2;
	uint8_t int_status_ext_odr_drdy_pin_2 : 1;
	uint8_t resv_2 : 1;
	uint8_t int_status_on_demand_pin_2 : 1;
	uint8_t resv_3 : 2;
};

// STATUS_MASK_PIN_0_7
struct icm_45686_status_mask_pin_0_7 {
	uint8_t int_accel_drdy_pin_0_dis : 1;
	uint8_t resv_1 : 2;
	uint8_t int_ext_odr_drdy_pin_0_dis : 1;
	uint8_t resv_2 : 1;
	uint8_t int_on_demand_pin_0_dis : 1;
	uint8_t resv_3 : 2;
};

// STATUS_MASK_PIN_8_15
struct icm_45686_status_mask_pin_8_15 {
	uint8_t int_accel_drdy_pin_1_dis : 1;
	uint8_t resv_1 : 2;
	uint8_t int_ext_odr_drdy_pin_1_dis : 1;
	uint8_t resv_2 : 1;
	uint8_t int_on_demand_pin_1_dis : 1;
	uint8_t resv_3 : 2;
};

// STATUS_MASK_PIN_16_23
struct icm_45686_status_mask_pin_16_23 {
	uint8_t int_accel_drdy_pin_2_dis : 1;
	uint8_t resv_1 : 2;
	uint8_t int_ext_odr_drdy_pin_2_dis : 1;
	uint8_t resv_2 : 1;
	uint8_t int_on_demand_pin_2_dis : 1;
	uint8_t resv_3 : 2;
};

enum icm_45686_edmp_int_src_drdy_mask {
	ICM_45686_EDMP_INT_SRC_ACCEL_DRDY_MASK = 0x01,
	ICM_45686_EDMP_INT_SRC_GYRO_DRDY_MASK = 0x02,
	ICM_45686_EDMP_INT_SRC_EXT_INT_DRDY_MASK = 0x04,
	ICM_45686_EDMP_INT_SRC_EXT_ODR_DRDY_MASK = 0x08,
	ICM_45686_EDMP_INT_SRC_WOM_DRDY_MASK = 0x10,
	ICM_45686_EDMP_INT_SRC_ON_DEMAND_MASK = 0x20
};

// INT_I2CM_SOURCE
struct icm_45686_int_i2cm_source {
	uint8_t int_status_i2cm_ioc_ext_trig_en : 1;
	uint8_t int_status_i2cm_smc_ext_odr_en : 1;
	uint8_t resv_1 : 6;
};

// ACCEL_WOM_X_THR
struct icm_45686_accel_wom_x_thr {
	uint8_t wom_x_th : 8;
};

// ACCEL_WOM_Y_THR
struct icm_45686_accel_wom_y_thr {
	uint8_t wom_y_th : 8;
};

// ACCEL_WOM_Z_THR
struct icm_45686_accel_wom_z_thr {
	uint8_t wom_z_th : 8;
};

// SELFTEST
struct icm_45686_selftest {
	uint8_t en_ax_st : 1;
	uint8_t en_ay_st : 1;
	uint8_t en_az_st : 1;
	uint8_t en_gx_st : 1;
	uint8_t en_gy_st : 1;
	uint8_t en_gz_st : 1;
	uint8_t resv_1 : 2;
};

// IPREG_MISC
struct icm_45686_ipreg_misc {
	uint8_t resv_1 : 1;
	uint8_t edmp_idle : 1;
	uint8_t resv_2 : 6;
};

// SW_PLL1_TRIM
struct icm_45686_sw_pll1_trim {
	uint8_t sw_pll1_trim : 8;
};

// FIFO_SRAM_SLEEP
struct icm_45686_fifo_sram_sleep {
	uint8_t fifo_gsleep_shared_sram : 2;
	uint8_t resv_1 : 6;
};

enum icm_45686_ip_reg_sys1_adr {
	ICM_45686_IPREG_SYS1_REG_42 = 0xa42a,
	ICM_45686_IPREG_SYS1_REG_43 = 0xa42b,
	ICM_45686_IPREG_SYS1_REG_56 = 0xa438,
	ICM_45686_IPREG_SYS1_REG_57 = 0xa439,
	ICM_45686_IPREG_SYS1_REG_70 = 0xa446,
	ICM_45686_IPREG_SYS1_REG_71 = 0xa447,
	ICM_45686_IPREG_SYS1_REG_166 = 0xa4a6,
	ICM_45686_IPREG_SYS1_REG_168 = 0xa4a8,
	ICM_45686_IPREG_SYS1_REG_170 = 0xa4aa,
	ICM_45686_IPREG_SYS1_REG_171 = 0xa4ab,
	ICM_45686_IPREG_SYS1_REG_172 = 0xa4ac,
	ICM_45686_IPREG_SYS1_REG_173 = 0xa4ad
};

// IPREG_SYS1_REG_166
struct icm_45686_ipreg_sys1_reg_166 {
	uint8_t resv_1 : 3;
	uint8_t gyro_afsr_mode : 2;
	uint8_t gyro_src_ctrl : 2;
	uint8_t resv_2 : 1;
};

/* gyro_src_ctrl */
enum icm_45686_ipreg_sys1_reg_166_gyro_src_ctrl_sel {
	ICM_45686_IPREG_SYS1_REG_166_GYRO_SRC_CTRL_INTPL_ON_FIR_ON = 0x2,
	ICM_45686_IPREG_SYS1_REG_166_GYRO_SRC_CTRL_INTPL_OFF_FIR_ON = 0x1,
	ICM_45686_IPREG_SYS1_REG_166_GYRO_SRC_CTRL_INTPL_OFF_FIR_OFF = 0x0
};

// IPREG_SYS1_REG_168
struct icm_45686_ipreg_sys1_reg_168 {
	uint8_t gyro_afsr_shared : 1;
	uint8_t gyro_ois_m6_byp : 1;
	uint8_t resv_1 : 6;
};

// IPREG_SYS1_REG_170
struct icm_45686_ipreg_sys1_reg_170 {
	uint8_t resv_1 : 1;
	uint8_t gyro_lp_avg_sel : 4;
	uint8_t gyro_ois_hpfbw_sel : 3;
};

/* gyro_lp_avg_sel */
enum icm_45686_ipreg_sys1_reg_170_gyro_lp_avg_sel {
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_64 = 0xC,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_32 = 0xB,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_20 = 0xA,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_18 = 0x9,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_16 = 0x8,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_11 = 0x7,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_10 = 0x6,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_8 = 0x5,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_7 = 0x4,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_5 = 0x3,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_4 = 0x2,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_2 = 0x1,
	ICM_45686_IPREG_SYS1_REG_170_GYRO_LP_AVG_1 = 0x0
};

// IPREG_SYS1_REG_171
struct icm_45686_ipreg_sys1_reg_171 {
	uint8_t gyro_ois_lpf1bw_sel : 3;
	uint8_t gyro_ois_lpf2bw_sel : 3;
	uint8_t resv_1 : 2;
};

// IPREG_SYS1_REG_172
struct icm_45686_ipreg_sys1_reg_172 {
	uint8_t gyro_ui_lpfbw_sel : 3;
	uint8_t resv_1 : 4;
	uint8_t gyro_ois_hpf1_byp : 1;
};

/* gyro_ui_lpfbw_sel */
enum icm_45686_ipreg_sys1_reg_172_gyro_ui_lpfbw_sel {
	ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_128 = 0x06,
	ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_64 = 0x05,
	ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_32 = 0x04,
	ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_16 = 0x03,
	ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_8 = 0x02,
	ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4 = 0x01,
	ICM_45686_IPREG_SYS1_REG_172_GYRO_UI_LPFBW_NO_FILTER = 0x00
};

// IPREG_SYS1_REG_173
struct icm_45686_ipreg_sys1_reg_173 {
	uint8_t gyro_ois_hpf2_byp : 1;
	uint8_t resv_1 : 7;
};

enum icm_45686_ip_reg_sys2_adr {
	ICM_45686_IPREG_SYS2_REG_24 = 0xa518,
	ICM_45686_IPREG_SYS2_REG_25 = 0xa519,
	ICM_45686_IPREG_SYS2_REG_32 = 0xa520,
	ICM_45686_IPREG_SYS2_REG_33 = 0xa521,
	ICM_45686_IPREG_SYS2_REG_40 = 0xa528,
	ICM_45686_IPREG_SYS2_REG_41 = 0xa529,
	ICM_45686_IPREG_SYS2_REG_123 = 0xa57b,
	ICM_45686_IPREG_SYS2_REG_129 = 0xa581,
	ICM_45686_IPREG_SYS2_REG_130 = 0xa582,
	ICM_45686_IPREG_SYS2_REG_131 = 0xa583,
	ICM_45686_IPREG_SYS2_REG_132 = 0xa584
};

// IPREG_SYS2_REG_123
struct icm_45686_ipreg_sys2_reg_123 {
	uint8_t accel_src_ctrl : 2;
	uint8_t resv_1 : 6;
};

/* accel_src_ctrl */
enum icm_45686_ipreg_sys2_reg_123_accel_src_ctrl_sel {
	ICM_45686_IPREG_SYS2_REG_123_ACCEL_SRC_CTRL_INTPL_ON_FIR_ON = 0x2,
	ICM_45686_IPREG_SYS2_REG_123_ACCEL_SRC_CTRL_INTPL_OFF_FIR_ON = 0x1,
	ICM_45686_IPREG_SYS2_REG_123_ACCEL_SRC_CTRL_INTPL_OFF_FIR_OFF = 0x0
};

// IPREG_SYS2_REG_129
struct icm_45686_ipreg_sys2_reg_129 {
	uint8_t accel_lp_avg_sel : 4;
	uint8_t accel_ois_hpfbw_sel : 3;
	uint8_t resv_1 : 1;
};

/* accel_lp_avg_sel */
enum icm_45686_ipreg_sys2_reg_129_accel_lp_avg_sel {
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_64 = 0xC,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_32 = 0xB,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_20 = 0xA,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_18 = 0x9,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_16 = 0x8,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_11 = 0x7,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_10 = 0x6,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_8 = 0x5,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_7 = 0x4,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_5 = 0x3,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_4 = 0x2,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_2 = 0x1,
	ICM_45686_IPREG_SYS2_REG_129_ACCEL_LP_AVG_1 = 0x0
};

// IPREG_SYS2_REG_130
struct icm_45686_ipreg_sys2_reg_130 {
	uint8_t accel_ois_lpf1bw_sel : 3;
	uint8_t accel_ois_lpf2bw_sel : 3;
	uint8_t resv_1 : 2;
};

// IPREG_SYS2_REG_131
struct icm_45686_ipreg_sys2_reg_131 {
	uint8_t accel_ui_lpfbw_sel : 3;
	uint8_t resv_1 : 5;
};

/* accel_ui_lpfbw_sel */
enum icm_45686_ipreg_sys2_reg_131_accel_ui_lpfbw {
	ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_128 = 0x06,
	ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_64 = 0x05,
	ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_32 = 0x04,
	ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_16 = 0x03,
	ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_8 = 0x02,
	ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4 = 0x01,
	ICM_45686_IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_NO_FILTER = 0x00
};

// IPREG_SYS2_REG_132
struct icm_45686_ipreg_sys2_reg_132 {
	uint8_t accel_ois_hpf1_byp : 1;
	uint8_t accel_ois_hpf2_byp : 1;
	uint8_t accel_ois_m6_byp : 1;
	uint8_t resv_1 : 5;
};

enum icm_45686_ip_reg_bar_adr {
	ICM_45686_IPREG_BAR_REG_57 = 0xa039,
	ICM_45686_IPREG_BAR_REG_58 = 0xa03a,
	ICM_45686_IPREG_BAR_REG_59 = 0xa03b,
	ICM_45686_IPREG_BAR_REG_60 = 0xa03c,
	ICM_45686_IPREG_BAR_REG_61 = 0xa03d,
	ICM_45686_IPREG_BAR_REG_62 = 0xa03e
};

// IPREG_BAR_REG_57
struct icm_45686_ipreg_bar_reg_57 {
	uint8_t resv_1 : 5;
	uint8_t io_opt1 : 1;
	uint8_t io_opt0 : 1;
	uint8_t resv_2 : 1;
};

// IPREG_BAR_REG_58
struct icm_45686_ipreg_bar_reg_58 {
	uint8_t io_opt2 : 1;
	uint8_t resv_1 : 2;
	uint8_t pads_ap_cs_pe_trim_d2a : 1;
	uint8_t pads_ap_cs_pud_trim_d2a : 1;
	uint8_t resv_2 : 1;
	uint8_t pads_ap_sclk_pe_trim_d2a : 1;
	uint8_t pads_ap_sclk_pud_trim_d2a : 1;
};

// IPREG_BAR_REG_59
struct icm_45686_ipreg_bar_reg_59 {
	uint8_t resv_1 : 1;
	uint8_t pads_ap_sdi_pe_trim_d2a : 1;
	uint8_t pads_ap_sdi_pud_trim_d2a : 1;
	uint8_t resv_2 : 1;
	uint8_t pads_ap_sdo_pe_trim_d2a : 1;
	uint8_t pads_ap_sdo_pud_trim_d2a : 1;
	uint8_t resv_3 : 1;
	uint8_t pads_pin7_pe_trim_d2a : 1;
};

// IPREG_BAR_REG_60
struct icm_45686_ipreg_bar_reg_60 {
	uint8_t pads_pin7_cs_pud_trim_d2a : 1;
	uint8_t resv_1 : 1;
	uint8_t pads_aux1_cs_pe_trim_d2a : 1;
	uint8_t pads_aux1_cs_pud_trim_d2a : 1;
	uint8_t pads_aux_sclk_tp2_from_pad_disable_trim_d2a : 1;
	uint8_t pads_aux1_sclk_pe_trim_d2a : 1;
	uint8_t pads_aux1_sclk_pud_trim_d2a : 1;
	uint8_t resv_2 : 1;
};

// IPREG_BAR_REG_61
struct icm_45686_ipreg_bar_reg_61 {
	uint8_t pads_aux1_sdi_pe_trim_d2a : 1;
	uint8_t pads_aux1_sdi_pud_trim_d2a : 1;
	uint8_t resv_1 : 1;
	uint8_t pads_aux1_sdo_pe_trim_d2a : 1;
	uint8_t pads_aux1_sdo_pud_trim_d2a : 1;
	uint8_t resv_2 : 1;
	uint8_t pads_int1_pe_trim_d2a : 1;
	uint8_t pads_int1_pud_trim_d2a : 1;
};

// IPREG_BAR_REG_62
struct icm_45686_ipreg_bar_reg_62 {
	uint8_t resv_1 : 1;
	uint8_t pads_int2_pe_trim_d2a : 1;
	uint8_t pads_int2_pud_trim_d2a : 1;
	uint8_t resv_2 : 5;
};

// Sensor data from registers
struct icm_45686_sensor_data {
	int16_t accel_data[3];
	int16_t gyro_data[3];
	int16_t temp_data;
} __attribute__((__packed__));

// Bank IMEM_SRAM

/* equivalent to 0.5 seconds in sample count at 400Hz */
#define ICM_45686_TAP_TMAX_400HZ 198
/* equivalent to 0.5 seconds in sample count at 800Hz */
#define ICM_45686_TAP_TMAX_800HZ 396
/* equivalent to 0.165 seconds in sample count at 400Hz */
#define ICM_45686_TAP_TMIN_400HZ 66
/* equivalent to 0.165 seconds in sample count at 800Hz */
#define ICM_45686_TAP_TMIN_800HZ 132
/* equivalent to 0.085 seconds in sample count at 400Hz */
#define ICM_45686_TAP_SMUDGE_REJECT_THR_400HZ 34
/* equivalent to 0.085 seconds in sample count at 800Hz */
#define ICM_45686_TAP_SMUDGE_REJECT_THR_800HZ 68

// FIFO configuration

#define ICM_45686_INVALID_VALUE_FIFO ((int16_t) 0x8000)
#define ICM_45686_INVALID_VALUE_FIFO_1B ((int8_t) 0x80)

struct icm_45686_fifo_config {
	uint8_t gyro_en; /**< Enable Gyro in FIFO */
	uint8_t accel_en; /**< Enable Accel in FIFO */
	uint8_t hires_en; /**< Enable High Resolution mode (20-bits long data) */
	uint16_t fifo_wm_th; /**< Watermark threshold value */
	enum icm_45686_fifo_config0_mode fifo_mode; /**< Operating mode of the FIFO */
	enum icm_45686_fifo_config0_depth fifo_depth; /**< FIFO size */
};

// FIFO config registers
struct icm_45686_fifo_config_regs {
	struct icm_45686_fifo_config0 config0;
	uint8_t config1_0;
	uint8_t config1_1;
	struct icm_45686_fifo_config2 config2;
	struct icm_45686_fifo_config3 config3;
	struct icm_45686_fifo_config4 config4;
} __attribute__((__packed__));

// Describe the content of the FIFO header
union icm_45686_fifo_header {
	uint8_t byte;
	struct {
		uint8_t gyro_odr_different : 1;
		uint8_t accel_odr_different : 1;
		uint8_t fsync_bit : 1;
		uint8_t timestamp_bit : 1;
		uint8_t twentybits_bit : 1;
		uint8_t gyro_bit : 1;
		uint8_t accel_bit : 1;
		uint8_t ext_header : 1;
	} bits;
};

// One frame of FIFO header+data
union icm_45686_fifo_frame {
	union icm_45686_fifo_header header;
	struct {
		union icm_45686_fifo_header header;
		int16_t sensor_data[3];
		int8_t temp_data;
	} byte_8;
	struct {
		union icm_45686_fifo_header header;
		int16_t accel_data[3];
		int16_t gyro_data[3];
		int8_t temp_data;
		uint16_t timestamp;
	} byte_16;
	struct {
		union icm_45686_fifo_header header;
		int32_t accel_data[3];
		int32_t gyro_data[3];
		int16_t temp_data;
		uint16_t timestamp;
	} byte_20;
};

typedef struct icm_45686_dsc *icm_45686;

struct icm_45686_dsc {
	struct spi_hal_dev spi;
	int fifo_frame_size;
};

_Static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Only little-endian supported");
_Static_assert(sizeof(struct icm_45686_pwr_mgmt0) == 1, "not 1 byte");

#endif
