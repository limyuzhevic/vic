#!/bin/bash

# Build script for NLM with Python bindings

echo "Building NLM project with Python bindings..."

echo "Cleaning previous build..."
rm -rf build
mkdir build
cd build

echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Building all targets..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo ""
echo "Generated files:"
find . -maxdepth 2 -type f -executable -name "*" -print2 | sed 's|^|.\/|'
echo ""
echo "Python extension (if built):"
find . -name "*.so" -o -name "*.pyd" | head -5

# Return to workspace root
cd ..
