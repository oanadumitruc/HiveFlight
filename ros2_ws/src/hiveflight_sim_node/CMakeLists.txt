cmake_minimum_required(VERSION 3.8)
project(hiveflight_sim_node)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(hiveflight_interfaces REQUIRED)
find_package(hiveflight_sim REQUIRED)
find_package(gazebo_ros REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(simulation_node
  src/simulation_node.cpp
)

target_include_directories(simulation_node PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../hiveflight_sim/include>
)

ament_target_dependencies(simulation_node
  rclcpp hiveflight_interfaces hiveflight_sim
  std_msgs
)

install(TARGETS simulation_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY models DESTINATION share/${PROJECT_NAME})
install(DIRECTORY worlds DESTINATION share/${PROJECT_NAME})

# The repository lives on a Windows filesystem, so source files may carry CRLF
# line endings. A CRLF shebang ("#!/usr/bin/env python3\r") makes the bridge
# fail to start under Linux ("env: 'python3\r': No such file or directory"),
# which silently leaves the Gazebo world empty. Normalize to LF at install time
# so the deployed script is always executable regardless of editor settings.
# CMAKE_CONFIGURE_DEPENDS forces a reconfigure whenever the script changes,
# otherwise edits to the .py would NOT be picked up (stale copy installed).
set_property(DIRECTORY APPEND PROPERTY
  CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/gazebo_swarm_bridge.py)
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/scripts/gazebo_swarm_bridge.py BRIDGE_SOURCE)
string(REPLACE "\r\n" "\n" BRIDGE_SOURCE "${BRIDGE_SOURCE}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gazebo_swarm_bridge.py "${BRIDGE_SOURCE}")
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/gazebo_swarm_bridge.py DESTINATION lib/${PROJECT_NAME})


ament_package()
