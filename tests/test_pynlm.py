#!/usr/bin/env python3
"""
Python binding tests for NLM (Neural Learning Machine) Python bindings.

Tests the pynlm Python API for Brain, Config, SimpleWorld, and AgentBrain classes.
"""

import sys
import pynlm
import traceback


def test_version():
    """Test that pynlm module has version attribute."""
    assert hasattr(pynlm, "__version__"), "pynlm module should have __version__ attribute"
    print("✓ Version test passed")


def test_config_creation():
    """Test basic Config creation and functionality."""
    # Test default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None, "createDefaultConfig should return a Config object"
    
    # Test basic functionality
    assert not config.has("test_key"), "New config should not have 'test_key'"
    
    # Test setting and getting values
    config.set("test_int", 42)
    assert config.has("test_int"), "Config should have 'test_int' after setting"
    
    # Test getOr for missing key
    missing_val = config.getOr("missing_key", 100)
    assert missing_val == 100, "getOr should return default for missing key"
    
    print("✓ Config creation test passed")


def test_brain_creation():
    """Test Brain creation and basic initialization."""
    # Create brain with default config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None, "createBrain should return a Brain object"
    
    # Test basic brain properties
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    
    # Test developmental stages
    stage = brain.getDevelopmentalStage()
    assert stage is not None, "Brain should have a developmental stage"
    
    print("✓ Brain creation test passed")


def test_simple_world_creation():
    """Test SimpleWorld creation and basic functionality."""
    # Create world
    world = pynlm.createSimpleWorld()
    assert world is not None, "createSimpleWorld should return a World object"
    
    # Test basic properties
    assert world.getWidth() > 0, "World should have positive width"
    assert world.getHeight() > 0, "World should have positive height"
    
    # Test configuration
    world.configure(width=20, height=15, visionWidth=10, visionHeight=10)
    assert world.getWidth() == 20, "World width should be configurable"
    assert world.getHeight() == 15, "World height should be configurable"
    
    print("✓ SimpleWorld creation test passed")


def test_agent_brain_creation():
    """Test AgentBrain creation and basic functionality."""
    # Create brain first
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "createAgentBrain should return an AgentBrain object"
    
    # Test agent properties
    assert agent.getBrain() is brain, "AgentBrain should reference the same brain"
    assert agent.getDevelopmentalStage() is not None, "AgentBrain should have developmental stage"
    
    print("✓ AgentBrain creation test passed")


def test_action_functionality():
    """Test Action class creation and manipulation."""
    # Test default action
    action = pynlm.Action()
    assert action is not None, "Action() should create an action"
    
    # Test action with type
    action2 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action2.getType() == pynlm.ActionType.MoveForward, "Action should store and return type"
    
    # Test action with parameters
    action3 = pynlm.Action(pynlm.ActionType.Interact, [1.0, 2.0, 3.0])
    assert action3.getType() == pynlm.ActionType.Interact, "Action should store type"
    params = action3.getParameters()
    assert len(params) == 3, "Action should have 3 parameters"
    
    # Test clone
    cloned = action3.clone()
    assert cloned.getType() == action3.getType(), "Cloned action should have same type"
    assert cloned.getParameters() == action3.getParameters(), "Cloned action should have same parameters"
    
    print("✓ Action functionality test passed")


def test_world_object_functionality():
    """Test WorldObject class creation and properties."""
    # Test default world object
    obj = pynlm.WorldObject()
    assert obj is not None, "WorldObject() should create an object"
    
    # Test world object with properties
    obj2 = pynlm.WorldObject(x=5.0, y=10.0, pynlm.WorldObjectType.Resource, value=42.0, radius=2.0)
    assert obj2.x == 5.0, "WorldObject should store x coordinate"
    assert obj2.y == 10.0, "WorldObject should store y coordinate"
    assert obj2.type == pynlm.WorldObjectType.Resource, "WorldObject should store type"
    assert obj2.value == 42.0, "WorldObject should store value"
    assert obj2.radius == 2.0, "WorldObject should store radius"
    
    print("✓ WorldObject functionality test passed")


def test_sensory_input():
    """Test SensoryInput classes (Vision, Audio, InternalSignals)."""
    # Test Vision
    vision = pynlm.Vision(10, 10, 3)
    assert vision is not None, "Vision() should create a Vision object"
    assert vision.getWidth() == 10, "Vision should have correct width"
    assert vision.getHeight() == 10, "Vision should have correct height"
    assert vision.getChannels() == 3, "Vision should have correct channels"
    
    # Test Audio
    audio = pynlm.Audio(44100, 1024)
    assert audio is not None, "Audio() should create an Audio object"
    assert audio.getSampleRate() == 44100, "Audio should have correct sample rate"
    assert audio.getNumSamples() == 1024, "Audio should have correct sample count"
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    assert internal is not None, "InternalSignals() should create an InternalSignals object"
    internal.addSignal(1.0)
    internal.addSignal(2.0)
    signals = internal.getAllSignals()
    assert len(signals) == 2, "InternalSignals should have 2 signals"
    
    print("✓ Sensory input test passed")


