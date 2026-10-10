#!/usr/bin/env python3

"""
Example: Performance Benchmarking

This script demonstrates how to benchmark NLM brain performance
and optimize configuration for different use cases.
"""

import pynlm
import time
import statistics
import matplotlib.pyplot as plt

def benchmark_brain(config, steps, description=""):
    """
    Benchmark brain performance with given configuration.
    
    Args:
        config: NLM configuration
        steps: Number of simulation steps
        description: Description of the benchmark
        
    Returns:
        dict: Benchmark results
    """
    print(f"Benchmarking: {description}")
    print(f"  Configuration: {config.get('brain.neuron_count')} neurons, "
          f"{config.get('brain.synapse_density')} synapse density")
    print(f"  Running {steps} simulation steps...")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Measure performance
    start_time = time.time()
    step_times = []
    
    for step in range(steps):
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        step_times.append(step_time)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    # Calculate statistics
    steps_per_second = steps / total_time
    avg_step_time = statistics.mean(step_times)
    min_step_time = min(step_times)
    max_step_time = max(step_times)
    std_step_time = statistics.stdev(step_times) if len(step_times) > 1 else 0.0
    
    # Brain statistics
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    total_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    e_i_ratio = brain.getExcitationInhibitionRatio()
    
    results = {
        'description': description,
        'steps': steps,
        'total_time': total_time,
        'steps_per_second': steps_per_second,
        'avg_step_time': avg_step_time,
        'min_step_time': min_step_time,
        'max_step_time': max_step_time,
        'std_step_time': std_step_time,
        'total_neurons': total_neurons,
        'total_synapses': total_synapses,
        'total_spikes': total_spikes,
        'avg_firing_rate': avg_firing_rate,
        'e_i_ratio': e_i_ratio,
        'neuron_density': total_synapses / total_neurons if total_neurons > 0 else 0,
    }
    
    # Print results
    print(f"  Results:")
    print(f"    Total time: {total_time:.3f}s")
    print(f"    Steps/second: {steps_per_second:.1f}")
    print(f"    Avg step time: {avg_step_time*1000:.1f}ms")
    print(f"    Step time std: {std_step_time*1000:.1f}ms")
    print(f"    Total neurons: {total_neurons}")
    print(f"    Total synapses: {total_synapses}")
    print(f"    Total spikes: {total_spikes}")
    print(f"    Avg firing rate: {avg_firing_rate:.3f} Hz")
    print(f"    E/I ratio: {e_i_ratio:.3f}")
    
    return results

def optimize_configuration(base_config, test_steps=1000):
    """
    Optimize configuration for different scenarios.
    
    Args:
        base_config: Base configuration
        test_steps: Number of steps for testing
        
    Returns:
        list: List of benchmark results for different configurations
    """
    print("=== Configuration Optimization ===")
    print("Testing different configurations for optimal performance...")
    
    configurations = []
    
    # Configuration 1: Small brain for testing
    config1 = pynlm.get_default_configuration()
    config1.set("brain.neuron_count", 500)
    config1.set("brain.synapse_density", 0.1)
    configurations.append((config1, "Small (500 neurons)"))
    
    # Configuration 2: Medium brain for realistic simulation
    config2 = pynlm.get_default_configuration()
    config2.set("brain.neuron_count", 2000)
    config2.set("brain.synapse_density", 0.05)
    configurations.append((config2, "Medium (2000 neurons)"))
    
    # Configuration 3: Large brain for comprehensive simulation
    config3 = pynlm.get_default_configuration()
    config3.set("brain.neuron_count", 5000)
    config3.set("brain.synapse_density", 0.02)
    configurations.append((config3, "Large (5000 neurons)"))
    
    # Configuration 4: High connectivity
    config4 = pynlm.get_default_configuration()
    config4.set("brain.neuron_count", 1000)
    config4.set("brain.synapse_density", 0.2)
    configurations.append((config4, "High density (1000 neurons, 0.2 density)"))
    
    # Configuration 5: Optimized for learning
    config5 = pynlm.get_default_configuration()
    config5.set("brain.neuron_count", 1500)
    config5.set("brain.synapse_density", 0.08)
    # Enable all learning mechanisms
    config5.set("plasticity.stdp.enable", True)
    config5.set("plasticity.hebbian.enable", True)
    config5.set("plasticity.structural.enable", True)
    config5.set("neuromod.dopamine.scale", 1.0)
    config5.set("neuromod.curiosity.enable", True)
    config5.set("neuromod.novelty.enable", True)
    config5.set("neuromod.prediction.error.scale", 1.0)
    configurations.append((config5, "Learning optimized"))
    
    results = []
    
    for config, description in configurations:
        try:
            result = benchmark_brain(config, test_steps, description)
            results.append(result)
        except Exception as e:
            print(f"  ERROR: {e}")
            print(f"  Skipping configuration: {description}")
    
    return results

def generate_performance_report(results):
    """
    Generate a performance comparison report.
    
    Args:
        results: List of benchmark results
    """
    if not results:
        print("No results to report")
        return
    
    print("\n=== Performance Report ===")
    
    # Find best performing configuration
    best_performance = max(results, key=lambda r: r['steps_per_second'])
    best_efficiency = min(results, key=lambda r: r['avg_firing_rate'] / r['steps_per_second'])
    
    print(f"Best Performance (steps/second): {best_performance['description']}")
    print(f"  Steps/second: {best_performance['steps_per_second']:.1f}")
    print(f"  Avg step time: {best_performance['avg_step_time']*1000:.1f}ms")
    
    print(f"\nBest Efficiency (steps/second per neuron): {best_efficiency['description']}")
    print(f"  Steps/second: {best_efficiency['steps_per_second']:.1f}")
    print(f"  Neurons: {best_efficiency['total_neurons']}")
    print(f"  Efficiency: {best_efficiency['steps_per_second']/best_efficiency['total_neurons']:.3f} steps/second/neuron")
    
    print(f"\nConfiguration Comparison:")
    print(f"{'Description':<20} {'Steps/s':>10} {'Avg Step (ms)':>12} {'Neurons':>8} {'Synapses':>10} {'Spikes/s':>10}")
    print(f"{'='*80}")
    
    for result in results:
        print(f"{result['description']:<20} {result['steps_per_second']:>10.1f} "
              f"{result['avg_step_time']*1000:>12.1f} {result['total_neurons']:>8} "
              f"{result['total_synapses']:>10} {result['total_spikes']/result['total_time'] if result['total_time'] > 0 else 0:>10.0f}")

def main():
    """Main function to run the benchmark example."""
    try:
        print("=== NLM Performance Benchmarking ===")
        
        # Test with recommended configuration
        config = pynlm.get_default_configuration()
        
        print("\n1. Running baseline benchmark...")
        baseline_results = benchmark_brain(config, 1000, "Baseline (recommended config)")
        
        print("\n2. Testing different configurations...")
        optimization_results = optimize_configuration(config, 500)
        
        print("\n3. Generating performance report...")
        generate_performance_report([baseline_results] + optimization_results)
        
        print("\n=== Benchmark Complete ===")
        print("\nNext steps:")
        print("  - Experiment with different neuron counts")
        print("  - Test various synapse densities")
        print("  - Compare performance with and without learning")
        print("  - Run real-world simulations with agent-world interaction")
        
    except Exception as e:
        print(f"Error running benchmark: {e}")
        print("\nMake sure you have:")
        print("  1. Built the NLM Python bindings")
        print("  2. Installed required dependencies (matplotlib, numpy)")
        print("  Run: python python/build.py --install")
        print("  pip install matplotlib numpy")

if __name__ == "__main__":
    main()