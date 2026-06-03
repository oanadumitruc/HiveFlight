#pragma once

#include <cstddef>
#include <vector>
#include <cmath>

#include "Config.hpp"
#include "Vec2.hpp"

struct Obstacle {
    Vec2 position;
    double radius;
};

struct DroneState {
    std::size_t id = 0;
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    double health = 100.0;  // Battery level (0-100)
};

#include "SpatialGrid.hpp"

class SwarmSimulation {
public:
    explicit SwarmSimulation(const SimConfig& cfg);

    void reset();
    void step();

    const std::vector<DroneState>& drones() const { return m_drones; }
    const std::vector<Obstacle>& obstacles() const { return m_obstacles; }
    const Vec2& target() const { return m_target; }
    const SimConfig& config() const { return m_cfg; }
    
    double time() const { return m_time; }
    int tick() const { return m_tick; }

private:
    void applyWrap(Vec2& p) const;
    void updateTarget();

    Vec2 separationForce(std::size_t i) const;
    Vec2 alignmentForce(std::size_t i) const;
    Vec2 cohesionForce(std::size_t i) const;
    Vec2 seekTarget(std::size_t i) const;
    Vec2 avoidObstacles(std::size_t i) const;
    Vec2 seek(const Vec2& target, const Vec2& from, const Vec2& currentVelocity) const;

    SpatialGrid m_grid;
    SimConfig m_cfg;
    std::vector<DroneState> m_drones;
    std::vector<Obstacle> m_obstacles;
    Vec2 m_target;
    double m_time = 0.0;
    int m_tick = 0;
};

