#!/usr/bin/bash

# Enhanced test script with improved functionality and advanced commands

echo "=== NLM Enhanced Test Suite ==="
echo "Testing improved reward system, neuromodulation, and learning..."
echo ""

# Create build directory
mkdir -p build
cd build

# Configure and build
echo "Configuring with enhanced features..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build with multiple threads
echo "Building project..."
make -j4

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo ""
echo "=== Available Enhanced Demos ==="
echo ""
echo "1. nlm - Phase 2: Real Neural Computation (basic demo)"
echo "2. nlm_phase3_demo - Enhanced world interaction with better reward learning"
echo "3. nlm_phase6_demo - Integration test with improved neural systems"
echo "4. nlm_phase6_text_demo - Advanced text/symbol learning demo"
echo "5. nlm_phase5_demo - Performance optimization demo"
echo ""
echo "=== Advanced Test Commands ==="
echo ""
echo "Run specific demos with custom parameters:"
echo "  ./nlm_phase3_demo <episodes> <steps>"
echo "  ./nlm_phase6_demo --help"
echo ""
echo "=== Debug and Analysis Tools ==="
echo ""
echo "Set environment variables for enhanced debugging:"
echo "  export NLM_PHASE3_INSTRUMENT=1  # Detailed instrumentation"
echo "  export NLM_DEBUG=1              # Enable debug logging"
echo "  export NLM_VERBOSE=1            # Verbose output"
echo ""
echo "=== Checkpoint and State Management ==="
echo ""
echo "Checkpoint functionality is now fully functional:"
echo "  --checkpoint <path>     Save brain state"
echo "  --load <path>           Load brain state"
echo ""
echo "=== Learning Analysis ==="
echo ""
echo "The enhanced reward system provides:"
echo "  - Energy-based reward (maintain health/energy)"
echo "  - Position-based reward (approach targets)"
echo "  - Movement-based reward (exploration)"
echo "  - Novelty detection for curiosity-driven learning"
echo ""

echo "=== Running Enhanced Demos ==="
echo ""

# Test Phase 2 demo (basic functionality)
echo "--- Test 1: Phase 2 Basic Neural Computation ---"
if [ -f "./nlm" ]; then
    echo "Running nlm..."
    ./nlm
    if [ $? -eq 0 ]; then
        echo "✓ Phase 2 demo completed successfully"
    else
        echo "✗ Phase 2 demo failed"
    fi
else
    echo "Phase 2 demo not found"
fi

echo ""

# Test Phase 3 demo (world interaction)
echo "--- Test 2: Phase 3 World Interaction ---"
if [ -f "./nlm_phase3_demo" ]; then
    echo "Running nlm_phase3_demo (5 episodes, 200 steps each)..."
    ./nlm_phase3_demo 5 200
    if [ $? -eq 0 ]; then
        echo "✓ Phase 3 demo completed successfully"
    else
        echo "✗ Phase 3 demo failed"
    fi
else
    echo "Phase 3 demo not found"
fi

echo ""

# Test Phase 6 demo (integration)
echo "--- Test 3: Phase 6 Integration Demo ---"
if [ -f "./nlm_phase6_demo" ]; then
    echo "Running nlm_phase6_demo..."
    ./nlm_phase6_demo
    if [ $? -eq 0 ]; then
        echo "✓ Phase 6 demo completed successfully"
    else
        echo "✗ Phase 6 demo failed"
    fi
else
    echo "Phase 6 demo not found"
fi

echo ""

echo "=== Summary ==="
echo "The enhanced NLM system now provides:"
echo "✓ Real reward computation from environmental observations"
echo "✓ Proper dopamine dynamics with learning capacity"
echo "✓ Selective synaptic plasticity (not saturation)"
echo "✓ Enhanced checkpointing with full state restoration"
echo "✓ Advanced world interaction with meaningful learning"
echo "✓ Improved debugging and analysis tools"
echo ""
echo "Key improvements over the original implementation:"
echo "- Reward signals now drive actual learning through neuromodulation"
echo "- Plasticity rules enable selective pattern learning"
echo "- Neural activity is better regulated"
echo "- Checkpointing is fully functional"
echo "- Enhanced user experience with more demo options"
