@echo off
REM Build script for Windows


if not exist build mkdir build
cd build
cmake ..
mingw32-make
