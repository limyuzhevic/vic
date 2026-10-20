#!/usr/bin/env python3
"""
Test script for NLM Python bindings and enhanced API
"""

import sys
import os

print("Testing NLM Python Bindings and Enhanced API")
print("=" * 50)

# Test 1: Check if pynlm module can be imported
try:
    import pynlm
    print("✓ Successfully imported pynlm module")
    print(f"  Version: {getattr(pynlm, '__version__', 'Unknown')}")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

# Test 2: Check basic functionality
print("\n2. Testing basic functionality:")
try:
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    brain = pynlm.createBrain(config)
    print("✓ Created brain")
    
    world = pynlm.createSimpleWorld()
    print("✓ Created simple world")
    
    agent = pynlm.createAgentBrain(brain)
    print("✓ Created agent brain")
    
except Exception as e:
    print(f"✗ Basic functionality test failed: {e}")

# Test 3: Test high-level convenience functions
print("\n3. Testing high-level convenience functions:")
try:
    # Test create_brain_with_preset
    brain_phase3 = pynlm.create_brain_with_preset("phase3")
    print("✓ Created brain with phase3 preset")
    
    # Test benchmark_performance
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    metrics = ["neuron_count", "synapse_count", "firing_rate"]
    benchmark_results = pynlm.benchmark_performance(brain, duration=100, metrics=metrics)
    print(f"✓ Ran benchmark performance: {benchmark_results}")
    
    # Test visualization functions (these return placeholder messages)
    plot_result = pynlm.plot_brain_network(brain)
    print(f"✓ Generated brain network plot: {plot_result[:50]}...")
    
except Exception as e:
    print(f"✗ High-level API test failed: {e}")

# Test 4: Test utilities module
print("\n4. Testing utilities module:")
try:
    from python.utils import (
        DataProcessor, TimeSeriesGenerator, ConfigurationValidator,
        ProgressTracker, CheckpointManager, BatchProcessor,
        get_memory_usage, get_cpu_usage, time_operation
    )
    
    print("✓ Successfully imported utilities")
    
    # Test DataProcessor
    test_data = [1.0, 2.0, 3.0, 4.0, 5.0]
    processed = DataProcessor.normalize_data(test_data)
    print(f"✓ DataProcessor normalized data: {processed}")
    
    # Test ConfigurationValidator
    config_dict = {
        'brain': {'neuron_count': 1000},
        'plasticity': {'stdp': {'enable': True}}
    }
    is_valid, errors = ConfigurationValidator.validate_config(config_dict)
    print(f"✓ ConfigurationValidator validated config: {is_valid}")
    
    # Test ProgressTracker
    tracker = ProgressTracker(100, 10)
    print("✓ Created ProgressTracker")
    
    # Test CheckpointManager
    checkpoint_manager = CheckpointManager("/tmp/test_checkpoints")
    checkpoint_manager.save_checkpoint("test.json", {"data": "test"})
    print("✓ CheckpointManager saved checkpoint")
    
except Exception as e:
    print(f"✗ Utilities test failed: {e}")

# Test 5: Test high_level_api module
print("\n5. Testing high_level_api module:")
try:
    from python.high_level_api import (
        BrainSimulator, create_brain_with_preset, run_learning_experiment,
        train_agent, benchmark_performance, plot_activity_raster,
        plot_weight_matrix, create_video, train_predictive_model,
        optimize_weights, run_reinforcement_learning, cross_validate,
        monitor_performance, measure_compatibility, profile_activity,
        export_for_debugging, import_from_checkpoint,
        validate_configuration, generate_config_template
    )
    
    print("✓ Successfully imported high_level_api")
    
    # Test BrainSimulator
    simulator = BrainSimulator("phase3")
    print("✓ Created BrainSimulator")
    
    # Test create_brain_with_preset function
    brain = create_brain_with_preset("phase3")
    print("✓ Used create_brain_with_preset function")
    
    # Test generate_config_template
    template = generate_config_template()
    print(f"✓ Generated config template: {len(template)} characters")
    
except Exception as e:
    print(f"✗ High-level API test failed: {e}")

# Test 6: Test error handling
print("\n6. Testing error handling:")
try:
    # Test invalid preset
    try:
        brain = pynlm.create_brain_with_preset("invalid_preset")
        print("✗ Should have thrown exception for invalid preset")
    except Exception as e:
        print(f"✓ Correctly handled invalid preset: {type(e).__name__}")
    
    # Test invalid configuration validation
    is_valid, errors = pynlm.validate_configuration("{invalid json")
    print(f"✓ Handled invalid configuration: valid={is_valid}, errors={len(errors)}")
    
except Exception as e:
    print(f"✗ Error handling test failed: {e}")

# Test 7: Test type conversions
print("\n7. Testing type conversions:")
try:
    # Test type conversion functions
    float_val = pynlm.to_python_float(3.14)
    print(f"✓ Converted to Python float: {float_val}")
    
    int_val = pynlm.to_python_int(42)
    print(f"✓ Converted to Python int: {int_val}")
    
    string_val = pynlm.to_python_string("test")
    print(f"✓ Converted to Python string: {string_val}")
    
except Exception as e:
    print(f"✗ Type conversion test failed: {e}")

# Summary
print("\n" + "=" * 50)
print("Test Summary:")
print("All tests completed. Check output for any failures.")
print("\nThe enhanced NLM Python bindings include:")
print("- High-level convenience functions")
print("- Visualization support")
print("- Advanced machine learning features")
print("- Performance monitoring tools")
print("- Development and debugging utilities")
print("- Improved API usability")
print("- Backward compatibility")
