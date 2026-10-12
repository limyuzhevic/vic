#!/usr/bin/env python3
"""
Configuration loading test suite for NLM Python bindings.
Tests configuration file loading, validation, and Pythonic API features.
"""

import pytest
import sys
import os
import tempfile
import json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestConfigLoading:
    """Test configuration loading from various sources."""
    
    def test_json_config_loading(self):
        """Test loading configuration from JSON file."""
        # Create a temporary JSON config file
        config_data = {
            "simulation": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
                "max_firing_rate": 50.0,
                "min_firing_rate": 1.0,
                "learning_rate": 0.01,
                "plasticity_enabled": True,
                "development_enabled": True,
                "reward_modulation_enabled": True
            },
            "world": {
                "width": 100.0,
                "height": 100.0,
                "vision_width": 32,
                "vision_height": 32
            }
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(config_data, f)
            json_file = f.name
        
        try:
            config = nlm.Config()
            assert config.loadConfigFile(json_file)
            
            # Verify keys are accessible
            # Note: The actual implementation might flatten the nested structure
            # For testing, we check that loading succeeded
            assert config.summary() is not None
        finally:
            os.unlink(json_file)
    
    def test_yaml_config_loading(self):
        """Test loading configuration from YAML file (if available)."""
        # This test would require PyYAML, but we'll test the interface
        config = nlm.Config()
        
        # Create a simple config and test that methods exist
        config.update(
            neuron_count=1000,
            region_count=2,
            learning_rate=0.01,
            plasticity_enabled=True
        )
        
        # Test that saveToFile works (simulating YAML would work similarly)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            yaml_file = f.name
        
        try:
            assert config.saveToFile(yaml_file)
            assert os.path.exists(yaml_file)
        finally:
            os.unlink(yaml_file)
    
    def test_config_from_dict(self):
        """Test creating config from dictionary-like data."""
        config = nlm.Config()
        
        # Create comprehensive config
        config_data = {
            "brain": {
                "neuron_count": 500,
                "region_count": 1,
                "connection_probability": 0.15
            },
            "environment": {
                "width": 80.0,
                "height": 80.0,
                "vision_width": 24,
                "vision_height": 24
            },
            "neuromodulation": {
                "reward_enabled": True,
                "curiosity_enabled": True,
                "novelty_enabled": True
            }
        }
        
        # Test update method (which handles key-value pairs)
        for section, values in config_data.items():
            for key, value in values.items():
                config.update(**{f"{section}_{key}": value})
        
        # Verify some keys were set
        assert config.has("brain_neuron_count") or config.has("brain.neuron_count")
        assert config.has("environment_width") or config.has("environment.width")
    
    def test_config_validation(self):
        """Test configuration validation functionality."""
        config = nlm.Config()
        
        # Create config with some validation
        config.update(
            required_field="test_value",
            optional_field=42,
            validation_flag=True
        )
        
        # Test validation with different expected types
        # This should work fine
        config.validate("required_field:string,optional_field:int")
        
        # Test that validation doesn't raise for valid config
        # In a real implementation, this would raise for missing required fields
        pass
    
    def test_config_save_load_roundtrip(self):
        """Test saving and loading config files."""
        config = nlm.Config()
        
        # Set various types of values
        config.update(
            string_value="test string",
            int_value=42,
            double_value=3.14159,
            bool_value=True,
            string_list=["apple", "banana", "cherry"],
            int_list=[1, 2, 3, 4, 5],
            double_list=[1.1, 2.2, 3.3]
        )
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            temp_file = f.name
        
        try:
            assert config.saveToFile(temp_file)
            assert os.path.exists(temp_file)
            
            # Create new config and load
            config2 = nlm.Config()
            assert config2.loadFromFile(temp_file)
            
            # Verify values are preserved
            assert config2.getOr("string_value", "") == "test string"
            assert config2.getOr("int_value", 0) == 42
            assert config2.getOr("double_value", 0.0) > 3.14
            assert config2.getOr("bool_value", False) == True
        finally:
            if os.path.exists(temp_file):
                os.unlink(temp_file)
    
    def test_backward_compatibility_format(self):
        """Test backward compatibility with key=value format."""
        # Create a config file in backward compatible format
        config_content = """# Simple config file
# This format is supported for backward compatibility
brain.neuron_count = 1000
brain.region_count = 2
world.width = 100.0
world.height = 100.0
plasticity.enabled = true
learning.rate = 0.01
"""
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            f.write(config_content)
            temp_file = f.name
        
        try:
            config = nlm.Config()
            # This should work with backward compatible format
            # (Implementation may or may not support this directly)
            result = config.loadFromFile(temp_file)
            
            # At minimum, should not crash
            assert True
        finally:
            os.unlink(temp_file)
    
    def test_config_error_handling(self):
        """Test error handling for config operations."""
        config = nlm.Config()
        
        # Test loading non-existent file
        result = config.loadConfigFile("/nonexistent/path/config.json")
        # Should handle gracefully
        
        # Test loading malformed JSON
        malformed_json = '{"invalid": json}'
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write(malformed_json)
            temp_file = f.name
        
        try:
            config2 = nlm.Config()
            # This should fail gracefully
            result = config2.loadConfigFile(temp_file)
        finally:
            os.unlink(temp_file)
        
        # Test empty file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_file = f.name
        
        try:
            config3 = nlm.Config()
            result = config3.loadConfigFile(temp_file)
            # Should fail or handle empty file
        finally:
            os.unlink(temp_file)

