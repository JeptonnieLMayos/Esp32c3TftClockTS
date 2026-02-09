# Rectangular Analog Clock (ESP-IDF)

An ESP-IDF–based analog clock UI rendered on a 240×320 LCD, using low-level graphics primitives and FreeRTOS tasks.
## Status
Work in progress.
This project is under active development. APIs, internal structure, and behavior may change.

## Target Hardware
- ESP32 (ESP-IDF)
- 240×320 SPI LCD (ILI9341-compatible)
- Integrated touch input on LCD


## Current State

Implemented:
- Rectangular analog clock layout
- Hour and minute markers
- Second, minute, and hour hands
- Touch-based interaction (in progress)

Planned / incomplete:
- Time synchronization
- Improved buffer reuse

## Build
Built using ESP-IDF.
