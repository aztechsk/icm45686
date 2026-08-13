# ICM-45686 Driver

Driver for the TDK ICM-45686 6-axis inertial measurement unit.

The driver provides SPI access to the ICM-45686 accelerometer and gyroscope, including sensor configuration, direct register access, FIFO handling, interrupt configuration and indirect IREG access.

## Features

- SPI communication through `spi_hal`
- Device initialization and soft reset
- `WHO_AM_I` verification
- Accelerometer and gyroscope mode control
- Output data rate configuration
- Accelerometer and gyroscope full-scale range configuration
- Low-Power averaging configuration
- Low-Noise bandwidth configuration
- Direct accelerometer, gyroscope and temperature data reads
- FIFO configuration and status handling
- FIFO stream, snapshot and bypass modes
- 8-, 16- and 20-byte FIFO frame parsing
- High-resolution 20-bit FIFO data support
- FIFO watermark configuration
- FIFO flush and frame count handling
- INT1 and INT2 source configuration
- Interrupt pin polarity, mode and drive configuration
- Interrupt status reading
- Accelerometer Low-Power clock selection
- Direct Bank0 register access
- Indirect 16-bit IREG access
- Register definitions for Bank0 and internal register banks

## Files

- `icm45686.c` - driver implementation
- `icm45686.h` - public driver API
- `icm45686_def.h` - register definitions, configuration enums and data structures

## Interface

The driver uses the project SPI HAL through `struct spi_hal_dev`.

The device descriptor is represented by:

```c
typedef struct icm_45686_dsc *icm_45686;
```

and contains the SPI device descriptor together with the currently configured FIFO frame size.

All public driver functions return `0` on success and a negative error code on failure.

## Sensor data

Direct register reads provide:

- 3-axis accelerometer data
- 3-axis gyroscope data
- temperature data

The driver converts the sensor register byte order to the native little-endian representation used by the target system.

## FIFO

The FIFO can be configured for:

- accelerometer data
- gyroscope data
- combined accelerometer and gyroscope data
- high-resolution 20-bit data
- watermark threshold
- stream, snapshot or bypass operation
- supported FIFO depth settings

FIFO frames are decoded according to the configured format:

- 8-byte frame for a single sensor
- 16-byte frame for accelerometer + gyroscope
- 20-byte high-resolution frame

## Interrupts

INT1 and INT2 can be configured independently for supported interrupt sources, including:

- FIFO full
- FIFO watermark
- data ready
- FSYNC
- reset done
- PLL ready
- wake-on-motion
- I3C protocol error
- I2C master done
- EDMP/APEX event

Interrupt pin polarity, pulse/latch mode and push-pull/open-drain drive can also be configured.

## Author

Jan Rusnak  
Copyright (c) 2025 AZTech
