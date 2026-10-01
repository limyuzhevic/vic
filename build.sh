#!/bin/bash
# NLM Build Script
# Comprehensive build script for the Neural Learning Machine project

set -e  # Exit on any error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

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

# Check for required dependencies
check_dependencies() {
    log_info "Checking dependencies..."
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        log_error "CMake is required but not installed"
        exit 1
    fi
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        log_error "C++ compiler is required but not installed"
        exit 1
    fi
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        log_error "Python 3 is required but not installed"
        exit 1
    fi
    
    # Check for pip
    if ! command -v pip3 &> /dev/null; then
        log_error "pip3 is required but not installed"
        exit 1
    fi
    
    # Check for pybind11 (will be installed via pip)
    log_info "Dependencies check passed"
}

# Build the C++ core
build_cpp_core() {
    log_info "Building C++ core..."
    
    local build_dir="${PROJECT_ROOT}/build"
    
    # Create build directory
    mkdir -p "${build_dir}"
    cd "${build_dir}"
    
    # Configure with CMake
    log_info "Configuring with CMake..."
    cmake ..
    
    # Build with parallel jobs
    log_info "Building with $(nproc) parallel jobs..."
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        log_info "C++ core build completed successfully"
    else
        log_error "C++ core build failed"
        exit 1
    fi
}

# Build Python bindings
build_python_bindings() {
    log_info "Building Python bindings..."
    
    local python_dir="${PROJECT_ROOT}/python"
    
    # Check if pybind11 is available
    if ! python3 -c "import pybind11" 2>/dev/null; then
        log_info "Installing pybind11..."
        pip3 install pybind11
    fi
    
    # Install scikit-build-core
    log_info "Installing scikit-build-core..."
    pip3 install scikit-build-core
    
    # Build and install the Python module
    log_info "Building and installing Python module..."
    cd "${python_dir}"
    pip3 install -e .
    
    if [ $? -eq 0 ]; then
        log_info "Python bindings build completed successfully"
    else
        log_error "Python bindings build failed"
        exit 1
    fi
}

# Build documentation
generate_documentation() {
    log_info "Generating documentation..."
    
    # Build API documentation
    if [ -f "${PROJECT_ROOT}/docs/ARCHITECTURE.md" ]; then
        log_info "ARCHITECTURE documentation found"
    fi
    
    if [ -f "${PROJECT_ROOT}/README.md" ]; then
        log_info "README documentation found"
    fi
    
    # Generate Python module documentation
    log_info "Generating Python module documentation..."
    python3 -m pydoc pynlm > "${PROJECT_ROOT}/docs/python_api.md" 2>&1 || true
    
    log_info "Documentation generation completed"
}

# Run tests
run_tests() {
    log_info "Running tests..."
    
    local build_dir="${PROJECT_ROOT}/build"
    
    # Run C++ tests
    if [ -f "${build_dir}/nlm_test" ]; then
        log_info "Running C++ tests..."
        cd "${build_dir}"
        ctest --output-on-failure
        
        if [ $? -eq 0 ]; then
            log_info "C++ tests completed successfully"
        else
            log_error "C++ tests failed"
            return 1
        fi
    else
        log_warn "C++ test executable not found"
    fi
    
    # Run Python tests
    if [ -d "${PROJECT_ROOT}/tests" ]; then
        log_info "Running Python tests..."
        cd "${PROJECT_ROOT}"
        python3 -m pytest tests/ -v
        
        if [ $? -eq 0 ]; then
            log_info "Python tests completed successfully"
        else
            log_error "Python tests failed"
            return 1
        fi
    else
        log_warn "Python test directory not found"
    fi
}

# Install the system
install_system() {
    log_info "Installing NLM system..."
    
    # Create installation directory
    local install_dir="${PROJECT_ROOT}/install"
    mkdir -p "${install_dir}"
    
    # Copy executables
    if [ -f "${PROJECT_ROOT}/build/nlm" ]; then
        cp "${PROJECT_ROOT}/build/nlm" "${install_dir}/bin/" 2>/dev/null || true
    fi
    
    if [ -f "${PROJECT_ROOT}/build/nlm_phase3_demo" ]; then
        cp "${PROJECT_ROOT}/build/nlm_phase3_demo" "${install_dir}/bin/" 2>/dev/null || true
    fi
    
    if [ -f "${PROJECT_ROOT}/build/nlm_phase4_demo" ]; then
        cp "${PROJECT_ROOT}/build/nlm_phase4_demo" "${install_dir}/bin/" 2>/dev/null || true
    fi
    
    # Copy Python module
    if [ -d "${PROJECT_ROOT}/python/pynlm" ]; then
        cp -r "${PROJECT_ROOT}/python/pynlm" "${install_dir}/lib/python/" 2>/dev/null || true
    fi
    
    # Create scripts directory
    mkdir -p "${install_dir}/scripts"
    
    log_info "NLM system installed to ${install_dir}"
}

# Clean build artifacts
clean_build() {
    log_info "Cleaning build artifacts..."
    
    local build_dir="${PROJECT_ROOT}/build"
    local install_dir="${PROJECT_ROOT}/install"
    
    # Remove build directory
    if [ -d "${build_dir}" ]; then
        rm -rf "${build_dir}"
        log_info "Removed build directory"
    fi
    
    # Remove install directory
    if [ -d "${install_dir}" ]; then
        rm -rf "${install_dir}"
        log_info "Removed install directory"
    fi
    
    # Remove Python cache
    find "${PROJECT_ROOT}" -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
    find "${PROJECT_ROOT}" -name "*.pyc" -delete 2>/dev/null || true
    
    log_info "Build artifacts cleaned"
}

# Show help
show_help() {
    cat << EOF
NLM Build Script
================

Usage: $0 [COMMAND]

Commands:
  build          Build the entire project (C++ core + Python bindings)
  cpp            Build only the C++ core
  python         Build only Python bindings
  test           Run tests
  doc            Generate documentation
  install        Install the system
  clean          Clean build artifacts
  help           Show this help message

Examples:
  $0 build       # Build everything
  $0 test        # Run tests
  $0 clean       # Clean build artifacts
EOF
}

# Main execution
main() {
    case "${1}" in
        build)
            check_dependencies
            build_cpp_core
            build_python_bindings
            generate_documentation
            install_system
            log_info "Build completed successfully!"
            ;;
        
        cpp)
            check_dependencies
            build_cpp_core
            log_info "C++ build completed successfully!"
            ;;
        
        python)
            check_dependencies
            build_python_bindings
            log_info "Python bindings build completed successfully!"
            ;;
        
        test)
            run_tests
            ;;
        
        doc)
            generate_documentation
            log_info "Documentation generation completed!"
            ;;
        
        install)
            install_system
            log_info "Installation completed!"
            ;;
        
        clean)
            clean_build
            log_info "Cleanup completed!"
            ;;
        
        help|--help|-h)
            show_help
            ;;
        
        *)
            log_error "Unknown command: $1"
            echo ""
            show_help
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"
