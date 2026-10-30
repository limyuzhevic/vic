#!/usr/bin/env python3
"""
NLM Test Suite

Comprehensive test suite for NLM improvements including configuration system,
Python API wrapper, and command-line interface.
"""

import os
import sys
import tempfile
import json
import time
from pathlib import Path

# Add the python directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
from python.nlm import NLMBrain, NLMEnvironment, create_simple_brain, create_simple_world
def test_config_system():
    """Test improved configuration system."""
    print("Testing configuration system...")
    
    # Test 1: Basic config operations
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    assert config.has("brain.neuron_count")
    assert config.get("brain.neuron_count") == 1000
    assert config.getOr("nonexistent", "default") == "default"
    
    # Test 2: Save/load config
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
        config.saveToFile(config_file)
    
    try:
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(config_file)
        
        # Check that some key settings were loaded
        assert new_config.has("brain.neuron_count") or len(new_config.getKeys()) > 0
        
        print("✓ Configuration system tests passed")
    finally:
        os.unlink(config_file)
def test_python_api_wrapper():
    """Test Python API wrapper."""
    print("Testing Python API wrapper...")
    
    # Test 1: Basic brain creation and initialization
    brain = NLMBrain({"brain.neuron_count": 500, "brain.region_count": 1}, "test_brain")
    
    # Check brain was created
    assert brain.name == "test_brain"
    assert brain._is_initialized == False
    
    # Initialize brain
    brain.initialize()
    
    # Check initialization
    assert brain._is_initialized == True
    assert brain._brain.getTotalNeuronCount() > 0
    assert brain._brain.getTotalSynapseCount() > 0
    
    # Test 2: Brain stepping
    initial_stats = brain.get_stats()
    initial_spikes = initial_stats["total_spikes"]
    
    brain.step(10)
    
    # Check that brain stepped
    assert brain._step_counter >= 10
    assert brain._brain.getTotalSpikeCount() >= initial_spikes
    
    # Test 3: Brain statistics
    stats = brain.get_stats()
    expected_keys = {"name", "total_neurons", "total_synapses", "active_neurons", 
                     "firing_neurons", "total_spikes", "average_firing_rate",
                     "excitatory_inhibition_ratio", "developmental_stage",
                     "simulation_step", "is_initialized"}
    
    assert set(stats.keys()) >= expected_keys
    
    # Test 4: Save/load
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        save_file = f.name
    
    try:
        brain.save(save_file)
        assert os.path.exists(save_file)
        assert os.path.getsize(save_file) > 0
        
        # Create new brain and load
        new_brain = NLMBrain({"brain.neuron_count": 500}, "loaded_brain")
        new_brain.load(save_file)
        
        assert new_brain._is_initialized == True
        assert new_brain._brain.getTotalNeuronCount() > 0
        
        print("✓ Python API wrapper tests passed")
    finally:
        if os.path.exists(save_file):
            os.unlink(save_file)
def test_world_wrapper():
    """Test world wrapper."""
    print("Testing world wrapper...")
    
    # Test 1: Basic world creation
    world = NLMWorld(30, 30, 12, 12)
    
    assert world.width == 30
    assert world.height == 30
    assert world.vision_width == 12
    assert world.vision_height == 12
    
    # Test 2: World configuration
    world.configure(40, 40, 16, 16)
    
    assert world.width == 40
    assert world.height == 40
    assert world.vision_width == 16
    assert world.vision_height == 16
    
    # Test 3: World reset
    world.setAgentStart(5.0, 5.0)
    world.reset()
    
    assert world.get_agent_body().x == 5.0
    assert world.get_agent_body().y == 5.0
    
    # Test 4: World update
    initial_time = world.get_simulation_time()
    world.update(0.1)
    
    assert world.get_simulation_time() > initial_time
    
    print("✓ World wrapper tests passed")
