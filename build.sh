#!/bin/bash
# Build script for NLM Phase 6

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Please install CMake 3.16 or higher."
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Compiler not found. Please install a C++ compiler."
    exit 1
fi

# Check CMake version
CMAKE_VERSION=$(cmake --version | head -n1 | awk '{print $3}')
CMAKE_MAJOR_VERSION=$(echo $CMAKE_VERSION | cut -d. -f1)
CMAKE_MINOR_VERSION=$(echo $CMAKE_VERSION | cut -d. -f2)

if [ $CMAKE_MAJOR_VERSION -lt 3 ] || ([ $CMAKE_MAJOR_VERSION -eq 3 ] && [ $CMAKE_MINOR_VERSION -lt 16 ]); then
    echo "CMake 3.16 or higher required. Found $CMAKE_VERSION."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF \
    -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests if they exist
if [ -f "nlm_test" ] || [ -f "CMakeFiles/nlm_test.dir/target" ]; then
    echo "Running tests..."
    ctest --output-on-failure
fi

echo "Build complete!"
echo "Targets created:"
ls -la
