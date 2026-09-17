#!/bin/bash
# NLM Build Script - Simplified project setup and build automation
# Phase 6: Final Integration

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="NLM - Neural Learning Machine"
PROJECT_VERSION="0.6.0"
BUILD_DIR="build"
CONFIG_FILE="configs/default.cfg"
PYTHON_PACKAGE="pynlm"

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

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check for required tools
check_prerequisites() {
    print_info "Checking prerequisites..."
    
    local missing_tools=()
    
    if ! command_exists cmake; then
        missing_tools+=("CMake")
    fi
    
    if ! command_exists g++; then
        missing_tools+=("g++")
    fi
    
    if ! command_exists pip3; then
        missing_tools+=("pip3")
    fi
    
    if ! command_exists python3; then
        missing_tools+=("python3")
    fi
    
    if [ ${#missing_tools[@]} -gt 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        print_info "Please install them and try again."
        exit 1
    fi
    
    print_success "All prerequisites found!"
}

# Function to setup Python dependencies
 setup_python_deps() {
    print_info "Setting up Python dependencies..."
    
    # Install Python build tools
    pip3 install --upgrade pip
    pip3 install pybind11>=2.11.0 scikit-build-core>=0.5.0 pytest numpy>=1.20
    
    print_success "Python dependencies installed successfully!"
}

# Function to configure CMake
configure_cmake() {
    print_info "Configuring CMake build..."
    
    local build_type="Release"
    
    # Check for Debug flag
    if [[ "$*" == *"--debug"* ]]; then
        build_type="Debug"
        print_warning "Building in Debug mode"
    fi
    
    # Configure CMake
    cmake .. -DCMAKE_BUILD_TYPE="$build_type" \
             -DPYTHON_EXECUTABLE="$(which python3)" \
             -DNLM_SOURCE_DIR="src" \
             -DCMAKE_CXX_STANDARD=20 \
             -DCMAKE_CXX_STANDARD_REQUIRED=ON
    
    print_success "CMake configuration complete!"
}

# Function to build the project
build_project() {
    print_info "Building the project..."
    
    local jobs=$(nproc)
    print_info "Using $jobs parallel jobs for compilation"
    
    make -j$jobs
    
    print_success "Build complete!"
}

# Function to build Python bindings
build_python_bindings() {
    print_info "Building Python bindings..."
    
    # Build Python module
    pip3 install -e . --verbose
    
    print_success "Python bindings built successfully!"
}

# Function to run tests
run_tests() {
    print_info "Running tests..."
    
    if [ -f "CTestTestfile.cmake" ]; then
        ctest --output-on-failure
    else
        print_warning "No C++ tests found (CMake test framework not configured)"
    fi
    
    # Run Python tests if available
    if command_exists python3 && python3 -m pytest --version >/dev/null 2>&1; then
        print_info "Running Python tests..."
        cd python && python3 -m pytest ./tests -v
    fi
    
    print_success "All tests completed!"
}

# Function to create symlinks for easier usage
create_symlinks() {
    print_info "Creating useful symlinks..."
    
    # Create symlinks in project root for convenience
    if [ ! -f "run_nlm" ]; then
        ln -sf "$BUILD_DIR/nlm" "run_nlm"
    fi
    
    if [ ! -f "run_nlm_phase3" ]; then
        ln -sf "$BUILD_DIR/nlm_phase3_demo" "run_nlm_phase3"
    fi
    
    if [ ! -f "run_nlm_phase4" ]; then
        ln -sf "$BUILD_DIR/nlm_phase4_demo" "run_nlm_phase4"
    fi
    
    if [ ! -f "run_nlm_phase6" ]; then
        ln -sf "$BUILD_DIR/nlm_phase6_demo" "run_nlm_phase6"
    fi
    
    print_success "Symlinks created!"
}

# Function to print help/usage
print_help() {
    cat << EOF
$NLM - Neural Learning Machine Build Script
Version: $PROJECT_VERSION

Usage: $0 [OPTIONS]

Options:
    build           Build the project (default)
    test            Build and run tests
    install-python  Build and install Python bindings
    help            Show this help message

Options:
    --debug         Build in Debug mode
    --quick         Quick build (no tests)
    --python-only   Only build Python bindings
    --config PATH   Use custom configuration file

Examples:
    $0                      # Build project with default settings
    $0 build --debug        # Build with debug symbols
    $0 test                 # Build and run tests
    $0 install-python       # Install Python bindings only
EOF
}

# Function to handle cleanup
cleanup() {
    print_info "Cleaning up..."
    
    if [ -d "$BUILD_DIR" ]; then
        print_info "Removing build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    # Remove symlinks
    rm -f run_nlm run_nlm_phase3 run_nlm_phase4 run_nlm_phase6
    
    print_success "Cleanup complete!"
}

# Function to build Python module only
build_python_only() {
    print_info "Building Python module only..."
    
    # Ensure Python environment
    setup_python_deps
    
    # Build Python module
    pip3 install -e . --verbose
    
    print_success "Python module built successfully!"
}

# Main build function
main() {
    local action="build"
    local config_file="$CONFIG_FILE"
    local quick_build=false
    local python_only=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug)
                CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug"
                shift
                ;;
            --quick)
                quick_build=true
                shift
                ;;
            --python-only)
                python_only=true
                shift
                ;;
            --config)
                if [[ -n $2 && ! $2 == -* ]]; then
                    config_file="$2"
                    shift 2
                else
                    print_error "Configuration file path required after --config"
                    exit 1
                fi
                ;;
            build|test|install-python|help)
                action="$1"
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                print_help
                exit 1
                ;;
        esac
    done
    
    # Set up environment
    check_prerequisites
    
    # Main action handling
    case "$action" in
        build)
            if [ "$python_only" = true ]; then
                build_python_only
            else
                # Create build directory
                mkdir -p "$BUILD_DIR"
                cd "$BUILD_DIR"
                
                # Configure and build
                configure_cmake
                build_project
                
                # Create symlinks
                cd ..
                create_symlinks
            fi
            ;;
        test)
            if [ "$quick_build" = true ]; then
                print_warning "Quick build mode - skipping full build"
            else
                # Create build directory
                mkdir -p "$BUILD_DIR"
                cd "$BUILD_DIR"
                
                # Configure and build
                configure_cmake
                build_project
                
                cd ..
            fi
            
            # Run tests
            run_tests
            ;;
        install-python)
            build_python_only
            ;;
        help)
            print_help
            ;;
    esac
}

# Execute main function
main "$@"