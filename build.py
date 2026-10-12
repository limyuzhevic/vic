#! /usr/bin/env bash

# Simple build script for NLM Python bindings
# This script provides a user-friendly interface to CMake

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored messages
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check for required tools
check_requirements() {
    print_info "Checking build requirements..."
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is required but not installed. Please install CMake 3.16 or higher."
        exit 1
    fi
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        print_error "Python 3.8+ is required but not found. Please install Python 3.8 or higher."
        exit 1
    fi
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "A C++ compiler (g++ or clang++) is required but not found."
        exit 1
    fi
    
    print_info "All requirements satisfied."
}

# Build function
build() {
    local build_type="${1:-Release}"
    local install_prefix="${2:-/usr/local}"
    
    print_info "Building NLM Python bindings (${build_type})..."
    
    # Create build directory
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Configure CMake
    print_info "Configuring CMake..."
    cmake \
        -DCMAKE_BUILD_TYPE="${build_type}" \
        -DCMAKE_INSTALL_PREFIX="${install_prefix}" \
        -DBUILD_PYTHON_PACKAGE=ON \
        -DBUILD_TESTS=OFF \
        -DBUILD_EXAMPLES=ON \
        "${SCRIPT_DIR}/python"
    
    # Build
    print_info "Building NLM Python bindings..."
    make -j$(nproc)
    
    # Install
    print_info "Installing NLM Python bindings..."
    make install
    
    print_info "Build and installation completed successfully!"
}

# Test function
test_build() {
    local build_dir="${1:-${BUILD_DIR}}"
    
    if [ ! -d "${build_dir}" ]; then
        print_error "Build directory not found: ${build_dir}"
        print_warning "Run './build.sh build' first to build the project."
        exit 1
    fi
    
    cd "${build_dir}"
    print_info "Running tests..."
    ctest --output-on-failure
}

# Clean function
clean() {
    local build_dir="${1:-${BUILD_DIR}}"
    
    if [ -d "${build_dir}" ]; then
        print_info "Cleaning build directory: ${build_dir}"
        rm -rf "${build_dir}"
        print_info "Build directory cleaned."
    else
        print_warning "Build directory not found: ${build_dir}"
    fi
}

# Help function
help() {
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  build [BUILD_TYPE] [PREFIX]   Build and install the project"
    echo "    BUILD_TYPE   Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)"
    echo "    PREFIX       Installation prefix (default: /usr/local)"
    echo ""
    echo "  test [BUILD_DIR]               Run tests"
    echo "    BUILD_DIR   Build directory (default: ./build)"
    echo ""
    echo "  clean [BUILD_DIR]              Clean build directory"
    echo "    BUILD_DIR   Build directory (default: ./build)"
    echo ""
    echo "  help                             Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build                       Build and install in /usr/local"
    echo "  $0 build Debug /opt/nlm        Build Debug version in /opt/nlm"
    echo "  $0 test ./build               Run tests from ./build"
    echo "  $0 clean                       Clean build directory"
}

# Main script logic
case "${1:-build}" in
    build)
        build "${2:-Release}" "${3:-/usr/local}"
        ;;
    test)
        test_build "${2:-${BUILD_DIR}}"
        ;;
    clean)
        clean "${2:-${BUILD_DIR}}"
        ;;
    help|--help|-h)
        help
        ;;
    *)
        print_error "Unknown command: ${1}"
        help
        exit 1
        ;;
esac