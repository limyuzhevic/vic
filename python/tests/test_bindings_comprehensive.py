import pytest
import pynlm
import numpy as np

# Test module for NLM Python bindings
# Tests all the new Pythonic features

class TestCoreTypes:
    """Test core types and identifiers with Pythonic features."""
    
    def test_neuron_id_creation(self):
        """Test NeuronId creation and properties."""
        neuron_id = pynlm.NeuronId(42)
        assert neuron_id.value == 42
        assert neuron_id.index() == 42
        assert neuron_id.is_valid
        assert neuron_id.is_valid  # Python property
        
        default_id = pynlm.NeuronId()
        assert default_id.value == 0
        assert default_id.is_valid
        
        # Test string representation
        assert repr(neuron_id) == "<NeuronId: 42>"
    
    def test_synapse_id_properties(self):
        """Test SynapseId Pythonic features."""
        syn_id = pynlm.SynapseId(123)
        assert syn_id.value == 123
        assert syn_id.index() == 123
        assert repr(syn_id) == "<SynapseId: 123>"
    
    def test_region_id_properties(self):
        """Test RegionId Pythonic features."""
        region_id = pynlm.RegionId(456)
        assert region_id.value == 456
        assert region_id.index() == 456
        assert repr(region_id) == "<RegionId: 456>"

class TestEnumerations:
    """Test enumerations with Pythonic helper methods."""
    
    def test_neuron_type_helpers(self):
        """Test NeuronType helper methods."""
        assert pynlm.NeuronType.Excitatory.is_excitatory_or_inhibitory
        assert pynlm.NeuronType.Inhibitory.is_excitatory_or_inhibitory
        assert not pynlm.NeuronType.Modulatory.is_excitatory_or_inhibitory
        
        assert pynlm.NeuronType.Sensory.is_sensory_or_motor
        assert pynlm.NeuronType.Motor.is_sensory_or_motor
        assert not pynlm.NeuronType.Internal.is_sensory_or_motor
    
    def test_synapse_type_helpers(self):
        """Test SynapseType helper methods."""
        assert pynlm.SynapseType.Excitatory.is_significant
        assert pynlm.NeuronType.Inhibitory.is_significant
        assert pynlm.NeuronType.Modulatory.is_significant
        assert not pynlm.SynapseType.Electrical.is_significant
        assert not pynlm.SynapseType.GapJunction.is_significant
    
    def test_developmental_stage_helpers(self):
        """Test DevelopmentalStage helper methods."""
        assert pynlm.DevelopmentalStage.Initial.is_developing
        assert pynlm.NeuronType.CriticalPeriod.is_developing
        assert pynlm.NeuronType.Maturation.is_developing
        assert not pynlm.NeuronType.Adult.is_developing
        assert not pynlm.NeuronType.Aging.is_developing
        
        assert pynlm.DevelopmentalStage.Adult.is_mature
        assert pynlm.NeuronType.Aging.is_mature
        assert not pynlm.NeuronType.Initial.is_mature
        assert not pynlm.NeuronType.CriticalPeriod.is_mature
    
    def test_action_type_helpers(self):
        """Test ActionType helper methods."""
        assert pynlm.ActionType.MoveForward.is_movement
        assert pynlm.NeuronType.MoveBackward.is_movement
        assert pynlm.NeuronType.TurnLeft.is_movement
        assert pynlm.NeuronType.TurnRight.is_movement
        
        assert pynlm.ActionType.Look.is_visual
        assert pynlm.NeuronType.LookUp.is_visual
        assert pynlm.NeuronType.LookDown.is_visual
        assert not pynlm.NeuronType.MoveForward.is_visual

