"""
Performance benchmarking utilities for NLM

This module provides comprehensive benchmarking capabilities for the NLM (Neural Learning Machine)
framework, allowing users to measure performance across different network configurations,
evaluate simulation efficiency, and compare different setups.

Key Features:
- Benchmark multiple network sizes
- Measure simulation performance (steps per second, spikes per step)
- Compare different configurations
- Generate detailed performance reports
- Support for various benchmarking scenarios
"""

import time
import statistics
from typing import List, Dict, Any, Optional
from pathlib import Path
import sys

# Add the parent directory to sys.path to import nlm
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm import Config, createBrain


def benchmark_configuration(config: Config, steps: int, description: str) -> Dict[str, Any]:
    """
    Benchmark a specific configuration.
    
    Args:
        config: NLM configuration
        steps: Number of simulation steps
        description: Description of the benchmark
        
    Returns:
        Dictionary with benchmark results
    """
    print(f"\nRunning benchmark: {description}")
    print(f"  Neurons: {config.getOr('neuron_count', 0)}")
    print(f"  Steps: {steps}")
    
    # Create brain
    brain = createBrain(config)
    brain.initialize()
    
    # Run simulation and measure performance
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        # Occasionally inject sensory input for realistic simulation
        if step % 100 == 0:
            pass  # Could add world interaction here
    
    end_time = time.time()
    
    total_time = end_time - start_time
    steps_per_second = steps / total_time if total_time > 0 else 0
    
    # Collect statistics
    results = {
        'description': description,
        'neuron_count': config.getOr('neuron_count', 0),
        'steps': steps,
        'total_time': total_time,
        'steps_per_second': steps_per_second,
        'spikes_per_step': brain.getTotalSpikeCount() / steps if steps > 0 else 0,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate_hz': brain.getAverageFiringRate(),
        'active_neurons': brain.getActiveNeuronCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
    }
    
    print(f"  Time: {total_time:.2f}s")
    print(f"  Steps/sec: {steps_per_second:.0f}")
    print(f"  Spikes/step: {results['spikes_per_step']:.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing rate: {results['firing_rate_hz']:.2f} Hz")
    
    return results


