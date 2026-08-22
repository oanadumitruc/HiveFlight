# Quick Start Guide - HiveFlight Drone Swarm Simulation

## Building

### Linux/WSL
```bash
cd /mnt/c/work/ubuntu_work/HiveFlight
bash build_linux.sh
```

### Windows (with MinGW)
```cmd
cd c:\work\ubuntu_work\HiveFlight
build_windows.bat
```

The executable `drone_swarm` will be created in the `build/` directory.

## Running Simulations

### Basic Run (Default Configuration)
```bash
./build/drone_swarm
```
Runs with default parameters: 5 drones, 10 steps

### With Custom Config File
```bash
./build/drone_swarm --config swarm_demo.conf
```

### Demo Configurations

#### Small Swarm (Fast)
```bash
./build/drone_swarm 5 50
```
5 drones, 50 timesteps, ~2 seconds execution

#### Medium Swarm (Recommended)
```bash
./build/drone_swarm --config swarm_demo.conf
```
25 drones, 200 timesteps, ~30 seconds execution

#### Large Swarm (Intensive)
```bash
./build/drone_swarm --config /path/to/custom.conf
```
Configure with 50+ drones and 500+ timesteps for extended simulation

## Configuration Parameters

Create a `.conf` file with these parameters:

```ini
# World dimensions
worldWidth=800.0
worldHeight=600.0
wrapAround=true

# Swarm
droneCount=25
seed=42

# Physics
dt=0.1
maxSpeed=60.0
maxForce=15.0

# Behavior weights
weightSeparation=1.8      # Collision avoidance
weightAlignment=1.0       # Velocity matching
weightCohesion=1.0        # Attraction to center
weightTarget=0.8          # Target seeking
weightObstacle=2.5        # Obstacle avoidance

# Perception
separationRadius=60.0
alignmentRadius=120.0
cohesionRadius=120.0

# Battery
batteryDrainRate=0.0015

# Rendering
sleepMs=50
steps=200
```

## Understanding the Output

### Grid Display
```
+--------+
|  >  # |  >= moving right
|  X    |  #= obstacle
| v  ^  |  X= target
+--------+
```

### Statistics Line
```
Step: 24 | Active Drones: 25/25 | Avg Health: 99.9% | Avg Speed: 19.66
```
- **Step**: Current simulation timestep
- **Active Drones**: Number of drones with health > 0
- **Avg Health**: Average battery percentage (0-100%)
- **Avg Speed**: Average velocity magnitude

### Final Report
```
=== SWARM STATISTICS (Tick=200) ===
  Active Drones    : 25/25
  Avg Speed        : 27.144 u/s
  Battery [min/max]: 98.6% / 99.3%
  Obstacles        : 3
  Simulation Time  : 20.00 s
```

## Performance Tuning

### Faster Simulation
- Reduce `steps` parameter
- Increase `sleepMs` (less rendering overhead)
- Decrease `droneCount`

### More Interesting Behavior
- Increase `weightTarget` for stronger target seeking
- Increase `droneCount` for complex interactions
- Adjust perception radii for different neighbor distances
- Modify force weights for different swarm personalities

### Lower Battery Drain
- Decrease `batteryDrainRate` (default 0.0015)
- Reduce `maxSpeed` to limit velocity

## Troubleshooting

**Simulation runs too fast**: Increase `sleepMs` in config

**Simulation too slow**: Decrease `droneCount` or `steps`

**Drones clustered too tightly**: Increase `weightSeparation`

**Drones ignoring target**: Increase `weightTarget`

**Drones crashing into obstacles**: Increase `weightObstacle`

## Output Files

- Console output: ASCII visualization and statistics
- Optional PPM frames: Requires `renderPpm=true` (generates frame files)
- No data files by default

## Advanced Usage

### Create Custom Configuration
```bash
cp swarm_demo.conf my_config.conf
# Edit my_config.conf with your parameters
./build/drone_swarm --config my_config.conf
```

### Run Batch Simulations
```bash
for i in {1..5}; do
  echo "Run $i"
  ./build/drone_swarm --config swarm_demo.conf
done
```

### Capture Output to File
```bash
./build/drone_swarm --config swarm_demo.conf > simulation_log.txt 2>&1
```

## Performance Metrics

Expected performance on modern systems:
- 25 drones × 200 steps: ~30 seconds
- 50 drones × 200 steps: ~90 seconds
- 100 drones × 200 steps: ~4 minutes

Actual timing varies with CPU and rendering settings.
