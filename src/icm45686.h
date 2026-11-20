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
 * @brief Blocking SPI driver API for TDK ICM-45686 (UI interface).
 *
 * The caller owns struct icm_45686_dsc and its lifetime. Set icm->spi.cfg
 * before init_icm_45686(). The device is then reset and configured by the caller.
 *
 * All operations require an initialized descriptor and a valid device pointer.
 * They may block for SPI/DMA completion or sensor delays and are not ISR-safe.
 * The driver provides no transaction-wide locking: one task must own the device,
 * including register access, configuration, and FIFO consumption.
 *
 * Sensor registers, FIFO data, and FIFO count must use the reset-default
 * little-endian format (SREG_CTRL.SREG_DATA_ENDIAN_SEL = 0). Data values are
 * returned as signed raw samples; physical-unit scaling is caller-specific.
 *
 * @note Functions returning int use 0 for success. SPI HAL errors
 *       (including -EHW and -EDMA) are propagated; selected operations also
 *       return -EHW for mismatch, timeout or malformed FIFO frames.
 * @note Invalid configuration arguments invoke crit_err_exit(BAD_PARAMETER)
 *       instead of returning an error. Low-level buffer/count preconditions
 *       are the caller's responsibility unless explicitly stated otherwise.
 */

#ifndef ICM45686_H
#define ICM45686_H

#include <gentyp.h>
#include "icm45686_def.h"

/**
 * @brief Initialize the SPI HAL device attached to an ICM-45686 descriptor.
 *
 * @pre @p icm points to a persistent descriptor with valid @p icm->spi.cfg.
 * @param[in,out] icm Caller-owned descriptor; its SPI HAL state is initialized.
 * @note This does not reset the sensor or configure FIFO/frame size.
 * @note Call once during startup, then run device reset and configuration.
 */
void init_icm_45686(icm_45686 icm);

/**
 * @brief Request a soft reset and wait for RESET_DONE.
 *
 * Writes REG_MISC2.SOFT_RST, waits up to 50 ms for INT1_STATUS0.RESET_DONE,
 * checks the self-clearing reset bit, and applies a 1 ms settling delay.
 * @param[in,out] icm Initialized device descriptor.
 * @return 0 on success; -EHW on device timeout/state failure; SPI error on I/O failure.
 * @note Sensor configuration and FIFO content are reset. The descriptor's
 *       software fifo_frame_size is not changed; reconfigure FIFO before reading.
 */
int icm_45686_soft_reset(icm_45686 icm);

/**
 * @brief Verify the expected WHO_AM_I value (0xE9).
 *
 * @param[in,out] icm Initialized device descriptor.
 * @return 0 for a matching value; -EHW on mismatch; SPI error on I/O failure.
 */
int icm_45686_whoami_check(icm_45686 icm);

/**
 * @brief Set the SPI pad slew-rate selector in DRIVE_CONFIG0.
 *
 * Performs a read-modify-write and waits 2 us after successful programming.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] slew Valid ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_* selector.
 * @return 0 on success; SPI error on I/O failure.
 * @note An out-of-range selector is a fatal parameter error.
 */
int icm_45686_set_spi_pads_slew(icm_45686 icm, enum icm_45686_drive_config0_pads_spi_slew slew);

/**
 * @brief Set the accelerometer power mode in PWR_MGMT0.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] mode OFF/OFF_ALT, LP, or LN mode selector.
 * @return 0 on success; SPI error on I/O failure.
 * @note Invalid selectors are fatal. The caller manages sensor start-up time
 *       and compatibility with the configured ODR.
 */
int icm_45686_set_accel_mode(icm_45686 icm, enum icm_45686_pwr_mgmt0_accel_mode mode);

/**
 * @brief Set the gyroscope power mode in PWR_MGMT0.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] mode OFF, STANDBY, LP, or LN mode selector.
 * @return 0 on success; SPI error on I/O failure.
 * @note Invalid selectors are fatal. The caller manages sensor start-up time
 *       and compatibility with the configured ODR.
 */
int icm_45686_set_gyro_mode(icm_45686 icm, enum icm_45686_pwr_mgmt0_gyro_mode mode);

/**
 * @brief Set the accelerometer UI output data rate.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] freq Encoded ODR from 1.5625 to 6400 Hz.
 * @return 0 on success; SPI error on I/O failure.
 * @note Only 12.5-400 Hz is usable in LP or LN; 800-6400 Hz requires LN;
 *       1.5625-6.25 Hz requires LP. The current power mode is not checked.
 *       Invalid ODR codes are fatal.
 */
int icm_45686_set_accel_freq(icm_45686 icm, enum icm_45686_accel_config0_odr freq);

