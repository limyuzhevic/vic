#!/bin/bash
# Build script for NLM enhanced Python bindings
# Run from the project root directory

set -e

echo "Building NLM enhanced Python bindings..."

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is required but not found."
    echo "Please install CMake (version 3.16 or higher)"
    exit 1
fi

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "Error: Python3 is required but not found."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure CMake with enhanced Python bindings support
cmake -D CMAKE_BUILD_TYPE=Release -D BUILD_PYTHON_BINDINGS=ON -D BUILD_ENHANCED_BINDINGS=ON ..

# Build the project
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Generated targets:"
    echo "  - nlm (original C++ executable)"
    echo "  - nlm_phase3_demo (Phase 3 demo)"
    echo "  - nlm_phase4_demo (Phase 4 demo)"
    echo "  - pynlm_enhanced (enhanced Python bindings)"
    echo "  - libnlm_core.a (core C++ library)"
    echo ""
    echo "To install Python bindings:"
    echo "  make install"
    echo ""
    echo "To test the enhanced bindings:"
    echo "  python3 ../examples.py"
    echo ""
    echo "For more information, see:"
    echo "  - examples.py (Python usage examples)"
    echo "  - REFACTORING_DOCUMENTATION.md (detailed documentation)"
else
    echo "Build failed!"
    exit 1
fi
