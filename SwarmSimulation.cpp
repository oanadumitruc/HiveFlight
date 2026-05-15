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
        m_drones.push_back(d);
    }
}

void SwarmSimulation::applyWrap(Vec2& p) const {
    if (!m_cfg.wrapAround) return;

    if (p.x < 0.0) p.x = m_cfg.worldWidth;
    if (p.x > m_cfg.worldWidth) p.x = 0.0;
    if (p.y < 0.0) p.y = m_cfg.worldHeight;
    if (p.y > m_cfg.worldHeight) p.y = 0.0;
}

void SwarmSimulation::step() {
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

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        const Vec2 curPos = m_drones[i].position;
        const Vec2 curVel = m_drones[i].velocity;

        Vec2 separation = separationForce(i);
        Vec2 alignment = alignmentForce(i);
        Vec2 cohesion = cohesionForce(i);

        Vec2 force = separation * m_cfg.weightSeparation +
                     alignment * m_cfg.weightAlignment +
                     cohesion * m_cfg.weightCohesion;

        Vec2 vel = curVel + force * m_cfg.dt;


        // limit speed
        double sp = vel.magnitude();
        if (sp > m_cfg.maxSpeed) {
            vel = vel.normalized() * m_cfg.maxSpeed;
        }

        Vec2 pos = curPos + vel * m_cfg.dt;
        applyWrap(pos);

        nextVel[i] = vel;
        nextPos[i] = pos;
    }

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        m_drones[i].velocity = nextVel[i];
        m_drones[i].position = nextPos[i];
    }
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
            steer = steer + diff.normalized() * (1.0 / d);
            count++;
        }
    });


    if (count == 0) return Vec2(0.0, 0.0);

    steer = steer / static_cast<double>(count);
    steer = steer.normalized() * m_cfg.desiredSeparationSpeed - vel;
    return steer;
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
            sum = sum + m_drones[j].velocity;
            count++;
        }
    });


    if (count == 0) return Vec2(0.0, 0.0);

    sum = sum / static_cast<double>(count);
    sum = sum.normalized() * m_cfg.desiredSeparationSpeed;
    return sum - vel;
}

Vec2 SwarmSimulation::cohesionForce(std::size_t i) const {
    const Vec2 pos = m_drones[i].position;
    const Vec2 from = m_drones[i].position;
    const Vec2 curVel = m_drones[i].velocity;

    Vec2 sum(0.0, 0.0);
    std::size_t count = 0;

    const double r = m_cfg.cohesionRadius;

    m_grid.forEachCandidate(pos, m_cfg.wrapAround, [&](std::size_t j) {
        if (j == i) return;
        double d = (pos - m_drones[j].position).magnitude();

        if (d > 0.0 && d < r) {
            sum = sum + m_drones[j].position;
            count++;
        }
    });


    if (count == 0) return Vec2(0.0, 0.0);

    Vec2 center = sum / static_cast<double>(count);
    return seek(center, pos, curVel);
}

Vec2 SwarmSimulation::seek(const Vec2& target, const Vec2& from, const Vec2& currentVelocity) const {
    Vec2 desired = target - from;
    desired = desired.normalized() * m_cfg.desiredSeparationSpeed;
    return desired - currentVelocity;
}

