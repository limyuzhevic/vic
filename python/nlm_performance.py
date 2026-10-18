# Performance Profiling and Benchmarking
# Advanced performance monitoring and analysis tools for NLM

import time
import json
import statistics
import numpy as np
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, asdict
from enum import Enum
import threading
import multiprocessing

class PerformanceMetric:
    """Data class for performance metrics."""
    
    def __init__(self, name: str, value: float, unit: str = "", description: str = ""):
        self.name = name
        self.value = value
        self.unit = unit
        self.description = description
        self.timestamp = time.time()
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'value': self.value,
            'unit': self.unit,
            'description': self.description,
            'timestamp': self.timestamp
        }

@dataclass
class BenchmarkResult:
    """Results from a benchmark test."""
    test_name: str
    steps: int
    repetitions: int
    mean_time: float
    time_std: float
    time_min: float
    time_max: float
    steps_per_second: float
    cpu_usage: float
    memory_usage: float
    metrics: List[PerformanceMetric]
    additional_data: Dict[str, Any]

class PerformanceProfiler:
    """
    Performance profiler for NLM simulations.
    
    Collects and analyzes performance metrics for NLM simulations,
    including timing, memory usage, CPU usage, and neural activity statistics.
    """
    
    def __init__(self):
        self.current_session = None
        self.benchmark_history = []
        self.profiling_enabled = False
        self.sampling_rate = 0.1  # Hz
        self.max_memory_samples = 1000
        self.memory_samples = []
        self.time_samples = []
        self.cpu_samples = []
        
    def start_session(self, session_name: str, description: str = ""):
        """Start a new profiling session."""
        self.current_session = {
            'name': session_name,
            'description': description,
            'start_time': time.time(),
            'end_time': None,
            'metrics': [],
            'memory_samples': [],
            'time_samples': [],
            'cpu_samples': []
        }
        self.profiling_enabled = True
        print(f"Performance profiling session started: {session_name}")
        
    def stop_session(self) -> Dict[str, Any]:
        """Stop the current profiling session and return results."""
        if not self.current_session:
            return {'error': 'No active profiling session'}
        
        self.current_session['end_time'] = time.time()
        self.profiling_enabled = False
        
        # Calculate summary statistics
        results = self._analyze_session(self.current_session)
        
        # Store in history
        self.benchmark_history.append(results)
        
        session_name = self.current_session['name']
        print(f"Performance profiling session completed: {session_name}")
        
        # Reset for next session
        self.current_session = None
        
        return results
    
    def record_metric(self, name: str, value: float, unit: str = "", description: str = ""):
        """Record a performance metric."""
        if self.profiling_enabled and self.current_session:
            metric = PerformanceMetric(name, value, unit, description)
            self.current_session['metrics'].append(asdict(metric))
            self.current_session['time_samples'].append(time.time())
    
    def record_memory_usage(self):
        """Record current memory usage (placeholder for actual implementation)."""
        if self.profiling_enabled and self.current_session:
            # Placeholder - actual memory tracking would use psutil or similar
            try:
                import psutil
                process = psutil.Process()
                memory_info = process.memory_info()
                self.current_session['memory_samples'].append({
                    'rss': memory_info.rss,
                    'vms': memory_info.vms,
                    'timestamp': time.time()
                })
            except ImportError:
                # Fallback: use artificial data if psutil is not available
                self.current_session['memory_samples'].append({
                    'rss': len(str(self.current_session)) * 1024,  # Rough estimate
                    'vms': 0,
                    'timestamp': time.time()
                })
    
    def record_cpu_usage(self):
        """Record current CPU usage (placeholder for actual implementation)."""
        if self.profiling_enabled and self.current_session:
            # Placeholder - actual CPU tracking would use psutil
            try:
                import psutil
                cpu_percent = psutil.cpu_percent()
                self.current_session['cpu_samples'].append({
                    'cpu_percent': cpu_percent,
                    'timestamp': time.time()
                })
            except ImportError:
                # Fallback: use artificial data
                self.current_session['cpu_samples'].append({
                    'cpu_percent': 50.0,  # Average estimate
                    'timestamp': time.time()
                })
    
    def run_benchmark(self, test_name: str, 
                     benchmark_func: Callable,
                     benchmark_args: tuple = (),
                     repetitions: int = 5,
                     collect_memory: bool = True,
                     collect_cpu: bool = True) -> BenchmarkResult:
        """
        Run a benchmark and collect performance metrics.
        
        Args:
            test_name: Name of the benchmark test
            benchmark_func: Function to benchmark
            benchmark_args: Arguments to pass to benchmark function
            repetitions: Number of repetitions
            collect_memory: Whether to collect memory usage
            collect_cpu: Whether to collect CPU usage
            
        Returns:
            BenchmarkResult object with detailed performance metrics
        """
        print(f"Running benchmark: {test_name} ({repetitions} repetitions)")
        
        # Start profiling session
        self.start_session(f"benchmark_{test_name}")
        
        # Warm-up runs (to stabilize performance)
        for i in range(2):
            self.record_metric("benchmark_iteration", i, "iteration", "Benchmark iteration")
            try:
                benchmark_func(*benchmark_args)
            except Exception as e:
                print(f"Warning: Benchmark iteration {i} failed: {e}")
        
        # Clear any accumulated metrics from warm-up
        self.current_session['metrics'] = []
        self.current_session['memory_samples'] = []
        self.current_session['cpu_samples'] = []
        
        # Actual benchmark runs
        times = []
        rewards = []
        firing_rates = []
        
        for i in range(repetitions):
            iteration_start = time.time()
            
            # Record metrics for each repetition
            self.record_metric("repetition", i, "repetition", f"Benchmark repetition {i}")
            
            if collect_memory:
                self.record_memory_usage()
                
            if collect_cpu:
                self.record_cpu_usage()
            
            # Run the benchmark function
            try:
                result = benchmark_func(*benchmark_args)
                
                # Extract metrics from result if available
                iteration_end = time.time()
                iteration_time = iteration_end - iteration_start
                times.append(iteration_time)
                
                if isinstance(result, dict):
                    rewards.append(result.get('total_reward', 0.0))
                    firing_rates.append(result.get('average_firing_rate', 0.0))
                else:
                    rewards.append(0.0)
                    firing_rates.append(0.0)
                    
            except Exception as e:
                print(f"Warning: Benchmark repetition {i} failed: {e}")
                times.append(float('inf'))  # Mark failed run
        
        # Stop session and get results
        results = self.stop_session()
        
        # Calculate benchmark statistics
        valid_times = [t for t in times if t != float('inf')]
        
        benchmark_result = BenchmarkResult(
            test_name=test_name,
            steps=len(benchmark_args) > 0 and benchmark_args[0] or 0,
            repetitions=repetitions,
            mean_time=statistics.mean(valid_times) if valid_times else 0.0,
            time_std=statistics.stdev(valid_times) if len(valid_times) > 1 else 0.0,
            time_min=min(valid_times) if valid_times else 0.0,
            time_max=max(valid_times) if valid_times else 0.0,
            steps_per_second=valid_times[0] / len(valid_times) if valid_times else 0.0,
            cpu_usage=statistics.mean([sample['cpu_percent'] for sample in results.get('cpu_samples', [])]) if results.get('cpu_samples') else 0.0,
            memory_usage=results.get('memory_samples', [{}])[-1].get('rss', 0) if results.get('memory_samples') else 0,
            metrics=[],  # Would collect from NLM interface
            additional_data={
                'reward_mean': np.mean(rewards) if rewards else 0.0,
                'reward_std': np.std(rewards) if len(rewards) > 1 else 0.0,
                'firing_rate_mean': np.mean(firing_rates) if firing_rates else 0.0,
                'firing_rate_std': np.std(firing_rates) if len(firing_rates) > 1 else 0.0,
                'success_rate': len(valid_times) / repetitions if repetitions > 0 else 0.0
            }
        )
        
        return benchmark_result
    
    def compare_benchmarks(self, benchmark1: BenchmarkResult, benchmark2: BenchmarkResult) -> Dict[str, Any]:
        """Compare two benchmark results and provide analysis."""
        comparison = {
            'benchmark1': benchmark1.test_name,
            'benchmark2': benchmark2.test_name,
            'performance_comparison': {},
            'recommendations': []
        }
        
        # Compare timing performance
        time_improvement = (benchmark1.mean_time - benchmark2.mean_time) / benchmark1.mean_time * 100
        comparison['performance_comparison']['timing_improvement_percent'] = time_improvement
        
        # Compare rewards
        reward_improvement = (benchmark2.additional_data.get('reward_mean', 0) - benchmark1.additional_data.get('reward_mean', 0)) / benchmark1.additional_data.get('reward_mean', 1) * 100
        comparison['performance_comparison']['reward_improvement_percent'] = reward_improvement
        
        # Generate recommendations
        if time_improvement > 10:
            comparison['recommendations'].append(
                f"Benchmark {benchmark2.test_name} is {time_improvement:.1f}% faster than {benchmark1.test_name}"
            )
        
        if benchmark2.additional_data.get('success_rate', 0) > benchmark1.additional_data.get('success_rate', 0):
            comparison['recommendations'].append(
                f"Benchmark {benchmark2.test_name} has higher success rate ({benchmark2.additional_data.get('success_rate', 0)*100:.1f}% vs {benchmark1.additional_data.get('success_rate', 0)*100:.1f}%)"
            )
        
        return comparison
    
    def generate_performance_report(self, benchmark_results: List[BenchmarkResult]) -> Dict[str, Any]:
        """Generate a comprehensive performance report."""
        report = {
            'summary': {
                'total_benchmarks': len(benchmark_results),
                'total_repetitions': sum(benchmark.steps * benchmark.repetitions for benchmark in benchmark_results),
                'date_generated': time.time(),
                'report_version': '1.0'
            },
            'benchmarks': [],
            'comparisons': [],
            'recommendations': []
        }
        
        # Add individual benchmark results
        for benchmark in benchmark_results:
            report['benchmarks'].append({
                'test_name': benchmark.test_name,
                'configuration': {
                    'steps': benchmark.steps,
                    'repetitions': benchmark.repetitions
                },
                'performance': {
                    'timing': {
                        'mean_time_seconds': benchmark.mean_time,
                        'time_std_seconds': benchmark.time_std,
                        'time_min_seconds': benchmark.time_min,
                        'time_max_seconds': benchmark.time_max,
                        'steps_per_second': benchmark.steps_per_second
                    },
                    'cpu_usage_percent': benchmark.cpu_usage,
                    'memory_usage_bytes': benchmark.memory_usage
                },
                'neural_performance': {
                    'reward_mean': benchmark.additional_data.get('reward_mean', 0.0),
                    'reward_std': benchmark.additional_data.get('reward_std', 0.0),
                    'firing_rate_mean': benchmark.additional_data.get('firing_rate_mean', 0.0),
                    'firing_rate_std': benchmark.additional_data.get('firing_rate_std', 0.0),
                    'success_rate': benchmark.additional_data.get('success_rate', 0.0)
                }
            })
        
        # Generate comparisons between similar benchmarks
        for i, benchmark1 in enumerate(benchmark_results):
            for j, benchmark2 in enumerate(benchmark_results[i+1:], i+1):
                if benchmark1.test_name != benchmark2.test_name:
                    comparison = self.compare_benchmarks(benchmark1, benchmark2)
                    report['comparisons'].append(comparison)
        
        # Generate overall recommendations
        if len(benchmark_results) > 1:
            fastest = min(benchmark_results, key=lambda x: x.mean_time)
            highest_reward = max(benchmark_results, key=lambda x: x.additional_data.get('reward_mean', 0))
            
            report['recommendations'].extend([
                f"For best performance: Use {fastest.test_name} (fastest: {fastest.mean_time:.3f}s)",
                f"For best learning: Use {highest_reward.test_name} (highest reward: {highest_reward.additional_data.get('reward_mean', 0):.3f})"
            ])
        
        return report
    
    def _analyze_session(self, session: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze a profiling session and return summary statistics."""
        analysis = {
            'session_name': session['name'],
            'duration': session['end_time'] - session['start_time'] if session['end_time'] else time.time() - session['start_time'],
            'metric_count': len(session['metrics']),
            'memory_samples_count': len(session['memory_samples']),
            'cpu_samples_count': len(session['cpu_samples']),
            'start_time': session['start_time'],
            'end_time': session['end_time']
        }
        
        # Analyze memory samples
        if session['memory_samples']:
            memory_rss_values = [sample.get('rss', 0) for sample in session['memory_samples']]
            analysis['memory_usage'] = {
                'mean_rss_bytes': np.mean(memory_rss_values),
                'max_rss_bytes': np.max(memory_rss_values),
                'min_rss_bytes': np.min(memory_rss_values)
            }
        
        # Analyze CPU samples
        if session['cpu_samples']:
            cpu_values = [sample.get('cpu_percent', 0) for sample in session['cpu_samples']]
            analysis['cpu_usage'] = {
                'mean_percent': np.mean(cpu_values),
                'max_percent': np.max(cpu_values),
                'min_percent': np.min(cpu_values)
            }
        
        # Analyze time metrics
        if session['metrics']:
            analysis['time_metrics'] = session['metrics']
        
        return analysis


# Convenience functions for common benchmarking scenarios

def benchmark_nlm_simulation(neuron_counts: List[int] = None,
                            step_counts: List[int] = None,
                            repetitions: int = 3) -> Dict[str, Any]:
    """
    Benchmark NLM simulation performance with different configurations.
    
    Args:
        neuron_counts: List of neuron counts to test
        step_counts: List of step counts to test  
        repetitions: Number of repetitions for each configuration
        
    Returns:
        Dictionary with benchmark suite results
    """
    if neuron_counts is None:
        neuron_counts = [100, 500, 1000]
    if step_counts is None:
        step_counts = [100, 500, 1000]
    
    profiler = PerformanceProfiler()
    
    # This would require an NLM interface function to benchmark
    # For now, return a placeholder structure
    benchmark_suite = {}
    
    for neurons in neuron_counts:
        for steps in step_counts:
            test_name = f"simulation_{neurons}_neurons_{steps}_steps"
            
            # Placeholder benchmark function
            def benchmark_simulation(neuron_count, step_count):
                """Placeholder simulation for benchmarking."""
                start_time = time.time()
                # Simulate some work
                time.sleep(0.01 * step_count / 1000)  # Rough approximation
                end_time = time.time()
                
                return {
                    'total_reward': (neuron_count + step_count) / 1000,
                    'average_firing_rate': 5.0 + neuron_count / 200,
                    'duration': end_time - start_time
                }
            
            # Run benchmark
            benchmark_result = profiler.run_benchmark(
                test_name=test_name,
                benchmark_func=benchmark_simulation,
                benchmark_args=(neurons, steps),
                repetitions=repetitions,
                collect_memory=True,
                collect_cpu=True
            )
            
            benchmark_suite[test_name] = benchmark_result.to_dict() if hasattr(benchmark_result, 'to_dict') else benchmark_result
    
    return benchmark_suite


def profile_nlm_function(func: Callable, *args, **kwargs) -> Dict[str, Any]:
    """
    Profile a single NLM function call.
    
    Args:
        func: Function to profile
        *args: Arguments to pass to function
        **kwargs: Keyword arguments to pass to function
        
    Returns:
        Dictionary with profiling results
    """
    profiler = PerformanceProfiler()
    
    def profiled_function(*f_args, **f_kwargs):
        profiler.start_session("function_profiling", f"Profiling {func.__name__}")
        profiler.record_metric("call_start", time.time(), "timestamp", "Function call start time")
        
        result = func(*f_args, **f_kwargs)
        
        profiler.record_metric("call_end", time.time(), "timestamp", "Function call end time")
        profiler.record_memory_usage()
        profiler.record_cpu_usage()
        
        profiling_results = profiler.stop_session()
        
        return result, profiling_results
    
    # Execute function with profiling
    result, profiling_results = profiled_function(*args, **kwargs)
    
    return {
        'result': result,
        'profiling': profiling_results
    }


def generate_performance_analysis_report(neural_data: Dict[str, Any],
                                        simulation_results: Dict[str, Any]) -> Dict[str, Any]:
    """
    Generate comprehensive performance analysis report.
    
    Args:
        neural_data: Neural activity data
        simulation_results: Simulation results
        
    Returns:
        Comprehensive analysis report
    """
    analysis_report = {
        'timestamp': time.time(),
        'analysis_type': 'comprehensive',
        'neural_data_summary': {
            'total_neurons': neural_data.get('total_neurons', 0),
            'total_synapses': neural_data.get('total_synapses', 0),
            'simulation_time': neural_data.get('simulation_time', 0)
        },
        'performance_metrics': {
            'efficiency': simulation_results.get('total_reward', 0) / max(1, simulation_results.get('duration', 1)),
            'stability': simulation_results.get('firing_rate_variance', 0),
            'adaptability': simulation_results.get('adaptation_score', 0)
        },
        'optimization_recommendations': [],
        'performance_trends': [],
        'quality_assessment': {
            'overall_score': 0.0,
            'component_scores': {}
        }
    }
    
    # Generate optimization recommendations based on data
    if analysis_report['neural_data_summary']['total_neurons'] > 500:
        analysis_report['optimization_recommendations'].append(
            "Consider reducing neuron count for faster simulations with minimal performance impact"
        )
    
    if analysis_report['performance_metrics']['efficiency'] < 0.1:
        analysis_report['optimization_recommendations'].append(
            "Simulation efficiency is low - consider optimizing synaptic connectivity"
        )
    
    # Calculate overall score
    total_score = 0.0
    component_weights = {
        'efficiency': 0.4,
        'stability': 0.3,
        'adaptability': 0.3
    }
    
    for component, weight in component_weights.items():
        if component in analysis_report['performance_metrics']:
            total_score += analysis_report['performance_metrics'][component] * weight
    
    analysis_report['quality_assessment']['overall_score'] = total_score
    
    return analysis_report


# Export key classes and functions
__all__ = [
    'PerformanceProfiler',
    'BenchmarkResult',
    'PerformanceMetric',
    'benchmark_nlm_simulation',
    'profile_nlm_function',
    'generate_performance_analysis_report'
]
