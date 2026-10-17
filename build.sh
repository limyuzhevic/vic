#!/bin/bash
# NLM Enhanced Build and Test Script
# Comprehensive build, test, and documentation script for NLM Phase 6 Enhanced

# Configuration
BUILD_DIR="build"
PYTHON_BINDINGS="ON"
BUILD_TYPE="Release"
TEST_COVERAGE="OFF"
VERBOSE_BUILD="OFF"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print functions
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check prerequisites
check_prerequisites() {
    print_info "Checking prerequisites..."
    
    # Check CMake
    if ! command_exists cmake; then
        print_error "CMake is required but not installed."
        return 1
    fi
    
    # Check CMake version
    CMAKE_VERSION=$(cmake --version | head -1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]*')
    CMAKE_MAJOR=$(echo "$CMAKE_VERSION" | cut -d. -f1)
    CMAKE_MINOR=$(echo "$CMAKE_VERSION" | cut -d. -f2)
    
    if [[ $CMAKE_MAJOR -lt 3 || ($CMAKE_MAJOR -eq 3 && $CMAKE_MINOR -lt 16) ]]; then
        print_error "CMake version $CMAKE_VERSION is required. Minimum version is 3.16."
        return 1
    fi
    
    # Check C++ compiler
    if ! command_exists g++ && ! command_exists clang++; then
        print_error "C++ compiler (g++ or clang++) is required but not found."
        return 1
    fi
    
    # Check C++20 support
    if g++ --version >/dev/null 2>&1; then
        CXX_VERSION_OUTPUT=$(g++ -std=c++20 -E -x c++ /dev/null 2>&1 | head -1)
        if [[ "$CXX_VERSION_OUTPUT" != *"supported"* ]]; then
            print_warning "C++20 support not confirmed. Some features may not work."
        fi
    fi
    
    # Check Python if bindings are needed
    if [[ "$PYTHON_BINDINGS" == "ON" ]]; then
        if ! command_exists python3; then
            print_error "Python 3 is required for Python bindings."
            return 1
        fi
        
        # Check pybind11
        if ! python3 -c "import pybind11" 2>/dev/null; then
            print_warning "pybind11 not found. Installing..."
            pip3 install pybind11 scikit-build-core pytest numpy
        fi
    fi
    
    print_success "All prerequisites checked."
    return 0
}

