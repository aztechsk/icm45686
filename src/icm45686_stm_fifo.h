/*
 * icm45686_stm_fifo.h
 *
 * Autors: Jan Rusnak.
 * (c) 2025 AZTech.
 */

/**
 * @file icm45686_stm_fifo.h
 * @brief FreeRTOS ICM-45686 FIFO acquisition state machine using INT1.
 *
 * Owns one ICM-45686 connected to SPI bus 0, creates a binary semaphore and
 * an acquisition task, and registers the "icms" statistics command. Built only
 * when ICM45686_SENSOR_MODE == ICM45686_FIFO_MODE. Optional report/sample
 * callbacks are provided; there is no queue or runtime sensor reconfiguration.
 *
 * @par Sensor configuration
 * Following soft reset, SPI pad slew configuration and WHO_AM_I verification,
 * the acquisition task configures the sensor as follows:
 * - SPI mode 0, 8-bit transfers, 12 MHz SCK, CSEL2, 20 ns CS setup delay;
 *   CS stays asserted between transfer segments.
 * - Accelerometer: Low-Noise mode, 50 Hz ODR, +/-4 g FSR, UI LPF at ODR/4.
 * - Gyroscope: Low-Noise mode, 50 Hz ODR, +/-2000 dps FSR, UI LPF at ODR/4.
 * - Both sensors run continuously; the task does not request conversions.
 *   Initial samples are not explicitly discarded for sensor stabilization.
 *
 * @par FIFO configuration
 * - Both accel and gyro are enabled, with temperature and timestamp data.
 * - FIFO depth is 2 KiB (DEPTH_APEX), holding 128 standard 16-byte or
 *   102 high-resolution 20-byte packets; mode is SNAPSHOT (stop on full),
 *   not STREAM. On FIFO_FULL, the task drains queued packets and issues
 *   FIFO_FLUSH before resuming acquisition.
 * - Watermark is one packet, with the interrupt condition count >= threshold.
 * - FIFO_THS and FIFO_FULL are routed to INT1; other sources are disabled.
 * - FIFO compression and external-sensor insertion are not supported by the
 *   fixed-frame decoder and must remain disabled (reset defaults).
 * - The LL FIFO setup enables the sensor timestamp and its insertion into
 *   dual-sensor packets, independently of the timestamp reporting switch.
 *
 * @par INT1 acquisition and recovery
 * ICM45686_INT1_CONT/ICM45686_INT1_PIN come from board.h. INT1 is active LOW,
 * push-pull. The GPIO is an input without pull or digital filter, with Schmitt
 * trigger enabled. ICM45686_INTR_TYPE selects one of the supported modes:
 * - FALL_EDGE_INTR_TYPE: sensor PULSE output, GPIO falling-edge trigger,
 *   with the reset-default 8 us pulse width. A latched edge is accepted
 *   even if the pin has already returned HIGH.
 * - LOW_LEVEL_INTR_TYPE: sensor LATCH output, GPIO low-level trigger; the
 *   callback also verifies that the pin is currently LOW.
 * At entry to recovery, GPIO INT1 is masked and the acquisition ISR is
 * disarmed. On reset, the binary semaphore is drained. FIFO interrupt sources
 * remain disabled until FIFO/ODR/FSR/filter setup is complete. The ISR is
 * armed before GPIO INT1 is enabled: a pending PIO event is accepted,
 * checked against FIFO status, and does not block recovery.
 * After the GPIO write is synchronized with DSB, FIFO_THS/FIFO_FULL are
 * routed to INT1, and both sensors enter Low-Noise mode. This STM does not
 * clear PIO_ISR or NVIC pending state explicitly; the first FIFO packet
 * is not discarded.
 * The armed ISR disables its GPIO interrupt and signals a binary semaphore.
 * The task reads INT1 status, then FIFO_COUNT and all available packets when
 * FIFO_THS or FIFO_FULL is set. On FIFO_FULL, it flushes FIFO after draining,
 * then re-enables GPIO interrupts. Each packet is decoded and optionally
 * reported synchronously in task context.
 * A missed interrupt triggers a 50 ms timeout fallback: the task masks
 * GPIO INT1, checks WHO_AM_I, acknowledges INT1_STATUS and reads FIFO_COUNT.
 * If data are present or FIFO_FULL is signaled, it drains any queued packets,
 * flushes on FIFO_FULL, and re-enables GPIO INT1. If no packets are consumed
 * for 1000 ms after the sensors start or the last good packet, the task enters
 * recovery. SPI,
 * status, FIFO count, frame decode and invalid FIFO count failures also
 * enter recovery, with a 5000 ms delay before reinitialization. This STM
 * does not guarantee lossless transport if FIFO fills or callbacks block.
 *
 * @par FIFO packet formats and physical units
 * ICM45686_FIFO_HIRES selects the fixed frame layout (little-endian data):
 * - 0: 16-byte packet: header (1 B), accel XYZ and gyro XYZ (signed 16-bit),
 *   temperature (signed 8-bit), timestamp (unsigned 16-bit). Raw acceleration
 *   and angular-rate scale to g = raw * 4 / 32768 and
 *   dps = raw * 2000 / 32768; degC = 25 + temp_raw / 2.
 * - 1: 20-byte packet: header (1 B), accel XYZ and gyro XYZ (signed 20-bit),
 *   temperature (signed 16-bit), timestamp (unsigned 16-bit), plus packed
 *   low bits of each axis. Accel has 19 effective bits (LSB always zero).
 *   High-resolution scaling is independent of the UI FSR registers:
 *   g = raw * 32 / 524288,
 *   dps = raw * 4000 / 524288; degC = 25 + temp_raw / 128.
 * In the reset-default timestamp configuration, one tick is 1 us. The
 * 16-bit value wraps modulo 65536 (65.536 ms); it is not FreeRTOS uptime.
 * Invalid accel/gyro/temperature markers suppress the corresponding text
 * fields or mark them "invalid". The raw callback receives validity flags.
 * Successfully decoded packets still increment read_frame_cnt.
 *
 * @par Optional output callbacks
 * ICM45686_REPORT_FN and ICM45686_REPORT_SAMPLE_FN select independent
 * application callbacks. They run synchronously in the acquisition task,
 * never in the GPIO ISR or after a failed SPI/FIFO read. The raw sample
 * callback runs once per decoded packet, including invalid sensor fields.
 * It receives signed raw accel/gyro, temperature, a 16-bit timestamp,
 * a 32-bit packet sequence, validity flags and the 16/20-bit format.
 * Each measurement type has an independent output routing selector:
 * 0=off, 1=local msg(), 2=report_fn, 3=both. Existing 0/1 settings retain
 * their original local-logging meaning. When sent to both destinations,
 * exactly the same formatted line, including the newline, is delivered.
 * If ICM45686_REPORT_MSG_NUM is 1, every line carries the same per-packet
 * sequence [000]..[999], incremented after the packet has been reported.
 * Up to five text callbacks may occur per packet; a callback is never
 * invoked for a measurement not routed to it.
 * Returning FALSE increments the corresponding diagnostic counter without
 * resetting the sensor. Buffers are valid only during each callback.
 * Keep callbacks bounded and nonblocking: GPIO INT1 is masked during FIFO
 * draining. No lossless delivery guarantee or internal sample queue exists.
 *
 * @par Build-time settings in sysconf.h and board configuration
 * - ICM45686_SENSOR_MODE: select ICM45686_FIFO_MODE to build this STM.
 * - ICM45686_TASK_PRIO / ICM45686_TASK_STACK_SIZE: FreeRTOS task priority
 *   and stack depth (StackType_t units).
 * - ICM45686_INTR_TYPE: LOW_LEVEL_INTR_TYPE or FALL_EDGE_INTR_TYPE.
 * - ICM45686_FIFO_HIRES: 0 for 16-byte, 1 for 20-byte packets.
 * - ICM45686_REPORT_ACCEL_SI / ICM45686_REPORT_GYRO_SI: report XYZ in g
 *   or dps, respectively, with two decimal places.
 * - ICM45686_REPORT_ACCEL_LSB / ICM45686_REPORT_GYRO_LSB: report signed
 *   raw XYZ decimal values in sensor LSB units.
 * - ICM45686_REPORT_FIFO_TMSTMP: report the raw 16-bit timestamp in us
 *   and valid temperature in degC. No separate temperature switch exists.
 * - ICM45686_REPORT_MSG_NUM: 1 adds the rolling [000]..[999] sequence
 *   number to each enabled measurement line (default 0). All lines from
 *   one decoded FIFO packet share the same number, including local/remote
 *   routes; the number advances once per decoded packet, then wraps to 0.
 *   The number tracks packets processed by this STM, not log delivery.
 * - Each of the five switches above selects its text destinations:
 *   0=off, 1=msg() only, 2=report_fn only, 3=both. Values 0 and 1 retain
 *   the original behavior. A callback route requires ICM45686_REPORT_FN=1.
 * - ICM45686_REPORT_FN: compile the text callback and its single 192-byte
 *   startup-allocated formatting buffer when 1 (default 0). Registering
 *   report_fn does not enable a destination not selected above.
 * - ICM45686_REPORT_SAMPLE_FN: enable the raw sample callback when 1
 *   (default 0); independent of text reporting and the text callback.
 * - ICM45686_PADS_SPI_SLEW: select the SPI pad slew setting programmed after
 *   reset. CS and INT1 controller/pin mappings are defined by the board.
 * Routing selectors do not alter FIFO or sensor setup. If all five are 0,
 * no measurement text is produced; decoded raw samples are independent.
 *
 * @par Changing the acquisition configuration
 * Of the sensor and report settings, only interrupt type, packet resolution,
 * SPI pad slew, logging and callback support have build-time switches.
 * ODR, FSR, LPF and Low-Noise power modes, FIFO depth/mode/watermark
 * and INT1 sources are fixed in accel_gyro_cfg() and intr_cfg() in
 * icm45686_stm_fifo.c; SPI bus settings
 * are fixed in init_icm45686_stm_fifo(). Changing these requires editing
 * the implementation and rebuilding. If the standard-mode FSR is changed,
 * update the conversion constants in report_data() as well. If ODR, packet
 * size or reporting workload changes, reconsider the 50 ms ISR wait timeout,
 * FIFO capacity and task processing time. Alternate FIFO sources, packet
 * formats or compression require matching decoder/report changes.
 *
 * @par Diagnostics
 * The "icms" command reports read_frame_cnt, fifo_full, unknown_intr,
 * fifo_empty_err, stm_rst, irq_pin_high, stale_irq, timeout_fifo_nonempty,
 * no_data_reset and max_fifo_backlog. stale_irq counts GPIO callback events
 * while acquisition is disarmed; they never set the semaphore. irq_pin_high
 * counts accepted falling-edge interrupts sampled after the pin returned
 * HIGH; it does not count dropped packets.
 * timeout_fifo_nonempty counts timeout fallbacks that found a nonempty
 * FIFO, not distinct lost interrupts. no_data_reset counts 1000 ms periods
 * without a successfully read packet that triggered recovery. A stale
 * pending interrupt may increment unknown_intr without stopping acquisition.
 * max_fifo_backlog is the largest validated FIFO packet count passed for draining,
 * including attempts that later fail; it is not an absolute hardware peak.
 * When enabled, report_fn_err and sample_fn_err count callbacks returning
 * FALSE. Counters are not cleared by an STM reset; they reset with firmware.
 *
 * @note Include sysconf.h before this header to select callback switches;
 *       both default to 0 if not defined.
 * @see ICM-45686 datasheet DS-000577, sections 6, 7, 15 and 17.
 */

