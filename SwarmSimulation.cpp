#include "SwarmSimulation.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

static double distance(const Vec2& a, const Vec2& b) {
    return (a - b).magnitude();
}

double Vec2::magnitude() const {
    return std::sqrt(x * x + y * y);
}

Vec2 Vec2::normalized() const {
    double mag = magnitude();
    if (mag > 0.0) return *this / mag;
    return Vec2(0.0, 0.0);
}

Vec2 Vec2::limit(double maxLen) const {
    double mag = magnitude();
    if (mag > maxLen) {
        return normalized() * maxLen;
    }
    return *this;
}

SwarmSimulation::SwarmSimulation(const SimConfig& cfg) : m_cfg(cfg) {
    reset();
}

void SwarmSimulation::reset() {
    std::random_device rd;
    std::mt19937 gen;
    if (m_cfg.seed != 0) {
        gen.seed(m_cfg.seed);
    } else {
        gen.seed(rd());
    }

    std::uniform_real_distribution<> posDist(m_cfg.posMin, m_cfg.posMax);
    std::uniform_real_distribution<> velDist(m_cfg.velMin, m_cfg.velMax);

    m_drones.clear();
    m_drones.reserve(m_cfg.droneCount);

    for (std::size_t i = 0; i < m_cfg.droneCount; ++i) {
        DroneState d;
        d.id = i + 1;
        d.position = Vec2(posDist(gen), posDist(gen));
        d.velocity = Vec2(velDist(gen), velDist(gen));
        d.acceleration = Vec2(0.0, 0.0);
        d.health = 100.0;
        m_drones.push_back(d);
    }

    // Initialize obstacles
    m_obstacles.clear();
    m_obstacles.push_back({Vec2(m_cfg.worldWidth * 0.3, m_cfg.worldHeight * 0.4), 30.0});
    m_obstacles.push_back({Vec2(m_cfg.worldWidth * 0.7, m_cfg.worldHeight * 0.3), 25.0});
    m_obstacles.push_back({Vec2(m_cfg.worldWidth * 0.5, m_cfg.worldHeight * 0.7), 35.0});

    // Initialize target
    m_target = Vec2(m_cfg.worldWidth / 2.0, m_cfg.worldHeight / 2.0);
    m_time = 0.0;
    m_tick = 0;
}

void SwarmSimulation::updateTarget() {
    // Move target in circular pattern
    const double PI = 3.14159265358979323846;
    double radiusX = m_cfg.worldWidth * 0.25;
    double radiusY = m_cfg.worldHeight * 0.25;
    
    m_target.x = m_cfg.worldWidth / 2.0 + radiusX * std::cos(m_time * 0.3);
    m_target.y = m_cfg.worldHeight / 2.0 + radiusY * std::sin(m_time * 0.2);
}

void SwarmSimulation::applyWrap(Vec2& p) const {
    if (!m_cfg.wrapAround) return;

    if (p.x < 0.0) p.x = m_cfg.worldWidth;
    if (p.x > m_cfg.worldWidth) p.x = 0.0;
    if (p.y < 0.0) p.y = m_cfg.worldHeight;
    if (p.y > m_cfg.worldHeight) p.y = 0.0;
}

void SwarmSimulation::step() {
    // Update target position
    updateTarget();
    m_time += m_cfg.dt;

    // Build grid for candidate neighbor search (spatial partitioning)
    m_grid.init(m_cfg.worldWidth, m_cfg.worldHeight, m_cfg.cohesionRadius);
    m_grid.cellSize = std::max(1e-6, m_cfg.cohesionRadius);
    m_grid.clear();

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        m_grid.insert(i, m_drones[i], m_cfg.wrapAround);
    }

    // compute all forces based on current state
    std::vector<Vec2> nextVel(m_drones.size());
    std::vector<Vec2> nextPos(m_drones.size());
    std::vector<double> nextHealth(m_drones.size());

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        if (m_drones[i].health <= 0.0) {
            nextVel[i] = m_drones[i].velocity;
            nextPos[i] = m_drones[i].position;
            nextHealth[i] = m_drones[i].health;
            continue;
        }

        const Vec2 curPos = m_drones[i].position;
        const Vec2 curVel = m_drones[i].velocity;
        Vec2 acc = Vec2(0.0, 0.0);

        Vec2 separation = separationForce(i);
        Vec2 alignment = alignmentForce(i);
        Vec2 cohesion = cohesionForce(i);
        Vec2 seekForce = seekTarget(i);
        Vec2 avoid = avoidObstacles(i);

        // Apply weighted forces as acceleration
        acc += separation * m_cfg.weightSeparation;
        acc += alignment * m_cfg.weightAlignment;
        acc += cohesion * m_cfg.weightCohesion;
        acc += seekForce * m_cfg.weightTarget;
        acc += avoid * m_cfg.weightObstacle;

        acc = acc.limit(m_cfg.maxForce);

        // Update velocity with acceleration
        Vec2 vel = curVel + acc * m_cfg.dt;
        vel = vel.limit(m_cfg.maxSpeed);

        // Update position
        Vec2 pos = curPos + vel * m_cfg.dt;
        applyWrap(pos);

        // Battery depletion based on velocity
        double health = m_drones[i].health - vel.magnitude() * m_cfg.batteryDrainRate * m_cfg.dt;
        health = std::max(health, 0.0);

        nextVel[i] = vel;
        nextPos[i] = pos;
        nextHealth[i] = health;
    }

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        m_drones[i].velocity = nextVel[i];
        m_drones[i].position = nextPos[i];
        m_drones[i].acceleration = (nextVel[i] - m_drones[i].velocity) / m_cfg.dt;
        m_drones[i].health = nextHealth[i];
    }

    ++m_tick;
}

