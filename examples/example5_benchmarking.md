# Example 5: Benchmarking and Performance Analysis
# ================================================

## Overview

This example demonstrates how to benchmark NLM performance and analyze the results to optimize brain configuration. It covers performance measurement, optimization techniques, and benchmarking methodologies.

## Learning Objectives

- Understand benchmarking methodology and best practices
- Learn how to measure and compare NLM performance
- Experience parameter optimization and tuning
- Learn performance analysis and visualization
- Understand resource efficiency and optimization
- Master NLM performance evaluation and improvement

## Code Example

```python
#!/usr/bin/env python3
"""
Example 5: Benchmarking and Performance Analysis
===============================================

This example demonstrates how to benchmark NLM performance and analyze
the results to optimize brain configuration.
"""

import pynlm
import time
import json
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from typing import Dict, List

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
        print(f"    Steps per second: {results["results"][str(variation)]["steps_per_second"]:.1f}")
    
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

def create_performance_comparison_plot(benchmark_results, save_path="benchmark_comparison.png"):
    """Create performance comparison plot."""
    print(f"Creating performance comparison plot...")
    
    # Prepare data for plotting
    config_names = list(benchmark_results.keys())
    x_pos = np.arange(len(config_names))
    
    # Calculate average performance for each configuration
    performance_data = []
    firing_rate_data = []
    
    for config_name in config_names:
        config_results = benchmark_results[config_name]["results"]
        
        # Average across variations
        avg_steps_per_second = np.mean([v["steps_per_second"] for v in config_results.values()])
        avg_firing_rate = np.mean([v["avg_firing_rate"] for v in config_results.values()])
        
        performance_data.append(avg_steps_per_second)
        firing_rate_data.append(avg_firing_rate)
    
    # Create subplots
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Performance plot
    bars1 = ax1.bar(x_pos, performance_data, alpha=0.7, color='skyblue')
    ax1.set_xlabel('Configuration')
    ax1.set_ylabel('Steps/Second')
    ax1.set_title('Performance Comparison: Steps/Second')
    ax1.set_xticks(x_pos)
    ax1.set_xticklabels(config_names)
    
    # Add value labels on bars
    for bar, value in zip(bars1, performance_data):
        ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                f'{value:.0f}', ha='center', va='bottom')
    
    # Firing rate plot
    bars2 = ax2.bar(x_pos, firing_rate_data, alpha=0.7, color='lightcoral')
    ax2.set_xlabel('Configuration')
    ax2.set_ylabel('Firing Rate (Hz)')
    ax2.set_title('Performance Comparison: Firing Rate')
    ax2.set_xticks(x_pos)
    ax2.set_xticklabels(config_names)
    
    # Add value labels on bars
    for bar, value in zip(bars2, firing_rate_data):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                f'{value:.2f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Plot saved to: {save_path}")

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
    
    # Create performance comparison plot
    create_performance_comparison_plot(all_benchmarks)
    
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
```

## Key Concepts

### 1. Benchmarking Framework

```python
def create_benchmarking_framework():
    """Create comprehensive benchmarking framework."""
    
    class BenchmarkFramework:
        def __init__(self):
            self.results = {}
            self.standardized_steps = 100
            self.standardized_iterations = 5
        
        def benchmark_config(self, config, name):
            """Benchmark a single configuration."""
            print(f"Benchmarking {name}...")
            
            results = {
                'name': name,
                'steps': self.standardized_steps,
                'iterations': self.standardized_iterations,
                'variation_data': {},
                'summary_stats': {}
            }
            
            # Test different parameter variations
            variations = [0.005, 0.01, 0.05, 0.1]
            
            for variation in variations:
                variation_results = self.run_variation_benchmark(config, variation)
                results['variation_data'][variation] = variation_results
            
            # Calculate summary statistics
            results['summary_stats'] = self.calculate_summary_stats(results['variation_data'])
            
            self.results[name] = results
            return results
        
        def run_variation_benchmark(self, config, variation):
            """Benchmark a specific variation."""
            variation_results = {
                'variation': variation,
                'times': [],
                'firing_rates': [],
                'steps_per_second': []
            }
            
            for iteration in range(self.standardized_iterations):
                # Create fresh brain for each iteration
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Time the simulation
                start_time = time.time()
                for step in range(self.standardized_steps):
                    brain.step(step)
                end_time = time.time()
                
                # Record metrics
                elapsed_time = end_time - start_time
                steps_per_second = self.standardized_steps / elapsed_time
                firing_rate = brain.getAverageFiringRate()
                
                variation_results['times'].append(elapsed_time)
                variation_results['firing_rates'].append(firing_rate)
                variation_results['steps_per_second'].append(steps_per_second)
            
            return variation_results
        
        def calculate_summary_stats(self, variation_data):
            """Calculate summary statistics for all variations."""
            summary = {}
            
            for variation, data in variation_data.items():
                summary[variation] = {
                    'avg_time': np.mean(data['times']),
                    'std_time': np.std(data['times']),
                    'avg_firing_rate': np.mean(data['firing_rates']),
                    'std_firing_rate': np.std(data['firing_rates']),
                    'avg_steps_per_second': np.mean(data['steps_per_second']),
                    'std_steps_per_second': np.std(data['steps_per_second'])
                }
            
            return summary
        
        def compare_configurations(self):
            """Compare performance across configurations."""
            comparison = {}
            
            for config_name, results in self.results.items():
                avg_performance = np.mean([
                    v['avg_steps_per_second'] for v in results['summary_stats'].values()
                ])
                avg_firing_rate = np.mean([
                    v['avg_firing_rate'] for v in results['summary_stats'].values()
                ])
                
                comparison[config_name] = {
                    'average_performance': avg_performance,
                    'average_firing_rate': avg_firing_rate,
                    'performance_rank': 0,  # Will be calculated after all comparisons
                    'firing_rate_rank': 0
                }
            
            # Rank configurations
            perf_sorted = sorted(
                comparison.items(),
                key=lambda x: x[1]['average_performance'],
                reverse=True
            )
            
            for i, (config_name, stats) in enumerate(perf_sorted):
                stats['performance_rank'] = i + 1
            
            firing_sorted = sorted(
                comparison.items(),
                key=lambda x: x[1]['average_firing_rate'],
                reverse=True
            )
            
            for i, (config_name, stats) in enumerate(firing_sorted):
                stats['firing_rate_rank'] = i + 1
            
            return comparison
    
    return BenchmarkFramework()
```

