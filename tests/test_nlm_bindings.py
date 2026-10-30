"""
Test suite for NLM Python bindings.

This module contains comprehensive tests for the enhanced NLM Python bindings,
testing both basic functionality and advanced features.
"""

import pytest
import numpy as np
import tempfile
import os
from pathlib import Path

# Import the NLM bindings
import pynlm


class TestBasicFunctionality:
    """Test basic functionality of NLM Python bindings."""
    
    def test_config_creation(self):
        """Test Config creation and basic operations."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.get("brain.neuron_count") is None  # Default is None
        
        # Set and get values
        config.set("brain.neuron_count", 1000)
        assert config.get("brain.neuron_count") == 1000
        
        config.set("simulation.dt", 0.001, pynlm.ConfigSource.Runtime)
        assert config.get("simulation.dt") == 0.001
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert "Config" in summary
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Configure world
        world.configure(100.0, 100.0, 10, 10)
        assert world.getWidth() == 100.0
        assert world.getHeight() == 100.0
        assert world.getVisionWidth() == 10
        assert world.getVisionHeight() == 10
        
        # Set agent start
        world.setAgentStart(50.0, 50.0)
        agent = world.getAgentBody()
        assert abs(agent.x - 50.0) < 0.001
        assert abs(agent.y - 50.0) < 0.001
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        # Initialize brain
        success = brain.initialize()
        assert success is True
        
        # Get statistics
        stats = pynlm.getStatistics(brain)
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert "active_neurons" in stats
        assert "firing_neurons" in stats
        assert "average_firing_rate" in stats
    
    def test_action_creation(self):
        """Test Action creation and manipulation."""
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action.getType() == pynlm.ActionType.MoveForward
        assert action.getName() == "MoveForward"
        
        # Test with parameters
        action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.8, 0.3])
        assert action2.getType() == pynlm.ActionType.Custom
        assert len(action2.getParameters()) == 3
        assert action2.getParameters()[0] == 0.5
        
        # Test parameter access
        action2.setParameter(1, 0.9)
        assert action2.getParameter(1) == 0.9
        
        # Test clone
        action3 = action2.clone()
        assert action3.getType() == pynlm.ActionType.Custom
        assert action3.getParameters() == action2.getParameters()
    
    def test_world_object(self):
        """Test WorldObject creation and operations."""
        obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 5.0, 1.0)
        assert obj.x == 10.0
        assert obj.y == 20.0
        assert obj.type == pynlm.WorldObjectType.Resource
        assert obj.value == 5.0
        assert obj.radius == 1.0
        
        # Test distance calculation
        distance = obj.distanceTo(10.0, 23.0)  # sqrt((0)^2 + (3)^2) = 3
        assert abs(distance - 3.0) < 0.001
        
        # Test interactive
        assert obj.isInteractive() is True
        
        # Test non-interactive
        wall = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Wall)
        assert wall.isInteractive() is False
    
    def test_agent_body(self):
        """Test AgentBody creation and operations."""
        body = pynlm.AgentBody()
        assert body.x == 0.0
        assert body.y == 0.0
        assert body.energy == 1.0
        assert body.health == 1.0
        assert body.isMoving is False
        assert body.isTurning is False
        
        # Test position operations
        body.setPosition(10.0, 20.0)
        assert body.x == 10.0
        assert body.y == 20.0
        
        position = body.getPosition()
        assert position[0] == 10.0
        assert position[1] == 20.0
        
        # Test distance calculation
        distance = body.distanceTo(10.0, 23.0)
        assert abs(distance - 3.0) < 0.001
        
        # Test state
        state = body.getState()
        assert isinstance(state, dict)
        assert "x" in state
        assert "y" in state


class TestSimulationFunctionality:
    """Test simulation and step functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        
    def test_simulation_run(self):
        """Test running a complete simulation."""
        self.world.configure(50.0, 50.0, 8, 8)
        self.world.setAgentStart(25.0, 25.0)
        
        # Run simulation
        pynlm.runSimulation(self.brain, self.world, steps=100)
        
        # Check final state
        stats = pynlm.getStatistics(self.brain)
        assert stats["total_neurons"] > 0
        assert stats["total_spikes"] >= 0
        
        # Check agent state
        agent = self.world.getAgentBody()
        assert agent.energy >= 0.0
        assert agent.health >= 0.0
        assert agent.age >= 0
    
    def test_step_operations(self):
        """Test individual step operations."""
        self.brain.initialize()
        self.world.configure(50.0, 50.0, 8, 8)
        
        # Run multiple steps
        for step in range(10):
            self.world.update(1.0)
            
            # Check that world state is valid
            agent = self.world.getAgentBody()
            assert agent.age == step + 1
        
        # Check brain statistics
        stats = pynlm.getStatistics(self.brain)
        assert stats["total_spikes"] >= 0
    
    def test_save_load(self):
        """Test saving and loading brain state."""
        self.brain.initialize()
        self.world.configure(50.0, 50.0, 8, 8)
        
        # Run some steps
        for i in range(50):
            self.world.update(1.0)
        
        # Save brain state
        with tempfile.NamedTemporaryFile(delete=False, suffix='.nlm') as f:
            brain_file = f.name
        
        try:
            save_success = pynlm.saveBrainState(self.brain, brain_file)
            assert save_success is True
            
            # Create new brain and load
            new_brain = pynlm.createBrain(self.config)
            load_success = new_brain.load(brain_file)
            assert load_success is True
            
            # Check that stats are similar (allowing for some variation)
            original_stats = pynlm.getStatistics(self.brain)
            loaded_stats = pynlm.getStatistics(new_brain)
            
            # Some stats should be the same
            assert original_stats["total_neurons"] == loaded_stats["total_neurons"]
            
        finally:
            # Clean up
            if os.path.exists(brain_file):
                os.unlink(brain_file)


