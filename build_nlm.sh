#!/bin/bash

# Build and Run NLM Phase 6 Integration
# This script provides advanced commands for testing and running NLM components

cd /workspace/app
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
echo "Building NLM Phase 6 Integration..."
make -j$(nproc)

echo "Build complete!"
echo ""
echo "Available executables:"
echo "1. nlm          - Phase 2 Basic Neural Computation Demo"
echo "2. nlm_phase3_demo  - Phase 3 World Interaction Demo"
echo "3. nlm_phase4_demo  - Phase 4 Advanced Cognitive Capabilities"
echo "4. nlm_phase6_demo  - Phase 6 Final Integration (UNBUILT)"
echo ""
echo "To run Phase 6 demo, first build it by adding to CMakeLists.txt"
echo "Then run: ./nlm_phase6_demo [args]"