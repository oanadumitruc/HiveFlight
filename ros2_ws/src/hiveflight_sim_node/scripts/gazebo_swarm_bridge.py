#!/usr/bin/env python3
"""Bridge between HiveFlight PoseArray topics and Gazebo entities.

Spawns drone/target marker models and streams simulated poses into
Gazebo via set_model_state / set_entity_state.

NOTE: This file MUST be saved with LF line endings. A CRLF shebang
("#!/usr/bin/env python3\\r") makes direct execution fail under Linux
with "env: 'python3\\r': No such file or directory", which silently
kills the bridge and leaves the Gazebo world empty.
"""
import os
import threading

import rclpy
from ament_index_python.packages import get_package_share_directory
from gazebo_msgs.msg import EntityState, ModelState, ModelStates
from gazebo_msgs.srv import SetEntityState, SetModelState, SpawnEntity
from geometry_msgs.msg import PoseArray
from std_msgs.msg import Int32MultiArray
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from rclpy.qos import HistoryPolicy, QoSProfile, ReliabilityPolicy

PACKAGE_SHARE = get_package_share_directory("hiveflight_sim_node")
DRONE_SDF = os.path.join(PACKAGE_SHARE, "models", "drone_marker", "model.sdf")
TARGET_SDF = os.path.join(PACKAGE_SHARE, "models", "target_marker", "model.sdf")
COLORS = (("0.08", "0.42", "0.95"), ("0.95", "0.18", "0.08"), ("0.12", "0.78", "0.32"), ("0.88", "0.20", "0.75"), ("0.95", "0.68", "0.08"))


