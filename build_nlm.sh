#!/bin/bash

# NLM Build Script
# Automated build and development helper for the NLM (熙然) neural simulation project

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
DEFAULT_BUILD_TYPE="Release"
DEFAULT_PYTHON_BINDINGS="OFF"
DEFAULT_TEST_COVERAGE="OFF"
DEFAULT_INSTALL_PREFIX="/usr/local"

# Print usage information
usage() {
    cat << EOF
NLM Build Script
================

Usage: $0 [OPTIONS]

Options:
    --build-type TYPE           Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)
    --python-bindings [ON|OFF]  Enable Python bindings (default: OFF)
    --tests                     Build and run tests (default: OFF)
    --install PREFIX           Install to PREFIX (default: /usr/local)
    --help                      Show this help message

Examples:
    $0 --build-type Release --python-bindings ON
    $0 --tests
EOF
}

# Print error message and exit
error_exit() {
    echo -e "${RED}Error: $1${NC}" >&2
    exit 1
}

# Check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required tools
check_requirements() {
    echo -e "${BLUE}Checking requirements...${NC}"
    
    local missing=0
    
    if ! command_exists cmake; then
        echo -e "${RED}CMake not found. Please install CMake 3.16 or later.${NC}"
        missing=1
    else
        local cmake_version=$(cmake --version | head -1 | awk '{print $3}')
        local cmake_major=$(echo $cmake_version | cut -d. -f1)
        local cmake_minor=$(echo $cmake_version | cut -d. -f2)
        
        if [ $cmake_major -lt 3 ] || ([ $cmake_major -eq 3 ] && [ $cmake_minor -lt 16 ]); then
            echo -e "${RED}CMake $cmake_version found. CMake 3.16+ is required.${NC}"
            missing=1
        else
            echo -e "${GREEN}CMake $cmake_version found.${NC}"
        fi
    fi
    
    if ! command_exists make; then
        echo -e "${RED}Make not found. Please install make.${NC}"
        missing=1
    else
        echo -e "${GREEN}Make found.${NC}"
    fi
    
    # Check for C++ compiler
    if ! g++ --version >/dev/null 2>&1; then
        echo -e "${RED}C++ compiler not found. Please install g++.${NC}"
        missing=1
    else
        echo -e "${GREEN}C++ compiler found.${NC}"
    fi
    
    # Check for Python if Python bindings are requested
    if [ "$PYTHON_BINDINGS" = "ON" ]; then
        if ! command_exists python3; then
            echo -e "${RED}Python3 not found. Python bindings require Python 3.8+.${NC}"
            missing=1
        else
            echo -e "${GREEN}Python3 found.${NC}"
        fi
        
        if ! command_exists pip3; then
            echo -e "${YELLOW}pip3 not found. Python dependencies may not install correctly.${NC}"
        else
            echo -e "${GREEN}pip3 found.${NC}"
        fi
    fi
    
    if [ $missing -ne 0 ]; then
        echo -e "${RED}Missing required dependencies. Please install missing packages.${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}All requirements satisfied.${NC}"
}

# Configure the build
configure_build() {
    local build_dir="build"
    local cmake_args=""
    
    echo -e "${BLUE}Configuring build in '$build_dir'...${NC}"
    
    # Set CMake build type
    case "$BUILD_TYPE" in
        "Debug"|"Release"|"RelWithDebInfo"|"MinSizeRel")
            cmake_args="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
            ;;
        *)
            echo -e "${YELLOW}Unknown build type '$BUILD_TYPE', using Release.${NC}"
            cmake_args="-DCMAKE_BUILD_TYPE=Release"
            ;;
    esac
    
    # Add Python bindings option
    if [ "$PYTHON_BINDINGS" = "ON" ]; then
        cmake_args="$cmake_args -DNLM_BUILD_PYTHON_BINDINGS=ON"
    else
        cmake_args="$cmake_args -DNLM_BUILD_PYTHON_BINDINGS=OFF"
    fi
    
    # Add Python executable
    if command_exists python3; then
        cmake_args="$cmake_args -DPYTHON_EXECUTABLE=$(which python3)"
    fi
    
    # Create build directory if it doesn't exist
    mkdir -p "$build_dir"
    cd "$build_dir" || error_exit "Cannot enter build directory"
    
    # Run CMake configuration
    echo -e "${BLUE}Running CMake configuration...${NC}"
    cmake $cmake_args .. || error_exit "CMake configuration failed"
    
    echo -e "${GREEN}CMake configuration completed.${NC}"
}

# Build the project
build_project() {
    local build_dir="build"
    local num_jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    echo -e "${BLUE}Building project with $num_jobs parallel jobs...${NC}"
    
    # Run make
    make -j$num_jobs || error_exit "Build failed"
    
    echo -e "${GREEN}Build completed successfully.${NC}"
}

# Run tests
run_tests() {
    local build_dir="build"
    
    if [ ! -f "$build_dir/nlm_test" ]; then
        echo -e "${YELLOW}Test executable not found. Building first...${NC}"
        build_project
    fi
    
    echo -e "${BLUE}Running tests...${NC}"
    
    cd "$build_dir" || error_exit "Cannot enter build directory"
    
    # Run the test executable
    ./nlm_test
    local test_result=$?
    
    # Run ctest if available
    if command_exists ctest; then
        echo -e "${BLUE}Running ctest...${NC}"
        ctest --output-on-failure
        local ctest_result=$?
        
        if [ $ctest_result -ne 0 ]; then
            return $ctest_result
        fi
    fi
    
    return $test_result
}

