"""Test script to verify NLM package functionality."""

import sys

print("Testing NLM Python package import...")

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    print(f"  Version: {pynlm.__version__}")
    
    # Test version info
    info = pynlm.getVersionInfo()
    print(f"  Author: {info['author']}")
    print(f"  Description: {info['description']}")
    
    # Test basic type definitions
    print(f"  NeuronId type: {type(pynlm.NeuronId)}")
    print(f"  SynapseId type: {type(pynlm.SynapseId)}")
    
    # Test factory functions
    config = pynlm.createDefaultConfig()
    print("✓ Successfully created default config")
    
    brain = pynlm.createBrain(config)
    print("✓ Successfully created brain")
    
    brain.initialize()
    print("✓ Successfully initialized brain")
    
    print(f"  Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Synapse count: {brain.getTotalSynapseCount()}")
    
    # Test basic simulation
    for i in range(5):
        brain.step(i)
    
    print(f"✓ Successfully ran simulation for 5 steps")
    print(f"  Total spike count: {brain.getTotalSpikeCount()}")
    
    print("\n✅ All import and basic functionality tests passed!")
    sys.exit(0)
    
except Exception as e:
    print(f"\n❌ Error during testing: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
