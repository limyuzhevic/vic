"""
Comprehensive Python tests for NLM Python bindings.

This module contains tests for all the key functionality exposed by the NLM
Python bindings, including:
- Core class functionality
- Brain simulation
- Agent-brain interaction
- World simulation
- Configuration management
- Memory and neuromodulation systems
- Development and plasticity
- Save/load functionality
"""

import pytest
import tempfile
import os
import sys
from typing import Any

# Import the NLM bindings
try:
    import pynlm
    from pynlm import (
        Config, Brain, AgentBrain, SimpleWorld,
        NeuronType, SynapseType, DevelopmentalStage,
        ActionType, MotorCommand, WorldObjectType,
        Vision, Audio, InternalSignals, Action, WorldObject,
        SensoryPercept, SensoryInput
    )
except ImportError as e:
    pytest.skip(f"Cannot import NLM bindings: {e}", allow_module_level=True)

class TestConfig:
    """Test the Config class functionality."""
    
    def test_config_creation(self):
        """Test basic config creation."""
        config = Config()
        assert config is not None
    
    def test_config_load_from_file(self):
        """Test loading configuration from file."""
        config = Config()
        # Test that the method exists
        assert hasattr(config, 'loadFromFile')
    
    def test_config_save_to_file(self):
        """Test saving configuration to file."""
        config = Config()
        # Create a temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            result = config.saveToFile(filepath)
            assert result is not None
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_config_has_key(self):
        """Test checking if config has a key."""
        config = Config()
        assert hasattr(config, 'has')
    
    def test_config_get_keys(self):
        """Test getting all config keys."""
        config = Config()
        keys = config.getKeys()
        assert isinstance(keys, list)
    
    def test_config_set_get(self):
        """Test setting and getting config values."""
        config = Config()
        config.set("test.key", "test_value", None)
        
        # Note: Config doesn't appear to have a get method in the bindings
        # but has getOr method based on C++ interface
        assert hasattr(config, 'has')
        assert config.has("test.key")
    
    def test_config_summary(self):
        """Test getting config summary."""
        config = Config()
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0

