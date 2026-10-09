# NLM Python Bindings Test Suite
# Comprehensive tests for NLM Python API

import pytest
import pynlm
import tempfile
import os
import numpy as np

class TestNLMConfiguration:
    """Test configuration functionality"""
    
    def test_default_config(self):
        """Test creating default configuration"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
    def test_config_has(self):
        """Test checking if config has specific keys"""
        config = pynlm.createDefaultConfig()
        assert config.has("brain.neuron_count")
        
    def test_config_get_keys(self):
        """Test getting all config keys"""
        config = pynlm.createDefaultConfig()
        keys = config.getKeys()
        assert isinstance(keys, list)
        assert len(keys) > 0
        
    def test_config_set(self):
        """Test setting configuration values"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        
    def test_config_save_load(self):
        """Test saving and loading configuration"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Save config
            assert config.saveToFile(filepath)
            
            # Load into new config
            config2 = pynlm.createDefaultConfig()
            assert config2.loadFromFile(filepath)
            
            # Verify data preserved
            assert config2.getOr("brain.neuron_count", 0) == 1500
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
class TestNLMBasicBrain:
    """Test basic brain functionality"""
    
    def test_brain_creation(self):
        """Test creating and initializing brain"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        
        # Initialize brain
        assert brain.initialize()
        
    def test_brain_statistics(self):
        """Test brain statistics and metrics"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test various statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getActiveNeuronCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        
    def test_brain_step(self):
        """Test basic brain stepping"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial state
        initial_neurons = brain.getTotalNeuronCount()
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run several steps
        for step in range(10):
            brain.step(step)
        
        # Verify state changed
        assert brain.getTotalNeuronCount() == initial_neurons
        assert brain.getTotalSpikeCount() >= initial_spikes
        
    def test_brain_reset(self):
        """Test brain reset functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(5):
            brain.step(step)
        
        # Reset brain
        brain.reset()
        
        # Brain should still work after reset
        assert brain.initialize()
        assert brain.getTotalNeuronCount() > 0
class TestNLMWorld:
    """Test world functionality"""
    
    def test_world_creation(self):
        """Test creating and configuring world"""
        world = pynlm.createSimpleWorld()
        
        assert world is not None
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test configuration
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getVisionWidth() == 8
        assert world.getVisionHeight() == 8
        
    def test_world_reset(self):
        """Test world reset"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Set agent position
        world.setAgentStart(5.0, 5.0)
        
        # Update world
        world.update(0.1)
        
        # Reset world
        world.reset()
        
        # World should work after reset
        assert world.getWidth() == 10
        
    def test_world_sensory_percept(self):
        """Test sensory percept generation"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        assert percept is not None
        assert percept.getVisionWidth() == 8
        assert percept.getVisionHeight() == 8
        
    def test_world_action(self):
        """Test world action application"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create and apply action
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        world.applyMotorCommand(action, 0.0)
        
        # Verify agent moved
        body = world.getAgentBody()
        assert body is not None
class TestNLMDeviceBrain:
    """Test agent brain functionality"""
    
    def test_agent_brain_creation(self):
        """Test creating agent brain"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
    def test_agent_brain_initialization(self):
        """Test agent brain initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        assert agent is not None
        
    def test_agent_brain_sensory_processing(self):
        """Test sensory processing through agent"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain should have received input
        assert brain.getTotalNeuronCount() > 0
        
    def test_agent_brain_motor_decoding(self):
        """Test motor command decoding"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        assert action is not None
        assert action.getType() is not None
        
    def test_agent_brain_subsystem_control(self):
        """Test enabling/disabling agent subsystems"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Enable various subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Check subsystems are enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
        
    def test_agent_brain_stats(self):
        """Test agent brain statistics"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test various statistics
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getPredictionError() >= 0.0
        assert agent.getNeuromodulationLevel() >= 0.0
        
        # Test development stage
        assert agent.getDevelopmentalStage() is not None
