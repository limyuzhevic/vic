#!/usr/bin/env bash
# NLM Development Helper Script
# This script provides common development commands for the NLM project

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
CONFIG_FILE="configs/default.cfg"

echo_green() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

echo_yellow() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

echo_red() {
    echo -e "${RED}[ERROR]${NC} $*"
}

help() {
    cat << EOF
NLM Development Helper Script

Available commands:

build [type]    Build the project (type: Debug, Release, RelWithDebInfo)
build-fast      Quick build with optimizations
build-debug     Build with debug symbols

run [config]   Run the main executable with optional config file
run-bench      Run performance benchmarks

install        Install the built software system-wide

format         Format source code with clang-format
lint           Check code quality with clang-tidy

check          Run basic code checks

docs           Generate documentation

test-quick     Run quick tests
 test-full     Run comprehensive tests (requires test setup)

clean          Clean build artifacts

status         Show project status and configuration

help           Show this help message

Examples:
  ./dev.sh build-release     # Build in Release mode
  ./dev.sh run-configs/myconfig.cfg  # Run with custom config
  ./dev.sh clean            # Clean build directory
EOF
}

build() {
    local build_type=${1:-${BUILD_TYPE}}
    
    echo_green "Building NLM in ${build_type} mode..."
    
    if [ ! -d "${BUILD_DIR}" ]; then
        echo_green "Creating build directory..."
        mkdir -p "${BUILD_DIR}"
    fi
    
    cd "${BUILD_DIR}" || return 1
    
    cmake .. -DCMAKE_BUILD_TYPE="${build_type}" \
        -DCMAKE_CXX_STANDARD=20 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
    
    if [ $? -eq 0 ]; then
        echo_green "Configuration successful. Building..."
        make -j$(nproc)
        
        if [ $? -eq 0 ]; then
            echo_green "Build completed successfully!"
            echo_green "Executable: ${BUILD_DIR}/nlm"
        else
            echo_red "Build failed!"
            return 1
        fi
    else
        echo_red "Configuration failed!"
        return 1
    fi
    
    cd - > /dev/null
}

build_fast() {
    echo_green "Building NLM with optimizations..."
    build RelWithDebInfo
}

build_debug() {
    echo_green "Building NLM with debug symbols..."
    build Debug
}

run() {
    local config_file=${1:-${CONFIG_FILE}}
    
    if [ ! -f "${config_file}" ]; then
        echo_red "Configuration file not found: ${config_file}"
        echo_green "Using default configuration..."
        config_file="${CONFIG_FILE}"
    fi
    
    if [ ! -f "${BUILD_DIR}/nlm" ]; then
        echo_red "Executable not found. Please build first!"
        echo_green "Running: ./dev.sh build-release"
        build_release
    fi
    
    echo_green "Running NLM with configuration: ${config_file}"
    "${BUILD_DIR}/nlm" "--config" "${config_file}"
}

run_bench() {
    if [ ! -f "${BUILD_DIR}/nlm" ]; then
        echo_red "Executable not found. Please build first!"
        echo_green "Running: ./dev.sh build-release"
        build_release
    fi
    
    echo_green "Running performance benchmarks..."
    "${BUILD_DIR}/nlm" benchmark --duration 60s
}

install() {
    if [ ! -f "${BUILD_DIR}/nlm" ]; then
        echo_red "Executable not found. Please build first!"
        echo_green "Running: ./dev.sh build-release"
        build_release
    fi
    
    echo_green "Installing NLM system-wide..."
    sudo mkdir -p /usr/local/bin
    sudo cp "${BUILD_DIR}/nlm" /usr/local/bin/
    sudo chmod +x /usr/local/bin/nlm
    
    echo_green "Installation complete!"
    echo_green "You can now run 'nlm' from anywhere"
}

format() {
    echo_green "Formatting source code..."
    find src include -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | while read -r file; do
        echo "  Formatting ${file}"
        clang-format -i "${file}"
    done
}

lint() {
    echo_green "Checking code quality with clang-tidy..."
    if command -v clang-tidy >/dev/null 2>&1; then
        find src include -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | while read -r file; do
            echo "  Linting ${file}"
            clang-tidy "${file}" -- -I include -I src
        done
    else
        echo_yellow "clang-tidy not found. Skipping linting."
        echo_yellow "Install with: sudo apt-get install clang-tidy"
    fi
}

check() {
    echo_green "Running basic code checks..."
    
    # Check for obvious syntax errors
    echo "Checking C++ syntax..."
    find src include -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | while read -r file; do
        g++ -std=c++20 -c -I include -I src "${file}" -o /dev/null
        if [ $? -eq 0 ]; then
            echo "  ✓ ${file}"
        else
            echo "  ✗ ${file}"
        fi
    done
}

