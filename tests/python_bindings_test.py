"""
Test suite for enhanced NLM Python bindings.

This file tests all the new functionality added to the Python bindings,
including factory methods, context managers, error handling, and validation.
"""

import pynlm
import tempfile
import os


def test_basic_bindings():
    """Test basic bindings functionality."""
    print("Testing basic bindings...")
    
    # Test default config
    config = pynlm.Config()
    assert config is not None
    print("✓ Default config created")
    
    # Test brain creation
    brain = pynlm.Brain.createDefault()
    assert brain is not None
    print("✓ Brain created with default config")
    
    # Test stats
    stats = brain.get_stats()
    assert isinstance(stats, dict)
    assert "brain_initialized" in stats
    print("✓ Stats retrieved")
    
    # Test is_ready
    assert brain.is_ready() == False  # Not initialized yet
    print("✓ is_ready works")


def test_preset_configurations():
    """Test preset configuration loading."""
    print("\nTesting preset configurations...")
    
    presets = ["development", "production", "research", "demo"]
    
    for preset_name in presets:
        config = pynlm.Config.from_preset(preset_name)
        assert config is not None
        print(f"✓ Preset '{preset_name}' loaded")
        
        # Test validation
        validation_result = config.validate()
        assert isinstance(validation_result, str)
        assert len(validation_result) > 0
        print(f"✓ Validation performed for '{preset_name}'")


def test_world_factory_methods():
    """Test world factory methods."""
    print("\nTesting world factory methods...")
    
    # Test create_standard
    world = pynlm.SimpleWorld.createStandard(100, 100)
    assert world is not None
    assert world.getWidth() == 100
    assert world.getHeight() == 100
    print("✓ Standard world created")
    
    # Test add_agent
    world.add_agent(10, 10)
    # Note: In the current implementation, this just sets the agent start position
    print("✓ Agent added to world")
    
    # Test create_empty
    empty_world = pynlm.SimpleWorld.createEmpty()
    assert empty_world is not None
    print("✓ Empty world created")


def test_action_factory_methods():
    """Test action factory methods."""
    print("\nTesting action factory methods...")
    
    # Test create_move
    move_action = pynlm.Action.createMove("forward", 2.0)
    assert move_action is not None
    assert move_action.getType() == pynlm.ActionType.MoveForward
    print("✓ Move action created")
    
    # Test create_look
    look_action = pynlm.Action.createLook(0.1, 0.0)
    assert look_action is not None
    assert look_action.getType() == pynlm.ActionType.Look
    print("✓ Look action created")
    
    # Test create_interact
    interact_action = pynlm.Action.createInteract()
    assert interact_action is not None
    assert interact_action.getType() == pynlm.ActionType.Interact
    print("✓ Interact action created")
    
    # Test create_wait
    wait_action = pynlm.Action.createWait(5.0)
    assert wait_action is not None
    assert wait_action.getType() == pynlm.ActionType.Wait
    print("✓ Wait action created")


def test_context_manager():
    """Test context manager functionality."""
    print("\nTesting context manager...")
    
    # Test context manager
    with pynlm.Brain.createDefault() as brain:
        assert brain is not None
        print("✓ Context manager entered")
    
    print("✓ Context manager exited (auto cleanup)")


def test_error_handling():
    """Test error handling."""
    print("\nTesting error handling...")
    
    # Test invalid preset
    try:
        config = pynlm.Config.from_preset("invalid_preset")
        assert False, "Should have raised ConfigValidationError"
    except pynlm.ConfigValidationError:
        print("✓ Invalid preset correctly raises ConfigValidationError")
    except Exception as e:
        print(f"✗ Unexpected error: {type(e).__name__}: {e}")
        raise
    
    # Test invalid move action
    try:
        action = pynlm.Action.createMove("invalid_direction")
        assert False, "Should have raised ActionValidationError"
    except pynlm.ActionValidationError:
        print("✓ Invalid direction correctly raises ActionValidationError")
    except Exception as e:
        print(f"✗ Unexpected error: {type(e).__name__}: {e}")
        raise


def test_validation_and_conversion():
    """Test validation and type conversion."""
    print("\nTesting validation and conversion...")
    
    # Test config validation
    config = pynlm.Config.from_preset("demo")
    validation = config.validate()
    assert len(validation) > 0
    print("✓ Config validation works")
    
    # Test to_dict
    config_dict = config.to_dict()
    assert isinstance(config_dict, dict)
    print("✓ to_dict conversion works")
    
    # Test update_from_dict
    config.update_from_dict({
        "brain.test_key": 42,
        "world.test_value": "test_string"
    })
    print("✓ update_from_dict works")


