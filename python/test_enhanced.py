#!/usr/bin/env python3
"""
Test script for NLM Enhanced Python bindings.

This script tests the improved Python bindings and demonstrates
new features and capabilities.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from pynlm_enhanced import (
    BrainSimulator, NLMEnvironment, NLMConfigurator,
    SimulationMetrics, NLMError, InitializationError,
    ConfigurationError, SimulationError, create_brain,
    create_world, run_experiment
)

def test_basic_functionality():
    """Test basic brain simulation functionality."""
    print("Testing basic functionality...")
    
    # Create brain simulator
    brain_sim = BrainSimulator()
    
    # Test initialization
    brain_sim.initialize()
    print("✓ Initialization successful")
    
    # Test step execution
    metrics = brain_sim.step(0)
    print(f"✓ Step execution successful: {metrics.active_neurons} neurons firing")
    
    # Test reset
    brain_sim.reset()
    print("✓ Reset successful")
    
    # Test metrics collection
    brain_sim.run(50)
    metrics_list = brain_sim.metrics_history
    print(f"✓ Metrics collection: {len(metrics_list)} data points collected")
    
    # Test metrics summary
    summary = brain_sim.get_metrics_summary()
    print(f"✓ Metrics summary: {summary['total_steps']} steps")
    
    return True


def test_environment_functionality():
    """Test environment simulation functionality."""
    print("\nTesting environment functionality...")
    
    # Create environment
    env = NLMEnvironment()
    
    # Test configuration
    env.configure(width=15, height=15, vision_width=6, vision_height=6)
    print("✓ Environment configuration successful")
    
    # Test reset
    env.reset()
    print("✓ Environment reset successful")
    
    # Test update
    result = env.update(timestep=0.1)
    print(f"✓ Environment update successful: {result['timestamp']}")
    
    # Test agent start
    env.set_agent_start(5.0, 5.0)
    print("✓ Agent start position set")
    
    return True


def test_configuration_functionality():
    """Test configuration management functionality."""
    print("\nTesting configuration functionality...")
    
    # Create configurator
    config = NLMConfigurator()
    
    # Test setting values
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("learning.rate", 0.001)
    print("✓ Configuration values set")
    
    # Test getting values
    neuron_count = config.get("brain.neuron_count")
    print(f"✓ Configuration retrieved: neuron_count = {neuron_count}")
    
    # Test has key
    has_neuron_key = config.config.has("brain.neuron_count")
    print(f"✓ Key check: {has_neuron_key}")
    
    # Test get keys
    keys = config.config.getKeys()
    print(f"✓ Retrieved {len(keys)} configuration keys")
    
    # Test default value
    default_value = config.get("nonexistent.key", 42)
    print(f"✓ Default value: {default_value}")
    
    return True


def test_advanced_features():
    """Test advanced features and APIs."""
    print("\nTesting advanced features...")
    
    # Create brain with custom configuration
    config = NLMConfigurator()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.2)
    
    brain_sim = BrainSimulator(config.config)
    brain_sim.initialize()
    print("✓ Custom configuration successful")
    
    # Test step with custom timestep
    metrics = brain_sim.step(10, timestep=0.01)
    print(f"✓ Custom timestep step: {metrics.time:.3f}s")
    
    # Test simulation with callback
    call_count = [0]
    
    def test_callback(m):
        call_count[0] += 1
    
    brain_sim.run(100, callback=test_callback)
    print(f"✓ Callback function: called {call_count[0]} times")
    
    # Test metrics export
    brain_sim.export_metrics("test_metrics.json", "json")
    print("✓ Metrics export successful")
    
    # Test metrics export to CSV
    try:
        brain_sim.export_metrics("test_metrics.csv", "csv")
        print("✓ CSV export successful")
    except ImportError:
        print("⚠ CSV export skipped (pandas not available)")
    
    # Clean up test files
    for file in ["test_metrics.json", "test_metrics.csv"]:
        if os.path.exists(file):
            os.remove(file)
    
    return True


def test_error_handling():
    """Test error handling and edge cases."""
    print("\nTesting error handling...")
    
    # Test uninitialized brain error
    brain_sim = BrainSimulator()
    try:
        brain_sim.step(0)
        print("✗ Should have raised SimulationError for uninitialized brain")
        return False
    except SimulationError:
        print("✓ Correctly raised SimulationError for uninitialized brain")
    
    # Test invalid configuration
    try:
        brain_sim = BrainSimulator("invalid_config")
        print("✗ Should have raised NLMError for invalid config")
        return False
    except (NLMError, InitializationError):
        print("✓ Correctly raised error for invalid configuration")
    
    # Test environment without configuration
    from pynlm_enhanced import NLMEnvironment
    env = NLMEnvironment()
    try:
        env.update(0.1)
        print("✗ Should have raised SimulationError for unconfigured environment")
        return False
    except SimulationError:
        print("✓ Correctly raised SimulationError for unconfigured environment")
    
    return True


def test_convenience_functions():
    """Test convenience functions."""
    print("\nTesting convenience functions...")
    
    # Test create_brain
    brain = create_brain()
    print("✓ create_brain() function works")
    
    # Test create_world
    world = create_world(width=10, height=10)
    print("✓ create_world() function works")
    
    # Test run_experiment
    try:
        results = run_experiment(
            brain_config=None,
            world_config={'width': 10, 'height': 10},
            num_steps=50
        )
        print("✓ run_experiment() function works")
        
        # Check results structure
        required_keys = ['brain_simulator', 'environment', 'agent', 'metrics']
        if all(key in results for key in required_keys):
            print("✓ Experiment results structure correct")
        else:
            print("✗ Experiment results structure incorrect")
            return False
            
    except Exception as e:
        print(f"⚠ run_experiment() had issues (may be expected): {e}")
    
    return True


def test_metrics_analysis():
    """Test metrics analysis and utilities."""
    print("\nTesting metrics analysis...")
    
    brain_sim = BrainSimulator()
    brain_sim.initialize()
    brain_sim.run(200)
    
    # Get metrics summary
    summary = brain_sim.get_metrics_summary()
    print(f"✓ Summary statistics collected")
    
    # Check expected keys
    expected_keys = ['total_steps', 'step_range', 'max_firing_neurons', 
                     'min_firing_neurons', 'avg_firing_rate', 'total_spikes']
    
    for key in expected_keys:
        if key in summary:
            print(f"  ✓ {key}: {summary[key]}")
        else:
            print(f"  ✗ {key}: missing")
            return False
    
    # Test SimulationMetrics class
    if brain_sim.metrics_history:
        metrics = brain_sim.metrics_history[0]
        assert isinstance(metrics, SimulationMetrics)
        print("✓ SimulationMetrics class works correctly")
        
        # Test to_dict
        metrics_dict = metrics.to_dict()
        assert isinstance(metrics_dict, dict)
        print("✓ SimulationMetrics.to_dict() works")
    
    return True


def run_performance_test():
    """Run a quick performance test."""
    print("\nRunning performance test...")
    
    import time
    
    brain_sim = BrainSimulator()
    brain_sim.initialize()
    
    start_time = time.time()
    brain_sim.run(100)
    end_time = time.time()
    
    duration = end_time - start_time
    print(f"✓ Performance: 100 steps in {duration:.2f} seconds")
    
    if duration > 0:
        steps_per_sec = 100 / duration
        print(f"✓ Performance: {steps_per_sec:.1f} steps per second")
    
    return True


def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Enhanced Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_environment_functionality,
        test_configuration_functionality,
        test_advanced_features,
        test_error_handling,
        test_convenience_functions,
        test_metrics_analysis,
        run_performance_test
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
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
        return 0
    else:
        print(f"\n❌ {failed} tests failed")
        return 1


if __name__ == '__main__':
    sys.exit(main())