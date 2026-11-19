#!/bin/bash

# NLM (Neural Learning Machine) - Unified Build Script
# This script provides a comprehensive build system for the NLM project,
# supporting C++ core, Python bindings, and testing.

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
PYTHON_BINDINGS=ON
RUN_TESTS=ON
INSTALL_DIR=""
VERBOSE=0

# Print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_debug() {
    if [ $VERBOSE -eq 1 ]; then
        echo -e "${BLUE}[DEBUG]${NC} $1"
    fi
}

# Print usage information
usage() {
    cat << EOF
NLM Build Script
===============

USAGE: $0 [OPTIONS]

Build and test the NLM (Neural Learning Machine) project.

OPTIONS:
  --type <type>           Build type: Debug, Release, RelWithDebInfo, MinSizeRel
                          (default: Release)
  --build-dir <dir>       Build directory (default: build)
  --no-python-bindings    Disable Python bindings
  --no-tests              Disable tests
  --install-dir <dir>     Installation directory
  --verbose, -v           Enable verbose output
  --help, -h              Show this help message

EXAMPLES:
  $0                       Build NLM in Release mode
  $0 --type Debug --verbose Build in Debug mode with verbose output
  $0 --no-python-bindings  Build without Python bindings
  $0 --build-dir mybuild    Use custom build directory

COMMON COMMANDS:
  $0 --type Debug          Build for development/debugging
  $0 --install-dir /usr/local Install to system directories
  $0 --no-python-bindings  Build only C++ components
  $0 --no-tests            Build without tests

For more information, visit: https://github.com/nlm-project/nlm
EOF
}

# Parse command line arguments
while [ $# -gt 0 ]; do
    case $1 in
        --type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --no-python-bindings)
            PYTHON_BINDINGS=OFF
            shift
            ;;
        --no-tests)
            RUN_TESTS=OFF
            shift
            ;;
        --install-dir)
            INSTALL_DIR="$2"
            shift 2
            ;;
        --verbose|-v)
            VERBOSE=1
            shift
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

print_info "Starting NLM build process..."
print_info "Build type: $BUILD_TYPE"
print_info "Build directory: $BUILD_DIR"
print_info "Python bindings: $PYTHON_BINDINGS"
print_info "Tests: $RUN_TESTS"

# Create build directory
print_info "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Configure with CMake
print_info "Configuring CMake project..."
cd "$BUILD_DIR"

CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
if [ "$PYTHON_BINDINGS" = "ON" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DNLM_BUILD_PYTHON_BINDINGS=ON"
else
    CMAKE_ARGS="$CMAKE_ARGS -DNLM_BUILD_PYTHON_BINDINGS=OFF"
fi

if [ -n "$INSTALL_DIR" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR"
fi

print_debug "Running CMake with args: $CMAKE_ARGS"
cmake $CMAKE_ARGS ..

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed!"
    exit 1
fi

# Build the project
print_info "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    print_error "Build failed!"
    exit 1
fi

# Run tests if enabled
if [ "$RUN_TESTS" = "ON" ]; then
    print_info "Running tests..."
    make test
    
    if [ $? -ne 0 ]; then
        print_error "Tests failed!"
        exit 1
    fi
fi

# Install if install directory specified
if [ -n "$INSTALL_DIR" ]; then
    print_info "Installing to $INSTALL_DIR..."
    make install
    
    if [ $? -ne 0 ]; then
        print_error "Installation failed!"
        exit 1
    fi
fi

# Print summary
print_info ""
print_info "Build completed successfully!"
print_info ""
print_info "Generated executables:"
cd ..

# Find and list executables (based on CMakeLists.txt targets)
EXECUTABLES=(
    "nlm:NLM Phase 2 demo (main neural computation)"
    "nlm_test:Unit test suite"
    "nlm_benchmark:Performance benchmark"
    "nlm_phase3_demo:NLM Phase 3 demo (world interaction)"
    "nlm_phase4_demo:NLM Phase 4 demo (cognitive functions)"
    "nlm_phase5_demo:NLM Phase 5 demo (performance optimizations)"
    "nlm_phase6_demo:NLM Phase 6 demo (full integration)"
    "nlm_phase6_text_demo:NLM Phase 6 text demo (text-based sense)"
)

for exe in "${EXECUTABLES[@]}"; do
    exe_name=$(echo $exe | cut -d: -f1)
    exe_desc=$(echo $exe | cut -d: -f2-)
    if [ -f "build/$exe_name" ]; then
        print_info "  - build/$exe_name ($exe_desc)"
    fi
done

if [ "$PYTHON_BINDINGS" = "ON" ]; then
    # Find Python bindings
    PYTHON_LIBS=$(find . -name "*pynlm*.so" -o -name "*pynlm*.pyd" 2>/dev/null | head -5)
    if [ -n "$PYTHON_LIBS" ]; then
        print_info "  - Python bindings:\n$(echo "$PYTHON_LIBS" | sed 's/^/    /')"
    fi
fi

print_info ""
print_info "To run the demo: ./build/nlm"
print_info "To run tests: ./build/nlm_test"
print_info "To run benchmark: ./build/nlm_benchmark"
print_info "To run Phase 3 demo: ./build/nlm_phase3_demo"
print_info "To run Phase 4 demo: ./build/nlm_phase4_demo"
print_info "To run Phase 5 demo: ./build/nlm_phase5_demo"
print_info "To run Phase 6 demo: ./build/nlm_phase6_demo"