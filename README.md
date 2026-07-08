# LLM4MentalHealth

LLM4MentalHealth is a research-oriented robotics project that explores how embodied, non-anthropomorphic social agents can support mental health and wellbeing. The current implementation centers on a desktop flower robot that combines expressive movement, color, and motorized stem behavior into a single interactive platform. The repository also contains the scaffolding for a future companion agent, the Pet Rock, as a second social robot platform.

## Project vision

This project sits at the intersection of robotics, human-computer interaction, and mental health research. The flower robot is designed to act as a non-verbal companion that can communicate emotion and intent through:

- five independently addressable petal LEDs,
- five petal-angle actuators,
- a motorized stem / continuum-style drive system,
- a ROS 2-based control interface for real-time command and monitoring.

The system is intentionally expressive rather than humanoid. The goal is to create a calm, approachable, low-stakes interaction experience that can be used in behavioral research and interactive prototypes.

## Physical structure of the robot

The robot is conceptually divided into two major physical subsystems:

1. Flower head
   - A five-petal structure that can open, close, and change color.
   - Each petal has an LED channel and an associated servo-driven motion channel.
   - The head is intended to support emotional expression through posture and visual state.

2. Stem and drive system
   - A motorized stem platform provides motion along the vertical axis and supports a continuum-like, organic movement profile.
   - The current firmware uses an N20 geared motor with encoder feedback for precise rotation / position control.

### Physical layout summary

- Embedded controller: ESP32-based microcontroller running the firmware.
- Actuation: PCA9685 servo driver for petal positioning and an N20 motor for stem motion.
- Sensing: encoder feedback from the N20 motor.
- Communication: Wi-Fi + micro-ROS bridge to a host computer.
- Operator interface: a Tkinter-based GUI and ROS 2 topics.

## Repository structure

- [README.md](README.md): project-level overview and architecture.
- [FullCode_ws](FullCode_ws): host-side ROS 2 workspace, GUI, and message definitions.
- [FullCodeMICROCONTROLLEr](FullCodeMICROCONTROLLEr): embedded firmware for the ESP32-based flower robot.
- [Testing](Testing): hardware and software experiments, prototypes, and validation sketches.

## Software architecture

The system uses a split architecture:

- A host-side ROS 2 layer publishes high-level commands.
- A micro-ROS agent bridges those commands over Wi-Fi to the ESP32.
- The embedded firmware interprets the command stream and drives the robot’s motors, LEDs, and servos.

### Core control flow

1. A user manipulates the GUI or a ROS topic publisher.
2. The host sends a RobotCommand message over ROS 2.
3. The micro-ROS bridge forwards the message to the ESP32.
4. The ESP32 updates servo positions, LED colors, and motor target state.
5. The embedded firmware continuously applies the new state and reports debugging information.

## ROS node map

```mermaid
flowchart LR
    A[User / Operator] --> B[flower_gui.py]
    B --> C[/flower_commands]
    C --> D[micro-ROS Agent]
    D --> E[ESP32 flower_node]
    E --> F[Servo Control]
    E --> G[LED Control]
    E --> H[N20 Motor Control]
    E --> I[/flower_debug]
    I --> J[Foxglove / Debug Tools]
```

### ROS topics and roles

- /flower_commands
  - Custom message type: RobotCommand.
  - Carries servo angles, LED brightness, LED colors, and N20 motor target state.

- /flower_debug
  - Debug string topic used for troubleshooting and runtime telemetry.

### ROS-side components

- flower_gui.py
  - Runs as a ROS 2 node named flower_gui_node.
  - Publishes command messages to /flower_commands.

- flower.launch.py
  - Launches the micro-ROS agent, Foxglove bridge, and GUI in one place.

## Code and subsystem overview

### Host-side workspace

The host workspace contains the user-facing control layer and the ROS message definition. It is responsible for turning operator input into a structured command stream.

### Embedded firmware

The firmware project contains the real-time control logic for the physical robot. It handles networking, command reception, LED rendering, servo actuation, and motor control.

## Development workflow

### Host workspace

From the ROS workspace, the typical flow is:

```bash
colcon build --packages-select flower_messages
source install/local_setup.bash
ros2 run micro_ros_setup build_agent.sh
source install/local_setup.bash
```

### Firmware

The embedded firmware is built with PlatformIO. The firmware expects a working Wi-Fi configuration and a running micro-ROS agent.

## Notes for future contributors

- The current implementation is focused on the flower robot and its expressive motion system.
- The message schema is intentionally compact so it can evolve into richer behavioral commands.
- The project is still structured as a prototype, but the separation between host control, ROS transport, and embedded actuation is already clear.
- The Pet Rock remains a future extension path and can share the same communication and control architecture.

For deeper implementation detail, see the READMEs in the subprojects:

- [FullCode_ws](FullCode_ws)
- [FullCodeMICROCONTROLLEr](FullCodeMICROCONTROLLEr)
