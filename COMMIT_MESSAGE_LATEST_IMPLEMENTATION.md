Latest changes — swarm splitting + target-following swarms

- Added per-drone target assignment (swarm splitting) in both 2D (SwarmSimulation) and 3D (SwarmSimulation3D)
- Drones periodically reassign to moving targets (reassignmentInterval, default 5s)
- Restricted boid neighborhood forces (separation/alignment/cohesion) to drones within the same assigned swarm
- Updated target seeking to follow the assigned target instead of the closest target
- Updated OpenGL 3D viewer to color each drone by its assigned target
- Build fix: removed duplicate SwarmSimulation3D::assignedTarget definition from SwarmSimulation3D.cpp (inline definition lives in SwarmSimulation3D.hpp)

