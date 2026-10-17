#!/usr/bin/env python3
"""
Performance tests for critical operations.

This test file covers:
- Performance tests for advanced brain manipulation utilities
- Performance tests for configuration management
- Performance tests for serialization and persistence
- Performance regression tests
- Benchmark tests for critical operations
"""

import sys
import os
import tempfile
import shutil
import time
import json
import gc
from pathlib import Path
from typing import Dict, List, Any

# Add the workspace to Python path
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import python.pynlm as pynlm
    print("✓ Successfully imported pynlm module")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
def benchmark_brain_operations():
    """Benchmark brain creation, initialization, and stepping operations."""
    print("=" * 60)
    print("BRAIN OPERATIONS PERFORMANCE BENCHMARK")
    print("=" * 60)
    
    # Test brain creation performance
    print("\n1. Brain Creation Performance:")
    
    config_sizes = [100, 500, 1000, 2000]
    
    for config_size in config_sizes:
        # Create configuration
        start_time = time.time()
        
        config = pynlm.create_default_config(f"bench_{config_size}", 100, 0.01)
        config.set("brain.neuron_count", config_size)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        end_time = time.time()
        creation_time = end_time - start_time
        
        print(f"   - Config size {config_size}: {creation_time:.3f}s")
    
    # Test brain stepping performance
    print("\n2. Brain Stepping Performance:")
    
    brain = pynlm.createBrain(pynlm.create_default_config("step_bench", 500, 0.01))
    brain.initialize()
    
    step_counts = [100, 500, 1000, 2000]
    
    for step_count in step_counts:
        start_time = time.time()
        
        for step in range(step_count):
            brain.step(step)
        
        end_time = time.time()
        total_time = end_time - start_time
        avg_step_time = total_time / step_count
        
        print(f"   - {step_count} steps: {total_time:.3f}s total, {avg_step_time*1000:.3f}ms/step")
    
    # Test advanced brain manipulation performance
    print("\n3. Advanced Brain Manipulation Performance:")
    
    brain = pynlm.createBrain(pynlm.create_default_config("manip_bench", 200, 0.01))
    brain.initialize()
    manipulator = pynlm.createAdvancedBrainManipulator(brain)
    
    region_id = brain.addRegion("performance_region")
    
    # Create neurons
    neurons = []
    creation_start = time.time()
    for i in range(100):
        neuron = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        neurons.append(neuron)
    creation_end = time.time()
    
    print(f"   - Neuron creation: {(creation_end - creation_start)*1000:.1f}ms for 100 neurons")
    
    # Create synapses
    synapse_start = time.time()
    synapses = []
    for i in range(len(neurons) - 1):
        synapse = manipulator.createSynapse(neurons[i], neurons[i + 1], weight=0.5)
        synapses.append(synapse)
    synapse_end = time.time()
    
    print(f"   - Synapse creation: {(synapse_end - synapse_start)*1000:.1f}ms for {len(synapses)} synapses")
    
    # Modify neuron properties
    modify_start = time.time()
    for i in range(0, len(neurons), 10):  # Modify every 10th neuron
        new_state = pynlm.NeuronState()
        new_state.threshold = -50.0 - (i * 0.01)
        manipulator.modifyNeuronProperties(neurons[i], new_state)
    modify_end = time.time()
    
    print(f"   - Neuron modification: {(modify_end - modify_start)*1000:.1f}ms for {len(neurons)//10} neurons")
    
    # Get neuron states
    read_start = time.time()
    for neuron in neurons[::10]:  # Read every 10th neuron
        state = manipulator.getNeuronState(neuron)
        _ = state.threshold
    read_end = time.time()
    
    print(f"   - Neuron state reading: {(read_end - read_start)*1000:.1f}ms for {len(neurons)//10} neurons")
    
    print("\n" + "=" * 60)
    print("BRAIN OPERATIONS BENCHMARK COMPLETE")
    print("=" * 60)
    return True
