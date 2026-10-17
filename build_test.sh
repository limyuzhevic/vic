#!/bin/bash
# Quick build test script for NLM Python bindings

# Check if we can compile the enhanced bindings
echo "Testing NLM Python bindings build..."

# First check if the build directory exists
if [ -d "/workspace/app/build" ]; then
    echo "Found existing build directory"
    cd /workspace/app/build
    # Try to run cmake if it exists
    if [ -f "CMakeCache.txt" ]; then
        echo "CMake cache found, attempting to build..."
        make -j4 2>&1 | tail -20
    else
        echo "No CMake cache, need to configure first"
        mkdir -p /workspace/app/build_temp
        cd /workspace/app/build_temp
        cmake ../ -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -10
    fi
else
    echo "Creating build directory..."
    mkdir -p /workspace/app/build_temp
    cd /workspace/app/build_temp
    cmake ../ -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -20
fi

echo "Build test completed"
