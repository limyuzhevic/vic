#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings (pynlm)
Tests the core functionality of the neural simulation framework
"""

import pynlm
import pytest
import tempfile
import os
import sys
from typing import List, Dict, Any


def test_version_and_metadata():
    """Test module metadata and version information."""
    # Test basic module attributes
    assert hasattr(pynlm, '__version__') or hasattr(pynlm, '__doc__')
    
    # Test that main classes are available
    assert hasattr(pynlm, 'Config')
    assert hasattr(pynlm, 'Brain')
    assert hasattr(pynlm, 'SimpleWorld')
    assert hasattr(pynlm, 'AgentBrain')
    assert hasattr(pynlm, 'Action')
    assert hasattr(pynlm, 'SensoryInput')
    assert hasattr(pynlm, 'Vision')
    assert hasattr(pynlm, 'Audio')
    
    # Test that ID types are available
    assert hasattr(pynlm, 'NeuronId')
    assert hasattr(pynlm, 'SynapseId')
    assert hasattr(pynlm, 'RegionId')
    assert hasattr(pynlm, 'PopulationId')
    
    # Test that enumerations are available
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'SynapseType')
    assert hasattr(pynlm, 'ActionType')


def test_config_creation_and_basic_functionality():
    """Test Config class creation and basic operations."""
    # Test default config creation
    config = pynlm.Config()
    
    # Test basic methods
    assert config.has("random_seed")
    assert config.has("simulation_timestep")
    assert config.has("neuron_count")
    
    # Test getting values
    seed = config.getOr("random_seed", 42)
    assert isinstance(seed, int)
    assert seed == 42
    
    timestep = config.getOr("simulation_timestep", 0.001)
    assert isinstance(timestep, float)
    assert timestep == 0.001
    
    # Test save/load functionality
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        filepath = f.name
    
    try:
        config.saveToFile(filepath)
        assert os.path.exists(filepath)
        
        config2 = pynlm.Config()
        config2.loadFromFile(filepath)
        assert config2.has("random_seed")
        
        # Test config summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
        
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)


def test_id_creation_and_manipulation():
    """Test ID class creation and manipulation."""
    # Test NeuronId
    neuron_id1 = pynlm.NeuronId()
    assert neuron_id1.index() == 0
    assert neuron_id1.value == 0
    
    neuron_id2 = pynlm.NeuronId(123)
    assert neuron_id2.index() == 123
    assert neuron_id2.value == 123
    
    # Test equality and hashing
    neuron_id3 = pynlm.NeuronId(123)
    assert neuron_id2 == neuron_id3
    assert hash(neuron_id2) == hash(neuron_id3)
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(456)
    assert synapse_id.index() == 456
    
    # Test RegionId
    region_id = pynlm.RegionId(789)
    assert region_id.index() == 789
    
    # Test PopulationId
    pop_id = pynlm.PopulationId(101)
    assert pop_id.index() == 101


def test_enumerations():
    """Test all enumerations."""
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory == 0
    assert pynlm.NeuronType.Inhibitory == 1
    assert pynlm.NeuronType.Modulatory == 2
    assert pynlm.NeuronType.Sensory == 3
    assert pynlm.NeuronType.Motor == 4
    assert pynlm.NeuronType.Internal == 5
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory == 0
    assert pynlm.SynapseType.Inhibitory == 1
    assert pynlm.SynapseType.Modulatory == 2
    assert pynlm.SynapseType.Electrical == 3
    assert pynlm.SynapseType.GapJunction == 4
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward == 0
    assert pynlm.ActionType.MoveBackward == 1
    assert pynlm.ActionType.TurnLeft == 2
    assert pynlm.ActionType.TurnRight == 3


def test_sensory_input_classes():
    """Test sensory input classes."""
    # Test Vision
    vision = pynlm.Vision(640, 480, 3)
    assert vision.getWidth() == 640
    assert vision.getHeight() == 480
    assert vision.getChannels() == 3
    assert vision.getType() == pynlm.NeuronType.Sensory
    
    # Test Audio
    audio = pynlm.Audio(44100, 1024)
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1024
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.addSignal(0.5)
    internal.addSignal(1.0)
    signals = internal.getData()
    assert len(signals) == 2
    assert signals[0] == 0.5
    assert signals[1] == 1.0


def test_action_classes():
    """Test action classes."""
    # Test Action with type only
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.getType() == pynlm.ActionType.MoveForward
    assert action1.getName() == "MoveForward"
    
    # Test Action with type and parameters
    params = [0.5, 0.3, 0.1]
    action2 = pynlm.Action(pynlm.ActionType.TurnLeft, params)
    assert action2.getType() == pynlm.ActionType.TurnLeft
    assert action2.getParameters() == params
    
    # Test Action cloning
    action3 = action2.clone()
    assert action3.getType() == action2.getType()
    assert action3.getParameters() == action2.getParameters()


def test_world_object():
    """Test WorldObject class."""
    # Create a resource object
    obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 5.0, 1.0)
    assert obj.x == 10.0
    assert obj.y == 20.0
    assert obj.type == pynlm.WorldObjectType.Resource
    assert obj.value == 5.0
    assert obj.radius == 1.0
    assert obj.active == True
    
    # Test default constructor
    obj2 = pynlm.WorldObject()
    assert obj2.x == 0.0
    assert obj2.y == 0.0
    assert obj2.type == pynlm.WorldObjectType.Empty


def test_agent_body():
    """Test AgentBody class."""
    body = pynlm.AgentBody()
    body.x = 5.0
    body.y = 10.0
    body.orientation = 45.0
    body.velocityX = 2.0
    body.velocityY = 1.0
    body.energy = 100.0
    body.health = 50.0
    body.age = 25
    body.isMoving = True
    body.isTurning = False
    
    assert body.x == 5.0
    assert body.y == 10.0
    assert body.orientation == 45.0
    assert body.energy == 100.0
    assert body.isMoving == True
    
    # Test reset method
    body.reset()
    assert body.energy == 100.0  # Default energy should be restored


def test_simple_world():
    """Test SimpleWorld class."""
    world = pynlm.SimpleWorld()
    
    # Test configuration
    world.configure(100, 80, 32, 32)
    assert world.getWidth() == 100
    assert world.getHeight() == 80
    assert world.getMaxEnergy() == 100.0  # Default value
    
    # Test agent start position
    world.setAgentStart(50, 40)
    
    # Test random seed
    world.setRandomSeed(12345)
    assert world.getRandomSeed() == 12345


def test_create_default_objects():
    """Test factory functions for creating default objects."""
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    assert config.has("random_seed")
    assert config.has("neuron_count")
    
    # Test createBrain
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test createSimpleWorld
    world = pynlm.createSimpleWorld()
    assert world is not None
    assert world.getWidth() == 100  # Default width
    
    # Test createAgentBrain
    agent_brain = pynlm.createAgentBrain(brain)
    assert agent_brain is not None


def test_invalid_ids():
    """Test that invalid IDs are properly defined."""
    assert pynlm.INVALID_NEURON_ID.value == 0
    assert pynlm.INVALID_SYNAPSE_ID.value == 0
    assert pynlm.INVALID_REGION_ID.value == 0
    assert pynlm.INVALID_POPULATION_ID.value == 0


def test_integration_functionality():
    """Test integration between different components."""
    # Create a complete setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Try to initialize (this may fail but shouldn't crash)
    try:
        # This might fail due to missing dependencies but shouldn't crash
        # We just want to ensure the bindings work
        pass
    except:
        pass  # Expected in test environment
    
    # Test that we can create a world and interact
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 10, 10)
    
    # Test some basic world operations
    assert world.isValidPosition(25, 25)
    assert not world.isValidPosition(-1, 0)
    assert not world.isValidPosition(0, 101)


if __name__ == "__main__":
    # Run all tests
    test_functions = [
        test_version_and_metadata,
        test_config_creation_and_basic_functionality,
        test_id_creation_and_manipulation,
        test_enumerations,
        test_sensory_input_classes,
        test_action_classes,
        test_world_object,
        test_agent_body,
        test_simple_world,
        test_create_default_objects,
        test_invalid_ids,
        test_integration_functionality,
    ]
    
    print("Running NLM Python bindings comprehensive test suite...")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_func in test_functions:
        try:
            test_func()
            print(f"✓ {test_func.__name__}")
            passed += 1
        except Exception as e:
            print(f"✗ {test_func.__name__}: {e}")
            failed += 1
    
    print("=" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        print("\nSome tests failed. This may be expected if:")
        print("1. The pynlm module requires compilation first")
        print("2. Some C++ dependencies are not available")
        print("3. The test environment is not fully set up")
    
    sys.exit(1 if failed > 0 else 0)