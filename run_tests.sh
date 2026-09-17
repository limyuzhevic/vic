#!/bin/bash

# NLM Performance Testing Automation Script
# This script provides comprehensive testing and benchmarking automation for NLM
# Usage: ./run_tests.sh [mode]
# Modes: all, unit, integration, performance, regression, continuous

set -e

# Configuration
PROJECT_ROOT="/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae95a01c-5f66-42ae-adef-cc6d9f0a7a6e"
BUILD_DIR="${PROJECT_ROOT}/build"
TEST_DIR="${PROJECT_ROOT}/tests"
PERF_DIR="${TEST_DIR}/performance"
INTEGRATION_DIR="${TEST_DIR}/integration"
SCRIPTS_DIR="${PROJECT_ROOT}/scripts"

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

log_header() {
    echo -e "${BLUE}[HEADER]${NC} $1"
}

# Function to check if build exists
check_build() {
    if [ ! -d "$BUILD_DIR" ] || [ ! -f "$BUILD_DIR/nlm_phase6_demo" ]; then
        log_warn "Build directory not found. Running CMake build..."
        cd "$PROJECT_ROOT"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
        if [ $? -ne 0 ]; then
            log_error "Build failed"
            exit 1
        fi
    fi
}

# Function to run unit tests
run_unit_tests() {
    log_header "Running Unit Tests"
    
    # Run enhanced logger tests
    if [ -f "${BUILD_DIR}/test_logger" ]; then
        log_info "Running enhanced logger tests..."
        "${BUILD_DIR}/test_logger"
    else
        log_warn "test_logger not found in build directory"
    fi
    
    # Run brain tests
    log_info "Building and running brain tests..."
    cd "$PROJECT_ROOT"
    mkdir -p build_unit_tests
    cd build_unit_tests
    cmake -DCMAKE_BUILD_TYPE=Debug ../
    make -j4
    
    # Run compiled tests
    if [ -f "./test_brain" ]; then
        ./test_brain
    fi
    
    if [ -f "./test_neuron" ]; then
        ./test_neuron
    fi
    
    if [ -f "./test_synapse" ]; then
        ./test_synapse
    fi
    
    cd "$PROJECT_ROOT"
}

# Function to run integration tests
run_integration_tests() {
    log_header "Running Integration Tests"
    
    # Compile and run integration tests
    cd "$PROJECT_ROOT"
    mkdir -p build_integration_tests
    cd build_integration_tests
    cmake -DCMAKE_BUILD_TYPE=Debug ../
    make -j4
    
    # Run integration tests
    if [ -f "./test_integration" ]; then
        ./test_integration
    fi
    
    # Run with profiling
    if [ -f "./nlm_phase6_demo" ]; then
        log_info "Running Phase 6 demo with profiling..."
        ./nlm_phase6_demo --profile
    fi
    
    cd "$PROJECT_ROOT"
}

# Function to run performance benchmarks
run_performance_benchmarks() {
    log_header "Running Performance Benchmarks"
    
    # Initialize benchmark directory
    mkdir -p "$PERF_DIR/baselines"
    mkdir -p "$PERF_DIR/results"
    
    # Run performance profiler tests
    log_info "Running performance profiler tests..."
    g++ -std=c++17 -I"${PROJECT_ROOT}/src" -I"${PROJECT_ROOT}/src/profiling" 
         -I"${PROJECT_ROOT}/src/brain" -I"${PROJECT_ROOT}/src/core" -I"${PROJECT_ROOT}/src/monitoring"
         "${PERF_DIR}/test_performance_profiler.cpp" "${PROJECT_ROOT}/src/profiling/PerformanceProfiler.cpp" 
         "${PROJECT_ROOT}/src/monitoring/PerformanceProfiler.cpp" -o "${PERF_DIR}/test_performance_profiler"
    
    if [ -f "${PERF_DIR}/test_performance_profiler" ]; then
        "${PERF_DIR}/test_performance_profiler"
    fi
    
    # Run comprehensive benchmarks
    log_info "Running comprehensive benchmarks..."
    cd "$PROJECT_ROOT"
    mkdir -p build_benchmarks
    cd build_benchmarks
    cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ../
    make -j4
    
    # Run brain benchmarks
    if [ -f "./benchmark_brain" ]; then
        ./benchmark_brain --benchmarks_dir="${PERF_DIR}" --results_dir="${PERF_DIR}/results"
    fi
    
    # Generate performance reports
    log_info "Generating performance reports..."
    if [ -f "${PERF_DIR}/generate_performance_reports.py" ]; then
        python3 "${PERF_DIR}/generate_performance_reports.py" "$PERF_DIR/results" "$PERF_DIR/baselines"
    fi
    
    cd "$PROJECT_ROOT"
}

