# HiveFlight ROS 2 and Gazebo Setup

## 1. Supported Environment

The current integration targets:

- Windows host
- Ubuntu 22.04 in WSL 2
- ROS 2 Humble
- Gazebo Classic 11 with `gazebo_ros_pkgs`
- `colcon`
- C++17 compiler and CMake
- Python 3 with ROS 2 Python packages

Gazebo Classic reached end of life in January 2025. It is retained here because the current bridge uses Gazebo Classic services such as `/spawn_entity` and `/gazebo/set_entity_state`.

## 2. Workspace and Source of Truth

Always build and launch from:

```bash
/mnt/c/work/ubuntu_work/HiveFlight/ros2_ws
```

The repository has an older `ros2/` integration copy. Do not source, build, or launch that copy for the current runtime. The active package sources are:

```text
ros2_ws/src/hiveflight_interfaces
ros2_ws/src/hiveflight_sim
ros2_ws/src/hiveflight_sim_node
```

The algorithm sources are shared from the repository root by `hiveflight_sim/CMakeLists.txt`:

```text
SwarmSimulation3D.cpp
SwarmSimulation3D.hpp
SpatialGrid3D.cpp
SpatialGrid3D.hpp
Vec3.hpp
```

Do not delete or move these files without updating the package CMake file.

## 3. Required ROS 2 Packages

Install or verify these packages in Ubuntu 22.04:

```bash
sudo apt update
sudo apt install -y \
  ros-humble-desktop \
  ros-humble-gazebo-ros-pkgs \
  ros-humble-geometry-msgs \
  ros-humble-std-msgs \
  python3-colcon-common-extensions \
  build-essential \
  cmake \
  python3
```

The project uses these ROS dependencies:

| Dependency | Used by | Purpose |
|---|---|---|
| `ament_cmake` | all packages | ROS 2 CMake build system |
| `rclcpp` | `hiveflight_sim`, `hiveflight_sim_node` | C++ ROS 2 node and adapter |
| `rclpy` | bridge runtime | Python ROS 2 bridge |
| `geometry_msgs` | interfaces and node | `Vector3`, `PoseArray` |
| `std_msgs` | simulation node and bridge | target assignment array |
| `hiveflight_interfaces` | adapter and node | generated `DroneMessage` |
| `gazebo_ros` | node package and launch | Gazebo Classic integration and launch |
| `gazebo_msgs` | bridge runtime | spawn and model/entity state services |
| `ament_index_python` | bridge and launch | resolve installed package share paths |
| `launch`, `launch_ros` | launch file | compose Gazebo, simulation, and bridge |
| `rosidl_default_generators` | interfaces | generate C/C++/Python message support |

## 4. Environment Initialization

Every new WSL terminal should run:

```bash
source /opt/ros/humble/setup.bash
source /mnt/c/work/ubuntu_work/HiveFlight/ros2_ws/install/setup.bash
```

Optional convenience function:

```bash
function hiveflight_env() {
  source /opt/ros/humble/setup.bash
  source /mnt/c/work/ubuntu_work/HiveFlight/ros2_ws/install/setup.bash
}
```

ROS 2 discovery must use the same domain in every terminal:

```bash
export ROS_DOMAIN_ID=0
```

For a WSL/Windows GUI, Gazebo Classic must be able to connect to the display. With WSLg this is normally automatic. Check:

```bash
echo "$DISPLAY"
echo "$WAYLAND_DISPLAY"
```

## 5. The `hf` CLI (recommended)

The repo ships a convenience script, `hf`, that wraps all common workflow commands. It is available at the repo root and in `ros2_ws/`:

```bash
# one-time setup: alias it so it works from anywhere
echo "alias hf='/mnt/c/work/ubuntu_work/HiveFlight/hf'" >> ~/.bashrc
source ~/.bashrc
```

