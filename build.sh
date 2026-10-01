#!/bin/bash

# NLM Build Script
# Phase 6: FINAL INTEGRATION

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="${BUILD_DIR:-build}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
GENERATE_TESTS="${GENERATE_TESTS:-off}"

# Print usage
print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --build-dir DIR      Build directory (default: build)"
    echo "  --build-type TYPE    Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)"
    echo "  --tests              Generate tests (default: off)"
    echo "  --help               Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                           Build in release mode (default)"
    echo "  $0 --build-dir build_debug --build-type Debug"
    echo "  $0 --tests                    Build with tests enabled"
    echo ""
    echo "After building, run:"
    echo "  $BUILD_DIR/nlm                     Run default integration test"
    echo "  $BUILD_DIR/nlm_phase3_demo         Run Phase 3 demo"
    echo "  $BUILD_DIR/nlm_phase4_demo         Run Phase 4 demo"
    echo "  $BUILD_DIR/nlm_test                Run unit tests"
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --tests)
            GENERATE_TESTS="on"
            shift
            ;;
        --help)
            print_usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo -e "${GREEN}Configuring project with CMake...${NC}"
echo "Build directory: $BUILD_DIR"
echo "Build type: $BUILD_TYPE"
echo "Generate tests: $GENERATE_TESTS"
echo ""

cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          -DBUILD_TESTS="$GENERATE_TESTS"

# Check if CMake configuration succeeded
if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
fi

# Build the project
echo -e "${GREEN}Building project...${NC}"
make -j$(nproc)

# Check if build succeeded
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"
echo ""
echo "Binaries available in: $(pwd)"
echo "  - nlm                  : Main integration test"
echo "  - nlm_phase3_demo      : Phase 3 demonstration"
echo "  - nlm_phase4_demo      : Phase 4 demonstration"
echo "  - nlm_test             : Unit tests"
echo ""
echo "Run with: ./nlm"
