#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>

#include "SwarmSimulation.hpp"

// Helper function for clamping (C++17 alternative)
template<typename T>
T clamp(T val, T min_val, T max_val) {
    return std::min(std::max(val, min_val), max_val);
}

class ConsoleRenderer {
public:
    ConsoleRenderer(int gridWidth = 80, int gridHeight = 30)
        : m_gridWidth(gridWidth), m_gridHeight(gridHeight) {}

    void print(const std::vector<DroneState>& drones, 
               const std::vector<Obstacle>& obstacles,
               const Vec2& target,
               int step,
               double time,
               const SimConfig& cfg) const {
        // Initialize grid
        std::vector<std::string> grid(m_gridHeight, std::string(m_gridWidth, ' '));

        // Draw obstacles
        for (const auto& obs : obstacles) {
            int cx = static_cast<int>(obs.position.x / cfg.worldWidth * m_gridWidth);
            int cy = static_cast<int>(obs.position.y / cfg.worldHeight * m_gridHeight);
            int r = std::max(1, static_cast<int>(obs.radius / cfg.worldWidth * m_gridWidth));
            
            for (int dy = -r; dy <= r; ++dy) {
                for (int dx = -r; dx <= r; ++dx) {
                    if (dx*dx + dy*dy <= r*r) {
                        int gx = cx + dx;
                        int gy = cy + dy;
                        if (gx >= 0 && gx < m_gridWidth && gy >= 0 && gy < m_gridHeight) {
                            grid[gy][gx] = '#';
                        }
                    }
                }
            }
        }

        // Draw target
        int tx = clamp(static_cast<int>(target.x / cfg.worldWidth * m_gridWidth), 0, m_gridWidth - 1);
        int ty = clamp(static_cast<int>(target.y / cfg.worldHeight * m_gridHeight), 0, m_gridHeight - 1);
        grid[ty][tx] = 'X';

        // Draw drones
        for (const auto& d : drones) {
            if (d.health <= 0.0) continue;
            
            int gx = clamp(static_cast<int>(d.position.x / cfg.worldWidth * m_gridWidth), 0, m_gridWidth - 1);
            int gy = clamp(static_cast<int>(d.position.y / cfg.worldHeight * m_gridHeight), 0, m_gridHeight - 1);
            
            // Symbol based on velocity direction
            char sym = '*';
            double vel_mag = d.velocity.magnitude();
            if (vel_mag > 1e-9) {
                double ang = std::atan2(d.velocity.y, d.velocity.x) * 180.0 / 3.14159265358979323846;
                if (ang > 135 || ang < -135) sym = '<';
                else if (ang > 45) sym = 'v';
                else if (ang < -45) sym = '^';
                else sym = '>';
            }
            grid[gy][gx] = sym;
        }

        // Print grid
        std::cout << "\033[H\033[2J";  // Clear screen
        std::cout << "\033[1;36m";
        std::cout << "+" << std::string(m_gridWidth, '-') << "+\n";
        for (int y = 0; y < m_gridHeight; ++y) {
            std::cout << "|" << grid[y] << "|\n";
        }
        std::cout << "+" << std::string(m_gridWidth, '-') << "+\n";
        std::cout << "\033[0m";

        // Print statistics
        int alive = 0;
        double avgHealth = 0.0;
        double avgSpeed = 0.0;
        
        for (const auto& d : drones) {
            if (d.health > 0.0) {
                ++alive;
                avgHealth += d.health;
                avgSpeed += d.velocity.magnitude();
            }
        }
        
        if (alive > 0) {
            avgHealth /= alive;
            avgSpeed /= alive;
        }

        std::cout << "\033[1;33m";
        std::cout << " Step: " << step << "  |  Active Drones: " << alive << "/" << drones.size()
                  << "  |  Avg Health: " << std::fixed << std::setprecision(1) << avgHealth << "%"
                  << "  |  Avg Speed: " << std::setprecision(2) << avgSpeed << "\n";
        std::cout << " Legend: >^<v=drone | *=stationary | #=obstacle | X=target\n";
        std::cout << "\033[0m";
    }

    void printStats(const std::vector<DroneState>& drones,
                    int tick,
                    double time,
                    const std::vector<Obstacle>& obstacles) const {
        std::cout << "\n\033[1;32m=== SWARM STATISTICS (Tick=" << tick << ") ===\033[0m\n";
        
        double avgSpeed = 0.0;
        double minHealth = 100.0;
        double maxHealth = 0.0;
        int alive = 0;
        
        for (const auto& d : drones) {
            if (d.health > 0.0) {
                ++alive;
                avgSpeed += d.velocity.magnitude();
                minHealth = std::min(minHealth, d.health);
                maxHealth = std::max(maxHealth, d.health);
            }
        }
        
        if (alive > 0) {
            avgSpeed /= alive;
        }

        std::cout << "  Active Drones    : " << alive << "/" << drones.size() << "\n";
        std::cout << "  Avg Speed        : " << std::fixed << std::setprecision(3) << avgSpeed << " u/s\n";
        std::cout << "  Battery [min/max]: " << std::setprecision(1) << minHealth << "% / " << maxHealth << "%\n";
        std::cout << "  Obstacles        : " << obstacles.size() << "\n";
        std::cout << "  Simulation Time  : " << std::setprecision(2) << time << " s\n";
    }

private:
    int m_gridWidth;
    int m_gridHeight;
};

