# HiveFlight: 2D vs 3D Simulation Comparison

## Project Evolution

HiveFlight began as a 2D drone swarm simulator and has been extended with a full 3D implementation using advanced spatial mathematics and isometric visualization.

## Component Comparison

### Vector Mathematics

| Aspect | 2D | 3D |
|--------|----|----|
| **Class** | `Vec2` | `Vec3` |
| **Coordinates** | x, z (no y) | x, y, z |
| **Location** | SwarmSimulation.hpp | Vec3.hpp |
| **Operations** | +, -, *, /, magnitude, normalize, limit | +, -, *, /, magnitude, normalize, limit, **cross product**, **dot product** |
| **Distance** | `(a - b).magnitude()` | `a.distance(b)` |
| **Operators** | Basic arithmetic | Basic + advanced |

**Vec3 Unique Methods:**
```cpp
Vec3 cross(const Vec3& o)      // Cross product for 3D rotations
double dot(const Vec3& o)      // Dot product for angles/alignment
double distance(const Vec3& o) // Euclidean distance
```

### Physics Engine

| Feature | 2D (SwarmSimulation) | 3D (SwarmSimulation3D) |
|---------|---------------------|------------------------|
| **Drones** | 25 (default) | 30 (default) |
| **World** | 800×600 (2D) | 200×200×150 (3D volume) |
| **Wrap-around** | Yes (toroidal) | No (hard boundaries) |
| **Boundaries** | Wrap on all axes | Clamp on all axes |
| **Spatial Grid** | Yes (optimized) | No (full O(n²)) |
| **Obstacles** | Circular (2D) | Spherical (3D) |
| **Target** | Circular orbit in XZ | 3D spiral orbit |

### Renderer

| Aspect | 2D | 3D |
|--------|----|----|
| **Display** | Top-down ASCII grid | Isometric ASCII projection |
| **Grid Size** | 80×30 | 100×40 |
| **Rendering** | Direct mapping | Isometric projection |
| **Symbols** | >^<v directions | >^<v + depth |
| **Export** | PPM images | OBJ, CSV |
| **Statistics** | Speed, battery, count | + Position, spread, center |

**Isometric Projection Formula:**
```cpp
screen_x = (world_x - world_z) * scale
screen_y = (world_y - (world_x + world_z) * 0.5) * scale
```

## Behavioral Differences

### Force Calculations

**2D (uses SpatialGrid for efficiency):**
```
Each force queries spatial grid -> O(1) average neighbor count
Total O(n) with grid optimization
```

**3D (brute force iteration):**
```
Each force iterates all drones
Total O(n²) per frame
```

### Movement Patterns

| Behavior | 2D | 3D |
|----------|----|----|
| **Separation** | Avoid XZ plane collisions | Avoid 3D sphere collisions |
| **Alignment** | 2D velocity matching | Full 3D velocity matching |
| **Cohesion** | Move to XZ center | Move to 3D center |
| **Target** | Circle in XZ plane | 3D spiral |
| **Bounds** | Wrap (continuous) | Clamp (hard walls) |

### Simulation Timings

**2D Configuration:**
- Drones: 25
- Steps: 200
- Timestep: 0.1s
- Total time: 20s simulation / ~2 min real

**3D Configuration:**
- Drones: 30
- Frames: 1800 (30 seconds at 60 FPS)
- Timestep: 0.016s
- Total time: 28.8s simulation / ~30 sec real

**Performance ratio:** 3D runs 4× faster per drone per unit time

## Structural Comparison

### 2D Code Organization
```
main.cpp (2D entry)
├── Config.hpp/cpp (parameters)
├── SwarmSimulation.hpp/cpp (physics with Vec2)
├── ConsoleRenderer.hpp (ASCII grid)
├── PpmRenderer.hpp/cpp (image output)
├── SpatialGrid.hpp/cpp (optimization)
└── build/drone_swarm
```

### 3D Code Organization
```
main_3d.cpp (3D entry)
├── Vec3.hpp (3D math)
├── SwarmSimulation3D.hpp/cpp (physics with Vec3)
├── Renderer3D.hpp/cpp (isometric rendering)
└── build/drone_swarm_3d
```

**Key Difference:** 3D is standalone, doesn't reuse 2D components

## Physics Engine Internals

### Force Weights (Configurable)

**2D Defaults:**
```
Separation: 1.8
Alignment:  1.0
Cohesion:   1.0
Target:     0.6
Obstacle:   2.5
```

**3D Defaults:**
```
Separation: 1.8
Alignment:  1.0
Cohesion:   1.0
Target:     0.8 (higher focus)
Obstacle:   2.5
```

### Perception Radii

**2D:**
- Separation: 50 units
- Alignment: 100 units
- Cohesion: 100 units

**3D:**
- Separation: 30 units (tighter)
- Alignment: 80 units (tighter)
- Cohesion: 80 units (tighter)

Ratios reflect 3D space larger volume

### Energy Model

**2D Battery Drain:**
```
health -= velocity.magnitude() * 0.002 * dt
```

**3D Battery Drain:**
```
health -= velocity.magnitude() * 0.001 * dt
```

3D drones have half the drain rate (longer battery life)

## Visualization Techniques

