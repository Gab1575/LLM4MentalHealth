# FullCode_ws

This workspace is the host-side control stack for the flower robot. It contains the ROS 2 message definitions, the graphical control interface, and the launch configuration that brings the robot’s software stack together.

## Purpose

The host workspace is responsible for taking human input and translating it into a structured command stream that can be sent to the ESP32-based flower controller.

## Folder and file overview

### Root files

- flower_gui.py
  - The main graphical control application.
  - Uses Tkinter to create a dashboard for servo angles, LED color, LED brightness, and the N20 motor target position.
  - Runs as a ROS 2 node and publishes RobotCommand messages to the /flower_commands topic.

- flower.launch.py
  - Launch file for the host-side runtime stack.
  - Starts the micro-ROS agent, Foxglove bridge, and the GUI process.

### ROS package structure

- src/flower_msgs/
  - Contains the custom ROS 2 message package used by the system.
  - The message definition is intentionally simple and compact so that it can be extended over time.

- src/flower_msgs/msg/RobotCommand.msg
  - Defines the command schema sent from the host to the embedded controller.
  - Fields include five servo angles, N20 PWM speed, N20 target rotations, five LED colors, and five LED brightness values.

- src/ros2.repos
  - Optional dependency manifest for pulling in ROS package sources when building the workspace.

## Deep dive by file

### flower_gui.py

This is the most important host-side application file. It combines three responsibilities:

1. GUI construction
   - Builds a Tkinter window with controls for servo values, LED brightness, LED color selection, and the N20 motor target.
   - Includes master override options so a single brightness or color value can be applied to all petals.

2. Configuration persistence
   - Saves and loads GUI state to a JSON file at ~/.flower_gui_settings.json.
   - Makes the control interface more usable during repeated testing and demonstrations.

3. ROS publishing
   - Creates a publisher for RobotCommand messages.
   - Converts user input into the ROS message schema and publishes it at a fixed interval.

Key functions:

- __init__
  - Initializes the ROS node, GUI widgets, and periodic callbacks.

- setup_ui
  - Builds the full interface, including servo sliders, LED controls, and the N20 motor controls.

- choose_color
  - Opens a color picker and updates the selected color variable.

- save_config / load_config
  - Store and restore the GUI state so the operator can resume quickly.

- publish_command
  - Packs the current GUI settings into a RobotCommand and publishes it.

- ros_spin
  - Drives ROS event processing while the Tkinter UI remains responsive.

### flower.launch.py

This launch file is the entry point for running the robot’s host-side tools together.

It starts:

- micro_ros_agent for serial/UDP bridging to the embedded firmware,
- foxglove_bridge for visualization and topic inspection,
- flower_gui.py for manual control.

This makes the system easier to run in one step, instead of manually launching each component separately.

### src/flower_msgs/msg/RobotCommand.msg

This message schema is the contract between the host controller and the ESP32 firmware.

Message fields:

- servo_angles[5]
  - Intended petal / actuator positions.

- n20_pwm
  - PWM value for the N20 drive motor.

- n20_target_rotations
  - Desired motor target position in rotations.

- led_colours_hex[5]
  - LED color values for each petal, encoded as hexadecimal color values.

- led_colours_brightness[5]
  - Per-petal brightness values from 0 to 255.

## Notes

The GUI currently assumes a specific workspace install path for the generated Python package. If the repository is moved, the import path in flower_gui.py should be updated to match the new install location.


