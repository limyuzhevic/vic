#!/bin/bash

# NLM Test Runner Script
# Phase 6: Final Integration - Complete Test Suite Runner
#
# This script builds and runs all tests for the NLM project:
# 1. C++ tests (compiled and executed)
# 2. Python binding tests (run with pytest)
#
# Usage: ./run_tests.sh

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
CPP_TEST_BINARY="nlm_test"
PYTHON_TEST_SCRIPT="test_python_bindings.py"

function print_header() {
    echo -e "${GREEN}========================================================${NC}"
    echo -e "${GREEN} NLM Comprehensive Test Suite Runner${NC}"
    echo -e "${GREEN} Phase 6: Final Integration - Complete Test Coverage${NC}"
    echo -e "${GREEN}========================================================${NC}"
    echo
}

function print_step() {
    echo -e "${YELLOW}>>> $1${NC}"
    echo
}

function check_prerequisites() {
    print_step "Checking prerequisites..."
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}ERROR: CMake is not installed. Please install CMake to build tests.${NC}"
        exit 1
    fi
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null; then
        echo -e "${RED}ERROR: g++ is not installed. Please install g++ to build tests.${NC}"
        exit 1
    fi
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        echo -e "${RED}ERROR: Python3 is not installed. Please install Python3 to run Python tests.${NC}"
        exit 1
    fi
    
    # Check for pip
    if ! command -v pip3 &> /dev/null; then
        echo -e "${RED}WARNING: pip3 is not installed. Python tests may fail.${NC}"
    fi
    
    echo "Prerequisites check passed."
    echo
}

function build_cpp_tests() {
    print_step "Building C++ tests..."
    
    # Create build directory if it doesn't exist
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Run CMake
    echo "Running CMake configuration..."
    cmake $CMAKE_ARGS ..
    
    # Build the tests
    echo "Building C++ tests..."
    make nlm_test -j$(nproc)
    
    cd ..
    
    if [ -f "$BUILD_DIR/$CPP_TEST_BINARY" ]; then
        echo "C++ tests built successfully."
        return 0
    else
        echo -e "${RED}ERROR: Failed to build C++ tests.${NC}"
        return 1
    fi
}

function run_cpp_tests() {
    print_step "Running C++ tests..."
    
    if [ ! -f "$BUILD_DIR/$CPP_TEST_BINARY" ]; then
        echo -e "${RED}ERROR: C++ test binary not found. Please build first.${NC}"
        return 1
    fi
    
    # Run the C++ tests
    echo "Running C++ test suite..."
    "$BUILD_DIR/$CPP_TEST_BINARY"
    
    # Capture the result
    TEST_RESULT=$?
    
    if [ $TEST_RESULT -eq 0 ]; then
        echo -e "${GREEN}C++ tests PASSED.${NC}"
    else
        echo -e "${RED}C++ tests FAILED.${NC}"
    fi
    
    return $TEST_RESULT
}

function run_python_tests() {
    print_step "Running Python binding tests..."
    
    # Check if pytest is available
    if ! command -v pytest &> /dev/null; then
        echo -e "${YELLOW}WARNING: pytest not found. Installing pytest...${NC}"
        pip3 install pytest --quiet
    fi
    
    # Run Python tests
    echo "Running Python binding tests..."
    
    # Check if we can import pynlm (the actual bindings)
    cd python
    if python3 -c "import pynlm" 2>/dev/null; then
        echo "PyBind11 bindings available, running with pytest..."
        pytest test_python_bindings.py -v
    else
        echo "PyBind11 bindings not available, running standalone tests..."
        python3 test_python_bindings.py
    fi
    cd ..
    
    # Capture the result
    PYTHON_TEST_RESULT=$?
    
    if [ $PYTHON_TEST_RESULT -eq 0 ]; then
        echo -e "${GREEN}Python tests PASSED.${NC}"
    else
        echo -e "${RED}Python tests FAILED.${NC}"
    fi
    
    return $PYTHON_TEST_RESULT
}

function show_test_summary() {
    print_step "Test Summary..."
    echo "The comprehensive test suite covers:"
    echo "  1. CLI Interface Tests (main.cpp)"
    echo "     - --help, --phase2, --phase4, --phase6 flag parsing"
    echo "     - Configuration file loading"
    echo "     - Error handling for invalid arguments"
    echo "  2. Python Bindings Tests (python/bindings.cpp)"
    echo "     - Custom exception handling (NLMException hierarchy)"
    echo "     - New utility functions (create_brain_from_config_file, etc.)"
    echo "     - Improved APIs (run_lifetime, learn_from_experience, render_ascii)"
    echo "     - Type safety and thread safety"
    echo "     - Backward compatibility"
    echo "  3. Integration Tests"
    echo "     - Phase 2, Phase 4, and Phase 6 demo functionality"
    echo "     - Memory and neuromodulation integration"
    echo "     - Checkpoint save/load functionality"
    echo "  4. Performance and Edge Cases"
    echo "     - Error conditions and recovery"
    echo "     - Boundary conditions"
    echo "     - Memory usage and cleanup"
    echo "  5. Documentation Examples"
    echo "     - Code snippet validation"
    echo
}

function main() {
    print_header
    
    # Check if we're in the right directory
    if [ ! -f "CMakeLists.txt" ]; then
        echo -e "${RED}ERROR: CMakeLists.txt not found. Please run this script from the project root.${NC}"
        exit 1
    fi
    
    # Check prerequisites
    check_prerequisites
    
    # Build C++ tests
    if build_cpp_tests; then
        # Run C++ tests
        if run_cpp_tests; then
            CPP_TESTS_PASSED=true
        else
            CPP_TESTS_PASSED=false
        fi
    else
        echo -e "${RED}ERROR: Skipping C++ tests due to build failure.${NC}"
        CPP_TESTS_PASSED=false
    fi
    
    echo
    
    # Run Python tests
    if run_python_tests; then
        PYTHON_TESTS_PASSED=true
    else
        PYTHON_TESTS_PASSED=false
    fi
    
    echo
    
    # Show summary
    show_test_summary
    
    # Final result
    echo -e "${GREEN}========================================================${NC}"
    if [ "$CPP_TESTS_PASSED" = true ] && [ "$PYTHON_TESTS_PASSED" = true ]; then
        echo -e "${GREEN}ALL TESTS PASSED!${NC}"
        echo -e "${GREEN}The comprehensive test suite covers all new functionality${NC}"
        echo -e "${GREEN}and improvements as requested in Phase 6.${NC}"
    else
        echo -e "${RED}SOME TESTS FAILED!${NC}"
        if [ "$CPP_TESTS_PASSED" = false ]; then
            echo -e "${RED}C++ tests failed. Check build and test output.${NC}"
        fi
        if [ "$PYTHON_TESTS_PASSED" = false ]; then
            echo -e "${RED}Python tests failed. Check test output.${NC}"
        fi
        echo -e "${RED}Please review the test failures and fix any issues.${NC}"
    fi
    echo -e "${GREEN}========================================================${NC}"
}

# Run main function
main "$@"