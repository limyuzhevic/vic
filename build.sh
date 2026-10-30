#!/usr/bin/env bash

# NLM Build Script
# Standard build scripts for NLM (Neural Learning Machine)

BUILD_DIR="build"
CMAKE_BUILD_TYPE="Release"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default build mode: release
MODE="release"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            CMAKE_BUILD_TYPE="Debug"
            MODE="debug"
            shift
            ;;
        --clean)
            echo -e "${YELLOW}Cleaning build directory...${NC}"
            rm -rf "$BUILD_DIR"
            echo -e "${GREEN}Clean complete${NC}"
            shift
            ;;
        --help|-h)
            echo "NLM Build Script"
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --debug    Build in debug mode (slower, with debug symbols)"
            echo "  --clean    Clean build directory"
            echo "  --help     Show this help message"
            echo ""
            echo "If no mode is specified, builds in release mode."
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Creating build directory: $BUILD_DIR${NC}"
    mkdir -p "$BUILD_DIR"
fi

# Change to build directory
cd "$BUILD_DIR" || exit 1

# Configure with CMake
echo -e "${YELLOW}Configuring CMake with $CMAKE_BUILD_TYPE build...${NC}"
cmake .. -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" -GNinja

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed${NC}"
    exit 1
fi

# Build with parallel jobs
echo -e "${YELLOW}Building with $MODE configuration...${NC}"
# Try ninja first, then fallback to make
if command -v ninja &> /dev/null; then
    ninja -j$(nproc)
else
    make -j$(nproc)
fi

if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build successful!${NC}"
    echo ""
    echo "Built executables:"
    echo "  - ./nlm              (main simulation)"
    echo "  - ./nlm_phase3_demo  (Phase 3 demonstration)"
    echo "  - ./nlm_phase4_demo  (Phase 4 demonstration)"
    echo ""
    echo "To run a demo:"
    echo "  ./nlm_phase6_demo  # Comprehensive integration test"
else
    echo -e "${RED}Build failed${NC}"
    exit 1
fi