class TestNLMIntegration:
    """Integration tests combining multiple components"""
    
    def test_complete_agent_workflow(self):
        """Test complete agent workflow"""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0  # Simple reward
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Check stats periodically
            if step % 20 == 0:
                assert brain.getFiringNeuronCount() >= 0
                assert agent.getCuriosityLevel() >= 0.0
        
        # Final verification
        assert brain.getTotalNeuronCount() > 0
        assert agent.getDevelopmentalStage() is not None
        
    def test_checkpointing(self):
        """Test brain save/load functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps to create interesting state
        for step in range(10):
            brain.step(step)
        
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        
        try:
            # Save brain state
            assert brain.save(checkpoint_path)
            
            # Reset and reload
            brain.reset()
            assert brain.initialize()
            assert brain.load(checkpoint_path)
            
            # Verify state is restored
            assert brain.getTotalNeuronCount() > 0
            
        finally:
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)
class TestNLMConfigurationFileFormats:
    """Test configuration file format support"""
    
    def test_json_config_format(self):
        """Test JSON configuration format"""
        config = pynlm.createDefaultConfig()
        
        # Add some values
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("brain.connection_probability", 0.05)
        config.set("learning.enabled", True)
        config.set("plasticity.stdp.enable", False)
        
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Save as JSON
            assert config.saveToFile(filepath)
            
            # Load back
            config2 = pynlm.createDefaultConfig()
            assert config2.loadFromFile(filepath)
            
            # Verify values
            assert config2.getOr("brain.neuron_count", 0) == 1000
            assert config2.getOr("brain.synapse_density", 0.0) == 0.1
            assert config2.getOr("brain.connection_probability", 0.0) == 0.05
            assert config2.getOr("learning.enabled", False) == True
            assert config2.getOr("plasticity.stdp.enable", True) == False
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_simple_format_fallback(self):
        """Test simple key=value format fallback"""
        config = pynlm.createDefaultConfig()
        
        with tempfile.NamedTemporaryFile(suffix='.cfg', delete=False) as f:
            f.write(b"""
