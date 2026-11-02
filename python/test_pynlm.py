#!/usr/bin/env python3
"""
Comprehensive test suite for pynlm Python bindings.
Tests all functionality of the NLM neural simulation framework.
"""

import sys
import os
import tempfile
import json
import numpy as np
import pytest
from pathlib import Path

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

# Import pynlm (will be built by CMake)
import pynlm

class TestNLMCore:
    """Test core NLM functionality."""
    
    def test_version_info(self):
        """Test version information is available."""
        assert hasattr(pynlm, '__version__')
        assert hasattr(pynlm, '__author__')
        assert hasattr(pynlm, '__description__')
        assert pynlm.__version__ == "0.1.0"
    
    def test_constants(self):
        """Test that constants are available."""
        assert hasattr(pynlm, 'INVALID_NEURON_ID')
        assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
        assert hasattr(pynlm, 'INVALID_REGION_ID')
        assert hasattr(pynlm, 'INVALID_POPULATION_ID')
        
        # Test constant values
        assert pynlm.INVALID_NEURON_ID.value == 0
        assert pynlm.INVALID_SYNAPSE_ID.value == 0
        assert pynlm.INVALID_REGION_ID.value == 0
        assert pynlm.INVALID_POPULATION_ID.value == 0
    
    def test_config_creation(self):
        """Test Config class creation and basic functionality."""
        config = pynlm.Config()
        
        # Test basic operations
        config.set("test_key", "test_value")
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_bool", True)
        
        assert config.has("test_key")
        assert config.has("test_int")
        assert config.has("test_float")
        assert config.has("test_bool")
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_config_file_operations(self):
        """Test Config file loading and saving."""
        config = pynlm.Config()
        config.set("key1", "value1")
        config.set("key2", 123)
        config.set("key3", 45.67)
        config.set("key4", False)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            temp_file = f.name
        
        try:
            # Load back
            config2 = pynlm.Config()
            config2.loadFromFile(temp_file)
            
            # Verify data
            assert config2.has("key1")
            assert config2.has("key2")
            assert config2.has("key3")
            assert config2.has("key4")
            
        finally:
            os.unlink(temp_file)
    
    def test_config_load_from_args(self):
        """Test Config loading from arguments."""
        config = pynlm.Config()
        
        # Test with various argument formats
        test_args = ["--neuron_count", "100", "--region_count", "2"]
        config.loadFromArgs(test_args)
        
        assert config.has("neuron_count")
        assert config.has("region_count")

class TestIDTypes:
    """Test ID type bindings."""
    
    def test_neuron_id(self):
        """Test NeuronId functionality."""
        # Test default constructor
        neuron_id1 = pynlm.NeuronId()
        assert neuron_id1.value == 0
        
        # Test constructor with value
        neuron_id2 = pynlm.NeuronId(42)
        assert neuron_id2.value == 42
        
        # Test equality
        assert neuron_id1.value == 0
        assert neuron_id2.value == 42
        assert neuron_id1 != neuron_id2
        
        # Test index
        assert neuron_id1.index() == 0
        assert neuron_id2.index() == 42
        
        # Test hashing and string representation
        assert hash(neuron_id1) == hash(0)
        assert hash(neuron_id2) == hash(42)
        
        assert str(neuron_id1) == "0"
        assert str(neuron_id2) == "42"
        assert repr(neuron_id1) == "<NeuronId: 0>"
        assert repr(neuron_id2) == "<NeuronId: 42>"
    
    def test_synapse_id(self):
        """Test SynapseId functionality."""
        synapse_id1 = pynlm.SynapseId()
        synapse_id2 = pynlm.SynapseId(123)
        
        assert synapse_id1.value == 0
        assert synapse_id2.value == 123
        assert synapse_id1 != synapse_id2
        assert synapse_id1.index() == 0
        assert synapse_id2.index() == 123
    
    def test_region_id(self):
        """Test RegionId functionality."""
        region_id1 = pynlm.RegionId()
        region_id2 = pynlm.RegionId(999)
        
        assert region_id1.value == 0
        assert region_id2.value == 999
        assert region_id1 != region_id2
        assert region_id1.index() == 0
        assert region_id2.index() == 999
    
    def test_population_id(self):
        """Test PopulationId functionality."""
        pop_id1 = pynlm.PopulationId()
        pop_id2 = pynlm.PopulationId(555)
        
        assert pop_id1.value == 0
        assert pop_id2.value == 555
        assert pop_id1 != pop_id2
        assert pop_id1.index() == 0
        assert pop_id2.index() == 555

