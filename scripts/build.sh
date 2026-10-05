#!/bin/bash
# NLM Build Script
# Simple and comprehensive build script for NLM Neural Learning Machine

# Set error handling
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

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Default configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
INSTALL_PREFIX=""
ENABLE_PYTHON_BINDINGS=true
BUILD_TESTS=true
BUILD_DOCS=false
BUILD_EXAMPLES=true
NUM_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --install-prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --no-python)
            ENABLE_PYTHON_BINDINGS=false
            shift
            ;;
        --no-tests)
            BUILD_TESTS=false
            shift
            ;;
        --docs)
            BUILD_DOCS=true
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES=false
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --debug                Build in Debug mode (default: Release)"
            echo "  --build-dir DIR        Build directory (default: build)"
            echo "  --install-prefix DIR   Installation prefix (default: system default)"
            echo "  --no-python            Disable Python bindings"
            echo "  --no-tests             Disable tests"
            echo "  --docs                 Build documentation"
            echo "  --no-examples          Disable examples"
            echo "  --help                 Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install system dependencies
install_dependencies() {
    print_info "Checking system dependencies..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Linux dependencies
        if command_exists apt-get; then
            print_info "Installing dependencies via apt-get..."
            sudo apt-get update
            sudo apt-get install -y \
                build-essential \
                cmake \
                git \
                python3-dev \
                python3-pip \
                python3-venv \
                libboost-all-dev \
                doxygen \
                graphviz \
                libgraphviz-dev
        elif command_exists yum; then
            print_info "Installing dependencies via yum..."
            sudo yum install -y \
                gcc \
                gcc-c++ \
                cmake \
                git \
                python3-devel \
                python3-pip \
                boost-devel \
                doxygen \
                graphviz-devel
        elif command_exists dnf; then
            print_info "Installing dependencies via dnf..."
            sudo dnf install -y \
                gcc \
                gcc-c++ \
                cmake \
                git \
                python3-devel \
                python3-pip \
                boost-devel \
                doxygen \
                graphviz-devel
        else
            print_warn "Package manager not detected. Please install dependencies manually."
        fi
        
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS dependencies
        if command_exists brew; then
            print_info "Installing dependencies via Homebrew..."
            brew install cmake python3 boost doxygen graphviz
        else
            print_warn "Homebrew not detected. Please install dependencies manually."
        fi
    else
        print_warn "Unknown OS type: $OSTYPE"
        print_warn "Please install build dependencies manually."
    fi
}

# Function to check Python dependencies
check_python_dependencies() {
    print_info "Checking Python dependencies..."
    
    if ! command_exists pip3; then
        print_error "pip3 not found. Please install pip."
        exit 1
    fi
    
    # Install Python development dependencies
    pip3 install --upgrade pip setuptools wheel
    
    # Install required Python packages
    pip3 install pybind11 scikit-build-core pytest numpy
    
    if [[ "$ENABLE_PYTHON_BINDINGS" == "true" ]]; then
        print_info "Python bindings will be enabled."
    else
        print_info "Python bindings are disabled."
    fi
}

# Function to configure CMake
configure_cmake() {
    print_info "Configuring CMake..."
    
    cd "$BUILD_DIR"
    
    # Base CMake command
    CMAKE_CMD="cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    
    # Add options based on configuration
    if [[ "$ENABLE_PYTHON_BINDINGS" == "true" ]]; then
        CMAKE_CMD="$CMAKE_CMD -DENABLE_PYTHON_BINDINGS=ON"
    else
        CMAKE_CMD="$CMAKE_CMD -DENABLE_PYTHON_BINDINGS=OFF"
    fi
    
    if [[ "$BUILD_TESTS" == "true" ]]; then
        CMAKE_CMD="$CMAKE_CMD -DBUILD_TESTS=ON"
    else
        CMAKE_CMD="$CMAKE_CMD -DBUILD_TESTS=OFF"
    fi
    
    if [[ "$BUILD_DOCS" == "true" ]]; then
        CMAKE_CMD="$CMAKE_CMD -DBUILD_DOCS=ON"
    else
        CMAKE_CMD="$CMAKE_CMD -DBUILD_DOCS=OFF"
    fi
    
    if [[ "$BUILD_EXAMPLES" == "true" ]]; then
        CMAKE_CMD="$CMAKE_CMD -DBUILD_EXAMPLES=ON"
    else
        CMAKE_CMD="$CMAKE_CMD -DBUILD_EXAMPLES=OFF"
    fi
    
    # Add install prefix if specified
    if [[ -n "$INSTALL_PREFIX" ]]; then
        CMAKE_CMD="$CMAKE_CMD -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
    fi
    
    # Run CMake
    print_info "Running: $CMAKE_CMD"
    eval "$CMAKE_CMD"
    
    cd - >/dev/null
}

# Function to build the project
build_project() {
    print_info "Building NLM..."
    
    cd "$BUILD_DIR"
    
    # Build with parallel jobs
    print_info "Building with $NUM_JOBS parallel jobs..."
    make -j$NUM_JOBS
    
    # Build Python bindings if enabled
    if [[ "$ENABLE_PYTHON_BINDINGS" == "true" ]]; then
        print_info "Building Python bindings..."
        make pynlm
    fi
    
    cd - >/dev/null
}

