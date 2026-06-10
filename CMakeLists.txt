cmake_minimum_required(VERSION 3.10)



project(DroneSwarm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
option(HIVEFLIGHT_BUILD_OPENGL_VIEWER "Build the interactive OpenGL swarm viewer" ON)

# 2D Simulation
add_executable(drone_swarm
    main.cpp
    Config.cpp
    SwarmSimulation.cpp
    PpmRenderer.cpp
    SpatialGrid.cpp
)

target_include_directories(drone_swarm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drone_swarm Threads::Threads)

# 3D Simulation
add_executable(drone_swarm_3d
    main_3d.cpp
    SwarmSimulation3D.cpp
    Renderer3D.cpp
    SpatialGrid3D.cpp
    DroneCommunication3D.cpp
)


target_include_directories(drone_swarm_3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(drone_swarm_3d Threads::Threads)

if(HIVEFLIGHT_BUILD_OPENGL_VIEWER)
    find_package(OpenGL)
    find_package(GLUT)

    if(OpenGL_FOUND AND GLUT_FOUND AND (OPENGL_GLU_FOUND OR TARGET OpenGL::GLU OR OPENGL_glu_LIBRARY))
        add_executable(hiveflight_gl_viewer
            main_opengl.cpp
            OpenGLSwarmViewer.cpp
            SwarmSimulation3D.cpp
            SpatialGrid3D.cpp
            DroneCommunication3D.cpp
        )

        target_include_directories(hiveflight_gl_viewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

        if(TARGET OpenGL::GL)
            target_link_libraries(hiveflight_gl_viewer PRIVATE OpenGL::GL)
            if(TARGET OpenGL::GLU)
                target_link_libraries(hiveflight_gl_viewer PRIVATE OpenGL::GLU)
            elseif(OPENGL_glu_LIBRARY)
                target_link_libraries(hiveflight_gl_viewer PRIVATE ${OPENGL_glu_LIBRARY})
            endif()
        else()
            target_link_libraries(hiveflight_gl_viewer PRIVATE ${OPENGL_LIBRARIES})
        endif()

        if(TARGET GLUT::GLUT)
            target_link_libraries(hiveflight_gl_viewer PRIVATE GLUT::GLUT)
        else()
            target_include_directories(hiveflight_gl_viewer PRIVATE ${GLUT_INCLUDE_DIRS})
            target_link_libraries(hiveflight_gl_viewer PRIVATE ${GLUT_LIBRARIES})
        endif()

        target_link_libraries(hiveflight_gl_viewer PRIVATE Threads::Threads)
    else()
        message(STATUS "OpenGL/GLU/GLUT not found; skipping hiveflight_gl_viewer target")
    endif()
endif()
