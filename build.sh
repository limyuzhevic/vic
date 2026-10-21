#!/bin/bash

# Build and Installation Script for NLM Neural Simulation Framework
# Simplified build script demonstrating the improved CMake configuration

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check for required tools
check_requirements() {
    print_info "Checking build requirements..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is required but not installed"
        exit 1
    fi
    
    if ! command -v g++ &> /dev/null; then
        print_error "C++ compiler is required but not installed"
        exit 1
    fi
    
    print_info "All build requirements satisfied"
}

# Configure build with options
configure_build() {
    print_info "Configuring NLM build..."
    
    # Create build directory if it doesn't exist
    mkdir -p "${BUILD_DIR}"
    
    # Configure with default options (can be overridden)
    cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DNLM_BUILD_PYTHON=ON \
        -DNLM_BUILD_TESTS=ON \
        -DNLM_BUILD_DEMOS=ON \
        ${EXTRA_CMAKE_ARGS:-}
    
    print_info "Build configuration complete"
}

# Build the project
build_project() {
    print_info "Building NLM neural simulation framework..."
    
    # Build with parallel jobs
    cmake --build "${BUILD_DIR}" --config Release -- -j$(nproc)
    
    print_info "Build complete"
}

# Run tests if available
run_tests() {
    print_info "Running NLM tests..."
    
    if [[ -f "${BUILD_DIR}/bin/nlm_test" ]]; then
        # Run the main test suite
        "${BUILD_DIR}/bin/nlm_test"
        print_info "All tests passed!"
    else
        print_warn "Test executable not found, skipping tests"
    fi
}

# Create installation directory
create_install_dir() {
    print_info "Creating installation directory structure..."
    
    INSTALL_DIR="${PROJECT_ROOT}/install"
    mkdir -p "${INSTALL_DIR}"
    
    # Create typical installation subdirectories
    mkdir -p "${INSTALL_DIR}/bin"
    mkdir -p "${INSTALL_DIR}/lib"
    mkdir -p "${INSTALL_DIR}/include"
    mkdir -p "${INSTALL_DIR}/share/doc"
    
    print_info "Installation directory structure created"
}

# Install the project
install_project() {
    print_info "Installing NLM to ${INSTALL_DIR:-"$PROJECT_ROOT/install"}..."
    
    local install_dir="${INSTALL_DIR:-${PROJECT_ROOT}/install}"
    
    # Run CMake install
    cmake --install "${BUILD_DIR}" \
        --prefix "${install_dir}" \
        --config Release
    
    print_info "Installation complete"
}

# Display build summary
show_summary() {
    print_info "=== NLM Build Summary ==="
    echo
    print_info "Build directory: ${BUILD_DIR}"
    print_info "Installation directory: ${INSTALL_DIR:-${PROJECT_ROOT}/install}"
    echo
    
    print_info "Key executables built:"
    if [[ -f "${BUILD_DIR}/bin/nlm" ]]; then
        echo "  - Neural simulation: ${BUILD_DIR}/bin/nlm"
    fi
    if [[ -f "${BUILD_DIR}/bin/nlm_phase3_demo" ]]; then
        echo "  - Phase 3 demo: ${BUILD_DIR}/bin/nlm_phase3_demo"
    fi
    if [[ -f "${BUILD_DIR}/bin/nlm_phase4_demo" ]]; then
        echo "  - Phase 4 demo: ${BUILD_DIR}/bin/nlm_phase4_demo"
    fi
    if [[ -f "${BUILD_DIR}/bin/nlm_test" ]]; then
        echo "  - Test suite: ${BUILD_DIR}/bin/nlm_test"
    fi
    echo
    
    print_info "Libraries built:"
    echo "  - Core neural simulation: ${BUILD_DIR}/lib/libnlm_core.a"
    echo "  - Agent system: ${BUILD_DIR}/lib/libnlm_agent.a"
    echo "  - World system: ${BUILD_DIR}/lib/libnlm_world.a"
    echo "  - Phase 3 experiments: ${BUILD_DIR}/lib/libnlm_phase3.a"
    echo "  - Phase 4 experiments: ${BUILD_DIR}/lib/libnlm_phase4.a"
    echo
    
    print_info "Headers installed in: ${install_dir}/include/nlm/"
    echo
}

# Show help information
show_help() {
    cat << EOF
NLM Neural Simulation Framework - Build Script

Usage: $0 [COMMAND] [OPTIONS]

Commands:
  configure   Configure the build (default)
  build       Build the project
  test        Run tests
  install     Install the project
  all         Configure, build, and install (default)
  clean       Clean build directory

Options:
  --build-dir DIR      Set build directory (default: build)
  --install-dir DIR    Set installation directory (default: ./install)
  --extra-cmake-args "ARGS" Pass additional CMake arguments

Examples:
  $0                      # Configure, build, and install with defaults
  $0 all --build-dir mybuild
  $0 install --install-dir /usr/local

For more detailed CMake options, modify the CMakeLists.txt file.
EOF
}

# Main execution
case "${1:-configure}" in
    configure)
        check_requirements
        configure_build
        ;;
        
    build)
        check_requirements
        build_project
        ;;
        
    test)
        run_tests
        ;;
        
    install)
        check_requirements
        configure_build
        build_project
        create_install_dir
        install_project
        show_summary
        ;;
        
    all)
        check_requirements
        configure_build
        build_project
        run_tests
        create_install_dir
        install_project
        show_summary
        ;;
        
    clean)
        print_info "Cleaning build directory..."
        rm -rf "${BUILD_DIR}"
        print_info "Build directory cleaned"
        ;;
        
    help|--help|-h)
        show_help
        ;;
        
    *)
        print_error "Unknown command: $1"
        show_help
        exit 1
        ;;
esac
