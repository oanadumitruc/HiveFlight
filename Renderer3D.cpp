#include "Renderer3D.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

Renderer3D::Point2D Renderer3D::project3D(const Vec3& p, const SwarmConfig3D& cfg) const {
    // Isometric projection
    // This creates a 3D appearance from 2D
    double scale = 1.5;
    double x = (p.x - p.z) * scale;
    double y = (p.y - (p.x + p.z) * 0.5) * scale;
    return {x, y};
}

void Renderer3D::printConsole(const std::vector<Drone3D>& drones,
                              const std::vector<Obstacle3D>& obstacles,
                              const Vec3& target,
                              const SwarmConfig3D& cfg,
                              int step) const {
    // Create 2D projection for console display
    const int width = 100;
    const int height = 40;
    
    std::vector<std::string> grid(height, std::string(width, ' '));

    // Project and render obstacles
    for (const auto& obs : obstacles) {
        Point2D proj = project3D(obs.position, cfg);
        int px = static_cast<int>(width / 2.0 + proj.x / 20.0);
        int py = static_cast<int>(height / 2.0 + proj.y / 20.0);
        int r = std::max(1, static_cast<int>(obs.radius / 10.0));

        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                if (dx*dx + dy*dy <= r*r) {
                    int gx = px + dx;
                    int gy = py + dy;
                    if (gx >= 0 && gx < width && gy >= 0 && gy < height) {
                        grid[gy][gx] = '#';
                    }
                }
            }
        }
    }

    // Project and render target
    Point2D targetProj = project3D(target, cfg);
    int tx = static_cast<int>(width / 2.0 + targetProj.x / 20.0);
    int ty = static_cast<int>(height / 2.0 + targetProj.y / 20.0);
    if (tx >= 0 && tx < width && ty >= 0 && ty < height) {
        grid[ty][tx] = 'X';
    }

    // Project and render drones
    for (const auto& d : drones) {
        if (d.health <= 0.0) continue;

        Point2D proj = project3D(d.position, cfg);
        int gx = static_cast<int>(width / 2.0 + proj.x / 20.0);
        int gy = static_cast<int>(height / 2.0 + proj.y / 20.0);

        if (gx >= 0 && gx < width && gy >= 0 && gy < height) {
            // Drone symbol based on velocity in 3D space
            char sym = '*';
            if (d.velocity.magnitude() > 0.5) {
                // Use velocity components to determine symbol
                double vlen = d.velocity.magnitude();
                double vx = d.velocity.x / vlen;
                double vz = d.velocity.z / vlen;
                
                if (std::abs(vx) > 0.7) sym = vx > 0 ? '>' : '<';
                else if (std::abs(vz) > 0.7) sym = vz > 0 ? 'v' : '^';
                else sym = '*';
            }
            
            grid[gy][gx] = sym;
        }
    }

    // Print header
    std::cout << "\033[H\033[2J";  // Clear screen
    std::cout << "\033[1;36m";
    std::cout << "+";
    for (int i = 0; i < width; ++i) std::cout << "-";
    std::cout << "+\n";

    // Print grid
    for (int y = 0; y < height; ++y) {
        std::cout << "|" << grid[y] << "|\n";
    }

    // Print footer
    std::cout << "+";
    for (int i = 0; i < width; ++i) std::cout << "-";
    std::cout << "+\n";
    std::cout << "\033[0m";

    // Print statistics
    int alive = 0;
    double avgHealth = 0.0;
    double avgSpeed = 0.0;
    double minZ = cfg.worldDepth;
    double maxZ = 0.0;

    for (const auto& d : drones) {
        if (d.health > 0.0) {
            ++alive;
            avgHealth += d.health;
            avgSpeed += d.velocity.magnitude();
            minZ = std::min(minZ, d.position.z);
            maxZ = std::max(maxZ, d.position.z);
        }
    }

    if (alive > 0) {
        avgHealth /= alive;
        avgSpeed /= alive;
    }

    std::cout << "\033[1;33m";
    std::cout << " Step: " << step << "  |  Active: " << alive << "/" << drones.size()
              << "  |  Health: " << std::fixed << std::setprecision(1) << avgHealth << "%"
              << "  |  Speed: " << std::setprecision(2) << avgSpeed
              << "  |  Depth: [" << minZ << ", " << maxZ << "]\n";
    std::cout << " Legend: >^<v*=drone | #=obstacle | X=target | (Isometric View)\n";
    std::cout << "\033[0m";
}

