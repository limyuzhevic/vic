#!/usr/bin/env python3
"""
Benchmark Suite for NLM Brain Simulation

Comprehensive benchmarking suite for comparing different brain configurations,
neuron models, and performance characteristics. Provides performance regression
testing, automated benchmarking with result storage, and detailed comparison
reports.

Features:
- Configuration comparison benchmarking
- Neuron model performance comparison
- Performance regression testing
- Automated benchmarking with result storage
- Statistical analysis and performance metrics
- HTML, JSON, and CSV report generation
- Multi-run benchmarking with aggregation

Usage:
    # Compare different configurations
    python benchmark_suite.py --configs config1.json config2.json config3.json
    
    # Benchmark neuron models
    python benchmark_suite.py --neuron-models neuron_a neuron_b neuron_c
    
    # Performance regression testing
    python benchmark_suite.py --regression --baseline baseline_results.json
    
    # Multi-run automated benchmarking
    python benchmark_suite.py --auto --output-dir results/ --runs 10
    
    # Generate comparative reports
    python benchmark_suite.py --report --format html --configs *.json

Examples:
    >>> from benchmark_suite import BenchmarkSuite
    >>> suite = BenchmarkSuite()
    >>> results = suite.compare_configs(['config1.json', 'config2.json'])
    >>> suite.generate_report(results, 'comparison_report.html')
    
    >>> suite.benchmark_neuron_models(['standard', 'enhanced', 'optimized'])
    >>> suite.generate_performance_summary('neuron_comparison.html')

See Also:
    performance_profiler.py - For detailed performance profiling
    memory_profiler.py - For memory benchmarking
    load_tester.py - For stress testing
    performance_analyzer.py - For bottleneck analysis

API Reference:
    BenchmarkSuite - Main benchmarking class
    compare_configs() - Compare different brain configurations
    benchmark_neuron_models() - Benchmark neuron models
    run_regression_test() - Performance regression testing
    generate_report() - Generate comprehensive reports

Author: NLM Team
Version: 1.0.0
License: MIT

For support and documentation, visit: https://github.com/your-repo/nlm-tools
"""

import time
import os
import sys
import argparse
from typing import Dict, List, Any, Optional, Tuple
from pathlib import Path
import json
import statistics
from datetime import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
import seaborn as sns

# Set style for matplotlib
plt.style.use('seaborn-v0_8')


class PerformanceMetrics:
    """Container for performance metrics."""
    
    def __init__(self):
        self.timings = []
        self.memory_usage = []
        self.cpu_usage = []
        self.throughput = []
        self.latency = []
        
    def add_sample(self, timing: float, memory: float, cpu: float, 
                   throughput: float, latency: float):
        """Add a performance sample."""
        self.timings.append(timing)
        self.memory_usage.append(memory)
        self.cpu_usage.append(cpu)
        self.throughput.append(throughput)
        self.latency.append(latency)
        
    def get_summary(self) -> Dict[str, Any]:
        """Get summary statistics."""
        if not self.timings:
            return {}
            
        summary = {
            'timings': {
                'mean': statistics.mean(self.timings),
                'median': statistics.median(self.timings),
                'std': statistics.stdev(self.timings) if len(self.timings) > 1 else 0,
                'min': min(self.timings),
                'max': max(self.timings),
                'count': len(self.timings)
            },
            'memory': {
                'mean': statistics.mean(self.memory_usage),
                'median': statistics.median(self.memory_usage),
                'std': statistics.stdev(self.memory_usage) if len(self.memory_usage) > 1 else 0,
                'min': min(self.memory_usage),
                'max': max(self.memory_usage)
            },
            'cpu': {
                'mean': statistics.mean(self.cpu_usage),
                'median': statistics.median(self.cpu_usage),
                'std': statistics.stdev(self.cpu_usage) if len(self.cpu_usage) > 1 else 0,
                'max': max(self.cpu_usage)
            },
            'throughput': {
                'mean': statistics.mean(self.throughput),
                'median': statistics.median(self.throughput),
                'std': statistics.stdev(self.throughput) if len(self.throughput) > 1 else 0,
                'min': min(self.throughput),
                'max': max(self.throughput)
            },
            'latency': {
                'mean': statistics.mean(self.latency),
                'median': statistics.median(self.latency),
                'std': statistics.stdev(self.latency) if len(self.latency) > 1 else 0,
                '95_percentile': np.percentile(self.latency, 95) if self.latency else 0
            }
        }
        
        # Calculate coefficient of variation
        summary['timings']['cv'] = summary['timings']['std'] / summary['timings']['mean'] if summary['timings']['mean'] > 0 else 0
        summary['memory']['cv'] = summary['memory']['std'] / summary['memory']['mean'] if summary['memory']['mean'] > 0 else 0
        summary['throughput']['cv'] = summary['throughput']['std'] / summary['throughput']['mean'] if summary['throughput']['mean'] > 0 else 0
        
        return summary