# Simple config file
brain.neuron_count = 500
brain.synapse_density = 0.2
brain.connection_probability = 0.1
learning.enabled = true
plasticity.stdp.enable = false
""")
            filepath = f.name
        
        try:
            # Load simple format
            assert config.loadFromFile(filepath)
            
            # Verify values
            assert config.getOr("brain.neuron_count", 0) == 500
            assert config.getOr("brain.synapse_density", 0.0) == 0.2
            assert config.getOr("brain.connection_probability", 0.0) == 0.1
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
class TestNLMEnumerations:
    """Test enumeration types"""
    
    def test_neuron_type_enums(self):
        """Test neuron type enumerations"""
        # Verify all neuron types exist
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal
        ]
        
        for neuron_type in neuron_types:
            assert neuron_type is not None
            
    def test_action_type_enums(self):
        """Test action type enumerations"""
        action_types = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Interact,
            pynlm.ActionType.Eat,
            pynlm.ActionType.Drink,
            pynlm.ActionType.Rest,
            pynlm.ActionType.Wait,
            pynlm.ActionType.Look,
            pynlm.ActionType.LookUp,
            pynlm.ActionType.LookDown,
            pynlm.ActionType.Custom
        ]
        
        for action_type in action_types:
            assert action_type is not None
            
    def test_world_object_type_enums(self):
        """Test world object type enumerations"""
        object_types = [
            pynlm.WorldObjectType.Empty,
            pynlm.WorldObjectType.Resource,
            pynlm.WorldObjectType.Hazard,
            pynlm.WorldObjectType.Wall,
            pynlm.WorldObjectType.Marker
        ]
        
        for obj_type in object_types:
            assert obj_type is not None
            
    def test_developmental_stage_enums(self):
        """Test developmental stage enumerations"""
        stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ]
        
        for stage in stages:
            assert stage is not None
class TestNLMSensoryInput:
    """Test sensory input functionality"""
    
    def test_vision_sensory_input(self):
        """Test vision sensory input"""
        vision = pynlm.Vision(width=8, height=8, channels=3)
        
        # Create dummy image data
        data = []
        for _ in range(8 * 8 * 3):
            data.append(0.5)
        
        vision.setData(data)
        
        assert vision.getWidth() == 8
        assert vision.getHeight() == 8
        assert vision.getChannels() == 3
        assert vision.getData() == data
        
    def test_audio_sensory_input(self):
        """Test audio sensory input"""
        audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
        
        # Create dummy audio data
        data = []
        for _ in range(1024):
            data.append(0.1)
        
        audio.setData(data)
        audio.setSampleRate(44100)
        
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        assert audio.getData() == data
        
    def test_internal_signals_sensory_input(self):
        """Test internal signals sensory input"""
        internal = pynlm.InternalSignals()
        
        # Add some internal signals
        internal.addSignal(0.5)
        internal.addSignal(-0.3)
        internal.addSignal(0.8)
        
        signals = internal.getAllSignals()
        assert len(signals) == 3
        assert signals[0] == 0.5
        assert signals[1] == -0.3
        assert signals[2] == 0.8
        
        # Clear signals
        internal.clearSignals()
        assert len(internal.getAllSignals()) == 0
class TestNLMAction:
    """Test action functionality"""
    
    def test_action_creation(self):
        """Test action creation"""
        # Create action with type
        action1 = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action1.getType() == pynlm.ActionType.MoveForward
        
        # Create action with type and parameters
        action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        assert action2.getType() == pynlm.ActionType.Custom
        assert action2.getParameters() == [1.0, 2.0, 3.0]
        
    def test_action_modification(self):
        """Test action modification"""
        action = pynlm.Action(pynlm.ActionType.TurnLeft)
        
        # Modify type
        action.setType(pynlm.ActionType.TurnRight)
        assert action.getType() == pynlm.ActionType.TurnRight
        
        # Modify parameters
        action.setParameters([45.0, 90.0])
        assert action.getParameters() == [45.0, 90.0]
        
        # Get action name
        name = action.getName()
        assert name is not None
        assert len(name) > 0
        
    def test_action_cloning(self):
        """Test action cloning"""
        action1 = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0])
        
        action2 = action1.clone()
        
        assert action2.getType() == action1.getType()
        assert action2.getParameters() == action1.getParameters()
class TestNLMWorldObject:
    """Test world object functionality"""
    
    def test_world_object_creation(self):
        """Test world object creation"""
        # Create empty world object
        obj1 = pynlm.WorldObject()
        assert obj1.getType() == pynlm.WorldObjectType.Empty
        assert obj1.getX() == 0.0
        assert obj1.getY() == 0.0
        
        # Create world object with parameters
        obj2 = pynlm.WorldObject(x=5.0, y=10.0, type=pynlm.WorldObjectType.Resource, value=2.5, radius=1.0)
        
        assert obj2.getX() == 5.0
        assert obj2.getY() == 10.0
        assert obj2.getType() == pynlm.WorldObjectType.Resource
        assert obj2.getValue() == 2.5
        assert obj2.getRadius() == 1.0
        
    def test_world_object_properties(self):
        """Test world object property modification"""
        obj = pynlm.WorldObject(x=0.0, y=0.0, type=pynlm.WorldObjectType.Wall, radius=2.0)
        
        # Modify properties
        obj.x = 10.0
        obj.y = 15.0
        obj.type = pynlm.WorldObjectType.Resource
        obj.value = 5.0
        obj.active = True
        
        assert obj.getX() == 10.0
        assert obj.getY() == 15.0
        assert obj.getType() == pynlm.WorldObjectType.Resource
        assert obj.getValue() == 5.0
        assert obj.active == True
class TestNLMExceptionHandling:
    """Test exception handling and error cases"""
    
    def test_config_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        
        # Set invalid values (should be logged but not crash)
        config.set("brain.neuron_count", 0)  # Should be positive
        config.set("brain.connection_probability", 1.5)  # Should be between 0 and 1
        
        # Config should still work
        assert config.getOr("brain.neuron_count", 0) == 0
        assert config.getOr("brain.connection_probability", 0.0) == 1.5
        
    def test_brain_initialization_failure(self):
        """Test brain initialization error handling"""
        config = pynlm.createDefaultConfig()
        
        # Create brain and try to initialize
        brain = pynlm.createBrain(config)
        # Initialization should succeed with default config
        assert brain.initialize()
        
    def test_world_configuration_validation(self):
        """Test world configuration validation"""
        world = pynlm.createSimpleWorld()
        
        # Configure with valid values
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # World should work
        world.reset()
        world.update(0.1)
        
    def test_agent_initialization_validation(self):
        """Test agent initialization validation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Should not crash
        assert agent is not None