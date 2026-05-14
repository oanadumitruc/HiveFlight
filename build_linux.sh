#!/bin/bash
# Build script for Linux (WSL/Ubuntu)


mkdir -p build
cd build
cmake ..
make
