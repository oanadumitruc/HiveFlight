# 3D Simulation Quick Reference

## Quick Start

```bash
# Build everything
bash build_linux.sh

# Run 3D simulation (default 30 drones, 30 seconds)
./build/drone_swarm_3d

# Run with specific configuration
./build/drone_swarm_3d --drones 50 --seed 123

# Export final state as 3D model
./build/drone_swarm_3d --export obj swarm_final.obj

# Export drone data for analysis
./build/drone_swarm_3d --export csv drone_positions.csv
```

## Key Differences from 2D

| Feature | 2D Simulation | 3D Simulation |
|---------|---------------|---------------|
| **Binary** | `drone_swarm` | `drone_swarm_3d` |
| **World Size** | 800×600 | 200×200×150 |
| **Drones** | 25 | 30 |
| **View** | Top-down ASCII | Isometric ASCII |
| **Z-Axis** | N/A | Full 3D depth |
| **Visualization** | ASCII grid | Isometric ASCII |
| **Export** | PPM images | OBJ, CSV |
| **Performance** | ~2 min (200 steps) | ~30 sec (1800 frames) |

## Output Interpretation

### Isometric View Legend
```
>  Drone moving right
<  Drone moving left
v  Drone moving down (toward camera)
^  Drone moving up (away from camera)
*  Drone neutral/speed-dependent direction
#  Obstacle boundary
X  Target position
```

### Depth Range
```
Depth: [62.12, 142.81]
 |     ^     ^
 |     |     └─ Maximum Z coordinate
 |     └─────── Minimum Z coordinate
 └───────────── Z-axis range (visible drones)
```

### Swarm Center
```
Swarm Center: (168.2, 77.2, 100.6)
              X      Y      Z
```

### Bounding Box
```
X[125.7, 200.0]  =  Drones span X from 125.7 to 200.0
Y[34.0, 121.2]   =  Drones span Y from 34.0 to 121.2
Z[63.4, 144.0]   =  Drones span Z from 63.4 to 144.0
```

## World Coordinates

- **X-axis**: Left-Right (0 to 200)
- **Y-axis**: Height (0 to 200) 
- **Z-axis**: Depth (0 to 150)

Initial drone region: 20 to 180 on all axes

## Performance Tips

### Fastest Rendering
```bash
./build/drone_swarm_3d --drones 10
```
~10 seconds execution

### Balanced Experience
```bash
./build/drone_swarm_3d --drones 30
```
~30 seconds execution (default)

### Detailed Simulation
```bash
./build/drone_swarm_3d --drones 50
```
~60 seconds execution

## Export Usage

### Viewing OBJ Files

**Blender:**
1. File → Import → Wavefront (.obj)
2. Select `swarm_final.obj`
3. Rotate view to examine drone positions

**MeshLab:**
1. File → Import Mesh
2. Select `swarm_final.obj`
3. Use trackball to rotate

**Online Viewers:**
- https://3dviewer.net
- Upload `swarm_final.obj`
- Interactive 3D exploration

### Analyzing CSV Export

**Python:**
```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('drone_positions.csv')
print(df.head())

# Plot positions
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(df['X'], df['Y'], df['Z'])
plt.show()
```

**R:**
```r
drones <- read.csv('drone_positions.csv')
summary(drones)
plot(drones$X, drones$Y)
```

## Configuration Customization

Create custom config by modifying `SwarmConfig3D` in `SwarmSimulation3D.hpp`:

```cpp
struct SwarmConfig3D {
    // World
    double worldWidth = 200.0;
    double worldHeight = 200.0;
    double worldDepth = 150.0;

    // Swarm
    std::size_t droneCount = 30;
    
    // Force weights
    double weightSeparation = 1.8;
    double weightAlignment = 1.0;
    double weightCohesion = 1.0;
    double weightTarget = 0.8;
    double weightObstacle = 2.5;
    
    // Limits
    double maxSpeed = 40.0;
    double maxForce = 12.0;
};
```

Then rebuild with `bash build_linux.sh`.

## Typical Output Sequence

```
Step 0:    Initialization
           • All drones at 100% health
           • Scattered randomly
           • Speed: ~8-10 u/s

Step 500:  Acceleration phase
           • Drones converge toward target
           • Speed increasing to ~15 u/s
           • Health: ~99.9%

Step 1000: Established motion
           • Swarm moving coherently
           • Target tracking active
           • Speed: ~18-20 u/s

Step 1800: Steady state
           • All drones responsive
           • Health: ~99.4-99.5%
           • Average speed: 19-20 u/s
```

## Visualization Interpretation

**Tight cluster of drones:**
- Strong cohesion
- Target seeking active
- Low spread

**Dispersed drones:**
- High separation weight
- Exploring volume
- Large bounding box

**Many v symbols (downward):**
- Drones moving in Z direction
- 3D depth movement
- Volume exploration

**Circular pattern around X:**
- Target following
- Cyclic behavior
- Coordinated motion

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Drones bunch up | Increase `weightSeparation` |
| Drones escape obstacles | Increase `weightObstacle` |
| No convergence to target | Increase `weightTarget` |
| Too scattered | Increase `weightCohesion` |
| Slow execution | Reduce `droneCount` |
| Sim too fast | Render gets skipped (expected) |

## Statistics Monitoring

Key metrics to observe:

1. **Active Drones**: Should stay at 30/30
2. **Average Health**: Slow drain (100% → 99.4% over 30s)
3. **Average Speed**: Accelerates then stabilizes (~19 u/s)
4. **Bounding Box**: Expands initially, then contracts
5. **Swarm Center**: Moves toward and orbits target
6. **Spread**: Drones maintain 70-90 unit spread

## Next Steps

- Try different seed values for different initial conditions
- Increase drone count gradually to observe scaling
- Compare 2D vs 3D behavior differences
- Export and visualize in 3D viewer
- Modify parameters and observe emergent behavior
- Analyze CSV exports for detailed statistics

## Resources

- Main documentation: [3D_SIMULATION.md](3D_SIMULATION.md)
- Architecture guide: [ARCHITECTURE.md](ARCHITECTURE.md)
- 2D simulation: [QUICKSTART.md](QUICKSTART.md)
- Physics details: [UPDATES.md](UPDATES.md)
