#include "SwarmSimulation3D.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

// ---------------------------------------------------------------------------
// Vec3 method implementations (if not in a separate Vec3.cpp)
// ---------------------------------------------------------------------------

// NOTE: If your project compiles Vec3 separately, guard these with
//       an include guard or move them to Vec3.cpp.

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static double clamp3d(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// ---------------------------------------------------------------------------
// Constructor — the communicator member must be initialized in the member
// initializer list because it holds references to m_drones and m_commStates.
// ---------------------------------------------------------------------------

SwarmSimulation3D::SwarmSimulation3D(const SwarmConfig3D& cfg)
    : m_cfg(cfg)
    , m_communicator(
          m_drones,
          m_commStates,
          cfg.commConfig,
          // TargetIndexFn: captures `this`; called by DroneCommunicator3D
          // to stamp outgoing messages and to evaluate incoming ones.
          [this](std::size_t i) { return assignedTarget(i); })
{
    reset();
}

void SwarmSimulation3D::queueCommunicationMessage(const DroneMessage3D& message) {
    m_communicator.queueExternalMessage(message);
}

// ---------------------------------------------------------------------------
// reset
// ---------------------------------------------------------------------------

void SwarmSimulation3D::reset() {
    m_time = 0.0;
    m_tick = 0;
    m_lastAssignBucket = -1;

    initDrones();
    initObstacles();
    initTargets();
    initializeAssignments();

    // Note: m_commStates and m_cohesionBias are resized lazily in
    // DroneCommunicator3D::syncBuffers() at the start of each broadcastMessages() call.
}

// ---------------------------------------------------------------------------
// Initialization helpers
// ---------------------------------------------------------------------------

void SwarmSimulation3D::initDrones() {
    std::mt19937 gen;
    if (m_cfg.seed != 0)
        gen.seed(m_cfg.seed);
    else {
        std::random_device rd;
        gen.seed(rd());
    }

    const double margin = 20.0;
    std::uniform_real_distribution<double> px(margin, m_cfg.worldWidth  - margin);
    std::uniform_real_distribution<double> py(margin, m_cfg.worldHeight - margin);
    std::uniform_real_distribution<double> pz(margin, m_cfg.worldDepth  - margin);
    std::uniform_real_distribution<double> vd(-8.0, 8.0);

    m_drones.clear();
    m_drones.resize(m_cfg.droneCount);

    for (std::size_t i = 0; i < m_cfg.droneCount; ++i) {
        Drone3D& d = m_drones[i];
        d.id           = i + 1;
        d.position     = Vec3(px(gen), py(gen), pz(gen));
        d.velocity     = Vec3(vd(gen), vd(gen), vd(gen));
        d.acceleration = Vec3(0.0, 0.0, 0.0);
        d.health       = 100.0;
        // Matches the enlarged Gazebo marker used in the demo, so obstacle
        // clearance remains visually believable.
        d.radius       = 3.2;
    }
}

void SwarmSimulation3D::initObstacles() {
    m_obstacles.clear();
    const double w = m_cfg.worldWidth;
    const double h = m_cfg.worldHeight;
    const double d = m_cfg.worldDepth;

    m_obstacles.push_back({Vec3(w * 0.30, h * 0.50, d * 0.40), 12.0});
    m_obstacles.push_back({Vec3(w * 0.70, h * 0.40, d * 0.60), 10.0});
    m_obstacles.push_back({Vec3(w * 0.50, h * 0.60, d * 0.25), 14.0});
    m_obstacles.push_back({Vec3(w * 0.20, h * 0.35, d * 0.70), 9.0});
    m_obstacles.push_back({Vec3(w * 0.80, h * 0.65, d * 0.30), 11.0});
}

void SwarmSimulation3D::initTargets() {
    const std::size_t n = std::max<std::size_t>(1, m_cfg.targetCount);
    m_targets.clear();
    m_targets.resize(n);

    const double cx = m_cfg.worldWidth  * 0.5;
    const double cy = m_cfg.worldHeight * 0.5;
    const double cz = m_cfg.worldDepth  * 0.5;
    const double rx = m_cfg.worldWidth  * 0.28;
    const double rz = m_cfg.worldDepth  * 0.28;
    const double kPi = 3.14159265358979323846;

    for (std::size_t t = 0; t < n; ++t) {
        const double phase = 2.0 * kPi * static_cast<double>(t) / static_cast<double>(n);
        m_targets[t] = Vec3(cx + rx * std::cos(phase),
                            cy,
                            cz + rz * std::sin(phase));
    }
}

void SwarmSimulation3D::initializeAssignments() {
    const std::size_t nt = std::max<std::size_t>(1, m_targets.size());
    m_targetAssignment.assign(m_drones.size(), 0);

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        double     bestDist = (m_targets[0] - m_drones[i].position).magnitude();
        std::size_t bestT   = 0;
        for (std::size_t t = 1; t < nt; ++t) {
            const double d = (m_targets[t] - m_drones[i].position).magnitude();
            if (d < bestDist) { bestDist = d; bestT = t; }
        }
        m_targetAssignment[i] = bestT;
    }
}

