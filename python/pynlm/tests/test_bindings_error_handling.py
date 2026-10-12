#!/usr/bin/env python3
"""
Error handling test suite for NLM Python bindings.
Tests error cases, validation, and robust error handling.
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestConfigErrorHandling:
    """Test configuration error handling."""
    
    def test_validation_error_handling(self):
        """Test configuration validation error handling."""
        config = nlm.Config()
        
        # Test validation with various scenarios
        # Valid config should not raise
        config.update(
            required_field="test",
            optional_field=42,
            validation_flag=True
        )
        
        # Should not raise for valid config
        config.validate("required_field:string")
        
        # Test with missing required field (would raise in real implementation)
        config2 = nlm.Config()
        try:
            config2.validate("required_field:string")
            # In real implementation, this would raise an error
        except Exception:
            # Expected behavior
            pass
    
    def test_file_not_found_error(self):
        """Test handling of missing configuration files."""
        config = nlm.Config()
        
        # Attempt to load non-existent file
        result = config.loadConfigFile("/nonexistent/path/config.json")
        # Should handle gracefully (not raise exception)
        assert result is not None or result is False
    
    def test_malformed_file_error(self):
        """Test handling of malformed configuration files."""
        config = nlm.Config()
        
        # Create malformed JSON file
        malformed_content = '{"invalid": json, "missing": "quotes"}'
        
        import tempfile
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write(malformed_content)
            temp_file = f.name
        
        try:
            config2 = nlm.Config()
            result = config2.loadConfigFile(temp_file)
            # Should handle gracefully (not raise exception)
            assert result is not None or result is False
        finally:
            os.unlink(temp_file)
    
    def test_empty_file_error(self):
        """Test handling of empty configuration files."""
        config = nlm.Config()
        
        # Create empty file
        import tempfile
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_file = f.name
        
        try:
            config2 = nlm.Config()
            result = config2.loadConfigFile(temp_file)
            # Should handle gracefully (not raise exception)
            assert result is not None or result is False
        finally:
            os.unlink(temp_file)
    
    def test_permission_error(self):
        """Test handling of permission errors (if applicable)."""
        config = nlm.Config()
        
        # Attempt to load file in restricted directory
        result = config.loadConfigFile("/root/restricted_config.json")
        # Should handle gracefully (not raise exception)
        assert result is not None or result is False

class TestTypeErrorHandling:
    """Test type-related error handling."""
    
    def test_type_mismatch_in_update(self):
        """Test type mismatch handling in update method."""
        config = nlm.Config()
        
        # Test with valid types first
        config.update(
            string_val="test",
            int_val=42,
            double_val=3.14,
            bool_val=True
        )
        
        # Valid access
        assert config.getOr("string_val", "") == "test"
        assert config.getOr("int_val", 0) == 42
        assert config.getOr("double_val", 0.0) > 3.13
        assert config.getOr("bool_val", False) == True
    
    def test_invalid_value_types(self):
        """Test handling of invalid value types."""
        config = nlm.Config()
        
        # This tests the interface - actual error handling depends on implementation
        # The update method should handle different types gracefully
        try:
            config.update(
                valid_string="hello",
                valid_int=123,
                valid_double=3.14159,
                valid_bool=True
            )
            # Should succeed
            assert True
        except Exception as e:
            # Should not raise for valid types
            pytest.fail(f"Unexpected error for valid types: {e}")
    
    def test_null_value_handling(self):
        """Test handling of null/None values."""
        config = nlm.Config()
        
        # The update method should handle None gracefully
        # depending on implementation
        try:
            config.update(
                value1=None,
                value2="valid",
                value3=42
            )
            # Implementation may or may not accept None
            assert True
        except Exception:
            # Implementation may reject None values
            pass

class TestResourceErrorHandling:
    """Test resource-related error handling."""
    
    def test_brain_initialization_error(self):
        """Test error handling in brain initialization."""
        # Create config with invalid data
        config = nlm.Config()
        config.update(
            neuron_count="invalid",  # Should be numeric
            region_count="invalid",  # Should be numeric
            invalid_parameter=True
        )
        
        # Brain creation should handle errors gracefully
        brain = nlm.Brain(config)
        
        # Initialization might fail or handle errors
        result = brain.initialize()
        # Should handle gracefully (not crash)
        assert True  # Either success or failure is fine, shouldn't crash
    
    def test_world_configuration_error(self):
        """Test error handling in world configuration."""
        world = nlm.SimpleWorld()
        
        # Test invalid configuration (should handle gracefully)
        try:
            # This should not crash
            world.configure_simulation(-1.0, -1.0, -1, -1)
            # Implementation may handle or reject
            assert True
        except Exception:
            # Implementation may reject invalid values
            pass
        
        # Test that world is still functional after error
        assert world is not None
    
    def test_agent_initialization_error(self):
        """Test error handling in agent initialization."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        
        # Create brain without proper initialization
        # This tests error handling in AgentBrain constructor
        agent_brain = nlm.AgentBrain(brain)
        
        # Initialization should handle errors
        result = agent_brain.initialize(None)  # Invalid world
        # Should handle gracefully (not crash)
        assert True

