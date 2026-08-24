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
        # When the Gazebo world plugin (libhiveflight_pose_plugin.so) applies
        # poses on the physics thread, the Python bridge only needs to spawn
        # models — its per-model service updates are disabled.
        DeclareLaunchArgument('use_plugin', default_value='true'),
        # Motion feel: raise sim_speed for faster motion; max_force controls
        # how aggressively drones turn (higher = less "floating").
        DeclareLaunchArgument('sim_speed', default_value='2.0'),
        DeclareLaunchArgument('max_force', default_value='24.0'),
        DeclareLaunchArgument('target_speed_multiplier', default_value='6.0'),
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
                'use_plugin': ParameterValue(LaunchConfiguration('use_plugin'), value_type=bool),
                'sim_speed': ParameterValue(LaunchConfiguration('sim_speed'), value_type=float),
                'max_force': ParameterValue(LaunchConfiguration('max_force'), value_type=float),
                'target_speed_multiplier': ParameterValue(LaunchConfiguration('target_speed_multiplier'), value_type=float),
            }],
        ),
    ])