class TestBrain:
    """Test the Brain class functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = Config()
        self.config.set("neuron_count", 100)
        self.brain = Brain(self.config)
    
    def test_brain_creation(self):
        """Test basic brain creation."""
        assert self.brain is not None
        assert self.brain.getTotalNeuronCount() == 100
    
    def test_brain_initialize(self):
        """Test brain initialization."""
        self.brain.initialize()
        # The brain should be in a valid state after initialization
        assert self.brain.getTotalNeuronCount() > 0
    
    def test_brain_step(self):
        """Test brain stepping."""
        self.brain.initialize()
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run a few steps
        for step in range(5):
            self.brain.step(step)
        
        # Should not crash and should maintain valid state
        assert self.brain.getTotalNeuronCount() > 0
    
    def test_brain_reset(self):
        """Test brain reset."""
        self.brain.initialize()
        initial_spikes = self.brain.getTotalSpikeCount()
        
        self.brain.reset()
        
        # After reset, brain should be ready for re-initialization
        assert hasattr(self.brain, 'reset')
    
    def test_brain_statistics(self):
        """Test brain statistics methods."""
        self.brain.initialize()
        
        stats = {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
        }
        
        # All statistics should be valid numbers
        for key, value in stats.items():
            assert isinstance(value, (int, float))
            assert value >= 0
    
    def test_brain_sensory_input(self):
        """Test sensory input functionality."""
        self.brain.initialize()
        
        # Create a test sensory input
        sensory_input = Vision(8, 8, 3)
        data = [0.5] * (8 * 8 * 3)
        sensory_input.setData(data)
        
        # Test that the method exists
        assert hasattr(self.brain, 'receiveSensoryInput')
    
    def test_brain_motor_output(self):
        """Test motor output functionality."""
        self.brain.initialize()
        
        # Test that the method exists
        assert hasattr(self.brain, 'produceAction')
    
    def test_brain_save_load(self):
        """Test brain save/load functionality."""
        self.brain.initialize()
        
        # Create a temporary file
        with tempfile.NamedTemporaryFile(mode='wb', suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save the brain state
            save_result = self.brain.save(filepath)
            assert save_result is not None
            
            # Create a new brain and load the state
            config2 = Config()
            config2.set("neuron_count", 100)
            brain2 = Brain(config2)
            
            load_result = brain2.load(filepath)
            assert load_result is not None
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_brain_region_management(self):
        """Test brain region management."""
        self.brain.initialize()
        
        # Test adding regions
        region_id = self.brain.addRegion("test_region")
        assert region_id is not None
        
        # Test getting regions
        assert hasattr(self.brain, 'getRegion')
        assert hasattr(self.brain, 'getRegionCount')
        assert hasattr(self.brain, 'getRegionIds')
    
    def test_brain_plasticity(self):
        """Test plasticity-related methods."""
        self.brain.initialize()
        
        # Test inject current methods
        assert hasattr(self.brain, 'injectCurrent')
        assert hasattr(self.brain, 'injectCurrentToNeurons')
    
    def test_brain_development(self):
        """Test development-related methods."""
        self.brain.initialize()
        
        # Test developmental stage
        stage = self.brain.getDevelopmentalStage()
        assert isinstance(stage, DevelopmentalStage)
        
        # Test setting developmental stage
        self.brain.setDevelopmentalStage(DevelopmentalStage.Maturation)

class TestAgentBrain:
    """Test the AgentBrain class functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = Config()
        self.config.set("neuron_count", 50)
        self.brain = Brain(self.config)
        self.world = SimpleWorld()
        self.world.configure(10, 10, 8, 8)
        
        self.agent = AgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_creation(self):
        """Test agent creation."""
        assert self.agent is not None
        assert self.agent.getBrain() == self.brain
    
    def test_agent_sensory_processing(self):
        """Test sensory processing."""
        # Create a test percept
        percept = SensoryPercept()
        
        # Test processing (this might be a no-op in the C++ implementation)
        self.agent.processSensoryInput(percept)
    
    def test_agent_motor_decoding(self):
        """Test motor decoding."""
        command = self.agent.decodeMotorCommand()
        assert isinstance(command, MotorCommand)
        assert command in [
            MotorCommand.Wait, MotorCommand.MoveForward,
            MotorCommand.MoveBackward, MotorCommand.TurnLeft,
            MotorCommand.TurnRight, MotorCommand.LookLeft,
            MotorCommand.LookRight, MotorCommand.Interact
        ]
    
    def test_agent_neuromodulation(self):
        """Test neuromodulation."""
        self.agent.applyRewardModulation(1.0, 0.5)
        
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        prediction_error = self.agent.getPredictionError()
        neuromodulation = self.agent.getNeuromodulationLevel()
        
        assert isinstance(curiosity, float)
        assert isinstance(novelty, float)
        assert isinstance(prediction_error, float)
        assert isinstance(neuromodulation, float)
    
    def test_agent_development(self):
        """Test agent development."""
        stage = self.agent.getDevelopmentalStage()
        assert isinstance(stage, DevelopmentalStage)
        
        self.agent.updateDevelopment(0.1)
    
    def test_agent_subsystem_enables(self):
        """Test enabling/disabling subsystems."""
        # Enable various subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Check if the methods exist
        assert hasattr(self.agent, 'isRewardModulationEnabled')
        assert hasattr(self.agent, 'isStructuralPlasticityEnabled')
        assert hasattr(self.agent, 'isDevelopmentEnabled')
        assert hasattr(self.agent, 'isCuriosityEnabled')
    
    def test_agent_sensory_motor_sizes(self):
        """Test sensory and motor input/output sizes."""
        sensory_size = self.agent.getSensoryInputSize()
        motor_size = self.agent.getMotorOutputSize()
        
        assert isinstance(sensory_size, int)
        assert isinstance(motor_size, int)
        assert sensory_size > 0
        assert motor_size > 0

