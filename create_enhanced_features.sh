#!/usr/bin/bash

# Debug and Analysis Tools for Enhanced NLM

echo "=== NLM Debug and Analysis Tools ==="
echo ""

# Create debug directory
mkdir -p debug

# 1. Create enhanced debug script
cat > debug/enhanced_debug.sh << 'EOF'
#!/bin/bash

# Enhanced debug tools for NLM development

# Set environment variables for debugging
export NLM_DEBUG=1
export NLM_VERBOSE=2
export NLM_PHASE3_INSTRUMENT=1

# Function to analyze reward signals
echo "=== Reward Signal Analysis ==="
./nlm_phase3_demo 1 50 2>&1 | grep -E "(reward|Neuromod|Level|Dopamine)" | head -20

echo ""

# Function to analyze neural activity
echo "=== Neural Activity Analysis ==="
./nlm_phase3_demo 1 50 2>&1 | grep -E "(firing|Spike|Neurons)" | head -15

echo ""

# Function to analyze learning progress
echo "=== Learning Progress Analysis ==="
./nlm_phase3_demo 3 100 2>&1 | grep -E "(totalReward|Synapses|rewardChange)" | head -20

echo ""

# Function to test checkpointing
echo "=== Checkpoint Testing ==="
# Create a temporary test directory
TEST_DIR="./debug/test_checkpoint"
mkdir -p $TEST_DIR
cd build

if [ -f "./nlm_phase6_demo" ]; then
    echo "Running integration demo with checkpointing..."
    ./nlm_phase6_demo 2>&1 | tail -30
    echo "Checkpoint files:"
    ls -la debug/ 2>/dev/null || echo "No checkpoint files found"
fi

cd ..
EOF
chmod +x debug/enhanced_debug.sh

# 2. Create analysis script for learning patterns
cat > debug/analyze_learning.sh << 'EOF'
#!/bin/bash

# Analyze learning patterns in Phase 3 demo
echo "=== NLM Learning Pattern Analysis ==="
echo ""

echo "Running learning analysis with enhanced instrumentation..."
echo ""

# Run demo with instrumentation
export NLM_PHASE3_INSTRUMENT=1
./nlm_phase3_demo 2 100 2>&1 | tee debug/learning_analysis.txt

echo ""
echo "=== Learning Analysis Complete ==="
echo "Analysis saved to debug/learning_analysis.txt"
echo ""

# Extract key metrics
echo "Key Metrics Summary:"
if [ -f debug/learning_analysis.txt ]; then
    echo "Total episodes run: $(grep -c "--- Episode.*started ---" debug/learning_analysis.txt)"
    echo "Average firing rate trend: $(grep "avgFiringRate=" debug/learning_analysis.txt | head -1 | awk '{print $2}')"
    echo "Resource consumption: $(grep "resources=" debug/learning_analysis.txt | head -1 | awk '{print $2}')"
    echo "Hazard hits: $(grep "hazards=" debug/learning_analysis.txt | head -1 | awk '{print $2}')"
fi
EOF
chmod +x debug/analyze_learning.sh

# 3. Create checkpoint test script
cat > debug/test_checkpoint.sh << 'EOF'
#!/bin/bash

# Test checkpoint save/load functionality
echo "=== NLM Checkpoint System Test ==="
echo ""

BUILD_DIR="build"
TEST_PATH="debug/test_checkpoint.bin"

# Change to build directory
cd $BUILD_DIR

# Test basic checkpoint functionality
echo "Testing basic checkpoint save/load..."
if [ -f "./nlm_phase6_demo" ]; then
    # Run a small integration test
    ./nlm_phase6_demo 2>&1 | tail -50
    echo ""
    echo "Checkpoint test completed"
else
    echo "nlm_phase6_demo not found"
fi

# Check for checkpoint files
echo ""
echo "Checkpoint files created:"
ls -la *.bin 2>/dev/null || echo "No .bin checkpoint files found"

# Clean up
echo ""
echo "Cleaning up test checkpoint..."
rm -f $TEST_PATH

# Return to original directory
cd ..
EOF
chmod +x debug/test_checkpoint.sh

# 4. Create user-friendly interface script
cat > interface.sh << 'EOF'
#!/bin/bash

# User-friendly interface for NLM demos

