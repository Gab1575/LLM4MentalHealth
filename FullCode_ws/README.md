# FullCode_ws

This workspace is the host-side control stack for the flower robot. It includes the host GUI, ROS2 message definitions, smart command multiplexing, and vision-based position sensing.

## Purpose

The host workspace translates user intent and visual perception into a structured ROS2 command stream for the embedded flower controller. It also provides runtime launch orchestration for camera-based localization and face-tracking perception.

## High-level ROS2 Architecture

The control chain is split into three core domains:

1. **Host UI and command generation**
   - `flower_gui.py` publishes operator-selected servo, LED, and N20 values.
   - It also publishes kinematic goals and control mode state.

2. **Command multiplexing**
   - `flower_mux.py` merges GUI state with either manual or kinematic pose commands.
   - It publishes the final `RobotCommand` to `/flower_commands` for the micro-ROS bridge.

3. **Vision-based localization & perception**
   - `localization_tracker` detects red and blue target balls and publishes 3D points to `/vision/red_ball` and `/vision/blue_ball`.
   - `face_tracker` publishes a lightweight point representing face pan/tilt angles to `/face_tracking_angles`.

4. **Hardware bridge**
   - `micro_ros_agent` bridges `/flower_commands` from ROS2 into the embedded firmware transport layer.
   - `foxglove_bridge` exposes ROS2 topics for debugging and visualization.

## Workspace files and packages

### Root files

- `flower_gui.py`
  - Main host-side control GUI.
  - Publishes to `/manual_commands`, `/kinematic_commands`, and `/control_mode`.
  - Uses `flower_msgs.msg.RobotCommand` for manual/full-state commands.
  - Uses `std_msgs.msg.Float64MultiArray` for kinematic goal publication.
  - Uses `std_msgs.msg.String` for publishing the selected control mode.

- `flower_mux.py`
  - Smart multiplexer node.
  - Subscribes to `/manual_commands`, `/kinematic_calculated_commands`, and `/control_mode`.
  - Publishes the merged `RobotCommand` to `/flower_commands`.
  - Ensures time, LED, and N20 payload fields from the GUI remain present in kinematic mode.

- `flower.launch.py`
  - Launch description for the host stack.
  - Starts `micro_ros_agent`, `foxglove_bridge`, `flower_gui`, `face_tracker`, `localization_tracker`, and `flower_mux`.

- `flower_gui_advanced.py`
  - An alternate, more compact dashboard implementation.
  - Provides a similar control surface and visual kinematics preview.

### ROS 2 package sources

- `src/flower_msgs/`
  - Defines the custom `RobotCommand` ROS2 message type.
  - Used by `flower_gui.py`, `flower_mux.py`, and the embedded controller bridge.

- `src/controller/controller/continuum_controller_node.py`
  - Continuum controller node that consumes kinematic commands and vision feedback.
  - Subscribes to `/kinematic_commands`, `/vision/red_ball`, and `/vision/blue_ball`.
  - Uses an internal forward kinematics model and ball-tracking pose estimation.

- `src/ros2-opencv-localization/`
  - Contains the `localization_tracker` node and a camera launch file.
  - Responsible for 3D ball localization from a connected USB camera.

- `src/ros2-opencv-face-tracker/`
  - Contains the face tracker node that publishes face pan/tilt offset angles.
  - Uses a second USB camera for live face detection.

- `src/uros/micro-ROS-Agent/`
  - Contains the micro-ROS agent package used by the host launch file.

## ROS2 Topic & Node Communication Tree

```text
[ Operator ]
      |
      v
flower_gui.py (host node)
  - publishes /manual_commands         (RobotCommand)
  - publishes /kinematic_commands      (Float64MultiArray)
  - publishes /control_mode            (String)
      |
      v
flower_mux.py (host mux node)
  - subscribes /manual_commands
  - subscribes /kinematic_calculated_commands
  - subscribes /control_mode
  - publishes /flower_commands        (RobotCommand)
      |
      v
micro_ros_agent (bridge)
      |
      v
[ Embedded ESP32 / firmware ]

[ Vision & Perception ]
      |
      v
localization_tracker (vision node)
  - publishes /vision/red_ball         (PointStamped)
  - publishes /vision/blue_ball        (PointStamped)
      |
      v
continuum_controller_node.py (controller)
  - subscribes /vision/red_ball
  - subscribes /vision/blue_ball
  - subscribes /kinematic_commands

face_tracker (perception node)
  - publishes /face_tracking_angles    (Point)
```

## Simplified Architecture Diagram

