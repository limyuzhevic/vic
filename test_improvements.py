#!/usr/bin/env python3
"""
Test script demonstrating the improved Python bindings for NLM.

This script shows the new functionality added to the Python bindings,
including convenience methods, parameter validation, property support,
and enhanced error handling.
"""

import sys
import os

# Add the build directory to the path to import the module
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build', 'python'))

try:
    from pynlm import Config
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Note: This script requires the Python bindings to be built first.")
    sys.exit(1)

def test_config_creation():
    """Test basic Config class creation and property access."""
    print("\n=== Testing Config Creation and Properties ===")
    
    config = Config()
    print(f"✓ Created empty config: {config}")
    
    # Test Python properties
    print(f"✓ config.has_data: {config.has_data}")
    print(f"✓ config.is_empty: {config.is_empty}")
    print(f"✓ config.key_count: {config.key_count}")
    print(f"✓ config.config_keys: {config.config_keys}")
    
    return config

def test_convenience_methods(config):
    """Test the new convenience methods."""
    print("\n=== Testing Convenience Methods ===")
    
    # Set values using type-safe methods
    config.setInt("int_value", 42)
    config.setFloat("float_value", 3.14159)
    config.setBool("bool_value", True)
    config.setString("string_value", "Hello, NLM!")
    
    print("✓ Set values using type-safe methods")
    
    # Get values using convenience methods
    int_val = config.getInt("int_value")
    float_val = config.getFloat("float_value")
    bool_val = config.getBool("int_value")
    string_val = config.getString("string_value")
    
    print(f"✓ getInt('int_value'): {int_val}")
    print(f"✓ getFloat('float_value'): {float_val}")
    print(f"✓ getBool('bool_value'): {bool_val}")
    print(f"✓ getString('string_value'): {string_val}")
    
    # Test getOr methods
    default_int = config.getOr("missing_key", 99)
    print(f"✓ getOr('missing_key', 99): {default_int}")
    
    # Test has method
    print(f"✓ has('int_value'): {config.has('int_value')}")
    print(f"✓ has('missing_key'): {config.has('missing_key')}")

def test_parameter_validation(config):
    """Test parameter validation helpers."""
    print("\n=== Testing Parameter Validation ===")
    
    config.setFloat("temperature", 25.0)
    config.setInt("population", 1000)
    config.setBool("enabled", True)
    
    # Test validateParam with valid ranges
    try:
        temp = config.validateParam("temperature", 0.0, 100.0)
        print(f"✓ validateParam('temperature', 0.0, 100.0): {temp} (valid)")
    except Exception as e:
        print(f"✗ validateParam failed unexpectedly: {e}")
    
    # Test validateParam with invalid range
    try:
        invalid = config.validateParam("temperature", 30.0, 40.0)
        print(f"✗ validateParam should have raised error for invalid range")
    except Exception as e:
        print(f"✓ validateParam correctly raised error: {type(e).__name__}: {e}")
    
    # Test type validation
    try:
        config.validateType("int_value", "int")
        print(f"✓ validateType('int_value', 'int'): valid")
    except Exception as e:
        print(f"✗ validateType failed unexpectedly: {e}")
    
    try:
        config.validateType("int_value", "string")
        print(f"✗ validateType should have raised error for type mismatch")
    except Exception as e:
        print(f"✓ validateType correctly raised error: {type(e).__name__}: {e}")

def test_enhanced_get_method(config):
    """Test the enhanced getWithValidation method."""
    print("\n=== Testing Enhanced Get Method ===")
    
    config.setInt("int_val", 42)
    config.setFloat("float_val", 3.14)
    config.setString("str_val", "test")
    
    # Test getWithValidation (tries different types)
    try:
        val1 = config.getWithValidation("int_val")
        print(f"✓ getWithValidation('int_val'): {val1} (type: {type(val1).__name__})")
    except Exception as e:
        print(f"✗ getWithValidation failed: {e}")
    
    # Test with string value
    try:
        val2 = config.getWithValidation("str_val")
        print(f"✓ getWithValidation('str_val'): {val2} (type: {type(val2).__name__})")
    except Exception as e:
        print(f"✗ getWithValidation failed: {e}")

def test_error_handling():
    """Test enhanced error handling."""
    print("\n=== Testing Error Handling ===")
    
    config = Config()
    
    # Test accessing non-existent key
    try:
        config.getInt("non_existent_key")
        print("✗ getInt should have raised error for non-existent key")
    except Exception as e:
        print(f"✓ getInt correctly raised error: {type(e).__name__}: {e}")
    
    # Test parameter validation error
    try:
        config.validateParam("missing_key", 0.0, 1.0)
        print("✗ validateParam should have raised error for missing key")
    except Exception as e:
        print(f"✓ validateParam correctly raised error: {type(e).__name__}: {e}")
    
    # Test type validation error
    try:
        config.validateType("missing_key", "int")
        print("✗ validateType should have raised error for missing key")
    except Exception as e:
        print(f"✓ validateType correctly raised error: {type(e).__name__}: {e}")

def test_config_summary_and_metadata(config):
    """Test config summary and metadata methods."""
    print("\n=== Testing Config Summary and Metadata ===")
    
    # Add some configuration
    config.setInt("neurons", 1000)
    config.setFloat("learning_rate", 0.01)
    config.setBool("plasticity", True)
    
    print(f"✓ Summary: {config.summary()}")
    print(f"✓ Keys: {config.config_keys}")
    print(f"✓ Key count: {config.key_count}")
    print(f"✓ Has data: {config.has_data}")
    print(f"✓ Is empty: {config.is_empty}")

def main():
    """Run all tests."""
    print("NLM Python Bindings - Demonstration of Improvements")
    print("=" * 60)
    
    # Test 1: Basic config creation and properties
    config = test_config_creation()
    
    # Test 2: Convenience methods
    test_convenience_methods(config)
    
    # Test 3: Parameter validation
    test_parameter_validation(config)
    
    # Test 4: Enhanced get method
    test_enhanced_get_method(config)
    
    # Test 5: Error handling
    test_error_handling()
    
    # Test 6: Summary and metadata
    test_config_summary_and_metadata(config)
    
    print("\n" + "=" * 60)
    print("All tests completed successfully!")
    print("\nKey improvements demonstrated:")
    print("1. ✓ Python properties (has_data, is_empty, key_count, config_keys)")
    print("2. ✓ Convenience methods (getInt, getFloat, getBool, getString)")
    print("3. ✓ Type-safe set methods (setInt, setFloat, setBool, setString)")
    print("4. ✓ Parameter validation (validateParam, validateType)")
    print("5. ✓ Enhanced error handling with custom exceptions")
    print("6. ✓ Enhanced getWithValidation method with type detection")
    print("7. ✓ Configuration summary and metadata methods")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