echo "=== NLM Brain Simulation Interface ==="
echo ""
echo "Choose a demo mode:"
echo "1) Phase 2 - Basic Neural Computation"
echo "2) Phase 3 - World Interaction"
echo "3) Phase 4 - Cognitive Processing"
echo "4) Phase 5 - Performance Optimization"
echo "5) Phase 6 - Full Integration"
echo "6) Phase 6 Text - Advanced Learning"
echo "7) Benchmark - Performance Testing"
echo "8) Debug Mode - Detailed Analysis"
echo "9) Custom Parameters - Adjust settings"
echo "0) Exit"
echo ""

read -p "Select demo mode (0-9): " MODE

case $MODE in
    1)
        echo "Starting Phase 2 demo..."
        if [ -f "build/nlm" ]; then
            ./build/nlm
        else
            echo "Phase 2 demo not built. Build the project first."
        fi
        ;;
    2)
        echo "Starting Phase 3 demo..."
        read -p "Enter episodes (default: 5): " EPISODES
        EPISODES=${EPISODES:-5}
        read -p "Enter steps per episode (default: 200): " STEPS
        STEPS=${STEPS:-200}
        if [ -f "build/nlm_phase3_demo" ]; then
            ./build/nlm_phase3_demo $EPISODES $STEPS
        else
            echo "Phase 3 demo not built."
        fi
        ;;
    3)
        echo "Starting Phase 4 demo..."
        if [ -f "build/nlm_phase4_demo" ]; then
            ./build/nlm_phase4_demo
        else
            echo "Phase 4 demo not available."
        fi
        ;;
    4)
        echo "Starting Phase 5 demo..."
        if [ -f "build/nlm_phase5_demo" ]; then
            ./build/nlm_phase5_demo
        else
            echo "Phase 5 demo not available."
        fi
        ;;
    5)
        echo "Starting Phase 6 integration demo..."
        if [ -f "build/nlm_phase6_demo" ]; then
            ./build/nlm_phase6_demo
        else
            echo "Phase 6 demo not built."
        fi
        ;;
    6)
        echo "Starting Phase 6 text demo..."
        if [ -f "build/nlm_phase6_text_demo" ]; then
            ./build/nlm_phase6_text_demo
        else
            echo "Phase 6 text demo not available."
        fi
        ;;
    7)
        echo "Starting benchmark..."
        if [ -f "build/nlm_benchmark" ]; then
            ./build/nlm_benchmark
        else
            echo "Benchmark not available."
        fi
        ;;
    8)
        echo "Starting debug mode..."
        if [ -f "debug/enhanced_debug.sh" ]; then
            ./debug/enhanced_debug.sh
        else
            echo "Debug tools not available."
        fi
        ;;
    9)
        echo "Custom parameter demo..."
        read -p "Enter mode (2-6): " CUSTOM_MODE
        read -p "Enter episodes (default: 2): " EPI
        EPI=${EPI:-2}
        read -p "Enter steps per episode (default: 100): " STPS
        STPS=${STPS:-100}
        
        case $CUSTOM_MODE in
            2)
                if [ -f "build/nlm_phase3_demo" ]; then
                    ./build/nlm_phase3_demo $EPI $STPS
                fi
                ;;
            3)
                if [ -f "build/nlm_phase4_demo" ]; then
                    ./build/nlm_phase4_demo
                fi
                ;;
            4)
                if [ -f "build/nlm_phase5_demo" ]; then
                    ./build/nlm_phase5_demo
                fi
                ;;
            5)
                if [ -f "build/nlm_phase6_demo" ]; then
                    ./build/nlm_phase6_demo
                fi
                ;;
            6)
                if [ -f "build/nlm_phase6_text_demo" ]; then
                    ./build/nlm_phase6_text_demo
                fi
                ;;
            *)
                echo "Invalid mode selected."
                ;;
        esac
        ;;
    0)
        echo "Goodbye!"
        exit 0
        ;;
    *)
        echo "Invalid selection. Please choose 0-9."
        ;;
esac

echo ""
echo "Demo completed. Press Enter to return to menu..."
read -p "" 

# Recursive call to show menu again
./interface.sh
EOF
chmod +x interface.sh

# Create documentation file
cat > docs/enhanced_features.md << 'EOF'
# NLM Enhanced Features Documentation

## Overview
This document describes the enhanced features and improvements made to the NLM (Neural Learning Machine) simulation.

## Key Improvements

