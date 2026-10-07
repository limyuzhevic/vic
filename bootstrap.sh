#!/bin/bash

# NLM Build Bootstrap Script
# Sets up development environment and builds the project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="NLM"
DEFAULT_BUILD_TYPE="Release"
DEFAULT_PYTHON_VERSION="3.10"

# Default configuration
BUILD_TYPE="$DEFAULT_BUILD_TYPE"
PYTHON_VERSION="$DEFAULT_PYTHON_VERSION"
BUILD_TESTS="ON"
BUILD_PYTHON="ON"
ENABLE_PROFILING="OFF"
ENABLE_COVERAGE="OFF"
CONFIG_FILE=""

# Print usage
usage() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --build-type TYPE        Build type (Debug, Release, RelWithDebInfo)"
    echo "  --python-version VERSION Python version (default: $PYTHON_VERSION)"
    echo "  --tests                 Enable/disable tests (default: enabled)"
    echo "  --python-bindings        Enable/disable Python bindings (default: enabled)"
    echo "  --profiling              Enable performance profiling (default: disabled)"
    echo "  --coverage               Enable code coverage (default: disabled)"
    echo "  --docs                   Build documentation (default: disabled)"
    echo "  --config FILE            Configuration file (JSON or simple format)"
    echo "  --help                  Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --build-type Debug --tests ON --python-bindings ON"
    echo "  $0 --build-type Release --coverage ON --profiling ON"
    echo "  $0 --config configs/debug.json"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --python-version)
            PYTHON_VERSION="$2"
            shift 2
            ;;
        --tests)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                BUILD_TESTS="$2"
                shift 2
            else
                echo "${RED}Error: --tests requires ON or OFF${NC}"
                exit 1
            fi
            ;;
        --python-bindings)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                BUILD_PYTHON="$2"
                shift 2
            else
                echo "${RED}Error: --python-bindings requires ON or OFF${NC}"
                exit 1
            fi
            ;;
        --profiling)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                ENABLE_PROFILING="$2"
                shift 2
            else
                echo "${RED}Error: --profiling requires ON or OFF${NC}"
                exit 1
            fi
            ;;
        --coverage)
            if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                ENABLE_COVERAGE="$2"
                shift 2
            else
                echo "${RED}Error: --coverage requires ON or OFF${NC}"
                exit 1
            fi
            ;;
        --docs)
            BUILD_DOCS="ON"
            shift
            ;;
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --help)
            usage
            exit 0
            ;;
        *)
            echo "${RED}Unknown option: $1${NC}"
            usage
            exit 1
            ;;
    esac

done

# Print configuration
function print_config() {
    echo "${BLUE}NLM Build Configuration:${NC}"
    echo "  Build type: $BUILD_TYPE"
    echo "  Python version: $PYTHON_VERSION"
    echo "  Tests: $BUILD_TESTS"
    echo "  Python bindings: $BUILD_PYTHON"
    echo "  Performance profiling: $ENABLE_PROFILING"
    echo "  Code coverage: $ENABLE_COVERAGE"
    echo "  Documentation: $BUILD_DOCS"
    if [[ -n "$CONFIG_FILE" ]]; then
        echo "  Configuration file: $CONFIG_FILE"
    fi
    echo ""
}

# Check system dependencies
function check_dependencies() {
    echo "${BLUE}Checking system dependencies...${NC}"
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        echo "${RED}CMake not found. Please install CMake >= 3.16${NC}"
        exit 1
    fi
    
    cmake_version=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+' | head -n1)
    cmake_major_version=$(echo $cmake_version | cut -d. -f1)
    
    if [[ $cmake_major_version -lt 3 ]]; then
        echo "${RED}CMake version $cmake_version is too old. Please install CMake >= 3.16${NC}"
        exit 1
    fi
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null; then
        echo "${RED}g++ compiler not found. Please install a C++ compiler${NC}"
        exit 1
    fi
    
    # Check Python
    if ! command -v python3 &> /dev/null; then
        echo "${RED}Python3 not found. Please install Python3${NC}"
        exit 1
    fi
    
    python_version=$(python3 --version 2>&1 | grep -o '[0-9]\+\.[0-9]\+')
    python_major_version=$(echo $python_version | cut -d. -f1)
    
    if [[ $python_major_version -lt 3 ]]; then
        echo "${RED}Python version $python_version is too old. Please install Python >= 3.8${NC}"
        exit 1
    fi
    
    echo "${GREEN}All dependencies found.${NC}"
}

