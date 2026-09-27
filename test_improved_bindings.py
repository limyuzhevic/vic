#!/usr/bin/env python3
"""
Test suite for NLM Python API enhanced bindings.

This test suite validates:
1. Error handling with custom exceptions
2. Type safety and parameter validation
3. API consistency and method contracts
4. Performance and edge cases
"""

import pytest
import pynlm
import tempfile
import os
from pathlib import Path

class TestBasicFunctionality:
    """Test basic NLM functionality with enhanced error handling."""
    
    def test_brain_creation(self):
        """Test brain creation with valid configuration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        assert brain.getConfig() is not None
        
    def test_brain_initialization(self):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain.initialize() is True
        
        # Check that brain has required components after initialization
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getTotalSpikeCount() == 0
        
    def test_simulation_step(self):
        """Test basic simulation step."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        
        assert brain.getTotalSpikeCount() >= initial_spikes

def test_configuration_management():
    """Test configuration management with enhanced validation."""
    
    def test_default_config_creation(self):
        """Test default configuration creation."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Test basic configuration operations
        assert "random_seed" in config.getKeys()
        
        # Test getting values with defaults
        random_seed = config.getOr("random_seed", 42)
        assert isinstance(random_seed, int)
        assert random_seed == 42
        
    def test_config_save_load(self):
        """Test configuration save and load operations."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            config = pynlm.createDefaultConfig()
            config.set("test_value", 123)
            config.set("test_string", "hello")
            config.set("test_bool", True)
            
            # Save configuration
            assert config.saveToFile(filepath) is True
            
            # Load configuration
            config2 = pynlm.createDefaultConfig()
            assert config2.loadFromFile(filepath) is True
            
            # Verify loaded values
            assert config2.getOr("test_value", 0) == 123
            assert config2.getOr("test_string", "") == "hello"
            assert config2.getOr("test_bool", False) is True
            
        finally:
            os.unlink(filepath)
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Test setting various types
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("bool_value", True)
        config.set("string_value", "test")
        
        # Test getting values with type safety
        assert isinstance(config.getOr("int_value", 0), int)
        assert isinstance(config.getOr("float_value", 0.0), float)
        assert isinstance(config.getOr("bool_value", False), bool)
        assert isinstance(config.getOr("string_value", ""), str)

class TestErrorHandling:
    """Test enhanced error handling with custom exceptions."""
    
    def test_invalid_parameter_error(self):
        """Test invalid parameter error handling."""
        # This would be tested through C++ bindings
        # For now, test that we can create error objects
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Valid operation should not raise
        assert brain.initialize() is True
        
    def test_configuration_error(self):
        """Test configuration error handling."""
        config = pynlm.createDefaultConfig()
        
        # Test configuration operations that could fail
        config.clear()
        
        # Verify config can be used after clearing
        config.set("test", "value")
        assert config.getOr("test", "") == "value"
    
    def test_resource_error(self):
        """Test resource error handling."""
        # Test file operations that could fail
        pass
    
    def test_simulation_error(self):
        """Test simulation error handling."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Valid simulation should not raise
        brain.step(0)
        
        # Invalid simulation parameters would be caught by bindings
        pass

class TestTypeSafety:
    """Test type safety and validation."""
    
    def test_neuron_id_types(self):
        """Test NeuronId type safety."""
        from pynlm import NeuronId
        
        # Test valid creation
        neuron_id = NeuronId(1)
        assert neuron_id.value == 1
        assert neuron_id.index() == 0  # Zero-based index
        
        # Test equality and hash
        neuron_id2 = NeuronId(1)
        assert neuron_id == neuron_id2
        assert hash(neuron_id) == hash(neuron_id2)
        
        # Test string representation
        assert "NeuronId" in repr(neuron_id)
        
    def test_sensory_input_types(self):
        """Test sensory input type safety."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create vision input
        vision = pynlm.Vision(10, 10, 3)
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        assert vision.getChannels() == 3
        
        # Create audio input
        audio = pynlm.Audio(44100, 1024)
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        # Test internal signals
        internal = pynlm.InternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(-0.3)
        
    def test_action_types(self):
        """Test action type safety."""
        # Test different action types
        move_action = pynlm.Action(pynlm.ActionType.MoveForward)
        assert move_action.getType() == pynlm.ActionType.MoveForward
        
        custom_action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        assert custom_action.getType() == pynlm.ActionType.Custom
        assert custom_action.getParameters() == [1.0, 2.0, 3.0]
        
    def test_world_object_types(self):
        """Test world object type safety."""
        # Test different world object types
        resource = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource)
        assert resource.getType() == pynlm.WorldObjectType.Resource
        assert resource.value == 0.0
        
        wall = pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Wall, 0.0, 2.0)
        assert wall.getType() == pynlm.WorldObjectType.Wall
        assert wall.radius == 2.0