class TestEnums:
    """Test enumeration bindings."""
    
    def test_neuron_type_enum(self):
        """Test NeuronType enum values."""
        # Test all enum values are accessible
        assert pynlm.NeuronType.Excitatory == pynlm.NeuronType.Excitatory
        assert pynlm.NeuronType.Inhibitory == pynlm.NeuronType.Inhibitory
        assert pynlm.NeuronType.Modulatory == pynlm.NeuronType.Modulatory
        assert pynlm.NeuronType.Sensory == pynlm.NeuronType.Sensory
        assert pynlm.NeuronType.Motor == pynlm.NeuronType.Motor
        assert pynlm.NeuronType.Internal == pynlm.NeuronType.Internal
    
    def test_synapse_type_enum(self):
        """Test SynapseType enum values."""
        assert pynlm.SynapseType.Excitatory == pynlm.SynapseType.Excitatory
        assert pynlm.SynapseType.Inhibitory == pynlm.SynapseType.Inhibitory
        assert pynlm.SynapseType.Modulatory == pynlm.SynapseType.Modulatory
        assert pynlm.SynapseType.Electrical == pynlm.SynapseType.Electrical
        assert pynlm.SynapseType.GapJunction == pynlm.SynapseType.GapJunction
    
    def test_developmental_stage_enum(self):
        """Test DevelopmentalStage enum values."""
        assert pynlm.DevelopmentalStage.Initial == pynlm.DevelopmentalStage.Initial
        assert pynlm.DevelopmentalStage.CriticalPeriod == pynlm.DevelopmentalStage.CriticalPeriod
        assert pynlm.DevelopmentalStage.Maturation == pynlm.DevelopmentalStage.Maturation
        assert pynlm.DevelopmentalStage.Adult == pynlm.DevelopmentalStage.Adult
        assert pynlm.DevelopmentalStage.Aging == pynlm.DevelopmentalStage.Aging
    
    def test_firing_state_enum(self):
        """Test FiringState enum values."""
        assert pynlm.FiringState.Resting == pynlm.FiringState.Resting
        assert pynlm.FiringState.Active == pynlm.FiringState.Active
        assert pynlm.FiringState.Refractory == pynlm.FiringState.Refractory
        assert pynlm.FiringState.Inhibited == pynlm.FiringState.Inhibited
    
    def test_action_type_enum(self):
        """Test ActionType enum values."""
        # Test some key action types
        assert pynlm.ActionType.MoveForward == pynlm.ActionType.MoveForward
        assert pynlm.ActionType.TurnLeft == pynlm.ActionType.TurnLeft
        assert pynlm.ActionType.Interact == pynlm.ActionType.Interact
        assert pynlm.ActionType.Eat == pynlm.ActionType.Eat
        assert pynlm.ActionType.Rest == pynlm.NeuronType.Rest
    
    def test_motor_command_enum(self):
        """Test MotorCommand enum values."""
        assert pynlm.MotorCommand.MoveForward == pynlm.MotorCommand.MoveForward
        assert pynlm.MotorCommand.TurnLeft == pynlm.MotorCommand.TurnLeft
        assert pynlm.MotorCommand.LookLeft == pynlm.MotorCommand.LookLeft
        assert pynlm.MotorCommand.Interact == pynlm.MotorCommand.Interact
    
    def test_world_object_type_enum(self):
        """Test WorldObjectType enum values."""
        assert pynlm.WorldObjectType.Empty == pynlm.WorldObjectType.Empty
        assert pynlm.WorldObjectType.Resource == pynlm.WorldObjectType.Resource
        assert pynlm.WorldObjectType.Hazard == pynlm.WorldObjectType.Hazard
        assert pynlm.WorldObjectType.Wall == pynlm.WorldObjectType.Wall
        assert pynlm.WorldObjectType.Marker == pynlm.WorldObjectType.Marker

class TestSensoryInput:
    """Test sensory input classes."""
    
    def test_vision(self):
        """Test Vision sensory input."""
        vision = pynlm.Vision(640, 480, 3)
        assert vision.getWidth() == 640
        assert vision.getHeight() == 480
        assert vision.getChannels() == 3
        assert vision.getDimensions() == 3
        
        # Test data setting
        test_data = [i for i in range(640 * 480 * 3)]
        vision.setData(test_data)
        retrieved_data = vision.getData()
        assert retrieved_data == test_data
    
    def test_audio(self):
        """Test Audio sensory input."""
        audio = pynlm.Audio(44100, 1024)
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        assert audio.getDimensions() == 2
        
        # Test data setting
        test_data = [i * 0.1 for i in range(1024)]
        audio.setData(test_data)
        retrieved_data = audio.getData()
        assert retrieved_data == test_data
    
    def test_internal_signals(self):
        """Test InternalSignals sensory input."""
        internal = pynlm.InternalSignals()
        assert internal.getDimensions() == 1
        
        # Test signal management
        internal.addSignal(1.0)
        internal.addSignal(2.0)
        internal.addSignal(3.0)
        
        signals = internal.getData()
        assert len(signals) == 3
        assert signals[0] == 1.0
        assert signals[1] == 2.0
        assert signals[2] == 3.0
        
        internal.clearSignals()
        assert len(internal.getData()) == 0

