#!/usr/bin/env bash
# NLM Example Build Script
# This script builds and runs the NLM example program

set -euo pipefail

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Check if NLM library is built
if [[ ! -d "$BUILD_DIR" ]]; then
    print_error "NLM build directory not found. Please build NLM first."
    print_info "To build NLM, run: mkdir build && cd build && cmake .. && make"
    exit 1
fi

# Check if libraries are built
if [[ ! -f "$BUILD_DIR/libnlm_core.a" ]]; then
    print_error "NLM library not found in build directory."
    print_info "Please build NLM first by running: cd build && make"
    exit 1
fi

print_info "Building NLM example..."

# Compile the example
COMPILER="g++"
CXXFLAGS="-std=c++17 -I${SCRIPT_DIR}/src -I${SCRIPT_DIR}"

# Create build directory for example
mkdir -p "${SCRIPT_DIR}/example_build"

# Compile the example
cd "${SCRIPT_DIR}/example_build"

${COMPILER} ${CXXFLAGS} "${SCRIPT_DIR}/example_main.cpp" \
    "${BUILD_DIR}/libnlm_core.a" \
    -o example

if [[ $? -ne 0 ]]; then
    print_error "Failed to compile NLM example."
    exit 1
fi

print_success "NLM example compiled successfully."

print_info "Running NLM example..."

# Run the example
if [[ -f "example" ]]; then
    ./example
    
    if [[ $? -eq 0 ]]; then
        print_success "NLM example executed successfully!"
    else
        print_error "NLM example execution failed."
        exit 1
    fi
else
    print_error "Example executable not found."
    exit 1
fi

cd "${SCRIPT_DIR}"
print_success "NLM example completed successfully!"