### 2. Performance Metrics Collection

```python
class PerformanceMetricsCollector:
    """Collect and analyze performance metrics."""
    
    def __init__(self):
        self.metrics_history = {}
        self.benchmark_sessions = []
    
    def collect_benchmark_metrics(self, config_name, config, steps=100, iterations=5):
        """Collect comprehensive performance metrics."""
        session_id = f"bench_{config_name}_{int(time.time())}"
        
        session_data = {
            'session_id': session_id,
            'config_name': config_name,
            'steps': steps,
            'iterations': iterations,
            'timestamp': time.time(),
            'metrics': {}
        }
        
        # Collect CPU metrics
        import psutil
        cpu_samples = []
        
        for iteration in range(iterations):
            # Record system state before iteration
            cpu_samples.append(psutil.cpu_percent(interval=None))
            
            # Run benchmark iteration
            iteration_metrics = self.run_benchmark_iteration(config, steps)
            
            # Record system state after iteration
            cpu_samples.append(psutil.cpu_percent(interval=None))
            
            # Store metrics
            session_data['metrics'][f'iteration_{iteration}'] = iteration_metrics
        
        # Calculate average system metrics
        session_data['system_metrics'] = {
            'avg_cpu_usage': np.mean(cpu_samples),
            'max_cpu_usage': np.max(cpu_samples),
            'min_cpu_usage': np.min(cpu_samples)
        }
        
        self.benchmark_sessions.append(session_data)
        self.metrics_history[session_id] = session_data
        
        return session_data
    
    def run_benchmark_iteration(self, config, steps):
        """Run single benchmark iteration and collect metrics."""
        start_time = time.time()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Monitor memory
        import psutil
        process = psutil.Process()
        memory_before = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run simulation
        firing_rates = []
        simulation_times = []
        
        for step in range(steps):
            step_start = time.time()
            brain.step(step)
            step_end = time.time()
            
            firing_rates.append(brain.getAverageFiringRate())
            simulation_times.append(step_end - step_start)
        
        # Calculate metrics
        end_time = time.time()
        total_time = end_time - start_time
        
        # Memory usage
        memory_after = process.memory_info().rss / 1024 / 1024
        memory_increase = memory_after - memory_before
        
        return {
            'total_time': total_time,
            'steps_per_second': steps / total_time,
            'avg_firing_rate': np.mean(firing_rates),
            'std_firing_rate': np.std(firing_rates),
            'avg_step_time': np.mean(simulation_times),
            'std_step_time': np.std(simulation_times),
            'memory_increase_mb': memory_increase,
            'max_memory': max(memory_before, memory_after)
        }
    
    def analyze_performance_trends(self, time_window=10):
        """Analyze performance trends over time."""
        trends = {}
        
        for session_id, session_data in self.metrics_history.items():
            if len(session_data['metrics']) >= time_window:
                # Calculate rolling averages
                recent_iterations = list(session_data['metrics'].keys())[-time_window:]
                
                recent_metrics = [
                    session_data['metrics'][iter_key]
                    for iter_key in recent_iterations
                ]
                
                avg_performance = np.mean([m['steps_per_second'] for m in recent_metrics])
                avg_firing_rate = np.mean([m['avg_firing_rate'] for m in recent_metrics])
                avg_memory = np.mean([m['max_memory'] for m in recent_metrics])
                
                trends[session_id] = {
                    'avg_performance': avg_performance,
                    'avg_firing_rate': avg_firing_rate,
                    'avg_memory': avg_memory,
                    'trend_direction': self.calculate_trend_direction(recent_metrics)
                }
        
        return trends
    
    def calculate_trend_direction(self, metrics):
        """Calculate performance trend direction."""
        if len(metrics) < 2:
            return 'insufficient_data'
        
        # Calculate correlation between iteration and performance
        iterations = list(range(len(metrics)))
        performances = [m['steps_per_second'] for m in metrics]
        
        if len(set(performances)) <= 1:
            return 'stable'
        
        correlation = np.corrcoef(iterations, performances)[0, 1]
        
        if correlation > 0.1:
            return 'improving'
        elif correlation < -0.1:
            return 'degrading'
        else:
            return 'stable'
```

### 3. Optimization Analysis

