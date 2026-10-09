#!/bin/bash
# NLM Build Script
# Comprehensive build and development helper script for NLM (Neural Learning Machine)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
PARALLEL_JOBS=$(nproc)
INSTALL_PREFIX=""
SKIP_TESTS=false
SKIP_PYTHON=false

# Function to print colored output
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

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check required tools
check_dependencies() {
    print_info "Checking dependencies..."
    
    local missing_deps=()
    
    if ! command_exists cmake; then
        missing_deps+=("CMake")
    fi
    
    if ! command_exists g++ && ! command_exists clang++; then
        missing_deps+=("C++ compiler (g++ or clang++)")
    fi
    
    if ! command_exists python3; then
        missing_deps+=("Python 3")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing required dependencies:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo ""
        print_info "Please install the missing dependencies and try again."
        return 1
    fi
    
    # Check CMake version
    local cmake_version=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+')
    local min_version="3.16"
    
    if [[ $(echo -e "$cmake_version\n$min_version" | sort -V | head -n1) != "$min_version" ]]; then
        print_warning "CMake version $cmake_version is less than minimum required $min_version"
        print_warning "Some features may not work properly."
    fi
    
    print_success "All dependencies check passed"
    return 0
}

# Function to create build directory
create_build_dir() {
    print_info "Creating build directory: $BUILD_DIR"
    
    if [ -d "$BUILD_DIR" ]; then
        if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
            print_info "Build directory already exists with cache. Clean or specify a different directory."
            return 1
        else
            print_warning "Build directory exists but is empty. Removing and recreating..."
            rm -rf "$BUILD_DIR"
        fi
    fi
    
    mkdir -p "$BUILD_DIR"
    print_success "Build directory created"
}

# Function to configure CMake
configure_cmake() {
    print_info "Configuring CMake with $BUILD_TYPE build..."
    
    local cmake_args="-DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_CXX_STANDARD=20"
    
    if [ -n "$INSTALL_PREFIX" ]; then
        cmake_args="$cmake_args -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
    fi
    
    # Add build options
    cmake_args="$cmake_args -DPYTHON_EXECUTABLE=$(which python3)"
    
    print_info "Running: cmake $cmake_args .."
    
    if ! cmake $cmake_args ..; then
        print_error "CMake configuration failed"
        return 1
    fi
    
    print_success "CMake configuration completed"
}

# Function to build the project
build_project() {
    print_info "Building project with $PARALLEL_JOBS parallel jobs..."
    
    print_info "Running: make -j$PARALLEL_JOBS"
    
    if ! make -j$PARALLEL_JOBS; then
        print_error "Build failed"
        return 1
    fi
    
    print_success "Build completed successfully"
}

# Function to run tests
run_tests() {
    if [ "$SKIP_TESTS" = true ]; then
        print_info "Skipping tests as requested"
        return 0
    fi
    
    print_info "Running tests..."
    
    if ! ctest --output-on-failure; then
        print_error "Tests failed"
        return 1
    fi
    
    print_success "All tests passed"
}

# Function to build Python bindings
build_python_bindings() {
    if [ "$SKIP_PYTHON" = true ]; then
        print_info "Skipping Python bindings as requested"
        return 0
    fi
    
    if ! command_exists pip; then
        print_warning "pip not found, skipping Python bindings installation"
        return 0
    fi
    
    print_info "Building Python bindings..."
    
    cd build
    
    if ! python3 -m pip install -e .; then
        print_error "Python bindings installation failed"
        cd ..
        return 1
    fi
    
    cd ..
    print_success "Python bindings installed successfully"
}

# Function to install the project
install_project() {
    print_info "Installing project..."
    
    cd build
    
    if ! make install; then
        print_error "Installation failed"
        cd ..
        return 1
    fi
    
    cd ..
    print_success "Installation completed"
}

# Function to show usage
usage() {
    cat << EOF
NLM Build Script

Usage: $0 [OPTIONS]

Options:
  --build-type TYPE      Build type: Debug or Release (default: Release)
  --build-dir DIR        Build directory (default: build)
  --jobs NUM             Number of parallel jobs (default: CPU cores)
  --install-prefix PREFIX Installation prefix (default: /usr/local)
  --skip-tests           Skip running tests after build
  --skip-python         Skip Python bindings installation
  --help                 Show this help message

Examples:
  $0                              # Build and test Release version
  $0 --build-type Debug           # Build Debug version
  $0 --build-dir build_debug      # Use custom build directory
  $0 --skip-tests                 # Build without running tests
  $0 --install-prefix /opt/nlm    # Install to custom location
EOF
}

# Function to parse command line arguments
parse_arguments() {
    while [ $# -gt 0 ]; do
        case $1 in
            --build-type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            --build-dir)
                BUILD_DIR="$2"
                shift 2
                ;;
            --jobs)
                PARALLEL_JOBS="$2"
                shift 2
                ;;
            --install-prefix)
                INSTALL_PREFIX="$2"
                shift 2
                ;;
            --skip-tests)
                SKIP_TESTS=true
                shift
                ;;
            --skip-python)
                SKIP_PYTHON=true
                shift
                ;;
            --help)
                usage
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                usage
                exit 1
                ;;
        esac
    done
}

# Main build function
build_all() {
    print_info "Starting NLM build process..."
    print_info "Build type: $BUILD_TYPE"
    print_info "Build directory: $BUILD_DIR"
    print_info "Parallel jobs: $PARALLEL_JOBS"
    
    # Check dependencies
    if ! check_dependencies; then
        exit 1
    fi
    
    # Create build directory
    create_build_dir || exit 1
    
    # Configure CMake
    if ! cd "$BUILD_DIR" && configure_cmake; then
        exit 1
    fi
    
    # Build project
    if ! build_project; then
        exit 1
    fi
    
    # Run tests
    if ! run_tests; then
        exit 1
    fi
    
    # Build Python bindings
    if ! build_python_bindings; then
        exit 1
    fi
    
    print_success "Build process completed successfully!"
    print_info ""
    print_info "Next steps:"
    print_info "  1. Run the main executable: ./$BUILD_DIR/nlm"
    print_info "  2. Install Python bindings: pip install -e ."
    print_info "  3. Run tests again: ctest --output-on-failure"
}

# Script entry point
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    parse_arguments "$@"
    build_all
fi
