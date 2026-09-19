#!/bin/bash
# Build helper script with advanced options

set -e

cd "$(dirname "$0")"

# Parse command line arguments
BUILD_TYPE="Release"
BUILD_DIR="build"
NUM_JOBS=$(nproc)
RUN_TESTS=false
GENERATE_DOCS=false
CLEAN_BUILD=false
PYTHON_BINDING=false
ADVANCED_OPTS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        "--debug")
            BUILD_TYPE="Debug"
            shift
            ;;
        "--clean")
            CLEAN_BUILD=true
            shift
            ;;
        "--no-tests")
            RUN_TESTS=false
            shift
            ;;
        "--docs")
            GENERATE_DOCS=true
            shift
            ;;
        "--python-only")
            PYTHON_BINDING=true
            shift
            ;;
        "--advanced")
            ADVANCED_OPTS=true
            shift
            ;;
        "--help")
            echo "NLM Build Script"
            echo "==============="
            echo "Usage: ./build.sh [options]"
            echo ""
            echo "Options:"
            echo "  --debug          Build with Debug flags"
            echo "  --clean          Clean build directory before building"
            echo "  --no-tests       Skip running tests after build"
            echo "  --docs           Generate documentation (placeholder)"
            echo "  --python-only    Only build Python bindings"
            echo "  --advanced       Enable advanced build options (SIMD, OpenMP)"
            echo "  --help           Show this help message"
            echo ""
            echo "Examples:"
            echo "  ./build.sh                # Standard release build"
            echo "  ./build.sh --debug        # Debug build with symbols"
            echo "  ./build.sh --clean        # Clean rebuild"
            echo "  ./build.sh --python-only  # Python bindings only"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Run ./build.sh --help for usage"
            exit 1
            ;;
    esac
done

# Clean build if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake with standard options
echo "Configuring CMake with $BUILD_TYPE build type..."

CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ "$ADVANCED_OPTS" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DSIMD_ENABLE=ON -DOPENMP_ENABLE=ON -DNLM_ENABLE_PERF_COUNTERS=ON"
fi

if [ "$PYTHON_BINDING" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DBUILD_PYTHON_BINDINGS=ON"
fi

cmake $CMAKE_ARGS ..

# Build with parallel jobs
echo "Building with $NUM_JOBS jobs..."
make -j$NUM_JOBS

# Run tests unless disabled
if [ "$RUN_TESTS" = true ]; then
    echo "Running tests..."
    ctest --output-on-failure
fi

# Generate documentation if requested
if [ "$GENERATE_DOCS" = true ]; then
    echo "Generating documentation..."
    if command -v doxygen &> /dev/null; then
        doxygen Doxyfile 2>/dev/null || echo "Doxygen not configured or failed"
    else
        echo "Doxygen not installed. Install with: sudo apt-get install doxygen"
    fi
fi

# Success message
echo "Build completed successfully!"
echo "Binaries are in: $(pwd)"
