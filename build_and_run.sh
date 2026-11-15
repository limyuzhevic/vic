#!/bin/bash
# NLM Build and Run Script
# Provides convenient build, test, and run commands for the NLM neural simulation framework

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
RUN_DEMO=""
RUN_PHASE=""
RUN_TESTS="false"
CLEAN_BUILD="false"
PYTHON_BINDINGS="false"
VERBOSE="false"

# Help function
function show_help() {
    echo "NLM Build and Run Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help                    Show this help message"
    echo "  -t, --type TYPE              Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)"
    echo "  -d, --build-dir DIR          Build directory (default: build)"
    echo "  -c, --clean                  Clean build directory before building"
    echo "  -v, --verbose                Verbose build output"
    echo "  --python-bindings           Enable Python bindings"
    echo ""
    echo "Run Options:"
    echo "  --run-demo                  Run main demo after build"
    echo "  --run-phase3                Run Phase 3 demo"
    echo "  --run-phase4                Run Phase 4 demo"
    echo "  --run-phase5                Run Phase 5 demo"
   icolo=""
    echo "  --run-phase6                Run Phase 6 demo"
    echo "  --run-benchmark             Run benchmark"
    echo ""
    echo "Test Options:"
    echo "  --run-tests                 Run tests after build"
    echo ""
    echo "Examples:"
    echo "  $0                          # Build and run main demo"
    echo "  $0 --run-phase6 --run-tests # Build, run tests, and run Phase 6 demo"
    echo "  $0 --clean --type Debug     # Clean debug build"
    echo ""
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -d|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN_BUILD="true"
            shift
            ;;
        -v|--verbose)
            VERBOSE="true"
            shift
            ;;
        --python-bindings)
            PYTHON_BINDINGS="true"
            shift
            ;;
        --run-demo)
            RUN_DEMO="true"
            shift
            ;;
        --run-phase3)
            RUN_PHASE="phase3"
            shift
            ;;
        --run-phase4)
            RUN_PHASE="phase4"
            shift
            ;;
        --run-phase5)
            RUN_PHASE="phase5"
            shift
            ;;
        --run-phase6)
            RUN_PHASE="phase6"
            shift
            ;;
        --run-benchmark)
            RUN_DEMO="benchmark"
            shift
            ;;
        --run-tests)
            RUN_TESTS="true"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Functions
function build_nlm() {
    echo "Building NLM neural simulation framework..."
    echo "Build type: $BUILD_TYPE"
    echo "Build directory: $BUILD_DIR"
    
    if [ "$CLEAN_BUILD" = "true" ] && [ -d "$BUILD_DIR" ]; then
        echo "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure CMake
    local cmake_args=".. -DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    
    if [ "$PYTHON_BINDINGS" = "true" ]; then
        cmake_args="$cmake_args -DNLM_BUILD_PYTHON_BINDINGS=ON"
    fi
    
    echo "Running CMake configuration..."
    cmake $cmake_args
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}CMake configuration failed!${NC}"
        exit 1
    fi
    
    # Build
    local make_args="-j$(nproc)"
    if [ "$VERBOSE" = "true" ]; then
        make_args="$make_args VERBOSE=1"
    fi
    
    echo "Building with $make_args..."
    make $make_args
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Build failed!${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}Build successful!${NC}"
    cd ".."
}

function run_demo() {
    local demo_name="$1"
    local demo_path="$BUILD_DIR/$demo_name"
    
    if [ ! -f "$demo_path" ]; then
        echo -e "${RED}Demo executable not found: $demo_path${NC}"
        echo "Available demos:"
        ls -la "$BUILD_DIR"/*.exe 2>/dev/null || ls -la "$BUILD_DIR"/* 2>/dev/null | grep -E "(nlm_|phase|benchmark)" || echo "  (check build directory manually)"
        return 1
    fi
    
    echo -e "${BLUE}Running $demo_name...${NC}"
    echo ""
    "$demo_path"
    local result=$?
    echo ""
    if [ $result -eq 0 ]; then
        echo -e "${GREEN}Demo completed successfully!${NC}"
    else
        echo -e "${RED}Demo failed with exit code $result${NC}"
    fi
    return $result
}

function run_tests() {
    echo -e "${BLUE}Running tests...${NC}"
    echo ""
    
    if [ ! -f "$BUILD_DIR/ctest" ] && [ ! -f "$BUILD_DIR/Makefile" ]; then
        echo -e "${YELLOW}No build directory found. Building first...${NC}"
        build_nlm
    fi
    
    cd "$BUILD_DIR"
    ctest --output-on-failure
    local result=$?
    cd ".."
    
    if [ $result -eq 0 ]; then
        echo -e "${GREEN}All tests passed!${NC}"
    else
        echo -e "${RED}Some tests failed!${NC}"
    fi
    return $result
}

function setup_python_bindings() {
    echo -e "${BLUE}Setting up Python bindings...${NC}"
    
    # Check if pynlm module exists
    if python3 -c "import pynlm" 2>/dev/null; then
        echo -e "${GREEN}Python bindings already installed.${NC}"
        return 0
    fi
    
    echo -e "${YELLOW}Python bindings need to be built...${NC}"
    echo ""
    echo "To install Python bindings manually:"
    echo "  cd .."
    echo "  pip install pybind11 scikit-build-core pytest numpy"
    echo "  pip install ."
    echo ""
    echo "Then you can run the Python examples from easy_usage.md"
    echo ""
}

# Main execution
if [ -d "$BUILD_DIR" ] && ( [ -f "$BUILD_DIR"/CMakeCache.txt ] || [ -f "$BUILD_DIR"/Makefile ] ); then
    echo -e "${BLUE}Build directory '$BUILD_DIR' already exists. Proceeding...${NC}"
elif [ ! -d "$BUILD_DIR" ]; then
    build_nlm
fi

# Run Python bindings setup
if [ "$PYTHON_BINDINGS" = "true" ]; then
    setup_python_bindings
fi

# Run tests if requested
if [ "$RUN_TESTS" = "true" ]; then
    run_tests
    if [ $? -ne 0 ]; then
        echo -e "${YELLOW}Tests failed. Skipping demo execution.${NC}"
        exit 1
    fi
fi

# Run demo
if [ "$RUN_DEMO" = "benchmark" ]; then
    run_demo "nlm_benchmark"
elif [ -n "$RUN_PHASE" ]; then
    run_demo "nlm_${RUN_PHASE}_demo"
elif [ "$RUN_DEMO" = "true" ] || [ "$RUN_TESTS" = "false" ] && [ -z "$RUN_PHASE" ]; then
    # Default to main demo if no specific demo requested
    run_demo "nlm"
fi

# Script summary
echo -e "${GREEN}NLM build and run script completed!${NC}"
echo ""
echo "To run demos manually from build directory ($BUILD_DIR):"
echo "  ./nlm                    # Main demo (Phase 2)"
echo "  ./nlm_phase3_demo        # Phase 3 demo"
echo "  ./nlm_phase4_demo        # Phase 4 demo"
echo "  ./nlm_phase5_demo        # Phase 5 demo"
echo "  ./nlm_phase6_demo        # Phase 6 demo"
echo "  ./nlm_benchmark          # Performance benchmark"
echo ""
echo "To install Python bindings:"
echo "  cd .."
echo "  pip install pybind11 scikit-build-core pytest numpy"
echo "  pip install ."
echo ""
echo "For help, run: $0 --help"
