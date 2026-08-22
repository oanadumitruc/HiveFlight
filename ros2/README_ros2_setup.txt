This directory contains the ROS 2 integration scaffolding for HiveFlight.

Note: The project currently builds with plain CMake (no ROS 2). To enable ROS 2 message generation
and rclcpp nodes, you will need an installed ROS 2 distribution and build tooling.

Next steps once code is added:
- Create a minimal ROS 2 package (or integrate into existing CMake) with:
  - package.xml
  - CMakeLists.txt using ament_cmake
  - msg/DroneMessage.msg
- Build with: colcon build

