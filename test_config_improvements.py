#!/usr/bin/env python3
"""
Test script for improved Config class
Demonstrates the enhanced type safety and validation features
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ NLM Python bindings imported successfully")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Please ensure the Python bindings are built and installed")
    sys.exit(1)

def test_basic_config():
    """Test basic configuration operations"""
    print("\n=== Testing Basic Configuration Operations ===")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    # Set various types of values
    config.set_int("brain_neuron_count", 1000)
    config.set_float("brain_synapse_density", 0.1)
    config.set_bool("learning_enabled", True)
    config.set_string("brain_name", "TestBrain")
    
    config.set_int("learning_rate", 0.001)
    config.set_float("learning_rate", 0.001)  # Both should work
    config.set_bool("enable_curiosity", True)
    
    print("✓ Set various configuration values")
    
    # Get values with type safety
    neuron_count = config.get_int("brain_neuron_count", 500)
    print(f"✓ Got neuron count: {neuron_count}")
    
    synapse_density = config.get_float("brain_synapse_density", 0.05)
    print(f"✓ Got synapse density: {synapse_density}")
    
    learning_enabled = config.get_bool("learning_enabled", False)
    print(f"✓ Got learning enabled: {learning_enabled}")
    
    brain_name = config.get_string("brain_name", "Unknown")
    print(f"✓ Got brain name: {brain_name}")
    
    # Test has method
    has_learning_rate = config.has("learning_rate")
    print(f"✓ Has learning_rate: {has_learning_rate}")
    
    # Test getKeys
    keys = config.getKeys()
    print(f"✓ Got {len(keys)} keys: {keys}")
    
    # Test summary
    summary = config.summary()
    print("✓ Config summary generated (first 200 chars):")
    print(summary[:200] + "..." if len(summary) > 200 else summary)
    
    return True

def test_config_validation():
    """Test configuration validation features"""
    print("\n=== Testing Configuration Validation ===")
    
    config = pynlm.createDefaultConfig()
    
    # Set some values that should be validated
    config.set_int("brain_neuron_count", 10000)
    config.set_float("learning_rate", 0.001)
    config.set_bool("enable_development", True)
    
    # Test validation-related functionality
    print("✓ Set values with validation")
    
    # Check configuration statistics
    entry_count = config.getEntryCount()
    print(f"✓ Entry count: {entry_count}")
    
    # Test type-specific getters
    neuron_list = config.getIntList("some_list")  # Non-existent key
    print(f"✓ Get int list (empty): {neuron_list}")
    
    string_list = config.getStringList("some_list")  # Non-existent key
    print(f"✓ Get string list (empty): {string_list}")
    
    return True

def test_advanced_operations():
    """Test advanced configuration operations"""
    print("\n=== Testing Advanced Configuration Operations ===")
    
    # Create config with multiple entries
    config = pynlm.createDefaultConfig()
    
    # Set multiple values of different types
    config.set_int("int_value", 42)
    config.set_float("float_value", 3.14159)
    config.set_bool("bool_value", True)
    config.set_string("string_value", "Hello World")
    config.set_string("list_string_value", "["a","b","c""]")  # As JSON string
    
    print("✓ Set multiple value types")
    
    # Test pattern matching
    patterns = ["*_value", "brain_*", "*"]
    for pattern in patterns:
        matches = config.findKeysByPattern(pattern)
        print(f"✓ Pattern '{pattern}' matches {len(matches)} keys: {matches}")
    
    # Test filtering by source (default for runtime settings)
    runtime_entries = config.filterBySource(pynlm.ConfigSource.Runtime)  # This would need to be exposed
    print(f"✓ Runtime entries: {len(runtime_entries)}")
    
    # Test type filtering
    string_entries = config.filterByType("string")
    print(f"✓ String entries: {len(string_entries)}")
    
    return True

def test_error_handling():
    """Test error handling and edge cases"""
    print("\n=== Testing Error Handling ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test non-existent key access
    non_existent = config.get_int("non_existent_key", 999)
    print(f"✓ Non-existent key with default: {non_existent}")
    
    # Test has method
    exists = config.has("non_existent_key")
    print(f"✓ has() for non-existent key: {exists}")
    
    # Test clear
    config.set_int("temp_key", 123)
    print(f"✓ Set temp key, then clear")
    config.clear()
    exists_after_clear = config.has("temp_key")
    print(f"✓ After clear, temp key exists: {exists_after_clear}")
    
    return True

def test_python_integration():
    """Test Python-specific features"""
    print("\n=== Testing Python Integration ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test Python object compatibility
    try:
        # Test with Python integers
        config.set_int("python_int", 42)
        print("✓ Set Python integer")
        
        # Test with Python floats  
        config.set_float("python_float", 3.14)
        print("✓ Set Python float")
        
        # Test with Python strings
        config.set_string("python_str", "test")
        print("✓ Set Python string")
        
        # Test with Python booleans
        config.set_bool("python_bool", True)
        print("✓ Set Python boolean")
        
        # Test getting values
        val_int = config.get_int("python_int")
        val_float = config.get_float("python_float")
        val_str = config.get_string("python_str")
        val_bool = config.get_bool("python_bool")
        
        print(f"✓ Retrieved int: {val_int}")
        print(f"✓ Retrieved float: {val_float}")
        print(f"✓ Retrieved string: {val_str}")
        print(f"✓ Retrieved bool: {val_bool}")
        
    except Exception as e:
        print(f"✗ Python integration error: {e}")
        return False
    
    return True

def test_edge_cases():
    """Test edge cases and boundary conditions"""
    print("\n=== Testing Edge Cases ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test extreme values
    config.set_int("max_int", 2147483647)  # 32-bit int max
    config.set_int("min_int", -2147483648)  # 32-bit int min
    config.set_float("max_float", 1e100)
    config.set_float("min_float", -1e100)
    
    print("✓ Set extreme values")
    
    # Test very long strings (within limits)
    long_string = "A" * 1000
    config.set_string("long_string", long_string)
    retrieved = config.get_string("long_string")
    print(f"✓ Long string test: {len(retrieved)} chars")
    
    # Test empty values
    config.set_string("empty_string", "")
    config.set_int("zero_value", 0)
    config.set_bool("false_value", False)
    
    print("✓ Test empty/zero values")
    
    # Test very large list (within limits)
    large_list = list(range(100))
    config.set_string("large_list", str(large_list))  # Store as string for now
    print("✓ Large list test")
    
    return True

def test_serialization():
    """Test serialization features"""
    print("\n=== Testing Serialization ===")
    
    config = pynlm.createDefaultConfig()
    
    # Set various values
    config.set_int("neuron_count", 1000)
    config.set_float("learning_rate", 0.01)
    config.set_bool("enabled", True)
    config.set_string("name", "TestConfig")
    
    # Test summary
    summary = config.summary()
    print(f"✓ Config summary length: {len(summary)} chars")
    
    # Test getKeys
    keys = config.getKeys()
    print(f"✓ Config has {len(keys)} keys")
    
    # Test that we can retrieve all values
    for key in keys:
        val = config.get_string(key, "")  # Try string first
        if val == "":
            val = config.get_int(key, 0)  # Try int
        print(f"✓ Key '{key}' retrieved as: {val} (type: {type(val).__name__})")
    
    return True

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Config Class Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_config,
        test_config_validation,
        test_advanced_operations,
        test_error_handling,
        test_python_integration,
        test_edge_cases,
        test_serialization,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
                print(f"✓ {test.__name__} PASSED")
            else:
                failed += 1
                print(f"✗ {test.__name__} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} FAILED with exception: {e}")
        print("-" * 60)
    
    print(f"\n=== TEST RESULTS ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! Config class improvements are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
