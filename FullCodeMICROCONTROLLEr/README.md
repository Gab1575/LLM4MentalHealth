# FullCodeMICROCONTROLLEr

This project is the embedded firmware for the flower robot. It runs on an ESP32-based controller and is responsible for receiving commands from ROS 2, driving the physical actuators, and maintaining the robot’s real-time behavior.

## Purpose

The firmware implements the robot’s low-level control layer. Its responsibilities are:

- establishing Wi-Fi connectivity for micro-ROS communication,
- receiving RobotCommand payloads from the host workstation,
- updating the servo-driven petal positions,
- driving petal LEDs with color and brightness values,
- controlling the N20 motor and encoder-based position loop,
- publishing debug telemetry back over ROS for troubleshooting.

## High-level architecture

The firmware is organized around a simple control loop:

1. Initialize Wi-Fi and the micro-ROS transport.
2. Initialize the hardware subsystems: LEDs, servos, and motor.
3. Wait for incoming RobotCommand messages.
4. Apply the new command state each loop iteration.
5. Update hardware outputs and publish debug information.

## Folder and file overview

### Root files

- platformio.ini
  - PlatformIO configuration for building and uploading the firmware.
  - Defines dependencies and board settings for the ESP32 target.

### include/

The include directory contains shared declarations and hardware abstractions.

- MicroRos.h
  - Central header for ROS-related globals, data structures, and function declarations.
  - Defines the shared Data struct that holds servo, LED, and N20 command state.

- N20.h
  - Pin definitions, constants, and function declarations for the N20 motor.

- petal_lights.h
  - LED subsystem declarations.

- servo_control.h
  - Servo subsystem declarations and constants.

- MicroRosWifiCredentialsEXAMPLE.h
  - Example template for storing Wi-Fi credentials locally.

### src/

The source directory contains the actual implementation of the firmware logic.

- main.cpp
  - Main application entry point.
  - Performs startup initialization and runs the main control loop.

- MicroRos.cpp
  - Implements Wi-Fi setup, micro-ROS initialization, message subscription, and debug publishing.

- servo_control.cpp
  - Implements servo initialization and motion control.

- petal_lights.cpp
  - Implements LED rendering and color updates.

- N20.cpp
  - Implements encoder-based position tracking and motor control.

## Deep dive by file

### src/main.cpp

This is the orchestrator for the firmware.

Responsibilities:

- initializes serial logging,
- sets up Wi-Fi,
- initializes LED control,
- initializes servo control,
- initializes the N20 motor,
- calls the micro-ROS setup routine,
- enters the main loop where each subsystem is refreshed from the latest command state.

Main loop behavior:

- processes pending ROS callbacks,
- updates each petal LED from the current command data,
- applies the latest servo target positions,
- updates the N20 motor position estimate and control loop,
- shows the new LED output.

### src/MicroRos.cpp

This file is the embedded ROS bridge.

Its main functions are:

- WiFiSetup()
  - Connects the ESP32 to Wi-Fi.
  - Resolves the host agent hostname using mDNS.
  - Configures the micro-ROS transport.

- MicroRosSetup()
  - Initializes the ROS 2 node, executor, subscription, and debug publisher.
  - Subscribes to the /flower_commands topic.

- subscription_callback()
  - Receives incoming RobotCommand data.
  - Copies the servo angles, N20 target, N20 speed, LED colors, and brightness values into the shared global data struct.

- send_debug()
  - Formats and publishes debug strings to the /flower_debug topic.

This module is the critical link between the host system and the physical hardware.

### src/servo_control.cpp

This file manages the flower’s petal motion.

Key responsibilities:

- initialize the PCA9685 servo driver over I2C,
- reset the servo state,
- move servos by a relative delta,
- set servos to an absolute target position,
- optionally sweep a servo across positions.

The servo mapping is important here. The firmware remaps logical petal indices to the physical PCA9685 output channels:

- petal 0 -> channel 7
- petal 1 -> channel 8
- petal 2 -> channel 9
- petal 3 -> channel 10
- petal 4 -> channel 11

This abstraction allows the software to think in terms of petals while the hardware uses a different physical channel arrangement.

### src/petal_lights.cpp

This file implements the flower’s visual output system.

Responsibilities:

- create and manage an LED array,
- update individual petal colors from incoming command values,
- scale each LED’s brightness,
- push the color values to the physical LEDs.

The implementation uses FastLED and a CRGB buffer. It updates the LED state in memory first and only shows the result once the full frame is prepared.

### src/N20.cpp

This file controls the N20 geared motor and its feedback loop.

Responsibilities:

- configure motor control pins,
- initialize encoder input pins,
- attach an interrupt service routine for encoder pulses,
- track motor position in rotations,
- apply a simple control loop that moves the motor toward the commanded target.

Key functions:

- n20EncoderISR()
  - Reads the encoder state and updates the tick count.

- n20MotorBegin()
  - Initializes the hardware and synchronizes the internal position estimate with the commanded target.

- n20MotorSetSpeed()
  - Applies PWM speed control.

- n20MotorSetDirection()
  - Sets the motor direction through the H-bridge pins.

- n20MotorControl()
  - Compares the current position to the target and drives the motor until it is within tolerance.

- n20MotorPosition()
  - Converts encoder ticks into a position estimate for the control loop.

## Subsystem breakdown

### Communication subsystem

The communication subsystem is centered in MicroRos.cpp and the shared header. It handles:

- Wi-Fi connection,
- mDNS discovery of the ROS agent,
- micro-ROS initialization,
- command subscriptions,
- debug topic publishing.

### Actuation subsystem

The actuation subsystem includes:

- servo_control.cpp for petal position,
- N20.cpp for stem / rotation movement.

### Expression subsystem

The expression subsystem includes:

- petal_lights.cpp for color and brightness,
- the shared command state structure that allows the host to compose mood-like visual states.

## Shared data model

The firmware uses a global Data structure to hold the current desired state. It is updated by the ROS subscriber and read by the main loop.

The shared state includes:

- five servo angles,
- one N20 PWM value,
- one N20 target rotation value,
- five LED colors,
- five LED brightness values.

This design keeps the control loop simple and makes the hardware state explicit.

## Build and run notes

The firmware is built with PlatformIO.

Typical workflow:

1. Ensure Wi-Fi credentials are configured in the local header file.
2. Build the firmware with PlatformIO.
3. Upload it to the ESP32.
4. Start the ROS-side runtime stack.
5. Observe the robot behavior and debug topics.

## Practical notes for contributors

- The current implementation is a prototype, so the control logic is intentionally straightforward.
- The ROS message path is compact and should be easy to evolve into richer behavior commands.
- The mapping between logical petal indices and physical PWM channels is a key detail that should be preserved when modifying the hardware layout.
- When changing the motor control behavior, keep in mind that the current loop uses encoder feedback and a position tolerance rather than a full PID controller.
