#!/bin/bash

# Script to build NLM Python bindings

set -e

echo "Building NLM Python bindings..."

echo "Step 1: Creating build directory..."
mkdir -p build
cd build

echo "Step 2: Configuring with CMake..."
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python3)

echo "Step 3: Building Python module..."
make -j4

echo "Step 4: Checking for Python module..."
if [ -f "pynlm*.so" ] || [ -f "pynlm*.pyd" ]; then
    echo "✓ Python module built successfully!"
    ls -la *.so *.pyd 2>/dev/null || true
else
    echo "✗ Python module not found. Looking for .so or .pyd files..."
    find . -name "*.so" -o -name "*.pyd" 2>/dev/null | head -5
    exit 1
fi

echo "Step 5: Testing Python module..."
cd ..
python3 -c "import sys; print('Python version:', sys.version); try: import pynlm; print('✓ Successfully imported pynlm'); print('Version:', pynlm.__version__); except ImportError as e: print('✗ Failed to import pynlm:', e); sys.exit(1)"

echo "Step 6: Running comprehensive test..."
python3 test_bindings.py

echo ""
echo "="*60
echo "BUILD AND TEST COMPLETED SUCCESSFULLY!"
echo "The NLM Python bindings are working correctly."
echo "="*60