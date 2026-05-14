@echo off
REM Build script for Windows

cd /mnt/c/work/ubuntu_work/HiveFlight
if not exist build mkdir build
cd build
cmake ..
mingw32-make
