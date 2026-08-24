# 3D Drone Swarm Simulation

## Overview

The 3D drone swarm simulation extends the HiveFlight project into three-dimensional space with advanced physics modeling, isometric projection visualization, and export capabilities.

## Features

### Core Capabilities
- **30 autonomous drones** in 3D space (200×200×150 world)
- **Reynolds Boids Algorithm** with 5 weighted behaviors in 3D
- **Obstacle avoidance** with spherical obstacles
- **Target seeking** with 3D spiral movement pattern
- **Battery management** with velocity-based energy drain
- **60 FPS simulation** for smooth dynamics

### Visualization
- **Isometric projection** for 3D-to-2D console display
- **Real-time ASCII rendering** (100×40 character display)
- **Depth tracking** showing swarm Z-coordinate range
- **Bounding box analysis** for swarm spatial distribution
- **Multiple export formats**: OBJ (3D model), CSV (data analysis)

### Physics Model
- **3D vector mathematics** with cross products, dot products
- **Acceleration-based integration** with force limiting
- **Realistic drone dynamics** with inertia and momentum
- **Hard world boundaries** preventing escape from simulation volume
- **Collision detection** between drones and obstacles

## Building

### Prerequisites
- C++17 compiler (g++, clang, or MSVC)
- CMake 3.10+
- Standard C++ library

### Build Commands

**Linux/WSL:**
```bash
cd HiveFlight
bash build_linux.sh
```

**Windows (with MinGW):**
```cmd
cd HiveFlight
build_windows.bat
```

**Manual Build:**
```bash
mkdir -p build
cd build
cmake ..
make
```

### Build Output
- `build/drone_swarm` - 2D simulation executable
- `build/drone_swarm_3d` - 3D simulation executable

## Running Simulations

### Basic 3D Simulation
```bash
./drone_swarm_3d
```
Default: 30 drones, ~30 seconds (1800 frames at 60 FPS)

### Custom Configuration
```bash
./drone_swarm_3d --drones 50 --seed 42
```

### With Export
```bash
./drone_swarm_3d --drones 30 --export obj output.obj
./drone_swarm_3d --drones 30 --export csv drones.csv
```

### Command-Line Options
```
--drones N          Number of drones (default: 30)
--seed S            Random seed for reproducibility (default: 42)
--export FORMAT     Export format: 'obj' or 'csv'
FILE                Output filename for export
--help              Display help message
```

## Understanding the Output

### Console Display

The isometric projection creates a 3D visual effect in 2D:
```
+------+
|  ^   |   ^ = drone moving up/away
|X *   |   * = drone (neutral/speed-dependent)
| >v   |   > < = drone moving left/right
| ##   |   X = target
| ### |   # = obstacle
|  #   |
+------+
```

**Statistics Line:**
```
Step: 100 | Active: 30/30 | Health: 99.8% | Speed: 18.5 | Depth: [62.1, 142.8]
```

### Final Statistics Report

```
=== 3D SWARM STATISTICS (Tick=1800) ===
  Active Drones      : 30/30
  Avg Speed          : 19.157 u/s
  Battery [min/max]  : 99.4% / 99.5%
  Swarm Center       : (168.2, 77.2, 100.6)
  Bounding Box       : X[125.7, 200.0] Y[34.0, 121.2] Z[63.4, 144.0]
  Swarm Spread       : 74 x 87 x 81
  Obstacles          : 3
  Simulation Time    : 28.80 s
```

**Key Metrics:**
- **Swarm Center**: Centroid of all active drones
- **Bounding Box**: Min/max coordinates for each axis
- **Swarm Spread**: Total volume drones occupy
- **Depth**: Z-coordinate range (3D distance perception)

## Physics Parameters

### Default Configuration (SwarmConfig3D)

```cpp
// World dimensions
worldWidth = 200.0
worldHeight = 200.0
worldDepth = 150.0

// Swarm parameters
droneCount = 30
seed = 42

// Force weights (Reynolds Boids)
weightSeparation = 1.8     // Avoid crowding
weightAlignment = 1.0      // Match velocities
weightCohesion = 1.0       // Move to center
weightTarget = 0.8         // Seek target
weightObstacle = 2.5       // Avoid obstacles

// Limits
maxSpeed = 40.0           // Speed limit
maxForce = 12.0           // Acceleration limit

// Perception radii
separationRadius = 30.0
alignmentRadius = 80.0
cohesionRadius = 80.0

// Battery
batteryDrainRate = 0.001

// Obstacle buffer
obstacleBuffer = 3.0
```

## Export Formats

### OBJ Format (3D Model)

```bash
./drone_swarm_3d --export obj final_frame.obj
```

Creates a Wavefront OBJ file containing:
- Spherical mesh representations of obstacles
- Small spheres for each active drone
- Star/cross marker for target position

Compatible with:
- Blender
- Maya
- 3D viewers (MeshLab, Viewer)
- Game engines

### CSV Format (Data Analysis)

