# HiveFlight

# Drone Swarm Simulation

This is a simple simulation of a swarm of 5 drones using basic flocking algorithms (separation, alignment, cohesion) in C++.

## Features

- 5 drones moving in a 2D space (800x600 units)
- Flocking behavior: drones try to stay close but not too close, align velocities, and move towards the center of mass
- Console output showing positions and velocities at each step
- Wrap-around boundaries

## Compilation

To compile the program, use g++:

```
g++ drone_swarm.cpp -o drone_swarm.exe -std=c++11
```

## Running

Execute the compiled program:

```
./drone_swarm.exe
```

The simulation will run for 100 steps, printing the state of each drone every 0.5 seconds.

## Algorithm Details

Each drone follows three rules:

1. **Separation**: Steer to avoid crowding local flockmates
2. **Alignment**: Steer towards the average heading of neighbors
3. **Cohesion**: Steer to move toward the average position of neighbors

These forces are combined and applied to update the drone's velocity and position.
