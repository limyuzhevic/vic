#!/bin/bash
# NLM Build Script
# Simple build script for NLM Phase 6 Final Integration

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== NLM Phase 6 Build Script ===${NC}"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake is not installed. Please install CMake 3.16 or higher.${NC}"
    exit 1
fi

# Check for C++ compiler
if ! command -v g++ &> /dev/null; then
    echo -e "${YELLOW}Warning: g++ not found. Using clang++ if available.${NC}"
    if ! command -v clang++ &> /dev/null; then
        echo -e "${RED}Error: No C++ compiler found. Please install g++ or clang++.${NC}"
        exit 1
    fi
    CXX=clang++
else
    CXX=g++
fi

# Check CMake version
CMAKE_VERSION=$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+')
CMAKE_MAJOR_MINOR=$(echo $CMAKE_VERSION | cut -d'.' -f1,2)

if (( $(echo "$CMAKE_MAJOR_MINOR < 3.16" | bc -l) )); then
    echo -e "${YELLOW}Warning: CMake version $CMAKE_VERSION is less than 3.16. Some features may not work.${NC}"
fi

BUILD_TYPE="Release"
BUILD_DIR="build"
PYTHON_BINDING="OFF"
RUN_TESTS="OFF"

echo -e "${YELLOW}Configuration:${NC}"
echo "  Build Type: $BUILD_TYPE"
echo "  Build Directory: $BUILD_DIR"
echo "  Python Bindings: $PYTHON_BINDING"
echo "  Run Tests: $RUN_TESTS"
echo "  C++ Compiler: $CXX"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --python)
            PYTHON_BINDING="ON"
            shift
            ;;
        --tests)
            RUN_TESTS="ON"
            shift
            ;;
        --clean)
            echo -e "${YELLOW}Cleaning build directory...${NC}"
            rm -rf "$BUILD_DIR"
            shift
            ;;
        --help|-h)
            echo -e "${BLUE}NLM Build Script${NC}"
            echo "Build options:"
            echo "  --debug              Build in Debug mode (default: Release)"
            echo "  --python             Enable Python bindings (requires pybind11)"
            echo "  --tests              Run tests after build"
            echo "  --clean              Clean build directory and rebuild"
            echo "  --help, -h           Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information."
            exit 1
            ;;
    esac
done

# Create build directory
echo -e "${YELLOW}Creating build directory: $BUILD_DIR${NC}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${YELLOW}Configuring CMake...${NC}"
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_CXX_COMPILER=$CXX"

if [ "$PYTHON_BINDING" = "ON" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DPYTHON_BINDING=ON"
fi

cmake $CMAKE_ARGS ..

# Check if CMake configuration succeeded
if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed.${NC}"
    exit 1
fi

# Build
echo -e "${YELLOW}Building NLM...${NC}"
make -j$(nproc)

# Check if build succeeded
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed.${NC}"
    exit 1
fi

# Run tests if requested
if [ "$RUN_TESTS" = "ON" ]; then
    echo -e "${YELLOW}Running tests...${NC}"
    ctest --output-on-failure
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Tests failed.${NC}"
        exit 1
    fi
fi

cd ..

echo -e "${GREEN}=== Build Complete ===${NC}"
echo "Generated executables:"
echo "  $BUILD_DIR/nlm                  (Main Phase 6 integration)"
echo "  $BUILD_DIR/nlm_phase3_demo      (Phase 3 demonstration)"
echo "  $BUILD_DIR/nlm_phase4_demo      (Phase 4 demonstration)"
echo "  $BUILD_DIR/nlm_phase6_demo      (Phase 6 demonstration)"
echo ""
echo "To run the main Phase 6 integration demo:"
echo "  ./$BUILD_DIR/nlm --demo full"
echo ""
echo "To run a quick integration test:"
echo "  ./$BUILD_DIR/nlm --quick"
echo ""
echo "To test checkpoint functionality:"
echo "  ./$BUILD_DIR/nlm --checkpoints"
