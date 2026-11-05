#!/bin/bash

# Project-wide cleanup and improvements summary

# 1. Fix broken reward system - implement real reward computation
# 2. Improve neuromodulation - implement proper dopamine dynamics
# 3. Enhance plasticity - ensure reward-based learning actually works
# 4. Optimize neural activity - reduce excessive firing
# 5. Add more demo commands for better user experience
# 6. Improve checkpointing - fix the stub implementation
# 7. Add better test instrumentation
# 8. Improve documentation and examples

# Build the project to test current state
echo "Building current project..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the main phase 2 demo to see current behavior
if [ -f "./nlm" ]; then
    echo "\n=== Running Phase 2 Demo ==="
    ./nlm
else
    echo "Phase 2 demo not built"
fi

# Run Phase 3 demo
if [ -f "./nlm_phase3_demo" ]; then
    echo "\n=== Running Phase 3 Demo ==="
    ./nlm_phase3_demo 2 200
else
    echo "Phase 3 demo not built"
fi

# Run Phase 6 demo
if [ -f "./nlm_phase6_demo" ]; then
    echo "\n=== Running Phase 6 Demo ==="
    ./nlm_phase6_demo
else
    echo "Phase 6 demo not built"
fi
