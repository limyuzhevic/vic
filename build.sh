#!/bin/bash
# NLM Build Script - Comprehensive build and test automation
# Usage: ./build.sh [options]
# Options: --help, --clean, --all, --tests, --docs, --install

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
ENABLE_TESTS=true
ENABLE_DOCS=false
ENABLE_INSTALL=false
PARALLEL_JOBS=$(nproc)
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --clean)
            clean_build
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --tests)
            ENABLE_TESTS=true
            shift
            ;;
        --no-tests)
            ENABLE_TESTS=false
            shift
            ;;
        --docs)
            ENABLE_DOCS=true
            shift
            ;;
        --install)
            ENABLE_INSTALL=true
            shift
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --build-dir=*)  # Changed from --build-dir= to --build-dir=* format
            BUILD_DIR="${1#*=}"
            shift
            ;;
        --jobs=*)  # Changed from --jobs= to --jobs=* format
            PARALLEL_JOBS="${1#*=}"
            shift
            ;;
        --help-documents)
            show_documentation
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

function show_help() {
    cat << EOF
NLM Build Script
================

Usage: ./build.sh [options]

Options:
  --help, -h              Show this help message
  --clean                 Clean build directory and rebuild
  --debug                 Build in Debug mode (with debug symbols)
  --tests                 Build and run tests (default)
  --no-tests              Build without tests
  --docs                   Build documentation (if available)
  --install               Install to system directories (requires sudo)
  --verbose, -v           Enable verbose output
  --build-dir=<dir>       Set build directory (default: build)
  --jobs=<num>            Set number of parallel jobs (default: $(nproc))
  --help-documents         Show documentation generation options

Build Types:
  Release (default)       Optimized build with all features
  Debug                   Build with debug symbols and assertions

Examples:
  ./build.sh                    # Standard release build with tests
  ./build.sh --debug            # Debug build
  ./build.sh --clean --tests    # Clean and rebuild with tests
  ./build.sh --build-dir=build2  # Build in custom directory
  ./build.sh --jobs=8           # Use 8 parallel jobs

The build script creates the following targets:
  nlm                    - Primary Phase 2 demo executable
  nlm_phase3_demo        - Phase 3 demonstration (World interaction)
  nlm_phase4_demo        - Phase 4 demonstration (Cognition)
  nlm_phase6_demo        - Phase 6 demo (Full integration)

After building, you can run:
  ./build/nlm                  - Phase 2 neural computation demo
  ./build/nlm_phase6_demo       - Phase 6 integration demo (recommended)
  ./build/nlm_phase4_demo       - Phase 4 cognition demo
  ./build/nlm_phase3_demo       - Phase 3 world interaction demo

Testing:
  ./build.sh --tests           # Build and run unit tests
  cd build && ctest              # Run tests after manual build

Documentation:
  Check the docs/ directory for comprehensive documentation
  ./build.sh --docs            # Build documentation (if supported)
EOF
}

function show_documentation() {
    cat << EOF
Documentation Build Options:
===========================

This project includes extensive documentation in the docs/ directory:

Core Documentation:
  docs/ARCHITECTURE.md           - System architecture overview
  docs/SCIENCE.md                - Scientific background and principles
  docs/ROADMAP.md                - Project roadmap and future plans
  docs/EXPERIMENTS.md            - Experiment descriptions and results
  docs/LIMITATIONS.md           - Known limitations and challenges
  docs/PHASE6_FINAL_AUDIT.md     - Phase 6 integration audit

User Guides:
  docs/USER_GUIDE.md            - Comprehensive user guide
  docs/API_REFERENCE.md         - Complete API documentation
  docs/CONFIGURATION.md          - Configuration options and examples

Advanced Topics:
  docs/ADVANCED_TOPICS.md        - Advanced usage and customization
  docs/PERFORMANCE_TUNING.md     - Performance optimization techniques
  docs/RESEARCH_METHODS.md      - Research methods and protocols

Quick Start Documentation:
  README.md                     - Quick start guide (this file)
  easy_usage.md                 - Beginner-friendly usage guide
  HOW_TO_USE.md                 - Detailed usage instructions

Building Documentation:
  Some documentation is auto-generated from source code comments.
  Check the CMakeLists.txt for documentation generation options.

For information on building documentation from source:
  - Check the project root for documentation generation scripts
  - Look for MkDocs, Doxygen, or similar documentation tools
  - See docs/ directory for configuration files

Note: This is a research project, and documentation is continuously updated.
EOF
}

