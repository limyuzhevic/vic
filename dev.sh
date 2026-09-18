#!/bin/bash

# NLM Development and Testing Script
# This script provides shortcuts for common development tasks

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

function print_header() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

function print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

function print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

function print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Check if build directory exists
if [ ! -d "${BUILD_DIR}" ]; then
    print_error "Build directory not found. Run ./setup.sh first."
    exit 1
fi

# Parse command line arguments
COMMAND="${1:-help}"


case "$COMMAND" in
    "build")
        print_header "Building NLM"
        cd "${BUILD_DIR}"
        make -j$(nproc)
        print_success "Build completed"
        ;;

    "test")
        print_header "Running NLM Tests"
        cd "${BUILD_DIR}"
        if [ -f "CTestTestfile.cmake" ]; then
            ctest --output-on-failure
            print_success "All tests passed"
        else
            print_warning "No tests found. Build may have been incomplete."
        fi
        ;;

    "run")
        print_header "Running NLM Phase 6 Demo"
        cd "${BUILD_DIR}"
        if [ -f "nlm_phase6_demo" ]; then
            ./nlm_phase6_demo
            print_success "Phase 6 demo completed"
        else
            print_error "nlm_phase6_demo not found. Build may have been incomplete."
        fi
        ;;

    "run-main")
        print_header "Running NLM Main Executable"
        cd "${BUILD_DIR}"
        if [ -f "nlm" ]; then
            ./nlm
            print_success "Main executable completed"
        else
            print_error "nlm not found. Build may have been incomplete."
        fi
        ;;

    "docs")
        print_header "Generating Documentation"
        if [ -f "${SCRIPT_DIR}/docs/PHASE6_FINAL_AUDIT.md" ]; then
            echo "Phase 6 Final Audit documentation available at:"
            echo "  ${SCRIPT_DIR}/docs/PHASE6_FINAL_AUDIT.md"
            print_success "Documentation generated"
        else
            print_warning "Documentation not found"
        fi
        ;;

    "python")
        print_header "Testing Python Bindings"
        cd "${SCRIPT_DIR}"
        if python3 -c "import pynlm; print('Python bindings loaded successfully')" 2>/dev/null; then
            print_success "Python bindings are working"
        else
            print_error "Python bindings are not working"
        fi
        ;;

    "clean")
        print_header "Cleaning Build Directory"
        cd "${SCRIPT_DIR}"
        if [ -d "build" ]; then
            rm -rf build/*
            print_success "Build directory cleaned"
        fi
        ;;

    "rebuild")
        print_header "Rebuilding Everything"
        cd "${SCRIPT_DIR}"
        if [ -d "build" ]; then
            cd build
            cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja
            make -j$(nproc)
            cd ..
            if [ -f "pyproject.toml" ]; then
                pip3 install -e .
            fi
            print_success "Complete rebuild completed"
        fi
        ;;

    "all")
        print_header "Running Complete Build and Test Suite"
        cd "${BUILD_DIR}"
        cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja
        make -j$(nproc)
        
        if [ -f "CTestTestfile.cmake" ]; then
            ctest --output-on-failure
        fi
        
        cd ..
        if [ -f "pyproject.toml" ]; then
            pip3 install -e .
        fi
        
        print_success "Complete test suite completed"
        ;;

    "benchmark")
        print_header "Running Performance Benchmarks"
        cd "${BUILD_DIR}"
        if [ -f "nlm_benchmark" ]; then
            ./nlm_benchmark --mode comprehensive
            print_success "Benchmarks completed"
        else
            print_warning "Benchmark executable not found"
        fi
        ;;

    "visualize")
        print_header "Generating Visualization Data"
        cd "${BUILD_DIR}"
        if [ -f "nlm_visualize" ]; then
            ./nlm_visualize --input ./brain.bin --output ./frames/ --format animated
            print_success "Visualization data generated"
        else
            print_warning "Visualization executable not found"
        fi
        ;;

    "help")
        print_header "NLM Development Script - Help"
        echo "Available commands:"
        echo "  build          - Build NLM from source"
        echo "  test           - Run NLM tests"
        echo "  run            - Run NLM Phase 6 demo"
        echo "  run-main       - Run NLM main executable"
        echo "  python         - Test Python bindings"
        echo "  docs           - Show documentation files"
        echo "  clean          - Clean build directory"
        echo "  rebuild        - Complete rebuild"
        echo "  all            - Full build and test suite"
        echo "  benchmark      - Run performance benchmarks"
        echo "  visualize      - Generate visualization data"
        echo ""
        echo "Usage examples:"
        echo "  ./dev.sh build"
        echo "  ./dev.sh run"
        echo "  ./dev.sh rebuild"
        ;;

    *)
        print_error "Unknown command: $COMMAND"
        echo "Run './dev.sh help' for available commands."
        exit 1
        ;;
esac

echo "=== Script completed ==="