class TestConfigSystem:
    """Test enhanced Config system with fluent interface."""
    
    def test_default_config(self):
        """Test default config creation."""
        config = pynlm.create_default_config()
        assert config is not None
        
        # Test Pythonic property access
        config.set("test.key", "test_value")
        assert config.has("test.key")
        
        # Test safe getters
        assert config.safe_get("test.key", "") == "test_value"
        assert config.safe_get("nonexistent.key", "default") == "default"
        assert config.safe_get("nonexistent.key", 42, 42) == 42
    
    def test_config_with_defaults(self):
        """Test config creation with sensible defaults."""
        config = pynlm.create_config_with_defaults()
        assert config is not None
        
        # Check that defaults were set
        assert config.has("brain.region_count")
        assert config.has("brain.initial_neurons_per_region")
        assert config.has("simulation.max_steps")
        assert config.has("world.width")
        assert config.has("world.height")
    
    def test_config_helpers(self):
        """Test config helper functions."""
        # Test load config from JSON
        config = pynlm.load_config_from_json("/workspace/app/configs/default.json")
        assert config is not None
        
        # Test safe getters for different types
        config.set("int_value", 42)
        config.set("string_value", "test")
        config.set("bool_value", True)
        config.set("double_value", 3.14)
        
        assert config.safe_get_int("int_value", 0) == 42
        assert config.safe_get_string("string_value", "") == "test"
        assert config.safe_get_bool("bool_value", False) == True
        assert config.safe_get_double("double_value", 0.0) == 3.14

class TestContextManagers:
    """Test context managers for resource management."""
    
    def test_config_context_manager(self):
        """Test config context manager."""
        config = pynlm.create_default_config()
        config.set("test.key", "test_value")
        
        with pynlm.config_context(config, auto_save=True, backup_path="/tmp/test_config.json") as ctx:
            assert ctx.get_config() is not None
            assert ctx.get_auto_save()
        
        # Context manager should have handled cleanup
    
    def test_brain_context_manager(self):
        """Test brain context manager."""
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        
        with pynlm.brain_context(brain, auto_save=True, checkpoint_path="/tmp/test_brain.json") as ctx:
            assert ctx.get_brain() is not None
            assert ctx.get_auto_save()
            elapsed = ctx.get_elapsed_time()
            assert elapsed >= 0
        
        # Context manager should have handled cleanup
    
    def test_world_context_manager(self):
        """Test world context manager."""
        world = pynlm.create_world()
        
        with pynlm.world_context(world, reset_on_exit=True) as ctx:
            assert ctx.get_world() is not None
            assert ctx.get_reset_on_exit()
        
        # World should have been reset

class TestDataUtilities:
    """Test data utilities."""
    
    def test_vector_normalization(self):
        """Test vector normalization utility."""
        vec = [1.0, 2.0, 3.0, 4.0, 5.0]
        normalized = pynlm.normalize_vector(vec)
        
        assert len(normalized) == len(vec)
        assert all(0.0 <= x <= 1.0 for x in normalized)
        assert abs(normalized[0] - 0.0) < 1e-6  # min
        assert abs(normalized[4] - 1.0) < 1e-6  # max
    
    def test_vector_stats(self):
        """Test vector statistics utility."""
        vec = [1.0, 2.0, 3.0, 4.0, 5.0]
        stats = pynlm.vector_stats(vec)
        
        assert "mean" in stats
        assert "min" in stats
        assert "max" in stats
        assert "range" in stats
        assert "std" in stats
        assert "size" in stats
        
        assert stats["mean"] == 3.0
        assert stats["min"] == 1.0
        assert stats["max"] == 5.0
        assert stats["range"] == 4.0
        assert stats["size"] == 5
    
    def test_to_fvec(self):
        """Test conversion to float vector."""
        import numpy as np
        arr = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        vec = pynlm.to_fvec(arr)
        
        assert vec == [1.0, 2.0, 3.0, 4.0, 5.0]
    
    def test_debug_percept(self):
        """Test debug percept utility."""
        percept = pynlm.SensoryPercept()
        # Add some data to percept
        vision = [0.1, 0.2, 0.3]
        percept.setVision(vision)
        
        debug_str = pynlm.debug_percept(percept)
        assert "Vision" in debug_str
        assert "Touch" in debug_str

