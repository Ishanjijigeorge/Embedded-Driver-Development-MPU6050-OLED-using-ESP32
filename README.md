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

## What I Implemented

### MPU6050 (Sensor Side)
- I2C communication at register level  
- Woke the sensor from sleep using the power management register  
- Read X, Y, Z accelerometer data using burst reads  
- Verified sensor communication using the WHO_AM_I register  
- Converted raw values into acceleration values (g)  

---

### OLED Display (Display Side)
- No high-level OLED display libraries used  
- Manual SSD1306 initialization sequence  
- Direct control of page and column addressing  
- Implemented screen clear and cursor positioning  
- Sent pixel data directly to the display  

---

### Text Rendering
- Implemented a custom 5×7 bitmap font  
- Manually rendered characters and numbers  
- Displayed real-time accelerometer values on the OLED  

---

## Bonus Implementation

- Calculated device tilt using accelerometer data  
- Applied simple low-pass filtering to reduce noise  
- Displayed tilt using a bubble-level style indicator  
- Updated only required display sections to reduce flicker  

---

## Firmware Flow

1. Initialize I2C communication  
2. Wake MPU6050 from sleep mode  
3. Initialize SSD1306 OLED display  
4. Read accelerometer data  
5. Convert raw values to physical units  
6. Calculate tilt angle  
7. Display values on OLED  

---

## Notes

- ESP32 was used as a professional microcontroller allowed by the task  
- Focus was on low-level firmware and driver development  
- UI complexity was intentionally kept minimal  

---

## Summary

This project demonstrates basic embedded firmware skills including register-level sensor interfacing, custom OLED driver development, and real-time data visualization without using high-level libraries.

