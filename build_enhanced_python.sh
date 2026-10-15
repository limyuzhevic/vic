#!/bin/bash

# Build script for enhanced NLM Python bindings
# This script builds the Python bindings with all new enhancements

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build_python"

# Clean and create build directory
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure CMake with Python bindings
cmake ${SCRIPT_DIR} \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE="${Python_EXECUTABLE:-python3}" \
    -DPYTHON_LIBRARY="${Python_LIBRARY:-}" \
    -DPYTHON_INCLUDE_DIR="${Python_INCLUDE_DIR:-}"

# Build the project
make -j$(nproc)

# Run Python bindings tests
if [ -f "${BUILD_DIR}/python/pynlm.so" ] || [ -f "${BUILD_DIR}/python/pynlm.pyd" ]; then
    echo "Python bindings built successfully!"
    echo "Building and installing Python package..."
    cd python
    ${Python_EXECUTABLE} -m pip install -e . 2>&1 || {
        echo "Warning: Failed to install Python package. This may be expected if dependencies are missing."
        echo "The shared library has been built successfully."
    }
    
    # Run the enhanced test suite
    echo "Running enhanced Python bindings test suite..."
    ${Python_EXECUTABLE} test_enhanced_bindings.py
    
    echo "All tests completed successfully!"
else
    echo "Error: Python bindings not found after build."
    echo "Check for build errors in the output above."
    exit 1
fi

echo "Enhanced NLM Python bindings build completed successfully!"
