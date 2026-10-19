#!/bin/bash

set -e

echo "Building NLM - Focus on Python Bindings and Core Library"
echo "========================================================="

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF \
    -DBUILD_PYTHON=ON \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -Wno-dev

echo "CMake configuration complete"

# Build just the core library and Python bindings
make nlm_core pynlm_bindings

if [ -f "libnlm_core.a" ]; then
    echo "✓ Core library built successfully"
else
    echo "✗ Core library build failed"
    exit 1
fi

if [ -f "libpynlm.so" ]; then
    echo "✓ Python bindings built successfully"
else
    echo "✗ Python bindings build failed"
    exit 1
fi

# Test Python bindings
if python3 -c "from pynlm import createDefaultConfig; print('Python bindings import successful')"; then
    echo "✓ Python bindings can be imported"
else
    echo "✗ Python bindings import failed"
    exit 1
fi

echo ""
echo "Build completed successfully!"
echo "Core library: $(ls -la build/libnlm_core.a 2>/dev/null || echo 'not found')"
echo "Python bindings: $(ls -la build/python/libpynlm.so 2>/dev/null || echo 'not found')"
