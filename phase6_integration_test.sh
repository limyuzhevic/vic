#!/bin/bash

echo "=== Phase 6 Integration Test ==="

echo "1. Source files:"
ls -la src/experiments/Phase6Demo.cpp

if [ ! -f "src/experiments/Phase6Demo.cpp" ]; then
    echo "ERROR: Phase6Demo.cpp not found!"
    exit 1
fi

echo "\n2. Attempting to compile Phase6Demo.cpp with all dependencies..."

echo "   Including source files from core libraries..."
# This is a simplified approach to test if the code can compile
# In a real CMake build, these would be linked automatically

g++ -std=c++20 -Isrc -I./src -g src/experiments/Phase6Demo.cpp \
    src/experiments/Phase6IntegratedExperiment.cpp \
    -o phase6_final_test 2>&1

if [ -f phase6_final_test ]; then
    echo "✓ Compilation successful!"
    echo "\n3. Running Phase 6 demo..."
    ./phase6_final_test
else
    echo "✗ Compilation failed!"
    echo "\n4. Checking compilation errors..."
    g++ -std=c++20 -Isrc -I./src -g src/experiments/Phase6Demo.cpp src/experiments/Phase6IntegratedExperiment.cpp -v 2>&1 | tail -100
    echo "\n5. Trying simpler compilation test..."
    # Just compile the Phase6Demo.cpp to check if there are basic syntax errors
    g++ -std=c++20 -Isrc -I./src -c src/experiments/Phase6Demo.cpp -o phase6_demo.o 2>&1
    if [ $? -eq 0 ]; then
        echo "✓ Phase6Demo.cpp compiles (has no syntax errors)"
    else
        echo "✗ Phase6Demo.cpp has syntax errors"
    fi
fi