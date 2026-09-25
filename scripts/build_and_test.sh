#!/usr/bin/env bash
# NLM Build and Test Script
# This script provides comprehensive build, test, and development commands for the NLM project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

function print_header() {
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE} NLM - Neural Learning Machine${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""
}

function print_step() {
    echo -e "${GREEN}>> $1${NC}"
}

function print_substep() {
    echo -e "${YELLOW}  $1${NC}"
}

function print_error() {
    echo -e "${RED}ERROR: $1${NC}" >&2
}

function print_warning() {
    echo -e "${YELLOW}WARNING: $1${NC}"
}

function check_prerequisites() {
    print_step "Checking prerequisites..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is not installed. Please install CMake 3.16 or higher."
        return 1
    fi
    
    if ! command -v g++ &> /dev/null; then
        print_error "g++ is not installed. Please install g++."
        return 1
    fi
    
    if ! command -v pip3 &> /dev/null; then
        print_warning "pip3 is not installed. Python bindings may not work properly."
    fi
    
    print_substep "CMake $(cmake --version | head -n1)"
    print_substep "g++ $(g++ --version | head -n1)"
    
    if [ -f "${PROJECT_ROOT}/pyproject.toml" ]; then
        print_substep "Python project configured with pyproject.toml"
        if command -v pip3 &> /dev/null; then
            print_substep "pip3 available"
        fi
    fi
    
    return 0
}

function build_nlm() {
    print_step "Building NLM..."
    
    if [ -d "${BUILD_DIR}" ]; then
        print_substep "Removing previous build directory..."
        rm -rf "${BUILD_DIR}"
    fi
    
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    local cmake_options="-DCMAKE_BUILD_TYPE=Release"
    
    # Check for C++17 support
    if [[ $(g++ --version | head -n1) =~ "[0-9]\.[0-9]*\.[0-9]*" ]]; then
        version="${BASH_REMATCH[0]}"
        print_substep "Compiler version: ${version}"
    fi
    
    # Run CMake configuration
    print_substep "Running CMake configuration..."
    if ! cmake ${cmake_options} ..; then
        print_error "CMake configuration failed."
        return 1
    fi
    
    # Build with parallel jobs
    print_substep "Building with 4 parallel jobs..."
    if ! make -j4; then
        print_error "Build failed."
        return 1
    fi
    
    cd "${PROJECT_ROOT}"
    print_substep "Build completed successfully!"
    return 0
}

function run_unit_tests() {
    print_step "Running unit tests..."
    
    if [ ! -f "${BUILD_DIR}/nlm_phase2_tests" ]; then
        print_error "Unit test executable not found. Please build the project first."
        return 1
    fi
    
    print_substep "Running NLM unit tests..."
    if ! "${BUILD_DIR}/nlm_phase2_tests"; then
        print_error "Unit tests failed."
        return 1
    fi
    
    print_substep "Unit tests passed!"
    return 0
}

function build_python_bindings() {
    print_step "Building Python bindings..."
    
    if [ ! -f "${PROJECT_ROOT}/pyproject.toml" ]; then
        print_error "pyproject.toml not found. Python bindings may not be supported."
        return 1
    fi
    
    # Check if Python development tools are available
    if command -v python3 &> /dev/null; then
        print_substep "Python3 found: $(python3 --version)"
    else
        print_error "Python3 not found. Cannot build Python bindings."
        return 1
    fi
    
    # Install Python dependencies
    print_substep "Installing Python dependencies..."
    if [ -f "${PROJECT_ROOT}/requirements.txt" ]; then
        if ! pip3 install -r "${PROJECT_ROOT}/requirements.txt"; then
            print_error "Failed to install Python dependencies."
            return 1
        fi
    else
        print_warning "requirements.txt not found. Using pyproject.toml for dependencies."
    fi
    
    # Build Python module
    print_substep "Building Python module..."
    cd "${PROJECT_ROOT}"
    
    # Create a simple setup.py if needed
    if [ ! -f "${PROJECT_ROOT}/setup.py" ]; then
        print_substep "Creating setup.py..."
        cat > "${PROJECT_ROOT}/setup.py" << 'EOF'
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import os

class NLMBuildExt(build_ext):
    def run(self):
        # Build the C++ extension
        build_dir = os.path.join(os.path.dirname(__file__), 'build')
        if os.path.exists(build_dir):
            print(f"Using existing build directory: {build_dir}")
        else:
            print("Building C++ extension...")
            subprocess.run(['cmake', '-DCMAKE_BUILD_TYPE=Release', '.'], cwd=build_dir, check=True)
            subprocess.run(['make', '-j4'], cwd=build_dir, check=True)
        
        # Copy bindings
        if os.path.exists(os.path.join('python', 'bindings.cpp')):
            print("Python bindings found. To build Python module, install scikit-build-core.")
        
        super(NLMBuildExt, self).run()

setup(
    name='pynlm',
    version='0.1.0',
    description='Python bindings for NLM (Neural Learning Machine)',
    packages=['pynlm'],
    ext_modules=[Extension('pynlm', sources=[])],
    cmdclass={'build_ext': NLMBuildExt},
    install_requires=[],
    python_requires='>=3.8',
)
EOF
    fi
    
    print_substep "Python bindings setup complete."
    print_warning "Note: Full Python bindings require scikit-build-core and pybind11"
    print_warning "To install dependencies: pip3 install scikit-build-core pybind11 numpy pytest"
    return 0
}

