cmake_minimum_required(VERSION 3.8)
project(hiveflight_sim)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(hiveflight_interfaces REQUIRED)

# SwarmSimulation3D.cpp is in the repository root. Support both the source
# package layout and the copied ros2_ws/src layout.
set(SWARM_SIM_3D_CPP "${CMAKE_CURRENT_SOURCE_DIR}/../../SwarmSimulation3D.cpp")
if(NOT EXISTS "${SWARM_SIM_3D_CPP}")
  set(SWARM_SIM_3D_CPP "${CMAKE_CURRENT_SOURCE_DIR}/../../../SwarmSimulation3D.cpp")
endif()
if(NOT EXISTS "${SWARM_SIM_3D_CPP}")
  message(FATAL_ERROR "SwarmSimulation3D.cpp not found at: ${SWARM_SIM_3D_CPP}")
endif()



set(SPATIAL_GRID_3D_CPP "${CMAKE_CURRENT_SOURCE_DIR}/../../SpatialGrid3D.cpp")
if(NOT EXISTS "${SPATIAL_GRID_3D_CPP}")
  set(SPATIAL_GRID_3D_CPP "${CMAKE_CURRENT_SOURCE_DIR}/../../../SpatialGrid3D.cpp")
endif()
if(NOT EXISTS "${SPATIAL_GRID_3D_CPP}")
  message(FATAL_ERROR "SpatialGrid3D.cpp not found")
endif()


add_library(hiveflight_ros2_adapter
  src/DroneRos2Adapter.cpp
  src/DroneCommunication3D.cpp
  ${SWARM_SIM_3D_CPP}
  ${SPATIAL_GRID_3D_CPP}
)


target_include_directories(hiveflight_ros2_adapter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)




target_compile_definitions(hiveflight_ros2_adapter PUBLIC HIVEFLIGHT_USE_ROS2=1)

ament_target_dependencies(hiveflight_ros2_adapter PUBLIC
  rclcpp geometry_msgs hiveflight_interfaces
)

install(TARGETS hiveflight_ros2_adapter
  EXPORT hiveflight_ros2_adapterTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_targets(hiveflight_ros2_adapterTargets HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp geometry_msgs hiveflight_interfaces)

ament_package()

