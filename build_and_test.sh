#!/bin/bash

# NLM Build and Test Script
# This script provides convenient commands for building and testing the NLM project

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

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

# Check if we are in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the project root directory."
    exit 1
fi

# Function to build the C++ core
build_cpp() {
    print_info "Building NLM C++ core..."
    
    if [ ! -d "build" ]; then
        mkdir build
    fi
    
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    
    if [ $? -eq 0 ]; then
        print_info "C++ build completed successfully!"
        cd ..
    else
        print_error "C++ build failed!"
        cd ..
        exit 1
    fi
}

# Function to build Python bindings
build_python() {
    print_info "Building NLM Python bindings..."
    
    if [ ! -f "pyproject.toml" ]; then
        print_error "pyproject.toml not found. This is required for Python bindings."
        exit 1
    fi
    
    pip install -e .
    
    if [ $? -eq 0 ]; then
        print_info "Python bindings installed successfully!"
    else
        print_error "Python bindings installation failed!"
        exit (right)
    fi
}

# Function to run tests
run_tests() {
    print_info "Running NLM tests..."
    
    # First, ensure the build directory exists
    if [ ! -d "build" ]; then
        print_warning "Build directory not found. Building first..."
        build_cpp
    fi
    
    cd build
    
    # Run C++ tests
    if [ -f "CTestTestfile.cmake" ]; then
        print_info "Running C++ tests..."
        ctest --output-on-failure
        
        if [ $? -eq 0 ]; then
            print_info "C++ tests passed!"
        else
            print_error "C++ tests failed!"
            cd ..
            exit 1
        fi
    else
        print_warning "No C++ tests found!"
    fi
    
    # Run Python tests if pytest is available
    cd ..
    if command -v python3 &> /dev/null; then
        if python3 -m pytest --version &> /dev/null; then
            print_info "Running Python tests..."
            python3 -m pytest tests/ -v
            
            if [ $? -eq 0 ]; then
                print_info "Python tests passed!"
            else
                print_error "Python tests failed!"
                exit 1
            fi
        else
            print_warning "pytest not found. Skipping Python tests."
        fi
    fi
    
    print_info "All tests completed!"
}

# Function to run demos
run_demos() {
    print_info "Running NLM demos..."
    
    if [ ! -d "build" ]; then
        print_warning "Build directory not found. Building first..."
        build_cpp
    fi
    
    cd build
    
    # List available demos
    echo "Available demos:"
    echo "1. ./nlm              - Phase 2: Basic Neural Computation"
    echo "2. ./nlm_phase3_demo   - Phase 3: World Interaction"
    echo "3. ./nlm_phase4_demo   - Phase 4: Cognitive Mechanisms"
    echo "4. ./nlm_phase5_demo   - Phase 5: Performance Optimizations"
    echo "5. ./nlm_phase6_demo   - Phase 6: Comprehensive Integration"
    echo "6. ./nlm_phase6_text_demo - Phase 6: Text Integration"
    echo "7. ./nlm_benchmark     - Performance Benchmarks"
    
    echo ""
    read -p "Enter demo number to run (or 'all' to run all demos): " choice
    
    case $choice in
        1)
            print_info "Running Phase 2 demo (nlm)..."
            ./nlm
            ;;
        2)
            print_info "Running Phase 3 demo (nlm_phase3_demo)..."
            ./nlm_phase3_demo
            ;;
        3)
            print_info "Running Phase 4 demo (nlm_phase4_demo)..."
            ./nlm_phase4_demo
            ;;
        4)
            print_info "Running Phase 5 demo (nlm_phase5_demo)..."
            ./nlm_phase5_demo
            ;;
        5)
            print_info "Running Phase 6 demo (nlm_phase6_demo)..."
            ./nlm_phase6_demo
            ;;
        6)
            print_info "Running Phase 6 text demo (nlm_phase6_text_demo)..."
            ./nlm_phase6_text_demo
            ;;
        7)
            print_info "Running benchmark (nlm_benchmark)..."
            ./nlm_benchmark
            ;;
        all)
            print_info "Running all demos..."
            for demo in nlm nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_phase6_text_demo nlm_benchmark; do
                if [ -f "$demo" ]; then
                    echo ""
                    print_info "Running $demo..."
                    ./$demo
                else
                    print_warning "$demo not found, skipping..."
                fi
            done
            ;;
        *)
            print_error "Invalid choice!"
            cd ..
            exit 1
            ;;
    esac
    
    cd ..
}

# Function to clean build artifacts
clean() {
    print_info "Cleaning build artifacts..."
    
    if [ -d "build" ]; then
        rm -rf build
        print_info "Build directory removed."
    else
        print_warning "Build directory not found."
    fi
    
    # Remove Python build artifacts
    if [ -d ".pytest_cache" ]; then
        rm -rf .pytest_cache
        print_info "Python test cache removed."
    fi
    
    # Remove checkpoint files
    if ls /tmp/nlm_test_checkpoint* 1> /dev/null 2>&1; then
        rm -f /tmp/nlm_test_checkpoint*
        print_info "Checkpoint files removed."
    fi
    
    print_info "Cleanup completed."
}

# Function to show help
help() {
    echo "NLM Build and Test Script"
    echo "========================="
    echo ""
    echo "Usage: ./build_and_test.sh [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  build-cpp     - Build the C++ core library"
    echo "  build-python  - Build Python bindings (pip install -e .)"
    echo "  build-all     - Build both C++ core and Python bindings"
    echo "  tests         - Run all tests (C++ and Python)"
    echo "  demos         - Run demo executables"
    echo "  clean         - Clean build artifacts"
    echo "  help          - Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./build_and_test.sh build-all    # Build everything"
    echo "  ./build_and_test.sh tests        # Run all tests"
    echo "  ./build_and_test.sh clean        # Clean up"
}

# Main script logic
case "$1" in
    build-cpp)
        build_cpp
        ;;
    build-python)
        build_python
        ;;
    build-all)
        build_cpp
        build_python
        ;;
    tests)
        run_tests
        ;;
    demos)
        run_demos
        ;;
    clean)
        clean
        ;;
    help|*)  # Default to help
        help
        ;;
esac

exit 0