# Function to install the project
install_project() {
    print_info "Installing NLM..."
    
    if [[ -z "$INSTALL_PREFIX" ]]; then
        print_warn "No install prefix specified. Using system default."
        print_warn "To install to a specific directory, use --install-prefix /path/to/prefix"
    fi
    
    cd "$BUILD_DIR"
    make install
    cd - >/dev/null
}

# Function to run tests
run_tests() {
    print_info "Running tests..."
    
    if [[ "$BUILD_TESTS" != "true" ]]; then
        print_info "Tests are disabled. Skipping."
        return
    fi
    
    cd "$BUILD_DIR"
    
    if [[ -f ./nlm_test ]]; then
        print_info "Running unit tests..."
        ./nlm_test
    else
        print_warn "Test executable not found. Skipping tests."
    fi
    
    if [[ -f ./ctest ]]; then
        print_info "Running CMake tests..."
        ctest --output-on-failure
    fi
    
    cd - >/dev/null
}

# Function to verify installation
verify_installation() {
    print_info "Verifying installation..."
    
    # Check executables
    print_info "Checking executables..."
    if [[ -f "$BUILD_DIR/bin/nlm" ]]; then
        print_info "✓ nlm executable found"
    else
        print_warn "nlm executable not found"
    fi
    
    if [[ -f "$BUILD_DIR/bin/nlm_phase6_demo" ]]; then
        print_info "✓ nlm_phase6_demo executable found"
    else
        print_warn "nlm_phase6_demo executable not found"
    fi
    
    # Check Python bindings
    if [[ "$ENABLE_PYTHON_BINDINGS" == "true" ]]; then
        print_info "Checking Python bindings..."
        if command_exists python3; then
            if python3 -c "import pynlm; print('✓ Python bindings found')" 2>/dev/null; then
                print_info "Python bindings version: $(python3 -c 'import pynlm; print(pynlm.__version__)' 2>/dev/null)"
            else
                print_warn "Python bindings not accessible"
            fi
        else
            print_warn "python3 not found"
        fi
    fi
    
    # Check documentation
    if [[ "$BUILD_DOCS" == "true" ]]; then
        print_info "Checking documentation..."
        if [[ -f "$BUILD_DIR/docs/html/index.html" ]]; then
            print_info "✓ Documentation found"
        else
            print_warn "Documentation not found"
        fi
    fi
}

# Function to show build summary
show_build_summary() {
    echo ""
    echo "============================================================"
    echo "BUILD SUMMARY"
    echo "============================================================"
    echo "Build type: $BUILD_TYPE"
    echo "Build directory: $BUILD_DIR"
    echo "Number of CPU cores used: $NUM_JOBS"
    echo "Python bindings: $ENABLE_PYTHON_BINDINGS"
    echo "Tests: $BUILD_TESTS"
    echo "Documentation: $BUILD_DOCS"
    echo "Examples: $BUILD_EXAMPLES"
    
    if [[ -n "$INSTALL_PREFIX" ]]; then
        echo "Install prefix: $INSTALL_PREFIX"
    fi
    
    echo ""
    echo "Next steps:"
    echo "1. Run: cd $BUILD_DIR && ./nlm_phase6_demo"
    echo "2. Run: cd $BUILD_DIR && ctest --output-on-failure"
    if [[ "$ENABLE_PYTHON_BINDINGS" == "true" ]]; then
        echo "3. Test Python: python3 -c 'import pynlm; print(pynlm.__version__)'"
    fi
    if [[ "$BUILD_DOCS" == "true" ]]; then
        echo "4. View documentation: open $BUILD_DIR/docs/html/index.html"
    fi
    echo ""
    echo "Installation:"
    echo "  To install system-wide: make install"
    echo "  To install to prefix: make install DESTDIR=<prefix>"
    echo "============================================================"
}

# Main execution
main() {
    print_info "NLM Build Script"
    print_info "Building NLM (熙然) Neural Learning Machine"
    print_info "============================================================"
    
    # Check if build directory exists
    if [[ -d "$BUILD_DIR" ]]; then
        print_warn "Build directory '$BUILD_DIR' already exists."
        print_warn "To use a different directory, specify --build-dir"
        read -p "Continue anyway? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "Build cancelled."
            exit 0
        fi
    fi
    
    # Create build directory
    print_info "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    
    # Install system dependencies (optional)
    if [[ "$OSTYPE" == "linux-gnu"* || "$OSTYPE" == "darwin"* ]]; then
        read -p "Install system dependencies? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            install_dependencies
        fi
    fi
    
    # Check Python dependencies
    if [[ "$ENABLE_PYTHON_BINDINGS" == "true" ]]; then
        check_python_dependencies
    fi
    
    # Configure CMake
    configure_cmake
    
    # Build project
    build_project
    
    # Install project (optional)
    read -p "Install to system? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if [[ -z "$INSTALL_PREFIX" ]]; then
            print_warn "No install prefix specified. Installing system-wide."
            read -p "This requires sudo. Continue? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                if command_exists sudo; then
                    sudo make install
                else
                    print_error "sudo not found. Cannot install system-wide."
                fi
            fi
        else
            read -p "Install to $INSTALL_PREFIX? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                install_project
            fi
        fi
    fi
    
    # Run tests (optional)
    read -p "Run tests? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        run_tests
    fi
    
    # Verify installation
    verify_installation
    
    # Show build summary
    show_build_summary
}

# Run main function with all arguments
main "$@"