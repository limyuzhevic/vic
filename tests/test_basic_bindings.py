"""Basic Bindings Test

Test that all major classes can be created and basic methods work.
"""

import pytest
import pynlm
from typing import List, Dict, Any


class TestBasicBindings:
    """Test basic pynlm bindings and class creation."""
    
    def test_id_classes_creation(self):
        """Test that ID classes can be created."""
        # Test NeuronId
        neuron_id = pynlm.NeuronId()
        assert neuron_id.value == 0
        
        neuron_id_with_value = pynlm.NeuronId(42)
        assert neuron_id_with_value.value == 42
        
        # Test SynapseId
        synapse_id = pynlm.SynapseId()
        assert synapse_id.value == 0
        
        synapse_id_with_value = pynlm.SynapseId(99)
        assert synapse_id_with_value.value == 99
        
        # Test RegionId
        region_id = pynlm.RegionId()
        assert region_id.value == 0
        
        # Test PopulationId
        pop_id = pynlm.PopulationId()
        assert pop_id.value == 0
    
    def test_enums_exist(self):
        """Test that all enum types exist and have expected values."""
        # NeuronType enum
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal,
        ]
        assert len(neuron_types) == 6
        
        # SynapseType enum
        synapse_types = [
            pynlm.SynapseType.Excitatory,
            pynlm.SynapseType.Inhibitory,
            pynlm.SynapseType.Modulatory,
            pynlm.SynapseType.Electrical,
            pynlm.SynapseType.GapJunction,
        ]
        assert len(synapse_types) == 5
        
        # DevelopmentalStage enum
        stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging,
        ]
        assert len(stages) == 5
    
    def test_config_class(self):
        """Test Config class basic functionality."""
        config = pynlm.Config()
        assert config is not None
        
        # Test has method
        assert config.has("test_key") == False
        
        # Test getKeys method
        keys = config.getKeys()
        assert isinstance(keys, list)
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert "Config" in summary
        
        # Test clear
        config.clear()
        assert len(config.getKeys()) == 0
    
    def test_sensory_input_classes(self):
        """Test sensory input classes."""
        # Create Vision sensory input
        vision = pynlm.Vision()
        assert vision is not None
        
        # Test Vision methods
        assert vision.getType() == "Vision"
        assert vision.getWidth() == 0
        assert vision.getHeight() == 0
        assert vision.getChannels() == 3
        
        # Create Vision with dimensions
        vision_2 = pynlm.Vision(640, 480, 3)
        assert vision_2.getWidth() == 640
        assert vision_2.getHeight() == 480
        assert vision_2.getChannels() == 3
        
        # Create Audio sensory input
        audio = pynlm.Audio()
        assert audio is not None
        assert audio.getType() == "Audio"
        
        # Create InternalSignals sensory input
        internal = pynlm.InternalSignals()
        assert internal is not None
        assert internal.getType() == "InternalSignals"
    
    def test_action_class(self):
        """Test Action class."""
        action = pynlm.Action()
        assert action is not None
        
        # Test default constructor
        assert action.getType() == pynlm.ActionType.Wait
        assert len(action.getParameters()) == 0
        
        # Test constructor with ActionType
        move_action = pynlm.Action(pynlm.ActionType.MoveForward)
        assert move_action.getType() == pynlm.ActionType.MoveForward
        
        # Test constructor with parameters
        action_with_params = pynlm.Action(
            pynlm.ActionType.MoveForward,
            [1.0, 2.0, 0.5]
        )
        assert action_with_params.getType() == pynlm.ActionType.MoveForward
        assert action_with_params.getParameters() == [1.0, 2.0, 0.5]
        
        # Test cloning
        cloned = action_with_params.clone()
        assert cloned.getType() == action_with_params.getType()
        assert cloned.getParameters() == action_with_params.getParameters()
        assert cloned is not action_with_params  # Different object
    
    def test_factory_functions(self):
        """Test factory functions for creating main objects."""
        # Test createDefaultConfig
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert isinstance(config, pynlm.Config)
        
        # Test createBrain
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        assert brain is not None
        assert isinstance(brain, pynlm.Brain)
        
        # Test createSimpleWorld
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert isinstance(world, pynlm.SimpleWorld)
        
        # Test createAgentBrain
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        assert agent_brain is not None
        assert isinstance(agent_brain, pynlm.AgentBrain)
    
    def test_constants(self):
        """Test that module constants are accessible."""
        # Check that invalid ID constants exist
        assert hasattr(pynlm, 'INVALID_NEURON_ID')
        assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
        assert hasattr(pynlm, 'INVALID_REGION_ID')
        assert hasattr(pynlm, 'INVALID_POPULATION_ID')
        
        # Check their values
        assert pynlm.INVALID_NEURON_ID.value == 0
        assert pynlm.INVALID_SYNAPSE_ID.value == 0
        assert pynlm.INVALID_REGION_ID.value == 0
        assert pynlm.INVALID_POPULATION_ID.value == 0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