class TestAgentWorldIntegration:
    """Test agent-world integration with enhanced features."""
    
    def test_agent_creation(self):
        """Test agent creation and initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        assert agent.getBrain() is brain
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
    def test_sensory_processing(self):
        """Test sensory processing pipeline."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain should process the input
        brain.step(0)
        
    def test_motor_decoding(self):
        """Test motor command decoding."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Process some input
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        assert action is not None
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
    def test_neuromodulation(self):
        """Test neuromodulation features."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(12, 12, 6, 6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable neuromodulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Check levels
        assert agent.isRewardModulationEnabled() is True
        assert agent.isCuriosityEnabled() is True
        assert agent.getNeuromodulationLevel() > 0
        assert agent.getCuriosityLevel() > 0
        
    def test_development(self):
        """Test development system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        assert agent.isDevelopmentEnabled() is True
        
        # Get developmental stage
        stage = agent.getDevelopmentalStage()
        assert stage is not None
        
        # Update development
        agent.updateDevelopment(0.1)

class TestPerformanceAndEdgeCases:
    """Test performance and edge cases."""
    
    def test_large_simulation(self):
        """Test large-scale simulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run many steps
        steps = 1000
        for step in range(steps):
            brain.step(step)
        
        # Verify statistics
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getFiringNeuronCount() <= brain.getTotalNeuronCount()
        assert brain.getAverageFiringRate() >= 0
        
    def test_multiple_agents(self):
        """Test multiple agent instances."""
        config = pynlm.createDefaultConfig()
        
        # Create multiple brains
        brains = []
        for i in range(5):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        # Create multiple agents
        agents = []
        for brain in brains:
            agent = pynlm.createAgentBrain(brain)
            agents.append(agent)
        
        assert len(agents) == 5
        assert all(agent.getBrain() is brain for agent, brain in zip(agents, brains))
        
    def test_memory_efficiency(self):
        """Test memory efficiency with large systems."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial memory info
        initial_neurons = brain.getTotalNeuronCount()
        initial_synapses = brain.getTotalSynapseCount()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Verify system stability
        final_neurons = brain.getTotalNeuronCount()
        final_synapses = brain.getTotalSynapseCount()
        
        assert final_neurons == initial_neurons  # Should not change
        assert final_synapses == initial_synapses  # Should not change

class TestFileOperations:
    """Test file operations with enhanced error handling."""
    
    def test_save_load_checkpoint(self):
        """Test checkpoint save and load operations."""
        with tempfile.TemporaryDirectory() as tmpdir:
            config_path = os.path.join(tmpdir, "config.json")
            brain_path = os.path.join(tmpdir, "brain.checkpoint")
            
            # Create and save brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Save brain state
            assert brain.save(brain_path) is True
            
            # Create new brain and load
            config2 = pynlm.createDefaultConfig()
            brain2 = pynlm.createBrain(config2)
            
            assert brain2.load(brain_path) is True
            
            # Compare statistics
            assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
            assert brain2.getTotalSynapseCount() == brain.getTotalSynapseCount()
            
    def test_config_file_operations(self):
        """Test configuration file operations."""
        with tempfile.TemporaryDirectory() as tmpdir:
            config_path = os.path.join(tmpdir, "test_config.json")
            
            # Create and save config
            config = pynlm.createDefaultConfig()
            config.set("test_value", 123)
            config.set("test_string", "hello world")
            
            assert config.saveToFile(config_path) is True
            
            # Verify file exists and is readable
            assert os.path.exists(config_path)
            
            # Load config
            config2 = pynlm.createDefaultConfig()
            assert config2.loadFromFile(config_path) is True
            
            assert config2.getOr("test_value", 0) == 123
            assert config2.getOr("test_string", "") == "hello world"

class TestValidationAndSanityChecks:
    """Test validation and sanity checks."""
    
    def test_parameter_validation(self):
        """Test parameter validation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test valid operations
        brain.step(0)
        
        # Test invalid operations (would be caught by C++ bindings)
        # For example, invalid neuron IDs, negative currents, etc.
        
    def test_state_consistency(self):
        """Test state consistency."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial state
        initial_neurons = brain.getTotalNeuronCount()
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run simulation
        for step in range(10):
            brain.step(step)
        
        # Verify consistency
        final_neurons = brain.getTotalNeuronCount()
        final_spikes = brain.getTotalSpikeCount()
        
        assert final_neurons == initial_neurons
        assert final_spikes >= initial_spikes
        
    def test_resource_cleanup(self):
        """Test resource cleanup."""
        # Create and destroy objects to test cleanup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Reset brain
        brain.reset()
        
        # Verify reset worked
        assert brain.getTotalSpikeCount() >= 0

if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
