#!/bin/bash
# build.sh - Build script for NLM neural simulation framework

set -e  # Exit on any error

echo "Building NLM (Neural Learning Machine) neural simulation framework..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=20 \
         -DNLM_ENABLE_TESTS=ON \
         -DNLM_ENABLE_PYTHON=ON

# Build with parallel jobs
echo "Building with CMake..."
make -j$(nproc)

# Run tests if enabled
if [ -f "CTestTestfile.cmake" ]; then
    echo "Running tests..."
    ctest --output-on-failure
else
    echo "No tests to run"
fi

# Check if executables were built
echo "Checking for executables..."
if [ -f "nlm" ]; then
    echo "✓ Core executable built: nlm"
elif [ -f "nlm_phase6_demo" ]; then
    echo "✓ Phase 6 demo executable built: nlm_phase6_demo"
else
    echo "⚠ No core executable found"
fi

if [ -f "pynlm.so" ] || [ -f "pynlm.pyd" ] || [ -f "pynlm.cpython*.so" ]; then
    echo "✓ Python bindings built"
else
    echo "⚠ Python bindings not found"
fi

echo "Build completed successfully!"