function run_phase6_demo() {
    print_step "Running Phase 6 integration demo..."
    
    if [ ! -f "${BUILD_DIR}/nlm_phase6_demo" ]; then
        print_error "Phase 6 demo executable not found. Please build the project first."
        return 1
    fi
    
    print_substep "Running Phase 6 integration demo..."
    if ! "${BUILD_DIR}/nlm_phase6_demo" --steps 100 --neurons 500; then
        print_error "Phase 6 demo failed."
        return 1
    fi
    
    print_substep "Phase 6 demo completed successfully!"
    return 0
}

function run_experiment_suite() {
    print_step "Running experiment suite..."
    
    # Run unit tests
    if ! run_unit_tests; then
        return 1
    fi
    
    # Run Phase 6 demo
    if ! run_phase6_demo; then
        return 1
    fi
    
    print_substep "All experiments completed successfully!"
    return 0
}

function run_integration_tests() {
    print_step "Running integration tests..."
    
    # Check for common integration test files
    if [ -f "${PROJECT_ROOT}/test_integration.sh" ]; then
        print_substep "Running integration test script..."
        if ! bash "${PROJECT_ROOT}/test_integration.sh"; then
            return 1
        fi
    else
        print_warning "No integration test script found. Skipping integration tests."
    fi
    
    # Run Python tests if available
    if command -v python3 &> /dev/null; then
        if [ -f "${PROJECT_ROOT}/python/tests/test_pynlm.py" ]; then
            print_substep "Running Python tests..."
            cd "${PROJECT_ROOT}"
            if ! python3 -m pytest python/tests/ -v; then
                return 1
            fi
        else
            print_warning "No Python tests found. Skipping Python integration tests."
        fi
    fi
    
    print_substep "Integration tests completed successfully!"
    return 0
}

function show_help() {
    print_header
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  build          Build the NLM project"
    echo "  test           Run unit tests"
    echo "  demo           Run Phase 6 integration demo"
    echo "  experiment     Run full experiment suite"
    echo "  integration    Run integration tests"
    echo "  all            Build and run all tests"
    echo ""
    echo "Options:"
    echo "  --steps N      Number of simulation steps (default: 100)"
    echo "  --neurons N    Number of neurons (default: 500)"
    echo "  --help         Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build"
    echo "  $0 test --steps 200 --neurons 1000"
    echo "  $0 all"
    echo ""
}

function main() {
    local command="${1:-build}"
    local steps="${2:-100}"
    local neurons="${3:-500}"
    
    case "${command}" in
        "build")
            check_prerequisites
            build_nlm
            build_python_bindings
            ;;
        "test")
            check_prerequisites
            build_nlm
            run_unit_tests
            ;;
        "demo")
            check_prerequisites
            build_nlm
            run_phase6_demo
            ;;
        "experiment")
            check_prerequisites
            build_nlm
            run_experiment_suite
            ;;
        "integration")
            check_prerequisites
            build_nlm
            run_integration_tests
            ;;
        "all")
            check_prerequisites
            build_nlm
            run_unit_tests
            run_phase6_demo
            build_python_bindings
            ;;
        "help")
            show_help
            ;;
        *)
            print_error "Unknown command: ${command}"
            show_help
            exit 1
            ;;
    esac
    
    local exit_code=$?
    
    if [ ${exit_code} -eq 0 ]; then
        print_step "Command completed successfully!"
    else
        print_error "Command failed with exit code ${exit_code}"
    fi
    
    exit ${exit_code}
}

# Main script execution
main "$@"