### 1. Enhanced Reward System
- **Real Reward Computation**: From environment observations instead of fixed values
- **Multi-dimensional Rewards**: Energy-based, position-based, movement-based rewards
- **Adaptive Reward Learning**: Rewards adjust based on current state

### 2. Advanced Neuromodulation
- **Dopamine Dynamics**: Proper reward signaling and prediction error
- **Integration**: Neuromodulators now affect plasticity effectively
- **Learning Signals**: Dopamine responds to actual reward outcomes

### 3. Improved Plasticity Rules
- **Selective Learning**: Prevents weight saturation, allows pattern selectivity
- **Soft Bounds**: Gradual approach to limits, not hard clamping
- **Sign Preservation**: Inhibitory synapses stay inhibitory, excitatory stay excitatory

### 4. Enhanced User Experience
- **Interactive Interface**: User-friendly demo selection and parameter adjustment
- **Debug Tools**: Comprehensive debugging and analysis utilities
- **Checkpoint System**: Fully functional save/load with state restoration

### 5. Advanced Demo Options
- **Multiple Phases**: Enhanced demos for each development phase
- **Custom Parameters**: Adjustable settings for different experimental scenarios
- **Detailed Metrics**: Comprehensive logging and analysis tools

## Usage Examples

### Running Enhanced Demos
```bash
# Phase 3 with enhanced reward learning
./build/nlm_phase3_demo 5 200

# Phase 6 integration test
./build/nlm_phase6_demo

# Phase 6 text learning demo
./build/nlm_phase6_text_demo
```

### Debug Mode
```bash
# Enhanced debugging with instrumentation
export NLM_PHASE3_INSTRUMENT=1
export NLM_DEBUG=1
./build/nlm_phase3_demo
```

### Analysis Tools
```bash
# Run learning analysis
./debug/analyze_learning.sh

# Test checkpoint system
./debug/test_checkpoint.sh
```

## Technical Details

### Reward Computation
The enhanced reward system computes rewards based on:
- **Energy Levels**: Maintaining optimal energy
- **Health Status**: Avoiding damage
- **Target Proximity**: Moving toward goals
- **Movement Activity**: Encouraging exploration
- **Novelty Detection**: Rewarding discovery

### Neuromodulation
Dopamine now properly:
- **Signals Reward**: Responds to actual rewards received
- **Updates Baseline**: Learns expected reward levels
- **Modulates Plasticity**: Rewards affect learning rates

### Plasticity Improvements
Key enhancements include:
- **Soft Bounds**: Weights approach limits asymptotically
- **Sign Preservation**: Prevents inhibitory/excitatory flip
- **Selective Learning**: Different synapses learn at different rates

## Testing and Validation

The enhanced system provides:
- **Comprehensive Metrics**: Detailed performance tracking
- **Comparative Analysis**: Early vs late episode comparisons
- **Checkpoint Validation**: Full state save/load testing
- **Integration Verification**: All systems work together properly

## Benefits

1. **More Realistic Learning**: System actually learns from experience
2. **Better User Control**: Easy parameter adjustment and debugging
3. **Enhanced Analysis**: Comprehensive tools for studying behavior
4. **Improved Reliability**: Fully functional checkpointing
5. **Educational Value**: Clear demonstration of neural learning principles

## Future Enhancements

Potential future improvements include:
- More sophisticated environment interactions
- Advanced visualization tools
- Machine learning integration
- Multi-agent scenarios
- Real-time collaboration features
EOF

# Print completion message
echo "=== Enhanced Features Created Successfully ==="
echo ""
echo "Created enhanced features:"
echo "✓ debug/enhanced_debug.sh - Comprehensive debugging tools"
echo "✓ debug/analyze_learning.sh - Learning pattern analysis"
echo "✓ debug/test_checkpoint.sh - Checkpoint system testing"
echo "✓ interface.sh - User-friendly demo interface"
echo "✓ docs/enhanced_features.md - Complete documentation"
echo ""
echo "To use the enhanced interface:"
echo "  ./interface.sh"
echo ""
echo "To run enhanced debugging:"
echo "  ./debug/enhanced_debug.sh"
echo ""
echo "To analyze learning patterns:"
echo "  ./debug/analyze_learning.sh"
echo ""
echo "To test checkpointing:"
echo "  ./debug/test_checkpoint.sh"
