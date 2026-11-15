#!/bin/bash

# NLM Build Script
# Simple script to build NLM with optional Python bindings

set -e  # Exit on any error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

function print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

function print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse command line arguments
BUILD_PYTHON_BINDINGS=0
BUILD_TYPE="Release"
TARGETS="all"

while [[ $# -gt 0 ]]; do
    case $1 in
        --python)
            BUILD_PYTHON_BINDINGS=1
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --targets)
            TARGETS="$2"
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --python      Build Python bindings (pynlm)"
            echo "  --debug       Build in Debug mode (default: Release)"
            echo "  --targets NAME Build specific targets (default: all)"
            echo "  --help, -h    Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                    # Build C++ library only (Release)"
            echo "  $0 --python           # Build C++ library + Python bindings"
            echo "  $0 --debug            # Build in Debug mode"
            echo "  $0 --targets nlm       # Build only the nlm executable"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use $0 --help for usage information"
            exit 1
            ;;
    esac
done

print_info "Starting NLM build process..."
print_info "Build directory: ${BUILD_DIR}"
print_info "Build type: ${BUILD_TYPE}"
print_info "Python bindings: $([ $BUILD_PYTHON_BINDINGS -eq 1 ] && echo 'Enabled' || echo 'Disabled')"

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure CMake
print_info "Configuring CMake..."
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
if [ $BUILD_PYTHON_BINDINGS -eq 1 ]; then
    CMAKE_ARGS="${CMAKE_ARGS} -DNLM_BUILD_PYTHON_BINDINGS=ON"
fi

cmake ${CMAKE_ARGS} ..

# Build the project
print_info "Building NLM (${TARGETS})..."
make -j$(nproc) ${TARGETS}

# Success message
print_info "Build completed successfully!"
print_info ""
print_info "To run tests:"
print_info "  ./test_main (for Phase 2 tests)"
print_info ""
print_info "To run demos:"
print_info "  ./nlm (Phase 2 demo)"
print_info "  ./nlm_phase3_demo (Phase 3 demo)"
print_info "  ./nlm_phase4_demo (Phase 4 demo)"
print_info "  ./nlm_phase6_demo (Phase 6 demo)"
print_info ""
print_info "To build Python bindings:"
print_info "  pip install . (if Python bindings were built)"
print_info ""
print_info "For more options, see BUILD.md or the NLM documentation."
