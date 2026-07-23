from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable
import os

def generate_launch_description():
    
    gui_script_path = os.path.expanduser('~/Desktop/llm4mentalhealth/FullCode_ws/flower_gui.py')
    
    # 1. Build the local CUDA library paths
    home = os.path.expanduser('~')
    ta_dir = f"{home}/.local/lib/python3.10/site-packages/nvidia"
    cuda_libs = f"{ta_dir}/cuda_runtime/lib:{ta_dir}/cublas/lib:{ta_dir}/cudnn/lib:{ta_dir}/cufft/lib:{ta_dir}/curand/lib:{ta_dir}/cusolver/lib:{ta_dir}/cusparse/lib:{ta_dir}/nccl/lib:{ta_dir}/nvtx/lib"
    
    # 2. Prepend them to whatever LD_LIBRARY_PATH currently is
    current_ld_path = os.environ.get('LD_LIBRARY_PATH', '')
    custom_ld_path = f"{cuda_libs}:{current_ld_path}" if current_ld_path else cuda_libs

    return LaunchDescription([
        SetEnvironmentVariable(
            name='LD_LIBRARY_PATH',
            value=custom_ld_path
        ),
        
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
        ),
        #Node(
        #    package='face_tracker',
        #    executable='face_tracker',
        #    name='face_tracker',
        #    output='screen'
        #),
        #Node(
        #    package='localization_tracker',
        #    executable='localization_tracker',
        #    name='localization_tracker',
        #    output='screen'
        #)
    ])
