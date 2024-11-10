# Water-Level-Control

This project implements a **Water Level Control ** using esp32. The system monitors the water level in a tank or reservoir and automatically controls a water pump based on pre-defined thresholds. It can be used in residential, agricultural, or industrial applications to maintain optimal water levels efficiently.

---

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [System Design](#system-design)
  - [Flowchart](#flowchart)
  - [Logic](#logic)
- [Installation and Usage](#installation-and-usage)


---

## Project Overview

This Water Level Control System uses ultrasonic sensors to measure the water level in a tank and control a pump to maintain the desired water level. The system checks the water level at regular intervals and toggles the pump on or off based on high and low thresholds, ensuring that water is only added when needed.

## Features

- **Automatic Pump Control**: Starts and stops the pump based on water levels.
- **Monitoring**: Continuously monitors water levels using sensors.
- **User-defined Thresholds**: Configurable high and low water level limits.
- **Energy Efficient**: Activates the pump only when necessary, saving energy.

## Hardware Requirements

- **esp32 Board**: Used as the main controller for the system.
- **Ultrasonic Sensor**: Measures water levels by calculating the distance between the sensor and the water surface.
- **Relay Module**: Controls the water pump based on signals from the Arduino.
- **Water Pump**: Manages water flow in and out of the tank or reservoir.
- **Power Supply**: Powers the Arduino, sensor, and pump.

## Software Requirements

- **Arduino IDE**: Used to write, compile, and upload the code to the Arduino board.

## System Design

### Flowchart

The system follows a logical sequence to monitor water levels and control the pump:
1. Measure the water level using an ultrasonic sensor.
2. If the level is below the minimum threshold, activate the pump.
3. If the level is above the maximum threshold, deactivate the pump.
4. Repeat the process at regular intervals to maintain the desired level.

### Logic

- **Low Threshold**: When the water level falls below this point, the pump is turned on to refill the tank.
- **High Threshold**: When the water level reaches this point, the pump is turned off to prevent overflow.

## Installation and Usage

1. **Setup Hardware**: Connect the ultrasonic sensor, relay module, and pump to the Arduino as per the schematic diagram (available in the repository).
2. **Configure Thresholds**: Define the high and low water level thresholds in the code.
3. **Upload Code**: Upload the `.ino` file to the Arduino using the Arduino IDE.
4. **Run System**: Power the system, and it will automatically begin monitoring and controlling the water level.

## Results

The Water Level Control System maintains the water level within the desired range. The pump activates only when necessary, contributing to efficient water management and reduced electricity usage.

