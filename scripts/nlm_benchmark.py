#!/usr/bin/env python3
"""
NLM Benchmark - Performance benchmark for enhanced Python bindings

This script benchmarks the performance of the enhanced NLM Python bindings,
focusing on the new features like batch operations, error handling, and
convenience methods.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src/python'))

import pynlm as nlm
import numpy as np
import time
import statistics
from typing import Dict, Any

def benchmark_configuration_creation():
    """Benchmark configuration creation and validation"""
    print("Benchmark: Configuration Creation")
    
    times = []
    for i in range(100):
        start_time = time.time()
        config = nlm.create_default_config()
        config.set_int("neuron_count", 100 + i * 10)
        config.set_float("simulation_timestep", 0.001 * (1 + i * 0.01))
        config.validate()
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    std_time = statistics.stdev(times)
    
    print(f"  Average time: {avg_time*1000:.2f} ms")
    print(f"  Min time: {min_time*1000:.2f} ms")
    print(f"  Max time: {max_time*1000:.2f} ms")
    print(f"  Std dev: {std_time*1000:.2f} ms")
    
    return avg_time

def benchmark_brain_creation():
    """Benchmark brain creation and initialization"""
    print("Benchmark: Brain Creation and Initialization")
    
    times = []
    for i in range(50):
        start_time = time.time()
        config = nlm.create_default_config()
        config.set_int("neuron_count", 100 + i * 20)
        
        brain = nlm.create_brain(config)
        brain.initialize()
        
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    std_time = statistics.stdev(times)
    
    print(f"  Average time: {avg_time*1000:.2f} ms")
    print(f"  Min time: {min_time*1000:.2f} ms")
    print(f"  Max time: {max_time*1000:.2f} ms")
    print(f"  Std dev: {std_time*1000:.2f} ms")
    
    return avg_time

def benchmark_batch_operations():
    """Benchmark batch operations"""
    print("Benchmark: Batch Operations")
    
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Create test data
    neuron_count = brain.get_total_neuron_count()
    test_neurons = min(10, neuron_count)
    batch_sizes = [1, 5, 10, test_neurons]
    
    results = {}
    
    for batch_size in batch_sizes:
        if batch_size == 0:
            continue
            
        currents = [(nlm.NeuronId(i), 10.0) for i in range(batch_size)]
        
        times = []
        for i in range(100):
            start_time = time.time()
            brain.inject_current_batch(currents)
            elapsed = time.time() - start_time
            times.append(elapsed)
        
        avg_time = statistics.mean(times)
        ops_per_second = 100 / avg_time
        
        results[batch_size] = {
            'avg_time_ms': avg_time * 1000,
            'ops_per_second': ops_per_second
        }
        
        print(f"  Batch size {batch_size}: {avg_time*1000:.2f} ms, {ops_per_second:.0f} ops/sec")
    
    return results

def benchmark_pattern_matching():
    """Benchmark pattern matching operations"""
    print("Benchmark: Pattern Matching")
    
    # Create test patterns
    pattern_size = 1000
    patterns = [
        np.random.random(pattern_size).astype(np.float32) for _ in range(10)
    ]
    
    times = []
    for iteration in range(100):
        start_time = time.time()
        
        # Compute similarity between patterns 0 and 1
        for i in range(1, len(patterns)):
            similarity = nlm.compute_pattern_similarity(
                patterns[0].tolist(),
                patterns[i].tolist()
            )
        
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    std_time = statistics.stdev(times)
    
    print(f"  Average time: {avg_time*1000:.2f} ms")
    print(f"  Min time: {min_time*1000:.2f} ms")
    print(f"  Max time: {max_time*1000:.2f} ms")
    print(f"  Std dev: {std_time*1000:.2f} ms")
    
    return avg_time

def benchmark_error_handling():
    """Benchmark error handling performance"""
    print("Benchmark: Error Handling")
    
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Test various error conditions
    times = []
    for i in range(100):
        start_time = time.time()
        
        try:
            # These operations should fail with appropriate exceptions
            config = nlm.create_default_config()
            # Try to trigger validation error by setting negative values
            config.set_int("test_value", -1)
            config.validate()
        except (nlm.ConfigurationError, nlm.NLMError):
            pass  # Expected
        
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    std_time = statistics.stdev(times)
    
    print(f"  Average time: {avg_time*1000:.2f} ms")
    print(f"  Min time: {min_time*1000:.2f} ms")
    print(f"  Max time: {max_time*1000:.2f} ms")
    print(f"  Std dev: {std_time*1000:.2f} ms")
    
    return avg_time

def benchmark_convenience_methods():
    """Benchmark convenience methods"""
    print("Benchmark: Convenience Methods")
    
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    methods = []
    
    # Benchmark get_neuron_potentials
    times = []
    for i in range(100):
        start_time = time.time()
        potentials = brain.get_neuron_potentials()
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    print(f"  get_neuron_potentials: {avg_time*1000:.2f} ms (100 iterations)")
    methods.append(('get_neuron_potentials', avg_time))
    
    # Benchmark get_synapse_weights
    times = []
    for i in range(100):
        start_time = time.time()
        weights = brain.get_synapse_weights()
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    print(f"  get_synapse_weights: {avg_time*1000:.2f} ms (100 iterations)")
    methods.append(('get_synapse_weights', avg_time))
    
    # Benchmark get_statistics
    times = []
    for i in range(100):
        start_time = time.time()
        stats = brain.get_statistics()
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    print(f"  get_statistics: {avg_time*1000:.2f} ms (100 iterations)")
    methods.append(('get_statistics', avg_time))
    
    # Benchmark get_most_active_neurons
    times = []
    for i in range(100):
        start_time = time.time()
        neurons = brain.get_most_active_neurons(10)
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    print(f"  get_most_active_neurons: {avg_time*1000:.2f} ms (100 iterations)")
    methods.append(('get_most_active_neurons', avg_time))
    
    return methods

def benchmark_memory_usage():
    """Benchmark memory usage of convenience methods"""
    print("Benchmark: Memory Usage")
    
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    import tracemalloc
    
    # Track memory usage for convenience methods
    tracemalloc.start()
    
    # Perform operations
    for i in range(1000):
        brain.get_neuron_potentials()
        brain.get_synapse_weights()
        brain.get_statistics()
        brain.get_most_active_neurons(5)
    
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    print(f"  Current memory usage: {current / 1024 / 1024:.2f} MB")
    print(f"  Peak memory usage: {peak / 1024 / 1024:.2f} MB")
    
    return peak

def benchmark_context_managers():
    """Benchmark context manager performance"""
    print("Benchmark: Context Managers")
    
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    times = []
    for i in range(100):
        start_time = time.time()
        
        with brain.create_simulator() as simulator:
            brain.step(i)
        
        elapsed = time.time() - start_time
        times.append(elapsed)
    
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    std_time = statistics.stdev(times)
    
    print(f"  Average time: {avg_time*1000:.2f} ms")
    print(f"  Min time: {min_time*1000:.2f} ms")
    print(f"  Max time: {max_time*1000:.2f} ms")
    print(f"  Std dev: {std_time*1000:.2f} ms")
    
    return avg_time

def benchmark_comprehensive():
    """Run comprehensive benchmark suite"""
    print("NLM Python Bindings Performance Benchmark")
    print("=" * 60)
    
    results = {}
    
    # Run benchmarks
    results['configuration_creation'] = benchmark_configuration_creation()
    results['brain_creation'] = benchmark_brain_creation()
    results['batch_operations'] = benchmark_batch_operations()
    results['pattern_matching'] = benchmark_pattern_matching()
    results['error_handling'] = benchmark_error_handling()
    results['convenience_methods'] = benchmark_convenience_methods()
    results['memory_usage'] = benchmark_memory_usage()
    results['context_managers'] = benchmark_context_managers()
    
    # Generate summary
    print("\n" + "=" * 60)
    print("Benchmark Summary")
    print("=" * 60)
    
    for category, data in results.items():
        if isinstance(data, dict):
            print(f"{category}:")
            for key, value in data.items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        print(f"  {key}_{subkey}: {subvalue}")
                else:
                    print(f"  {key}: {value}")
        else:
            print(f"{category}: {data}")
    
    print("\nBenchmark completed successfully!")
    
    return results

def main():
    """Main benchmark function"""
    try:
        results = benchmark_comprehensive()
        return 0
    except Exception as e:
        print(f"\nBenchmark failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())