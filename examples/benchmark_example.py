#!/usr/bin/env python3
"""
NLM Python Example: Performance Benchmarking
=============================================

This example demonstrates the enhanced NLM API with performance benchmarking,
profiling, and optimization analysis.
"""

import pynlm as nlm
import pynlm.utils as utils
import time
import numpy as np

def main():
    print("NLM Performance Benchmarking Example")
    print("=" * 50)
    
    # Create configurations for benchmarking
    print("1. Setting up benchmark configurations...")
    
    # Small brain configuration
    small_config = utils.ConfigUtils.create_validation_config()
    small_config.set("neuron_count", 100)
    small_config.set("region_count", 2)
    
    # Medium brain configuration
    medium_config = utils.ConfigUtils.create_research_config(neuron_count=1000)
    
    # Large brain configuration
    large_config = utils.ConfigUtils.create_research_config(neuron_count=5000)
    
    # Create brains
    print("\n2. Creating benchmark brains...")
    small_brain = nlm.create_brain(small_config)
    medium_brain = nlm.create_brain(medium_config)
    large_brain = nlm.create_brain(large_config)
    
    # Create worlds
    print("\n3. Creating benchmark worlds...")
    small_world = utils.WorldUtils.create_empty_world(width=50, height=50)
    medium_world = utils.WorldUtils.create_empty_world(width=100, height=100)
    large_world = utils.WorldUtils.create_empty_world(width=200, height=200)
    
    # Run benchmark
    print("\n4. Running performance benchmark...")
    benchmark_steps = 100
    iterations = 3
    
    results = {}
    
    # Small brain benchmark
    print(f"   Small brain (100 neurons):")
    with utils.SimulationUtils.monitor_performance():
        start_time = time.time()
        for iteration in range(iterations):
            small_stats = nlm.BrainUtils.run_simulation(small_brain, benchmark_steps, 1.0)
        end_time = time.time()
        
        small_benchmark = utils.run_benchmark(small_brain, small_world, benchmark_steps, iterations)
        results['small'] = small_benchmark
        print(f"     Mean time: {small_benchmark['mean_time']:.3f}s")
        print(f"     Steps per second: {small_benchmark['steps_per_second']:.0f}")
    
    # Medium brain benchmark
    print(f"\n   Medium brain (1000 neurons):")
    with utils.SimulationUtils.monitor_performance():
        start_time = time.time()
        for iteration in range(iterations):
            medium_stats = nlm.BrainUtils.run_simulation(medium_brain, benchmark_steps, 1.0)
        end_time = time.time()
        
        medium_benchmark = utils.run_benchmark(medium_brain, medium_world, benchmark_steps, iterations)
        results['medium'] = medium_benchmark
        print(f"     Mean time: {medium_benchmark['mean_time']:.3f}s")
        print(f"     Steps per second: {medium_benchmark['steps_per_second']:.0f}")
    
    # Large brain benchmark
    print(f"\n   Large brain (5000 neurons):")
    with utils.SimulationUtils.monitor_performance():
        start_time = time.time()
        for iteration in range(iterations):
            large_stats = nlm.BrainUtils.run_simulation(large_brain, benchmark_steps, 1.0)
        end_time = time.time()
        
        large_benchmark = utils.run_benchmark(large_brain, large_world, benchmark_steps, iterations)
        results['large'] = large_benchmark
        print(f"     Mean time: {large_benchmark['mean_time']:.3f}s")
        print(f"     Steps per second: {large_benchmark['steps_per_second']:.0f}")
    
    # Analyze benchmark results
    print("\n5. Analyzing benchmark results...")
    
    print("\n   Benchmark Summary:")
    print("     Configuration | Neurons | Steps/Sec | Mean Time (s)")
    print("     --------------|---------|-----------|---------------")
    for size, result in results.items():
        neurons = {
            'small': 100,
            'medium': 1000,
            'large': 5000
        }[size]
        print(f"     {size.capitalize():12} | {neurons:7d} | {result['steps_per_second']:9.0f} | {result['mean_time']:13.3f}")
    
    # Performance scaling analysis
    print("\n   Performance Scaling Analysis:")
    neurons = [100, 1000, 5000]
    steps_per_sec = [results['small']['steps_per_second'],
                     results['medium']['steps_per_second'],
                     results['large']['steps_per_second']]
    
    # Calculate scaling factors
    scaling_factor = []
    for i in range(1, len(neurons)):
        neuron_ratio = neurons[i] / neurons[i-1]
        speed_ratio = steps_per_sec[i] / steps_per_sec[i-1]
        scaling_factor.append(speed_ratio / neuron_ratio)
    
    print(f"     Neuron count scaling: {neurons}")
    print(f"     Steps per second: {steps_per_sec}")
    print(f"     Scaling factor (vs linear): {scaling_factor}")
    
    # Simulate brain analysis
    print("\n   Brain Statistics Comparison:")
    for size, brain in [('Small', small_brain), ('Medium', medium_brain), ('Large', large_brain)]:
        stats = nlm.BrainUtils.get_neuron_statistics(brain)
        print(f"     {size} brain:")
        print(f"       Total neurons: {stats['total_neurons']}")
        print(f"       Average firing rate: {stats['avg_rate']:.2f} Hz")
        print(f"       E/I ratio: {stats['e_i_ratio']:.2f}")
        print(f"       Total spikes: {stats['spike_count']}")
    
    # Plot benchmark results
    try:
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Performance comparison
        axes[0, 0].bar(['Small', 'Medium', 'Large'], steps_per_sec, color=['skyblue', 'lightgreen', 'salmon'])
        axes[0, 0].set_title('Steps per Second')
        axes[0, 0].set_ylabel('Steps/sec')
        
        # Timing comparison
        axes[0, 1].bar(['Small', 'Medium', 'Large'], 
                       [results['small']['mean_time'], results['medium']['mean_time'], results['large']['mean_time']],
                       color=['skyblue', 'lightgreen', 'salmon'])
        axes[0, 1].set_title('Mean Time per Simulation')
        axes[0, 1].set_ylabel('Seconds')
        
        # Neuron count vs performance
        axes[1, 0].scatter(neurons, steps_per_sec, s=[100, 500, 1000], alpha=0.7)
        for i, neuron in enumerate(neurons):
            axes[1, 0].annotate(f'{neuron}', (neuron, steps_per_sec[i]), textcoords="offset points", 
                              xytext=(0,10), ha='center')
        axes[1, 0].set_title('Neuron Count vs Performance')
        axes[1, 0].set_xlabel('Neuron Count')
        axes[1, 0].set_ylabel('Steps per Second')
        axes[1, 0].set_xscale('log')
        axes[1, 0].set_yscale('log')
        
        # Statistical summary
        axes[1, 1].axis('tight')
        axes[1, 1].axis('off')
        
        stats_table = [['Metric', 'Small', 'Medium', 'Large'],
                      ['Neurons', 100, 1000, 5000],
                      ['Steps/sec', f"{steps_per_sec[0]:.0f}", f"{steps_per_sec[1]:.0f}", f"{steps_per_sec[2]:.0f}"],
                      ['Mean Time (s)', f"{results['small']['mean_time']:.3f}", f"{results['medium']['mean_time']:.3f}", f"{results['large']['mean_time']:.3f}"],
                      ['E/I Ratio', f"{nlm.BrainUtils.get_neuron_statistics(small_brain)['e_i_ratio']:.2f}", 
                       f"{nlm.BrainUtils.get_neuron_statistics(medium_brain)['e_i_ratio']:.2f}",
                       f"{nlm.BrainUtils.get_neuron_statistics(large_brain)['e_i_ratio']:.2f}"]]
        
        table = axes[1, 1].table(cellText=stats_table, loc='center', cellLoc='center')
        table.auto_set_font_size(False)
        table.set_fontsize(9)
        table.scale(1, 1.5)
        axes[1, 1].set_title('Benchmark Summary')
        
        plt.tight_layout()
        plt.savefig('benchmark_results.png', dpi=150, bbox_inches='tight')
        print("\n   Plot saved: benchmark_results.png")
        
    except ImportError:
        print("\n   Matplotlib not available, skipping plots")
    
    # Save benchmark results
    print("\n6. Saving benchmark results...")
    benchmark_data = {
        'configurations': {
            'small': small_config.to_dict(),
            'medium': medium_config.to_dict(),
            'large': large_config.to_dict()
        },
        'benchmark_results': results,
        'neurons': neurons,
        'steps_per_second': steps_per_sec,
        'scaling_factors': scaling_factor,
        'timestamp': time.time()
    }
    
    utils.DataUtils.save_to_file(benchmark_data, "benchmark_results.json")
    print("   Saved benchmark results to: benchmark_results.json")
    
    # Save brain states
    small_brain.save("small_brain.nlm")
    medium_brain.save("medium_brain.nlm")
    large_brain.save("large_brain.nlm")
    print("   Saved brain states to: small_brain.nlm, medium_brain.nlm, large_brain.nlm")
    
    print("\nBenchmark completed successfully!")
    print("\nPerformance Summary:")
    print(f"  - Small brain (100 neurons): {results['small']['steps_per_second']:.0f} steps/sec")
    print(f"  - Medium brain (1000 neurons): {results['medium']['steps_per_second']:.0f} steps/sec")
    print(f"  - Large brain (5000 neurons): {results['large']['steps_per_second']:.0f} steps/sec")
    print(f"  - Scaling factor: {scaling_factor[-1]:.2f} (vs linear scaling)")

if __name__ == "__main__":
    main()
