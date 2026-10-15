#!/usr/bin/env python3

"""
Example 5: Performance Benchmarking

Demonstrates performance benchmarking for different brain sizes.
This example shows how to measure and analyze performance.
"""

import sys
import os
import time

# Add parent directory to path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
import json
from pathlib import Path

def main():
    print("=== NLM Example 5: Performance Benchmarking ===\n")
    
    # Test different brain sizes
    brain_sizes = [100, 500, 1000, 2000]
    
    print(f"Running benchmarks for different brain sizes...")
    print(f"{'Neurons':<10} {'Synapses':<12} {'Spikes':<10} {'Time(s)':<10} {'Steps/s':<10} {'Spikes/step':<12}")
    print(f"{'-' * 70}")
    
    benchmark_results = []
    
    for neurons in brain_sizes:
        print(f"\nBenchmarking {neurons} neurons...")
        
        # Create configuration
        config = pynlm.create_default_config()
        config.set("neuron_count", neurons)
        
        # Create brain
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Run benchmark
        steps = 200
        start_time = time.time()
        
        for step in range(steps):
            brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.SENSORY, 10.0f)
            brain.step(step, step * 0.001)
        
        end_time = time.time()
        total_time = end_time - start_time
        steps_per_second = steps / total_time
        spikes_per_step = brain.get_total_spike_count() / steps
        
        benchmark_results.append({
            "neurons": neurons,
            "synapses": brain.get_total_synapse_count(),
            "total_spikes": brain.get_total_spike_count(),
            "total_time": total_time,
            "steps_per_second": steps_per_second,
            "spikes_per_step": spikes_per_step
        })
        
        print(f"{neurons:<10} {brain.get_total_synapse_count():<12} {brain.get_total_spike_count():<10} "
              f"{total_time:<10.2f} {steps_per_second:<10.0f} {spikes_per_step:<12.2f}")
    
    # Print benchmark summary
    print(f"\nBenchmark Summary:")
    
    # Save benchmark results
    print(f"\nSaving benchmark results...")
    
    benchmark_path = Path("results") / "benchmark_results.json"
    with open(benchmark_path, "w") as f:
        json.dump(benchmark_results, f, indent=2)
    
    print(f"   Results saved to: {benchmark_path}")
    
    print(f"\n✓ Example completed successfully!")
    return True

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n✗ Error: {e}")
        sys.exit(1)