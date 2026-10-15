#!/usr/bin/env python3
"""
Python bindings test for NLM (Neural Learning Machine)

This script tests the Python bindings for the NLM neural network simulator.
To run these tests, you need to install the pynlm module.

Usage:
    python3 test_bindings.py

Prerequisites:
    1. Build the NLM C++ library and Python bindings
    2. Install the pynlm module

The tests cover:
    - Basic configuration loading and saving
    - Brain creation and simulation
    - Agent functionality
    - Error handling
"""

import sys
import os
import tempfile
import traceback

try:
    import pynlm
    print("pynlm imported successfully")
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    print("Please build and install the NLM Python bindings first.")
    sys.exit(1)

def test_basic_config():
    """Test basic configuration functionality"""
    print("\n=== Testing Basic Configuration ===")
    
    config = pynlm.Config()
    
    # Test setting different types of values
    config.set("int_value", 42)
    config.set("double_value", 3.14159)
    config.set("string_value", "Hello, NLM!")
    config.set("bool_value", True)
    
    # Test getting values
    assert config.get("int_value") == 42
    assert config.get("double_value") > 3.14
    assert config.get("string_value") == "Hello, NLM!"
    assert config.get("bool_value") == True
    
    # Test getOr for missing values
    assert config.getOr("missing", "default") == "default"
    assert config.getOr("missing", 999) == 999
    
    print("✓ Basic configuration tests passed")
    return True

def test_config_file_io():
    """Test configuration file loading and saving"""
    print("\n=== Testing Configuration File I/O ===")
    
    config = pynlm.Config()
    
    # Set some values
    config.set("test_int", 123)
    config.set("test_string", "test_value")
    config.set("test_bool", False)
    
    # Save to a temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        temp_file = f.name
    
    try:
        # Save configuration
        success = config.saveToFile(temp_file)
        assert success, "Failed to save configuration to file"
        
        # Load configuration from file
        config2 = pynlm.Config()
        success = config2.loadFromFile(temp_file)
        assert success, "Failed to load configuration from file"
        
        # Verify values were loaded correctly
        assert config2.get("test_int") == 123
        assert config2.get("test_string") == "test_value"
        assert config2.get("test_bool") == False
        
        print("✓ Configuration file I/O tests passed")
        return True
        
    finally:
        # Clean up temporary file
        if os.path.exists(temp_file):
            os.unlink(temp_file)

def test_json_config():
    """Test JSON configuration format"""
    print("\n=== Testing JSON Configuration ===")
    
    config = pynlm.Config()
    
    # Set some values
    config.set("json_int", 100)
    config.set("json_string", "json_test")
    config.set("json_bool", True)
    
    # Save as JSON file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        # Save configuration
        success = config.saveToFile(temp_file)
        assert success, "Failed to save configuration as JSON"
        
        # Load configuration as JSON
        config2 = pynlm.Config()
        success = config2.loadFromFile(temp_file)
        assert success, "Failed to load configuration as JSON"
        
        # Verify values were loaded correctly
        assert config2.get("json_int") == 100
        assert config2.get("json_string") == "json_test"
        assert config2.get("json_bool") == True
        
        print("✓ JSON configuration tests passed")
        return True
        
    finally:
        # Clean up temporary file
        if os.path.exists(temp_file):
            os.unlink(temp_file)

def test_brain_creation():
    """Test brain creation and basic functionality"""
    print("\n=== Testing Brain Creation ===")
    
    config = pynlm.Config()
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    # Create a brain
    brain = pynlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    
    # Test basic properties
    assert brain.getConfig() is not None
    assert brain.getRegionCount() >= 0
    assert brain.getTotalNeuronCount() >= 0
    
    print("✓ Brain creation tests passed")
    return True

def test_agent_brain():
    """Test agent brain functionality"""
    print("\n=== Testing Agent Brain ===")
    
    config = pynlm.Config()
    brain = pynlm.createBrain(config)
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    assert agent_brain is not None, "Failed to create agent brain"
    
    # Test basic properties
    assert agent_brain.getBrain() is not None
    assert agent_brain.getSensoryInputSize() >= 0
    assert agent_brain.getMotorOutputSize() >= 0
    
    # Test initialization with world
    world = pynlm.createSimpleWorld()
    agent_brain.initialize(world)
    
    print("✓ Agent brain tests passed")
    return True

def test_error_handling():
    """Test error handling"""
    print("\n=== Testing Error Handling ===")
    
    config = pynlm.Config()
    
    # Test getting non-existent key
    # Note: The current implementation might throw an exception or return None
    # This depends on the actual Python binding implementation
    try:
        # This might not throw an exception depending on the binding
        val = config.get("non_existent_key")
        # If no exception, val should be None
        print(f"✓ Non-existent key handled (returned: {val})")
    except Exception as e:
        print(f"✓ Non-existent key correctly raised exception: {type(e).__name__}")
    
    # Test loading from non-existent file
    config2 = pynlm.Config()
    success = config2.loadFromFile("/non/existent/path/file.cfg")
    assert not success, "Should fail to load from non-existent file"
    
    print("✓ Error handling tests passed")
    return True

def test_config_summary():
    """Test configuration summary functionality"""
    print("\n=== Testing Configuration Summary ===")
    
    config = pynlm.Config()
    config.set("summary_test", "test_value")
    config.set("another_field", 123)
    
    # Test summary method
    summary = config.summary()
    assert isinstance(summary, str)
    assert "Configuration" in summary
    assert "summary_test" in summary
    assert "another_field" in summary
    
    print("✓ Configuration summary tests passed")
    return True

def run_all_tests():
    """Run all Python binding tests"""
    print("=== NLM Python Bindings Test Suite ===")
    print("Testing the Python bindings for NLM neural network simulator")
    
    tests = [
        ("Basic Configuration", test_basic_config),
        ("Configuration File I/O", test_config_file_io),
        ("JSON Configuration", test_json_config),
        ("Brain Creation", test_brain_creation),
        ("Agent Brain", test_agent_brain),
        ("Error Handling", test_error_handling),
        ("Configuration Summary", test_config_summary),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\nRunning {test_name}...")
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"✗ {test_name} FAILED (returned False)")
        except Exception as e:
            failed += 1
            print(f"✗ {test_name} FAILED: {e}")
            traceback.print_exc()
    
    print("\n" + "="*50)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("="*50)
    
    if failed == 0:
        print("🎉 All Python binding tests passed!")
        return True
    else:
        print("❌ Some tests failed")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)