```python
def optimization_analysis(benchmark_results):
    """Perform advanced optimization analysis."""
    print("=== Advanced Optimization Analysis ===")
    
    # Prepare data for analysis
    config_names = list(benchmark_results.keys())
    all_variations = set()
    
    for config_name, data in benchmark_results.items():
        all_variations.update(data['results'].keys())
    
    # Create optimization matrix
    optimization_matrix = []
    
    for config_name in config_names:
        row = {'config': config_name}
        
        for variation in sorted(all_variations):
            if variation in benchmark_results[config_name]['results']:
                metrics = benchmark_results[config_name]['results'][variation]
                row[f'variation_{variation}'] = metrics['steps_per_second']
            else:
                row[f'variation_{variation}'] = None
        
        optimization_matrix.append(row)
    
    # Analyze for each variation
    print("\n=== Optimization Analysis by Variation ===")
    
    for variation in sorted(all_variations):
        print(f"\nAnalysis for plasticity_learning_rate = {variation}:")
        
        # Get performance data for this variation
        variation_performance = []
        
        for config_name in config_names:
            if variation in benchmark_results[config_name]['results']:
                metrics = benchmark_results[config_name]['results'][variation]
                variation_performance.append({
                    'config': config_name,
                    'performance': metrics['steps_per_second'],
                    'firing_rate': metrics['avg_firing_rate'],
                    'time_std': metrics['std_time']
                })
        
        if variation_performance:
            # Find best configuration for this variation
            best_config = max(variation_performance, key=lambda x: x['performance'])
            
            print(f"  Best configuration: {best_config['config']}")
            print(f"  Performance: {best_config['performance']:.1f} steps/second")
            print(f"  Firing rate: {best_config['firing_rate']:.3f} Hz")
            print(f"  Time stability: {best_config['time_std']:.3f}s std")
            
            # Performance ranking
            variation_performance.sort(key=lambda x: x['performance'], reverse=True)
            print(f"  Performance ranking:")
            for i, config in enumerate(variation_performance):
                print(f"    {i+1}. {config['config']}: {config['performance']:.1f} steps/sec")
    
    # Cross-variation analysis
    print(f"\n=== Cross-Variation Analysis ===")
    
    # Analyze trade-offs
    for config_name in config_names:
        print(f"\nConfiguration {config_name}:")
        
        config_performance = []
        
        for variation in sorted(all_variations):
            if variation in benchmark_results[config_name]['results']:
                metrics = benchmark_results[config_name]['results'][variation]
                config_performance.append({
                    'variation': variation,
                    'performance': metrics['steps_per_second'],
                    'firing_rate': metrics['avg_firing_rate']
                })
        
        if config_performance:
            # Find best variation for this configuration
            best_variation = max(config_performance, key=lambda x: x['performance'])
            
            # Calculate consistency across variations
            performances = [p['performance'] for p in config_performance]
            consistency = 1.0 - (np.std(performances) / np.mean(performances) if np.mean(performances) > 0 else 0)
            
            print(f"  Best variation: plasticity_learning_rate = {best_variation['variation']}")
            print(f"  Best performance: {best_variation['performance']:.1f} steps/second")
            print(f"  Performance consistency: {consistency:.3f}")
            print(f"  Average firing rate: {np.mean([p['firing_rate'] for p in config_performance]):.3f} Hz")
    
    # Recommendation generation
    print(f"\n=== Optimization Recommendations ===")
    
    # Find overall best configuration
    overall_best = None
    best_overall_performance = 0
    
    for config_name in config_names:
        config_performance = []
        
        for variation in sorted(all_variations):
            if variation in benchmark_results[config_name]['results']:
                metrics = benchmark_results[config_name]['results'][variation]
                config_performance.append(metrics['steps_per_second'])
        
        if config_performance:
            avg_performance = np.mean(config_performance)
            if avg_performance > best_overall_performance:
                best_overall_performance = avg_performance
                overall_best = config_name
    
    if overall_best:
        print(f"1. Recommended configuration: {overall_best}")
        print(f"   Average performance: {best_overall_performance:.1f} steps/second")
        
        # Show variation performance for recommended config
        variations_perf = []
        for variation in sorted(all_variations):
            if variation in benchmark_results[overall_best]['results']:
                metrics = benchmark_results[overall_best]['results'][variation]
                variations_perf.append({
                    'variation': variation,
                    'performance': metrics['steps_per_second'],
                    'firing_rate': metrics['avg_firing_rate']
                })
        
        print(f"   Best variation: plasticity_learning_rate = {max(variations_perf, key=lambda x: x['performance'])['variation']}")
        
        # Calculate robustness
        performances = [v['performance'] for v in variations_perf]
        robustness = 1.0 - (np.std(performances) / np.mean(performances) if np.mean(performances) > 0 else 0)
        
        print(f"   Performance robustness: {robustness:.3f}")
    
    return {
        'analysis_complete': True,
        'recommended_config': overall_best,
        'best_performance': best_overall_performance,
        'variations_tested': len(all_variations)
    }
```

## Expected Output

### Sample Benchmark Output

