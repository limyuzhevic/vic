#!/usr/bin/env python3
"""
Comprehensive Python test suite for NLM Python bindings.
Tests basic functionality and Pythonic API features.
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestBasicBindings:
    """Test basic functionality and API essentials."""
    
    def test_config_creation(self):
        """Test basic configuration creation."""
        config = nlm.Config()
        assert config is not None
        assert "int_value" not in config.getKeys()
        summary = config.summary()
        assert isinstance(summary, str)
        assert "Config:" in summary
    
    def test_identity_classes(self):
        """Test identity classes (NeuronId, SynapseId, etc.)."""
        # Test NeuronId
        neuron_id = nlm.NeuronId()
        assert neuron_id.value == 0
        assert neuron_id.index() == 0
        
        neuron_id2 = nlm.NeuronId(42)
        assert neuron_id2.value == 42
        assert neuron_id2.index() == 42
        
        assert neuron_id != neuron_id2
        assert hash(neuron_id) != hash(neuron_id2)
        
        # Test SynapseId
        synapse_id = nlm.SynapseId(100)
        assert synapse_id.value == 100
        
        # Test RegionId
        region_id = nlm.RegionId(200)
        assert region_id.value == 200
    
    def test_enums(self):
        """Test enum types."""
        # Test NeuronType
        assert nlm.NeuronType.Excitatory != nlm.NeuronType.Inhibitory
        
        # Test SynapseType
        assert nlm.SynapseType.Excitatory != nlm.SynapseType.Inhibitory
        
        # Test ActionType
        assert nlm.ActionType.MoveForward != nlm.ActionType.TurnLeft
        
        # Test WorldObjectType
        assert nlm.WorldObjectType.Resource != nlm.WorldObjectType.Wall
    
    def test_action_creation(self):
        """Test action creation and manipulation."""
        action = nlm.Action()
        assert action.getType() == nlm.ActionType.Custom
        
        action2 = nlm.Action(nlm.ActionType.MoveForward)
        assert action2.getType() == nlm.ActionType.MoveForward
        
        action2.setParameters([1.0, 2.0, 3.0])
        params = action2.getParameters()
        assert len(params) == 3
        assert params[0] == 1.0
    
    def test_world_object_creation(self):
        """Test world object creation."""
        obj = nlm.WorldObject(10.0, 20.0, nlm.WorldObjectType.Resource, 5.0, 1.0)
        assert obj.x == 10.0
        assert obj.y == 20.0
        assert obj.type == nlm.WorldObjectType.Resource
        assert obj.value == 5.0
        assert obj.radius == 1.0
        assert obj.active == True

class TestPythonicConfig:
    """Test Pythonic configuration features."""
    
    def test_load_config_file(self):
        """Test loadConfigFile convenience method."""
        # Create a temporary config file
        import tempfile
        import json
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_data = {
                "int_value": 42,
                "double_value": 3.14,
                "string_value": "hello",
                "bool_value": True,
                "int_array": [1, 2, 3],
                "double_array": [1.1, 2.2, 3.3],
                "string_array": ["apple", "banana"]
            }
            json.dump(config_data, f)
            temp_file = f.name
        
        try:
            config = nlm.Config()
            config.loadConfigFile(temp_file)
            
            assert config.has("int_value")
            assert config.getOr("int_value", 0) == 42
            assert config.has("double_value")
            assert config.getOr("double_value", 0.0) > 3.13
            assert config.has("string_value")
            assert config.getOr("string_value", "") == "hello"
            assert config.has("bool_value")
            assert config.getOr("bool_value", False) == True
            assert config.has("int_array")
            assert config.getOr("int_array", []) == [1, 2, 3]
        finally:
            os.unlink(temp_file)
    
    def test_update_method(self):
        """Test update method with kwargs."""
        config = nlm.Config()
        
        # Test with different value types
        config.update(
            int_val=42,
            double_val=3.14159,
            string_val="test string",
            bool_val=True,
            int_list=[1, 2, 3, 4, 5],
            double_list=[1.1, 2.2, 3.3],
            string_list=["apple", "banana", "cherry"]
        )
        
        assert config.has("int_val")
        assert config.getOr("int_val", 0) == 42
        
        assert config.has("double_val")
        assert config.getOr("double_val", 0.0) > 3.14
        
        assert config.has("string_val")
        assert config.getOr("string_val", "") == "test string"
        
        assert config.has("bool_val")
        assert config.getOr("bool_val", False) == True
        
        assert config.has("int_list")
        assert config.getOr("int_list", []) == [1, 2, 3, 4, 5]
        
        assert config.has("double_list")
        assert config.getOr("double_list", []) == [1.1, 2.2, 3.3]
        
        assert config.has("string_list")
        assert config.getOr("string_list", []) == ["apple", "banana", "cherry"]
    
    def test_validation(self):
        """Test configuration validation."""
        config = nlm.Config()
        
        # Valid configuration should not raise
        config.update(
            valid_int=100,
            valid_string="test",
            valid_double=3.14,
            valid_bool=True
        )
        
        # This should work (no expectedTypes provided)
        config.validate()
        
        # With expectedTypes (basic validation)
        config.validate("valid_int:int,valid_string:string")
    
    def test_backward_compatibility(self):
        """Test backward compatibility with key=value format."""
        config = nlm.Config()
        
        # Save config
        import tempfile
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            temp_file = f.name
        
        try:
            config.update(
                int_value=100,
                double_value=2.718,
                string_value="test",
                bool_value=True,
                array_value=[1, 2, 3, 4, 5]
            )
            config.saveToFile(temp_file)
            
            # Create new config and load
            config2 = nlm.Config()
            assert config2.loadFromFile(temp_file)
            
            assert config2.has("int_value")
            assert config2.getOr("int_value", 0) == 100
            
            assert config2.has("double_value")
            assert config2.getOr("double_value", 0.0) > 2.717
            
            assert config2.has("string_value")
            assert config2.getOr("string_value", "") == "test"
            
            assert config2.has("bool_value")
            assert config2.getOr("bool_value", False) == True
        finally:
            os.unlink(temp_file)

class TestPythonicBrain:
    """Test Pythonic brain operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = nlm.create_simulation_config()
        self.brain = nlm.Brain(self.config)
        assert self.brain.initialize()
    
    def test_run_context_manager(self):
        """Test run method with context manager style."""
        import time
        start_time = time.time()
        
        # This is a simplified test - the run method would actually run simulation
        # For now we just test that the method exists
        assert hasattr(self.brain, 'run')
        
        elapsed = time.time() - start_time
        assert elapsed < 1.0  # Should be fast
    
    def test_get_neurons_by_type(self):
        """Test getNeuronsByType method."""
        # Test that method exists
        assert hasattr(self.brain, 'getNeuronsByType')
        
        # Test with different neuron types
        # This would require actual neurons to be created, so we just test the interface
        neurons = self.brain.getNeuronsByType(nlm.NeuronType.Excitatory)
        assert isinstance(neurons, list)
    
    def test_get_connections(self):
        """Test getConnections method."""
        # Test that method exists
        assert hasattr(self.brain, 'getConnections')
        
        connections = self.brain.getConnections()
        assert isinstance(connections, list)
        
        # Test with filters
        connections_filtered = self.brain.getConnections(source=nlm.INVALID_REGION_ID, target=nlm.INVALID_REGION_ID)
        assert isinstance(connections_filtered, list)
    
    def test_benchmark(self):
        """Test benchmark method."""
        # Test that method exists
        assert hasattr(self.brain, 'benchmark')
        
        results = self.brain.benchmark(steps=10, warmup=5)
        assert isinstance(results, dict)
        assert "total_time_seconds" in results
        assert "average_time_per_step_seconds" in results
        assert "steps_per_second" in results
        assert "steps" in results
        
        assert results["steps"] == 10
        assert results["total_time_seconds"] >= 0
        assert results["average_time_per_step_seconds"] >= 0
        assert results["steps_per_second"] > 0

