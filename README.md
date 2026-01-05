# Embedded Driver Development – MPU6050 & OLED using ESP32

## Overview

This project was implemented as part of an embedded firmware / driver development task.  
The objective was to interface an MPU6050 accelerometer and an SSD1306 OLED display using low-level I2C communication, without relying on high-level libraries.

---

## Hardware Used

- ESP32  
- MPU6050 (Accelerometer)  
- SSD1306 OLED Display (128×64, I2C)

---

## Firmware Implementation

### MPU6050 (Sensor Interface)

- I2C communication implemented using Arduino `Wire` as the transport layer  
- Sensor woken from sleep by writing to the power management register  
- X, Y, Z accelerometer data read using burst register reads  
- Sensor presence verified using the `WHO_AM_I` register  
- Raw accelerometer values converted to physical units (g)

> Note: The firmware logic is independent of the I2C abstraction layer and can be migrated to a register-level I2C driver if required.

---

### OLED Display (SSD1306)

- No external OLED display libraries used  
- SSD1306 initialized manually using datasheet-defined command sequences  
- Direct control of page and column addressing  
- Implemented screen clearing and cursor positioning  
- Pixel data written directly to the display over I2C

---

### Text Rendering

- Custom 5×7 bitmap font implemented  
- Characters rendered manually without graphics libraries  
- Real-time accelerometer values displayed on the OLED

---

## Additional Functionality

- Device tilt calculated using accelerometer data  
- Simple low-pass filtering applied to reduce noise  
- Bubble-level style indicator implemented for tilt visualization  
- Display update rate controlled using non-blocking timing (`millis()`)

---

## Firmware Flow

1. Initialize I2C interface  
2. Wake MPU6050 from sleep mode  
3. Initialize SSD1306 OLED display  
4. Validate sensor using `WHO_AM_I`  
5. Read accelerometer data  
6. Convert raw values to physical units  
7. Calculate tilt angle  
8. Update OLED display

---

## Notes

- ESP32 was used as a production-capable microcontroller  
- Dynamic memory usage was avoided in the main loop  
- Blocking delays were avoided to keep the firmware scalable  
- Focus was on correctness, clarity, and embedded-safe practices

---

## Summary

This project demonstrates embedded firmware fundamentals including sensor interfacing, custom OLED control, safe data formatting, and real-time visualization. The implementation prioritizes clarity and correctness and is structured to allow future migration to lower-level peripheral drivers if required.