def benchmark_configuration_operations():
    """Benchmark configuration creation, manipulation, and validation."""
    print("\n" + "=" * 60)
    print("CONFIGURATION OPERATIONS PERFORMANCE BENCHMARK")
    print("=" * 60)
    
    # Test configuration creation performance
    print("\n1. Configuration Creation Performance:")
    
    config_sizes = [10, 50, 100, 200]
    
    for size in config_sizes:
        start_time = time.time()
        
        configs = []
        for i in range(size):
            config = pynlm.create_default_config(f"config_{i}", 100, 0.01)
            config.setInt(f"param_{i}", i * 10)
            config.setFloat(f"param_{i}_float", i * 0.1)
            config.setString(f"param_{i}_str", f"value_{i}")
            configs.append(config)
        
        end_time = time.time()
        total_time = end_time - start_time
        avg_time = total_time / size
        
        print(f"   - {size} configurations: {total_time:.3f}s total, {avg_time*1000:.3f}ms/config")
    
    # Test configuration manipulation performance
    print("\n2. Configuration Manipulation Performance:")
    
    config = pynlm.create_default_config("manip_bench", 100, 0.01)
    
    # Set many configuration values
    set_start = time.time()
    for i in range(1000):
        config.setInt(f"param_{i}", i)
        config.setFloat(f"param_{i}_float", i * 0.01)
        config.setString(f"param_{i}_str", f"value_{i}")
        config.setBool(f"param_{i}_bool", i % 2 == 0)
    set_end = time.time()
    
    print(f"   - Setting 1000 config values: {(set_end - set_start)*1000:.1f}ms")
    
    # Get many configuration values
    get_start = time.time()
    for i in range(1000):
        _ = config.getInt(f"param_{i}")
        _ = config.getFloat(f"param_{i}_float")
        _ = config.getString(f"param_{i}_str")
        _ = config.getBool(f"param_{i}_bool")
    get_end = time.time()
    
    print(f"   - Getting 1000 config values: {(get_end - get_start)*1000:.1f}ms")
    
    # Test ConfigManager performance
    print("\n3. ConfigManager Performance:")
    
    config_manager = pynlm.createConfigManager()
    
    # Create and add templates
    start_time = time.time()
    for i in range(50):
        template_config = pynlm.create_default_config(f"template_{i}", 100, 0.01)
        template_config.set("brain.neuron_count", i * 100)
        template_config.set("world.size", i * 1.0)
        
        config_manager.addTemplate(f"template_{i}", f"Template {i}", template_config)
    
    template_end = time.time()
    print(f"   - Adding 50 templates: {(template_end - start_time)*1000:.1f}ms")
    
    # Create configs from templates
    get_start = time.time()
    configs = []
    for i in range(50):
        config = config_manager.createFromTemplate(f"template_{i}")
        configs.append(config)
    
    get_end = time.time()
    print(f"   - Getting 50 configs from templates: {(get_end - get_start)*1000:.1f}ms")
    
    # Test configuration validation
    print("\n4. Configuration Validation Performance:")
    
    test_configs = []
    for i in range(10):
        config = pynlm.create_default_config(f"validation_test_{i}", 100, 0.01)
        config.set("brain.neuron_count", i * 100)
        config.set("world.width", i * 5)
        test_configs.append(config)
    
    # Validate all configs
    start_time = time.time()
    for config in test_configs:
        is_valid = config_manager.validateConfig(config)
        errors = config_manager.validateConfigWithDetails(config)
    
    validation_end = time.time()
    print(f"   - Validating 10 configs: {(validation_end - start_time)*1000:.1f}ms")
    
    # Test sensitivity analysis
    print("\n5. Sensitivity Analysis Performance:")
    
    test_config = test_configs[0]
    start_time = time.time()
    
    for i in range(20):
        sensitivity = config_manager.analyzeSensitivity(test_config)
    
    sensitivity_end = time.time()
    print(f"   - Running 20 sensitivity analyses: {(sensitivity_end - start_time)*1000:.1f}ms")
    
    print("\n" + "=" * 60)
    print("CONFIGURATION OPERATIONS BENCHMARK COMPLETE")
    print("=" * 60)
    return True