class ConfigurationBenchmark:
    """Benchmark a specific brain configuration."""
    
    def __init__(self, config_name: str):
        self.config_name = config_name
        self.metrics = PerformanceMetrics()
        self.run_results = []
        
    def run_benchmark(self, simulation_func, *args, **kwargs) -> Dict[str, Any]:
        """Run benchmark for a single iteration."""
        import psutil
        
        # Start monitoring
        start_time = time.time()
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024
        initial_cpu = process.cpu_percent()
        
        # Run simulation
        simulation_start = time.time()
        result = simulation_func(*args, **kwargs)
        simulation_end = time.time()
        
        # Calculate metrics
        simulation_time = simulation_end - simulation_start
        total_time = time.time() - start_time
        final_memory = process.memory_info().rss / 1024 / 1024
        final_cpu = process.cpu_percent()
        
        # Calculate derived metrics
        memory_delta = final_memory - initial_memory
        cpu_delta = final_cpu - initial_cpu
        
        # Estimate throughput (assuming simulation has known steps)
        if hasattr(result, 'steps_completed'):
            throughput = result.steps_completed / simulation_time
        elif 'steps' in kwargs:
            throughput = kwargs['steps'] / simulation_time
        else:
            throughput = 1000 / simulation_time  # Default assumption
            
        latency = simulation_time / 1000 if simulation_time > 0 else 0
        
        # Store metrics
        self.metrics.add_sample(
            timing=simulation_time,
            memory=memory_delta,
            cpu=cpu_delta,
            throughput=throughput,
            latency=latency
        )
        
        run_result = {
            'config': self.config_name,
            'timestamp': datetime.now().isoformat(),
            'simulation_time': simulation_time,
            'total_time': total_time,
            'initial_memory_mb': initial_memory,
            'final_memory_mb': final_memory,
            'memory_delta_mb': memory_delta,
            'initial_cpu_percent': initial_cpu,
            'final_cpu_percent': final_cpu,
            'cpu_delta_percent': cpu_delta,
            'throughput': throughput,
            'latency': latency,
            'result': result
        }
        
        self.run_results.append(run_result)
        return run_result


