ROS 2 integration overview for HiveFlight

This repo currently builds a plain CMake simulation.
For ROS 2 message bridging, we provide ROS 2 packages under ros2/:

- ros2/hiveflight_interfaces: msg/DroneMessage.msg
- ros2/hiveflight_sim: DroneRos2Adapter (publishes/subscribes DroneMessage)

Build with colcon from ros2/:
  colcon build

Then ensure you source the install/setup.*:
  . install/setup.bash

Note: SwarmSimulation3D integration wiring in main sim code is next.

