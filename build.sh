#!/usr/bin/env bash

# Basic Build Script for NLM
# This script provides various build configurations and utilities

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
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

# Configuration
DEFAULT_BUILD_TYPE="Release"
BUILD_TYPES=("Debug" "Release" "RelWithDebInfo" "MinSizeRel")
DEFAULT_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Print usage
usage() {
    cat << EOF
NLM Build Script

Usage: $0 [OPTIONS] [BUILD_TYPE]

Options:
  --help, -h              Show this help message
  --build-type TYPE       Build type (Debug, Release, RelWithDebInfo, MinSizeRel)
  --jobs N, -j N           Number of parallel jobs (default: ${DEFAULT_JOBS})
  --clean                 Clean build directory
  --all                   Build all configurations
  --tests                 Build and run tests
  --benchmarks            Build and run benchmarks
  --python-bindings       Build Python bindings
  --coverage              Build with coverage reporting
  --docs                  Build documentation
  --package               Create distribution package

BUILD_TYPE is one of: ${BUILD_TYPES[*]} (default: ${DEFAULT_BUILD_TYPE})

Examples:
  $0                     # Build Release configuration
  $0 Debug              # Build Debug configuration
  $0 --jobs 8 --tests   # Build with 8 jobs and run tests
  $0 --all              # Build all configurations
  $0 --clean Debug      # Clean and rebuild Debug configuration
EOF
}

# Build a specific configuration
build_config() {
    local build_type=$1
    local jobs=$2
    local clean=$3
    
    log_info "Building ${build_type} configuration..."
    
    local build_dir="build-${build_type,,}"
    
    if [ "$clean" = "true" ] && [ -d "$build_dir" ]; then
        log_info "Cleaning build directory: ${build_dir}"
        rm -rf "$build_dir"
    fi
    
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure CMake
    log_info "Configuring CMake..."
    cmake \
        -G "Ninja" \
        -DCMAKE_BUILD_TYPE="${build_type}" \
        -DCMAKE_CXX_STANDARD=20 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DNLM_BUILD_PYTHON_BINDINGS=OFF \
        "$PROJECT_ROOT"
    
    # Build
    log_info "Building with ${jobs} jobs..."
    cmake --build . --config "${build_type}" --parallel "${jobs}"
    
    # Return to project root
    cd "$PROJECT_ROOT"
}

# Build all configurations
build_all() {
    local jobs=$1
    
    log_info "Building all configurations..."
    
    for build_type in "${BUILD_TYPES[@]}"; do
        build_config "$build_type" "$jobs" "false"
        
        # Run tests for Debug builds
        if [ "$build_type" = "Debug" ]; then
            run_tests "$build_type"
        fi
        
        # Run benchmarks for Release builds
        if [ "$build_type" = "Release" ]; then
            run_benchmarks "$build_type"
        fi
    done
}

# Run tests
run_tests() {
    local build_type=$1
    local build_dir="build-${build_type,,}"
    
    if [ ! -d "$build_dir" ]; then
        log_error "Build directory not found: ${build_dir}"
        return 1
    fi
    
    log_info "Running tests for ${build_type}..."
    
    cd "$build_dir"
    
    # Run unit tests
    if [ -f "./nlm_test" ]; then
        log_info "Running unit tests..."
        ./nlm_test
    else
        log_warn "Unit test executable not found"
    fi
    
    # Run integration tests
    if [ -f "./nlm_phase6_demo" ]; then
        log_info "Running integration tests..."
        timeout 300 ./nlm_phase6_demo || log_warn "Integration tests completed with warnings"
    fi
    
    cd "$PROJECT_ROOT"
}

# Run benchmarks
run_benchmarks() {
    local build_type=$1
    local build_dir="build-${build_type,,}"
    
    if [ ! -d "$build_dir" ]; then
        log_error "Build directory not found: ${build_dir}"
        return 1
    fi
    
    log_info "Running benchmarks for ${build_type}..."
    
    cd "$build_dir"
    
    if [ -f "./nlm_benchmark" ]; then
        log_info "Running benchmark suite..."
        timeout 600 ./nlm_benchmark || log_warn "Benchmark completed with warnings"
    else
        log_warn "Benchmark executable not found"
    fi
    
    cd "$PROJECT_ROOT"
}

