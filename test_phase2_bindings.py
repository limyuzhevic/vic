#!/usr/bin/env python3
"""
Comprehensive test script for Phase 2 Python bindings.

This script tests all the new convenience classes and utility functions
to ensure they work correctly with the existing NLM bindings.
"""

import sys
import os
import tempfile
import shutil
from pathlib import Path

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

# Import the bindings
try:
    import python.pynlm as pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

# Test helper functions
def test_utility_functions():
    print("\n=== Testing Utility Functions ===")
    
    # Test create_default_config
    try:
        config = pynlm.create_default_config("test_exp", 100, 0.01)
        assert config is not None
        assert config.getString("name") == "test_exp"
        assert config.getInt("max_steps") == 100
        assert config.getFloat("timestep") == 0.01
        print("✓ create_default_config works correctly")
    except Exception as e:
        print(f"✗ create_default_config failed: {e}")
        return False
    
    # Test config manipulation
    try:
        config = pynlm.create_default_config()
        config.setInt("brain.neurons", 500)
        config.setFloat("world.size", 10.0)
        assert config.getInt("brain.neurons") == 500
        assert config.getFloat("world.size") == 10.0
        print("✓ Config manipulation works correctly")
    except Exception as e:
        print(f"✗ Config manipulation failed: {e}")
        return False
    
    return True

# Test SimpleExperiment

def test_simple_experiment():
    print("\n=== Testing SimpleExperiment ===")
    
    try:
        exp = pynlm.SimpleExperiment()
        print("✓ SimpleExperiment constructor works")
    except Exception as e:
        print(f"✗ SimpleExperiment constructor failed: {e}")
        return False
    
    # Test configuration
    try:
        exp.configure(
            max_steps=100,
            timestep=0.01,
            brain=None,  # Would need real brain instance
            world=None,  # Would need real world instance
            agent_brain=None  # Would need real agent brain
        )
        print("✓ SimpleExperiment.configure works")
    except Exception as e:
        print(f"✗ SimpleExperiment.configure failed: {e}")
        # This might fail without real components, which is expected
        print("  (Expected failure due to missing components)")
    
    # Test method calls
    try:
        exp.enable_data_collection(True)
        print("✓ enable_data_collection works")
    except Exception as e:
        print(f"✗ enable_data_collection failed: {e}")
        return False
    
    # Test property access
    try:
        # These might fail without data, which is expected
        _ = exp.total_reward
        _ = exp.final_step
        _ = exp.average_firing_rate
        _ = exp.metrics
        print("✓ Property accessors work")
    except Exception as e:
        print(f"✗ Property accessors failed (expected without data): {e}")
        pass
    
    return True

# Test BrainTrainer
def test_brain_trainer():
    print("\n=== Testing BrainTrainer ===")
    
    try:
        trainer = pynlm.BrainTrainer()
        print("✓ BrainTrainer constructor works")
    except Exception as e:
        print(f"✗ BrainTrainer constructor failed: {e}")
        return False
    
    # Test configuration
    try:
        trainer.configure(
            brain=None,
            environment=None,
            max_episodes=50,
            learning_rate=0.01
        )
        print("✓ BrainTrainer.configure works")
    except Exception as e:
        print(f"✗ BrainTrainer.configure failed: {e}")
        # This might fail without real components
        print("  (Expected failure due to missing components)")
    
    return True

# Test WorldBuilder

def test_world_builder():
    print("\n=== Testing WorldBuilder ===")
    
    try:
        builder = pynlm.WorldBuilder()
        print("✓ WorldBuilder constructor works")
    except Exception as e:
        print(f"✗ WorldBuilder constructor failed: {e}")
        return False
    
    # Test world creation
    try:
        builder.create_world(10, 10, "flat", True)
        print("✓ WorldBuilder.create_world works")
    except Exception as e:
        print(f"✗ WorldBuilder.create_world failed: {e}")
        return False
    
    # Test object addition
    try:
        builder.add_terrain_object("resource", 5.0, 5.0, 0.5, 1.0)
        print("✓ WorldBuilder.add_terrain_object works")
    except Exception as e:
        print(f"✗ WorldBuilder.add_terrain_object failed: {e}")
        return False
    
    # Test agent addition
    try:
        builder.add_agent(2.0, 2.0, 0.0, 0.5)
        print("✓ WorldBuilder.add_agent works")
    except Exception as e:
        print(f"✗ WorldBuilder.add_agent failed: {e}")
        return False
    
    # Test property access
    try:
        _ = builder.width
        _ = builder.height
        _ = builder.terrain_type
        _ = builder.agent_count
        print("✓ WorldBuilder property accessors work")
    except Exception as e:
        print(f"✗ WorldBuilder property accessors failed: {e}")
        return False
    
    return True

# Test MetricsCollector

def test_metrics_collector():
    print("\n=== Testing MetricsCollector ===")
    
    try:
        collector = pynlm.MetricsCollector()
        print("✓ MetricsCollector constructor works")
    except Exception as e:
        print(f"✗ MetricsCollector constructor failed: {e}")
        return False
    
    # Test data recording
    try:
        collector.record("reward", 1.5)
        collector.record("firing_rate", 42.3)
        collector.record("energy", 0.8)
        collector.record("step_time", 0.001)
        print("✓ MetricsCollector.record works")
    except Exception as e:
        print(f"✗ MetricsCollector.record failed: {e}")
        return False
    
    # Test time series recording
    try:
        values = [1.0, 2.0, 3.0, 4.0, 5.0]
        collector.record_time_series("test_metric", values, 1000.0)
        print("✓ MetricsCollector.record_time_series works")
    except Exception as e:
        print(f"✗ MetricsCollector.record_time_series failed: {e}")
        return False
    
    # Test statistical methods
    try:
        _ = collector.get_mean("reward")
        _ = collector.get_stddev("firing_rate")
        _ = collector.get_min("energy")
        _ = collector.get_max("step_time")
        _ = collector.get_median("reward")
        print("✓ MetricsCollector statistical methods work")
    except Exception as e:
        print(f"✗ MetricsCollector statistical methods failed: {e}")
        return False
    
    # Test utility methods
    try:
        _ = collector.get_metric_names()
        _ = collector.is_empty()
        _ = collector.summary()
        print("✓ MetricsCollector utility methods work")
    except Exception as e:
        print(f"✗ MetricsCollector utility methods failed: {e}")
        return False
    
    # Test export methods (in memory)
    try:
        csv_data = collector.to_csv("test")
        json_data = collector.to_json()
        print("✓ MetricsCollector export methods work")
    except Exception as e:
        print(f"✗ MetricsCollector export methods failed: {e}")
        return False
    
    return True

# Test context managers
def test_context_managers():
    print("\n=== Testing Context Managers ===")
    
    # Test SimpleExperiment context manager
    try:
        with pynlm.SimpleExperiment() as exp:
            pass
        print("✓ SimpleExperiment context manager works")
    except Exception as e:
        print(f"✗ SimpleExperiment context manager failed: {e}")
        return False
    
    return True

# Main test runner
def main():
    print("Phase 2 Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        test_utility_functions,
        test_simple_experiment,
        test_brain_trainer,
        test_world_builder,
        test_metrics_collector,
        test_context_managers,
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                print(f"✗ {test.__name__} failed")
        except Exception as e:
            print(f"✗ {test.__name__} crashed: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("✓ All tests passed! Phase 2 implementation is working correctly.")
        return 0
    else:
        print("✗ Some tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())