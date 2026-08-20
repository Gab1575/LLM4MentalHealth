"""flower.launch.py - Host-side launch description for the flower robot.

Brings up the full host stack in one command: the micro-ROS agent (hardware
bridge), Foxglove bridge (debugging/visualization), the operator GUI, the two
perception nodes, the command mux, and the kinematic controller. See the
FullCode_ws README for how these nodes connect via ROS topics.
"""

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable
import os

def generate_launch_description():

    # 1. Build the local CUDA library paths.
    # RetinaFace (used by face_tracker) pulls in GPU-accelerated ops whose
    # shared libraries live under pip's nvidia-* packages rather than the
    # system library path, so they have to be added explicitly or the node
    # fails to import RetinaFace at runtime.
    home = os.path.expanduser('~')
    ta_dir = f"{home}/.local/lib/python3.10/site-packages/nvidia"
    cuda_libs = f"{ta_dir}/cuda_runtime/lib:{ta_dir}/cublas/lib:{ta_dir}/cudnn/lib:{ta_dir}/cufft/lib:{ta_dir}/curand/lib:{ta_dir}/cusolver/lib:{ta_dir}/cusparse/lib:{ta_dir}/nccl/lib:{ta_dir}/nvtx/lib"

    # 2. Prepend them to whatever LD_LIBRARY_PATH currently is, so nothing
    # the parent shell already set is lost.
    current_ld_path = os.environ.get('LD_LIBRARY_PATH', '')
    custom_ld_path = f"{cuda_libs}:{current_ld_path}" if current_ld_path else cuda_libs

    return LaunchDescription([
        # Applied to every node launched below, since SetEnvironmentVariable
        # affects the whole launch context.
        SetEnvironmentVariable(
            name='LD_LIBRARY_PATH',
            value=custom_ld_path
        ),

        # Hardware bridge: forwards /flower_commands to the ESP32 over UDP.
        Node(
            package='micro_ros_agent',
            executable='micro_ros_agent',
            name='micro_ros_agent',
            arguments=['udp4', '--port', '8888'],
            output='screen'
        ),
        # Debugging/visualization: exposes ROS 2 topics to Foxglove Studio.
        Node(
            package='foxglove_bridge',
            executable='foxglove_bridge',
            name='foxglove_bridge',
            output='screen'
        ),
        # Operator dashboard: publishes /manual_commands, /kinematic_commands, /control_mode.
        Node(
            package='flower_gui',
            executable='flower_gui',
            name='flower_gui',
            output='screen'
        ),
        # Perception: publishes /face_tracking_angles.
        Node(
            package='face_tracker',
            executable='face_tracker',
            name='face_tracker',
            output='screen'
        ),
        # Perception: publishes /vision/red_ball and /vision/blue_ball.
        Node(
            package='localization_tracker',
            executable='localization_tracker',
            name='localization_tracker',
            output='screen'
        ),
        # Merges manual/kinematic sources into the final /flower_commands stream.
        Node(
            package='flower_mux',
            executable='flower_mux',
            name='flower_mux',
            output='screen'
        ),
        # Kinematic solver: /kinematic_commands -> /kinematic_calculated_commands.
        Node(
            package='controller',
            executable='continuum_controller_node',
            name='continuum_controller',
            output='screen'
        )
    ])
