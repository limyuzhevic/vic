"""
Performance Benchmarking Example
=================================

This example demonstrates how to benchmark and optimize NLM brain performance.
It shows how to measure simulation speed, resource usage, and identify
performance bottlenecks for optimization.

## What This Example Shows

1. Setting up performance benchmarks
2. Measuring simulation throughput
3. Analyzing neuron processing rates
4. Identifying performance bottlenecks
5. Comparing different configurations
6. Best practices for performance optimization

## When to Use This Example

- You need to optimize brain performance
- You're running large-scale simulations
- You want to compare different configurations
- You're preparing for performance-critical applications
- You need to validate that optimizations are working
"""

import pynlm
from advanced_api import AdvancedBrain, NLMConfigurator, benchmark_brain, validate_brain_state
import time
import statistics

def main():
    """Run the performance benchmarking example"""
    print("=== Performance Benchmarking Example ===")
    print()
    
    # 1. Set up benchmark configurations
    print("Step 1: Setting up benchmark configurations...")
    
    # Small brain for quick testing
    small_config = NLMConfigurator.create_config('minimal')
    small_brain = AdvancedBrain(small_config)
    
    # Medium brain for standard testing
    medium_config = NLMConfigurator.create_config('production')
    medium_brain = AdvancedBrain(medium_config)
    
    # Large brain for stress testing (if configured)
    large_config = NLMConfigurator.create_config(brain.neuron_count=2000)
    large_brain = AdvancedBrain(large_config)
    
    brains = {
        'small': small_brain,
        'medium': medium_brain,
        'large': large_brain
    }
    
    print("Benchmark configurations:")
    for name, brain in brains.items():
        print(f"  {name:6s}: {brain.getTotalNeuronCount():6d} neurons")
    
    # 2. Validate configurations
    print("Step 2: Validating configurations...")
    
    for name, brain in brains.items():
        issues = validate_brain_state(brain)
        if issues:
            print(f"  {name:6s}: VALIDATION WARNINGS - {issues}")
        else:
            print(f"  {name:6s}: VALIDATION PASSED")
    
    # 3. Run benchmarks
    print("Step 3: Running performance benchmarks...")
    print()
    
    benchmark_results = {}
    
    for name, brain in brains.items():
        print(f"Benchmarking {name} brain ({brain.getTotalNeuronCount()} neurons)...")
        
        # Run multiple trials for stable measurements
        trial_times = []
        trial_rates = []
        
        for trial in range(3):
            print(f"  Trial {trial + 1}/3...")
            
            start_time = time.time()
            benchmark = benchmark_brain(brain, 500)
            end_time = time.time()
            
            trial_times.append(end_time - start_time)
            trial_rates.append(benchmark['steps_per_second'])
            
            # Brief pause between trials
            time.sleep(0.1)
        
        # Calculate statistics
        avg_time = statistics.mean(trial_times)
        std_time = statistics.stdev(trial_times) if len(trial_times) > 1 else 0
        avg_rate = statistics.mean(trial_rates)
        std_rate = statistics.stdev(trial_rates) if len(trial_rates) > 1 else 0
        
        benchmark_results[name] = {
            'avg_time': avg_time,
            'std_time': std_time,
            'avg_rate': avg_rate,
            'std_rate': std_rate,
            'neurons': brain.getTotalNeuronCount(),
            'final_firing_rate': brain.getAverageFiringRate()
        }
        
        print(f"    Average time: {avg_time:.3f}s ± {std_time:.3f}s")
        print(f"    Average rate: {avg_rate:.1f} ± {std_rate:.1f} steps/sec")
        print(f"    Neurons per second: {avg_rate * brain.getTotalNeuronCount():.0f}")
    
    # 4. Analyze and compare results
    print("Step 4: Analyzing and comparing results...")
    print()
    
    print("Performance comparison:")
    print(f"{'Configuration':<12} {'Neurons':<8} {'Time(s)':<10} {'Rate(steps/s)':<15} {'Neurons/sec':<15}")
    print(f"{'------------':<12} {'-------':<8} {'--------':<10} {'---------------':<15} {'------------':<15}")
    
    for name, results in benchmark_results.items():
        neurons_per_sec = results['avg_rate'] * results['neurons']
        print(f"{name:<12} {results['neurons']:<8} "
              f"{results['avg_time']:<10.3f} "
              f"{results['avg_rate']:<15.1f} "
              f"{neurons_per_sec:<15.0f}")
    
    # 5. Scaling analysis
    print("Step 5: Scaling analysis...")
    
    # Calculate scaling efficiency
    if len(benchmark_results) >= 2:
        sizes = []
        rates = []
        
        for name, results in sorted(benchmark_results.items(), 
                                  key=lambda x: x[1]['neurons']):
            sizes.append(results['neurons'])
            rates.append(results['avg_rate'])
        
        # Calculate scaling efficiency
        if len(sizes) >= 2:
            size_ratio = sizes[-1] / sizes[0] if sizes[0] > 0 else 1
            rate_ratio = rates[-1] / rates[0] if rates[0] > 0 else 1
            efficiency = rate_ratio / size_ratio if size_ratio > 0 else 1
            
            print(f"  Scaling efficiency: {efficiency:.2f}")
            print(f"  Size ratio: {sizes[-1] / sizes[0]:.1f}x")
            print(f"  Rate ratio: {rates[-1] / rates[0]:.2f}x")
            
            if efficiency > 1.0:
                print("  → Superlinear scaling (unexpected)")
            elif efficiency < 1.0:
                print("  → Sublinear scaling (expected with overhead)")
            else:
                print("  → Linear scaling (ideal)")
    
    # 6. Recommendations
    print("Step 6: Performance recommendations...")
    
    best_brain = max(benchmark_results.items(), key=lambda x: x[1]['avg_rate'])
    most_efficient = max(benchmark_results.items(), 
                        key=lambda x: x[1]['avg_rate'] / x[1]['neurons'])
    
    print("Recommendations:")
    print(f"  Best throughput: {best_brain[0]} brain ({best_brain[1]['avg_rate']:.1f} steps/sec)")
    print(f"  Most efficient: {most_efficient[0]} brain ({most_efficient[1]['avg_rate']/most_efficient[1]['neurons']:.4f} steps/neuron/sec)")
    
    # Memory efficiency check
    print()
    print("Memory efficiency considerations:")
    for name, results in benchmark_results.items():
        neurons_per_sec = results['avg_rate'] * results['neurons']
        efficiency_score = results['avg_rate'] / results['neurons']
        print(f"  {name:6s}: {efficiency_score:.4f} steps/neuron/sec")
    
    print()
    print("=== Performance Benchmarking Complete ===")
    print()
    print("For optimization, consider:")
    print("  1. Larger brain sizes may show better efficiency")
    print("  2. Hardware constraints may affect scaling")
    print("  3. Configuration affects both size and performance")
    print("  4. Balance throughput with resource usage")

if __name__ == "__main__":
    main()
