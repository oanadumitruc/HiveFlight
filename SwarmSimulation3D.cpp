#include "SwarmSimulation3D.hpp"

#include <algorithm>
#include <cmath>
#include <random>

const double PI = 3.14159265358979323846;

SwarmSimulation3D::SwarmSimulation3D(const SwarmConfig3D& cfg) : m_cfg(cfg) {
    reset();
}

void SwarmSimulation3D::reset() {
    std::random_device rd;
    std::mt19937 gen;
    if (m_cfg.seed != 0) {
        gen.seed(m_cfg.seed);
    } else {
        gen.seed(rd());
    }

    std::uniform_real_distribution<> posDist(m_cfg.posMin, m_cfg.posMax);
    std::uniform_real_distribution<> velDist(m_cfg.velMin, m_cfg.velMax);
    std::uniform_real_distribution<> depthDist(30.0, m_cfg.worldDepth - 30.0);

    m_drones.clear();
    m_drones.reserve(m_cfg.droneCount);

    for (std::size_t i = 0; i < m_cfg.droneCount; ++i) {
        Drone3D d;
        d.id = i + 1;
        d.position = Vec3(posDist(gen), depthDist(gen), posDist(gen));
        d.velocity = Vec3(velDist(gen), velDist(gen), velDist(gen));
        d.acceleration = Vec3(0.0, 0.0, 0.0);
        d.health = 100.0;
        m_drones.push_back(d);
    }

    // Initialize obstacles as spheres
    m_obstacles.clear();
    m_obstacles.push_back({Vec3(m_cfg.worldWidth * 0.3, m_cfg.worldHeight * 0.5, m_cfg.worldDepth * 0.4), 15.0});
    m_obstacles.push_back({Vec3(m_cfg.worldWidth * 0.7, m_cfg.worldHeight * 0.5, m_cfg.worldDepth * 0.3), 12.0});
    m_obstacles.push_back({Vec3(m_cfg.worldWidth * 0.5, m_cfg.worldHeight * 0.7, m_cfg.worldDepth * 0.7), 18.0});

    // Initialize target in center
    m_target = Vec3(m_cfg.worldWidth / 2.0, m_cfg.worldHeight / 2.0, m_cfg.worldDepth / 2.0);
    m_time = 0.0;
    m_tick = 0;
}

void SwarmSimulation3D::updateTarget() {
    // Moving target in 3D space (spiral pattern)
    double radiusXZ = m_cfg.worldWidth * 0.2;
    double radiusY = m_cfg.worldHeight * 0.15;
    
    m_target.x = m_cfg.worldWidth / 2.0 + radiusXZ * std::cos(m_time * 0.3);
    m_target.y = m_cfg.worldHeight / 2.0 + radiusY * std::sin(m_time * 0.4);
    m_target.z = m_cfg.worldDepth / 2.0 + radiusXZ * std::sin(m_time * 0.3);
}

void SwarmSimulation3D::applyWorldBounds(Vec3& p) const {
    // Hard boundaries (no wrap)
    if (p.x < 0.0) p.x = 0.0;
    if (p.x > m_cfg.worldWidth) p.x = m_cfg.worldWidth;
    if (p.y < 0.0) p.y = 0.0;
    if (p.y > m_cfg.worldHeight) p.y = m_cfg.worldHeight;
    if (p.z < 0.0) p.z = 0.0;
    if (p.z > m_cfg.worldDepth) p.z = m_cfg.worldDepth;
}

