#!/usr/bin/env bash
#
# Enhanced NLM Build and Test Script
#
# This script provides convenient commands for building and testing
the enhanced NLM project with improved checkpointing and learning capabilities.
#

# Configuration
BUILD_DIR="build"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
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

print_header() {
    echo -e "${PURPLE}[HEADER]${NC} $1"
}

print_step() {
    echo -e "${CYAN}[STEP]${NC} $1"
}

# Function to check if command exists
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

# Function to install dependencies
install_dependencies() {
    local pkg_manager=$(detect_package_manager)
    
    if [ "$pkg_manager" = "unknown" ]; then
        print_error "Package manager not detected. Please install dependencies manually."
        print_info "Common dependencies for NLM:"
        print_info "  - CMake >= 3.16"
        print_info "  - GCC >= 9.0 or Clang >= 10.0"
        print_info "  - Make"
        print_info "  - Git"
        return 1
    fi
    
    print_step "Installing dependencies using $pkg_manager..."
    
    case $pkg_manager in
        apt-get)
            sudo apt-get update
            sudo apt-get install -y cmake build-essential git pkg-config
            ;;
        yum|dnf)
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y cmake git
            ;;
        pacman)
            sudo pacman -Sy cmake gcc make git
            ;;
    esac
    
    print_success "Dependencies installed successfully"
}

# Function to configure build
configure_build() {
    local build_type="Release"
    local enable_checkpoint_demo="ON"
    local enable_testing="ON"
    local enable_profiling="OFF"
    local enable_sanitizers="OFF"
    
    print_step "Configuring NLM build..."
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug)
                build_type="Debug"
                shift
                ;;
            --enable-profiling)
                enable_profiling="ON"
                shift
                ;;
            --enable-sanitizers)
                enable_sanitizers="ON"
                shift
                ;;
            --disable-checkpoint-demo)
                enable_checkpoint_demo="OFF"
                shift
                ;;
            --disable-testing)
                enable_testing="OFF"
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
    
    # Configure CMake
    local cmake_cmd="cmake -B \"$BUILD_DIR\" -D NLM_BUILD_CHECKPOINT_DEMO=${enable_checkpoint_demo} \
            -D NLM_BUILD_TESTING=${enable_testing} -D NLM_ENABLE_PROFILING=${enable_profiling} \
            -D NLM_ENABLE_SANITIZERS=${enable_sanitizers} -DCMAKE_BUILD_TYPE=${build_type}"
    
    print_info "Running: $cmake_cmd"
    
    if eval "$cmake_cmd"; then
        print_success "Build configured successfully"
    else
        print_error "Build configuration failed"
        return 1
    fi
}

# Function to build the project
build_project() {
    print_step "Building NLM project..."
    
    if [ ! -d "$BUILD_DIR" ]; then
        print_error "Build directory not found. Run configure script first."
        return 1
    fi
    
    cd "$BUILD_DIR"
    
    # Build with parallel jobs
    local jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    print_info "Building with $jobs parallel jobs..."
    
    if make -j$jobs; then
        print_success "Build completed successfully"
        print_info "Executables are in: $BUILD_DIR/bin"
    else
        print_error "Build failed"
        return 1
    fi
    
    cd "$PROJECT_ROOT"
}

# Function to run tests
run_tests() {
    print_step "Running NLM tests..."
    
    if [ ! -d "$BUILD_DIR" ]; then
        print_error "Build directory not found. Run configure script first."
        return 1
    fi
    
    cd "$BUILD_DIR"
    
    # Run CTest
    if ctest --output-on-failure; then
        print_success "All tests passed"
    else
        print_error "Some tests failed"
        return 1
    fi
    
    cd "$PROJECT_ROOT"
}

# Function to run enhanced checkpoint demo
run_enhanced_checkpoint_demo() {
    print_step "Running enhanced checkpoint demo..."
    
    local demo_path="$BUILD_DIR/bin/nlm_enhanced_checkpoint_demo"
    
    if [ ! -f "$demo_path" ]; then
        print_error "Enhanced checkpoint demo not found. Build the project first."
        return 1
    fi
    
    print_info "Running enhanced checkpoint demo..."
    print_info "Check './enhanced_checkpoints/' for saved checkpoints"
    print_info "Press Ctrl+C to stop (save checkpoint will be created automatically)"
    
    # Run the demo
    "$demo_path"
}

# Function to run Phase 3 demo
run_phase3_demo() {
    print_step "Running Phase 3 demo..."
    
    local demo_path="$BUILD_DIR/bin/nlm_phase3_demo"
    
    if [ ! -f "$demo_path" ]; then
        print_error "Phase 3 demo not found. Build the project first."
        return 1
    fi
    
    print_info "Running Phase 3 demo (world interaction)..."
    
    # Run with default parameters
    "$demo_path" 5 200
}

