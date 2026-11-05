#!/usr/bin/env python3
"""
Backward Compatibility Test for NLM Python Bindings

This script tests that the enhanced Python bindings maintain
full backward compatibility with the original API.
"""

import pynlm
import numpy as np

def test_original_factory_functions():
    """Test original factory functions (camelCase)."""
    print("Testing original factory functions...")
    
    # Test original factory functions
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("  ✓ createDefaultConfig() works")
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("  ✓ createBrain() works")
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    print("  ✓ createSimpleWorld() works")
    
    agent_brain = pynlm.createAgentBrain(brain)
    assert agent_brain is not None
    print("  ✓ createAgentBrain() works")

def test_original_constants():
    """Test original global constants."""
    print("\nTesting original global constants...")
    
    # Test original constants
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    print("  ✓ INVALID_NEURON_ID exists")
    print("  ✓ INVALID_SYNAPSE_ID exists")
    print("  ✓ INVALID_REGION_ID exists")
    print("  ✓ INVALID_POPULATION_ID exists")

def test_original_types():
    """Test original types still work."""
    print("\nTesting original types...")
    
    # Test original types
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.value == 42
    assert neuron_id.index() == 42
    print("  ✓ NeuronId works")
    
    synapse_id = pynlm.SynapseId(123)
    assert synapse_id.value == 123
    print("  ✓ SynapseId works")
    
    region_id = pynlm.RegionId(456)
    assert region_id.value == 456
    print("  ✓ RegionId works")
    
    population_id = pynlm.PopulationId(789)
    assert population_id.value == 789
    print("  ✓ PopulationId works")
    
    # Test original enumerations
    neuron_type = pynlm.NeuronType.Excitatory
    assert neuron_type == pynlm.NeuronType.Excitatory
    print("  ✓ NeuronType enumeration works")
    
    synapse_type = pynlm.SynapseType.Excitatory
    assert synapse_type == pynlm.SynapseType.Excitatory
    print("  ✓ SynapseType enumeration works")
    
    developmental_stage = pynlm.DevelopmentalStage.Adult
    assert developmental_stage == pynlm.DevelopmentalStage.Adult
    print("  ✓ DevelopmentalStage enumeration works")
    
    firing_state = pynlm.FiringState.Active
    assert firing_state == pynlm.FiringState.Active
    print("  ✓ FiringState enumeration works")
    
    action_type = pynlm.ActionType.MoveForward
    assert action_type == pynlm.ActionType.MoveForward
    print("  ✓ ActionType enumeration works")
    
    motor_command = pynlm.MotorCommand.MoveForward
    assert motor_command == pynlm.MotorCommand.MoveForward
    print("  ✓ MotorCommand enumeration works")
    
    world_object_type = pynlm.WorldObjectType.Resource
    assert world_object_type == pynlm.WorldObjectType.Resource
    print("  ✓ WorldObjectType enumeration works")

def test_original_config_api():
    """Test original config API still works."""
    print("\nTesting original config API...")
    
    config = pynlm.createDefaultConfig()
    
    # Test original config methods
    config.set("key", "value")
    assert config.has("key")
    print("  ✓ set() and has() work")
    
    config.saveToFile("/tmp/test_config.json")
    print("  ✓ saveToFile() works")
    
    keys = config.getKeys()
    assert isinstance(keys, list)
    print("  ✓ getKeys() works")
    
    config.clear()
    assert not config.has("key")
    print("  ✓ clear() works")
    
    summary = config.summary()
    assert isinstance(summary, str)
    print("  ✓ summary() works")

def test_original_action_api():
    """Test original action API still works."""
    print("\nTesting original action API...")
    
    action = pynlm.Action()
    
    # Test original action methods
    action.setType(pynlm.ActionType.MoveForward)
    assert action.getType() == pynlm.ActionType.MoveForward
    print("  ✓ setType() and getType() work")
    
    parameters = [0.5, 0.5]
    action.setParameters(parameters)
    assert action.getParameters() == parameters
    print("  ✓ setParameters() and getParameters() work")
    
    action_name = action.getName()
    assert isinstance(action_name, str)
    print("  ✓ getName() works")
    
    cloned_action = action.clone()
    assert cloned_action.getType() == action.getType()
    print("  ✓ clone() works")

def test_original_world_object_api():
    """Test original world object API still works."""
    print("\nTesting original world object API...")
    
    obj = pynlm.WorldObject()
    
    # Test original world object attributes
    obj.x = 1.0
    obj.y = 2.0
    obj.radius = 0.5
    obj.type = pynlm.WorldObjectType.Resource
    obj.value = 5.0
    obj.active = True
    
    assert obj.x == 1.0
    assert obj.y == 2.0
    assert obj.radius == 0.5
    assert obj.type == pynlm.WorldObjectType.Resource
    assert obj.value == 5.0
    assert obj.active == True
    print("  ✓ Original attributes work")

