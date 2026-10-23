#!/usr/bin/env python3
"""
Python tests for NLM bindings.
Tests the basic functionality of the NLM Python bindings.
"""

import sys
import tempfile
import os
from pathlib import Path

# Add the build directory to Python path
sys.path.insert(0, str(Path(__file__).parent / "build"))

try:
    import pynlm
    print("Successfully imported pynlm")
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Please build the bindings first using scikit-build")
    sys.exit(1)

def test_config_creation():
    """Test Config creation and basic operations."""
    print("Testing Config creation...")
    
    config = pynlm.createDefaultConfig()
    assert config is not None, "Config creation failed"
    
    # Test has method
    assert not config.has("test"), "Config should not have 'test' key"
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str), "Summary should be a string"
    print(f"Config summary: {summary}")
    
    print("✓ Config creation test passed")

def test_config_set_get():
    """Test Config set and get operations."""
    print("Testing Config set/get operations...")
    
    config = pynlm.createDefaultConfig()
    
    # Test setting different types
    config.set("int_value", 42)
    int_val = config.get("int_value")
    assert int_val == 42, f"Expected 42, got {int_val}"
    
    config.set("double_value", 3.14)
    double_val = config.get("double_value")
    assert double_val == 3.14, f"Expected 3.14, got {double_val}"
    
    config.set("string_value", "hello")
    string_val = config.get("string_value")
    assert string_val == "hello", f"Expected 'hello', got {string_val}"
    
    config.set("bool_value", True)
    bool_val = config.get("bool_value")
    assert bool_val is True, f"Expected True, got {bool_val}"
    
    # Test getOr method
    missing_val = config.getOr("missing_key", 999)
    assert missing_val == 999, f"Expected 999, got {missing_val}"
    
    print("✓ Config set/get test passed")

def test_config_save_load():
    """Test Config save and load operations."""
    print("Testing Config save/load operations...")
    
    config = pynlm.createDefaultConfig()
    config.set("test_int", 123)
    config.set("test_string", "test_value")
    config.set("test_bool", False)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        # Save config
        result = config.saveToFile(temp_file)
        assert result, "Config save failed"
        
        # Create new config and load
        config2 = pynlm.createDefaultConfig()
        result = config2.loadFromFile(temp_file)
        assert result, "Config load failed"
        
        # Verify loaded values
        assert config2.get("test_int") == 123, "Loaded int value mismatch"
        assert config2.get("test_string") == "test_value", "Loaded string value mismatch"
        assert config2.get("test_bool") is False, "Loaded bool value mismatch"
        
        print("✓ Config save/load test passed")
    finally:
        # Clean up
        if os.path.exists(temp_file):
            os.unlink(temp_file)

def test_config_keys_and_remove():
    """Test Config keys and remove operations."""
    print("Testing Config keys and remove operations...")
    
    config = pynlm.createDefaultConfig()
    
    # Add some keys
    config.set("key1", 1)
    config.set("key2", 2)
    config.set("key3", 3)
    
    # Get all keys
    keys = config.getKeys()
    assert "key1" in keys, "key1 should be in keys"
    assert "key2" in keys, "key2 should be in keys"
    assert "key3" in keys, "key3 should be in keys"
    
    # Remove a key
    config.remove("key2")
    assert not config.has("key2"), "key2 should have been removed"
    assert "key1" in config.getKeys(), "key1 should still be present"
    
    print("✓ Config keys and remove test passed")

def test_sensory_input():
    """Test SensoryInput subclasses."""
    print("Testing SensoryInput...")
    
    # Test Vision
    vision = pynlm.Vision(10, 10, 3)
    assert vision.getWidth() == 10, "Vision width mismatch"
    assert vision.getHeight() == 10, "Vision height mismatch"
    assert vision.getChannels() == 3, "Vision channels mismatch"
    
    # Test setting data
    test_data = [0.1] * 300  # 10x10x3 = 300 values
    vision.setData(test_data)
    
    # Test Audio
    audio = pynlm.Audio(44100, 1024)
    assert audio.getSampleRate() == 44100, "Audio sample rate mismatch"
    assert audio.getNumSamples() == 1024, "Audio num samples mismatch"
    
    # Test setting audio data
    audio_data = [0.5] * 1024
    audio.setData(audio_data)
    
    print("✓ SensoryInput test passed")

