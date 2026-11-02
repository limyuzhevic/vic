#!/usr/bin/env python3
"""
Test suite for enhanced NLM Python bindings.
Validates new Python-specific convenience methods and error handling.
"""

import sys
import os
import tempfile
import shutil

# Add the build directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

try:
    import pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Please ensure the Python bindings are built first:")
    print("  mkdir build && cd build && cmake .. && make")
    sys.exit(1)

def test_config_convenience():
    """Test Python-specific Config convenience methods."""
    print("\n=== Testing Config Convenience Methods ===")
    
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    # Test updateFromDict
    config_data = {
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "hello",
        "bool_value": True,
        "list_values": [1, 2, 3, 4, 5]
    }
    config.updateFromDict(config_data)
    print("✓ Updated config from dictionary")
    
    # Test validation
    try:
        config.validate()
        print("✓ Configuration validation passed")
    except pynlm.ValidationError as e:
        print(f"✗ Configuration validation failed: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected validation error: {e}")
        return False
    
    # Test toDict
    config_dict = config.toDict()
    print("✓ Converted config to dictionary")
    
    # Test getSchema
    schema = config.getSchema()
    print("✓ Retrieved configuration schema")
    
    return True

def test_brain_convenience():
    """Test Python-specific Brain convenience methods."""
    print("\n=== Testing Brain Convenience Methods ===")
    
    # Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    
    # Test runEpisode
    steps_completed = brain.runEpisode(5)
    print(f"✓ Completed {steps_completed} steps via runEpisode")
    
    # Test saveBrain
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        save_path = f.name
    
    try:
        success = brain.saveBrain(save_path)
        if success:
            print(f"✓ Successfully saved brain to {save_path}")
            
            # Test loadBrain
            success = brain.loadBrain(save_path)
            if success:
                print("✓ Successfully loaded brain from file")
            else:
                print("✗ Failed to load brain from file")
                return False
        else:
            print("✗ Failed to save brain to file")
            return False
    finally:
        if os.path.exists(save_path):
            os.unlink(save_path)
    
    # Test getEpisodeStats
    stats = brain.getEpisodeStats()
    print("✓ Retrieved brain statistics")
    
    # Test getDevelopmentStage
    dev_stage = brain.getDevelopmentStage()
    print(f"✓ Retrieved developmental stage: {dev_stage}")
    
    # Test enableDevelopment
    brain.enableDevelopment(True)
    print("✓ Enabled development system")
    
    # Test resetForNewEpisode
    brain.resetForNewEpisode()
    print("✓ Reset brain for new episode")
    
    return True

def test_error_handling():
    """Test enhanced error handling."""
    print("\n=== Testing Enhanced Error Handling ===")
    
    # Test validation error
    config = pynlm.createDefaultConfig()
    try:
        config.validate()
        print("✓ Configuration validation passed (expected)")
    except pynlm.ValidationError as e:
        print(f"✓ Caught ValidationError: {e}")
    except Exception as e:
        print(f"✗ Unexpected error type: {type(e).__name__}: {e}")
        return False
    
    # Test file import error
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Try to load a non-existent file
    try:
        success = brain.loadBrain("/nonexistent/file.bin")
        if not success:
            print("✓ loadBrain returned False for non-existent file")
    except pynlm.ImportError as e:
        print(f"✓ Caught ImportError: {e}")
    except Exception as e:
        print(f"✗ Unexpected error type: {type(e).__name__}: {e}")
        return False
    
    # Test that custom exceptions are available
    try:
        raise pynlm.NLMRuntimeError("Test error")
    except pynlm.NLMRuntimeError as e:
        print(f"✓ Custom NLMRuntimeError works: {e}")
    
    try:
        raise pynlm.NLMValidationError("Test validation error")
    except pynlm.NLMValidationError as e:
        print(f"✓ Custom NLMValidationError works: {e}")
    
    return True

def test_development_system():
    """Test development system APIs."""
    print("\n=== Testing Development System ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    
    # Test developmental stage access
    dev_stage = brain.getDevelopmentStage()
    print(f"✓ Retrieved developmental stage: {dev_stage}")
    
    # Test developmental stage setting
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    print("✓ Set developmental stage to Maturation")
    
    # Test enableDevelopment
    brain.enableDevelopment(True)
    print("✓ Enabled development system")
    
    return True

def test_simulation_callbacks():
    """Test simulation callbacks."""
    print("\n=== Testing Simulation Callbacks ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    
    # Set up callback counters
    pre_step_count = 0
    post_step_count = 0
    
    def pre_step_callback(step):
        nonlocal pre_step_count
        pre_step_count += 1
        print(f"  Pre-step {step} called")
    
    def post_step_callback(step, stats):
        nonlocal post_step_count
        post_step_count += 1
        print(f"  Post-step {step} called with {stats['active_neurons']} active neurons")
    
    # Enable callbacks
    brain.enableCallbacks(pre_step_callback, post_step_callback)
    print("✓ Enabled simulation callbacks")
    
    # Run simulation with callbacks
    brain.runEpisode(3)
    print(f"✓ Simulation completed with {pre_step_count} pre-steps and {post_step_count} post-steps")
    
    return True

def test_performance_monitoring():
    """Test performance monitoring."""
    print("\n=== Testing Performance Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.enablePerformanceMonitoring()
    print("✓ Enabled performance monitoring")
    
    # Run simulation
    for i in range(10):
        brain.step(i)
    
    # Get performance stats
    perf_stats = brain.getPerformanceStats()
    print("✓ Retrieved performance statistics")
    
    # Verify stats structure
    required_keys = ['total_neurons', 'total_synapses', 'active_neurons', 
                     'firing_neurons', 'total_spikes', 'exc_inh_ratio']
    
    for key in required_keys:
        if key in perf_stats:
            print(f"  ✓ {key}: {perf_stats[key]}")
        else:
            print(f"  ✗ Missing key: {key}")
            return False
    
    return True

def test_visualization():
    """Test visualization features."""
    print("\n=== Testing Visualization Features ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.enableVisualization()
    print("✓ Enabled visualization support")
    
    # Run simulation
    for i in range(5):
        brain.step(i)
    
    # Export for visualization
    viz_data = brain.exportForVisualization()
    print("✓ Exported brain data for visualization")
    
    # Verify viz data structure
    if 'brain_state' in viz_data:
        print("  ✓ Contains brain_state")
    if 'timestamp' in viz_data:
        print(f"  ✓ Contains timestamp: {viz_data['timestamp']}")
    
    return True

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Config Convenience", test_config_convenience),
        ("Brain Convenience", test_brain_convenience),
        ("Error Handling", test_error_handling),
        ("Development System", test_development_system),
        ("Simulation Callbacks", test_simulation_callbacks),
        ("Performance Monitoring", test_performance_monitoring),
        ("Visualization", test_visualization),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        try:
            if test_func():
                print(f"✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"✗ {test_name} FAILED")
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} FAILED with exception: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print(f"\n{'='*50}")
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("✓ All tests passed!")
        return 0
    else:
        print(f"✗ {failed} tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())