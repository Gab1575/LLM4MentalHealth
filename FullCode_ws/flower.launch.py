from launch import LaunchDescription
from launch_ros.actions import Node
import os

def generate_launch_description():
    
    gui_script_path = os.path.expanduser('~/Desktop/llm4mentalhealth/FullCode_ws/flower_gui.py')

    return LaunchDescription([
        Node(
            package='micro_ros_agent',
            executable='micro_ros_agent',
            name='micro_ros_agent',
            arguments=['udp4', '--port', '8888'],
            output='screen'
        ),
        Node(
            package='foxglove_bridge',
            executable='foxglove_bridge',
            name='foxglove_bridge',
            output='screen'
        ),
        Node(
            executable='python3',
            arguments=[gui_script_path],
            name='flower_gui',
            output='screen'
        )
    ])