docs() {
    echo_green "Generating documentation..."
    if command -v doxygen >/dev/null 2>&1; then
        if [ -f "Doxyfile" ]; then
            doxygen Doxyfile
            echo_green "Documentation generated in docs/"
        else
            echo_red "Doxyfile not found. Skipping documentation generation."
        fi
    else
        echo_yellow "doxygen not found. Skipping documentation generation."
        echo_yellow "Install with: sudo apt-get install doxygen"
    fi
}

test_quick() {
    echo_green "Running quick tests..."
    if [ -f "${BUILD_DIR}/nlm_test" ]; then
        "${BUILD_DIR}/nlm_test" --quick
    else
        echo_yellow "Test executable not found. Building tests..."
        build_test
        if [ -f "${BUILD_DIR}/nlm_test" ]; then
            "${BUILD_DIR}/nlm_test" --quick
        else
            echo_red "Test build failed!"
        fi
    fi
}

test_full() {
    echo_green "Running comprehensive tests..."
    echo_yellow "Note: This requires a complete test setup and may take a long time."
    echo_yellow "Available tests:"
    echo_yellow "  - Unit tests for core components"
    echo_yellow "  - Integration tests for brain systems"
    echo_yellow "  - Performance benchmarks"
    echo_yellow "  - Configuration validation tests"
    
    if [ -f "${BUILD_DIR}/nlm_test" ]; then
        "${BUILD_DIR}/nlm_test"
    else
        echo_red "Test executable not found!"
        echo_green "Running: ./dev.sh build-test"
        build_test
        if [ -f "${BUILD_DIR}/nlm_test" ]; then
            "${BUILD_DIR}/nlm_test"
        else
            echo_red "Test build failed!"
        fi
    fi
}

build_test() {
    echo_green "Building test suite..."
    build "${BUILD_TYPE}"  # Use same build type as main
    
    if [ -f "${BUILD_DIR}/Makefile" ]; then
        make nlm_test -j$(nproc)
    else
        echo_red "Cannot build tests. No Makefile found."
    fi
}
clean() {
    echo_green "Cleaning build artifacts..."
    
    if [ -d "${BUILD_DIR}" ]; then
        echo_green "Removing ${BUILD_DIR}/ directory..."
        rm -rf "${BUILD_DIR}"
    fi
    
    # Remove other common build artifacts
    rm -f *.o *.so *.a *.dylib
    rm -rf bin obj dist
    
    # Remove documentation if generated
    if [ -d "docs" ]; then
        echo_green "Removing docs/ directory..."
        rm -rf docs
    fi
    
    echo_green "Clean complete!"
}

status() {
    echo_green "NLM Project Status"
    echo_green "=================="
    
    # Check for build directory
    if [ -d "${BUILD_DIR}" ]; then
        echo_green "Build directory: ${BUILD_DIR}"
        
        if [ -f "${BUILD_DIR}/nlm" ]; then
            echo_green "✓ Main executable: ${BUILD_DIR}/nlm"
        else
            echo_yellow "✗ Main executable: Not found"
        fi
        
        if [ -f "${BUILD_DIR}/nlm_test" ]; then
            echo_green "✓ Test executable: ${BUILD_DIR}/nlm_test"
        else
            echo_yellow "✗ Test executable: Not found"
        fi
        
        # Show build information
        echo "Build configuration:"
        cat "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null | grep "CMAKE_BUILD_TYPE" | head -1
    else
        echo_yellow "Build directory: Not found"
        echo_green "Run: ./dev.sh build-release to build the project"
    fi
    
    # Check for configuration files
    echo ""
    echo "Configuration files:"
    
    if [ -f "${CONFIG_FILE}" ]; then
        echo_green "✓ Default configuration: ${CONFIG_FILE}"
    else
        echo_yellow "✗ Default configuration: Not found"
    fi
    
    # List all config files
    if [ -d "configs" ]; then
        echo "Additional config files in configs/:"
        ls -la configs/
    fi
    
    echo ""
    echo "Source files:"
    echo "  C++ source files in src/: $(($(find src -name "*.cpp" | wc -l)))"
    echo "  Header files in include/: $(($(find include -name "*.hpp" 2>/dev/null | wc -l)))"
    echo "  Documentation files: $(($(find . -name "*.md" -o -name "*.txt" | wc -l)))"
}

case "$1" in
    build|build-*)
        shift
        build "$1"
        ;;
    build_fast)
        build_fast
        ;;
    build_debug)
        build_debug
        ;;
    run)
        run "$1"
        ;;
    run-bench)
        run_bench
        ;;
    install)
        install
        ;;
    format)
        format
        ;;
    lint)
        lint
        ;;
    check)
        check
        ;;
    docs)
        docs
        ;;
    test-quick)
        test_quick
        ;;
    test-full)
        test_full
        ;;
    clean)
        clean
        ;;
    status)
        status
        ;;
    help|*)
        help
        ;;
    *)
        echo_red "Unknown command: $1"
        echo_green "Use './dev.sh help' for available commands"
        exit 1
        ;;
esac

exit 0
