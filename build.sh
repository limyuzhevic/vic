#!/bin/bash
# NLM Build Convenience Script
# 
# This script provides easy build and validation options for the NLM project

set -e  # Exit on any error

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

function print_usage() {
    cat << EOF
NLM Build Convenience Script

Usage: ./build.sh [COMMAND]

Commands:
  build      Build the project (default)
  clean      Clean the build directory
  test       Build and run tests
  python     Build Python bindings
  all        Build all targets (C++ and Python)
  validate   Validate the build by checking executables
  help       Show this help message

Examples:
  ./build.sh              # Build the project
  ./build.sh test         # Build and run tests
  ./build.sh python       # Build Python bindings only
  ./build.sh clean         # Clean build directory

Phase 6 Integration Demo:
  $BUILD_DIR/nlm_phase6_demo     # Run Phase 6 integration test

Python Bindings:
  pip install .                 # Install Python bindings
  python -c "import pynlm; print('NLM Python bindings loaded successfully')"
EOF
}

function build_project() {
    echo "Building NLM project..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
        echo "CMake configuration failed!"
        exit 1
    fi
    
    if ! make -j$(nproc); then
        echo "Build failed!"
        exit 1
    fi
    
    echo "Build completed successfully!"
    cd "$PROJECT_DIR"
}

function clean_build() {
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "Build directory cleaned."
}

function build_tests() {
    echo "Building and running tests..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    if ! cmake .. -DCMAKE_BUILD_TYPE=Debug; then
        echo "CMake configuration failed!"
        exit 1
    fi
    
    if ! make -j$(nproc); then
        echo "Build failed!"
        exit 1
    fi
    
    if command -v ctest &> /dev/null; then
        if ! ctest --output-on-failure; then
            echo "Tests failed!"
            exit 1
        fi
    else
        echo "Warning: ctest not found. Skipping test execution."
    fi
    
    echo "Tests completed successfully!"
    cd "$PROJECT_DIR"
}

function build_python() {
    echo "Building Python bindings..."
    
    if command -v pip &> /dev/null; then
        cd "$PROJECT_DIR"
        pip install pybind11 scikit-build-core numpy
        pip install -e .
        echo "Python bindings installed successfully!"
    else
        echo "Error: pip not found. Please install pip and Python dependencies."
        exit 1
    fi
}

function build_all() {
    echo "Building all targets (C++ and Python)..."
    build_project
    build_python
}

function validate_build() {
    echo "Validating build..."
    
    cd "$BUILD_DIR"
    
    # Check for core executables
    executables=("nlm" "nlm_phase3_demo" "nlm_phase4_demo" "nlm_phase6_demo")
    
    for exe in "${executables[@]}"; do
        if [ -f "$exe" ]; then
            echo "✓ Found executable: $exe"
        else
            echo "✗ Missing executable: $exe"
        fi
    done
    
    # Check for Python module
    if [ -f "../python/bindings.cpp" ]; then
        echo "✓ Python bindings source found"
    else
        echo "✗ Python bindings source not found"
    fi
    
    # Check CMake configuration
    if [ -f "CMakeCache.txt" ]; then
        echo "✓ CMake configuration found"
    else
        echo "✗ CMake configuration not found"
    fi
    
    echo "Validation completed."
    cd "$PROJECT_DIR"
}

# Main command handling
case "${1:-build}" in
    build)
        build_project
        ;;
    clean)
        clean_build
        ;;
    test)
        build_tests
        ;;
    python)
        build_python
        ;;
    all)
        build_all
        ;;
    validate)
        validate_build
        ;;
    help|*)
        print_usage
        ;;
    *)
        echo "Unknown command: $1"
        print_usage
        exit 1
        ;;
esac