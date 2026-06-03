#!/bin/bash
# Build script for Linux (WSL/Ubuntu)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DHIVEFLIGHT_BUILD_OPENGL_VIEWER=ON
cmake --build "$BUILD_DIR" --parallel

echo
echo "Build complete."
echo "Executables are in: $BUILD_DIR"
echo "If OpenGL/GLU/GLUT were found, run: $BUILD_DIR/hiveflight_gl_viewer"