class TestAction:
    """Test Action class."""
    
    def test_action_creation(self):
        """Test Action creation and properties."""
        # Test default constructor
        action1 = pynlm.Action()
        assert action1.getType() is None
        
        # Test constructor with type
        action2 = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action2.getType() == pynlm.ActionType.MoveForward
        
        # Test constructor with type and parameters
        action3 = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0, 3.0])
        assert action3.getType() == pynlm.ActionType.MoveForward
        assert action3.getParameters() == [1.0, 2.0, 3.0]
    
    def test_action_properties(self):
        """Test Action property setters and getters."""
        action = pynlm.Action(pynlm.ActionType.TurnLeft, [0.5])
        
        # Test setting properties
        action.setType(pynlm.ActionType.MoveForward)
        assert action.getType() == pynlm.ActionType.MoveForward
        
        action.setParameters([1.0, 2.0, 3.0])
        assert action.getParameters() == [1.0, 2.0, 3.0]
        
        # Test name getter
        assert action.getName() == "MoveForward"
    
    def test_action_clone(self):
        """Test Action cloning."""
        original = pynlm.Action(pynlm.ActionType.Eat, [1.5, 2.5])
        cloned = original.clone()
        
        assert cloned.getType() == original.getType()
        assert cloned.getParameters() == original.getParameters()
        assert cloned is not original  # Different objects
    
    def test_action_string_representations(self):
        """Test Action string representations."""
        action = pynlm.Action(pynlm.ActionType.TurnRight, [0.1])
        
        assert "Action type=" in str(action)
        assert "Action type=" in repr(action)
        assert "TurnRight" in action.getName()

class TestWorldObject:
    """Test WorldObject class."""
    
    def test_world_object_creation(self):
        """Test WorldObject creation."""
        # Test default constructor
        obj1 = pynlm.WorldObject()
        assert obj1.x == 0.0
        assert obj1.y == 0.0
        assert obj1.radius == 0.5
        assert obj1.type == pynlm.WorldObjectType.Empty
        assert obj1.active == True
        
        # Test constructor with parameters
        obj2 = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 5.0, 2.0)
        assert obj2.x == 10.0
        assert obj2.y == 20.0
        assert obj2.radius == 2.0
        assert obj2.type == pynlm.WorldObjectType.Resource
        assert obj2.value == 5.0
    
    def test_world_object_properties(self):
        """Test WorldObject property access."""
        obj = pynlm.WorldObject()
        obj.x = 15.0
        obj.y = 25.0
        obj.radius = 3.0
        obj.type = pynlm.WorldObjectType.Wall
        obj.value = 10.0
        obj.active = False
        
        assert obj.x == 15.0
        assert obj.y == 25.0
        assert obj.radius == 3.0
        assert obj.type == pynlm.WorldObjectType.Wall
        assert obj.value == 10.0
        assert obj.active == False
    
    def test_world_object_distance(self):
        """Test WorldObject distance calculation."""
        obj1 = pynlm.WorldObject(0.0, 0.0)
        obj2 = pynlm.WorldObject(3.0, 4.0)
        
        distance = obj1.distanceTo(obj2)
        assert abs(distance - 5.0) < 0.001  # 3-4-5 triangle

class TestAgentBody:
    """Test AgentBody class."""
    
    def test_agent_body_creation(self):
        """Test AgentBody creation."""
        body = pynlm.AgentBody()
        assert body.x == 0.0
        assert body.y == 0.0
        assert body.orientation == 0.0
        assert body.velocityX == 0.0
        assert body.velocityY == 0.0
        assert body.angularVelocity == 0.0
        assert body.energy == 0.0
        assert body.health == 0.0
        assert body.age == 0.0
        assert body.isMoving == False
        assert body.isTurning == False
        assert body.isMoving == False
        assert body.lastActionTime == 0.0
    
    def test_agent_body_properties(self):
        """Test AgentBody property access."""
        body = pynlm.AgentBody()
        body.x = 10.0
        body.y = 20.0
        body.orientation = 90.0
        body.velocityX = 2.0
        body.velocityY = 3.0
        body.angularVelocity = 1.0
        body.energy = 100.0
        body.health = 95.0
        body.age = 5
        body.isMoving = True
        body.isTurning = True
        body.lastActionTime = 1.0
        
        assert body.x == 10.0
        assert body.y == 20.0
        assert body.orientation == 90.0
        assert body.velocityX == 2.0
        assert body.velocityY == 3.0
        assert body.angularVelocity == 1.0
        assert body.energy == 100.0
        assert body.health == 95.0
        assert body.age == 5
        assert body.isMoving == True
        assert body.isTurning == True
        assert body.lastActionTime == 1.0
    
    def test_agent_body_distance(self):
        """Test AgentBody distance calculation."""
        body = pynlm.AgentBody()
        body.x = 10.0
        body.y = 10.0
        
        distance = body.distanceTo(20.0, 20.0)
        expected = (10.0**2 + 10.0**2)**0.5
        assert abs(distance - expected) < 0.001
    
    def test_agent_body_reset(self):
        """Test AgentBody reset."""
        body = pynlm.AgentBody()
        body.x = 10.0
        body.y = 20.0
        body.energy = 50.0
        body.health = 80.0
        body.isMoving = True
        
        body.reset()
        
        assert body.x == 0.0
        assert body.y == 0.0
        assert body.energy == 0.0
        assert body.health == 0.0
        assert body.isMoving == False

