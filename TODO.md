# TODO - HiveFlight Gazebo swarm fixes

- [x] Update `ros2_ws/src/hiveflight_sim_node/models/drone_marker/model.sdf` to a realistic, non-static quadcopter-style model (remove 2m sphere).

- [x] Fix `ros2_ws/src/hiveflight_sim_node/launch/hiveflight.launch.py` to remove the hardcoded Windows/Linux path and build the script path dynamically from package share.

- [x] Update `ros2_ws/src/hiveflight_sim_node/CMakeLists.txt` to install the Python bridge script (and ensure it’s runnable / line endings handled) and update launch accordingly.

- [ ] Improve diagnostics in `ros2_ws/src/hiveflight_sim_node/scripts/gazebo_swarm_bridge.py` (warn when services are not ready, throttle warnings, warn on empty PoseArray).
- [ ] Build and test: `colcon build` and run the launch; verify `/spawn_entity` and `/gazebo/set_entity_state` availability and drones move.

