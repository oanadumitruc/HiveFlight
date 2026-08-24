# HiveFlight — Architecture & Implementation Notes

This document describes how the HiveFlight project is structured and how its main simulation/visualization components work in practice.

---

## 1) High-level architecture

HiveFlight provides:

- **2D swarm simulation** (Reynolds Boids variant)
- **3D swarm simulation** (acceleration-based Reynolds Boids in a bounded 3D volume)
- **Visualization** in two forms:
  - Console **ASCII** renderers (2D + 3D)
  - Optional **interactive OpenGL** viewer for 3D
- **Export** for 3D simulation results:
  - **OBJ** for geometry visualization
  - **CSV** for analysis

At a high level, the pipeline is:

1. `Config` (or `SwarmConfig3D`) defines world + swarm parameters
2. `SwarmSimulation` / `SwarmSimulation3D` updates drone states each tick
3. A renderer (console/OBJ/CSV/OpenGL) consumes the updated state

---

## 2) Build / targets (CMake)

The root CMake file (`HiveFlight/CMakeLists.txt`) builds these executables:

- `drone_swarm` (2D)
- `drone_swarm_3d` (3D console + export)
- `hiveflight_gl_viewer` (optional, interactive OpenGL)

OpenGL viewer is guarded by:

- `option(HIVEFLIGHT_BUILD_OPENGL_VIEWER ... ON)`
- presence of OpenGL + GLUT (+ optional GLU)

---

## 3) Source layout

Key files:

### 2D simulation
- `main.cpp`
- `Config.cpp/.hpp`
- `SwarmSimulation.cpp/.hpp`
- `SpatialGrid.cpp/.hpp` (neighbor query optimization)
- `PpmRenderer.cpp/.hpp` (optional image output)
- `ConsoleRenderer.hpp` (ASCII visualization)

### 3D simulation
- `main_3d.cpp`
- `SwarmSimulation3D.cpp/.hpp` (3D physics)
- `Renderer3D.cpp/.hpp` (ASCII + OBJ + CSV + stats)
- `Vec3.hpp` (3D math)

### 3D OpenGL visualization (optional)
- `main_opengl.cpp`
- `OpenGLSwarmViewer.cpp/.hpp` (OpenGL window + rendering loop)

---

## 4) Configuration system

### 2D (`SimConfig` via `Config.hpp/Config.cpp`)

The 2D simulation uses a config struct (`SimConfig`) loaded from a `.conf` file and/or overridden by command line.

Conceptually it includes:
- world dimensions
- drone count
- dt, maxSpeed, maxForce
- perception radii
- behavior weights
- rendering options

### 3D (`SwarmConfig3D` via `SwarmSimulation3D.hpp`)

The 3D config is `SwarmConfig3D` (declared in `SwarmSimulation3D.hpp`) and includes:

- **World**: `worldWidth`, `worldHeight`, `worldDepth`
- **Swarm init**: `droneCount`, `seed`, `posMin/posMax`, `velMin/velMax`
- **Physics**: `dt`, `maxSpeed`, `maxForce`
- **Boids radii**: `separationRadius`, `alignmentRadius`, `cohesionRadius`
- **Behavior weights**: `weightSeparation`, `weightAlignment`, `weightCohesion`, `weightTarget`, `weightObstacle`
- **Energy**: `batteryDrainRate`
- **Obstacles**: `obstacleBuffer`
- **Rendering**: `renderTarget`, `showVelocityVectors`

Priority rules (matching the project docs):
- **Arguments > File > Defaults**

---

## 5) 2D implementation (core ideas)

### 5.1 SwarmSimulation (2D)

`SwarmSimulation` manages:
- `m_drones` (position/velocity/acceleration/health)
- `m_obstacles` (position/radius)
- `m_target` and `updateTarget()`
- force calculation + integration in `step()`

Behavior is a Reynolds Boids style weighted sum (separation/alignment/cohesion/seek/obstacle avoidance).

### 5.2 SpatialGrid optimization

Neighbor forces in vanilla Boids are expensive (`O(n^2)`).

`SpatialGrid` partitions the 2D world into cells so each drone only checks nearby drones.

This makes force calculation closer to `O(n)` average.

### 5.3 Rendering loop

The 2D main loop (in `main.cpp`) repeatedly:
1. render console and (optionally) PPM
2. call `sim.step()`
3. sleep a configured delay

---

## 6) 3D implementation (core ideas)

### 6.1 Data structures

In `SwarmSimulation3D.hpp`:

- `Drone3D`
  - `id`
  - `position`, `velocity`, `acceleration` (`Vec3`)
  - `health` battery level
  - `radius` for collision margin

- `Obstacle3D`
  - `position` (`Vec3`)
  - `radius`

- `SwarmConfig3D`
  - world + physics + behavior weights + dt + export/render flags

### 6.2 SwarmSimulation3D flow

In `SwarmSimulation3D.cpp`:

#### `reset()`
- initializes drones:
  - random positions within `[posMin, posMax]`
  - random velocities within `[velMin, velMax]`
  - random depth within `[30, worldDepth-30]`
- initializes 3 static obstacles as spheres
- sets initial target at the world center

#### `step()`
Core update per tick:

1. `updateTarget()` — moves the target in a 3D spiral-like pattern (uses `m_time`)
2. advance time: `m_time += dt`
3. for each drone:
   - if `health <= 0`, keep it inert
   - compute forces:
     - `separationForce(i)`
     - `alignmentForce(i)`
     - `cohesionForce(i)`
     - `seekTarget(i)`
     - `avoidObstacles(i)`
   - weighted acceleration sum:

     `acc += separation * weightSeparation + alignment * weightAlignment + cohesion * weightCohesion + seek * weightTarget + avoid * weightObstacle`

   - limit acceleration: `acc.limit(maxForce)`
   - explicit Euler integration:
     - `vel = curVel + acc * dt`
     - clamp `vel.limit(maxSpeed)`
     - `pos = curPos + vel * dt`
     - clamp to hard world boundaries
   - battery/health depletion:

     `health -= |vel| * batteryDrainRate * dt`

