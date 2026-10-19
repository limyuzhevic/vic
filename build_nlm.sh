#!/usr/bin/env bash
# NLM Build Script
# 
# This script builds all NLM executables and demonstrates various build configurations.
# It showcases the advanced build system and configuration options.

# Set strict error handling

set -e

# Configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
GENERATE_DOCS=false
RUN_TESTS=false
ENABLE_ADVANCED_FEATURES=true

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print usage information
usage() {
    cat << EOF
NLM Build Script
===============

Usage: $0 [OPTIONS]

Options:
    --type TYPE           Build type: Debug, Release, RelWithDebInfo, MinSizeRel
    --build-dir DIR       Build directory (default: build)
    --generate-docs       Generate documentation
    --run-tests          Run tests after building
    --no-advanced        Disable advanced features
    --help               Show this help message

Examples:
    $0 --type Release                     Build optimized release version
    $0 --type Debug --run-tests           Build debug version and run tests
    $0 --generate-docs                    Build and generate documentation

Build Targets:
    nlm                  Main NLM executable
    nlm_phase3_demo      Phase 3 demonstration
    nlm_phase4_demo      Phase 4 demonstration
    nlm_phase5_demo      Phase 5 demonstration (performance optimized)
    nlm_phase6_demo      Phase 6 demonstration (integration test)
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --generate-docs)
            GENERATE_DOCS=true
            shift
            ;;
        --run-tests)
            RUN_TESTS=true
            shift
            ;;
        --no-advanced)
            ENABLE_ADVANCED_FEATURES=false
            shift
            ;;
        --help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Print build information
echo -e "${BLUE}NLM Build Script${NC}"
echo -e "Build type: ${YELLOW}$BUILD_TYPE${NC}"
echo -e "Build directory: ${YELLOW}$BUILD_DIR${NC}"
echo -e "Advanced features: ${YELLOW}$ENABLE_ADVANCED_FEATURES${NC}"
echo -e "Generate documentation: ${YELLOW}$GENERATE_DOCS${NC}"
echo -e "Run tests: ${YELLOW}$RUN_TESTS${NC}"
echo

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${GREEN}Configuring CMake...${NC}"

CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ "$ENABLE_ADVANCED_FEATURES" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_ADVANCED_FEATURES=ON"
else
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_ADVANCED_FEATURES=OFF"
fi

if [ "$GENERATE_DOCS" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DBUILD_DOCUMENTATION=ON"
fi

cmake $CMAKE_ARGS ..

# Build all targets
echo -e "${GREEN}Building targets...${NC}"

# Main NLM executable
make nlm -j$(nproc)

# Phase 3 demo
make nlm_phase3_demo -j$(nproc)

# Phase 4 demo
make nlm_phase4_demo -j$(nproc)

# Phase 5 demo (performance optimized)
make nlm_phase5_demo -j$(nproc)

# Phase 6 demo (integration test)
make nlm_phase6_demo -j$(nproc)

# Documentation generation
if [ "$GENERATE_DOCS" = true ]; then
    echo -e "${GREEN}Generating documentation...${NC}"
    make docs
fi

# Run tests
if [ "$RUN_TESTS" = true ]; then
    echo -e "${GREEN}Running tests...${NC}"
    make test
fi

cd ..

# Print build summary
echo

echo -e "${GREEN}=== Build Summary ===${NC}"
echo -e "Build type: $BUILD_TYPE"
echo -e "Build directory: $BUILD_DIR"
echo -e "Generated executables:"
echo -e "  - $BUILD_DIR/nlm"
echo -e "  - $BUILD_DIR/nlm_phase3_demo"
echo -e "  - $BUILD_DIR/nlm_phase4_demo"
echo -e "  - $BUILD_DIR/nlm_phase5_demo"
echo -e "  - $BUILD_DIR/nlm_phase6_demo"

if [ "$GENERATE_DOCS" = true ]; then
    echo -e "  - Documentation in docs/"
fi

if [ "$RUN_TESTS" = true ]; then
    echo -e "  - Tests completed"
fi

echo

echo -e "${YELLOW}Quick usage examples:${NC}"
echo
echo -e "# Run main NLM with different modes:"
echo -e "./build/nlm --mode phase3 --trials 10 --steps 200"
echo -e "./build/nlm --mode phase4 --trials 50"
echo -e "./build/nlm --mode phase5 --trials 20"
echo -e "./build/nlm --mode phase6"
echo
echo -e "# Run Phase 3 demo directly:"
echo -e "./build/nlm_phase3_demo 5 200"
echo
echo -e "# Run Phase 4 demo directly:"
echo -e "./build/nlm_phase4_demo 50"
echo
echo -e "# Run Phase 5 demo (performance optimized):"
echo -e "./build/nlm_phase5_demo 10 100 true"
echo
echo -e "# Run Phase 6 demo (integration test):"
echo -e "./build/nlm_phase6_demo"
echo
echo -e "# View help for main NLM:"
echo -e "./build/nlm --help"

echo

echo -e "${GREEN}Build completed successfully!${NC}"
