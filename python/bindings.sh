#!/usr/bin/env bash
# Python bindings build script
# This script builds and installs Python bindings for NLM

set -e

echo "Building Python bindings for NLM"
echo "================================="

# Check for Python
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python3 not found. Python bindings require Python."
    exit 1
fi

# Check for pybind11
if ! python3 -c "import pybind11" 2>/dev/null; then
    echo "ERROR: pybind11 not found for Python"
    echo "Please install pybind11: pip install pybind11"
    exit 1
fi

echo "Found Python3"
echo "Found pybind11"

# Check for NLM build directory
if [[ ! -f "build/CMakeCache.txt" ]]; then
    echo "ERROR: NLM build directory not found. Build NLM first:"
    echo "  mkdir build && cd build"
    echo "  cmake .. -DNLM_BUILD_PYTHON=ON"
    exit 1
fi

# Configure CMake for Python bindings
cmake -S python -B python/build -DNLM_BUILD_PYTHON=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python3)

# Build Python bindings
cmake --build python/build --config Release

# Install Python bindings
pip install -e python/build/

echo ""
echo "Python bindings built and installed successfully!"
echo ""
echo "To use the Python bindings:"
echo "  import pynlm"
echo ""
echo "To rebuild:"
echo "  cd python && rm -rf build && cmake -B build -S . && cmake --build build"