| Command | What it does |
|---|---|
| `hf kill` | Kill leftover `gzserver` / `gzclient` / `simulation_node` processes |
| `hf build` | Clean rebuild of `ros2_ws` (`rm -rf build install log` + `colcon build --symlink-install --merge-install`) |
| `hf run [args...]` | Source the workspace and launch `hiveflight.launch.py` (extra args passed through) |
| `hf all` | `kill` + `build` + `run` in one shot |
| `hf hz` | Check publish rate of `/hiveflight/swarm_0/drone_poses` |

Examples:

```bash
hf kill
hf build
hf run drone_count:=20 target_count:=3 gui:=true
hf all                      # full cycle: kill -> build -> run
```

> **Note:** ROS Humble lives in the **Ubuntu-22.04** WSL distro. If your default distro differs, run commands via `wsl -d Ubuntu-22.04` or set it as default with `wsl --set-default Ubuntu-22.04`.

The manual equivalents of each command are documented below.

## 6. Clean Build (manual)

Stop old processes before rebuilding:

```bash
pkill -x gzserver 2>/dev/null || true
pkill -x gzclient 2>/dev/null || true
pkill -x simulation_node 2>/dev/null || true
```

A clean build avoids merged/isolated install conflicts and stale bridge scripts:

```bash
cd /mnt/c/work/ubuntu_work/HiveFlight/ros2_ws
rm -rf build install log
source /opt/ros/humble/setup.bash
colcon build --symlink-install --merge-install --event-handlers console_direct+
source install/setup.bash
```

The `--merge-install` option is required when the existing install directory was created with merged layout.

## 7. Launch the Full System

Default runtime:

```bash
ros2 launch hiveflight_sim_node hiveflight.launch.py
```

Defaults:

```text
drone_count=20
target_count=1
gui=true
```

Override values without editing code:

```bash
ros2 launch hiveflight_sim_node hiveflight.launch.py \
  drone_count:=20 \
  target_count:=3 \
  gui:=true
```

The launch file starts:

1. Gazebo Classic with `hiveflight.world`.
2. `simulation_node`.
3. The installed `gazebo_swarm_bridge.py`.

The world loads `libgazebo_ros_state.so`, and the bridge uses `/spawn_entity` plus `/gazebo/set_model_state` or `/gazebo/set_entity_state`.

## 8. Verify Startup

Check nodes:

```bash
ros2 node list
```

Expected:

```text
/gazebo
/gazebo_ros_state
/hiveflight_sim_node
/gazebo_swarm_bridge
```

Check services:

```bash
ros2 service list | grep -E "spawn_entity|set_model_state|set_entity_state"
```

At minimum:

```text
/spawn_entity
/gazebo/set_entity_state
```

Some installations also expose:

```text
/gazebo/set_model_state
```

Check topics:

```bash
ros2 topic list | grep hiveflight
```

Expected:

```text
/hiveflight/swarm_0/communication
/hiveflight/swarm_0/drone_poses
/hiveflight/swarm_0/target_poses
/hiveflight/swarm_0/target_assignments
```

Check pose rates:

```bash
ros2 topic hz /hiveflight/swarm_0/drone_poses
ros2 topic hz /hiveflight/swarm_0/target_poses
```

The simulation should publish near 60 Hz unless the machine is overloaded.

Check actual Gazebo models:

```bash
ros2 topic echo /gazebo/model_states
```

Expected names:

```text
ground_plane
drone_0 ... drone_19
target_0
```

## 8. Bridge Health Output

The bridge logs model paths, subscribed topics, spawn requests, spawn results, and health state. Healthy output includes:

```text
Spawning 20 colored quadcopters (sequential)
Requesting Gazebo spawn: drone_0
Drone spawned: drone_0
All quadcopters spawned; live updates enabled
Spawning 1 colored targets (sequential)
Target spawned: target_0
All targets spawned; live updates enabled
```

The periodic health line should eventually show:

```text
drones_topic=True
targets_topic=True
spawn_service=True
drone_ready=True
target_ready=True
gazebo_drones=20
gazebo_targets=1
```

## 9. Manual Service Tests

Test model-state control when available:

