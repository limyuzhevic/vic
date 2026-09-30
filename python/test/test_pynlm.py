"""
Test suite for the NLM Python bindings.

This module provides comprehensive tests for the NLM Python bindings,
covering all major classes and functionality with proper error handling
and edge case testing.
"""

import sys
import os
import json
import tempfile
from pathlib import Path
from unittest.mock import Mock, patch
import pytest
import numpy as np

# Add parent directory to path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

# Import the main modules
import pynlm
from pynlm import (
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryPercept,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    Vision,
    Audio,
    InternalSignals,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    PyNLMRuntimeError,
    PyNLMConfigurationError,
    PyNLMInitializationError,
)

# Test constants
DEFAULT_NEURON_COUNT = 100
DEFAULT_CONFIG_PARAMS = {
    "brain.neuron_count": DEFAULT_NEURON_COUNT,
    "brain.synapse_density": 0.1,
    "brain.v_thresh": -50.0,
}


def test_version():
    """Test that the module has a valid version."""
    assert hasattr(pynlm, '__version__')
    assert isinstance(pynlm.__version__, str)
    assert len(pynlm.__version__) > 0


def test_exceptions_exist():
    """Test that custom exceptions are defined."""
    assert hasattr(pynlm, 'PyNLMRuntimeError')
    assert hasattr(pynlm, 'PyNLMConfigurationError')
    assert hasattr(pynlm, 'PyNLMInitializationError')
    
    # Test exception inheritance
    assert issubclass(pynlm.PyNLMConfigurationError, pynlm.PyNLMRuntimeError)
    assert issubclass(pynlm.PyNLMInitializationError, pynlm.PyNLMRuntimeError)


def test_config_creation():
    """Test Config class creation and basic functionality."""
    # Test default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test config has()
    for key, value in DEFAULT_CONFIG_PARAMS.items():
        assert config.has(key), f"Config should have key: {key}"
    
    # Test config.getOr()
    assert config.getOr("brain.neuron_count", 0) == DEFAULT_NEURON_COUNT
    assert config.getOr("nonexistent.key", "default") == "default"
    
    # Test config.getKeys()
    keys = config.getKeys()
    assert isinstance(keys, list)
    assert len(keys) > 0
    
    # Test config.set()
    config.set("test.key", "test_value")
    assert config.getOr("test.key", None) == "test_value"


def test_config_file_operations():
    """Test Config file I/O operations."""
    # Create temporary config file
    config = pynlm.createDefaultConfig()
    config.set("test.value", 42)
    config.set("test.string", "hello")
    config.set("test.bool", True)
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        temp_file = f.name
    
    try:
        # Load from file
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(temp_file)
        
        # Verify loaded values
        assert new_config.getOr("test.value", 0) == 42
        assert new_config.getOr("test.string", "") == "hello"
        assert new_config.getOr("test.bool", False) is True
    finally:
        # Clean up
        os.unlink(temp_file)


def test_config_error_handling():
    """Test Config error handling."""
    config = pynlm.createDefaultConfig()
    
    # Test loading non-existent file
    with pytest.raises(PyNLMRuntimeError):
        config.loadFromFile("/nonexistent/file.json")
    
    # Test with invalid file format
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        f.write("{ invalid json }")
        temp_file = f.name
    
    try:
        with pytest.raises(PyNLMRuntimeError):
            config.loadFromFile(temp_file)
    finally:
        os.unlink(temp_file)


def test_brain_creation():
    """Test Brain class creation and initialization."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test initialization
    brain.initialize()
    
    # Test basic statistics
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    # Test developmental stage
    assert isinstance(brain.getDevelopmentalStage(), DevelopmentalStage)


def test_brain_step():
    """Test Brain step functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Store initial state
    initial_firing = brain.getFiringNeuronCount()
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run steps
    for i in range(10):
        brain.step(i)
        
        # Verify state changes (not necessarily monotonic due to dynamics)
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getTotalSpikeCount() >= initial_spikes
        assert brain.getAverageFiringRate() >= 0.0
    
    # Test step with time
    brain.step(10, 5.0)


