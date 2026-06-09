#pragma once

#include "Vec3.hpp"

#include <cstddef>
#include <vector>


struct Drone3D {
    std::size_t id = 0;
    Vec3 position;
    Vec3 velocity;
    Vec3 acceleration;
    double health = 100.0;
    double radius = 0.5; // For collision detection
};

struct Obstacle3D {
    Vec3 position;
    double radius;
};

struct SwarmConfig3D {
    // World dimensions
    double worldWidth = 200.0;
    double worldHeight = 200.0;
    double worldDepth = 150.0;

    // Swarm
    std::size_t droneCount = 30;
    std::size_t targetCount = 3;
    unsigned int seed = 42;

    // Swarm splitting (drone -> target assignment)
    // Drones are assigned to a specific target and flock only with drones assigned to the same target.
    // Periodically reassigned so the split can follow moving targets.
    double reassignmentInterval = 5.0; // seconds (0 => never reassign)

    // Initialization
    double posMin = 20.0;
    double posMax = 180.0;
    double velMin = -18.0;
    double velMax = 18.0;

    // Physics
    double dt = 0.016; // ~60 FPS
    double maxSpeed = 70.0;
    double maxForce = 20.0;

    // Perception
    double separationRadius = 30.0;
    double alignmentRadius = 80.0;
    double cohesionRadius = 80.0;

    // Force weights
    double weightSeparation = 1.8;
    double weightAlignment = 1.0;
    double weightCohesion = 1.0;
    double weightTarget = 0.8;
    double weightObstacle = 2.5;

    // Battery
    double batteryDrainRate = 0.001;

    // Obstacle
    double obstacleBuffer = 3.0;

    // Rendering
    bool renderTarget = true;
    bool showVelocityVectors = false;
};

class SwarmSimulation3D {
public:
    explicit SwarmSimulation3D(const SwarmConfig3D& cfg);

    void reset();
    void step();

    const std::vector<Drone3D>& drones() const { return m_drones; }
    const std::vector<Obstacle3D>& obstacles() const { return m_obstacles; }
    const std::vector<Vec3>& targets() const { return m_targets; }

    // Backwards compatible “first target” accessor used by some viewers.
    const Vec3& target() const {
        static const Vec3 dummyTarget(0.0, 0.0, 0.0);
        return m_targets.empty() ? dummyTarget : m_targets[0];
    }

    // Needed by the OpenGL viewer to color each drone by its swarm/target.
    std::size_t assignedTarget(std::size_t droneIndex) const {
        if (m_targetAssignment.size() != m_drones.size()) return 0;
        if (m_targets.empty()) return 0;
        std::size_t t = m_targetAssignment[droneIndex];
        if (t >= m_targets.size()) t = 0;
        return t;
    }

    const SwarmConfig3D& config() const { return m_cfg; }
    double time() const { return m_time; }
    int tick() const { return m_tick; }

private:
    void updateTarget();

    void initializeAssignments();
    void updateAssignmentsIfNeeded();

    Vec3 separationForce(std::size_t i) const;
    Vec3 alignmentForce(std::size_t i) const;
    Vec3 cohesionForce(std::size_t i) const;
    Vec3 seekTarget(std::size_t i) const;
    Vec3 avoidObstacles(std::size_t i) const;
    Vec3 seek(const Vec3& target, const Vec3& from, const Vec3& currentVelocity) const;

    void applyWorldBounds(Vec3& p) const;

    SwarmConfig3D m_cfg;
    std::vector<Drone3D> m_drones;
    std::vector<Obstacle3D> m_obstacles;
    std::vector<Vec3> m_targets;

    // For swarm splitting: each drone belongs to exactly one target group.
    std::vector<std::size_t> m_targetAssignment;

    double m_time = 0.0;
    int m_tick = 0;
};

