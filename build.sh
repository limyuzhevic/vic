#!/bin/bash
# NLM Build Script
# Common build workflows for the NLM (Neural Learning Machine) project

# Color output for better readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
GENERATOR="Unix Makefiles"
NUM_JOBS=$(nproc)

# Print usage
function print_usage() {
    echo "NLM Build Script"
    echo "================"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --help                    Show this help message"
    echo "  --type TYPE              Build type (Debug, Release, RelWithDebInfo, MinSizeRel) (default: Release)"
    echo "  --generator GEN          CMake generator (Unix Makefiles, Ninja, etc.) (default: Unix Makefiles)"
    echo "  --jobs JOBS              Number of parallel jobs (default: $(nproc))"
    echo "  --build-dir DIR          Build directory (default: build)"
    echo "  --python                 Build Python bindings"
    echo "  --benchmark              Build benchmark executable"
    echo "  --all                    Build all targets (default)"
    echo "  --clean                  Clean build directory"
    echo ""
    echo "Examples:"
    echo "  $0                          # Build all targets (default)"
    echo "  $0 --type Debug --generator Ninja --jobs 8"
    echo "  $0 --python                 # Build only Python bindings"
    echo "  $0 --benchmark              # Build only benchmark executable"
    echo "  $0 --clean                  # Clean build directory"
}

# Function to print colored messages
function print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

function print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

function print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a command exists
function command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check for required tools
function check_dependencies() {
    print_info "Checking dependencies..."

    # Check for CMake
    if ! command_exists cmake; then
        print_error "CMake not found. Please install CMake (>= 3.16)."
        return 1
    fi

    # Check CMake version
    local cmake_version=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' || echo "0.0.0")
    local required_version="3.16"
    
    # Simple version comparison (not robust, but works for common cases)
    if [[ "$(echo "$cmake_version" | cut -d. -f1)" -lt "$(echo "$required_version" | cut -d. -f1)" ]]; then
        print_warning "CMake version $cmake_version is older than required $required_version"
    elif [[ "$(echo "$cmake_version" | cut -d. -f1)" -eq "$(echo "$required_version" | cut -d. -f1)" ]]; then
        if [[ "$(echo "$cmake_version" | cut -d. -f2)" -lt "$(echo "$required_version" | cut -d. -f2)" ]]; then
            print_warning "CMake version $cmake_version is older than required $required_version"
        fi
    fi

    # Check for C++ compiler
    if ! command_exists g++ && ! command_exists clang++; then
        print_error "C++ compiler not found. Please install g++ or clang++."
        return 1
    fi

    # Check for Python (optional)
    if command_exists python3; then
        print_info "Python3 found at $(which python3)"
    else
        print_warning "Python3 not found. Python bindings may not build."
    fi

    # Check for pybind11 (optional)
    if command_exists pip3; then
        print_info "pip3 found at $(which pip3)"
    fi

    return 0
}

# Function to configure CMake
function configure_cmake() {
    print_info "Configuring CMake project..."

    # Create build directory
    mkdir -p "$BUILD_DIR"

    # Configure with CMake
    cd "$BUILD_DIR"

    local cmake_args="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

    if [[ "$GENERATOR" == "Ninja" ]]; then
        cmake_args="$cmake_args -G Ninja"
    fi

    # Python bindings support
    if [[ "$BUILD_PYTHON" == "true" ]]; then
        cmake_args="$cmake_args -DPYTHON_EXECUTABLE=$(which python3)"
    fi

    # Common CMake options
    cmake_args="$cmake_args \
        -DCMAKE_CXX_STANDARD=20 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_CXX_EXTENSIONS=OFF \
        -DCMAKE_POSITION_INDEPENDENT_VARIABLES=ON"

    print_info "Running: cmake $cmake_args .."

    if ! cmake $cmake_args ..; then
        print_error "CMake configuration failed!"
        return 1
    fi

    cd - > /dev/null
    return 0
}

