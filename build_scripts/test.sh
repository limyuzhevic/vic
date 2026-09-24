#!/bin/bash

# test.sh - NLM Test Script
# Comprehensive testing automation for NLM project

set -euo pipefail

# Default configuration
BUILD_DIR="build"
PYTHON_VENV="venv"
PYTHON_TESTS="tests"
CPP_TESTS=""
COVERAGE_THRESHOLD=80
TIMEOUT_SECONDS=600
VERBOSE=false
PARALLEL_JOBS=$(nproc)

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

Run tests for NLM project with comprehensive test options.

Options:
  --build-dir DIR            Build directory (default: build)
  --python-venv DIR           Python virtual environment directory (default: venv)
  --python-tests DIR          Python tests directory (default: tests)
  --cpp-tests DIR             C++ tests directory (default: auto-detected)
  --coverage-threshold N      Minimum code coverage percentage (default: 80)
  --timeout SECONDS           Test timeout in seconds (default: 600)
  --verbose                   Enable verbose output
  --parallel N               Number of parallel jobs for C++ tests (default: cpu cores)
  --no-cpp                    Skip C++ tests
  --no-python                 Skip Python tests
  --coverage                  Generate code coverage report
  --coverage-html             Generate HTML coverage report
  --coverage-xml              Generate XML coverage report
  --list-tests                List available tests
  -h, --help                  Show this help message

Examples:
  $0                                          # Run all tests with defaults
  $0 --verbose --coverage                      # Run tests with verbose output and coverage
  $0 --build-dir mybuild --python-venv venv     # Use custom directories
  $0 --no-cpp                                  # Skip C++ tests
  $0 --no-python                               # Skip Python tests
  $0 --coverage-threshold 90                    # Require 90% coverage
EOF
}

# List available tests
list_tests() {
    log_info "Available tests:"
    
    # List C++ tests
    if [[ -d "$BUILD_DIR" ]]; then
        log_info "C++ tests:"
        if [[ -f "$BUILD_DIR/ctest" ]]; then
            echo "  Executable: $BUILD_DIR/ctest"
        fi
        
        # Find test executables
        find "$BUILD_DIR" -name "*test*" -type f -executable 2>/dev/null | head -10 | while read -r test; do
            local test_name=$(basename "$test")
            echo "  Executable: $test"
        done
    fi
    
    # List Python tests
    log_info "Python tests:"
    find "$PYTHON_TESTS" -name "test_*.py" -type f | while read -r test; do
        local test_name=$(basename "$test" .py)
        echo "  File: $test"
    done
}

# Check test dependencies
check_test_dependencies() {
    log_info "Checking test dependencies..."
    
    # Check if build directory exists
    if [[ ! -d "$BUILD_DIR" ]]; then
        log_error "Build directory '$BUILD_DIR' not found. Run build first."
        exit 1
    fi
    
    # Check if Python virtual environment exists
    if [[ ! -d "$PYTHON_VENV" ]]; then
        log_warning "Python virtual environment '$PYTHON_VENV' not found. Creating..."
        python3 -m venv "$PYTHON_VENV"
    fi
    
    # Activate virtual environment
    source "$PYTHON_VENV/bin/activate"
    
    # Check if pytest is available
    if ! command -v pytest &> /dev/null; then
        log_error "pytest not found. Installing..."
        pip install --no-cache-dir pytest
    fi
    
    # Check if coverage is available
    if ! command -v coverage &> /dev/null; then
        log_error "coverage not found. Installing..."
        pip install --no-cache-dir coverage
    fi
}

