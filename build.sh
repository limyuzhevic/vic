#!/bin/bash

# NLM Build Script
# Enhanced build script for NLM (熙然) brain simulator
# Created as part of code improvements to standardize build process

# Set up colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default build configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
NUM_CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
PYTHON_BUILD=false
SKIP_TESTS=false
CLEAN_BUILD=false

# Print usage information
usage() {
    cat << EOF
NLM Build Script
================

Usage: $0 [options]

Options:
  --build-type TYPE    Build type (Debug, Release, RelWithDebInfo)
  --build-dir DIR      Build directory (default: build)
  --cores NUM          Number of parallel build cores (default: auto-detect)
  --python             Build Python bindings
  --skip-tests         Skip running tests
  --clean              Perform clean build (remove existing build directory)
  --help               Show this help message

Examples:
  $0                           # Standard release build with tests
  $0 --build-type Debug        # Debug build
  $0 --python --clean          # Clean Python bindings build
  $0 --skip-tests              # Build only, no tests

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --cores)
            NUM_CORES="$2"
            shift 2
            ;;
        --python)
            PYTHON_BUILD=true
            shift
            ;;
        --skip-tests)
            SKIP_TESTS=true
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
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

# Function to print colored messages
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check for required tools
check_dependencies() {
    local missing_deps=""
    
    if ! command -v cmake &> /dev/null; then
        missing_deps="$missing_deps CMake "
    fi
    
    if ! command -v make &> /dev/null && ! command -v ninja &> /dev/null; then
        missing_deps="$missing_deps make/ninja "
    fi
    
    if [ "$PYTHON_BUILD" = true ] && ! command -v python3 &> /dev/null; then
        missing_deps="$missing_deps python3 "
    fi
    
    if [ ! -z "$missing_deps" ]; then
        print_error "Missing required dependencies: $missing_deps"
        echo "Please install these dependencies and try again."
        exit 1
    fi
    
    print_info "All dependencies satisfied"
}

# Clean build directory if requested
prepare_build() {
    if [ "$CLEAN_BUILD" = true ] && [ -d "$BUILD_DIR" ]; then
        print_warning "Removing existing build directory: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || {
        print_error "Cannot cd to build directory: $BUILD_DIR"
        exit 1
    }
}

# Configure CMake build
configure_cmake() {
    local cmake_args="-DCMAKE_BUILD_TYPE=$BUILD_TYPE -GNinja"
    
    # Try to use ninja first, fallback to make
    if ! command -v ninja &> /dev/null; then
        cmake_args="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
        print_warning "Ninja not found, using make"
    fi
    
    print_info "Configuring CMake build in: $BUILD_DIR"
    print_info "Build type: $BUILD_TYPE"
    print_info "Build tool: $(command -v ninja 2>/dev/null || echo make)"
    
    # Run CMake configuration
    if ! cmake $cmake_args ..; then
        print_error "CMake configuration failed"
        exit 1
    fi
}

# Build the project
build_project() {
    print_info "Building project with $NUM_CORES core(s)"
    
    if command -v ninja &> /dev/null; then
        if ! ninja -j$NUM_CORES; then
            print_error "Build failed"
            exit 1
        fi
    else
        if ! make -j$NUM_CORES; then
            print_error "Build failed"
            exit 1
        fi
    fi
    
    print_info "Build completed successfully"
}

# Build Python bindings
build_python_bindings() {
    if [ "$PYTHON_BUILD" = false ]; then
        return 0
    fi
    
    print_info "Building Python bindings..."
    
    # Check if pyproject.toml exists
    if [ ! -f "../pyproject.toml" ]; then
        print_warning "pyproject.toml not found, skipping Python bindings build"
        return 0
    fi
    
    # Build Python bindings using scikit-build-core
    if ! pip3 install -e . --no-build-isolation 2>&1 | tail -20; then
        print_error "Python bindings build failed"
        exit 1
    fi
    
    print_info "Python bindings built successfully"
}

# Run tests
run_tests() {
    if [ "$SKIP_TESTS" = true ]; then
        print_warning "Skipping tests (--skip-tests flag)"
        return 0
    fi
    
    print_info "Running tests..."
    
    if command -v ctest &> /dev/null; then
        if ! ctest --output-on-failure; then
            print_error "Tests failed"
            exit 1
        fi
    else
        print_warning "ctest not found, skipping tests"
    fi
    
    print_info "All tests passed"
}

# Create installation directory and install
install_project() {
    print_info "Installing project..."
    
    # Create installation directory structure
    mkdir -p ../install/bin
    mkdir -p ../install/lib
    mkdir -p ../install/include
    
    # Install binaries
    if [ -f "nlm" ]; then
        cp nlm ../install/bin/
    fi
    
    if [ -f "nlm_phase3_demo" ]; then
        cp nlm_phase3_demo ../install/bin/
    fi
    
    if [ -f "nlm_phase4_demo" ]; then
        cp nlm_phase4_demo ../install/bin/
    fi
    
    if [ -f "nlm_phase6_demo" ]; then
        cp nlm_phase6_demo ../install/bin/
    fi
    
    # Install libraries
    if [ -f "libnlm_core.so" ]; then
        cp libnlm_core.so ../install/lib/
    fi
    
    if [ -f "libnlm_agent.so" ]; then
        cp libnlm_agent.so ../install/lib/
    fi
    
    if [ -f "libnlm_world.so" ]; then
        cp libnlm_world.so ../install/lib/
    fi
    
    # Install headers
    if [ -f "../src/brain/Brain.hpp" ]; then
        cp ../src/brain/Brain.hpp ../install/include/
    fi
    
    print_info "Installation completed in: ../install"
}

# Main build function
main() {
    print_info "Starting NLM build process"
    print_info "Build directory: $BUILD_DIR"
    print_info "Build type: $BUILD_TYPE"
    
    # Check dependencies
    check_dependencies
    
    # Prepare build directory
    prepare_build
    
    # Configure and build
    configure_cmake
    build_project
    
    # Build Python bindings if requested
    build_python_bindings
    
    # Run tests if requested
    run_tests
    
    # Install project
    install_project
    
    print_info "=== Build completed successfully ==="
    echo ""
    echo "To run the demos:"
    echo "  cd $BUILD_DIR"
    echo "  ./nlm_phase6_demo     # Phase 6 integration demo"
    echo "  ./nlm_phase3_demo     # Phase 3 world interaction demo"
    echo "  ./nlm_phase4_demo     # Phase 4 cognitive integration demo"
    echo ""
    echo "To use Python bindings:"
    echo "  pip3 install -e ../   # Install Python package"
    echo "  python3 -c 'import pynlm; print(pynlm.__version__)'"
    echo ""
    echo "To view documentation:"
    echo "  cat ../README.md"
    echo "  ls -la ../docs/"
}

# Run main function
main