class TestActionResult:
    """Test ActionResult class."""
    
    def test_action_result_creation(self):
        """Test ActionResult creation."""
        # Test default constructor
        result1 = pynlm.ActionResult()
        assert result1.reward == 0.0
        assert result1.success == False
        assert result1.message == ""
        
        # Test constructor with parameters
        result2 = pynlm.ActionResult(5.0, True, "Success!")
        assert result2.reward == 5.0
        assert result2.success == True
        assert result2.message == "Success!"
    
    def test_action_result_properties(self):
        """Test ActionResult property access."""
        result = pynlm.ActionResult(2.5, False, "Failed")
        
        result.reward = 10.0
        result.success = True
        result.message = "Done"
        
        assert result.reward == 10.0
        assert result.success == True
        assert result.message == "Done"
    
    def test_action_result_string_representations(self):
        """Test ActionResult string representations."""
        success_result = pynlm.ActionResult(5.0, True, "Success!")
        failure_result = pynlm.ActionResult(-2.0, False, "Failed")
        
        # Test that string representations contain expected information
        success_str = str(success_result)
        failure_str = str(failure_result)
        
        assert "reward=" in success_str
        assert "reward=" in failure_str
        assert "SUCCESS" in success_str
        assert "FAILURE" in failure_str

class TestSimpleWorld:
    """Test SimpleWorld class."""
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        world = pynlm.SimpleWorld()
        
        # Test initial configuration
        assert world.getWidth() == 0
        assert world.getHeight() == 0
        assert world.getMaxEnergy() == 0.0
        assert world.getEnergyDecayRate() == 0.0
        assert world.getSimulationTime() == 0.0
        assert world.getRandomSeed() == 0
        
        # Configure the world
        world.configure(100, 100, 32, 32)
        
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        assert world.getMaxEnergy() == 100.0  # Default is width * height
        assert world.getSimulationTime() == 0.0
    
    def test_world_configuration(self):
        """Test world configuration methods."""
        world = pynlm.SimpleWorld()
        world.configure(50, 75, 16, 16)
        
        world.setMaxEnergy(200.0)
        world.setEnergyDecayRate(0.1)
        world.setRandomSeed(42)
        
        assert world.getMaxEnergy() == 200.0
        assert world.getEnergyDecayRate() == 0.1
        assert world.getRandomSeed() == 42
    
    def test_world_agent_operations(self):
        """Test world agent operations."""
        world = pynlm.SimpleWorld()
        world.configure(100, 100, 32, 32)
        world.setAgentStart(10, 20)
        
        body = world.getAgentBody()
        assert body.x == 10.0
        assert body.y == 20.0
        
        # Set agent properties
        body.x = 15.0
        body.y = 25.0
        body.energy = 50.0
        body.isMoving = True
        
        # Verify properties are updated in world
        assert world.getAgentBody().x == 15.0
        assert world.getAgentBody().y == 25.0
    
    def test_world_object_operations(self):
        """Test world object operations."""
        world = pynlm.SimpleWorld()
        world.configure(100, 100, 32, 32)
        
        # Create a world object
        obj = pynlm.WorldObject(30.0, 40.0, pynlm.WorldObjectType.Resource, 5.0, 2.0)
        
        # Add object to world
        world.addObject(obj)
        
        # Verify object can be retrieved
        # Note: SimpleWorld doesn't have a getObject method, but we can test isValidPosition
        assert world.isValidPosition(30.0, 40.0)
        
        # Test position validation
        assert world.isValidPosition(0.0, 0.0)
        assert world.isValidPosition(99.0, 99.0)
        assert not world.isValidPosition(-1.0, 0.0)
        assert not world.isValidPosition(0.0, 100.0)
    
    def test_world_update(self):
        """Test world update method."""
        world = pynlm.SimpleWorld()
        world.configure(100, 100, 32, 32)
        
        initial_time = world.getSimulationTime()
        
        # Update world
        world.update(0.1)
        
        assert world.getSimulationTime() == initial_time + 0.1

