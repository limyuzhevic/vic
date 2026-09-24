#!/bin/bash
# Build and run the enhanced NLM Phase 2 test suite
# This script demonstrates how to compile and run the enhanced test framework

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the project root directory."
    exit 1
fi

# Check if build directory exists
if [ ! -d "build" ]; then
    print_info "Creating build directory..."
    mkdir -p build
    cd build
else
    cd build
fi

# Configure with CMake
print_info "Configuring project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed."
    exit 1
fi

# Build the project
print_info "Building project..."
make -j4

if [ $? -ne 0 ]; then
    print_error "Build failed."
    exit 1
fi

# Run the enhanced test suite
print_info "Running enhanced NLM Phase 2 test suite..."
print_info "Use './nlm --verbose' for detailed output"
print_info "Use './nlm --save-results' to save results to CSV"
print_info "Use './nlm --help' for help information"
print_info ""
./nlm --verbose

# Capture the exit code
EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
    print_info "All tests passed successfully!"
elif [ $EXIT_CODE -eq 1 ]; then
    print_error "Some tests failed. Check the output above for details."
else
    print_error "Test execution failed with exit code $EXIT_CODE."
fi

exit $EXIT_CODE