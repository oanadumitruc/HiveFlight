# HiveFlight Architecture Overview

## Project Structure

```
HiveFlight/
├── CMakeLists.txt              # Build configuration
├── build_linux.sh              # Linux/WSL build script
├── build_windows.bat           # Windows build script
├── swarm_demo.conf             # Demo configuration
│
├── Source Files
├── main.cpp                    # Entry point
├── Config.cpp/hpp              # Configuration management
├── SwarmSimulation.cpp/hpp     # Core physics engine
├── ConsoleRenderer.hpp         # ASCII visualization
├── PpmRenderer.cpp/hpp         # Image rendering (optional)
├── SpatialGrid.cpp/hpp         # Spatial partitioning for optimization
│
└── Documentation
    ├── README.md               # Original project overview
    ├── UPDATES.md              # Enhancement documentation
    ├── SIMULATION_RESULTS.md   # Run results and metrics
    └── QUICKSTART.md           # Quick reference guide
```

## Core Components

### 1. SwarmSimulation Engine

**File**: `SwarmSimulation.cpp/hpp`

**Responsibilities**:
- Manage drone states (position, velocity, acceleration, health)
- Calculate physics forces (separation, alignment, cohesion)
- Implement target seeking behavior
- Handle obstacle avoidance
- Track battery consumption
- Update simulation state each timestep

**Key Methods**:
- `step()` - Advance simulation by one timestep
- `separationForce()` - Collision avoidance calculation
- `alignmentForce()` - Velocity alignment with neighbors
- `cohesionForce()` - Attraction to neighbor center
- `seekTarget()` - Movement toward target
- `avoidObstacles()` - Repulsion from obstacles

### 2. Configuration System

**File**: `Config.cpp/hpp`

**Responsibilities**:
- Load configuration from files or command-line arguments
- Store simulation parameters
- Provide defaults for all settings
- Parse key-value configuration syntax

**Key Features**:
- File-based configuration with `.conf` format
- Command-line parameter overrides
- Default values for all parameters
- Comments and inline comments support

### 3. Console Renderer

**File**: `ConsoleRenderer.hpp`

**Responsibilities**:
- Render swarm state as ASCII grid
- Display drone positions with directional symbols
- Show obstacle boundaries
- Mark target location
- Output real-time statistics

**Features**:
- Grid-based visualization (80×30 characters default)
- Directional drone symbols: `>`, `<`, `^`, `v`
- Obstacle rendering with `#` markers
- Target marker with `X`
- Real-time battery and speed statistics

### 4. Spatial Grid Optimization

**File**: `SpatialGrid.cpp/hpp`

**Responsibilities**:
- Organize drones into spatial cells
- Efficient neighbor queries
- Reduce force calculation complexity from O(n²) to O(n)

**Algorithm**:
- Hash-grid implementation
- Cell-based neighbor iteration
- Wrap-around handling for toroidal worlds

## Physics Model

### Force Calculation

Each drone experiences a combination of forces:

```
totalForce = sep*w_sep + ali*w_ali + coh*w_coh + seek*w_seek + avoid*w_avoid
```

Where:
- **sep** = Separation force (avoid crowding)
- **ali** = Alignment force (match neighbor velocities)
- **coh** = Cohesion force (move toward neighbors)
- **seek** = Target seeking force
- **avoid** = Obstacle avoidance force

### Integration

Standard Euler integration with force limiting:

```cpp
acceleration = totalForce.limit(maxForce)
velocity = (velocity + acceleration * dt).limit(maxSpeed)
position = position + velocity * dt
```

### Battery System

Energy depletion based on movement:

```cpp
health -= velocity.magnitude() * batteryDrainRate * dt
```

## Data Flow

```
┌─────────────────────────────────────┐
│   Configuration (swarm_demo.conf)   │
└────────────┬────────────────────────┘
             │
             ▼
┌─────────────────────────────────────┐
│   SimConfig (parameters)            │
└────────────┬────────────────────────┘
             │
             ▼
┌─────────────────────────────────────┐
│   SwarmSimulation (engine)          │
│  ├─ Drones[]                        │
│  ├─ Obstacles[]                     │
│  ├─ Target (Vec2)                   │
│  └─ SpatialGrid                     │
└────────────┬────────────────────────┘
             │
             ▼
   ┌─────────────────────┐
   │  Physics Step       │
   │  - Calculate forces │
   │  - Update velocity  │
   │  - Update position  │
   │  - Drain battery    │
   └─────────┬───────────┘
             │
    ┌────────┴────────┐
    ▼                 ▼
┌──────────────┐  ┌──────────────┐
│ Console      │  │ PPM Renderer │
│ Renderer     │  │ (optional)   │
└──────────────┘  └──────────────┘
```

