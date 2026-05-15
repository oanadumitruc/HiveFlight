#pragma once

#include <cstddef>
#include <string>

struct SimConfig {
    // World
    double worldWidth = 800.0;
    double worldHeight = 600.0;
    bool wrapAround = true;

    // Swarm
    std::size_t droneCount = 5;
    unsigned int seed = 0; // 0 => random_device

    // Initialization ranges
    double posMin = 200.0;
    double posMax = 500.0;
    double velMin = -20.0;
    double velMax = 20.0;

    // Simulation
    double dt = 0.1;
    int steps = 10;
    int sleepMs = 100;

    // Flocking parameters
    double separationRadius = 50.0;
    double alignmentRadius = 100.0;
    double cohesionRadius = 100.0;

    double desiredSeparationSpeed = 50.0; // used as a target speed in steering

    double weightSeparation = 1.5;
    double weightAlignment = 1.0;
    double weightCohesion = 1.0;

    double maxSpeed = 50.0;

    // Visualization
    bool renderPpm = false;
    int renderImageW = 800;
    int renderImageH = 600;
    int renderEvery = 1; // render every N steps
    std::string renderFramesDir = "frames";
};

// Extremely small config loader for key=value pairs.

// Unknown keys are ignored. Lines starting with '#' are comments.
SimConfig loadConfigFromFile(const std::string& path);
SimConfig loadConfigFromArgs(int argc, char** argv);

