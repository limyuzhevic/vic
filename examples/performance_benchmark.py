#!/usr/bin/env python3
"""
NLM Performance Optimization and Benchmarking Example

This example demonstrates:
- Advanced performance monitoring and profiling
- Multi-mode benchmarking
- Resource usage optimization
- Comparative analysis of different configurations
"""

import pynlm
import time
import statistics
import sys
from typing import Dict, List, Tuple, Optional
class NLMPerformanceProfiler:
    """Advanced performance profiler for NLM brains"""

    def __init__(self):
        self.results = {
            'configurations': {},
            'benchmarks': {},
            'performance_metrics': {}
        }

    def create_optimized_brain(self, config_name: str, config_params: Dict) -> pynlm.Brain:
        """Create brain with specified configuration"""
        config = pynlm.createDefaultConfig()

        for key, value in config_params.items():
            config.set(key, value, pynlm.ConfigSource.Runtime)

        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain

    def benchmark_performance(self, brain: pynlm.Brain, duration: int,
                            description: str) -> Dict:
        """Comprehensive performance benchmarking"""
        print(f"\nBenchmarking: {description}")
        print(f"Duration: {duration} steps")

        # Initialize monitoring
        step_times = []
        spike_counts = []
        firing_rates = []
        curiosity_levels = []
        novelty_levels = []
        prediction_errors = []

        benchmark_start_time = time.time()

        for step in range(duration):
            step_start_time = time.time()

            # Perform computation
            brain.step(step)

            step_end_time = time.time()
            step_duration = step_end_time - step_start_time

            # Collect metrics
            step_times.append(step_duration)
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())

            # Note: We need an agent to get neuromodulation metrics
            # This is a conceptual implementation
            step_end_time = time.time()
            step_duration = step_end_time - step_start_time
            step_times.append(step_duration)

        benchmark_end_time = time.time()
        total_time = benchmark_end_time - benchmark_start_time

        # Calculate performance metrics
        performance = {
            'description': description,
            'total_duration': duration,
            'wallclock_time': total_time,
            'steps_per_second': duration / total_time if total_time > 0 else 0,
            'avg_step_time': statistics.mean(step_times) if step_times else 0,
            'min_step_time': min(step_times) if step_times else 0,
            'max_step_time': max(step_times) if step_times else 0,
            'step_time_std': statistics.stdev(step_times) if len(step_times) > 1 else 0,
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_spike_rate': statistics.mean(spike_counts) if spike_counts else 0,
            'max_spike_rate': max(spike_counts) if spike_counts else 0,
            'avg_firing_rate': statistics.mean(firing_rates) if firing_rates else 0,
            'firing_rate_std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
            'spike_count_variance': statistics.variance(spike_counts) if len(spike_counts) > 1 else 0,
            'stability_score': self._calculate_stability_score(spike_counts) if spike_counts else 0,
        }

        return performance

    def _calculate_stability_score(self, spike_counts: List[int]) -> float:
        """Calculate neural stability score based on spike count variance"""
        if len(spike_counts) < 2:
            return 1.0

        mean_spikes = statistics.mean(spike_counts)
        variance = statistics.variance(spike_counts)
        coefficient_of_variation = (variance ** 0.5) / mean_spikes if mean_spikes > 0 else 0

        # Lower CV indicates more stable activity
        stability_score = max(0.0, 1.0 - coefficient_of_variation)
        return stability_score

    def run_configuration_comparison(self, configurations: List[Dict]):
        """Compare performance across different configurations"""
        print("=== NLM Configuration Performance Comparison ===")

        for i, config_params in enumerate(configurations):
            config_name = config_params.get('name', f"Config_{i+1}")
            print(f"\nConfiguration: {config_name}")

            # Create brain with this configuration
            brain = self.create_optimized_brain(config_name, config_params)

            # Run comprehensive benchmarks
            duration = config_params.get('benchmark_duration', 500)
            performance = self.benchmark_performance(brain, duration, config_name)

            # Store results
            self.results['configurations'][config_name] = config_params
            self.results['benchmarks'][config_name] = performance
            self.results['performance_metrics'][config_name] = self._extract_key_metrics(performance)

            print(f"  Performance score: {performance.get('performance_score', 'N/A')}")

    def _extract_key_metrics(self, performance: Dict) -> Dict:
        """Extract key performance metrics for comparison"""
        return {
            'throughput': performance['steps_per_second'],
            'stability': performance['stability_score'],
            'efficiency': performance['avg_step_time'],
            'spike_activity': performance['avg_spike_rate'],
            'neural_variance': performance['spike_count_variance']
        }

    def generate_performance_report(self) -> str:
        """Generate comprehensive performance report"""
        report = []
        report.append("=== NLM Performance Analysis Report ===\n")

        if not self.results['benchmarks']:
            return "No benchmark data available.\n"

        # Configuration comparison
        report.append("1. CONFIGURATION COMPARISON")
        report.append("=" * 40)

        # Sort by performance score for ranking
        sorted_configs = sorted(self.results['performance_metrics'].items(),
                                key=lambda x: x[1].get('throughput', 0), reverse=True)

        report.append(f"{'Configuration':<15} {'Throughput':<12} {'Stability':<10} {'Efficiency':<12} {'Spike Rate':<12}")
        report.append("-" * 65)

        for config_name, metrics in sorted_configs:
            report.append(f"{config_name:<15} {metrics['throughput']:>9.1f} {metrics['stability']:>8.3f} "
                        f"{metrics['efficiency']*1000:>9.2f}ms {metrics['spike_activity']:>9.1f}")

        report.append("\n")

        # Best configuration analysis
        report.append("2. BEST CONFIGURATION ANALYSIS")
        report.append("=" * 40)

        best_config = sorted_configs[0][0] if sorted_configs else None
        if best_config:
            best_perf = self.results['benchmarks'][best_config]
            report.append(f"Best configuration: {best_config}")
            report.append(f"  Total spikes generated: {best_perf['total_spikes']}")
            report.append(f"  Average firing rate: {best_perf['avg_firing_rate']:.2f} Hz")
            report.append(f"  Stability score: {best_perf['stability_score']:.3f}")
            report.append(f"  Step efficiency: {best_perf['avg_step_time']*1000:.2f}ms average")
            report.append(f"  Performance rating: {self._rate_performance(best_perf)}")

        report.append("\n")

        # Performance optimization recommendations
        report.append("3. PERFORMANCE OPTIMIZATION RECOMMENDATIONS")
        report.append("=" * 40)

        recommendations = self._generate_recommendations(sorted_configs)
        for i, recommendation in enumerate(recommendations, 1):
            report.append(f"{i}. {recommendation}")

        report.append("\n")

        # Memory usage analysis (conceptual)
        report.append("4. MEMORY USAGE ANALYSIS")
        report.append("=" * 40)
        report.append("Note: Memory analysis would require profiling tools")
        report.append("Current recommendations:")
        report.append("  - Monitor neuron count vs. connection density")
        report.append("  - Balance between model complexity and performance")
        report.append("  - Consider checkpointing for long-running simulations")

        return "\n".join(report)

    def _rate_performance(self, performance: Dict) -> str:
        """Rate performance based on key metrics"""
        throughput = performance['steps_per_second']
        stability = performance['stability_score']
        avg_step_time = performance['avg_step_time']

        if throughput > 1000 and stability > 0.8 and avg_step_time < 0.01:
            return "EXCELLENT"
        elif throughput > 500 and stability > 0.6 and avg_step_time < 0.02:
            return "GOOD"
        elif throughput > 100 and stability > 0.4:
            return "FAIR"
        else:
            return "NEEDS IMPROVEMENT"

    def _generate_recommendations(self, sorted_configs: List[Tuple[str, Dict]]) -> List[str]:
        """Generate optimization recommendations based on benchmark results"""
        recommendations = []

        if len(sorted_configs) < 2:
            return ["Run more configurations for comparison analysis"]

        # Configuration 1 analysis
        config1_name, config1_metrics = sorted_configs[0]
        config2_name, config2_metrics = sorted_configs[1]

        throughput_ratio = config1_metrics['throughput'] / max(config2_metrics['throughput'], 0.001)
        stability_diff = config1_metrics['stability'] - config2_metrics['stability']

        if throughput_ratio > 1.5:
            recommendations.append(
                f"Configuration '{config1_name}' shows {throughput_ratio:.1f}x better throughput. "
                f"Consider using this configuration for high-performance applications."
            )
        elif stability_diff > 0.2:
            recommendations.append(
                f"Configuration '{config1_name}' has {stability_diff:.2f} higher stability. "
                f"Use for applications requiring stable neural activity."
            )
        else:
            recommendations.append(
                "Configurations show similar performance. Consider application-specific requirements."
            )

        # Efficiency recommendations
        efficiency_gaps = []
        for config_name, metrics in sorted_configs:
            if metrics['efficiency'] > 0.05:  # > 50ms per step
                efficiency_gaps.append(config_name)

        if efficiency_gaps:
            recommendations.append(
                f"Configurations {', '.join(efficiency_gaps)} have high step processing times. "
                f"Consider reducing neuron count or optimizing connectivity."
            )

        # Spike activity recommendations
        spike_rates = [metrics['spike_activity'] for _, metrics in sorted_configs]
        if max(spike_rates) / max(min(spike_rates), 0.001) > 3:
            recommendations.append(
                "Large variance in spike activity detected. "
                "This may indicate different learning dynamics or stability issues."
            )

        # General optimization
        recommendations.append(
            "For production use, balance between throughput, stability, and computational efficiency."
        )

        return recommendations
