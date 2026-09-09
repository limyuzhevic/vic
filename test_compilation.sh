#!/usr/bin/env bash
# Simple test to verify NLM Interactive Visualization compilation

set -e

echo "Testing NLM Interactive Visualization compilation..."

# Create build directory
mkdir -p build_test
cd build_test

# Configure with interactive visualization enabled
cmake -D CMAKE_BUILD_TYPE=Debug ..

# Build only
make -j$(nproc) 2>&1 | tail -20

# Check if build succeeded
if [ -f "./nlm" ]; then
    echo "✓ Build successful!"
    echo ""
    echo "The interactive visualization system has been successfully compiled."
    echo "You can now run the tests to verify functionality."
else
    echo "✗ Build failed!"
    exit 1
fi
