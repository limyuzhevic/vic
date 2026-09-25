#!/bin/bash
# scripts/run_tests.sh
# Test execution script for NLM

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Color codes for output
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

# Parse command line arguments
VERBOSE=false
RUN_INTEGRATION=false
RUN_PERFORMANCE=false
RUN_STRESS=false
COVERAGE_REPORT=""
BENCHMARK_OUTPUT=""
TEST_FILTER=""
BUILD_DIR="${PROJECT_ROOT}/build"

while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -i|--integration)
            RUN_INTEGRATION=true
            shift
            ;;
        -p|--performance)
            RUN_PERFORMANCE=true
            shift
            ;;
        -s|--stress)
            RUN_STRESS=true
            shift
            ;;
        -c|--coverage)
            COVERAGE_REPORT="$2"
            shift 2
            ;;
        -b|--benchmark)
            BENCHMARK_OUTPUT="$2"
            shift 2
            ;;
        -f|--filter)
            TEST_FILTER="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -v, --verbose           Enable verbose output"
            echo "  -i, --integration       Run integration tests only"
            echo "  -p, --performance       Run performance benchmarks only"
            echo "  -s, --stress            Run stress tests only"
            echo "  -c, --coverage FILE     Generate coverage report to FILE"
            echo "  -b, --benchmark FILE    Save benchmark results to FILE"
            echo "  -f, --filter PATTERN    Filter tests by PATTERN"
            echo "  -h, --help              Show this help message"
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Build the project
log_info "Building NLM..."
cd "$PROJECT_ROOT"
if ! cmake -B "$BUILD_DIR" -S "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release; then
    log_error "Build failed"
    exit 1
fi

if ! cmake --build "$BUILD_DIR" --config Release --parallel $(nproc); then
    log_error "Build failed"
    exit 1
fi

log_info "Build completed successfully"

# Run tests
log_info "Running NLM tests..."

# Run unit tests
if [ "$RUN_INTEGRATION" = false ] && [ "$RUN_PERFORMANCE" = false ] && [ "$RUN_STRESS" = false ]; then
    cd "$BUILD_DIR"
    if [ -f "./nlm_tests" ]; then
        log_info "Running unit tests..."
        if [ "$VERBOSE" = true ]; then
            ./nlm_tests --verbose
        else
            ./nlm_tests
        fi
        
        if [ "$TEST_FILTER" != "" ]; then
            log_info "Filtering tests by pattern: $TEST_FILTER"
        fi
    else
        log_error "Test executable not found: ./nlm_tests"
        exit 1
    fi
fi

# Run integration tests
if [ "$RUN_INTEGRATION" = true ] || [ "$RUN_STRESS" = true ]; then
    cd "$BUILD_DIR"
    if [ -f "./nlm_integration_tests" ]; then
        log_info "Running integration tests..."
        if [ "$VERBOSE" = true ]; then
            ./nlm_integration_tests --verbose
        else
            ./nlm_integration_tests
        fi
    else
        log_error "Integration test executable not found: ./nlm_integration_tests"
    fi
fi

# Run performance benchmarks
if [ "$RUN_PERFORMANCE" = true ] || [ "$RUN_STRESS" = true ]; then
    cd "$BUILD_DIR"
    if [ -f "./nlm_benchmarks" ]; then
        log_info "Running performance benchmarks..."
        if [ "$VERBOSE" = true ]; then
            ./nlm_benchmarks --verbose
        else
            ./nlm_benchmarks
        fi
        
        if [ "$BENCHMARK_OUTPUT" != "" ]; then
            log_info "Saving benchmark results to: $BENCHMARK_OUTPUT"
            cp benchmarks_results.json "$BENCHMARK_OUTPUT" 2>/dev/null || log_warn "Could not save benchmark results"
        fi
    else
        log_error "Benchmark executable not found: ./nlm_benchmarks"
    fi
fi

# Generate coverage report
if [ "$COVERAGE_REPORT" != "" ]; then
    cd "$BUILD_DIR"
    if [ -f "./coverage_analyzer" ]; then
        log_info "Generating coverage report: $COVERAGE_REPORT"
        ./coverage_analyzer --output "$COVERAGE_REPORT"
    else
        log_error "Coverage analyzer executable not found: ./coverage_analyzer"
    fi
fi

log_info "Test execution completed"

# Summary
if [ "$VERBOSE" = true ]; then
    log_info "Test run completed"
fi

exit 0