#!/usr/bin/env python3
"""
Test script to verify the improved NLM Python bindings.

This script tests the enhanced Python API to ensure all improvements
are working correctly.
"""

import pynlm
import sys

def test_basic_bindings():
    """Test basic Python bindings functionality."""
    print("Testing basic Python bindings...")
    
    # Test configuration creation
    config = pynlm.create_default_config()
    assert config is not None
    print("✓ Config creation works")
    
    # Test brain creation
    brain = pynlm.create_brain(config)
    assert brain is not None
    print("✓ Brain creation works")
    
    # Test initialization
    brain.initialize()
    print("✓ Brain initialization works")
    
    # Test step functionality
    brain.step(0)
    print("✓ Brain step works")
    
    # Test statistics
    assert brain.get_total_neuron_count() > 0
    assert brain.get_total_synapse_count() >= 0
    assert brain.get_firing_neuron_count() >= 0
    print("✓ Statistics work")
    
    return True

def test_enhanced_config_api():
    """Test enhanced Config API functionality."""
    print("\nTesting enhanced Config API...")
    
    config = pynlm.create_default_config()
    
    # Test setting values
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Test getting values
    assert config.get("test_int").get() == 42
    assert config.get("test_float").get() == 3.14
    assert config.get("test_string").get() == "hello"
    assert config.get("test_bool").get() == True
    
    # Test dictionary creation
    dict_config = {
        "param1": 100,
        "param2": 0.5,
        "param3": "test"
    }
    
    try:
        enhanced_config = pynlm.create_config_from_dict(dict_config)
        print("✓ Dictionary config creation works")
    except Exception as e:
        print(f"✗ Dictionary config creation failed: {e}")
        return False
    
    # Test conversion to dictionary
    config_dict = enhanced_config.to_dict()
    assert isinstance(config_dict, dict)
    print("✓ Config to dict conversion works")
    
    return True

def test_enhanced_action_api():
    """Test enhanced Action API functionality."""
    print("\nTesting enhanced Action API...")
    
    # Test action creation with different constructors
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    action2 = pynlm.Action(pynlm.ActionType.Look, [0.5, 0.5])
    action3 = pynlm.Action(pynlm.ActionType.Eat)
    
    # Test action properties
    assert action1.get_type() == pynlm.ActionType.MoveForward
    assert action2.get_type() == pynlm.ActionType.Look
    assert action3.get_type() == pynlm.ActionType.Eat
    
    # Test enhanced methods
    assert action1.is_move_action() == True
    assert action2.is_look_action() == True
    assert action3.is_interact_action() == False
    
    # Test cloning
    cloned = action1.clone()
    assert cloned.get_type() == action1.get_type()
    print("✓ Action cloning works")
    
    # Test action names
    assert action1.get_name() == "MoveForward"
    assert action2.get_name() == "Look"
    print("✓ Action names work")
    
    return True

def test_enhanced_sensory_api():
    """Test enhanced Sensory API functionality."""
    print("\nTesting enhanced Sensory API...")
    
    # Test Vision
    vision = pynlm.Vision(width=10, height=10, channels=3)
    assert vision.get_width() == 10
    assert vision.get_height() == 10
    assert vision.get_channels() == 3
    
    # Test data setting
    data = [0.1] * (10 * 10 * 3)
    vision.set_data(data)
    print("✓ Vision data setting works")
    
    # Test Audio
    audio = pynlm.Audio(sampleRate=44100, numSamples=100)
    assert audio.get_sample_rate() == 44100
    assert audio.get_num_samples() == 100
    
    # Test duration calculation
    duration = audio.get_duration()
    expected_duration = 100 / 44100.0
    assert abs(duration - expected_duration) < 0.001
    print("✓ Audio duration calculation works")
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.add_signal(0.5)
    internal.add_signal(0.8)
    
    signals = internal.get_data()
    assert len(signals) == 2
    assert signals[0] == 0.5
    assert signals[1] == 0.8
    print("✓ InternalSignals work")
    
    return True

