import os

from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    package_share = get_package_share_directory('hiveflight_sim_node')
    gazebo_launch = os.path.join(
        get_package_share_directory('gazebo_ros'),
        'launch',
        'gazebo.launch.py',
    )

    return LaunchDescription([
        DeclareLaunchArgument('gui', default_value='true'),
        DeclareLaunchArgument('drone_count', default_value='20'),
        DeclareLaunchArgument('target_count', default_value='1'),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gazebo_launch),
            launch_arguments={
                'gui': LaunchConfiguration('gui'),
                'world': os.path.join(package_share, 'worlds', 'hiveflight.world'),
                'pause': 'false',
            }.items(),
        ),
        Node(
            package='hiveflight_sim_node',
            executable='simulation_node',
            name='hiveflight_sim_node',
            output='screen',
            parameters=[{
                'drone_count': ParameterValue(LaunchConfiguration('drone_count'), value_type=int),
                'target_count': ParameterValue(LaunchConfiguration('target_count'), value_type=int),
            }],
        ),
        # Invoke python3 explicitly instead of relying on the script's shebang:
        # the repo lives on a Windows filesystem, so the installed copy may carry
        # CRLF endings and "#!/usr/bin/env python3\r" fails to exec under Linux.
        Node(
            package='hiveflight_sim_node',
            executable='/usr/bin/python3',
            name='gazebo_swarm_bridge',
            output='screen',
            emulate_tty=True,
            arguments=[
                os.path.join(package_share, '..', '..', 'lib', 'hiveflight_sim_node', 'gazebo_swarm_bridge.py'),
            ],
            parameters=[{
                'drone_count': ParameterValue(LaunchConfiguration('drone_count'), value_type=int),
                'target_count': ParameterValue(LaunchConfiguration('target_count'), value_type=int),
            }],
        ),
    ])