## 5. Optional OpenGL Visualization (3D Viewer)

HiveFlight also includes an **interactive OpenGL-based viewer** (`hiveflight_gl_viewer`) that renders the same 3D swarm state using real 3D graphics.


### OpenGL Viewer Executable
- **Target**: `hiveflight_gl_viewer`
- **Entry**: `main_opengl.cpp`

### Key Components
- `OpenGLSwarmViewer.cpp/.hpp`
  - Handles window/display (`display()`, `reshape()`)
  - Draws scene elements: world box, grid, obstacles, target, drones, overlays
  - Implements camera + interaction: keyboard/special/mouse/timer

### Data Flow (Simulation → OpenGL)

```
┌───────────────────────┐
│      SwarmSimulation3D │
└───────────┬───────────┘
            │ step() / state updates
            ▼
┌───────────────────────┐
│     OpenGLSwarmViewer   │
│  ├─ setCamera()         │
│  ├─ drawScene()        │
│  ├─ drawDrones()       │
│  └─ drawObstacles()    │
└───────────┬───────────┘
            │ OpenGL rendering calls
            ▼
┌───────────────────────┐
│  OpenGL Window (interactive) │
└───────────────────────┘
```


## Simulation Loop

```cpp
for (int step = 0; step < cfg.steps; ++step) {
    // Render current state
    console.print(drones, obstacles, target, step, time, cfg);
    
    // Optional: render to PPM image
    if (cfg.renderPpm && step % cfg.renderEvery == 0) {
        ppm.render(drones, step, cfg);
    }
    
    // Advance simulation
    sim.step();
    
    // Rendering delay
    sleep(cfg.sleepMs);
}
```

## Performance Characteristics

### Time Complexity per Step
- Force calculation: O(n) average with spatial grid
- Position update: O(n)
- Overall: O(n) with spatial optimization

### Space Complexity
- Drones: O(n)
- Spatial grid: O(n)
- Total: O(n)

### Rendering Overhead
- Console rendering: O(grid_width × grid_height)
- Typically small: O(2400) for 80×30 grid
- PPM rendering: O(image_width × image_height × 3) if enabled

## Key Algorithms

### Reynolds Boids
Three core behaviors weighted and combined:

1. **Separation**: Steer to avoid crowding local flockmates
2. **Alignment**: Steer towards average heading of local flockmates
3. **Cohesion**: Steer to move toward average location of local flockmates

### Spatial Grid (Hash Grid)
- Partition 2D space into cells
- Insert each drone into cell
- Iterate only neighboring cells for force queries
- Reduces quadratic complexity to linear average case

### Vector Normalization and Limiting
- Normalize forces to ensure consistent steering magnitude
- Limit acceleration and velocity
- Prevent instability and unrealistic motion

## Configuration Hierarchy

1. **Defaults** - Hardcoded in `SimConfig` struct
2. **File** - Loaded from `.conf` file via `loadConfigFromFile()`
3. **Arguments** - Command-line overrides via `--config` and other flags

Priority: Arguments > File > Defaults

## Extension Points

Potential areas for adding features:

1. **Multiple targets** - Modify `m_target` to `targets[]`
2. **Dynamic obstacles** - Add obstacle spawning/removal
3. **Formation control** - Add formation geometry constraints
4. **Communication** - Implement drone-to-drone messaging
5. **Sensors** - Add range sensors, LiDAR simulation
6. **Advanced rendering** - Color-coded visualization
7. **Statistics collection** - Track metrics over time
8. **Swarm intelligence** - Pheromone trails, voting systems

## Building and Testing

### Clean Build
```bash
cd build
make clean
cmake ..
make
```

### Debug Mode (if CMake configured for it)
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### Running with Debugging
```bash
gdb ./drone_swarm
run --config swarm_demo.conf
```

## Future Optimizations

- **SIMD**: Vectorize vector operations
- **GPU**: Parallel force calculations
- **Threading**: Multi-core simulation
- **Spatial coherence**: Better grid management
- **Adaptive timestep**: Dynamic dt adjustment
- **Early exit**: Cull non-interacting drones
