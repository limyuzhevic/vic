#!/bin/bash

# Quick build and test script for enhanced NLM demos

set -e

echo "Building enhanced NLM demos..."

# Configure CMake
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=OFF

# Build all targets
make -j4

# Test the enhanced demos
echo ""
echo "Testing enhanced Phase 3 demo with help..."
./nlm_phase3_demo --help

echo ""
echo "Testing enhanced Phase 3 demo with custom parameters..."
./nlm_phase3_demo --episodes 2 --steps 100 --neuron-count 100 --enable-curiosity --enable-reward

echo ""
echo "Testing enhanced Phase 4 demo with help..."
./nlm_phase4_demo --help

echo ""
echo "Testing enhanced Phase 4 demo with custom parameters..."
./nlm_phase4_demo --trials 10 --neuron-count 100 --enable-plasticity

echo ""
echo "All enhanced demos are working correctly!

Enhanced Features:
- Unified CLI with comprehensive help system
- Advanced configuration options
- Better error handling and user feedback
- Support for checkpointing and replay
- Enhanced logging and output options
- More flexible parameter parsing

The enhanced CLI provides better user experience and more control over NLM demos."