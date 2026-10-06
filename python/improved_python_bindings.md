# Performance Benchmarking and Profiling for NLM

This document provides comprehensive guidance for performance benchmarking and profiling the NLM (Neural Learning Machine) framework.

## Overview

Performance benchmarking is essential for understanding the capabilities and limitations of the NLM framework. This guide provides tools and methods for measuring performance, identifying bottlenecks, and optimizing simulations.

## Performance Measurement

### Basic Performance Metrics

```python
import pynlm as nlm
import time
import statistics
import matplotlib.pyplot as plt

class NLMPerformanceProfiler:
    def __init__(self, brain):
        self.brain = brain
        self.step_times = []
        self.firing_rates = []
        self.spike_counts = []
        self.memory_usage = []
        self.cpu_usage = []
    
    def profile_step(self, step_num, current_time):
        """Profile a single simulation step."""
        start_time = time.time()
        
        # Execute the step
        self.brain.step(step_num, current_time)
        
        end_time = time.time()
        step_time = end_time - start_time
        self.step_times.append(step_time)
        
        # Collect metrics
        self.firing_rates.append(self.brain.getAverageFiringRate())
        self.spike_counts.append(self.brain.getTotalSpikeCount())
        
        return step_time
    
    def get_performance_stats(self):
        """Get comprehensive performance statistics."""
        if not self.step_times:
            return None
        
        return {
            'total_steps': len(self.step_times),
            'total_time': sum(self.step_times),
            'avg_step_time': statistics.mean(self.step_times),
            'std_step_time': statistics.stdev(self.step_times) if len(self.step_times) > 1 else 0,
            'min_step_time': min(self.step_times),
            'max_step_time': max(self.step_times),
            'steps_per_second': len(self.step_times) / sum(self.step_times),
            'avg_firing_rate': statistics.mean(self.firing_rates),
            'std_firing_rate': statistics.stdev(self.firing_rates) if len(self.firing_rates) > 1 else 0,
            'total_spikes': self.spike_counts[-1] if self.spike_counts else 0,
            'spike_rate': self.spike_counts[-1] / (sum(self.step_times) if sum(self.step_times) > 0 else 1)
        }
    
    def plot_performance(self):
        """Generate performance plots."""
        if not self.step_times:
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Step time distribution
        axes[0, 0].hist(self.step_times, bins=50, alpha=0.7, edgecolor='black')
        axes[0, 0].set_title('Step Time Distribution')
        axes[0, 0].set_xlabel('Time per Step (s)')
        axes[0, 0].set_ylabel('Frequency')
        
        # Step time over time (rolling average)
        window_size = min(100, len(self.step_times))
        if window_size > 0:
            rolling_avg = [statistics.mean(self.step_times[max(0, i-window_size):i+1]) 
                          for i in range(len(self.step_times))]
        else:
            rolling_avg = self.step_times
        
        axes[0, 1].plot(rolling_avg)
        axes[0, 1].set_title('Step Time (Rolling Average)')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Time per Step (s)')
        
        # Firing rate over time
        axes[1, 0].plot(self.firing_rates)
        axes[1, 0].set_title('Firing Rate Over Time')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Firing Rate')
        
        # Spike count over time
        axes[1, 1].plot(self.spike_counts)
        axes[1, 1].set_title('Spike Count Over Time')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Total Spikes')
        
        plt.tight_layout()
        plt.show()
    
    def export_performance_data(self, filepath):
        """Export performance data to CSV."""
        import csv
        
        with open(filepath, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['Step', 'StepTime', 'FiringRate', 'TotalSpikes'])
            
            for i, (step_time, firing_rate, spike_count) in enumerate(zip(
                self.step_times, self.firing_rates, self.spike_counts)):
                writer.writerow([i, step_time, firing_rate, spike_count])

# Usage example
class NMLOptimizer:
    def __init__(self):
        self.profiler = None
    
    def optimize_brain_configuration(self, base_config, parameter_ranges, iterations=10):
        """Optimize brain configuration using performance metrics."""
        best_config = None
        best_performance = -float('inf')
        
        for iteration in range(iterations):
            # Generate random configuration from ranges
            config = self._generate_config_from_ranges(base_config, parameter_ranges)
            
            # Create brain and profile performance
            brain = nlm.createBrain(config)
            brain.initialize()
            
            self.profiler = NLMPerformanceProfiler(brain)
            
            # Run simulation
            for step in range(100):
                self.profiler.profile_step(step, step * 0.001)
            
            # Calculate performance score
            performance = self._calculate_performance_score(self.profiler.get_performance_stats())
            
            print(f"Iteration {iteration + 1}: Performance score = {performance:.3f}")
            
            # Update best configuration
            if performance > best_performance:
                best_performance = performance
                best_config = config
        
        return best_config, best_performance
    
    def _generate_config_from_ranges(self, base_config, parameter_ranges):
        """Generate configuration from parameter ranges."""
        config = nlm.createDefaultConfig()
        
        # Copy base configuration
        for key in base_config.getKeys():
            config.set(key, base_config.getOr<double>(key, 0.0))
        
        # Apply random parameters within ranges
        for param, (min_val, max_val) in parameter_ranges.items():
            if param == "neuron_count":
                config.set("neuron_count", int(np.random.uniform(min_val, max_val)))
            elif param == "connection_probability":
                config.set("connection_probability", np.random.uniform(min_val, max_val))
            elif param == "region_count":
                config.set("region_count", int(np.random.uniform(min_val, max_val)))
            elif param == "enable_plasticity":
                config.set("enable_plasticity", np.random.choice([True, False]))
        
        return config
    
    def _calculate_performance_score(self, stats):
        """Calculate performance score from statistics."""
        if not stats:
            return -float('inf')
        
        # Weighted score based on multiple metrics
        firing_rate_score = min(stats['avg_firing_rate'] / 10.0, 1.0)  # Target ~10 Hz
        stability_score = 1.0 - min(stats['std_step_time'] / 0.001, 1.0)  # Lower step time variability is better
        efficiency_score = min(stats['steps_per_second'] / 1000.0, 1.0)  # Target ~1000 steps/sec
        
        # Combined score
        return (firing_rate_score * 0.4 + stability_score * 0.3 + efficiency_score * 0.3)

# Advanced benchmarking with multiple configurations
class ConfigurationBenchmark:
    def __init__(self):
        self.benchmark_results = []
    
    def benchmark_configuration(self, config, steps=500, description=""):
        """Benchmark a specific configuration."""
        print(f"Benchmarking: {description}")
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        profiler = NLMPerformanceProfiler(brain)
        
        start_time = time.time()
        for step in range(steps):
            profiler.profile_step(step, step * 0.001)
        end_time = time.time()
        
        stats = profiler.get_performance_stats()
        stats['wall_clock_time'] = end_time - start_time
        stats['description'] = description
        stats['config_params'] = self._extract_config_params(config)
        
        self.benchmark_results.append(stats)
        
        return stats
    
    def _extract_config_params(self, config):
        """Extract configuration parameters for comparison."""
        params = {}
        for key in ["neuron_count", "region_count", "connection_probability",
                   "enable_plasticity", "enable_development"]:
            if config.has(key):
                params[key] = config.getOr<double>(key, 0.0)
        return params
    
    def compare_configurations(self):
        """Compare all benchmarked configurations."""
        if len(self.benchmark_results) < 2:
            return
        
        print("\n=== Configuration Comparison ===")
        
        # Create comparison table
        headers = ["Config", "Neurons", "Regions", "Conn Prob", "Plasticity",
                  "Dev", "Firing Rate", "Steps/sec", "Stability", "Total Time"]
        
        print(f"{'':<10}", end="")
        for header in headers[1:]:
            print(f"{header:<15}", end="")
        print()
        
        for i, result in enumerate(self.benchmark_results):
            params = result['config_params']
            print(f"{result['description']:<10}", end="")
            print(f"{params.get('neuron_count', 'N/A'):<15}", end="")
            print(f"{params.get('region_count', 'N/A'):<15}", end="")
            print(f"{params.get('connection_probability', 'N/A'):<15}", end="")
            print(f"{str(params.get('enable_plasticity', 'N/A')):<15}", end="")
            print(f"{str(params.get('enable_development', 'N/A')):<15}", end="")
            print(f"{result['avg_firing_rate']:<15.2f}", end="")
            print(f"{result['steps_per_second']:<15.0f}", end="")
            stability = 1.0 - min(result['std_step_time'] / 0.001, 1.0)
            print(f"{stability:<15.2f}", end="")
            print(f"{result['wall_clock_time']:<15.2f}")
    
    def generate_performance_report(self, output_file="benchmark_report.html"):
        """Generate a detailed performance report."""
        html = self._generate_html_report()
        
        with open(output_file, 'w') as f:
            f.write(html)
        
        print(f"Performance report saved to {output_file}")
    
    def _generate_html_report(self):
        """Generate HTML report content."""
        html = """
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Benchmark Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }
        .section { margin: 20px 0; }
        .stats-table { border-collapse: collapse; width: 100%; }
        .stats-table th, .stats-table td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        .stats-table th { background-color: #f2f2f2; }
        .chart { margin: 20px 0; }
        .summary { background-color: #ecf0f1; padding: 15px; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Benchmark Report</h1>
        <p>Generated on """ + time.strftime("%Y-%m-%d %H:%M:%S") + """
    </div>
    
    <div class="section">
        <h2>Benchmark Summary</h2>
        <div class="summary">
            <p>Total configurations tested: """ + str(len(self.benchmark_results)) + """
            <p>Best performance: """ + self._get_best_performance_summary() + """
            <p>Average firing rate range: """ + self._get_firing_rate_range() + """
        </div>
    </div>
    
    <div class="section">
        <h2>Detailed Results</h2>
        <table class="stats-table">
            <thead>
                <tr>
                    <th>Configuration</th>
                    <th>Neurons</th>
                    <th>Regions</th>
                    <th>Connection Probability</th>
                    <th>Plasticity</th>
                    <th>Development</th>
                    <th>Firing Rate (Hz)</th>
                    <th>Steps/sec</th>
                    <th>Stability</th>
                    <th>Total Time (s)</th>
                </tr>
            </thead>
            <tbody>
                """ + self._generate_html_table_rows() + """
            </tbody>
        </table>
    </div>
</body>
</html>
        """
        return html
    
    def _generate_html_table_rows(self):
        """Generate HTML table rows for benchmark results."""
        rows = []
        for result in self.benchmark_results:
            params = result['config_params']
            stability = 1.0 - min(result['std_step_time'] / 0.001, 1.0)
            row = f"""
                <tr>
                    <td>{result['description']}</td>
                    <td>{params.get('neuron_count', 'N/A')}</td>
                    <td>{params.get('region_count', 'N/A')}</td>
                    <td>{params.get('connection_probability', 'N/A'):.3f}</td>
                    <td>{str(params.get('enable_plasticity', 'N/A'))}</td>
                    <td>{str(params.get('enable_development', 'N/A'))}</td>
                    <td>{result['avg_firing_rate']:.2f}</td>
                    <td>{result['steps_per_second']:.0f}</td>
                    <td>{stability:.3f}</td>
                    <td>{result['wall_clock_time']:.2f}</td>
                </tr>
            """
            rows.append(row)
        return "".join(rows)
    
    def _get_best_performance_summary(self):
        """Get best performance summary."""
        if not self.benchmark_results:
            return "N/A"
        
        best = max(self.benchmark_results, key=lambda x: x['avg_firing_rate'] * x['steps_per_second'])
        return f"{best['description']} ({best['avg_firing_rate']:.2f} Hz, {best['steps_per_second']:.0f} steps/sec)"
    
    def _get_firing_rate_range(self):
        """Get firing rate range."""
        if not self.benchmark_results:
            return "N/A"
        
        firing_rates = [r['avg_firing_rate'] for r in self.benchmark_results]
        return f"{min(firing_rates):.2f} - {max(firing_rates):.2f} Hz"

# Advanced profiling with detailed metrics
class DetailedProfiler:
    def __init__(self, brain):
        self.brain = brain
        self.detailed_metrics = []
        self.memory_samples = []
        self.cpu_samples = []
    
    def profile_detailed(self, step_num, current_time):
        """Profile step with detailed metrics."""
        import psutil
        import os
        
        start_time = time.time()
        
        # Collect baseline metrics
        process = psutil.Process(os.getpid())
        memory_before = process.memory_info().rss / 1024 / 1024  # MB
        cpu_before = process.cpu_percent()
        
        # Execute the step
        self.brain.step(step_num, current_time)
        
        end_time = time.time()
        
        # Collect post-execution metrics
        memory_after = process.memory_info().rss / 1024 / 1024  # MB
        cpu_after = process.cpu_percent()
        
        # Calculate metrics
        step_time = end_time - start_time
        memory_change = memory_after - memory_before
        cpu_change = cpu_after - cpu_before
        
        # Get brain-specific metrics
        metrics = {
            'step': step_num,
            'step_time': step_time,
            'memory_before_mb': memory_before,
            'memory_after_mb': memory_after,
            'memory_change_mb': memory_change,
            'cpu_before_percent': cpu_before,
            'cpu_after_percent': cpu_after,
            'cpu_change_percent': cpu_change,
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'timestamp': time.time()
        }
        
        self.detailed_metrics.append(metrics)
        self.memory_samples.append(memory_after)
        self.cpu_samples.append(cpu_after)
        
        return metrics
    
    def get_memory_efficiency(self):
        """Calculate memory efficiency metrics."""
        if not self.memory_samples:
            return None
        
        return {
            'max_memory_mb': max(self.memory_samples),
            'min_memory_mb': min(self.memory_samples),
            'avg_memory_mb': statistics.mean(self.memory_samples),
            'memory_growth_mb': self.memory_samples[-1] - self.memory_samples[0],
            'memory_efficiency': (self.memory_samples[0] / max(self.memory_samples[1], 1)) - 1.0
        }
    
    def get_cpu_efficiency(self):
        """Calculate CPU efficiency metrics."""
        if not self.cpu_samples:
            return None
        
        return {
            'avg_cpu_percent': statistics.mean(self.cpu_samples),
            'max_cpu_percent': max(self.cpu_samples),
            'cpu_efficiency': 100.0 / max(self.cpu_samples, key=lambda x: x if x > 0 else 1)
        }
    
    def detect_memory_leaks(self):
        """Detect potential memory leaks."""
        if len(self.memory_samples) < 10:
            return None
        
        # Check for sustained memory growth
        first_third = self.memory_samples[:len(self.memory_samples)//3]
        last_third = self.memory_samples[-len(first_third):]
        
        avg_first = statistics.mean(first_third)
        avg_last = statistics.mean(last_third)
        
        growth_rate = (avg_last - avg_first) / len(first_third)
        
        return {
            'growth_rate_mb_per_step': growth_rate,
            'potential_leak': growth_rate > 0.01,  # More than 0.01 MB per step
            'total_growth_mb': avg_last - avg_first
        }
    
    def plot_detailed_metrics(self):
        """Plot detailed metrics."""
        if not self.detailed_metrics:
            return
        
        fig, axes = plt.subplots(3, 2, figsize=(15, 12))
        
        metrics = self.detailed_metrics
        steps = [m['step'] for m in metrics]
        
        # Step time
        axes[0, 0].plot(steps, [m['step_time'] for m in metrics])
        axes[0, 0].set_title('Step Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Time (s)')
        
        # Memory usage
        axes[0, 1].plot(steps, [m['memory_after_mb'] for m in metrics])
        axes[0, 1].set_title('Memory Usage')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Memory (MB)')
        
        # CPU usage
        axes[1, 0].plot(steps, [m['cpu_after_percent'] for m in metrics])
        axes[1, 0].set_title('CPU Usage')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('CPU (%)')
        
        # Firing rate
        axes[1, 1].plot(steps, [m['firing_rate'] for m in metrics])
        axes[1, 1].set_title('Firing Rate')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Firing Rate (Hz)')
        
        # Active neurons
        axes[2, 0].plot(steps, [m['active_neurons'] for m in metrics])
        axes[2, 0].set_title('Active Neurons')
        axes[2, 0].set_xlabel('Step')
        axes[2, 0].set_ylabel('Count')
        
        # E/I ratio
        axes[2, 1].plot(steps, [m['e_i_ratio'] for m in metrics])
        axes[2, 1].set_title('Excitatory/Inhibitory Ratio')
        axes[2, 1].set_xlabel('Step')
        axes[2, 1].set_ylabel('Ratio')
        
        plt.tight_layout()
        plt.show()

# Utility functions for performance analysis
class PerformanceAnalyzer:
    @staticmethod
    def calculate_stability_coefficient(metrics):
        """Calculate stability coefficient from metrics."""
        if len(metrics) < 2:
            return 0.0
        
        # Calculate coefficient of variation for key metrics
        step_times = [m['step_time'] for m in metrics]
        firing_rates = [m['firing_rate'] for m in metrics]
        
        step_time_cv = statistics.stdev(step_times) / statistics.mean(step_times) if statistics.mean(step_times) > 0 else 0
        firing_rate_cv = statistics.stdev(firing_rates) / statistics.mean(firing_rates) if statistics.mean(firing_rates) > 0 else 0
        
        # Weighted stability score
        stability = 1.0 - (step_time_cv * 0.6 + firing_rate_cv * 0.4)
        return max(0.0, stability)
    
    @staticmethod
    def find_optimal_parameters(benchmark_results):
        """Find optimal parameters based on benchmark results."""
        if not benchmark_results:
            return None
        
        # Score based on multiple metrics
        for result in benchmark_results:
            # Calculate composite score
            firing_score = min(result['avg_firing_rate'] / 10.0, 1.0)
            efficiency_score = min(result['steps_per_second'] / 1000.0, 1.0)
            
            # Calculate stability
            stability_score = 1.0 - min(result['std_step_time'] / 0.001, 1.0)
            
            result['composite_score'] = (firing_score * 0.4 + 
                                       efficiency_score * 0.4 + 
                                       stability_score * 0.2)
        
        # Return best configuration
        best = max(benchmark_results, key=lambda x: x['composite_score'])
        return best
    
    @staticmethod
    def analyze_scaling(benchmark_results):
        """Analyze scaling behavior with different configurations."""
        if len(benchmark_results) < 2:
            return None
        
        # Extract key metrics
        neuron_counts = []
        firing_rates = []
        step_rates = []
        
        for result in benchmark_results:
            params = result['config_params']
            neuron_counts.append(params.get('neuron_count', 0))
            firing_rates.append(result['avg_firing_rate'])
            step_rates.append(result['steps_per_second'])
        
        # Calculate scaling relationships
        analysis = {
            'neuron_firing_rate_correlation': PerformanceAnalyzer._calculate_correlation(neuron_counts, firing_rates),
            'neuron_step_rate_correlation': PerformanceAnalyzer._calculate_correlation(neuron_counts, step_rates),
            'firing_rate_step_rate_correlation': PerformanceAnalyzer._calculate_correlation(firing_rates, step_rates)
        }
        
        return analysis
    
    @staticmethod
    def _calculate_correlation(x, y):
        """Calculate Pearson correlation coefficient."""
        if len(x) < 2 or len(y) < 2:
            return 0.0
        
        mean_x = statistics.mean(x)
        mean_y = statistics.mean(y)
        
        numerator = sum((xi - mean_x) * (yi - mean_y) for xi, yi in zip(x, y))
        denominator_x = sum((xi - mean_x) ** 2 for xi in x)
        denominator_y = sum((yi - mean_y) ** 2 for yi in y)
        
        if denominator_x == 0 or denominator_y == 0:
            return 0.0
        
        return numerator / (denominator_x * denominator_y) ** 0.5

# Main performance benchmarking workflow
class PerformanceBenchmarkWorkflow:
    def __init__(self):
        self.benchmark = ConfigurationBenchmark()
        self.analyzer = PerformanceAnalyzer()
    
    def run_complete_benchmark(self, configurations, steps=500):
        """Run complete performance benchmark for multiple configurations."""
        print("=== NLM Performance Benchmark ===")
        
        # Benchmark each configuration
        for i, config in enumerate(configurations):
            self.benchmark.benchmark_configuration(
                config, steps, description=f"Config {i+1}"
            )
        
        # Compare configurations
        self.benchmark.compare_configurations()
        
        # Analyze scaling
        scaling_analysis = self.analyzer.analyze_scaling(self.benchmark.benchmark_results)
        if scaling_analysis:
            print("\n=== Scaling Analysis ===")
            for metric, correlation in scaling_analysis.items():
                print(f"{metric}: {correlation:.3f}")
        
        # Find optimal configuration
        optimal = self.analyzer.find_optimal_parameters(self.benchmark.benchmark_results)
        if optimal:
            print(f"\n=== Optimal Configuration ===")
            print(f"Description: {optimal['description']}")
            print(f"Composite Score: {optimal['composite_score']:.3f}")
            print(f"Firing Rate: {optimal['avg_firing_rate']:.2f} Hz")
            print(f"Steps/sec: {optimal['steps_per_second']:.0f}")
            print(f"Stability: {1.0 - min(optimal['std_step_time'] / 0.001, 1.0):.3f}")
        
        # Generate detailed report
        self.benchmark.generate_performance_report()
        
        return self.benchmark.benchmark_results

# Example usage
if __name__ == "__main__":
    # Create different brain configurations
    configs = []
    
    # Configuration 1: Small brain
    config1 = nlm.createDefaultConfig()
    config1.set("neuron_count", 100)
    config1.set("region_count", 1)
    config1.set("connection_probability", 0.2)
    config1.set("random_seed", 42)
    configs.append(config1)
    
    # Configuration 2: Medium brain
    config2 = nlm.createDefaultConfig()
    config2.set("neuron_count", 500)
    config2.set("region_count", 2)
    config2.set("connection_probability", 0.15)
    config2.set("random_seed", 123)
    configs.append(config2)
    
    # Configuration 3: Large brain
    config3 = nlm.createDefaultConfig()
    config3.set("neuron_count", 1000)
    config3.set("region_count", 4)
    config3.set("connection_probability", 0.1)
    config3.set("random_seed", 456)
    configs.append(config3)
    
    # Configuration 4: With plasticity
    config4 = nlm.createDefaultConfig()
    config4.set("neuron_count", 500)
    config4.set("region_count", 2)
    config4.set("connection_probability", 0.15)
    config4.set("random_seed", 789)
    config4.set("enable_plasticity", True)
    config4.set("enable_development", True)
    configs.append(config4)
    
    # Run complete benchmark
    results = PerformanceBenchmarkWorkflow().run_complete_benchmark(configs, steps=300)
```