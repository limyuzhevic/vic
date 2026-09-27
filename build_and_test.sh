#!/usr/bin/env bash
"""
Enhanced NLM Python Bindings Build and Test Script
===================================================

This script provides comprehensive build, test, and verification functionality
for the enhanced NLM Python bindings.

Usage:
    ./build_and_test.sh [command]

Commands:
    build        Build the Python bindings
    test         Run the test suite
    examples     Run example scripts
    clean        Clean build artifacts
    all         Build, test, and run examples (default)
"""

set -e

# Configuration
PYTHON_VERSION="3.8"
BUILD_TYPE="Release"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
PYTHON_PACKAGE_DIR="${PROJECT_ROOT}/python"
EXAMPLES_DIR="${PROJECT_ROOT}/examples"
TEST_DIR="${PROJECT_ROOT}/tests"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

log_debug() {
    echo -e "${BLUE}[DEBUG]${NC} $1"
}

# Make script executable
chmod 755 "$0"

# Check if we're in the right directory
if [ ! -f "${PROJECT_ROOT}/pyproject.toml" ]; then
    log_error "pyproject.toml not found. Please run from project root."
    exit 1
fi

# Check Python version
check_python_version() {
    local python_version=$(python3 --version 2>&1 | grep -o '[0-9]\+\.[0-9]\+')
    log_info "Python version: $python_version"
    
    # Check if version is sufficient
    if [ "$(echo "$python_version >= 3.8" | bc)" -ne 1 ]; then
        log_warn "Python 3.8+ recommended, found $python_version"
    fi
}

# Check for required tools
check_tools() {
    local tools_missing=()
    
    if ! command -v cmake &> /dev/null; then
        tools_missing+=("CMake")
    fi
    
    if ! command -v python3 &> /dev/null; then
        tools_missing+=("Python 3")
    fi
    
    if ! command -v pip3 &> /dev/null; then
        tools_missing+=("pip3")
    fi
    
    if [ ${#tools_missing[@]} -gt 0 ]; then
        log_error "Missing required tools: ${tools_missing[*]}"
        log_info "Please install missing tools and try again."
        exit 1
    fi
}

# Build the Python bindings
build_bindings() {
    log_info "Building enhanced NLM Python bindings..."
    
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Configure with CMake
    log_info "Configuring build with CMake..."
    cmake -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
          -DPYTHON_EXECUTABLE="$(which python3)" \
          "${PROJECT_ROOT}"
    
    # Build
    log_info "Building bindings..."
    make -j$(nproc)
    
    # Install Python package
    log_info "Installing Python package..."
    pip3 install -e "${PROJECT_ROOT}"
    
    cd "${PROJECT_ROOT}"
    log_info "Build completed successfully!"
}

# Run tests
run_tests() {
    log_info "Running test suite..."
    
    # Check if tests directory exists
    if [ ! -d "${TEST_DIR}" ]; then
        log_warn "Tests directory not found at ${TEST_DIR}"
        log_info "Looking for test files in python directory..."
        
        if [ ! -f "${PYTHON_PACKAGE_DIR}/test_*.py" ]; then
            log_warn "No test files found. Skipping tests."
            return 0
        fi
        
        # Run pytest if available
        if command -v pytest &> /dev/null; then
            log_info "Running pytest..."
            pytest "${PYTHON_PACKAGE_DIR}/test_*.py" -v
        else
            log_warn "pytest not found. Install with: pip3 install pytest"
        fi
        return
    fi
    
    # Run C++ tests if available
    if [ -f "${BUILD_DIR}/tests" ]; then
        log_info "Running C++ tests..."
        "${BUILD_DIR}/tests"
    fi
    
    # Run Python tests
    if command -v pytest &> /dev/null; then
        log_info "Running Python tests..."
        pytest "${TEST_DIR}" -v
    else
        log_warn "pytest not found. Some tests may be skipped."
    fi
}

# Run examples
run_examples() {
    log_info "Running example scripts..."
    
    # Check if examples directory exists
    if [ ! -d "${EXAMPLES_DIR}" ]; then
        log_error "Examples directory not found at ${EXAMPLES_DIR}"
        exit 1
    fi
    
    # Check for example scripts
    local example_files=("${EXAMPLES_DIR}/basic_usage.py" "${EXAMPLES_DIR}/advanced_usage.py" "${EXAMPLES_DIR}/comprehensive_example.py")
    local success_count=0
    local total_count=0
    
    for example in "${example_files[@]}"; do
        if [ -f "$example" ]; then
            ((total_count++))
            log_info "Running $example..."
            
            if python3 "$example" > "${example%.py}.output" 2>&1; then
                log_info "✓ $example completed successfully"
                ((success_count++))
            else
                log_error "✗ $example failed"
                log_debug "Output: $(cat "${example%.py}.output" | tail -20)"
            fi
        else
            log_warn "$example not found"
        fi
    done
    
    log_info "Examples completed: ${success_count}/${total_count} successful"
    
    if [ $success_count -lt $total_count ]; then
        log_warn "Some examples failed. Check output files for details."
        return 1
    fi
}

# Clean build artifacts
clean_build() {
    log_info "Cleaning build artifacts..."
    
    rm -rf "${BUILD_DIR}"
    rm -rf "${PROJECT_ROOT}/dist"
    rm -rf "${PROJECT_ROOT}/python/*.so"
    rm -rf "${PROJECT_ROOT}/python/*.pyd"
    rm -rf "${PROJECT_ROOT}/python/*.cpp"
    
    # Remove example output files
    find "${EXAMPLES_DIR}" -name "*.output" -delete
    
    log_info "Clean completed"
}

# Verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    # Test Python import
    if python3 -c "import pynlm; print(f'pynlm version: {pynlm.__version__}')" 2>&1; then
        log_info "✓ Python import successful"
    else
        log_error "✗ Python import failed"
        return 1
    fi
    
    # Test basic functionality
    if python3 -c "
import pynlm as nlm
config = nlm.Config.create_default()
config.set_with_validation('brain.neuronCount', 100)
brain = nlm.Brain(config)
brain.initialize()
print('✓ Basic functionality test passed')
" 2>&1; then
        log_info "✓ Basic functionality test passed"
    else
        log_error "✗ Basic functionality test failed"
        return 1
    fi
    
    log_info "Installation verification completed successfully"
}

# Main function
main() {
    local command="${1:-all}"
    
    check_python_version
    check_tools
    
    case "${command}" in
        "build")
            build_bindings
            verify_installation
            ;;
        "test")
            build_bindings
            run_tests
            ;;
        "examples")
            build_bindings
            run_examples
            ;;
        "clean")
            clean_build
            ;;
        "all"|"")
            build_bindings
            run_tests
            run_examples
            ;;
        *)
            log_error "Unknown command: ${command}"
            echo "Available commands: build, test, examples, clean, all"
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"