# Install Python dependencies
function install_python_deps() {
    echo "${BLUE}Installing Python dependencies...${NC}"
    
    if [[ -f "requirements.txt" ]]; then
        pip3 install -r requirements.txt
        echo "${GREEN}Python dependencies installed.${NC}"
    else
        echo "${YELLOW}No requirements.txt found. Skipping Python dependency installation.${NC}"
    fi
}

# Create build directory
function create_build_dir() {
    local build_dir="build"
    
    if [[ -d "$build_dir" ]]; then
        echo "${YELLOW}Build directory '$build_dir' already exists.${NC}"
        read -p "Do you want to remove it and recreate? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            rm -rf "$build_dir"
            mkdir "$build_dir"
            echo "${GREEN}Build directory removed and recreated.${NC}"
        else
            echo "${BLUE}Using existing build directory.${NC}"
        fi
    else
        mkdir "$build_dir"
        echo "${GREEN}Build directory created.${NC}"
    fi
    
    BUILD_DIR="$build_dir"
}

# Configure CMake
function configure_cmake() {
    echo "${BLUE}Configuring CMake...${NC}"
    
    cd "$BUILD_DIR"
    
    # Base CMake arguments
    local cmake_args="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    
    # Add options based on configuration
    if [[ "$BUILD_TESTS" == "ON" ]]; then
        cmake_args="$cmake_args -DBUILD_TESTS=ON"
    else
        cmake_args="$cmake_args -DBUILD_TESTS=OFF"
    fi
    
    if [[ "$BUILD_PYTHON" == "ON" ]]; then
        cmake_args="$cmake_args -DBUILD_PYTHON=ON"
        
        # Set Python executable
        if command -v python3 &> /dev/null; then
            cmake_args="$cmake_args -DPYTHON_EXECUTABLE=$(which python3)"
        fi
    else
        cmake_args="$cmake_args -DBUILD_PYTHON=OFF"
    fi
    
    if [[ "$ENABLE_PROFILING" == "ON" ]]; then
        cmake_args="$cmake_args -DENABLE_PROFILING=ON"
    fi
    
    if [[ "$ENABLE_COVERAGE" == "ON" ]]; then
        cmake_args="$cmake_args -DENABLE_COVERAGE=ON"
    fi
    
    if [[ "$BUILD_DOCS" == "ON" ]]; then
        cmake_args="$cmake_args -DBUILD_DOCS=ON"
    fi
    
    # Configuration file
    if [[ -n "$CONFIG_FILE" ]]; then
        if [[ "$CONFIG_FILE" == *.json ]]; then
            cmake_args="$cmake_args -DCONFIG_FILE=$CONFIG_FILE"
        elif [[ "$CONFIG_FILE" == *.cfg ]]; then
            # Convert simple config to JSON for CMake
            if command -v python3 &> /dev/null; then
                python3 -c "
import json
import sys
config = {}
with open('$CONFIG_FILE', 'r') as f:
    for line in f:
        line = line.strip()
        if line and not line.startswith('#') and '=' in line:
            key, value = line.split('=', 1)
            key = key.strip()
            value = value.strip()
            # Convert value to appropriate type
            if value.lower() == 'true':
                value = True
            elif value.lower() == 'false':
                value = False
            elif value.isdigit():
                value = int(value)
            else:
                try:
                    value = float(value)
                except ValueError:
                    pass
            config[key] = value

json_config = json.dumps(config, indent=2)
with open('$CONFIG_FILE.json', 'w') as f:
    f.write(json_config)
"
                CONFIG_FILE="$CONFIG_FILE.json"
            fi
            cmake_args="$cmake_args -DCONFIG_FILE=$CONFIG_FILE"
        fi
    fi
    
    # Run CMake
    echo "Running: cmake $cmake_args .."
    if ! cmake $cmake_args ..; then
        echo "${RED}CMake configuration failed.${NC}"
        exit 1
    fi
    
    echo "${GREEN}CMake configuration completed.${NC}"
}