/**
 * @brief Set the gyroscope UI output data rate.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] freq Encoded ODR from 1.5625 to 6400 Hz.
 * @return 0 on success; SPI error on I/O failure.
 * @note Only 12.5-400 Hz is usable in LP or LN; 800-6400 Hz requires LN;
 *       1.5625-6.25 Hz requires LP. The current power mode is not checked.
 *       Invalid ODR codes are fatal.
 */
int icm_45686_set_gyro_freq(icm_45686 icm, enum icm_45686_gyro_config0_odr freq);

/**
 * @brief Select the accelerometer UI full-scale range.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] fsr Encoded range: +/-2, +/-4, +/-8, +/-16 or +/-32 g.
 * @return 0 on success; SPI error on I/O failure.
 * @note Invalid selectors are fatal. The function does not convert samples.
 */
int icm_45686_set_accel_fsr(icm_45686 icm, enum icm_45686_accel_config0_ui_fs_sel fsr);

/**
 * @brief Select the gyroscope UI full-scale range.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] fsr Encoded range: +/-15.625 through +/-4000 degrees/s.
 * @return 0 on success; SPI error on I/O failure.
 * @note Invalid selectors are fatal. The function does not convert samples.
 */
int icm_45686_set_gyro_fsr(icm_45686 icm, enum icm_45686_gyro_config0_ui_fs_sel fsr);

/**
 * @brief Configure accelerometer averaging in Low-Power mode.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] avg Encoded averaging factor 1 to 64 (supported enumerators only).
 * @return 0 on success; IREG/SPI error or -EHW on indirect-access timeout.
 * @note Out-of-range values are fatal. This does not change the power mode.
 */
int icm_45686_set_accel_lp_avg(icm_45686 icm, enum icm_45686_ipreg_sys2_reg_129_accel_lp_avg_sel avg);

/**
 * @brief Configure gyroscope averaging in Low-Power mode.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] avg Encoded averaging factor 1 to 64 (supported enumerators only).
 * @return 0 on success; IREG/SPI error or -EHW on indirect-access timeout.
 * @note Out-of-range values are fatal. This does not change the power mode.
 */
int icm_45686_set_gyro_lp_avg(icm_45686 icm, enum icm_45686_ipreg_sys1_reg_170_gyro_lp_avg_sel avg);

/**
 * @brief Select the accelerometer UI Low-Noise low-pass-filter bandwidth.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] bw NO_FILTER or ODR divisor 4, 8, 16, 32, 64 or 128.
 * @return 0 on success; IREG/SPI error or -EHW on indirect-access timeout.
 * @note Invalid selectors are fatal. This does not change the power mode.
 */
int icm_45686_set_accel_ln_bw(icm_45686 icm, enum icm_45686_ipreg_sys2_reg_131_accel_ui_lpfbw bw);

/**
 * @brief Select the gyroscope UI Low-Noise low-pass-filter bandwidth.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] bw NO_FILTER or ODR divisor 4, 8, 16, 32, 64 or 128.
 * @return 0 on success; IREG/SPI error or -EHW on indirect-access timeout.
 * @note Invalid selectors are fatal. This does not change the power mode.
 */
int icm_45686_set_gyro_ln_bw(icm_45686 icm, enum icm_45686_ipreg_sys1_reg_172_gyro_ui_lpfbw_sel bw);

/**
 * @brief Read 14 consecutive UI sensor-data register bytes.
 *
 * Reads accel XYZ, gyro XYZ and die temperature as seven signed 16-bit
 * raw values. Data is converted from sensor little-endian to host representation.
 * @param[in,out] icm Initialized device descriptor.
 * @param[out] data Caller-owned result buffer, valid after success.
 * @return 0 on success; SPI error on I/O failure.
 * @note Accel/gyro units are raw LSB with scaling set by configured FSR.
 *       Temperature in degrees C is 25 + temp_data / 128. A failed SPI
 *       transfer may leave @p data partially modified.
 */
int icm_45686_get_reg_data(icm_45686 icm, struct icm_45686_sensor_data *data);

/**
 * @brief Read the driver's supported subset of FIFO configuration.
 *
 * Reports FIFO mode, depth, watermark, accel/gyro enable and high resolution.
 * A reserved or reset-default FIFO_DEPTH code is normalized to DEPTH_APEX
 * in the output; this is not an exact readback of the hardware depth field.
 * @param[in,out] icm Initialized device descriptor.
 * @param[out] cfg Caller-owned configuration result.
 * @return 0 on success; SPI error on I/O failure.
 * @note Does not change the sensor, flush FIFO, or update fifo_frame_size.
 *       External-sensor insertion, compression and other FIFO settings are not
 *       represented in the returned structure.
 */
int icm_45686_get_fifo_config(icm_45686 icm, struct icm_45686_fifo_config *cfg);

