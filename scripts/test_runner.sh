#!/bin/bash

# NLM Development Testing Script
# This script runs the test suite and provides testing utilities

set -e

echo "=== NLM Development Testing Suite ==="

# Function to print colored output
print_info() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

print_success() {
    echo -e "\033[1;32m[SUCCESS]\033[0m $1"
}

print_error() {
    echo -e "\033[1;31m[ERROR]\033[0m $1"
}

# Run all tests
run_all_tests() {
    print_info "Running complete test suite..."
    
    if [ ! -f "build/CMakeCache.txt" ]; then
        print_error "Release build not found. Run './scripts/dev_setup.sh' first."
        exit 1
    fi
    
    print_info "Running Release build tests..."
    cd build && ctest --output-on-failure
    cd ..
    
    print_success "Release build tests completed."
}

# Run specific test categories
run_unit_tests() {
    print_info "Running unit tests..."
    cd build && ctest -R "unit_tests" --output-on-failure
    cd ..
}

run_integration_tests() {
    print_info "Running integration tests..."
    cd build && ctest -R "integration_tests" --output-on-failure
    cd ..
}

run_performance_tests() {
    print_info "Running performance tests..."
    cd build && ctest -R "performance_tests" --output-on-failure
    cd ..
}

# Test analysis
analyze_tests() {
    print_info "Analyzing test results..."
    
    if [ -f "build/Testing/Temporary/LastTest.log" ]; then
        print_info "Generating test summary..."
        cd build && ctest -N
        cd ..
    fi
}

# Generate test report
generate_test_report() {
    print_info "Generating test report..."
    
    if [ -d "build" ]; then
        print_info "Creating test report..."
        
        # Generate test summary
        cd build && ctest -N > ../test_summary.txt
        
        # Generate performance report if available
        if [ -f "performance_results.json" ]; then
            echo "Performance metrics included" >> test_summary.txt
        fi
        
        cd ..
        print_success "Test report generated: test_summary.txt"
    fi
}

# Debug tests
debug_tests() {
    print_info "Setting up debug test environment..."
    
    # Create debug build if needed
    if [ ! -f "build_debug/CMakeCache.txt" ]; then
        print_info "Creating debug build..."
        cmake -S . -B build_debug -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON -DNLM_ENABLE_COVERAGE=ON
    fi
    
    print_info "Building debug version..."
    cmake --build build_debug --parallel
    
    print_info "Running tests with sanitizers..."
    cd build_debug && ctest --output-on-failure
    cd ..
}

# Run tests with coverage
run_coverage_tests() {
    print_info "Running tests with coverage..."
    
    if [ ! -f "build/CMakeCache.txt" ]; then
        print_error "Release build with coverage not found."
        exit 1
    fi
    
    cd build && ctest --output-on-failure
    
    if [ -f "coverage_report.html" ]; then
        print_info "Coverage report available: coverage_report.html"
    fi
    
    cd ..
}

# Test performance benchmarks
run_performance_benchmarks() {
    print_info "Running performance benchmarks..."
    
    # Run performance tests
    run_performance_tests
    
    # Generate performance analysis
    print_info "Analyzing performance data..."
    
    # Check for performance metrics
    if [ -d "build/Testing/Temporary" ]; then
        echo "Performance benchmarks completed."
        echo "Results available in build/Testing/Temporary/"
    fi
}

# Main function
main() {
    case "$1" in
        all)
            run_all_tests
            ;;
        unit)
            run_unit_tests
            ;;
        integration)
            run_integration_tests
            ;;
        performance)
            run_performance_benchmarks
            ;;
        coverage)
            run_coverage_tests
            ;;
        debug)
            debug_tests
            ;;
        analyze)
            analyze_tests
            ;;
        report)
            generate_test_report
            ;;
        *)
            echo "Usage: $0 {all|unit|integration|performance|coverage|debug|analyze|report}"
            exit 1
            ;;
    esac
}

# Handle command line arguments
if [ $# -eq 0 ]; then
    echo "Usage: $0 {all|unit|integration|performance|coverage|debug|analyze|report}"
    echo "Examples:"
    echo "  $0 all         Run all tests"
    echo "  $0 unit        Run only unit tests"
    echo "  $0 integration Run only integration tests"
    echo "  $0 debug       Run tests with debug builds and sanitizers"
    exit 1
fi

main "$1"