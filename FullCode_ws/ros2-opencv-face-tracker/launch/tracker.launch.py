from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='usb_cam',
            executable='usb_cam_node_exe',
            name='usb_cam',
            parameters=[
                {'video_device': '/dev/video2'}, # The confirmed video capture node
                {'image_width': 640},            # The hardware's preferred 30fps width
                {'image_height': 480},           # The hardware's preferred 30fps height
                {'framerate': 30.0}
            ]
        ),
        
        Node(
            package='localization_tracker',
            executable='localization_tracker',
            name='localization_tracker_node'
        )
    ])