```bash
./drone_swarm_3d --export csv drones.csv
```

Exports drone state with columns:
- ID: Drone identifier
- X, Y, Z: Position coordinates
- VelX, VelY, VelZ: Velocity components
- Health: Battery level (0-100)
- Speed: Velocity magnitude

Use for:
- Data analysis in Python/R
- Statistical analysis
- Post-processing visualization
- Custom visualization tools

## Simulation Dynamics

### Behavior Forces (3D Extensions)

1. **Separation Force**
   - Prevents drone crowding
   - Repulsion within 30 unit radius
   - Distance-weighted magnitude

2. **Alignment Force**
   - Drones align velocities with neighbors
   - Perception radius: 80 units
   - Creates coordinated movement

3. **Cohesion Force**
   - Attraction to neighbor swarm center
   - Keeps group together
   - Prevents fragmentation

4. **Target Seeking Force**
   - Continuous attraction to target
   - 3D spiral movement pattern (period: ~20 seconds)
   - Overrides other behaviors when weighted high

5. **Obstacle Avoidance Force**
   - Repulsion from spherical obstacles
   - Strongest force weight (2.5)
   - Safety buffer: 3.0 units

### Integration Scheme

Standard explicit Euler integration:
```
acceleration = weighted_forces.limit(maxForce)
velocity += acceleration * dt
velocity = velocity.limit(maxSpeed)
position += velocity * dt
position = clamp(position, world_bounds)
health -= velocity.magnitude() * drainRate * dt
```

### Timestep
- Fixed at dt = 0.016 seconds (60 FPS)
- 1800 frames = 28.8 seconds real simulation time
- Real execution time varies with CPU

## 3D-to-2D Projection

### Isometric Projection

Used for console visualization:
```
screen_x = (world_x - world_z) * scale
screen_y = (world_y - (world_x + world_z) * 0.5) * scale
```

**Properties:**
- Maintains depth perception
- Shows 3D orientation intuitively
- Works well in fixed-size console
- No perspective distortion

## Performance Characteristics

### Time Complexity
- Force calculation: O(n²) average, O(n) with spatial optimization
- Position update: O(n)
- Rendering: O(grid_width × grid_height)
- Overall: O(n) per frame with optimizations

### Space Complexity
- Drone states: O(n)
- Obstacle list: O(m)
- Rendering grid: O(constant, ~4000 chars)

### Typical Performance
- 30 drones × 1800 frames: ~30 seconds
- 50 drones × 1800 frames: ~60 seconds
- 100 drones × 1800 frames: ~180 seconds

## Comparison: 2D vs 3D

| Aspect | 2D | 3D |
|--------|----|----|
| Dimensions | 2 (X, Z only) | 3 (X, Y, Z) |
| Grid size | 80×30 | 100×40 |
| Perception | Radius | Sphere |
| Movement | Planar | Volumetric |
| Speed | ~30 sec (25 drones) | ~30 sec (30 drones) |
| Complexity | O(n²) → O(n) | O(n²) (no spatial grid) |

## Troubleshooting

**Simulation runs too fast:**
- Increase `sleepMs` parameter (rendering delay)
- Reduce `droneCount`

**Drones escape bounds:**
- Decrease `maxSpeed`
- Increase `weightObstacle`

**Swarm too dispersed:**
- Increase `weightCohesion`
- Decrease `separationRadius`

**Export file too large:**
- Reduce sphere mesh quality (edit `Renderer3D.cpp`)
- Use CSV export instead of OBJ

## Future Enhancements

1. **OpenGL Rendering** - Full 3D graphics with rotation/zoom
2. **Dynamic obstacles** - Real-time obstacle spawning/removal
3. **Multi-swarm** - Multiple independent swarms
4. **Communication topology** - Network visualization
5. **Realistic physics** - Wind simulation, acceleration limits
6. **Performance optimization** - Spatial partitioning with octree
7. **Statistical analysis** - Energy metrics, formation quality
8. **Interactive control** - Mouse/keyboard manipulation

## File Structure

```
HiveFlight/
├── Vec3.hpp              # 3D vector mathematics
├── SwarmSimulation3D.hpp # 3D physics engine
├── SwarmSimulation3D.cpp # Physics implementation
├── Renderer3D.hpp        # 3D visualization
├── Renderer3D.cpp        # Renderer implementation
├── main_3d.cpp           # 3D entry point
├── CMakeLists.txt        # Build configuration (updated)
└── build/
    └── drone_swarm_3d    # 3D executable
```

## References

- [Reynolds Boids Algorithm](http://www.red3d.com/cwr/boids/) - Original boid behavior design
- [Isometric Projection](https://en.wikipedia.org/wiki/Isometric_projection) - 3D-to-2D visualization
- [Euler Integration](https://en.wikipedia.org/wiki/Euler_method) - Numerical physics
- [Collision Detection](https://en.wikipedia.org/wiki/Collision_detection) - Sphere collision
