#!/bin/bash

# NLM Build Script
# Comprehensive build automation for the NLM (熙然) computational brain project
# Phase 6: FINAL INTEGRATION

set -e  # Exit on any error
set -o pipefail  # Exit on pipeline errors

# Configuration
PROJECT_NAME="NLM"
PROJECT_VERSION="0.1.0"
BUILD_DIR="${1:-build}"
BUILD_TYPE="${2:-Release}"
PARALLEL_JOBS="${3:-$(nproc)}"
RUN_TESTS="${4:-true}"
VERBOSE="${5:-false}"
CONFIG_FILE="${6:-configs/default.cfg}"

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
    if [ "$VERBOSE" = "true" ]; then
        echo -e "${BLUE}[DEBUG]${NC} $1"
    fi
}

# Check if we're in the right directory
check_environment() {
    log_info "Checking build environment..."
    
    if [ ! -f "CMakeLists.txt" ]; then
        log_error "CMakeLists.txt not found. Please run this script from the project root."
        exit 1
    fi
    
    if [ ! -f "pyproject.toml" ]; then
        log_error "pyproject.toml not found. Please run this script from the project root."
        exit 1
    fi
    
    # Check for required tools
    if ! command -v cmake &> /dev/null; then
        log_error "CMake not found. Please install CMake."
        exit 1
    fi
    
    if ! command -v make &> /dev/null; then
        log_error "make not found. Please install make."
        exit 1
    fi
    
    log_info "Environment check passed."
}

# Create build directory
create_build_dir() {
    log_info "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || exit 1
}

# Configure CMake
cmake_configure() {
    log_info "Configuring CMake with build type: $BUILD_TYPE"
    
    local cmake_args="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    
    if [ "$VERBOSE" = "true" ]; then
        cmake_args="$cmake_args -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
    fi
    
    log_debug "CMake arguments: $cmake_args"
    
    cmake $cmake_args ..
}

# Build the project
build_project() {
    log_info "Building project with $PARALLEL_JOBS parallel jobs..."
    
    if [ "$VERBOSE" = "true" ]; then
        make -j$PARALLEL_JOBS VERBOSE=1
    else
        make -j$PARALLEL_JOBS
    fi
    
    log_info "Build completed successfully."
}

# Run tests
run_tests() {
    if [ "$RUN_TESTS" != "true" ]; then
        log_warn "Test execution disabled. To enable, set RUN_TESTS=true"
        return 0
    fi
    
    log_info "Running tests..."
    
    if [ -f "CTestTestfile.cmake" ]; then
        if ctest --output-on-failure; then
            log_info "All tests passed."
        else
            log_error "Tests failed."
            return 1
        fi
    else
        log_warn "No CTest configuration found. Skipping tests."
    fi
}

# Validate configuration file
validate_config() {
    if [ ! -f "$CONFIG_FILE" ]; then
        log_warn "Configuration file $CONFIG_FILE not found."
        return 0
    fi
    
    log_info "Validating configuration file: $CONFIG_FILE"
    
    # Basic validation - check for duplicate lines
    if command -v sort &> /dev/null; then
        local line_count=$(wc -l < "$CONFIG_FILE" | tr -d ' ')
        local unique_count=$(sort "$CONFIG_FILE" | uniq | wc -l | tr -d ' ')
        
        if [ "$line_count" -ne "$unique_count" ]; then
            log_warn "Configuration file may contain duplicate lines."
            log_debug "Total lines: $line_count, Unique lines: $unique_count"
        fi
    fi
    
    log_info "Configuration validation completed."
}

# Build Python bindings
build_python_bindings() {
    log_info "Building Python bindings..."
    
    if [ -f "../python/CMakeLists.txt" ]; then
        mkdir -p python_build
        cd python_build || return 0
        
        if cmake ../python; then
            make -j$PARALLEL_JOBS
            cd .. || return 0
        else
            log_warn "Failed to build Python bindings. Skipping."
        fi
    else
        log_warn "Python bindings directory not found. Skipping."
    fi
}

# Run Phase 6 demo
run_phase6_demo() {
    log_info "Running Phase 6 integration demo..."
    
    if [ ! -f "nlm_phase6_demo" ]; then
        log_error "nlm_phase6_demo executable not found. Build the project first."
        return 1
    fi
    
    ./nlm_phase6_demo
}

# Display build summary
display_summary() {
    echo -e "\n${GREEN}=== Build Summary ===${NC}"
    echo "Project: $PROJECT_NAME v$PROJECT_VERSION"
    echo "Build directory: $(pwd)"
    echo "Build type: $BUILD_TYPE"
    echo "Parallel jobs: $PARALLEL_JOBS"
    echo "Tests enabled: $RUN_TESTS"
    echo ""
    echo -e "${GREEN}Available executables:${NC}"
    if [ -f "nlm" ]; then
        echo "  ./nlm - Main NLM executable"
    fi
    if [ -f "nlm_phase3_demo" ]; then
        echo "  ./nlm_phase3_demo - Phase 3 demo"
    fi
    if [ -f "nlm_phase4_demo" ]; then
        echo "  ./nlm_phase4_demo - Phase 4 demo"
    fi
    if [ -f "nlm_phase6_demo" ]; then
        echo "  ./nlm_phase6_demo - Phase 6 integration demo"
    fi
    echo ""
    echo -e "${GREEN}Available libraries:${NC}"
    if [ -f "libnlm_core.a" ] || [ -f "libnlm_core.so" ]; then
        echo "  libnlm_core.a - Core neural computation library"
    fi
    if [ -f "libnlm_agent.a" ] || [ -f "libnlm_agent.so" ]; then
        echo "  libnlm_agent.a - Agent interface library"
    fi
    if [ -f "libnlm_world.a" ] || [ -f "libnlm_world.so" ]; then
        echo "  libnlm_world.a - World simulation library"
    fi
    if [ -f "libnlm_phase6.a" ] || [ -f "libnlm_phase6.so" ]; then
        echo "  libnlm_phase6.a - Phase 6 experiments library"
    fi
}

# Clean build
do_clean() {
    log_info "Cleaning build directory..."
    
    if [ -d "$BUILD_DIR" ]; then
        cd "$BUILD_DIR"
        rm -rf *
        cd ..
        log_info "Cleaned $BUILD_DIR"
    else
        log_warn "Build directory $BUILD_DIR does not exist."
    fi
}

# Main build process
main() {
    log_info "Starting $PROJECT_NAME build process..."
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --clean)
                do_clean
                exit 0
                ;;
            --debug)
                BUILD_TYPE="Debug"
                shift
                ;;
            --relwithdebinfo)
                BUILD_TYPE="RelWithDebInfo"
                shift
                ;;
            --no-tests)
                RUN_TESTS="false"
                shift
                ;;
            --verbose)
                VERBOSE="true"
                shift
                ;;
            --help)
                echo "Usage: $0 [options]"
                echo "Options:"
                echo "  --clean          Clean build directory and exit"
                echo "  --debug          Build with debug symbols (default Release)"
                echo "  --relwithdebinfo Build with debug symbols and optimization (default)"
                echo "  --no-tests       Disable test execution"
                echo "  --verbose        Enable verbose output"
                echo "  --help           Show this help message"
                exit 0
                ;;
            *)
                shift
                ;;
        esac
    done
    
    # Execute build process
    check_environment
    create_build_dir
    cmake_configure
    build_project
    run_tests
    validate_config
    build_python_bindings
    display_summary
    
    log_info "$PROJECT_NAME build completed successfully!"
}

# Run main function with all arguments
main "$@"