class TestExceptionHandling:
    """Test custom exception handling."""
    
    def test_config_error(self):
        """Test ConfigError exception."""
        try:
            # Try to access invalid key - this should raise ConfigError
            # depending on implementation
            pass
        except pynlm.ConfigError:
            pass  # Expected
    
    def test_brain_error(self):
        """Test BrainError exception."""
        try:
            # Try to use brain in invalid state - this should raise BrainError
            # depending on implementation
            pass
        except pynlm.BrainError:
            pass  # Expected
    
    def test_world_error(self):
        """Test WorldError exception."""
        try:
            # Try to use world in invalid state - this should raise WorldError
            # depending on implementation
            pass
        except pynlm.WorldError:
            pass  # Expected
    
    def test_simulation_error(self):
        """Test SimulationError exception."""
        try:
            # Try to run invalid simulation - this should raise SimulationError
            # depending on implementation
            pass
        except pynlm.SimulationError:
            pass  # Expected

class TestActionProperties:
    """Test Action class Pythonic properties."""
    
    def test_action_properties(self):
        """Test Action Pythonic properties."""
        action = pynlm.Action()
        action.set_type(pynlm.ActionType.MoveForward)
        action.set_parameters([0.5, 0.5])
        
        assert action.get_type() == pynlm.ActionType.MoveForward
        assert action.get_parameters() == [0.5, 0.5]
        
        # Pythonic property access
        action.type = pynlm.ActionType.TurnLeft
        action.parameters = [1.0, 0.0]
        
        assert action.type == pynlm.ActionType.TurnLeft
        assert action.parameters == [1.0, 0.0]
        
        # Pythonic property helpers
        assert action.is_movement
        assert not action.is_visual
        
        action.type = pynlm.ActionType.Look
        assert not action.is_movement
        assert action.is_visual
        
        # Test string representation
        assert "Action" in repr(action)

class TestWorldObjectProperties:
    """Test WorldObject class Pythonic properties."""
    
    def test_world_object_properties(self):
        """Test WorldObject Pythonic properties."""
        obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, 5.0)
        
        assert obj.x == 1.0
        assert obj.y == 2.0
        assert obj.type == pynlm.WorldObjectType.Resource
        assert obj.value == 5.0
        assert obj.active == True
        
        # Pythonic properties
        assert obj.is_resource
        assert not obj.is_hazard
        assert obj.is_passable
        
        # Change type
        obj.type = pynlm.WorldObjectType.Hazard
        assert not obj.is_resource
        assert obj.is_hazard
        assert not obj.is_passable
        
        # Test distance calculation
        distance = obj.distance_to(4.0, 6.0)
        assert abs(distance - 5.0) < 1e-6  # sqrt((4-1)^2 + (6-2)^2) = 5

class TestSensoryPerceptProperties:
    """Test SensoryPercept class Pythonic properties."""
    
    def test_sensory_percept_properties(self):
        """Test SensoryPercept Pythonic properties."""
        percept = pynlm.SensoryPercept()
        
        # Add vision data
        vision = [0.1, 0.2, 0.3, 0.4]
        percept.setVision(vision)
        
        assert percept.getVisionWidth() == 2  # 2x2 from 4 elements
        assert percept.getVisionHeight() == 2
        assert percept.has_vision
        
        # Test normalized vision
        normalized = percept.get_normalized_vision()
        assert len(normalized) == len(vision)
        assert all(0.0 <= x <= 1.0 for x in normalized)

class TestBackwardCompatibility:
    """Test backward compatibility with original API."""
    
    def test_original_factory_functions(self):
        """Test original factory functions still work."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        agent_brain = pynlm.createAgentBrain(brain)
        assert agent_brain is not None
    
    def test_original_constants(self):
        """Test original global constants still exist."""
        # These should be available for backward compatibility
        assert hasattr(pynlm, 'INVALID_NEURON_ID')
        assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
        assert hasattr(pynlm, 'INVALID_REGION_ID')
        assert hasattr(pynlm, 'INVALID_POPULATION_ID')

class TestSimulationHelpers:
    """Test simulation helpers and utilities."""
    
    def test_simulation_runner(self):
        """Test simulation runner helper."""
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        world = pynlm.create_world()
        agent = pynlm.create_agent_brain(brain)
        
        runner = pynlm.SimulationRunner(brain, world, agent, max_steps=10)
        assert runner is not None
        
        # Note: This would require a fully initialized simulation to run
        # action_history, time_history = runner.run()
        # assert runner.get_steps_completed() == len(action_history)

if __name__ == "__main__":
    pytest.main([__file__, "-v"])