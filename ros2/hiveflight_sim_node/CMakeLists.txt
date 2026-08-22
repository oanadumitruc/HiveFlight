cmake_minimum_required(VERSION 3.8)
project(hiveflight_sim_node)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(hiveflight_interfaces REQUIRED)
find_package(hiveflight_sim REQUIRED)

add_executable(simulation_node
  src/simulation_node.cpp
)

target_include_directories(simulation_node PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../hiveflight_sim/include>
)

ament_target_dependencies(simulation_node
  rclcpp hiveflight_interfaces hiveflight_sim
)

install(TARGETS simulation_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

# NOTE: Do NOT install the Python bridge script.
# The repository files are stored on a Windows filesystem and may contain CRLF line endings,
# which can cause Exec format errors when ROS2 tries to run the installed copy.
# We run the bridge from the source tree instead.


ament_package()


