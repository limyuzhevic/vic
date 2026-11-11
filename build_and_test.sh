#!/bin/bash
# Build and test script for NLM improvements

set -e

echo "==========================================="
echo "Building NLM with improved command-line interface"
echo "==========================================="

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j4

# Check if the demo executables were built
echo ""
echo "Checking built executables..."

if [ -f "./nlm_phase3_demo" ]; then
    echo "✓ nlm_phase3_demo built successfully"
else
    echo "✗ nlm_phase3_demo build failed"
fi

if [ -f "./nlm_phase4_demo" ]; then
    echo "✓ nlm_phase4_demo built successfully"
else
    echo "✗ nlm_phase4_demo build failed"
fi

if [ -f "./nlm_phase5_demo" ]; then
    echo "✓ nlm_phase5_demo built successfully"
else
    echo "✗ nlm_phase5_demo build failed"
fi

if [ -f "./nlm_phase6_demo" ]; then
    echo "✓ nlm_phase6_demo built successfully"
else
    echo "✗ nlm_phase6_demo build failed"
fi

if [ -f "./nlm_phase6_text_demo" ]; then
    echo "✓ nlm_phase6_text_demo built successfully"
else
    echo "✗ nlm_phase6_text_demo build failed"
fi

if [ -f "./nlm_benchmark" ]; then
    echo "✓ nlm_benchmark built successfully"
else
    echo "✗ nlm_benchmark build failed"
fi

echo ""
echo "==========================================="
echo "Testing improved command-line interface"
echo "==========================================="

# Test help functionality
echo "Testing help functionality..."
./nlm_phase3_demo --help | head -20

echo ""
echo "Testing version functionality..."
./nlm_phase3_demo --version

echo ""
echo "Testing custom parameters..."
./nlm_phase3_demo --episodes 2 --steps 50 2>&1 | head -10

echo ""
echo "Testing configuration file..."
./nlm_phase3_demo --config ../configs/fast_test.cfg 2>&1 | head -10

echo ""
echo "==========================================="
echo "Improvements Summary"
echo "==========================================="
echo "1. ✓ Enhanced command-line interface with proper argument parsing"
echo "2. ✓ Added help and version flags"
echo "3. ✓ Added configuration file support (INI format)"
echo "4. ✓ Created common utilities for initialization and setup"
echo "5. ✓ Added performance monitoring utilities"
echo "6. ✓ Enhanced logging system with configurable levels"
echo "7. ✓ Updated all Phase demos to use improved CLI"
echo "8. ✓ Added example configuration files"
echo "9. ✓ Created usage documentation script"
echo "10. ✓ Improved error handling and user feedback"
echo ""
echo "The NLM project now has:
- Better command-line interface
- Configuration file support
- Common utilities for all demos
- Enhanced logging and monitoring
- Improved documentation and examples
- Better error handling and user feedback
"
echo "==========================================="