# Function to run Phase 6 demo
run_phase6_demo() {
    print_step "Running Phase 6 demo..."
    
    local demo_path="$BUILD_DIR/bin/nlm_phase6_demo"
    
    if [ ! -f "$demo_path" ]; then
        print_error "Phase 6 demo not found. Build the project first."
        return 1
    fi
    
    print_info "Running Phase 6 demo (integration test)..."
    
    # Run with default parameters
    "$demo_path"
}

# Function to show help
show_help() {
    cat << EOF
Enhanced NLM Build and Test Script

Usage: $0 [OPTIONS]

Commands:
  configure [OPTIONS]  Configure the build system
  build                Build the project
  test                 Run all tests
  run-enhanced-demo    Run enhanced checkpoint demo
  run-phase3-demo      Run Phase 3 demo
  run-phase6-demo      Run Phase 6 demo
  clean                Clean build artifacts
  help                 Show this help message

Options for configure:
  --debug              Build in Debug mode (default: Release)
  --enable-profiling   Enable performance profiling
  --enable-sanitizers  Enable memory sanitizers
  --disable-checkpoint-demo Disable enhanced checkpoint demo
  --disable-testing    Disable test suite
  -h, --help           Show help message

Examples:
  $0 configure --debug --enable-profiling
  $0 build
  $0 test
  $0 run-enhanced-demo
  $0 run-phase3-demo
  $0 run-phase6-demo

Files created:
  build/          CMake build directory
  build/bin/      Executables
  enhanced_checkpoints/  Checkpoint files (from enhanced demo)

For more information, see the README.md file.

Enhanced Features:
  - Improved checkpointing system with better error handling
  - Enhanced reward reporting with meaningful values
  - Improved learning dynamics with pattern selectivity
  - Comprehensive testing and validation
  - Better documentation and examples
EOF
}

# Function to clean build artifacts
clean_build() {
    print_step "Cleaning build artifacts..."
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Build directory removed: $BUILD_DIR"
    fi
    
    # Remove enhanced checkpoints directory
    if [ -d "enhanced_checkpoints" ]; then
        rm -rf enhanced_checkpoints
        print_success "Enhanced checkpoints directory removed"
    fi
    
    print_info "Clean completed"
}

# Function to show build status
show_build_status() {
    print_header "=== NLM Enhanced Build Status ==="
    
    if [ -d "$BUILD_DIR" ]; then
        echo "Build directory: $BUILD_DIR"
        echo "Build type: $(grep CMAKE_BUILD_TYPE "$BUILD_DIR/CMakeCache.txt" | cut -d '=' -f2 2>/dev/null || echo 'Unknown')"
        
        echo ""
        echo "Targets in $BUILD_DIR/bin:"
        if [ -d "$BUILD_DIR/bin" ]; then
            for exe in "$BUILD_DIR/bin"/*; do
                if [ -f "$exe" ] && [ -x "$exe" ]; then
                    local name=$(basename "$exe")
                    local size=$(stat -c%s "$exe" 2>/dev/null || stat -f%z "$exe" 2>/dev/null || echo "?")
                    echo "  $name ($(numfmt --to=iec-i3 $size 2>/dev/null || echo "$size bytes"))"
                fi
            done
        fi
        
        echo ""
        echo "Test results:"
        if [ -f "$BUILD_DIR/CTestTestfile.cmake" ]; then
            echo "  Tests configured in $BUILD_DIR"
            echo "  Run 'cd $BUILD_DIR && ctest --output-on-failure' to run tests"
        fi
        
        echo ""
        echo "Checkpoint files:"
        if [ -d "enhanced_checkpoints" ]; then
            local count=$(find enhanced_checkpoints -name "*.bin" | wc -l)
            echo "  $count checkpoint files in enhanced_checkpoints/"
        else
            echo "  No enhanced_checkpoints directory found"
        fi
    else
        echo "No build directory found. Run '$0 configure' first."
    fi
}

# Main script logic
main() {
    # Check if script is being sourced
    if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
        # This is a standalone script execution
        if [ $# -eq 0 ]; then
            show_help
            exit 1
        fi
        
        case "$1" in
            configure)
                shift
                install_dependencies
                configure_build "$@"
                ;;
            build)
                build_project
                ;;
            test)
                run_tests
                ;;
            run-enhanced-demo)
                run_enhanced_checkpoint_demo
                ;;
            run-phase3-demo)
                run_phase3_demo
                ;;
            run-phase6-demo)
                run_phase6_demo
                ;;
            clean)
                clean_build
                ;;
            help|--help|-h)
                show_help
                ;;
            status)
                show_build_status
                ;;
            *)
                print_error "Unknown command: $1"
                show_help
                exit 1
                ;;
        esac
    else
        # Script is being sourced, just print help
        echo "This script is meant to be run directly, not sourced."
        show_help
    fi
}

# Run main function with original arguments
main "$@"