```text
[ Operator ]
      |
      v
flower_gui.py
  publishes /manual_commands  + /kinematic_commands + /control_mode
      |
      v
flower_mux.py
  selects mode -> merges GUI state with calculated kinematic commands
      |
      v
/flower_commands  ---> micro_ros_agent ---> embedded firmware

[ Vision ]
      |
      +--> localization_tracker
      |      publishes /vision/red_ball
      |               /vision/blue_ball
      |      (USB camera sees red and blue target balls)
      |
      +--> continuum_controller_node
      |      subscribes /vision/red_ball
      |                 /vision/blue_ball
      |                 /kinematic_commands
      |
      +--> face_tracker
             publishes /face_tracking_angles
             (separate camera, face pan/tilt signal)
```

## Topic semantics and data flow

### `/manual_commands` (RobotCommand)
- Published by `flower_gui.py` continuously.
- Contains the full manual output state from the GUI, including:
  - `servo_angles[5]`
  - `servo_time[5]`
  - `n20_pwm`
  - `n20_target_rotations`
  - `n20_zero`
  - `led_colours_hex[5]`
  - `led_colours_brightness[5]`
- Used as the authoritative state cache in `flower_mux.py` for both manual and kinematic modes.

### `/kinematic_commands` (Float64MultiArray)
- Published by `flower_gui.py` from the kinematic control panel.
- Carries `[theta1, phi1, theta2, phi2]` as raw joint targets.
- Consumed by the continuum controller and/or any kinematic solver node.

### `/control_mode` (String)
- Published by `flower_gui.py` when the UI mode changes.
- Values are `manual` or `kinematic`.
- Controls whether `flower_mux.py` forwards raw manual state or uses the kinematic-calculated command path.

### `/flower_commands` (RobotCommand)
- Final merged command stream.
- Published by `flower_mux.py` to the hardware bridge.
- This is the topic the embedded controller ultimately receives via `micro_ros_agent`.

### `/vision/red_ball` and `/vision/blue_ball` (PointStamped)
- Published by `localization_tracker`.
- Each message carries the 3D position of a color-marked ball in camera coordinates.
- Used by `continuum_controller_node.py` to infer the flower head pose and to align the continuum motion.

### `/face_tracking_angles` (Point)
- Published by `face_tracker`.
- `x` and `y` carry pan and tilt error angles in degrees.
- Intended for face-aligned perception or downstream attention control.

## Physical sensor mapping

### Localization camera
- The `localization_tracker` node uses OpenCV to capture frames from a USB camera.
- It detects red and blue balls using HSV color segmentation.
- It converts ball pixel measurements into 3D coordinates using camera intrinsics (`fx`, `fy`, `cx`, `cy`).
- The two balls define the flower head geometry so the controller can compute a forward-facing tip pose.

### Face-tracker camera
- The `face_tracker` node reads from an attached camera and detects the closest face using RetinaFace.
- It publishes a minimal steering signal as pan/tilt error angles.
- This is useful for adding a second perception channel for attention-based interaction.

### Embedded hardware connection
- `micro_ros_agent` runs as a bridge node in `flower.launch.py`.
- It exposes host-side `RobotCommand` traffic to the ESP32/microcontroller transport layer.
- The controller receives the final merged `/flower_commands` message rather than raw GUI-only messages.

## File summary

### `flower_gui.py`
- Primary host control UI.
- Publishes manual and kinematic command streams.
- Saves GUI state to `~/.flower_gui_settings.json`.
- Uses `flower_msgs.msg.RobotCommand`.

### `flower_mux.py`
- Smart mode-dependent message multiplexer.
- Ensures kinematic mode keeps the GUI's latest LED and N20 state.
- Sends one coherent `RobotCommand` stream to hardware.

### `flower.launch.py`
- Host-side launch orchestration.
- Starts the GUI, the vision nodes, `micro_ros_agent`, and `foxglove_bridge` together.

### `src/flower_msgs/msg/RobotCommand.msg`
- Defines the host-to-hardware command schema.
- Ensures consistent message structure across GUI, mux, and firmware.

### `src/controller/controller/continuum_controller_node.py`
- Visible in source as the pose-aware motion controller.
- Infers flower head pose from color-ball vision input.
- Subscribes to `/kinematic_commands` and vision points.

### `src/ros2-opencv-localization/`
- Contains the camera-based localization tracker.
- Publishes `/vision/red_ball` and `/vision/blue_ball`.

### `src/ros2-opencv-face-tracker/`
- Contains the face tracker node.
- Publishes `/face_tracking_angles`.

## Notes

- The workspace assumes the host GUI and message packages are installed in a local Python path under `~/Desktop/llm4mentalhealth/FullCode_ws/install/`.
- If you move the repository, update the GUI import path and the launch file camera device settings accordingly.
- The root launch file currently starts the perception and control bridge nodes, but the embedded controller node may require a separate launch or firmware-side startup.


