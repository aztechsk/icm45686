# ICM-45686 Driver

Embedded C driver for the TDK ICM-45686 6-axis inertial sensor, combining a 3-axis accelerometer and a 3-axis gyroscope.

The project consists of a low-level SPI driver for direct device access and two optional FreeRTOS state-machine layers for interrupt-driven sensor data acquisition.

The low-level driver provides sensor initialization, reset and identification, accelerometer and gyroscope configuration, direct register data access, FIFO operation, interrupt configuration and access to both Bank0 and indirect IREG registers.

## Features

* SPI communication with the ICM-45686
* Sensor soft reset and device identification check
* Accelerometer and gyroscope power-mode configuration
* Output data rate and full-scale range configuration
* Low-Power averaging and Low-Noise bandwidth configuration
* Reading accelerometer, gyroscope and temperature data
* FIFO configuration, flushing and frame reading
* Standard and high-resolution FIFO data handling
* INT1 and INT2 source and pin configuration
* Direct Bank0 register access
* Indirect IREG access
* Optional FreeRTOS state machine using data-ready interrupts
* Optional FreeRTOS state machine using FIFO watermark interrupts

## Files

### `icm45686.c`

Low-level ICM-45686 driver implementation.

It provides SPI-based device access, reset and identification, accelerometer and gyroscope configuration, sensor data reads, FIFO handling, interrupt configuration and access to the device register spaces.

### `icm45686.h`

Public interface of the low-level driver.

It contains the public API declarations for device initialization, sensor configuration, data acquisition, FIFO operation, interrupt handling and direct register access.

### `icm45686_def.h`

ICM-45686 register and data definitions.

It contains register addresses, configuration values, register layouts, interrupt structures, sensor data structures, FIFO configuration and FIFO frame definitions used by the driver.

### `icm45686_stm_regs.c`

FreeRTOS state-machine layer using the ICM-45686 data-ready interrupt.

It initializes and configures the sensor, waits for completed measurements and reads accelerometer, gyroscope and temperature data directly from the sensor registers.

### `icm45686_stm_regs.h`

Public interface of the register-data state-machine layer.

It provides initialization of the corresponding FreeRTOS sensor task.

### `icm45686_stm_fifo.c`

FreeRTOS state-machine layer using FIFO watermark interrupts.

It initializes and configures the sensor and FIFO, waits for FIFO events and processes the available FIFO frames.

### `icm45686_stm_fifo.h`

Public interface of the FIFO state-machine layer.

It provides initialization of the corresponding FreeRTOS sensor task.

## Dependencies

The low-level driver depends on:

* a project-specific SPI HAL
* FreeRTOS timing support
* project-specific common, timing and error-handling facilities

The state-machine layers additionally use FreeRTOS tasks and synchronization primitives together with project-specific GPIO, pinmux, command-line and diagnostic facilities.

These dependencies are intentionally kept outside this repository and must be provided by the target project.

## Usage

An application may use the low-level driver directly when it needs explicit control over sensor configuration, data acquisition, FIFO operation or interrupt handling.

Two example state-machine layers are provided for continuous interrupt-driven acquisition. The register-data variant reads measurements directly after the sensor signals data availability, while the FIFO variant acquires measurements through the internal FIFO using its watermark interrupt.

SPI configuration, GPIO assignment, interrupt type, sensor operating parameters and optional measurement reporting are target-specific and are expected to be configured by the surrounding project.

## License

See the license information in the source files.

## Author

Jan Rusnak  
AZTech
