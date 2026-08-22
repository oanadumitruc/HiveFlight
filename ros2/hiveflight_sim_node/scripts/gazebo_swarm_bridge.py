#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from rclpy.executors import MultiThreadedExecutor
from rclpy.callback_groups import ReentrantCallbackGroup
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from geometry_msgs.msg import PoseArray
from gazebo_msgs.srv import SpawnEntity, SetEntityState
from gazebo_msgs.msg import EntityState
import os

THIS_DIR = os.path.dirname(os.path.realpath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(THIS_DIR, ".."))
MODEL_SDF_PATH = os.path.join(REPO_ROOT, "models", "drone_marker", "model.sdf")


class GazeboSwarmBridge(Node):
    def __init__(self):
        super().__init__("gazebo_swarm_bridge")

        self.cb_group = ReentrantCallbackGroup()

        self.spawned = False
        self.spawn_in_progress = False
        self.pending_spawns = 0

        self.spawn_client = self.create_client(
            SpawnEntity, "/spawn_entity", callback_group=self.cb_group
        )
        self.state_client = self.create_client(
            SetEntityState, "/gazebo/set_entity_state", callback_group=self.cb_group
        )
        visualization_qos = QoSProfile(
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
            reliability=ReliabilityPolicy.BEST_EFFORT,
        )

        self.sub = self.create_subscription(
            PoseArray,
            "/hiveflight/swarm_0/drone_poses",
            self.on_poses,
            visualization_qos,
            callback_group=self.cb_group,
        )

        self.get_logger().info("Gazebo swarm bridge started, waiting for PoseArray...")

    def _load_sdf_xml(self):
        if not os.path.exists(MODEL_SDF_PATH):
            self.get_logger().error(f"SDF model not found at: {MODEL_SDF_PATH}")
            return None
        with open(MODEL_SDF_PATH, "r", encoding="utf-8") as f:
            return f.read()

    def spawn_all(self, count: int):
        if self.spawned or self.spawn_in_progress:
            return

        if not self.spawn_client.service_is_ready():
            return

        sdf_xml = self._load_sdf_xml()
        if sdf_xml is None:
            return

        self.spawn_in_progress = True
        self.pending_spawns = count

        for i in range(count):
            req = SpawnEntity.Request()
            req.name = f"drone_{i}"
            req.xml = sdf_xml
            req.initial_pose.position.x = float(i)
            req.initial_pose.position.y = 0.0
            req.initial_pose.position.z = 0.0
            req.reference_frame = "world"

            future = self.spawn_client.call_async(req)
            future.add_done_callback(self._on_spawn_done)

        self.get_logger().info(f"Requested spawn of {count} drone markers")

    def _on_spawn_done(self, future):
        self.pending_spawns -= 1
        try:
            result = future.result()
            if not result.success:
                self.get_logger().warn(f"Spawn failed: {result.status_message}")
        except Exception as e:
            self.get_logger().error(f"Spawn call raised exception: {e}")

        if self.pending_spawns <= 0:
            self.spawned = True
            self.spawn_in_progress = False
            self.get_logger().info("All drone markers spawned, switching to pose updates")

    def on_poses(self, msg: PoseArray):
        if not self.spawned:
            self.spawn_all(len(msg.poses))
            return

        if not self.state_client.service_is_ready():
            return

        for i, pose in enumerate(msg.poses):
            state = EntityState()
            state.name = f"drone_{i}"
            state.pose = pose
            state.reference_frame = "world"

            req = SetEntityState.Request()
            req.state = state

            future = self.state_client.call_async(req)
            future.add_done_callback(self._on_state_set_done)

    def _on_state_set_done(self, future):
        try:
            result = future.result()
            if not result.success:
                self.get_logger().warn(
                    "set_entity_state returned success=False",
                    throttle_duration_sec=5.0,
                )
        except Exception as e:
            self.get_logger().error(
                f"set_entity_state call raised exception: {e}",
                throttle_duration_sec=5.0,
            )


def main():
    rclpy.init()
    node = GazeboSwarmBridge()
    executor = MultiThreadedExecutor(num_threads=4)
    executor.add_node(node)
    try:
        executor.spin()
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
