#include "DroneRos2Adapter.hpp"

#include "DroneCommunication3D.hpp"

#include <algorithm>

DroneRos2Adapter::DroneRos2Adapter(std::size_t droneCount,
                                   std::size_t swarmId,
                                   const std::vector<std::size_t>& targetIndexByDrone
#ifdef HIVEFLIGHT_USE_ROS2
                                   , std::shared_ptr<rclcpp::Node> node
#endif
                                   )

    : m_droneCount(droneCount)
    , m_swarmId(swarmId)
    , m_targetIndexByDrone(targetIndexByDrone)
#ifdef HIVEFLIGHT_USE_ROS2
    , m_node(std::move(node))
#endif
{
#ifdef HIVEFLIGHT_USE_ROS2
    const std::string broadcastTopic = "/hiveflight/swarm_" + std::to_string(m_swarmId) + "/broadcast";

    m_pub = m_node->create_publisher<hiveflight_interfaces::msg::DroneMessage>(broadcastTopic, 10);

    m_sub = m_node->create_subscription<hiveflight_interfaces::msg::DroneMessage>(
        broadcastTopic,
        50,
        [this](const hiveflight_interfaces::msg::DroneMessage::SharedPtr msg) {
            this->onRosMessage(msg);
        });
#endif
}

void DroneRos2Adapter::onRosMessage(const hiveflight_interfaces::msg::DroneMessage::SharedPtr msg) {

    // Convert ROS payload -> internal payload.
    DroneMessage3D m;
    m.senderIndex = static_cast<std::size_t>(msg->sender_id);
    m.position = Vec3(msg->position.x, msg->position.y, msg->position.z);
    m.velocity = Vec3(msg->velocity.x, msg->velocity.y, msg->velocity.z);
    m.targetIndex = static_cast<std::size_t>(msg->target_id);
    m.timestamp = static_cast<double>(msg->timestamp);

    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        m_pendingMessages.push_back(m);
    }
    m_gotAny.store(true, std::memory_order_relaxed);
}

void DroneRos2Adapter::publishBroadcasts(const std::vector<DroneCommState3D>& commStates,
                                          double /*currentTime*/) {

#ifdef HIVEFLIGHT_USE_ROS2
    // Publish each sender's lastBroadcast.

    // Note: The internal communicator already filled lastBroadcast for
    // each sender during broadcastMessages().

    for (std::size_t sender = 0; sender < std::min(m_droneCount, commStates.size()); ++sender) {
        const DroneMessage3D& internal = commStates[sender].lastBroadcast;

        hiveflight_interfaces::msg::DroneMessage rosMsg;
        rosMsg.sender_id = static_cast<int32_t>(internal.senderIndex);
        rosMsg.position.x = static_cast<float>(internal.position.x);
        rosMsg.position.y = static_cast<float>(internal.position.y);
        rosMsg.position.z = static_cast<float>(internal.position.z);
        rosMsg.velocity.x = static_cast<float>(internal.velocity.x);
        rosMsg.velocity.y = static_cast<float>(internal.velocity.y);
        rosMsg.velocity.z = static_cast<float>(internal.velocity.z);
        rosMsg.target_id = static_cast<int32_t>(internal.targetIndex);
        rosMsg.timestamp = static_cast<float>(internal.timestamp);

        m_pub->publish(rosMsg);
    }
#else
    (void)commStates;
#endif
}

void DroneRos2Adapter::spinOnce() {
#ifdef HIVEFLIGHT_USE_ROS2
    // Placeholder for future work.
    // SwarmSimulation3D integration can spin the node/executor.
    (void)m_gotAny;
#else
    // no-op
#endif
}

std::vector<DroneMessage3D> DroneRos2Adapter::takePendingMessages() {
    std::vector<DroneMessage3D> messages;
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        messages.swap(m_pendingMessages);
    }
    m_gotAny.store(false, std::memory_order_relaxed);
    return messages;
}

