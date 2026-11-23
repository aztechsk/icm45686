/*
 * icm45686_stm_regs.h
 *
 * Autors: Jan Rusnak.
 * (c) 2025 AZTech.
 */

/**
 * @file icm45686_stm_regs.h
 * @brief FreeRTOS ICM-45686 data-register acquisition state machine using INT1.
 *
 * Owns one ICM-45686 on SPI bus 0. Creates a binary semaphore and acquisition
 * task, and registers the "icms" statistics command. The implementation is
 * built only when ICM45686_SENSOR_MODE == ICM45686_REGS_MODE. It reads the
 * latest UI sensor data directly, without FIFO buffering or sensor timestamps.
 * Optional text and raw-sample callbacks run synchronously in task context.
 *
 * @par Sensor configuration
 * Following soft reset, SPI pad slew setup and WHO_AM_I verification, the task
 * configures the device as follows:
 * - SPI mode 0, 8-bit transfers, 12 MHz SCK, CSEL2, 20 ns CS setup delay.
 * - Accelerometer: Low-Noise mode, 50 Hz ODR, +/-4 g FSR, UI LPF at ODR/4.
 * - Gyroscope: Low-Noise mode, 50 Hz ODR, +/-2000 dps FSR, UI LPF at ODR/4.
 * - The accelerometer low-power clock source is set to RCOSC; both sensors
 *   remain in Low-Noise mode and run continuously.
 * - Only UI DATA_READY is routed to INT1; other interrupt sources are disabled.
 *
 * @par Gyroscope startup and INT1 acquisition
 * After enabling the sensors, the task waits for the configured 70 ms
 * startup interval (ICM_45686_GYR_STARTUP_TIME_US), rounded to ticks with
 * an extra scheduler tick. GPIO INT1 and the sensor UI DATA_READY interrupt
 * source remain disabled during startup. The task then reads INT1 status,
 * discards any pending semaphore signal, enables UI DATA_READY on INT1 and
 * finally enables the GPIO interrupt. This handoff does not call
 * gpio_hal_intr_clear(), which would clear shared PIO interrupt status.
 * No additional per-sample validity test is performed.
 * INT1 is active LOW, push-pull. The GPIO input has no pull or digital filter
 * and has the Schmitt trigger enabled. ICM45686_INTR_TYPE selects:
 * - FALL_EDGE_INTR_TYPE: sensor PULSE output and GPIO falling-edge trigger.
 *   The ISR accepts a captured edge even if the pin has returned HIGH.
 * - LOW_LEVEL_INTR_TYPE: sensor LATCH output and GPIO low-level trigger.
 *   The ISR also verifies that the input pin is currently LOW.
 * The ISR disables the GPIO interrupt and signals the task via a binary
 * semaphore. The task reads INT1 status and, when UI DATA_READY is set, reads
 * 14 consecutive UI data-register bytes in one SPI burst. It then reports
 * the sample and re-enables GPIO interrupts.
 * A 50 ms semaphore timeout increments drdy_timeout_cnt and checks WHO_AM_I.
 * If no successful data-register read occurs for 1000 ms
 * (NO_DRDY_TMO_MS), no_data_reset_cnt increments and the STM enters the
 * normal recovery path. A failed sensor/SPI operation also enters recovery:
 * optionally wait before soft reset and reconfiguration. The GPIO interrupt
 * remains masked during sample reporting and is disabled immediately on
 * entering recovery, before any configured backoff.
 *
 * @par Sampling limitations and raw data
 * Unlike the FIFO STM, direct register acquisition has no sample queue:
 * newer conversions overwrite earlier values. Delayed or missed DATA_READY
 * events can therefore lose samples without a detectable gap. The binary
 * semaphore and read_data_reg_cnt cannot establish lossless acquisition.
 * The 14-byte burst contains signed 16-bit, little-endian accel XYZ, gyro XYZ
 * and temperature, converted to host byte order by the low-level driver.
 * With the fixed FSR configuration, the physical unit conversions are:
 * - Accel in g = accel_raw * 4 / 32768.
 * - Gyro in dps = gyro_raw * 2000 / 32768.
 * - Die temperature in degC = 25 + temp_raw / 128.
 * struct icm_45686_sensor_data exposes these raw device LSB values, without
 * FIFO timestamps or per-sensor validity flags. The 70 ms startup delay does
 * not prove that every subsequent sample is otherwise valid.
 *
 * @par Optional output callbacks
 * ICM45686_REPORT_SAMPLE_FN enables a raw callback, invoked once per
 * successful register read, before any text reports. The callback receives
 * the original signed 16-bit XYZ and temperature values; they are not scaled.
 * ICM45686_REPORT_FN enables a formatted-text callback and a reusable
 * 192-byte formatting buffer allocated once during initialization.
 * Each of the four text selectors independently chooses destinations:
 * 0=off, 1=local msg(), 2=report_fn, 3=both. Selectors 0 and 1 preserve
 * the original local-logging behavior. When the selector is 3, exactly the
 * same formatted line (including its newline) goes to both destinations.
 * If no report callback is registered, callback-routed lines are skipped;
 * there is no automatic fallback to msg(). SI and raw LSB outputs include
 * the die temperature on each enabled accel/gyro line.
 * ICM45686_REPORT_MSG_NUM=1 adds [000]..[999] to every enabled text line;
 * all lines for a register sample share the same number, which advances once
 * per processed sample and wraps to 000. It persists across STM
 * recovery, but resets with the firmware. It numbers processed samples,
 * not hardware conversions or successfully delivered output. With all text
 * selectors disabled, no sequence number is maintained.
 * Both callbacks are synchronous in the acquisition task, never the ISR;
 * no internal delivery queue or lossless callback guarantee exists. Their
 * pointers are valid only during the call. Returning FALSE increments the
 * relevant diagnostic counter without resetting the STM. Keep callback
 * work bounded and nonblocking: it delays service of the next DATA_READY.
 *
 * @par Build-time settings in sysconf.h and board configuration
 * - ICM45686_SENSOR_MODE: select ICM45686_REGS_MODE for this STM.
 * - ICM45686_TASK_PRIO / ICM45686_TASK_STACK_SIZE: FreeRTOS task priority
 *   and stack depth (StackType_t units).
 * - ICM45686_INTR_TYPE: LOW_LEVEL_INTR_TYPE or FALL_EDGE_INTR_TYPE.
 * - ICM45686_RESET_STM_TMO_MS: optional recovery backoff in milliseconds,
 *   set by the client in sysconf.h. If omitted, no backoff is applied.
 *   A positive value must convert to at least one FreeRTOS tick.
 * - ICM45686_PADS_SPI_SLEW: SPI pad slew programmed after reset.
 * - ICM45686_REPORT_ACCEL_SI / ICM45686_REPORT_GYRO_SI: XYZ in g or dps,
 *   with two decimals, plus die temperature in degC.
 * - ICM45686_REPORT_ACCEL_LSB / ICM45686_REPORT_GYRO_LSB: signed raw XYZ
 *   decimal LSB values and signed raw temperature.
 * - The four report switches accept 0=off, 1=msg(), 2=callback, 3=both;
 *   callback routes require ICM45686_REPORT_FN=1 at compile time.
 * - ICM45686_REPORT_FN: compile the text callback when 1 (default 0).
 * - ICM45686_REPORT_SAMPLE_FN: compile the raw callback when 1 (default 0),
 *   independently of text reporting.
 * - ICM45686_REPORT_MSG_NUM: enable the [000]..[999] text sequence when 1
 *   (default 0), independently of which text destination is selected.
 * SPI bus, chip select, GPIO controller and INT1 pin assignments come from
 * the implementation and board configuration; routing does not change ODR,
 * FSR, LPF, interrupt sources or sensor acquisition.
 *
 * @par Changing the acquisition configuration
 * SPI settings are fixed in init_icm45686_stm_regs(); ODR, FSR, LPF, power
 * modes and INT1 sources are fixed in accel_gyro_cfg() and intr_cfg() in
 * icm45686_stm_regs.c. Changing them requires editing the implementation
 * and rebuilding. If FSR changes, update the conversion constants in
 * report_data(). If ODR or callback workload changes, reconsider the 50 ms
 * interrupt wait timeout and whether direct registers can satisfy the
 * application's data-loss requirements; use the FIFO STM when buffering
 * and sensor-side timestamps are needed.
 *
 * @par Diagnostics
 * The "icms" command reports read_data_reg (successful 14-byte reads),
 * unknown_intr (status lacked UI DATA_READY after an ISR wakeup),
 * drdy_timeout (50 ms semaphore wait timeouts), no_data_reset (watchdog-triggered
 * restarts after 1000 ms without a successful sample read) and stm_rst
 * (recovery cycles). drdy_timeout counts timeout occurrences, not the number
 * of lost samples. The additional "ICM45686 STM err" counters record
 * fatal failures: sw_reset (sensor reset), probe (WHO_AM_I checks),
 * cfg (SPI slew, INT1 or accel/gyro configuration), int_stat (INT1 status
 * read during acquisition), and reg_data (14-byte UI data read). Each
 * fatal transition increments one reason counter, including no_data_reset.
 * stm_rst increments after the configured recovery delay, so it may
 * temporarily lag behind the sum of fatal reason counters.
 * If built, report_fn_err and sample_fn_err count callbacks returning FALSE;
 * an unregistered callback is not counted as an error. Counters persist
 * through STM recovery and reset only when the firmware restarts.
 *
 * @note Include sysconf.h before this header to select callback features;
 *       ICM45686_REPORT_FN, ICM45686_REPORT_SAMPLE_FN and
 *       ICM45686_REPORT_MSG_NUM default to 0 when unspecified.
 * @see ICM-45686 datasheet DS-000577, UI sensor registers and INT1 sections.
 */

