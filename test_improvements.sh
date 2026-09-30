#!/usr/bin/env bash

# Test script to verify NLM improvements
cd "$(dirname "$0")"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Test the Phase 6 integration
if [ -f "./nlm_phase6_demo" ]; then
    echo "Testing Phase 6 integration..."
    ./nlm_phase6_demo
    if [ $? -eq 0 ]; then
        echo "✓ Phase 6 integration test PASSED"
    else
        echo "✗ Phase 6 integration test FAILED"
        exit 1
    fi
else
    echo "✗ nlm_phase6_demo not found"
    exit 1
fi

# Test Python bindings
if [ -f "../python/build/pynlm*.so" ] || [ -f "../python/build/pynlm*.pyd" ]; then
    echo "Testing Python bindings..."
    cd ..
    python3 -c "
import sys
sys.path.append('python/build')
try:
    import pynlm
    print('✓ Python bindings import successful')
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test brain step
    brain.step(0)
    print('✓ Basic brain operation successful')
    
    # Test stats
    neurons = brain.getTotalNeuronCount()
    spikes = brain.getTotalSpikeCount()
    print(f'Brain stats: {neurons} neurons, {spikes} total spikes')
    
    # Test memory systems
    if brain.getWorkingMemory():
        print('✓ Working memory integrated')
    if brain.getEpisodicMemory():
        print('✓ Episodic memory integrated')
    if brain.getPredictionSystem():
        print('✓ Prediction system integrated')
        
except Exception as e:
    print(f'✗ Python test FAILED: {e}')
    sys.exit(1)
"
else
    echo "⚠️  Python bindings not built (this is expected for this test)"
fi

echo "✓ All tests completed successfully"