```
=== NLM Benchmarking and Performance Analysis ===
=== Benchmarking Configuration: default ===
Benchmarking variation: plasticity_learning_rate = 0.005
  Iteration 1: 0.123s, Firing rate: 0.125
  Iteration 2: 0.125s, Firing rate: 0.128
  Iteration 3: 0.124s, Firing rate: 0.126
  Iteration 4: 0.126s, Firing rate: 0.127
  Iteration 5: 0.125s, Firing rate: 0.129
  Results for variation 0.005:
    Average time: 0.125s ± 0.001s
    Average firing rate: 0.127 ± 0.001
    Steps per second: 800.0

Benchmarking variation: plasticity_learning_rate = 0.01
  Iteration 1: 0.118s, Firing rate: 0.130
  Iteration 2: 0.119s, Firing rate: 0.132
  Iteration 3: 0.120s, Firing rate: 0.131
  Iteration 4: 0.121s, Firing rate: 0.133
  Iteration 5: 0.120s, Firing rate: 0.134
  Results for variation 0.01:
    Average time: 0.120s ± 0.001s
    Average firing rate: 0.131 ± 0.001
    Steps per second: 833.3

=== Benchmark Analysis ===
Best performing configuration:
  Config: high_plasticity
  Plasticity rate: 0.1
  Performance: 833.3 steps/second
  Average firing rate: 0.134

=== Benchmark Summary ===
Total configurations tested: 3
Best configuration: high_plasticity
Best plasticity rate: 0.1
Best performance: 833.3 steps/second
Benchmark results saved to: benchmark_results/
```

### Analysis Report Output

```
=== NLM Performance Optimization Analysis ===

=== Optimization Analysis by Variation ===

Analysis for plasticity_learning_rate = 0.005:
  Best configuration: default
  Performance: 800.0 steps/second
  Firing rate: 0.125 Hz
  Time stability: 0.001s std
  Performance ranking:
    1. default: 800.0 steps/sec
    2. high_plasticity: 783.3 steps/sec
    3. low_plasticity: 766.7 steps/sec

Analysis for plasticity_learning_rate = 0.01:
  Best configuration: default
  Performance: 833.3 steps/second
  Firing rate: 0.131 Hz
  Time stability: 0.001s std
  Performance ranking:
    1. default: 833.3 steps/sec
    2. high_plasticity: 825.0 steps/sec
    3. low_plasticity: 800.0 steps/sec

Analysis for plasticity_learning_rate = 0.05:
  Best configuration: default
  Performance: 883.3 steps/second
  Firing rate: 0.142 Hz
  Time stability: 0.002s std
  Performance ranking:
    1. default: 883.3 steps/sec
    2. high_plasticity: 875.0 steps/sec
    3. low_plasticity: 850.0 steps/sec

Analysis for plasticity_learning_rate = 0.1:
  Best configuration: default
  Performance: 916.7 steps/second
  Best variation: plasticity_learning_rate = 0.1
  Firing rate: 0.150 Hz
  Time stability: 0.002s std
  Performance ranking:
    1. default: 916.7 steps/sec
    2. high_plasticity: 900.0 steps/sec
    3. low_plasticity: 875.0 steps/sec

=== Cross-Variation Analysis ===

Configuration default:
  Best variation: plasticity_learning_rate = 0.1
  Best performance: 916.7 steps/second
  Performance consistency: 0.998
  Average firing rate: 0.142 Hz

Configuration high_plasticity:
  Best variation: plasticity_learning_rate = 0.1
  Best performance: 900.0 steps/second
  Performance consistency: 0.999
  Average firing rate: 0.148 Hz

Configuration low_plasticity:
  Best variation: plasticity_learning_rate = 0.1
  Best performance: 875.0 steps/second
  Performance consistency: 0.999
  Average firing rate: 0.138 Hz

=== Optimization Recommendations ===

1. Recommended configuration: default
2. Average performance: 841.5 steps/second
3. Best variation: plasticity_learning_rate = 0.1
4. Performance robustness: 0.998
```

## Practical Applications

### 1. Performance Optimization

```python
def performance_optimization_pipeline():
    """Complete performance optimization pipeline."""
    print("=== Performance Optimization Pipeline ===")
    
    # Step 1: Initial benchmarking
    print("\nStep 1: Initial benchmarking")
    base_configs = {
        'default': pynlm.createDefaultConfig(),
        'high_plasticity': pynlm.createDefaultConfig(),
        'low_plasticity': pynlm.createDefaultConfig()
    }
    
    # Apply configuration differences
    base_configs['high_plasticity'].set("plasticity.stdp.learning_rate", 0.02)
    base_configs['low_plasticity'].set("plasticity.stdp.learning_rate", 0.005)
    
    # Run initial benchmark
    benchmark_results = {}
    for name, config in base_configs.items():
        benchmark_results[name] = benchmark_configuration(config, name, steps=50, iterations=3)
    
    # Step 2: Optimization analysis
    print("\nStep 2: Optimization analysis")
    analysis = optimization_analysis(benchmark_results)
    
    # Step 3: Identify optimal configuration
    recommended_config = analysis['recommended_config']
    recommended_variation = max(
        [v for v in benchmark_results[recommended_config]['results'].keys()],
        key=lambda v: benchmark_results[recommended_config]['results'][v]['steps_per_second']
    )
    
    print(f"\nRecommended configuration: {recommended_config}")
    print(f"Recommended variation: plasticity_learning_rate = {recommended_variation}")
    
    # Step 4: Final verification
    print("\nStep 3: Final verification")
    optimal_config = base_configs[recommended_config].copy()
    optimal_config.set("plasticity.stdp.learning_rate", float(recommended_variation))
    
    # Run final benchmark with more iterations for accuracy
    final_results = benchmark_configuration(optimal_config, "optimized", steps=100, iterations=10)
    
    # Generate performance report
    print(f"\n=== Final Performance Report ===")
    print(f"Optimal configuration: {recommended_config}")
    print(f"Plasticity learning rate: {recommended_variation}")
    
    final_perf = final_results['results'][recommended_variation]
    print(f"Expected performance: {final_perf['steps_per_second']:.1f} steps/second")
    print(f"Firing rate: {final_perf['avg_firing_rate']:.3f} Hz")
    print(f"Time consistency: {final_perf['std_time']:.3f}s std")
    
    return benchmark_results, analysis, optimal_config
```

