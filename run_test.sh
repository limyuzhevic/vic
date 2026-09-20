#!/bin/bash

# Simple test script to verify NLM integration
# Run this from the workspace root directory

set -e

echo "=== NLM Integration Test ==="
echo "Testing Phase 6 integration...

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j4

# Run the Phase 6 demo
if [ -f ./nlm_phase6_demo ]; then
    echo "Running Phase 6 integration demo..."
    ./nlm_phase6_demo
else
    echo "ERROR: Phase 6 demo not found!"
    exit 1
fi

echo "\n=== Test Complete ==="
echo "If the demo ran successfully, integration is working!"
