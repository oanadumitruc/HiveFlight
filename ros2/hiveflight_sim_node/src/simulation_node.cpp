#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_array.hpp"
#include "hiveflight_interfaces/msg/drone_message.hpp"
#include "DroneRos2Adapter.hpp"
#include "SwarmSimulation3D.hpp"

#include <memory>
#include <vector>
#include <chrono>
#include <cmath>


class HiveflightSimNode : public rclcpp::Node {
public:
  static std::shared_ptr<HiveflightSimNode> create() {
    auto node = std::shared_ptr<HiveflightSimNode>(new HiveflightSimNode());
    node->post_init();
    return node;
  }

private:
  HiveflightSimNode() : Node("hiveflight_sim_node") {
    // Declare ROS2 parameters (tunable without recompiling)
    this->declare_parameter("drone_count",          30);
    this->declare_parameter("target_count",          3);
    this->declare_parameter("seed",                 42);
    this->declare_parameter("world_width",         200.0);
    this->declare_parameter("world_height",        200.0);
    this->declare_parameter("world_depth",         150.0);
    this->declare_parameter("max_speed",            28.0);
    this->declare_parameter("weight_separation",     1.8);
    this->declare_parameter("weight_cohesion",       1.0);
    this->declare_parameter("weight_alignment",      1.0);
    this->declare_parameter("weight_target",         0.7);
    this->declare_parameter("swarm_id",              0);
    this->declare_parameter("tick_ms",              16);   // ~60 Hz
  }

  void post_init() {
    // Build SwarmConfig3D from ROS2 parameters
    SwarmConfig3D cfg;

    // Publisher for drone positions so Gazebo bridge can spawn/update markers.
    // NOTE: We intentionally publish only poses to keep message size small.
    cfg.droneCount   = static_cast<std::size_t>(this->get_parameter("drone_count").as_int());
    cfg.targetCount  = static_cast<std::size_t>(this->get_parameter("target_count").as_int());
    cfg.seed         = static_cast<unsigned int>(this->get_parameter("seed").as_int());
    cfg.worldWidth   = this->get_parameter("world_width").as_double();
    cfg.worldHeight  = this->get_parameter("world_height").as_double();
    cfg.worldDepth   = this->get_parameter("world_depth").as_double();
    cfg.maxSpeed     = this->get_parameter("max_speed").as_double();
    cfg.weightSeparation = this->get_parameter("weight_separation").as_double();
    cfg.weightCohesion   = this->get_parameter("weight_cohesion").as_double();
    cfg.weightAlignment  = this->get_parameter("weight_alignment").as_double();
    cfg.weightTarget     = this->get_parameter("weight_target").as_double();

    m_sim = std::make_unique<SwarmSimulation3D>(cfg);

    const std::size_t swarmId =
      static_cast<std::size_t>(this->get_parameter("swarm_id").as_int());
    const int tickMs = static_cast<int>(this->get_parameter("tick_ms").as_int());
    const std::string topicPrefix = "/hiveflight/swarm_" + std::to_string(swarmId);
    rclcpp::QoS visualizationQos(rclcpp::KeepLast(1));
    visualizationQos.best_effort();
    m_pose_pub = this->create_publisher<geometry_msgs::msg::PoseArray>(
      topicPrefix + "/drone_poses", visualizationQos);
    m_target_pub = this->create_publisher<geometry_msgs::msg::PoseArray>(
      topicPrefix + "/target_poses", visualizationQos);

    m_adapter = std::make_shared<DroneRos2Adapter>(
      cfg.droneCount,
      swarmId,
      m_targetIndexByDrone,
      shared_from_this()
    );

    m_timer = this->create_wall_timer(
      std::chrono::milliseconds(tickMs),
      [this]() { tick(); }
    );

    RCLCPP_INFO(this->get_logger(),
      "HiveFlight node started — %zu drones, %zu targets, swarm_id=%zu",
      cfg.droneCount, cfg.targetCount, swarmId);
  }

  void tick() {
    m_adapter->spinOnce();
    for (const auto& message : m_adapter->takePendingMessages()) {
      m_sim->queueCommunicationMessage(message);
    }
    m_sim->step();
    m_adapter->publishBroadcasts(m_sim->commStates(), m_sim->time());

    geometry_msgs::msg::PoseArray poses;
    poses.header.stamp = this->now();
    poses.header.frame_id = "world";
    poses.poses.reserve(m_sim->drones().size());

    for (const auto& d : m_sim->drones()) {
      geometry_msgs::msg::Pose p;
      p.position.x = static_cast<double>(d.position.x);
      p.position.y = static_cast<double>(d.position.y);
      p.position.z = static_cast<double>(d.position.z);
      const double horizontalSpeed = std::hypot(d.velocity.x, d.velocity.y);
      const double speed = std::hypot(horizontalSpeed, d.velocity.z);
      if (speed > 1e-6) {
        const double yaw = std::atan2(d.velocity.y, d.velocity.x);
        const double pitch = std::atan2(-d.velocity.z, horizontalSpeed);
        const double halfYaw = yaw * 0.5;
        const double halfPitch = pitch * 0.5;
        p.orientation.x = -std::sin(halfYaw) * std::sin(halfPitch);
        p.orientation.y = std::cos(halfYaw) * std::sin(halfPitch);
        p.orientation.z = std::sin(halfYaw) * std::cos(halfPitch);
        p.orientation.w = std::cos(halfYaw) * std::cos(halfPitch);
      } else {
        p.orientation.w = 1.0;
      }
      poses.poses.push_back(p);
    }

    m_pose_pub->publish(poses);

    geometry_msgs::msg::PoseArray targets;
    targets.header = poses.header;
    targets.poses.reserve(m_sim->targets().size());
    for (const auto& target : m_sim->targets()) {
      geometry_msgs::msg::Pose pose;
      pose.position.x = target.x;
      pose.position.y = target.y;
      pose.position.z = target.z;
      pose.orientation.w = 1.0;
      targets.poses.push_back(pose);
    }
    m_target_pub->publish(targets);






    // Log summary every 60 ticks (~1 second)
    if (m_sim->tick() % 60 == 0) {
      const auto& drones = m_sim->drones();
      std::size_t alive = 0;
      for (const auto& d : drones) if (d.health > 0.0) ++alive;

      RCLCPP_INFO(this->get_logger(),
        "tick=%d  t=%.2fs  alive=%zu/%zu  comm_msgs=%zu  comm=%s",
        m_sim->tick(),
        m_sim->time(),
        alive,
        drones.size(),
        m_sim->commMessagesLastFrame(),
        m_sim->commEnabled() ? "ON" : "OFF");
    }
  }

  std::unique_ptr<SwarmSimulation3D>  m_sim;
  std::shared_ptr<DroneRos2Adapter>   m_adapter;
  rclcpp::TimerBase::SharedPtr        m_timer;
  std::vector<std::size_t>            m_targetIndexByDrone;
  rclcpp::Publisher<geometry_msgs::msg::PoseArray>::SharedPtr m_pose_pub;
  rclcpp::Publisher<geometry_msgs::msg::PoseArray>::SharedPtr m_target_pub;

};


int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  auto node = HiveflightSimNode::create();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
