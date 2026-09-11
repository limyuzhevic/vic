#!/usr/bin/env bash

# Simple build and test script for NLM Phase 6

echo "=== Building NLM Phase 6 Integration Test ==="
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "=== Building test_integration.cpp ==="
g++ -std=c++20 -I../src test_integration.cpp -o test_integration

if [ $? -ne 0 ]; then
    echo "Failed to build test_integration.cpp"
    exit 1
fi

echo "=== Running test_integration.cpp ==="
./test_integration

echo "=== Test completed ==="