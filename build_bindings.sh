#!/usr/bin/env bash

# Build script for enhanced NLM Python bindings
# This script builds the enhanced Python bindings with all new functionality

set -e

PROJECT_ROOT="/workspace/app"
PYTHON_DIR="${PROJECT_ROOT}/python"
BUILD_DIR="${PROJECT_ROOT}/build_python"

echo "Building enhanced NLM Python bindings..."

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
cmake "${PYTHON_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE="$(which python3)" \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON

# Build the module
make -j$(nproc)

# Check if the module was built successfully
if [ -f "libpynlm*.so" ] || [ -f "pynlm*.pyd" ]; then
    echo "✓ Python bindings built successfully!"
    echo "  Module location: ${BUILD_DIR}/libpynlm*.so or ${BUILD_DIR}/pynlm*.pyd"
    
    # Test basic import
    cd "${PROJECT_ROOT}"
    python3 -c "import sys; sys.path.insert(0, '${BUILD_DIR}'); import pynlm; print('✓ pynlm imported successfully'); print('  Available classes:', len([c for c in dir(pynlm) if not c.startswith('_')]))"
else
    echo "✗ Failed to build Python bindings"
    exit 1
fi

# Also build the main C++ project
if [ -f "${PROJECT_ROOT}/build/nlm" ]; then
    echo "✓ Main NLM binary already built"
else
    echo "Building main NLM project..."
    mkdir -p "${PROJECT_ROOT}/build"
    cd "${PROJECT_ROOT}/build"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
fi

# Run Phase 6 demo to verify everything works
if [ -f "${PROJECT_ROOT}/build/nlm_phase6_demo" ]; then
    echo "\nRunning Phase 6 demo to verify integration..."
    cd "${PROJECT_ROOT}/build"
    ./nlm_phase6_demo 2>&1 | head -50
else
    echo "⚠ Phase 6 demo not found, skipping integration test"
fi

echo "\n✓ All builds completed successfully!"