def create_comparison_configurations():
    """Define different configurations for comparison"""
    configurations = [
        {
            'name': 'Compact',
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.1,
            'random_seed': 42,
            'benchmark_duration': 300
        },
        {
            'name': 'Balanced',
            'neuron_count': 1000,
            'region_count': 3,
            'connection_probability': 0.12,
            'random_seed': 123,
            'benchmark_duration': 500
        },
        {
            'name': 'Dense',
            'neuron_count': 1500,
            'region_count': 4,
            'connection_probability': 0.15,
            'random_seed': 456,
            'benchmark_duration': 400
        },
        {
            'name': 'Efficient',
            'neuron_count': 800,
            'region_count': 2,
            'connection_probability': 0.08,
            'random_seed': 789,
            'benchmark_duration': 600
        },
        {
            'name': 'High Plasticity',
            'neuron_count': 1200,
            'region_count': 3,
            'connection_probability': 0.2,
            'random_seed': 999,
            'benchmark_duration': 450,
            'stdp_ltp_weight': 0.03,
            'stdp_ltd_weight': 0.025
        }
    ]

    return configurations
def main():
    """Main function to run performance comparison"""
    print("=== NLM Performance Optimization and Benchmarking ===")
    print("This tool compares different NLM brain configurations for optimal performance")
    print()

    # Initialize profiler
    profiler = NLMPerformanceProfiler()

    # Get configurations
    configurations = create_comparison_configurations()

    # Run comparison
    profiler.run_configuration_comparison(configurations)

    # Generate report
    report = profiler.generate_performance_report()
    print(report)

    # Save report to file
    with open("performance_report.txt", "w") as f:
        f.write(report)

    print("Report saved to 'performance_report.txt'")
if __name__ == "__main__":
    main()