# Configure build
configure_build() {
    print_info "Configuring NLM build with CMake..."
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure CMake
    CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    
    if [[ "$PYTHON_BINDINGS" == "ON" ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DPYTHON_EXECUTABLE=$(which python3)"
    fi
    
    if [[ "$VERBOSE_BUILD" == "ON" ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
    fi
    
    if [[ "$TEST_COVERAGE" == "ON" ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DENABLE_COVERAGE=ON"
    fi
    
    # Run CMake
    print_info "Running CMake configuration..."
    cmake $CMAKE_ARGS ..
    
    if [[ $? -ne 0 ]]; then
        print_error "CMake configuration failed."
        return 1
    fi
    
    print_success "CMake configuration completed."
    return 0
}

# Build NLM
build_nlm() {
    print_info "Building NLM..."
    
    # Build with parallel jobs
    local jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    print_info "Building with $jobs parallel jobs..."
    make -j$jobs
    
    if [[ $? -ne 0 ]]; then
        print_error "Build failed."
        return 1
    fi
    
    print_success "NLM build completed successfully."
    return 0
}

# Install Python bindings
install_python_bindings() {
    if [[ "$PYTHON_BINDINGS" == "ON" ]]; then
        print_info "Installing Python bindings..."
        
        # Check if we're in the right directory
        if [[ ! -f "../pyproject.toml" && ! -f "../setup.py" && ! -f "../CMakeLists.txt" ]]; then
            print_error "Python bindings directory not found. Skipping Python bindings installation."
            return 0
        fi
        
        # Install via pip
        cd ..
        print_info "Running pip install..."
        pip3 install -e . 2>&1 | tail -20
        
        if [[ $? -ne 0 ]]; then
            print_error "Python bindings installation failed."
            return 1
        fi
        
        print_success "Python bindings installed successfully."
        return 0
    else
        print_info "Skipping Python bindings installation (disabled)."
        return 0
    fi
}

# Run unit tests
run_tests() {
    print_info "Running unit tests..."
    
    # Run ctest
    if command_exists ctest; then
        print_info "Running C++ unit tests..."
        ctest --output-on-failure
        
        if [[ $? -ne 0 ]]; then
            print_error "Some C++ tests failed."
            return 1
        fi
        
        print_success "All C++ tests passed."
    else
        print_warning "ctest not found. Skipping C++ tests."
    fi
    
    # Run Python tests if available
    if [[ -f "../test.py" || -f "../setup.py" ]]; then
        print_info "Running Python tests..."
        cd ..
        python3 -m pytest test.py -v
        
        if [[ $? -ne 0 ]]; then
            print_error "Some Python tests failed."
            return 1
        fi
        
        print_success "All Python tests passed."
        cd "$BUILD_DIR"
    else
        print_warning "No Python test files found. Skipping Python tests."
    fi
    
    return 0
}

# Run NLM demos and examples
run_demos() {
    print_info "Running NLM demos and examples..."
    
    cd "$BUILD_DIR"
    
    # List of executables to test
    EXECUTABLES=(
        "nlm"
        "nlm_phase3_demo"
        "nlm_phase4_demo"
        "nlm_phase6_demo"
    )
    
    for exe in "${EXECUTABLES[@]}"; do
        if [[ -f "$exe" ]]; then
            print_info "Running $exe..."
            ./$exe --help 2>&1 | head -5
            
            if [[ $? -eq 0 ]]; then
                print_success "$exe executed successfully."
            else
                print_warning "$exe execution had warnings or returned non-zero."
            fi
        else
            print_warning "$exe not found. Skipping."
        fi
    done
    
    return 0
}

# Create documentation
create_documentation() {
    print_info "Creating documentation..."
    
    # Check if enhanced documentation exists
    if [[ -f "../ENHANCED_DOCUMENTATION.md" ]]; then
        print_info "Enhanced documentation found. Creating build report..."
        
        # Generate build report
        cat > build_report.txt << EOF
NLM Phase 6 Enhanced Build Report
================================

Build Date: $(date)
Build Type: $BUILD_TYPE
Python Bindings: $PYTHON_BINDINGS

Executables Built:
EOF
        
        for exe in "$BUILD_DIR"/*; do
            if [[ -x "$exe" && -f "$exe" && "$exe" != "$BUILD_DIR/CMakeCache.txt" ]]; then
                echo "  $(basename "$exe")" >> build_report.txt
            fi
        done
        
        cat >> build_report.txt << EOF

Tests Run: $TEST_COVERAGE

Build Status: SUCCESS

For complete documentation, see:
  - ENHANCED_DOCUMENTATION.md (main documentation)
  - README.md (basic usage)
  - HOW_TO_USE.md (Python API)
  - easy_usage.md (beginner guide)
EOF
        
        print_success "Documentation and build report created."
    else
        print_warning "Enhanced documentation not found. Skipping documentation creation."
    fi
    
    return 0
}

# Clean build directory
clean_build() {
    print_info "Cleaning build directory..."
    
    if [[ -d "$BUILD_DIR" ]]; then
        read -p "Are you sure you want to remove the build directory? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            rm -rf "$BUILD_DIR"
            print_success "Build directory cleaned."
        else
            print_info "Build directory cleaning cancelled."
        fi
    else
        print_info "Build directory not found."
    fi
    
    return 0
}

# Main function
main() {
    print_info "NLM Phase 6 Enhanced Build and Test Script"
    print_info "========================================="
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --build-dir=*
                BUILD_DIR="${1#*=}"
                shift
                ;;
            --no-python-bindings)
                PYTHON_BINDINGS="OFF"
                shift
                ;;
            --debug-build)
                VERBOSE_BUILD="ON"
                shift
                ;;
            --test-coverage)
                TEST_COVERAGE="ON"
                shift
                ;;
            --clean)
                clean_build
                return 0
                ;;
            --help)
                echo "Usage: $0 [options]"
                echo "Options:"
                echo "  --build-dir=<dir>           Build directory (default: build)"
                echo "  --no-python-bindings        Disable Python bindings installation"
                echo "  --debug-build               Enable verbose build output"
                echo "  --test-coverage             Enable test coverage reporting"
                echo "  --clean                     Clean build directory and exit"
                echo "  --help                      Show this help message"
                return 0
                ;;
            *)
                echo "Unknown option: $1"
                echo "Use --help for usage information."
                return 1
                ;;
        esac
    done
    
    # Run build process
    if check_prerequisites; then
        if configure_build; then
            if build_nlm; then
                if install_python_bindings; then
                    if run_tests; then
                        if run_demos; then
                            create_documentation
                            
                            print_success "Build and test completed successfully!"
                            print_info "To run NLM demos, use:"
                            print_info "  cd $BUILD_DIR && ./nlm demo"
                            print_info "  cd $BUILD_DIR && ./nlm benchmark"
                            print_info "  For more help: cd $BUILD_DIR && ./nlm --help"
                        else
                            print_error "Demo execution failed."
                            return 1
                        fi
                    else
                        print_error "Tests failed."
                        return 1
                    fi
                else
                    print_error "Python bindings installation failed."
                    return 1
                fi
            else
                print_error "Build failed."
                return 1
            fi
        else
            print_error "Build configuration failed."
            return 1
        fi
    else
        print_error "Prerequisite check failed."
        return 1
    fi
}

# Run main function
main "$@"
