#!/usr/bin/env bash
# Quick Start Script for NLM Neural Learning Machine
# Simple usage for new users

echo "=== NLM Neural Learning Machine Quick Start ==="
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Building NLM..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    if [ $? -ne 0 ]; then
        echo "Build failed! Please check CMake output."
        exit 1
    fi
    cd ..
fi

echo "Build complete!"
echo ""
echo "=== Available Demos ==="
echo "1. Quick demo (Phase 2 - Basic neural computation)"
echo "   ./build/nlm phase2 --verbose"
echo ""
echo "2. Advanced demo (Phase 6 - Full integration)"
echo "   ./build/nlm phase6 --profile --neuron-count 2000"
echo ""
echo "3. Configuration examples"
echo "   Create config.json with your settings"
echo "   ./build/nlm phase2 --config config.json"
echo ""
echo "=== Configuration Files ==="
echo "Example config.json:"
echo "{"
echo "    \"neuron_count\": 500,"
echo "    \"simulation_timestep\": 0.001,"
echo "    \"random_seed\": 42,"
echo "    \"stdp_ltp_weight\": 0.01,"
echo "    \"stdp_ltd_weight\": 0.012,"
echo "    \"stdp_tau\": 20.0"
echo "}"
echo ""
echo "=== Quick Help ==="
echo "Run ./build/nlm --help for all options"
echo ""
echo "=== Files Created ==="
echo "- build/nlm - NLM executable"
echo "- checkpoints/ - Brain state storage"
echo "- logs/ - Debug logs"
echo ""
echo "Happy neural simulation!"
