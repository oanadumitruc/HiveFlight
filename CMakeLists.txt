cmake_minimum_required(VERSION 3.10)

project(DroneSwarm)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

add_executable(drone_swarm drone_swarm.cpp)

target_link_libraries(drone_swarm Threads::Threads)
