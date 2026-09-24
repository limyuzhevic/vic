#!/bin/bash

# build.sh - NLM Build Script
# Comprehensive build automation for NLM project

set -euo pipefail

# Default configuration
BUILD_TYPE="Release"
PARALLEL_JOBS=$(nproc)
INSTALL_PREFIX=""
ENABLE_FEATURES=""
RUN_TESTS=true
BUILD_PYTHON_BINDINGS=true
BUILD_DOCS=false
GENERATE_COVERAGE=false
TIMEOUT_SECONDS=3600

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Print usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build NLM project with comprehensive build options.

Options:
  --debug                    Build in Debug mode (default: Release)
  --install-prefix DIR       Install to specified prefix
  --features FEATURE-LIST    Enable features (all,mpi,cuda,simd,validation)
  --jobs N                  Number of parallel jobs (default: cpu cores)
  --no-tests                 Skip running tests
  --no-python                Skip building Python bindings
  --docs                     Generate documentation
  --coverage                 Generate code coverage report
  --timeout SECONDS          Build timeout in seconds (default: 3600)
  --clean-build              Clean build directory before building
  --list-targets             List available build targets
  -h, --help                 Show this help message

Examples:
  $0                                    # Default release build with tests
  $0 --debug --features all              # Debug build with all features
  $0 --install-prefix /usr/local          # Install to system location
  $0 --jobs 8                            # Build with 8 parallel jobs
  $0 --coverage --docs                    # Build with coverage and docs
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --install-prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --features)
            ENABLE_FEATURES="$2"
            shift 2
            ;;
        --jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --no-tests)
            RUN_TESTS=false
            shift
            ;;
        --no-python)
            BUILD_PYTHON_BINDINGS=false
            shift
            ;;
        --docs)
            BUILD_DOCS=true
            shift
            ;;
        --coverage)
            GENERATE_COVERAGE=true
            shift
            ;;
        --timeout)
            TIMEOUT_SECONDS="$2"
            shift 2
            ;;
        --clean-build)
            if [[ -d "build" ]]; then
                log_info "Cleaning build directory..."
                rm -rf build
            fi
            shift
            ;;
        --list-targets)
            list_targets
            exit 0
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# List build targets
list_targets() {
    log_info "Available build targets:"
    echo "  nlm              - Main NLM executable"
    echo "  nlm_core         - Core library"
    echo "  nlm_agent        - Agent system library"
    echo "  nlm_world        - World system library"
    echo "  nlm_phase3       - Phase 3 experiments"
    echo "  nlm_phase4       - Phase 4 experiments"
    echo "  nlm_phase3_demo  - Phase 3 demo executable"
    echo "  nlm_phase4_demo  - Phase 4 demo executable"
    echo "  benchmark        - Benchmark executable"
    echo "  performance_tests - Performance test executable"
    echo ""
    echo "Available CMake options:"
    echo "  -DENABLE_MPI=ON/OFF           - Enable MPI support"
    echo "  -DCUDA_ENABLED=ON/OFF         - Enable CUDA support"
    echo "  -DENABLE_SIMD=ON/OFF           - Enable SIMD optimizations"
    echo "  -DBUILD_SHARED_LIBS=ON/OFF     - Build shared libraries"
    echo "  -DBUILD_TESTS=ON/OFF           - Build test executables"
    echo "  -DBUILD_EXAMPLES=ON/OFF        - Build example programs"
    echo "  -DNLM_PYTHON_BINDINGS=ON/OFF   - Build Python bindings"
    echo "  -DENABLE_VALIDATION=ON/OFF     - Enable runtime validation"
    echo "  -DCMAKE_BUILD_TYPE=TYPE       - Build type (Debug/Release/RelWithDebInfo/MinSizeRel)"
}

