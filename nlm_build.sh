#!/usr/bin/env bash
# NLM Build and Run Script
# Provides unified interface for building, running, and managing NLM demos

set -e  # Exit on error

# Default configuration
BUILD_TYPE="Release"
GENERATE_DOCS=false
RUN_TESTS=false
CLEAN_BUILD=false
PYTHON_BINDINGS=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --docs)
            GENERATE_DOCS=true
            shift
            ;;
        --tests)
            RUN_TESTS=true
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --python)
            PYTHON_BINDINGS=true
            shift
            ;;
        --help)
            echo "NLM Build and Run Script"
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --build-type TYPE    Build type (Debug, Release, RelWithDebInfo)"
            echo "  --docs               Generate documentation"
            echo "  --tests              Run tests after build"
            echo "  --clean              Clean build directories before building"
            echo "  --python             Build Python bindings"
            echo "  --help               Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print status messages
print_status() {
    echo -e "${GREEN}[STATUS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to clean build directories
clean_build() {
    print_status "Cleaning build directories..."
    rm -rf build
    rm -rf checkpoints
    rm -f nlm.log
    print_status "Build directories cleaned"
}

# Function to create build directory
create_build_dir() {
    if [ ! -d "build" ]; then
        mkdir -p build
    fi
}

# Function to run CMake
run_cmake() {
    local build_type=$1
    print_status "Running CMake configuration..."
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=${build_type} \
        -DNLM_BUILD_PYTHON_BINDINGS=${PYTHON_BINDINGS}
    cd ..
}

# Function to run build
run_build() {
    print_status "Building NLM..."
    cd build
    make -j$(nproc)
    cd ..
}

# Function to run tests
run_tests() {
    print_status "Running NLM tests..."
    if [ -f "build/tests" ]; then
        ./build/tests
    else
        print_warning "Test executable not found"
    fi
}

# Function to generate documentation
generate_docs() {
    print_status "Generating documentation..."
    # Check for available documentation tools
    if command_exists doxygen; then
        print_status "Doxygen found, generating API documentation..."
        if [ ! -d "docs" ]; then
            mkdir -p docs
        fi
        doxygen Doxyfile 2>/dev/null || print_warning "Doxygen documentation generation failed"
    else
        print_warning "Doxygen not found, skipping documentation generation"
    fi
}

# Function to run demos
run_demo() {
    local demo_name=$1
    local demo_args=$2
    
    print_status "Running ${demo_name}..."
    if [ -f "build/${demo_name}" ]; then
        ./build/${demo_name} ${demo_args}
    else
        print_error "Demo executable '${demo_name}' not found"
        exit 1
    fi
}

# Function to run Python bindings test
run_python_test() {
    print_status "Testing Python bindings..."
    if command_exists python3; then
        cd python
        python3 -c "import pynlm; print('Python bindings loaded successfully')"
        cd ..
    else
        print_warning "Python3 not found, skipping Python bindings test"
    fi
}

# Main execution
if [ "$CLEAN_BUILD" = true ]; then
    clean_build
fi

create_build_dir

if [ "$GENERATE_DOCS" = true ]; then
    generate_docs
fi

run_cmake "${BUILD_TYPE}"
run_build

if [ "$RUN_TESTS" = true ]; then
    run_tests
fi

if [ "$PYTHON_BINDINGS" = true ]; then
    run_python_test
fi

print_status "Build completed successfully!"
print_status "Executables are available in the 'build' directory:"
ls -la build/
