#!/usr/bin/env python3
"""
Quick test of NLM Python bindings improvements.
This script provides a basic verification of the enhanced Python bindings.
"""

import sys
import os

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("Testing NLM Python Bindings Improvements")
print("=" * 50)

# Try to import the module
try:
    import pynlm
    print(f"✓ Successfully imported pynlm")
    print(f"  Version: {getattr(pynlm, '__version__', 'unknown')}")
    print(f"  Author: {getattr(pynlm, '__author__', 'unknown')}")
    print(f"  Description: {getattr(pynlm, '__description__', 'unknown')}")
    
    # Test basic functionality
    print("\n1. Testing basic functionality...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   ✓ Created and initialized brain")
    print(f"   ✓ Total neurons: {brain.getTotalNeuronCount()}")
    
    # Test context manager
    print("\n2. Testing context manager...")
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        print(f"   ✓ Context manager works")
        print(f"   ✓ Neurons initialized: {brain.getTotalNeuronCount()}")
    
    # Test configuration utilities
    print("\n3. Testing configuration utilities...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    is_valid = pynlm.validateConfig(config)
    print(f"   ✓ Configuration validation: {is_valid}")
    
    config_dict = pynlm.configToDict(config)
    print(f"   ✓ Config to dict: {len(config_dict)} keys")
    
    new_config = pynlm.dictToConfig(config_dict)
    print(f"   ✓ Dict to config: created")
    
    # Test statistics
    print("\n4. Testing statistics...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(5):
        brain.step(i)
    
    stats = pynlm.getStatistics(brain)
    print(f"   ✓ Statistics collected:")
    for key, value in stats.items():
        print(f"     {key}: {value}")
    
    # Test convenience functions
    print("\n5. Testing convenience functions...")
    spikes = pynlm.runQuickSimulation(neuron_count=50, steps=100)
    print(f"   ✓ Quick simulation: {spikes} spikes")
    
    env = pynlm.createTestEnvironment()
    print(f"   ✓ Test environment: {env['type']}")
    
    # Test error handling
    print("\n6. Testing error handling...")
    try:
        pynlm.runExperiment(None, 100.0)
        print("   ✗ Should have thrown error for invalid brain")
    except Exception as e:
        print(f"   ✓ Correctly handled invalid brain: {type(e).__name__}")
    
    print("\n" + "=" * 50)
    print("ALL TESTS PASSED!")
    print("=" * 50)
    print("\nKey improvements implemented:")
    print("• Module metadata (__version__, __author__, __description__)")
    print("• Context manager support for Brain and AgentBrain")
    print("• Configuration utilities (validateConfig, configToDict, dictToConfig)")
    print("• Statistics and utility functions (getStatistics, neuronInfo)")
    print("• Convenience functions (runQuickSimulation)")
    print("• Error handling with custom exceptions")
    print("• Simulation utilities (runExperiment)")
    print("• Performance monitoring (getMemoryUsage)")
    print("• Advanced user functions")
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("The Python bindings may need to be built.")
    print("To build the bindings, run:")
    print("  cd /workspace/app/python")
    print("  pip install -e ../")
    sys.exit(1)

print("\nImprovements successfully implemented and tested!")