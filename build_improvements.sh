#!/usr/bin/env bash
# Simple build script for NLM improvements test

set -e  # Exit on any error

echo "Building NLM improvements test..."

# Create build directory
BUILD_DIR="${BUILD_DIR:-build}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_BUILD_PYTHON_BINDINGS=OFF \
    -DBUILD_TESTING=OFF

# Build the core library
echo "Building nlm_core..."
make nlm_core -j4

# Build the test executable
echo "Building test_improvements..."
make test_improvements

# Run the test
echo "Running improvement tests..."
./test_improvements

echo "All tests passed!

Next steps:
1. Review the test output to verify improvements
2. Check the test files for more examples
3. Update existing code to use new conversion functions
4. Add new constants as needed
5. Profile performance to identify further optimizations