#ifndef ICM45686_STM_FIFO_H
#define ICM45686_STM_FIFO_H

#ifndef ICM45686_REPORT_FN
#define ICM45686_REPORT_FN 0
#endif
#ifndef ICM45686_REPORT_SAMPLE_FN
#define ICM45686_REPORT_SAMPLE_FN 0
#endif
#ifndef ICM45686_REPORT_MSG_NUM
#define ICM45686_REPORT_MSG_NUM 0
#endif
#if ICM45686_REPORT_MSG_NUM != 0 && ICM45686_REPORT_MSG_NUM != 1
#error "ICM45686_REPORT_MSG_NUM must be 0 or 1"
#endif
#if ICM45686_REPORT_FN != 0 && ICM45686_REPORT_FN != 1
#error "ICM45686_REPORT_FN must be 0 or 1"
#endif
#if ICM45686_REPORT_SAMPLE_FN != 0 && ICM45686_REPORT_SAMPLE_FN != 1
#error "ICM45686_REPORT_SAMPLE_FN must be 0 or 1"
#endif

#if ICM45686_REPORT_FN == 1 || ICM45686_REPORT_SAMPLE_FN == 1
#include <gentyp.h>
#endif
#if ICM45686_REPORT_SAMPLE_FN == 1
#include <stdint.h>

