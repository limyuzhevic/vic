#!/usr/bin/env bash

# build.sh - Build script for NLM Phase 6

# Set error handling
set -e

# Default build type
BUILD_TYPE="Release"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            echo "Cleaning build directory..."
            rm -rf build
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug    Build with debug symbols"
            echo "  --clean    Clean build directory before building"
            echo "  --help     Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Create build directory
mkdir -p build

# Configure CMake
cd build
echo "Configuring CMake with $BUILD_TYPE build type..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build with parallel jobs
echo "Building NLM Phase 6..."
make -j$(nproc)

# Check if build was successful
if [[ $? -eq 0 ]]; then
    echo "✅ Build successful!"
    echo "\nTo run the Phase 6 demo, execute:"
    echo "  ./build/nlm_phase6_demo"
    echo "\nOr to run the main executable:"
    echo "  ./build/nlm"
else
    echo "❌ Build failed!"
    exit 1
fi
