#!/usr/bin/env bash
# NLM Build and Test Script
# This script provides common build, test, and run commands for the NLM project

# Set strict mode for bash scripting
set -euo pipefail

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration variables
BUILD_TYPE="Release"
BUILD_DIR="build"
INSTALL_PREFIX="/usr/local"
TEST_TARGET="test_nlm"

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

# Function to check CMake version
check_cmake_version() {
    local min_version="3.16"
    local cmake_version="$(cmake --version | head -1 | awk '{print $3}')"
    
    if ! command_exists cmake; then
        print_error "CMake is not installed"
        return 1
    fi
    
    # Compare version (simplified)
    local major_version="${cmake_version%%.*}"
    local minor_version="${cmake_version#*.}"
    local minor_version="${minor_version%%.*}"
    
    if [[ $major_version -gt 3 ]] || [[ $major_version -eq 3 && $minor_version -ge 16 ]]; then
        print_info "CMake version $cmake_version found (minimum $min_version)"
        return 0
    else
        print_error "CMake version $cmake_version is less than minimum $min_version"
        return 1
    fi
}

# Function to check C++ compiler
check_cpp_compiler() {
    if ! command_exists g++; then
        print_error "g++ compiler not found"
        return 1
    fi
    
    local cpp_version="$(g++ --version | head -1)"
    print_info "Found C++ compiler: $cpp_version"
    return 0
}

# Function to create build directory
create_build_dir() {
    if [[ ! -d "$BUILD_DIR" ]]; then
        print_info "Creating build directory: $BUILD_DIR"
        mkdir -p "$BUILD_DIR"
    else
        print_info "Build directory already exists: $BUILD_DIR"
    fi
}

# Function to configure CMake
configure_cmake() {
    local cmake_args=(
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
        "-DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
        "-DNLM_ENABLE_TESTS=ON"
    )
    
    if [[ "$BUILD_TYPE" == "Debug" ]]; then
        cmake_args+=("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
    fi
    
    print_info "Configuring CMake with args: ${cmake_args[*]}"
    cd "$BUILD_DIR" || {
        print_error "Cannot enter build directory: $BUILD_DIR"
        return 1
    }
    
    cmake "${cmake_args[@]}" ..
    if [[ $? -ne 0 ]]; then
        print_error "CMake configuration failed"
        return 1
    fi
    
    print_success "CMake configuration completed"
}

# Function to build the project
build_project() {
    local cores=$(nproc)
    print_info "Building with $cores CPU cores"
    
    if [[ "$BUILD_TYPE" == "Debug" ]]; then
        make -j$((cores * 2)) || {
            print_error "Build failed"
            return 1
        }
    else
        make -j$cores || {
            print_error "Build failed"
            return 1
        }
    fi
    
    print_success "Build completed successfully"
}

# Function to run tests
run_tests() {
    if [[ ! -f "./nlm" ]]; then
        print_error "Main executable not found. Build the project first."
        return 1
    fi
    
    # Run the main demo
    print_info "Running Phase 6 integration demo..."
    ./nlm_phase6_demo || {
        print_error "Phase 6 demo failed"
        return 1
    }
    
    # Run unit tests if available
    if [[ -f "./test_nlm" ]]; then
        print_info "Running unit tests..."
        ./test_nlm --verbose || {
            print_error "Unit tests failed"
            return 1
        }
    fi
    
    print_success "All tests completed successfully"
}

# Function to install the project
install_project() {
    if [[ ! -f "./nlm" ]]; then
        print_error "Main executable not found. Build the project first."
        return 1
    fi
    
    print_info "Installing to $INSTALL_PREFIX"
    make install || {
        print_error "Installation failed"
        return 1
    }
    
    print_success "Installation completed"
}

# Function to run benchmarks
run_benchmarks() {
    if [[ ! -f "./nlm_benchmark" ]]; then
        print_warning "Benchmark executable not found. Building benchmarks first..."
        make nlm_benchmark || {
            print_error "Benchmark build failed"
            return 1
        }
    fi
    
    print_info "Running benchmarks..."
    ./nlm_benchmark --help || {
        print_error "Benchmarks not available"
        return 1
    }
    
    print_success "Benchmarks completed"
}

# Function to run the example
run_example() {
    if [[ ! -f "./nlm" ]]; then
        print_error "Main executable not found. Build the project first."
        return 1
    fi
    
    # First compile the example
    if ! command_exists g++; then
        print_error "g++ compiler not found"
        return 1
    fi
    
    print_info "Compiling example..."
    g++ -std=c++17 -I../src -I. -o example example_main.cpp ../src/brain/Brain.cpp ../src/core/Config/Config.cpp ../src/core/Logger/Logger.cpp || {
        print_error "Example compilation failed"
        return 1
    }
    
    print_info "Running example..."
    ./example || {
        print_error "Example execution failed"
        return 1
    }
    
    print_success "Example completed"
}

# Function to show help
 show_help() {
    cat << EOF
NLM (熙然) Build and Test Script

Usage: $0 <command> [options]

Commands:
    build          Build the project (default)
    test           Run tests
    install        Install the project
    benchmark      Run benchmarks
    example        Run example program
    clean          Clean build directory
    help           Show this help message

Options:
    -t, --type TYPE      Build type (Release, Debug) (default: Release)
    -p, --prefix PREFIX  Installation prefix (default: $INSTALL_PREFIX)
    -j, --jobs JOBS      Number of parallel jobs (default: auto-detect)

Examples:
    $0 build                # Build the project
    $0 build --type=Debug  # Build in debug mode
    $0 test                 # Build and test
    $0 install              # Build and install
    $0 clean                # Clean build directory
EOF
}

# Function to clean build directory
clean_build() {
    if [[ -d "$BUILD_DIR" ]]; then
        print_info "Cleaning build directory: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned"
    else
        print_warning "Build directory does not exist: $BUILD_DIR"
    fi
}

# Parse command line arguments
command="build"
jobs=""

while [[ $# -gt 0 ]]; do
    case $1 in
        build|test|install|benchmark|example|clean|help)
            command="$1"
            shift
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -j|--jobs)
            jobs="$2"
            shift 2
            ;;
        -h|--help)
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

# Execute the requested command
case "$command" in
    build)
        check_cmake_version || exit 1
        check_cpp_compiler || exit 1
        create_build_dir
        configure_cmake
        build_project
        print_success "Build completed successfully"
        ;;
    test)
        check_cmake_version || exit 1
        check_cpp_compiler || exit 1
        create_build_dir
        configure_cmake
        build_project
        run_tests
        print_success "All tests completed successfully"
        ;;
    install)
        check_cmake_version || exit 1
        check_cpp_compiler || exit 1
        create_build_dir
        configure_cmake
        build_project
        install_project
        print_success "Installation completed successfully"
        ;;
    benchmark)
        check_cmake_version || exit 1
        check_cpp_compiler || exit 1
        create_build_dir
        configure_cmake
        build_project
        run_benchmarks
        print_success "Benchmarks completed successfully"
        ;;
    example)
        check_cpp_compiler || exit 1
        run_example
        print_success "Example completed successfully"
        ;;
    clean)
        clean_build
        print_success "Cleanup completed"
        ;;
    help)
        show_help
        ;;
    *)
        print_error "Invalid command: $command"
        show_help
        exit 1
        ;;
esac

print_info "NLM build and test script completed"