def test_new_pythonic_api():
    """Test new Pythonic API features."""
    print("\nTesting new Pythonic API features...")
    
    # Test Pythonic properties
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.is_valid
    print("  ✓ Pythonic property .is_valid works")
    
    # Test helper methods on enumerations
    neuron_type = pynlm.NeuronType.Excitatory
    assert neuron_type.is_excitatory_or_inhibitory
    assert neuron_type.is_sensory_or_motor
    print("  ✓ Enumeration helper methods work")
    
    # Test safe getters
    config = pynlm.create_default_config()
    config.set("int_key", 42)
    config.set("str_key", "test")
    config.set("bool_key", True)
    config.set("double_key", 3.14)
    
    int_val = config.safe_get_int("int_key", 0)
    assert int_val == 42
    print("  ✓ safe_get_int() works")
    
    str_val = config.safe_get_string("str_key", "")
    assert str_val == "test"
    print("  ✓ safe_get_string() works")
    
    bool_val = config.safe_get_bool("bool_key", False)
    assert bool_val == True
    print("  ✓ safe_get_bool() works")
    
    double_val = config.safe_get_double("double_key", 0.0)
    assert double_val == 3.14
    print("  ✓ safe_get_double() works")
    
    # Test data utilities
    vec = [1.0, 2.0, 3.0, 4.0, 5.0]
    normalized = pynlm.normalize_vector(vec)
    assert all(0.0 <= x <= 1.0 for x in normalized)
    print("  ✓ normalize_vector() works")
    
    stats = pynlm.vector_stats(vec)
    assert "mean" in stats
    assert "std" in stats
    print("  ✓ vector_stats() works")

def test_context_managers():
    """Test context managers."""
    print("\nTesting context managers...")
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    world = pynlm.create_world()
    
    # Test brain context manager
    with pynlm.brain_context(brain) as ctx:
        assert ctx.get_brain() is not None
        print("  ✓ brain_context works")
    
    # Test world context manager
    with pynlm.world_context(world) as ctx:
        assert ctx.get_world() is not None
        print("  ✓ world_context works")
    
    # Test config context manager
    with pynlm.config_context(config) as ctx:
        assert ctx.get_config() is not None
        print("  ✓ config_context works")

def test_new_factory_functions():
    """Test new factory functions (snake_case)."""
    print("\nTesting new factory functions...")
    
    config = pynlm.create_config_with_defaults()
    assert config is not None
    assert config.has("brain.region_count")
    print("  ✓ create_config_with_defaults() works")
    
    config = pynlm.load_config_from_json("/workspace/app/configs/default.json")
    assert config is not None
    print("  ✓ load_config_from_json() works")
    
    world = pynlm.create_world()
    assert world is not None
    print("  ✓ create_world() works")
    
    # Test configure helpers
    configured_world = pynlm.configure_world(world, 20.0, 20.0, 8, 8)
    assert configured_world is not None
    print("  ✓ configure_world() works")
    
    resource_world = pynlm.add_resource_to_world(world, 5.0, 5.0, 1.0)
    assert resource_world is not None
    print("  ✓ add_resource_to_world() works")

def test_exception_handling():
    """Test exception handling."""
    print("\nTesting exception handling...")
    
    # Test that new exceptions are available
    assert hasattr(pynlm, 'NLMException')
    assert hasattr(pynlm, 'ConfigError')
    assert hasattr(pynlm, 'BrainError')
    assert hasattr(pynlm, 'WorldError')
    assert hasattr(pynlm, 'SimulationError')
    print("  ✓ All new exception classes available")
    
    # Test exception inheritance
    try:
        raise pynlm.ConfigError("Test config error")
    except pynlm.NLMException:
        print("  ✓ ConfigError inherits from NLMException")
    
    try:
        raise pynlm.BrainError("Test brain error")
    except pynlm.NLMException:
        print("  ✓ BrainError inherits from NLMException")

def run_all_tests():
    """Run all backward compatibility tests."""
    print("=" * 60)
    print("NLM Python Bindings - Backward Compatibility Test")
    print("=" * 60)
    
    try:
        test_original_factory_functions()
        test_original_constants()
        test_original_types()
        test_original_config_api()
        test_original_action_api()
        test_original_world_object_api()
        
        print("\n" + "-" * 60)
        print("ORIGINAL API TESTS COMPLETED SUCCESSFULLY")
        print("-" * 60)
        
        test_new_pythonic_api()
        test_context_managers()
        test_new_factory_functions()
        test_exception_handling()
        
        print("\n" + "-" * 60)
        print("NEW PYTHONIC FEATURES TESTS COMPLETED SUCCESSFULLY")
        print("-" * 60)
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED! ✓")
        print("=" * 60)
        print("\nThe enhanced Python bindings maintain full")
        print("backward compatibility with the original API.")
        print("You can now use both:")
        print("  - Original camelCase API (createDefaultConfig, etc.)")
        print("  - New snake_case API (create_default_config, etc.)")
        print("  - Pythonic features (properties, helpers, context managers)")
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    return True

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)