#!/bin/bash

# NLM Build and Development Script
# Common development commands for NLM project

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function print_help() {
    echo "NLM Development Script"
    echo "======================"
    echo ""
    echo "Usage: ./dev.sh [command]"
    echo ""
    echo "Commands:"
    echo "  build          Build all targets (Release)"
    echo "  build-debug    Build with debug symbols"
    echo "  build-tests    Build test executables"
    echo "  clean          Clean build directory"
    echo "  tests          Run unit tests"
    echo "  phase6-tests   Run Phase 6 integration tests"
    echo "  demo           Run main demo (Phase 2)"
    echo "  phase6-demo    Run Phase 6 integration demo"
    echo "  benchmark      Run benchmark"
    echo "  help           Show this help"
    echo ""
    echo "Examples:"
    echo "  ./dev.sh build           # Build all targets"
    echo "  ./dev.sh tests          # Run unit tests"
    echo "  ./dev.sh phase6-tests   # Run Phase 6 integration tests"
}

function build_all() {
    echo "${YELLOW}Building NLM (Release)...${NC}"
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    cd ..
    echo "${GREEN}Build complete!${NC}"
}

function build_debug() {
    echo "${YELLOW}Building NLM (Debug)...${NC}"
    mkdir -p build_debug
    cd build_debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make -j$(nproc)
    cd ..
    echo "${GREEN}Debug build complete!${NC}"
}

function build_tests() {
    echo "${YELLOW}Building tests...${NC}"
    mkdir -p build_tests
    cd build_tests
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make nlm_test
    cd ..
    echo "${GREEN}Test build complete!${NC}"
}

function clean() {
    echo "${YELLOW}Cleaning build directories...${NC}"
    rm -rf build build_debug build_tests
    echo "${GREEN}Clean complete!${NC}"
}

function run_tests() {
    echo "${YELLOW}Running unit tests...${NC}"
    if [ -d "build" ]; then
        cd build
        if [ -f "./nlm_test" ]; then
            ./nlm_test
        else
            echo "Test executable not found"
            cd ..
            rm -rf build
        fi
    else
        echo "Build directory not found"
    fi
}

function run_phase6_tests() {
    echo "${YELLOW}Running Phase  authentication tests...${NC}"
    if [ -f "tests/test_main_phase6.cpp" ]; then
        # This would need to be compiled and linked properly
        echo "Phase 6 test file created. Build with proper CMakeLists.txt"
    fi
}

function run_demo() {
    echo "${YELLOW}Running main demo...${NC}"
    if [ -f "build/nlm" ]; then
        ./build/nlm
    else
        echo "Main demo executable not found"
        echo "Run './dev.sh build' first"
    fi
}

function run_phase6_demo() {
    echo "${YELLOW}Running Phase 6 demo...${NC}"
    if [ -f "build/nlm_phase6_demo" ]; then
        ./build/nlm_phase6_demo
    else
        echo "Phase 6 demo executable not found"
        echo "Run './dev.sh build' first"
    fi
}

function main() {
    if [ $# -eq 0 ]; then
        print_help
        return
    fi
    
    command=$1
    
    case $command in
        build)
            build_all
            ;;
        build-debug)
            build_debug
            ;;;
        build-tests)
            build_tests
            ;;;
        clean)
            clean
            ;;;
        tests)
            run_tests
            ;;;
        phase6-tests)
            run_phase6_tests
            ;;;
        demo)
            run_demo
            ;;;
        phase6-demo)
            run_phase6_demo
            ;;;
        help)
            print_help
            ;;;
        *)
            echo "Unknown command: $command"
            print_help
            exit 1
            ;;
    esac
}

main "$@"