class GazeboSwarmBridge(Node):
    def __init__(self):
        super().__init__("gazebo_swarm_bridge")
        self.declare_parameter("swarm_id", 0)
        self.declare_parameter("drone_count", 20)
        self.declare_parameter("target_count", 1)
        # Gazebo's per-model state services are comparatively expensive. The
        # simulation still publishes at its physics rate, while this bridge
        # sends only the latest visualization sample at bounded rates.
        self.declare_parameter("drone_update_hz", 25.0)
        self.declare_parameter("target_update_hz", 30.0)
        # When the Gazebo world plugin applies poses on the physics thread,
        # this bridge only spawns models and skips all pose service calls.
        self.declare_parameter("use_plugin", True)
        swarm_id = self.get_parameter("swarm_id").value
        prefix = f"/hiveflight/swarm_{swarm_id}"
        qos = QoSProfile(history=HistoryPolicy.KEEP_LAST, depth=1, reliability=ReliabilityPolicy.BEST_EFFORT)

        self.lock = threading.Lock()
        self.drone_poses = {}
        self.target_poses = {}
        self.assignments = {}
        # Keep target traffic independent: drone updates must not consume every
        # service slot and starve the latency-sensitive target stream.
        self.drone_in_flight = set()
        self.target_in_flight = set()
        self.max_drone_in_flight = 6
        self.next_drone_index = 0
        self.spawn_client = self.create_client(SpawnEntity, "/spawn_entity")
        self.state_client = self.create_client(SetModelState, "/gazebo/set_model_state")
        self.entity_state_client = self.create_client(SetEntityState, "/gazebo/set_entity_state")
        self.state_mode = None
        self.drones_ready = False
        self.targets_ready = False
        # Sequential spawn state: gazebo_ros_factory drops responses when many
        # /spawn_entity requests are fired in parallel (observed: 8 of 20 spawn
        # replies never arrived, leaving drones_ready stuck False and freezing
        # every drone marker). Spawns are therefore chained one-by-one.
        self.drone_spawn_active = False
        self.target_spawn_active = False
        self.drone_spawn_next = 0
        self.target_spawn_next = 0
        self.drone_spawn_failed = False
        self.target_spawn_failed = False
        self.drone_xml = ""
        self.target_xml = ""
        self.drone_assignments = {}
        self.drone_count = int(self.get_parameter("drone_count").value)
        self.target_count = int(self.get_parameter("target_count").value)
        self.received_drones = False
        self.received_targets = False
        self.last_target_pose = None
        self.gazebo_model_names = set()
        self.target_pose_sequence = 0
        self.last_target_update_sequence = 0
        self.drone_update_period = 1.0 / max(
            1.0, float(self.get_parameter("drone_update_hz").value)
        )
        self.target_update_period = 1.0 / max(
            1.0, float(self.get_parameter("target_update_hz").value)
        )

        self.create_subscription(PoseArray, f"{prefix}/drone_poses", self.on_drones, qos)
        self.create_subscription(PoseArray, f"{prefix}/target_poses", self.on_targets, qos)
        self.create_subscription(Int32MultiArray, f"{prefix}/target_assignments", self.on_assignments, qos)
        self.create_subscription(ModelStates, "/gazebo/model_states", self.on_model_states, qos)
        # Register target first; it also owns a separate in-flight budget.
        self.create_timer(self.target_update_period, self.update_target_models)
        self.create_timer(self.drone_update_period, self.update_models)
        self.create_timer(0.5, self.ensure_models_spawned)
        self.create_timer(5.0, self.health_report)
        self.get_logger().info("Gazebo bridge started: quadcopter model visualization")
        self.get_logger().info(f"Drone model: {DRONE_SDF}")
        self.get_logger().info(f"Target model: {TARGET_SDF}")
        self.get_logger().info(f"Subscribed to {prefix}/drone_poses and {prefix}/target_poses")

    def ensure_models_spawned(self):
        if not self.spawn_client.service_is_ready():
            self.get_logger().warn(
                "Waiting for /spawn_entity; no Gazebo models can appear yet",
                throttle_duration_sec=5.0,
            )
            return
        if not self.drones_ready and not self.drone_spawn_active:
            self.spawn_drones(self.drone_count)
        if not self.targets_ready and not self.target_spawn_active:
            self.spawn_targets(self.target_count)

    @staticmethod
    def read_model(path):
        with open(path, "r", encoding="utf-8") as model_file:
            return model_file.read()

    @staticmethod
    def colored_model(xml, color):
        red, green, blue = color
        return xml.replace("__RED__", red).replace("__GREEN__", green).replace("__BLUE__", blue)

    def on_drones(self, message):
        self.received_drones = True
        with self.lock:
            self.drone_poses = {index: pose for index, pose in enumerate(message.poses)}
            should_spawn = not self.drones_ready and not self.drone_spawn_active
        if should_spawn:
            self.spawn_drones(len(message.poses))

    def on_targets(self, message):
        self.received_targets = True
        with self.lock:
            self.target_poses = {index: pose for index, pose in enumerate(message.poses)}
            self.target_pose_sequence += 1
            current = message.poses[0] if message.poses else None
            moved = current is not None and self.last_target_pose is not None and (
                abs(current.position.x - self.last_target_pose.position.x) > 1e-4 or
                abs(current.position.y - self.last_target_pose.position.y) > 1e-4 or
                abs(current.position.z - self.last_target_pose.position.z) > 1e-4
            )
            self.last_target_pose = current
            should_spawn = not self.targets_ready and not self.target_spawn_active
        if moved:
            self.get_logger().debug("Target pose stream is changing")
        if should_spawn:
            self.spawn_targets(len(message.poses))

    def on_assignments(self, message):
        with self.lock:
            self.assignments = {index: target for index, target in enumerate(message.data)}

    def on_model_states(self, message):
        with self.lock:
            self.gazebo_model_names = set(message.name)

    def spawn_drones(self, count):
        if not self.spawn_client.service_is_ready():
            self.get_logger().warn("/spawn_entity is not ready; drones cannot be rendered", throttle_duration_sec=5.0)
            return
        try:
            xml = self.read_model(DRONE_SDF)
        except OSError as error:
            self.get_logger().error(f"Cannot read drone SDF: {error}")
            return
        with self.lock:
            if self.drones_ready or self.drone_spawn_active:
                return
            self.drone_spawn_active = True
            self.drone_spawn_next = 0
            self.drone_spawn_failed = False
            self.drone_xml = xml
            self.drone_assignments = dict(self.assignments)
        self.get_logger().info(f"Spawning {count} colored quadcopters (sequential)")
        self.spawn_next_drone(count)

    def spawn_next_drone(self, count):
        with self.lock:
            index = self.drone_spawn_next
            if index >= count:
                return
            self.drone_spawn_next += 1
            target_index = self.drone_assignments.get(index, index)
            xml = self.colored_model(self.drone_xml, COLORS[target_index % len(COLORS)])
        request = SpawnEntity.Request()
        request.name = f"drone_{index}"
        request.xml = xml
        request.initial_pose.position.z = 50.0
        request.reference_frame = "world"
        self.get_logger().info(f"Requesting Gazebo spawn: {request.name}")
        future = self.spawn_client.call_async(request)
        future.add_done_callback(
            lambda completed, model=request.name, total=count: self.drone_spawn_done(completed, model, total)
        )

    def spawn_targets(self, count):
        if not self.spawn_client.service_is_ready():
            self.get_logger().warn("/spawn_entity is not ready; targets cannot be rendered", throttle_duration_sec=5.0)
            return
        try:
            xml = self.read_model(TARGET_SDF)
        except OSError as error:
            self.get_logger().error(f"Cannot read target SDF: {error}")
            return
        with self.lock:
            if self.targets_ready or self.target_spawn_active:
                return
            self.target_spawn_active = True
            self.target_spawn_next = 0
            self.target_spawn_failed = False
            self.target_xml = xml
        self.get_logger().info(f"Spawning {count} colored targets (sequential)")
        self.spawn_next_target(count)

    def spawn_next_target(self, count):
        with self.lock:
            index = self.target_spawn_next
            if index >= count:
                return
            self.target_spawn_next += 1
            xml = self.colored_model(self.target_xml, COLORS[index % len(COLORS)])
        request = SpawnEntity.Request()
        request.name = f"target_{index}"
        request.xml = xml
        request.initial_pose.position.z = 50.0
        request.reference_frame = "world"
        self.get_logger().info(f"Requesting Gazebo spawn: {request.name}")
        future = self.spawn_client.call_async(request)
        future.add_done_callback(
            lambda completed, model=request.name, total=count: self.target_spawn_done(completed, model, total)
        )

    def drone_spawn_done(self, future, model, total):
        success = False
        try:
            result = future.result()
            success = result.success
            if not result.success:
                self.get_logger().error(f"Drone spawn failed for {model}: {result.status_message}")
            else:
                self.get_logger().info(f"Drone spawned: {model}")
        except Exception as error:
            self.get_logger().error(f"Drone spawn exception for {model}: {error}")
        if not success:
            with self.lock:
                self.drone_spawn_failed = True
                self.drone_spawn_active = False
            return
        with self.lock:
            index = self.drone_spawn_next
        if index < total:
            self.spawn_next_drone(total)
            return
        with self.lock:
            self.drones_ready = True
            self.drone_spawn_active = False
        self.get_logger().info("All quadcopters spawned; live updates enabled")

    def target_spawn_done(self, future, model, total):
        success = False
        try:
            result = future.result()
            success = result.success
            if not result.success:
                self.get_logger().error(f"Target spawn failed for {model}: {result.status_message}")
            else:
                self.get_logger().info(f"Target spawned: {model}")
        except Exception as error:
            self.get_logger().error(f"Target spawn exception for {model}: {error}")
        if not success:
            with self.lock:
                self.target_spawn_failed = True
                self.target_spawn_active = False
            return
        with self.lock:
            index = self.target_spawn_next
        if index < total:
            self.spawn_next_target(total)
            return
        with self.lock:
            self.targets_ready = True
            self.target_spawn_active = False
        self.get_logger().info("All targets spawned; live updates enabled")

    def health_report(self):
        # NOTE: rclpy loggers do not support printf-style extra args
        # ("logger.info(fmt, a, b)" raises TypeError) - format first.
        drone_count = len([n for n in self.gazebo_model_names if n.startswith("drone_")])
        target_count = len([n for n in self.gazebo_model_names if n.startswith("target_")])
        self.get_logger().info(
            f"Bridge health: drones_topic={self.received_drones} "
            f"targets_topic={self.received_targets} "
            f"spawn_service={self.spawn_client.service_is_ready()} "
            f"drone_ready={self.drones_ready} target_ready={self.targets_ready} "
            f"gazebo_drones={drone_count} gazebo_targets={target_count} "
            f"state_mode={self.state_mode}"
        )

    def update_models(self):
        if self.get_parameter("use_plugin").value:
            return
        if not self.ensure_state_service():
            return
        with self.lock:
            poses_by_index = dict(self.drone_poses) if self.drones_ready else {}

        # A fixed iteration order with a capped in-flight set permanently
        # favored drone_0..drone_5. Rotate the starting index so every drone
        # receives visualization bandwidth, including drone_6 and above.
        count = len(poses_by_index)
        for offset in range(count):
            index = (self.next_drone_index + offset) % count
            pose = poses_by_index.get(index)
            if pose is None:
                continue
            name = f"drone_{index}"
            with self.lock:
                if (name in self.drone_in_flight or
                        len(self.drone_in_flight) >= self.max_drone_in_flight):
                    if len(self.drone_in_flight) >= self.max_drone_in_flight:
                        break
                    continue
                self.drone_in_flight.add(name)
                self.next_drone_index = (index + 1) % count
            if self.state_mode == "model":
                state = ModelState()
                state.model_name = name
                state.pose = pose
                state.reference_frame = "world"
                request = SetModelState.Request()
                request.model_state = state
                client = self.state_client
            else:
                state = EntityState()
                state.name = name
                state.pose = pose
                state.reference_frame = "world"
                request = SetEntityState.Request()
                request.state = state
                client = self.entity_state_client
            client.call_async(request).add_done_callback(
                lambda future, model=name: self.update_done(future, model, self.drone_in_flight)
            )

    def update_target_models(self):
        """Always submit the newest target pose through the verified state API."""
        if self.get_parameter("use_plugin").value:
            return
        if not self.targets_ready or not self.ensure_state_service():
            return
        with self.lock:
            target_poses = [
                (index, pose)
                for index, pose in self.target_poses.items()
            ]
            sequence = self.target_pose_sequence
        for index, pose in target_poses:
            name = f"target_{index}"
            with self.lock:
                if name in self.target_in_flight:
                    continue
                self.target_in_flight.add(name)
            if self.state_mode == "model":
                state = ModelState()
                state.model_name = name
                state.pose = pose
                state.reference_frame = "world"
                request = SetModelState.Request()
                request.model_state = state
                client = self.state_client
            else:
                state = EntityState()
                state.name = name
                state.pose = pose
                state.reference_frame = "world"
                request = SetEntityState.Request()
                request.state = state
                client = self.entity_state_client
            client.call_async(request).add_done_callback(
                lambda future, model=name, pose_sequence=sequence: self.target_update_done(
                    future, model, pose_sequence
                )
            )

    def target_update_done(self, future, model, pose_sequence):
        self.update_done(future, model, self.target_in_flight)
        try:
            result = future.result()
            if result.success:
                self.last_target_update_sequence = pose_sequence
        except Exception:
            pass

    def ensure_state_service(self):
        """Select and validate the Gazebo state API for either update stream."""
        if self.state_mode is None:
            if self.state_client.service_is_ready():
                self.state_mode = "model"
                self.get_logger().info("Using /gazebo/set_model_state")
            elif self.entity_state_client.service_is_ready():
                self.state_mode = "entity"
                self.get_logger().info("Using /gazebo/set_entity_state fallback")
            else:
                self.get_logger().warn(
                    "Waiting for /gazebo/set_model_state or /gazebo/set_entity_state",
                    throttle_duration_sec=5.0,
                )
                return False
        return ((self.state_mode == "model" and self.state_client.service_is_ready()) or
                (self.state_mode == "entity" and self.entity_state_client.service_is_ready()))

    def update_done(self, future, model, in_flight):
        with self.lock:
            in_flight.discard(model)
        try:
            result = future.result()
            if not result.success:
                self.get_logger().warn(f"Gazebo rejected {model}: {result.status_message}")
        except Exception as error:
            self.get_logger().error(f"Gazebo update failed for {model}: {error}")


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
