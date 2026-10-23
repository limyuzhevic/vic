#!/bin/bash

# NLM Test Build and Run Script
# This script builds and runs the NLM tests, including the new JSON parser tests

set -e  # Exit on any error

echo "=== NLM Test Build and Run Script ==="
echo "Building NLM tests with JSON parser support..."

# Create build directory
mkdir -p build
cd build

# Configure CMake with Release build type
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build all targets
echo "Building targets..."
make -j$(nproc)

# Run tests
echo "Running tests..."
./nlm_test

# Check if tests passed
if [ $? -eq 0 ]; then
    echo "✅ All tests passed successfully!"
    echo "✅ JSON parser functionality is working correctly!"
    echo "✅ Config JSON improvements are complete!"
else
    echo "❌ Tests failed!"
    exit 1
fi

echo "=== Build and test completed successfully ==="