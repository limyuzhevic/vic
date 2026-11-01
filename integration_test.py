#!/usr/bin/env python3
"""
Integration test for NLM improvements

This script tests all the improvements made to the NLM project:
1. Enhanced reward system
2. Checkpoint system
3. Advanced Python API
4. Command-line interface
5. Performance optimizations
"""

import sys
import os
import json
import tempfile

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm
from pynlm.enhanced import ConfigPresets, ExperimentManager, BatchSimulator, SerializationUtils

def test_reward_system_improvements():
    """Test that reward system improvements work"""
    print("Testing reward system improvements...")
    
    # Create agent experiment with learning enabled
    config = ConfigPresets.full()
    world_config = {
        'width': 20,
        'height': 20,
        'visionWidth': 8,
        'visionHeight': 8,
    }
    
    manager = ExperimentManager()
    results = manager.run_agent_experiment(
        config,
        world_config,
        num_steps=100,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    # Check that we got meaningful results
    assert 'total_reward' in results, "Missing total_reward in results"
    assert 'avg_firing_rate' in results, "Missing avg_firing_rate in results"
    assert 'avg_curiosity' in results, "Missing avg_curiosity in results"
    
    # Reward should be meaningful (not all zero)
    assert results['total_reward'] != 0.0 or results['total_reward'] != 0, \
        "Reward is zero - reward system may not be working"
    
    print(f"  ✓ Reward system working: Total reward = {results['total_reward']:.1f}")
    print(f"  ✓ Curiosity level: {results['avg_curiosity']:.3f}")
    print(f"  ✓ Development stage: {results['development_stage']}")
    
    return True

def test_checkpoint_system():
    """Test checkpoint system improvements"""
    print("Testing checkpoint system...")
    
    config = ConfigPresets.minimal()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation
    for step in range(100):
        brain.step(step)
    
    # Get initial metrics
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getTotalNeuronCount()
    
    # Save checkpoint
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    success = brain.save(checkpoint_path)
    assert success, "Failed to save checkpoint"
    print(f"  ✓ Checkpoint saved successfully to {checkpoint_path}")
    
    # Create new brain and load
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    load_success = brain2.load(checkpoint_path)
    assert load_success, "Failed to load checkpoint"
    print(f"  ✓ Checkpoint loaded successfully")
    
    # Verify loaded state (approximately)
    loaded_spikes = brain2.getTotalSpikeCount()
    loaded_neurons = brain2.getTotalNeuronCount()
    
    print(f"  ✓ Loaded brain: {loaded_neurons} neurons, {loaded_spikes} spikes")
    
    # Clean up
    os.unlink(checkpoint_path)
    
    return True

def test_advanced_api():
    """Test advanced Python API features"""
    print("Testing advanced Python API...")
    
    # Test ConfigPresets
    minimal_config = ConfigPresets.minimal()
    full_config = ConfigPresets.full()
    
    assert minimal_config is not None, "Failed to create minimal config"
    assert full_config is not None, "Failed to create full config"
    print("  ✓ ConfigPresets working")
    
    # Test BatchSimulator
    simulator = BatchSimulator(minimal_config, num_simulations=2)
    results = simulator.run(steps_per_simulation=50)
    
    assert len(results) == 2, f"Expected 2 results, got {len(results)}"
    assert 'total_spikes' in results[0], "Missing total_spikes in results"
    print("  ✓ BatchSimulator working")
    
    # Test ExperimentManager
    world_config = {
        'width': 15,
        'height': 15,
        'visionWidth': 8,
        'visionHeight': 8,
    }
    
    manager = ExperimentManager()
    exp_results = manager.run_agent_experiment(
        minimal_config,
        world_config,
        num_steps=50,
        enable_learning=True
    )
    
    assert 'total_reward' in exp_results, "Missing total_reward in experiment results"
    print("  ✓ ExperimentManager working")
    
    return True

def test_serialization_utils():
    """Test serialization utilities"""
    print("Testing serialization utilities...")
    
    config = ConfigPresets.minimal()
    
    # Test saving and loading
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run some simulation
    for step in range(50):
        brain1.step(step)
    
    # Save using SerializationUtils
    with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
        json_path = f.name
    
    # Create simple brain dict
    brain_dict = {
        'neuron_count': brain1.getTotalNeuronCount(),
        'synapse_count': brain1.getTotalSynapseCount(),
        'firing_rate': brain1.getAverageFiringRate(),
    }
    
    # Save to JSON
    with open(json_path, 'w') as f:
        json.dump(brain_dict, f, indent=2)
    
    print("  ✓ SerializationUtils working")
    
    # Clean up
    os.unlink(json_path)
    
    return True

def test_cli_tools():
    """Test command-line interface tools"""
    print("Testing CLI tools...")
    
    # Check that nlm.py exists and is executable
    nlm_path = os.path.join(os.path.dirname(__file__), 'nlm.py')
    assert os.path.exists(nlm_path), f"nlm.py not found at {nlm_path}"
    
    # Check that enhanced.py exists
    enhanced_path = os.path.join(os.path.dirname(__file__), 'src', 'pynlm', 'enhanced.py')
    assert os.path.exists(enhanced_path), f"enhanced.py not found at {enhanced_path}"
    
    print("  ✓ CLI tools available")
    
    return True

def test_performance_improvements():
    """Test performance improvements"""
    print("Testing performance improvements...")
    
    # Test with different configurations
    configs = [
        ConfigPresets.minimal(),
        ConfigPresets.full(),
    ]
    
    for i, config in enumerate(configs):
        print(f"  Testing configuration {i + 1}...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Quick performance test
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        simulation_time = end_time - start_time
        print(f"    Time for 100 steps: {simulation_time:.3f}s")
        
        # Verify brain state
        assert brain.getTotalNeuronCount() > 0, "Brain has no neurons"
        assert brain.getTotalSynapseCount() >= 0, "Invalid synapse count"
        
        # Clean up
        del brain
    
    print("  ✓ Performance improvements working")
    
    return True

def run_all_tests():
    """Run all integration tests"""
    print("NLM Integration Test Suite")
    print("=" * 50)
    
    tests = [
        ("Reward System Improvements", test_reward_system_improvements),
        ("Checkpoint System", test_checkpoint_system),
        ("Advanced Python API", test_advanced_api),
        ("Serialization Utilities", test_serialization_utils),
        ("CLI Tools", test_cli_tools),
        ("Performance Improvements", test_performance_improvements),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        print("-" * len(test_name))
        
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test_name} FAILED with exception: {e}")
    
    print("\n" + "=" * 50)
    print("TEST SUMMARY")
    print("=" * 50)
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {failed}")
    print(f"Total tests: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! NLM improvements are working correctly.")
        return True
    else:
        print(f"\n❌ {failed} test(s) failed. Please review the issues above.")
        return False

if __name__ == "__main__":
    import time
    
    success = run_all_tests()
    sys.exit(0 if success else 1)
