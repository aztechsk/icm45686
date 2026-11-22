# ICM-45686 Driver

Embedded C driver for the TDK InvenSense ICM-45686 six-axis inertial sensor (3-axis accelerometer,
3-axis gyroscope and internal temperature sensor).

The library contains a low-level (LL) driver for direct configuration and SPI access, plus two optional
FreeRTOS acquisition state machines (STMs). The LL driver exposes configurable sensor functions;
the STMs provide ready-to-use acquisition with a deliberately fixed measurement configuration.

## Features

- SPI communication over the sensor's UI interface
- Soft reset and WHO_AM_I identification (`0xE9`)
- Accelerometer and gyroscope power modes, output data rates (ODR) and full-scale ranges (FSR)
- Low-Power averaging and Low-Noise UI low-pass-filter settings
- Signed accelerometer, gyroscope and temperature register reads
- Uncompressed FIFO configuration, packet reading and flushing; 16-bit and 20-bit FIFO data
- INT1/INT2 source selection, pin configuration and status reading
- Direct Bank0 and indirect IREG access
- Optional FreeRTOS data-ready (REGS) and FIFO-watermark (FIFO) acquisition STMs
- Optional formatted reporting and binary sample callbacks in either STM

## Files

| File | Purpose |
| --- | --- |
| `src/icm45686.c` | LL SPI, sensor configuration, registers, FIFO and interrupt handling |
| `src/icm45686.h` | Public LL API and its calling contracts |
| `src/icm45686_def.h` | Register addresses, selector values, descriptors and sample structures |
| `src/icm45686_stm_regs.c` | FreeRTOS acquisition from UI data registers on DATA_READY |
| `src/icm45686_stm_regs.h` | REGS STM startup, configuration and callback documentation |
| `src/icm45686_stm_fifo.c` | FreeRTOS FIFO acquisition on watermark/full events |
| `src/icm45686_stm_fifo.h` | FIFO STM startup, configuration, sample and callback documentation |

## Sensor capabilities and LL configuration

The ICM-45686 has independently configurable accelerometer and gyroscope signal paths. Its UI output
registers supply signed 16-bit measurements; the FIFO also supports high-resolution accel/gyro data.
The sensor has programmable interrupts and a normally 2 KiB FIFO. Its hardware can provide up to
8 KiB when the relevant APEX functions are disabled. This module does not implement APEX/EDMP
algorithms, compressed FIFO decoding, or the I2C/I3C/AUX interfaces; the LL uses UI SPI.

The settings in this section are available to a **client using the LL driver directly**. They are
**not** runtime or `sysconf.h` options for the two supplied STMs.

### Power and output data rate

The LL functions `icm_45686_set_accel_mode()` and `icm_45686_set_gyro_mode()` select the power mode
of each sensor. Their exposed choices are:

| Sensor | Mode selectors | Application meaning |
| --- | --- | --- |
| Accelerometer | OFF (including OFF_ALT), LP, LN | Disabled, Low-Power or Low-Noise acquisition |
| Gyroscope | OFF, STANDBY, LP, LN | Disabled, standby, Low-Power or Low-Noise acquisition |

These selectors can implement the sensor's documented sleep, gyro-standby, single-sensor and 6-axis
LP/LN operating modes. Accelerometer-only LP operation also supports RCOSC (ALP) or WUOSC (AULP)
clock selection through `icm_45686_select_accel_lp_clk()`, subject to the device's clock restrictions.
The LL does not automatically validate combinations of power mode and ODR.

Accel and gyro ODRs are selected independently with `icm_45686_set_accel_freq()` and
`icm_45686_set_gyro_freq()`:

| Power mode | Available UI ODR values |
| --- | --- |
| LP only | 1.5625, 3.125, 6.25 Hz |
| LP or LN | 12.5, 25, 50, 100, 200, 400 Hz |
| LN only | 800, 1600, 3200, 6400 Hz |

