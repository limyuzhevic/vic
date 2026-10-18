#!/bin/bash
set -e

# Configure CMake
cmake -B build -S .

# Build the project
cmake --build build --config Release

# Run the test
./build/test_enhanced_memory

# Check if Python bindings can be compiled (optional)
echo "=== Testing Python Bindings ==="
g++ -std=c++20 -I./src -I./python -I./include -I/usr/local/include -c python/bindings.cpp -o build/bindings.o 2>&1 || echo "Python bindings compilation skipped (pybind11 may not be installed)"

echo "=== Build and Test Complete ==="
