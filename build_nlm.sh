#!/usr/bin/env bash
# NLM Build Automation Script
# This script simplifies the build process for NLM

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to get system architecture
print_system_info() {
    print_info "System Information:"
    if command_exists "uname"; then
        echo "  OS: $(uname -s)"
        echo "  Architecture: $(uname -m)"
        echo "  Kernel: $(uname -r)"
    fi
    
    if command_exists "python3"; then
        echo "  Python: $(python3 --version)"
    fi
    
    if command_exists "cmake"; then
        echo "  CMake: $(cmake --version | head -n1)"
    fi
    
    if command_exists "g++"; then
        echo "  GCC: $(g++ --version | head -n1)"
    elif command_exists "clang++"; then
        echo "  Clang: $(clang++ --version | head -n1 | head -n1)"
    fi
}

# Function to check build dependencies
check_dependencies() {
    print_info "Checking build dependencies..."
    
    # Check for C++ compiler
    if command_exists "g++"; then
        print_info "Found GCC compiler"
    elif command_exists "clang++"; then
        print_info "Found Clang compiler"
    else
        print_error "No C++ compiler found. Please install g++ or clang++."
        return 1
    fi
    
    # Check for CMake
    if command_exists "cmake"; then
        version=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+')
        if [[ $(echo "$version >= 3.16" | bc -l) -eq 1 ]]; then
            print_info "Found CMake $version (requirements met)"
        else
            print_warning "CMake version $version may be too old. Version 3.16+ recommended."
        fi
    else
        print_error "CMake not found. Please install CMake >= 3.16."
        return 1
    fi
    
    # Check for Python
    if command_exists "python3"; then
        print_info "Found Python $(python3 --version 2>&1 | cut -d' ' -f2)"
    else
        print_warning "Python3 not found. Python bindings may not work."
    fi
    
    # Check for build directory
    if [ ! -d "build" ]; then
        print_info "Creating build directory..."
        mkdir -p build
    else
        print_info "Build directory already exists"
    fi
    
    return 0
}

# Function to configure CMake
configure_cmake() {
    print_info "Configuring CMake..."
    
    local build_type="Release"
    if [ "$1" = "debug" ]; then
        build_type="Debug"
        print_info "Building in Debug mode"
    fi
    
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=$build_type \
              -DPYTHON_EXECUTABLE=$(which python3 || which python 2>/dev/null || echo "")
    
    if [ $? -eq 0 ]; then
        print_info "CMake configuration successful"
        return 0
    else
        print_error "CMake configuration failed"
        return 1
    fi
}

# Function to build NLM
build_nlm() {
    print_info "Building NLM..."
    
    cd build
    
    # Use all available CPU cores
    local cpu_cores=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo "4")
    print_info "Using $cpu_cores CPU cores for parallel build"
    
    make -j$cpu_cores
    
    if [ $? -eq 0 ]; then
        print_info "Build successful!"
        return 0
    else
        print_error "Build failed"
        return 1
    fi
}

# Function to build Python bindings
build_python_bindings() {
    print_info "Building Python bindings..."
    
    if [ ! -f "pyproject.toml" ]; then
        print_warning "pyproject.toml not found. Python bindings may not be available."
        return 0
    fi
    
    cd build
    
    # Try to install via pip
    if command_exists "pip3"; then
        print_info "Installing via pip..."
        pip3 install -e ..
    elif command_exists "pip"; then
        print_info "Installing via pip..."
        pip install -e ..
    else
        print_warning "pip not found. Skipping Python bindings installation."
    fi
}

# Function to run tests
run_tests() {
    print_info "Running tests..."
    
    cd build
    
    if [ -f "CTestTestfile.cmake" ]; then
        ctest --output-on-failure
        
        if [ $? -eq 0 ]; then
            print_info "All tests passed!"
            return 0
        else
            print_error "Some tests failed"
            return 1
        fi
    else
        print_warning "No tests found"
        return 0
    fi
}

# Function to run demos
run_demos() {
    print_info "Running demonstration programs..."
    
    cd build
    
    # Check if demo executables exist
    if [ -f "./nlm_phase6_demo" ]; then
        print_info "Running Phase 6 integration demo..."
        ./nlm_phase6_demo
        
        if [ $? -eq 0 ]; then
            print_info "Demo completed successfully"
        else
            print_error "Demo failed"
            return 1
        fi
    else
        print_warning "Phase 6 demo not found"
    fi
    
    if [ -f "./nlm_phase3_demo" ]; then
        print_info "Running Phase 3 demo..."
        ./nlm_phase3_demo
        
        if [ $? -eq 0 ]; then
            print_info "Phase 3 demo completed successfully"
        else
            print_error "Phase 3 demo failed"
            return 1
        fi
    else
        print_warning "Phase 3 demo not found"
    fi
}

# Function to install NLM
install_nlm() {
    print_info "Installing NLM..."
    
    cd build
    
    if [ -f "Makefile" ]; then
        make install
        
        if [ $? -eq 0 ]; then
            print_info "Installation successful"
            return 0
        else
            print_error "Installation failed"
            return 1
        fi
    else
        print_error "Makefile not found. Cannot install."
        return 1
    fi
}

# Function to clean build
 clean_build() {
    print_info "Cleaning build directory..."
    
    if [ -d "build" ]; then
        cd build
        make clean
        rm -rf *
        cd ..
        print_info "Build directory cleaned"
    else
        print_warning "Build directory not found"
    fi
}

# Function to show help
 show_help() {
    cat << EOF
NLM Build Automation Script
===========================

Usage: $0 [COMMAND] [OPTIONS]

Commands:
  build [debug]    Build NLM (use 'debug' for Debug build)
  python           Build and install Python bindings
  test             Run tests
  demo            Run demo programs
  install         Install NLM system-wide
  clean           Clean build directory
  info            Show system information
  help            Show this help message

Examples:
  $0                    # Quick build in Release mode
  $0 build debug        # Build in Debug mode
  $0 python             # Build Python bindings
  $0 test               # Run tests
  $0 clean              # Clean build directory

EOF
}

# Main script logic
case "${1:-}" in
    build)
        print_info "Starting NLM build process..."
        print_system_info
        check_dependencies
        configure_cmake "$2"
        build_nlm
        ;;
    python)
        check_dependencies
        build_python_bindings
        ;;
    test)
        check_dependencies
        run_tests
        ;;
    demo)
        check_dependencies
        build_nlm
        run_demos
        ;;
    install)
        check_dependencies
        build_nlm
        install_nlm
        ;;
    clean)
        clean_build
        ;;
    info)
        print_system_info
        ;;
    help|*)
        show_help
        ;;
    *)
        echo "Unknown command: $1"
        show_help
        exit 1
        ;;
esac

exit 0