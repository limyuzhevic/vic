#!/usr/bin/env bash

# Test script to run all tests and verify current state

echo "Building and running tests..."

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_4f4d6a04-6ffa-4362-93cc-baf0f3b76950

# Build with CMake
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)

# Run tests
./test_brain
./test_synapse
./test_neuron
./test_types
./test_clock
./test_config

# Return to original directory
cd ..

echo "Tests completed!"