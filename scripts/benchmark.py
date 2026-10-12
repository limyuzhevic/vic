#!/usr/bin/env python3
"""
Benchmarking Script

Runs performance benchmarks and compares different configurations.
Measures memory usage and generates performance reports.

Usage: python scripts/benchmark.py [config1] [config2] ... [options]
"""

import time
import psutil
import os
import sys
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
import json
import statistics
from datetime import datetime
import pandas as pd

class PerformanceProfiler:
    """Profiles and benchmarks NLM simulation performance."""
    
    def __init__(self):
        self.process = psutil.Process(os.getpid())
        self.metrics = {}
        self.start_time = None
        self.end_time = None
        
    def start_profiling(self):
        """Start performance profiling."""
        self.start_time = time.time()
        self.metrics['start_time'] = self.start_time
        self.metrics['cpu_percent_start'] = self.process.cpu_percent()
        self.metrics['memory_mb_start'] = self.process.memory_info().rss / 1024 / 1024
        
    def stop_profiling(self):
        """Stop performance profiling."""
        self.end_time = time.time()
        self.metrics['end_time'] = self.end_time
        self.metrics['total_time'] = self.end_time - self.start_time
        self.metrics['cpu_percent'] = self.process.cpu_percent()
        self.metrics['memory_mb'] = self.process.memory_info().rss / 1024 / 1024
        self.metrics['memory_mb_delta'] = (
            self.metrics['memory_mb'] - self.metrics['memory_mb_start']
        )
        
        return self.metrics
    
    def measure_time(self, func, *args, **kwargs):
        """Measure execution time of a function.
        
        Args:
            func: Function to execute
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Tuple of (result, metrics)
        """
        self.start_profiling()
        result = func(*args, **kwargs)
        metrics = self.stop_profiling()
        return result, metrics


def run_simulation(config_path: str) -> Dict[str, Any]:
    """Run a simulation with given configuration.
    
    Args:
        config_path: Path to configuration file
        
    Returns:
        Dictionary with simulation metrics
    """
    # This is a mock implementation - would be replaced with actual simulation code
    # For demonstration purposes, we'll simulate a workload
    
    results = {
        'config': config_path,
        'timestamp': datetime.now().isoformat(),
        'status': 'completed',
        'metrics': {}
    }
    
    profiler = PerformanceProfiler()
    
    # Simulate simulation steps
    num_steps = 1000
    for i in range(num_steps):
        # Simulate neural computations
        time.sleep(0.0001)  # Small delay to simulate work
        
        if i % 100 == 0:
            # Measure memory and CPU at intervals
            metrics = profiler.stop_profiling()
            results['metrics'][i] = metrics
    
    # Get final metrics
    final_metrics = profiler.stop_profiling()
    results['final_metrics'] = final_metrics
    
    # Calculate derived metrics
    results['throughput'] = num_steps / final_metrics['total_time']
    results['avg_memory_usage'] = statistics.mean([
        m['memory_mb_delta'] for m in results['metrics'].values()
    ])
    
    return results


def compare_configs(configs: List[str]) -> Dict[str, Any]:
    """Compare performance across multiple configurations.
    
    Args:
        configs: List of configuration file paths
        
    Returns:
        Comparison results dictionary
    """
    print("Running benchmarks for configurations:")
    for config in configs:
        print(f"  - {config}")
    
    comparison = {
        'configs': configs,
        'results': {},
        'comparison': {}
    }
    
    # Run benchmarks for each config
    for config in configs:
        print(f"\nBenchmarking {config}...")
        try:
            result = run_simulation(config)
            comparison['results'][config] = result
            
            # Extract key metrics
            comparison['comparison'][config] = {
                'total_time': result['final_metrics']['total_time'],
                'throughput': result['throughput'],
                'memory_usage': result['avg_memory_usage'],
                'status': result['status']
            }
            
        except Exception as e:
            print(f"Error benchmarking {config}: {e}")
            comparison['results'][config] = {
                'config': config,
                'status': 'failed',
                'error': str(e)
            }
    
    return comparison


