#!/bin/bash
echo "Phase 6 Build Test"
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

if [ $? -eq 0 ]; then
    echo "✓ CMake and make completed successfully"
    echo "Checking for nlm_phase6_demo executable..."
    if [ -f nlm_phase6_demo ]; then
        echo "✓ nlm_phase6_demo found"
        echo "Running Phase 6 demo..."
        ./nlm_phase6_demo
    else
        echo "✗ nlm_phase6_demo not found"
        find . -name "*phase6*" -type f 2>/dev/null
    fi
else
    echo "✗ CMake or make failed"
fi