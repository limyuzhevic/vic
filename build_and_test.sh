#!/usr/bin/env bash

# Simple demo script for experiment results persistence
# This script can be used to test the save/load functionality

cd /workspace/app

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Please install CMake (version 3.16 or higher)."
    echo "Installation instructions:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  macOS: brew install cmake"
    echo "  CentOS/RHEL: sudo yum install cmake"
    exit 1
fi

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Check if build was successful
if [ -f "./nlm_test" ]; then
    echo "Build successful! Running tests..."
    ./nlm_test
else
    echo "Build failed! Check CMake output for errors."
    exit 1
fi

# Run the Phase 6 demo if it exists
if [ -f "./nlm_phase6_demo" ]; then
    echo "Running Phase 6 demo..."
    ./nlm_phase6_demo
else
    echo "Phase 6 demo not found in build directory."
fi