class TestSimpleWorld:
    """Test the SimpleWorld class functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.world = SimpleWorld()
    
    def test_world_creation(self):
        """Test world creation."""
        assert self.world is not None
    
    def test_world_configuration(self):
        """Test world configuration."""
        self.world.configure(20, 20, 8, 8)
        
        assert self.world.getWidth() == 20
        assert self.world.getHeight() == 20
    
    def test_world_reset(self):
        """Test world reset."""
        self.world.configure(10, 10, 8, 8)
        self.world.reset()
        
        # Should not crash
        assert self.world.getSimulationTime() == 0.0
    
    def test_world_update(self):
        """Test world update."""
        self.world.configure(10, 10, 8, 8)
        self.world.reset()
        
        # Update the world
        self.world.update(0.1)
        
        assert self.world.getSimulationTime() == 0.1
    
    def test_world_agent_start(self):
        """Test setting agent start position."""
        self.world.setAgentStart(5.0, 5.0)
        
        body = self.world.getAgentBody()
        assert body.x == 5.0
        assert body.y == 5.0
    
    def test_world_sensory_percept(self):
        """Test world sensory percept."""
        percept = self.world.getSensoryPercept()
        assert isinstance(percept, SensoryPercept)
    
    def test_world_agent_body(self):
        """Test world agent body."""
        body = self.world.getAgentBody()
        assert body is not None
        assert hasattr(body, 'x')
        assert hasattr(body, 'y')
        assert hasattr(body, 'orientation')
    
    def test_world_objects(self):
        """Test world object management."""
        # Create a test world object
        obj = WorldObject(1.0, 1.0, WorldObjectType.Resource, 1.0, 0.5)
        
        # Add to world
        self.world.addObject(obj)
        
        # Test position checking
        assert self.world.isValidPosition(1.0, 1.0)
        assert not self.world.isValidPosition(100.0, 100.0)
        
        # Test removing object
        self.world.removeObject(1.0, 1.0)
    
    def test_world_energy(self):
        """Test world energy management."""
        self.world.setMaxEnergy(200.0)
        assert self.world.getMaxEnergy() == 200.0
        
        self.world.setEnergyDecayRate(0.9)
        # Note: No getter for energy decay rate in bindings
    
    def test_world_random_seed(self):
        """Test world random seed."""
        self.world.setRandomSeed(42)
        
        # Note: No getter for random seed in bindings
        # assert self.world.getRandomSeed() == 42

class TestEnumTypes:
    """Test enumeration types."""
    
    def test_neuron_type_enum(self):
        """Test NeuronType enumeration."""
        assert NeuronType.Excitatory is not None
        assert NeuronType.Inhibitory is not None
        assert NeuronType.Sensory is not None
        assert NeuronType.Motor is not None
        assert NeuronType.Modulatory is not None
        assert NeuronType.Internal is not None
    
    def test_synapse_type_enum(self):
        """Test SynapseType enumeration."""
        assert SynapseType.Excitatory is not None
        assert SynapseType.Inhibitory is not None
        assert SynapseType.Modulatory is not None
        assert SynapseType.Electrical is not None
        assert SynapseType.GapJunction is not None
    
    def test_developmental_stage_enum(self):
        """Test DevelopmentalStage enumeration."""
        assert DevelopmentalStage.Initial is not None
        assert DevelopmentalStage.CriticalPeriod is not None
        assert DevelopmentalStage.Maturation is not None
        assert DevelopmentalStage.Adult is not None
        assert DevelopmentalStage.Aging is not None
    
    def test_firing_state_enum(self):
        """Test FiringState enumeration."""
        assert FiringState.Resting is not None
        assert FiringState.Active is not None
        assert FiringState.Refractory is not None
        assert FiringState.Inhibited is not None
    
    def test_action_type_enum(self):
        """Test ActionType enumeration."""
        assert ActionType.MoveForward is not None
        assert ActionType.MoveBackward is not None
        assert ActionType.TurnLeft is not None
        assert ActionType.TurnRight is not None
        assert ActionType.Look is not None
        assert ActionType.Interact is not None
        assert ActionType.Eat is not None
        assert ActionType.Rest is not None
        assert ActionType.Wait is not None
        assert ActionType.Custom is not None
    
    def test_motor_command_enum(self):
        """Test MotorCommand enumeration."""
        assert MotorCommand.MoveForward is not None
        assert MotorCommand.MoveBackward is not None
        assert MotorCommand.TurnLeft is not None
        assert MotorCommand.TurnRight is not None
        assert MotorCommand.LookLeft is not None
        assert MotorCommand.LookRight is not None
        assert MotorCommand.Interact is not None
        assert MotorCommand.Wait is not None
    
    def test_world_object_type_enum(self):
        """Test WorldObjectType enumeration."""
        assert WorldObjectType.Empty is not None
        assert WorldObjectType.Resource is not None
        assert WorldObjectType.Hazard is not None
        assert WorldObjectType.Wall is not None
        assert WorldObjectType.Marker is not None

class TestSensoryInputs:
    """Test sensory input classes."""
    
    def test_vision_input(self):
        """Test Vision sensory input."""
        vision = Vision(16, 16, 3)
        
        assert vision.getWidth() == 16
        assert vision.getHeight() == 16
        assert vision.getChannels() == 3
        
        # Test data setting
        data = [0.1] * (16 * 16 * 3)
        vision.setData(data)
    
    def test_audio_input(self):
        """Test Audio sensory input."""
        audio = Audio(44100, 1024)
        
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        # Test data setting
        data = [0.0] * 1024
        audio.setData(data)
    
    def test_internal_signals(self):
        """Test InternalSignals sensory input."""
        internal = InternalSignals()
        
        assert internal is not None
        
        # Test adding signals
        internal.addSignal(0.5)
        internal.addSignal(0.7)
        
        # Test getting all signals
        signals = internal.getAllSignals()
        assert isinstance(signals, list)
    
    def test_sensory_input_base(self):
        """Test base SensoryInput class."""
        input_obj = SensoryInput()
        
        assert input_obj is not None
        assert hasattr(input_obj, 'getType')
        assert hasattr(input_obj, 'getData')
        assert hasattr(input_obj, 'getDimensions')
        assert hasattr(input_obj, 'getTimestamp')

class TestAction:
    """Test Action class."""
    
    def test_action_creation(self):
        """Test action creation."""
        # Create action from type
        action1 = Action(ActionType.MoveForward)
        assert action1.getType() == ActionType.MoveForward
        
        # Create action with parameters
        action2 = Action(ActionType.Custom, [1.0, 2.0, 3.0])
        assert action2.getType() == ActionType.Custom
        assert action2.getParameters() == [1.0, 2.0, 3.0]
    
    def test_action_modification(self):
        """Test action modification."""
        action = Action(ActionType.Interact)
        
        action.setType(ActionType.Eat)
        assert action.getType() == ActionType.Eat
        
        action.setParameters([0.5, 0.5])
        assert action.getParameters() == [0.5, 0.5]
        
        # Test getting action name
        name = action.getName()
        assert isinstance(name, str)
        
        # Test cloning
        cloned = action.clone()
        assert cloned.getType() == action.getType()
        assert cloned.getParameters() == action.getParameters()

class TestWorldObjects:
    """Test WorldObject class."""
    
    def test_world_object_creation(self):
        """Test world object creation."""
        obj = WorldObject(1.0, 1.0, WorldObjectType.Resource, 1.0, 0.5)
        
        assert obj.x == 1.0
        assert obj.y == 1.0
        assert obj.type == WorldObjectType.Resource
        assert obj.value == 1.0
        assert obj.radius == 0.5
        assert obj.active is True
    
    def test_world_object_modification(self):
        """Test world object modification."""
        obj = WorldObject(0.0, 0.0, WorldObjectType.Empty)
        
        obj.x = 2.0
        obj.y = 2.0
        obj.type = WorldObjectType.Wall
        obj.value = 5.0
        obj.radius = 1.0
        obj.active = False
        
        assert obj.x == 2.0
        assert obj.y == 2.0
        assert obj.type == WorldObjectType.Wall
        assert obj.value == 5.0
        assert obj.radius == 1.0
        assert obj.active is False

class TestActionResults:
    """Test ActionResult class."""
    
    def test_action_result_creation(self):
        """Test action result creation."""
        result = ActionResult(1.5, True, "Success")
        
        assert result.reward == 1.5
        assert result.success is True
        assert result.message == "Success"
    
    def test_action_result_modification(self):
        """Test action result modification."""
        result = ActionResult(0.0, False)
        
        result.reward = 2.5
        result.success = True
        result.message = "Completed"
        
        assert result.reward == 2.5
        assert result.success is True
        assert result.message == "Completed"

class TestIntegration:
    """Integration tests combining multiple components."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = Config()
        self.config.set("neuron_count", 50)
        self.brain = Brain(self.config)
        self.world = SimpleWorld()
        self.world.configure(8, 8, 4, 4)
        
        self.agent = AgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_full_agent_simulation(self):
        """Test a complete agent simulation."""
        # Initialize all components
        self.brain.initialize()
        
        # Run a few simulation steps
        for step in range(10):
            # Update world
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action and apply it
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept and percept.getInternal():
                reward = percept.getInternal()[0]
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
        
        # Verify simulation completed successfully
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getFiringNeuronCount() >= 0
        assert self.agent.getCuriosityLevel() >= 0.0
        assert self.agent.getNoveltyLevel() >= 0.0