# Build project
function build_project() {
    echo "${BLUE}Building project...${NC}"
    
    cd "$BUILD_DIR"
    
    # Build with parallel jobs
    local cpu_count=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    echo "Running: make -j$cpu_count"
    if ! make -j$cpu_count; then
        echo "${RED}Build failed.${NC}"
        exit 1
    fi
    
    echo "${GREEN}Build completed.${NC}"
}

# Install Python bindings
function install_python_bindings() {
    if [[ "$BUILD_PYTHON" == "ON" ]]; then
        echo "${BLUE}Installing Python bindings...${NC}"
        
        if [[ -d "$BUILD_DIR/python_modules" ]]; then
            # Copy Python modules to system location or create virtual environment
            local python_path="$BUILD_DIR/python_modules"
            
            if command -v python3 &> /dev/null; then
                # Install using pip
                pip3 install --user -e "$BUILD_DIR/python_modules"
                echo "${GREEN}Python bindings installed.${NC}"
            fi
        else
            echo "${YELLOW}Python bindings not found. Skipping installation.${NC}"
        fi
    else
        echo "${BLUE}Python bindings not enabled. Skipping installation.${NC}"
    fi
}

# Run tests
function run_tests() {
    if [[ "$BUILD_TESTS" == "ON" ]]; then
        echo "${BLUE}Running tests...${NC}"
        
        cd "$BUILD_DIR"
        
        # Run unit tests
        if [[ -x "./nlm_test" ]]; then
            echo "Running unit tests..."
            if ! ./nlm_test; then
                echo "${YELLOW}Unit tests failed, but continuing...${NC}"
            fi
        else
            echo "${YELLOW}Unit test executable not found.${NC}"
        fi
        
        # Run integration tests
        if [[ -x "./nlm_phase6_demo" ]]; then
            echo "Running integration tests..."
            if ! ./nlm_phase6_demo; then
                echo "${YELLOW}Integration tests failed, but continuing...${NC}"
            fi
        else
            echo "${YELLOW}Integration test executable not found.${NC}"
        fi
        
        # Run performance benchmarks
        if [[ -x "./benchmark" ]]; then
            echo "Running performance benchmarks..."
            if ! ./benchmark; then
                echo "${YELLOW}Performance benchmarks failed, but continuing...${NC}"
            fi
        else
            echo "${YELLOW}Benchmark executable not found.${NC}"
        fi
        
        # Run Python tests
        if [[ -d "../python/tests" ]]; then
            echo "Running Python tests..."
            if ! python3 -m pytest ../python/tests/ -v; then
                echo "${YELLOW}Python tests failed, but continuing...${NC}"
            fi
        else
            echo "${YELLOW}Python tests directory not found.${NC}"
        fi
        
        echo "${GREEN}All tests completed.${NC}"
    else
        echo "${BLUE}Tests disabled. Skipping test execution.${NC}"
    fi
}

# Build documentation
function build_docs() {
    if [[ "$BUILD_DOCS" == "ON" ]]; then
        echo "${BLUE}Building documentation...${NC}"
        
        cd "$BUILD_DIR"
        
        if [[ -x "make doc" ]]; then
            if ! make doc; then
                echo "${YELLOW}Documentation build failed, but continuing...${NC}"
            fi
        else
            echo "${YELLOW}Documentation target not found.${NC}"
        fi
        
        echo "${GREEN}Documentation build completed.${NC}"
    else
        echo "${BLUE}Documentation build disabled. Skipping.${NC}"
    fi
}

# Generate coverage report
function generate_coverage() {
    if [[ "$ENABLE_COVERAGE" == "ON" ]]; then
        echo "${BLUE}Generating coverage report...${NC}"
        
        cd "$BUILD_DIR"
        
        if [[ -x "make coverage" ]]; then
            if ! make coverage; then
                echo "${YELLOW}Coverage report generation failed.${NC}"
            fi
        else
            echo "${YELLOW}Coverage target not found.${NC}"
        fi
        
        echo "${GREEN}Coverage report generated.${NC}"
    else
        echo "${BLUE}Coverage reporting disabled. Skipping.${NC}"
    fi
}

