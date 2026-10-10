#!/bin/bash
# build-ci.sh - Example CI build script for NLM

set -e

# CI Configuration
BUILD_TYPE="Release"
BUILD_DIR="build"
PYTHON_BINDINGS=ON
TESTS=ON
PERFORMANCE=OFF
COVERAGE=OFF
ASAN=OFF
TSAN=OFF

# Detect CI environment
if [ -n "$GITHUB_ACTIONS" ]; then
    # GitHub Actions configuration
    echo "Detected GitHub Actions CI environment"
    CPU_COUNT=$(nproc)
elif [ -n "$GITLAB_CI" ]; then
    # GitLab CI configuration
    echo "Detected GitLab CI environment"
    CPU_COUNT=$(nproc)
elif [ -n "$JENKINS_URL" ]; then
    # Jenkins configuration
    echo "Detected Jenkins CI environment"
    CPU_COUNT=$([ "$(uname -m)" = "x86_64" ] && echo "4" || echo "2")
else
    # Default configuration
    CPU_COUNT=$(nproc)
fi

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with coverage and sanitizers for CI
CMAKE_ARGS="
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DNLM_ENABLE_PYTHON_BINDINGS=$PYTHON_BINDINGS \
    -DNLM_ENABLE_TESTS=$TESTS \
    -DNLM_ENABLE_PERFORMANCE=$PERFORMANCE \
    -DNLM_ENABLE_COVERAGE=$COVERAGE \
    -DNLM_ENABLE_ASAN=$ASAN \
    -DNLM_ENABLE_TSAN=$TSAN \
    -GNinja \
    -DCMAKE_CXX_STANDARD=20
"

# Configure CMake
cmake $CMAKE_ARGS

# Build with parallel jobs
cmake --build . --config $BUILD_TYPE -- -j$CPU_COUNT

# Run tests with coverage if enabled
if [ "$TESTS" = "ON" ] && [ "$COVERAGE" = "ON" ]; then
    echo "Running tests with coverage..."
    ctest --output-on-failure --no-compress-output
    
    # Generate coverage report
    echo "Generating coverage report..."
    find . -name "*.gcda" -delete
    find . -name "*.gcno" -delete
    
    # If gcov is available
    if command -v gcov >/dev/null 2>&1; then
        gcov --version >/dev/null 2>&1 && echo "GCOV available"
        # Add coverage reporting logic here
    fi
elif [ "$TESTS" = "ON" ]; then
    echo "Running tests..."
    ctest --output-on-failure --no-compress-output
fi

# Run Python bindings tests if Python is available
if [ "$PYTHON_BINDINGS" = "ON" ] && command -v python3 >/dev/null 2>&1; then
    echo "Testing Python bindings..."
    cd ..
    python3 -c "import pynlm; print('Python bindings imported successfully')" || {
        echo "Python bindings test failed!"
        exit 1
    }
    cd build
fi

# Package the build
if command -v cpack >/dev/null 2>&1; then
    echo "Creating packages..."
    cpack --config $BUILD_TYPE
    
    # Store packages as artifacts
    mv *.deb . 2>/dev/null || true
    mv *.rpm . 2>/dev/null || true
    mv *.tar.gz . 2>/dev/null || true
fi

# Generate build report
echo "Generating build report..."
cd ..
cat > build_report.txt << EOF
Build Report
============
Build directory: $BUILD_DIR
Build type: $BUILD_TYPE
Compiler: $(c++ --version 2>/dev/null || echo "Unknown")
CMake version: $(cmake --version 2>/dev/null | head -1)
CPU count: $CPU_COUNT

Features:
- Python bindings: $PYTHON_BINDINGS
- Tests: $TESTS
- Performance optimizations: $PERFORMANCE
- Code coverage: $COVERAGE
- AddressSanitizer: $ASAN
- ThreadSanitizer: $TSAN

Build completed at: $(date)
EOF

echo "CI build completed successfully!"
