"""
Python NLM Bindings Test Suite

This file provides comprehensive tests for the NLM Python bindings.
It demonstrates the functionality and ensures the bindings work correctly.
"""

import pynlm
import tempfile
import os

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=== Testing Basic Functionality ===")
    
    # Test 1: Configuration creation and basic operations
    config = pynlm.Config()
    assert not config.has("test_key")
    
    config["test_key"] = 42
    assert config.has("test_key")
    assert config["test_key"] == 42
    print("✓ Configuration basic operations passed")
    
    # Test 2: Configuration with operator syntax
    config["int_value"] = 100
    config["float_value"] = 3.14
    config["string_value"] = "test"
    config["bool_value"] = True
    
    assert config["int_value"] == 100
    assert abs(config["float_value"] - 3.14) < 0.001
    assert config["string_value"] == "test"
    assert config["bool_value"] == True
    print("✓ Configuration type handling passed")
    
    # Test 3: Configuration get_or method
    assert config.get_or("missing_key", "default") == "default"
    assert config.get_or("int_value", 999) == 100
    print("✓ Configuration get_or method passed")
    
    # Test 4: Configuration validation
    try:
        config.validate(["int_value", "missing_key"])
        assert False, "Should have raised NLMError"
    except pynlm.NLMError:
        pass  # Expected
    
    assert config.validate(["int_value"], {"int_value": "int"})
    print("✓ Configuration validation passed")
    
    # Test 5: Configuration items and keys
    keys = config.get_keys()
    assert isinstance(keys, list)
    assert len(keys) > 0
    
    items = config.items()
    assert isinstance(items, list)
    assert len(items) == len(keys)
    print("✓ Configuration keys/items methods passed")
    
    # Test 6: Configuration update
    config.update({"new_key": "value", "another_key": 123})
    assert config["new_key"] == "value"
    assert config["another_key"] == 123
    print("✓ Configuration update method passed")

def test_config_file_operations():
    """Test configuration file operations."""
    print("\n=== Testing Configuration File Operations ===")
    
    # Create temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
        f.write('{"test_value": 123, "name": "test_config"}')
    
    try:
        # Test loading
        config = pynlm.Config()
        assert config.load_from_file(temp_file)
        assert config["test_value"] == 123
        assert config["name"] == "test_config"
        print("✓ Configuration file loading passed")
        
        # Test saving
        config["new_value"] = "test"
        assert config.save_to_file(temp_file)
        
        # Load again to verify save worked
        config2 = pynlm.Config()
        config2.load_from_file(temp_file)
        assert config2["new_value"] == "test"
        print("✓ Configuration file saving passed")
        
    finally:
        # Clean up
        os.unlink(temp_file)

def test_identifier_classes():
    """Test identifier classes."""
    print("\n=== Testing Identifier Classes ===")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.value == 42
    assert neuron_id.index() == 42
    assert neuron_id == pynlm.NeuronId(42)
    assert neuron_id != pynlm.NeuronId(99)
    assert hash(neuron_id) == hash(pynlm.NeuronId(42))
    assert repr(neuron_id) == "<NeuronId: 42>"
    assert str(neuron_id) == "42"
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(100)
    assert synapse_id.value == 100
    assert synapse_id.index() == 100
    assert synapse_id == pynlm.SynapseId(100)
    assert synapse_id != pynlm.SynapseId(200)
    print("✓ Identifier classes basic functionality passed")

def test_sensory_objects():
    """Test sensory input objects."""
    print("\n=== Testing Sensory Objects ===")
    
    # Test Vision
    vision = pynlm.Vision()
    assert vision.getWidth() == 0
    assert vision.getHeight() == 0
    assert vision.getChannels() == 3
    
    vision.configure(640, 480, 3)
    assert vision.getWidth() == 640
    assert vision.getHeight() == 480
    assert vision.getChannels() == 3
    
    # Set vision data
    vision_data = [0.5] * (640 * 480 * 3)
    vision.setData(vision_data)
    print("✓ Vision object functionality passed")
    
    # Test Audio
    audio = pynlm.Audio()
    audio.setSampleRate(44100)
    audio.setNumSamples(1024)
    
    audio_data = [0.1 * (i % 2) for i in range(1024)]
    audio.setData(audio_data)
    
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1024
    print("✓ Audio object functionality passed")
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.addSignal(0.8)
    internal.addSignal(-0.3)
    internal.addSignal(1.2)
    print("✓ InternalSignals object functionality passed")

