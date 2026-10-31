#!/bin/bash
# NLM Build and Test Script
# This script provides a comprehensive build and testing framework for the NLM brain simulation project

set -e

cd "$(dirname "$0")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="${BUILD_DIR:-build}"
CMAKE_FLAGS="${CMAKE_FLAGS:-}"
TEST_TIMEOUT="300"

# Default configuration values
DEFAULT_NEURON_COUNT=500
DEFAULT_STEPS=1000
DEFAULT_REGION_COUNT=1
DEFAULT_CONNECTION_PROB=0.15

echo -e "${BLUE}╔════════════════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                    NLM Build and Test Script                             ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════════════════════╝${NC}"
echo

# Function to print status messages
print_status() {
    local status=$1
    local message=$2
    case $status in
        INFO)
            echo -e "${BLUE}[INFO]${NC} $message"
            ;;
        WARNING)
            echo -e "${YELLOW}[WARNING]${NC} $message"
            ;;
        ERROR)
            echo -e "${RED}[ERROR]${NC} $message"
            ;;
        SUCCESS)
            echo -e "${GREEN}[SUCCESS]${NC} $message"
            ;;
    esac
}

# Function to create build directory
create_build_dir() {
    if [ ! -d "$BUILD_DIR" ]; then
        print_status INFO "Creating build directory: $BUILD_DIR"
        mkdir -p "$BUILD_DIR"
    fi
    cd "$BUILD_DIR"
}

# Function to check if build directory exists
check_build_dir() {
    if [ ! -d "$BUILD_DIR" ]; then
        print_status ERROR "Build directory '$BUILD_DIR' does not exist. Run 'build' first."
        exit 1
    fi
    cd "$BUILD_DIR"
}

# Function to run CMake
run_cmake() {
    local build_type="${1:-Release}"
    print_status INFO "Configuring project with CMake ($build_type)..."
    
    cmake "$CMAKE_SOURCE_DIR" \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_CXX_STANDARD=20 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_CXX_EXTENSIONS=OFF \
        "$CMAKE_FLAGS"
}

# Function to run make
run_make() {
    local jobs="${1:--j$(nproc)}"
    print_status INFO "Building project with make $jobs..."
    
    make $jobs
}

# Function to run tests
run_tests() {
    local test_target="${1:-all}"
    print_status INFO "Running tests ($test_target)..."
    
    if [ "$test_target" = "all" ]; then
        ctest --output-on-failure --timeout "$TEST_TIMEOUT"
    else
        ctest --output-on-failure --timeout "$TEST_TIMEOUT" -R "$test_target"
    fi
}

# Function to run benchmarks
run_benchmarks() {
    print_status INFO "Running benchmarks..."
    
    if [ -f "./nlm" ]; then
        print_status INFO "Running main benchmark (500 neurons, 1000 steps)..."
        ./nlm --benchmark=500,1000
    fi
    
    if [ -f "./nlm_phase3_demo" ]; then
        print_status INFO "Running Phase 3 demo..."
        ./nlm_phase3_demo
    fi
    
    if [ -f "./nlm_phase4_demo" ]; then
        print_status INFO "Running Phase 4 demo..."
        ./nlm_phase4_demo
    fi
}

# Function to run Phase 6 demo
run_phase6_demo() {
    print_status INFO "Running Phase 6 integration demo..."
    
    if [ -f "./nlm_phase6_demo" ]; then
        ./nlm_phase6_demo
    elif [ -f "./build/nlm_phase6_demo" ]; then
        ./build/nlm_phase6_demo
    else
        print_status WARNING "Phase 6 demo executable not found"
    fi
}

