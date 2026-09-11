#!/bin/bash

# Simple build script for NLM Phase 6

cd $(dirname $0)
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run test_integration.cpp
echo "Testing Phase 6 integration..."
g++ -std=c++20 -I../src ../test_integration.cpp -o test_integration

if [ -f test_integration ]; then
    echo "Running test_integration..."
    ./test_integration
else
    echo "Failed to build test_integration.cpp"
fi