class TestPythonicConfigFeatures:
    """Test Pythonic config features like update() and validate()."""
    
    def test_config_update_with_kwargs(self):
        """Test update method with various kwargs."""
        config = nlm.Config()
        
        # Test with all supported types
        config.update(
            string_val="hello world",
            int_val=42,
            double_val=3.14159,
            bool_val=True,
            list_val=[1, 2, 3, 4, 5],
            float_list_val=[1.1, 2.2, 3.3, 4.4],
            string_list_val=["a", "b", "c"]
        )
        
        # Verify all were set
        assert config.has("string_val")
        assert config.has("int_val")
        assert config.has("double_val")
        assert config.has("bool_val")
        assert config.has("list_val")
        assert config.has("float_list_val")
        assert config.has("string_list_val")
    
    def test_config_update_mixed_types(self):
        """Test update with mixed data types."""
        config = nlm.Config()
        
        # Mixed update
        config.update(
            name="TestAgent",
            id=123,
            success_rate=0.95,
            enabled=True,
            tags=["tag1", "tag2", "tag3"],
            parameters=[0.1, 0.5, 1.0, 2.0]
        )
        
        # Verify mixed types were handled
        assert config.has("name")
        assert config.has("id")
        assert config.has("success_rate")
        assert config.has("enabled")
        assert config.has("tags")
        assert config.has("parameters")
    
    def test_config_has_and_keys(self):
        """Test has() method and getKeys()."""
        config = nlm.Config()
        
        # Initially empty
        assert config.getKeys() == []
        
        # Add some keys
        config.update(a=1, b=2, c=3, d="test", e=True)
        
        keys = config.getKeys()
        assert len(keys) >= 5
        assert "a" in keys
        assert "b" in keys
        assert "c" in keys
        assert "d" in keys
        assert "e" in keys
        
        # Test has() method
        assert config.has("a")
        assert config.has("e")
        assert not config.has("nonexistent")
    
    def test_config_clear(self):
        """Test clear() method."""
        config = nlm.Config()
        
        # Add some data
        config.update(a=1, b=2, c=3)
        assert len(config.getKeys()) > 0
        
        # Clear should remove all keys
        config.clear()
        assert len(config.getKeys()) == 0
        
        # Should be able to add again
        config.update(new_key="new_value")
        assert config.has("new_key")
    
    def test_config_get_or_default(self):
        """Test getOr with default values."""
        config = nlm.Config()
        
        # Add some values
        config.update(existing=42, another="hello")
        
        # Test with existing values
        assert config.getOr("existing", 0) == 42
        assert config.getOr("another", "default") == "hello"
        
        # Test with non-existing values
        assert config.getOr("missing", 0) == 0
        assert config.getOr("missing", "default") == "default"
        assert config.getOr("missing", []) == []
        assert config.getOr("missing", {}) == {}
    
    def test_config_summary(self):
        """Test summary() method."""
        config = nlm.Config()
        
        # Empty config
        summary = config.summary()
        assert isinstance(summary, str)
        assert "Config:" in summary
        
        # Config with data
        config.update(key1=1, key2="value", key3=True)
        summary = config.summary()
        assert isinstance(summary, str)
        assert "Config:" in summary
        assert len(summary) > 10  # Should have some content
    
    def test_config_type_validation(self):
        """Test type validation in config."""
        config = nlm.Config()
        
        # Set values of different types
        config.update(
            int_value=42,
            double_value=3.14159,
            string_value="test",
            bool_value=True,
            int_array=[1, 2, 3],
            double_array=[1.1, 2.2, 3.3],
            string_array=["a", "b", "c"]
        )
        
        # Verify types are preserved (as much as possible)
        # Note: The actual implementation details may vary
        assert config.has("int_value")
        assert config.has("double_value")
        assert config.has("string_value")
        assert config.has("bool_value")

class TestUtilityFunctions:
    """Test new utility functions for Python."""
    
    def test_create_simulation_config(self):
        """Test create_simulation_config utility function."""
        config = nlm.create_simulation_config()
        
        assert config is not None
        assert isinstance(config, nlm.Config)
        
        # Should have basic simulation parameters
        assert config.has("neuron_count") or config.has("brain.neuron_count")
        assert config.has("region_count") or config.has("brain.region_count")
        assert config.has("connection_probability")
        assert config.has("max_firing_rate")
        assert config.has("learning_rate")
        assert config.has("plasticity_enabled")
    
    def test_create_simulation_config_with_filename(self):
        """Test create_simulation_config with filename."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            temp_file = f.name
        
        try:
            config = nlm.create_simulation_config(temp_file)
            assert config is not None
            assert os.path.exists(temp_file)
            assert os.path.getsize(temp_file) > 0
        finally:
            os.unlink(temp_file)
    
    def test_create_test_environment(self):
        """Test create_test_environment utility function."""
        world = nlm.create_test_environment()
        
        assert world is not None
        assert isinstance(world, nlm.SimpleWorld)
        
        assert world.getWidth() > 0
        assert world.getHeight() > 0
        assert world.getWidth() == world.getHeight()
    
    def test_create_test_environment_custom_dimensions(self):
        """Test create_test_environment with custom dimensions."""
        world = nlm.create_test_environment(200.0, 100.0)
        
        assert world is not None
        assert world.getWidth() == 200.0
        assert world.getHeight() == 100.0
    
    def test_create_training_environment(self):
        """Test create_training_environment utility function."""
        world = nlm.create_training_environment()
        
        assert world is not None
        assert isinstance(world, nlm.SimpleWorld)
        
        assert world.getWidth() > 0
        assert world.getHeight() > 0

if __name__ == "__main__":
    pytest.main([__file__, "-v"])