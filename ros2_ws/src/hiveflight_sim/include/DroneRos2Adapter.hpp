#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// ROS 2 integration is optional.
// When compiling without ROS 2, define HIVEFLIGHT_USE_ROS2=0.
#ifdef HIVEFLIGHT_USE_ROS2
#include "rclcpp/rclcpp.hpp"
#include "hiveflight_interfaces/msg/drone_message.hpp"
#endif




#include "Vec3.hpp"
#include "DroneCommunication3D.hpp"

// Adapter that bridges:
//  - internal simulation comm layer (DroneMessage3D in DroneCommState3D)
//  - ROS 2 topics (hiveflight_interfaces/msg/DroneMessage)
//
// It is intentionally minimal: simulation owns the internal communicator.
// ROS 2 is used only to move DroneMessage payloads across processes.
class DroneRos2Adapter {
public:
    DroneRos2Adapter(std::size_t droneCount,
                      std::size_t swarmId,
                      const std::vector<std::size_t>& targetIndexByDrone
#ifdef HIVEFLIGHT_USE_ROS2
                      , std::shared_ptr<rclcpp::Node> node
#endif
                      );

    // Non-copyable
    DroneRos2Adapter(const DroneRos2Adapter&) = delete;
    DroneRos2Adapter& operator=(const DroneRos2Adapter&) = delete;

    // Called every simulation tick AFTER internal broadcast has populated
    // m_commStates[..].lastBroadcast and cleared inbox.
    void publishBroadcasts(const std::vector<DroneCommState3D>& commStates,
                            double currentTime);

    // Must be called before internal processMessages().
    void spinOnce();
    std::vector<DroneMessage3D> takePendingMessages();

private:
#ifdef HIVEFLIGHT_USE_ROS2
    void onRosMessage(const hiveflight_interfaces::msg::DroneMessage::SharedPtr msg);
#endif

    std::size_t m_droneCount = 0;
    std::size_t m_swarmId = 0;

    const std::vector<std::size_t>& m_targetIndexByDrone;

#ifdef HIVEFLIGHT_USE_ROS2
    std::shared_ptr<rclcpp::Node> m_node;

    rclcpp::Subscription<hiveflight_interfaces::msg::DroneMessage>::SharedPtr m_sub;
    rclcpp::Publisher<hiveflight_interfaces::msg::DroneMessage>::SharedPtr m_pub;
#endif

    // Temporary inbox buffer.
    mutable std::mutex m_pendingMutex;
    std::vector<DroneMessage3D> m_pendingMessages;

    std::atomic<bool> m_gotAny{false};
};