/**
 * @brief Program FIFO mode, depth, watermark and internal sensor sources.
 *
 * Disables the FIFO interface, enters bypass, programs FIFO registers,
 * then enables the requested mode/interface. Also sets SMC_CONTROL_0.TMST_EN
 * and FIFO timestamp insertion for dual-sensor or high-resolution frames.
 * The descriptor's fifo_frame_size becomes 0 when neither sensor is selected,
 * otherwise 8, 16 or 20 bytes based on sources/HIRES, even in BYPASS mode.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] cfg FIFO settings; valid mode and depth, 0/1 enable fields;
 *                HIRES requires both accel and gyro. Non-bypass mode requires
 *                at least one sensor source. Watermark is in packets; 0 disables it.
 * @return 0 on success; SPI/IREG error or -EHW on indirect-access timeout.
 * @note Invalid settings are fatal. No rollback is performed on partial failure.
 *       The API supports fixed, uncompressed internal-sensor packets only;
 *       external sensor insertion and FIFO compression must remain disabled.
 *       Other FIFO register bits are preserved rather than forced off.
 *       This function does not explicitly issue FIFO_FLUSH; flush when required
 *       by the application's reconfiguration/retained-data policy.
 */
int icm_45686_set_fifo_config(icm_45686 icm, const struct icm_45686_fifo_config *cfg);

/**
 * @brief Issue FIFO_FLUSH and wait for its self-clear.
 *
 * Resets FIFO read/write pointers and control state, preserving configuration.
 * @param[in,out] icm Initialized device descriptor.
 * @return 0 on success; -EHW on the approximately 1 s timeout;
 *         SPI error on I/O failure.
 * @note Does not update the software fifo_frame_size.
 */
int icm_45686_flush_fifo(icm_45686 icm);

/**
 * @brief Read the number of complete packets currently stored in FIFO.
 *
 * Reads FIFO_COUNT twice to comply with errata AN-000364 (section 2.2),
 * then converts the 16-bit little-endian packet count.
 * @param[in,out] icm Initialized device descriptor.
 * @param[out] count Packet count, not byte count; assigned on success only.
 * @return 0 on success; SPI error on I/O failure.
 * @note Does not consume FIFO data.
 */
int icm_45686_get_frame_count(icm_45686 icm, int *count);

/**
 * @brief Pop and decode one fixed-format FIFO packet.
 *
 * The expected packet size is the descriptor's fifo_frame_size from a
 * successful icm_45686_set_fifo_config(): 8 (one sensor), 16 (both sensors),
 * or 20 (both sensors in HIRES). The output union member corresponds to that
 * size; the header identifies the source in an 8-byte packet.
 * @param[in,out] icm Initialized device descriptor with FIFO configured.
 * @param[out] frame Caller-owned decoded sample and raw header.
 * @return 0 on success; -EHW if the packet header conflicts with the format;
 *         SPI error on I/O failure.
 * @pre A complete packet is available in an enabled FIFO that uses uncompressed
 *      internal sensors without external-sensor insertion. Do not call in BYPASS
 *      mode or before FIFO setup.
 * @note Reads consume FIFO bytes even when subsequent header validation fails.
 *       On failure @p frame may contain only a header or stale field values.
 *       Invalid-sample markers must be handled by the caller; no SI conversion
 *       or timestamp interpretation is performed.
 */
int icm_45686_get_fifo_frame(icm_45686 icm, union icm_45686_fifo_frame *frame);

/**
 * @brief Replace the enabled interrupt-source bitmap for INT1 or INT2.
 *
 * Writes both source-enable registers for the chosen interrupt output.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] num ICM_45686_INT1 or ICM_45686_INT2.
 * @param[in] cfg Complete source bitmap; each field must be 0 or 1.
 * @return 0 on success; SPI error on I/O failure.
 * @note Invalid interrupt number or bitmap field is fatal. This call replaces
 *       all 15 source enable bits; it does not merge with the old configuration.
 */
int icm_45686_set_config_int_source(icm_45686 icm, enum icm_45686_int_num num, const struct icm_45686_int_bitmap *cfg);

/**
 * @brief Configure INT1/INT2 electrical drive, polarity and output mode.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] num ICM_45686_INT1 or ICM_45686_INT2.
 * @param[in] cfg Active polarity (LOW/HIGH), output mode (PULSE/LATCH), and
 *                electrical drive (push-pull/open-drain).
 * @return 0 on success; SPI error on I/O failure.
 * @pre Disable all interrupt sources on the selected interface before changing
 *      polarity or PULSE/LATCH mode, as required by the device datasheet.
 * @note Invalid interrupt number or pin selector is fatal.
 */
