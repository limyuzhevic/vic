#!/usr/bin/env bash
"""
Build script for Enhanced NLM Python Bindings

This script provides easy build and installation instructions for the
enhanced NLM Python bindings.
"""

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Function to print colored messages
print_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check Python version
check_python() {
    if ! command_exists python3; then
        print_error "Python3 is not installed. Please install Python3."
        exit 1
    fi

    PYTHON_VERSION=$(python3 --version 2>&1 | grep -o 'Python [0-9]*\.[0-9]*')
    print_info "Found Python version: $PYTHON_VERSION"

    # Check if version is sufficient
    if [[ "$PYTHON_VERSION" < "Python 3.6" ]]; then
        print_warning "Python version $PYTHON_VERSION is below the recommended version 3.6"
    fi
}

# Function to check pybind11
check_pybind11() {
    if ! command_exists cmake; then
        print_error "CMake is not installed. Please install CMake."
        exit 1
    fi

    # Check if pybind11 is available
    if [ -f "/usr/local/include/pybind11/pybind11.h" ] || \
       [ -f "/usr/include/pybind11/pybind11.h" ]; then
        print_info "Found pybind11 headers"
    else
        print_warning "pybind11 headers not found in standard locations"
        print_info "Trying to install pybind11 via pip..."

        if command_exists pip3; then
            pip3 install pybind11
        else
            print_error "pip3 is not available. Please install pybind11 manually."
            print_info "Install instructions: https://pybind11.readthedocs.io/en/stable/faq.html"
            exit 1
        fi
    fi
}

# Function to build enhanced bindings
build_enhanced_bindings() {
    print_info "Building Enhanced NLM Python Bindings..."

    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    # Run CMake
    print_info "Running CMake..."
    cmake "$PROJECT_ROOT/python" \
        -DPYTHON_EXECUTABLE=$(which python3) \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_TESTING=OFF

    # Build the module
    print_info "Building module..."
    make -j$(nproc)

    # Check if build was successful
    if [ -f "nlm_enhanced$(python3-config --extension-suffix 2>/dev/null || echo '.so')" ]; then
        print_info "✓ Enhanced bindings built successfully"
        print_info "Module location: $BUILD_DIR/nlm_enhanced$(python3-config --extension-suffix 2>/dev/null || echo '.so')"
    else
        print_error "✗ Enhanced bindings build failed"
        exit 1
    fi

    cd "$SCRIPT_DIR"
}

# Function to install enhanced bindings
install_enhanced_bindings() {
    print_info "Installing Enhanced NLM Python bindings..."

    # Create build directory if it doesn't exist
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    # Install the module
    print_info "Installing module..."
    make install

    # Check if installation was successful
    if [ -f "${PROJECT_ROOT}/python/nlm_enhanced$(python3-config --extension-suffix 2>/dev/null || echo '.so')" ]; then
        print_info "✓ Enhanced bindings installed successfully"
        print_info "Module location: ${PROJECT_ROOT}/python/nlm_enhanced$(python3-config --extension-suffix 2>/dev/null || echo '.so')"
    else
        print_warning "Installation may have succeeded but module not found in expected location"
    fi

    cd "$SCRIPT_DIR"
}

# Function to run tests
run_tests() {
    print_info "Running tests..."

    # Check if module can be imported
    if python3 -c "from nlm_enhanced import *" 2>/dev/null; then
        print_info "✓ Module import test passed"
    else
        print_warning "Module import test failed"
        print_info "This may be expected if the module hasn't been built yet."
    fi

    # Run demo script if available
    if [ -f "demo_enhanced.py" ]; then
        print_info "Running demo script..."
        python3 demo_enhanced.py
    fi
}

# Function to show build information
show_build_info() {
    print_info "Enhanced NLM Python Bindings Build Information"
    print_info "==========================="
    print_info "Version: 2.0.0"
    print_info "Build date: $(date)"
    print_info "Python version: $(python3 --version)"
    print_info "CMake version: $(cmake --version | head -1)"
    print_info "Platform: $(uname -s)"
    print_info "Compiler: $(c++ --version | head -1)"

    if [ -f "build/Makefile" ]; then
        if grep -q "pybind11" "build/Makefile"; then
            print_info "✓ pybind11 support: Yes"
        else
            print_info "✓ pybind11 support: No"
        fi
    fi
}

# Function to show usage information
show_usage() {
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  build          Build enhanced bindings (default)"
    echo "  install        Install enhanced bindings to system"
    echo "  test           Run tests"
    echo "  info           Show build information"
    echo "  help           Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build        Build the enhanced bindings"
    echo "  $0 install      Install the enhanced bindings"
    echo "  $0 test        Run tests after building"
    echo ""
    echo "After building, you can use the enhanced bindings with:"
    echo "  from nlm_enhanced import *"
    echo ""
    echo "For more information, see:"
    echo "  README.md in the python directory"
    echo "  docs/ directory for additional documentation"
}

# Main script logic
case "$1" in
    build)
        print_info "Building Enhanced NLM Python bindings..."
        check_python
        check_pybind11
        build_enhanced_bindings
        run_tests
        ;;

    install)
        print_info "Installing Enhanced NLM Python bindings..."
        check_python
        check_pybind11
        build_enhanced_bindings
        install_enhanced_bindings
        ;;

    test)
        print_info "Running tests..."
        check_python
        run_tests
        ;;

    info)
        show_build_info
        ;;

    help|--help|-h)
        show_usage
        ;;

    *)
        echo "Error: Unknown command '$1'"
        echo ""
        show_usage
        exit 1
        ;;
esac

# Print completion message
print_info "Build process completed successfully!"
print_info "You can now use the enhanced NLM bindings with:")
print_info "  from nlm_enhanced import *"
print_info ""
print_info "For more information, see the README.md file in the python directory.
