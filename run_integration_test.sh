#!/bin/bash
# Phase 6 Integration Test for NLM
# This script runs the comprehensive integration test

# Navigate to the project root
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ec27e2f7-71a3-455f-ae54-6a7426b240f2

# Create build directory and compile
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake (use Release mode for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs (use number of CPU cores)
make -j$(nproc)

# Run the Phase 6 demo
./nlm_phase6_demo
