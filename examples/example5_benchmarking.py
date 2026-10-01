#!/usr/bin/env python3
"""
Example 5: Benchmarking and Performance Analysis
================================================

This example demonstrates how to benchmark NLM performance and analyze
the results to optimize brain configuration.
"""

import pynlm
import time
import json
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

def benchmark_configuration(config_name, steps=100, iterations=5, 
                          variations=None):
    """Benchmark a specific configuration."""
    print(f"=== Benchmarking Configuration: {config_name} ===")
    
    if variations is None:
        variations = [0.01, 0.05, 0.1]
    
    results = {
        "config_name": config_name,
        "steps": steps,
        "iterations": iterations,
        "variations": variations,
        "results": {}
    }
    
    # Run benchmarks for each variation
    for variation in variations:
        print(f"\nBenchmarking variation: plasticity_learning_rate = {variation}")
        
        times = []
        firing_rates = []
        
        for iteration in range(iterations):
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("plasticity.stdp.learning_rate", variation)
            config.set("brain.neuron_count", 500)  # Fixed size for fair comparison
            
            # Create and initialize brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time the simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            iteration_time = end_time - start_time
            times.append(iteration_time)
            firing_rates.append(brain.getAverageFiringRate())
            
            print(f"  Iteration {iteration + 1}: {iteration_time:.3f}s, "
                  f"Firing rate: {firing_rates[-1]:.3f}")
        
        # Calculate statistics
        avg_time = np.mean(times)
        std_time = np.std(times)
        avg_firing_rate = np.mean(firing_rates)
        std_firing_rate = np.std(firing_rates)
        
        results["results"][str(variation)] = {
            "avg_time": avg_time,
            "std_time": std_time,
            "avg_firing_rate": avg_firing_rate,
            "std_firing_rate": std_firing_rate,
            "steps_per_second": steps / avg_time if avg_time > 0 else 0
        }
        
        print(f"  Results for variation {variation}:")
        print(f"    Average time: {avg_time:.3f}s ± {std_time:.3f}s")
        print(f"    Average firing rate: {avg_firing_rate:.3f} ± {std_firing_rate:.3f}")
        print(f"    Steps per second: {results['results'][str(variation)]['steps_per_second']:.1f}")
    
    return results

def analyze_benchmark_results(benchmark_results):
    """Analyze and visualize benchmark results."""
    print(f"\n=== Benchmark Analysis ===")
    
    # Find best performing configuration
    best_config = None
    best_performance = 0
    
    for config_name, data in benchmark_results.items():
        for variation, metrics in data["results"].items():
            if metrics["steps_per_second"] > best_performance:
                best_performance = metrics["steps_per_second"]
                best_config = (config_name, variation, metrics)
    
    print(f"\nBest performing configuration:")
    if best_config:
        config_name, variation, metrics = best_config
        print(f"  Config: {config_name}")
        print(f"  Plasticity rate: {variation}")
        print(f"  Performance: {metrics['steps_per_second']:.1f} steps/second")
        print(f"  Average firing rate: {metrics['avg_firing_rate']:.3f}")
    
    # Generate summary statistics
    summary = {
        "total_configs_tested": len(benchmark_results),
        "best_config": best_config[0] if best_config else None,
        "best_plasticity_rate": best_config[1] if best_config else None,
        "best_performance": best_performance,
        "configs": {}
    }
    
    for config_name, data in benchmark_results.items():
        config_summary = {
            "steps_per_second": {},
            "firing_rates": {}
        }
        
        for variation, metrics in data["results"].items():
            config_summary["steps_per_second"][variation] = metrics["steps_per_second"]
            config_summary["firing_rates"][variation] = metrics["avg_firing_rate"]
        
        summary["configs"][config_name] = config_summary
    
    return summary

def main():
    """Main benchmarking function."""
    print("=== NLM Benchmarking and Performance Analysis ===")
    
    # Create output directory
    output_dir = Path("benchmark_results")
    output_dir.mkdir(exist_ok=True)
    
    # Test different base configurations
    base_configs = [
        "default",
        "high_plasticity",
        "low_plasticity"
    ]
    
    all_benchmarks = {}
    
    for config_name in base_configs:
        benchmark_results = benchmark_configuration(
            config_name=config_name,
            steps=50,  # Smaller for faster testing
            iterations=3,
            variations=[0.005, 0.01, 0.05]
        )
        all_benchmarks[config_name] = benchmark_results
    
    # Analyze results
    analysis = analyze_benchmark_results(all_benchmarks)
    
    # Save results
    with open(output_dir / "benchmark_results.json", 'w') as f:
        json.dump(all_benchmarks, f, indent=2)
    
    with open(output_dir / "benchmark_analysis.json", 'w') as f:
        json.dump(analysis, f, indent=2)
    
    print(f"\nBenchmark results saved to: {output_dir}/")
    print(f"  - benchmark_results.json: Detailed timing data")
    print(f"  - benchmark_analysis.json: Summary analysis")
    
    # Print summary
    print(f"\n=== Benchmark Summary ===")
    print(f"Total configurations tested: {analysis['total_configs_tested']}")
    print(f"Best configuration: {analysis['best_config']}")
    print(f"Best plasticity rate: {analysis['best_plasticity_rate']}")
    print(f"Best performance: {analysis['best_performance']:.1f} steps/second")
    
    # Create a simple text report
    report_file = output_dir / "benchmark_report.txt"
    with open(report_file, 'w') as f:
        f.write("NLM Benchmarking Report\n")
        f.write("=" * 50 + "\n\n")
        f.write(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        f.write(f"Total configurations tested: {analysis['total_configs_tested']}\n")
        f.write(f"Best performing configuration: {analysis['best_config']}\n")
        f.write(f"Best plasticity rate: {analysis['best_plasticity_rate']}\n")
        f.write(f"Best performance: {analysis['best_performance']:.1f} steps/second\n\n")
        
        f.write("\nConfiguration Performance:\n")
        for config_name, data in all_benchmarks.items():
            f.write(f"\n{config_name}:")
            for variation, metrics in data["results"].items():
                f.write(f"\n  Plasticity {variation}: {metrics['steps_per_second']:.1f} steps/sec, "
                       f"{metrics['avg_firing_rate']:.3f} firing rate")
    
    print(f"Benchmark report saved to: {report_file}")

if __name__ == "__main__":
    main()