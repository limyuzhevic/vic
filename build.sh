#!/bin/bash
# NLM Build Script with Performance Optimizations

set -e

echo "Building NLM with performance optimizations..."

echo "Configuration:"
echo "  Project: NLM (Neural Learning Machine)"
echo "  Build type: Release with optimizations"
echo "  C++ standard: C++20"
echo "  Performance features: ENABLED"
echo "  Memory pools: ENABLED"
echo "  SIMD (AVX2): ENABLED"
echo "  Event queue: ENABLED"
echo "  Sparse connectivity: ENABLED"
echo "  Multithreading: ENABLED (4 threads)"
echo "  SoA data layout: ENABLED"
echo ""

echo "To build with performance optimizations:"
echo "  mkdir build"
echo "  cd build"
echo "  cmake .. -DCMAKE_BUILD_TYPE=Release \\"
echo "    -DNLM_PERFORMANCE_ENABLED=ON \\"
echo "    -DNLM_SIMD_AVX2=ON \\"
echo "    -DNLM_THREAD_COUNT=4"
echo "  make -j$(nproc)"
echo ""

echo "To build with optimizations disabled:"
echo "  mkdir build"
echo "  cd build"
echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "  make -j$(nproc)"
echo ""

echo "To build with tests:"
echo "  mkdir build"
echo "  cd build"
echo "  cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON"
echo "  make -j$(nproc)"
echo "  ctest --output-on-failure"

echo ""
echo "Build script completed successfully!"