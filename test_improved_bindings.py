"""
Basic Python test to validate improved NLM Python bindings.
This test ensures that all the improvements made to the bindings work correctly.
"""

import pynlm
import pytest


def test_basic_imports():
    """Test that all main classes are properly exported."""
    # Test basic factory functions
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    # Test enumerations
    neuron_type = pynlm.NeuronType.Excitatory
    synapse_type = pynlm.SynapseType.Excitatory
    stage = pynlm.DevelopmentalStage.Initial
    
    assert neuron_type is not None
    assert synapse_type is not None
    assert stage is not None


def test_config_functionality():
    """Test improved Config class functionality."""
    config = pynlm.createDefaultConfig()
    
    # Test basic operations
    config.set("test.key", "test.value")
    assert config.has("test.key")
    assert config.get("test.key") == "test.value"
    
    # Test container operations
    assert len(config) > 0
    assert "test.key" in config
    
    # Test serialization
    config.saveToFile("test_config.json")
    
    # Test loading
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("test_config.json")
    assert config2.has("test.key")
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0


def test_vision_safety():
    """Test that Vision input validation works correctly."""
    vision = pynlm.Vision(10, 10, 3)
    
    # Test valid data
    valid_data = [0.5] * (10 * 10 * 3)
    vision.setData(valid_data)
    assert vision.getWidth() == 10
    assert vision.getHeight() == 10
    assert vision.getChannels() == 3
    
    # Test validation methods
    assert vision.getTotalPixels() == 100
    assert vision.validate() is True
    
    # Test empty data validation
    empty_vision = pynlm.Vision(5, 5, 1)
    with pytest.raises(pybind11.error)
    :
        empty_vision.setData([])


def test_agent_body_convenience_methods():
    """Test new convenience methods on AgentBody."""
    body = pynlm.AgentBody()
    body.x = 5.0
    body.y = 10.0
    body.velocityX = 3.0
    body.velocityY = 4.0
    body.energy = 50.0
    body.maxEnergy = 100.0
    body.health = 80.0
    body.maxHealth = 100.0
    
    # Test new convenience methods
    speed = body.getSpeed()
    assert abs(speed - 5.0) < 0.001  # sqrt(3^2 + 4^2) = 5
    
    angle = body.getAngle()
    assert abs(angle - 0.927) < 0.01  # atan2(4, 3)
    
    assert body.getEnergyRatio() == 0.5
    assert body.getHealthRatio() == 0.8
    assert body.isHealthy() is True
    
    # Test dict conversion
    body_dict = body.to_dict()
    assert "x" in body_dict
    assert "y" in body_dict
    assert "speed" in body_dict


def test_world_position_validation():
    """Test world position validation and bounds checking."""
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Test valid position
    world.setAgentPosition(10.0, 10.0)
    
    # Test invalid position (out of bounds)
    with pytest.raises(pybind11.error):
        world.setAgentPosition(30.0, 10.0)
    
    with pytest.raises(pybind11.error):
        world.setAgentPosition(10.0, 30.0)


def test_action_convenience_methods():
    """Test new convenience methods on Action class."""
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    
    # Test new convenience methods
    assert not action.isEmpty()
    assert action.getName() is not None
    assert action.getDescription() is not None
    
    # Test adding parameters
    action.addParameter(1.0)
    assert len(action.getParameters()) == 1
    
    # Test parameter removal
    action.removeParameter(0)
    assert len(action.getParameters()) == 0


def test_sensory_percept_validation():
    """Test sensory percept validation."""
    percept = pynlm.SensoryPercept()
    
    # Test validation
    assert percept.validate() is True
    
    # Test dict conversion
    percept_dict = percept.to_dict()
    assert "vision" in percept_dict
    assert "timestamp" in percept_dict


def test_brain_stats():
    """Test brain statistics functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test stats method
    stats = brain.getStats()
    assert isinstance(stats, dict)
    assert "neuronCount" in stats
    assert "synapseCount" in stats
    assert "firingCount" in stats
    
    # Test validation
    assert brain.validate() is True


def test_agent_brain_status():
    """Test agent brain status functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test status method
    status = agent.getStatus()
    assert isinstance(status, dict)
    assert "developmentalStage" in status
    assert "neuromodulationLevel" in status
    
    # Test validation
    assert agent.validate() is True


def test_utility_functions():
    """Test utility functions."""
    # Test version
    version = pynlm.printVersion()
    assert isinstance(version, str)
    assert "NLM Python Bindings" in version
    
    # Test memory usage
    memory_info = pynlm.getMemoryUsage()
    assert isinstance(memory_info, str)
    
    # Test validation utility
    config_dict = {"width": 10, "height": 10}
    result = pynlm.validateAll(config_dict)
    assert result is True


def test_backward_compatibility():
    """Test that existing APIs still work (backward compatibility)."""
    # Test old-style initialization
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test old-style step
    brain.step(0)
    brain.step(1)
    
    # Test old-style world interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Test old-style object creation
    world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
    assert world_obj.x == 5.0
    assert world_obj.y == 5.0
    assert world_obj.type == pynlm.WorldObjectType.Resource


if __name__ == "__main__":
    # Run all tests
    test_basic_imports()
    print("✓ Basic imports test passed")
    
    test_config_functionality()
    print("✓ Config functionality test passed")
    
    test_vision_safety()
    print("✓ Vision safety test passed")
    
    test_agent_body_convenience_methods()
    print("✓ Agent body convenience methods test passed")
    
    test_world_position_validation()
    print("✓ World position validation test passed")
    
    test_action_convenience_methods()
    print("✓ Action convenience methods test passed")
    
    test_sensory_percept_validation()
    print("✓ Sensory percept validation test passed")
    
    test_brain_stats()
    print("✓ Brain stats test passed")
    
    test_agent_brain_status()
    print("✓ Agent brain status test passed")
    
    test_utility_functions()
    print("✓ Utility functions test passed")
    
    test_backward_compatibility()
    print("✓ Backward compatibility test passed")
    
    print("\n✅ All tests passed! The improved Python bindings are working correctly.")