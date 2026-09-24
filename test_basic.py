#!/usr/bin/env python3
"""Simple test to check if pynlm is working."""

import sys

try:
    import pynlm
    print(f"Successfully imported pynlm, version: {getattr(pynlm, '__version__', 'unknown')}")
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    print(f"Config created, has random_seed: {config.has('random_seed')}")
    
    brain = pynlm.createBrain(config)
    print(f"Brain created: {brain}")
    
    brain.initialize()
    print(f"Brain initialized, neurons: {brain.getTotalNeuronCount()}")
    
    brain.step(0)
    print(f"Brain stepped successfully")
    
    action = brain.produceAction()
    print(f"Action produced: {action}")
    
    print("All basic tests passed!")
    
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)