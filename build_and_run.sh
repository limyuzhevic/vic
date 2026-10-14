#!/bin/bash
# NLM Build and Run Script
# This script builds and runs the NLM Phase 6 integration demo

set -e

echo "=== NLM Phase 6 Build and Run Script ==="

# Build directory
BUILD_DIR="build"

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p $BUILD_DIR
fi

# Configure CMake
echo "Configuring CMake..."
cd $BUILD_DIR
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building NLM core libraries..."
make -j$(nproc)

# Run the main demo
echo ""
echo "Running NLM Phase 6 Integration Demo..."
echo ""

./nlm

echo ""
echo "=== Demo Complete ==="
