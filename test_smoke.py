#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings are working.
This is a basic smoke test to ensure the module loads correctly.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ Successfully imported pynlm module")
    
    # Check version
    print(f"  Version: {getattr(pynlm, '__version__', 'unknown')}")
    
    # Check main classes
    print(f"  Available classes: {[cls for cls in dir(pynlm) if not cls.startswith('_')]}")
    
    # Try to create a simple config
    try:
        config = pynlm.createDefaultConfig()
        print("✓ Successfully created default configuration")
        
        # Try to set some values
        config.set("neuron_count", 100)
        config.set("brain.synapse_density", 0.1)
        
        # Try to get values
        neuron_count = config.getOr("neuron_count", 500)
        print(f"✓ Configuration test passed (neuron_count: {neuron_count})")
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
    
    # Try to create a simple brain
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Successfully created and initialized brain")
        print(f"  Neuron count: {brain.getTotalNeuronCount()}")
        print(f"  Synapse count: {brain.getTotalSynapseCount()}")
        
        # Run a few steps
        for i in range(5):
            brain.step(i)
        
        print(f"✓ Successfully ran brain simulation (total spikes: {brain.getTotalSpikeCount()})")
        
    except Exception as e:
        print(f"✗ Brain test failed: {e}")
        import traceback
        traceback.print_exc()
    
    print("\n" + "="*60)
    print("NLM Python bindings test completed!")
    print("="*60)
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("This might be expected if the C++ extensions haven't been built yet.")
    sys.exit(1)

except Exception as e:
    print(f"✗ Unexpected error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
