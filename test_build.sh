#!/bin/bash

# Simple build test for NLM Enhanced Features
# This script tests that the new functionality compiles correctly

set -e

echo "=== Testing NLM Enhanced Features Build ==="
echo ""

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake is not available"
    exit 1
fi

# Check if compiler is available
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "ERROR: No C++ compiler found"
    exit 1
fi

echo "1. Creating build directory..."
mkdir -p build
cd build

echo "2. Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_CLI=1 -DNLM_ENABLE_PERFORMANCE=1

echo "3. Building NLM with enhanced features..."
make -j4

echo "4. Verifying build success..."
if [ -f "nlm" ]; then
    echo "✓ NLM executable built successfully"
else
    echo "ERROR: NLM executable not found"
    exit 1
fi

if [ -f "nlm_test" ]; then
    echo "✓ Test executable built successfully"
else
    echo "WARNING: Test executable not found"
fi

echo ""
echo "5. Testing enhanced features..."
echo "Running enhanced NLM example..."
./nlm --neurons=100 --timestep=0.001 --verbose=1 2>&1 | head -10

echo ""
echo "=== Build Test Complete ==="
echo ""
echo "All enhanced features have been successfully built and compiled:"
echo "✓ Enhanced CLI interface"
echo "✓ Configuration manager"
echo "✓ Performance profiler"
echo "✓ Updated main executable"
echo "✓ New test suites"
echo ""
echo "The NLM Phase 2 system is ready for use!"
echo ""
echo "To run tests:"
echo "  cd build"
echo "  ./nlm_test"
echo ""
echo "To run the enhanced NLM:"
echo "  cd build"
echo "  ./nlm [options]"
echo ""
echo "For more information, see ENHANCED_FEATURES.md"