def test_action_result():
    """Test ActionResult class."""
    # Test default action result
    result = pynlm.ActionResult()
    assert result is not None, "ActionResult() should create an ActionResult"
    
    # Test action result with values
    result2 = pynlm.ActionResult(reward=10.0, success=True, message="Success!")
    assert result2.reward == 10.0, "ActionResult should store reward"
    assert result2.success == True, "ActionResult should store success"
    assert result2.message == "Success!", "ActionResult should store message"
    
    print("✓ ActionResult test passed")


def test_agent_body():
    """Test AgentBody class."""
    # Test default agent body
    body = pynlm.AgentBody()
    assert body is not None, "AgentBody() should create an AgentBody"
    
    # Test agent body properties
    body.x = 5.0
    body.y = 10.0
    body.orientation = 90.0
    body.velocityX = 1.0
    body.velocityY = 0.5
    body.energy = 75.0
    body.health = 90.0
    body.age = 5
    
    assert body.x == 5.0, "AgentBody should store x"
    assert body.y == 10.0, "AgentBody should store y"
    assert body.orientation == 90.0, "AgentBody should store orientation"
    assert body.energy == 75.0, "AgentBody should store energy"
    assert body.health == 90.0, "AgentBody should store health"
    
    # Test reset
    body.reset()
    # After reset, default values should be restored (not checking exact values)
    assert body.isMoving == False, "AgentBody reset should reset movement state"
    
    print("✓ AgentBody test passed")


def test_sensory_percept():
    """Test SensoryPercept class."""
    # Test default sensory percept
    percept = pynlm.SensoryPercept()
    assert percept is not None, "SensoryPercept() should create a SensoryPercept"
    
    # Test setting and getting vision
    vision = pynlm.Vision(8, 8, 3)
    percept.setVision(vision)
    retrieved_vision = percept.getVision()
    assert retrieved_vision is not None, "SensoryPercept should store vision"
    assert retrieved_vision.getWidth() == 8, "SensoryPercept should preserve vision width"
    
    print("✓ SensoryPercept test passed")


def test_enums():
    """Test that all Python enums are accessible and have expected values."""
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.NeuronType.Inhibitory is not None
    assert pynlm.NeuronType.Sensory is not None
    assert pynlm.NeuronType.Motor is not None
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.ActionType.TurnLeft is not None
    assert pynlm.ActionType.Interact is not None
    assert pynlm.ActionType.Eat is not None
    
    # Test WorldObjectType
    assert pynlm.WorldObjectType.Empty is not None
    assert pynlm.WorldObjectType.Resource is not None
    assert pynlm.WorldObjectType.Wall is not None
    
    # Test DevelopmentalStage
    assert pynlm.DevelopmentalStage.Initial is not None
    assert pynlm.DevelopmentalStage.CriticalPeriod is not None
    assert pynlm.DevelopmentalStage.Maturation is not None
    
    print("✓ Enums test passed")


def test_error_handling():
    """Test error handling in Python bindings."""
    # Test that config methods handle invalid types gracefully
    config = pynlm.createDefaultConfig()
    
    # Test invalid type for getOr (should handle gracefully)
    # This test verifies that the Python bindings don't crash on basic usage
    result = config.getOr("definitely_does_not_exist", 999)
    assert result == 999, "getOr should return default for missing key"
    
    print("✓ Error handling test passed")


def test_construction():
    """Test construction of various objects with different parameters."""
    # Test Vision with different parameters
    vision1 = pynlm.Vision(5)
    assert vision1.getWidth() == 5
    
    vision2 = pynlm.Vision(10, 20, 1)
    assert vision2.getWidth() == 10
    assert vision2.getHeight() == 20
    assert vision2.getChannels() == 1
    
    # Test Audio with different parameters
    audio1 = pynlm.Audio(11025)
    assert audio1.getSampleRate() == 11025
    
    audio2 = pynlm.Audio(48000, 2048)
    assert audio2.getSampleRate() == 48000
    assert audio2.getNumSamples() == 2048
    
    # Test Action with different parameter combinations
    action1 = pynlm.Action()
    action2 = pynlm.Action(pynlm.ActionType.TurnLeft)
    action3 = pynlm.Action(pynlm.ActionType.Rest, [0.0, 0.0])
    
    assert action1.getType() is not None
    assert action2.getType() == pynlm.ActionType.TurnLeft
    assert action3.getType() == pynlm.ActionType.Rest
    
    print("✓ Construction test passed")


def run_all_tests():
    """Run all Python binding tests."""
    print("=== Running Python Binding Tests for NLM ===\n")
    
    tests = [
        test_version,
        test_config_creation,
        test_brain_creation,
        test_simple_world_creation,
        test_agent_brain_creation,
        test_action_functionality,
        test_world_object_functionality,
        test_sensory_input,
        test_action_result,
        test_agent_body,
        test_sensory_percept,
        test_enums,
        test_error_handling,
        test_construction,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            print(f"  Traceback: {traceback.format_exc()}")
            failed += 1
    
    print(f"\n=== Test Results ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total:  {len(tests)}")
    
    if failed > 0:
        print(f"\n⚠️  {failed} test(s) failed")
        return False
    else:
        print(f"\n✅ All {passed} tests passed!")
        return True


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)