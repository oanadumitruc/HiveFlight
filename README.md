# HiveFlight 🐝

**HiveFlight** is a drone swarm simulation platform written in modern **C++17**. It implements the
[Reynolds Boids](https://en.wikipedia.org/wiki/Boids) flocking algorithm in both 2D and 3D, with
multiple visualization front-ends (ASCII console, interactive OpenGL) and a full **ROS 2 + Gazebo**
integration for realistic multi-drone simulation.

## Core Capabilities

- **Up to 30+ autonomous drones** in 3D space (200×200×150 world)
- **Reynolds Boids algorithm** with 5 weighted behaviors (separation, alignment, cohesion, target seeking, obstacle avoidance)
- **Battery management** with velocity-based energy drain
- **60 FPS simulation** with spatial-grid neighbor acceleration (O(n))
- **Multiple renderers**: 2D ASCII, 3D isometric ASCII, interactive OpenGL viewer, PPM frames
- **Data export**: OBJ 3D models, CSV per-drone telemetry
- **ROS 2 Humble + Gazebo Classic 11 integration** — spawn drones in Gazebo and drive them from the same swarm engine

## Repository Structure

```
HiveFlight/
├── CMakeLists.txt              # Root build config (2D, 3D, OpenGL targets)
├── build_linux.sh / .bat       # Convenience build scripts
├── swarm_demo.conf             # Sample configuration file
│
├── Core sources (repo root)
│   ├── main.cpp / drone_swarm.cpp        # 2D entry points
│   ├── main_3d.cpp                       # 3D console entry point
│   ├── main_opengl.cpp                   # OpenGL viewer entry point
│   ├── SwarmSimulation.*                 # 2D physics engine
│   ├── SwarmSimulation3D.*               # 3D physics engine
│   ├── SpatialGrid.* / SpatialGrid3D.*   # Neighbor acceleration
│   ├── Vec2.hpp / Vec3.hpp               # Math primitives
│   ├── Config.*                          # Configuration system
│   ├── ConsoleRenderer.hpp / Renderer3D.*# ASCII renderers + OBJ/CSV export
│   ├── PpmRenderer.*                     # PPM frame export
│   └── OpenGLSwarmViewer.*               # Interactive OpenGL viewer
│
├── docs/                       # All project documentation (see docs/README.md)
├── ros2_ws/                    # ACTIVE ROS 2 workspace (source of truth)
│   └── src/
│       ├── hiveflight_interfaces/   # Custom DroneMessage.msg
│       ├── hiveflight_sim/          # ROS 2 adapter library around the sim core
│       └── hiveflight_sim_node/     # Simulation node, Gazebo world plugin, launch file, Gazebo bridge
├── ros2/                       # LEGACY ROS 2 copy — do not build this one
└── build/, install/, log/      # Build artifacts (generated)
```

## Quick Start

### Standalone (no ROS required)

Prerequisites: **C++17 compiler** (g++ / clang / MSVC / MinGW), **CMake ≥ 3.10**.

```bash
cd HiveFlight
bash build_linux.sh          # Linux / WSL
build_windows.bat            # Windows (MinGW)

# Run the 2D simulation
./build/drone_swarm --config swarm_demo.conf

# Run the 3D simulation
./build/drone_swarm_3d --drones 30 --seed 42

# Export results
./build/drone_swarm_3d --export obj output.obj
./build/drone_swarm_3d --export csv drones.csv
```

Optional interactive 3D viewer (needs OpenGL + GLUT):

```bash
sudo apt install freeglut3-dev libglu1-mesa-dev   # Debian/Ubuntu
./build/hiveflight_gl_viewer --drones 60 --seed 7
```

Viewer controls: mouse drag = orbit · wheel = zoom · Space = pause · R = reset · V = velocity vectors · +/- speed · Q/Esc = quit.

### ROS 2 + Gazebo mode

Target environment: Windows host + **Ubuntu 22.04 (WSL 2)**, **ROS 2 Humble**, **Gazebo Classic 11**.

```bash
# Install dependencies (Ubuntu 22.04)
sudo apt install -y ros-humble-desktop ros-humble-gazebo-ros-pkgs \
  python3-colcon-common-extensions build-essential cmake

# Build the workspace — or use the hf CLI (recommended)
cd ros2_ws
./hf build

# Launch Gazebo + simulation node + Gazebo bridge
./hf run drone_count:=20 target_count:=1 gui:=true
```

The `hf` CLI wraps the whole workflow:

| Command | Action |
|---|---|
| `hf kill` | Kill leftover `gzserver` / `gzclient` / `simulation_node` |
| `hf build` | Clean rebuild (`colcon build --symlink-install --merge-install`) |
| `hf run [args...]` | Launch `hiveflight.launch.py` (args passed through) |
| `hf all` | kill → build → run in one shot |
| `hf hz` | Check publish rate of the drone poses topic |

Manual equivalent:

```bash
cd ros2_ws
source /opt/ros/humble/setup.bash
colcon build --symlink-install --merge-install
source install/setup.bash
ros2 launch hiveflight_sim_node hiveflight.launch.py drone_count:=20 target_count:=1 gui:=true
```

Verify: `hf hz` should report near 60 Hz.

Motion feel is tunable at launch time (no recompile needed):

```bash
./hf run sim_speed:=2.0 max_force:=24.0 target_speed_multiplier:=6.0
```

| Parameter | Default | Effect |
|---|---|---|
| `sim_speed` | 2.0 | Global time scale — simulation advances N× faster in wall time |
| `max_force` | 24.0 | Steering aggressiveness — higher = sharper turns, less "floating" |
| `target_speed_multiplier` | 6.0 | Target orbital speed — higher pulls the swarm faster |
| `use_plugin` | true | Poses applied by the Gazebo world plugin on the physics thread (smooth); false = legacy service round-robin |

### Demo scene

The launch loads `hiveflight.world`, a countryside airfield ready for demos:
a **country road** with dashed markings, power lines and parked traffic runs
along the south edge; around it sit a golden **wheat field**, a **plowed
field**, a green **crop field**, hay bales, a red **barn with silo**, a
tractor and two **wind turbines**. All tall scenery lives *outside* the
200×200 m flight volume, so the swarm never clips through it. Drones fly a
compact 60 m ceiling and Gazebo opens **pre-framed on the swarm** from a
zoomed-out south-west establishing shot — no zooming needed. There is no
distance fog, so the scene stays crisp at any zoom level. Drones are
rendered at **3× scale** (~2.7 m rotor span) to stay visible at that
distance.

Full setup, verification and troubleshooting: [docs/ROS2_SETUP.md](docs/ROS2_SETUP.md).

## Dependencies

| Component | Requirement | Notes |
|---|---|---|
| Compiler | C++17 (g++, clang, MSVC, MinGW) | Required |
| CMake | ≥ 3.10 | Required |
| STL only | — | Standalone sim has no external libs |
| freeglut + OpenGL (+ GLU) | optional | Only for `hiveflight_gl_viewer` |
| ROS 2 Humble desktop | optional | For the Gazebo integration |
| gazebo_ros_pkgs (Gazebo Classic 11) | optional | Spawn/state services used by the bridge |
| colcon, Python 3 | optional | ROS 2 workspace build & Python bridge |

## Documentation

All documentation lives in [`docs/`](docs/README.md). Start there for quick starts,
architecture deep-dives, the ROS 2 integration guide, performance data, and the roadmap.

## License

See [LICENSE](LICENSE).
