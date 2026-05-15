#pragma once

#include <iostream>
#include <vector>

#include "SwarmSimulation.hpp"

class ConsoleRenderer {
public:
    void print(const std::vector<DroneState>& drones, int step) const {
        std::cout << "Step " << step << ":\n";
        for (const auto& d : drones) {
            std::cout << "Drone " << d.id << ": (" << d.position.x << ", " << d.position.y
                      << ") Vel: (" << d.velocity.x << ", " << d.velocity.y << ")\n";
        }
        std::cout << "\n";
    }
};

