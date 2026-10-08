#!/bin/bash
# Build script for NLM (熙然) - Neural Learning Machine
# Provides multiple build configurations and convenience commands

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

help() {
    cat << EOF
NLM Build and Run Script
========================

Usage: $0 [command] [options]

Commands:
  build [config]     Build the project (config: debug, release, fast, profile)
  run [args...]      Run the main NLM simulation
  demo [type]        Run a demo mode (phase3, phase4, phase6, agent)
  clean              Clean build directory
  test               Run tests
  install [prefix]   Install the project (optional prefix)
  config             Show current configuration
  help               Show this help message

Examples:
  $0 build release    Build with optimizations for production
  $0 run --help       Show command-line options for main executable
  $0 demo phase6      Run Phase 6 integration demo

For more details, see docs/BUILD.md
EOF
}

build_project() {
    local config="${1:-release}"
    
    case $config in
        debug|release|fast|profile)
            print_info "Building NLM with $config configuration..."
            ;;
        *)
            print_error "Unknown configuration: $config"
            print_info "Using 'release' configuration"
            config="release"
            ;;
    esac
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure CMake
    print_info "Configuring CMake with $config configuration..."
    case $config in
        debug)
            cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-O0 -g" \
                -DNLM_ENABLE_TESTS=ON -DNLM_ENABLE_PROFILING=ON
            ;;
        fast)
            cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2 -march=native" \
                -DNLM_ENABLE_OPTIMIZATIONS=ON
            ;;
        profile)
            cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2 -g" \
                -DNLM_ENABLE_PROFILING=ON -DNLM_ENABLE_PROFILING_GUI=ON
            ;;
        *)
            cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3"
            ;;
    esac
    
    # Build with parallel jobs
    print_info "Building NLM core library..."
    make -j$(nproc) nlm_core nlm_agent nlm_world
    
    print_info "Building NLM main executable..."
    make -j$(nproc) nlm
    
    # Build demo executables
    print_info "Building demo executables..."
    make -j$(nproc) nlm_phase3_demo nlm_phase4_demo
    
    print_success "Build complete! Executables are in: $BUILD_DIR"
}

run_nlm() {
    if [ ! -f "$BUILD_DIR/nlm" ]; then
        print_error "NLM executable not found. Please build first:"
        print_info "  $0 build [config]"
        return 1
    fi
    
    print_info "Starting NLM simulation..."
    cd "$BUILD_DIR"
    ./nlm "$@"
}

run_demo() {
    local demo_type="${1:-phase6}"
    local build_target=""
    
    case $demo_type in
        phase3)
            build_target="nlm_phase3_demo"
            ;;
        phase4)
            build_target="nlm_phase4_demo"
            ;;
        phase6)
            build_target="nlm_phase3_demo"  # Use phase3 demo as phase6 integration is in main
            ;;
        agent)
            build_target="nlm"  # Main executable with agent capabilities
            ;;
        *)
            print_error "Unknown demo type: $demo_type"
            print_info "Available: phase3, phase4, phase6, agent"
            help
            return 1
            ;;
    esac
    
    if [ ! -f "$BUILD_DIR/$build_target" ]; then
        print_error "Demo executable not found. Building..."
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake .. 2>/dev/null || {
            print_error "CMake configuration failed"
            return 1
        }
        make -j$(nproc) $build_target
    fi
    
    print_info "Running $demo_type demo..."
    cd "$BUILD_DIR"
    ./$build_target "$@"
}

clean_build() {
    if [ -d "$BUILD_DIR" ]; then
        print_info "Cleaning build directory..."
        cd "$PROJECT_ROOT"
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned"
    else
        print_info "Build directory does not exist"
    fi
}

run_tests() {
    if [ ! -d "$BUILD_DIR" ]; then
        print_info "Build directory not found. Building..."
        build_project
    fi
    
    cd "$BUILD_DIR"
    if [ -f "CTestTestfile.cmake" ]; then
        ctest --output-on-failure
    else
        print_warning "Tests not enabled. Building with tests..."
        build_project debug
        ctest --output-on-failure
    fi
}

install_project() {
    local prefix="${1:-/usr/local}"
    
    if [ ! -d "$BUILD_DIR" ]; then
        print_info "Build directory not found. Building..."
        build_project
    fi
    
    cd "$BUILD_DIR"
    print_info "Installing NLM to $prefix..."
    make install DESTDIR="$prefix" 2>/dev/null || {
        print_error "Install failed. Try running with sudo for system-wide installation"
        return 1
    }
    
    print_success "Installation complete"
}

show_config() {
    cat << EOF
NLM Configuration
=================

Build directory: $BUILD_DIR
Project root: $PROJECT_ROOT

Current build configuration:
$(if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
    grep -E "CMAKE_BUILD_TYPE|PYTHON_EXECUTABLE|NLM_SOURCE_DIR" "$BUILD_DIR/CMakeCache.txt" | head -10
else
    echo "No existing build found"
fi)

Build status:
$(if [ -f "$BUILD_DIR/nlm" ]; then
    echo "  ✓ NLM executable built"
else
    echo "  ✗ NLM executable not found"
fi)
$(if [ -f "$BUILD_DIR/nlm_phase3_demo" ]; then
    echo "  ✓ Phase3 demo executable built"
else
    echo "  ✗ Phase3 demo executable not found"
fi)
$(if [ -f "$BUILD_DIR/nlm_phase4_demo" ]; then
    echo "  ✓ Phase4 demo executable built"
else
    echo "  ✗ Phase4 demo executable not found"
fi)

Available commands:
  $0 build [config]     - Build the project
  $0 run [args...]      - Run the main NLM simulation
  $0 demo [type]        - Run a demo mode
  $0 test               - Run tests
  $0 clean              - Clean build directory
  $0 install [prefix]   - Install the project

See docs/BUILD.md for detailed build instructions
EOF
}

# Main script logic
case "${1:-}" in
    "build")
        build_project "${2:-}"
        ;;
    "run")
        run_nlm "${@:2}"
        ;;
    "demo")
        run_demo "${2:-}"
        ;;
    "clean")
        clean_build
        ;;
    "test")
        run_tests
        ;;
    "install")
        install_project "${2:-}"
        ;;
    "config")
        show_config
        ;;
    "help"|--help|-h)
        help
        ;;
    *)
        help
        print_error "Unknown command: ${1:-}"
        exit 1
        ;;
esac
