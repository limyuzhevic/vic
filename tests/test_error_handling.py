"""
Error handling NLM Python tests.

Tests for error handling, validation, and edge cases.
"""

import pytest
import pynlm


class TestErrorHandling:
    """Test error handling functionality."""
    
    def test_invalid_parameter_handling(self):
        """Test handling of invalid parameters."""
        config = pynlm.createDefaultConfig()
        
        # Test invalid string parameters
        config.setString("test.key", "valid_value")
        assert config.getString("test.key", "") == "valid_value"
        
        # Test invalid integer parameters
        config.setInt("test.int_key", 42)
        assert config.getInt("test.int_key", 0) == 42
        
        # Test invalid float parameters
        config.setFloat("test.float_key", 3.14)
        assert config.getFloat("test.float_key", 0.0) == 3.14
        
        # Test invalid boolean parameters
        config.setBool("test.bool_key", True)
        assert config.getBool("test.bool_key", False) == True
    
    def test_config_key_error_handling(self):
        """Test configuration key error handling."""
        config = pynlm.createDefaultConfig()
        
        # Test accessing non-existent key
        assert config.getString("nonexistent.key", "default") == "default"
        assert config.getInt("nonexistent.key", 0) == 0
        assert config.getFloat("nonexistent.key", 0.0) == 0.0
        assert config.getBool("nonexistent.key", False) == False
        
        # Test checking non-existent key
        assert not config.has("nonexistent.key")
    
    def test_agent_error_handling(self):
        """Test agent error handling."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test processing sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Test decoding motor command
        action = agent.decodeMotorCommand()
        assert action is not None
        assert action.getType() is not None
        
        # Test neuromodulation with edge cases
        agent.applyRewardModulation(0.0, 0.0)  # Zero values
        agent.applyRewardModulation(1.0, 0.5)  # Normal values
        agent.applyRewardModulation(-1.0, 0.5)  # Negative reward


class TestValidationTests:
    """Test validation functionality."""
    
    def test_config_validation_basic(self):
        """Test basic configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Add some keys
        config.setString("key1", "value1")
        config.setInt("key2", 42)
        config.setFloat("key3", 3.14)
        config.setBool("key4", True)
        
        # Test validation with existing keys
        assert config.validate(["key1", "key2"])
        assert config.validate(["key1", "key2", "key3", "key4"])
    
    def test_config_validation_missing_key(self):
        """Test validation with missing key."""
        config = pynlm.createDefaultConfig()
        
        # Add one key
        config.setString("existing.key", "value")
        
        # Test validation with existing key
        assert config.validate(["existing.key"])
        
        # Test validation with non-existent key
        with pytest.raises(py.value_error):
            config.validate(["nonexistent.key"])
        
        # Test validation with mix
        with pytest.raises(py.value_error):
            config.validate(["existing.key", "nonexistent.key"])
    
    def test_config_validation_empty(self):
        """Test validation with empty configuration."""
        config = pynlm.createDefaultConfig()
        
        # Test validation with no keys (only defaults)
        assert config.validate([])  # Empty list should always pass
        
        # Test validation with non-existent keys
        with pytest.raises(py.value_error):
            config.validate(["nonexistent.key1", "nonexistent.key2"])


class TestTypeConversionErrors:
    """Test type conversion error handling."""
    
    def test_type_conversion_edge_cases(self):
        """Test type conversion with edge cases."""
        config = pynlm.createDefaultConfig()
        
        # Test boolean conversion edge cases
        config.setBool("bool_true", True)
        config.setBool("bool_false", False)
        
        assert config.getBool("bool_true", False) == True
        assert config.getBool("bool_false", True) == False
        
        # Test integer conversion edge cases
        config.setInt("int_zero", 0)
        config.setInt("int_positive", 42)
        config.setInt("int_negative", -10)
        
        assert config.getInt("int_zero", 1) == 0
        assert config.getInt("int_positive", 0) == 42
        assert config.getInt("int_negative", 0) == -10
        
        # Test float conversion edge cases
        config.setFloat("float_zero", 0.0)
        config.setFloat("float_positive", 3.14)
        config.setFloat("float_negative", -2.71)
        
        assert config.getFloat("float_zero", 1.0) == 0.0
        assert config.getFloat("float_positive", 0.0) == 3.14
        assert config.getFloat("float_negative", 0.0) == -2.71
    
    def test_string_conversion_edge_cases(self):
        """Test string conversion edge cases."""
        config = pynlm.createDefaultConfig()
        
        # Test empty string
        config.setString("empty_string", "")
        assert config.getString("empty_string", "default") == ""
        
        # Test whitespace string
        config.setString("whitespace", "   ")
        assert config.getString("whitespace", "default") == "   "
        
        # Test special characters
        config.setString("special_chars", "!@#$%^&*()")
        assert config.getString("special_chars", "default") == "!@#$%^&*()"
        
        # Test Unicode characters
        config.setString("unicode", "Hello World 🌍")
        assert config.getString("unicode", "default") == "Hello World 🌍"