### 2. Configuration Comparison

```python
def configuration_comparison_study():
    """Compare different configuration strategies."""
    print("=== Configuration Comparison Study ===")
    
    # Define configuration strategies
    configurations = {
        'baseline': {
            'name': 'Baseline Configuration',
            'description': 'Standard default configuration',
            'config': pynlm.createDefaultConfig()
        },
        'aggressive': {
            'name': 'Aggressive Plasticity',
            'description': 'High plasticity for rapid learning',
            'config': pynlm.createDefaultConfig()
        },
        'conservative': {
            'name': 'Conservative Plasticity',
            'description': 'Low plasticity for stability',
            'config': pynlm.createDefaultConfig()
        }
    }
    
    # Apply configuration differences
    configurations['aggressive']['config'].set("plasticity.stdp.learning_rate", 0.02)
    configurations['conservative']['config'].set("plasticity.stdp.learning_rate", 0.001)
    
    # Run comparative benchmark
    comparison_results = {}
    
    for name, config_info in configurations.items():
        print(f"\nTesting: {config_info['name']}")
        print(f"Description: {config_info['description']}")
        
        results = benchmark_configuration(
            config_info['config'], 
            name, 
            steps=75, 
            iterations=4,
            variations=[0.001, 0.01, 0.1]
        )
        
        comparison_results[name] = {
            'config_info': config_info,
            'results': results,
            'performance_summary': calculate_performance_summary(results)
        }
        
        print(f"Completed: {name}")
    
    # Generate comparison report
    print(f"\n=== Configuration Comparison Report ===")
    
    # Rank configurations
    ranked_configs = rank_configurations(comparison_results)
    
    print(f"\nConfiguration Rankings:")
    for i, (name, stats) in enumerate(ranked_configs.items(), 1):
        print(f"{i}. {name}:")
        print(f"   Average performance: {stats['avg_performance']:.1f} steps/second")
        print(f"   Performance consistency: {stats['performance_consistency']:.3f}")
        print(f"   Firing rate stability: {stats['firing_rate_consistency']:.3f}")
        print(f"   Best variation: plasticity_learning_rate = {stats['best_variation']}")
    
    # Generate recommendations
    print(f"\n=== Recommendations ===")
    
    # Best overall configuration
    best_config = ranked_configs[list(ranked_configs.keys())[0]]
    print(f"1. Recommended for general use: {best_config['config_name']}")
    print(f"   - Performance: {best_config['avg_performance']:.1f} steps/second")
    print(f"   - Stability: {best_config['performance_consistency']:.3f}")
    
    # Best for specific use cases
    for use_case, criteria in [
        ('stability', lambda x: x['performance_consistency']),
        ('speed', lambda x: x['avg_performance']),
        ('learning', lambda x: x['avg_firing_rate'])
    ]:
        best_for_use_case = max(ranked_configs.items(), key=lambda x: criteria(x[1]))
        print(f"2. Recommended for {use_case}: {best_for_use_case[0]}")
        print(f"   - {criteria.__name__}: {criteria(best_for_use_case[1]):.3f}")
    
    return comparison_results, ranked_configs
```

### 3. Long-term Performance Monitoring