4. apply next state vectors back to `m_drones`
5. increment `m_tick`

#### `separationForce(i)` / `alignmentForce(i)` / `cohesionForce(i)`
All are neighbor loops (`O(n^2)`), using radii filters based on Euclidean distance.

- **separation**: pushes away from close neighbors (distance-weighted)
- **alignment**: steers towards average neighbor velocity
- **cohesion**: steers towards average neighbor position (then uses `seek(center, ...)`)

#### `seekTarget(i)`
Steers each drone toward the current target position.

#### `avoidObstacles(i)`
Repels from obstacle spheres using:

- safe distance = `obstacle.radius + obstacleBuffer`
- if drone is inside safe distance, add a push proportional to penetration depth

### 6.3 World bounds

3D simulation uses **hard boundaries** (clamping position if it exits the volume). No wrap.

---

## 7) 3D rendering & export

### 7.1 Renderer3D

`Renderer3D` (in `Renderer3D.hpp/.cpp`) is responsible for:

- console 3D ASCII projection (`printConsole`)
- isometric projection (`project3D`)
- printing runtime statistics (`printStats`)
- exporting:
  - OBJ via `exportOBJ`
  - CSV via `exportCSV`

The OBJ exporter is intended to produce a mesh-like representation of:
- obstacles (spheres)
- drone markers (small spheres)
- target marker

### 7.2 Exports

`drone_swarm_3d` supports:
- `--export obj <file>`
- `--export csv <file>`

---

## 8) OpenGL viewer (optional interactive 3D)

### 8.1 Why it exists

Console rendering provides an ASCII approximation.

The OpenGL viewer provides:
- interactive camera (yaw/pitch/zoom)
- 3D rendering of world box/grid/obstacles/drones/target

### 8.2 Entry point and viewer class

- Entry: `main_opengl.cpp`
- Main class: `OpenGLSwarmViewer`

`main_opengl.cpp` parses a small CLI subset (e.g. `--drones`, `--seed`, `--world`, `--vectors`) and constructs `OpenGLSwarmViewer`.

> Diagram update note: this OpenGL viewer is the optional “true 3D visualization” complementing the console ASCII renderers and the OBJ/CSV exporters.


### 8.3 Responsibilities of OpenGLSwarmViewer

In `OpenGLSwarmViewer.hpp`, `OpenGLSwarmViewer`:
- owns `SwarmSimulation3D m_sim`
- maintains camera parameters (`m_yaw`, `m_pitch`, `m_distance`)
- maintains rendering/step pacing (`m_stepsPerFrame`, paused/velocity vectors)
- defines rendering helpers:
  - `drawWorldBox()`, `drawGrid()`, `drawObstacles()`, `drawTarget()`, `drawDrones()`
  - geometry primitives: `drawSphere()`, `drawLine()`
  - overlay text: `drawOverlay()`, `drawText()`

It runs an OpenGL loop via GLUT callbacks:
- `displayCallback`, `reshapeCallback`
- `keyboardCallback`, `specialCallback`
- `mouseCallback`, `motionCallback`
- `timerCallback`

### 8.4 Simulation → OpenGL data flow

The viewer:
1. advances `SwarmSimulation3D` based on timer frames
2. reads `m_sim.drones()`, `m_sim.obstacles()`, `m_sim.target()`
3. draws the scene

---

## 9) Extending the project

Common extension points:

- **Performance (3D)**: replace `O(n^2)` neighbor loops with spatial partitioning (octree/grid)
- **Rendering**: richer obstacle/drone geometry, GPU acceleration, better materials
- **Physics**: wind fields, dynamic obstacles, more realistic actuator/drag models
- **Analysis**: additional exports (trajectories, energy curves), formation quality metrics

---

## 10) Quick component map

| Functionality | 2D | 3D | OpenGL |
|---|---|---|---|
| Simulation engine | `SwarmSimulation` | `SwarmSimulation3D` | (uses `SwarmSimulation3D`) |
| Neighbor acceleration | `SpatialGrid` | (not yet) | (not yet) |
| Console render | `ConsoleRenderer.hpp` | `Renderer3D::printConsole` | (no) |
| Export | (PPM optional) | `Renderer3D::exportOBJ/exportCSV` | (no) |
| Interactive 3D | (none) | (ASCII only) | `OpenGLSwarmViewer` |

---

## 11) Architecture diagram (simulation → render/export → viewer)

```mermaid
flowchart TD
  subgraph Input[Inputs]
    A1[CLI args] --> B0[Config]
    A2[.conf file] --> B0
  end

  B0 --> C2D[SwarmSimulation (2D)]
  B0 --> C3D[SwarmSimulation3D (3D)]

  %% 2D rendering
  C2D --> D2[ConsoleRenderer / ASCII (2D)]
  C2D --> E2[PpmRenderer (optional PPM)]

  %% 3D rendering / export
  C3D --> D3[Renderer3D: ASCII (isometric)]
  C3D --> E3O[Renderer3D: exportOBJ (.obj)]
  C3D --> E3C[Renderer3D: exportCSV (.csv)]

  %% OpenGL viewer
  C3D --> G0[OpenGLSwarmViewer (optional interactive)
main_opengl.cpp + OpenGLSwarmViewer]

  %% edges (explicit)
  G0 --> W[Window / interactive camera]
```

---

_End of document._


