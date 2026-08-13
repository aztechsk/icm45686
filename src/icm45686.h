/*
 * icm45686.h
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

/**
 * @file icm45686.h
 * @brief API for TDK ICM-45686.
 *
 * @note All functions return 0 on success.
 *       Negative values indicate errors. Specifically:
 *       - `-EHW` denotes a hardware-level fault.
 *       - Errors propagated from the SPI HAL layer (`spi_hal_xfer()` failure)
 *         may include `-EHW` (hardware error) or `-EDMA` (DMA error).
 */

#ifndef ICM45686_H
#define ICM45686_H

#include "spi_hal.h"
#include "icm45686_def.h"

/**
 * @brief Initialize the ICM-45686 descriptor.
 *
 * @param icm Device handle.
 */
void init_icm_45686(icm_45686 icm);

/**
 * @brief Perform a device soft reset and wait until it completes.
 *
 * Issues a soft reset by writing the soft-reset bit in @ref ICM_45686_REG_MISC2.
 *
 * @param icm Device handle.
 * @return 0 on success; negative error code propagated from \c spi_hal_xfer() on I/O failure;
 *         negative error (\c -EHW) on timeout.
 */
int icm_45686_soft_reset(icm_45686 icm);

/**
 * @brief Read and verify the device WHO_AM_I value.
 *
 * Reads @ref ICM_45686_WHOAMI (0x72) and compares it against the expected
 * device identifier.
 *
 * @param icm Device handle.
 * @return 0 if the read value matches the expected identifier; negative error
 *         code propagated from \c spi_hal_xfer() on I/O failure;
 *         negative error (e.g., \c -EHW) if the value does not match.
 */
int icm_45686_whoami_check(icm_45686 icm);

/**
 * @brief Configure SPI pads slew.
 *
 * @param icm  Device handle.
 * @param slew The requested slew time.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_spi_pads_slew(icm_45686 icm, enum icm_45686_drive_config0_pads_spi_slew slew);

/**
 * @brief Configure accel mode.
 *
 * @param icm  Device handle.
 * @param mode The requested mode.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_accel_mode(icm_45686 icm, enum icm_45686_pwr_mgmt0_accel_mode mode);

/**
 * @brief Configure gyro mode.
 *
 * @param icm  Device handle.
 * @param mode The requested mode.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_gyro_mode(icm_45686 icm, enum icm_45686_pwr_mgmt0_gyro_mode mode);

/**
 * @brief Configure accel output data rate.
 *
 * @param icm  Device handle.
 * @param freq The requested frequency.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_accel_freq(icm_45686 icm, enum icm_45686_accel_config0_odr freq);

/**
 * @brief Configure gyro output data rate.
 *
 * @param icm  Device handle.
 * @param freq The requested frequency.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_gyro_freq(icm_45686 icm, enum icm_45686_gyro_config0_odr freq);

/**
 * @brief Set accel full scale range.
 *
 * @param icm Device handle.
 * @param fsr Requested full scale range.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_accel_fsr(icm_45686 icm, enum icm_45686_accel_config0_ui_fs_sel fsr);

/**
 * @brief Set gyro full scale range.
 *
 * @param icm Device handle.
 * @param fsr Requested full scale range.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_gyro_fsr(icm_45686 icm, enum icm_45686_gyro_config0_ui_fs_sel fsr);

/**
 * @brief Set accel Low-Power averaging value.
 *
 * @param icm Device handle.
 * @param avg  Requested averaging value.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_accel_lp_avg(icm_45686 icm, enum icm_45686_ipreg_sys2_reg_129_accel_lp_avg_sel avg);

/**
 * @brief Set gyro Low-Power averaging value.
 *
 * @param icm Device handle.
 * @param avg  Requested averaging value.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_gyro_lp_avg(icm_45686 icm, enum icm_45686_ipreg_sys1_reg_170_gyro_lp_avg_sel avg);

/**
 * @brief Set accel Low-Noise bandwidth value.
 *
 * @param icm Device handle.
 * @param bw  Requested bandwidth value.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_accel_ln_bw(icm_45686 icm, enum icm_45686_ipreg_sys2_reg_131_accel_ui_lpfbw bw);

/**
 * @brief Set gyro Low-Noise bandwidth value.
 *
 * @param icm Device handle.
 * @param bw  Requested bandwidth value.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_gyro_ln_bw(icm_45686 icm, enum icm_45686_ipreg_sys1_reg_172_gyro_ui_lpfbw_sel bw);

/**
 * @brief Get current sensor data from the registers.
 *
 * @param icm  Device handle.
 * @param data Current accel, gyro and temperature data from the registers.
 * @return 0 on success; negative value on error.
 */
int icm_45686_get_reg_data(icm_45686 icm, struct icm_45686_sensor_data *data);

/**
 * @brief Gets the current FIFO configuration.
 *
 * @param icm Device handle.
 * @param cfg Structure containing the FIFO configuration.
 * @return 0 on success; negative value on error.
 */
int icm_45686_get_fifo_config(icm_45686 icm, struct icm_45686_fifo_config *cfg);

