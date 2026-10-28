#!/usr/bin/env bash

# NLM CLI Build Script
# This script builds the NLM command-line interface

set -e

echo "Building NLM Command Line Interface..."

# Create build directory
mkdir -p build

# Configure CMake
cd build
cmake ..

# Build
make -j$(nproc)

cd ..

echo "Build complete!"
echo ""
echo "To run the NLM CLI:"
echo "  cd build"
echo "  ./nlm_cli [options] <command>"
echo ""
echo "Available commands:"
echo "  run        Run simulation"
echo "  batch      Run batch experiments"
echo "  save       Save brain state"
echo "  load       Load brain state"
echo "  benchmark  Run performance benchmarks"
echo "  explore    Explore hyperparameters"
echo "  monitor    Monitor brain"
echo "  help       Show help"