int icm_45686_set_config_int_pin(icm_45686 icm, enum icm_45686_int_num num, const struct icm_45686_int_pin_cfg *cfg);

/**
 * @brief Read and acknowledge the selected interrupt status bitmap.
 *
 * Reads the two read-to-clear INT1/INT2 status registers.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] num ICM_45686_INT1 or ICM_45686_INT2.
 * @param[out] stat Caller-owned status flags; assigned on success only.
 * @return 0 on success; SPI error on I/O failure.
 * @note The hardware clears reported status flags upon register read.
 *       An invalid interrupt number is fatal.
 */
int icm_45686_get_int_status(icm_45686 icm, enum icm_45686_int_num num, struct icm_45686_int_bitmap *stat);

/**
 * @brief Select the accelerometer-only Low-Power clock configuration.
 *
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] clk RCOSC (ALP) or WUOSC (AULP) selector.
 * @return 0 on success; SPI/IREG error or -EHW on indirect-access timeout.
 * @note Effective in accelerometer-only LP operation (subject to RTC mode).
 *       I3C synchronous timing control or RTC mode requires RCOSC/ALP.
 *       Does not itself enable LP mode; invalid clock selection is fatal.
 */
int icm_45686_select_accel_lp_clk(icm_45686 icm, enum icm_45686_smc_control_0_accel_lp_clk_sel clk);

/**
 * @brief Read @p n bytes from Bank 0 via UI SPI.
 *
 * Ordinary register reads access consecutive addresses. FIFO_DATA is a FIFO
 * stream port, not a block of sequential Bank 0 addresses. The function
 * prepends the read command bit (0x80); one-byte reads use polling and
 * larger reads use the configured SPI DMA transfer path.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] adr Starting 7-bit Bank 0 register address.
 * @param[out] data Writable destination of at least @p n bytes.
 * @param[in] n Positive number of bytes; caller ensures a valid register range.
 * @return 0 on success; SPI error on I/O failure.
 * @note The buffer is prefilled with 0xFF before transfer and may be
 *       overwritten even on SPI failure. No pointer/count validation is done.
 */
int icm_45686_bank0_read(icm_45686 icm, enum icm_45686_bank0_adr adr, void *data, int n);

/**
 * @brief Write @p n sequential Bank 0 register bytes via UI SPI.
 *
 * Prepends the write command; one-byte writes use polling and larger writes
 * use the configured SPI DMA transfer path.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] adr Starting 7-bit Bank 0 register address.
 * @param[in,out] data At least @p n source bytes; SPI may overwrite this buffer
 *                     unless @p ro_data is true.
 * @param[in] n Positive number of bytes within a valid register range.
 * @param[in] ro_data True to transmit a stack copy and preserve @p data;
 *                    false to transmit from @p data directly.
 * @return 0 on success; SPI error on I/O failure.
 * @note True @p ro_data uses an n-byte variable-length stack buffer. The
 *       caller must bound @p n for available task stack. No argument validation.
 */
int icm_45686_bank0_write(icm_45686 icm, enum icm_45686_bank0_adr adr, void *data, int n, boolean_t ro_data);

/**
 * @brief Read @p n bytes from consecutive 16-bit indirect addresses.
 *
 * Programs the address once, waits for IREG_DONE, and reads IREG_DATA once
 * per byte with completion checks and the required minimum access gap.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] adr Indirect address; lower 16 bits select the starting register.
 * @param[out] data Writable buffer of at least @p n bytes.
 * @param[in] n Positive byte count; caller ensures valid address range/buffer.
 * @return 0 on success; -EHW on indirect-access timeout; SPI error on I/O failure.
 * @note Multi-transaction operation; a failure can leave partially read output.
 *       Calls must not interleave with other IREG operations on this device.
 */
int icm_45686_ireg_read(icm_45686 icm, int adr, void *data, int n);

/**
 * @brief Write @p n bytes to consecutive 16-bit indirect addresses.
 *
 * Programs the address and first data byte in one SPI burst to suppress an
 * unintended read prefetch; remaining bytes use auto-incremented IREG_DATA.
 * @param[in,out] icm Initialized device descriptor.
 * @param[in] adr Indirect address; lower 16 bits select the starting register.
 * @param[in] data Source buffer of at least @p n bytes; not modified.
 * @param[in] n Positive byte count; caller ensures valid address range/buffer.
 * @return 0 on success; -EHW on indirect-access timeout; SPI error on I/O failure.
 * @note Reads @p data[0] unconditionally; n must be positive. Multi-transaction
 *       operation without rollback: a failure may leave partial writes.
 *       Calls must not interleave with other IREG operations on this device.
 */
int icm_45686_ireg_write(icm_45686 icm, int adr, const void *data, int n);

#endif
