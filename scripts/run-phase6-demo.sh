#!/usr/bin/env bash
# Convenience script to run Phase 6 demo easily

set -e

echo "NLM Phase 6 Integration Demo"
echo "=========================="

# Build if not already built
if [ ! -f "./nlm_phase6_demo" ]; then
    echo "Building NLM Phase 6 demo..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    cd ..
    echo "Build complete."
fi

echo ""
echo "Running Phase 6 integration demo..."
echo ""

# Run the demo
./nlm_phase6_demo

echo ""
echo "Phase 6 demo completed successfully!"
