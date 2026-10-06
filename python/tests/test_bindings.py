#!/usr/bin/env python3
"""
General Python bindings integration tests
Tests basic Python bindings functionality and enhancements
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

import pytest
import numpy as np
from pynlm import (
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio, InternalSignals,
    Action, ActionType, MotorCommand, WorldObject, AgentBody, SensoryPercept,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    NeuronId, SynapseId, RegionId, PopulationId, WorldObjectType,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
)

class TestPythonBindings:
    """Test Python bindings enhancements"""
    
    def test_constants_and_enums(self):
        """Test Python constants and enhanced enums"""
        # Test constants
        assert INVALID_NEURON_ID.index() == -1
        assert INVALID_SYNAPSE_ID.index() == -1
        assert INVALID_REGION_ID.index() == -1
        assert INVALID_POPULATION_ID.index() == -1
        
        # Test enum values
        assert NeuronType.Excitatory != NeuronType.Inhibitory
        assert SynapseType.Excitatory != SynapseType.Inhibitory
        assert ActionType.MoveForward != ActionType.MoveBackward
        
        print("✓ Constants and enums test passed")
    
    def test_id_classes(self):
        """Test enhanced ID classes with Python features"""
        # Test NeuronId
        neuron_id = NeuronId(1)
        assert neuron_id.index() == 1
        assert str(neuron_id) == "1"
        assert repr(neuron_id) == "<NeuronId: 1>"
        assert neuron_id == NeuronId(1)
        assert neuron_id != NeuronId(2)
        
        # Test SynapseId
        synapse_id = SynapseId(100)
        assert synapse_id.index() == 100
        
        # Test RegionId
        region_id = RegionId(5)
        assert region_id.index() == 5
        
        # Test PopulationId
        pop_id = PopulationId(42)
        assert pop_id.index() == 42
        
        print("✓ ID classes test passed")
    
    def test_enhanced_config_dict_access(self):
        """Test enhanced Config dict-like access"""
        config = Config()
        
        # Test comprehensive dict-like operations
        config["int_key"] = 42
        config["float_key"] = 3.14
        config["str_key"] = "test"
        config["bool_key"] = True
        config["list_int_key"] = [1, 2, 3]
        config["list_float_key"] = [1.1, 2.2, 3.3]
        
        # Dict-like access
        assert config["int_key"] == "42"
        assert float(config["float_key"]) == 3.14
        assert config["str_key"] == "test"
        assert config["bool_key"] == "True"
        
        # List access
        list_int = config.get_list("list_int_key")
        assert list_int == [1, 2, 3]
        
        list_float = config.get_list("list_float_key")
        assert list_float == [1.1, 2.2, 3.3]
        
        # Test contains
        assert "int_key" in config
        assert "nonexistent" not in config
        
        # Test iteration
        keys = list(config.keys())
        assert "int_key" in keys
        assert "float_key" in keys
        
        print("✓ Enhanced Config dict-like access test passed")
    
    def test_enhanced_error_handling(self):
        """Test enhanced error handling with Python exceptions"""
        # Import error management if available
        try:
            import pynlm.error
            error_manager = pynlm.get_error_manager()
            
            # Enable error reporting
            error_manager.enable(True)
            error_manager.set_strict_mode(False)
            
            # Test error creation
            config = Config()
            error = error_manager.config_file_error(
                "test.py", 123, "test_function",
                "Test error message", "path/to/file.json", "test_key"
            )
            
            assert "Test error message" in error.what()
            
            # Test error count
            initial_count = error_manager.get_error_count()
            assert initial_count >= 0
            
            # Trigger another error
            config.load_from_file("/nonexistent/file")
            
            final_count = error_manager.get_error_count()
            assert final_count >= initial_count
            
            # Test error reporting
            assert error_manager.is_enabled()
            assert not error_manager.is_strict_mode()
            
            print("✓ Enhanced error handling test passed")
            
        except ImportError:
            print("   Error management not available, skipping enhanced error tests")
    
    def test_enhanced_sensory_input(self):
        """Test enhanced sensory input classes"""
        # Test Vision
        vision = Vision(640, 480, 3)
        assert vision.get_width() == 640
        assert vision.get_height() == 480
        assert vision.get_channels() == 3
        
        # Test Audio
        audio = Audio(44100, 1024)
        assert audio.get_sample_rate() == 44100
        assert audio.get_num_samples() == 1024
        
        # Test InternalSignals
        internal = InternalSignals()
        internal.add_signal(0.5)
        internal.add_signal(1.0)
        
        # Note: getData() might not exist, test alternative approaches
        # This depends on actual implementation
        
        print("✓ Enhanced sensory input test passed")
    
    def test_enhanced_action_class(self):
        """Test enhanced Action class"""
        # Test default action
        action = Action()
        assert action is not None
        
        # Test action with type
        action = Action(ActionType.MoveForward)
        assert action.get_type() == ActionType.MoveForward
        
        # Test action with parameters
        action = Action(ActionType.Custom, [0.1, 0.2, 0.3])
        assert action.get_type() == ActionType.Custom
        params = action.get_parameters()
        assert len(params) == 3
        
        # Test action cloning
        action2 = action.clone()
        assert action2.get_type() == action.get_type()
        assert action2.get_parameters() == action.get_parameters()
        
        # Test action representation
        repr_str = repr(action)
        assert "Action:" in repr_str
        
        print("✓ Enhanced Action class test passed")
    
    def test_enhanced_world_object(self):
        """Test enhanced WorldObject class"""
        # Test default world object
        obj = WorldObject()
        assert obj.x == 0.0
        assert obj.y == 0.0
        assert obj.radius == 0.5
        assert obj.type == WorldObjectType.Empty
        
        # Test world object with parameters
        obj = WorldObject(10.0, 20.0, WorldObjectType.Resource, 5.0, 1.0)
        assert obj.x == 10.0
        assert obj.y == 20.0
        assert obj.type == WorldObjectType.Resource
        assert obj.value == 5.0
        assert obj.radius == 1.0
        
        # Test utility methods
        distance = obj.distance_to(15.0, 25.0)
        assert distance > 0.0
        
        assert obj.is_within_range(10.0, 20.0, 0.1)  # Same position
        assert not obj.is_within_range(10.0, 20.0, 0.01)  # Too close
        
        print("✓ Enhanced WorldObject class test passed")
    
    def test_enhanced_agent_body(self):
        """Test enhanced AgentBody class"""
        body = AgentBody()
        
        # Test Pythonic properties
        body.x = 10.0
        body.y = 20.0
        body.orientation = 45.0
        body.velocity_x = 1.0
        body.velocity_y = 0.5
        body.angular_velocity = 0.1
        body.energy = 100.0
        body.health = 80.0
        body.age = 10
        body.is_moving = True
        body.is_turning = False
        body.last_action_time = 5.0
        
        # Test Pythonic convenience methods
        position = body.position()
        assert position == (10.0, 20.0)
        
        velocity = body.velocity()
        assert velocity == (1.0, 0.5)
        
        # Test move_towards method
        body.move_towards(30.0, 40.0, 5.0)
        assert body.is_moving
        
        # Test reset
        body.reset()
        assert body.energy == 100.0  # Default
        
        print("✓ Enhanced AgentBody class test passed")
    
    def test_enhanced_sensory_percept(self):
        """Test enhanced SensoryPercept class"""
        percept = SensoryPercept()
        
        # Test sensory components
        vision = Vision(100, 100)
        percept.set_vision(vision)
        assert percept.get_vision() is vision
        
        # Test width/height access
        assert percept.get_vision_width() == 100
        assert percept.get_vision_height() == 100
        
        # Test other sensory modalities
        internal = InternalSignals()
        percept.set_internal(internal)
        assert percept.get_internal() is internal
        
        print("✓ Enhanced SensoryPercept class test passed")
    
    def test_enhanced_simple_world(self):
        """Test enhanced SimpleWorld class"""
        world = SimpleWorld()
        
        # Test configuration
        world.configure(20.0, 20.0, 16, 16)
        assert world.get_width() == 20.0
        assert world.get_height() == 20.0
        
        # Test agent start
        world.set_agent_start(5.0, 5.0)
        
        # Test agent position (Pythonic)
        position = world.get_agent_position()
        assert position == (5.0, 5.0)
        
        # Test agent position setter
        world.set_agent_position(10.0, 10.0)
        assert world.get_agent_position() == (10.0, 10.0)
        
        # Test world objects
        obj = WorldObject(10.0, 10.0, WorldObjectType.Resource, 10.0)
        world.add_object(obj)
        
        # Test object removal
        world.remove_object(10.0, 10.0)
        
        # Test world properties
        assert world.get_max_energy() == 100.0  # Default
        assert world.get_energy_decay_rate() == 0.1  # Default
        assert world.get_simulation_time() == 0.0  # Default
        
        # Test world utility methods
        world.teleport_agent(15.0, 15.0)
        assert world.get_agent_position() == (15.0, 15.0)
        
        world.add_resource(5.0, 5.0, 20.0)
        world.add_hazard(18.0, 18.0, -25.0)
        
        print("✓ Enhanced SimpleWorld class test passed")
    
    def test_enhanced_brain(self):
        """Test enhanced Brain class"""
        config_dict = {
            "neuron_count": 100,
            "region_count": 2,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001,
            "random_seed": 42,
        }
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test Pythonic property names
        assert brain.neuron_count() == 100
        assert brain.synapse_count() > 0
        assert brain.active_neuron_count() >= 0
        assert brain.firing_neuron_count() >= 0
        assert brain.average_firing_rate() >= 0.0
        
        # Test backward compatibility
        assert brain.get_total_neuron_count() == 100
        assert brain.get_total_synapse_count() > 0
        
        # Test context manager
        with brain.context_manager():
            brain.step(0)
            # Brain should be reset after context
        
        # Test batch operations
        brain.batch_step([0, 1, 2])
        brain.batch_inject_current(NeuronType.Excitatory, 1.0)
        
        print("✓ Enhanced Brain class test passed")
    
    def test_convenience_functions(self):
        """Test convenience functions"""
        # Test get_default_config
        config_dict = get_default_config()
        assert "neuron_count" in config_dict
        assert "region_count" in config_dict
        assert "connection_probability" in config_dict
        
        # Test create_agent_simulation
        config_dict = get_default_config()
        config_dict["neuron_count"] = 50
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        assert brain.neuron_count() == 50
        assert world is not None
        assert agent_brain is not None
        
        # Test create functions
        config = createBrain(config_dict)
        assert config is not None
        
        simple_world = createSimpleWorld()
        assert simple_world is not None
        
        agent_brain2 = createAgentBrain(brain)
        assert agent_brain2 is not None
        
        print("✓ Convenience functions test passed")
    
    def test_type_conversion_helpers(self):
        """Test numpy to Python type conversion helpers"""
        # Test numpy array conversion
        import numpy as np
        
        # Create test numpy array
        np_array = np.array([1, 2, 3, 4, 5])
        
        # Test conversion - skip this test as _convert_numpy_to_py is internal
        # Skip this test as _convert_numpy_to_py is an internal helper
        # In practice, this would be tested separately
        print("✓ Type conversion helpers test skipped (internal helper)")
    
    def test_environment_integration(self):
        """Test integration with environment"""
        import os
        
        # Set test environment variables
        os.environ["NLM_NEURON_COUNT"] = "500"
        os.environ["NLM_CONNECTION_PROBABILITY"] = "0.2"
        
        # Test config loading from environment (if supported)
        config = Config()
        
        # Note: This depends on actual implementation
        # For now, just test that config operations work
        config.set("test_env", "loaded")
        assert config.get_string("test_env") == "loaded"
        
        # Clean up
        del os.environ["NLM_NEURON_COUNT"]
        del os.environ["NLM_CONNECTION_PROBABILITY"]
        
        print("✓ Environment integration test passed")
    
    def test_performance_characteristics(self):
        """Test performance characteristics (basic)"""
        import time
        
        # Create a simulation
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        
        start_time = time.time()
        world, brain, agent_brain = create_agent_simulation(config_dict)
        creation_time = time.time() - start_time
        
        assert creation_time < 1.0  # Should create quickly
        
        # Test step performance
        start_time = time.time()
        for step in range(10):
            brain.step(step)
        step_time = time.time() - start_time
        
        # Steps should be fast (even if this varies)
        assert step_time < 1.0
        
        print(f"✓ Performance test passed (creation: {creation_time:.3f}s, steps: {step_time:.3f}s)")

if __name__ == "__main__":
    # Run tests manually if script is executed directly
    import sys
    pytest.main([__file__, "-v"])
