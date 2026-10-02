#!/bin/bash
#
# NLM Build Automation Script
# Provides convenient build shortcuts and automation for the NLM project
#

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default build type
BUILD_TYPE="Release"

# Show help
function show_help() {
    echo "NLM Build Automation Script"
    echo "=========================="
    echo ""
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  build        Build the project (default)"
    echo "  build-debug  Build with debug symbols"
    echo "  clean        Clean build artifacts"
    echo "  test         Run tests"
    echo "  demo         Build and run demo programs"
    echo "  help         Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build-debug                    # Build with debug symbols"
    echo "  $0 clean                        # Clean build artifacts"
    echo "  $0 test                         # Run all tests"
    echo ""
}

# Build the project
function build_project() {
    local build_type="${1:-${BUILD_TYPE}}"
    
    echo -e "${BLUE}Building NLM with ${build_type} configuration...${NC}"
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure CMake
    cmake .. -DCMAKE_BUILD_TYPE="${build_type}"
    
    # Build with all cores
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Build successful!${NC}"
        echo -e "${YELLOW}Executables:${NC}"
        find . -name "nlm*" -type f -executable | head -10
    else
        echo -e "${RED}Build failed!${NC}"
        return 1
    fi
}

# Build with debug symbols
function build_debug() {
    build_project "Debug"
}

# Clean build artifacts
function clean_project() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    
    # Remove build directory
    if [ -d "build" ]; then
        rm -rf build
        echo -e "${GREEN}Build directory removed.${NC}"
    else
        echo -e "${YELLOW}No build directory found.${NC}"
    fi
    
    # Remove any other build artifacts
    for dir in debug build_*; do
        if [ -d "${dir}" ]; then
            rm -rf "${dir}"
        fi
    done
    
    echo -e "${GREEN}Clean complete.${NC}"
}

# Run tests
function run_tests() {
    echo -e "${BLUE}Running tests...${NC}"
    
    # Create test build directory
    mkdir -p test_build
    cd test_build
    
    # Configure with tests enabled
    cmake .. -DCMAKE_BUILD_TYPE="Debug" -DBUILD_TESTING=ON
    
    # Build tests
    make -j$(nproc)
    
    # Run tests
    if [ -f "ctest" ]; then
        ./ctest --output-on-failure
        return $?
    else
        echo -e "${RED}Test executable not found.${NC}"
        return 1
    fi
}

# Build and run demos
function build_and_run_demos() {
    echo -e "${BLUE}Building and running demo programs...${NC}"
    
    # Build main project
    build_project "Release"
    
    if [ $? -ne 0 ]; then
        return 1
    fi
    
    cd build
    
    # Run demo programs
    echo -e "${YELLOW}Running demos...${NC}"
    
    # Phase 3 demo
    if [ -f "nlm_phase3_demo" ]; then
        echo "Running Phase 3 demo..."
        ./nlm_phase3_demo
        if [ $? -ne 0 ]; then
            echo -e "${RED}Phase 3 demo failed.${NC}"
        fi
    fi
    
    # Phase 4 demo
    if [ -f "nlm_phase4_demo" ]; then
        echo "Running Phase 4 demo..."
        ./nlm_phase4_demo
        if [ $? -ne 0 ]; then
            echo -e "${RED}Phase 4 demo failed.${NC}"
        fi
    fi
    
    # Phase 6 demo
    if [ -f "nlm_phase6_demo" ]; then
        echo "Running Phase 6 demo..."
        ./nlm_phase6_demo
        if [ $? -ne 0 ]; then
            echo -e "${RED}Phase 6 demo failed.${NC}"
        fi
    fi
}

# Main script logic
function main() {
    local command="${1:-build}"
    
    case "${command,,}" in
        build|build-debug|clean|test|demo|help)
            shift
            "${command}" "$@"
            ;;
        *)
            echo -e "${RED}Unknown command: ${command}${NC}"
            show_help
            return 1
            ;;
    esac
}

# Execute main function with all arguments
main "$@"