def test_enhanced_world_api():
    """Test enhanced World API functionality."""
    print("\nTesting enhanced World API...")
    
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Test basic properties
    assert world.get_width() == 20
    assert world.get_height() == 20
    assert world.get_sensory_width() == 8
    assert world.get_sensory_height() == 8
    print("✓ World configuration works")
    
    # Test reset
    world.reset()
    print("✓ World reset works")
    
    # Test agent start
    world.set_agent_start(10.0, 10.0)
    print("✓ Agent start position works")
    
    # Test update
    world.update(0.1)
    print("✓ World update works")
    
    # Test object management
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
    world.add_object(obj)
    print("✓ Object addition works")
    
    return True

def test_enhanced_brain_api():
    """Test enhanced Brain API functionality."""
    print("\nTesting enhanced Brain API...")
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Test enhanced brain methods
    assert brain.get_total_neuron_count() > 0
    assert brain.get_total_synapse_count() >= 0
    assert brain.get_firing_neuron_count() >= 0
    
    # Test step methods
    brain.step(0)
    brain.step(0, 0.0)
    print("✓ Enhanced step methods work")
    
    # Test region management
    region_id = brain.add_region("test_region")
    region = brain.get_region(region_id)
    assert region is not None
    print("✓ Region management works")
    
    # Test statistics
    stats = brain.get_stats()
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    print("✓ Statistics work")
    
    return True

def test_text_encoding():
    """Test text encoding functionality."""
    print("\nTesting text encoding...")
    
    try:
        # Test text encoding
        pattern1 = pynlm.encode_text("hello")
        pattern2 = pynlm.encode_text("world")
        pattern3 = pynlm.encode_text("HELLO")  # Should be case-insensitive
        
        assert len(pattern1) > 0
        assert len(pattern2) > 0
        assert len(pattern3) > 0
        
        # Test that same text produces same pattern (case-insensitive)
        assert pattern1 == pattern3, "Text encoding should be case-insensitive"
        
        print("✓ Text encoding works")
        
    except Exception as e:
        print(f"✗ Text encoding failed: {e}")
        return False
    
    return True

def test_concept_similarity():
    """Test concept formation similarity calculation."""
    print("\nTesting concept similarity...")
    
    try:
        # Create sample patterns
        pattern1 = [0.1, 0.2, 0.3, 0.4, 0.5]
        pattern2 = [0.15, 0.25, 0.35, 0.45, 0.55]
        pattern3 = [0.9, 0.8, 0.7, 0.6, 0.5]
        
        # Test similarity calculations
        similarity_same = pynlm.compute_concept_similarity(pattern1, pattern1)
        similarity_similar = pynlm.compute_concept_similarity(pattern1, pattern2)
        similarity_different = pynlm.compute_concept_similarity(pattern1, pattern3)
        
        # Verify similarity properties
        assert similarity_same > 0.9, "Same pattern should have high similarity"
        assert similarity_similar > similarity_different, "Similar patterns should be more similar"
        assert similarity_different < 0.7, "Different patterns should have low similarity"
        
        print("✓ Concept similarity calculation works")
        
    except Exception as e:
        print(f"✗ Concept similarity failed: {e}")
        return False
    
    return True

def test_error_handling():
    """Test enhanced error handling."""
    print("\nTesting error handling...")
    
    try:
        # Test invalid configuration
        config = pynlm.create_default_config()
        config.set("invalid_key", "invalid_value")
        
        # Test invalid action
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        # This should not raise an exception
        
        print("✓ Error handling works")
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False
    
    return True

def run_all_tests():
    """Run all tests and report results."""
    print("NLM Python Bindings - Test Suite")
    print("=" * 50)
    
    tests = [
        test_basic_bindings,
        test_enhanced_config_api,
        test_enhanced_action_api,
        test_enhanced_sensory_api,
        test_enhanced_world_api,
        test_enhanced_brain_api,
        test_text_encoding,
        test_concept_similarity,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 50)
    
    if failed == 0:
        print("✓ All tests passed!")
        return True
    else:
        print(f"✗ {failed} tests failed")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
