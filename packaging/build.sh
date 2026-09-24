#!/bin/bash

# NLM Packaging Script
# This script builds and packages NLM for distribution

set -e  # Exit on error

# Configuration
PROJECT_NAME="nlm"
VERSION="0.1.0"
BUILD_DIR="${BUILD_DIR:-build}"
INSTALL_PREFIX="${INSTALL_PREFIX:-/usr/local}"
PACKAGE_TYPE="${PACKAGE_TYPE:-all}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --install-prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --package-type)
            PACKAGE_TYPE="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --build-dir DIR      Build directory (default: build)"
            echo "  --install-prefix PREFIX Installation prefix (default: /usr/local)"
            echo "  --package-type TYPE   Package type: all/rpm/deb (default: all)"
            echo "  --help               Show this help"
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Main build function
build_project() {
    log_info "Building NLM ${VERSION}"
    
    # Create build directory
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Configure CMake
    log_info "Configuring CMake..."
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
        -DNLM_ENABLE_TESTS=ON \
        -DNLM_ENABLE_VISUALIZATION=OFF \
        -DNLM_ENABLE_DOCUMENTATION=OFF \
        ..
    
    # Build project
    log_info "Building project..."
    make -j$(nproc)
    
    cd ..
}

# Package function
package_project() {
    log_info "Packaging NLM ${VERSION}"
    
    cd "${BUILD_DIR}"
    
    case "${PACKAGE_TYPE}" in
        "rpm")
            log_info "Creating RPM package..."
            cpack --config "${CMAKE_CURRENT_SOURCE_DIR}/packaging/RPM.template.in"
            ;;
        "deb")
            log_info "Creating DEB package..."
            cpack --config "${CMAKE_CURRENT_SOURCE_DIR}/packaging/DEBIAN.template.in"
            ;;
        "all")
            log_info "Creating both RPM and DEB packages..."
            cpack --config "${CMAKE_CURRENT_SOURCE_DIR}/packaging/RPM.template.in"
            cpack --config "${CMAKE_CURRENT_SOURCE_DIR}/packaging/DEBIAN.template.in"
            ;;
        *)
            log_error "Unknown package type: ${PACKAGE_TYPE}"
            exit 1
            ;;
    esac
    
    cd ..
}

# Install function
install_project() {
    log_info "Installing NLM to ${INSTALL_PREFIX}"
    
    cd "${BUILD_DIR}"
    make install
    cd ..
}

# Run tests function
run_tests() {
    log_info "Running tests..."
    
    cd "${BUILD_DIR}"
    ctest --output-on-failure
    cd ..
}

# Clean function
clean_project() {
    log_info "Cleaning build directory..."
    rm -rf "${BUILD_DIR}"
}

# Main execution
case "${1:-build}" in
    "build")
        build_project
        ;;
    "package")
        build_project
        package_project
        ;;
    "install")
        build_project
        install_project
        ;;
    "test")
        build_project
        run_tests
        ;;
    "clean")
        clean_project
        ;;
    "all")
        build_project
        package_project
        install_project
        run_tests
        ;;
    *)
        log_error "Unknown command: ${1}"
        echo "Usage: $0 {build|package|install|test|clean|all}"
        exit 1
        ;;
esac
