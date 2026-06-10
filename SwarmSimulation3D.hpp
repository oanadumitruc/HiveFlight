#pragma once

#include <cstddef>
#include <vector>
#include <string>

#include "Vec3.hpp"
#include "SpatialGrid3D.hpp"
#include "DroneCommunication3D.hpp"

// ---------------------------------------------------------------------------
// Primitive types
// ---------------------------------------------------------------------------

struct Drone3D {
    std::size_t id     = 0;
    Vec3        position;
    Vec3        velocity;
    Vec3        acceleration;
    double      health = 100.0;
    double      radius = 2.0;
};

struct Obstacle3D {
    Vec3   position;
    double radius = 10.0;
};

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

struct SwarmConfig3D {
    // World dimensions
    double worldWidth  = 200.0;
    double worldHeight = 200.0;
    double worldDepth  = 150.0;

    // Swarm
    std::size_t droneCount  = 30;
    std::size_t targetCount = 3;
    unsigned int seed       = 42;

    // Target reassignment interval (seconds; 0 = never)
    double reassignmentInterval = 6.0;

    // Flocking radii
    double separationRadius = 12.0;
    double alignmentRadius  = 28.0;
    double cohesionRadius   = 28.0;

    // Steering weights
    double weightSeparation = 1.8;
    double weightAlignment  = 1.0;
    double weightCohesion   = 1.0;
    double weightTarget     = 0.7;
    double weightObstacle   = 3.0;
    // Weight for communication-derived cohesion bias (applied on top of normal cohesion)
    double weightComm       = 1.0;

    // Limits
    double maxSpeed = 28.0;
    double maxForce = 8.0;

    // Battery
    double batteryDrainRate = 0.0015;

    // Obstacle avoidance buffer
    double obstacleBuffer = 3.5;

    // Timestep
    double dt = 0.016; // ~60 FPS

    // Communication subsystem
    CommunicationConfig3D commConfig;  // default: radius=30, enabled=true, weight=0.35

    // Viewer hint
    bool showVelocityVectors = true;
};

// ---------------------------------------------------------------------------
// SwarmSimulation3D
// ---------------------------------------------------------------------------

class SwarmSimulation3D {
public:
    explicit SwarmSimulation3D(const SwarmConfig3D& cfg);

    // --- Main simulation tick ---
    void step();

    // --- Reset to initial conditions ---
    void reset();

    // --- Read-only accessors (used by renderer and viewer) ---
    const std::vector<Drone3D>&    drones()    const { return m_drones; }
    const std::vector<Obstacle3D>& obstacles() const { return m_obstacles; }
    const std::vector<Vec3>&       targets()   const { return m_targets; }

    int    tick() const { return m_tick; }
    double time() const { return m_time; }

    // Target assignment for drone i (used by viewer to colour drones).
    std::size_t assignedTarget(std::size_t droneIndex) const;

    // Communication stats for overlay.
    std::size_t commMessagesLastFrame() const;
    bool        commEnabled()           const;
    // Toggle communication on/off at runtime (keyboard shortcut 'C').
    void        toggleComm();

private:
    // --- Initialization ---
    void initDrones();
    void initObstacles();
    void initTargets();
    void initializeAssignments();

    // --- Per-step helpers ---
    void updateTargets();
    void updateAssignmentsIfNeeded();
    void applyBounds(Vec3& p) const;

    // --- Steering forces ---
    Vec3 separationForce (std::size_t i) const;
    Vec3 alignmentForce  (std::size_t i) const;
    Vec3 cohesionForce   (std::size_t i) const;
    Vec3 seekTarget      (std::size_t i) const;
    Vec3 avoidObstacles  (std::size_t i) const;
    Vec3 seek(const Vec3& target, const Vec3& from, const Vec3& vel) const;

    // --- Data ---
    SwarmConfig3D              m_cfg;

    std::vector<Drone3D>       m_drones;
    std::vector<Obstacle3D>    m_obstacles;
    std::vector<Vec3>          m_targets;
    std::vector<std::size_t>   m_targetAssignment; // drone i -> target index

    SpatialGrid3D              m_grid;

    // Communication subsystem
    std::vector<DroneCommState3D> m_commStates;
    DroneCommunicator3D           m_communicator;

    double m_time  = 0.0;
    int    m_tick  = 0;
    int    m_lastAssignBucket = -1;
};
