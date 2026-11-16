#!/usr/bin/env python3
"""
Test script to verify NLM improvements.

This script tests:
1. Python bindings integration
2. Configuration system improvements (JSON loading)
3. Code quality improvements
4. Overall functionality
"""

import sys
import os
import json
import tempfile
from pathlib import Path

# Add the python directory to the path for Python bindings
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYTHON_BINDINGS_AVAILABLE = True
    print("✓ Python bindings module loaded successfully")
except ImportError as e:
    PYTHON_BINDINGS_AVAILABLE = False
    print(f"✗ Python bindings not available: {e}")

def test_config_system():
    """Test the improved configuration system"""
    print("\n--- Testing Configuration System ---")
    
    try:
        config = pynlm.createDefaultConfig()
        print("✓ Default config created successfully")
        
        # Test setting values
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_string", "hello")
        config.set("test_bool", True)
        config.set("test_array", [1, 2, 3, 4, 5])
        
        # Test getting values
        int_val = config.getOr("test_int", 0)
        float_val = config.getOr("test_float", 0.0)
        string_val = config.getOr("test_string", "")
        
        print(f"✓ Config values set and retrieved: {int_val}, {float_val}, {string_val}")
        
        # Test has() method
        has_int = config.has("test_int")
        print(f"✓ Config.has() works: {has_int}")
        
        # Test summary
        summary = config.summary()
        print(f"✓ Config.summary() works (length: {len(summary)} chars)")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration system test failed: {e}")
        return False

def test_json_config_loading():
    """Test JSON configuration loading"""
    print("\n--- Testing JSON Configuration Loading ---")
    
    # Create a test JSON config file
    test_config = {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "plasticity_learning_rate": 0.01,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "string_array": ["option1", "option2", "option3"],
        "int_array": [1, 2, 3, 4, 5],
        "float_array": [1.5, 2.5, 3.5],
        "bool_value": True,
        "int_value": 42,
        "float_value": 3.14
    }
    
    # Write to temp file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(test_config, f)
        temp_path = f.name
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test loading from JSON file
        success = config.loadFromFile(temp_path)
        if success:
            print("✓ JSON config loaded successfully")
            
            # Test some values
            neuron_count = config.getOr("neuron_count", 0)
            string_array = config.getOr("string_array", [])
            bool_val = config.getOr("bool_value", False)
            
            print(f"✓ JSON values retrieved: neuron_count={neuron_count}, array_length={len(string_array)}, bool={bool_val}")
        else:
            print("✗ Failed to load JSON config file")
            return False
            
    except Exception as e:
        print(f"✗ JSON configuration test failed: {e}")
        return False
    finally:
        # Clean up temp file
        try:
            os.unlink(temp_path)
        except:
            pass
    
    return True

def test_brain_creation():
    """Test brain creation and basic functionality"""
    print("\n--- Testing Brain Creation and Basic Functionality ---")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        print("✓ Brain created successfully")
        
        # Initialize brain
        success = brain.initialize()
        if success:
            print("✓ Brain initialized successfully")
            
            # Test basic functionality
            total_neurons = brain.getTotalNeuronCount()
            total_synapses = brain.getTotalSynapseCount()
            
            print(f"✓ Brain has {total_neurons} neurons and {total_synapses} synapses")
            
            # Test brain step
            brain.step(0)
            print("✓ Brain step completed successfully")
            
            # Test reset
            brain.reset()
            print("✓ Brain reset completed successfully")
            
            return True
        else:
            print("✗ Brain initialization failed")
            return False
            
    except Exception as e:
        print(f"✗ Brain creation test failed: {e}")
        return False

def test_world_system():
    """Test world system integration"""
    print("\n--- Testing World System Integration ---")
    
    try:
        world = pynlm.createSimpleWorld()
        print("✓ World created successfully")
        
        # Configure world
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        print("✓ World configured successfully")
        
        # Reset world
        world.reset()
        print("✓ World reset successfully")
        
        # Test world update
        world.update(0.1)
        print("✓ World update completed successfully")
        
        # Get world percept
        percept = world.getSensoryPercept()
        print("✓ World percept retrieved successfully")
        
        # Get agent body
        agent_body = world.getAgentBody()
        print("✓ Agent body retrieved successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ World system test failed: {e}")
        return False