void Renderer3D::exportOBJ(const std::vector<Drone3D>& drones,
                           const std::vector<Obstacle3D>& obstacles,
                           const Vec3& target,
                           const std::string& filename) const {
    std::ofstream obj(filename);
    if (!obj.is_open()) return;

    obj << "# 3D Drone Swarm Simulation\n";
    obj << "# Drones, Obstacles, Target\n\n";

    int vertexCount = 1;

    // Write obstacles
    obj << "# Obstacles\n";
    for (size_t i = 0; i < obstacles.size(); ++i) {
        const auto& obs = obstacles[i];
        obj << "# Obstacle " << (i + 1) << "\n";
        
        // Create sphere mesh for obstacle
        int rings = 8;
        int sectors = 8;
        double r = obs.radius;
        
        for (int ring = 0; ring <= rings; ++ring) {
            double theta = (double)ring / rings * 3.14159265;
            for (int sec = 0; sec <= sectors; ++sec) {
                double phi = (double)sec / sectors * 6.28318530;
                double x = r * std::sin(theta) * std::cos(phi) + obs.position.x;
                double y = r * std::cos(theta) + obs.position.y;
                double z = r * std::sin(theta) * std::sin(phi) + obs.position.z;
                obj << "v " << x << " " << y << " " << z << "\n";
            }
        }
    }

    // Write target as a star/marker
    obj << "\n# Target\n";
    obj << "v " << target.x << " " << target.y << " " << target.z << "\n";
    obj << "v " << (target.x + 2.0) << " " << target.y << " " << target.z << "\n";
    obj << "v " << (target.x - 2.0) << " " << target.y << " " << target.z << "\n";
    obj << "v " << target.x << " " << (target.y + 2.0) << " " << target.z << "\n";
    obj << "v " << target.x << " " << (target.y - 2.0) << " " << target.z << "\n";
    obj << "v " << target.x << " " << target.y << " " << (target.z + 2.0) << "\n";
    obj << "v " << target.x << " " << target.y << " " << (target.z - 2.0) << "\n";

    // Write drones as small spheres
    obj << "\n# Drones\n";
    for (const auto& d : drones) {
        if (d.health <= 0.0) continue;

        double r = d.radius;
        int rings = 4;
        int sectors = 4;
        
        for (int ring = 0; ring <= rings; ++ring) {
            double theta = (double)ring / rings * 3.14159265;
            for (int sec = 0; sec <= sectors; ++sec) {
                double phi = (double)sec / sectors * 6.28318530;
                double x = r * std::sin(theta) * std::cos(phi) + d.position.x;
                double y = r * std::cos(theta) + d.position.y;
                double z = r * std::sin(theta) * std::sin(phi) + d.position.z;
                obj << "v " << x << " " << y << " " << z << "\n";
            }
        }
    }

    obj.close();
}

void Renderer3D::exportCSV(const std::vector<Drone3D>& drones,
                           const std::string& filename) const {
    std::ofstream csv(filename);
    if (!csv.is_open()) return;

    csv << "ID,X,Y,Z,VelX,VelY,VelZ,Health,Speed\n";

    for (const auto& d : drones) {
        double speed = d.velocity.magnitude();
        csv << d.id << ","
            << d.position.x << ","
            << d.position.y << ","
            << d.position.z << ","
            << d.velocity.x << ","
            << d.velocity.y << ","
            << d.velocity.z << ","
            << d.health << ","
            << speed << "\n";
    }

    csv.close();
}

void Renderer3D::printStats(const std::vector<Drone3D>& drones,
                            int tick,
                            double time,
                            const std::vector<Obstacle3D>& obstacles,
                            const SwarmConfig3D& cfg) const {
    std::cout << "\n\033[1;32m=== 3D SWARM STATISTICS (Tick=" << tick << ") ===\033[0m\n";

    double avgSpeed = 0.0;
    double minHealth = 100.0;
    double maxHealth = 0.0;
    int alive = 0;

    double centerX = 0.0, centerY = 0.0, centerZ = 0.0;
    double minX = cfg.worldWidth, maxX = 0.0;
    double minY = cfg.worldHeight, maxY = 0.0;
    double minZ = cfg.worldDepth, maxZ = 0.0;

    for (const auto& d : drones) {
        if (d.health > 0.0) {
            ++alive;
            avgSpeed += d.velocity.magnitude();
            minHealth = std::min(minHealth, d.health);
            maxHealth = std::max(maxHealth, d.health);

            centerX += d.position.x;
            centerY += d.position.y;
            centerZ += d.position.z;

            minX = std::min(minX, d.position.x);
            maxX = std::max(maxX, d.position.x);
            minY = std::min(minY, d.position.y);
            maxY = std::max(maxY, d.position.y);
            minZ = std::min(minZ, d.position.z);
            maxZ = std::max(maxZ, d.position.z);
        }
    }

    if (alive > 0) {
        avgSpeed /= alive;
        centerX /= alive;
        centerY /= alive;
        centerZ /= alive;
    }

    std::cout << "  Active Drones      : " << alive << "/" << drones.size() << "\n";
    std::cout << "  Avg Speed          : " << std::fixed << std::setprecision(3) << avgSpeed << " u/s\n";
    std::cout << "  Battery [min/max]  : " << std::setprecision(1) << minHealth << "% / " << maxHealth << "%\n";
    std::cout << "  Swarm Center       : (" << std::setprecision(1) << centerX << ", " << centerY << ", " << centerZ << ")\n";
    std::cout << "  Bounding Box       : X[" << minX << ", " << maxX << "] "
              << "Y[" << minY << ", " << maxY << "] Z[" << minZ << ", " << maxZ << "]\n";
    std::cout << "  Swarm Spread       : " << std::setprecision(0) 
              << (maxX - minX) << " x " << (maxY - minY) << " x " << (maxZ - minZ) << "\n";
    std::cout << "  Obstacles          : " << obstacles.size() << "\n";
    std::cout << "  Simulation Time    : " << std::setprecision(2) << time << " s\n";
}
