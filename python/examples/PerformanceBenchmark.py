# PerformanceBenchmark.py - Performance Benchmarking and Optimization

This example demonstrates performance benchmarking and optimization techniques for the NLM Python bindings. It shows how to measure, analyze, and optimize neural simulation performance.

```python
import pynlm
import time
import psutil
import numpy as np
import matplotlib.pyplot as plt
import gc
from functools import wraps
from typing import Dict, List, Callable, Any
import os

class PerformanceMetrics:
    """Collect and analyze performance metrics for NLM simulations."""
    
    def __init__(self):
        self.timings = []
        self.memory_usage = []
        self.cpu_usage = []
        self.step_times = []
        self.neuron_counts = []
        self.simulation_configs = []
        
    def record_timing(self, operation: str, duration: float):
        """Record timing for an operation."""
        self.timings.append({
            'operation': operation,
            'duration': duration,
            'timestamp': time.time()
        })
    
    def record_memory(self):
        """Record current memory usage."""
        process = psutil.Process(os.getpid())
        memory_mb = process.memory_info().rss / 1024 / 1024
        self.memory_usage.append(memory_mb)
    
    def record_cpu(self):
        """Record current CPU usage."""
        cpu_percent = psutil.cpu_percent()
        self.cpu_usage.append(cpu_percent)
    
    def record_step_performance(self, step: int, neurons_firing: int, 
                              step_time: float, spike_count: int):
        """Record performance metrics for a simulation step."""
        self.step_times.append(step_time)
        self.neuron_counts.append(neurons_firing)
        self.simulation_configs.append({
            'step': step,
            'neurons_firing': neurons_firing,
            'step_time': step_time,
            'spike_count': spike_count,
            'firing_rate': neurons_firing / 1000.0 if neurons_firing > 0 else 0.0
        })
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary."""
        if not self.timings:
            return {}
        
        step_times = np.array(self.step_times)
        neuron_counts = np.array(self.neuron_counts)
        
        summary = {
            'total_operations': len(self.timings),
            'average_operation_time': np.mean([t['duration'] for t in self.timings]),
            'min_operation_time': np.min([t['duration'] for t in self.timings]),
            'max_operation_time': np.max([t['duration'] for t in self.timings]),
            'operation_std': np.std([t['duration'] for t in self.timings]),
            
            'simulation': {
                'total_steps': len(step_times),
                'average_step_time': np.mean(step_times),
                'min_step_time': np.min(step_times),
                'max_step_time': np.max(step_times),
                'step_time_std': np.std(step_times),
                'average_firing_rate': np.mean(neuron_counts),
                'peak_firing_rate': np.max(neuron_counts),
                'average_spikes_per_step': np.mean([c for _, c in self.simulation_configs]),
            },
            
            'resource_usage': {
                'peak_memory_mb': max(self.memory_usage) if self.memory_usage else 0,
                'average_memory_mb': np.mean(self.memory_usage) if self.memory_usage else 0,
                'memory_growth_mb': (max(self.memory_usage) - min(self.memory_usage)) 
                                   if self.memory_usage and len(self.memory_usage) > 1 else 0,
                'average_cpu_percent': np.mean(self.cpu_usage) if self.cpu_usage else 0,
                'peak_cpu_percent': max(self.cpu_usage) if self.cpu_usage else 0,
            }
        }
        
        return summary

class BenchmarkRunner:
    """Run performance benchmarks for NLM simulations."""
    
    def __init__(self, metrics: PerformanceMetrics = None):
        self.metrics = metrics or PerformanceMetrics()
        self.simulation_configs = []
        
    def benchmark_configuration(self, config: Dict[str, Any], steps: int = 100) -> Dict[str, Any]:
        """Benchmark a specific configuration."""
        print(f"Benchmarking configuration: {config}")
        
        # Create brain with configuration
        nlm_config = pynlm.createDefaultConfig()
        for key, value in config.items():
            self._setConfigValue(nlm_config, key, value)
        
        # Create brain
        start_time = time.time()
        brain = pynlm.createBrain(nlm_config)
        init_time = time.time() - start_time
        
        brain.initialize()
        init_time += time.time() - start_time
        
        # Run simulation steps
        step_start_times = []
        spike_counts = []
        
        for step in range(steps):
            step_start = time.time()
            
            # Record state before step
            neurons_before = brain.getFiringNeuronCount()
            
            # Perform step
            brain.step(step, step * 0.001)
            
            # Record state after step
            neurons_after = brain.getFiringNeuronCount()
            spike_count = brain.getTotalSpikeCount() - (step * 10 if step > 0 else 0)
            
            step_time = time.time() - step_start
            
            # Record metrics
            self.metrics.record_step_performance(step, neurons_after, step_time, spike_count)
            step_start_times.append(step_time)
            spike_counts.append(spike_count)
            
            # Record resource usage periodically
            if step % 10 == 0:
                self.metrics.record_memory()
                self.metrics.record_cpu()
        
        total_time = time.time() - start_time
        
        # Calculate configuration performance
        avg_step_time = np.mean(step_start_times)
        avg_spikes_per_step = np.mean(spike_counts)
        firing_rate_variance = np.var([c for _, c in self.simulation_configs])
        
        return {
            'configuration': config,
            'initialization_time': init_time,
            'total_simulation_time': total_time,
            'average_step_time': avg_step_time,
            'average_spikes_per_step': avg_spikes_per_step,
            'firing_rate_variance': firing_rate_variance,
            'performance_rating': self._calculatePerformanceRating(avg_step_time, avg_spikes_per_step),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount()
        }
    
    def benchmark_multiple_configurations(self, configurations: List[Dict[str, Any]],
                                        steps_per_config: int = 50) -> List[Dict[str, Any]]:
        """Benchmark multiple configurations."""
        results = []
        
        for i, config in enumerate(configurations):
            print(f"\n[{i+1}/{len(configurations)}] Benchmarking configuration {i+1}")
            
            # Force garbage collection before each benchmark
            gc.collect()
            self.metrics.record_memory()
            
            result = self.benchmark_configuration(config, steps_per_config)
            result['run'] = i + 1
            results.append(result)
            
            # Brief pause between benchmarks
            time.sleep(0.1)
        
        return results
    
    def benchmark_scaling(self, base_config: Dict[str, Any], neuron_counts: List[int], 
                         steps: int = 30) -> List[Dict[str, Any]]:
        """Benchmark scaling behavior with different neuron counts."""
        print(f"Benchmarking scaling behavior...")
        
        scaling_results = []
        
        for neuron_count in neuron_counts:
            config = base_config.copy()
            config['brain.neuron_count'] = neuron_count
            config['brain.synapse_density'] = 0.1 + (neuron_count / 10000) * 0.04  # Scale density
            
            print(f"  Neuron count: {neuron_count}")
            result = self.benchmark_configuration(config, steps)
            scaling_results.append(result)
        
        return scaling_results
    
    def benchmark_plasticity_effect(self, base_config: Dict[str, Any], steps: int = 100) -> Dict[str, Any]:
        """Benchmark the effect of different plasticity settings."""
        print(f"Benchmarking plasticity effects...")
        
        plasticity_configs = [
            {'plasticity.stdp.enable': True, 'plasticity.hebbian.enable': False, 'plasticity.structural.enable': False},
            {'plasticity.stdp.enable': True, 'plasticity.hebbian.enable': True, 'plasticity.structural.enable': False},
            {'plasticity.stdp.enable': True, 'plasticity.hebbian.enable': True, 'plasticity.structural.enable': True},
            {'plasticity.stdp.enable': False, 'plasticity.hebbian.enable': True, 'plasticity.structural.enable': False},
        ]
        
        plasticity_results = []
        
        for i, plasticity_config in enumerate(plasticity_configs):
            config = base_config.copy()
            config.update(plasticity_config)
            
            print(f"  Plasticity config {i+1}: STDP={plasticity_config['plasticity.stdp.enable']}, "
                  f"Hebbian={plasticity_config['plasticity.hebbian.enable']}, "
                  f"Structural={plasticity_config['plasticity.structural.enable']}")
            
            result = self.benchmark_configuration(config, steps)
            result['plasticity_config'] = plasticity_config
            plasticity_results.append(result)
        
        return {
            'plasticity_benchmarks': plasticity_results,
            'summary': self._analyzePlasticityEffects(plasticity_results)
        }
    
    def _setConfigValue(self, config, key: str, value):
        """Set configuration value with type conversion."""
        if '.' not in key:
            if isinstance(value, bool):
                config.set(key, value, pynlm.ConfigSource::Default)
            elif isinstance(value, int):
                config.set_int(key, value)
            elif isinstance(value, float):
                config.set_float(key, value)
            else:
                config.set_string(key, str(value))
        else:
            config.set_float(key, float(value))
    
    def _calculatePerformanceRating(self, avg_step_time: float, avg_spikes_per_step: float) -> float:
        """Calculate a performance rating (0-100)."""
        # Ideal step time (lower is better)
        ideal_step_time = 0.01
        step_time_score = max(0, 100 * (ideal_step_time / max(avg_step_time, 0.001)))
        
        # Ideal spike rate (moderate is usually best)
        ideal_spike_rate = 100
        spike_rate_score = 100 * np.exp(-((avg_spikes_per_step - ideal_spike_rate) / 50) ** 2)
        
        return (step_time_score + spike_rate_score) / 2
    
    def _analyzePlasticityEffects(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze the effects of different plasticity settings."""
        if not results:
            return {}
        
        ratings = [r['performance_rating'] for r in results]
        step_times = [r['average_step_time'] for r in results]
        spike_rates = [r['average_spikes_per_step'] for r in results]
        
        return {
            'best_plasticity_config': results[np.argmax(ratings)],
            'average_rating': np.mean(ratings),
            'rating_std': np.std(ratings),
            'step_time_trend': np.polyfit(range(len(step_times)), step_times, 1),
            'spike_rate_trend': np.polyfit(range(len(spike_rates)), spike_rates, 1),
            'recommendation': self._getPlasticityRecommendation(results)
        }
    
    def _getPlasticityRecommendation(self, results: List[Dict[str, Any]]) -> str:
        """Get recommendation based on plasticity benchmark results."""
        if not results:
            return "No data available for recommendation"
        
        best = max(results, key=lambda x: x['performance_rating'])
        best_config = best['plasticity_config']
        
        enabled_plasticity = []
        for key, value in best_config.items():
            if value:
                enabled_plasticity.append(key.split('.')[1])
        
        return f"Enable {', '.join(enabled_plasticity)} for optimal performance"

class Visualizer:
    """Create visualizations for benchmark results."""
    
    @staticmethod
    def plot_configuration_comparison(config_results: List[Dict[str, Any]], save_path: str = None):
        """Plot comparison of different configurations."""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Performance Benchmark - Configuration Comparison', fontsize=16)
        
        if len(config_results) < 2:
            print("Need at least 2 configurations for comparison")
            return
        
        # Plot 1: Performance rating comparison
        ax1 = axes[0, 0]
        configs = [f"Config {i+1}" for i in range(len(config_results))]
        ratings = [r['performance_rating'] for r in config_results]
        colors = ['green' if r == max(ratings) else 'blue' for r in ratings]
        
        bars = ax1.bar(configs, ratings, color=colors)
        ax1.set_ylabel('Performance Rating (0-100)')
        ax1.set_title('Configuration Performance Comparison')
        ax1.set_ylim(0, 100)
        
        # Add value labels on bars
        for bar, rating in zip(bars, ratings):
            ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                    f'{rating:.1f}', ha='center', va='bottom')
        
        # Plot 2: Step time comparison
        ax2 = axes[0, 1]
        step_times = [r['average_step_time'] for r in config_results]
        ax2.bar(configs, step_times, color='orange')
        ax2.set_ylabel('Average Step Time (seconds)')
        ax2.set_title('Step Time Comparison')
        ax2.set_yscale('log')
        
        # Plot 3: Spike rate comparison
        ax3 = axes[1, 0]
        spike_rates = [r['average_spikes_per_step'] for r in config_results]
        ax3.bar(configs, spike_rates, color='red')
        ax3.set_ylabel('Average Spikes per Step')
        ax3.set_title('Spike Rate Comparison')
        
        # Plot 4: Scaling comparison (if available)
        ax4 = axes[1, 1]
        neuron_counts = [r.get('neuron_count', 0) for r in config_results]
        ax4.scatter(neuron_counts, ratings, s=100, alpha=0.7)
        
        if len(neuron_counts) > 1:
            # Add trend line
            z = np.polyfit(neuron_counts, ratings, 1)
            p = np.poly1d(z)
            x_range = np.linspace(min(neuron_counts), max(neuron_counts), 100)
            ax4.plot(x_range, p(x_range), 'r--', alpha=0.8, label='Trend')
            ax4.legend()
        
        ax4.set_xlabel('Neuron Count')
        ax4.set_ylabel('Performance Rating')
        ax4.set_title('Performance vs. Network Size')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Plot saved to {save_path}")
        
        plt.show()
    
    @staticmethod
    def plot_scaling_analysis(scaling_results: List[Dict[str, Any]], save_path: str = None):
        """Plot scaling analysis results."""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Performance Scaling Analysis', fontsize=16)
        
        if not scaling_results:
            print("No scaling results to plot")
            return
        
        neuron_counts = [r['neuron_count'] for r in scaling_results]
        avg_step_times = [r['average_step_time'] for r in scaling_results]
        spike_rates = [r['average_spikes_per_step'] for r in scaling_results]
        ratings = [r['performance_rating'] for r in scaling_results]
        
        # Plot 1: Neuron count vs step time (log-log)
        ax1 = axes[0, 0]
        ax1.loglog(neuron_counts, avg_step_times, 'bo-', linewidth=2, markersize=8)
        ax1.set_xlabel('Neuron Count (log)')
        ax1.set_ylabel('Average Step Time (seconds, log)')
        ax1.set_title('Scaling: Neurons vs. Step Time')
        ax1.grid(True, alpha=0.3)
        
        # Add trend line
        z = np.polyfit(np.log(neuron_counts), np.log(avg_step_times), 1)
        p = np.poly1d(z)
        x_range = np.linspace(np.log(min(neuron_counts)), np.log(max(neuron_counts)), 100)
        ax1.loglog(np.exp(x_range), np.exp(p(x_range)), 'r--', alpha=0.8, label=f'Power law: slope={z[0]:.2f}')
        ax1.legend()
        
        # Plot 2: Neuron count vs performance rating
        ax2 = axes[0, 1]
        ax2.plot(neuron_counts, ratings, 'go-', linewidth=2, markersize=8)
        ax2.set_xlabel('Neuron Count')
        ax2.set_ylabel('Performance Rating')
        ax2.set_title('Performance vs. Network Size')
        ax2.grid(True, alpha=0.3)
        
        # Add trend line
        z = np.polyfit(neuron_counts, ratings, 1)
        p = np.poly1d(z)
        x_range = np.linspace(min(neuron_counts), max(neuron_counts), 100)
        ax2.plot(x_range, p(x_range), 'r--', alpha=0.8, label=f'Linear trend')
        ax2.legend()
        
        # Plot 3: Neuron count vs spike rate
        ax3 = axes[1, 0]
        ax3.plot(neuron_counts, spike_rates, 'ro-', linewidth=2, markersize=8)
        ax3.set_xlabel('Neuron Count')
        ax3.set_ylabel('Average Spikes per Step')
        ax3.set_title('Activity Scaling with Network Size')
        ax3.grid(True, alpha=0.3)
        
        # Plot 4: Performance efficiency (rating per neuron)
        ax4 = axes[1, 1]
        efficiency = [r / n for r, n in zip(ratings, neuron_counts)]
        ax4.plot(neuron_counts, efficiency, 'bo-', linewidth=2, markersize=8)
        ax4.set_xlabel('Neuron Count')
        ax4.set_ylabel('Performance per Neuron')
        ax4.set_title('Computational Efficiency')
        ax4.grid(True, alpha=0.3)
        
        # Add efficiency trend
        z = np.polyfit(neuron_counts, efficiency, 1)
        p = np.poly1d(z)
        x_range = np.linspace(min(neuron_counts), max(neuron_counts), 100)
        ax4.plot(x_range, p(x_range), 'r--', alpha=0.8, label='Trend')
        ax4.legend()
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Scaling plot saved to {save_path}")
        
        plt.show()

# Example usage
if __name__ == "__main__":
    print("=== NLM Performance Benchmarking ===")
    
    # Create performance metrics and benchmark runner
    metrics = PerformanceMetrics()
    runner = BenchmarkRunner(metrics)
    
    # Define benchmark configurations
    configurations = [
        {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.08,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': False,
            'neuromod.curiosity.enable': False
        },
        {
            'brain.neuron_count': 1500,
            'brain.synapse_density': 0.12,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
            'neuromod.curiosity.enable': True
        },
        {
            'brain.neuron_count': 2000,
            'brain.synapse_density': 0.15,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True
        }
    ]
    
    # Run configuration benchmarks
    print("\n--- Configuration Benchmarks ---")
    config_results = runner.benchmark_multiple_configurations(configurations, steps_per_config=50)
    
    # Show results
    print("\nBenchmark Results:")
    for result in config_results:
        print(f"Config {result['run']}: Performance Rating={result['performance_rating']:.1f}, "
              f"Step Time={result['average_step_time']:.4f}s, "
              f"Spikes/Step={result['average_spikes_per_step']:.1f}")
    
    # Run scaling benchmark
    print("\n--- Scaling Benchmark ---")
    scaling_results = runner.benchmark_scaling(configurations[0], 
                                              [500, 1000, 2000, 5000], 
                                              steps=30)
    
    # Run plasticity benchmark
    print("\n--- Plasticity Benchmark ---")
    plasticity_results = runner.benchmark_plasticity_effect(configurations[0], steps=80)
    
    # Show plasticity analysis
    analysis = plasticity_results['summary']
    print(f"Best plasticity configuration: {[k for k, v in analysis['best_plasticity_config']['plasticity_config'].items() if v]}")
    print(f"Average performance rating: {analysis['average_rating']:.1f}")
    print(f"Recommendation: {analysis['recommendation']}")
    
    # Create visualizations
    print("\n--- Creating Visualizations ---")
    Visualizer.plot_configuration_comparison(config_results, "configuration_comparison.png")
    Visualizer.plot_scaling_analysis(scaling_results, "scaling_analysis.png")
    
    # Show overall metrics summary
    print("\n--- Overall Performance Summary ---")
    summary = metrics.get_summary()
    if summary:
        print(f"Total operations: {summary['total_operations']}")
        print(f"Average operation time: {summary['average_operation_time']:.4f}s")
        print(f"Simulation average step time: {summary['simulation']['average_step_time']:.4f}s")
        print(f"Peak memory usage: {summary['resource_usage']['peak_memory_mb']:.1f} MB")
        print(f"Average CPU usage: {summary['resource_usage']['average_cpu_percent']:.1f}%")
    
    # Save detailed results
    results_data = {
        'configurations': config_results,
        'scaling': scaling_results,
        'plasticity': plasticity_results,
        'metrics_summary': summary
    }
    
    with open('benchmark_results.json', 'w') as f:
        import json
        json.dump(results_data, f, indent=2, default=str)
    
    print(f"\nDetailed results saved to benchmark_results.json")
    print("\n=== Benchmarking Complete ===")
