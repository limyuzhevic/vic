#!/bin/bash

# NLM Test Runner Script
# =======================
# This script runs the comprehensive NLM test suite including unit tests,
# integration tests, performance benchmarks, and regression tests.
#
# Usage:
#   ./run_tests.sh              # Run all tests
#   ./run_tests.sh --benchmark  # Run performance benchmarks
#   ./run_tests.sh --unit       # Run unit tests only
#   ./run_tests.sh --integration # Run integration tests only
#   ./run_tests.sh --help       # Show help information
#
# Test Categories:
# - Unit Tests: Component-level testing (neurons, synapses, basic operations)
# - Integration Tests: System-level testing (brain initialization, learning)
# - Performance Tests: Benchmarking and optimization
# - Regression Tests: Bug prevention and stability verification
#
# Output:
# - Test results are saved to: test_results.txt
# - Performance metrics are saved to: performance_metrics.txt
# - All test logs are saved to: test_logs/

# Exit on any error
set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
TEST_BINARY="$BUILD_DIR/nlm_test"

# Colors for output
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
NC="\033[0m" # No Color

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
    if [[ "$DEBUG" == "true" ]]; then
        echo -e "${BLUE}[DEBUG]${NC} $1"
    fi
}

# Print help information
print_help() {
    cat << EOF
NLM Test Suite Runner
=====================

Usage:
  $0                    Run all tests (default)
  $0 --benchmark        Run performance benchmarks only
  $0 --unit             Run unit tests only
  $0 --integration      Run integration tests only
  $0 --help             Show this help message

Test Categories:
  Unit Tests            Component-level testing (neurons, synapses, basic operations)
  Integration Tests     System-level testing (brain initialization, learning, plasticity)
  Performance Tests     Benchmarking and optimization testing
  Regression Tests      Bug prevention and stability verification

Output Files:
  test_results.txt      Detailed test results and summary
  performance_metrics.txt Performance benchmark results
  test_logs/            Detailed test logs (if verbose mode enabled)

Configuration:
  DEBUG=true           Enable debug logging (can be set to false to reduce output)
  VERBOSE=true         Enable verbose output
  TIMEOUT=300         Test timeout in seconds (default: 5 minutes)

Examples:
  $0 --unit
  # Run only unit tests with detailed output

  $0 --benchmark
  # Run performance benchmarks

  $0 --integration --verbose
  # Run integration tests with verbose output

  $0 --help
  # Show detailed help information
EOF
}

# Check if test binary exists
check_test_binary() {
    if [[ ! -f "$TEST_BINARY" ]]; then
        log_error "Test binary not found: $TEST_BINARY"
        log_error "Please build the project first with: cmake --build build"
        exit 1
    fi
}

# Run unit tests
run_unit_tests() {
    log_info "Running unit tests..."
    
    local test_args="--unit"
    if [[ "$VERBOSE" == "true" ]]; then
        test_args="$test_args --verbose"
    fi
    
    if [[ "$DEBUG" == "true" ]]; then
        log_debug "Unit test arguments: $test_args"
    fi
    
    # Run the test binary with unit test flag
    if timeout $TIMEOUT "$TEST_BINARY" $test_args; then
        log_info "Unit tests completed successfully"
    else
        log_error "Unit tests failed or timed out after ${TIMEOUT}s"
        exit 1
    fi
}

# Run integration tests
run_integration_tests() {
    log_info "Running integration tests..."
    
    local test_args="--integration"
    if [[ "$VERBOSE" == "true" ]]; then
        test_args="$test_args --verbose"
    fi
    
    if [[ "$DEBUG" == "true" ]]; then
        log_debug "Integration test arguments: $test_args"
    fi
    
    # Run the test binary with integration test flag
    if timeout $TIMEOUT "$TEST_BINARY" $test_args; then
        log_info "Integration tests completed successfully"
    else
        log_error "Integration tests failed or timed out after ${TIMEOUT}s"
        exit 1
    fi
}

# Run performance benchmarks
run_performance_benchmarks() {
    log_info "Running performance benchmarks..."
    
    local benchmark_args="--benchmark"
    if [[ "$VERBOSE" == "true" ]]; then
        benchmark_args="$benchmark_args --verbose"
    fi
    
    if [[ "$DEBUG" == "true" ]]; then
        log_debug "Benchmark arguments: $benchmark_args"
    fi
    
    # Run the test binary with benchmark flag
    if timeout $TIMEOUT "$TEST_BINARY" $benchmark_args; then
        log_info "Performance benchmarks completed successfully"
    else
        log_error "Performance benchmarks failed or timed out after ${TIMEOUT}s"
        exit 1
    fi
}

# Run all tests
run_all_tests() {
    log_info "Running all NLM tests..."
    
    # Run unit tests
    run_unit_tests
    
    # Run integration tests
    run_integration_tests
    
    # Run performance benchmarks
    run_performance_benchmarks
    
    log_info "All tests completed successfully!"
}

# Setup test environment
setup_environment() {
    log_info "Setting up test environment..."
    
    # Create directories for test outputs
    mkdir -p "$PROJECT_ROOT/test_logs"
    mkdir -p "$PROJECT_ROOT/results"
    
    # Set default values
    DEBUG="${DEBUG:-false}"
    VERBOSE="${VERBOSE:-true}"
    TIMEOUT="${TIMEOUT:-300}"
    
    # Check if test binary exists
    check_test_binary
    
    log_info "Test environment setup completed"
}

# Cleanup test environment
cleanup_environment() {
    log_info "Cleaning up test environment..."
    
    # Remove temporary files
    rm -f "$PROJECT_ROOT/test_results.txt"
    rm -f "$PROJECT_ROOT/performance_metrics.txt"
    
    # Archive logs if they exist
    if [[ -d "$PROJECT_ROOT/test_logs" ]]; then
        log_info "Test logs are available in: $PROJECT_ROOT/test_logs"
    fi
    
    log_info "Test environment cleanup completed"
}

# Main execution
main() {
    # Parse command line arguments
    for arg in "$@"; do
        case $arg in
            --help)
                print_help
                exit 0
                ;;
            --benchmark)
                BENCHMARK_ONLY=true
                shift
                ;;
            --unit)
                UNIT_ONLY=true
                shift
                ;;
            --integration)
                INTEGRATION_ONLY=true
                shift
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --debug)
                DEBUG=true
                shift
                ;;
            --timeout=*) 
                TIMEOUT="${arg#--timeout=}"
                shift
                ;;
            *)
                log_error "Unknown argument: $arg"
                print_help
                exit 1
                ;;
        esac
    done
    
    # Setup environment
    setup_environment
    
    # Run tests based on arguments
    if [[ "${INTEGRATION_ONLY:-false}" == "true" ]]; then
        run_integration_tests
    elif [[ "${UNIT_ONLY:-false}" == "true" ]]; then
        run_unit_tests
    elif [[ "${BENCHMARK_ONLY:-false}" == "true" ]]; then
        run_performance_benchmarks
    else
        run_all_tests
    fi
    
    # Cleanup environment
    cleanup_environment
    
    # Display test results summary
    if [[ -f "$PROJECT_ROOT/test_results.txt" ]]; then
        log_info "Test results summary:"
        cat "$PROJECT_ROOT/test_results.txt"
    fi
    
    log_info "NLM Test Suite completed successfully!"
}

# Run main function with all arguments
main "$@"