def test_action():
    """Test Action class."""
    print("Testing Action...")
    
    # Create action by type
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.getType() == pynlm.ActionType.MoveForward, "Action type mismatch"
    
    # Create action with type and parameters
    action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
    assert action2.getType() == pynlm.ActionType.Custom, "Action type mismatch"
    assert action2.getParameters() == [1.0, 2.0, 3.0], "Action parameters mismatch"
    
    # Test action cloning
    action3 = action1.clone()
    assert action3.getType() == pynlm.ActionType.MoveForward, "Cloned action type mismatch"
    
    print("✓ Action test passed")

def test_world_object():
    """Test WorldObject class."""
    print("Testing WorldObject...")
    
    # Create world object
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0, 0.5)
    assert obj.x == 5.0, "Object x position mismatch"
    assert obj.y == 5.0, "Object y position mismatch"
    assert obj.type == pynlm.WorldObjectType.Resource, "Object type mismatch"
    assert obj.value == 10.0, "Object value mismatch"
    assert obj.radius == 0.5, "Object radius mismatch"
    
    print("✓ WorldObject test passed")

def test_agent_body():
    """Test AgentBody class."""
    print("Testing AgentBody...")
    
    body = pynlm.AgentBody()
    
    # Set attributes
    body.x = 10.0
    body.y = 20.0
    body.orientation = 90.0
    body.velocityX = 1.5
    body.velocityY = 2.0
    body.angularVelocity = 0.1
    body.energy = 100.0
    body.health = 50.0
    body.age = 5
    body.isMoving = True
    body.isTurning = False
    
    # Verify attributes
    assert body.x == 10.0, "AgentBody x mismatch"
    assert body.y == 20.0, "AgentBody y mismatch"
    assert body.orientation == 90.0, "AgentBody orientation mismatch"
    assert body.energy == 100.0, "AgentBody energy mismatch"
    assert body.isMoving is True, "AgentBody isMoving mismatch"
    
    # Test reset
    body.reset()
    # After reset, values should be back to defaults
    assert body.energy == 0.0, "AgentBody energy should be reset to 0"
    
    print("✓ AgentBody test passed")

def test_simple_world():
    """Test SimpleWorld class."""
    print("Testing SimpleWorld...")
    
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(20, 20, 84, 84)  # width, height, visionWidth, visionHeight
    assert world.getWidth() == 20, "World width mismatch"
    assert world.getHeight() == 20, "World height mismatch"
    
    # Set agent start
    world.setAgentStart(5.0, 5.0)
    
    # Create agent body and set
    agent_body = world.getAgentBody()
    assert agent_body.x == 5.0, "Agent x position mismatch"
    assert agent_body.y == 5.0, "Agent y position mismatch"
    
    # Reset world
    world.reset()
    
    # Test object operations
    obj = pynlm.WorldObject(1.0, 1.0, pynlm.WorldObjectType.Resource, 5.0)
    world.addObject(obj)
    
    # Check if position is valid
    assert world.isValidPosition(1.0, 1.0), "Position should be valid"
    assert not world.isValidPosition(50.0, 50.0), "Position should be invalid"
    
    print("✓ SimpleWorld test passed")

def run_all_tests():
    """Run all Python tests."""
    print("=" * 60)
    print("Running NLM Python Bindings Tests")
    print("=" * 60)
    
    tests = [
        test_config_creation,
        test_config_set_get,
        test_config_save_load,
        test_config_keys_and_remove,
        test_sensory_input,
        test_action,
        test_world_object,
        test_agent_body,
        test_simple_world,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)

if __name__ == "__main__":
    run_all_tests()