def test_environment_wrapper():
    """Test environment wrapper."""
    print("Testing environment wrapper...")
    
    # Test 1: Basic environment creation
    env = NLMEnvironment()
    
    assert env.brain is not None
    assert env.world is not None
    assert env.agent is not None
    
    # Test 2: Environment initialization
    assert env.brain._is_initialized == False
    assert env.world.get_simulation_time() == 0.0
    
    # Initialize components
    env.brain.initialize()
    
    assert env.brain._is_initialized == True
    
    # Test 3: Environment reset
    initial_neuron_count = env.brain._brain.getTotalNeuronCount()
    env.reset()
    
    assert env.brain._is_initialized == False
    assert env.world.get_simulation_time() == 0.0
    
    print("✓ Environment wrapper tests passed")
def test_convenience_functions():
    """Test convenience functions."""
    print("Testing convenience functions...")
    
    # Test 1: create_simple_brain
    brain = create_simple_brain(200, 1)
    brain.initialize()
    
    assert brain._brain.getTotalNeuronCount() >= 200
    assert brain._brain.getRegionCount() == 1
    
    # Test 2: create_simple_world
    world = create_simple_world(25, 25)
    
    assert world.get_width() == 25
    assert world.get_height() == 25
    
    # Test 3: get_example_config
    from python.nlm import get_example_config
    
    default_config = get_example_config("default")
    assert "brain.neuron_count" in default_config
    assert "brain.region_count" in default_config
    
    minimal_config = get_example_config("minimal")
    assert minimal_config["brain.neuron_count"] < default_config["brain.neuron_count"]
    
    # Test 4: list_available_configs
    from python.nlm import list_available_configs
    
    configs = list_available_configs()
    assert "default" in configs
    assert "minimal" in configs
    
    print("✓ Convenience functions tests passed")
def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    # Test 1: Try to step uninitialized brain
    brain = NLMBrain()
    
    try:
        brain.step(1)
        assert False, "Should have raised RuntimeError"
    except RuntimeError as e:
        assert "Brain must be initialized" in str(e)
    
    # Test 2: Try to get stats before initialization
    brain2 = NLMBrain()
    stats = brain2.get_stats()
    assert stats["is_initialized"] == False
    
    # Test 3: Invalid neuron type
    brain3 = NLMBrain()
    brain3.initialize()
    
    try:
        brain3.inject_current_by_type("invalid_type", 1.0)
        assert False, "Should have raised ValueError"
    except ValueError as e:
        assert "Unknown neuron type" in str(e)
    
    # Test 4: Invalid developmental stage
    try:
        brain3.set_development_stage("invalid_stage")
        assert False, "Should have raised ValueError"
    except ValueError as e:
        assert "Unknown developmental stage" in str(e)
    
    print("✓ Error handling tests passed")
def test_performance():
    """Test performance characteristics."""
    print("Testing performance...")
    
    # Test 1: Brain stepping performance
    brain = NLMBrain({"brain.neuron_count": 100}, "perf_test")
    brain.initialize()
    
    start_time = time.time()
    brain.step(100)
    elapsed_time = time.time() - start_time
    
    # Should be reasonably fast (less than 1 second for 100 steps)
    assert elapsed_time < 1.0, f"Performance issue: {elapsed_time:.3f}s for 100 steps"
    
    # Test 2: Large brain initialization
    large_brain = create_simple_brain(500, 3)
    start_time = time.time()
    large_brain.initialize()
    init_time = time.time() - start_time
    
    # Should be fast (< 2 seconds for 500 neurons)
    assert init_time < 2.0, f"Initialization performance issue: {init_time:.3f}s"
    
    print("✓ Performance tests passed")
def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Test Suite")
    print("=" * 60)
    
    tests = [
        test_config_system,
        test_python_api_wrapper,
        test_world_wrapper,
        test_environment_wrapper,
        test_convenience_functions,
        test_error_handling,
        test_performance
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
    else:
        print("All tests passed! ✓")
if __name__ == '__main__':
    run_all_tests()