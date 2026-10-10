#!/usr/bin/env bash

# Memory System Test Runner
# This script provides convenient access to memory system tests

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/../build"

# Default build type
BUILD_TYPE="Release"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug      Build and run in Debug mode"
            echo "  --help, -h   Show this help message"
            echo ""
            echo "This script runs the NLM Memory System Integration Tests."
            echo "It builds the project if needed, then runs the memory system tests."
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo "=== NLM Memory System Test Runner ==="
echo "Build type: ${BUILD_TYPE}"
echo "Script directory: ${SCRIPT_DIR}"
echo

# Check if build directory exists
if [[ ! -d "${BUILD_DIR}" ]]; then
    echo "Build directory not found. Building project..."
    echo "To build, run: cmake -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
    echo "Then run: cmake --build ${BUILD_DIR}"
    echo ""
    echo "Attempting to build now..."
    
    # Create build directory
    mkdir -p "${BUILD_DIR}"
    
    # Configure CMake
    if cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"; then
        echo "CMake configuration successful."
    else
        echo "${RED}CMake configuration failed.${NC}"
        echo "Please build manually:"
        echo "  cmake -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
        echo "  cmake --build ${BUILD_DIR}"
        exit 1
    fi
    
    # Build the project
    if cmake --build "${BUILD_DIR}"; then
        echo "Build successful."
    else
        echo "${RED}Build failed.${NC}"
        echo "Please build manually:"
        echo "  cmake --build ${BUILD_DIR}"
        exit 1
    fi
fi

# Check if test executables exist
MEMORY_TEST_EXEC="${BUILD_DIR}/nlm_memory_standalone"
REGULAR_TEST_EXEC="${BUILD_DIR}/nlm_test"

if [[ ! -f "${MEMORY_TEST_EXEC}" ]]; then
    echo "${YELLOW}Memory test executable not found. Looking for nlm_test...${NC}"
    if [[ -f "${REGULAR_TEST_EXEC}" ]]; then
        echo "Using nlm_test as memory test (memory_system_tests.cpp is part of it)"
        TEST_EXEC="${REGULAR_TEST_EXEC}"
    else
        echo "${RED}Neither nlm_memory_standalone nor nlm_test found.${NC}"
        echo "Please build the project first:"
        echo "  cmake --build ${BUILD_DIR}"
        exit 1
    fi
else
    echo "Found memory test executable: ${MEMORY_TEST_EXEC}"
    TEST_EXEC="${MEMORY_TEST_EXEC}"
fi

# Run the memory system tests
echo ""
echo "=== Running Memory System Tests ==="

echo "Test executable: ${TEST_EXEC}"
echo "Arguments: --gtest_filter=*memory* (if available)"
echo ""

# Run the tests
if "${TEST_EXEC}"; then
    echo ""
    echo "${GREEN}=== All Memory System Tests Passed ===${NC}"
    exit 0
else
    echo ""
    echo "${RED}=== Some Memory System Tests Failed ===${NC}"
    echo "Check the output above for details on failed tests."
    exit 1
fi