class TestAgentBrain:
    """Test AgentBrain class."""
    
    def test_agent_brain_creation(self):
        """Test AgentBrain creation and initialization."""
        # Create components
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Initialize
        agent_brain.initialize(world)
        
        # Test basic properties
        assert agent_brain.getSensoryInputSize() > 0
        assert agent_brain.getMotorOutputSize() > 0
        
        # Test developmental stage
        assert agent_brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Initial
    
    def test_agent_brain_sensory_processing(self):
        """Test agent brain sensory processing."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # The action should be produced (though it may be a default action)
        action = brain.produceAction()
        assert action is not None
    
    def test_agent_brain_motor_decoding(self):
        """Test agent brain motor decoding."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Get current brain activity
        brain_before = brain.getActiveNeuronCount()
        
        # Process some input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Decode motor command
        action = agent_brain.decodeMotorCommand()
        assert action is not None
        assert action.getType() is not None
        
        # Get brain activity after processing
        brain_after = brain.getActiveNeuronCount()
        
        # Activity may change, but we're testing the interface works
        assert brain_before >= 0
        assert brain_after >= 0
    
    def test_agent_brain_neuromodulation(self):
        """Test agent brain neuromodulation features."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Test neuromodulation levels
        neuromod_level = agent_brain.getNeuromodulationLevel()
        curiosity_level = agent_brain.getCuriosityLevel()
        novelty_level = agent_brain.getNoveltyLevel()
        prediction_error = agent_brain.getPredictionError()
        
        assert neuromod_level >= 0
        assert curiosity_level >= 0
        assert novelty_level >= 0
        assert prediction_error >= 0
        
        # Test enable/disable features
        agent_brain.enableRewardModulation(True)
        assert agent_brain.isRewardModulationEnabled()
        
        agent_brain.enableStructuralPlasticity(True)
        assert agent_brain.isStructuralPlasticityEnabled()
        
        agent_brain.enableDevelopment(True)
        assert agent_brain.isDevelopmentEnabled()
        
        agent_brain.enableCuriosity(True)
        assert agent_brain.isCuriosityEnabled()
    
    def test_agent_brain_development(self):
        """Test agent brain development system."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Get initial developmental stage
        initial_stage = agent_brain.getDevelopmentalStage()
        
        # Update development
        for timestep in range(10):
            agent_brain.updateDevelopment(timestep)
        
        # Get stage after updates
        final_stage = agent_brain.getDevelopmentalStage()
        
        # Stage may or may not change depending on implementation
        assert final_stage in [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ]
    
    def test_agent_brain_reset(self):
        """Test agent brain reset."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Do some work to change state
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        agent_brain.updateDevelopment(0)
        
        # Reset
        agent_brain.reset()
        
        # State should be reset
        assert agent_brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Initial
        assert agent_brain.getNeuromodulationLevel() >= 0

class TestBrain:
    """Test Brain class."""
    
    def test_brain_creation(self):
        """Test brain creation and basic functionality."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        # Initialize
        assert brain.initialize()
        
        # Test basic properties
        assert brain.getRegionCount() == 0
        assert brain.getTotalNeuronCount() == 0
        assert brain.getTotalSynapseCount() == 0
        assert brain.getActiveNeuronCount() == 0
        assert brain.getFiringNeuronCount() == 0
        assert brain.getAverageFiringRate() == 0.0
        assert brain.getExcitationInhibitionRatio() == 1.0
        assert brain.getTotalSpikeCount() == 0
        
        # Test developmental stage
        assert brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Initial
    
    def test_brain_with_config(self):
        """Test brain creation with non-default config."""
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        
        brain = pynlm.createBrain(config)
        assert brain.initialize()
        
        # Check that config was used
        brain_config = brain.getConfig()
        assert brain_config.has("neuron_count")
        assert brain_config.has("region_count")
    
    def test_brain_step_methods(self):
        """Test different brain step methods."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test step with currentStep only
        brain.step(0)
        
        # Test step with currentStep and currentTime
        brain.step(1, 0.1)
        
        # Test with float timestamp
        brain.step(2, 0.2)
    
    def test_brain_state_management(self):
        """Test brain state management (save/load)."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial spike count
        initial_spikes = brain.getTotalSpikeCount()
        
        # Do some work
        brain.step(0)
        brain.step(1, 0.1)
        
        # Get spike count after work
        final_spikes = brain.getTotalSpikeCount()
        
        # Reset brain
        brain.reset()
        
        # Spike count should be back to initial
        reset_spikes = brain.getTotalSpikeCount()
        assert reset_spikes == initial_spikes
    
    def test_brain_region_management(self):
        """Test brain region management."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Add regions
        region1 = brain.addRegion("Region1")
        region2 = brain.addRegion("Region2")
        
        # Check region count
        assert brain.getRegionCount() == 2
        
        # Get region IDs
        region_ids = brain.getRegionIds()
        assert len(region_ids) == 2
        
        # Get regions
        regions = brain.getRegions()
        assert len(regions) == 2
    
    def test_brain_constants(self):
        """Test that brain constants are accessible."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test invalid IDs
        assert pynlm.INVALID_NEURON_ID.value == 0
        assert pynlm.INVALID_SYNAPSE_ID.value == 0
        assert pynlm.INVALID_REGION_ID.value == 0
        assert pynlm.INVALID_POPULATION_ID.value == 0

