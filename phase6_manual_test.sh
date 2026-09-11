#!/bin/bash
echo "=== Testing Phase 6 Integration ==="

echo "1. Checking if Phase6Demo.cpp exists:"
if [ -f "src/experiments/Phase6Demo.cpp" ]; then
    echo "✓ Phase6Demo.cpp exists"
else
    echo "✗ Phase6Demo.cpp NOT found"
    exit 1
fi

echo "2. Checking CMakeLists.txt includes Phase6Demo.cpp:"
grep -n "Phase6Demo" CMakeLists.txt

echo "3. Attempting to compile Phase6Demo.cpp manually:"
g++ -std=c++20 -Isrc -I./src -g src/experiments/Phase6Demo.cpp -o phase6_test

if [ -f phase6_test ]; then
    echo "✓ Compilation successful!"
    echo "4. Running Phase 6 demo..."
    ./phase6_test
else
    echo "✗ Compilation failed!"
    echo "Attempting to compile with all dependencies..."
    g++ -std=c++20 -Isrc -I./src -g src/experiments/Phase6Demo.cpp src/experiments/Phase6IntegratedExperiment.cpp -o phase6_full_test
    if [ -f phase6_full_test ]; then
        echo "✓ Full compilation successful!"
        echo "Running Phase 6 demo..."
        ./phase6_full_test
    else
        echo "✗ Full compilation also failed!"
    fi
fi