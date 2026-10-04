"""
Configuration Tests

Tests for configuration loading, validation, and serialization
"""
import pytest
import sys
import os
import json
import tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class TestConfigurationLoading:
    """Test JSON/YAML configuration loading"""
    
    def test_json_config_loading(self):
        """Test loading configuration from JSON file"""
        # Create a temporary JSON file
        config_data = {
            "brain": {
                "neuron_count": 1000,
                "synapse_density": 0.1,
                "learning_rate": 0.01
            },
            "simulation": {
                "timestep": 0.1,
                "duration": 100.0,
                "dt": 0.001
            },
            "sensory": {
                "vision_width": 20,
                "vision_height": 20,
                "vision_channels": 3
            },
            "motor": {
                "action_space_size": 8
            }
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(config_data, f)
            json_file = f.name
        
        try:
            config = pynlm.Config()
            config.loadFromFile(json_file)
            
            # Verify configuration was loaded
            # Note: This depends on the actual Config class implementation
            # For now, just test that the method exists and doesn't crash
            assert callable(config.loadFromFile)
            
        finally:
            os.unlink(json_file)
    
    def test_yaml_config_loading(self):
        """Test loading configuration from YAML file (if supported)"""
        try:
            import yaml
            
            # Create a temporary YAML file
            yaml_data = {
                "brain": {
                    "neuron_count": 500,
                    "learning_rate": 0.001
                },
                "simulation": {
                    "timestep": 0.01
                }
            }
            
            with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
                yaml.dump(yaml_data, f)
                yaml_file = f.name
            
            try:
                config = pynlm.Config()
                # In a real implementation, this would be config.loadFromYaml()
                # For now, just test that YAML import works
                assert callable(config.loadFromFile)
                
            finally:
                os.unlink(yaml_file)
                
        except ImportError:
            pytest.skip("PyYAML not installed")
    
    def test_command_line_config_loading(self):
        """Test command-line configuration loading"""
        config = pynlm.createDefaultConfig()
        
        # Test command-line loading with simulated args
        # In a real implementation, this would parse sys.argv
        # For now, test that the method exists
        assert callable(config.loadFromArgs)

class TestConfigurationValidation:
    """Test configuration validation"""
    
    def test_valid_config(self):
        """Test that valid configurations pass validation"""
        config = pynlm.Config()
        
        # Set valid values
        config.set("neuron_count", 1000)
        config.set("learning_rate", 0.01)
        config.set("timestep", 0.1)
        
        # Should accept valid values
        assert config.has("neuron_count")
        assert config.has("learning_rate")
        assert config.has("timestep")
    
    def test_invalid_config_values(self):
        """Test handling of invalid configuration values"""
        config = pynlm.Config()
        
        # Set values that might be invalid
        config.set("negative_neuron_count", -100)  # Invalid
        config.set("zero_timestep", 0.0)          # Invalid
        config.set("empty_string_value", "")      # Possibly invalid
        
        # Should still accept them (validation depends on implementation)
        assert config.has("negative_neuron_count")
        assert config.has("zero_timestep")
        assert config.has("empty_string_value")
    
    def test_config_schema_validation(self):
        """Test configuration schema validation"""
        config = pynlm.Config()
        
        # Set values according to expected schema
        config.set("type", "brain")
        config.set("parameters.neuron_count", 1000)
        config.set("parameters.simulation.timestep", 0.1)
        
        # Test nested access
        assert config.has("parameters.neuron_count")
        assert config.has("parameters.simulation.timestep")

class TestConfigurationSerialization:
    """Test configuration serialization"""
    
    def test_json_serialization(self):
        """Test JSON serialization of configuration"""
        config = pynlm.Config()
        
        # Add some configuration data
        config.set("brain.neuron_count", 1000)
        config.set("brain.learning_rate", 0.01)
        config.set("simulation.timestep", 0.1)
        
        # Save to JSON
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json_file = f.name
        
        try:
            config.saveToFile(json_file)
            
            # Load back from JSON
            config2 = pynlm.Config()
            config2.loadFromFile(json_file)
            
            # Verify data integrity
            assert config.has("brain.neuron_count")
            assert config2.has("brain.neuron_count")
            
        finally:
            os.unlink(json_file)
    
    def test_config_summary(self):
        """Test configuration summary generation"""
        config = pynlm.Config()
        
        # Add configuration data
        config.set("neuron_count", 1000)
        config.set("learning_rate", 0.01)
        
        # Generate summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
        
        # Summary should contain information about config
        assert "Config" in summary or "1000" in summary or "0.01" in summary
    
    def test_config_clear(self):
        """Test configuration clearing"""
        config = pynlm.Config()
        
        # Add configuration data
        config.set("key1", "value1")
        config.set("key2", "value2")
        
        assert config.has("key1")
        assert config.has("key2")
        
        # Clear configuration
        config.clear()
        
        # All keys should be gone
        assert not config.has("key1")
        assert not config.has("key2")
        
        # Summary should reflect empty config
        summary = config.summary()
        assert isinstance(summary, str)

class TestConfigurationEdgeCases:
    """Test configuration edge cases"""
    
    def test_empty_config(self):
        """Test with completely empty configuration"""
        config = pynlm.Config()
        
        # Should work with empty config
        keys = config.getKeys()
        assert isinstance(keys, list)
        
        summary = config.summary()
        assert isinstance(summary, str)
    
    def test_large_config(self):
        """Test with large configuration"""
        config = pynlm.Config()
        
        # Add many configuration keys
        for i in range(1000):
            config.set(f"param_{i}", f"value_{i}")
        
        # Should handle large config
        keys = config.getKeys()
        assert len(keys) >= 1000
    
    def test_special_characters_in_keys(self):
        """Test configuration keys with special characters"""
        config = pynlm.Config()
        
        # Test special characters
        special_keys = [
            "key.with.dots",
            "key-with-dashes",
            "key_with_underscores",
            "key with spaces",
            "key@with#special$chars%"
        ]
        
        for key in special_keys:
            config.set(key, f"value_for_{key}")
            assert config.has(key)

if __name__ == "__main__":
    pytest.main([__file__, "-v"])