class TestFactoryFunctions:
    """Test factory functions and utilities."""
    
    def test_default_config_creation(self):
        """Test default config factory function."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert isinstance(config, pynlm.Config)
        
        # Should have default settings
        config.set("neuron_count", 100)
        assert config.has("neuron_count")
    
    def test_brain_creation_factory(self):
        """Test brain creation factory function."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        assert isinstance(brain, pynlm.Brain)
        assert brain.initialize()
    
    def test_world_creation_factory(self):
        """Test world creation factory function."""
        world = pynlm.createSimpleWorld()
        
        assert world is not None
        assert isinstance(world, pynlm.SimpleWorld)
        
        # Configure world
        world.configure(100, 100, 32, 32)
        assert world.getWidth() == 100
        assert world.getHeight() == 100
    
    def test_agent_brain_creation_factory(self):
        """Test agent brain creation factory function."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        agent_brain = pynlm.createAgentBrain(brain)
        
        assert agent_brain is not None
        assert isinstance(agent_brain, pynlm.AgentBrain)
    
    def test_complete_system_factory(self):
        """Test complete NLM system factory function."""
        brain, world, agent_brain = pynlm.create_nlm_system()
        
        assert brain is not None
        assert world is not None
        assert agent_brain is not None
        
        assert isinstance(brain, pynlm.Brain)
        assert isinstance(world, pynlm.SimpleWorld)
        assert isinstance(agent_brain, pynlm.AgentBrain)
        
        # Initialize agent brain with world
        agent_brain.initialize(world)
        
        # System should be functional
        assert brain.initialize()
        assert agent_brain.getSensoryInputSize() > 0
        assert agent_brain.getMotorOutputSize() > 0

class TestSystemUtilities:
    """Test system utilities and integration functions."""
    
    def test_get_system_stats(self):
        """Test system statistics function."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        stats = pynlm.get_system_stats(brain)
        
        assert isinstance(stats, dict)
        assert "neuron_count" in stats
        assert "synapse_count" in stats
        assert "active_neurons" in stats
        assert "firing_neurons" in stats
        assert "avg_firing_rate" in stats
        assert "excitement_inhibition_ratio" in stats
        assert "total_spikes" in stats
        assert "region_count" in stats
        
        # Check value types
        assert isinstance(stats["neuron_count"], int)
        assert isinstance(stats["synapse_count"], int)
        assert isinstance(stats["active_neurons"], int)
        assert isinstance(stats["firing_neurons"], int)
        assert isinstance(stats["avg_firing_rate"], float)
        assert isinstance(stats["excitement_inhibition_ratio"], float)
        assert isinstance(stats["total_spikes"], int)
        assert isinstance(stats["region_count"], int)
    
    def test_safe_step(self):
        """Test safe step function with error handling."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Safe step should work
        assert pynlm.safe_step(brain, 0, 0.1)
        assert pynlm.safe_step(brain, 1, 0.2)
        
        # Test that invalid brain raises exception
        with pytest.raises(pynlm.NLMException):
            bad_brain = None
            pynlm.safe_step(bad_brain, 0, 0.1)
    
    def test_batch_process_sensory(self):
        """Test batch sensory processing."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create multiple percepts
        percepts = []
        for i in range(5):
            # In a real scenario, these would be actual sensory inputs
            # For testing, we'll create empty percepts
            percepts.append(pynlm.SensoryPercept())
        
        # Batch process
        actions = pynlm.batch_process_sensory(brain, percepts, 0.1)
        
        assert isinstance(actions, list)
        assert len(actions) == 5
        
        # Each action should be an Action object
        for action in actions:
            assert isinstance(action, pynlm.Action)
            assert action.getType() is not None
    
    def test_run_simulation(self):
        """Test complete simulation run utility."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(100, 100, 32, 32)
        world.setAgentStart(50, 50)
        
        # Run simulation
        brain, world, agent_brain = pynlm.create_nlm_system()
        agent_brain.initialize(world)
        
        body = pynlm.run_simulation(brain, world, 10, 0.1)
        
        assert body is not None
        assert isinstance(body, pynlm.AgentBody)
        
        # Agent should have moved (position may be changed)
        assert body.x >= 0 and body.x <= 100
        assert body.y >= 0 and body.y <= 100

class TestIntegration:
    """Test integration and complex scenarios."""
    
    def test_complete_brain_world_cycle(self):
        """Test complete brain-world interaction cycle."""
        # Create complete system
        brain, world, agent_brain = pynlm.create_nlm_system()
        agent_brain.initialize(world)
        
        # Run simulation for multiple steps
        for step in range(5):
            # Get current sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Update development
            agent_brain.updateDevelopment(step)
            
            # Get motor command
            action = agent_brain.decodeMotorCommand()
            
            # Apply to world
            world.applyMotorCommand(action, step * 0.1)
            
            # Step brain
            brain.step(step, step * 0.1)
            
            # Check that system is still functional
            assert brain.getActiveNeuronCount() >= 0
            assert agent_brain.getNeuromodulationLevel() >= 0
        
        # Verify world state
        body = world.getAgentBody()
        assert body.x >= 0 and body.x <= 100
        assert body.y >= 0 and body.y <= 100
        assert body.energy >= 0 and body.energy <= 100
    
    def test_configuration_persistence(self):
        """Test configuration persistence through save/load."""
        # Create and configure brain
        config = pynlm.Config()
        config.set("neuron_count", 150)
        config.set("region_count", 3)
        config.set("connection_probability", 0.05)
        
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Get initial stats
        stats1 = pynlm.get_system_stats(brain1)
        
        # Save brain state
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            brain1.save(f.name)
            temp_file = f.name
        
        try:
            # Create new brain and load saved state
            config2 = pynlm.Config()
            brain2 = pynlm.createBrain(config2)
            brain2.load(temp_file)
            
            # Get stats from loaded brain
            stats2 = pynlm.get_system_stats(brain2)
            
            # Stats should be similar (though not identical due to random initialization)
            assert stats2["neuron_count"] == stats1["neuron_count"]
            assert stats2["region_count"] == stats1["region_count"]
            
        finally:
            os.unlink(temp_file)
    
    def test_multi_agent_scenario(self):
        """Test multi-agent scenario with shared world."""
        # Create shared world
        world = pynlm.createSimpleWorld()
        world.configure(200, 200, 64, 64)
        world.setAgentStart(100, 100)
        
        # Create multiple agent brains sharing the same brain instance
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        agent1 = pynlm.createAgentBrain(brain)
        agent2 = pynlm.createAgentBrain(brain)  # Same brain, different agent
        
        agent1.initialize(world)
        agent2.initialize(world)
        
        # Each agent processes sensory input independently
        percept1 = world.getSensoryPercept()
        percept2 = world.getSensoryPercept()
        
        agent1.processSensoryInput(percept1)
        agent2.processSensoryInput(percept2)
        
        # Both agents should have produced actions
        action1 = agent1.decodeMotorCommand()
        action2 = agent2.decodeMotorCommand()
        
        assert action1 is not None
        assert action2 is not None
        
        # Apply different actions to world
        world.applyMotorCommand(action1, 0.0)
        # agent2's action would be applied next

class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_null_brain_handling(self):
        """Test handling of null/invalid brain references."""
        # Test with None brain (should raise exception)
        with pytest.raises(pynlm.NLMException):
            pynlm.get_system_stats(None)
    
    def test_invalid_config_operations(self):
        """Test handling of invalid configuration operations."""
        config = pynlm.Config()
        
        # These operations should work
        config.set("valid_key", "valid_value")
        assert config.has("valid_key")
        
        # Check that non-existent keys return default values
        # (implementation dependent)
    
    def test_sensor_data_validation(self):
        """Test sensor data validation."""
        vision = pynlm.Vision(10, 10, 3)
        
        # Valid data should work
        valid_data = [i for i in range(10 * 10 * 3)]
        vision.setData(valid_data)
        assert vision.getData() == valid_data
        
        # Invalid data length should be handled gracefully
        # (depends on implementation)
    
    def test_action_validation(self):
        """Test action validation."""
        # Valid actions should work
        action1 = pynlm.Action(pynlm.ActionType.MoveForward)
        action2 = pynlm.Action(pynlm.ActionType.TurnLeft, [0.5])
        
        assert action1.getType() == pynlm.ActionType.MoveForward
        assert action2.getType() == pynlm.ActionType.TurnLeft
        
        # Invalid action types (if any) should be handled
        # (depends on implementation)
    
    def test_world_boundary_handling(self):
        """Test world boundary handling."""
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        
        # Valid positions should work
        assert world.isValidPosition(0, 0)
        assert world.isValidPosition(99, 99)
        
        # Invalid positions should be rejected
        assert not world.isValidPosition(-1, 0)
        assert not world.isValidPosition(0, -1)
        assert not world.isValidPosition(100, 0)
        assert not world.isValidPosition(0, 100)
        assert not world.isValidPosition(200, 200)

class TestPerformance:
    """Test performance-related functionality."""
    
    def test_large_config(self):
        """Test with large configuration."""
        config = pynlm.Config()
        
        # Set many configuration values
        for i in range(100):
            config.set(f"key_{i}", i * 10)
        
        assert config.getKeys()  # Should not crash
        
        # Verify all keys are accessible
        for i in range(100):
            assert config.has(f"key_{i}")
    
    def test_batch_operations(self):
        """Test batch operations performance."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create multiple percepts
        percepts = [pynlm.SensoryPercept() for _ in range(10)]
        
        # Batch process
        actions = pynlm.batch_process_sensory(brain, percepts, 0.01)
        
        assert len(actions) == 10
        
        # Each action should be valid
        for action in actions:
            assert isinstance(action, pynlm.Action)
    
    def test_memory_efficiency(self):
        """Test memory efficiency with object cloning."""
        original = pynlm.Action(pynlm.ActionType.Eat, [1.0, 2.0, 3.0])
        
        # Clone many times
        clones = []
        for i in range(100):
            clone = original.clone()
            clones.append(clone)
        
        # All clones should have the same properties
        for clone in clones:
            assert clone.getType() == original.getType()
            assert clone.getParameters() == original.getParameters()
        
        # Clean up
        del clones