# Function to build the project
function build_project() {
    print_info "Building NLM project..."

    cd "$BUILD_DIR"

    # Build with parallel jobs
    print_info "Building with $NUM_JOBS parallel jobs..."

    if [[ "$GENERATOR" == "Ninja" ]]; then
        if ! ninja -j$NUM_JOBS; then
            print_error "Build failed!"
            return 1
        fi
    else
        if ! make -j$NUM_JOBS; then
            print_error "Build failed!"
            return 1
        fi
    fi

    cd - > /dev/null

    # List built targets
    print_info "Build completed successfully!"
    print_info "Built targets in $BUILD_DIR:"

    if [[ -f "$BUILD_DIR/nlm" ]]; then
        print_info "  - nlm (main demo executable)"
    fi

    if [[ -f "$BUILD_DIR/nlm_phase3_demo" ]]; then
        print_info "  - nlm_phase3_demo (Phase 3 demonstration)"
    fi

    if [[ -f "$BUILD_DIR/nlm_phase4_demo" ]]; then
        print_info "  - nlm_phase4_demo (Phase 4 demonstration)"
    fi

    if [[ -f "$BUILD_DIR/benchmark" ]]; then
        print_info "  - benchmark (performance benchmark)"
    fi

    if [[ -f "$BUILD_DIR/nlm_cli" ]]; then
        print_info "  - nlm_cli (NLM command line interface)"
    fi

    if [[ -d "$BUILD_DIR/python" ]]; then
        print_info "  - python/ (Python bindings)"
    fi

    return 0
}

# Function to build Python bindings
function build_python_bindings() {
    print_info "Building Python bindings..."

    if [[ ! -f "$BUILD_DIR/pypynlm.so" && ! -f "$BUILD_DIR/pynlm.cpython*.so" ]]; then
        print_info "Python bindings not found in build directory."
        print_info "Building with Python support..."

        BUILD_PYTHON=true
        configure_cmake
        build_project
    fi

    # Check if Python bindings were built
    if find "$BUILD_DIR" -name "*.so" -o -name "*.pyd" | grep -q .; then
        print_info "Python bindings built successfully!"
        return 0
    else
        print_warning "Python bindings may not have been built."
        print_info "You may need to install pybind11 and other dependencies."
        return 1
    fi
}

# Function to build benchmark executable
function build_benchmark() {
    print_info "Building benchmark executable..."

    if [[ -f "$BUILD_DIR/benchmark" ]]; then
        print_info "Benchmark executable already exists."
        return 0
    fi

    BUILD_BENCHMARK=true
    configure_cmake
    build_project

    if [[ ! -f "$BUILD_DIR/benchmark" ]]; then
        print_error "Benchmark executable not found after build!"
        return 1
    fi

    print_info "Benchmark executable built successfully!"
    return 0
}

# Function to build CLI executable
function build_cli() {
    print_info "Building NLM CLI executable..."

    if [[ -f "$BUILD_DIR/nlm_cli" ]]; then
        print_info "NLM CLI executable already exists."
        return 0
    fi

    BUILD_CLI=true
    configure_cmake
    build_project

    if [[ ! -f "$BUILD_DIR/nlm_cli" ]]; then
        print_error "NLM CLI executable not found after build!"
        return 1
    fi

    print_info "NLM CLI executable built successfully!"
    return 0
}

# Function to clean build directory
function clean_build() {
    print_info "Cleaning build directory..."

    if [[ -d "$BUILD_DIR" ]]; then
        cd "$BUILD_DIR"
        if [[ "$GENERATOR" == "Ninja" ]]; then
            ninja clean 2>/dev/null || make clean
        else
            make clean
        fi
        cd - > /dev/null
        print_info "Build directory cleaned."
    else
        print_info "Build directory does not exist."
    fi
}

