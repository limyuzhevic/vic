#!/bin/bash
# NLM Unified Build Script
# Simplified build and installation for NLM (Neural Learning Machine)

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
INSTALL_PREFIX="${HOME}/.local"
NUM_JOBS=$(nproc)
SKIP_TESTS=false
PYTHON_INSTALL=true
BUILD_DOCS=false

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

# Function to detect package manager
detect_package_manager() {
    if command_exists apt-get; then
        echo "apt-get"
    elif command_exists yum; then
        echo "yum"
    elif command_exists dnf; then
        echo "dnf"
    elif command_exists pacman; then
        echo "pacman"
    else
        echo "unknown"
    fi
}

# Function to install system dependencies
install_system_dependencies() {
    local package_manager=$(detect_package_manager)
    
    print_info "Installing system dependencies..."
    
    case $package_manager in
        "apt-get")
            sudo apt-get update
            sudo apt-get install -y \
                build-essential \
                cmake \
                git \
                python3-dev \
                python3-pip \
                python3-setuptools \
                libpybind11-dev \
                libomp-dev \
                doxygen \
                graphviz \
                pkg-config
            ;;
        "yum" | "dnf")
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y \
                cmake \
                git \
                python3-devel \
                python3-pip \
                pybind11-devel \
                libomp-devel \
                doxygen \
                graphviz \
                pkgconfig
            ;;
        "pacman")
            sudo pacman -Sy --noconfirm \
                base-devel \
                cmake \
                git \
                python \
                python-pip \
                pybind11 \
                libomp \
                doxygen \
                graphviz \
                pkgconf
            ;;
        *)
            print_warning "Unknown package manager. Please install dependencies manually."
            ;;
    esac
}

# Function to setup Python environment
setup_python_environment() {
    print_info "Setting up Python environment..."
    
    if ! command_exists python3; then
        print_error "Python3 is not installed."
        exit 1
    fi
    
    # Upgrade pip and setuptools
    python3 -m pip install --upgrade pip setuptools wheel
    
    # Install Python build dependencies
    python3 -m pip install scikit-build-core pybind11
}

# Function to build the project
build_project() {
    print_info "Building NLM..."
    
    if [ ! -d "build" ]; then
        mkdir -p build
    fi
    
    cd build
    
    # Configure CMake
    cmake ..
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
    -DPYTHON_EXECUTABLE=$(which python3)
    
    # Build the project
    cmake --build . --config $BUILD_TYPE -j$NUM_JOBS
    
    cd ..
}

# Function to install the project
install_project() {
    print_info "Installing NLM..."
    
    if [ ! -d "build" ]; then
        print_error "Build directory not found. Run './build.sh build' first."
        exit 1
    fi
    
    cd build
    
    # Install to prefix
    cmake --install . --prefix $INSTALL_PREFIX
    
    # Install Python package
    if [ "$PYTHON_INSTALL" = true ]; then
        print_info "Installing Python package..."
        cd ..
        pip3 install -e .
        cd build
    fi
    
    cd ..
}

# Function to run tests
run_tests() {
    if [ "$SKIP_TESTS" = true ]; then
        print_warning "Skipping tests as requested."
        return
    fi
    
    print_info "Running tests..."
    
    if [ -d "build" ]; then
        cd build
        
        # Run C++ tests
        if command_exists ctest; then
            print_info "Running C++ unit tests..."
            ctest --output-on-failure
        fi
        
        # Run Python tests
        if command_exists python3 && [ -f "../pyproject.toml" ]; then
            print_info "Running Python tests..."
            python3 -m pytest ../tests/ -v
        fi
        
        cd ..
    else
        print_warning "Build directory not found. Skipping tests."
    fi
}

# Function to build documentation
build_documentation() {
    if [ "$BUILD_DOCS" = false ]; then
        print_info "Documentation build skipped."
        return
    fi
    
    print_info "Building documentation..."
    
    if command_exists doxygen; then
        if [ -f "Doxyfile" ]; then
            doxygen Doxyfile
        else
            print_warning "Doxyfile not found. Generating basic configuration..."
            cmake -B docs -H . -DCMAKE_BUILD_TYPE=Release
            cmake --build docs
        fi
    else
        print_warning "Doxygen not installed. Skipping documentation build."
    fi
}

# Function to show help
show_help() {
    echo "NLM Unified Build Script"
    echo "========================"
    echo ""
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  help                    Show this help message"
    echo "  install                 Build and install NLM (default)"
    echo "  build-only              Build only (skip installation)"
    echo "  test-only               Build and run tests only"
    echo "  clean                   Clean build directory"
    echo ""
    echo "Options:"
    echo "  --type TYPE             Build type (Release, Debug, RelWithDebInfo) (default: Release)"
    echo "  --prefix PREFIX         Installation prefix (default: ~/.local)"
    echo "  --jobs JOBS             Number of parallel jobs (default: $(nproc))"
    echo "  --no-python             Skip Python installation"
    echo "  --docs                  Build documentation"
    echo "  --skip-tests            Skip tests"
    echo ""
    echo "Examples:"
    echo "  $0                      Install NLM with default settings"
    echo "  $0 build-only --type Debug"
    echo "  $0 --prefix /usr/local --docs"
}

# Function to clean build directory
clean_build() {
    print_info "Cleaning build directory..."
    
    if [ -d "build" ]; then
        rm -rf build
        print_success "Build directory cleaned."
    else
        print_info "Build directory not found."
    fi
}

# Main script logic
main() {
    # Parse command line arguments
    if [ $# -eq 0 ]; then
        # Default: install
        COMMAND="install"
    else
        COMMAND="$1"
        shift
    fi
    
    # Parse options
    while [ $# -gt 0 ]; do
        case $1 in
            --type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            --prefix)
                INSTALL_PREFIX="$2"
                shift 2
                ;;
            --jobs)
                NUM_JOBS="$2"
                shift 2
                ;;
            --no-python)
                PYTHON_INSTALL=false
                shift
                ;;
            --docs)
                BUILD_DOCS=true
                shift
                ;;
            --skip-tests)
                SKIP_TESTS=true
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Check for prerequisites
    if ! command_exists cmake; then
        print_error "CMake is not installed."
        echo "Please install CMake first (https://cmake.org/download/)."
        exit 1
    fi
    
    if ! command_exists python3; then
        print_error "Python3 is not installed."
        echo "Please install Python3 first."
        exit 1
    fi
    
    # Execute command
    case $COMMAND in
        "install")
            install_system_dependencies
            setup_python_environment
            build_project
            install_project
            run_tests
            build_documentation
            print_success "NLM installation completed successfully!"
            ;;
        "build-only")
            build_project
            print_success "Build completed!"
            ;;
        "test-only")
            install_system_dependencies
            setup_python_environment
            build_project
            run_tests
            print_success "Tests completed!"
            ;;
        "clean")
            clean_build
            ;;
        "help"|*)
            show_help
            ;;
        *)
            print_error "Unknown command: $COMMAND"
            show_help
            exit 1
            ;;
    esac
}

# Run main function
main "$@"