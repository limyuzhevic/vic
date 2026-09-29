#!/usr/bin/env python3
"""
Comprehensive Configuration System Tests for NLM

Tests JSON/YAML parsing and serialization, nested configuration support,
schema validation, and backward compatibility with legacy format.
"""

import sys
import os
import json
import tempfile
import pytest
import yaml
from pathlib import Path

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config, ConfigSchema
except ImportError as e:
    print(f"Error importing Config modules: {e}")
    print("Make sure NLM is properly built.")
    sys.exit(1)
class TestConfiguration:
    """Test configuration system features"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        # Create a temporary directory for test files
        self.temp_dir = tempfile.mkdtemp()
        self.json_file = os.path.join(self.temp_dir, "test_config.json")
        self.yaml_file = os.path.join(self.temp_dir, "test_config.yaml")
        self.legacy_file = os.path.join(self.temp_dir, "legacy_config")
    
    def teardown_method(self):
        """Clean up after each test method"""
        import shutil
        if os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)
    
    def test_json_parsing(self):
        """Test JSON parsing functionality"""
        test_config = {
            "neuron_count": 100,
            "plasticity_rate": 0.1,
            "simulation_speed": 1.0,
            "working_memory_capacity": 50,
            "learning_enabled": True,
            "features": ["spiking", "plasticity", "prediction"],
            "parameters": {
                "threshold": -55.0,
                "resting_potential": -70.0,
                "adaptation_rate": 0.01
            }
        }
        
        # Save to JSON
        with open(self.json_file, 'w') as f:
            json.dump(test_config, f, indent=2)
        
        # Load from JSON file
        config = Config()
        success = config.loadFromJsonFile(self.json_file)
        assert success == True
        
        # Verify values
        assert config.getOr("neuron_count", 0) == 100
        assert config.getOr("plasticity_rate", 0.0) == 0.1
        assert config.getOr("simulation_speed", 0.0) == 1.0
        assert config.getOr("working_memory_capacity", 0) == 50
        assert config.getOr("learning_enabled", False) == True
        
        # Verify array values
        features = config.getOr("features", [])
        assert features == ["spiking", "plasticity", "prediction"]
        
        # Verify nested object (dot notation)
        threshold = config.getOr("parameters.threshold", 0.0)
        assert threshold == -55.0
    
    def test_yaml_parsing(self):
        """Test YAML parsing functionality"""
        test_config = {
            "neuron_count": 200,
            "plasticity_rate": 0.15,
            "simulation_speed": 0.8,
            "working_memory_capacity": 100,
            "learning_enabled": False,
            "features": ["spiking", "plasticity", "prediction", "neuromodulation"],
            "parameters": {
                "threshold": -60.0,
                "resting_potential": -65.0,
                "adaptation_rate": 0.02
            }
        }
        
        # Save to YAML
        with open(self.yaml_file, 'w') as f:
            yaml.dump(test_config, f)
        
        # Load from YAML file
        config = Config()
        success = config.loadFromYamlFile(self.yaml_file)
        assert success == True
        
        # Verify values
        assert config.getOr("neuron_count", 0) == 200
        assert config.getOr("plasticity_rate", 0.0) == 0.15
        assert config.getOr("simulation_speed", 0.0) == 0.8
        assert config.getOr("working_memory_capacity", 0) == 100
        assert config.getOr("learning_enabled", False) == False
        
        # Verify array values
        features = config.getOr("features", [])
        assert features == ["spiking", "plasticity", "prediction", "neuromodulation"]
        
        # Verify nested object (dot notation)
        threshold = config.getOr("parameters.threshold", 0.0)
        assert threshold == -60.0
    
    def test_json_serialization(self):
        """Test JSON serialization functionality"""
        config = Config()
        config.set("neuron_count", 100)
        config.set("plasticity_rate", 0.1)
        config.set("working_memory_capacity", 50)
        config.set("learning_enabled", True)
        config.set("features", ["spiking", "plasticity"])
        
        # Save to JSON file
        success = config.saveToJsonFile(self.json_file)
        assert success == True
        
        # Load from file and verify
        config2 = Config()
        success2 = config2.loadFromJsonFile(self.json_file)
        assert success2 == True
        
        # Verify all values
        assert config2.getOr("neuron_count", 0) == 100
        assert config2.getOr("plasticity_rate", 0.0) == 0.1
        assert config2.getOr("working_memory_capacity", 0) == 50
        assert config2.getOr("learning_enabled", False) == True
        
        features = config2.getOr("features", [])
        assert features == ["spiking", "plasticity"]
    
    def test_yaml_serialization(self):
        """Test YAML serialization functionality"""
        config = Config()
        config.set("neuron_count", 150)
        config.set("plasticity_rate", 0.12)
        config.set("working_memory_capacity", 75)
        config.set("learning_enabled", True)
        config.set("features", ["spiking", "plasticity", "prediction"])
        
        # Save to YAML file
        success = config.saveToYamlFile(self.yaml_file)
        assert success == True
        
        # Load from file and verify
        config2 = Config()
        success2 = config2.loadFromYamlFile(self.yaml_file)
        assert success2 == True
        
        # Verify all values
        assert config2.getOr("neuron_count", 0) == 150
        assert config2.getOr("plasticity_rate", 0.0) == 0.12
        assert config2.getOr("working_memory_capacity", 0) == 75
        assert config2.getOr("learning_enabled", False) == True
        
        features = config2.getOr("features", [])
        assert features == ["spiking", "plasticity", "prediction"]
    
    def test_legacy_format_support(self):
        """Test backward compatibility with legacy key=value format"""
        legacy_content = """# Legacy configuration file
