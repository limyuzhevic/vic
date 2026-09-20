#!/bin/bash
set -e

echo "Building NLM with optimized settings..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir -p build
    cd build
    
    # Configure with optimized settings
    cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=20 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_CXX_EXTENSIONS=OFF \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DNLM_BUILD_SHARED=ON \
        -DNLM_BUILD_TESTS=ON \
        -DNLM_ENABLE_PYTHON_BINDINGS=ON \
        -DNLM_ENABLE_EXPERIMENTS=ON \
        -DNLM_BUILD_DOCUMENTATION=OFF
    
    echo "Configuration complete."
else
    cd build
    echo "Updating build...
fi

# Build with all targets
echo "Building targets..."
make -j$(nproc)

# Run tests if enabled
if [ -f "nlm_test" ]; then
    echo "Running tests..."
    ./nlm_test
fi

echo "Build successful!"
