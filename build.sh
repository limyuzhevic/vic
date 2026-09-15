#!/bin/bash

set -e

echo "=== NLM Build Script ==="
echo ""

# Create build directory
mkdir -p build

# Check if we need to reconfigure
BUILD_DIR="build"
if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    echo "Running cmake configuration..."
    cd $BUILD_DIR
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cd ..
fi

# Build with parallel jobs
echo "Building project with maximum parallelism..."
cd $BUILD_DIR
make -j$(nproc)

# List built targets
echo ""
echo "=== Build Complete ==="
echo ""
echo "Successfully built targets:"
echo "  nlm - Main demonstration program"
echo "  nlm_phase3_demo - Phase 3 demonstration"
echo "  nlm_phase4_demo - Phase 4 demonstration"  
echo "  nlm_phase6_demo - Phase 6 integration test"
echo "  nlm_test - Unit test suite"
echo ""

# Quick status check
echo "Checking target executables..."
if [ -f "$BUILD_DIR/nlm" ]; then
    echo "  ✓ nlm exists ($(du -h $BUILD_DIR/nlm | cut -f1))"
fi
if [ -f "$BUILD_DIR/nlm_phase6_demo" ]; then
    echo "  ✓ nlm_phase6_demo exists ($(du -h $BUILD_DIR/nlm_phase6_demo | cut -f1))"
fi
if [ -f "$BUILD_DIR/nlm_test" ]; then
    echo "  ✓ nlm_test exists ($(du -h $BUILD_DIR/nlm_test | cut -f1))"
fi

# Check for any build issues
if [ ! -f "$BUILD_DIR/nlm_test" ] || [ ! -x "$BUILD_DIR/nlm_test" ]; then
    echo ""
    echo "⚠ Warning: nlm_test executable not found or not executable"
    echo "This may affect test suite capabilities."
fi

echo ""
echo "Build finished successfully! You can now run the demonstrations and tests.