// ---------------------------------------------------------------------------
// assignedTarget — public accessor also used by the TargetIndexFn lambda
// ---------------------------------------------------------------------------

std::size_t SwarmSimulation3D::assignedTarget(std::size_t droneIndex) const {
    if (m_targetAssignment.size() != m_drones.size()) return 0;
    if (m_targets.empty()) return 0;
    return std::min(m_targetAssignment[droneIndex], m_targets.size() - 1);
}

// ---------------------------------------------------------------------------
// Communication stats accessors
// ---------------------------------------------------------------------------

std::size_t SwarmSimulation3D::commMessagesLastFrame() const {
    return m_communicator.lastDeliveredMessages();
}

bool SwarmSimulation3D::commEnabled() const {
    return m_communicator.config().enableCommunication;
}

void SwarmSimulation3D::toggleComm() {
    m_communicator.config().enableCommunication =
        !m_communicator.config().enableCommunication;
}

// ---------------------------------------------------------------------------
// updateTargets — orbiting motion
// ---------------------------------------------------------------------------

void SwarmSimulation3D::updateTargets() {
    const double kPi = 3.14159265358979323846;
    const double cx = m_cfg.worldWidth  * 0.5;
    const double cy = m_cfg.worldHeight * 0.5;
    const double cz = m_cfg.worldDepth  * 0.5;
    const double rx = m_cfg.worldWidth  * 0.28;
    const double rz = m_cfg.worldDepth  * 0.28;
    const std::size_t n = m_targets.size();

    for (std::size_t t = 0; t < n; ++t) {
        const double speed = (0.20 + 0.04 * static_cast<double>(t))
                           * m_cfg.targetMotionSpeedMultiplier;
        const double phaseX = m_time * speed + 2.0 * kPi * static_cast<double>(t) / static_cast<double>(n);
        const double phaseZ = m_time * speed * 0.8 + 2.0 * kPi * static_cast<double>(t) / static_cast<double>(n) + 0.6;
        const double phaseY = m_time * 0.12 * m_cfg.targetMotionSpeedMultiplier
                            + static_cast<double>(t) * 1.1;

        m_targets[t] = Vec3(
            cx + rx * std::cos(phaseX),
            cy + m_cfg.worldHeight * 0.18 * std::sin(phaseY),
            cz + rz * std::sin(phaseZ));
    }
}

// ---------------------------------------------------------------------------
// updateAssignmentsIfNeeded — periodic nearest-target reassignment
// ---------------------------------------------------------------------------

void SwarmSimulation3D::updateAssignmentsIfNeeded() {
    if (m_cfg.reassignmentInterval <= 0.0) return;
    if (m_targets.empty() || m_drones.empty()) return;

    const int bucket = static_cast<int>(m_time / m_cfg.reassignmentInterval);
    if (bucket == m_lastAssignBucket) return;
    m_lastAssignBucket = bucket;

    const std::size_t nt = m_targets.size();
    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        if (m_drones[i].health <= 0.0) continue;

        double     best = (m_targets[0] - m_drones[i].position).magnitude();
        std::size_t bestT = 0;
        for (std::size_t t = 1; t < nt; ++t) {
            const double d = (m_targets[t] - m_drones[i].position).magnitude();
            if (d < best) { best = d; bestT = t; }
        }
        m_targetAssignment[i] = bestT;
    }
}

