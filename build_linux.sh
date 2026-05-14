#!/bin/bash
# Build script for Linux (WSL/Ubuntu)

cd /mnt/c/work/ubuntu_work/HiveFlight
mkdir -p build
cd build
cmake ..
make