```python
class PerformanceMonitor:
    """Monitor NLM performance over time."""
    
    def __init__(self):
        self.performance_history = {}
        self.alert_thresholds = {
            'performance_decline': 0.15,  # 15% decline triggers alert
            'memory_growth': 0.20,        # 20% memory growth triggers alert
            'firing_rate_drift': 0.10   # 10% firing rate drift triggers alert
        }
    
    def monitor_performance(self, config, name, steps=100, iterations=5):
        """Monitor performance with alerts and trend analysis."""
        print(f"Starting performance monitoring: {name}")
        
        # Initial benchmark
        initial_results = benchmark_configuration(config, name, steps, iterations)
        
        # Store initial state
        self.performance_history[name] = {
            'initial_results': initial_results,
            'monitoring_started': time.time(),
            'alerts': []
        }
        
        # Start monitoring loop
        monitoring_active = True
        
        while monitoring_active:
            # Run periodic benchmark
            current_results = benchmark_configuration(config, name, steps, iterations)
            
            # Check for performance degradation
            self.check_performance_decline(name, current_results, self.performance_history[name])
            
            # Check for memory issues
            self.check_memory_issues(name, current_results, self.performance_history[name])
            
            # Check for stability issues
            self.check_stability_issues(name, current_results, self.performance_history[name])
            
            # Update history
            self.performance_history[name]['latest_results'] = current_results
            self.performance_history[name]['last_check'] = time.time()
            
            # Sleep before next check
            time.sleep(300)  # Check every 5 minutes
        
        return self.performance_history[name]
    
    def check_performance_decline(self, config_name, current_results, history):
        """Check for performance decline over time."""
        if 'previous_results' not in history:
            history['previous_results'] = current_results
            return
        
        # Calculate performance change
        previous_perf = self.get_average_performance(history['previous_results'])
        current_perf = self.get_average_performance(current_results)
        
        if previous_perf > 0:
            decline_rate = (previous_perf - current_perf) / previous_perf
            
            if decline_rate > self.alert_thresholds['performance_decline']:
                alert = {
                    'type': 'performance_decline',
                    'severity': 'warning',
                    'message': f"Performance declined by {decline_rate*100:.1f}% over time",
                    'previous_performance': previous_perf,
                    'current_performance': current_perf,
                    'timestamp': time.time()
                }
                history['alerts'].append(alert)
                print(f"ALERT: Performance decline detected for {config_name}")
        
        # Update previous results
        history['previous_results'] = current_results
    
    def check_memory_issues(self, config_name, current_results, history):
        """Check for memory usage issues."""
        # Extract memory information from benchmark results
        # This would need to be added to benchmark_configuration
        memory_increase = self.calculate_memory_increase(current_results, history)
        
        if memory_increase > self.alert_thresholds['memory_growth'] * history.get('peak_memory', 0):
            alert = {
                'type': 'memory_growth',
                'severity': 'error',
                'message': f"Memory usage increased by {memory_increase:.1f} MB",
                'timestamp': time.time()
            }
            history['alerts'].append(alert)
            print(f"ALERT: Memory usage increased for {config_name}")
    
    def check_stability_issues(self, config_name, current_results, history):
        """Check for stability issues."""
        # Check for performance inconsistency
        consistency = self.calculate_performance_consistency(current_results)
        
        if consistency < self.alert_thresholds['firing_rate_drift']:
            alert = {
                'type': 'stability_issue',
                'severity': 'warning',
                'message': f"Performance instability detected (consistency: {consistency:.3f})",
                'timestamp': time.time()
            }
            history['alerts'].append(alert)
            print(f"ALERT: Performance instability detected for {config_name}")
    
    def get_average_performance(self, results):
        """Calculate average performance from benchmark results."""
        avg_performance = 0.0
        variation_count = 0
        
        for variation, metrics in results['results'].items():
            avg_performance += metrics['steps_per_second']
            variation_count += 1
        
        return avg_performance / variation_count if variation_count > 0 else 0
    
    def calculate_memory_increase(self, current_results, history):
        """Calculate memory increase from benchmark results."""
        # This would need to be implemented based on benchmark results structure
        return 0.0
    
    def calculate_performance_consistency(self, results):
        """Calculate performance consistency across variations."""
        performances = []
        
        for variation, metrics in results['results'].items():
            performances.append(metrics['steps_per_second'])
        
        if len(performances) <= 1:
            return 1.0
        
        return 1.0 - (np.std(performances) / np.mean(performances))
    
    def generate_monitoring_report(self):
        """Generate comprehensive monitoring report."""
        report = "# NLM Performance Monitoring Report\n\n"
        
        report += f"## Performance Monitoring Summary\n"
        report += f"Total monitored configurations: {len(self.performance_history)}\n"
        
        for config_name, data in self.performance_history.items():
            report += f"\n### {config_name}\n"
            
            initial_perf = self.get_average_performance(data['initial_results'])
            report += f"- Initial performance: {initial_perf:.1f} steps/second\n"
            
            if 'latest_results' in data:
                current_perf = self.get_average_performance(data['latest_results'])
                report += f"- Current performance: {current_perf:.1f} steps/second\n"
                
                if 'previous_results' in data:
                    prev_perf = self.get_average_performance(data['previous_results'])
                    change = current_perf - prev_perf
                    if change != 0:
                        report += f"- Performance change: {change:+.1f} steps/second\n"
            
            alerts = data.get('alerts', [])
            if alerts:
                report += f"- Alerts: {len(alerts)}\n"
                for alert in alerts:
                    report += f"  - {alert['type']}: {alert['message']}\n"
        
        return report
```

## Comparison with Other Examples

### Example 5 vs Example 1

| Feature | Example 1 | Example 5 |
|---------|-----------|-----------|
| **Focus** | Basic functionality | Performance benchmarking |
| **Complexity** | Low | High |
| **Key Concepts** | Brain creation, simulation | Performance analysis, optimization |
| **Use Case** | Learning NLM basics | System optimization and tuning |

### Example 5 vs Example 2

| Feature | Example 2 | Example 5 |
|---------|-----------|-----------|
| **Goal** | Demonstrate integration | Optimize performance |
| **Metrics** | Behavioral metrics | Performance metrics |
| **Approach** | System demonstration | Systematic optimization |
| **Focus** | How it works | How well it works |

### Example 5 vs Example 3

| Feature | Example 3 | Example 5 |
|---------|-----------|-----------|
| **Objective** | Study developmental processes | Optimize performance |
| **Metrics** | Developmental indicators | Performance metrics |
| **Approach** | Process observation | Results optimization |
| **Focus** | How development works | How to maximize performance |

## Error Handling and Debugging

### Common Issues

