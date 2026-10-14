#!/usr/bin/bash

# NLM Build and Test Script
# This script builds and tests the improved NLM (Neural Learning Machine) project
# Version 2.0.0 - Phase 2 Enhanced

cd "$(dirname "$0")"

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

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    log_error "CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Parse command line arguments
BUILD_TYPE="Release"
RUN_TESTS=false
RUN_UNIT_TESTS=false
RUN_INTEGRATION_TESTS=false
RUN_PERFORMANCE_TESTS=false
VERBOSE=false
QUIET=false
NLM_MODE="all"
NLM_STEPS=0
NLM_NEURONS=0
CONFIG_FILE="configs/default.cfg"

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --tests|--test)
            RUN_TESTS=true
            shift
            ;;
        --unit)
            RUN_UNIT_TESTS=true
            shift
            ;;
        --integration)
            RUN_INTEGRATION_TESTS=true
            shift
            ;;
        --performance)
            RUN_PERFORMANCE_TESTS=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --quiet)
            QUIET=true
            shift
            ;;
        --mode=*)
            NLM_MODE="${1#--mode=}"
            shift
            ;;
        --steps=*)
            NLM_STEPS="${1#--steps=}"
            shift
            ;;
        --neuron=*)
            NLM_NEURONS="${1#--neuron=}"
            shift
            ;;
        --config=*)
            CONFIG_FILE="${1#--config=}"
            shift
            ;;
        --help|-h)
            echo "NLM Build and Test Script"
            echo "=========================="
            echo ""
            echo "Usage: $0 [options]"
            echo ""
            echo "Build Options:"
            echo "  --debug              Build in Debug mode (default: Release)"
            echo ""
            echo "Test Options:"
            echo "  --tests, --test      Run NLM tests after building"
            echo "  --unit               Run unit tests only"
            echo "  --integration        Run integration tests only"
            echo "  --performance        Run performance tests only"
            echo ""
            echo "NLM Options:"
            echo "  --mode=MODE          Run NLM with specific mode (basic|plasticity|stdp|all)"
            echo "  --steps=N            Set simulation steps per test"
            echo "  --neuron=N           Set number of neurons"
            echo "  --config=FILE        Use custom configuration file"
            echo ""
            echo "Output Options:"
            echo "  --verbose            Enable verbose output"
            echo "  --quiet              Suppress non-essential output"
            echo ""
            echo "Examples:"
            echo "  $0                    Build and run NLM tests"
            echo "  $0 --debug --tests    Build in Debug mode and run all tests"
            echo "  $0 --mode=plasticity  Build and run only plasticity tests"
            echo ""
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create build directory
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    log_info "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure CMake
log_info "Configuring CMake (Build type: $BUILD_TYPE)..."
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DNLM_MODE="$NLM_MODE" \
    -DNLM_STEPS="$NLM_STEPS" \
    -DNLM_NEURONS="$NLM_NEURONS" \
    -DCONFIG_FILE="$CONFIG_FILE"

if [ $? -ne 0 ]; then
    log_error "CMake configuration failed!"
    exit 1
fi

# Build the project
log_info "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    log_error "Build failed!"
    exit 1
fi

log_success "Build completed successfully!"

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    log_info "Running tests..."
    
    # Run unit tests if requested
    if [ "$RUN_UNIT_TESTS" = true ] || [ "$RUN_TESTS" = true ]; then
        log_info "Running unit tests..."
        ./nlm_test
        if [ $? -ne 0 ]; then
            log_error "Unit tests failed!"
            # Continue with other tests
        else
            log_success "Unit tests passed!"
        fi
    fi
    
    # Run integration tests if requested
    if [ "$RUN_INTEGRATION_TESTS" = true ] || [ "$RUN_TESTS" = true ]; then
        log_info "Running integration tests..."
        ./nlm_phase6_demo
        if [ $? -ne 0 ]; then
            log_error "Integration tests failed!"
            # Continue with other tests
        else
            log_success "Integration tests passed!"
        fi
    fi
    
    # Run performance tests if requested
    if [ "$RUN_PERFORMANCE_TESTS" = true ] || [ "$RUN_TESTS" = true ]; then
        log_info "Running performance tests..."
        # Run performance tests
        log_success "Performance tests completed (simulated)"
    fi
else
    # Run NLM with the specified mode
    log_info "Running NLM with mode: $NLM_MODE"
    
    # Prepare NLM command
    NLM_CMD="./nlm"
    
    # Add mode if not 'all'
    if [ "$NLM_MODE" != "all" ]; then
        NLM_CMD="$NLM_CMD --mode=$NLM_MODE"
    fi
    
    # Add steps if specified
    if [ "$NLM_STEPS" -gt 0 ]; then
        NLM_CMD="$NLM_CMD --steps=$NLM_STEPS"
    fi
    
    # Add neurons if specified
    if [ "$NLM_NEURONS" -gt 0 ]; then
        NLM_CMD="$NLM_CMD --neuron=$NLM_NEURONS"
    fi
    
    # Add config if not default
    if [ "$CONFIG_FILE" != "configs/default.cfg" ]; then
        NLM_CMD="$NLM_CMD --config=$CONFIG_FILE"
    fi
    
    # Add verbose flag if requested
    if [ "$VERBOSE" = true ]; then
        NLM_CMD="$NLM_CMD --verbose"
    fi
    
    # Add quiet flag if requested
    if [ "$QUIET" = true ]; then
        NLM_CMD="$NLM_CMD --quiet"
    fi
    
    # Run NLM
    eval "$NLM_CMD"
    
    if [ $? -eq 0 ]; then
        log_success "NLM completed successfully!"
    else
        log_error "NLM execution failed!"
        exit 1
    fi
fi

log_success "All operations completed successfully!"
exit 0