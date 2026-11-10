#!/bin/bash

# NLM Brain Simulator Build and Test Script
# This script builds the project and runs all tests with modern improvements

set -e  # Exit on any error

# Configuration
PROJECT_NAME="NLM-Brain-Simulator"
BUILD_DIR="build"
TEST_DIR="tests"
CONFIG_FILE="configs/default.cfg"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print functions
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Parse command line arguments
BUILD_TESTS=true
RUN_TESTS=true
BUILD_TYPE="Release"
CLEAN=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --no-tests)
            BUILD_TESTS=false
            RUN_TESTS=false
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --no-tests    Build without tests and skip test execution"
            echo "  --debug       Build in Debug mode (with debug symbols)"
            echo "  --clean       Clean build directory before building"
            echo "  --help        Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    print_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure CMake
print_info "Configuring CMake build..."
cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DENABLE_TESTS=$BUILD_TESTS \
    -DCMAKE_INSTALL_PREFIX=../install ..

# Build the project
print_info "Building project..."
cmake --build . --config "$BUILD_TYPE" --parallel $(nproc)

# Check if build succeeded
if [ $? -ne 0 ]; then
    print_error "Build failed!"
    exit 1
fi

print_success "Build completed successfully!"

# Run tests if enabled
if [ "$RUN_TESTS" = true ]; then
    if [ ! -f "nlm_test_framework" ]; then
        print_warning "Test framework not found, skipping tests..."
    else
        print_info "Running tests..."
        ./nlm_test_framework
        
        if [ $? -eq 0 ]; then
            print_success "All tests passed!"
        else
            print_error "Tests failed!"
            exit 1
        fi
    fi
fi

print_success "Build and test process completed successfully!"

# Print build summary
echo ""
echo "Build Summary:"
echo "=============="
echo "Project: $PROJECT_NAME"
echo "Build Type: $BUILD_TYPE"
echo "Tests: $([ "$RUN_TESTS" = true ] && echo "Enabled" || echo "Disabled")"
echo "Build Directory: $BUILD_DIR"
echo ""

echo "To run tests manually:"
echo "  cd $BUILD_DIR"
echo "  ./nlm_test_framework"
echo ""

echo "To run the brain simulator:"
echo "  cd $BUILD_DIR"
echo "  ./nlm --help"
