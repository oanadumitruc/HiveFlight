#!/bin/bash
# Build script for Linux (WSL/Ubuntu)

set -e


SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

# Build ROS 2 packages first so hiveflight_interfacesConfig.cmake exists.
ROS2_WS_DIR="$SCRIPT_DIR/ros2_ws"
COLCON_BUILD_BASE="$ROS2_WS_DIR/build"
COLCON_INSTALL_BASE="$ROS2_WS_DIR/install"

# Clean previous ROS overlay to avoid stale/partial installs.
rm -rf "$ROS2_WS_DIR/src" "$COLCON_BUILD_BASE" "$COLCON_INSTALL_BASE" "$ROS2_WS_DIR/log"
rm -f "$ROS2_WS_DIR"/COLCON_IGNORE

mkdir -p "$ROS2_WS_DIR/src"

# Copy packages into the colcon workspace (simple + robust across filesystems).
cp -R "$SCRIPT_DIR/ros2/hiveflight_interfaces" "$ROS2_WS_DIR/src/"
cp -R "$SCRIPT_DIR/ros2/hiveflight_sim" "$ROS2_WS_DIR/src/"
cp -R "$SCRIPT_DIR/ros2/hiveflight_sim_node" "$ROS2_WS_DIR/src/"

# ROS setup.bash expects certain env vars; avoid strict bash mode issues.
set +u
source /opt/ros/humble/setup.bash
set -u


# Build everything under ./ros2_ws/src, and install into ./ros2_ws/install
colcon build \
  --base-paths "${ROS2_WS_DIR}/src" \
  --build-base "$COLCON_BUILD_BASE" \
  --install-base "$COLCON_INSTALL_BASE" \
  --merge-install \
  --event-handlers console_direct+ \
  --parallel-workers 4


# Make sure CMake finds the generated configs.
# Some colcon versions assume COLCON_TRACE is set when bash runs in strict/unset mode.
# Avoid aborting on `set -u` by forcing it to empty before sourcing.
export COLCON_TRACE=""
source "$COLCON_INSTALL_BASE/setup.bash"


# If possible, set hiveflight_interfaces_DIR directly.
HIVEFLIGHT_INTERFACES_DIR=""
if [ -f "$COLCON_INSTALL_BASE/share/hiveflight_interfaces/cmake/hiveflight_interfacesConfig.cmake" ]; then
  HIVEFLIGHT_INTERFACES_DIR="$COLCON_INSTALL_BASE/share/hiveflight_interfaces/cmake"
fi

# Build the non-ROS root CMake project (executables).
CMAKE_PREFIX_PATH="$COLCON_INSTALL_BASE:${CMAKE_PREFIX_PATH:-}"
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
  -DHIVEFLIGHT_BUILD_OPENGL_VIEWER=ON \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  ${HIVEFLIGHT_INTERFACES_DIR:+-Dhiveflight_interfaces_DIR="$HIVEFLIGHT_INTERFACES_DIR"}

cmake --build "$BUILD_DIR" --parallel

echo
echo "Build complete."
echo "Root executables are in: $BUILD_DIR"
echo "ROS 2 install overlay is in: $COLCON_INSTALL_BASE"
echo "If OpenGL/GLUT were found, run: $BUILD_DIR/hiveflight_gl_viewer"