Vec2 SwarmSimulation::separationForce(std::size_t i) const {
    const Vec2 pos = m_drones[i].position;
    const Vec2 vel = m_drones[i].velocity;

    Vec2 steer(0.0, 0.0);
    std::size_t count = 0;

    const double r = m_cfg.separationRadius;

    m_grid.forEachCandidate(pos, m_cfg.wrapAround, [&](std::size_t j) {
        if (j == i) return;
        Vec2 diff = pos - m_drones[j].position;
        double d = diff.magnitude();
        if (d > 0.0 && d < r) {
            // steer away: direction away weighted by distance
            steer += diff.normalized() * (1.0 / d);
            count++;
        }
    });

    if (count == 0) return Vec2(0.0, 0.0);

    steer = steer / static_cast<double>(count);
    steer = steer.normalized() * m_cfg.maxSpeed - vel;
    return steer.limit(m_cfg.maxForce);
}

Vec2 SwarmSimulation::alignmentForce(std::size_t i) const {
    const Vec2 pos = m_drones[i].position;
    const Vec2 vel = m_drones[i].velocity;

    Vec2 sum(0.0, 0.0);
    std::size_t count = 0;

    const double r = m_cfg.alignmentRadius;

    m_grid.forEachCandidate(pos, m_cfg.wrapAround, [&](std::size_t j) {
        if (j == i) return;
        double d = (pos - m_drones[j].position).magnitude();
        if (d > 0.0 && d < r) {
            sum += m_drones[j].velocity;
            count++;
        }
    });

    if (count == 0) return Vec2(0.0, 0.0);

    sum = sum / static_cast<double>(count);
    sum = sum.normalized() * m_cfg.maxSpeed;
    return (sum - vel).limit(m_cfg.maxForce);
}

Vec2 SwarmSimulation::cohesionForce(std::size_t i) const {
    const Vec2 pos = m_drones[i].position;
    const Vec2 curVel = m_drones[i].velocity;

    Vec2 sum(0.0, 0.0);
    std::size_t count = 0;

    const double r = m_cfg.cohesionRadius;

    m_grid.forEachCandidate(pos, m_cfg.wrapAround, [&](std::size_t j) {
        if (j == i) return;
        double d = (pos - m_drones[j].position).magnitude();
        if (d > 0.0 && d < r) {
            sum += m_drones[j].position;
            count++;
        }
    });

    if (count == 0) return Vec2(0.0, 0.0);

    Vec2 center = sum / static_cast<double>(count);
    return seek(center, pos, curVel);
}

Vec2 SwarmSimulation::seekTarget(std::size_t i) const {
    const Vec2 pos = m_drones[i].position;
    const Vec2 vel = m_drones[i].velocity;
    
    Vec2 desired = (m_target - pos).normalized() * m_cfg.maxSpeed;
    return (desired - vel).limit(m_cfg.maxForce);
}

Vec2 SwarmSimulation::avoidObstacles(std::size_t i) const {
    const Vec2 pos = m_drones[i].position;
    Vec2 steer(0.0, 0.0);
    
    for (const auto& obs : m_obstacles) {
        Vec2 diff = pos - obs.position;
        double d = diff.magnitude();
        double safeDistance = obs.radius + m_cfg.obstacleBuffer;
        
        if (d < safeDistance && d > 1e-9) {
            // Push away from obstacle, stronger the closer we are
            steer += diff.normalized() * (safeDistance - d) / safeDistance;
        }
    }
    
    return steer.limit(m_cfg.maxForce * 2.0);
}

Vec2 SwarmSimulation::seek(const Vec2& target, const Vec2& from, const Vec2& currentVelocity) const {
    Vec2 desired = target - from;
    desired = desired.normalized() * m_cfg.maxSpeed;
    return (desired - currentVelocity).limit(m_cfg.maxForce);
}

