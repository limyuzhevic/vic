#!/bin/bash

# Build and run NLM Phase 2 demo with Python bindings

set -e

echo "Building NLM with Python bindings..."
mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Available executables:"
    ls -la nlm* 2>/dev/null || true
    echo ""
    echo "Available Python modules:"
    find . -name "*.so" -o -name "*.pyd" 2>/dev/null | grep -E "pynlm|pybind" | head -5
    echo ""
    echo "To run the main Phase 2 demo:"
    echo "./nlm"
    echo ""
    echo "To run Phase 6 integration test:"
    echo "./nlm_phase6_demo"
    echo ""
    echo "To test Python bindings:"
    echo "cd .. && python3 -c \"import sys; sys.path.insert(0, 'build'); import pynlm; print('pynlm imported successfully')\""
else
    echo "Build failed!"
    exit 1
fi