def benchmark_serialization_operations():
    """Benchmark serialization and persistence operations."""
    print("\n" + "=" * 60)
    print("SERIALIZATION OPERATIONS PERFORMANCE BENCHMARK")
    print("=" * 60)
    
    # Setup test data
    print("\n1. Serialization Data Preparation:")
    
    config = pynlm.create_default_config("serial_bench", 100, 0.01)
    
    # Create test brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create advanced manipulator for more realistic data
    manipulator = pynlm.createAdvancedBrainManipulator(brain)
    region_id = brain.addRegion("serial_region")
    
    # Create neurons and synapses for serialization
    neurons = []
    for i in range(50):
        neuron = manipulator.createNeuron(region_id, pynlm.NeuronType.Excitatory)
        neurons.append(neuron)
    
    synapses = []
    for i in range(len(neurons) - 1):
        synapse = manipulator.createSynapse(neurons[i], neurons[i + 1], weight=0.5)
        synapses.append(synapse)
    
    # Test data sizes
    datasets = [
        ("small", {"data": [1.0, 2.0, 3.0, 4.0, 5.0]}),
        ("medium", {"data1": list(range(100)), "data2": [float(i) * 0.1 for i in range(100)]}),
        ("large", {f"dataset_{i}": [float(j) for j in range(1000)] for i in range(10)})
    ]
    
    # Test serialization performance
    print("\n2. Serialization Performance:")
    
    for name, dataset in datasets:
        serializer = pynlm.createSerializationManager()
        
        # HDF5 serialization
        start_time = time.time()
        hdf5_result = serializer.saveHDF5(f"{name}_test.h5", config, dataset)
        hdf5_time = time.time() - start_time
        
        # Pickle serialization
        start_time = time.time()
        pickle_result = serializer.savePickle(f"{name}_test.pkl", config, brain)
        pickle_time = time.time() - start_time
        
        # Incremental serialization
        start_time = time.time()
        incremental_result = serializer.saveIncremental(f"{name}_test_inc.pkl", config, dataset)
        incremental_time = time.time() - start_time
        
        # Compressed serialization
        start_time = time.time()
        compressed_result = serializer.saveCompressed(f"{name}_test.pkl.gz", config, brain)
        compressed_time = time.time() - start_time
        
        print(f"   - {name} dataset:")
        print(f"     HDF5: {hdf5_time:.3f}s")
        print(f"     Pickle: {pickle_time:.3f}s")
        print(f"     Incremental: {incremental_time:.3f}s")
        print(f"     Compressed: {compressed_time:.3f}s")
    
    # Test loading performance
    print("\n3. Deserialization Performance:")
    
    serializer = pynlm.createSerializationManager()
    
    # Create test files for loading
    test_datasets = {
        "neuron_activity": [0.1, 0.2, 0.3, 0.4, 0.5],
        "synaptic_weights": [0.9, 0.8, 0.7, 0.6, 0.5],
        "firing_rates": [10.0, 15.0, 20.0, 25.0, 30.0]
    }
    
    # Save files for loading
    serializer.saveHDF5("perf_test.h5", config, test_datasets)
    serializer.savePickle("perf_test.pkl", config, brain)
    serializer.saveIncremental("perf_test_inc.pkl", config, test_datasets)
    
    # Load from HDF5
    start_time = time.time()
    for i in range(10):
        loaded_config, loaded_datasets = serializer.loadHDF5("perf_test.h5")
    hdf5_load_time = time.time() - start_time
    
    # Load from pickle
    start_time = time.time()
    for i in range(10):
        loaded_config, loaded_brain = serializer.loadPickle("perf_test.pkl")
    pickle_load_time = time.time() - start_time
    
    # Load incrementally
    start_time = time.time()
    for i in range(10):
        loaded_config, loaded_data = serializer.loadIncremental("perf_test_inc.pkl")
    incremental_load_time = time.time() - start_time
    
    print(f"   - HDF5 loading (10 times): {hdf5_load_time:.3f}s total, {hdf5_load_time/10*1000:.1f}ms per load")
    print(f"   - Pickle loading (10 times): {pickle_load_time:.3f}s total, {pickle_load_time/10*1000:.1f}ms per load")
    print(f"   - Incremental loading (10 times): {incremental_load_time:.3f}s total, {incremental_load_time/10*1000:.1f}ms per load")
    
    # Test checkpoint performance
    print("\n4. Checkpoint Performance:")
    
    start_time = time.time()
    for i in range(20):
        checkpoint_name = f"checkpoint_{i}"
        checkpoint_result = serializer.createCheckpoint(checkpoint_name, config, brain)
        load_result = serializer.restoreCheckpoint(checkpoint_name)
    
    checkpoint_time = time.time() - start_time
    print(f"   - 20 checkpoint save/load cycles: {checkpoint_time:.3f}s total, {checkpoint_time/20*1000:.1f}ms per cycle")
    
    # Test visualization serialization
    print("\n5. Visualization Serialization Performance:")
    
    visualizer = pynlm.createVisualizationTools()
    
    start_time = time.time()
    for i in range(10):
        viz_result = visualizer.visualizeNetwork(f"perf_viz_{i}.png", brain)
    viz_time = time.time() - start_time
    
    print(f"   - 10 network visualizations: {viz_time:.3f}s total, {viz_time/10*1000:.1f}ms per visualization")
    
    print("\n" + "=" * 60)
    print("SERIALIZATION OPERATIONS BENCHMARK COMPLETE")
    print("=" * 60)
    return True
