#!/bin/bash
set -e

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
    cd build
    echo "Running CMake configuration..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    echo "Building..."
    make -j4
else
    echo "Build directory exists, rebuilding..."
    cd build
    make -j4
fi

echo "Build complete!"