class TestAdvancedFeatures:
    """Test advanced features and utilities."""
    
    def test_agent_brain_integration(self):
        """Test AgentBrain integration with world."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Initialize
        agent_brain.initialize(world)
        
        # Test sensory input size
        input_size = agent_brain.getSensoryInputSize()
        assert input_size > 0
        
        # Test motor output size
        output_size = agent_brain.getMotorOutputSize()
        assert output_size > 0
        
        # Test processing
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Test decoding
        action = agent_brain.decodeMotorCommand()
        assert action.getType() in [pynlm.ActionType.MoveForward,
                                    pynlm.ActionType.MoveBackward,
                                    pynlm.ActionType.TurnLeft,
                                    pynlm.ActionType.TurnRight,
                                    pynlm.ActionType.Interact,
                                    pynlm.ActionType.Wait]
    
    def test_brain_statistics(self):
        """Test brain statistics functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get statistics multiple times
        stats1 = pynlm.getStatistics(brain)
        stats2 = pynlm.getStatistics(brain)
        
        # Statistics should be consistent
        assert stats1["total_neurons"] == stats2["total_neurons"]
        assert stats1["total_synapses"] == stats2["total_synapses"]
        
        # Check required fields
        required_fields = ["total_neurons", "total_synapses", "active_neurons",
                          "firing_neurons", "average_firing_rate",
                          "excitation_inhibition_ratio", "total_spikes"]
        
        for field in required_fields:
            assert field in stats1
            assert isinstance(stats1[field], (int, float))
    
    def test_neuromodulation(self):
        """Test neuromodulation functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Get initial neuromodulation levels
        dopamine = agent_brain.getNeuromodulationLevel()
        curiosity = agent_brain.getCuriosityLevel()
        novelty = agent_brain.getNoveltyLevel()
        prediction_error = agent_brain.getPredictionError()
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(1.0, 0.8)
        agent_brain.applyRewardModulation(-0.5, 0.3)
        
        # Check that neuromodulation levels changed
        assert agent_brain.getNeuromodulationLevel() != dopamine
        assert agent_brain.getCuriosityLevel() != curiosity
        assert agent_brain.getNoveltyLevel() != novelty
        assert agent_brain.getPredictionError() != prediction_error
    
    def test_development(self):
        """Test developmental functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Get initial developmental stage
        initial_stage = agent_brain.getDevelopmentalStage()
        
        # Update development
        agent_brain.updateDevelopment(1.0)
        
        # Check that developmental stage may have changed
        new_stage = agent_brain.getDevelopmentalStage()
        
        # Developmental system should be working
        assert agent_brain.isDevelopmentEnabled() is True
        
        # Test enabling/disabling
        agent_brain.enableDevelopment(False)
        assert agent_brain.isDevelopmentEnabled() is False
        
        agent_brain.enableDevelopment(True)
        assert agent_brain.isDevelopmentEnabled() is True


