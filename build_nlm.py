#!/usr/bin/env bash
"""
Easy build script for NLM Python bindings development workflow.

This script provides simplified commands for building, testing, and running
NLM Python bindings. It handles the complex setup process and provides
helpful error messages for common issues.
"""

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

# Function to check Python version
check_python_version() {
    local required_version="3.8"
    local current_version
    
    if command_exists python3; then
        current_version=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
        
        if [[ "$(echo "$current_version >= $required_version" | bc -l 2>/dev/null || echo "0")" -eq 1 ]]; then
            print_info "Python $current_version found (required: >= $required_version)"
            return 0
        else
            print_error "Python $current_version is too old. Required: >= $required_version"
            return 1
        fi
    else
        print_error "python3 is not installed"
        return 1
    fi
}

# Function to check CMake version
check_cmake_version() {
    local required_version="3.16"
    
    if command_exists cmake; then
        local current_version
        current_version=$(cmake --version | head -n1 | awk '{print $3}')
        
        if [[ "$(echo "$current_version >= $required_version" | bc -l 2>/dev/null || echo "0")" -eq 1 ]]; then
            print_info "CMake $current_version found (required: >= $required_version)"
            return 0
        else
            print_error "CMake $current_version is too old. Required: >= $required_version"
            return 1
        fi
    else
        print_error "cmake is not installed"
        return 1
    fi
}

# Function to install build dependencies
install_build_dependencies() {
    print_info "Installing build dependencies..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Check for common package managers
        if command_exists apt-get; then
            print_info "Detected apt-get package manager"
            sudo apt-get update
            sudo apt-get install -y \
                build-essential \
                cmake \
                python3-dev \
                python3-pip \
                python3-venv \
                pkg-config \
                libssl-dev \
                libffi-dev \
                python3-setuptools
        elif command_exists yum; then
            print_info "Detected yum package manager"
            sudo yum install -y \
                gcc-c++ \
                cmake \
                python3-devel \
                python3-pip \
                python3-venv \
                pkgconfig \
                openssl-devel \
                libffi-devel
        elif command_exists dnf; then
            print_info "Detected dnf package manager"
            sudo dnf install -y \
                gcc-c++ \
                cmake \
                python3-devel \
                python3-pip \
                python3-venv \
                pkgconfig \
                openssl-devel \
                libffi-devel
        else
            print_error "Could not determine package manager. Please install dependencies manually."
            return 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if command_exists brew; then
            print_info "Detected Homebrew package manager"
            brew install cmake python3 pkg-config openssl libffi
        else
            print_error "Homebrew not found. Please install Homebrew or install dependencies manually."
            return 1
        fi
    else
        print_error "Unsupported OS: $OSTYPE"
        return 1
    fi
    
    print_success "Build dependencies installed"
}

# Function to build the project
build_project() {
    local build_type="${1:-Release}"
    local build_dir="${2:-build}"
    
    print_info "Building NLM with $build_type configuration..."
    
    # Check if build directory exists
    if [[ -d "$build_dir" ]]; then
        print_warning "Build directory '$build_dir' already exists. Removing..."
        rm -rf "$build_dir"
    fi
    
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with CMake
    print_info "Configuring CMake..."
    cmake .. -DCMAKE_BUILD_TYPE="$build_type" \
              -DPYTHON_EXECUTABLE="$(which python3)"
    
    # Build the project
    print_info "Building project..."
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo "4")
    
    cd "$PROJECT_ROOT"
    print_success "Build completed successfully"
}

# Function to install Python package
install_python_package() {
    local install_mode="${1:-editable}"
    
    print_info "Installing NLM Python bindings in $install_mode mode..."
    
    # Check if we're in a virtual environment
    if [[ -n "$VIRTUAL_ENV" ]]; then
        print_info "Running in virtual environment: $VIRTUAL_ENV"
    else
        print_warning "Not running in a virtual environment. Consider creating one."
    fi
    
    # Upgrade pip and setuptools
    print_info "Upgrading pip and setuptools..."
    pip install --upgrade pip setuptools wheel
    
    # Install dependencies
    print_info "Installing Python dependencies..."
    pip install -e ".[dev]"
    
    # Build and install the package
    print_info "Building and installing pynlm..."
    pip install -e . --no-build-isolation
    
    print_success "Python package installed successfully"
}

# Function to run tests
run_tests() {
    print_info "Running NLM tests..."
    
    # Run Python bindings tests if available
    if [[ -f "python/test_bindings.py" ]]; then
        print_info "Running Python bindings tests..."
        python3 python/test_bindings.py
    else
        print_warning "Python bindings test script not found"
    fi
    
    # Run C++ tests if available
    if [[ -d "build" && -f "build/nlm_test" ]]; then
        print_info "Running C++ unit tests..."
        ./build/nlm_test --output-on-failure
    else
        print_warning "C++ test executable not found. Build the project first."
    fi
}

# Function to run demo
run_demo() {
    print_info "Running NLM demo..."
    
    # Run Python demo if available
    if [[ -f "python/test_bindings.py" ]]; then
        python3 -c "import pynlm; print('NLM Version:', pynlm.__version__)"
        python3 -c "
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print('Brain created with', brain.getTotalNeuronCount(), 'neurons')
for i in range(10):
    brain.step(i)
print('Total spikes:', brain.getTotalSpikeCount())
"
    else
        print_error "Demo not available"
        return 1
    fi
}

# Function to show help
show_help() {
    cat << EOF
NLM Development Workflow Script

Usage: $0 <command> [options]

Commands:
  build [build_type] [build_dir]    Build the project (default: Release build, default dir: build)
  install [mode]                     Install Python package (default: editable mode)
  test                                Run tests
  demo                                Run demo
  install-deps                       Install system dependencies (Linux/macOS)
  check                              Check system requirements
  help                                Show this help message

Options:
  build_type    Release or Debug build type
  build_dir     Custom build directory path
  mode          editable, wheel, or system install

Examples:
  $0 build Debug
  $0 build Release build_dev
  $0 install wheel
  $0 test
  $0 demo
  $0 install-deps
  $0 check

EOF
}

# Function to check system requirements
check_requirements() {
    print_info "Checking system requirements..."
    
    local all_ok=true
    
    if ! check_python_version; then
        all_ok=false
    fi
    
    if ! check_cmake_version; then
        all_ok=false
    fi
    
    if ! command_exists make; then
        print_error "make is not installed"
        all_ok=false
    else
        print_info "make found"
    fi
    
    if ! command_exists pip3; then
        print_error "pip3 is not installed"
        all_ok=false
    else
        print_info "pip3 found"
    fi
    
    if $all_ok; then
        print_success "All system requirements met"
        return 0
    else
        print_error "Some system requirements are not met"
        return 1
    fi
}

# Main script logic
case "${1:-}" in
    build)
        build_type="${2:-Release}"
        build_dir="${3:-build}"
        build_project "$build_type" "$build_dir"
        ;;
    install)
        install_mode="${2:-editable}"
        install_python_package "$install_mode"
        ;;
    test)
        run_tests
        ;;
    demo)
        run_demo
        ;;
    install-deps)
        install_build_dependencies
        ;;
    check)
        check_requirements
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "Unknown command: $1"
        echo "Run '$0 help' for usage information."
        exit 1
        ;;
esac
