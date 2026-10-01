#!/usr/bin/env python3
"""
Test script for the improved Python bindings in NLM.

This script verifies that the Python bindings work correctly after the improvements.
"""

import sys
import os
import tempfile
import json

def test_bindings_import():
    """Test that the pynlm module can be imported."""
    print("Testing module import...")
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        return pynlm
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return None

def test_config_basics(config):
    """Test basic Config functionality."""
    print("\nTesting Config basics...")
    
    # Test creation
    config = config.create_config()
    print("✓ Config created")
    
    # Test setting values
    config.set("learning_rate", 0.01)
    config.set("batch_size", 32)
    config.set("enabled", True)
    print("✓ Values set")
    
    # Test getting values
    lr = config.get("learning_rate")
    assert lr is not None, "learning_rate should not be None"
    assert lr == 0.01, f"Expected 0.01, got {lr}"
    print(f"✓ learning_rate retrieved: {lr}")
    
    bs = config.get("batch_size")
    assert bs is not None, "batch_size should not be None"
    assert bs == 32, f"Expected 32, got {bs}"
    print(f"✓ batch_size retrieved: {bs}")
    
    # Test get_or with default
    missing = config.get_or("missing_key", 100)
    assert missing == 100, f"Expected 100, got {missing}"
    print(f"✓ get_or with default works: {missing}")
    
    # Test keys
    keys = config.keys()
    assert len(keys) >= 3, f"Expected at least 3 keys, got {len(keys)}"
    print(f"✓ keys() works: {keys}")
    
    # Test items
    items = config.items()
    assert len(items) >= 3, f"Expected at least 3 items, got {len(items)}"
    print(f"✓ items() works: {len(items)} items")
    
    # Test update
    config.update({"new_key": "new_value", "another_key": 42})
    assert config.get("new_key") == "new_value"
    assert config.get("another_key") == 42
    print("✓ update() works")
    
    # Test merge
    config2 = config.create_config()
    config2.set("extra_key", "extra_value")
    config.merge(config2)
    assert config.get("extra_key") == "extra_value"
    print("✓ merge() works")
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str), f"Summary should be string, got {type(summary)}"
    print(f"✓ summary() works: {summary}")
    
    print("✓ All Config tests passed")

def test_config_file_operations(config):
    """Test file operations with Config."""
    print("\nTesting Config file operations...")
    
    config_obj = config.create_config()
    
    # Set some test values
    config_obj.set("test_int", 123)
    config_obj.set("test_float", 3.14159)
    config_obj.set("test_string", "hello world")
    config_obj.set("test_bool", True)
    
    # Create temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_path = f.name
    
    try:
        # Test save_to_file
        success = config_obj.save_to_file(temp_path)
        assert success, "save_to_file should return True"
        print("✓ save_to_file() works")
        
        # Verify file content
        with open(temp_path, 'r') as f:
            saved_data = json.load(f)
        
        assert "test_int" in saved_data
        assert saved_data["test_int"] == 123
        print("✓ File content is correct")
        
        # Test load_from_file
        config_obj2 = config.create_config()
        success = config_obj2.load_from_file(temp_path)
        assert success, "load_from_file should return True"
        print("✓ load_from_file() works")
        
        # Verify loaded data
        assert config_obj2.get("test_int") == 123
        assert config_obj2.get("test_float") is not None
        assert config_obj2.get("test_string") == "hello world"
        assert config_obj2.get("test_bool") is True
        print("✓ File data loaded correctly")
        
    finally:
        # Clean up
        if os.path.exists(temp_path):
            os.unlink(temp_path)
    
    print("✓ All file operation tests passed")

def test_config_validation(config):
    """Test Config validation."""
    print("\nTesting Config validation...")
    
    config_obj = config.create_config()
    
    # Set test values
    config_obj.set("int_val", 42)
    config_obj.set("float_val", 3.14)
    config_obj.set("string_val", "test")
    config_obj.set("bool_val", True)
    
    # Test valid schema
    schema = {
        "int_val": "int",
        "float_val": "float", 
        "string_val": "str",
        "bool_val": "bool"
    }
    
    success = config_obj.validate(schema)
    assert success, "validate() should return True for valid schema"
    print("✓ validate() works with valid schema")
    
    # Test invalid schema
    invalid_schema = {
        "int_val": "str",  # Wrong type
        "missing_key": "int"  # Missing key
    }
    
    try:
        config_obj.validate(invalid_schema)
        assert False, "validate() should raise exception for invalid schema"
    except Exception as e:
        assert "Validation errors:" in str(e)
        print(f"✓ validate() correctly raises exception: {str(e)[:100]}...")
    
    print("✓ All validation tests passed")

def test_neuron_id(neuron_id):
    """Test NeuronId functionality."""
    print("\nTesting NeuronId...")
    
    # Test creation
    id1 = neuron_id(100)
    id2 = neuron_id()
    print("✓ NeuronId creation works")
    
    # Test properties
    assert id1.value == 100
    assert id1.index() == 100
    print("✓ NeuronId properties work")
    
    # Test comparison
    id3 = neuron_id(100)
    assert id1 == id3
    assert id1 != id2
    print("✓ NeuronId comparison works")
    
    # Test hash
    ids = {id1, id2, id3}
    assert len(ids) == 2  # id1 and id3 are the same
    print("✓ NeuronId hash works")
    
    # Test string representation
    str_repr = str(id1)
    assert "NeuronId" in str_repr
    assert "100" in str_repr
    print("✓ NeuronId string representation works")
    
    print("✓ All NeuronId tests passed")

def test_world_creation(world):
    """Test world creation and basic functionality."""
    print("\nTesting world creation...")
    
    world_obj = world()
    print("✓ World created")
    
    # Test configuration
    world_obj.configure(100.0, 100.0, 84, 84)
    print("✓ World configured")
    
    # Test dimensions
    assert world_obj.get_width() == 100.0
    assert world_obj.get_height() == 100.0
    print("✓ World dimensions work")
    
    # Test reset
    world_obj.reset()
    print("✓ World reset")
    
    print("✓ All world tests passed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing NLM Python Bindings Improvements")
    print("=" * 60)
    
    # Test module import
    pynlm = test_bindings_import()
    if not pynlm:
        print("\n✗ Failed to import pynlm module. Tests cannot proceed.")
        return 1
    
    try:
        # Test Config
        test_config_basics(pynlm)
        
        # Test file operations
        test_config_file_operations(pynlm)
        
        # Test validation
        test_config_validation(pynlm)
        
        # Test NeuronId
        test_neuron_id(pynlm.NeuronId)
        
        # Test world creation
        test_world_creation(pynlm.create_simple_world)
        
        print("\n" + "=" * 60)
        print("✓ All tests passed successfully!")
        print("=" * 60)
        return 0
        
    except Exception as e:
        print(f"\n✗ Test failed with exception: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())