# Install the project
install_project() {
    local build_dir="build"
    local install_prefix="${INSTALL_PREFIX:-$DEFAULT_INSTALL_PREFIX}"
    
    echo -e "${BLUE}Installing to '$install_prefix'...${NC}"
    
    cd "$build_dir" || error_exit "Cannot enter build directory"
    
    # Run make install
    make install DESTDIR="$install_prefix" || error_exit "Installation failed"
    
    echo -e "${GREEN}Installation completed.${NC}"
}

# Build Python bindings
build_python_bindings() {
    local python_dir="python"
    
    echo -e "${BLUE}Building Python bindings...${NC}"
    
    if [ ! -d "$python_dir" ]; then
        echo -e "${RED}Python directory not found. Cannot build Python bindings.${NC}"
        return 1
    fi
    
    cd "$python_dir" || error_exit "Cannot enter python directory"
    
    # Check for pyproject.toml or setup.py
    if [ -f "pyproject.toml" ]; then
        echo -e "${BLUE}Building with pyproject.toml...${NC}"
        pip3 install -e . || error_exit "Python package installation failed"
    elif [ -f "setup.py" ]; then
        echo -e "${BLUE}Building with setup.py...${NC}"
        python3 setup.py build_ext --inplace || error_exit "Python extension build failed"
    else
        echo -e "${RED}No Python build configuration found.${NC}"
        return 1
    fi
    
    cd - >/dev/null || error_exit "Cannot return to original directory"
    
    echo -e "${GREEN}Python bindings built successfully.${NC}"
}

# Show build status
show_status() {
    local build_dir="build"
    
    if [ ! -d "$build_dir" ]; then
        echo -e "${YELLOW}Build directory not found.${NC}"
        return
    fi
    
    cd "$build_dir" || error_exit "Cannot enter build directory"
    
    echo -e "${BLUE}Build status:${NC}"
    echo ""
    
    # Check for executables
    local executables=(
        "nlm: Phase 2 neural computation demo"
        "nlm_phase3_demo: Phase 3 world interaction demo"
        "nlm_phase4_demo: Phase 4 cognitive integration demo"
        "nlm_phase5_demo: Phase 5 performance optimizations demo"
        "nlm_phase6_demo: Phase 6 final integration demo"
        "nlm_benchmark: Performance benchmark"
    )
    
    for exe in "${executables[@]}"; do
        local name=${exe%%:*}
        local desc=${exe#*:}
        
        if [ -f "$name" ]; then
            local size=$(stat -c%s "$name" 2>/dev/null || stat -f%z "$name" 2>/dev/null || echo "unknown")
            echo -e "  ${GREEN}✓${NC} $name ($size bytes) - $desc"
        else
            echo -e "  ${RED}✗${NC} $name - Not built"
        fi
    done
    
    echo ""
    
    # Check for test executable
    if [ -f "nlm_test" ]; then
        echo -e "  ${GREEN}✓${NC} nlm_test (unit tests)"
    else
        echo -e "  ${RED}✗${NC} nlm_test (not built)"
    fi
    
    # Check for library files
    if ls lib*.a 1>/dev/null 2>&1; then
        echo -e "  ${GREEN}✓${NC} Static libraries (.a files)"
    fi
    
    if ls lib*.so 1>/dev/null 2>&1; then
        echo -e "  ${GREEN}✓${NC} Shared libraries (.so files)"
    fi
    
    # Check for Python module
    local python_module="${PYTHON_MODULE:-pynlm}"
    if [ -f "src/${python_module}.so" ] || [ -f "src/${python_module}.pyd" ] || [ -d "src/${python_module}.egg-link" ]; then
        echo -e "  ${GREEN}✓${NC} Python module ($python_module)"
    else
        echo -e "  ${RED}✗${NC} Python module ($python_module) - Not built"
    fi
    
    cd - >/dev/null || error_exit "Cannot return to original directory"
}

# Main script
main() {
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --build-type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            --python-bindings)
                PYTHON_BINDINGS="$2"
                shift 2
                ;;
            --tests)
                RUN_TESTS=1
                shift
                ;;
            --install)
                INSTALL_PREFIX="$2"
                shift 2
                ;;
            --help)
                usage
                exit 0
                ;;
            *)
                echo -e "${RED}Unknown option: $1${NC}"
                usage
                exit 1
                ;;
        esac
    done
    
    # Set defaults
    BUILD_TYPE="${BUILD_TYPE:-$DEFAULT_BUILD_TYPE}"
    PYTHON_BINDINGS="${PYTHON_BINDINGS:-$DEFAULT_PYTHON_BINDINGS}"
    INSTALL_PREFIX="${INSTALL_PREFIX:-$DEFAULT_INSTALL_PREFIX}"
    
    # Show status if no actions specified
    if [ -z "$RUN_TESTS" ] && [ ! -d "build" ]; then
        show_status
        exit 0
    fi
    
    # Check requirements
    check_requirements
    
    # Configure, build, and optionally run tests
    configure_build
    build_project
    
    if [ -n "$RUN_TESTS" ]; then
        if ! run_tests; then
            error_exit "Tests failed"
        fi
    fi
    
    # Build Python bindings if requested
    if [ "$PYTHON_BINDINGS" = "ON" ]; then
        build_python_bindings
    fi
}

# Run main function with all arguments
main "$@"
