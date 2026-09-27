#!/bin/bash

# Set compiler optimization flags
export CFLAGS="-O3 -march=native -flto"
export CXXFLAGS="-O3 -march=native -flto"

# Memory debugging (uncomment for leak detection)
# export CFLAGS="$CFLAGS -fsanitize=address -fno-omit-frame-pointer"
# export CXXFLAGS="$CXXFLAGS -fsanitize=address -fno-omit-frame-pointer"

# Performance analysis (uncomment for profiling)
# export CFLAGS="$CFLAGS -pg"
# export CXXFLAGS="$CXXFLAGS -pg"

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release          -DBUILD_TESTING=ON          -DENABLE_PERFORMANCE_TESTS=ON          -DENABLE_MEMORY_SANITIZER=OFF          -DENABLE_VALGRIND_TESTS=OFF

# Build with all tests
cmake --build . -j$(nproc)