function clean_build() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        echo -e "${GREEN}Build directory removed.${NC}"
    else
        echo -e "${BLUE}Build directory not found.${NC}"
    fi
}

function run_build() {
    echo -e "${BLUE}Configuring NLM build with CMake...${NC}"
    echo "Build type: $BUILD_TYPE"
    echo "Build directory: $BUILD_DIR"
    echo "Parallel jobs: $PARALLEL_JOBS"
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    if [ "$VERBOSE" = true ]; then
        cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
                 -DCMAKE_CXX_STANDARD=20 \
                 -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                 -DENABLE_TESTS=$ENABLE_TESTS \
                 -DENABLE_DOCS=$ENABLE_DOCS
    else
        cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
                 -DCMAKE_CXX_STANDARD=20 \
                 -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                 -DENABLE_TESTS=$ENABLE_TESTS \
                 -DENABLE_DOCS=$ENABLE_DOCS > /dev/null 2>&1
    fi
    
    # Build
    echo -e "${BLUE}Building NLM...${NC}"
    if [ "$VERBOSE" = true ]; then
        make -j$PARALLEL_JOBS VERBOSE=1
    else
        make -j$PARALLEL_JOBS > /dev/null 2>&1
    fi
    
    # Build documentation if requested
    if [ "$ENABLE_DOCS" = true ]; then
        echo -e "${BLUE}Building documentation...${NC}"
        # Add documentation build commands here
        echo "Documentation build not yet configured."
    fi
    
    cd ..  # Return to project root
    
    # List built targets
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo "\nBuilt targets:"
    for target in "$BUILD_DIR/nlm" "$BUILD_DIR/nlm_phase3_demo" "$BUILD_DIR/nlm_phase4_demo" "$BUILD_DIR/nlm_phase6_demo"; do
        if [ -f "$target" ]; then
            echo "  ✓ $target ($(ls -lh $target | awk '{print $5}'))"
        fi
    done
}

function run_tests() {
    if [ "$ENABLE_TESTS" = true ]; then
        echo -e "${BLUE}Running tests...${NC}"
        cd "$BUILD_DIR"
        
        # Run CTest
        if command -v ctest &> /dev/null; then
            ctest --output-on-failure
        else
            echo -e "${YELLOW}CTest not found. Skipping tests.${NC}"
        fi
        
        # Run any Python tests if available
        if [ -f "../python/setup.py" ] || [ -f "../pyproject.toml" ]; then
            echo -e "${BLUE}Running Python tests...${NC}"
            cd ..
            if command -v pytest &> /dev/null; then
                pytest python/ -v
            else
                echo -e "${YELLOW}pytest not found. Skipping Python tests.${NC}"
            fi
        fi
        
        cd ..  # Return to project root
    else
        echo -e "${YELLOW}Tests disabled.${NC}"
    fi
}

function run_install() {
    if [ "$ENABLE_INSTALL" = true ]; then
        echo -e "${BLUE}Installing NLM...${NC}"
        cd "$BUILD_DIR"
        
        if command -v sudo &> /dev/null; then
            sudo make install
        else
            echo -e "${YELLOW}sudo not available. Skipping installation.${NC}"
        fi
        
        cd ..  # Return to project root
    else
        echo -e "${YELLOW}Installation not requested.${NC}"
    fi
}

# Main execution
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    show_help
    exit 0
elif [ "$1" = "--help-documents" ]; then
    show_documentation
    exit 0
elif [ "$1" = "--clean" ]; then
    clean_build
    run_build
elif [ "$1" = "--install" ]; then
    run_build
    run_install
elif [ "$1" = "--tests" ] || [ "$1" = "--no-tests" ]; then
    run_build
    run_tests
else
    # Default: build with tests
    run_build
    run_tests
fi