# Function to run regression tests
run_regression_tests() {
    log_header "Running Regression Tests"
    
    # Run comparison with baseline
    log_info "Running regression analysis..."
    cd "$PROJECT_ROOT"
    mkdir -p build_regression_tests
    cd build_regression_tests
    cmake -DCMAKE_BUILD_TYPE=Debug ../
    make -j4
    
    if [ -f "./test_regression" ]; then
        ./test_regression
    fi
    
    # Generate regression report
    log_info "Generating regression report..."
    if [ -f "${SCRIPTS_DIR}/generate_regression_report.sh" ]; then
        bash "${SCRIPTS_DIR}/generate_regression_report.sh" "$PERF_DIR"
    fi
    
    cd "$PROJECT_ROOT"
}

# Function to run continuous benchmarking
run_continuous_benchmarks() {
    log_header "Starting Continuous Benchmarking"
    
    # Initialize continuous benchmarking directory
    mkdir -p "$PERF_DIR/continuous"
    
    # Run continuous benchmarking script
    log_info "Starting continuous benchmarking service..."
    if [ -f "${SCRIPTS_DIR}/start_continuous_benchmarking.py" ]; then
        python3 "${SCRIPTS_DIR}/start_continuous_benchmarking.py" "$PERF_DIR/continuous"
    else
        log_warn "Continuous benchmarking script not found"
    fi
}

# Function to run all tests
run_all_tests() {
    log_header "Running All Tests"
    
    # Build if necessary
    check_build
    
    # Run all test suites
    run_unit_tests
    run_integration_tests
    run_performance_benchmarks
    run_regression_tests
    
    log_info "All tests completed"
}

# Function to show usage
usage() {
    echo "Usage: $0 [mode]"
    echo ""
    echo "Modes:"
    echo "  all      Run all tests (unit, integration, performance, regression)"
    echo "  unit     Run unit tests only"
    echo "  integration  Run integration tests only"
    echo "  performance    Run performance benchmarks only"
    echo "  regression    Run regression tests only"
    echo "  continuous    Start continuous benchmarking"
    echo "  clean     Clean build directories"
    echo ""
    echo "Environment variables:"
    echo "  NLM_BUILD_TYPE      Build type (Debug, Release, RelWithDebInfo)"
    echo "  NLM_ENABLE_PROFILING    Enable profiling (true/false)"
    echo "  NLM_ENABLE_BENCHMARKS  Enable benchmarking (true/false)"
    echo ""
    echo "Examples:"
    echo "  $0 all"
    echo "  $0 performance"
    echo "  $0 integration"
}

# Function to clean build directories
clean() {
    log_header "Cleaning Build Directories"
    
    rm -rf "${PROJECT_ROOT}/build"
    rm -rf "${PROJECT_ROOT}/build_unit_tests"
    rm -rf "${PROJECT_ROOT}/build_integration_tests"
    rm -rf "${PROJECT_ROOT}/build_benchmarks"
    rm -rf "${PROJECT_ROOT}/build_regression_tests"
    
    log_info "Build directories cleaned"
}

# Main script logic
case "$1" in
    "all")
        run_all_tests
        ;;
    "unit")
        check_build
        run_unit_tests
        ;;
    "integration")
        check_build
        run_integration_tests
        ;;
    "performance")
        check_build
        run_performance_benchmarks
        ;;
    "regression")
        check_build
        run_regression_tests
        ;;
    "continuous")
        run_continuous_benchmarks
        ;;
    "clean")
        clean
        ;;
    "--help"|"-h")
        usage
        ;;
    *)
        log_error "Unknown mode: $1"
        usage
        exit 1
        ;;
esac
