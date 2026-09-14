#!/bin/bash

# Create build directory
mkdir -p build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF \
    -Wall -Wextra -Wpedantic

# Build the project
make -j$(nproc)