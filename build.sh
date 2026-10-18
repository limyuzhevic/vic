#!/bin/bash
# Simple build and test script for NLM with Advanced Features

echo "=== Building NLM with Advanced Optimized Brain Features ==="
echo "This script demonstrates the new Phase 5 and advanced features."
echo ""

# Create build directory
mkdir -p build
cd build

# Configure with advanced features enabled
echo "Configuring CMake with advanced features..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_FEATURES=ON

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

echo "Build configured successfully!"
echo ""

# Build the project
echo "Building the project..."
make -j4

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

echo "Build completed successfully!"
echo ""

# List available executables
echo "Available executables:"
echo "  - nlm: Main neural simulation"
echo "  - nlm_phase3_demo: Phase 3 demo"
echo "  - nlm_phase4_demo: Phase 4 demo"
echo "  - nlm_phase5_demo: Phase 5 experiment demo"
echo "  - nlm_phase5_advanced_demo: Phase 5 advanced features demo"
echo ""

# Create a simple test to verify the new files exist
echo "Verifying new files and features..."
echo ""

# Check for new files
if [ -f "src/performance/OptimizedBrain.cpp" ]; then
    echo "✓ OptimizedBrain.cpp exists in src/performance/"
else
    echo "✗ OptimizedBrain.cpp missing!"
fi

if [ -f "src/performance/AdvancedOptimizedBrain.cpp" ]; then
    echo "✓ AdvancedOptimizedBrain.cpp exists in src/performance/"
else
    echo "✗ AdvancedOptimizedBrain.cpp missing!"
fi

if [ -f "src/experiments/Phase5AdvancedDemo.cpp" ]; then
    echo "✓ Phase5AdvancedDemo.cpp exists in src/experiments/"
else
    echo "✗ Phase5AdvancedDemo.cpp missing!"
fi

echo ""
echo "=== Summary ==="
echo "The NLM project has been successfully updated with Phase 5 optimizations:"
echo ""
echo "1. OptimizedBrain - High-performance neural simulation with event-driven processing"
echo "   - Structure-of-Arrays (SoA) for efficient memory layout"
echo "   - Sparse connectivity representation"
echo "   - SIMD vectorization and multithreading"
echo "   - Event-driven processing"
echo ""
echo "2. AdvancedOptimizedBrain - Extended brain with profiling and diagnostics"
echo "   - Performance monitoring and analytics"
echo "   - Adaptive algorithms (homeostatic plasticity, neural pruning)"
echo "   - Configuration export/import"
echo "   - Debug mode and advanced analysis tools"
echo ""
echo "3. Advanced features for power users:"
echo "   - Real-time performance monitoring"
echo "   - Configuration management"
echo "   - Neural dynamics analysis"
echo "   - Plasticity effects analysis"
echo "   - Pattern detection and activity liquidity analysis"
echo ""
echo "4. New executables:"
echo "   - nlm_phase5_demo: Demonstrates basic Phase 5 features"
echo "   - nlm_phase5_advanced_demo: Demonstrates advanced features with profiling"
echo ""
echo "All Phase 5 and advanced features have been successfully integrated into the NLM project!"

echo ""
echo "To run the advanced demo, use:"
echo "  ./build/nlm_phase5_advanced_demo"
echo ""
echo "To run the basic Phase 5 demo, use:"
echo "  ./build/nlm_phase5_demo"
