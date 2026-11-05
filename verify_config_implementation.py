#!/usr/bin/env python3
"""
Quick verification script for NLM JSON/YAML configuration support.
This script tests the basic functionality of the new configuration system.
"""

import os
import sys
import tempfile
from pathlib import Path

# Add parent directory to path for pynlm import
sys.path.insert(0, str(Path(__file__).parent.parent))

import pynlm


def test_basic_functionality():
    """Test basic configuration functionality."""
    print("Testing basic configuration functionality...")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Set some values
    config.set("test_string", "hello")
    config.set("test_integer", 42)
    config.set("test_float", 3.14)
    config.set("test_boolean", True)
    
    # Verify values
    assert config.has("test_string"), "test_string not found"
    assert config.has("test_integer"), "test_integer not found"
    assert config.has("test_float"), "test_float not found"
    assert config.has("test_boolean"), "test_boolean not found"
    
    # Check value types
    string_val = config.get("test_string")
    assert string_val.has_value() and string_val.value() == "hello", f"Expected 'hello', got {string_val.value()}"
    
    int_val = config.get("test_integer")
    assert int_val.has_value() and int_val.value() == 42, f"Expected 42, got {int_val.value()}"
    
    float_val = config.get("test_float")
    assert float_val.has_value() and abs(float_val.value() - 3.14) < 0.01, f"Expected 3.14, got {float_val.value()}"
    
    bool_val = config.get("test_boolean")
    assert bool_val.has_value() and bool_val.value() == True, f"Expected True, got {bool_val.value()}"
    
    print("  ✓ Basic functionality test passed")


def test_save_load_json():
    """Test saving and loading configuration in JSON format."""
    print("Testing JSON save/load functionality...")
    
    config = pynlm.createDefaultConfig()
    
    # Set some values
    config.set("json_test", "json_value")
    config.set("json_number", 123)
    
    # Save to JSON file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        f.flush()
        
        try:
            success = config.saveToJsonFile(f.name)
            assert success, "Failed to save to JSON file"
            
            # Load back from JSON file
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromJsonFile(f.name)
            assert success, "Failed to load from JSON file"
            
            # Verify values
            assert config2.has("json_test"), "json_test not found"
            assert config2.has("json_number"), "json_number not found"
            
            test_val = config2.get("json_test")
            assert test_val.has_value() and test_val.value() == "json_value", f"Expected 'json_value', got {test_val.value()}"
            
            num_val = config2.get("json_number")
            assert num_val.has_value() and num_val.value() == 123, f"Expected 123, got {num_val.value()}"
            
            print("  ✓ JSON save/load test passed")
            
        finally:
            os.unlink(f.name)


def test_save_load_yaml():
    """Test saving and loading configuration in YAML format."""
    print("Testing YAML save/load functionality...")
    
    config = pynlm.createDefaultConfig()
    
    # Set some values
    config.set("yaml_test", "yaml_value")
    config.set("yaml_number", 456)
    
    # Save to YAML file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.flush()
        
        try:
            success = config.saveToYamlFile(f.name)
            assert success, "Failed to save to YAML file"
            
            # Load back from YAML file
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromYamlFile(f.name)
            assert success, "Failed to load from YAML file"
            
            # Verify values
            assert config2.has("yaml_test"), "yaml_test not found"
            assert config2.has("yaml_number"), "yaml_number not found"
            
            test_val = config2.get("yaml_test")
            assert test_val.has_value() and test_val.value() == "yaml_value", f"Expected 'yaml_value', got {test_val.value()}"
            
            num_val = config2.get("yaml_number")
            assert num_val.has_value() and num_val.value() == 456, f"Expected 456, got {num_val.value()}"
            
            print("  ✓ YAML save/load test passed")
            
        finally:
            os.unlink(f.name)


def test_auto_detect_format():
    """Test auto-detection of file format."""
    print("Testing auto-detection of format...")
    
    # Test JSON detection
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        f.write('{"auto_test": 789}')
        f.flush()
        
        try:
            config = pynlm.createDefaultConfig()
            success = config.loadFromFile(f.name)
            assert success, "Failed to auto-detect JSON"
            assert config.has("auto_test"), "auto_test not found"
            assert config.get("auto_test").has_value() and config.get("auto_test").value() == 789, f"Expected 789, got {config.get('auto_test').value()}"
            print("  ✓ JSON auto-detection test passed")
        finally:
            os.unlink(f.name)
    
    # Test YAML detection
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write('auto_test: 890\n')
        f.flush()
        
        try:
            config = pynlm.createDefaultConfig()
            success = config.loadFromFile(f.name)
            assert success, "Failed to auto-detect YAML"
            assert config.has("auto_test"), "auto_test not found"
            assert config.get("auto_test").has_value() and config.get("auto_test").value() == 890, f"Expected 890, got {config.get('auto_test').value()}"
            print("  ✓ YAML auto-detection test passed")
        finally:
            os.unlink(f.name)
    
    # Test key=value detection
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write('auto_test = 901\n')
        f.flush()
        
        try:
            config = pynlm.createDefaultConfig()
            success = config.loadFromFile(f.name)
            assert success, "Failed to auto-detect key=value"
            assert config.has("auto_test"), "auto_test not found"
            assert config.get("auto_test").has_value() and config.get("auto_test").value() == 901, f"Expected 901, got {config.get('auto_test').value()}"
            print("  ✓ key=value auto-detection test passed")
        finally:
            os.unlink(f.name)


def test_config_summary():
    """Test configuration summary functionality."""
    print("Testing configuration summary...")
    
    config = pynlm.createDefaultConfig()
    
    # Add some values
    config.set("summary_test", "summary_value")
    config.set("summary_number", 999)
    
    # Get summary
    summary = config.summary()
    assert "summary" in summary.lower(), "Summary should contain 'summary'"
    assert "test" in summary.lower(), "Summary should contain 'test'"
    assert "number" in summary.lower(), "Summary should contain 'number'"
    
    print("  ✓ Configuration summary test passed")


def test_config_to_json_yaml():
    """Test conversion to JSON and YAML."""
    print("Testing conversion to JSON/YAML...")
    
    config = pynlm.createDefaultConfig()
    
    # Set some values
    config.set("convert_test", "convert_value")
    config.set("convert_number", 111)
    
    # Test conversion to JSON
    json_string = config.toJson()
    assert json_string.startswith('{'), "JSON should start with '{'"
    assert '"convert_test"' in json_string, "JSON should contain 'convert_test'"
    assert '"convert_number"' in json_string, "JSON should contain 'convert_number'"
    
    # Test conversion to YAML
    yaml_string = config.toYaml()
    assert 'convert_test:' in yaml_string, "YAML should contain 'convert_test:'"
    assert 'convert_number:' in yaml_string, "YAML should contain 'convert_number:'"
    
    print("  ✓ Conversion to JSON/YAML test passed")


def main():
    """Run all verification tests."""
    print("NLM JSON/YAML Configuration System Verification")
    print("=" * 50)
    
    try:
        test_basic_functionality()
        test_save_load_json()
        test_save_load_yaml()
        test_auto_detect_format()
        test_config_summary()
        test_config_to_json_yaml()
        
        print("\n" + "=" * 50)
        print("All verification tests passed! ✓")
        print("\nConfiguration system is working correctly with:")
        print("- Backward compatibility with key=value format")
        print("- JSON configuration support")
        print("- YAML configuration support")
        print("- Automatic format detection")
        print("- Full API support")
        return 0
        
    except Exception as e:
        print(f"\nVerification failed: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
