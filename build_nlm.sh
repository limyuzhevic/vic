#!/bin/bash
"""
NLM (Neural Learning Machine) Build and Test Script

This script provides convenient commands for building and testing the NLM project.
It can be used to compile the C++ core and run the Python tests.

Usage:
    ./build_nlm.sh [command]

Commands:
    build        Build the NLM C++ library and Python bindings
    test         Run the Python bindings tests
    test-all     Run all tests (unit tests, integration tests, Python tests)
    clean        Clean build artifacts
    help         Show this help message
"""

set -e

# Project directories
PROJECT_ROOT="/workspace/app"
BUILD_DIR="$PROJECT_ROOT/build"
PYTHON_DIR="$PROJECT_ROOT/python"
SRC_DIR="$PROJECT_ROOT/src"
TESTS_DIR="$PROJECT_ROOT/tests"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function print_header() {
    echo -e "${GREEN}=== NLM Build and Test Script ===${NC}"
    echo "$1"
    echo -e "=================================${NC}\n"
}

function print_success() {
    echo -e "${GREEN}✅ ${1}${NC}"
}

function print_error() {
    echo -e "${RED}❌ ${1}${NC}"
}

function print_warning() {
    echo -e "${YELLOW}⚠️  ${1}${NC}"
}

function check_dependencies() {
    print_header "Checking Dependencies..."
    
    # Check for CMake
    if command -v cmake &> /dev/null; then
        print_success "CMake found: $(cmake --version | head -1)"
    else
        print_error "CMake not found. Please install CMake."
        return 1
    fi
    
    # Check for C++ compiler
    if command -v g++ &> /dev/null; then
        print_success "g++ found: $(g++ --version | head -1)"
    elif command -v clang++ &> /dev/null; then
        print_success "clang++ found: $(clang++ --version | head -1)"
    else
        print_error "C++ compiler not found. Please install g++ or clang++."
        return 1
    fi
    
    # Check for Python
    if command -v python3 &> /dev/null; then
        print_success "Python3 found: $(python3 --version)"
    else
        print_error "Python3 not found. Please install Python3."
        return 1
    fi
    
    # Check for pip
    if command -v pip3 &> /dev/null; then
        print_success "pip3 found: $(pip3 --version | head -1)"
    else
        print_error "pip3 not found. Please install pip3."
        return 1
    fi
    
    return 0
}

function build_nlm() {
    print_header "Building NLM..."
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    print "Configuring CMake..."
    if cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release; then
        print_success "CMake configuration successful"
    else
        print_error "CMake configuration failed"
        return 1
    fi
    
    # Build the project
    print "Building NLM library and executables..."
    if make -j$(nproc); then
        print_success "Build successful"
        print_success "Build directory: $BUILD_DIR"
        print_success "Executables: $BUILD_DIR/nlm"
        print_success "Python bindings: $BUILD_DIR/python/pynlm*.so"
    else
        print_error "Build failed"
        return 1
    fi
    
    # Return to project root
    cd "$PROJECT_ROOT"
    
    return 0
}

function test_python_bindings() {
    print_header "Testing Python Bindings..."
    
    # Check if Python module exists
    if [ ! -f "$BUILD_DIR/python/pynlm*.so" ]; then
        print_error "Python bindings not found. Build the project first."
        return 1
    fi
    
    # Find the Python module path
    PYTHON_MODULE=$(find "$BUILD_DIR/python" -name "*.so" -type f | head -1)
    
    if [ -z "$PYTHON_MODULE" ]; then
        print_error "Could not find Python bindings module"
        return 1
    fi
    
    print "Found Python module: $PYTHON_MODULE"
    
    # Add module to Python path
    export PYTHONPATH="$(dirname "$PYTHON_MODULE"):$PYTHONPATH"
    
    # Test 1: Basic import
    print "Test 1: Importing pynlm..."
    if python3 -c "import pynlm; print('✅ pynlm imported successfully')"; then
        print_success "Python bindings import test passed"
    else
        print_error "Python bindings import test failed"
        return 1
    fi
    
    # Test 2: Basic functionality
    print "Test 2: Testing basic functionality..."
    if python3 "/workspace/app/test_pynlm.py"; then
        print_success "Python bindings functionality test passed"
    else
        print_error "Python bindings functionality test failed"
        return 1
    fi
    
    # Test 3: Integration tests
    print "Test 3: Running integration tests..."
    if python3 "/workspace/app/test_integration.py"; then
        print_success "Integration tests passed"
    else
        print_error "Integration tests failed"
        return 1
    fi
    
    return 0
}

function test_all() {
    print_header "Running All Tests..."
    
    # Test C++ unit tests if available
    if [ -f "$BUILD_DIR/nlm_test" ]; then
        print "Running C++ unit tests..."
        if "$BUILD_DIR/nlm_test"; then
            print_success "C++ unit tests passed"
        else
            print_error "C++ unit tests failed"
            return 1
        fi
    else
        print_warning "C++ unit test executable not found. Skipping C++ tests."
    fi
    
    # Test Python bindings
    if test_python_bindings; then
        print_success "All tests passed"
        return 0
    else
        print_error "Some tests failed"
        return 1
    fi
}

function clean_build() {
    print_header "Cleaning Build Artifacts..."
    
    if [ -d "$BUILD_DIR" ]; then
        print "Removing build directory..."
        rm -rf "$BUILD_DIR"
        print_success "Build directory removed"
    else
        print_warning "Build directory not found"
    fi
    
    # Remove Python cache
    if [ -d "$PROJECT_ROOT/python/__pycache__" ]; then
        print "Removing Python cache..."
        rm -rf "$PROJECT_ROOT/python/__pycache__"
        print_success "Python cache removed"
    fi
    
    # Remove test cache
    if [ -d "$PROJECT_ROOT/__pycache__" ]; then
        print "Removing test cache..."
        rm -rf "$PROJECT_ROOT/__pycache__"
        print_success "Test cache removed"
    fi
    
    return 0
}

function show_help() {
    cat << EOF
NLM (Neural Learning Machine) Build and Test Script

Usage: $0 <command>

Commands:
  build          Build the NLM C++ library and Python bindings
  test           Run the Python bindings tests
  test-all       Run all tests (unit tests, integration tests, Python tests)
  clean          Clean build artifacts
  help           Show this help message

Examples:
  $0 build       # Build the NLM project
  $0 test        # Test the Python bindings
  $0 test-all    # Run all tests
  $0 clean       # Clean build artifacts

Note: The build directory will be created at /workspace/app/build
      Python bindings will be installed to $BUILD_DIR/python/

For more information, visit: https://github.com/nlm-project/nlm
EOF
}

# Main script logic
case "$1" in
    build)
        check_dependencies
        build_nlm
        ;;
    test)
        if [ ! -d "$BUILD_DIR" ]; then
            print_error "Build directory not found. Build the project first."
            exit 1
        fi
        test_python_bindings
        ;;
    test-all)
        if [ ! -d "$BUILD_DIR" ]; then
            print_error "Build directory not found. Build the project first."
            exit 1
        fi
        test_all
        ;;
    clean)
        clean_build
        ;;
    help|*
        show_help
        ;;
esac

exit 0