def run_benchmark_suite(benchmarks: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    """
    Run a suite of benchmarks.
    
    Args:
        benchmarks: List of benchmark configurations
        
    Returns:
        List of benchmark results
    """
    results = []
    
    print("NLM Performance Benchmark Suite")
    print("=================================")
    
    for benchmark in benchmarks:
        config = benchmark['config']
        steps = benchmark['steps']
        description = benchmark['description']
        
        result = benchmark_configuration(config, steps, description)
        results.append(result)
    
    # Summary
    print("\nBenchmark Summary")
    print("=================")
    print(f"{'Description':<35} {'Neurons':<10} {'Steps/sec':<12} {'Spikes/step':<12} {'Time (s)':<10}")
    print("-" * 95)
    
    for result in results:
        print(f"{result['description']:<35} {result['neuron_count']:<10} "
              f"{result['steps_per_second']:<12.0f} "
              f"{result['spikes_per_step']:<12.2f} "
              f"{result['total_time']:<10.2f}")
    
    return results


def profile_memory_usage(brain, steps: int) -> Dict[str, Any]:
    """
    Profile memory usage of NLM operations.
    
    Args:
        brain: NLM brain instance
        steps: Number of steps to simulate
        
    Returns:
        Memory usage statistics
    """
    print("\nMemory Profiling")
    print("=================")
    print("Memory profiling requires additional tools (e.g., memory_profiler)")
    print("For now, monitoring basic brain state sizes:")
    
    # Estimate memory usage based on brain structure
    stats = {
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'region_count': brain.getRegionCount(),
        'estimated_memory_kb': (brain.getTotalNeuronCount() * 100 +  # ~100 bytes per neuron
                               brain.getTotalSynapseCount() * 50 +   # ~50 bytes per synapse
                               brain.getRegionCount() * 1000) / 1024,  # ~1KB per region
    }
    
    print(f"  Estimated memory usage: {stats['estimated_memory_kb']:.1f} KB")
    print(f"  Neurons: {stats['neuron_count']}")
    print(f"  Synapses: {stats['synapse_count']}")
    print(f"  Regions: {stats['region_count']}")
    
    return stats


def compare_configurations(configs: List[Config], descriptions: List[str], steps: int) -> List[Dict[str, Any]]:
    """
    Compare multiple configurations.
    
    Args:
        configs: List of configurations to compare
        descriptions: List of descriptions for each configuration
        steps: Number of steps to simulate for comparison
        
    Returns:
        Comparison results
    """
    if len(configs) != len(descriptions):
        raise ValueError("Number of configs must match number of descriptions")
    
    print(f"\nComparing Configurations (Steps: {steps})")
    print("=========================================")
    
    results = []
    for i, (config, description) in enumerate(zip(configs, descriptions)):
        print(f"\n{i+1}. {description}")
        
        # Create brain
        brain = createBrain(config)
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        result = {
            'description': description,
            'neuron_count': config.getOr('neuron_count', 0),
            'steps_per_second': steps / (end_time - start_time),
            'spikes_per_step': brain.getTotalSpikeCount() / steps,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
        }
        
        print(f"  Steps/sec: {result['steps_per_second']:.0f}")
        print(f"  Spikes/step: {result['spikes_per_step']:.2f}")
        print(f"  Total spikes: {result['total_spikes']}")
        print(f"  Firing rate: {result['firing_rate']:.2f} Hz")
        print(f"  E/I ratio: {result['exc_inh_ratio']:.2f}")
        
        results.append(result)
    
    return results


def generate_performance_report(results: List[Dict[str, Any]], report_file: Optional[str] = None):
    """
    Generate a detailed performance report.
    
    Args:
        results: List of benchmark results
        report_file: Optional file path to save the report
    """
    print("\nDetailed Performance Report")
    print("===========================")
    
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
    print(f"Report generated: {timestamp}")
    print(f"Number of benchmarks: {len(results)}")
    print()
    
    # Overall statistics
    all_neurons = [r['neuron_count'] for r in results]
    all_steps_per_sec = [r['steps_per_second'] for r in results]
    all_spikes_per_step = [r['spikes_per_step'] for r in results]
    
    print("Overall Statistics:")
    print(f"  Neuron count range: {min(all_neurons)} - {max(all_neurons)}")
    print(f"  Steps/sec range: {min(all_steps_per_sec):.0f} - {max(all_steps_per_sec):.0f}")
    print(f"  Steps/sec average: {statistics.mean(all_steps_per_sec):.0f}")
    print(f"  Steps/sec std dev: {statistics.stdev(all_steps_per_sec) if len(all_steps_per_sec) > 1 else 0:.0f}")
    print(f"  Spikes/step range: {min(all_spikes_per_step):.2f} - {max(all_spikes_per_step):.2f}")
    print()
    
    # Detailed results
    for i, result in enumerate(results):
        print(f"Benchmark {i+1}: {result['description']}")
        print(f"  Configuration: {result['neuron_count']} neurons")
        print(f"  Performance: {result['steps_per_second']:.0f} steps/second")
        print(f"  Activity: {result['spikes_per_step']:.2f} spikes per step")
        print(f"  Total simulation time: {result['total_time']:.2f} seconds")
        print()
    
    # Save report if requested
    if report_file:
        report_data = {
            'timestamp': timestamp,
            'results': results,
            'summary': {
                'average_steps_per_second': statistics.mean(all_steps_per_sec),
                'std_dev_steps_per_second': statistics.stdev(all_steps_per_sec) if len(all_steps_per_sec) > 1 else 0,
                'average_spikes_per_step': statistics.mean(all_spikes_per_step),
                'neuron_count_range': {'min': min(all_neurons), 'max': max(all_neurons)},
            }
        }
        
        with open(report_file, 'w') as f:
            import json
            json.dump(report_data, f, indent=2)
        
        print(f"Report saved to: {report_file}")


def main_benchmark_runner(args):
    """
    Main benchmark runner function.
    
    Args:
        args: Command-line arguments
        
    Returns:
        Benchmark results
    """
    # Create different benchmark configurations
    benchmarks = [
        {
            'config': Config(),
            'steps': 1000,
            'description': 'Small Network (100 neurons)'
        },
        {
            'config': Config(),
            'steps': 500,
            'description': 'Medium Network (500 neurons)'
        },
        {
            'config': Config(),
            'steps': 200,
            'description': 'Large Network (1000 neurons)'
        }
    ]
    
    # Set different neuron counts for each benchmark
    neuron_counts = [100, 500, 1000]
    for i, neuron_count in enumerate(neuron_counts):
        benchmarks[i]['config'].set("neuron_count", neuron_count)
        benchmarks[i]['description'] = f"Benchmark {i+1}: {neuron_count} neurons"
    
    # Run benchmarks
    results = run_benchmark_suite(benchmarks)
    
    # Generate detailed report
    generate_performance_report(results)
    
    return results


if __name__ == "__main__":
    # Example usage
    from nlm import createDefaultConfig
    
    # Create different benchmark configurations
    benchmarks = [
        {
            'config': createDefaultConfig(),
            'steps': 1000,
            'description': 'Small Network'
        },
        {
            'config': createDefaultConfig(),
            'steps': 500,
            'description': 'Medium Network'
        },
        {
            'config': createDefaultConfig(),
            'steps': 200,
            'description': 'Large Network'
        }
    ]
    
    # Set different neuron counts for each benchmark
    configs = [
        {"neuron_count": 100},
        {"neuron_count": 500},
        {"neuron_count": 1000}
    ]
    
    for i, config_dict in enumerate(configs):
        for key, value in config_dict.items():
            benchmarks[i]['config'].set(key, value)
    
    # Run benchmarks
    results = run_benchmark_suite(benchmarks)
    
    # Generate detailed report
    generate_performance_report(results)