# Print build summary
function print_summary() {
    echo "${BLUE}${PROJECT_NAME} Build Summary${NC}"
    echo "========================="
    echo ""
    echo "Build directory: $BUILD_DIR"
    echo "Build type: $BUILD_TYPE"
    echo "Tests: $BUILD_TESTS"
    echo "Python bindings: $BUILD_PYTHON"
    echo "Performance profiling: $ENABLE_PROFILING"
    echo "Code coverage: $ENABLE_COVERAGE"
    echo "Documentation: $BUILD_DOCS"
    echo ""
    
    echo "Executable locations:"
    echo "  Main executable: $BUILD_DIR/nlm"
    echo "  Phase 3 demo: $BUILD_DIR/nlm_phase3_demo"
    echo "  Phase 4 demo: $BUILD_DIR/nlm_phase4_demo"
    echo "  Benchmark: $BUILD_DIR/benchmark"
    echo ""
    
    if [[ -d "$BUILD_DIR/python_modules" ]]; then
        echo "Python modules: $BUILD_DIR/python_modules"
    fi
    
    echo "Test executables:"
    echo "  Unit tests: $BUILD_DIR/nlm_test"
    echo ""
    
    if [[ "$BUILD_TESTS" == "ON" ]]; then
        echo "To run tests:"
        echo "  cd $BUILD_DIR"
        echo "  ./nlm_test                    # Run unit tests"
        echo "  ./nlm_phase6_demo              # Run integration tests"
        echo "  ./benchmark                   # Run performance benchmarks"
        echo ""
    fi
    
    echo "To run the demo:"
    echo "  cd $BUILD_DIR"
    echo "  ./nlm_phase6_demo"
    echo ""
    
    echo "To run benchmarks:"
    echo "  cd $BUILD_DIR"
    echo "  ./benchmark"
    echo ""
    
    echo "To install Python bindings:"
    echo "  pip3 install -e $BUILD_DIR/python_modules"
    echo ""
    
    echo "To build documentation:"
    echo "  cd $BUILD_DIR"
    echo "  make doc"
    echo ""
    
    echo "To generate coverage report:"
    echo "  cd $BUILD_DIR"
    echo "  make coverage"
}

# Main function
function main() {
    echo "${BLUE}=== $PROJECT_NAME Build Script ===${NC}"
    echo ""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --build-type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            --python-version)
                PYTHON_VERSION="$2"
                shift 2
                ;;
            --tests)
                if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                    BUILD_TESTS="$2"
                    shift 2
                else
                    echo "${RED}Error: --tests requires ON or OFF${NC}"
                    exit 1
                fi
                ;;
            --python-bindings)
                if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                    BUILD_PYTHON="$2"
                    shift 2
                else
                    echo "${RED}Error: --python-bindings requires ON or OFF${NC}"
                    exit 1
                fi
                ;;
            --profiling)
                if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                    ENABLE_PROFILING="$2"
                    shift 2
                else
                    echo "${RED}Error: --profiling requires ON or OFF${NC}"
                    exit 1
                fi
                ;;
            --coverage)
                if [[ "$2" == "ON" || "$2" == "OFF" ]]; then
                    ENABLE_COVERAGE="$2"
                    shift 2
                else
                    echo "${RED}Error: --coverage requires ON or OFF${NC}"
                    exit 1
                fi
                ;;
            --docs)
                BUILD_DOCS="ON"
                shift
                ;;
            --config)
                CONFIG_FILE="$2"
                shift 2
                ;;
            --help)
                usage
                exit 0
                ;;
            *)
                echo "${RED}Unknown option: $1${NC}"
                usage
                exit 1
                ;;
        esac
    done
    
    # Print configuration
    print_config
    
    # Check dependencies
    check_dependencies
    
    # Install Python dependencies
    install_python_deps
    
    # Create build directory
    create_build_dir
    
    # Configure CMake
    configure_cmake
    
    # Build project
    build_project
    
    # Install Python bindings
    install_python_bindings
    
    # Run tests
    run_tests
    
    # Build documentation
    build_docs
    
    # Generate coverage report
    generate_coverage
    
    # Print summary
    print_summary
}

# Run main function
main "$@"