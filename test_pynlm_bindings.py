#!/usr/bin/env python3
"""
Test script for NLM Python bindings
Tests all the new functionality added for improvements
"""

import pynlm
import sys
import traceback

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("=== Testing Basic Functionality ===")
    
    # Test 1: Create a default configuration
    try:
        config = pynlm.createDefaultConfig()
        print("✓ Created default configuration")
    except Exception as e:
        print(f"✗ Failed to create default config: {e}")
        return False
    
    # Test 2: Create a brain with config
    try:
        brain = pynlm.createBrain(config)
        print("✓ Created brain with configuration")
    except Exception as e:
        print(f"✗ Failed to create brain: {e}")
        return False
    
    # Test 3: Initialize brain
    try:
        brain.initialize()
        print("✓ Initialized brain")
    except Exception as e:
        print(f"✗ Failed to initialize brain: {e}")
        return False
    
    # Test 4: Brain step
    try:
        brain.step(0)
        print("✓ Brain step completed")
    except Exception as e:
        print(f"✗ Brain step failed: {e}")
        return False
    
    # Test 5: Brain statistics
    try:
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        firing = brain.getFiringNeuronCount()
        print(f"✓ Brain stats: {neurons} neurons, {synapses} synapses, {firing} firing")
    except Exception as e:
        print(f"✗ Failed to get brain stats: {e}")
        return False
    
    return True

def test_error_handling():
    """Test error handling and exception types"""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Create invalid config for testing
        config = pynlm.createDefaultConfig()
        
        # Test file operation errors (using non-existent file)
        try:
            config.loadFromFile("non_existent_file.json")
            print("✗ Should have thrown FileError for non-existent file")
            return False
        except pynlm.FileError as e:
            print("✓ FileError raised for non-existent file:", str(e)[:50] + "...")
        except Exception as e:
            print(f"✗ Wrong exception type for file error: {type(e).__name__}: {e}")
            return False
        
        # Test invalid input errors
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        # Create a simple sensory input for testing
        from pynlm import Vision
        vision = Vision(8, 8, 3)
        vision.setData([0.0, 0.0, 0.0] * 8 * 8)  # Empty vision data
        
        print("✓ Error handling tests completed")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_pythonic_features():
    """Test Pythonic features like context managers"""
    print("\n=== Testing Pythonic Features ===")
    
    try:
        # Test context manager for Brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        with brain as b:
            b.initialize()
            print("✓ Brain context manager worked")
        
        # Test context manager for Config
        config2 = pynlm.createDefaultConfig()
        with config2 as c:
            c.set("test.key", "test.value")
            print("✓ Config context manager worked")
        
        # Test string representation
        config_str = str(config2)
        print(f"✓ Config string representation: {config_str[:50]}...")
        
        # Test repr
        brain_repr = repr(brain)
        print(f"✓ Brain repr: {brain_repr}")
        
        return True
        
    except Exception as e:
        print(f"✗ Pythonic features test failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_functions():
    """Test advanced factory functions and utilities"""
    print("\n=== Testing Advanced Functions ===")
    
    try:
        # Test createBrainWithDefaults
        brain = pynlm.createBrainWithDefaults()
        print("✓ createBrainWithDefaults worked")
        
        # Test loadConfigFromFile
        import tempfile
        import json
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_data = {"brain": {"neuron_count": 500}}
            json.dump(config_data, f)
            temp_file = f.name
        
        try:
            config = pynlm.loadConfigFromFile(temp_file)
            print("✓ loadConfigFromFile worked")
            
            # Test validateConfig
            pynlm.validateConfig(config)
            print("✓ validateConfig worked")
            
        finally:
            import os
            os.unlink(temp_file)
        
        # Test createAgentFromConfig
        config = pynlm.createDefaultConfig()
        agent = pynlm.createAgentFromConfig(config)
        print("✓ createAgentFromConfig worked")
        
        # Test createStandardWorld
        world = pynlm.createStandardWorld()
        print("✓ createStandardWorld worked")
        
        # Test runQuickTest
        brain2 = pynlm.runQuickTest()
        print("✓ runQuickTest worked")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced functions test failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_agent_world():
    """Test advanced agent-world environment"""
    print("\n=== Testing Advanced Agent-World ===")
    
    try:
        # Test createAgentWorld
        config = pynlm.createDefaultConfig()
        agent_world = pynlm.createAgentWorld(config, enableLearning=True, enableDevelopment=True)
        print("✓ createAgentWorld worked")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced agent-world test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Error Handling", test_error_handling),
        ("Pythonic Features", test_pythonic_features),
        ("Advanced Functions", test_advanced_functions),
        ("Advanced Agent-World", test_advanced_agent_world),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            print(f"✗ {test_name} FAILED with exception: {e}")
            traceback.print_exc()
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed!")
        return 0
    else:
        print("❌ Some tests failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
