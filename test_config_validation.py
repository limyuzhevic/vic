#!/usr/bin/env python3
"""
Test script for NLM Configuration Validation System.

This script tests the basic functionality of the NLM configuration validation system.
"""

import os
import sys
import json
import yaml
from pathlib import Path

# Add the src directory to the path so we can import NLM modules
sys.path.insert(0, str(Path(__file__).parent / "src"))

try:
    from core.Config.Config import Config, ConfigSchema
    from core.Validation.Validation import ValidationFramework, ValidationStrategy
    print("✓ Successfully imported NLM modules")
except ImportError as e:
    print(f"✗ Import error: {e}")
    sys.exit(1)

def test_basic_config_functionality():
    """Test basic configuration functionality."""
    print("\n=== Testing Basic Configuration Functionality ===")
    
    # Test 1: Create config and set values
    config = Config()
    print("✓ Created Config instance")
    
    # Set various types of values
    config.set("int_value", 42)
    config.set("double_value", 3.14)
    config.set("string_value", "Hello World")
    config.set("bool_value", True)
    config.set("vector_value", std::vector<int>({1, 2, 3, 4, 5}))
    
    print("✓ Set various configuration values")
    
    # Test getting values
    int_val = config.get<int>("int_value")
    double_val = config.get<double>("double_value")
    string_val = config.get<std::string>("string_value")
    bool_val = config.get<bool>("bool_value")
    
    print(f"✓ Got values: int={int_val}, double={double_val}, string={string_val}, bool={bool_val}")
    
    # Test getOr with default
    default_val = config.getOr<int>("missing_key", 100)
    print(f"✓ Got default value for missing key: {default_val}")
    
    # Test has
    print(f"✓ Has int_value: {config.has('int_value')}")
    print(f"✓ Has missing_key: {config.has('missing_key')}")
    
    # Test remove
    config.remove("string_value")
    print(f"✓ Removed string_value, has string_value: {config.has('string_value')}")
    
    return config

def test_schema_functionality():
    """Test schema functionality."""
    print("\n=== Testing Schema Functionality ===")
    
    # Create a schema
    schema = ConfigSchema()
    schema.filename = "test_schema.json"
    schema.version = "1.0.0"
    
    # Add fields
    schema.addField("test_int", "int", true, "Test integer field", "0", "1000")
    schema.addField("test_double", "double", true, "Test double field", "0.0", "1.0")
    schema.addField("test_string", "string", false, "Test string field", "", "")
    schema.addField("test_bool", "bool", false, "Test boolean field", "", "")
    
    print("✓ Created and populated schema")
    
    # Test schema validation
    config = Config()
    config.set("test_int", 500)
    config.set("test_double", 0.5)
    config.set("test_string", "test")
    config.set("test_bool", true)
    
    is_valid = schema.validateWithFramework(config)
    print(f"✓ Validated schema: valid={is_valid}")
    
    # Test validation errors
    config.set("test_int", 2000)  # Out of range
    errors = schema.getValidationErrorsWithFramework(config)
    print(f"✓ Validation errors found: {'Yes' if not errors.empty() else 'No'}")
    if errors.empty():
        print(f"  Errors: {errors}")
    
    return schema

def test_validation_framework():
    """Test validation framework."""
    print("\n=== Testing Validation Framework ===")
    
    # Get global validation framework
    framework = ValidationFramework()
    print("✓ Created validation framework")
    
    # Test validation strategy
    framework.setValidationStrategy(ValidationStrategy::Strict)
    print("✓ Set validation strategy to Strict")
    
    framework.setValidationStrategy(ValidationStrategy::Lenient)
    print("✓ Set validation strategy to Lenient")
    
    framework.setValidationStrategy(ValidationStrategy::Report)
    print("✓ Set validation strategy to Report")
    
    return framework

def test_config_io():
    """Test configuration input/output operations."""
    print("\n=== Testing Configuration I/O ===")
    
    config = Config()
    
    # Set some values
    config.set("test_int", 100)
    config.set("test_string", "test_value")
    config.set("test_bool", true)
    
    # Save to JSON
    json_output = config.saveToJSON()
    print(f"✓ Generated JSON: {json_output[:100]}...")
    
    # Save to YAML
    yaml_output = config.saveToYAML()
    print(f"✓ Generated YAML: {yaml_output[:100]}...")
    
    # Test loading from JSON
    config.loadFromJSON(json_output)
    print("✓ Loaded config from JSON")
    
    # Test loading from YAML
    config.loadFromYAML(yaml_output)
    print("✓ Loaded config from YAML")
    
    return config

def main():
    """Main test function."""
    print("NLM Configuration Validation System - Test Suite")
    print("=" * 50)
    
    try:
        # Run tests
        test_basic_config_functionality()
        test_schema_functionality()
        test_validation_framework()
        test_config_io()
        
        print("\n" + "=" * 50)
        print("✓ All tests completed successfully!")
        return 0
        
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