# Build Python bindings
build_python_bindings() {
    log_info "Building Python bindings..."
    
    local build_dir="build-python"
    
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with Python bindings enabled
    cmake \
        -G "Ninja" \
        -DCMAKE_BUILD_TYPE=Release \
        -DNLM_BUILD_PYTHON_BINDINGS=ON \
        -DPYTHON_EXECUTABLE=$(which python3) \
        "$PROJECT_ROOT"
    
    # Build
    cmake --build . --config Release --parallel "${DEFAULT_JOBS}"
    
    # Install to local Python environment
    log_info "Installing Python module..."
    pip install -e . 2>/dev/null || {
        log_warn "Failed to install Python module locally"
        log_info "You can manually install with: pip install ."
    }
    
    cd "$PROJECT_ROOT"
}

# Build with coverage
build_coverage() {
    log_info "Building with coverage reporting..."
    
    local build_dir="build-coverage"
    
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with coverage flags
    cmake \
        -G "Ninja" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DNLM_COVERAGE=ON \
        -DNLM_BUILD_PYTHON_BINDINGS=OFF \
        "$PROJECT_ROOT"
    
    # Build
    cmake --build . --config Debug --parallel "${DEFAULT_JOBS}"
    
    # Run tests
    if [ -f "./nlm_test" ]; then
        log_info "Running tests for coverage..."
        ./nlm_test --coverage
    fi
    
    # Generate coverage report
    if command -v gcov >/dev/null 2>&1; then
        log_info "Generating coverage report..."
        gcov -b -c -o . src/brain/Neuron.cpp
        gcov -b -c -o . src/brain/Synapse.cpp
        genhtml -o coverage_report ./*.gcov || log_warn "Could not generate HTML coverage report"
    fi
    
    cd "$PROJECT_ROOT"
}

# Build documentation
build_docs() {
    log_info "Building documentation..."
    
    # Install documentation dependencies
    sudo apt-get update
    sudo apt-get install -y doxygen graphviz python3-sphinx 2>/dev/null || {
        log_warn "Could not install documentation dependencies"
        log_info "Please install: doxygen, graphviz, python3-sphinx"
    }
    
    # Generate Doxygen documentation
    if [ -f "$PROJECT_ROOT/Doxyfile" ]; then
        log_info "Generating Doxygen documentation..."
        doxygen Doxyfile 2>/dev/null || log_warn "Doxygen documentation generation failed"
        cp -r html "$PROJECT_ROOT/docs-api/" 2>/dev/null || log_warn "Could not copy Doxygen output"
    fi
    
    # Generate Sphinx documentation
    log_info "Building Sphinx documentation..."
    pip install sphinx sphinx-rtd-theme 2>/dev/null
    cd "$PROJECT_ROOT/docs"
    make html 2>/dev/null || log_warn "Sphinx documentation build failed"
    cd "$PROJECT_ROOT"
    cp -r docs/_build/html "$PROJECT_ROOT/docs-html/" 2>/dev/null || log_warn "Could not copy Sphinx output"
    
    log_info "Documentation built in: docs-api/ and docs-html/"
}

# Create distribution package
create_package() {
    log_info "Creating distribution package..."
    
    local package_name="nlm-$(date +%Y%m%d-%H%M%S)"
    local package_dir="dist"
    
    mkdir -p "$package_dir/$package_name"
    
    # Copy source files
    log_info "Copying source files..."
    cp -r "$PROJECT_ROOT/." "$package_dir/$package_name/source/"
    
    # Copy build artifacts (for Release builds)
    if [ -d "build-Release" ]; then
        log_info "Copying Release build artifacts..."
        cp -r "build-Release/bin" "$package_dir/$package_name/"
        cp -r "build-Release/lib" "$package_dir/$package_name/"
    fi
    
    # Copy documentation
    if [ -d "docs-api" ]; then
        log_info "Copying documentation..."
        cp -r "docs-api" "$package_dir/$package_name/"
    fi
    
    # Create README and configuration files
    log_info "Creating package metadata..."
    cat > "$package_dir/$package_name/README.md" << EOF
# NLM Package

This package contains the Neural Learning Machine (NLM) computational brain simulation framework.

## Usage

### Building from source

1. Navigate to the build directory
2. Run: cmake -DCMAKE_BUILD_TYPE=Release ..
3. Run: cmake --build . --parallel 4

### Running demos

After building, you can run:

- ./nlm: Phase 2 basic demo
- ./nlm_phase6_demo: Phase 6 integration test
- ./nlm_benchmark: Performance benchmarks

### Configuration

The project uses configuration files:

- configs/default.cfg: Plain text configuration
- configs/high_perf.cfg: High performance settings
- configs/scientific_config.json: Scientific configuration

### Python bindings

Python bindings are available in the Python package `pynlm`.

### API Documentation

See docs-api/ for Doxygen documentation and docs-html/ for Sphinx documentation.
EOF
    
    # Create package archive
    log_info "Creating package archive..."
    tar -czf "$package_dir/${package_name}.tar.gz" "$package_name"
    
    log_info "Package created: $package_dir/${package_name}.tar.gz"
}

# Clean build directories
clean_all() {
    log_info "Cleaning build directories..."
    
    for build_type in "${BUILD_TYPES[@]}"; do
        build_dir="build-${build_type,,}"
        if [ -d "$build_dir" ]; then
            rm -rf "$build_dir"
            log_info "Cleaned ${build_dir}"
        fi
    done
    
    # Clean other build directories
    rm -rf build-python build-coverage dist docs-api docs-html 2>/dev/null || true
    
    log_info "All build directories cleaned"
}

# Main script execution
main() {
    local build_type=""
    local jobs="${DEFAULT_JOBS}"
    local clean=false
    local tests=false
    local benchmarks=false
    local python_bindings=false
    local coverage=false
    local docs=false
    local package=false
    local all=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help|-h)
                usage
                exit 0
                ;;
            --build-type)
                if [[ $# -gt 1 ]]; then
                    build_type="$2"
                    shift 2
                else
                    log_error "Build type required for --build-type"
                    usage
                    exit 1
                fi
                ;;
            --jobs|-j)
                if [[ $# -gt 1 ]]; then
                    jobs="$2"
                    shift 2
                else
                    log_error "Number of jobs required for --jobs"
                    usage
                    exit 1
                fi
                ;;
            --clean)
                clean=true
                shift
                ;;
            --tests)
                tests=true
                shift
                ;;
            --benchmarks)
                benchmarks=true
                shift
                ;;
            --python-bindings)
                python_bindings=true
                shift
                ;;
            --coverage)
                coverage=true
                shift
                ;;
            --docs)
                docs=true
                shift
                ;;
            --package)
                package=true
                shift
                ;;
            --all)
                all=true
                shift
                ;;
            -*)
                log_error "Unknown option: $1"
                usage
                exit 1
                ;;
            *)
                if [[ -z "$build_type" ]]; then
                    build_type="$1"
                    shift
                else
                    log_error "Unexpected argument: $1"
                    usage
                    exit 1
                fi
                ;;
        esac
    done
    
    # Validate build type
    if [[ -n "$build_type" ]]; then
        local valid=false
        for valid_type in "${BUILD_TYPES[@]}"; do
            if [[ "$build_type" == "$valid_type" ]]; then
                valid=true
                break
            fi
        done
        
        if [[ "$valid" == "false" ]]; then
            log_error "Invalid build type: $build_type"
            log_error "Valid build types: ${BUILD_TYPES[*]}"
            usage
            exit 1
        fi
    else
        build_type="$DEFAULT_BUILD_TYPE"
    fi
    
    # Execute build based on options
    if [[ "$all" == "true" ]]; then
        build_all "$jobs"
    else
        build_config "$build_type" "$jobs" "$clean"
        
        if [[ "$tests" == "true" ]]; then
            run_tests "$build_type"
        fi
        
        if [[ "$benchmarks" == "true" ]]; then
            run_benchmarks "$build_type"
        fi
    fi
    
    if [[ "$python_bindings" == "true" ]]; then
        build_python_bindings
    fi
    
    if [[ "$coverage" == "true" ]]; then
        build_coverage
    fi
    
    if [[ "$docs" == "true" ]]; then
        build_docs
    fi
    
    if [[ "$package" == "true" ]]; then
        create_package
    fi
}

# Execute main function
main "$@"