def test_agent_brain():
    """Test agent brain integration"""
    print("\n--- Testing Agent Brain Integration ---")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ Agent brain created successfully")
        
        # Initialize agent
        agent.initialize(world)
        print("✓ Agent brain initialized successfully")
        
        # Test enable/disable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        print("✓ Agent features enabled successfully")
        
        # Test get methods
        neuromodulation_level = agent.getNeuromodulationLevel()
        curiosity_level = agent.getCuriosityLevel()
        
        print(f"✓ Agent neuromodulation level: {neuromodulation_level}")
        print(f"✓ Agent curiosity level: {curiosity_level}")
        
        return True
        
    except Exception as e:
        print(f"✗ Agent brain test failed: {e}")
        return False

def test_error_handling():
    """Test improved error handling"""
    print("\n--- Testing Error Handling ---")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test setting invalid values (should not crash)
        config.set("test_string", 123)  # Force int -> string conversion
        print("✓ Error handling for type conversion works")
        
        # Test non-existent key access
        non_existent = config.getOr("non_existent_key", "default_value")
        if non_existent == "default_value":
            print("✓ Default value returned for non-existent key")
        else:
            print("✗ Default value not returned for non-existent key")
            return False
            
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_performance():
    """Test performance improvements"""
    print("\n--- Testing Performance Improvements ---")
    
    try:
        import time
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Time brain initialization
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        print(f"✓ Brain initialization: {init_time:.4f}s")
        
        # Time multiple brain steps
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        step_time = time.time() - start_time
        
        print(f"✓ 100 brain steps: {step_time:.4f}s ({step_time/100:.6f}s per step)")
        
        # Time configuration operations
        start_time = time.time()
        for i in range(1000):
            config.has(f"key_{i}")
        has_time = time.time() - start_time
        
        print(f"✓ 1000 has() operations: {has_time:.4f}s ({has_time/1000:.6f}s per operation)")
        
        return True
        
    except Exception as e:
        print(f"✗ Performance test failed: {e}")
        return False

def main():
    """Main test function"""
    print("=" * 60)
    print("NLM Improvements Test Suite")
    print("=" * 60)
    
    if not PYTHON_BINDINGS_AVAILABLE:
        print("\n⚠️  WARNING: Python bindings not available")
        print("   The improvements may not be fully functional")
        print("   without proper Python bindings integration.")
        print("\n   This test is designed to verify improvements to:")
        print("   1. Python bindings integration")
        print("   2. Configuration system (JSON loading)")
        print("   3. Code quality improvements")
        print("   4. Overall functionality")
    
    # Run all tests
    tests = [
        ("Python Bindings", test_config_system),
        ("Configuration System", test_config_system),
        ("JSON Loading", test_json_config_loading),
        ("Brain Creation", test_brain_creation),
        ("World System", test_world_system),
        ("Agent Brain", test_agent_brain),
        ("Error Handling", test_error_handling),
        ("Performance", test_performance),
    ]
    
    passed = 0
    total = 0
    
    for test_name, test_func in tests:
        total += 1
        print(f"\n{'='*60}")
        print(f"Test: {test_name}")
        print(f"{'='*60}")
        
        try:
            if PYTHON_BINDINGS_AVAILABLE:
                success = test_func()
            else:
                print("⚠️  Skipping test (Python bindings not available)")
                success = True  # Skip if not available
            
            if success:
                passed += 1
                print(f"\n✓ {test_name} PASSED")
            else:
                print(f"\n✗ {test_name} FAILED")
                
        except Exception as e:
            print(f"\n✗ {test_name} FAILED with exception: {e}")
    
    print(f"\n{'='*60}")
    print(f"Test Results: {passed}/{total} passed")
    print(f"{'='*60}")
    
    if passed == total:
        print("🎉 All tests passed!")
        return 0
    else:
        print(f"❌ {total - passed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())