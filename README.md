# HiveFlight
# HiveFlight

## Drone Swarm Simulation

This is a simple simulation of a swarm of 5 drones using basic flocking algorithms (separation, alignment, cohesion) in C++.

## Features

- 5 drones moving in a 2D space (800x600 units)
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

#### Option 3: Using CMake GUI
1. Open CMake GUI
2. Set source path to `{your project folder}`
3. Set build path to `{your project folder}`
4. Click "Configure" and select your compiler (MinGW or Visual Studio)
5. Click "Generate"
6. Build using `mingw32-make` or open the generated `.sln` file in Visual Studio

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

The simulation will run for 100 steps, printing the state of each drone every 0.5 seconds.

## Algorithm Details

Each drone follows three rules:

1. **Separation**: Steer to avoid crowding local flockmates
2. **Alignment**: Steer towards the average heading of neighbors
3. **Cohesion**: Steer to move toward the average position of neighbors

These forces are combined and applied to update the drone's velocity and position.