These are sensor configuration values, not a guarantee that every conversion will be delivered
to application code. The caller selects a compatible mode and allows for sensor startup.

### Measurement range and filtering

| Parameter | Values exposed by LL | Function |
| --- | --- | --- |
| Accel FSR | +/-2, 4, 8, 16, 32 g | `icm_45686_set_accel_fsr()` |
| Gyro FSR | +/-15.625, 31.25, 62.5, 125, 250, 500, 1000, 2000, 4000 dps | `icm_45686_set_gyro_fsr()` |
| Accel LP averaging | 1, 2, 4, 5, 7, 8, 10, 11, 16, 18, 20, 32, 64 samples | `icm_45686_set_accel_lp_avg()` |
| Gyro LP averaging | Same supported factors | `icm_45686_set_gyro_lp_avg()` |
| Accel LN UI LPF | No filter, or ODR/4, /8, /16, /32, /64, /128 | `icm_45686_set_accel_ln_bw()` |
| Gyro LN UI LPF | Same bandwidth choices | `icm_45686_set_gyro_ln_bw()` |

The LL returns raw signed sensor values; it does not apply application-side calibration or SI
conversion. Scale acceleration and angular velocity using the *configured* FSR.
The LL also provides `icm_45686_set_spi_pads_slew()` for supported SPI output slew selectors.

### Reading data, FIFO and interrupts through LL

- `icm_45686_get_reg_data()` reads one 14-byte UI register burst: accel XYZ, gyro XYZ and die
  temperature (`struct icm_45686_sensor_data`), all signed 16-bit raw values. Temperature in
  degrees C is `25 + temp_data / 128`.
- `icm_45686_set_fifo_config()` accepts BYPASS, STREAM (overwrite on full) or SNAPSHOT
  (stop on full), internal accel and/or gyro sources, watermark in **packets**, FIFO depth
  selectors and optional HIRES when both sensors are enabled. The fixed packet formats are
  8 B (one sensor), 16 B (both) and 20 B (both with HIRES). The LL does not handle FIFO
  compression or external-sensor insertion. Maximum FIFO depth depends on device features
  and the selected depth code; this driver does not manage APEX feature disabling.
- `icm_45686_get_frame_count()` returns a count of **packets**, not bytes. The caller reads
  packets with `icm_45686_get_fifo_frame()` and can flush with `icm_45686_flush_fifo()`.
  Dual-sensor FIFO configuration also enables timestamp insertion.
- `icm_45686_set_config_int_source()` configures the full 15-source bitmap for INT1 or INT2;
  `icm_45686_set_config_int_pin()` selects polarity, pulse/latch mode and push-pull/open-drain
  drive. `icm_45686_get_int_status()` reads and acknowledges interrupt status.
- `icm_45686_bank0_read/write()` and `icm_45686_ireg_read/write()` expose registers not covered
  by dedicated helpers. The caller is responsible for register semantics, access ordering
  and any feature-specific configuration.

For LL-only use, allocate and retain `struct icm_45686_dsc`, initialize its `spi.cfg`, call
`init_icm_45686()`, then perform reset, configuration and acquisition explicitly. A single
task must own the sensor; the driver is blocking, not ISR-safe, and provides no
transaction-wide locking. Little-endian data configuration is required. Detailed API
preconditions and error behavior are documented in `src/icm45686.h`. For an LL-only build,
compile the LL source without the two STM `.c` files; the supplied Makefile lists all three
sources and the STM translation units require the project mode definitions.

## Using an acquisition state machine

Select **one** acquisition STM with `ICM45686_SENSOR_MODE` in the parent project's `sysconf.h`.
The project supplies distinct values for both `ICM45686_REGS_MODE` and `ICM45686_FIFO_MODE`.
Call the selected initialization function **once** during startup, after required board,
SPI/GPIO/PIO, command and FreeRTOS services are available. Each STM owns the device, one
binary semaphore and one acquisition task. Do not access the same device concurrently
through the LL API while an STM owns it.