def test_action_objects():
    """Test action objects."""
    print("\n=== Testing Action Objects ===")
    
    # Test basic action creation
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action.getType() == pynlm.ActionType.MoveForward
    assert action.getName() == "MoveForward"
    
    # Test action with parameters
    action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
    assert action2.getType() == pynlm.ActionType.Custom
    assert action2.getParameters() == [1.0, 2.0, 3.0]
    
    # Test action modification
    action.setParameters([0.5, 0.0])
    assert action.getParameters() == [0.5, 0.0]
    
    # Test action cloning
    action_copy = action.clone()
    assert action_copy.getType() == action.getType()
    assert action_copy.getParameters() == action.getParameters()
    
    print("✓ Action object functionality passed")

def test_world_objects():
    """Test world object operations."""
    print("\n=== Testing World Objects ===")
    
    world = pynlm.create_simple_world()
    world.configure(100, 100, 20, 20)
    
    assert world.getWidth() == 100
    assert world.getHeight() == 100
    assert world.getVisionWidth() == 20
    assert world.getVisionHeight() == 20
    
    # Test agent body
    body = world.getAgentBody()
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.orientation == 0.0
    assert body.energy == 100.0
    print("✓ World basic functionality passed")
    
    # Test world object creation and management
    resource = pynlm.WorldObject(30, 40, pynlm.WorldObjectType.Resource, 10.0)
    assert resource.x == 30.0
    assert resource.y == 40.0
    assert resource.type == pynlm.WorldObjectType.Resource
    assert resource.value == 10.0
    assert resource.active == True
    
    hazard = pynlm.WorldObject(70, 60, pynlm.WorldObjectType.Hazard, 5.0)
    assert hazard.type == pynlm.WorldObjectType.Hazard
    print("✓ World object management passed")

def test_agent_functionality():
    """Test agent brain functionality."""
    print("\n=== Testing Agent Functionality ===")
    
    # Create brain and world
    config = pynlm.Config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    world = pynlm.create_simple_world()
    world.configure(100, 100, 20, 20)
    
    # Create agent brain
    agent_brain = pynlm.create_agent_brain(brain)
    agent_brain.initialize(world)
    
    # Test agent brain methods
    assert agent_brain.getSensoryInputSize() > 0
    assert agent_brain.getMotorOutputSize() > 0
    print("✓ Agent brain initialization passed")
    
    # Test development features
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    assert agent_brain.isDevelopmentEnabled()
    assert agent_brain.isCuriosityEnabled()
    
    print("✓ Agent brain development features passed")
    
    # Test reward modulation
    agent_brain.applyRewardModulation(1.0, 0.8)
    print("✓ Agent brain reward modulation passed")

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    config = pynlm.Config()
    
    # Test validation errors
    try:
        config.validate(["missing_key"])
        assert False, "Should have raised NLMError"
    except pynlm.NLMError:
        print("✓ Configuration validation error handling passed")
    
    # Test type validation errors
    config.set("int_key", 42)
    try:
        config.get_with_validation("int_key", "string")
        assert False, "Should have raised NLMError"
    except pynlm.NLMError:
        print("✓ Type validation error handling passed")
    
    # Test file operation errors
    try:
        config.load_from_file("nonexistent_file.json")
        assert False, "Should have raised NLMError"
    except pynlm.NLMError:
        print("✓ File operation error handling passed")

def run_all_tests():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    test_basic_functionality()
    test_config_file_operations()
    test_identifier_classes()
    test_sensory_objects()
    test_action_objects()
    test_world_objects()
    test_agent_functionality()
    test_error_handling()
    
    print("\n" + "=" * 50)
    print("All tests passed! ✓")
    print("\nThe NLM Python bindings are working correctly.")

if __name__ == "__main__":
    run_all_tests()
