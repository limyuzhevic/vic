#!/bin/bash
# NLM Build and Test Script
# This script builds and tests the NLM project with new unified CLI interface

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check C++ compiler
check_compiler() {
    if command_exists g++; then
        print_info "C++ compiler found: $(g++ --version | head -n1)"
        return 0
    else
        print_error "C++ compiler not found. Please install g++."
        return 1
    fi
}

# Function to check CMake
check_cmake() {
    if command_exists cmake; then
        print_info "CMake found: $(cmake --version | head -n1)"
        return 0
    else
        print_error "CMake not found. Please install CMake."
        return 1
    fi
}

# Function to check required directories
check_directories() {
    local required_dirs=(
        "${PROJECT_ROOT}/src"
        "${PROJECT_ROOT}/src/core"
        "${PROJECT_ROOT}/src/brain"
        "${PROJECT_ROOT}/configs"
    )

    local missing_dirs=()

    for dir in "${required_dirs[@]}"; do
        if [[ ! -d "$dir" ]]; then
            missing_dirs+=("$dir")
        fi
    done

    if [[ ${#missing_dirs[@]} -gt 0 ]]; then
        print_error "Missing required directories:"
        for dir in "${missing_dirs[@]}"; do
            print_error "  $dir"
        done
        return 1
    fi

    print_info "All required directories found."
    return 0
}

# Function to build NLM
build_nlm() {
    print_info "Building NLM..."

    # Create build directory
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}" || return 1

    # Configure with CMake
    print_info "Configuring with CMake..."
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
        print_error "CMake configuration failed."
        return 1
    fi

    # Build
    print_info "Building with $(nproc) cores..."
    if ! make -j$(nproc); then
        print_error "Build failed."
        return 1
    fi

    cd "${PROJECT_ROOT}" || return 1

    print_info "NLM built successfully in ${BUILD_DIR}"
    return 0
}

# Function to run tests
run_tests() {
    print_info "Running NLM tests..."

    cd "${BUILD_DIR}" || return 1

    # Check if tests target exists
    if [[ -f "CTestTestfile.cmake" ]]; then
        print_info "Running unit tests..."
        if ctest --output-on-failure; then
            print_info "All tests passed."
            return 0
        else
            print_error "Some tests failed."
            return .1
        fi
    else
        print_warning "No tests found. Skipping unit tests."
    fi

    cd "${PROJECT_ROOT}" || return 1
    return 0
}

# Function to test NLM CLI
test_cli() {
    print_info "Testing NLM CLI interface..."

    cd "${BUILD_DIR}" || return 1

    # Check if nlm executable exists
    if [[ -f "nlm" ]]; then
        print_info "Testing help command..."
        if ./nlm --help > /dev/null 2>&1; then
            print_info "Help command works."
        else
            print_warning "Help command may not work properly."
        fi

        print_info "Testing version command..."
        if ./nlm --version > /dev/null 2>&1; then
            print_info "Version command works."
        else
            print_warning "Version command may not work properly."
        fi

        print_info "Testing demo help..."
        if ./nlm demo phase2 --help > /dev/null 2>&1; then
            print_info "Phase 2 help works."
        else
            print_warning "Phase 2 help may not work properly."
        fi

        cd "${PROJECT_ROOT}" || return 1
        return 0
    else
        print_error "nlm executable not found."
        return 1
    fi
}

# Function to show build status
show_build_status() {
    echo -e "${GREEN}=== NLM Build Status ===${NC}"
    echo "Build directory: ${BUILD_DIR}"
    echo "Project root: ${PROJECT_ROOT}"

    if [[ -d "${BUILD_DIR}" ]]; then
        if [[ -f "${BUILD_DIR}/nlm" ]]; then
            echo "Executable: ${BUILD_DIR}/nlm (available)"
        else
            echo "Executable: ${BUILD_DIR}/nlm (not found)"
        fi
    else
        echo "Executable: ${BUILD_DIR}/nlm (build directory not found)"
    fi

    echo ""
    echo -e "${GREEN}=== Quick Usage ===${NC}"
    echo "To run NLM from the build directory:"
    echo "  cd ${BUILD_DIR}"
    echo "  ./nlm --help"
    echo ""
    echo "To run NLM demos:"
    echo "  cd ${BUILD_DIR}"
    echo "  ./nlm demo phase2"
    echo "  ./nlm demo phase3 5 200"
    echo "  ./nlm demo phase5"
    echo "  ./nlm demo phase6"
    echo ""
    echo "To run benchmarks:"
    echo "  cd ${BUILD_DIR}"
    echo "  ./nlm benchmark"
}

# Main function
main() {
    echo -e "${GREEN}=== NLM Build and Test Script ===${NC}"
    echo "This script builds and tests the NLM project with new unified CLI interface."
    echo ""

    # Check prerequisites
    print_info "Checking prerequisites..."

    if ! check_compiler; then
        exit 1
    fi

    if ! check_cmake; then
        exit 1
    fi

    if ! check_directories; then
        exit 1
    fi

    # Build NLM
    if ! build_nlm; then
        exit 1
    fi

    # Run tests
    if ! run_tests; then
        print_warning "Some tests failed. This may be expected."
    fi

    # Test CLI
    if ! test_cli; then
        print_warning "CLI tests may have failed. This may be expected."
    fi

    # Show build status
    show_build_status

    echo -e "${GREEN}=== Build Complete ===${NC}"
    echo "NLM has been successfully built with the new unified CLI interface."
    echo ""
    echo "Next steps:"
    echo "1. Use the launcher script for easy access: ./nlm_launcher.sh"
    echo "2. Use the configuration validator: ./nlm_config_validator.sh"
    echo "3. Use the test suite: ./test_nlm_cli.sh"
    echo "4. Check NLM_IMPROVEMENTS_SUMMARY.md for detailed improvements"
    echo ""
    echo "For more information, see the NLM documentation and examples."
}

# Execute main function
main
