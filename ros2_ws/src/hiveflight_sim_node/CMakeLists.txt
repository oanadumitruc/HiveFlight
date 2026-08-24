cmake_minimum_required(VERSION 3.8)
project(hiveflight_sim_node)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(hiveflight_interfaces REQUIRED)
find_package(hiveflight_sim REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(gazebo_ros REQUIRED)
find_package(gazebo_msgs REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(simulation_node
  src/simulation_node.cpp
)

target_include_directories(simulation_node PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../hiveflight_sim/include>
)

ament_target_dependencies(simulation_node
  rclcpp
  hiveflight_interfaces
  hiveflight_sim
  geometry_msgs
  nav_msgs
  std_msgs
)

install(TARGETS simulation_node
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY models DESTINATION share/${PROJECT_NAME})
install(DIRECTORY worlds DESTINATION share/${PROJECT_NAME})

# Normalize CRLF -> LF for Python script
set_property(DIRECTORY APPEND PROPERTY
  CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/gazebo_swarm_bridge.py)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/scripts/gazebo_swarm_bridge.py BRIDGE_SOURCE)
string(REPLACE "\r\n" "\n" BRIDGE_SOURCE "${BRIDGE_SOURCE}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gazebo_swarm_bridge.py "${BRIDGE_SOURCE}")

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/gazebo_swarm_bridge.py
  DESTINATION lib/${PROJECT_NAME})

ament_package()