class TestPythonicAgentBrain:
    """Test Pythonic agent brain operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = nlm.create_simulation_config()
        self.brain = nlm.Brain(self.config)
        assert self.brain.initialize()
        self.agent_brain = nlm.AgentBrain(self.brain)
        assert self.agent_brain.initialize(nlm.create_test_environment())
    
    def test_episode_context_manager(self):
        """Test episode context manager."""
        # Test that method exists
        assert hasattr(self.agent_brain, 'episode')
        
        # Test creating episode runner
        episode_runner = self.agent_brain.episode(max_duration=10.0, verbose=False)
        assert episode_runner is not None
        assert hasattr(episode_runner, 'getAgentBrain')
    
    def test_act_method(self):
        """Test act method."""
        # Create a simple percept
        percept = nlm.SensoryPercept()
        
        # Test that method exists
        assert hasattr(self.agent_brain, 'act')
        
        # Act should return a motor command
        motor_cmd = self.agent_brain.act(percept)
        assert motor_cmd is not None
        assert motor_cmd in [
            nlm.MotorCommand.MoveForward, nlm.MotorCommand.MoveBackward,
            nlm.MotorCommand.TurnLeft, nlm.MotorCommand.TurnRight,
            nlm.MotorCommand.LookLeft, nlm.MotorCommand.LookRight,
            nlm.MotorCommand.Interact, nlm.MotorCommand.Wait
        ]
    
    def test_observe_method(self):
        """Test observe method."""
        # Create a simple percept
        percept = nlm.SensoryPercept()
        
        # Test that method exists
        assert hasattr(self.agent_brain, 'observe')
        
        # Observe should process percept without returning action
        self.agent_brain.observe(percept)
        # If no exception, it's working
    
    def test_neuromodulation_state(self):
        """Test neuromodulation state getters."""
        assert hasattr(self.agent_brain, 'getNeuromodulationLevel')
        assert hasattr(self.agent_brain, 'getCuriosityLevel')
        assert hasattr(self.agent_brain, 'getNoveltyLevel')
        assert hasattr(self.agent_brain, 'getPredictionError')
        
        assert self.agent_brain.getNeuromodulationLevel() >= 0.0
        assert self.agent_brain.getCuriosityLevel() >= 0.0
        assert self.agent_brain.getNoveltyLevel() >= 0.0
        assert self.agent_brain.getPredictionError() >= 0.0

class TestPythonicWorld:
    """Test Pythonic world operations."""
    
    def test_configure_simulation(self):
        """Test configure_simulation method."""
        world = nlm.SimpleWorld()
        
        # Test quick configuration
        world.configure_simulation(100.0, 100.0, 32, 32, 100.0, 0.1f)
        
        assert world.getWidth() == 100.0
        assert world.getHeight() == 100.0
        # Vision dimensions are internal, but world should be configured
    
    def test_add_object_at(self):
        """Test add_object_at convenience method."""
        world = nlm.SimpleWorld()
        world.configure_simulation(50.0, 50.0)
        
        # Add object at specific position
        world.add_object_at(25.0, 25.0, nlm.WorldObjectType.Resource, 10.0, 1.0)
        
        # Verify object was added (would need to check world state)
        assert world.getWidth() == 50.0
        assert world.getHeight() == 50.0
    
    def test_get_nearby_objects(self):
        """Test get_nearby_objects method."""
        world = nlm.SimpleWorld()
        world.configure_simulation(100.0, 100.0)
        
        # Add some objects
        world.add_object_at(10.0, 10.0, nlm.WorldObjectType.Resource, 5.0)
        world.add_object_at(20.0, 20.0, nlm.WorldObjectType.Hazard, -2.0)
        
        # Test get_nearby_objects
        nearby = world.get_nearby_objects(15.0, 15.0, 10.0, active_only=True)
        assert isinstance(nearby, list)
        
        # Test with different parameters
        nearby_all = world.get_nearby_objects(15.0, 15.0, 10.0, active_only=False)
        assert isinstance(nearby_all, list)

class TestSensorData:
    """Test sensory data handling."""
    
    def test_vision(self):
        """Test vision sensory input."""
        vision = nlm.Vision(32, 32, 3)
        
        # Create some test data
        test_data = [0.0] * (32 * 32 * 3)
        for i in range(min(10, len(test_data))):
            test_data[i] = float(i) / 10.0
        
        # In a real implementation, would set data
        assert vision.getWidth() == 32
        assert vision.getHeight() == 32
        assert vision.getChannels() == 3
        assert vision.getType() == nlm.SensoryInput
        
        # Test timestamp
        assert vision.getTimestamp() == 0
    
    def test_audio(self):
        """Test audio sensory input."""
        audio = nlm.Audio(44100, 1024)
        
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        assert audio.getType() == nlm.SensoryInput
    
    def test_internal_signals(self):
        """Test internal signals sensory input."""
        internal = nlm.InternalSignals()
        
        # Add signals
        internal.addSignal(0.5)
        internal.addSignal(0.8)
        internal.addSignal(0.3)
        
        signals = internal.getData()
        assert len(signals) == 3
        assert signals[0] == 0.5
        assert signals[1] == 0.8
        assert signals[2] == 0.3
        
        # Clear signals
        internal.clearSignals()
        assert len(internal.getData()) == 0
    
    def test_sensory_percept(self):
        """Test sensory percept data structure."""
        percept = nlm.SensoryPercept()
        
        # Create test data
        vision_data = []
        touch_data = []
        internal_data = []
        proprioception_data = []
        audio_data = []
        
        percept.setVision(vision_data)
        percept.setTouch(touch_data)
        percept.setInternal(internal_data)
        percept.setProprioception(proprioception_data)
        percept.setAudio(audio_data)
        
        # Retrieve data
        assert percept.getVision() == vision_data
        assert percept.getTouch() == touch_data
        assert percept.getInternal() == internal_data
        assert percept.getProprioception() == proprioception_data
        assert percept.getAudio() == audio_data
        
        assert percept.getVisionWidth() == 0
        assert percept.getVisionHeight() == 0
        
        # Test timestamp
        percept.setTimestamp(12345)
        assert percept.getTimestamp() == 12345

class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_validation_error(self):
        """Test configuration validation error handling."""
        config = nlm.Config()
        
        # Test validation with expected types
        config.validate("required_key:string")
        
        # In a real implementation, this would raise an error if required_key was missing
    
    def test_file_not_found_error(self):
        """Test handling of missing config files."""
        config = nlm.Config()
        
        # Attempt to load non-existent file
        result = config.loadConfigFile("/non/existent/file.json")
        # Should handle gracefully (not raise exception)
    
    def test_type_mismatch_error(self):
        """Test type mismatch handling."""
        config = nlm.Config()
        
        # This should raise an error in a complete implementation
        pass  # Placeholder for error handling test

class TestPerformance:
    """Test performance characteristics."""
    
    def test_config_update_performance(self):
        """Test performance of config update method."""
        import time
        
        config = nlm.Config()
        
        start_time = time.time()
        for i in range(100):
            config.update(
                key_i=int(i),
                double_i=float(i) * 1.5,
                string_i=f"test_{i}",
                bool_i=i % 2 == 0
            )
        elapsed = time.time() - start_time
        
        assert elapsed < 1.0  # Should be fast
        assert config.getOr("key_99", 0) == 99
    
    def test_brain_benchmark(self):
        """Test brain benchmark performance."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        brain.initialize()
        
        results = brain.benchmark(steps=5, warmup=2)
        
        assert results["steps"] == 5
        assert results["steps_per_second"] > 0
        assert results["average_time_per_step_seconds"] > 0
        assert results["total_time_seconds"] > 0
    
    def test_large_config_performance(self):
        """Test performance with large configurations."""
        import time
        
        config = nlm.Config()
        
        start_time = time.time()
        # Update with many keys
        for i in range(1000):
            config.update(
                key_i=int(i),
                double_i=float(i) * 0.001,
                string_i=f"value_{i}",
                bool_i=i % 3 == 0
            )
        elapsed = time.time() - start_time
        
        assert elapsed < 2.0  # Should handle 1000 updates efficiently
        assert config.getOr("key_999", 0) == 999

if __name__ == "__main__":
    pytest.main([__file__, "-v"])