| Selected mode | Startup function | Acquisition method | Sensor-side buffering |
| --- | --- | --- | --- |
| `ICM45686_REGS_MODE` | `init_icm45686_stm_regs()` | INT1 DATA_READY; 14-byte burst | None |
| `ICM45686_FIFO_MODE` | `init_icm45686_stm_fifo()` | INT1 FIFO watermark/full; packet drain | 2 KiB FIFO |

Both STMs configure **continuous** sensor acquisition; neither one triggers each conversion.
They are alternatives to custom LL-driven acquisition, not alternative low-power modes of
the silicon. Neither STM exposes runtime changes to ODR, FSR, filters or FIFO settings.

### Fixed acquisition settings in both STMs

| Setting | REGS STM | FIFO STM |
| --- | --- | --- |
| SPI | Bus 0, mode 0, 8-bit, 12 MHz, CSEL2; 20 ns CS setup, no CS rise between segments | Same |
| Accelerometer | LN, 50 Hz, +/-4 g, UI LPF ODR/4 | Same |
| Gyroscope | LN, 50 Hz, +/-2000 dps, UI LPF ODR/4 | Same |
| INT1 electrical | Active LOW, push-pull | Same |
| INT1 source | `UI_DRDY` | `FIFO_THS` and `FIFO_FULL` |
| Completion wait | Binary semaphore, 50 ms timeout | Binary semaphore, 50 ms timeout and FIFO fallback |
| No-data watchdog | 1000 ms since last successful register read | 1000 ms since last successfully read packet |
| Recovery | Mask INT1; wait 5000 ms; reset/reconfigure | Same, including FIFO reconfiguration |

The REGS STM waits approximately 70 ms for gyro startup before routing DATA_READY to INT1.
It can overwrite unread conversions and does not detect missing intermediate samples.
The FIFO STM uses **SNAPSHOT** (stop-on-full), not STREAM, a one-packet watermark and a 2 KiB
buffer. On `FIFO_FULL`, it drains the available packets, flushes FIFO and continues.
A 50 ms timeout in FIFO mode also checks WHO_AM_I, interrupt status and FIFO_COUNT, and
reads any pending packets. The FIFO STM does not discard initial samples solely for
stabilization. Neither STM guarantees lossless delivery under arbitrary callback load.

Changing these fixed measurement, SPI or FIFO parameters requires modifying the selected
`icm45686_stm_*.c` implementation and rebuilding, including its reporting conversion
constants where applicable. The `sysconf.h` switches below do **not** change these values.

## Build-time configuration (`sysconf.h` and `board.h`)

| Setting | REGS | FIFO | Meaning |
| --- | :---: | :---: | --- |
| `ICM45686_SENSOR_MODE` | yes | yes | Select one STM; define both distinct `*_REGS_MODE`/`*_FIFO_MODE` constants |
| `ICM45686_TASK_PRIO` | yes | yes | Acquisition FreeRTOS task priority |
| `ICM45686_TASK_STACK_SIZE` | yes | yes | Acquisition task stack depth (FreeRTOS units) |
| `ICM45686_INTR_TYPE` | yes | yes | `LOW_LEVEL_INTR_TYPE` or `FALL_EDGE_INTR_TYPE` |
| `ICM45686_PADS_SPI_SLEW` | yes | yes | SPI slew selector from `ICM_45686_DRIVE_CONFIG0_PADS_SPI_SLEW_*` |
| `ICM45686_FIFO_HIRES` | - | yes | `0` for 16-byte packets, `1` for 20-byte HIRES packets |
| `ICM45686_REPORT_ACCEL_SI` / `ICM45686_REPORT_GYRO_SI` | yes | yes | Text reports in g / dps |
| `ICM45686_REPORT_ACCEL_LSB` / `ICM45686_REPORT_GYRO_LSB` | yes | yes | Text reports in raw signed sensor LSB |
| `ICM45686_REPORT_FIFO_TMSTMP` | - | yes | FIFO timestamp and valid temperature report |
| `ICM45686_REPORT_FN` | yes | yes | Compile text callback and its 192-byte startup buffer (default `0`) |
| `ICM45686_REPORT_SAMPLE_FN` | yes | yes | Compile binary sample callback (default `0`) |
| `ICM45686_REPORT_MSG_NUM` | yes | yes | Include rolling `[000]`..`[999]` number in text reports (default `0`) |
| `ICM45686_CS_CONT_ID` / `ICM45686_CS_PIN_ID` | yes | yes | Board-specific SPI chip-select controller and pin mask |
| `ICM45686_INT1_CONT` / `ICM45686_INT1_PIN` | yes | yes | Board-specific GPIO interrupt controller and pin mask |

