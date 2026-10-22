#!/usr/bin/env python3

"""
Comprehensive test suite for pynlm Python bindings
Tests all the new functionality added to the NLM Python bindings.
"""

import pynlm
import tempfile
import os
import json
from typing import List, Callable, Any

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("Testing basic functionality...")
    
    # Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test context manager
    with pynlm.PyBrain(brain) as b:
        # Test initialization
        assert b.initialize()
        
        # Test statistics
        assert b.getTotalNeuronCount() > 0
        assert b.getTotalSynapseCount() >= 0
        assert b.getFiringNeuronCount() >= 0
        assert b.getAverageFiringRate() >= 0.0f
        
        # Test step
        b.step(0)
        
        # Test region management
        region_id = b.addRegion("test_region")
        assert region_id.value != 0
        
        region = b.getRegion(region_id)
        assert region is not None
        
        # Test action production
        action = b.produceAction()
        assert action is not None
        print(f"  Produced action: {action.getName()}")
    
    print("  ✓ Basic functionality test passed")

def test_validation_and_error_handling():
    """Test parameter validation and error handling."""
    print("Testing validation and error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test validation of neuron IDs
    invalid_neuron = pynlm.INVALID_NEURON_ID
    try:
        brain.injectCurrent(invalid_neuron, 0.1f)
        assert False, "Should have thrown exception for invalid neuron ID"
    except Exception as e:
        assert "invalid neuron ID" in str(e)
        print(f"  ✓ Correctly rejected invalid neuron ID: {e}")
    
    # Test validation of filepaths
    brain.initialize()
    try:
        brain.save("")
        assert False, "Should have thrown exception for empty filepath"
    except Exception as e:
        assert "Filepath cannot be empty" in str(e) or "length" in str(e)
        print(f"  ✓ Correctly validated filepath: {e}")
    
    print("  ✓ Validation and error handling test passed")

def test_configuration_handling():
    """Test configuration handling."""
    print("Testing configuration handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test setting and getting configuration values
    config.setInt("test_int", 42)
    assert config.getInt("test_int") == 42
    
    config.setFloat("test_float", 3.14)
    assert abs(config.getFloat("test_float") - 3.14) < 0.001
    
    config.setString("test_string", "hello")
    assert config.getString("test_string") == "hello"
    
    config.setBool("test_bool", True)
    assert config.getInt("test_bool") == 1  # bool is stored as int
    
    # Test has method
    assert config.has("test_int")
    assert not config.has("nonexistent_key")
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0
    
    print(f"  ✓ Configuration summary: {summary}")
    print("  ✓ Configuration handling test passed")

def test_sensory_input():
    """Test sensory input classes."""
    print("Testing sensory input...")
    
    # Test Vision
    vision = pynlm.Vision(100, 100, 3)
    data = [0.5] * (100 * 100 * 3)
    vision.setData(data)
    assert vision.getWidth() == 100
    assert vision.getHeight() == 100
    assert vision.getChannels() == 3
    print("  ✓ Vision class test passed")
    
    # Test Audio
    audio = pynlm.Audio(44100, 1024)
    audio_data = [0.1] * 1024
    audio.setData(audio_data)
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1024
    print("  ✓ Audio class test passed")
    
    print("  ✓ Sensory input test passed")

def test_action_classes():
    """Test action classes."""
    print("Testing action classes...")
    
    # Test basic action
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action.getType() == pynlm.ActionType.MoveForward
    assert action.getName() == "MoveForward"
    
    # Test action with parameters
    action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.7])
    assert action2.getType() == pynlm.ActionType.Custom
    params = action2.getParameters()
    assert len(params) == 2
    assert params[0] == 0.5
    
    # Test action cloning
    action3 = action2.clone()
    assert action3.getType() == action2.getType()
    assert action3.getParameters() == action2.getParameters()
    
    # Test action representation
    repr_str = repr(action)
    assert "Action:" in repr_str
    print(f"  ✓ Action representation: {repr_str}")
    
    print("  ✓ Action classes test passed")