// ---------------------------------------------------------------------------
// applyBounds — clamp or reflect at world edges (no wrap-around in 3D)
// ---------------------------------------------------------------------------

void SwarmSimulation3D::applyBounds(Vec3& p) const {
    const double margin = 2.0;
    p.x = clamp3d(p.x, margin, m_cfg.worldWidth  - margin);
    p.y = clamp3d(p.y, margin, m_cfg.worldHeight - margin);
    p.z = clamp3d(p.z, margin, m_cfg.worldDepth  - margin);
}

// ---------------------------------------------------------------------------
// step — main simulation tick
// ---------------------------------------------------------------------------

void SwarmSimulation3D::step() {
    m_time += m_cfg.dt;

    // 1. Move targets
    updateTargets();

    // 2. Periodic target reassignment
    updateAssignmentsIfNeeded();

    // 3. Rebuild spatial grid for flocking neighbor queries
    const double gridCell = m_cfg.cohesionRadius;
    m_grid.init(m_cfg.worldWidth, m_cfg.worldHeight, m_cfg.worldDepth, gridCell);
    m_grid.clear();
    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        if (m_drones[i].health > 0.0)
            m_grid.insert(i, m_drones[i], /*wrapAround=*/false);
    }

    // 4. Communication broadcast — fills inboxes, stamps with current target
    m_communicator.broadcastMessages(m_time);

    // 5. Communication processing — builds cohesionBias per drone
    m_communicator.processMessages();
    const std::vector<Vec3>& commBias = m_communicator.cohesionBias();

    // 6. Compute steering forces and integrate (double-buffered: read old state, write new)
    std::vector<Vec3>   nextVel(m_drones.size());
    std::vector<Vec3>   nextPos(m_drones.size());
    std::vector<double> nextHealth(m_drones.size());

    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        if (m_drones[i].health <= 0.0) {
            nextVel[i]    = m_drones[i].velocity;
            nextPos[i]    = m_drones[i].position;
            nextHealth[i] = 0.0;
            continue;
        }

        Vec3 acc(0.0, 0.0, 0.0);

        acc += separationForce(i) * m_cfg.weightSeparation;
        acc += alignmentForce(i)  * m_cfg.weightAlignment;
        acc += cohesionForce(i)   * m_cfg.weightCohesion;
        acc += seekTarget(i)      * (m_cfg.weightTarget * 2.0);
        acc += avoidObstacles(i)  * m_cfg.weightObstacle;

        // 7. Inject communication cohesion bias
        acc += commBias[i] * m_cfg.weightComm;

        acc = acc.limit(m_cfg.maxForce);

        Vec3 vel = m_drones[i].velocity + acc * m_cfg.dt;
        vel = vel.limit(m_cfg.maxSpeed);

        Vec3 pos = m_drones[i].position + vel * m_cfg.dt;
        applyBounds(pos);

        double health = m_drones[i].health
                        - vel.magnitude() * m_cfg.batteryDrainRate * m_cfg.dt;
        health = std::max(health, 0.0);

        nextVel[i]    = vel;
        nextPos[i]    = pos;
        nextHealth[i] = health;
    }

    // Commit
    for (std::size_t i = 0; i < m_drones.size(); ++i) {
        const Vec3 previousVelocity = m_drones[i].velocity;
        m_drones[i].velocity     = nextVel[i];
        m_drones[i].position     = nextPos[i];
        m_drones[i].acceleration = (nextVel[i] - previousVelocity) / m_cfg.dt;
        m_drones[i].health       = nextHealth[i];
    }

    ++m_tick;
}

// ---------------------------------------------------------------------------
// Steering forces
// ---------------------------------------------------------------------------