def run_all_tests():
    """Run all tests and report results."""
    print("Running comprehensive pynlm Python bindings tests...")
    print("=" * 60)
    
    # Create test runner
    import unittest
    
    # Load all test cases
    loader = unittest.TestLoader()
    
    # Run specific test suites
    suites = [
        ('Core', TestNLMCore),
        ('ID Types', TestIDTypes),
        ('Enums', TestEnums),
        ('Sensory Input', TestSensoryInput),
        ('Action', TestAction),
        ('WorldObject', TestWorldObject),
        ('AgentBody', TestAgentBody),
        ('ActionResult', TestActionResult),
        ('SimpleWorld', TestSimpleWorld),
        ('AgentBrain', TestAgentBrain),
        ('Brain', TestBrain),
        ('Factory Functions', TestFactoryFunctions),
        ('System Utilities', TestSystemUtilities),
        ('Integration', TestIntegration),
        ('Error Handling', TestErrorHandling),
        ('Performance', TestPerformance),
    ]
    
    total_tests = 0
    failed_tests = 0
    
    for suite_name, test_class in suites:
        print(f"\n{suite_name}:")
        print("-" * 40)
        
        # Create test suite
        suite = loader.loadTestsFromTestCase(test_class)
        
        # Run tests with custom runner
        runner = unittest.TextTestRunner(verbosity=0, stream=open(os.devnull, 'w'))
        result = runner.run(suite)
        
        total_tests += result.testsRun
        failed_tests += len(result.failures) + len(result.errors)
        
        if result.failures:
            print(f"  FAILED: {len(result.failures)} tests")
            for test, trace in result.failures:
                print(f"    {test}: {trace.split(chr(10))[-1]}")
        
        if result.errors:
            print(f"  ERROR: {len(result.errors)} tests")
            for test, trace in result.errors:
                print(f"    {test}: {trace.split(chr(10))[-1]}")
    
    print("\n" + "=" * 60)
    print(f"Test Summary:")
    print(f"  Total tests: {total_tests}")
    print(f"  Passed: {total_tests - failed_tests}")
    print(f"  Failed: {failed_tests}")
    print(f"  Success rate: {(total_tests - failed_tests) / total_tests * 100:.1f}%")
    
    if failed_tests > 0:
        print("\nSome tests failed. Please check the output above for details.")
        return False
    else:
        print("\nAll tests passed successfully!")
        return True

if __name__ == "__main__":
    # Check if pynlm module is available
    try:
        import pynlm
        print("pynlm module loaded successfully")
        
        # Run tests
        success = run_all_tests()
        sys.exit(0 if success else 1)
        
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Please build the pynlm module first using CMake:")
        print("  mkdir build && cd build && cmake .. && make")
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)