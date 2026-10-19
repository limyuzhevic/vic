#!/bin/bash

# Phase 6 Build Script for NLM (熙然) neural learning machine
# Phase 6: Final Integration

echo "=== NLM (熙然) Phase 6 Build Script ==="
echo "Phase 6: Final Integration of All Neural Systems"
echo ""

# Configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
INSTALL_DIR=""
ENABLE_TESTS=true
ENABLE_PYTHON=true
USE_PHASE6_MAIN=true

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --no-tests)
            ENABLE_TESTS=false
            shift
            ;;
        --no-python)
            ENABLE_PYTHON=false
            shift
            ;;
        --no-phase6-main)
            USE_PHASE6_MAIN=false
            shift
            ;;
        --install)
            INSTALL_DIR="${2:-/usr/local}"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug             Build in Debug mode (default: Release)"
            echo "  --no-tests          Disable tests build"
            echo "  --no-python         Disable Python bindings"
            echo "  --no-phase6-main    Use Phase 2 main executable instead of Phase 6"
            echo "  --install DIR       Install to DIR (default: /usr/local)"
            echo "  --build-dir DIR     Specify build directory (default: build)"
            echo "  --help              Show this help"
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
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring CMake (Build type: $BUILD_TYPE)..."
if [ "$USE_PHASE6_MAIN" = true ]; then
    # Phase 6 main executable
    cmake ..
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DENABLE_TESTS="$ENABLE_TESTS" \
        -DENABLE_PYTHON="$ENABLE_PYTHON" \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
        -DUSE_PHASE6_MAIN=ON
else
    # Phase 2 main executable (for backward compatibility)
    cmake ..
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DENABLE_TESTS="$ENABLE_TESTS" \
        -DENABLE_PYTHON="$ENABLE_PYTHON" \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
        -DUSE_PHASE6_MAIN=OFF
fi

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

# Build all targets
echo "Building all targets..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    exit 1
fi

# Build tests if enabled
if [ "$ENABLE_TESTS" = true ]; then
    echo "Building tests..."
    if command -v ctest &> /dev/null; then
        ctest --output-on-failure
        if [ $? -ne 0 ]; then
            echo "WARNING: Some tests failed, but build completed successfully"
        fi
    else
        echo "WARNING: ctest not found, skipping tests"
    fi
fi

# Build Python bindings if enabled
if [ "$ENABLE_PYTHON" = true ]; then
    echo "Building Python bindings..."
    python -m pip install -e .
    if [ $? -ne 0 ]; then
        echo "ERROR: Python bindings build failed"
        exit 1
    fi
fi

# Install if specified
if [ -n "$INSTALL_DIR" ]; then
    echo "Installing to $INSTALL_DIR..."
    make install
    if [ $? -ne 0 ]; then
        echo "ERROR: Installation failed"
        exit 1
    fi
fi

echo ""
echo "=== Phase 6 Build Complete ==="
echo ""
echo "Built targets:"
echo "  - nlm: Main neural simulation executable (Phase 6)"
echo "  - nlm_phase3_demo: Phase 3 demonstration"
echo "  - nlm_phase4_demo: Phase 4 demonstration"
echo "  - nlm_phase6_demo: Phase 6 integration demo"
echo "  - nlm_test: Unit test suite"
echo "  - pynlm: Python bindings library"
echo ""
echo "To run the main Phase 6 simulation:"
echo "  cd build"
echo "  ./nlm [options]"
echo ""
echo "To run Phase 6 integration demo:"
echo "  cd build"
echo "  ./nlm_phase6_demo"
echo ""
echo "To run tests:"
echo "  cd build"
echo "  ./nlm_test"
echo ""
echo "To install Python bindings:"
echo "  pip install -e ."