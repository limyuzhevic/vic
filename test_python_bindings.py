#!/usr/bin/env python3
"""
Python bindings test for NLM (Neural Learning Machine)
Tests the enhanced Pythonic interface for the Config class and other components.
"""

import sys
import os
import json
import tempfile
from pathlib import Path

# Add the Python bindings to the path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYTHON_AVAILABLE = True
except ImportError as e:
    print(f"Python bindings not available: {e}")
    PYTHON_AVAILABLE = False

class TestPythonBindings:
    """Test class for Python bindings functionality."""
    
    def __init__(self):
        if not PYTHON_AVAILABLE:
            raise RuntimeError("Python bindings not available")
        
        self.test_dir = Path(tempfile.mkdtemp())
        print(f"Created test directory: {self.test_dir}")
    
    def cleanup(self):
        """Clean up test files."""
        import shutil
        if self.test_dir.exists():
            shutil.rmtree(self.test_dir)
            print(f"Cleaned up test directory: {self.test_dir}")
    
    def test_basic_config(self):
        """Test basic Config class functionality."""
        print("\n=== Testing Basic Config Class ===")
        
        # Test default Config
        config = pynlm.Config()
        assert not config.has("test_key"), "New config should not have test_key"
        
        # Test setting and getting values
        config.set("test_int", 42)
        assert config.has("test_int"), "Config should have test_int"
        
        int_val = config.get("test_int")
        assert int_val == 42, f"Expected 42, got {int_val}"
        
        # Test with default value
        missing_val = config.get("missing_key", "default_value")
        assert missing_val == "default_value", f"Expected 'default_value', got {missing_val}"
        
        print("✓ Basic Config functionality test passed")
    
    def test_enhanced_config(self):
        """Test EnhancedConfig class with Pythonic interface."""
        print("\n=== Testing EnhancedConfig Class ===")
        
        # Test creating enhanced config
        enhanced_config = pynlm.EnhancedConfig.createDefault()
        assert enhanced_config.size() == 0, "New enhanced config should be empty"
        assert enhanced_config.empty(), "New enhanced config should be empty"
        
        # Test setting values with different types
        enhanced_config.set("string_value", "hello world")
        enhanced_config.set("int_value", 123)
        enhanced_config.set("float_value", 3.14159)
        enhanced_config.set("bool_value", True)
        enhanced_config.set("array_value", [1.0, 2.0, 3.0, 4.0, 5.0])
        
        # Test using indexing
        assert enhanced_config["string_value"] == "hello world"
        assert enhanced_config["int_value"] == 123
        assert enhanced_config["float_value"] == 3.14159
        assert enhanced_config["bool_value"] == True
        assert enhanced_config["array_value"] == [1.0, 2.0, 3.0, 4.0, 5.0]
        
        # Test 'in' operator
        assert "string_value" in enhanced_config
        assert "missing_key" not in enhanced_config
        
        # Test get with defaults
        assert enhanced_config.get("string_value") == "hello world"
        assert enhanced_config.get("missing_key", "default_value") == "default_value"
        
        # Test getOr method
        assert enhanced_config.getOr("string_value", "default") == "hello world"
        assert enhanced_config.getOr("missing_key", "default") == "default"
        
        # Test contains method
        assert enhanced_config.contains("string_value")
        assert not enhanced_config.contains("missing_key")
        
        # Test dictionary interface
        dict_config = enhanced_config.getAsDict()
        assert dict_config["string_value"] == "hello world"
        assert dict_config["int_value"] == 123
        
        # Test update method
        new_values = {"key1": "value1", "key2": 42, "key3": True, "key4": [1.0, 2.0]}
        enhanced_config.update(new_values)
        assert enhanced_config["key1"] == "value1"
        assert enhanced_config["key2"] == 42
        assert enhanced_config["key3"] == True
        assert enhanced_config["key4"] == [1.0, 2.0]
        
        # Test iteration
        key_count = 0
        for key in enhanced_config:
            assert isinstance(key, str)
            key_count += 1
        assert key_count > 0, "Should have iterated over some keys"
        
        # Test length
        assert len(enhanced_config) > 0
        
        # Test delete
        del enhanced_config["key1"]
        assert "key1" not in enhanced_config
        
        print("✓ EnhancedConfig test passed")
    
    def test_file_operations(self):
        """Test file loading and saving."""
        print("\n=== Testing File Operations ===")
        
        # Create enhanced config and set values
        config = pynlm.EnhancedConfig.createDefault()
        config.set("brain_name", "TestBrain")
        config.set("neuron_count", 1000)
        config.set("learning_rate", 0.01)
        config.set("enabled", True)
        
        # Save to file
        test_file = self.test_dir / "test_config.json"
        success = config.saveToFile(str(test_file))
        assert success, "saveToFile should return True"
        
        # Verify file was created
        assert test_file.exists(), "File should be created"
        
        # Load from file
        config2 = pynlm.EnhancedConfig.createDefault()
        success = config2.loadFromFile(str(test_file))
        assert success, "loadFromFile should return True"
        
        # Verify values
        assert config2["brain_name"] == "TestBrain"
        assert config2["neuron_count"] == 1000
        assert config2["learning_rate"] == 0.01
        assert config2["enabled"] == True
        
        print("✓ File operations test passed")
    
    def test_json_parsing(self):
        """Test JSON parsing from file."""
        print("\n=== Testing JSON Parsing ===")
        
        # Create JSON content with different types
        json_content = {
            "name": "Test Brain",
            "neuron_count": 2000,
            "region_count": 10,
            "connection_probability": 0.15,
            "enable_plasticity": True,
            "learning_rate": 0.05,
            "parameters": [0.1, 0.2, 0.3, 0.4, 0.5],
            "description": "JSON test configuration"
        }
        
        # Write JSON to file
        json_file = self.test_dir / "test.json"
        with open(json_file, 'w') as f:
            json.dump(json_content, f)
        
        # Load with enhanced config
        config = pynlm.EnhancedConfig.createDefault()
        success = config.loadFromFile(str(json_file))
        assert success, "JSON loading should succeed"
        
        # Verify JSON parsing
        assert config["name"] == "Test Brain"
        assert config["neuron_count"] == 2000
        assert config["region_count"] == 10
        assert config["connection_probability"] == 0.15
        assert config["enable_plasticity"] == True
        assert config["learning_rate"] == 0.05
        assert config["parameters"] == [0.1, 0.2, 0.3, 0.4, 0.5]
        assert config["description"] == "JSON test configuration"
        
        print("✓ JSON parsing test passed")
    
    def test_dictionary_operations(self):
        """Test dictionary-like operations."""
        print("\n=== Testing Dictionary Operations ===")
        
        config = pynlm.EnhancedConfig.createDefault()
        
        # Test that config has dictionary-like behavior
        config.update({
            "nested": {
                "inner_key": "inner_value",
                "inner_number": 42
            }
        })
        
        # Note: Currently we don't support nested dictionaries, but we can test flat dictionaries
        config.update({
            "key1": "value1",
            "key2": 123,
            "key3": True,
            "key4": [1.0, 2.0, 3.0]
        })
        
        # Verify flat dictionary operations
        assert config["key1"] == "value1"
        assert config["key2"] == 123
        assert config["key3"] == True
        assert config["key4"] == [1.0, 2.0, 3.0]
        
        # Test asDict method
        as_dict = config.getAsDict()
        assert as_dict["key1"] == "value1"
        assert as_dict["key2"] == 123
        
        print("✓ Dictionary operations test passed")
    
    def test_config_summary(self):
        """Test configuration summary functionality."""
        print("\n=== Testing Configuration Summary ===")
        
        config = pynlm.EnhancedConfig.createDefault()
        
        # Add some values
        config.set("key1", "value1")
        config.set("key2", 42)
        config.set("key3", True)
        
        # Test string representation
        config_str = str(config)
        assert "Configuration" in config_str
        assert "2 entries" in config_str
        
        # Test repr
        config_repr = repr(config)
        assert "<Config:" in config_repr
        
        print("✓ Configuration summary test passed")
    
    def test_comprehensive_workflow(self):
        """Test comprehensive workflow with all features."""
        print("\n=== Testing Comprehensive Workflow ===")
        
        # Create new config
        config = pynlm.EnhancedConfig.createDefault()
        
        # Step 1: Set up initial configuration
        config.update({
            "system_name": "NLM_Test_Brain",
            "version": "1.0.0",
            "max_neurons": 5000,
            "learning_enabled": True,
            "plasticity_enabled": True,
            "development_stages": 5,
            "reward_weights": [1.0, 0.5, 0.1]
        })
        
        # Step 2: Verify initial setup
        assert config["system_name"] == "NLM_Test_Brain"
        assert config["max_neurons"] == 5000
        assert config["learning_enabled"] == True
        assert config["development_stages"] == 5
        
        # Step 3: Modify some values
        config.set("learning_rate", 0.001)
        config.set("temperature", 25.0)  # New key
        config["debug_mode"] = False  # Using indexing
        
        # Step 4: Verify modifications
        assert config["learning_rate"] == 0.001
        assert config["temperature"] == 25.0
        assert config["debug_mode"] == False
        
        # Step 5: Remove some values
        del config["temperature"]
        assert "temperature" not in config
        
        # Step 6: Save to file
        config_file = self.test_dir / "comprehensive_config.json"
        assert config.saveToFile(str(config_file))
        
        # Step 7: Load into new config
        config2 = pynlm.EnhancedConfig.createDefault()
        assert config2.loadFromFile(str(config_file))
        
        # Step 8: Verify loaded config
        assert config2["system_name"] == "NLM_Test_Brain"
        assert config2["max_neurons"] == 5000
        assert config2["learning_rate"] == 0.001
        assert "temperature" not in config2  # Should be removed
        
        # Step 9: Test summary
        summary = config.summary()
        assert "Configuration" in summary
        
        # Step 10: Test dictionary conversion
        config_dict = config.getAsDict()
        assert config_dict["system_name"] == "NLM_Test_Brain"
        assert config_dict["max_neurons"] == 5000
        
        print("✓ Comprehensive workflow test passed")
    
    def run_all_tests(self):
        """Run all Python bindings tests."""
        print("=" * 60)
        print("Running Python Bindings Tests for NLM")
        print("=" * 60)
        
        try:
            self.test_basic_config()
            self.test_enhanced_config()
            self.test_file_operations()
            self.test_json_parsing()
            self.test_dictionary_operations()
            self.test_config_summary()
            self.test_comprehensive_workflow()
            
            print("\n" + "=" * 60)
            print("✅ ALL PYTHON BINDINGS TESTS PASSED!")
            print("=" * 60)
            
        except Exception as e:
            print(f"\n❌ TEST FAILED: {e}")
            import traceback
            traceback.print_exc()
            return False
        finally:
            self.cleanup()
        
        return True

def main():
    """Main test runner."""
    if not PYTHON_AVAILABLE:
        print("❌ Python bindings not available. Cannot run tests.")
        return 1
    
    try:
        tester = TestPythonBindings()
        success = tester.run_all_tests()
        return 0 if success else 1
    except Exception as e:
        print(f"❌ Test setup failed: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