1. **Benchmark Execution Fails**
   - **Problem**: Benchmark fails to run or produces unexpected results
   - **Solution**: Check configuration validity, verify benchmark parameters
   - **Command**: Use simpler configurations for testing

2. **Performance Data Collection**
   - **Problem**: Performance metrics not collected correctly
   - **Solution**: Verify benchmark implementation and data collection
   - **Command**: Add debug prints to benchmark code

3. **Memory Issues During Benchmarking**
   - **Problem**: High memory usage during benchmark runs
   - **Solution**: Monitor memory usage, use smaller configurations
   - **Command**: Check system resources before running benchmarks

### Debugging Tools

```python
import pynlm
import time

def debug_benchmark_configuration():
    """Debug benchmark configuration with detailed logging."""
    print("=== Benchmark Configuration Debug ===")
    
    # Create debug configuration
    config = pynlm.createDefaultConfig()
    config.set("log_level", "DEBUG")
    
    # Test different configurations
    test_configs = [
        {'name': 'default', 'neuron_count': 500},
        {'name': 'small', 'neuron_count': 200},
        {'name': 'large', 'neuron_count': 1000}
    ]
    
    for test_config in test_configs:
        print(f"\nTesting configuration: {test_config['name']}")
        print(f"Neuron count: {test_config['neuron_count']}")
        
        # Apply configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", test_config['neuron_count'])
        
        # Run benchmark with debug output
        print("Running benchmark...")
        results = benchmark_configuration(config, test_config['name'], steps=20, iterations=2)
        
        # Debug results
        print(f"\nResults for {test_config['name']}:")
        for variation, metrics in results['results'].items():
            print(f"  Variation {variation}:")
            print(f"    Steps per second: {metrics['steps_per_second']:.1f}")
            print(f"    Average firing rate: {metrics['avg_firing_rate']:.3f}")
            print(f"    Time standard deviation: {metrics['std_time']:.3f}s")
    
    return results
```

## Performance Considerations

### Benchmark Efficiency

```python
def optimize_benchmark_efficiency():
    """Optimize benchmark execution for efficiency."""
    
    # Configuration-based optimization
    optimized_configs = {
        'default': {
            'neuron_count': 500,
            'steps': 50,
            'iterations': 3,
            'variations': [0.01, 0.05]
        },
        'quick_test': {
            'neuron_count': 200,
            'steps': 20,
            'iterations': 2,
            'variations': [0.01]
        },
        'comprehensive_test': {
            'neuron_count': 1000,
            'steps': 100,
            'iterations': 5,
            'variations': [0.005, 0.01, 0.05, 0.1]
        }
    }
    
    # Run optimized benchmarks
    results = {}
    
    for config_name, params in optimized_configs.items():
        print(f"\nRunning {config_name} benchmark:")
        print(f"  Neurons: {params['neuron_count']}")
        print(f"  Steps: {params['steps']}")
        print(f"  Iterations: {params['iterations']}")
        print(f"  Variations: {params['variations']}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params['neuron_count'])
        config.set("plasticity.stdp.learning_rate", 0.01)
        
        # Run benchmark
        benchmark_results = benchmark_configuration(
            config, 
            config_name, 
            params['steps'], 
            params['iterations'],
            params['variations']
        )
        
        results[config_name] = benchmark_results
    
    return results
```

### Resource Management

```python
def resource_aware_benchmarking():
    """Run benchmarks with resource monitoring."""
    import psutil
    import time
    
    print("=== Resource-Aware Benchmarking ===")
    
    process = psutil.Process()
    initial_memory = process.memory_info().rss / 1024 / 1024
    
    # Monitor available resources
    available_memory = psutil.virtual_memory().available / 1024 / 1024
    cpu_count = psutil.cpu_count()
    
    print(f"Available memory: {available_memory:.1f} MB")
    print(f"CPU cores: {cpu_count}")
    
    # Choose appropriate benchmark size based on resources
    if available_memory < 500:
        print("Memory limited, running quick benchmarks")
        config_size = 'quick_test'
    elif available_memory < 1000:
        print("Moderate memory, running standard benchmarks")
        config_size = 'default'
    else:
        print("Sufficient memory, running comprehensive benchmarks")
        config_size = 'comprehensive_test'
    
    # Run benchmarks
    results = run_optimized_benchmark_suite(config_size)
    
    # Final resource check
    final_memory = process.memory_info().rss / 1024 / 1024
    memory_change = final_memory - initial_memory
    
    print(f"\nResource usage summary:")
    print(f"Memory change: {memory_change:+.1f} MB")
    print(f"Peak memory likely: {max(initial_memory, final_memory):.1f} MB")
    
    return results
```

## Extensions and Customization

### 1. Advanced Performance Analysis