# Function to build configurations
build_configurations() {
    local configs_dir="configs"
    
    if [ ! -d "$configs_dir" ]; then
        print_status WARNING "configs directory not found, skipping configuration builds"
        return
    fi
    
    print_status INFO "Building configurations..."
    
    for config_file in "$configs_dir"/*.cfg; do
        if [ -f "$config_file" ]; then
            local config_name="$(basename "$config_file" .cfg)"
            print_status INFO "Building configuration: $config_name"
            ./nlm --config "$config_file" 2>/dev/null || true
        fi
    done
}

# Function to generate documentation
build_docs() {
    print_status INFO "Building documentation..."
    
    # Check for Doxygen
    if command -v doxygen >/dev/null 2>&1; then
        if [ -f Doxyfile ]; then
            doxygen
        else
            print_status WARNING "Doxyfile not found, skipping documentation generation"
        fi
    else
        print_status WARNING "Doxygen not installed, skipping documentation generation"
    fi
}

# Function to clean build
clean_build() {
    if [ -d "$BUILD_DIR" ]; then
        print_status INFO "Cleaning build directory: $BUILD_DIR"
        cd "$BUILD_DIR"
        make clean
        cd ..
        rm -rf "$BUILD_DIR"/*
    else
        print_status INFO "Build directory $BUILD_DIR does not exist"
    fi
}

# Function to show help
function show_help() {
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo
    echo "Commands:"
    echo "  build        Configure and build the project"
    echo "  clean        Clean the build directory"
    echo "  test         Run all tests"
    echo "  benchmark    Run performance benchmarks"
    echo "  phase6       Run Phase 6 integration demo"
    echo "  docs         Build documentation"
    echo "  config       Build various configuration files"
    echo "  setup        Setup development environment"
    echo
    echo "Options:"
    echo "  --build-dir DIR      Set build directory (default: build)"
    echo "  --build-type TYPE    Set build type: Debug, Release, RelWithDebInfo (default: Release)"
    echo "  --cmake-flags FLAGS  Additional CMake flags"
    echo "  --jobs N             Number of parallel jobs (default: number of CPU cores)"
    echo "  --test-pattern PATTERN Run tests matching pattern"
    echo "  --help               Show this help message"
    echo
    echo "Examples:"
    echo "  $0 build --build-dir=build --build-type=Debug"
    echo "  $0 test --test-pattern=Phase3 --jobs=4"
    echo "  $0 benchmark"
    echo "  $0 phase6"
    echo
}

# Parse command line arguments
COMMAND="${1:-build}"
shift || true

while [ $# -gt 0 ]; do
    case "$1" in
        --build-dir=*)
            BUILD_DIR="${1#*=}"
            shift
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --build-type=*)
            BUILD_TYPE="${1#*=}"
            shift
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --cmake-flags=*)
            CMAKE_FLAGS="${CMAKE_FLAGS} ${1#*=}"
            shift
            ;;
        --cmake-flags)
            CMAKE_FLAGS="${CMAKE_FLAGS} $2"
            shift 2
            ;;
        --jobs=*)
            JOBS="${1#*=}"
            shift
            ;;
        --jobs)
            JOBS="$2"
            shift 2
            ;;
        --test-pattern=*)
            TEST_PATTERN="${1#*=}"
            shift
            ;;
        --test-pattern)
            TEST_PATTERN="$2"
            shift 2
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
    shift

done

# Execute command
create_build_dir

"$(printf 'case "%s" in
    build)
        run_cmake "%s"
        run_make "%s"
        ;;
    clean)
        clean_build
        ;;
    test)
        if [ -n "$TEST_PATTERN" ]; then
            run_tests "$TEST_PATTERN"
        else
            run_tests "all"
        fi
        ;;
    benchmark)
        run_benchmarks
        ;;
    phase6)
        run_phase6_demo
        ;;
    docs)
        build_docs
        ;;
    config)
        build_configurations
        ;;
    setup)
        echo "Setting up development environment..."
        if ! command -v cmake >/dev/null 2>&1; then
            echo "CMake not found. Installing..."
            # Install CMake (platform-specific)
        fi
        echo "Development environment setup complete."
        ;;
    *)
        echo -e "${RED}Unknown command: $COMMAND${NC}"
        show_help
        exit 1
        ;;
    esac
" | sed 's/\$BUILD_TYPE/"${BUILD_TYPE:-Release}"/g' | sed 's/\$JOBS/"${JOBS:--j$(nproc)}"/g')