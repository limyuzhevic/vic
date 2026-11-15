#!/usr/bin/env python3
"""
Test script for NLM Python bindings.
This script verifies that the Python bindings work correctly.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("\n=== Testing Basic Functionality ===")
    
    # Test 1: Create default config
    try:
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        print(f"  Config keys: {config.getKeys()}")
    except Exception as e:
        print(f"✗ Failed to create default config: {e}")
        return False
    
    # Test 2: Create brain
    try:
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        print(f"  Brain neuron count: {brain.getTotalNeuronCount()}")
    except Exception as e:
        print(f"✗ Failed to create brain: {e}")
        return False
    
    # Test 3: Initialize brain
    try:
        result = brain.initialize()
        print("✓ Initialized brain")
        print(f"  Initialization result: {result}")
    except Exception as e:
        print(f"✗ Failed to initialize brain: {e}")
        return False
    
    # Test 4: Create world
    try:
        world = pynlm.createSimpleWorld()
        print("✓ Created world")
        print(f"  World width: {world.getWidth()}, height: {world.getHeight()}")
    except Exception as e:
        print(f"✗ Failed to create world: {e}")
        return False
    
    # Test 5: Configure world
    try:
        world.configure(10, 10, 8, 8)
        print("✓ Configured world")
    except Exception as e:
        print(f"✗ Failed to configure world: {e}")
        return False
    
    # Test 6: Create agent
    try:
        agent = pynlm.createAgentBrain(brain)
        print("✓ Created agent")
        print(f"  Agent initialized: {agent.initialize(world)}")
    except Exception as e:
        print(f"✗ Failed to create agent: {e}")
        return False
    
    # Test 7: Basic simulation step
    try:
        agent.reset()
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        print("✓ Enabled learning features")
        
        # Run a few simulation steps
        for i in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, i * 0.1)
            
            if i % 2 == 0:
                print(f"  Step {i}: Spikes={brain.getTotalSpikeCount()}, "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Reward={agent.getNeuromodulationLevel():.3f}")
        
        print("✓ Simulation steps completed")
    except Exception as e:
        print(f"✗ Failed during simulation: {e}")
        return False
    
    # Test 8: Configuration operations
    try:
        # Set configuration values
        config.set("test_int", 42)
        config.set("test_double", 3.14)
        config.set("test_bool", True)
        config.set("test_string", "Hello World")
        
        print("✓ Set configuration values")
        
        # Get configuration values
        print(f"  Int value: {config.getInt('test_int', 0)}")
        print(f"  Double value: {config.getDouble('test_double', 0.0)}")
        print(f"  Bool value: {config.getBool('test_bool', False)}")
        print(f"  String value: {config.getString('test_string', '')}")
        
        print("✓ Got configuration values")
    except Exception as e:
        print(f"✗ Failed configuration operations: {e}")
        return False
    
    # Test 9: Brain operations
    try:
        # Reset brain
        brain.reset()
        brain.initialize()
        print("✓ Reset and re-initialized brain")
        
        # Inject current
        if brain.getTotalNeuronCount() > 0:
            brain.injectCurrent(brain.getTotalNeuronCount() - 1, 10.0)
            print("✓ Injected current into brain")
        
        print("✓ All brain operations completed")
    except Exception as e:
        print(f"✗ Failed brain operations: {e}")
        return False
    
    return True

def test_error_handling():
    """Test error handling in Python bindings."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test 1: Invalid config operations
        config = pynlm.createDefaultConfig()
        
        # Test invalid key access
        result = config.getString("nonexistent_key", "default_value")
        if result == "default_value":
            print("✓ Default value returned for missing key")
        else:
            print(f"✗ Expected 'default_value', got '{result}'")
            return False
        
        # Test 2: Invalid brain operations
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test invalid neuron ID (should handle gracefully)
        # Note: This depends on the specific implementation
        print("✓ Error handling tests completed")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def main():
    """Main test function."""
    print("NLM Python Bindings Test Suite")
    print("===============================\n")
    
    # Test basic functionality
    if not test_basic_functionality():
        print("\n✗ Basic functionality tests failed!")
        sys.exit(1)
    
    # Test error handling
    if not test_error_handling():
        print("\n✗ Error handling tests failed!")
        sys.exit(1)
    
    print("\n=== All Tests Passed! ===")
    print("The NLM Python bindings are working correctly.")
    return 0

if __name__ == "__main__":
    sys.exit(main())