/**
 * @brief One FIFO packet delivered to the raw sample callback.
 *
 * accel_raw and gyro_raw hold signed XYZ sensor LSB values, sign-extended
 * to int32_t for the 20-bit format. Values are not scaled to physical units.
 * A validity flag is FALSE if any axis of that sensor group has an invalid
 * marker; the raw values remain unchanged, even when invalid. temp_raw holds
 * a signed 8-bit FIFO temperature in 16-bit mode or a signed 16-bit FIFO
 * temperature in 20-bit mode, extended to int16_t. Valid temperature is
 * 25 + temp_raw / 2 degC (16-bit mode) or 25 + temp_raw / 128 degC
 * (20-bit mode).
 * The raw 16-bit timestamp uses 1 us/tick with reset-default sensor settings
 * and wraps modulo 65536. The sequence is the 1-based read_frame_cnt value;
 * it counts decoded packets, including packets with invalid sensor values,
 * continues across sensor resets and wraps modulo 2^32. It does not detect
 * packets lost inside the sensor FIFO. The STM uses both accel and gyro in
 * every packet.
 *
 * @note data_bits is 16 or 20, selected by ICM45686_FIFO_HIRES at build time.
 * @note The pointer passed to the callback is valid only during that call.
 */
struct icm45686_fifo_sample {
	int32_t accel_raw[3]; /**< Signed raw accel X/Y/Z in sensor LSB. */
	int32_t gyro_raw[3]; /**< Signed raw gyro X/Y/Z in sensor LSB. */
	int16_t temp_raw; /**< Signed FIFO temperature output in sensor LSB. */
	uint16_t timestamp; /**< Raw FIFO timestamp counter (1 us/tick). */
	boolean_t accel_valid; /**< TRUE if all three accel axes are valid. */
	boolean_t gyro_valid; /**< TRUE if all three gyro axes are valid. */
	boolean_t temp_valid; /**< TRUE if temperature is valid. */
	uint8_t data_bits; /**< 16 (standard FIFO) or 20 (HIRES FIFO). */
	uint32_t sequence; /**< 1-based decoded packet sequence, wraps modulo 2^32. */
};
#endif

