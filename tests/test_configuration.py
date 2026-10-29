"""
Configuration NLM Python tests for configuration system.

Tests for configuration file loading, type conversion, validation, and presets.
"""

import pytest
import tempfile
import os
import pynlm


class TestConfigurationFileLoading:
    """Test configuration file loading functionality."""
    
    def test_load_from_config_file(self):
        """Test loading configuration from file with type conversion."""
        # Create a temporary config file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("""# Test configuration file
random_seed = 12345
simulation_timestep = 0.01
neuron_count = 500
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
dopamine_baseline = 0.5
reward_discount_factor = 0.95
environment_name = TestWorld
environment_width = 20
environment_height = 20
log_level = INFO
log_to_file = false
max_simulation_steps = 5000
visualization_enabled = false
""")
            config_file = f.name
        
        try:
            # Load configuration
            config = pynlm.createDefaultConfig()
            result = config.loadFromConfigFile(config_file)
            
            # Check success
            assert result == True
            assert config.has("random_seed")
            assert config.has("simulation_timestep")
            assert config.has("neuron_count")
            
            # Check type conversion
            assert config.getInt("random_seed", 0) == 12345
            assert config.getFloat("simulation_timestep", 0.0) == 0.01
            assert config.getInt("neuron_count", 0) == 500
            assert config.getFloat("connection_probability", 0.0) == 0.15
            assert config.getFloat("stdp_ltp_weight", 0.0) == 0.02
            assert config.getFloat("stdp_ltd_weight", 0.0) == 0.015
            assert config.getFloat("synaptogenesis_rate", 0.0) == 0.001
            assert config.getFloat("pruning_rate", 0.0) == 0.0001
            assert config.getFloat("dopamine_baseline", 0.0) == 0.5
            assert config.getFloat("reward_discount_factor", 0.0) == 0.95
            assert config.getString("environment_name", "") == "TestWorld"
            assert config.getInt("environment_width", 0) == 20
            assert config.getInt("environment_height", 0) == 20
        finally:
            # Clean up
            os.unlink(config_file)
    
    def test_load_from_simple_config_file(self):
        """Test loading configuration from simple key=value file."""
        # Create a simple config file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("""# Simple config
simple_int = 42
simple_float = 3.14159
simple_bool = true
simple_string = hello world
""")
            config_file = f.name
        
        try:
            # Load configuration
            config = pynlm.createDefaultConfig()
            result = config.loadFromFile(config_file)
            
            # Check success
            assert result == True
            assert config.has("simple_int")
            assert config.has("simple_float")
            assert config.has("simple_bool")
            assert config.has("simple_string")
            
            # Check type conversion (might be stored as string in simple format)
            assert config.getString("simple_int", "") == "42"
            assert config.getString("simple_float", "") == "3.14159"
            assert config.getString("simple_bool", "") == "true"
            assert config.getString("simple_string", "") == "hello world"
        finally:
            # Clean up
            os.unlink(config_file)


class TestConfigurationValidation:
    """Test configuration validation functionality."""
    
    def test_config_validation_success(self):
        """Test successful configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Add some keys
        config.setString("key1", "value1")
        config.setInt("key2", 42)
        config.setFloat("key3", 3.14)
        
        # Validate with existing keys
        assert config.validate(["key1", "key2"])
        assert config.validate(["key1", "key2", "key3"])
    
    def test_config_validation_failure(self):
        """Test configuration validation failure."""
        config = pynlm.createDefaultConfig()
        
        # Try to validate with non-existent key
        with pytest.raises(py.value_error, match="Missing required configuration key"):
            config.validate(["nonexistent.key"])
        
        # Try to validate with mix of existing and non-existent keys
        config.setString("existing.key", "value")
        with pytest.raises(py.value_error, match="Missing required configuration key"):
            config.validate(["existing.key", "nonexistent.key"])
    
    def test_config_has_method(self):
        """Test configuration key existence checking."""
        config = pynlm.createDefaultConfig()
        
        # Initially no custom keys
        assert not config.has("nonexistent.key")
        
        # Add a key
        config.setString("new.key", "value")
        assert config.has("new.key")
        
        # Non-existent key should still return false
        assert not config.has("still.nonexistent")


class TestConfigurationValidation:
    """Test configuration key existence and validation."""
    
    def test_config_key_existence(self):
        """Test configuration key existence checking."""
        config = pynlm.createDefaultConfig()
        
        # Initially no custom keys
        assert not config.has("nonexistent.key")
        
        # Add a key
        config.setString("new.key", "value")
        assert config.has("new.key")
        
        # Non-existent key should still return false
        assert not config.has("still.nonexistent")
    
    def test_config_key_validation(self):
        """Test configuration key validation."""
        config = pynlm.createDefaultConfig()
        
        # Add some keys
        config.setString("key1", "value1")
        config.setInt("key2", 42)
        
        # Validate existing keys
        assert config.validate(["key1", "key2"])
        
        # Try to validate with non-existent key
        with pytest.raises(py.value_error):
            config.validate(["key1", "nonexistent.key"])