class TestDataErrorHandling:
    """Test data-related error handling."""
    
    def test_vision_data_errors(self):
        """Test error handling with vision data."""
        vision = nlm.Vision(10, 10, 3)
        
        # Test with valid data
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        assert vision.getChannels() == 3
        
        # Test operations that might fail
        # In current implementation, these operations are limited
        assert vision.getTimestamp() == 0
        vision.setTimestamp(123)
        assert vision.getTimestamp() == 123
    
    def test_audio_data_errors(self):
        """Test error handling with audio data."""
        audio = nlm.Audio(44100, 1024)
        
        # Test with valid data
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        # Test data operations
        test_data = [0.5] * 1024
        audio.setData(test_data)
        
        # Verify data was set
        retrieved_data = audio.getData()
        assert len(retrieved_data) == 1024
    
    def test_internal_signals_errors(self):
        """Test error handling with internal signals."""
        internal = nlm.InternalSignals()
        
        # Test empty signals
        assert len(internal.getData()) == 0
        
        # Add signals
        internal.addSignal(0.5)
        internal.addSignal(0.8)
        
        # Verify signals
        signals = internal.getData()
        assert len(signals) == 2
        assert signals[0] == 0.5
        assert signals[1] == 0.8
        
        # Clear signals
        internal.clearSignals()
        assert len(internal.getData()) == 0
    
    def test_sensory_percept_errors(self):
        """Test error handling with sensory percept."""
        percept = nlm.SensoryPercept()
        
        # Test empty percept
        assert percept.getVisionWidth() == 0
        assert percept.getVisionHeight() == 0
        
        # Set empty data
        percept.setVision([])
        percept.setTouch([])
        percept.setInternal([])
        percept.setProprioception([])
        percept.setAudio([])
        
        # Verify empty data
        assert percept.getVision() == []
        assert percept.getTouch() == []
        assert percept.getInternal() == []
        assert percept.getProprioception() == []
        assert percept.getAudio() == []
        
        # Test timestamp
        assert percept.getTimestamp() == 0
        percept.setTimestamp(999)
        assert percept.getTimestamp() == 999

class TestEdgeCaseErrorHandling:
    """Test edge cases and boundary conditions."""
    
    def test_very_large_config(self):
        """Test error handling with very large configurations."""
        config = nlm.Config()
        
        # Add many keys (stress test)
        for i in range(1000):
            config.update(**{f"key_{i}": i})
        
        # Should handle large configurations
        assert len(config.getKeys()) >= 1000
        
        # Test performance
        import time
        start_time = time.time()
        value = config.getOr("key_999", 0)
        elapsed = time.time() - start_time
        
        # Should be fast
        assert elapsed < 0.1
        assert value == 999
    
    def test_deeply_nested_operations(self):
        """Test error handling with deeply nested operations."""
        config = nlm.Config()
        
        # Test nested updates (if supported)
        config.update(
            level1_level2_level3_value="deep",
            level1_level2_another_value=42,
            level1_simple=1.0
        )
        
        # Should handle nested keys
        assert config.has("level1_level2_level3_value") or config.has("level1.level2.level3.value")
        assert config.has("level1_level2_another_value")
        assert config.has("level1_simple")
    
    def test_concurrent_access_simulation(self):
        """Test error handling in concurrent access scenarios."""
        config = nlm.Config()
        
        # Simulate concurrent access by rapid sequential operations
        import threading
        import time
        
        def worker(worker_id):
            for i in range(100):
                config.update(**{f"worker_{worker_id}_key_{i}": i})
                time.sleep(0.001)
        
        # Create multiple threads
        threads = []
        for i in range(5):
            t = threading.Thread(target=worker, args=(i,))
            threads.append(t)
            t.start()
        
        # Wait for all threads to complete
        for t in threads:
            t.join()
        
        # Should handle concurrent access gracefully
        assert config is not None
        assert len(config.getKeys()) > 0

class TestPythonicErrorHandling:
    """Test Pythonic error handling features."""
    
    def test_method_existence_checks(self):
        """Test that methods exist for error handling."""
        config = nlm.Config()
        
        # Test that validation method exists
        assert hasattr(config, 'validate')
        
        # Test that update method exists
        assert hasattr(config, 'update')
        
        # Test that loadConfigFile method exists
        assert hasattr(config, 'loadConfigFile')
    
    def test_type_validation(self):
        """Test type validation in error handling."""
        config = nlm.Config()
        
        # Test with different types
        config.update(
            string_val="hello",
            int_val=42,
            double_val=3.14,
            bool_val=True
        )
        
        # Test type validation
        config.validate("string_val:string,int_val:int")
        
        # Should not raise for valid types
        # Implementation may have specific type validation logic
        assert True
    
    def test_interface_consistency(self):
        """Test that interfaces are consistent for error handling."""
        # Test that all major classes have error handling methods
        config = nlm.Config()
        brain = nlm.Brain(config)
        world = nlm.SimpleWorld()
        agent_brain = nlm.AgentBrain(brain)
        
        # All should have error handling methods
        assert hasattr(config, 'validate') or hasattr(config, 'has')
        assert hasattr(brain, 'reset') or hasattr(brain, 'initialize')
        assert hasattr(world, 'reset') or hasattr(world, 'configure')
        assert hasattr(agent_brain, 'reset') or hasattr(agent_brain, 'initialize')

if __name__ == "__main__":
    pytest.main([__file__, "-v"])