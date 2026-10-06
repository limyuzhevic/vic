"""
Enhanced Python Bindings Test Suite
Test the new Pythonic features of pynlm enhanced bindings.
"""

import pytest
import pynlm
import tempfile
import os


class TestEnhancedBindings:
    """Test the enhanced Pythonic features of pynlm bindings."""
    
    def test_python_properties(self):
        """Test Python properties instead of get/set methods."""
        config = pynlm.create_default_config()
        
        # Set values using property-like access
        config.set_float("learning_rate", 0.01)
        config.set_int("neuron_count", 1000)
        config.set_string("activation", "relu")
        
        # Get values using property-like access
        assert config.get_float("learning_rate") == 0.01
        assert config.get_int("neuron_count") == 1000
        assert config.get_string("activation") == "relu"
        
        # Test dict-like access
        config["learning_rate"] = 0.05
        assert config["learning_rate"] == "0.05"  # String conversion
        
        # Test 'in' operator
        assert "learning_rate" in config
        assert "nonexistent" not in config
    
    def test_context_manager_checkpoint(self):
        """Test checkpoint context manager."""
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        
        # Create a simple checkpoint
        with tempfile.NamedTemporaryFile(delete=False, suffix='.sav') as f:
            checkpoint_file = f.name
        
        try:
            brain.initialize()
            # Initialize with some basic setup
            brain.add_region("test_region")
            
            # Use checkpoint context manager
            with brain.checkpoint(checkpoint_file):
                brain.add_region("region_1")
                brain.add_region("region_2")
                # Model should be automatically saved when exiting context
            
            # Load the checkpoint
            brain.reset()
            brain.initialize()
            brain.load(checkpoint_file)
            
            # Verify data was loaded
            assert brain.region_count > 0
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
    
    def test_type_conversions(self):
        """Test type conversions from Python to C++."""
        # Test helper function for converting Python lists to vectors
        py_list = [1.0, 2.0, 3.0, 4.0, 5.0]
        vector = pynlm.to_vector(py_list)
        assert len(vector) == 5
        assert vector[0] == 1.0
        assert vector[4] == 5.0
        
        # Test helper function for converting Python dicts to maps
        py_dict = {"weight": 0.5, "bias": 0.1, "threshold": 0.8}
        map_result = pynlm.to_map(py_dict)
        assert len(map_result) == 3
        assert "weight" in map_result
        assert map_result["weight"] == 0.5
        
        # Test dict value getter
        value = pynlm.get_dict_value(py_dict, "weight", 0.0)
        assert value == 0.5
        
        default_value = pynlm.get_dict_value(py_dict, "nonexistent", 1.0)
        assert default_value == 1.0
    
    def test_pythonic_api(self):
        """Test Pythonic API (__len__, __getitem__, iteration)."""
        config = pynlm.create_default_config()
        
        # Test __len__ support
        config.set("a", 1)
        config.set("b", 2)
        config.set("c", 3)
        assert len(config) == 3  # Should get keys count
        
        # Test iteration
        keys = []
        for key in config:
            keys.append(key)
        assert len(keys) == 3
        assert "a" in keys
        assert "b" in keys
        assert "c" in keys
        
        # Test in operator
        assert "a" in config
        assert "d" not in config
    
    def test_error_handling(self):
        """Test error handling and Python exception translation."""
        config = pynlm.create_default_config()
        
        # Test validation
        error_message = pynlm.validate_config(config)
        assert "Configuration is valid" in error_message
        
        # Add invalid configuration
        config.set("brain.neuron_count", -1)
        config.set("brain.synapse_density", 1.5)
        
        # Test error reporting
        errors = pynlm.get_config_errors(config)
        assert len(errors) > 0
        assert "brain.neuron_count must be positive" in errors[0]
        
        # Test that ValueError is raised for invalid config
        try:
            pynlm.validate_config(config)
            assert False, "Should have raised ValueError"
        except pynlm.ValueError as e:
            assert "Configuration validation failed" in str(e)
    
    def test_convenience_functions(self):
        """Test convenience functions for common operations."""
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Add some regions
        region1 = brain.addRegion("region1")
        region2 = brain.addRegion("region2")
        
        # Test region properties
        assert brain.region_count > 0
        assert brain.total_neuron_count > 0
        assert brain.total_synapse_count > 0
        
        # Test len() support
        assert len(brain) > 0
        
        # Test smart brain creation
        smart_brain = pynlm.create_smart_brain(config)
        assert smart_brain is not None
        
        with smart_brain:
            # Should have initialized
            pass
        
        # Test dictionary-like state access
        state = smart_brain.get_state_dict()
        assert "neuron_count" in state
        assert "synapse_count" in state
    
    def test_world_object_factory(self):
        """Test world object factory function."""
        # Create world objects using factory function
        resource = pynlm.create_world_object(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
        assert resource is not None
        assert resource.x == 5.0
        assert resource.y == 5.0
        assert resource.type == pynlm.WorldObjectType.Resource
        assert resource.value == 1.0
        
        hazard = pynlm.create_world_object(10.0, 10.0, pynlm.WorldObjectType.Hazard, 0.5, 1.0)
        assert hazard.radius == 1.0
    
    def test_sensory_input_properties(self):
        """Test enhanced sensory input with property access."""
        vision = pynlm.Vision(640, 480, 3)
        assert vision.shape == (640, 480, 3)  # Tuple property
        
        audio = pynlm.Audio(44100, 1024)
        assert audio.duration > 0.0  # Calculated property
    
    def test_agent_body_properties(self):
        """Test agent body with enhanced property access."""
        body = pynlm.AgentBody()
        body.x = 10.0
        body.y = 20.0
        body.orientation = 0.5
        body.velocityX = 5.0
        body.velocityY = 3.0
        
        assert body.speed() > 0.0  # Convenience method
        assert not body.is_idle()  # Convenience method
    
    def test_backward_compatibility(self):
        """Test that backward compatibility is maintained."""
        # Test that old-style method calls still work
        config = pynlm.create_default_config()
        config.set("old_style", "value")
        assert config.has("old_style")
        assert config.getKeys()
        assert config.summary()
        
        # Test that old-style factory functions still work
        config2 = pynlm.create_default_config()
        brain = pynlm.create_brain(config2)
        assert brain is not None
        
        # Test that old-style region access still works
        region = brain.addRegion("test")
        retrieved_region = brain.getRegion(region)
        assert retrieved_region is not None
    
    def test_advanced_error_handling(self):
        """Test advanced error handling features."""
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        
        # Test safe brain operation
        def safe_operation(brain_ptr):
            brain_ptr.addRegion("test")
        
        result = pynlm.safe_brain_operation(brain, safe_operation)
        assert "successfully" in result.lower()
        
        # Test error reporting with context
        try:
            pynlm.report_error("TEST_ERROR", "Test error message", "Context info")
        except pynlm.RuntimeError as e:
            assert "TEST_ERROR" in str(e)
            assert "Context info" in str(e)


class TestIntegration:
    """Integration tests for complete workflows using new features."""
    
    def test_complete_workflow(self):
        """Test a complete workflow using enhanced features."""
        # Create configuration with property access
        config = pynlm.create_default_config()
        config.set_float("learning_rate", 0.01)
        config.set_int("brain.neuron_count", 500)
        config.set_string("activation_function", "sigmoid")
        
        # Create brain and initialize
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Add regions using property access
        region_ids = brain.region_ids
        assert brain.region_count > 0
        
        # Use checkpoint with context manager
        with tempfile.NamedTemporaryFile(delete=False, suffix='.sav') as f:
            checkpoint_file = f.name
        
        try:
            with brain.checkpoint(checkpoint_file):
                # Run some simulation steps
                for i in range(10):
                    brain.step(i)
                
                # Add more regions inside checkpoint
                brain.addRegion("checkpoint_region")
            
            # Verify checkpoint was saved and can be loaded
            brain.reset()
            brain.initialize()
            brain.load(checkpoint_file)
            assert brain.region_count > 0
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
        
        # Use agent brain with enhanced features
        world = pynlm.create_simple_world()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Verify agent was initialized
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
        # Test neuromodulation
        agent.applyRewardModulation(1.0, 0.5)
        assert agent.getNeuromodulationLevel() is not None
    
    def test_smart_brain_features(self):
        """Test smart brain with enhanced Python features."""
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", 100)
        
        smart_brain = pynlm.create_smart_brain(config)
        assert smart_brain is not None
        
        # Test property-like access
        assert smart_brain.get_neuron_count() == 100
        assert smart_brain.get_synapse_count() > 0
        assert smart_brain.get_firing_rate() >= 0.0
        
        # Test simulation
        result = smart_brain.run_simulation(5)
        assert "5 simulation steps completed" in result
        
        # Test state dictionary
        state = smart_brain.get_state_dict()
        assert isinstance(state, dict)
        assert "neuron_count" in state
        assert "synapse_count" in state
        
        # Test enhanced string representation
        brain_str = smart_brain.to_string()
        assert "SmartBrain:" in brain_str
        assert "Neuron count:" in brain_str
        
        # Test context manager
        with smart_brain:
            # Should be initialized
            assert smart_brain.get_neuron_count() > 0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