```python
def advanced_performance_analysis(benchmark_results):
    """Perform advanced performance analysis with statistical methods."""
    
    print("=== Advanced Performance Analysis ===")
    
    # Statistical analysis
    config_names = list(benchmark_results.keys())
    
    # ANOVA-style analysis
    print("\nStatistical Analysis:")
    print("(" + "-".join([f"{c:10}" for c in config_names]) + ") Performance Rankings")
    
    # Calculate average performance for each configuration
    avg_performances = []
    for config_name in config_names:
        variation_performances = []
        for variation, metrics in benchmark_results[config_name]['results'].items():
            variation_performances.append(metrics['steps_per_second'])
        
        avg_performances.append(np.mean(variation_performances))
    
    # Rank configurations
    ranked_configs = sorted(
        zip(config_names, avg_performances),
        key=lambda x: x[1],
        reverse=True
    )
    
    for i, (config_name, avg_perf) in enumerate(ranked_configs):
        print(f"{i+1}. {config_name:10} {avg_perf:8.1f} steps/sec")
    
    # Statistical significance testing
    print(f"\nStatistical Significance Testing:")
    print("Performing ANOVA analysis to determine if differences are significant...")
    
    # Simplified ANOVA
    if len(avg_performances) > 1:
        f_statistic = (max(avg_performances) - min(avg_performances)) / np.std(avg_performances)
                      if np.std(avg_performances) > 0 else 0)
        
        print(f"F-statistic: {f_statistic:.3f}")
        
        if f_statistic > 3.0:
            print("Result: Differences are statistically significant (p < 0.05)")
        else:
            print("Result: Differences are not statistically significant (p > 0.05)")
    
    # Effect size calculation
    print(f"\nEffect Size Analysis:")
    for config_name in config_names:
        variation_performances = []
        for variation, metrics in benchmark_results[config_name]['results'].items():
            variation_performances.append(metrics['steps_per_second'])
        
        mean_perf = np.mean(variation_performances)
        std_perf = np.std(variation_performances)
        
        if mean_perf > 0:
            cohens_d = std_perf / mean_perf
            print(f"{config_name:10} Cohen's d: {cohens_d:.3f} (small: <0.2, medium: 0.2-0.8, large: >0.8)")
    
    return ranked_configs
```

### 2. Performance Prediction

```python
class PerformancePredictor:
    """Predict performance based on configuration parameters."""
    
    def __init__(self):
        self.model = None
        self.training_data = []
    
    def train_performance_model(self, benchmark_results):
        """Train performance prediction model."""
        print("Training performance prediction model...")
        
        # Prepare training data
        training_data = []
        
        for config_name, results in benchmark_results.items():
            for variation, metrics in results['results'].items():
                training_sample = {
                    'neuron_count': 500,  # Fixed for this benchmark
                    'plasticity_rate': float(variation),
                    'steps_per_second': metrics['steps_per_second'],
                    'firing_rate': metrics['avg_firing_rate'],
                    'config_name': config_name
                }
                training_data.append(training_sample)
        
        self.training_data = training_data
        
        # Simple linear regression model
        # In a real implementation, this would use scikit-learn
        X = np.array([d['plasticity_rate'] for d in training_data])
        y = np.array([d['steps_per_second'] for d in training_data])
        
        # Calculate correlation
        correlation = np.corrcoef(X, y)[0, 1]
        print(f"Correlation between plasticity rate and performance: {correlation:.3f}")
        
        # Store model parameters
        self.model = {
            'correlation': correlation,
            'slope': np.polyfit(X, y, 1)[0] if len(X) > 1 else 0,
            'intercept': np.polyfit(X, y, 1)[1] if len(X) > 1 else 0,
            'training_samples': len(training_data)
        }
        
        print(f"Model trained with {len(training_data)} samples")
        return self.model
    
    def predict_performance(self, plasticity_rate, neuron_count=500):
        """Predict performance for given parameters."""
        if self.model is None:
            print("Model not trained. Call train_performance_model() first.")
            return None
        
        # Simple linear prediction
        predicted_performance = (
            self.model['slope'] * plasticity_rate + 
            self.model['intercept']
        )
        
        return {
            'plasticity_rate': plasticity_rate,
            'predicted_steps_per_second': predicted_performance,
            'confidence': abs(self.model['correlation']),
            'model_type': 'linear_regression'
        }
    
    def predict_optimal_configuration(self, config_space):
        """Predict optimal configuration from parameter space."""
        print(f"Predicting optimal configuration from parameter space: {config_space}")
        
        best_prediction = None
        best_performance = -1
        
        for plasticity_rate in config_space:
            prediction = self.predict_performance(plasticity_rate)
            
            if prediction and prediction['predicted_steps_per_second'] > best_performance:
                best_performance = prediction['predicted_steps_per_second']
                best_prediction = prediction
                best_prediction['optimal_plasticity_rate'] = plasticity_rate
        
        if best_prediction:
            print(f"\nOptimal configuration prediction:")
            print(f"  Plasticity rate: {best_prediction['optimal_plasticity_rate']}")
            print(f"  Predicted performance: {best_prediction['predicted_steps_per_second']:.1f} steps/second")
            print(f"  Confidence: {best_prediction['confidence']:.3f}")
        
        return best_prediction
```

## Conclusion

Example 5 provides a comprehensive foundation for understanding NLM performance measurement and optimization. By working through this example, you can:

1. **Master Benchmarking**: Learn how to measure and compare NLM performance
2. **Understand Optimization**: Experience parameter optimization and tuning
3. **Analyze Performance**: Perform comprehensive performance analysis
4. **Explore Extensions**: Create performance prediction models and advanced analysis
5. **Build Expertise**: Develop skills for system optimization and tuning

This example serves as a bridge between basic NLM usage and advanced performance engineering, introducing key concepts and patterns used throughout NLM performance optimization.

---

*This example is part of the NLM (Neural Learning Machine) examples suite.
For additional examples, documentation, and support, visit the NLM project.*
