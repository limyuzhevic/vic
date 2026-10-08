#!/usr/bin/env python3
"""
Test script for NLM Python bindings.
Tests basic functionality and improvements.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print(f"✓ Successfully imported pynlm v{pynlm.__version__}")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Test 1: Create default config
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        # Test 2: Create brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Test 3: Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Test 4: Run simulation steps
        for i in range(10):
            brain.step(i)
        print("✓ Ran 10 simulation steps")
        
        # Test 5: Get statistics
        neurons = brain.getTotalNeuronCount()
        firing = brain.getFiringNeuronCount()
        spikes = brain.getTotalSpikeCount()
        print(f"✓ Got brain stats: {neurons} neurons, {firing} firing, {spikes} spikes")
        
        # Test 6: Test new API patterns
        stats = brain.formatStats()
        print(f"✓ Formatted stats: {len(stats)} characters")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_pythonic_api():
    """Test new Pythonic API patterns"""
    print("\n=== Testing Pythonic API ===")
    
    try:
        # Test 1: Property-like access
        config = pynlm.createDefaultConfig()
        config.set("test_key", 42)
        value = config.get("test_key")
        if value.has_value() and value.value() == 42:
            print("✓ Property-like access works")
        else:
            print("✗ Property-like access failed")
            return False
            
        # Test 2: Dictionary-like interface
        config["test_dict_key"] = 100
        if config.has("test_dict_key"):
            print("✓ Dictionary-like interface works")
        else:
            print("✗ Dictionary-like interface failed")
            return False
            
        # Test 3: Context manager
        brain = pynlm.createBrain(100, 2)
        brain.initialize()
        with brain.create_context() as ctx:
            ctx.step(0)
            if ctx.isActive():
                print("✓ Context manager works")
            else:
                print("✗ Context manager failed")
                return False
                
        return True
        
    except Exception as e:
        print(f"✗ Pythonic API test failed: {e}")
        return False

def test_helper_functions():
    """Test helper functions and utilities"""
    print("\n=== Testing Helper Functions ===")
    
    try:
        # Test 1: Vision creation helper
        vision = pynlm.createVision(8, 8, 3)
        print("✓ Vision helper works")
        
        # Test 2: Audio creation helper
        audio = pynlm.createAudio(44100, 1024)
        print("✓ Audio helper works")
        
        # Test 3: Brain configuration helper
        config = pynlm.createDefaultConfig()
        pynlm.configureBrain(config, 500, 5)
        neurons = config.get("brain.neuron_count")
        if neurons.has_value() and neurons.value() == 500:
            print("✓ Brain configuration helper works")
        else:
            print("✗ Brain configuration helper failed")
            return False
            
        # Test 4: Preconfigured brains
        simple_brain = pynlm.createBrain("simple")
        print("✓ Simple brain creation works")
        
        complex_brain = pynlm.createBrain("complex")
        print("✓ Complex brain creation works")
        
        return True
        
    except Exception as e:
        print(f"✗ Helper functions test failed: {e}")
        return False

def test_advanced_patterns():
    """Test advanced usage patterns"""
    print("\n=== Testing Advanced Patterns ===")
    
    try:
        # Test 1: Learning agent creation
        brain = pynlm.createBrain("medium")
        agent = pynlm.createLearningAgent(brain)
        status = agent.getLearningStatus()
        if "reward_modulation" in status:
            print("✓ Learning agent creation works")
        else:
            print("✗ Learning agent creation failed")
            return False
            
        # Test 2: World simulation helper
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Note: Full simulation would require implementing the complete agent loop
        # This just tests that the world can be created and configured
        print("✓ World simulation helper works")
        
        # Test 3: Data serialization helpers
        percept = pynlm.SensoryPercept()
        percept_dict = percept.to_dict()
        if isinstance(percept_dict, dict):
            print("✓ Data serialization helpers work")
        else:
            print("✗ Data serialization helpers failed")
            return False
            
        return True
        
    except Exception as e:
        print(f"✗ Advanced patterns test failed: {e}")
        return False

def test_error_handling():
    """Test error handling and validation"""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test 1: Configuration validation
        config = pynlm.createDefaultConfig()
        pynlm.configureBrain(config, 10, 2)  # This should work
        print("✓ Configuration validation works")
        
        # Test 2: Invalid configuration (should raise error)
        try:
            config2 = pynlm.createDefaultConfig()
            pynlm.configureBrain(config2, -1, 2)  # Invalid: negative neurons
            print("✗ Error handling failed - should have raised ValueError")
            return False
        except (ValueError, Exception):
            print("✓ Error handling works - caught invalid configuration")
            
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_module_info():
    """Test module information and utilities"""
    print("\n=== Testing Module Info ===")
    
    try:
        # Test 1: Version info
        version = pynlm.get_version()
        print(f"✓ Version info: {version}")
        
        # Test 2: Constants
        print(f"✓ Constants available: INVALID_NEURON_ID={pynlm.INVALID_NEURON_ID}")
        
        # Test 3: Module info print
        print("\nModule information:")
        pynlm.print_info()
        print("✓ Module info printed")
        
        # Test 4: Example usage
        print("\nExample usage patterns:")
        pynlm.example_usage()
        print("✓ Example usage shown")
        
        return True
        
    except Exception as e:
        print(f"✗ Module info test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Pythonic API", test_pythonic_api),
        ("Helper Functions", test_helper_functions),
        ("Advanced Patterns", test_advanced_patterns),
        ("Error Handling", test_error_handling),
        ("Module Info", test_module_info),
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
            print(f"✗ {test_name} ERROR: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    if passed == total:
        print("🎉 All tests passed!")
        return 0
    else:
        print("❌ Some tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