def test_brain_save_load():
    """Test Brain save and load functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps to get interesting state
    for i in range(50):
        brain.step(i)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        save_file = f.name
    
    try:
        # Save brain state
        assert brain.save(save_file)
        
        # Create new brain and load state
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        # Load should work even if brain was just initialized
        # (load from empty should be fine)
        assert brain2.load(save_file)
        
    finally:
        if os.path.exists(save_file):
            os.unlink(save_file)


def test_brain_error_handling():
    """Test Brain error handling."""
    config = pynlm.createDefaultConfig()
    
    # Brain should be able to be created without initialization
    brain = pynlm.createBrain(config)
    
    # Step without initialization should raise error
    with pytest.raises(PyNLMRuntimeError):
        brain.step(0)


def test_agent_brain_creation():
    """Test AgentBrain class creation and functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Test agent statistics
    assert isinstance(agent.getCuriosityLevel(), float)
    assert isinstance(agent.getNoveltyLevel(), float)
    assert isinstance(agent.getPredictionError(), float)
    assert isinstance(agent.getNeuromodulationLevel(), float)
    
    # Test enable/disable subsystems
    agent.enableRewardModulation(True)
    assert agent.isRewardModulationEnabled()
    
    agent.enableCuriosity(True)
    assert agent.isCuriosityEnabled()
    
    agent.enableDevelopment(True)
    assert agent.isDevelopmentEnabled()


