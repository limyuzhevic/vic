#!/usr/bin/env python3
"""
Basic test script for NLM Python bindings.
Run with: python -m pytest tests/ -v
"""

import sys
import os
import tempfile
import shutil
from pathlib import Path

# Add the python directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm as nlm
def test_basic_import():
    """Test that all core classes can be imported."""
    # Test basic imports
    assert hasattr(nlm, 'NeuronId')
    assert hasattr(nlm, 'SynapseId')
    assert hasattr(nlm, 'RegionId')
    assert hasattr(nlm, 'PopulationId')
    assert hasattr(nlm, 'NeuronType')
    assert hasattr(nlm, 'SynapseType')
    assert hasattr(nlm, 'DevelopmentalStage')
    assert hasattr(nlm, 'FiringState')
    assert hasattr(nlm, 'ActionType')
    assert hasattr(nlm, 'MotorCommand')
    assert hasattr(nlm, 'WorldObjectType')
    
    print("✓ All basic imports successful")
def test_config():
    """Test configuration class."""
    config = nlm.Config()
    
    # Test setting and getting values
    config.set("test_int", 42)
    config.set("test_double", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Test get methods
    assert config.get("test_int").value == 42
    assert config.get("test_double").value == 3.14
    assert config.get("test_string").value == "hello"
    assert config.get("test_bool").value == True
    
    # Test getOr with defaults
    assert config.getOr("missing_int", 99) == 99
    assert config.getOr("missing_string", "default") == "default"
    
    # Test has
    assert config.has("test_int")
    assert not config.has("nonexistent")
    
    # Test getKeys
    keys = config.getKeys()
    assert "test_int" in keys
    assert "test_double" in keys
    assert "test_string" in keys
    assert "test_bool" in keys
    
    print("✓ Configuration tests passed")
def test_action():
    """Test action class."""
    action = nlm.Action(nlm.ActionType.MoveForward)
    assert action.getType() == nlm.ActionType.MoveForward
    
    action.setType(nlm.ActionType.Interact)
    assert action.getType() == nlm.ActionType.Interact
    
    # Test parameters
    params = [0.1, 0.2, 0.3]
    action.setParameters(params)
    assert action.getParameters() == params
    
    # Test cloning
    action2 = action.clone()
    assert action2.getType() == action.getType()
    assert action2.getParameters() == action.getParameters()
    
    print("✓ Action tests passed")
def test_world_object():
    """Test world object class."""
    obj = nlm.WorldObject(1.0, 2.0, nlm.WorldObjectType.Resource)
    assert obj.x == 1.0
    assert obj.y == 2.0
    assert obj.type == nlm.WorldObjectType.Resource
    assert obj.radius == 0.5  # default
    
    # Test setting values
    obj.value = 42.0
    obj.active = False
    assert obj.value == 42.0
    assert not obj.active
    
    print("✓ WorldObject tests passed")
def test_agent_body():
    """Test agent body class."""
    body = nlm.AgentBody()
    
    # Test default values
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.orientation == 0.0
    assert body.velocityX == 0.0
    assert body.velocityY == 0.0
    assert body.energy == 1.0
    assert body.health == 1.0
    assert body.age == 0.0
    
    # Test setting values
    body.x = 5.0
    body.y = 3.0
    body.orientation = 90.0
    body.energy = 0.8
    body.health = 0.9
    body.age = 10.0
    
    assert body.x == 5.0
    assert body.y == 3.0
    assert body.orientation == 90.0
    assert body.energy == 0.8
    assert body.health == 0.9
    assert body.age == 10.0
    
    # Test reset
    body.reset()
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.orientation == 0.0
    assert body.energy == 1.0
    assert body.health == 1.0
    assert body.age == 0.0
    
    print("✓ AgentBody tests passed")
def test_sensory_percept():
    """Test sensory percept class."""
    percept = nlm.SensoryPercept()
    
    # Test default empty values
    assert percept.getVisionWidth() == 0
    assert percept.getVisionHeight() == 0
    assert percept.getTouch() == []
    assert percept.getInternal() == []
    assert percept.getProprioception() == []
    assert percept.getAudio() == []
    
    # Test setting values
    vision_data = [0.1, 0.2, 0.3, 0.4]
    percept.setVision(vision_data)
    assert percept.getVision() == vision_data
    assert percept.getVisionWidth() == 4
    assert percept.getVisionHeight() == 1
    
    touch_data = [1.0, 0.0, 1.0]
    percept.setTouch(touch_data)
    assert percept.getTouch() == touch_data
    
    internal_data = [0.5, -0.5]
    percept.setInternal(internal_data)
    assert percept.getInternal() == internal_data
    
    proprioception_data = [1.0, -1.0, 0.0]
    percept.setProprioception(proprioception_data)
    assert percept.getProprioception() == proprioception_data
    
    audio_data = [0.8, 0.9, 1.0]
    percept.setAudio(audio_data)
    assert percept.getAudio() == audio_data
    
    # Test timestamp
    timestamp = 12345.0
    percept.setTimestamp(timestamp)
    assert percept.getTimestamp() == timestamp
    
    print("✓ SensoryPercept tests passed")
def test_simple_world():
    """Test simple world class."""
    world = nlm.SimpleWorld()
    
    # Test default configuration
    assert world.getWidth() == 0
    assert world.getHeight() == 0
    assert world.getVisionWidth() == 0
    assert world.getVisionHeight() == 0
    
    # Configure world
    world.configure(20, 20, 16, 16)
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getVisionWidth() == 16
    assert world.getVisionHeight() == 16
    
    # Test reset
    world.reset()
    assert world.getSimulationTime() == 0.0
    
    # Test agent start
    world.setAgentStart(10.0, 5.0)
    # Note: AgentBody position is stored internally
    
    # Test max energy
    world.setMaxEnergy(2.0)
    assert world.getMaxEnergy() == 2.0
    
    # Test energy decay rate
    world.setEnergyDecayRate(0.1)
    assert world.getEnergyDecayRate() == 0.1
    
    # Test random seed
    world.setRandomSeed(42)
    assert world.getRandomSeed() == 42
    
    print("✓ SimpleWorld tests passed")
def test_factory_functions():
    """Test factory functions."""
    # Test createDefaultConfig
    config = nlm.createDefaultConfig()
    assert config is not None
    assert isinstance(config, nlm.Config)
    
    # Test createSimpleWorld
    world = nlm.createSimpleWorld()
    assert world is not None
    assert isinstance(world, nlm.SimpleWorld)
    
    # Test createBrain
    brain = nlm.createBrain()
    assert brain is not None
    assert isinstance(brain, nlm.Brain)
    
    # Test with custom config
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    assert brain.getConfig() is not None
    
    print("✓ Factory functions tests passed")
def test_enums():
    """Test enumeration values."""
    # Test NeuronType
    assert nlm.NeuronType.Excitatory == 0
    assert nlm.NeuronType.Inhibitory == 1
    assert nlm.NeuronType.Modulatory == 2
    assert nlm.NeuronType.Sensory == 3
    assert nlm.NeuronType.Motor == 4
    assert nlm.NeuronType.Internal == 5
    
    # Test SynapseType
    assert nlm.SynapseType.Excitatory == 0
    assert nlm.SynapseType.Inhibitory == 1
    assert nlm.SynapseType.Modulatory == 2
    assert nlm.SynapseType.Electrical == 3
    assert nlm.SynapseType.GapJunction == 4
    
    # Test DevelopmentalStage
    assert nlm.DevelopmentalStage.Initial == 0
    assert nlm.DevelopmentalStage.CriticalPeriod == 1
    assert nlm.DevelopmentalStage.Maturation == 2
    assert nlm.DevelopmentalStage.Adult == 3
    assert nlm.DevelopmentalStage.Aging == 4
    
    # Test FiringState
    assert nlm.FiringState.Resting == 0
    assert nlm.FiringState.Active == 1
    assert nlm.FiringState.Refractory == 2
    assert nlm.FiringState.Inhibited == 3
    
    # Test ActionType
    assert nlm.ActionType.MoveForward == 0
    assert nlm.ActionType.MoveBackward == 1
    assert nlm.ActionType.MoveLeft == 2
    assert nlm.ActionType.MoveRight == 3
    assert nlm.ActionType.TurnLeft == 4
    assert nlm.ActionType.TurnRight == 5
    assert nlm.ActionType.Look == 6
    assert nlm.ActionType.LookUp == 7
    assert nlm.ActionType.LookDown == 8
    assert nlm.ActionType.Interact == 9
    assert nlm.ActionType.Eat == 10
    assert nlm.ActionType.Drink == 11
    assert nlm.ActionType.Rest == 12
    assert nlm.ActionType.Wait == 13
    assert nlm.ActionType.Custom == 14
    
    # Test MotorCommand
    assert nlm.MotorCommand.MoveForward == 0
    assert nlm.MotorCommand.MoveBackward == 1
    assert nlm.MotorCommand.TurnLeft == 2
    assert nlm.MotorCommand.TurnRight == 3
    assert nlm.MotorCommand.LookLeft == 4
    assert nlm.MotorCommand.LookRight == 5
    assert nlm.MotorCommand.Interact == 6
    assert nlm.MotorCommand.Wait == 7
    
    # Test WorldObjectType
    assert nlm.WorldObjectType.Empty == 0
    assert nlm.WorldObjectType.Resource == 1
    assert nlm.WorldObjectType.Hazard == 2
    assert nlm.WorldObjectType.Wall == 3
    assert nlm.WorldObjectType.Marker == 4
    
    print("✓ Enum tests passed")
def test_version():
    """Test version information."""
    assert nlm.__version__ == "0.1.0"
    assert nlm.__license__ == "MIT"
    assert "NLM" in nlm.__author__
    
    print("✓ Version tests passed")
if __name__ == "__main__":
    """Run all tests."""
    print("Running NLM Python bindings tests...\n")
    
    try:
        test_basic_import()
        test_config()
        test_action()
        test_world_object()
        test_agent_body()
        test_sensory_percept()
        test_simple_world()
        test_factory_functions()
        test_enums()
        test_version()
        
        print("\n" + "="*50)
        print("All tests passed! ✓")
        print("="*50)
        
    except Exception as e:
        print(f"\nTest failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