/**
 * @brief Initialize the ICM-45686 FIFO acquisition task once at startup.
 *
 * Configures its SPI device, allocates a binary semaphore, registers the
 * "icms" command and creates the FreeRTOS acquisition task. Sensor reset,
 * configuration and ongoing acquisition happen later in that task.
 *
 * @pre SPI/GPIO/PIO backends, board pin mappings and command/output services
 *      are available; task, semaphore and optional report-buffer allocation
 *      must be possible.
 * @note Intended for one-time startup initialization; not ISR-safe.
 * @note Only implemented when ICM45686_SENSOR_MODE == ICM45686_FIFO_MODE.
 * @note Semaphore/task allocation failure invokes crit_err_exit(). Runtime
 *       sensor and SPI failures are handled by the task's recovery path.
 */
void init_icm45686_stm_fifo(void);

#if ICM45686_REPORT_FN == 1
/**
 * @brief Install or clear the formatted text-report callback.
 *
 * Called for each measurement line whose routing selector includes bit 1
 * (value 2 or 3). Value 2 delivers only to the callback; value 3 delivers
 * identical text to msg() and the callback, including the trailing newline.
 * When ICM45686_REPORT_MSG_NUM is 1, each line carries the shared per-packet
 * sequence [000]..[999], with the same number at both destinations.
 * Each of the five routing selectors is independent; zero to five lines
 * may be delivered per decoded packet. Invalid sensor values are reported
 * in the same text format for local and remote output. Returning FALSE
 * increments report_fn_err_cnt once per failed callback line.
 *
 * @param[in] report_fn Callback function or NULL to disable reporting.
 * @note Invoked synchronously from the acquisition FreeRTOS task, not ISR.
 *       The STM owns and reuses the report buffer; copy text before return.
 * @note Register during initialization. Replacement while the task reports
 *       data is not synchronized. Callback must not block on other tasks.
 * @note Only available when ICM45686_REPORT_FN == 1.
 */
void icm45686_stm_set_report_fn(boolean_t (*report_fn)(const char *));
#endif

#if ICM45686_REPORT_SAMPLE_FN == 1
/**
 * @brief Install or clear the raw FIFO sample callback.
 *
 * Called for every decoded FIFO packet with raw signed acceleration, gyro
 * and temperature values, validity flags, 16-bit timestamp, packet sequence
 * and format bits.
 * A false validity flag does not suppress delivery. FALSE from the callback
 * increments sample_fn_err_cnt but does not reset acquisition.
 *
 * @param[in] sample_fn Callback function or NULL to disable reporting.
 * @note Called synchronously in the acquisition task, not an ISR. Keep work
 *       bounded and nonblocking (for example xQueueSend(..., 0)).
 * @note Sample storage is valid only during the call; copy if retaining it.
 *       There is no internal queue or lossless delivery guarantee.
 * @note Register during initialization; concurrent replacement is not
 *       synchronized. Only available when ICM45686_REPORT_SAMPLE_FN == 1.
 */
void icm45686_stm_set_sample_fn(boolean_t (*sample_fn)(const struct icm45686_fifo_sample *));
#endif

#endif
