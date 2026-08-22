#include "DroneCommunication3D.hpp"

// Full Drone3D definition needed to access .position, .velocity, .health.
// Included only in the .cpp to break the circular header chain:
//   SwarmSimulation3D.hpp -> DroneCommunication3D.hpp  (one-way in headers)
//   DroneCommunication3D.cpp -> SwarmSimulation3D.hpp  (safe: .cpp sees both)
#include "SwarmSimulation3D.hpp"

#include <algorithm>
#include <cstddef>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

double DroneCommunicator3D::distanceSquared(const Vec3& a, const Vec3& b) {
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    const double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

// ---------------------------------------------------------------------------
// Constructor
//
// m_drones is EMPTY at this point — SwarmSimulation3D's member initializer list
// runs before the constructor body, and reset()/initDrones() haven't been
// called yet.  We intentionally do NOT resize m_cohesionBias here; that is
// done lazily at the top of broadcastMessages() every frame, so it always
// matches the live drone count even after a reset().
// ---------------------------------------------------------------------------

DroneCommunicator3D::DroneCommunicator3D(std::vector<Drone3D>&         drones,
                                         std::vector<DroneCommState3D>& commStates,
                                         const CommunicationConfig3D&   cfg,
                                         TargetIndexFn                  targetIndexFn)
    : m_drones(drones)
    , m_commStates(commStates)
    , m_cfg(cfg)
    , m_targetIndexFn(std::move(targetIndexFn))
{
    // Intentionally empty — see comment above.
}

// ---------------------------------------------------------------------------
// syncBuffers — called at the top of broadcastMessages() every frame.
// Ensures m_cohesionBias and m_commStates always match m_drones.size().
// This is the single authoritative resize point; it handles initial
// construction, reset(), and any future dynamic drone add/remove.
// ---------------------------------------------------------------------------

void DroneCommunicator3D::syncBuffers() {
    const std::size_t n = m_drones.size();

    if (m_cohesionBias.size() != n)
        m_cohesionBias.assign(n, Vec3(0.0, 0.0, 0.0));

    if (m_commStates.size() != n)
        m_commStates.assign(n, DroneCommState3D{});
}

// ---------------------------------------------------------------------------
// broadcastMessages
// ---------------------------------------------------------------------------

void DroneCommunicator3D::broadcastMessages(double currentTime) {
    // Always sync first — this is safe to call every frame (O(1) when sizes match).
    syncBuffers();

    const std::size_t count = m_drones.size();

    // Reset per-frame accumulators.
    m_lastDeliveredMessages = 0;
    std::fill(m_cohesionBias.begin(), m_cohesionBias.end(), Vec3(0.0, 0.0, 0.0));

    for (std::size_t i = 0; i < count; ++i)
        m_commStates[i].inbox.clear();

    if (!m_cfg.enableCommunication || count == 0)
        return;

    for (std::size_t i = 0; i < count; ++i) {
        if (m_drones[i].health > 0.0)
            deliverBroadcast(i, currentTime);
    }

    const double radiusSq = m_cfg.communicationRadius * m_cfg.communicationRadius;
    for (const DroneMessage3D& message : m_externalMessages) {
        if (message.senderIndex >= count) continue;
        for (std::size_t receiver = 0; receiver < count; ++receiver) {
            if (receiver == message.senderIndex) continue;
            if (m_drones[receiver].health <= 0.0) continue;
            if (distanceSquared(message.position, m_drones[receiver].position) > radiusSq) continue;
            m_commStates[receiver].inbox.push_back(message);
            ++m_lastDeliveredMessages;
        }
    }
    m_externalMessages.clear();
}

void DroneCommunicator3D::queueExternalMessage(const DroneMessage3D& message) {
    m_externalMessages.push_back(message);
}

// ---------------------------------------------------------------------------
// deliverBroadcast
// ---------------------------------------------------------------------------

void DroneCommunicator3D::deliverBroadcast(std::size_t senderIndex, double currentTime) {
    const Drone3D& sender = m_drones[senderIndex];

    DroneMessage3D msg;
    msg.senderIndex = senderIndex;
    msg.position    = sender.position;
    msg.velocity    = sender.velocity;
    msg.targetIndex = m_targetIndexFn(senderIndex);
    msg.timestamp   = currentTime;

    m_commStates[senderIndex].lastBroadcast = msg;

    const double      radiusSq = m_cfg.communicationRadius * m_cfg.communicationRadius;
    const std::size_t count    = m_drones.size();

    for (std::size_t i = 0; i < count; ++i) {
        if (i == senderIndex)           continue;
        if (m_drones[i].health <= 0.0) continue;
        if (distanceSquared(sender.position, m_drones[i].position) > radiusSq) continue;

        m_commStates[i].inbox.push_back(msg);
        ++m_lastDeliveredMessages;
    }
}

// ---------------------------------------------------------------------------
// processMessages
// ---------------------------------------------------------------------------

void DroneCommunicator3D::processMessages() {
    if (!m_cfg.enableCommunication) return;

    const std::size_t count = m_drones.size();
    // Guard: broadcastMessages() must have been called first.
    if (m_cohesionBias.size() != count || m_commStates.size() != count)
        return;

    for (std::size_t i = 0; i < count; ++i) {
        const auto& inbox = m_commStates[i].inbox;
        if (inbox.empty()) continue;

        const std::size_t myTarget = m_targetIndexFn(i);

        Vec3        sumPos(0.0, 0.0, 0.0);
        std::size_t sameTargetCount = 0;

        for (const DroneMessage3D& msg : inbox) {
            if (msg.targetIndex == myTarget) {
                sumPos += msg.position;
                ++sameTargetCount;
            }
        }

        if (sameTargetCount == 0) continue;

        const double inv = 1.0 / static_cast<double>(sameTargetCount);
        Vec3 centroid(sumPos.x * inv, sumPos.y * inv, sumPos.z * inv);

        m_cohesionBias[i] = (centroid - m_drones[i].position)
                            * m_cfg.communicationCohesionWeight;
    }
}