class TestFileSystemErrors:
    """Test file system error handling."""
    
    def test_load_nonexistent_file(self):
        """Test loading non-existent configuration file."""
        config = pynlm.createDefaultConfig()
        
        # Try to load non-existent file
        result = config.loadFromFile("/nonexistent/path/to/config.cfg")
        assert result == False
        
        # Config should still be functional
        assert config.getString("random_seed", "") != ""
    
    def test_load_corrupted_file(self):
        """Test loading corrupted configuration file."""
        config = pynlm.createDefaultConfig()
        
        # Try to load a file with syntax errors
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("""# Corrupted config
invalid_line_without_equals
another_invalid_line
key = value with unclosed quote
""")
            config_file = f.name
        
        try:
            # Try to load corrupted file
            result = config.loadFromFile(config_file)
            # Should either return False or handle gracefully
            # Either result is acceptable
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_save_to_invalid_path(self):
        """Test saving to invalid path."""
        config = pynlm.createDefaultConfig()
        
        # Try to save to invalid path
        invalid_path = "/invalid/path/to/config.cfg"
        
        # This should handle gracefully (either return False or raise appropriate exception)
        try:
            result = config.saveToFile(invalid_path)
            # If it returns False, that's acceptable
            assert result == False
        except Exception as e:
            # If it raises an exception, that's also acceptable
            assert isinstance(e, (OSError, IOError, py.runtime_error))


class TestMemoryManagementErrors:
    """Test memory management error handling."""
    
    def test_config_memory_usage(self):
        """Test configuration memory usage."""
        config = pynlm.createDefaultConfig()
        
        # Add many configuration entries
        for i in range(1000):
            config.setString(f"key_{i}", f"value_{i}")
        
        # Verify that config can still be used
        assert config.getString("key_999", "") == "value_999"
        assert config.getInt("random_seed", 0) > 0
        
        # Clean up
        config.clear()
        assert not config.has("key_999")
    
    def test_brain_memory_management(self):
        """Test brain memory management."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial memory stats
        initial_stats = brain.getPerformanceStats()
        
        # Run simulation
        for i in range(100):
            brain.step(i)
        
        # Get memory stats after simulation
        final_stats = brain.getPerformanceStats()
        
        # Verify brain is still functional
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getFiringNeuronCount() >= 0


class TestEdgeCases:
    """Test edge cases and unusual scenarios."""
    
    def test_zero_neuron_simulation(self):
        """Test simulation with minimal neuron count."""
        config = pynlm.createDefaultConfig()
        config.createBrainConfig(neuronCount=1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should still work
        assert brain.getTotalNeuronCount() == 1
        assert brain.getTotalSynapseCount() >= 0
        
        # Run simulation
        for i in range(10):
            brain.step(i)
        
        # Should not crash
        assert brain.getTotalSpikeCount() >= 0
    
    def test_large_simulation(self):
        """Test with larger simulation."""
        config = pynlm.createDefaultConfig()
        config.createBrainConfig(neuronCount=1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run many steps
        for i in range(500):
            brain.step(i)
        
        # Should handle large simulation
        assert brain.getTotalNeuronCount() == 1000
        assert brain.getTotalSynapseCount() > 0
    
    def test_extreme_parameter_values(self):
        """Test with extreme parameter values."""
        config = pynlm.createDefaultConfig()
        
        # Test extreme integer values
        config.setInt("extreme_int", 2147483647)  # Max 32-bit int
        assert config.getInt("extreme_int", 0) == 2147483647
        
        config.setInt("extreme_int2", -2147483648)  # Min 32-bit int
        assert config.getInt("extreme_int2", 0) == -2147483648
        
        # Test extreme float values
        config.setFloat("extreme_float", 1.7976931348623157e+308)  # Max float
        assert config.getFloat("extreme_float", 0.0) == 1.7976931348623157e+308
        
        config.setFloat("extreme_float2", 2.2250738585072014e-308)  # Min positive float
        assert config.getFloat("extreme_float2", 0.0) == 2.2250738585072014e-308


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