class TestConfigurationMerging:
    """Test configuration merging functionality."""
    
    def test_config_copy(self):
        """Test configuration copying."""
        config1 = pynlm.createDefaultConfig()
        config1.setString("key1", "value1")
        config1.setInt("key2", 42)
        
        # Create copy
        config2 = config1.copyConfig()
        
        # Check that copy has same values
        assert config2.has("key1")
        assert config2.has("key2")
        assert config2.getString("key1", "") == "value1"
        assert config2.getInt("key2", 0) == 42
        
        # Modify original, check copy is unchanged
        config1.setString("key1", "modified_value")
        assert config2.getString("key1", "") == "value1"  # Copy should be unchanged
    
    def test_config_merge(self):
        """Test configuration merging."""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        # Modify config2
        config2.setString("extra.key", "extra_value")
        config2.setInt("extra.int", 123)
        
        # Merge config2 into config1
        config1.mergeConfig(config2)
        
        # Check that config1 has config2's values
        assert config1.has("extra.key")
        assert config1.has("extra.int")
        assert config1.getString("extra.key", "") == "extra_value"
        assert config1.getInt("extra.int", 0) == 123
        
        # Check that config1 still has its original values
        assert config1.has("random_seed")
        assert config1.has("simulation_timestep")


class TestConfigurationTemplates:
    """Test configuration template methods."""
    
    def test_brain_config_template(self):
        """Test brain configuration template."""
        config = pynlm.createDefaultConfig()
        config.createBrainConfig(neuronCount=1000, regionCount=2, connectionProb=0.1)
        
        assert config.getInt("brain.neuron_count", 0) == 1000
        assert config.getInt("brain.region_count", 0) == 2
        assert config.getFloat("brain.connection_probability", 0.0) == 0.1
        
        # Check default values are preserved
        assert config.has("random_seed")
        assert config.has("simulation_timestep")
    
    def test_agent_config_template(self):
        """Test agent configuration template."""
        config = pynlm.createDefaultConfig()
        config.createAgentConfig()
        
        assert config.getBool("agent.reward_modulation", False) == True
        assert config.getBool("agent.structural_plasticity", False) == True
        assert config.getBool("agent.development", False) == True
        assert config.getBool("agent.curiosity", False) == True
        assert config.getBool("agent.planning", False) == True
    
    def test_world_config_template(self):
        """Test world configuration template."""
        config = pynlm.createDefaultConfig()
        config.createWorldConfig(width=30, height=30, visionWidth=16, visionHeight=16)
        
        assert config.getInt("world.width", 0) == 30
        assert config.getInt("world.height", 0) == 30
        assert config.getInt("world.vision_width", 0) == 16
        assert config.getInt("world.vision_height", 0) == 16
    
    def test_complete_config_setup(self):
        """Test complete configuration setup."""
        # Create a complete configuration setup
        config = pynlm.createDefaultConfig()
        config.createBrainConfig(neuronCount=500)
        config.createAgentConfig()
        config.createWorldConfig(width=20, height=20)
        
        # Verify all templates worked
        assert config.getInt("brain.neuron_count", 0) == 500
        assert config.getBool("agent.reward_modulation", False) == True
        assert config.getInt("world.width", 0) == 20
        assert config.getInt("world.height", 0) == 20


class TestConfigurationFileIO:
    """Test configuration file input/output."""
    
    def test_save_config_to_file(self):
        """Test saving configuration to file."""
        config = pynlm.createDefaultConfig()
        
        # Add some test values
        config.setString("test.string_key", "string_value")
        config.setInt("test.int_key", 123)
        config.setFloat("test.float_key", 3.14)
        config.setBool("test.bool_key", True)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
        
        try:
            # Save configuration
            result = config.saveToFile(config_file)
            assert result == True
            
            # Verify file was created and contains data
            assert os.path.exists(config_file)
            assert os.path.getsize(config_file) > 0
            
            # Try to load it back
            config2 = pynlm.createDefaultConfig()
            result2 = config2.loadFromFile(config_file)
            assert result2 == True
            
            # Check that values are preserved
            assert config2.getString("test.string_key", "") == "string_value"
            assert config2.getInt("test.int_key", 0) == 123
            assert config2.getFloat("test.float_key", 0.0) == 3.14
            assert config2.getBool("test.bool_key", False) == True
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_save_empty_config(self):
        """Test saving empty configuration."""
        config = pynlm.createDefaultConfig()
        
        # Save empty config
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
        
        try:
            result = config.saveToFile(config_file)
            assert result == True
            
            # Verify file was created
            assert os.path.exists(config_file)
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)


class TestConfigurationUtilityFunctions:
    """Test configuration utility functions."""
    
    def test_config_summary(self):
        """Test configuration summary generation."""
        config = pynlm.createDefaultConfig()
        
        # Add some values
        config.setString("key1", "value1")
        config.setInt("key2", 42)
        
        # Get summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert "Configuration" in summary
        assert "key1" in summary
        assert "key2" in summary
        assert "2 entries" in summary
    
    def test_config_clear(self):
        """Test configuration clearing."""
        config = pynlm.createDefaultConfig()
        
        # Add a value
        config.setString("test.key", "test_value")
        assert config.has("test.key")
        
        # Clear configuration
        config.clear()
        
        # Check that key is removed
        assert not config.has("test.key")
        assert not config.has("random_seed")  # Default keys should also be cleared
    
    def test_config_get_keys(self):
        """Test getting configuration keys."""
        config = pynlm.createDefaultConfig()
        
        # Initially has default keys
        keys = config.getKeys()
        assert len(keys) > 0
        assert "random_seed" in keys
        assert "simulation_timestep" in keys
        
        # Add a custom key
        config.setString("custom.key", "value")
        
        # Get keys again
        keys = config.getKeys()
        assert "custom.key" in keys
        assert len(keys) >= 2  # At least default + custom


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