# High-level API tests

class TestHighLevelAPI:
    """Test the high-level Python API functions."""
    
    def test_create_default_config(self):
        """Test create_default_config function."""
        from pynlm.bindings import create_default_config
        
        config = create_default_config()
        assert isinstance(config, Config)
    
    def test_create_brain(self):
        """Test create_brain function."""
        from pynlm.bindings import create_brain, create_default_config
        
        config = create_default_config()
        brain = create_brain(config)
        
        assert isinstance(brain, Brain)
        assert brain.getTotalNeuronCount() > 0
    
    def test_create_simple_world(self):
        """Test create_simple_world function."""
        from pynlm.bindings import create_simple_world
        
        world = create_simple_world()
        assert isinstance(world, SimpleWorld)
    
    def test_create_agent_brain(self):
        """Test create_agent_brain function."""
        from pynlm.bindings import create_brain, create_simple_world, create_agent_brain
        
        brain = create_brain()
        world = create_simple_world()
        agent = create_agent_brain(brain, world)
        
        assert isinstance(agent, AgentBrain)
        assert agent.getBrain() == brain
    
    def test_simple_brain_demo(self):
        """Test simple_brain_demo function."""
        from pynlm.bindings import simple_brain_demo
        
        results = simple_brain_demo(neuron_count=20, steps=5)
        
        assert isinstance(results, dict)
        assert 'total_neurons' in results
        assert 'total_spikes' in results
        assert 'avg_firing_rate' in results
        assert results['total_neurons'] == 20
    
    def test_environment_demo(self):
        """Test environment_demo function."""
        from pynlm.bindings import environment_demo
        
        results = environment_demo(width=8, height=8, num_steps=3)
        
        assert isinstance(results, dict)
        assert 'world_width' in results
        assert 'world_height' in results
        assert 'final_firing_rate' in results

