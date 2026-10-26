#!/bin/bash

# Simple build script for NLM Phase 6 fixes

echo "Building NLM with Phase 6 fixes..."

# Create build directory
mkdir -p build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Executable: ./nlm"
else
    echo "Build failed!"
    exit 1
fi