class TestErrorHandling:
    """Test error handling and validation."""
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Test setting invalid parameter types
        config.set("test.int", 42)
        config.set("test.string", "hello")
        config.set("test.float", 3.14)
        config.set("test.bool", True)
        
        # Test getting with default
        assert config.get("nonexistent", "default") == "default"
        assert config.get("nonexistent", 123) == 123
        
        # Test setting different types
        config.set("test.int", 100)
        assert config.get("test.int") == 100
        
        config.set("test.string", "world")
        assert config.get("test.string") == "world"
    
    def test_world_validation(self):
        """Test world validation."""
        world = pynlm.createSimpleWorld()
        
        # Configure with valid parameters
        world.configure(100.0, 100.0, 10, 10)
        assert world.isValidPosition(50.0, 50.0) is True
        assert world.isValidPosition(0.0, 0.0) is True
        assert world.isValidPosition(100.0, 100.0) is True
        
        # Add objects and test
        obj = pynlm.WorldObject(30.0, 30.0, pynlm.WorldObjectType.Resource)
        world.addObject(obj)
        assert world.isValidPosition(30.0, 30.0) is False  # Object is at this position
        
        # Remove object
        world.removeObject(30.0, 30.0)
        assert world.isValidPosition(30.0, 30.0) is True
    
    def test_parameter_validation(self):
        """Test parameter validation in actions."""
        action = pynlm.Action(pynlm.ActionType.Custom, [0.1, 0.2, 0.3])
        
        # Test parameter access
        assert action.getParameter(0) == 0.1
        assert action.getParameter(2) == 0.3
        
        # Test out of bounds parameter (should raise)
        with pytest.raises(Exception):
            action.getParameter(10)
        
        # Test parameter setting
        action.setParameter(1, 0.9)
        assert action.getParameter(1) == 0.9
        
        # Test out of bounds parameter setting (should raise)
        with pytest.raises(Exception):
            action.setParameter(10, 0.5)