def test_world_integration():
    """Test world integration."""
    print("Testing world integration...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(800, 600, 80, 60)
    assert world.getWidth() == 800
    assert world.getHeight() == 600
    
    # Set agent start position
    world.setAgentStart(400, 300)
    
    # Test object manipulation
    obj = pynlm.WorldObject(100, 100, pynlm.WorldObjectType.Resource, 10.0)
    world.addObject(obj)
    
    assert world.isValidPosition(150, 150)
    assert not world.isValidPosition(-10, -10)
    
    # Test agent body
    body = world.getAgentBody()
    assert body is not None
    assert body.x == 400
    assert body.y == 300
    
    print("  ✓ World integration test passed")

def test_simulation_utilities():
    """Test simulation utilities and monitoring."""
    print("Testing simulation utilities...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test simulation monitoring
    monitor = pynlm.monitorSimulation(brain, report_interval=10)
    
    # Simulate a few steps and monitor
    for step in range(20):
        brain.step(step)
        stats = monitor(step)
        assert stats.steps_completed == step + 1
        assert stats.total_spikes >= 0
        print(f"  Step {step}: {stats.to_string()}")
    
    print("  ✓ Simulation utilities test passed")

def test_helper_functions():
    """Test helper utility functions."""
    print("Testing helper functions...")
    
    # Test data validation and sanitization
    data = [1.0, -2.0, 5.0, 0.5]
    sanitized = pynlm.validate_and_sanitize(data, expected_size=4, min_value=-1.0, max_value=2.0)
    assert len(sanitized) == 4
    assert all(-1.0 <= x <= 2.0 for x in sanitized)
    print(f"  ✓ Validation function: {sanitized}")
    
    # Test safe softmax
    logits = [1.0, 2.0, 3.0]
    probs = pynlm.safe_softmax(logits)
    assert abs(sum(probs) - 1.0) < 0.001
    assert all(0.0 <= p <= 1.0 for p in probs)
    print(f"  ✓ Softmax function: {probs}")
    
    # Test safe execute
    def successful_func():
        return "success"
    
    def failing_func():
        raise ValueError("Test error")
    
    assert pynlm.safe_execute(successful_func, "test") == True
    assert pynlm.safe_execute(failing_func, "test") == False
    print("  ✓ Safe execute function works")
    
    print("  ✓ Helper functions test passed")

def test_memory_systems():
    """Test memory system access."""
    print("Testing memory systems...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory system access
    working_memory = brain.getWorkingMemory()
    assert working_memory is not None
    
    episodic_memory = brain.getEpisodicMemory()
    assert episodic_memory is not None
    
    associative_memory = brain.getAssociativeMemory()
    assert associative_memory is not None
    
    # Test prediction system
    prediction_system = brain.getPredictionSystem()
    assert prediction_system is not None
    
    # Test cognition systems
    planner = brain.getPlanner()
    assert planner is not None
    
    concept_formation = brain.getConceptFormation()
    assert concept_formation is not None
    
    attention = brain.getAttention()
    assert attention is not None
    
    # Test development system
    development_system = brain.getDevelopmentSystem()
    assert development_system is not None
    
    # Test neuromodulators
    dopamine = brain.getDopamine()
    assert dopamine is not None
    
    curiosity = brain.getCuriosity()
    assert curiosity is not None
    
    novelty = brain.getNovelty()
    assert novelty is not None
    
    prediction_error = brain.getPredictionErrorSignal()
    assert prediction_error is not None
    
    print("  ✓ Memory systems test passed")

def test_serialization():
    """Test brain serialization and deserialization."""
    print("Testing serialization...")
    
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some simulation steps
    for step in range(10):
        brain1.step(step)
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.pkl', delete=False) as f:
        filepath = f.name
    
    try:
        # Test saving
        result = brain1.save(filepath)
        assert result
        print(f"  ✓ Brain saved to {filepath}")
        
        # Test loading with another brain
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        result = brain2.load(filepath)
        assert result
        print("  ✓ Brain loaded successfully")
        
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    print("  ✓ Serialization test passed")

def test_episode_simulation():
    """Test episode simulation utilities."""
    print("Testing episode simulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Create some dummy observations
    observations = []
    for i in range(5):
        vision = pynlm.Vision(10, 10, 3)
        data = [float(i) % 1.0] * (10 * 10 * 3)
        vision.setData(data)
        observations.append(vision)
    
    # Test episode simulation
    action = pynlm.simulateEpisode(brain, observations, maxSteps=50)
    assert action is not None
    print(f"  ✓ Episode simulation produced action: {action.getName()}")
    
    # Test trial simulation
    action2 = pynlm.runTrial(brain, observations)
    assert action2 is not None
    print(f"  ✓ Trial simulation produced action: {action2.getName()}")
    
    print("  ✓ Episode simulation test passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_validation_and_error_handling,
        test_configuration_handling,
        test_sensory_input,
        test_action_classes,
        test_world_integration,
        test_simulation_utilities,
        test_helper_functions,
        test_memory_systems,
        test_serialization,
        test_episode_simulation,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ Test failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("All tests passed! ✓")
    else:
        print(f"Some tests failed ({failed}). Please check the output above.")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)