The non-board settings are supplied through project configuration; the controller/pin
assignments come from `board.h`. `ICM45686_REPORT_FN`, `ICM45686_REPORT_SAMPLE_FN` and
`ICM45686_REPORT_MSG_NUM` have documented header defaults of `0`. Define the report routing
selectors explicitly in the parent project, even when unused. The FIFO resolution switch
accepts only `0` or `1`.

For either STM, `ICM45686_INTR_TYPE` selects a consistent sensor/GPIO interrupt pair:

- `FALL_EDGE_INTR_TYPE`: sensor PULSE, GPIO falling edge (the captured edge is used even if
  INT1 has returned HIGH).
- `LOW_LEVEL_INTR_TYPE`: sensor LATCH, GPIO active-low level (the ISR also checks pin level).

The GPIO input has no pull/filter and uses its Schmitt trigger. Changing the interrupt
type does not change the sensor ODR, FIFO mode or data decoding.

## Obtaining measurements and reporting

### Text destinations

Each text-report selector is an independent destination code:

| Value | Destination |
| --- | --- |
| `0` | Disabled |
| `1` | Local `msg()` logging |
| `2` | `icm45686_stm_set_report_fn()` callback only |
| `3` | Both destinations, with identical text including newline |

The four accel/gyro SI/LSB selectors are available in both STMs. Only FIFO mode has the
fifth selector `ICM45686_REPORT_FIFO_TMSTMP`; it prints the raw 16-bit FIFO timestamp
in microseconds and temperature when valid. REGS SI/LSB lines include temperature;
FIFO accel/gyro lines do not. A callback destination (`2` or `3`) requires
`ICM45686_REPORT_FN == 1` at build time. A registered callback alone does not enable
any route; absent callback registration does not cause local fallback.

The optional formatted callback has the same signature in either STM:

```c
void icm45686_stm_set_report_fn(boolean_t (*report_fn)(const char *));
```

### Binary sample callbacks and physical units

With `ICM45686_REPORT_SAMPLE_FN == 1`, the selected STM provides the same setter name
but a **mode-specific sample type**:

```c
/* REGS mode */
void icm45686_stm_set_sample_fn(boolean_t (*sample_fn)(const struct icm_45686_sensor_data *));

/* FIFO mode */
void icm45686_stm_set_sample_fn(boolean_t (*sample_fn)(const struct icm45686_fifo_sample *));
```

Only the active STM implements its signature. Include `sysconf.h` before the selected
STM header so the callback feature macros agree. Register callbacks during startup,
after the corresponding `init_icm45686_stm_*()` call.

| Sample source | Accel (g) | Gyro (dps) | Temperature (degrees C) |
| --- | --- | --- | --- |
| REGS, signed 16-bit | `raw * 4 / 32768` | `raw * 2000 / 32768` | `25 + temp_raw / 128` |
| FIFO standard, signed 16-bit | `raw * 4 / 32768` | `raw * 2000 / 32768` | `25 + temp_raw / 2` (8-bit temp) |
| FIFO HIRES, signed 20-bit | `raw * 32 / 524288` | `raw * 4000 / 524288` | `25 + temp_raw / 128` (16-bit temp) |

