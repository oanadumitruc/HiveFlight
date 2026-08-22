import os

from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    gazebo_launch = os.path.join(
        get_package_share_directory('gazebo_ros'),
        'launch',
        'gazebo.launch.py',
    )

    return LaunchDescription([
        DeclareLaunchArgument('gui', default_value='true'),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gazebo_launch),
            launch_arguments={'gui': LaunchConfiguration('gui')}.items(),
        ),
        Node(
            package='hiveflight_sim_node',
            executable='simulation_node',
            name='hiveflight_sim_node',
            output='screen',
        ),
        # Run the bridge from the source-tree python file to avoid executing the (possibly CRLF-corrupted)
        # installed copy in ros2_ws/install/lib/...
        Node(
            package='hiveflight_sim_node',
            executable='/usr/bin/python3',
            name='gazebo_swarm_bridge',
            output='screen',
            arguments=[
                '/mnt/c/work/ubuntu_work/HiveFlight/ros2/hiveflight_sim_node/scripts/gazebo_swarm_bridge.py',
            ],
        ),
    ])


