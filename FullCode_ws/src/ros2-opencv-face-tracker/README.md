# ROS 2 OpenCV Face Tracker: Zero-Latency Edge Perception 

![ROS 2](https://img.shields.io/badge/ROS_2-Humble-22314E?logo=ros)
![OpenCV](https://img.shields.io/badge/OpenCV-Python-5C3EE8?logo=opencv)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A hyper-optimized, real-time computer vision node for ROS 2 that achieves zero-latency face tracking by bypassing standard middleware serialization bottlenecks.

## 📖 Project Overview

In standard ROS 2 perception pipelines, relying on generic camera drivers and `CvBridge` introduces severe frame-rate drops on standard edge hardware (often sub-1 FPS). This is caused by the heavy network serialization overhead of translating massive raw image matrices into ROS network messages and back again.

This package re-architects the perception pipeline into a lightweight, all-in-one edge node. By utilizing OpenCV's direct hardware interface (`cv2.VideoCapture`) to pull frames straight into the processing script, this node keeps heavy image matrices entirely off the ROS local network. The result is flawless, real-time bounding box tracking that leaves maximum CPU overhead available for downstream robotic control.

## ✨ Key Features

* **Zero-Latency Processing:** Bypasses `usb_cam` and `CvBridge` serialization, reading the hardware stream directly.
* **Optimized Haar Cascades:** Fine-tuned detection parameters (`scaleFactor=1.3`, `minNeighbors=5`) to maximize frames per second without sacrificing accuracy.
* **Dynamic Contrast Adjustment:** Integrates CLAHE (Contrast Limited Adaptive Histogram Equalization) to maintain robust tracking in poor or uneven lighting conditions.
* **Native GUI Rendering:** Utilizes lightweight `cv2.imshow` native windows instead of heavy ROS debugging GUIs (like `rqt`) to eliminate rendering bottlenecks.

## ⚙️ System Requirements

* **OS:** Ubuntu 22.04 LTS
* **Middleware:** ROS 2 Humble Hawksbill
* **Dependencies:** Python 3, OpenCV (`python3-opencv`), `cv_bridge`, `sensor_msgs`
* **Hardware:** Any standard USB Web Camera (tested on `/dev/video*`)

## 🛠️ Installation & Build Instructions

1. **Clone the repository into your ROS 2 workspace:**
   ```bash
   cd ~/your_ws/src
   git clone [https://github.com/abinaabey2006/ros2-opencv-face-tracker.git](https://github.com/abinaabey2006/ros2-opencv-face-tracker.git)

    Navigate to the workspace root and resolve dependencies:
    Bash

    cd ~/your_ws
    rosdep install --from-paths src --ignore-src -r -y

    Build the package:
    Bash

    colcon build --packages-select face_tracker

    Source the workspace:
    Bash

    source install/setup.bash

🚀 Usage

To launch the real-time tracking node, ensure your USB camera is plugged in and run:
Bash

ros2 run face_tracker face_tracker

A native OpenCV window will open displaying the real-time camera feed with a tracking bounding box.

    Note: To cleanly exit the node and release the camera hardware, press Ctrl+C in the terminal.

🔮 Future Scope: Autonomous Motion Control

The current architecture successfully isolates and tracks targets at the edge. The immediate next step for this project is to publish the (x, y) center coordinates of the detected bounding box as lightweight geometry_msgs/Point or geometry_msgs/Pose data.

This will act as the foundational perception data to drive autonomous motion control systems—allowing physical platforms (like a TurtleBot 3 in Gazebo) to execute trajectory adjustments to keep the target dynamically centered in its field of view.

Built by Abina Abey.
