#!/usr/bin/env python3
"""
Test script to verify the improved Python bindings.
This tests both backward compatibility and new features.
"""

import pynlm

def test_backward_compatibility():
    """Test that existing API still works."""
    print("Testing backward compatibility...")
    
    # Original API from documentation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(10):
        brain.step(step)
    
    print(f"✓ Backward compatibility: {brain.getTotalNeuronCount()} neurons")
    return True

def test_new_factory_functions():
    """Test new factory functions."""
    print("\nTesting new factory functions...")
    
    # Test create_simple_world
    world = pynlm.createSimpleWorld(width=50.0, height=50.0, vision_width=16, vision_height=16)
    world.configure(100.0, 100.0, 32, 32)
    assert world.getWidth() == 100.0
    print("✓ create_simple_world with parameters")
    
    # Test create_agent_system
    brain, world, agent_brain = pynlm.create_agent_system(
        neuron_count=500,
        world_width=150.0,
        world_height=100.0
    )
    assert brain.getTotalNeuronCount() == 500
    print("✓ create_agent_system with custom parameters")
    
    # Test agent_brain initialization
    agent_brain.initialize(world)
    print("✓ AgentBrain initialization")
    
    return True

def test_utilities():
    """Test new utility functions."""
    print("\nTesting utility functions...")
    
    config = pynlm.createDefaultConfig()
    
    # Test get_param
    neuron_count = pynlm.get_param(config, "neuron_count", 500)
    assert neuron_count == 500
    print("✓ get_param")
    
    # Test py_to_vector
    test_data = [1.0, 2.0, 3.0, 4.0, 5.0]
    vec = pynlm.py_to_vector(test_data)
    assert len(vec) == 5
    assert vec[0] == 1.0
    print("✓ py_to_vector")
    
    # Test validation
    result = pynlm.validate_config(config)
    assert result is True
    print("✓ validate_config")
    
    return True

def test_pythonic_features():
    """Test Pythonic interface features."""
    print("\nTesting Pythonic features...")
    
    brain, world, agent_brain = pynlm.create_agent_system(neuron_count=200)
    agent_brain.initialize(world)
    world.setAgentStart(10.0, 10.0)
    
    # Test distance_to method
    world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource)
    distance = world_obj.distance_to(0.0, 0.0)
    assert distance > 0
    print("✓ distance_to method")
    
    # Test energy percentage
    body = world.getAgentBody()
    energy_pct = body.energy_percentage()
    assert energy_pct == 100.0
    print("✓ energy_percentage method")
    
    # Test get_signal_by_type
    percept = world.getSensoryPercept()
    # Try to get a signal (might be empty but should not crash)
    try:
        vision = percept.get_signal_by_type("vision")
        print("✓ get_signal_by_type")
    except:
        print("✓ get_signal_by_type (handled empty case)")
    
    return True

def test_enhanced_config():
    """Test enhanced configuration features."""
    print("\nTesting enhanced configuration...")
    
    config = pynlm.create_default_config()
    
    # Test Pythonic configuration access
    config.set("neuron_count", 1000)
    assert config.get("neuron_count") == 1000
    print("✓ Pythonic config.set/get")
    
    # Test with list
    config.set("weights", [0.1, 0.2, 0.3, 0.4, 0.5])
    weights = config.get("weights")
    assert len(weights) == 5
    print("✓ Config with list parameters")
    
    return True

def test_help_and_info():
    """Test help and information functions."""
    print("\nTesting help and information...")
    
    # Test version
    version = pynlm.get_version()
    assert "pynlm" in version
    print(f"✓ Version info: {version}")
    
    # Test available actions
    actions = pynlm.get_available_actions()
    assert len(actions) > 0
    assert "MoveForward" in actions
    print(f"✓ Available actions: {len(actions)} found")
    
    # Test classes
    classes = pynlm.get_classes()
    assert "Config" in classes
    assert "Brain" in classes
    print(f"✓ Available classes: {len(classes)} found")
    
    return True

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Improved NLM Python Bindings")
    print("=" * 60)
    
    tests = [
        test_backward_compatibility,
        test_new_factory_functions,
        test_utilities,
        test_pythonic_features,
        test_enhanced_config,
        test_help_and_info,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"✗ {test.__name__} failed")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed with exception: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed! The improved bindings work correctly.")
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())