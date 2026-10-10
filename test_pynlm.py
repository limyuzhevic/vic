#!/usr/bin/env python3
"""
Simple test to verify the NLM Python module structure.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

def test_imports():
    """Test that all required modules can be imported."""
    print("Testing NLM Python module imports...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    
    try:
        from pynlm import Config, Brain, SimpleWorld, AgentBrain
        print("✓ Successfully imported core classes")
    except ImportError as e:
        print(f"✗ Failed to import core classes: {e}")
        return False
    
    try:
        from pynlm import NeuronType, SynapseType, ActionType
        print("✓ Successfully imported enums")
    except ImportError as e:
        print(f"✗ Failed to import enums: {e}")
        return False
    
    try:
        from pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
        print("✓ Successfully imported factory functions")
    except ImportError as e:
        print(f"✗ Failed to import factory functions: {e}")
        return False
    
    try:
        from pynlm import INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
        print("✓ Successfully imported constants")
    except ImportError as e:
        print(f"✗ Failed to import constants: {e}")
        return False
    
    return True

def test_factory_functions():
    """Test that factory functions work."""
    print("\nTesting factory functions...")
    
    try:
        from pynlm import createDefaultConfig, createBrain
        
        # Create config
        config = createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        # Create brain
        brain = createBrain(config)
        print("✓ createBrain() works")
        
        # Initialize brain
        result = brain.initialize()
        if result:
            print("✓ brain.initialize() works")
        else:
            print("✗ brain.initialize() failed")
            return False
        
        # Test basic brain methods
        neuron_count = brain.getTotalNeuronCount()
        print(f"✓ brain.getTotalNeuronCount() returns {neuron_count}")
        
        synapse_count = brain.getTotalSynapseCount()
        print(f"✓ brain.getTotalSynapseCount() returns {synapse_count}")
        
        # Test step
        brain.step(0)
        print("✓ brain.step(0) works")
        
        # Test reset
        brain.reset()
        print("✓ brain.reset() works")
        
        return True
        
    except Exception as e:
        print(f"✗ Factory function test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enums():
    """Test that enums are accessible."""
    print("\nTesting enums...")
    
    try:
        from pynlm import NeuronType, SynapseType, ActionType
        
        # Test NeuronType
        print(f"✓ NeuronType.Excitatory = {NeuronType.Excitatory}")
        print(f"✓ NeuronType.Inhibitory = {NeuronType.Inhibitory}")
        print(f"✓ NeuronType.Sensory = {NeuronType.Sensory}")
        print(f"✓ NeuronType.Motor = {NeuronType.Motor}")
        
        # Test SynapseType
        print(f"✓ SynapseType.Excitatory = {SynapseType.Excitatory}")
        print(f"✓ SynapseType.Inhibitory = {SynapseType.Inhibitory}")
        
        # Test ActionType
        print(f"✓ ActionType.MoveForward = {ActionType.MoveForward}")
        print(f"✓ ActionType.TurnLeft = {ActionType.TurnLeft}")
        print(f"✓ ActionType.Interact = {ActionType.Interact}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enum test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_constants():
    """Test that constants are accessible."""
    print("\nTesting constants...")
    
    try:
        from pynlm import INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
        
        print(f"✓ INVALID_NEURON_ID = {INVALID_NEURON_ID}")
        print(f"✓ INVALID_SYNAPSE_ID = {INVALID_SYNAPSE_ID}")
        print(f"✓ INVALID_REGION_ID = {INVALID_REGION_ID}")
        print(f"✓ INVALID_POPULATION_ID = {INVALID_POPULATION_ID}")
        
        # Test that they're the expected type
        import numpy as np
        assert isinstance(INVALID_NEURON_ID, np.uint64), f"Expected np.uint64, got {type(INVALID_NEURON_ID)}"
        
        return True
        
    except Exception as e:
        print(f"✗ Constant test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("NLM Python Module Test")
    print("=" * 40)
    
    tests = [
        ("Import Test", test_imports),
        ("Factory Function Test", test_factory_functions),
        ("Enum Test", test_enums),
        ("Constant Test", test_constants),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        print("-" * len(test_name))
        if test_func():
            passed += 1
            print(f"✓ {test_name} PASSED")
        else:
            failed += 1
            print(f"✗ {test_name} FAILED")
    
    print("\n" + "=" * 40)
    print(f"Test Summary: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("\n🎉 All tests passed! The NLM Python module is working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    exit(main())
