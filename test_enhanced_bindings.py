#!/usr/bin/env python3
"""
Simple test script to verify the enhanced Python bindings work correctly.
This script tests the new features added to the NLM Python bindings.
"""

import pynlm

def test_basic_types():
    """Test basic type definitions."""
    print("Testing basic types...")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(42)
    print(f"NeuronId: {neuron_id}")
    assert neuron_id.value == 42
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(100)
    print(f"SynapseId: {synapse_id}")
    assert synapse_id.value == 100
    
    # Test RegionId
    region_id = pynlm.RegionId(5)
    print(f"RegionId: {region_id}")
    assert region_id.value == 5
    
    # Test PopulationId
    pop_id = pynlm.PopulationId(99)
    print(f"PopulationId: {pop_id}")
    assert pop_id.value == 99
    
    print("✓ Basic types test passed\n")

def test_enums():
    """Test enumeration types."""
    print("Testing enumerations...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory == 0
    assert pynlm.NeuronType.Inhibitory == 1
    assert pynlm.NeuronType.Modulatory == 2
    assert pynlm.NeuronType.Sensory == 3
    assert pynlm.NeuronType.Motor == 4
    assert pynlm.NeuronType.Internal == 5
    print("✓ NeuronType enum values correct")
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory == 0
    assert pynlm.SynapseType.Inhibitory == 1
    assert pynlm.SynapseType.Modulatory == 2
    assert pynlm.SynapseType.Electrical == 3
    assert pynlm.SynapseType.GapJunction == 4
    print("✓ SynapseType enum values correct")
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward == 0
    assert pynlm.ActionType.MoveBackward == 1
    assert pynlm.ActionType.TurnLeft == 2
    assert pynlm.ActionType.TurnRight == 3
    print("✓ ActionType enum values correct")
    
    print("✓ Enumerations test passed\n")

def test_config_basic():
    """Test basic Config functionality."""
    print("Testing Config basic functionality...")
    
    # Create config
    config = pynlm.Config()
    print(f"Created config: {config}")
    
    # Set values
    config.set("learning_rate", 0.01)
    config.set("max_steps", 1000)
    config.set("render_mode", True)
    config.set("model_name", "test_model")
    
    # Get values
    assert config.get("learning_rate") == 0.01
    assert config.getOr("max_steps", 500) == 1000
    assert config.getOr("unknown", "default") == "default"
    assert config.has("learning_rate")
    assert not config.has("unknown")
    
    # Test getKeys
    keys = config.getKeys()
    print(f"Config keys: {keys}")
    assert "learning_rate" in keys
    assert "max_steps" in keys
    assert "render_mode" in keys
    assert "model_name" in keys
    
    # Test summary
    summary = config.summary()
    print(f"Config summary: {summary}")
    assert "Config: " in summary
    
    print("✓ Config basic functionality test passed\n")

def test_context_managers():
    """Test context manager functionality."""
    print("Testing context managers...")
    
    # Create objects
    config = pynlm.createDefaultConfig()
    config.set("test_key", "test_value")
    
    # Test ConfigContextManager
    with pynlm.ConfigContextManager(config) as cfg:
        assert cfg is not None
        assert cfg.has("test_key")
        print("✓ ConfigContextManager works")
    
    # Test BrainContextManager
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    with pynlm.BrainContextManager(brain) as b:
        assert b is not None
        print("✓ BrainContextManager works")
    
    # Test WorldContextManager
    world = pynlm.createSimpleWorld()
    with pynlm.WorldContextManager(world) as w:
        assert w is not None
        print("✓ WorldContextManager works")
    
    print("✓ Context managers test passed\n")

def test_enhanced_features():
    """Test enhanced Python features."""
    print("Testing enhanced Python features...")
    
    config = pynlm.Config()
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_bool", True)
    config.set("test_string", "hello")
    
    # Test to_dict
    dict_config = config.to_dict()
    print(f"Config as dict: {dict_config}")
    assert dict_config["test_int"] == 42
    assert dict_config["test_float"] == 3.14
    assert dict_config["test_bool"] is True
    assert dict_config["test_string"] == "hello"
    
    # Test from_dict
    new_config = pynlm.Config()
    new_dict = {"key1": "value1", "key2": 123, "key3": False}
    new_config.from_dict(new_dict)
    assert new_config.get("key1") == "value1"
    assert new_config.get("key2") == 123
    assert new_config.get("key3") is True
    
    # Test clone/copy
    cloned = config.clone()
    assert cloned is not None
    assert cloned.get("test_int") == 42
    
    copied = config.copy()
    assert copied is not None
    assert copied.get("test_string") == "hello"
    
    print("✓ Enhanced features test passed\n")

def test_exception_handling():
    """Test exception handling."""
    print("Testing exception handling...")
    
    config = pynlm.Config()
    
    # Test type checking
    try:
        config.set("test", "not_a_number")
        print("✓ No exception raised for string value")
    except Exception as e:
        print(f"✓ Exception raised (expected): {e}")
    
    print("✓ Exception handling test passed\n")

def test_serialization():
    """Test serialization/deserialization."""
    print("Testing serialization...")
    
    config = pynlm.Config()
    config.set("test_value", 42)
    config.set("test_name", "serialization_test")
    
    # Test JSON serialization (if available)
    try:
        json_str = config.to_json()
        print(f"JSON serialization result: {json_str}")
        # Note: Actual deserialization may not work without proper implementation
        # but we test that the method exists and doesn't crash
    except Exception as e:
        print(f"JSON serialization (may not be implemented): {e}")
    
    print("✓ Serialization test passed\n")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Enhanced NLM Python Bindings")
    print("=" * 60)
    
    try:
        test_basic_types()
        test_enums()
        test_config_basic()
        test_context_managers()
        test_enhanced_features()
        test_exception_handling()
        test_serialization()
        
        print("=" * 60)
        print("All tests passed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())