#!/usr/bin/env bash
# NLM Build Script - Simplified build process for users

# Color codes for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Default configuration
BUILD_DIR="build"
BUILD_TYPE="Release"
RUN_TESTS=true
INSTALL_PREFIX=""

# Print colored messages
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

# Show help
function show_help {
    cat << EOF
=== NLM Build Script ===

Usage: $0 [OPTIONS]

Options:
  --help                    Show this help message
  --build-dir DIR           Set build directory (default: build)
  --type TYPE              Build type: Release, Debug, RelWithDebInfo (default: Release)
  --no-tests               Don't run tests after build
  --install-prefix PREFIX  Installation prefix (default: system prefix)

Examples:
  $0                         Build Release version with tests
  $0 --type Debug             Build Debug version
  $0 --build-dir mybuild      Build to custom directory
  $0 --no-tests               Build without running tests

Quick Start:
  1. mkdir build
  2. cd build
  3. cmake .. -DCMAKE_BUILD_TYPE=Release
  4. make -j$(nproc)

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help)
            show_help
            exit 0
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --no-tests)
            RUN_TESTS=false
            shift
            ;;
        --install-prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Check for required tools
check_requirements() {
    print_info "Checking build requirements..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is not installed or not in PATH"
        print_info "Please install CMake from https://cmake.org/download/"
        exit 1
    fi
    
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "No C++ compiler found. Please install g++ or clang++."
        exit 1
    fi
    
    if ! command -v make &> /dev/null; then
        print_error "Make is not installed or not in PATH"
        exit 1
    fi
    
    print_success "All requirements found"
}

# Create build directory
create_build_dir() {
    if [ ! -d "$BUILD_DIR" ]; then
        print_info "Creating build directory: $BUILD_DIR"
        mkdir -p "$BUILD_DIR"
        print_success "Build directory created"
    else
        print_info "Using existing build directory: $BUILD_DIR"
    fi
}

# Configure CMake
configure_cmake() {
    cd "$BUILD_DIR"
    
    print_info "Configuring CMake with type: $BUILD_TYPE"
    
    local cmake_cmd="cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    
    if [ -n "$INSTALL_PREFIX" ]; then
        cmake_cmd="$cmake_cmd -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
    fi
    
    # Add platform-specific optimizations for Release builds
    if [ "$BUILD_TYPE" = "Release" ]; then
        cmake_cmd="$cmake_cmd -DCMAKE_CXX_FLAGS=\"-O3 -march=native\""
    fi
    
    eval $cmake_cmd
    
    if [ $? -eq 0 ]; then
        print_success "CMake configuration completed"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
}

# Build the project
build_project() {
    cd "$BUILD_DIR"
    
    print_info "Building NLM project..."
    print_info "This may take several minutes depending on system resources"
    
    # Determine number of parallel jobs
    local num_jobs=$(nproc)
    if [ "$num_jobs" -lt 2 ]; then
        num_jobs=2
    fi
    
    print_info "Using $num_jobs parallel jobs"
    
    # Build with progress output
    if command -v progress &> /dev/null; then
        make -j$num_jobs --progress
    else
        make -j$num_jobs
    fi
    
    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Run tests
run_tests() {
    if [ "$RUN_TESTS" = false ]; then
        print_warning "Skipping tests (--no-tests specified)"
        return
    fi
    
    print_info "Running tests..."
    
    if ! command -v ctest &> /dev/null; then
        print_warning "CTest not found, skipping tests"
        return
    fi
    
    # Run tests with verbose output
    ctest --output-on-failure --verbose
    
    if [ $? -eq 0 ]; then
        print_success "All tests passed"
    else
        print_error "Some tests failed"
        exit 1
    fi
}

# Show build results
show_results() {
    cd "$BUILD_DIR"
    
    echo ""
    print_success "=== Build Complete ==="
    echo ""
    
    # List executables
    echo "Generated executables:"
    if [ -f "nlm" ]; then
        echo "  nlm                 - Main simulation executable"
    fi
    if [ -f "nlm_phase3_demo" ]; then
        echo "  nlm_phase3_demo     - Phase 3 demonstration"
    fi
    if [ -f "nlm_phase4_demo" ]; then
        echo "  nlm_phase4_demo     - Phase 4 demonstration"
    fi
    if [ -f "nlm_test" ]; then
        echo "  nlm_test           - Unit test suite"
    fi
    
    # List libraries
    echo ""
    echo "Generated libraries:"
    if [ -f "libnlm_core.a" ]; then
        echo "  libnlm_core.a       - Core brain components"
    fi
    if [ -f "libnlm_agent.a" ]; then
        echo "  libnlm_agent.a      - Agent system"
    fi
    if [ -f "libnlm_world.a" ]; then
        echo "  libnlm_world.a      - World simulation"
    fi
    
    # Installation instructions
    if [ "$INSTALL_PREFIX" ]; then
        echo ""
        echo "To install to $INSTALL_PREFIX:"
        echo "  make install"
    fi
    
    echo ""
    echo "Next steps:"
    echo "  1. Run './nlm' to test the main executable"
    echo "  2. Run './nlm_phase3_demo' for Phase 3 demonstration"
    echo "  3. Run './nlm_test' to run the test suite"
    echo "  4. Run '$0 help' for more options"
}

# Main build process
main() {
    print_info "Starting NLM build process..."
    print_info "Build type: $BUILD_TYPE"
    print_info "Build directory: $BUILD_DIR"
    print_info "Run tests: $RUN_TESTS"
    
    check_requirements
    create_build_dir
    configure_cmake
    build_project
    run_tests
    show_results
    
    print_success "Build process completed successfully!"
}

# Run main function
main