def generate_performance_report(comparison: Dict[str, Any], output_path: str = None):
    """Generate performance report from comparison results.
    
    Args:
        comparison: Comparison results dictionary
        output_path: Output file path
    """
    if output_path is None:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_path = f"benchmark_report_{timestamp}.html"
    
    report = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Benchmark Performance Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .config-section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric-table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        .metric-table th, .metric-table td {{ border: 1px solid #ddd; padding: 10px; text-align: left; }}
        .metric-table th {{ background-color: #f2f2f2; }}
        .best {{ background-color: #d4edda; font-weight: bold; }}
        .metric-value {{ font-family: monospace; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Benchmark Report</h1>
        <p>Generated: {datetime.now().isoformat()}</p>
    </div>
    
    <h2>Configuration Comparisons</h2>
    
    <table class="metric-table">
        <thead>
            <tr>
                <th>Configuration</th>
                <th>Total Time (s)</th>
                <th>Throughput (steps/sec)</th>
                <th>Avg Memory Usage (MB)</th>
                <th>Status</th>
            </tr>
        </thead>
        <tbody>
"""
    
    # Add rows for each config
    for config, metrics in comparison.get('comparison', {}).items():
        is_best = False
        best_metric = None
        
        if metrics['status'] == 'completed':
            # Determine if this is the best for any metric
            if best_metric is None:
                best_metric = config
                is_best = True
        
        row_class = " best" if is_best else ""
        report += f"""
            <tr class="{row_class}">
                <td>{config}</td>
                <td class="metric-value">{metrics.get('total_time', 'N/A'):.3f}</td>
                <td class="metric-value">{metrics.get('throughput', 'N/A'):.1f}</td>
                <td class="metric-value">{metrics.get('memory_usage', 'N/A'):.1f}</td>
                <td>{metrics.get('status', 'N/A')}</td>
            </tr>
        """
    
    report += """
        </tbody>
    </table>
    
    <h2>Detailed Results</h2>
    
"""
    
    # Add detailed results for each config
    for config, result in comparison.get('results', {}).items():
        if result.get('status') == 'completed':
            report += f"""
    <div class="config-section">
        <h3>{config}</h3>
        <ul>
            <li><strong>Total Time:</strong> {result['final_metrics']['total_time']:.3f} seconds</li>
            <li><strong>Throughput:</strong> {result['throughput']:.1f} steps/second</li>
            <li><strong>Initial Memory:</strong> {result['final_metrics']['memory_mb_start']:.1f} MB</li>
            <li><strong>Final Memory:</strong> {result['final_metrics']['memory_mb']:.1f} MB</li>
            <li><strong>Memory Delta:</strong> {result['final_metrics']['memory_mb_delta']:.1f} MB</li>
            <li><strong>CPU Usage:</strong> {result['final_metrics']['cpu_percent']:.1f}%</li>
        </ul>
    </div>
            """
        else:
            report += f"""
    <div class="config-section">
        <h3>{config} - FAILED</h3>
        <p><strong>Error:</strong> {result.get('error', 'Unknown error')}</p>
    </div>
            """
    
    report += """
</body>
</html>
"""
    
    with open(output_path, 'w') as f:
        f.write(report)
    
    print(f"Performance report saved to: {output_path}")


def save_comparison_json(comparison: Dict[str, Any], output_path: str = None):
    """Save comparison results as JSON.
    
    Args:
        comparison: Comparison results dictionary
        output_path: Output file path
    """
    if output_path is None:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_path = f"benchmark_comparison_{timestamp}.json"
    
    with open(output_path, 'w') as f:
        json.dump(comparison, f, indent=2, default=str)
    
    print(f"Comparison data saved to: {output_path}")


def main():
    """Main entry point for the benchmarking script."""
    parser = argparse.ArgumentParser(description="Benchmark NLM simulations")
    parser.add_argument("configs", nargs="+", help="Configuration files to benchmark")
    parser.add_argument("-o", "--output", help="Output directory for reports")
    parser.add_argument("-r", "--runs", type=int, default=1, help="Number of runs per configuration")
    parser.add_argument("--json", action="store_true", help="Save comparison data as JSON")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    # Create output directory if specified
    if args.output:
        os.makedirs(args.output, exist_ok=True)
    
    # Run benchmarks
    all_comparisons = []
    for i in range(args.runs):
        if args.runs > 1:
            print(f"\nRun {i + 1}/{args.runs}")
        
        comparison = compare_configs(args.configs)
        all_comparisons.append(comparison)
    
    # Aggregate results if multiple runs
    if args.runs > 1:
        final_comparison = {
            'configs': args.configs,
            'runs': args.runs,
            'aggregated_results': {}
        }
        
        # Aggregate metrics across runs
        for config in args.configs:
            config_results = []
            for run in all_comparisons:
                if config in run.get('comparison', {}):
                    config_results.append(run['comparison'][config])
            
            if config_results:
                final_comparison['aggregated_results'][config] = {
                    'avg_time': statistics.mean([r['total_time'] for r in config_results]),
                    'avg_throughput': statistics.mean([r['throughput'] for r in config_results]),
                    'avg_memory': statistics.mean([r['memory_usage'] for r in config_results]),
                    'runs': len(config_results)
                }
        
        comparison = final_comparison
    else:
        comparison = all_comparisons[0]
    
    # Generate reports
    output_dir = args.output or "."
    
    report_path = os.path.join(output_dir, "performance_report.html")
    generate_performance_report(comparison, report_path)
    
    if args.json:
        json_path = os.path.join(output_dir, "benchmark_data.json")
        save_comparison_json(comparison, json_path)
    
    # Print summary
    print("\n" + "=" * 60)
    print("BENCHMARK SUMMARY")
    print("=" * 60)
    
    if 'aggregated_results' in comparison:
        for config, metrics in comparison['aggregated_results'].items():
            print(f"\n{config}:")
            print(f"  Average Time: {metrics['avg_time']:.3f}s")
            print(f"  Average Throughput: {metrics['avg_throughput']:.1f} steps/sec")
            print(f"  Average Memory: {metrics['avg_memory']:.1f} MB")
    else:
        for config, metrics in comparison.get('comparison', {}).items():
            print(f"\n{config}:")
            print(f"  Time: {metrics['total_time']:.3f}s")
            print(f"  Throughput: {metrics['throughput']:.1f} steps/sec")
            print(f"  Memory: {metrics['memory_usage']:.1f} MB")
            print(f"  Status: {metrics['status']}")
    
    print(f"\nReports saved to: {output_dir}")


if __name__ == "__main__":
    main()