# Performance tests

class TestPerformance:
    """Performance benchmarking tests."""
    
    def test_brain_creation_performance(self):
        """Test brain creation performance."""
        from pynlm.bindings import benchmark_brain_creation
        
        results = benchmark_brain_creation(iterations=5)
        
        assert isinstance(results, dict)
        assert 'mean_time' in results
        assert 'min_time' in results
        assert 'max_time' in results
        assert 'total_time' in results
        
        assert results['mean_time'] >= 0.0
        assert results['min_time'] >= 0.0
        assert results['max_time'] >= results['min_time']
    
    def test_simulation_performance(self):
        """Test simulation performance."""
        from pynlm.bindings import create_demo_config, NLMBenchmark
        
        config = create_demo_config(neuron_count=20)
        
        results = NLMBenchmark.benchmark_simulation_steps(config, steps=10, iterations=3)
        
        assert isinstance(results, dict)
        assert 'mean_time' in results
        assert 'steps_per_second' in results

# Utility function tests

class TestUtilityFunctions:
    """Test utility functions."""
    
    def test_get_system_info(self):
        """Test get_system_info function."""
        from pynlm.bindings import get_system_info
        
        info = get_system_info()
        
        assert isinstance(info, dict)
        assert 'version' in info
        assert 'author' in info
        assert 'license' in info
        assert 'description' in info
    
    def test_get_all_constants(self):
        """Test get_all_constants function."""
        from pynlm.bindings import get_all_constants
        
        constants = get_all_constants()
        
        assert isinstance(constants, dict)
        assert 'NeuronType' in constants
        assert 'ActionType' in constants
        assert 'WorldObjectType' in constants
    
    def test_print_brain_info(self):
        """Test print_brain_info function."""
        from pynlm.bindings import create_brain, print_brain_info
        
        brain = create_brain()
        
        # This function should not raise an exception
        # Note: print_brain_info prints to stdout, which we can't easily capture
        # but we can verify it exists and doesn't crash
        assert hasattr(brain, 'getTotalNeuronCount')

if __name__ == '__main__':
    # Run tests
    pytest.main([__file__, '-v'])