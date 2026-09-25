#!/bin/bash

# NLM Brain Command Line Interface
# Provides easy testing and demonstration of the improved NLM system

set -e

cd "$(dirname "$0")"

# Create build directory if it doesn't exist
mkdir -p build

# Configure CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DENABLE_TESTING=ON

# Build the project
make -C build -j4

# Run integration tests
echo "=== Running NLM Integration Tests ==="
if [ -f "build/nlm_phase6_demo" ]; then
    echo "Running Phase 6 demo..."
    build/nlm_phase6_demo
else
    echo "ERROR: Phase 6 demo not found. Please check build output."
    exit 1
fi

# Run unit tests if available
echo "=== Running Unit Tests ==="
if [ -f "build/test_nlm" ]; then
    echo "Running unit tests..."
    build/test_nlm
elif [ -f "build/tests/test_nlm" ]; then
    echo "Running unit tests..."
    build/tests/test_nlm
else
    echo "Unit tests not found. Skipping."
fi

echo "=== Testing Complete ==="