# Run C++ tests
run_cpp_tests() {
    if [[ "$1" == "--no-cpp" ]]; then
        log_warning "Skipping C++ tests as requested"
        return 0
    fi
    
    log_info "Running C++ tests..."
    
    # Check if CTestTestfile.cmake exists
    if [[ ! -f "$BUILD_DIR/CTestTestfile.cmake" ]]; then
        log_warning "No C++ tests found ($BUILD_DIR/CTestTestfile.cmake)"
        return 0
    fi
    
    # Run CTest
    cd "$BUILD_DIR"
    
    # Get list of test files
    local test_files=($(ctest -N 2>/dev/null | grep "^Test #" | awk '{print $3}'))
    local test_count=${#test_files[@]}
    
    if [[ $test_count -eq 0 ]]; then
        log_warning "No C++ tests found"
        cd ..
        return 0
    fi
    
    log_info "Found $test_count C++ test(s)"
    
    # Run CTest with verbose output if requested
    if [[ "$VERBOSE" == true ]]; then
        if ctest --output-on-failure --timeout $TIMEOUT_SECONDS -j$PARALLEL_JOBS -V; then
            log_success "C++ tests passed"
        else
            log_error "C++ tests failed"
            exit 1
        fi
    else
        if ctest --output-on-failure --timeout $TIMEOUT_SECONDS -j$PARALLEL_JOBS; then
            log_success "C++ tests passed"
        else
            log_error "C++ tests failed"
            exit 1
        fi
    fi
    
    cd ..
}

# Run Python tests
run_python_tests() {
    if [[ "$1" == "--no-python" ]]; then
        log_warning "Skipping Python tests as requested"
        return 0
    fi
    
    log_info "Running Python tests..."
    
    # Activate Python virtual environment
    source "$PYTHON_VENV/bin/activate"
    
    # Check if pytest can find tests
    if [[ ! -d "$PYTHON_TESTS" ]]; then
        log_error "Python tests directory '$PYTHON_TESTS' not found."
        exit 1
    fi
    
    # Find all test files
    local test_files=("$PYTHON_TESTS"/*.py)
    local test_count=${#test_files[@]}
    
    if [[ $test_count -eq 0 ]]; then
        log_warning "No Python tests found in '$PYTHON_TESTS'"
        return 0
    fi
    
    log_info "Found $test_count Python test file(s)"
    
    # Run pytest with options
    local pytest_args=(
        "$PYTHON_TESTS" 
        --cov=pynlm 
        --cov-report=term-missing
    )
    
    if [[ "$VERBOSE" == true ]]; then
        pytest_args+=(--verbose)
    fi
    
    if [[ "$2" == "--coverage" ]]; then
        pytest_args+=(
            --cov=pynlm 
            --cov-report=xml 
            --cov-report=html 
            --cov-report=term
        )
    fi
    
    pytest_args+=(--timeout $TIMEOUT_SECONDS)
    
    # Run pytest
    if pytest "${pytest_args[@]}"; then
        log_success "Python tests passed"
    else
        log_error "Python tests failed"
        exit 1
    fi
}

# Generate coverage report
generate_coverage() {
    if [[ "$2" == "--coverage" ]]; then
        log_info "Generating coverage report..."
        
        source "$PYTHON_VENV/bin/activate"
        
        # Generate coverage report
        coverage report --fail-under=$COVERAGE_THRESHOLD
        
        # Generate HTML coverage report if requested
        if [[ "$3" == "--coverage-html" ]]; then
            coverage html
            log_success "HTML coverage report generated"
        fi
        
        # Generate XML coverage report if requested
        if [[ "$4" == "--coverage-xml" ]]; then
            coverage xml
            log_success "XML coverage report generated"
        fi
    fi
}

# Run benchmarks
run_benchmarks() {
    log_info "Running benchmarks..."
    
    if [[ -f "$BUILD_DIR/benchmark" ]]; then
        log_info "Running benchmark executable..."
        ./$BUILD_DIR/benchmark --benchmark_out=benchmark_results.json --benchmark_out_format=json
        log_success "Benchmarks completed"
    else
        log_warning "Benchmark executable not found in $BUILD_DIR/benchmark"
    fi
}

# Run performance tests
run_performance_tests() {
    log_info "Running performance tests..."
    
    if [[ -f "$BUILD_DIR/performance_tests" ]]; then
        log_info "Running performance test executable..."
        ./$BUILD_DIR/performance_tests --output performance_metrics.json
        log_success "Performance tests completed"
    else
        log_warning "Performance test executable not found in $BUILD_DIR/performance_tests"
    fi
}

# Main test function
main() {
    log_info "Starting NLM test process..."
    log_info "Build directory: $BUILD_DIR"
    log_info "Python virtual environment: $PYTHON_VENV"
    log_info "Python tests directory: $PYTHON_TESTS"
    
    # Check test dependencies
    check_test_dependencies
    
    # Run C++ tests
    run_cpp_tests "$@"
    
    # Run Python tests
    run_python_tests "$@"
    
    # Generate coverage report
    generate_coverage "$@"
    
    # Run benchmarks
    run_benchmarks
    
    # Run performance tests
    run_performance_tests
    
    log_success "Test process completed successfully"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --python-venv)
            PYTHON_VENV="$2"
            shift 2
            ;;
        --python-tests)
            PYTHON_TESTS="$2"
            shift 2
            ;;
        --cpp-tests)
            CPP_TESTS="$2"
            shift 2
            ;;
        --coverage-threshold)
            COVERAGE_THRESHOLD="$2"
            shift 2
            ;;
        --timeout)
            TIMEOUT_SECONDS="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --parallel)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --no-cpp)
            CPP_TESTS_FLAG="--no-cpp"
            shift
            ;;
        --no-python)
            PYTHON_TESTS_FLAG="--no-python"
            shift
            ;;
        --coverage)
            COVERAGE_FLAG="--coverage"
            shift
            ;;
        --coverage-html)
            COVERAGE_HTML_FLAG="--coverage-html"
            shift
            ;;
        --coverage-xml)
            COVERAGE_XML_FLAG="--coverage-xml"
            shift
            ;;
        --list-tests)
            list_tests
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

# Run main function
main "$@"