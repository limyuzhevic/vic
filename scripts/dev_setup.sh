#!/bin/bash

# NLM Development Setup Script
# This script sets up the development environment for the NLM project

set -e  # Exit on any error

echo "=== NLM Development Environment Setup ==="

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

# Check for required dependencies
check_dependencies() {
    print_info "Checking dependencies..."
    
    local errors=0
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Please install CMake 3.20 or higher."
        errors=$((errors + 1))
    else
        local cmake_version=$(cmake --version | head -n 1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | cut -d. -f1)
        if [ "$cmake_version" -lt 3 ]; then
            print_error "CMake version $cmake_version is too old. Please install CMake 3.20 or higher."
            errors=$((errors + 1))
        else
            print_success "CMake found: $(cmake --version | head -n 1)"
        fi
    fi
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "Neither g++ nor clang++ found. Please install a C++ compiler."
        errors=$((errors + 1))
    else
        if command -v g++ &> /dev/null; then
            print_success "g++ found: $(g++ --version | head -n 1)"
        else
            print_success "clang++ found: $(clang++ --version | head -n 1)"
        fi
    fi
    
    # Check for Git
    if ! command -v git &> /dev/null; then
        print_error "Git not found. Please install Git."
        errors=$((errors + 1))
    else
        print_success "Git found: $(git --version)"
    fi
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        print_error "Python 3 not found. Please install Python 3.8 or higher."
        errors=$((errors + 1))
    else
        local python_version=$(python3 --version | grep -o '[0-9]\+\.[0-9]\+')
        if [ "$(echo "$python_version >= 3.8" | bc)" -eq 0 ]; then
            print_error "Python version $python_version is too old. Please install Python 3.8 or higher."
            errors=$((errors + 1))
        else
            print_success "Python 3 found: $(python3 --version)"
        fi
    fi
    
    if [ $errors -gt 0 ]; then
        print_error "$errors dependency check(s) failed. Please install missing dependencies."
        return 1
    fi
    
    print_success "All dependencies checked successfully."
}

# Setup build directory
setup_build_directory() {
    print_info "Setting up build directory..."
    
    if [ -d "build" ]; then
        print_info "Build directory already exists."
    else
        print_info "Creating build directory..."
        mkdir -p build
    fi
    
    if [ -d "build_debug" ]; then
        print_info "Debug build directory already exists."
    else
        print_info "Creating debug build directory..."
        mkdir -p build_debug
    fi
    
    print_success "Build directories ready."
}

# Configure CMake
configure_cmake() {
    print_info "Configuring CMake..."
    
    # Release build configuration
    if [ ! -f "build/CMakeCache.txt" ]; then
        print_info "Configuring Release build..."
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_DOCS=ON
        print_success "Release build configured."
    else
        print_info "Release build already configured."
    fi
    
    # Debug build configuration
    if [ ! -f "build_debug/CMakeCache.txt" ]; then
        print_info "Configuring Debug build with sanitizers..."
        cmake -S . -B build_debug -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON -DNLM_ENABLE_COVERAGE=ON -DBUILD_TESTS=ON
        print_success "Debug build configured."
    else
        print_info "Debug build already configured."
    fi
}

# Build the project
build_project() {
    print_info "Building project..."
    
    print_info "Building Release build..."
    cmake --build build --parallel
    print_success "Release build completed."
    
    print_info "Building Debug build..."
    cmake --build build_debug --parallel
    print_success "Debug build completed."
}

# Run tests
run_tests() {
    print_info "Running tests..."
    
    if [ -f "build/nlm_test" ]; then
        print_info "Running Release build tests..."
        cd build && ctest --output-on-failure
        cd ..
        print_success "Release build tests completed."
    fi
    
    if [ -f "build_debug/nlm_test" ]; then
        print_info "Running Debug build tests..."
        cd build_debug && ctest --output-on-failure
        cd ..
        print_success "Debug build tests completed."
    fi
}

# Generate documentation
generate_documentation() {
    print_info "Generating documentation..."
    
    if [ -f "build/docs" ]; then
        print_info "Generating API documentation..."
        cmake --build build --target docs
        print_success "Documentation generated in build/docs/html/"
    fi
}

# Setup Python environment
setup_python_environment() {
    print_info "Setting up Python environment..."
    
    if [ -f "pyproject.toml" ]; then
        print_info "Installing Python package in development mode..."
        pip install -e .[dev]
        print_success "Python package installed."
    fi
}

# Display build information
display_build_info() {
    print_info "Build Information:"
    if [ -f "build/CMakeCache.txt" ]; then
        echo "Release build:"
        grep -E "CMAKE_CXX_COMPILER|CMAKE_BUILD_TYPE|NLM_VERSION" build/CMakeCache.txt | head -10
    fi
    echo
    if [ -f "build_debug/CMakeCache.txt" ]; then
        echo "Debug build:"
        grep -E "CMAKE_CXX_COMPILER|CMAKE_BUILD_TYPE|NLM_VERSION" build_debug/CMakeCache.txt | head -10
    fi
}

# Main function
main() {
    print_info "Starting NLM Development Environment Setup..."
    
    check_dependencies
    setup_build_directory
    configure_cmake
    build_project
    run_tests
    generate_documentation
    setup_python_environment
    display_build_info
    
    print_success "Development environment setup completed successfully!"
    print_info "\nNext steps:"
    print_info "  - Use 'cd build && ./nlm' to run the main application"
    print_info "  - Use 'cd build && ./nlm_phase6_demo' to run the Phase 6 integration demo"
    print_info "  - Use 'cd build && ./nlm_test' to run tests"
    print_info "  - Open VS Code to debug the project"
    print_info "  - Use 'cmake --build build --target docs_open' to view documentation"
}

# Parse command line arguments
case "$1" in
    --debug)
        set -x
        ;;
    --help)
        echo "Usage: $0 [OPTIONS]"
        echo "Options:"
        echo "  --debug      Enable debug output"
        echo "  --help       Show this help message"
        echo "  --tests-only Run only tests"
        echo "  --no-build   Skip building"
        exit 0
        ;;
    --tests-only)
        main --skip-build
        run_tests
        exit 0
        ;;
    --no-build)
        shift
        main --skip-build
        return
        ;;
    *)
        main
        ;;
esac

# Handle --skip-build flag if passed as parameter
if [[ "$@" == *"--skip-build"* ]]; then
    main --skip-build
else
    main
fi

print_success "Development environment setup completed!"