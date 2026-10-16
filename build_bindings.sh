#!/bin/bash
# Build script for the refactored NLM Python bindings

set -e

echo "Building NLM Python bindings..."
echo "====================================="

echo "Creating build directory..."
mkdir -p build
cd build

echo "Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "Building..."
make -j$(nproc)

echo ""
echo "Build completed successfully!"
echo "The Python bindings have been refactored into:")
echo "  - types_bindings.cpp: Basic types (NeuronId, SynapseId, enums)"
echo "  - config_bindings.cpp: Configuration classes (Config)"
echo "  - brain_bindings.cpp: Brain interface (Brain class and helpers)"
echo "  - agent_bindings.cpp: Agent and world interfaces (AgentBrain, SimpleWorld, etc.)"
echo ""
echo "The module is now built as a single pynlm.so Python extension"
echo "while maintaining the same user-facing API."