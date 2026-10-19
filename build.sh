#!/bin/bash

set -e

echo "Building NLM (Neural Learning Machine) - All Phases"
echo "=================================================="

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF \
    -DBUILD_PYTHON=ON \
    -DPYTHON_EXECUTABLE=$(which python3)

# Build all targets
make -j$(nproc)

# Check if Python bindings were built
if [ -f "python/pynlm.so" ]; then
    echo "✓ Python bindings built successfully: python/pynlm.so"
else
    echo "✗ Python bindings build failed"
    exit 1
fi

# Check if all executables were built
EXECUTABLES=("./nlm" "./nlm_phase3_demo" "./nlm_phase4_demo" "./nlm_phase6_demo")

for exe in "${EXECUTABLES[@]}"; do
    if [ -f "$exe" ]; then
        echo "✓ $exe built successfully"
    else
        echo "✗ $exe build failed"
    fi
done

echo ""
echo "Build completed!"
echo "To run Phase 2 demo:"
echo "  ./build/nlm"
echo ""
echo "To test Python bindings:"
echo "  cd build/python"
echo "  python3 -c \"from pynlm import createBrain, createSimpleWorld; print('Import successful')\""
