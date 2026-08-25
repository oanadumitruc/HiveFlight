# TODO - HiveFlight Gazebo swarm fixes

- [x] Update `ros2_ws/src/hiveflight_sim_node/models/drone_marker/model.sdf` to a realistic, non-static quadcopter-style model (remove 2m sphere).

- [x] Fix `ros2_ws/src/hiveflight_sim_node/launch/hiveflight.launch.py` to remove the hardcoded Windows/Linux path and build the script path dynamically from package share.

- [x] Update `ros2_ws/src/hiveflight_sim_node/CMakeLists.txt` to install the Python bridge script (and ensure it’s runnable / line endings handled) and update launch accordingly.

- [x] Improve diagnostics in `ros2_ws/src/hiveflight_sim_node/scripts/gazebo_swarm_bridge.py` (throttled service-ready warnings + empty-PoseArray guards — an empty *first* PoseArray previously set `drone_spawn_active` and wedged spawning until restart).

- [x] Demo scenery in `ros2_ws/src/hiveflight_sim_node/worlds/hiveflight.world`: country road with markings/power lines/signs, wheat + plowed + crop fields, hay bales, barn/silo, parked pickup/van/tractor, wind turbines, cinematic SW camera.

- [x] Build-blocker fixed: Gazebo Classic 11 cannot convert `<cone>` geometry (`msgs.cc: "Unknown geometry type"` → `Failed to load the World` → gzserver exit 255 → black GUI, no `/spawn_entity`). All 30 cones in `hiveflight.world` (pine tiers) and 2 in `target_marker/model.sdf` (landing beams) replaced with cylinders; world now loads headless with both plugins active.

- [x] **Crash fix**: `hiveflight_pose_plugin.cpp` read `assignments_` (an `std::unordered_map`) from the physics thread while the ROS executor thread wrote it — an unsynchronised data race that corrupted the heap and segfaulted gzserver (surfacing inside `libdraco.so`, exit -11, ~1–2 min after spawn). The unused colour-remap was removed along with the race. **Verified headless end-to-end: 20 drones + 1 target spawned, `gazebo_drones=20 gazebo_targets=1`, simulation ran 460+ sim-seconds with zero new segfaults (dmesg delta 0), server alive through the whole 210 s soak.**

- [x] Build and test: `colcon build` and run the launch; verified `/spawn_entity` availability and drones move (headless E2E). GUI run on WSLg pending user confirmation — if the window alone is sluggish, `export LIBGL_ALWAYS_SOFTWARE=1`.

