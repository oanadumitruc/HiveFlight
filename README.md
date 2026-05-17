# HiveFlight
 [ Demo video](output.mp4)

https://github.com/oanadumitruc/HiveFlight/blob/main/output.mp4

## Drone Swarm Simulation

This is a simple simulation of a swarm drones using basic flocking algorithms (separation, alignment, cohesion) in C++.

## Features

- drones moving in a 2D space (800x600 units)
- Flocking behavior: drones try to stay close but not too close, align velocities, and move towards the center of mass
- Console output showing positions and velocities at each step
- Wrap-around boundaries

## Prerequisites

- CMake 3.10 or higher
- C++ compiler (g++/clang++ on Linux, MinGW or MSVC on Windows)
- Git Bash or WSL (for Linux builds on Windows)

## Building
### Linux (Native or WSL)

#### Option 1: Using the build script
```bash
cd {your project folder}
./build_linux.sh
```

#### Option 2: Manual commands
```bash
cd {your project folder}
mkdir -p build
cd build
cmake ..
make
```

### Windows (using MinGW)

#### Option 1: Using the build script
```cmd
cd {your project folder}
build_windows.bat
```

#### Option 2: Manual commands
```cmd
cd {your project folder}
if not exist build mkdir build
cd build
cmake ..
mingw32-make
```

## Running

### Linux
```bash
cd {your project folder}/HiveFlight/build
./drone_swarm
```

### Windows
```cmd
cd {your project folder}/HiveFlight\build
drone_swarm.exe
```

Or simply double-click `drone_swarm.exe` in the build folder.

---
### Example of command parameters for rendering
```
rm -rf frames_many && mkdir -p frames_many

./drone_swarm \
  --steps 300 \
  --dt 0.1 \
  --droneCount 20 \
  --renderPpm true \
  --renderEvery 1 \
  --renderFramesDir frames_many \
  --renderImageW 800 \
  --renderImageH 600
```

Create the video
```
ffmpeg -framerate 30 -i frames_many/frame_%05d.ppm -c:v libx264 -pix_fmt yuv420p swarm.mp4
```

## Algorithm Details

Each drone follows three rules:

1. **Separation**: Steer to avoid crowding local flockmates
2. **Alignment**: Steer towards the average heading of neighbors
3. **Cohesion**: Steer to move toward the average position of neighbors

These forces are combined and applied to update the drone's velocity and position.
