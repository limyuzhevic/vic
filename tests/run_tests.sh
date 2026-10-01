#!/usr/bin/env bash
# NLM Phase 6 Improved Components Test Runner
# This script runs all the enhanced component tests for NLM Phase 6

# Configuration
PROJECT_ROOT="/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_032c81e1-5b0f-4291-b9a7-b7c1a9b2a45c"
BUILD_DIR="${PROJECT_ROOT}/build"
TEST_BINARY="${BUILD_DIR}/nlm_test_improved_components"

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

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    log_error "Build directory not found: $BUILD_DIR"
    log_info "Please run CMake build first"
    log_info "Commands:"
    log_info "  mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release"
    log_info "  make -j4"
    exit 1
fi

# Check if test binary exists
if [ ! -f "$TEST_BINARY" ]; then
    log_error "Test binary not found: $TEST_BINARY"
    log_info "Please build the tests first"
    log_info "Run: make -j4"
    exit 1
fi

# Function to run tests
run_tests() {
    log_info "Running NLM Phase 6 Improved Components Tests..."
    log_info "Tests include:"
    log_info "  ✓ Enhanced config validation and error handling"
    log_info "  ✓ Real reward computation with multiple components"
    log_info "  ✓ Enhanced neuromodulation system integration"
    log_info "  ✓ Comprehensive error handling and logging"
    log_info "  ✓ Integration with existing Phase 6 experiment"
    log_info ""
    
    # Run the test binary
    log_info "Executing: $TEST_BINARY"
    echo "========================================="
    
    if $TEST_BINARY; then
        log_info ""
        log_info "========================================="
        log_info "✓ All tests passed successfully!"
        log_info ""
        log_info "The NLM Phase 6 improved components are working correctly."
        log_info ""
        log_info "Summary of improvements tested:"
        log_info "  ✓ Enhanced config validation with error messages"
        log_info "  ✓ Real reward computation based on survival, efficiency, learning, and exploration"
        log_info "  ✓ Enhanced neuromodulation integration with cross-system effects"
        log_info "  ✓ Comprehensive error handling and context-aware logging"
        log_info "  ✓ Integration with existing Phase 6 experiment"
        exit 0
    else
        local exit_code=$?
        log_error ""
        log_error "========================================="
        log_error "✗ Tests failed with exit code: $exit_code"
        log_error ""
        log_error "Please check the output above for details on test failures."
        exit $exit_code
    fi
}

# Function to run specific tests
run_specific_test() {
    local test_name=$1
    log_info "Running specific test: $test_name"
    log_warn "Specific test execution not yet implemented"
    log_info "Please run all tests instead: ./run_tests.sh"
}

# Main script logic
case "$1" in
    "--help"|-h)
        echo "Usage: $0 [OPTIONS]"
        echo ""
        echo "Options:"
        echo "  --help, -h          Show this help message"
        echo "  --run-specific      Run specific tests (not yet implemented)"
        echo "  --build             Build the tests first"
        echo ""
        echo "To run all tests:"
        echo "  $0"
        echo ""
        echo "To build and run tests:"
        echo "  mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4 && cd .. && $0"
        exit 0
        ;;
    "--build"|-b)
        log_info "Building NLM tests..."
        cd "$PROJECT_ROOT"
        mkdir -p build
        cd build
        
        if cmake .. -DCMAKE_BUILD_TYPE=Release; then
            log_info "CMake configuration successful"
            if make -j4; then
                log_info "Build successful"
                cd "$PROJECT_ROOT"
                run_tests
            else
                log_error "Build failed"
                exit 1
            fi
        else
            log_error "CMake configuration failed"
            exit 1
        fi
        ;;
    "--run-specific"|-r)
        if [ -n "$2" ]; then
            run_specific_test "$2"
        else
            log_error "No test name specified"
            echo "Usage: $0 --run-specific <test_name>"
            echo "Available tests:"
            echo "  config_validation"
            echo "  reward_computation"
            echo "  neuromodulation"
            echo "  error_handling"
            echo "  phase6_integration"
            exit 1
        fi
        ;;
    *)
        # Default: run all tests (assuming they are already built)
        if [ -f "$TEST_BINARY" ]; then
            run_tests
        else
            log_warn "Test binary not found. Building tests first..."
            bash "$0" --build
        fi
        ;;
esac

echo "Usage: $0 [OPTIONS]"
echo ""
echo "Options:"
echo "  --help, -h          Show this help message"
echo "  --build, -b         Build the tests first"
echo "  --run-specific, -r Run specific tests (not yet implemented)"
echo ""
echo "To run all tests:"
echo "  $0"
exit 1
