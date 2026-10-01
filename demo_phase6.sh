#!/bin/bash
# Phase 6 Demo Script - Shows NLM Improvements

# Navigate to the build directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory not found. Please run 'mkdir build && cd build && cmake ..' first."
    exit 1
fi

echo "=== NLM Phase 6 Demo Script ==="
echo "This demo showcases the improvements made to the NLM system."
echo ""

# Check if the Phase 6 demo executable exists
if [ -f "$BUILD_DIR/nlm_phase6_demo" ]; then
    echo "Running Phase 6 demo executable..."
    echo ""
    "$BUILD_DIR/nlm_phase6_demo"
    echo ""
    echo "Phase 6 demo completed successfully!"
else
    echo "Warning: Phase 6 demo executable not found."
    echo "Available executables in build directory:"
    ls -la "$BUILD_DIR/nlm*" 2>/dev/null | head -10
fi

echo ""
echo "=== Improvements Made ==="
echo "1. Fixed duplicate configuration values in default.cfg"
echo "2. Added Phase 6 demo executable to CMakeLists.txt"
echo "3. Updated Python bindings to match actual Config interface"
echo "4. Fixed documentation mismatches between Phase 6 and implementation"
echo "5. Enhanced configuration loading and saving"
echo ""
echo "=== Usage Examples ==="
echo ""
echo "# To build with Phase 6 support:"
echo "mkdir build && cd build"
echo "cmake .."
echo "make"
echo ""
echo "# Run Phase 6 integration demo:"
echo "./nlm_phase6_demo"
echo ""
echo "# For Phase 3 and Phase 4 demos:"
echo "./nlm_phase3_demo"
echo "./nlm_phase4_demo"
echo ""
echo "# For simple neural computation:"
echo "./nlm"