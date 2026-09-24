#!/usr/bin/env bash
# Comprehensive CMake build and testing script for NLM
# This script provides easy access to all CMake features

set -e

echo "NLM CMake Build and Testing Script"
echo "==================================="

# Show help
if [[ "$1" == "help" ]]; then
    echo "Usage: ./build.sh [command]"
    echo ""
    echo "Commands:"
    echo "  build          Build NLM (default)"
    echo "  rebuild        Rebuild NLM"
    echo "  install        Install NLM to system location"
    echo "  test           Run tests"
    echo "  package        Create distribution packages"
    echo "  docs           Generate documentation"
    echo "  clean          Clean build artifacts"
    echo "  help           Show this help message"
    exit 0
fi

# Default build directory
BUILD_DIR="${BUILD_DIR:-build}"

# Check CMake version
check_cmake() {
    if ! command -v cmake &> /dev/null; then
        echo "ERROR: CMake not found. Please install CMake 3.20 or higher."
        exit 1
    fi

    cmake_version=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+')
    echo "Found CMake ${cmake_version}"

    if [[ $(echo "${cmake_version} >= 3.20" | bc -l) -ne 1 ]]; then
        echo "ERROR: CMake 3.20 or higher is required"
        exit 1
    fi
}

# Configure build
cfg_build() {
    echo "Configuring NLM build..."
    echo "Build directory: ${BUILD_DIR}"
    echo "Options: ${*}"
    
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Default options
    CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
    
    # Parse additional options
    while [[ $# -gt 0 ]]; do
        case $1 in
            --shared)
                CMAKE_ARGS+=" -DBUILD_SHARED_LIBS=ON"
                shift
                ;;
            --tests)
                CMAKE_ARGS+=" -DNLM_BUILD_TESTS=ON"
                shift
                ;;
            --python)
                CMAKE_ARGS+=" -DNLM_BUILD_PYTHON=ON"
                shift
                ;;
            --sanitizers)
                CMAKE_ARGS+=" -DNLM_ENABLE_SANITIZERS=ON"
                shift
                ;;
            --profiling)
                CMAKE_ARGS+=" -DNLM_ENABLE_PROFILING=ON"
                shift
                ;;
            --experiments)
                CMAKE_ARGS+=" -DNLM_ENABLE_EXPERIMENTS=ON"
                shift
                ;;
            --ccache)
                CMAKE_ARGS+=" -DNLM_ENABLE_CCACHE=ON"
                shift
                ;;
            --debug)
                CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=Debug"
                shift
                ;;
            --prefix)
                CMAKE_ARGS+=" -DCMAKE_INSTALL_PREFIX=${2}"
                shift 2
                ;;
            *)
                echo "Unknown option: $1"
                echo "Run './build.sh help' for usage"
                exit 1
                ;;
        esac
    done
    
    # Run CMake configure
    eval "cmake .. ${CMAKE_ARGS}"
    echo "Configuration complete."
}

# Build target
build_target() {
    echo "Building NLM..."
    cd "${BUILD_DIR}"
    cmake --build . -- -j$(nproc)
    echo "Build complete."
}

# Run tests
run_tests() {
    echo "Running NLM tests..."
    cd "${BUILD_DIR}"
    
    if [[ -f "CTestTestfile.cmake" ]]; then
        ctest --output-on-failure
    else
        echo "No tests configured. Build with -DNLM_BUILD_TESTS=ON"
    fi
}

# Package distribution
create_package() {
    echo "Creating NLM distribution packages..."
    cd "${BUILD_DIR}"
    cpack --config CPackConfig.cmake
    echo "Package creation complete."
}

# Install to system
install_target() {
    echo "Installing NLM..."
    cd "${BUILD_DIR}"
    cmake --install . --prefix /usr/local
    echo "Installation complete."
}

# Clean build artifacts
clean_build() {
    echo "Cleaning build artifacts..."
    if [[ -d "${BUILD_DIR}" ]]; then
        rm -rf "${BUILD_DIR}"
        echo "Build directory removed."
    else
        echo "Build directory does not exist."
    fi
}

# Main command dispatcher
case "${1:-build}" in
    build)
        check_cmake
        cfg_build "${@:2}"
        build_target
        ;;
    rebuild)
        check_cmake
        clean_build
        cfg_build "${@:2}"
        build_target
        ;;
    install)
        check_cmake
        cfg_build "${@:2}"
        install_target
        ;;
    test)
        check_cmake
        cfg_build "${@:2}"
        run_tests
        ;;
    package)
        check_cmake
        cfg_build "${@:2}"
        create_package
        ;;
    clean)
        clean_build
        ;;
    *)
        echo "Unknown command: ${1}"
        echo "Run './build.sh help' for usage"
        exit 1
        ;;
esac
