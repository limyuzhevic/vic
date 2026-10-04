#!/usr/bin/env python3
"""
Performance Benchmarking for NLM

This script benchmarks the performance of NLM Python bindings and
compares different configurations for optimal performance.
"""

import sys
import time
import statistics
from pathlib import Path

def time_operation(operation, *args, **kwargs):
    """Time an operation and return the result and execution time"""
    start_time = time.perf_counter()
    result = operation(*args, **kwargs)
    end_time = time.perf_counter()
    return result, end_time - start_time

def benchmark_configuration(config_name, neuron_count, synapse_density, steps):
    """Benchmark a specific configuration"""
    try:
        import pynlm
        
        print(f"\nBenchmarking {config_name}:")
        print(f"  Configuration: {neuron_count} neurons, {synapse_density} synapse density")
        print(f"  Simulation steps: {steps}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", synapse_density)
        
        # Warm-up run
        brain = pynlm.createBrain(config)
        brain.initialize()
        for i in range(10):
            brain.step(i)
        
        # Benchmark
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        step_times = []
        spike_counts = []
        
        for step in range(steps):
            step_start = time.perf_counter()
            brain.step(step)
            step_end = time.perf_counter()
            
            step_times.append(step_end - step_start)
            spike_counts.append(brain.getTotalSpikeCount())
        
        # Calculate statistics
        avg_step_time = statistics.mean(step_times)
        std_step_time = statistics.stdev(step_times) if len(step_times) > 1 else 0.0
        total_time = sum(step_times)
        
        # Performance metrics
        steps_per_second = steps / total_time if total_time > 0 else 0
        avg_spikes_per_step = statistics.mean(spike_counts) if spike_counts else 0
        max_spikes_per_step = max(spike_counts) if spike_counts else 0
        
        print(f"  Performance:")
        print(f"    Total time: {total_time:.3f}s")
        print(f"    Steps per second: {steps_per_second:.1f}")
        print(f"    Average step time: {avg_step_time*1000:.2f}ms (±{std_step_time*1000:.2f}ms)")
        print(f"    Average spikes per step: {avg_spikes_per_step:.1f}")
        print(f"    Maximum spikes per step: {max_spikes_per_step}")
        
        return {
            'config': config_name,
            'neurons': neuron_count,
            'synapse_density': synapse_density,
            'steps': steps,
            'total_time': total_time,
            'steps_per_second': steps_per_second,
            'avg_step_time': avg_step_time,
            'std_step_time': std_step_time,
            'avg_spikes_per_step': avg_spikes_per_step,
            'max_spikes_per_step': max_spikes_per_step
        }
        
    except ImportError:
        print(f"  ✗ Failed to import pynlm for {config_name}")
        return None
    except Exception as e:
        print(f"  ✗ Error benchmarking {config_name}: {e}")
        return None

def main():
    """Main benchmarking function"""
    print("=" * 70)
    print("NLM Performance Benchmarking")
    print("=" * 70)
    
    # Define configurations to benchmark
    configurations = [
        ("Small", 100, 0.05, 100),
        ("Medium", 500, 0.08, 200),
        ("Large", 1000, 0.1, 150),
        ("XLarge", 2000, 0.12, 100),
    ]
    
    results = []
    
    print("\nRunning performance benchmarks...")
    for config_name, neurons, density, steps in configurations:
        result = benchmark_configuration(config_name, neurons, density, steps)
        if result:
            results.append(result)
    
    # Print summary
    if results:
        print("\n" + "=" * 70)
        print("Benchmark Summary")
        print("=" * 70)
        
        print(f"{'Configuration':<12} {'Neurons':<8} {'Density':<8} {'Steps':<6} {'Time(s)':<8} {'Steps/s':<8} {'Avg Step(ms)':<12}")
        print("-" * 70)
        
        for result in results:
            print(f"{result['config']:<12} {result['neurons']:<8} "
                  f"{result['synapse_density']:<8.3f} {result['steps']:<6} "
                  f"{result['total_time']:<8.3f} {result['steps_per_second']:<8.1f} "
                  f"{result['avg_step_time']*1000:<12.2f}")
        
        # Find best performing configuration
        if results:
            fastest = min(results, key=lambda x: x['total_time'] / x['steps'])
            most_steps = max(results, key=lambda x: x['steps_per_second'])
            
            print(f"\nBest Performing Configuration:")
            print(f"  Fastest: {fastest['config']} ({fastest['steps_per_second']:.1f} steps/s)")
            print(f"  Most Steps/s: {most['config']} ({most['steps_per_second']:.1f} steps/s)")
            
            # Memory efficiency (spikes per step)
            most_efficient = max(results, key=lambda x: x['avg_spikes_per_step'])
            print(f"  Most Efficient: {most_efficient['config']} ({most_efficient['avg_spikes_per_step']:.1f} avg spikes/step)")
    
    print("\n" + "=" * 70)
    print("Benchmark completed!")
    print("=" * 70)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())