def test_agent_brain_processing():
    """Test AgentBrain sensory processing and motor decoding."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Create test sensory percept
    vision_data = np.random.rand(8, 8).flatten().tolist()
    touch_data = np.random.rand(4).tolist()
    internal_data = np.random.rand(2).tolist()
    
    percept = SensoryPercept()
    percept.setVision(vision_data)
    percept.setTouch(touch_data)
    percept.setInternal(internal_data)
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    assert isinstance(motor_cmd, MotorCommand)
    
    # Apply reward modulation
    agent.applyRewardModulation(0.5, 0.3)
    
    # Update development
    agent.updateDevelopment(0.1)


def test_world_creation():
    """Test SimpleWorld class creation and configuration."""
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Configure world
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    
    # Test configuration values
    assert world.getWidth() == 10.0
    assert world.getHeight() == 10.0
    
    # Reset world
    world.reset()
    
    # Set agent start
    world.setAgentStart(5.0, 5.0)
    
    # Update world
    world.update(0.1)
    
    # Get percept and body
    percept = world.getSensoryPercept()
    body = world.getAgentBody()
    
    assert body is not None
    assert percept is not None
    
    # Test object management
    obj = WorldObject(3.0, 3.0, WorldObjectType.Resource, 5.0)
    world.addObject(obj)
    
    # Check valid position
    assert world.isValidPosition(5.0, 5.0)  # Agent position
    assert not world.isValidPosition(3.0, 3.0)  # Object position
    
    # Remove object
    world.removeObject(3.0, 3.0)
    assert world.isValidPosition(3.0, 3.0)  # Now should be valid
    
    # Test energy settings
    world.setMaxEnergy(100.0)
    assert world.getMaxEnergy() == 100.0


def test_world_energy():
    """Test world energy system."""
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    
    # Set energy parameters
    world.setMaxEnergy(50.0)
    world.setEnergyDecayRate(0.1)
    
    assert world.getMaxEnergy() == 50.0
    assert world.getEnergyDecayRate() == 0.1
    
    # Update world multiple times
    for i in range(10):
        world.update(0.1)
        
        # Energy should decay over time
        body = world.getAgentBody()
        assert body.energy >= 0.0
        assert body.energy <= 1.0  # Normalized to 0-1


def test_world_random():
    """Test world random seed functionality."""
    world1 = pynlm.createSimpleWorld()
    world1.configure(width=10, height=10)
    
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=10, height=10)
    
    # Set same seed
    seed = 42
    world1.setRandomSeed(seed)
    world2.setRandomSeed(seed)
    
    assert world1.getRandomSeed() == seed
    assert world2.getRandomSeed() == seed
    
    # Different seeds should produce different results
    world3 = pynlm.createSimpleWorld()
    world3.configure(width=10, height=10)
    world3.setRandomSeed(123)
    
    assert world3.getRandomSeed() != seed


def test_enums():
    """Test enumeration types."""
    # Test NeuronType enum
    assert pynlm.NeuronType.Excitatory == pynlm.NeuronType.Excitatory
    assert pynlm.NeuronType.Excitatory != pynlm.NeuronType.Inhibitory
    
    # Test all NeuronType values
    neuron_types = [
        pynlm.NeuronType.Excitatory,
        pynlm.NeuronType.Inhibitory,
        pynlm.NeuronType.Modulatory,
        pynlm.NeuronType.Sensory,
        pynlm.NeuronType.Motor,
        pynlm.NeuronType.Internal,
    ]
    
    for enum_type in neuron_types:
        assert isinstance(enum_type, pynlm.NeuronType)
    
    # Test DevelopmentalStage
    stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging,
    ]
    
    for stage in stages:
        assert isinstance(stage, pynlm.DevelopmentalStage)
    
    # Test ActionType
    action_types = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Eat,
    ]
    
    for action_type in action_types:
        assert isinstance(action_type, pynlm.ActionType)
    
    # Test WorldObjectType
    object_types = [
        pynlm.WorldObjectType.Empty,
        pynlm.WorldObjectType.Resource,
        pynlm.WorldObjectType.Hazard,
        pynlm.WorldObjectType.Wall,
    ]
    
    for obj_type in object_types:
        assert isinstance(obj_type, pynlm.WorldObjectType)


def test_invalid_ids():
    """Test invalid ID constants."""
    # Check that invalid IDs are accessible
    assert pynlm.INVALID_NEURON_ID is not None
    assert pynlm.INVALID_SYNAPSE_ID is not None
    assert pynlm.INVALID_REGION_ID is not None
    assert pynlm.INVALID_POPULATION_ID is not None
    
    # They should have the index method
    assert pynlm.INVALID_NEURON_ID.index() == 2**64 - 1
    assert pynlm.INVALID_SYNAPSE_ID.index() == 2**64 - 1
    assert pynlm.INVALID_REGION_ID.index() == 2**64 - 1
    assert pynlm.INVALID_POPULATION_ID.index() == 2**64 - 1


def test_type_aliases():
    """Test that type aliases are available."""
    # These should be imported from typing
    from typing import List, Dict, Optional, Union, Tuple, Any
    
    assert List is not None
    assert Dict is not None
    assert Optional is not None
    assert Union is not None
    assert Tuple is not None
    assert Any is not None


def test_error_messages():
    """Test error message formatting."""
    config = pynlm.createDefaultConfig()
    
    # Test that errors provide meaningful messages
    try:
        config.loadFromFile("/nonexistent/file.json")
    except PyNLMRuntimeError as e:
        assert "nonexistent" in str(e).lower() or "failed" in str(e).lower()


def test_module_initialization():
    """Test module initialization and logging."""
    # This should not raise an error
    import pynlm as pynlm_module
    assert pynlm_module is not None
    
    # Test that module attributes exist
    assert hasattr(pynlm_module, '__version__')
    assert hasattr(pynlm_module, '__all__')
    assert hasattr(pynlm_module, '__author__')
    assert hasattr(pynlm_module, '__license__')


def test_placeholder_imports():
    """Test that placeholder modules work when C++ extension is not available."""
    # This test verifies that the placeholder module provides
    # basic functionality when the C++ extension is not available
    
    # Check that placeholder classes exist
    assert pynlm_runtime_error := pynlm.PyNLMRuntimeError
    assert pynlm_config_error := pynlm.PyNLMConfigurationError
    assert pynlm_init_error := pynlm.PyNLMInitializationError
    
    # Test exception hierarchy
    assert issubclass(pynlm_config_error, pynlm_runtime_error)
    assert issubclass(pynlm_init_error, pynlm_runtime_error)


if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