neuron_count = 100
plasticity_rate = 0.1
simulation_speed = 1.0
working_memory_capacity = 50
learning_enabled = true
features = spiking,plasticity,prediction
"""
        
        # Save legacy content
        with open(self.legacy_file, 'w') as f:
            f.write(legacy_content)
        
        # Load from legacy file
        config = Config()
        success = config.loadFromFile(self.legacy_file)
        assert success == True
        
        # Verify basic values (note: legacy parsing may have limitations)
        assert config.has("neuron_count")
        assert config.has("plasticity_rate")
        assert config.has("simulation_speed")
        assert config.has("working_memory_capacity")
        
        # Verify integer and boolean values
        neuron_count = config.getOr("neuron_count", 0)
        assert neuron_count == 100
        
        learning_enabled = config.getOr("learning_enabled", False)
        assert learning_enabled == True
    
    def test_nested_configuration(self):
        """Test nested configuration support with dot notation"""
        config = Config()
        
        # Set nested values using dot notation
        config.set("brain.neuron_count", 100)
        config.set("brain.plasticity_rate", 0.1)
        config.set("brain.simulation_speed", 1.0)
        config.set("brain.working_memory_capacity", 50)
        
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.stdp.trace_decay", 0.1)
        
        config.set("neuromodulation.dopamine.baseline", 0.5)
        config.set("neuromodulation.dopamine.sensitivity", 0.8)
        
        config.set("prediction.horizon", 3)
        config.set("prediction.accuracy_threshold", 0.9)
        
        # Test nested get
        neuron_count = config.getOr("brain.neuron_count", 0)
        assert neuron_count == 100
        
        learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.0)
        assert learning_rate == 0.01
        
        baseline = config.getOr("neuromodulation.dopamine.baseline", 0.0)
        assert baseline == 0.5
        
        horizon = config.getOr("prediction.horizon", 0)
        assert horizon == 3
        
        # Test key extraction for nested keys
        keys = config.getKeys()
        brain_keys = [k for k in keys if k.startswith("brain.")]
        plasticity_keys = [k for k in keys if k.startswith("plasticity.")]
        
        assert len(brain_keys) == 4
        assert len(plasticity_keys) == 2
    
    def test_schema_validation(self):
        """Test schema validation functionality"""
        # Create a schema
        schema = ConfigSchema("test_schema")
        schema.addField("neuron_count", "int", "Number of neurons", "1-10000")
        schema.addField("plasticity_rate", "double", "Plasticity rate", "0.0-1.0")
        schema.addField("working_memory_capacity", "int", "Working memory capacity", "0-1000")
        schema.addField("learning_enabled", "bool", "Whether learning is enabled", "")
        
        # Create valid config
        valid_config = Config()
        valid_config.set("neuron_count", 100)
        valid_config.set("plasticity_rate", 0.1)
        valid_config.set("working_memory_capacity", 50)
        valid_config.set("learning_enabled", True)
        
        # Test validation
        validation_result = valid_config.validateSchema(schema)
        assert validation_result.isValid() == True
        
        # Create invalid config (values out of range)
        invalid_config = Config()
        invalid_config.set("neuron_count", 0)  # Below minimum
        invalid_config.set("plasticity_rate", 1.5)  # Above maximum
        invalid_config.set("working_memory_capacity", 1500)  # Above maximum
        
        invalid_validation = invalid_config.validateSchema(schema)
        assert invalid_validation.isValid() == False
        
        # Test field validation
        assert invalid_config.validateField("neuron_count", schema) == False
        assert invalid_config.validateField("plasticity_rate", schema) == False
        assert invalid_config.validateField("working_memory_capacity", schema) == False
        assert invalid_config.validateField("learning_enabled", schema) == True  # Valid type and no range
    
    def test_validation_error_handling(self):
        """Test validation error handling and reporting"""
        schema = ConfigSchema("error_test")
        schema.addField("critical_param", "int", "Critical parameter", "10-100")
        schema.addField("optional_param", "double", "Optional parameter", "0.0-1.0")
        
        config = Config()
        config.set("critical_param", 5)  # Invalid: too low
        config.set("optional_param", 2.0)  # Invalid: too high
        
        validation_result = config.validateSchema(schema)
        assert validation_result.isValid() == False
        assert len(validation_result.errors) > 0
        
        # Check error messages
        error_messages = validation_result.toString()
        assert "error" in error_messages.lower()
        
        # Test last error access
        last_error = config.getLastError()
        assert last_error != ""
    
    def test_config_merge_and_inheritance(self):
        """Test configuration merging and inheritance"""
        # Base config
        base_config = Config()
        base_config.set("neuron_count", 100)
        base_config.set("plasticity_rate", 0.1)
        base_config.set("working_memory_capacity", 50)
        
        # Override config
        override_config = Config()
        override_config.set("plasticity_rate", 0.2)  # Override
        override_config.set("simulation_speed", 2.0)  # New parameter
        
        # Test merge with override
        merged_config = Config()
        merged_config.mergeFrom(base_config, override=True)
        merged_config.mergeFrom(override_config, override=True)
        
        # Verify merged values
        assert merged_config.getOr("neuron_count", 0) == 100  # From base
        assert merged_config.getOr("plasticity_rate", 0.0) == 0.2  # From override
        assert merged_config.getOr("working_memory_capacity", 0) == 50  # From base
        assert merged_config.getOr("simulation_speed", 0.0) == 2.0  # From override
        
        # Test merge without override
        merged_no_override = Config()
        merged_no_override.mergeFrom(base_config, override=False)
        merged_no_override.mergeFrom(override_config, override=False)
        
        # Override values should be ignored
        assert merged_no_override.getOr("plasticity_rate", 0.0) == 0.1  # From base, not override
        assert merged_no_override.getOr("simulation_speed", 0.0) == 2.0  # New parameter added
    
    def test_config_clear_and_reset(self):
        """Test configuration clearing and resetting"""
        config = Config()
        config.set("neuron_count", 100)
        config.set("plasticity_rate", 0.1)
        config.set("working_memory_capacity", 50)
        
        # Verify values exist
        assert config.has("neuron_count")
        assert config.has("plasticity_rate")
        assert config.has("working_memory_capacity")
        
        # Clear config
        config.clear()
        
        # Verify all values are gone
        assert not config.has("neuron_count")
        assert not config.has("plasticity_rate")
        assert not config.has("working_memory_capacity")
        
        # Test summary method
        summary = config.summary()
        assert "Configuration" in summary
        assert "0 entries" in summary
    
    def test_config_error_handling(self):
        """Test configuration error handling"""
        # Test loading non-existent file
        config = Config()
        success = config.loadFromJsonFile("/non/existent/file.json")
        assert success == False
        
        # Verify error is set
        error = config.getLastError()
        assert error != ""
        
        # Test saving to invalid path
        # This may not work depending on permissions, but we can test the error handling
        test_config = Config()
        test_config.set("test_key", "test_value")
        
        # The save should attempt but may fail
        # We just verify it doesn't crash
        try:
            test_config.saveToJsonFile("/invalid/path/test.json")
        except:
            pass  # Expected to fail
    
    def test_config_entry_management(self):
        """Test configuration entry management"""
        config = Config()
        
        # Add multiple entries
        config.set("key1", "value1")
        config.set("key2", 42)
        config.set("key3", 3.14)
        config.set("key4", True)
        
        # Verify entries exist
        assert config.has("key1")
        assert config.has("key2")
        assert config.has("key3")
        assert config.has("key4")
        
        # Verify get methods work
        assert config.getOr("key1", "") == "value1"
        assert config.getOr("key2", 0) == 42
        assert abs(config.getOr("key3", 0.0) - 3.14) < 0.01
        assert config.getOr("key4", False) == True
        
        # Test remove
        config.remove("key2")
        assert not config.has("key2")
        
        # Test keys extraction
        keys = config.getKeys()
        assert "key1" in keys
        assert "key2" not in keys
        assert "key3" in keys
        assert "key4" in keys
        
        # Test entries extraction
        entries = config.getEntries()
        entry_keys = [entry.key for entry in entries]
        assert "key1" in entry_keys
        assert "key2" not in entry_keys
        assert "key3" in entry_keys
        assert "key4" in entry_keys
    
    def test_configuration_types(self):
        """Test different configuration value types"""
        config = Config()
        
        # Test different types
        config.set("string_value", "hello")
        config.set("int_value", 42)
        config.set("int64_value", 9223372036854775807)  # Large int
        config.set("double_value", 3.14159)
        config.set("bool_value", True)
        config.set("int_list", [1, 2, 3, 4, 5])
        config.set("double_list", [1.1, 2.2, 3.3])
        config.set("string_list", ["a", "b", "c"])
        
        # Verify retrieval
        assert config.getOr("string_value", "") == "hello"
        assert config.getOr("int_value", 0) == 42
        assert config.getOr("double_value", 0.0) == 3.14159
        assert config.getOr("bool_value", False) == True
        
        int_list = config.getOr("int_list", [])
        assert int_list == [1, 2, 3, 4, 5]
        
        double_list = config.getOr("double_list", [])
        assert double_list == [1.1, 2.2, 3.3]
        
        string_list = config.getOr("string_list", [])
        assert string_list == ["a", "b", "c"]


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