#ifndef ICM45686_STM_REGS_H
#define ICM45686_STM_REGS_H

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
#include "icm45686_def.h"
#endif

/**
 * @brief Initialize the ICM-45686 register-data acquisition task at startup.
 *
 * Sets up the SPI device descriptor, allocates a binary semaphore and, when
 * text callbacks are compiled in, a persistent formatting buffer. Registers
 * the "icms" command and creates the FreeRTOS acquisition task. Sensor reset,
 * configuration, startup delay and continuous acquisition run in that task.
 *
 * @pre SPI/GPIO/PIO backends, board pin mappings, command and output services
 *      are ready; task, semaphore and optional report-buffer allocation must
 *      be possible.
 * @note Call once during application startup, from task/init context; not
 *       ISR-safe. There is no deinitialization or runtime task recreation.
 * @note Only implemented when ICM45686_SENSOR_MODE == ICM45686_REGS_MODE.
 * @note Allocation failure calls crit_err_exit(). Sensor/SPI runtime failures
 *       are handled by the task's recovery state.
 */
void init_icm45686_stm_regs(void);

#if ICM45686_REPORT_FN == 1
/**
 * @brief Install or clear the formatted measurement-text callback.
 *
 * Invoked for each accel/gyro SI/LSB line whose selector is 2 or 3, after
 * a successful register burst read. At most four callback lines are sent
 * per sample. With a selector of 3, msg() receives the identical line,
 * including the trailing newline and optional [000]..[999] number. Each SI
 * or LSB line includes temperature. Returning FALSE increments
 * report_fn_err_cnt once per failed callback line; no STM reset occurs.
 * NULL disables remote delivery without affecting locally routed text.
 *
 * @param[in] report_fn Callback function or NULL to disable remote output.
 * @note Runs synchronously in the FreeRTOS acquisition task, never an ISR;
 *       keep it nonblocking. The STM reuses its text buffer, so copy the
 *       string before returning if it must outlive the call.
 * @note Register during initialization. Concurrent callback replacement is
 *       not synchronized. Only available when ICM45686_REPORT_FN == 1.
 */
void icm45686_stm_set_report_fn(boolean_t (*report_fn)(const char *));
#endif

#if ICM45686_REPORT_SAMPLE_FN == 1
/**
 * @brief Install or clear the raw UI data-register sample callback.
 *
 * Called once after each successful 14-byte register read, before text
 * reporting. Receives struct icm_45686_sensor_data: signed 16-bit raw accel
 * XYZ, gyro XYZ and temperature in device LSB. No FIFO timestamp or
 * per-sensor validity flags are available. A startup delay of at least
 * 70 ms is applied before acquisition begins. Returning FALSE increments
 * sample_fn_err_cnt without resetting the STM. NULL disables the callback.
 *
 * @param[in] sample_fn Callback function or NULL to disable raw delivery.
 * @note Runs synchronously in the FreeRTOS acquisition task, not ISR;
 *       keep it bounded and nonblocking (for example xQueueSend(..., 0)).
 * @note The data pointer is valid only during the call; copy if retaining
 *       data. Register during initialization: concurrent replacement is not
 *       synchronized. Only available with ICM45686_REPORT_SAMPLE_FN == 1.
 */
void icm45686_stm_set_sample_fn(boolean_t (*sample_fn)(const struct icm_45686_sensor_data *));
#endif

#endif