void SwarmSimulation3D::step() {
    updateTarget();
    m_time += m_cfg.dt;

    std::vector<Vec3> nextVel(m_drones.size());
    std::vector<Vec3> nextPos(m_drones.size());
    std::vector<double> nextHealth(m_drones.size());

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        if (m_drones[i].health <= 0.0) {
            nextVel[i] = m_drones[i].velocity;
            nextPos[i] = m_drones[i].position;
            nextHealth[i] = m_drones[i].health;
            continue;
        }

        const Vec3 curPos = m_drones[i].position;
        const Vec3 curVel = m_drones[i].velocity;
        Vec3 acc = Vec3(0.0, 0.0, 0.0);

        Vec3 separation = separationForce(i);
        Vec3 alignment = alignmentForce(i);
        Vec3 cohesion = cohesionForce(i);
        Vec3 seekForce = seekTarget(i);
        Vec3 avoid = avoidObstacles(i);

        acc += separation * m_cfg.weightSeparation;
        acc += alignment * m_cfg.weightAlignment;
        acc += cohesion * m_cfg.weightCohesion;
        acc += seekForce * m_cfg.weightTarget;
        acc += avoid * m_cfg.weightObstacle;

        acc = acc.limit(m_cfg.maxForce);

        Vec3 vel = curVel + acc * m_cfg.dt;
        vel = vel.limit(m_cfg.maxSpeed);

        Vec3 pos = curPos + vel * m_cfg.dt;
        applyWorldBounds(pos);

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

Vec3 SwarmSimulation3D::separationForce(std::size_t i) const {
    const Vec3 pos = m_drones[i].position;
    const Vec3 vel = m_drones[i].velocity;

    Vec3 steer;
    int count = 0;

    for (std::size_t j = 0; j < m_drones.size(); ++j) {
        if (i == j) continue;
        if (m_drones[j].health <= 0.0) continue;

        Vec3 diff = pos - m_drones[j].position;
        double d = diff.magnitude();
        
        if (d > 0.0 && d < m_cfg.separationRadius) {
            steer += diff.normalized() * (1.0 / (d + 0.1));
            count++;
        }
    }

    if (count == 0) return Vec3(0.0, 0.0, 0.0);

    steer = steer * (1.0 / count);
    steer = steer.normalized() * m_cfg.maxSpeed - vel;
    return steer.limit(m_cfg.maxForce);
}

Vec3 SwarmSimulation3D::alignmentForce(std::size_t i) const {
    const Vec3 pos = m_drones[i].position;
    const Vec3 vel = m_drones[i].velocity;

    Vec3 sum;
    int count = 0;

    for (std::size_t j = 0; j < m_drones.size(); ++j) {
        if (i == j) continue;
        if (m_drones[j].health <= 0.0) continue;

        double d = pos.distance(m_drones[j].position);
        if (d > 0.0 && d < m_cfg.alignmentRadius) {
            sum += m_drones[j].velocity;
            count++;
        }
    }

    if (count == 0) return Vec3(0.0, 0.0, 0.0);

    sum = sum * (1.0 / count);
    sum = sum.normalized() * m_cfg.maxSpeed;
    return (sum - vel).limit(m_cfg.maxForce);
}

Vec3 SwarmSimulation3D::cohesionForce(std::size_t i) const {
    const Vec3 pos = m_drones[i].position;
    const Vec3 curVel = m_drones[i].velocity;

    Vec3 sum;
    int count = 0;

    for (std::size_t j = 0; j < m_drones.size(); ++j) {
        if (i == j) continue;
        if (m_drones[j].health <= 0.0) continue;

        double d = pos.distance(m_drones[j].position);
        if (d > 0.0 && d < m_cfg.cohesionRadius) {
            sum += m_drones[j].position;
            count++;
        }
    }

    if (count == 0) return Vec3(0.0, 0.0, 0.0);

    Vec3 center = sum * (1.0 / count);
    return seek(center, pos, curVel);
}

Vec3 SwarmSimulation3D::seekTarget(std::size_t i) const {
    const Vec3 pos = m_drones[i].position;
    const Vec3 vel = m_drones[i].velocity;
    
    Vec3 desired = (m_target - pos).normalized() * m_cfg.maxSpeed;
    return (desired - vel).limit(m_cfg.maxForce);
}

Vec3 SwarmSimulation3D::avoidObstacles(std::size_t i) const {
    const Vec3 pos = m_drones[i].position;
    Vec3 steer;
    
    for (const auto& obs : m_obstacles) {
        Vec3 diff = pos - obs.position;
        double d = diff.magnitude();
        double safeDistance = obs.radius + m_cfg.obstacleBuffer;
        
        if (d < safeDistance && d > 1e-9) {
            steer += diff.normalized() * (safeDistance - d) / safeDistance;
        }
    }
    
    return steer.limit(m_cfg.maxForce * 2.0);
}

Vec3 SwarmSimulation3D::seek(const Vec3& target, const Vec3& from, const Vec3& currentVelocity) const {
    Vec3 desired = target - from;
    desired = desired.normalized() * m_cfg.maxSpeed;
    return (desired - currentVelocity).limit(m_cfg.maxForce);
}
