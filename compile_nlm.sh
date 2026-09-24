#!/usr/bin/env bash

# Compilation script for NLM Phase 2
# This script compiles the enhanced NLM codebase with all improvements

echo "=== NLM Phase 2 Compilation Script ==="
echo "This script compiles the enhanced NLM codebase with all improvements"
echo ""

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake is not installed. Please install CMake to compile the NLM codebase."
    exit 1
fi

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

# Configure CMake
cd build
echo "Configuring CMake with build type: Release..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed."
    exit 1
fi

# Compile the project
echo "Compiling NLM Phase 2..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed."
    exit 1
fi

# Verify the executable was created
if [ -f "nlm" ]; then
    echo ""
    echo "=== Compilation Successful ==="
    echo "The NLM Phase 2 executable has been created in: $(pwd)/nlm"
    echo ""
    echo "To run the NLM Phase 2 tests, execute:"
    echo "  ./nlm"
    echo ""
    echo "The executable includes all enhancements:"
    echo "  ✓ Fixed Neuron/Synapse header implementation inconsistencies"
    echo "  ✓ Improved main.cpp - refactored test flow and error handling"
    echo "  ✓ Added command line argument validation"
    echo "  ✓ Refactored repetitive code patterns"
    echo "  ✓ Enhanced Config class with proper JSON/YAML parser"
    echo "  ✓ Expanded test coverage and added integration tests"
    echo "  ✓ Improved logging system with structured logging"
    echo ""
else
    echo "ERROR: The NLM executable was not created."
    exit 1
fi