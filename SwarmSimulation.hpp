#pragma once

#include <cstddef>
#include <vector>

#include "Config.hpp"

#include "SpatialGrid.hpp"

struct Vec2 {

    double x = 0.0;
    double y = 0.0;

    Vec2() = default;
    Vec2(double x_, double y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2 operator*(double s) const { return Vec2(x * s, y * s); }
    Vec2 operator/(double s) const { return Vec2(x / s, y / s); }

    double magnitude() const;
    Vec2 normalized() const;
};

struct DroneState {
    std::size_t id = 0;
    Vec2 position;
    Vec2 velocity;
};

class SwarmSimulation {
public:
    explicit SwarmSimulation(const SimConfig& cfg);

    void reset();
    void step();

    const std::vector<DroneState>& drones() const { return m_drones; }
    const SimConfig& config() const { return m_cfg; }

private:
    void applyWrap(Vec2& p) const;

    Vec2 separationForce(std::size_t i) const;
    Vec2 alignmentForce(std::size_t i) const;
    Vec2 cohesionForce(std::size_t i) const;
    Vec2 seek(const Vec2& target, const Vec2& from, const Vec2& currentVelocity) const;

    SpatialGrid m_grid;
    SimConfig m_cfg;
    std::vector<DroneState> m_drones;
};