# Check system dependencies
check_dependencies() {
    log_info "Checking system dependencies..."
    
    # Check CMake
    if ! command -v cmake &> /dev/null; then
        log_error "CMake not found. Please install CMake (version 3.16 or higher)."
        exit 1
    fi
    
    local cmake_version=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || echo "0.0.0")
    local required_version="3.16"
    
    if [[ $(echo "$cmake_version >= $required_version" | bc -l) -ne 1 ]]; then
        log_error "CMake version $cmake_version is too old. Required version: $required_version"
        exit 1
    fi
    
    log_success "CMake $cmake_version found"
    
    # Check C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        log_error "No C++ compiler found. Please install g++ or clang++."
        exit 1
    fi
    
    # Check Python
    if ! command -v python3 &> /dev/null; then
        log_error "Python3 not found. Please install Python3 (version 3.8 or higher)."
        exit 1
    fi
    
    local python_version=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}')")
    local required_python_version="3.8"
    
    if [[ $(echo "$python_version >= $required_python_version" | bc -l) -ne 1 ]]; then
        log_error "Python $python_version is too old. Required version: $required_python_version"
        exit 1
    fi
    
    log_success "Python $python_version found"
    
    # Check pip
    if ! command -v pip3 &> /dev/null && ! command -v pip &> /dev/null; then
        log_error "pip not found. Please install pip."
        exit 1
    fi
}

# Setup Python environment
setup_python() {
    log_info "Setting up Python environment..."
    
    # Check if Python virtual environment exists
    if [[ ! -d "venv" ]]; then
        log_info "Creating Python virtual environment..."
        python3 -m venv venv
    fi
    
    # Activate virtual environment
    source venv/bin/activate
    
    # Upgrade pip and setuptools
    log_info "Upgrading pip and setuptools..."
    pip install --no-cache-dir --upgrade pip setuptools wheel
    
    # Install scikit-build-core and pybind11
    log_info "Installing scikit-build-core and pybind11..."
    pip install --no-cache-dir scikit-build-core>=0.5.0 pybind11>=2.11.0
    
    # Install optional development dependencies if requested
    if [[ "$BUILD_PYTHON_BINDINGS" == true ]]; then
        log_info "Building Python bindings..."
        pip install --no-cache-dir -e .[dev]
    fi
}

# Configure CMake
cmake_configure() {
    log_info "Configuring CMake build..."
    
    local cmake_args="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON"
    
    # Add install prefix if specified
    if [[ -n "$INSTALL_PREFIX" ]]; then
        cmake_args="${cmake_args} -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}"
    fi
n    
    # Add feature flags
    if [[ -n "$ENABLE_FEATURES" ]]; then
        case $ENABLE_FEATURES in
            "all")
                cmake_args="${cmake_args} -DENABLE_MPI=ON -DCUDA_ENABLED=ON -DENABLE_SIMD=ON -DENABLE_VALIDATION=ON"
                ;;
            "mpi")
                cmake_args="${cmake_args} -DENABLE_MPI=ON"
                ;;
            "cuda")
                cmake_args="${cmake_args} -DCUDA_ENABLED=ON"
                ;;
            "simd")
                cmake_args="${cmake_args} -DENABLE_SIMD=ON"
                ;;
            "validation")
                cmake_args="${cmake_args} -DENABLE_VALIDATION=ON"
                ;;
        esac
    fi
n    
    # Add Python bindings flag
    if [[ "$BUILD_PYTHON_BINDINGS" == false ]]; then
        cmake_args="${cmake_args} -DNLM_PYTHON_BINDINGS=OFF"
    fi
n    
    # Export compile commands for tooling
    cmake_args="${cmake_args} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    
    log_info "Running CMake with arguments: ${cmake_args}"
    
    # Configure with timeout
    timeout ${TIMEOUT_SECONDS} cmake -B build -S . ${cmake_args} || {
        log_error "CMake configuration failed or timed out after ${TIMEOUT_SECONDS} seconds"
        exit 1
    }
}

# Build project
build_project() {
    log_info "Building project with ${PARALLEL_JOBS} parallel jobs..."
    
    # Build with timeout
    timeout ${TIMEOUT_SECONDS} cmake --build build --parallel ${PARALLEL_JOBS} --config ${BUILD_TYPE} || {
        log_error "Build failed or timed out after ${TIMEOUT_SECONDS} seconds"
        exit 1
    }
    
    log_success "Build completed successfully"
}