HIRES FIFO scaling is **fixed by the sensor** and independent of its UI FSR setting;
accel has 19 effective data bits (its low bit is zero). REGS has no timestamp or
per-sensor validity flags. FIFO samples have `accel_raw[3]`, `gyro_raw[3]`,
`temp_raw`, `timestamp`, `accel_valid`, `gyro_valid`, `temp_valid`, `data_bits`
(16 or 20) and `sequence`.

The FIFO timestamp is a **raw 16-bit sensor counter**, normally 1 microsecond per tick
with the STM's reset-default timestamp configuration; it wraps every 65.536 ms and
is not system uptime. FIFO `sequence` starts at 1, increments for every decoded
packet (including packets marked invalid), wraps modulo 2^32 and continues across
sensor recovery. It cannot identify packets lost *inside* the hardware FIFO.
The optional text report number `[000]`..`[999]` also counts processed
samples/packets and is not a timestamp or a verified hardware conversion count.

Both callbacks execute **synchronously in the acquisition FreeRTOS task**, never
in the GPIO ISR. Callback data pointers are valid only until the callback returns;
copy values that must be retained. Keep callback work bounded and nonblocking
(for example, `xQueueSend(..., 0)`). A return value of `FALSE` increments the
respective diagnostic counter without resetting acquisition. A missing callback
is not treated as an error. Text output, binary callbacks and sample acquisition
are independent: disabling text does not stop measurement acquisition.

## Errors and diagnostics

Both STMs enter recovery on failed sensor/SPI operations or when the 1000 ms
no-data watchdog expires. Recovery masks INT1, waits approximately 5 seconds,
then attempts reset and reconfiguration. REGS timeout checks WHO_AM_I; FIFO
timeout additionally inspects interrupt status and FIFO_COUNT. Neither STM
calls an application callback for failed SPI reads.

The `icms` command shows mode-specific counters:

- REGS: `read_data_reg`, `unknown_intr`, `drdy_timeout`, `no_data_reset`, `stm_rst`.
  DRDY timeout counts waits that expired, not a count of lost conversions.
- FIFO: `read_frame_cnt`, `fifo_full`, `unknown_intr`, `fifo_empty_err`,
  `irq_pin_high`, `stale_irq`, `timeout_fifo_nonempty`, `no_data_reset`,
  `max_fifo_backlog`, `stm_rst`. `timeout_fifo_nonempty` counts timeout
  fallbacks that **found** a nonempty FIFO; `max_fifo_backlog` is the largest
  validated count offered for draining, not necessarily packets successfully
  drained or the hardware peak.
- With callbacks enabled, `report_fn_err` and/or `sample_fn_err` count callbacks
  returning `FALSE`. Diagnostics survive STM recovery and reset on firmware restart.

The state machines deliberately provide no delivery queue, guaranteed sample
cadence, runtime sensor reconfiguration or automatic application-side filtering.
Clients needing different acquisition settings or custom processing can use
the LL driver directly or adapt the selected STM implementation.

## Dependencies

The LL driver requires the project's SPI HAL, FreeRTOS timing and common,
timing and error-handling facilities. The STMs additionally require FreeRTOS
tasks/semaphores, GPIO and pinmux HALs, board definitions, command-line
registration and `msg()` diagnostics. The parent firmware supplies `sysconf.h`,
`board.h` and the platform backends; they are not provided by this library.

## Reference

TDK InvenSense ICM-45686 datasheet **DS-000577, revision 1.0**: power modes
(section 4.20), FIFO packet layout (section 6), indirect registers (section 14),
endianness (section 15) and register descriptions (section 17).

## License

See the license information in the source files.

## Author

Jan Rusnak  
AZTech
