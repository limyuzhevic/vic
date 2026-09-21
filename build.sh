#!/bin/bash
# NLM Build and Test Script
# Easy build script for NLM (Neural Learning Machine)

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default configuration
BUILD_TYPE="Release"
INSTALL_PREFIX="/usr/local"
TEST_MODE=false
CLEAN_BUILD=false
JOBS=$(nproc)
PYTHON_INSTALL=false

# Print usage
print_usage() {
    cat << EOF
NLM Build and Test Script

Usage: $0 [OPTIONS]

Options:
  --type TYPE             Build type: Debug or Release (default: Release)
  --prefix PREFIX         Install prefix (default: /usr/local)
  --jobs N                Number of parallel jobs (default: $(nproc))
  --test                  Run tests after build
  --clean                 Clean build directories
  --python-install        Install Python bindings
  --help                  Show this help message

Examples:
  $0                     # Build in Release mode
  $0 --type Debug        # Build in Debug mode
  $0 --test              # Build and run tests
  $0 --clean --test      # Clean build and test

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --jobs)
            JOBS="$2"
            shift 2
            ;;
        --test)
            TEST_MODE=true
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --python-install)
            PYTHON_INSTALL=true
            shift
            ;;
        --help)
            print_usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_header() {
    echo -e "${BLUE}[BUILD]${NC} $1"
}

# Check if we are in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    log_error "CMakeLists.txt not found. Please run this script from the NLM root directory."
    exit 1
fi

# Clean build if requested
if [ "$CLEAN_BUILD" = true ]; then
    log_header "Performing clean build..."
    if [ -d "build" ]; then
        rm -rf build
    fi
fi

# Create build directory
mkdir -p build
cd build

# Configure CMake
log_header "Configuring CMake (${BUILD_TYPE} build)..."
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}

# Build the project
log_header "Building NLM (${JOBS} parallel jobs)..."
make -j${JOBS}

# Check if build succeeded
if [ $? -ne 0 ]; then
    log_error "Build failed!"
    exit 1
fi

log_info "Build successful!"

# Install Python bindings if requested
if [ "$PYTHON_INSTALL" = true ]; then
    log_header "Installing Python bindings..."
    cd ..
    pip install -e .
    if [ $? -eq 0 ]; then
        log_info "Python bindings installed successfully!"
    else
        log_error "Failed to install Python bindings!"
        exit 1
    fi
    cd build
fi

# Run tests if requested
if [ "$TEST_MODE" = true ]; then
    log_header "Running tests..."
    if command -v ctest &> /dev/null; then
        ctest --output-on-failure
        if [ $? -eq 0 ]; then
            log_info "All tests passed!"
        else
            log_error "Tests failed!"
            exit 1
        fi
    else
        log_warn "CTest not found. Skipping tests."
    fi
fi

# Display build summary
log_header "Build Summary"
echo "Build type: ${BUILD_TYPE}"
echo "Install prefix: ${INSTALL_PREFIX}"
echo "Parallel jobs: ${JOBS}"
echo "Python bindings: $([ "$PYTHON_INSTALL" = true ] && echo "Yes" || echo "No")"
echo ""
echo "Build outputs:"
if [ -f "nlm" ]; then
    echo "  - Main executable: ./build/nlm"
elif [ -f "bin/nlm" ]; then
    echo "  - Main executable: ./bin/nlm"
fi

if [ -f "../pynlm/__init__.py" ]; then
    echo "  - Python bindings: $(python3 -c 'import pynlm; print(pynlm.__version__)')"
fi

echo ""
log_info "You can run the main demo with:"
echo "  ./build/nlm --help"

# Return to original directory
cd ..
