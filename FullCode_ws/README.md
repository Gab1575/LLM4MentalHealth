# For Changes
## 1. Build the custom messages
colcon build --packages-select flower_messages

## 2. Source the workspace so the terminal knows they exist
source install/local_setup.bash

## 3. Rebuild the Agent so it links against the newly sourced messages
ros2 run micro_ros_setup build_agent.sh

## 4. Source one last time
source install/local_setup.bash

## 5. Run the program
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888

### SHORTCUT
colcon build --packages-select flower_messages && source install/local_setup.bash && ros2 run micro_ros_setup build_agent.sh && source install/local_setup.bash && ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888


## FOXGLOVE BRIDGE

source /opt/ros/humble/setup.bash
source ~/microros_ws/install/local_setup.bash

ros2 run foxglove_bridge foxglove_bridge