class BenchmarkSuite:
    """Comprehensive benchmark suite for NLM brain simulations."""
    
    def __init__(self, output_dir: str = "benchmark_results"):
        """Initialize the benchmark suite.
        
        Args:
            output_dir: Directory to save benchmark results
        """
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        
        self.configurations = {}
        self.neuron_models = {}
        self.baseline_results = {}
        self.comparison_results = {}
        
    def add_configuration(self, name: str, simulation_func: callable, 
                         config_path: Optional[str] = None):
        """Add a configuration to benchmark.
        
        Args:
            name: Configuration name
            simulation_func: Simulation function to benchmark
            config_path: Path to configuration file (optional)
        """
        self.configurations[name] = {
            'function': simulation_func,
            'path': config_path,
            'type': 'configuration'
        }
        
    def add_neuron_model(self, name: str, simulation_func: callable,
                        model_params: Optional[Dict[str, Any]] = None):
        """Add a neuron model to benchmark.
        
        Args:
            name: Neuron model name
            simulation_func: Simulation function to benchmark
            model_params: Model parameters (optional)
        """
        self.neuron_models[name] = {
            'function': simulation_func,
            'params': model_params or {},
            'type': 'neuron_model'
        }
        
    def compare_configs(self, config_names: List[str], iterations: int = 5,
                       output_file: Optional[str] = None) -> Dict[str, Any]:
        """Compare different brain configurations.
        
        Args:
            config_names: List of configuration names to compare
            iterations: Number of iterations per configuration
            output_file: Output file path for results
            
        Returns:
            Comparison results dictionary
        """
        print(f"Comparing configurations: {config_names}")
        print(f"Iterations per configuration: {iterations}")
        
        comparison = {
            'configs': config_names,
            'iterations': iterations,
            'timestamp': datetime.now().isoformat(),
            'results': {},
            'summary': {}
        }
        
        # Run benchmarks for each configuration
        for config_name in config_names:
            print(f"\nBenchmarking {config_name}...")
            
            if config_name not in self.configurations:
                print(f"Warning: Configuration '{config_name}' not found")
                continue
                
            config = self.configurations[config_name]
            benchmark = ConfigurationBenchmark(config_name)
            
            for i in range(iterations):
                print(f"  Iteration {i + 1}/{iterations}")
                try:
                    result = benchmark.run_benchmark(config['function'])
                    if i == 0:  # Store full results for first iteration only
                        comparison['results'][config_name] = result
                except Exception as e:
                    print(f"    Error: {e}")
                    
            # Store benchmark summary
            comparison['summary'][config_name] = benchmark.metrics.get_summary()
            
        # Generate comparison report
        if output_file:
            self._save_comparison(comparison, output_file)
            
        self.comparison_results['config_comparison'] = comparison
        return comparison
    
    def benchmark_neuron_models(self, model_names: List[str], iterations: int = 5,
                               output_file: Optional[str] = None) -> Dict[str, Any]:
        """Benchmark different neuron models.
        
        Args:
            model_names: List of neuron model names to benchmark
            iterations: Number of iterations per model
            output_file: Output file path for results
            
        Returns:
            Neuron model comparison results
        """
        print(f"Benchmarking neuron models: {model_names}")
        print(f"Iterations per model: {iterations}")
        
        neuron_comparison = {
            'models': model_names,
            'iterations': iterations,
            'timestamp': datetime.now().isoformat(),
            'results': {},
            'summary': {}
        }
        
        # Run benchmarks for each neuron model
        for model_name in model_names:
            print(f"\nBenchmarking {model_name}...")
            
            if model_name not in self.neuron_models:
                print(f"Warning: Neuron model '{model_name}' not found")
                continue
                
            model = self.neuron_models[model_name]
            benchmark = ConfigurationBenchmark(model_name)
            
            for i in range(iterations):
                print(f"  Iteration {i + 1}/{iterations}")
                try:
                    result = benchmark.run_benchmark(model['function'])
                    if i == 0:  # Store full results for first iteration only
                        neuron_comparison['results'][model_name] = result
                except Exception as e:
                    print(f"    Error: {e}")
                    
            # Store benchmark summary
            neuron_comparison['summary'][model_name] = benchmark.metrics.get_summary()
            
        # Generate report
        if output_file:
            self._save_comparison(neuron_comparison, output_file)
            
        self.comparison_results['neuron_comparison'] = neuron_comparison
        return neuron_comparison
    
    def run_regression_test(self, baseline_file: str, new_config_names: List[str],
                           iterations: int = 5) -> Dict[str, Any]:
        """Run performance regression testing.
        
        Args:
            baseline_file: Path to baseline results file
            new_config_names: List of new configuration names to test
            iterations: Number of iterations per configuration
            
        Returns:
            Regression test results
        """
        print(f"Running regression test against baseline: {baseline_file}")
        
        # Load baseline results
        try:
            with open(baseline_file, 'r') as f:
                baseline_data = json.load(f)
            self.baseline_results = baseline_data
        except Exception as e:
            print(f"Error loading baseline: {e}")
            return {'status': 'failed', 'error': str(e)}
            
        regression_test = {
            'baseline_file': baseline_file,
            'new_configs': new_config_names,
            'iterations': iterations,
            'timestamp': datetime.now().isoformat(),
            'regression_analysis': {}
        }
        
        # Compare each configuration with baseline
        for config_name in new_config_names:
            print(f"\nAnalyzing regression for {config_name}...")
            
            # Run benchmark for new config
            new_benchmark = ConfigurationBenchmark(config_name)
            for i in range(iterations):
                result = new_benchmark.run_benchmark(self.configurations[config_name]['function'])
                
            new_summary = new_benchmark.metrics.get_summary()
            
            # Compare with baseline if available
            if config_name in baseline_data.get('summary', {}):
                baseline_summary = baseline_data['summary'][config_name]
                
                # Calculate regression metrics
                regression_analysis = self._analyze_regression(baseline_summary, new_summary)
                regression_test['regression_analysis'][config_name] = regression_analysis
                
                # Print warnings for significant regressions
                if regression_analysis.get('significant_regression', False):
                    print(f"  ⚠️  SIGNIFICANT REGRESSION DETECTED for {config_name}")
                    print(f"     Performance change: {regression_analysis.get('performance_change_percent', 0):.1f}%")
            else:
                regression_test['regression_analysis'][config_name] = {
                    'status': 'baseline_not_found',
                    'summary': new_summary
                }
                
        return regression_test
    
    def auto_benchmark(self, iterations: int = 5, output_dir: Optional[str] = None,
                      formats: List[str] = ['json', 'html', 'csv']) -> Dict[str, Any]:
        """Run automated benchmarking on all configured items.
        
        Args:
            iterations: Number of iterations per item
            output_dir: Output directory (uses instance output_dir if None)
            formats: Output formats to generate
            
        Returns:
            Complete automated benchmark results
        """
        output_dir = Path(output_dir or self.output_dir)
        output_dir.mkdir(exist_ok=True)
        
        print(f"Running automated benchmarking with {iterations} iterations per item")
        print(f"Output directory: {output_dir}")
        
        automated_results = {
            'timestamp': datetime.now().isoformat(),
            'iterations': iterations,
            'output_formats': formats,
            'config_results': {},
            'neuron_model_results': {},
            'summary': {}
        }
        
        # Benchmark all configurations
        for config_name in self.configurations.keys():
            print(f"\nAuto-benchmarking configuration: {config_name}")
            config_result = self.compare_configs([config_name], iterations=iterations)
            automated_results['config_results'][config_name] = config_result
            
        # Benchmark all neuron models
        for model_name in self.neuron_models.keys():
            print(f"\nAuto-benchmarking neuron model: {model_name}")
            model_result = self.benchmark_neuron_models([model_name], iterations=iterations)
            automated_results['neuron_model_results'][model_name] = model_result
            
        # Generate reports
        for fmt in formats:
            if fmt == 'json':
                output_file = output_dir / "automated_results.json"
                with open(output_file, 'w') as f:
                    json.dump(automated_results, f, indent=2, default=str)
                    
            elif fmt == 'html':
                output_file = output_dir / "automated_report.html"
                self._generate_automated_html_report(automated_results, output_file)
                
            elif fmt == 'csv':
                output_file = output_dir / "automated_results.csv"
                self._generate_automated_csv_report(automated_results, output_file)
                
        print(f"\nAutomated benchmarking complete. Results saved to: {output_dir}")
        return automated_results
    
    def generate_report(self, comparison: Dict[str, Any], output_file: str,
                       report_type: str = "comparison") -> str:
        """Generate comprehensive benchmark report.
        
        Args:
            comparison: Comparison results dictionary
            output_file: Output file path
            report_type: Type of report ('comparison', 'regression', 'summary')
            
        Returns:
            Path to generated report
        """
        if report_type == "comparison":
            self._generate_comparison_report(comparison, output_file)
        elif report_type == "regression":
            self._generate_regression_report(comparison, output_file)
        elif report_type == "summary":
            self._generate_summary_report(comparison, output_file)
            
        return output_file
    
    def _analyze_regression(self, baseline: Dict[str, Any], 
                          current: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze regression between baseline and current results."""
        regression = {
            'status': 'analyzed',
            'performance_changes': {},
            'significant_regression': False,
            'recommendations': []
        }
        
        # Compare key metrics
        baseline_time = baseline.get('timings', {}).get('mean', 0)
        current_time = current.get('timings', {}).get('mean', 0)
        
        baseline_memory = baseline.get('memory', {}).get('mean', 0)
        current_memory = current.get('memory', {}).get('mean', 0)
        
        baseline_throughput = baseline.get('throughput', {}).get('mean', 0)
        current_throughput = current.get('throughput', {}).get('mean', 0)
        
        # Calculate percentage changes
        if baseline_time > 0:
            time_change = (current_time - baseline_time) / baseline_time * 100
            regression['performance_changes']['time'] = {
                'baseline': baseline_time,
                'current': current_time,
                'change_percent': time_change,
                'regression': time_change > 5  # 5% threshold
            }
            
        if baseline_memory > 0:
            memory_change = (current_memory - baseline_memory) / baseline_memory * 100
            regression['performance_changes']['memory'] = {
                'baseline': baseline_memory,
                'current': current_memory,
                'change_percent': memory_change,
                'regression': memory_change > 10  # 10% threshold
            }
            
        if baseline_throughput > 0:
            throughput_change = (current_throughput - baseline_throughput) / baseline_throughput * 100
            regression['performance_changes']['throughput'] = {
                'baseline': baseline_throughput,
                'current': current_throughput,
                'change_percent': throughput_change,
                'regression': throughput_change < -5  # 5% degradation
            }
            
        # Determine if significant regression
        for metric_name, change_info in regression['performance_changes'].items():
            if change_info.get('regression', False):
                regression['significant_regression'] = True
                regression['recommendations'].append(
                    f"Address {metric_name} regression: {change_info['change_percent']:.1f}% change"
                )
                
        return regression
    
    def _save_comparison(self, comparison: Dict[str, Any], output_file: str):
        """Save comparison results to file."""
        output_path = Path(output_file)
        output_path.parent.mkdir(exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(comparison, f, indent=2, default=str)
            
        print(f"Comparison saved to: {output_path}")
    
    def _generate_comparison_report(self, comparison: Dict[str, Any], output_file: str):
        """Generate HTML comparison report."""
        html_content = self._create_comparison_html(comparison)
        with open(output_file, 'w') as f:
            f.write(html_content)
            
        print(f"Comparison report generated: {output_file}")
    
    def _generate_regression_report(self, regression: Dict[str, Any], output_file: str):
        """Generate HTML regression report."""
        html_content = self._create_regression_html(regression)
        with open(output_file, 'w') as f:
            f.write(html_content)
            
        print(f"Regression report generated: {output_file}")
    
    def _generate_summary_report(self, summary: Dict[str, Any], output_file: str):
        """Generate HTML summary report."""
        html_content = self._create_summary_html(summary)
        with open(output_file, 'w') as f:
            f.write(html_content)
            
        print(f"Summary report generated: {output_file}")
    
    def _create_comparison_html(self, comparison: Dict[str, Any]) -> str:
        """Create HTML content for comparison report."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Benchmark Comparison Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }}
        .header {{ background-color: #2c3e50; color: white; padding: 30px; border-radius: 5px; }}
        .section {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric-table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        .metric-table th, .metric-table td {{ border: 1px solid #ddd; padding: 12px; text-align: left; }}
        .metric-table th {{ background-color: #f2f2f2; }}
        .best {{ background-color: #d4edda; font-weight: bold; }}
        .regression {{ background-color: #f8d7da; }}
        .chart-container {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }}
        .summary-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }}
        .summary-card {{ background: #f8f9fa; padding: 20px; border-radius: 5px; border-left: 4px solid #007bff; }}
        .value {{ font-size: 24px; font-weight: bold; color: #007bff; }}
        .label {{ color: #6c757d; font-size: 14px; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Benchmark Comparison Report</h1>
        <p>Generated: {timestamp}</p>
        <p>Items Compared: {len(comparison.get('configs', []) + comparison.get('models', []))}</p>
        <p>Iterations per Item: {comparison.get('iterations', 1)}</p>
    </div>
    
    <div class="section">
        <h2>Overview</h2>
        <div class="summary-grid">
    """
        
        if 'summary' in comparison:
            for item_name, summary in comparison['summary'].items():
                avg_time = summary.get('timings', {}).get('mean', 0)
                avg_memory = summary.get('memory', {}).get('mean', 0)
                avg_throughput = summary.get('throughput', {}).get('mean', 0)
                
                html += f"""
            <div class="summary-card">
                <div class="value">{item_name}</div>
                <div class="label">Average Time: {avg_time:.3f}s</div>
                <div class="label">Average Memory: {avg_memory:.1f}MB</div>
                <div class="label">Average Throughput: {avg_throughput:.1f} ops/sec</div>
            </div>
                """
                
        html += """
        </div>
    </div>
    
    <div class="section">
        <h2>Detailed Results</h2>
    """
        
        # Create comparison tables
        if 'summary' in comparison:
            headers = ['Item', 'Avg Time (s)', 'Avg Memory (MB)', 'Avg Throughput', 'Time CV']
            html += f"""
        <table class="metric-table">
            <thead>
                <tr>
                    {''.join([f'<th>{h}</th>' for h in headers])}
                </tr>
            </thead>
            <tbody>
            """
            
            for item_name, summary in comparison['summary'].items():
                avg_time = summary.get('timings', {}).get('mean', 0)
                avg_memory = summary.get('memory', {}).get('mean', 0)
                avg_throughput = summary.get('throughput', {}).get('mean', 0)
                cv = summary.get('timings', {}).get('cv', 0)
                
                # Determine if this is the best performer
                is_best_time = False
                is_best_memory = False
                is_best_throughput = False
                
                if 'best_performers' in comparison:
                    if item_name in comparison['best_performers'].get('time', []):
                        is_best_time = True
                    if item_name in comparison['best_performers'].get('memory', []):
                        is_best_memory = True
                    if item_name in comparison['best_performers'].get('throughput', []):
                        is_best_throughput = True
                        
                row_class = " best" if (is_best_time or is_best_memory or is_best_throughput) else ""
                
                html += f"""
                <tr class="{row_class}">
                    <td><strong>{item_name}</strong></td>
                    <td>{avg_time:.3f}</td>
                    <td>{avg_memory:.1f}</td>
                    <td>{avg_throughput:.1f}</td>
                    <td>{cv:.3f}</td>
                </tr>
                """
                
            html += """
            </tbody>
        </table>
            """
            
        html += """
    </div>
    
    <div class="chart-container">
        <h2>Performance Charts</h2>
        <canvas id="comparisonChart" width="800" height="400"></canvas>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <script>
            const ctx = document.getElementById('comparisonChart').getContext('2d');
            new Chart(ctx, {
                type: 'bar',
                data: {
                    labels: ["""
        ]
        
        # Add data for charts
        if 'summary' in comparison:
            items = list(comparison['summary'].keys())
            avg_times = [comparison['summary'][item].get('timings', {}).get('mean', 0) for item in items]
            avg_memories = [comparison['summary'][item].get('memory', {}).get('mean', 0) for item in items]
            avg_throughputs = [comparison['summary'][item].get('throughput', {}).get('mean', 0) for item in items]
            
            for item in items:
                html += f"'{item}', "
                
            html = html[:-2] + """]',
                    datasets: [{
                        label: 'Average Time (s)',
                        data: ["""
            ]
            
            for time_val in avg_times:
                html += f"{time_val:.3f}, "
                
            html = html[:-2] + """],
                        backgroundColor: 'rgba(255, 99, 132, 0.2)',
                        borderColor: 'rgba(255, 99, 132, 1)',
                        borderWidth: 1
                    }, {
                        label: 'Average Memory (MB)',
                        data: ["""
            ]
            
            for mem_val in avg_memories:
                html += f"{mem_val:.1f}, "
                
            html = html[:-2] + """],
                        backgroundColor: 'rgba(54, 162, 235, 0.2)',
                        borderColor: 'rgba(54, 162, 235, 1)',
                        borderWidth: 1
                    }, {
                        label: 'Average Throughput',
                        data: ["""
            ]
            
            for throughput_val in avg_throughputs:
                html += f"{throughput_val:.1f}, "
                
            html = html[:-2] + """],
                        backgroundColor: 'rgba(75, 192, 192, 0.2)',
                        borderColor: 'rgba(75, 192, 192, 1)',
                        borderWidth: 1
                    }]
                },
                options: {
                    responsive: true,
                    plugins: {
                        title: { display: true, text: 'Configuration Comparison' }
                    },
                    scales: {
                        y: { beginAtZero: true, title: { display: true, text: 'Value' } },
                        x: { title: { display: true, text: 'Configuration' } }
                    }
                }
            });
        </script>
    </div>
    
</body>
</html>
        """
        
        return html
    
    def _generate_automated_html_report(self, results: Dict[str, Any], output_file: str):
        """Generate HTML report for automated benchmarking."""
        html_content = self._create_automated_html(results)
        with open(output_file, 'w') as f:
            f.write(html_content)
            
        print(f"Automated HTML report generated: {output_file}")
    
    def _generate_automated_csv_report(self, results: Dict[str, Any], output_file: str):
        """Generate CSV report for automated benchmarking."""
        df_data = []
        
        # Collect configuration results
        for config_name, config_result in results.get('config_results', {}).items():
            summary = config_result.get('summary', {})
            for item_name, item_summary in summary.items():
                df_data.append({
                    'Type': 'Configuration',
                    'Name': config_name,
                    'Item': item_name,
                    'Avg_Time': item_summary.get('timings', {}).get('mean', 0),
                    'Avg_Memory': item_summary.get('memory', {}).get('mean', 0),
                    'Avg_Throughput': item_summary.get('throughput', {}).get('mean', 0),
                    'Time_CV': item_summary.get('timings', {}).get('cv', 0)
                })
                
        # Collect neuron model results
        for model_name, model_result in results.get('neuron_model_results', {}).items():
            summary = model_result.get('summary', {})
            for item_name, item_summary in summary.items():
                df_data.append({
                    'Type': 'Neuron Model',
                    'Name': model_name,
                    'Item': item_name,
                    'Avg_Time': item_summary.get('timings', {}).get('mean', 0),
                    'Avg_Memory': item_summary.get('memory', {}).get('mean', 0),
                    'Avg_Throughput': item_summary.get('throughput', {}).get('mean', 0),
                    'Time_CV': item_summary.get('timings', {}).get('cv', 0)
                })
                
        # Create DataFrame and save
        df = pd.DataFrame(df_data)
        df.to_csv(output_file, index=False)
        
        print(f"Automated CSV report generated: {output_file}")
        
    def _create_automated_html(self, results: Dict[str, Any]) -> str:
        """Create HTML content for automated benchmark report."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Automated Benchmark Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }}
        .header {{ background-color: #28a745; color: white; padding: 30px; border-radius: 5px; }}
        .section {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }}
        .summary-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }}
        .summary-card {{ background: #f8f9fa; padding: 20px; border-radius: 5px; border-left: 4px solid #28a745; }}
        .value {{ font-size: 24px; font-weight: bold; color: #28a745; }}
        .label {{ color: #6c757d; font-size: 14px; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ border: 1px solid #ddd; padding: 12px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .best {{ background-color: #d4edda; font-weight: bold; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Automated Benchmark Report</h1>
        <p>Generated: {timestamp}</p>
        <p>Iterations: {results.get('iterations', 1)}</p>
        <p>Output Formats: {', '.join(results.get('output_formats', []))}</p>
    </div>
    
    <div class="section">
        <h2>Summary</h2>
        <div class="summary-grid">
    """
        
        # Summary statistics
        config_count = len(results.get('config_results', {}))
        model_count = len(results.get('neuron_model_results', {}))
        
        html += f"""
            <div class="summary-card">
                <div class="value">{config_count}</div>
                <div class="label">Configurations Benchmarked</div>
            </div>
            <div class="summary-card">
                <div class="value">{model_count}</div>
                <div class="label">Neuron Models Benchmarked</div>
            </div>
            <div class="value">{results.get('iterations', 1)}</div>
            <div class="label">Iterations per Item</div>
        </div>
    </div>
    
    <div class="section">
        <h2>Configuration Benchmark Results</h2>
        <table>
            <thead>
                <tr>
                    <th>Configuration</th>
                    <th>Average Time (s)</th>
                    <th>Average Memory (MB)</th>
                    <th>Average Throughput</th>
                    <th>Performance CV</th>
                </tr>
            </thead>
            <tbody>
    """
        
        # Configuration results
        for config_name, config_result in results.get('config_results', {}).items():
            summary = config_result.get('summary', {})
            for item_name, item_summary in summary.items():
                avg_time = item_summary.get('timings', {}).get('mean', 0)
                avg_memory = item_summary.get('memory', {}).get('mean', 0)
                avg_throughput = item_summary.get('throughput', {}).get('mean', 0)
                cv = item_summary.get('timings', {}).get('cv', 0)
                
                html += f"""
                <tr>
                    <td><strong>{config_name}</strong></td>
                    <td>{avg_time:.3f}</td>
                    <td>{avg_memory:.1f}</td>
                    <td>{avg_throughput:.1f}</td>
                    <td>{cv:.3f}</td>
                </tr>
                """
                
        html += """
            </tbody>
        </table>
    </div>
    
    <div class="section">
        <h2>Neuron Model Benchmark Results</h2>
        <table>
            <thead>
                <tr>
                    <th>Neuron Model</th>
                    <th>Average Time (s)</th>
                    <th>Average Memory (MB)</th>
                    <th>Average Throughput</th>
                    <th>Performance CV</th>
                </tr>
            </thead>
            <tbody>
    """
        
        # Neuron model results
        for model_name, model_result in results.get('neuron_model_results', {}).items():
            summary = model_result.get('summary', {})
            for item_name, item_summary in summary.items():
                avg_time = item_summary.get('timings', {}).get('mean', 0)
                avg_memory = item_summary.get('memory', {}).get('mean', 0)
                avg_throughput = item_summary.get('throughput', {}).get('mean', 0)
                cv = item_summary.get('timings', {}).get('cv', 0)
                
                html += f"""
                <tr>
                    <td><strong>{model_name}</strong></td>
                    <td>{avg_time:.3f}</td>
                    <td>{avg_memory:.1f}</td>
                    <td>{avg_throughput:.1f}</td>
                    <td>{cv:.3f}</td>
                </tr>
                """
                
        html += """
            </tbody>
        </table>
    </div>
    
</body>
</html>
        """
        
        return html


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Brain Simulation Benchmark Suite")
    parser.add_argument("--auto", action="store_true", help="Run automated benchmarking")
    parser.add_argument("--configs", nargs="+", help="List of configurations to benchmark")
    parser.add_argument("--neuron-models", nargs="+", help="List of neuron models to benchmark")
    parser.add_argument("--regression", action="store_true", help="Run performance regression testing")
    parser.add_argument("--baseline", help="Baseline results file for regression testing")
    parser.add_argument("--iterations", type=int, default=5, help="Number of iterations per benchmark")
    parser.add_argument("--output-dir", help="Output directory for results")
    parser.add_argument("--report", action="store_true", help="Generate reports")
    parser.add_argument("--format", choices=["html", "json", "csv"], default="html", help="Report format")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    # Create benchmark suite
    suite = BenchmarkSuite(args.output_dir or "benchmark_results")
    
    # Load mock simulation functions for demonstration
    def mock_simulation(config_name: str, **kwargs) -> Dict[str, Any]:
        """Mock simulation function."""
        import time
        import random
        
        # Simulate variable workload
        base_time = random.uniform(0.1, 0.5)
        time.sleep(base_time)
        
        return {
            'config': config_name,
            'timestamp': datetime.now().isoformat(),
            'steps_completed': random.randint(800, 1200),
            'iterations': kwargs.get('iterations', 1),
            'config_params': kwargs
        }
        
    def mock_neuron_simulation(model_name: str, **kwargs) -> Dict[str, Any]:
        """Mock neuron simulation function."""
        import time
        import random
        
        # Simulate variable workload based on model complexity
        complexity = len(str(kwargs.get('params', {})))
        base_time = random.uniform(0.05 + complexity * 0.01, 0.3 + complexity * 0.02)
        time.sleep(base_time)
        
        return {
            'model': model_name,
            'timestamp': datetime.now().isoformat(),
            'steps_completed': random.randint(1000, 2000),
            'iterations': kwargs.get('iterations', 1),
            'model_params': kwargs
        }
        
    # Add mock configurations
    suite.add_configuration("baseline_config", mock_simulation)
    suite.add_configuration("optimized_config", lambda **kwargs: mock_simulation("optimized_config", **kwargs))
    suite.add_configuration("advanced_config", lambda **kwargs: mock_simulation("advanced_config", **kwargs))
    
    # Add mock neuron models
    suite.add_neuron_model("standard_neuron", mock_neuron_simulation)
    suite.add_neuron_model("enhanced_neuron", lambda **kwargs: mock_neuron_simulation("enhanced_neuron", **kwargs))
    suite.add_neuron_model("optimized_neuron", lambda **kwargs: mock_neuron_simulation("optimized_neuron", **kwargs))
    
    # Run benchmarks based on arguments
    if args.auto:
        print("Running automated benchmarking...")
        suite.auto_benchmark(
            iterations=args.iterations,
            output_dir=args.output_dir,
            formats=[args.format]
        )
    elif args.configs:
        print(f"Comparing configurations: {args.configs}")
        output_file = f"{args.output_dir or 'benchmark_results'}/{args.format}_comparison.{args.format}"
        suite.compare_configs(args.configs, iterations=args.iterations, output_file=output_file)
    elif args.neuron_models:
        print(f"Benchmarking neuron models: {args.neuron_models}")
        output_file = f"{args.output_dir or 'benchmark_results'}/{args.format}_neuron_models.{args.format}"
        suite.benchmark_neuron_models(args.neuron_models, iterations=args.iterations, output_file=output_file)
    elif args.regression and args.baseline:
        print(f"Running regression test against baseline: {args.baseline}")
        regression_result = suite.run_regression_test(args.baseline, args.configs or [], iterations=args.iterations)
        print(f"Regression test complete")
    else:
        print("Benchmark suite requires --auto, --configs, or --neuron-models")
        print("Example usage:")
        print("  python benchmark_suite.py --auto")
        print("  python benchmark_suite.py --configs baseline_config optimized_config")
        print("  python benchmark_suite.py --neuron-models standard_neuron enhanced_neuron")
        print("  python benchmark_suite.py --regression --baseline baseline.json --configs new_config1 new_config2")
        
    print("\nBenchmarking complete!")