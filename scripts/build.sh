#!/bin/bash
# build.sh - Example build script for NLM

set -e

# Configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
PYTHON_BINDINGS=ON
TESTS=ON
PERFORMANCE=OFF
COVERAGE=OFF
ASAN=OFF
TSAN=OFF

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --coverage)
            COVERAGE=ON
            shift
            ;;
        --asan)
            ASAN=ON
            shift
            ;;
        --tsan)
            TSAN=ON
            shift
            ;;
        --performance)
            PERFORMANCE=ON
            shift
            ;;
        --no-python)
            PYTHON_BINDINGS=OFF
            shift
            ;;
        --no-tests)
            TESTS=OFF
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug         Build in Debug mode (default: Release)"
            echo "  --coverage      Enable code coverage"
            echo "  --asan          Enable AddressSanitizer"
            echo "  --tsan          Enable ThreadSanitizer"
            echo "  --performance   Enable performance optimizations"
            echo "  --no-python     Disable Python bindings"
            echo "  --no-tests      Disable tests"
            echo "  --help          Show this help message"
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
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DNLM_ENABLE_PYTHON_BINDINGS=$PYTHON_BINDINGS \
    -DNLM_ENABLE_TESTS=$TESTS \
    -DNLM_ENABLE_PERFORMANCE=$PERFORMANCE \
    -DNLM_ENABLE_COVERAGE=$COVERAGE \
    -DNLM_ENABLE_ASAN=$ASAN \
    -DNLM_ENABLE_TSAN=$TSAN \
    -GNinja

# Build
cmake --build . --config $BUILD_TYPE -- -j$(nproc)

# Run tests if enabled
if [ "$TESTS" = "ON" ]; then
    echo "Running tests..."
    ctest --output-on-failure --no-compress-output
fi

# Install locally
cmake --build . --target install --config $BUILD_TYPE

echo "Build completed successfully!"
