#pragma once

#include <cstddef>
#include <functional>
#include <vector>

#include "Vec3.hpp"

// Forward declaration only — full definition in SwarmSimulation3D.hpp.
// DroneCommunication3D.cpp includes SwarmSimulation3D.hpp to resolve it.
struct Drone3D;

// ---------------------------------------------------------------------------
// Wire protocol
// ---------------------------------------------------------------------------

struct DroneMessage3D {
    std::size_t senderIndex = static_cast<std::size_t>(-1);
    Vec3        position;
    Vec3        velocity;
    std::size_t targetIndex = static_cast<std::size_t>(-1);
    double      timestamp   = 0.0;
};

struct DroneCommState3D {
    DroneMessage3D              lastBroadcast{};
    std::vector<DroneMessage3D> inbox;
};

struct CommunicationConfig3D {
    double communicationRadius         = 30.0;
    bool   enableCommunication         = true;
    bool   storeInboxMessages          = true;
    double communicationCohesionWeight = 0.35;
};

// ---------------------------------------------------------------------------
// DroneCommunicator3D
// ---------------------------------------------------------------------------

class DroneCommunicator3D {
public:
    // targetIndexFn(i) returns the assigned target index for drone i.
    // Injected to avoid including SwarmSimulation3D.hpp in this header.
    using TargetIndexFn = std::function<std::size_t(std::size_t)>;

    DroneCommunicator3D(std::vector<Drone3D>&         drones,
                        std::vector<DroneCommState3D>& commStates,
                        const CommunicationConfig3D&   cfg,
                        TargetIndexFn                  targetIndexFn);

    // --- Per-frame call order ---
    // 1. broadcastMessages() — sync buffers, clear inboxes, deliver messages.
    // 2. processMessages()   — compute cohesionBias[] from inbox.
    void broadcastMessages(double currentTime);
    void processMessages();

    // Per-drone communication cohesion bias to add into steering accumulator.
    const std::vector<Vec3>& cohesionBias() const { return m_cohesionBias; }

    // Metrics
    std::size_t lastDeliveredMessages() const { return m_lastDeliveredMessages; }

    // Config access
    CommunicationConfig3D&       config()       { return m_cfg; }
    const CommunicationConfig3D& config() const { return m_cfg; }

private:
    // Resize m_cohesionBias and m_commStates to match m_drones.size().
    // Called at the top of broadcastMessages() every frame — O(1) when
    // sizes already match, so safe to call unconditionally.
    void syncBuffers();

    void   deliverBroadcast(std::size_t senderIndex, double currentTime);
    static double distanceSquared(const Vec3& a, const Vec3& b);

    std::vector<Drone3D>&          m_drones;
    std::vector<DroneCommState3D>& m_commStates;
    CommunicationConfig3D          m_cfg;
    TargetIndexFn                  m_targetIndexFn;

    std::vector<Vec3>  m_cohesionBias;
    std::size_t        m_lastDeliveredMessages = 0;
};