Vec3 SwarmSimulation3D::separationForce(std::size_t i) const {
    const Vec3        pos   = m_drones[i].position;
    const Vec3        vel   = m_drones[i].velocity;
    const std::size_t myT   = assignedTarget(i);
    const double      r     = m_cfg.separationRadius;

    Vec3        steer(0.0, 0.0, 0.0);
    std::size_t count = 0;

    m_grid.forEachCandidate(pos, /*wrapAround=*/false, [&](std::size_t j) {
        if (j == i) return;
        if (assignedTarget(j) != myT) return;

        Vec3   diff = pos - m_drones[j].position;
        double d    = diff.magnitude();
        if (d > 0.0 && d < r) {
            steer += diff.normalized() * (1.0 / d);
            ++count;
        }
    });

    if (count == 0) return Vec3(0.0, 0.0, 0.0);

    steer = steer / static_cast<double>(count);
    steer = steer.normalized() * m_cfg.maxSpeed - vel;
    return steer.limit(m_cfg.maxForce);
}

Vec3 SwarmSimulation3D::alignmentForce(std::size_t i) const {
    const Vec3        pos = m_drones[i].position;
    const Vec3        vel = m_drones[i].velocity;
    const std::size_t myT = assignedTarget(i);
    const double      r   = m_cfg.alignmentRadius;

    Vec3        sum(0.0, 0.0, 0.0);
    std::size_t count = 0;

    m_grid.forEachCandidate(pos, false, [&](std::size_t j) {
        if (j == i) return;
        if (assignedTarget(j) != myT) return;

        double d = (pos - m_drones[j].position).magnitude();
        if (d > 0.0 && d < r) {
            sum += m_drones[j].velocity;
            ++count;
        }
    });

    if (count == 0) return Vec3(0.0, 0.0, 0.0);

    sum = sum / static_cast<double>(count);
    sum = sum.normalized() * m_cfg.maxSpeed;
    return (sum - vel).limit(m_cfg.maxForce);
}

Vec3 SwarmSimulation3D::cohesionForce(std::size_t i) const {
    const Vec3        pos = m_drones[i].position;
    const Vec3        vel = m_drones[i].velocity;
    const std::size_t myT = assignedTarget(i);
    const double      r   = m_cfg.cohesionRadius;

    Vec3        sum(0.0, 0.0, 0.0);
    std::size_t count = 0;

    m_grid.forEachCandidate(pos, false, [&](std::size_t j) {
        if (j == i) return;
        if (assignedTarget(j) != myT) return;

        double d = (pos - m_drones[j].position).magnitude();
        if (d > 0.0 && d < r) {
            sum += m_drones[j].position;
            ++count;
        }
    });

    if (count == 0) return Vec3(0.0, 0.0, 0.0);

    Vec3 center = sum / static_cast<double>(count);
    return seek(center, pos, vel);
}

Vec3 SwarmSimulation3D::seekTarget(std::size_t i) const {
    if (m_targets.empty()) return Vec3(0.0, 0.0, 0.0);

    const Vec3   pos    = m_drones[i].position;
    const Vec3   vel    = m_drones[i].velocity;
    const Vec3&  target = m_targets[assignedTarget(i)];

    Vec3   toTarget = target - pos;
    double dist     = toTarget.magnitude();
    if (dist < 1e-9) return Vec3(0.0, 0.0, 0.0);

    Vec3 desired = toTarget.normalized() * m_cfg.maxSpeed;
    return (desired - vel).limit(m_cfg.maxForce);
}

Vec3 SwarmSimulation3D::avoidObstacles(std::size_t i) const {
    const Vec3 pos = m_drones[i].position;
    Vec3 steer(0.0, 0.0, 0.0);

    for (const auto& obs : m_obstacles) {
        Vec3   diff = pos - obs.position;
        double d    = diff.magnitude();
        double safe = obs.radius + m_cfg.obstacleBuffer + m_drones[i].radius;
        if (d < safe && d > 1e-9) {
            steer += diff.normalized() * ((safe - d) / safe);
        }
    }

    return steer.limit(m_cfg.maxForce * 2.0);
}

Vec3 SwarmSimulation3D::seek(const Vec3& target, const Vec3& from, const Vec3& vel) const {
    Vec3 desired = (target - from).normalized() * m_cfg.maxSpeed;
    return (desired - vel).limit(m_cfg.maxForce);
}
