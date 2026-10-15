#!/usr/bin/env bash
# NLM Build Script

# Set default build type if not specified
if [ -z "$1" ]; then
    BUILD_TYPE="Release"
else
    BUILD_TYPE="$1"
fi

echo "Building NLM $BUILD_TYPE..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_CXX_STANDARD=20

# Build with all targets
make -j$(nproc)

# Run tests if requested
if [ "$2" = "--test" ]; then
    echo "Running tests..."
    ctest --output-on-failure
fi

echo "Build complete!"
echo "\nAvailable executables:"
for exe in bin/*; do
    if [ -x "$exe" ]; then
        echo "  $(basename $exe)"
    fi
done