#!/bin/bash

# NLM Build Script for Linux/macOS
# Optimized for speed and minimal disk usage

set -euo pipefail

# Configuration options
BUILD_TYPE="Release"
BUILD_DIR="build"
ENABLE_OPENMP=true
ENABLE_AVX2=true
ENABLE_SSE4_2=true
INSTALL_PREFIX="/usr/local"
PARALLEL_JOBS=$(nproc)

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --minsize)
            BUILD_TYPE="MinSizeRel"
            shift
            ;;
        --relwithdebinfo)
            BUILD_TYPE="RelWithDebInfo"
            shift
            ;;
        --no-openmp)
            ENABLE_OPENMP=false
            shift
            ;;
        --no-simd)
            ENABLE_AVX2=false
            ENABLE_SSE4_2=false
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug           Build with debug symbols (default: Release)"
            echo "  --release         Build optimized for speed (default)"
            echo "  --minsize         Build with minimal binary size"
            echo "  --relwithdebinfo  Build with debug symbols and optimizations"
            echo "  --no-openmp       Disable OpenMP parallelization"
            echo "  --no-simd         Disable SIMD instructions"
            echo "  --prefix DIR      Installation prefix (default: /usr/local)"
            echo "  --help, -h        Show this help"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Print build configuration
echo "============================================"
echo "NLM Build Configuration"
echo "============================================"
echo "Build Type: $BUILD_TYPE"
echo "Build Directory: $BUILD_DIR"
echo "OpenMP: $ENABLE_OPENMP"
echo "AVX2: $ENABLE_AVX2"
echo "SSE4.2: $ENABLE_SSE4_2"
echo "Parallel Jobs: $PARALLEL_JOBS"
echo "Install Prefix: $INSTALL_PREFIX"
echo "============================================"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DENABLE_OPENMP="$ENABLE_OPENMP" \
    -DENABLE_AVX2="$ENABLE_AVX2" \
    -DENABLE_SSE4_2="$ENABLE_SSE4_2" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_CXX_STANDARD=20

# Build
cmake --build . --config "$BUILD_TYPE" -- -j$PARALLEL_JOBS

# Optional: Run tests
echo "Running tests..."
./nlm_test --output-on-failure

# Optional: Install
# echo "Installing to $INSTALL_PREFIX..."
# cmake --install . --prefix "$INSTALL_PREFIX"

# Show built executables
echo ""
echo "Build completed successfully!"
echo ""
echo "Built executables:"
ls -la bin/ 2>/dev/null || ls -la

echo ""
echo "To run the main demo:"
echo "  ./nlm"
echo ""
echo "To run tests:"
echo "  ./nlm_test"