### 2D ASCII
```
Direct coordinate mapping to grid
grid[y][x] = drone_symbol
```

Example:
```
|  >  v  |
| *   #  |
|X   ^   |
```

### 3D Isometric
```
Project 3D → 2D using isometric transformation
grid[y][x] = projected_drone_symbol
```

Example (3D cube appears angled):
```
|   ^    |
|  * v   |
| <  >   |
```

## Data Export Capabilities

### 2D Export
- **PPM Images**: Frame-by-frame rendering
- **Console**: Real-time ASCII display
- Format: Portable pixmap image files

### 3D Export
- **OBJ Models**: 3D mesh geometry (18KB per export)
  - Spheres for obstacles
  - Points/spheres for drones
  - Star marker for target
- **CSV Data**: Drone states (50 bytes per drone)
  - Position, velocity, health, speed
- Compatible with Blender, MeshLab, Python, R

## Compilation Comparison

### 2D Build
```
CMake configuration
└── Compile:
    ├── main.cpp → SwarmSimulation (Vec2)
    ├── Config.cpp
    ├── PpmRenderer.cpp
    ├── SpatialGrid.cpp
    └── Link → drone_swarm (28 KB)
```

### 3D Build
```
CMake configuration
└── Compile:
    ├── main_3d.cpp → SwarmSimulation3D (Vec3)
    ├── Renderer3D.cpp
    └── Link → drone_swarm_3d (45 KB)
```

**Size difference:** 3D binary larger due to more complex rendering

## Runtime Behavior

### 2D Swarm Characteristics
- Compact clusters in X-Z plane
- Wrap-around maintains swarm density
- Predictable circular target pursuit
- Visible spatial grid optimization

### 3D Swarm Characteristics
- Volumetric distribution in X-Y-Z
- Swarm spreads into 3D space
- 3D spiral target pursuit
- Larger perceptual distances
- More natural 3D motion

## Parameter Adjustment Guide

### To Make Swarm Tighter (2D or 3D)
```cpp
weightSeparation = 1.0  // Was 1.8
weightCohesion = 2.0    // Was 1.0
```

### To Make Swarm Follow Target Better (3D)
```cpp
weightTarget = 1.5      // Was 0.8
```

### To Avoid Obstacles Better (3D)
```cpp
weightObstacle = 4.0    // Was 2.5
obstacleBuffer = 5.0    // Was 3.0
```

## Performance Scaling

### 2D (with spatial grid optimization)
- 25 drones: Linear scaling O(n)
- 50 drones: ~2× slower
- 100 drones: ~4× slower

### 3D (no spatial grid)
- 30 drones: O(n²)
- 60 drones: ~4× slower
- 100 drones: ~11× slower

**Optimization opportunity:** Add spatial partitioning to 3D

## Swarm Analysis Metrics

### Common to Both
- Active drone count
- Average battery level
- Average velocity
- Individual health

### 2D Only
- Toroidal wrap events
- Grid cell occupancy

### 3D Only
- Swarm center (3D point)
- Bounding box (X, Y, Z ranges)
- Swarm spread volume
- Depth range statistics
- Min/max health per axis

## Usage Scenarios

### Choose 2D When
- Analyzing planar dynamics
- Faster execution needed
- PPM video output desired
- 2D specific behaviors studied
- Educational simplicity

### Choose 3D When
- Studying volumetric swarms
- 3D visualization needed
- OBJ model export required
- Spatial distribution analysis
- Realistic 3D physics desired

## Extension Pathways

### 2D Enhancements
- Add 3D obstacles (2.5D approach)
- Implement Z-layer rendering
- Multi-color per Z-depth
- Advanced PPM animations

### 3D Enhancements
- **OpenGL rendering** (true 3D graphics)
- **Spatial partitioning** (octree/grid)
- **Multi-swarm support** (separate flocks)
- **Formation flying** (geometric constraints)
- **Physics simulation** (wind, currents)

## File Statistics

### 2D Codebase
```
main.cpp:           ~30 lines
SwarmSimulation.hpp: ~45 lines
SwarmSimulation.cpp: ~180 lines
ConsoleRenderer.hpp: ~110 lines
Total active code:  ~1500 lines
```

### 3D Codebase
```
main_3d.cpp:         ~90 lines
Vec3.hpp:            ~85 lines
SwarmSimulation3D.hpp: ~75 lines
SwarmSimulation3D.cpp: ~230 lines
Renderer3D.hpp/cpp:  ~320 lines
Total active code:   ~800 lines
```

## Summary Table

| Metric | 2D | 3D |
|--------|----|----|
| **Executable** | drone_swarm | drone_swarm_3d |
| **Binary Size** | 28 KB | 45 KB |
| **Drones** | 25 | 30 |
| **World Volume** | 480,000 (2D) | 6,000,000 (3D) |
| **Avg Speed** | 25-27 u/s | 19-20 u/s |
| **Runtime (default)** | ~2 minutes | ~30 seconds |
| **Spatial Grid** | Yes | No |
| **Export** | PPM images | OBJ, CSV |
| **Visualization** | Top-down grid | Isometric grid |
| **Code Complexity** | Medium | Medium |
| **Physics O(n)** | Optimized | Unoptimized |