# Function to run tests
function run_tests() {
    print_info "Running tests..."

    # For now, just check if executables exist
    if [[ ! -f "$BUILD_DIR/nlm" ]]; then
        print_error "nlm executable not found. Please build first."
        return 1
    fi

    if [[ ! -f "$BUILD_DIR/benchmark" ]]; then
        print_error "benchmark executable not found. Please build first."
        return 1
    fi

    print_info "Running nlm demo..."
    if ! "$BUILD_DIR/nlm" --help >/dev/null 2>&1; then
        print_info "Note: nlm demo may require additional configuration."
    fi

    print_info "Running benchmark..."
    if ! "$BUILD_DIR/benchmark" >/dev/null 2>&1; then
        print_info "Note: benchmark may require additional configuration."
    fi

    print_info "Tests completed (basic checks only)."
}

# Function to install Python package
function install_python_package() {
    print_info "Installing NLM Python package..."

    if [[ ! -f "$BUILD_DIR/pypynlm.so" && ! -f "$BUILD_DIR/pynlm.cpython*.so" ]]; then
        print_error "Python bindings not found. Please build with --python flag first."
        return 1
    fi

    # Find the Python bindings file
    local pybind_file=$(find "$BUILD_DIR" -name "*.so" -o -name "*.pyd" | head -n1)

    if [[ -z "$pybind_file" ]]; then
        print_error "Could not find Python bindings file."
        return 1
    fi

    # Copy to Python site-packages
    if command_exists python3; then
        local python_lib_dir=$(python3 -c "import site; print(site.getsitepackages()[0])" 2>/dev/null || echo "")
        if [[ -z "$python_lib_dir" ]]; then
            python_lib_dir=$(python3 -c "import sysconfig; print(sysconfig.get_path('platlib'))" 2>/dev/null || echo "")
        fi

        if [[ ! -z "$python_lib_dir" ]]; then
            local dest_file="$python_lib_dir/pynlm.so"
            cp "$pybind_file" "$dest_file"
            print_info "Copied Python bindings to $dest_file"

            # Try to build Python package
            cd python || cd .  # Try to enter python directory if it exists

            if [[ -f "pyproject.toml" ]]; then
                print_info "Building Python package with pip..."
                if ! pip3 install -e .; then
                    print_warning "pip install failed. You may need to install dependencies manually."
                fi
            fi

            cd - > /dev/null
        else
            print_warning "Could not determine Python library directory."
            print_warning "Manual installation may be required."
        fi
    else
        print_error "Python3 not found. Cannot install Python bindings."
        return 1
    fi

    print_info "Python package installation completed."
}

# Main function
function main() {
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help)
                print_usage
                exit 0
                ;;
            --type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            --generator)
                GENERATOR="$2"
                shift 2
                ;;
            --jobs)
                NUM_JOBS="$2"
                shift 2
                ;;
            --build-dir)
                BUILD_DIR="$2"
                shift 2
                ;;
            --python)
                BUILD_PYTHON=true
                shift
                ;;
            --benchmark)
                BUILD_BENCHMARK=true
                shift
                ;;
            --cli)
                BUILD_CLI=true
                shift
                ;;
            --all)
                shift
                ;;
            --clean)
                clean_build
                exit 0
                ;;
            --install-python)
                install_python_package
                exit 0
                ;;
            --run-tests)
                run_tests
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done

    # Check for required dependencies
    if ! check_dependencies; then
        print_error "Dependency check failed. Please install missing dependencies."
        exit 1
    fi

    # Determine what to build
    local build_all=true

    if [[ "$BUILD_PYTHON" == "true" ]]; then
        build_all=false
        build_python_bindings
    fi

    if [[ "$BUILD_BENCHMARK" == "true" ]]; then
        build_all=false
        build_benchmark
    fi

    if [[ "$BUILD_CLI" == "true" ]]; then
        build_all=false
        build_cli
    fi

    # Build all targets if no specific target was requested
    if [[ "$build_all" == "true" ]]; then
        configure_cmake
        build_project
    fi

    print_info "Build completed successfully!"
}

# Run main function with all arguments
main "$@"
