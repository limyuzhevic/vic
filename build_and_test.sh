#!/bin/bash
# Build and test script for enhanced NLM Brain

set -e

echo "Building enhanced NLM Brain..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Build the test program
echo "Building integration test..."
g++ -std=c++20 -I../src -I../tests -I./include -o test_integration test_integration.cpp

# Run the main test suite
echo "Running main test suite..."
./nlm_test

# Run integration test
echo "Running integration test..."
./test_integration

echo "All tests passed! Enhanced NLM Brain integration successful."