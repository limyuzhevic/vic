#!/bin/bash

# Simple build script for NLM
# This script provides faster builds without CMake's complexity
# Supports: build, run, clean, test

BUILD_DIR="${BUILD_DIR:-build}"
TARGET="${TARGET:-nlm}"
CONFIG="${CONFIG:-Release}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

echo_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

echo_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if CMakeLists.txt exists
if [ ! -f "CMakeLists.txt" ]; then
    echo_error "CMakeLists.txt not found. This is not a CMake project."
    echo_info "This build script is designed for CMake-based projects."
    exit 1
fi

build() {
    echo_info "Building NLM in $CONFIG mode..."
    
    if [ ! -d "$BUILD_DIR" ]; then
        echo_info "Creating build directory..."
        mkdir -p "$BUILD_DIR"
    fi
    
    cd "$BUILD_DIR"
    
    if [ -f "Makefile" ]; then
        echo_info "Makefile already exists, running make..."
        make -j$(nproc)
    else
        echo_info "Running CMake..."
        cmake .. -DCMAKE_BUILD_TYPE="$CONFIG"
        echo_info "Running make..."
        make -j$(nproc)
    fi
    
    if [ $? -eq 0 ]; then
        echo_info "Build successful!"
        echo_info "Executable location: $BUILD_DIR/$TARGET"
    else
        echo_error "Build failed!"
        exit 1
    fi
    
    cd ..
}

run() {
    local target_file="$BUILD_DIR/$TARGET"
    
    if [ ! -f "$target_file" ]; then
        echo_error "Executable not found: $target_file"
        echo_info "Building first..."
        build
        target_file="$BUILD_DIR/$TARGET"
    fi
    
    shift
    echo_info "Running $TARGET with arguments: $@"
    "$target_file" "$@"
}

test() {
    echo_info "Running tests..."
    
    if [ ! -d "$BUILD_DIR" ]; then
        echo_info "Creating build directory..."
        mkdir -p "$BUILD_DIR"
    fi
    
    cd "$BUILD_DIR"
    
    # Check if test executable exists
    if [ -f "nlm_test" ]; then
        echo_info "Running nlm_test..."
        ./nlm_test
    elif [ -f "test" ]; then
        echo_info "Running test executable..."
        ./test
    else
        echo_warn "No test executable found in $BUILD_DIR"
        echo_info "Checking for CTest...
        if [ -f "CTestTestfile.cmake" ]; then
            echo_info "Running CTest..."
            ctest
        else
            echo_info "No tests found. You can run the demo executables instead."
        fi
    fi
    
    cd ..
}

clean() {
    echo_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo_info "Clean complete."
}

help() {
    echo "NLM Build Script"
    echo "==============="
    echo ""
    echo "Usage: $0 [command] [options]"
    echo ""
    echo "Commands:"
    echo "  build      Build the project (default)"
    echo "  run        Run the main executable (with optional args)"
    echo "  test       Run tests (if available)"
    echo "  clean      Remove build directory"
    echo "  help       Show this help message"
    echo ""
    echo "Options:"
    echo "  --config CONFIG      Set build configuration (Debug, Release, RelWithDebInfo)"
    echo "  --target TARGET      Set target executable name"
    echo "  --build-dir DIR      Set build directory (default: build)"
    echo ""
    echo "Examples:"
    echo "  $0                    # Build with defaults"
    echo "  $0 run --integration  # Run with integration flag"
    echo "  $0 clean              # Clean build directory"
    echo ""
}

# Parse command line arguments
if [ $# -eq 0 ]; then
    build
else
    case "$1" in
        build)
            build
            ;;
        run)
            run "$@"
            ;;
        test)
            test
            ;;
        clean)
            clean
            ;;
        help|--help|-h)
            help
            ;;
        *)
            echo_error "Unknown command: $1"
            help
            exit 1
            ;;
    esac
fi