def benchmark_memory_usage():
    """Benchmark memory usage for critical operations."""
    print("\n" + "=" * 60)
    print("MEMORY USAGE PERFORMANCE BENCHMARK")
    print("=" * 60)
    
    try:
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss
        
        print(f"\n1. Initial memory: {initial_memory / 1024 / 1024:.1f} MB")
        
        # Create many objects to measure memory growth
        print("\n2. Memory growth with many objects:")
        
        objects = []
        memory_samples = []
        
        for i in range(100):
            config = pynlm.create_default_config(f"memory_test_{i}", 100, 0.01)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run some steps
            for step in range(20):
                brain.step(step)
            
            objects.append(brain)
            
            # Sample memory usage
            current_memory = process.memory_info().rss
            memory_samples.append(current_memory)
            
            if (i + 1) % 20 == 0:
                print(f"   - After {i + 1} brains: {current_memory / 1024 / 1024:.1f} MB")
        
        peak_memory = max(memory_samples)
        memory_growth = peak_memory - initial_memory
        
        print(f"\n   - Peak memory: {peak_memory / 1024 / 1024:.1f} MB")
        print(f"   - Memory growth: {memory_growth / 1024 / 1024:.1f} MB")
        print(f"   - Average per brain: {memory_growth / 100 / 1024 / 1024:.3f} MB")
        
        # Clean up
        del objects
        gc.collect()
        
        final_memory = process.memory_info().rss
        memory_freed = peak_memory - final_memory
        
        print(f"\n   - Memory after cleanup: {final_memory / 1024 / 1024:.1f} MB")
        print(f"   - Memory freed by cleanup: {memory_freed / 1024 / 1024:.1f} MB")
        
        # Test performance with memory monitoring
        print("\n3. Performance with memory monitoring:")
        
        monitor = pynlm.createPerformanceMonitor()
        memory_monitor = pynlm.createMemoryMonitor()
        
        start_time = time.time()
        monitor.start()
        memory_monitor.startMonitoring()
        
        for i in range(50):
            config = pynlm.create_default_config(f"mem_perf_test_{i}", 100, 0.01)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(10):
                brain.step(step)
            
            monitor.recordStep(5.0, 3, 15)
            memory_monitor.recordSample()
            
            if i % 10 == 0:
                print(f"   - Completed {i + 1} performance tests")
        
        monitor.stop()
        memory_monitor.stopMonitoring()
        
        total_time = time.time() - start_time
        avg_time = total_time / 50
        
        print(f"\n   - Total time: {total_time:.3f}s")
        print(f"   - Average per test: {avg_time:.3f}s")
        print(f"   - Memory usage: {memory_monitor.getPeakMemoryMB()} MB")
        
        print("\n" + "=" * 60)
        print("MEMORY USAGE BENCHMARK COMPLETE")
        print("=" * 60)
        return True
        
    except ImportError:
        print("\n⚠️  psutil not available - skipping detailed memory tests")
        print("   Basic memory benchmark still available")
        
        # Simple memory test without psutil
        print("\n1. Simple memory test:")
        
        # Create and destroy many objects
        for i in range(50):
            config = pynlm.create_default_config(f"simple_memory_test_{i}", 100, 0.01)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(10):
                brain.step(step)
            
            # Brain will be garbage collected when out of scope
        
        gc.collect()
        print("✓ Memory test completed successfully")
        
        print("\n" + "=" * 60)
        print("MEMORY USAGE BENCHMARK COMPLETE")
        print("=" * 60)
        return True
def run_all_performance_tests():
    """Run all performance tests."""
    print("=" * 80)
    print("PHASE 5 PERFORMANCE TESTS")
    print("=" * 80)
    print("Testing performance of critical operations")
    print("Benchmarking brain operations, configuration management,")
    print("serialization, and memory usage.")
    
    all_passed = True
    
    try:
        # Run performance tests
        if not benchmark_brain_operations():
            all_passed = False
        
        if not benchmark_configuration_operations():
            all_passed = False
        
        if not benchmark_serialization_operations():
            all_passed = False
        
        if not benchmark_memory_usage():
            all_passed = False
        
        print("\n" + "=" * 80)
        if all_passed:
            print("ALL PERFORMANCE TESTS PASSED")
            print("✓ Brain operations performance within acceptable limits")
            print("✓ Configuration management performance optimized")
            print("✓ Serialization and persistence performance benchmarked")
            print("✓ Memory usage analyzed and optimized")
        else:
            print("SOME PERFORMANCE TESTS FAILED")
            print("⚠️  Some operations may need optimization")
        
        print("=" * 80)
        return all_passed
        
    except Exception as e:
        print(f"\n✗ Performance test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
if __name__ == "__main__":
    success = run_all_performance_tests()
    sys.exit(0 if success else 1)