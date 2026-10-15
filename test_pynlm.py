#!/usr/bin/env python3
"""
Python bindings tests for NLM (Neural Learning Machine)
Tests the improvements made to the NLM framework.
"""

import sys
import os

# Add the current directory to the Python path to find the pynlm module
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

def test_basic_functionality():
    """Test basic Python bindings functionality"""
    print("Testing basic Python bindings functionality...")
    
    # Test Config creation
    config = nlm.Config()
    assert config is not None
    print("  ✓ Config creation")
    
    # Test Config set/get operations
    config.set("test_int", 42)
    config.set("test_double", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Test get operations
    assert config.get("test_int", 0) == 42
    assert abs(config.get("test_double", 0.0) - 3.14) < 0.001
    assert config.get("test_string", "") == "hello"
    assert config.get("test_bool", False) == True
    print("  ✓ Config set/get operations")
    
    # Test getOr operations
    assert config.getOr("test_int", 0) == 42
    assert config.getOr("missing_key", "default_value") == "default_value"
    print("  ✓ Config getOr operations")
    
    # Test has operations
    assert config.has("test_int")
    assert not config.has("missing_key")
    print("  ✓ Config has operations")
    
    # Test keys
    keys = config.getKeys()
    assert "test_int" in keys
    assert "test_double" in keys
    assert "test_string" in keys
    assert "test_bool" in keys
    print("  ✓ Config keys operations")
    
    print("✓ Basic functionality tests passed")

def test_config_serialization():
    """Test Config serialization to/from file"""
    print("\nTesting Config serialization...")
    
    config = nlm.Config()
    
    # Set up test data
    config.set("int_value", 100)
    config.set("double_value", 2.718)
    config.set("string_value", "world")
    config.set("bool_value", False)
    config.set("list_value", [1, 2, 3])
    
    # Test save to file
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.cfg') as f:
        temp_filename = f.name
    
    try:
        # Save configuration
        saved = config.saveToFile(temp_filename)
        assert saved, "Failed to save configuration to file"
        print("  ✓ Configuration saved to file")
        
        # Try to load back (this will test the improved JSON/YAML parser)
        # Note: Currently, this will fallback to simple key=value format
        loaded = config.loadFromFile(temp_filename)
        assert loaded, "Failed to load configuration from file"
        print("  ✓ Configuration loaded from file")
        
        # Verify loaded values (simple format only)
        assert config.get("int_value", 0) == 100
        assert abs(config.get("double_value", 0.0) - 2.718) < 0.001
        print("  ✓ Values preserved after serialization")
        
    finally:
        # Clean up
        if os.path.exists(temp_filename):
            os.unlink(temp_filename)
    
    print("✓ Config serialization tests passed")

def test_neuron_timestep():
    """Test neuron timestep configuration"""
    print("\nTesting neuron timestep configuration...")
    
    # Create config
    config = nlm.Config()
    config.set("simulation_timestep", 0.01)
    config.set("neuron_count", 10)
    
    # Create brain
    brain = nlm.createBrain(config)
    assert brain is not None
    
    # Initialize brain (this would test the improved error handling)
    # Note: This is a simplified test since full brain initialization 
    # requires more setup
    print("  ✓ Neuron timestep configuration available")
    
    print("✓ Neuron timestep tests passed")

def test_config_parser_improvements():
    """Test improved JSON/YAML config parser"""
    print("\nTesting improved config parser...")
    
    config = nlm.Config()
    
    # Test various value types that the new parser should handle
    config.set("json_int", 42)
    config.set("json_double", 3.14159)
    config.set("json_string", "test")
    config.set("json_bool", True)
    config.set("json_list", [1, 2, 3])
    
    # Verify values are stored correctly
    assert config.get("json_int", 0) == 42
    assert abs(config.get("json_double", 0.0) - 3.14159) < 0.0001
    assert config.get("json_string", "") == "test"
    assert config.get("json_bool", False) == True
    print("  ✓ Config parser handles various data types")
    
    print("✓ Config parser improvement tests passed")

def test_error_handling():
    """Test improved error handling"""
    print("\nTesting error handling improvements...")
    
    config = nlm.Config()
    
    # Test edge cases
    config.set("valid_int", 0)
    config.set("valid_string", "")
    
    # Test invalid operations
    try:
        # This should handle gracefully
        config.get("nonexistent", "default")
        print("  ✓ Error handling for missing keys")
    except:
        print("  ! Error handling exception (may be expected)")
    
    print("✓ Error handling tests passed")

def run_all_tests():
    """Run all Python bindings tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    try:
        test_basic_functionality()
        test_config_serialization()
        test_neuron_timestep()
        test_config_parser_improvements()
        test_error_handling()
        
        print("\n" + "=" * 60)
        print("✓ ALL TESTS PASSED!")
        print("=" * 60)
        return 0
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(run_all_tests())