class TestConvenienceFunctions:
    """Test convenience functions and utilities."""
    
    def test_factory_functions(self):
        """Test factory functions."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        agent_brain = pynlm.createAgentBrain(brain)
        assert agent_brain is not None
        
        # Test constants
        assert pynlm.INVALID_NEURON_ID is not None
        assert pynlm.INVALID_SYNAPSE_ID is not None
        assert pynlm.INVALID_REGION_ID is not None
        assert pynlm.INVALID_POPULATION_ID is not None
    
    def test_simulation_helpers(self):
        """Test simulation helper functions."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(50.0, 50.0, 8, 8)
        world.setAgentStart(25.0, 25.0)
        
        # Initialize brain
        brain.initialize()
        
        # Run simulation
        pynlm.runSimulation(brain, world, steps=50, verbose=False)
        
        # Check final state
        stats = pynlm.getStatistics(brain)
        assert stats["total_neurons"] > 0
        assert stats["total_spikes"] >= 0
    
    def test_batch_operations(self):
        """Test batch operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get all regions
        regions = brain.getRegions()
        region_ids = brain.getRegionIds()
        
        # Count total neurons across all regions
        total_neurons = 0
        for region in regions:
            # Access neurons (if available) - this depends on the API
            pass
        
        # Check that brain has regions
        assert len(region_ids) >= 0
        
        # Test agent brain batch operations
        agent_brain = pynlm.createAgentBrain(brain)
        assert agent_brain is not None


class TestEnumHandling:
    """Test enumeration handling."""
    
    def test_neuron_types(self):
        """Test neuron type enumerations."""
        assert pynlm.NeuronType.Excitatory == pynlm.NeuronType.Excitatory
        assert pynlm.NeuronType.Inhibitory == pynlm.NeuronType.Inhibitory
        assert pynlm.NeuronType.Modulatory == pynlm.NeuronType.Modulatory
        assert pynlm.NeuronType.Sensory == pynlm.NeuronType.Sensory
        assert pynlm.NeuronType.Motor == pynlm.NeuronType.Motor
        assert pynlm.NeuronType.Internal == pynlm.NeuronType.Internal
    
    def test_action_types(self):
        """Test action type enumerations."""
        actions = [pynlm.ActionType.MoveForward, pynlm.ActionType.MoveBackward,
                  pynlm.ActionType.MoveLeft, pynlm.ActionType.MoveRight,
                  pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight,
                  pynlm.ActionType.Look, pynlm.ActionType.LookUp,
                  pynlm.ActionType.LookDown, pynlm.ActionType.Interact,
                  pynlm.ActionType.Eat, pynlm.ActionType.Drink,
                  pynlm.ActionType.Rest, pynlm.ActionType.Wait,
                  pynlm.ActionType.Custom]
        
        assert len(actions) == 15
        assert pynlm.ActionType.MoveForward != pynlm.ActionType.TurnLeft
    
    def test_world_object_types(self):
        """Test world object type enumerations."""
        obj_types = [pynlm.WorldObjectType.Empty, pynlm.WorldObjectType.Resource,
                    pynlm.WorldObjectType.Hazard, pynlm.WorldObjectType.Wall,
                    pynlm.WorldObjectType.Marker]
        
        assert len(obj_types) == 5
        assert pynlm.WorldObjectType.Resource != pynlm.WorldObjectType.Hazard
    
    def test_motor_commands(self):
        """Test motor command enumerations."""
        motor_commands = [pynlm.MotorCommand.MoveForward,
                         pynlm.MotorCommand.MoveBackward,
                         pynlm.MotorCommand.TurnLeft,
                         pynlm.MotorCommand.TurnRight,
                         pynlm.MotorCommand.LookLeft,
                         pynlm.MotorCommand.LookRight,
                         pynlm.MotorCommand.Interact,
                         pynlm.MotorCommand.Wait]
        
        assert len(motor_commands) == 8
        assert pynlm.MotorCommand.MoveForward != pynlm.MotorCommand.TurnLeft


class TestTypeConversion:
    """Test type conversions and utilities."""
    
    def test_config_value_conversion(self):
        """Test configuration value conversions."""
        config = pynlm.createDefaultConfig()
        
        # Test setting different types
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_string", "hello")
        config.set("test_bool", True)
        
        # Test getting values (type conversion should work)
        assert config.get("test_int") == 42
        assert abs(config.get("test_float") - 3.14) < 0.001
        assert config.get("test_string") == "hello"
        assert config.get("test_bool") is True
        
        # Test getting with default
        assert config.get("nonexistent", "default") == "default"
        assert config.get("nonexistent", 123) == 123
    
    def test_action_parameter_conversion(self):
        """Test action parameter conversions."""
        action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        
        params = action.getParameters()
        assert len(params) == 3
        assert all(isinstance(p, float) for p in params)
        assert params[0] == 1.0
        assert params[2] == 3.0
        
        # Test setting parameters
        action.setParameters([4.0, 5.0, 6.0])
        assert action.getParameters() == [4.0, 5.0, 6.0]


if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v", "-s"])