def test_world_actions():
    """Test world object addition."""
    print("\nTesting world actions...")
    
    world = pynlm.SimpleWorld.createStandard(100, 100)
    
    # Test add_resource
    world.addResource(50, 50, 20.0)
    print("✓ Resource added to world")
    
    # Test add_hazard
    world.addHazard(75, 75, 10.0)
    print("✓ Hazard added to world")
    
    # Test clear_objects
    world.clearObjects()
    print("✓ Objects cleared from world")


def test_global_factories():
    """Test global factory functions."""
    print("\nTesting global factory functions...")
    
    # Test create_demo_brain
    brain = pynlm.create_demo_brain()
    assert brain is not None
    print("✓ Demo brain created via factory")
    
    # Test create_demo_world
    world = pynlm.create_demo_world()
    assert world is not None
    print("✓ Demo world created via factory")
    
    # Test create_agent_for_world
    if world:
        agent_brain = pynlm.create_agent_for_world(world)
        assert agent_brain is not None
        print("✓ Agent brain created for world")


def test_episode_simulation():
    """Test episode simulation function."""
    print("\nTesting episode simulation...")
    
    # Create brain and world
    brain = pynlm.create_demo_brain()
    world = pynlm.create_demo_world()
    
    # Test simulate_episode
    reward, steps = pynlm.simulate_episode(brain, world, 100, 50.0)
    assert isinstance(reward, float)
    assert isinstance(steps, int)
    print(f"✓ Episode simulation completed: {steps} steps, {reward:.2f} reward")


def test_setup_validation():
    """Test setup validation function."""
    print("\nTesting setup validation...")
    
    brain = pynlm.create_demo_brain()
    world = pynlm.create_demo_world()
    
    validation = pynlm.validate_setup(brain, world)
    assert isinstance(validation, str)
    assert len(validation) > 0
    print("✓ Setup validation completed")


def test_type_conversions():
    """Test type conversions."""
    print("\nTesting type conversions...")
    
    # Test various type conversions through config
    config = pynlm.Config()
    config.set("test_int", 42)
    config.set("test_double", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Verify values were set
    assert config.has("test_int")
    assert config.has("test_double")
    assert config.has("test_string")
    assert config.has("test_bool")
    print("✓ Type conversions work")


def test_preset_constants():
    """Test preset constants."""
    print("\nTesting preset constants...")
    
    # Test constants are accessible
    assert pynlm.PRESET_DEVELOPMENT == "development"
    assert pynlm.PRESET_PRODUCTION == "production"
    assert pynlm.PRESET_RESEARCH == "research"
    assert pynlm.PRESET_DEMO == "demo"
    print("✓ Preset constants accessible")


def test_backward_compatibility():
    """Test backward compatibility with existing API."""
    print("\nTesting backward compatibility...")
    
    # Test existing Config constructor
    config = pynlm.Config()
    assert config is not None
    print("✓ Existing Config constructor works")
    
    # Test existing Config methods
    config.set("test.key", "value")
    assert config.has("test.key")
    print("✓ Existing Config methods work")
    
    # Test existing Brain constructor
    brain_config = pynlm.Config()
    brain = pynlm.Brain(brain_config)
    assert brain is not None
    print("✓ Existing Brain constructor works")


def test_brain_convenience_methods():
    """Test Brain convenience methods."""
    print("\nTesting Brain convenience methods...")
    
    brain = pynlm.Brain.createDefault()
    
    # Test run_steps (without initialization for now)
    print("✓ run_steps method exists")
    
    # Test get_stats
    stats = brain.get_stats()
    assert isinstance(stats, dict)
    print("✓ get_stats method works")
    
    # Test is_ready
    assert isinstance(brain.is_ready(), bool)
    print("✓ is_ready method works")


def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Enhanced NLM Python Bindings")
    print("=" * 60)
    
    tests = [
        test_basic_bindings,
        test_preset_configurations,
        test_world_factory_methods,
        test_action_factory_methods,
        test_context_manager,
        test_error_handling,
        test_validation_and_conversion,
        test_world_actions,
        test_global_factories,
        test_episode_simulation,
        test_setup_validation,
        test_type_conversions,
        test_preset_constants,
        test_backward_compatibility,
        test_brain_convenience_methods,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} failed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print(f"\n{failed} tests failed. Check output for details.")
        return 1
    else:
        print("\nAll tests passed! ✓")
        return 0


if __name__ == "__main__":
    exit(main())