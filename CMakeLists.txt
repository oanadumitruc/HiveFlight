cmake_minimum_required(VERSION 3.10)
project(hiveflight)

#
# --- COMMON SETTINGS ---
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

#
# --- ROS 2 DEPENDENCIES ---
#
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(hiveflight_interfaces REQUIRED)

#
# --- OLD DRONESWARM IMPLEMENTATION (2D + 3D) ---
#
add_executable(drone_swarm
    main.cpp
    Config.cpp
    SwarmSimulation.cpp
    PpmRenderer.cpp
    SpatialGrid.cpp
)
target_include_directories(drone_swarm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drone_swarm PRIVATE Threads::Threads)

add_executable(drone_swarm_3d
    main_3d.cpp
    SwarmSimulation3D.cpp
    Renderer3D.cpp
    SpatialGrid3D.cpp
    DroneCommunication3D.cpp
)
target_include_directories(drone_swarm_3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drone_swarm_3d PRIVATE Threads::Threads)

#
# --- OPTIONAL OPENGL VIEWER ---
#
option(HIVEFLIGHT_BUILD_OPENGL_VIEWER "Build the interactive OpenGL swarm viewer" ON)

if(HIVEFLIGHT_BUILD_OPENGL_VIEWER)
    find_package(OpenGL)
    find_package(GLUT)

    if(OpenGL_FOUND AND GLUT_FOUND)
        add_executable(hiveflight_gl_viewer
            main_opengl.cpp
            OpenGLSwarmViewer.cpp
            SwarmSimulation3D.cpp
            SpatialGrid3D.cpp
            DroneCommunication3D.cpp
        )

        target_include_directories(hiveflight_gl_viewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(hiveflight_gl_viewer PRIVATE Threads::Threads)

        if(TARGET OpenGL::GL)
            target_link_libraries(hiveflight_gl_viewer PRIVATE OpenGL::GL)
        endif()

        if(TARGET OpenGL::GLU)
            target_link_libraries(hiveflight_gl_viewer PRIVATE OpenGL::GLU)
        endif()

        if(TARGET GLUT::GLUT)
            target_link_libraries(hiveflight_gl_viewer PRIVATE GLUT::GLUT)
        endif()
    else()
        message(STATUS "OpenGL/GLUT not found; skipping hiveflight_gl_viewer")
    endif()
endif()

#
# --- ROS 2 SIMULATION NODE ---
#
add_executable(simulation_node
    src/simulation_node.cpp
)
ament_target_dependencies(simulation_node
    rclcpp
    geometry_msgs
    hiveflight_interfaces
)

install(TARGETS simulation_node
    DESTINATION lib/${PROJECT_NAME}
)

#
# --- INSTALL LAUNCH FILES ---
#
install(
    DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
)

#
# --- EXPORT ROS 2 PACKAGE ---
#
ament_package()
