#!/bin/bash

set -e

# NLM Build Script
# Builds the Neural Learning Machine project with all Phase 6 integration

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default build configuration
BUILD_DIR="build"
BUILD_TYPE="Release"
PARALLEL_JOBS=$(nproc)

# Usage information
function show_usage() {
    cat << EOF
NLM Build Script

Usage: $0 [OPTIONS]

Options:
  -h, --help              Show this help message
  -d, --debug             Build in Debug mode
  -b, --build-dir DIR     Specify build directory (default: build)
  -j, --jobs N            Number of parallel jobs (default: all cores)
  -c, --clean             Clean build directory before building
  -t, --tests             Run tests after building

Examples:
  $0                        # Build in Release mode
  $0 --debug                # Build in Debug mode
  $0 --clean --tests        # Clean build and run tests
EOF
}

# Error handling
function handle_error() {
    echo -e "${RED}Error: Build failed at line $1${NC}" >&2
    exit 1
}

trap 'handle_error $LINENO' ERR

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_usage
            exit 0
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -t|--tests)
            RUN_TESTS=true
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}" >&2
            show_usage
            exit 1
            ;;
    esac
done

# Print build configuration
echo -e "${BLUE}NLM Build Script${NC}"
echo -e "Build directory: ${YELLOW}$BUILD_DIR${NC}"
echo -e "Build type: ${YELLOW}$BUILD_TYPE${NC}"
echo -e "Parallel jobs: ${YELLOW}$PARALLEL_JOBS${NC}"

# Check if build directory exists
if [ "$CLEAN_BUILD" = true ] && [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${YELLOW}Configuring CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF

# Build the project
echo -e "${YELLOW}Building project...${NC}"
make -j"$PARALLEL_JOBS"

# Show available executables
echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${BLUE}Available executables:${NC}"
for exe in "$BUILD_DIR"/nlm* 2>/dev/null; do
    if [ -x "$exe" ]; then
        echo -e "  ${GREEN}$exe${NC}"
    fi
done

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    echo -e "${YELLOW}Running tests...${NC}"
    ctest --output-on-failure
fi

cd ..

echo -e "${GREEN}Build script completed!${NC}}"