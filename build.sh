#!/usr/bin/env bash
set -e

# NLM Python Package Build Script
# This script provides common build and development commands for the NLM Python package

# NLM Python Package Build Script
# This script provides common build and development commands for the NLM Python package

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
VERBOSE=0
SKIP_TESTS=0
PYTHON_VERSION=""

cn() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

show_help() {
    cat << EOF
NLM Python Package Build Script

Usage: $0 [OPTIONS]

Options:
    --build-type TYPE        Build type (Release, Debug, RelWithDebInfo). Default: Release
    --verbose                Enable verbose output
    --skip-tests             Skip running tests after build
    --python-version VERSION Specify Python version
    --help                   Show this help message

Examples:
    $0                    # Build in Release mode with tests
    $0 --build-type Debug --verbose
    $0 --skip-tests

Available commands:
    build                    Build the Python package
    test                     Run tests
    install                  Install the package
    clean                    Clean build artifacts
    help                     Show this help message

EOF
}

build_package() {
    cn "Building NLM Python package..."
    cn "Build type: $BUILD_TYPE"
    
    # Check if we have the required dependencies
    if ! command -v python3 &> /dev/null; then
        error "Python3 is required but not installed."
        exit 1
    fi
    
    # Check Python version
    local python_cmd="python3"
    if [ -n "$PYTHON_VERSION" ]; then
        python_cmd="python$PYTHON_VERSION"
        if ! command -v $python_cmd &> /dev/null; then
            error "Python $PYTHON_VERSION is required but not installed."
            exit 1
        fi
    fi
    
    # Check if scikit-build-core is available
    if ! $python_cmd -c "import scikit_build_core" 2>/dev/null; then
        warn "scikit-build-core not found. Installing..."
        $python_cmd -m pip install scikit-build-core>=0.5.0
    fi
    
    # Check if pybind11 is available
    if ! $python_cmd -c "import pybind11" 2>/dev/null; then
        warn "pybind11 not found. Installing..."
        $python_cmd -m pip install pybind11>=2.11.0
    fi
    
    # Build the package
    cn "Building with $python_cmd"
    
    if [ $VERBOSE -eq 1 ]; then
        $python_cmd -m build --verbose
    else
        $python_cmd -m build
    fi
    
    if [ $? -eq 0 ]; then
        cn "Build successful!"
        # List the built distributions
        find dist -name "*.whl" -o -name "*.tar.gz" | head -10 | while read dist_file; do
            echo "  Built: $dist_file"
        done
    else
        error "Build failed!"
        exit 1
    fi
}

test_package() {
    cn "Testing NLM Python package..."
    
    # Find the most recent built wheel
    local wheel=$(find dist -name "*.whl" -type f | sort -r | head -1)
    
    if [ -z "$wheel" ]; then
        error "No wheel found. Build the package first with './build.sh build'."
        exit 1
    fi
    
    cn "Testing with wheel: $wheel"
    
    # Install the wheel
    cn "Installing wheel..."
    pip install --force-reinstall "$wheel"
    
    # Run tests
    cn "Running tests..."
    if [ $VERBOSE -eq 1 ]; then
        python -m pytest tests/ -v --tb=short
    else
        python -m pytest tests/ -v
    fi
    
    if [ $? -eq 0 ]; then
        cn "Tests passed!"
    else
        error "Tests failed!"
        exit 1
    fi
}

install_package() {
    cn "Installing NLM Python package..."
    
    # Find the most recent built wheel
    local wheel=$(find dist -name "*.whl" -type f | sort -r | head -1)
    
    if [ -z "$wheel" ]; then
        error "No wheel found. Build the package first with './build.sh build'."
        exit 1
    fi
    
    # Install the wheel
    pip install --force-reinstall "$wheel"
    
    cn "Package installed successfully!"
    
    # Test the installation
    cn "Testing installation..."
    python -c "import pynlm; print(f'NLM Version: {pynlm.__version__}')"
}

clean_build() {
    cn "Cleaning build artifacts..."
    
    # Clean Python artifacts
    find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
    find . -name "*.pyc" -delete
    find . -name "*.pyo" -delete
    find . -name ".pytest_cache" -type d -exec rm -rf {} + 2>/dev/null || true
    
    # Clean build directory
    if [ -d "build" ]; then
        rm -rf build
        cn "Cleaned build directory"
    fi
    
    # Clean dist directory
    if [ -d "dist" ]; then
        rm -rf dist
        cn "Cleaned dist directory"
    fi
    
    # Clean .egg-info directories
    find . -name "*.egg-info" -type d -exec rm -rf {} + 2>/dev/null || true
    
    # Clean CMake cache and build files
    if [ -d "cmake-build" ]; then
        rm -rf cmake-build
        cn "Cleaned cmake-build directory"
    fi
    
    # Clean any other build artifacts
    find . -name "CMakeCache.txt" -delete
    find . -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
    find . -name "*.cmake" -delete
    
    cn "Cleanup complete!"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=1
            shift
            ;;
        --skip-tests)
            SKIP_TESTS=1
            shift
            ;;
        --python-version)
            PYTHON_VERSION="$2"
            shift 2
            ;;
        --help)
            show_help
            exit 0
            ;;
        build)
            build_package
            shift
            ;;
        test)
            test_package
            shift
            ;;
        install)
            install_package
            shift
            ;;
        clean)
            clean_build
            shift
            ;;
        help)
            show_help
            shift
            ;;
        *)
            error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# If no command specified, show help
show_help