```bash
ros2 service call /gazebo/set_model_state gazebo_msgs/srv/SetModelState \
"{model_state: {model_name: 'drone_0', pose: {position: {x: 20.0, y: 20.0, z: 20.0}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}, twist: {linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}, reference_frame: 'world'}}"
```

Test entity-state control when that is the available API:

```bash
ros2 service call /gazebo/set_entity_state gazebo_msgs/srv/SetEntityState \
"{state: {name: 'drone_0', pose: {position: {x: 20.0, y: 20.0, z: 20.0}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}, reference_frame: 'world'}}"
```

If the command waits forever, the service is only a stale ROS graph entry or belongs to another Gazebo process. Kill all Gazebo processes and launch the full system again.

## 10. Stale Build and Clock Skew

When building from a Windows-mounted filesystem, generated files can appear newer than the current WSL clock:

```text
Clock skew detected
File has modification time ... in the future
```

This can preserve stale install artifacts. Use:

```bash
cd /mnt/c/work/ubuntu_work/HiveFlight/ros2_ws
find src -type f -exec touch {} +
rm -rf build install log
source /opt/ros/humble/setup.bash
colcon build --symlink-install --merge-install --event-handlers console_direct+
source install/setup.bash
```

Verify the installed bridge, not only the source file:

```bash
grep -n "Gazebo bridge\|SetModelState\|SetEntityState" \
  install/lib/hiveflight_sim_node/gazebo_swarm_bridge.py
```

## 11. Troubleshooting Matrix

| Observation | Diagnosis | Action |
|---|---|---|
| Only `ground_plane` in `/gazebo/model_states` | Bridge did not spawn models | Check bridge process and `/spawn_entity`; clean rebuild |
| No bridge startup output | Bridge executable/import failed | Check launch output; verify `rclpy`, `ament_index_python`, and installed script |
| `/spawn_entity` absent | Gazebo factory plugin missing | Start through the package launch file and verify `gazebo_ros_factory` |
| `drones_topic=False` | Simulation node or namespace mismatch | Check `ros2 topic list` and source the workspace |
| `target_poses` has no messages | Wrong/stale simulation executable | Rebuild and verify node startup count |
| Spawn fails with duplicate name | Old Gazebo instance still owns models | Kill all Gazebo processes and relaunch |
| State service waits forever | Service graph is stale or wrong domain | Check `ROS_DOMAIN_ID`, nodes, and service type |
| Bridge says model missing | State update raced model creation | Wait for spawn completion; rebuild current bridge |
| Old sphere/plane remains | Existing model was not recreated | Restart Gazebo after rebuilding |
| Gazebo GUI is blank but models exist | Camera is not framed on world objects | Use Gazebo camera reset/focus and inspect `/gazebo/model_states` |
| Simulation ticks but GUI is jerky | Gazebo service/update load is high | Reduce counts, retain latest-only QoS, inspect CPU/FPS |

## 12. Reproducible Smoke Test

Run in terminal 1:

```bash
cd /mnt/c/work/ubuntu_work/HiveFlight/ros2_ws
source /opt/ros/humble/setup.bash
source install/setup.bash
ros2 launch hiveflight_sim_node hiveflight.launch.py drone_count:=2 target_count:=1
```

Run in terminal 2:

```bash
source /opt/ros/humble/setup.bash
source /mnt/c/work/ubuntu_work/HiveFlight/ros2_ws/install/setup.bash
ros2 topic hz /hiveflight/swarm_0/drone_poses
```

Run in terminal 3:

```bash
source /opt/ros/humble/setup.bash
source /mnt/c/work/ubuntu_work/HiveFlight/ros2_ws/install/setup.bash
ros2 topic echo /gazebo/model_states
```

Pass criteria:

- The bridge starts without Python exceptions.
- Two drones and one target appear in `model_states`.
- Pose topics publish continuously.
- Gazebo model names remain present.
- Positions change between samples.
- No `does not exist` or spawn response errors appear.
