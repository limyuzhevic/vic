#!/usr/bin/env bash
#!/bin/bash
set -e

# Build script for NLM (Neural Learning Machine) Phase 6 Integration

echo "=== Building NLM Phase 6 Integration ==="

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "ERROR: CMakeLists.txt not found. Please run from project root."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_PYTHON=ON \
    -DBUILD_EXPERIMENTS=ON

# Build with parallel jobs
echo "Building..."
make -j$(nproc)

# Run tests if they exist
echo "Running tests..."
if [ -f "tests/nlm_tests" ]; then
    ./tests/nlm_tests
else
    echo "Test executable not found, skipping tests."
fi

# Build Python bindings if requested
if [ -d "../python" ] && [ -f "../pyproject.toml" ]; then
    echo "Building Python bindings..."
    cd ..
    python -m pip install -e .
fi

echo "=== Build Complete ==="
