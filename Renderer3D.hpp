#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "SwarmSimulation3D.hpp"

class Renderer3D {
public:
    Renderer3D() = default;

    // Console-based 3D ASCII projection
    void printConsole(const std::vector<Drone3D>& drones,
                      const std::vector<Obstacle3D>& obstacles,
                      const Vec3& target,
                      const SwarmConfig3D& cfg,
                      int step) const;

    // Export to OBJ format for external visualization
    void exportOBJ(const std::vector<Drone3D>& drones,
                   const std::vector<Obstacle3D>& obstacles,
                   const Vec3& target,
                   const std::string& filename) const;

    // Export to CSV for data analysis
    void exportCSV(const std::vector<Drone3D>& drones,
                   const std::string& filename) const;

    // Print 3D statistics
    void printStats(const std::vector<Drone3D>& drones,
                    int tick,
                    double time,
                    const std::vector<Obstacle3D>& obstacles,
                    const SwarmConfig3D& cfg) const;

private:
    // Project 3D point to 2D for console display
    // Uses isometric projection
    struct Point2D {
        double x, y;
    };

    Point2D project3D(const Vec3& p, const SwarmConfig3D& cfg) const;
};