# Run tests
run_tests() {
    if [[ "$RUN_TESTS" == false ]]; then
        log_warning "Test execution disabled by command line option"
        return 0
    fi
    
    log_info "Running tests..."
    
    # Run C++ tests
    if [[ -f "build/ctest" ]] || [[ -f "build/CTestTestfile.cmake" ]]; then
        log_info "Running C++ tests..."
        cd build
        if ctest --output-on-failure --timeout 300; then
            log_success "C++ tests passed"
        else
            log_error "C++ tests failed"
            exit 1
        fi
        cd ..
    else
        log_warning "No C++ tests found"
    fi
    
    # Run Python tests if Python bindings were built
    if [[ "$BUILD_PYTHON_BINDINGS" == true ]]; then
        log_info "Running Python tests..."
        source venv/bin/activate
        
        if pytest tests/ 
            --cov=pynlm 
            --cov-report=xml 
            --cov-report=html 
            --cov-report=term-missing 
            --cov-config=.coveragerc 
            --timeout 300; then
            log_success "Python tests passed"
        else
            log_error "Python tests failed"
            exit 1
        fi
    else
        log_warning "Python bindings not built, skipping Python tests"
    fi
    
    # Generate coverage report if requested
    if [[ "$GENERATE_COVERAGE" == true ]]; then
        log_info "Generating coverage report..."
        source venv/bin/activate
        coverage report --fail-under=80
        coverage html
        coverage xml
        log_success "Coverage report generated"
    fi
}

# Build documentation
build_documentation() {
    if [[ "$BUILD_DOCS" == false ]]; then
        return 0
    fi
    
    log_info "Building documentation..."
    
    source venv/bin/activate
    pip install --no-cache-dir sphinx sphinx-rtd-theme breathe myst-parser
    
    # Build API documentation
    if [[ -d "docs" ]]; then
        log_info "Building Sphinx documentation..."
        cd docs
        sphinx-build -b html . _build/html
        sphinx-build -b latex . _build/latex
        sphinx-build -b man . _build/man
        cd ..
        log_success "Sphinx documentation built"
    fi
    
    # Build C++ documentation if Doxygen is available
    if command -v doxygen &> /dev/null; then
        log_info "Building Doxygen documentation..."
        if [[ -f "Doxyfile" ]]; then
            doxygen Doxyfile
            log_success "Doxygen documentation built"
        fi
    fi
}

# Run benchmarks
run_benchmarks() {
    log_info "Running benchmarks..."
    
    if [[ -f "build/benchmark" ]]; then
        log_info "Running benchmark executable..."
        ./benchmark --benchmark_out=benchmark_results.json --benchmark_out_format=json
        log_success "Benchmarks completed"
    else
        log_warning "Benchmark executable not found"
    fi
}

# Run performance tests
run_performance_tests() {
    log_info "Running performance tests..."
    
    if [[ -f "build/performance_tests" ]]; then
        log_info "Running performance test executable..."
        ./performance_tests --output performance_metrics.json
        log_success "Performance tests completed"
    else
        log_warning "Performance test executable not found"
    fi
}

# Install project
install_project() {
    if [[ -z "$INSTALL_PREFIX" ]]; then
        log_warning "No install prefix specified, skipping installation"
        return 0
    fi
    
    log_info "Installing project to ${INSTALL_PREFIX}..."
    
    cd build
    cmake --install . --config ${BUILD_TYPE}
    
    # Install Python package
    if [[ "$BUILD_PYTHON_BINDINGS" == true ]]; then
        cd ..
        pip install --prefix "${INSTALL_PREFIX}/python" -e .
    fi
    
    log_success "Installation completed"
}

# Main build function
main() {
    log_info "Starting NLM build process..."
    log_info "Build type: ${BUILD_TYPE}"
    log_info "Parallel jobs: ${PARALLEL_JOBS}"
    log_info "Python bindings: ${BUILD_PYTHON_BINDINGS}"
    log_info "Documentation: ${BUILD_DOCS}"
    
    # Check dependencies
    check_dependencies
    
    # Setup Python environment
    setup_python
    
    # Configure CMake
    cmake_configure
    
    # Build project
    build_project
    
    # Run tests
    run_tests
    
    # Run benchmarks
    run_benchmarks
    
    # Run performance tests
    run_performance_tests
    
    # Build documentation
    build_documentation
    
    # Install if requested
    install_project
    
    log_success "Build process completed successfully!"
    log_info "Build directory: ${PWD}/build"
    log_info "Distribution directory: ${PWD}/dist"
}

# Trap errors and clean up
trap 'log_error "Build failed. See build directory for details."' ERR

# Run main function
main "$@"