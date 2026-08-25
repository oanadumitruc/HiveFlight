// HiveFlight Gazebo world plugin.
//
// Subscribes directly to the swarm PoseArray topics and applies every pose
// to the matching Gazebo model on the physics thread via Model::SetWorldPose.
// This replaces the per-model set_model_state service round-robin in
// gazebo_swarm_bridge.py: one ROS message updates ALL drones each physics
// step, so markers move at (near) simulation rate instead of ~7.5 Hz.
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_array.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace gazebo
{

  class HiveflightPosePlugin : public WorldPlugin
  {
  public:
    void Load(physics::WorldPtr world, sdf::ElementPtr sdf) override
    {
      world_ = world;

      // Optional SDF configuration:
      //   <ros><argument>--ros-args -p swarm_id:=1</argument></ros> is not
      // needed; we read parameters from SDF instead for simplicity.
      if (sdf && sdf->HasElement("swarm_id"))
      {
        swarm_id_ = sdf->Get<int>("swarm_id");
      }
      prefix_ = "/hiveflight/swarm_" + std::to_string(swarm_id_);

      if (!rclcpp::ok())
      {
        rclcpp::init(0, nullptr);
        owns_rclcpp_ = true;
      }
      node_ = rclcpp::Node::make_shared("hiveflight_pose_plugin");

      // Keep only the latest sample; poses are consumed every physics step.
      auto qos = rclcpp::QoS(rclcpp::KeepLast(1)).best_effort();

      drone_sub_ = node_->create_subscription<geometry_msgs::msg::PoseArray>(
          prefix_ + "/drone_poses", qos,
          [this](const geometry_msgs::msg::PoseArray::SharedPtr msg)
          {
            StorePoses(msg->poses, drone_poses_);
          });

      target_sub_ = node_->create_subscription<geometry_msgs::msg::PoseArray>(
          prefix_ + "/target_poses", qos,
          [this](const geometry_msgs::msg::PoseArray::SharedPtr msg)
          {
            StorePoses(msg->poses, target_poses_);
          });

      executor_thread_ = std::thread([this]()
                                     {
      executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
      executor_->add_node(node_);
      executor_->spin(); });

      // Apply poses at every physics iteration — cheap direct pose writes.
      update_conn_ = event::Events::ConnectWorldUpdateBegin(
          std::bind(&HiveflightPosePlugin::OnWorldUpdate, this, std::placeholders::_1));

      gzmsg << "[hiveflight_pose_plugin] loaded, subscribing to " << prefix_
            << "/drone_poses and " << prefix_ << "/target_poses\n";
    }

    ~HiveflightPosePlugin() override
    {
      if (executor_)
      {
        executor_->cancel();
      }
      if (executor_thread_.joinable())
      {
        executor_thread_.join();
      }
      node_.reset();
      if (owns_rclcpp_ && rclcpp::ok())
      {
        rclcpp::shutdown();
      }
    }

  private:
    static ignition::math::Pose3d ToPose(const geometry_msgs::msg::Pose &p)
    {
      return {p.position.x, p.position.y, p.position.z,
              p.orientation.w, p.orientation.x, p.orientation.y, p.orientation.z};
    }

    void StorePoses(const std::vector<geometry_msgs::msg::Pose> &msgs,
                    std::vector<ignition::math::Pose3d> &out)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      out.resize(msgs.size());
      for (size_t i = 0; i < msgs.size(); ++i)
      {
        out[i] = ToPose(msgs[i]);
      }
    }

    void ApplyModels(const std::vector<ignition::math::Pose3d> &poses,
                     const char *model_prefix)
    {
      // NOTE: model colouring is handled by the Python spawner; this plugin
      // only streams poses. The previous target-colour remap read an
      // unordered_map from the physics thread while the ROS executor wrote
      // it - an unsynchronised data race that corrupted the heap and
      // segfaulted gzserver (crash surfaced inside libdraco).
      for (size_t i = 0; i < poses.size(); ++i)
      {
        if (auto model = world_->ModelByName(model_prefix + std::to_string(i)))
        {
          model->SetWorldPose(poses[i]);
        }
      }
    }

    void OnWorldUpdate(const common::UpdateInfo &)
    {
      std::vector<ignition::math::Pose3d> drones;
      std::vector<ignition::math::Pose3d> targets;
      {
        std::lock_guard<std::mutex> lock(mutex_);
        drones = drone_poses_;
        targets = target_poses_;
      }
      if (!drones.empty())
      {
        ApplyModels(drones, "drone_");
      }
      if (!targets.empty())
      {
        ApplyModels(targets, "target_");
      }
    }

    physics::WorldPtr world_;
    event::ConnectionPtr update_conn_;
    rclcpp::Node::SharedPtr node_;
    rclcpp::Subscription<geometry_msgs::msg::PoseArray>::SharedPtr drone_sub_;
    rclcpp::Subscription<geometry_msgs::msg::PoseArray>::SharedPtr target_sub_;
    rclcpp::executors::SingleThreadedExecutor::SharedPtr executor_;
    std::thread executor_thread_;
    std::mutex mutex_;
    std::vector<ignition::math::Pose3d> drone_poses_;
    std::vector<ignition::math::Pose3d> target_poses_;
    std::string prefix_;
    int swarm_id_{0};
    bool owns_rclcpp_{false};
  };

  GZ_REGISTER_WORLD_PLUGIN(HiveflightPosePlugin)
} // namespace gazebo