/**
 * @brief Configures the FIFO to the specified state.
 *
 * @param icm Device handle.
 * @param cfg Structure containing the FIFO configuration.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_fifo_config(icm_45686 icm, const struct icm_45686_fifo_config *cfg);

/**
 * @brief Flush FIFO content.
 *
 * @param icm Device handle.
 * @return 0 on success; negative value on error.
 */
int icm_45686_flush_fifo(icm_45686 icm);

/**
 * @brief Get FIFO frame count.
 *
 * @param icm   Device handle.
 * @param count The number of frames in the FIFO.
 * @return 0 on success; negative value on error.
 */
int icm_45686_get_frame_count(icm_45686 icm, int *count);

/**
 * @brief Get one frame of FIFO data.
 *
 * @param icm   Device handle.
 * @param frame Accel, gyro and temperature data from the top frame on the FIFO.
 * @return 0 on success; negative value on error.
 */
int icm_45686_get_fifo_frame(icm_45686 icm, union icm_45686_fifo_frame *frame);

/**
 * @brief Configure interrupt sources.
 *
 * @param icm Device handle.
 * @param num Interrupt number.
 * @param cfg The interrupt sources structure.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_config_int_source(icm_45686 icm, enum icm_45686_int_num num, const struct icm_45686_int_bitmap *cfg);

/**
 * @brief Configure interrupt pin behavior.
 *
 * @param icm Device handle.
 * @param num Interrupt number.
 * @param cfg Structure with the requested configuration.
 * @return 0 on success; negative value on error.
 */
int icm_45686_set_config_int_pin(icm_45686 icm, enum icm_45686_int_num num, const struct icm_45686_int_pin_cfg *cfg);

/**
 * @brief Read interrupt status.
 *
 * @param icm  Device handle.
 * @param num  Interrupt number.
 * @param stat The interrupt state structure.
 * @return 0 on success; negative value on error.
 */
int icm_45686_get_int_status(icm_45686 icm, enum icm_45686_int_num num, struct icm_45686_int_bitmap *stat);

/**
 * @brief Select which clock to use when in Low Power mode.
 *
 * Use `ICM_45686_SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC` for Low Power (LP) mode.
 * Use `ICM_45686_SMC_CONTROL_0_ACCEL_LP_CLK_WUOSC` for Ultra Low Power (ULP) mode.
 *
 * @param icm Device handle.
 * @param clk Selected clock.
 * @return 0 on success; negative value on error.
 */
int icm_45686_select_accel_lp_clk(icm_45686 icm, enum icm_45686_smc_control_0_accel_lp_clk_sel clk);

/**
 * @brief Direct UI/Bank0 READ starting at @p adr.
 *
 * Buffer @p data will be overwritten with the received data.
 *
 * @param icm  Device handle.
 * @param adr  Bank0 address (from @ref icm_45686_bank0_adr).
 * @param data Destination buffer to be overwritten by received bytes.
 * @param n    Number of bytes to read (>0).
 * @return 0 on success; negative error code propagated from \c spi_hal_xfer() on failure.
 */
int icm_45686_bank0_read(icm_45686 icm, enum icm_45686_bank0_adr adr, void *data, int n);

/**
 * @brief Direct UI/Bank0 WRITE starting at @p adr.
 *
 * Transmits @p n bytes from @p data.
 * @p data may be overwritten by received bytes. If @p ro_data is true, an internal
 * copy is made to preserve the original contents of @p data.
 *
 * @param icm     Device handle.
 * @param adr     Bank0 address (from @ref icm_45686_bank0_adr).
 * @param data    Source buffer with bytes to transmit (may be overwritten).
 * @param n       Number of bytes to write (>0).
 * @param ro_data When true, the function transmits from an internal copy and
 *                leaves @p data unmodified.
 * @return 0 on success; negative error code propagated from \c spi_hal_xfer() on failure.
 */
int icm_45686_bank0_write(icm_45686 icm, enum icm_45686_bank0_adr adr, void *data, int n, boolean_t ro_data);

/**
 * @brief IREG READ - indirect read from 16-bit address @p adr.
 *
 * Programs the IREG address and then reads repeatedly, one byte per access.
 * Buffer @p data will be overwritten by received bytes.
 *
 * @param icm  Device handle.
 * @param adr  16-bit target address in the IREG space (lower 16 bits of @p adr are used).
 * @param data Destination buffer to be overwritten by received bytes.
 * @param n    Number of bytes to read (>0).
 * @return 0 on success; negative error code propagated from \c spi_hal_xfer() on failure.
 */
int icm_45686_ireg_read(icm_45686 icm, int adr, void *data, int n);

/**
 * @brief IREG WRITE - indirect write to 16-bit address @p adr.
 *
 * Programs the IREG address and then writes repeatedly, one byte per access.
 *
 * @param icm  Device handle.
 * @param adr  16-bit target address in the IREG space (lower 16 bits of @p adr are used).
 * @param data Source buffer with bytes to write.
 * @param n    Number of bytes to write (>0).
 * @return 0 on success; negative error code propagated from \c spi_hal_xfer() on failure.
 */
int icm_45686_ireg_write(icm_45686 icm, int adr, void *data, int n);

#endif
