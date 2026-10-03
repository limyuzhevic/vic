"""
NLM Profiling and Benchmarking Module

Advanced profiling tools for NLM simulations with support for:
- Performance timing and measurement
- Memory profiling
- CPU profiling
- Benchmark testing
- Comparative analysis
- Performance regression detection
- Resource usage monitoring
"""

import time
import psutil
import threading
import numpy as np
import pandas as pd
import json
from typing import List, Dict, Optional, Tuple, Union, Any, Callable
from pathlib import Path
import warnings
from dataclasses import dataclass
from enum import Enum
import tracemalloc
import cProfile
import pstats
from io import StringIO

class ProfilingMode(Enum):
    """Profiling modes."""
    BASIC = "basic"
    DETAILED = "detailed"
    CPU = "cpu"
    MEMORY = "memory"
    CUSTOM = "custom"
class PerformanceMetrics:
    """
    Class: 
    Class: 
    Class: Container for performance metrics.
    
    
    """""""""
    
    def __init__(self):
        self.timings: List[Dict[str, float]] = []
        self.memory_samples: List[Dict[str, float]] = []
        self.cpu_samples: List[Dict[str, float]] = []
        self.system_samples: List[Dict[str, float]] = []
        
    def add_timing(self, operation: str, duration: float, 
                   metadata: Optional[Dict] = None) -> Any:
    """
    Function: Add timing measurement.
    """""
        self.timings.append({
            'operation': operation,
            'duration_ms': duration * 1000,
            'timestamp': time.time(),
            'metadata': metadata or {}
        })
        
    def add_memory_sample(self, operation: str, memory_mb: float,
                         metadata: Optional[Dict] = None) -> Any:
    """
    Function: Add memory measurement.
    """""
        self.memory_samples.append({
            'operation': operation,
            'memory_mb': memory_mb,
            'timestamp': time.time(),
            'metadata': metadata or {}
        })
        
    def add_cpu_sample(self, operation: str, cpu_percent: float,
                      metadata: Optional[Dict] = None) -> Any:
    """
    Function: Add CPU measurement.
    """""
        self.cpu_samples.append({
            'operation': operation,
            'cpu_percent': cpu_percent,
            'timestamp': time.time(),
            'metadata': metadata or {}
        })
        
    def add_system_sample(self, memory_percent: float, cpu_percent: float,
                         metadata: Optional[Dict] = None) -> Any:
    """
    Function: Add system resource measurement.
    """""
        self.system_samples.append({
            'memory_percent': memory_percent,
            'cpu_percent': cpu_percent,
            'timestamp': time.time(),
            'metadata': metadata or {}
        })
        
    def to_dataframe(self) -> pd.DataFrame:
        """Convert metrics to pandas DataFrame."""
        if self.timings:
            timings_df = pd.DataFrame(self.timings)
        else:
            timings_df = pd.DataFrame(columns=['operation', 'duration_ms', 'timestamp', 'metadata'])
            
        if self.memory_samples:
            memory_df = pd.DataFrame(self.memory_samples)
        else:
            memory_df = pd.DataFrame(columns=['operation', 'memory_mb', 'timestamp', 'metadata'])
            
        if self.cpu_samples:
            cpu_df = pd.DataFrame(self.cpu_samples)
        else:
            cpu_df = pd.DataFrame(columns=['operation', 'cpu_percent', 'timestamp', 'metadata'])
            
        if self.system_samples:
            system_df = pd.DataFrame(self.system_samples)
        else:
            system_df = pd.DataFrame(columns=['memory_percent', 'cpu_percent', 'timestamp', 'metadata'])
            
        return {
            'timings': timings_df,
            'memory': memory_df,
            'cpu': cpu_df,
            'system': system_df
        }
class NLMProfiler:
    """
    Class: 
    Class: 
    Class: Main profiler for NLM simulations.
    
    
    """""""""
    
    def __init__(self, mode: ProfilingMode = ProfilingMode.BASIC):
        self.mode = mode
        self.metrics = PerformanceMetrics()
        self.is_recording = False
        self.profiler_thread = None
        self.recording_interval = 1.0  # seconds
        
        if mode == ProfilingMode.CPU:
            self.cpu_profiler = cProfile.Profile()
        elif mode == ProfilingMode.MEMORY:
            tracemalloc.start()
            
    def start_profiling(self) -> Any:
    """
    Function: Start profiling.
    """""
        self.is_recording = True
        
        if self.mode == ProfilingMode.CPU:
            self.cpu_profiler.enable()
            
        # Start system monitoring thread
        self.profiler_thread = threading.Thread(target=self._monitor_system_resources)
        self.profiler_thread.daemon = True
        self.profiler_thread.start()
        
    def stop_profiling(self) -> Any:
    """
    Function: Stop profiling.
    """""
        self.is_recording = False
        
        if self.mode == ProfilingMode.CPU:
            self.cpu_profiler.disable()
            
        if self.mode == ProfilingMode.MEMORY:
            current, peak = tracemalloc.get_traced_memory()
            self.add_memory_sample("total_allocation", current / 1024 / 1024)
            tracemalloc.stop()
            
        if self.profiler_thread:
            self.profiler_thread.join(timeout=5.0)
            
    def profile_function(self, func: Callable, *args, **kwargs) -> Any:
        """Profile a single function call."""
        if self.mode == ProfilingMode.CPU:
            self.cpu_profiler.enable()
            
        start_time = time.time()
        
        # Start memory tracking if needed
        if self.mode == ProfilingMode.MEMORY:
            tracemalloc.start()
            
        try:
            result = func(*args, **kwargs)
            
            # Record performance
            duration = time.time() - start_time
            self.add_timing(func.__name__, duration)
            
            # Record memory usage
            if self.mode == ProfilingMode.MEMORY:
                current, _ = tracemalloc.get_traced_memory()
                self.add_memory_sample(f"{func.__name__}_memory", current / 1024 / 1024)
                tracemalloc.stop()
                
            return result
            
        except Exception as e:
            duration = time.time() - start_time
            self.add_timing(f"{func.__name__}_error", duration)
            raise
            
        finally:
            if self.mode == ProfilingMode.CPU:
                self.cpu_profiler.disable()
                
    def _monitor_system_resources(self) -> Any:
    """
    Function: Monitor system resources in background thread.
    """""
        while self.is_recording:
            # Get system info
            process = psutil.Process()
            memory_info = process.memory_info()
            cpu_percent = process.cpu_percent()
            
            # Add samples
            self.add_system_sample(
                memory_info.rss / 1024 / 1024,  # MB
                cpu_percent,
                {'thread': threading.current_thread().name}
            )
            
            time.sleep(self.recording_interval)
            
    def profile_simulation_step(self, step_number: int,
                               operation: str = "simulation_step",
                               **kwargs) -> Dict[str, float]:
        """Profile a single simulation step."""
        metrics = {}
        
        # Profile step execution
        start_time = time.time()
        
        # Add system samples before step
        process = psutil.Process()
        memory_before = process.memory_info().rss / 1024 / 1024
        cpu_before = process.cpu_percent()
        
        # Execute step (would integrate with actual NLM simulation)
        # This is a placeholder for actual simulation step profiling
        time.sleep(0.001)  # Simulate step execution
        
        # Measure after step
        duration = time.time() - start_time
        memory_after = process.memory_info().rss / 1024 / 1024
        cpu_after = process.cpu_percent()
        
        # Calculate metrics
        memory_delta = memory_after - memory_before
        cpu_avg = (cpu_before + cpu_after) / 2
        
        metrics.update({
            'step': step_number,
            'execution_time_ms': duration * 1000,
            'memory_delta_mb': memory_delta,
            'cpu_avg_percent': cpu_avg,
        })
        
        # Add to profiler metrics
        self.add_timing(operation, duration, {'step': step_number})
        self.add_memory_sample(operation, memory_after, {'step': step_number})
        self.add_cpu_sample(operation, cpu_avg, {'step': step_number})
        
        return metrics
    
    def run_benchmark(self, simulation_func: Callable, 
                     steps: int, warmup_steps: int = 10,
                     description: str = "NLM Benchmark") -> Dict[str, Any]:
        """Run comprehensive benchmark."""
        # print(f"Running benchmark: {description}")
        # print(f"  Steps: {steps}, Warmup: {warmup_steps}")
        
        # Clear previous metrics
        self.metrics = PerformanceMetrics()
        
        # Warmup phase
        # print("  Warming up...")
        for step in range(warmup_steps):
            self.profile_simulation_step(step, "warmup")
            
        # Clear metrics from warmup
        self.metrics.timings = [m for m in self.metrics.timings if m['operation'] != 'warmup']
        self.metrics.memory_samples = [m for m in self.metrics.memory_samples if m['operation'] != 'warmup']
        self.metrics.cpu_samples = [m for m in self.metrics.cpu_samples if m['operation'] != 'warmup']
        
        # Benchmark phase
        # print(f"  Running {steps} steps...")
        step_metrics = []
        
        for step in range(steps):
            metrics = self.profile_simulation_step(step, "simulation")
            step_metrics.append(metrics)
            
        # Calculate summary statistics
        execution_times = [m['execution_time_ms'] for m in step_metrics]
        memory_deltas = [m['memory_delta_mb'] for m in step_metrics]
        cpu_avgs = [m['cpu_avg_percent'] for m in step_metrics]
        
        summary = {
            'description': description,
            'steps': steps,
            'warmup_steps': warmup_steps,
            'total_time_s': sum(execution_times) / 1000,
            'avg_step_time_ms': np.mean(execution_times),
            'std_step_time_ms': np.std(execution_times),
            'min_step_time_ms': np.min(execution_times),
            'max_step_time_ms': np.max(execution_times),
            'median_step_time_ms': np.median(execution_times),
            'memory_delta_mb': np.sum(memory_deltas),
            'avg_cpu_percent': np.mean(cpu_avgs),
            'steps_per_second': steps / (sum(execution_times) / 1000) if sum(execution_times) > 0 else 0,
            'step_metrics': step_metrics,
        }
        
        # print(f"  ✓ Benchmark complete!")
        # print(f"    Total time: {summary['total_time_s']:.2f}s")
        # print(f"    Avg step time: {summary['avg_step_time_ms']:.2f}ms")
        # print(f"    Steps per second: {summary['steps_per_second']:.1f}")
        
        return summary
    
    def analyze_performance(self, metrics_data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze performance data and generate insights."""
        step_metrics = metrics_data.get('step_metrics', [])
        
        if not step_metrics:
            return {'error': 'No performance data to analyze'}
            
        df = pd.DataFrame(step_metrics)
        
        analysis = {
            'basic_statistics': {
                'mean_step_time_ms': df['execution_time_ms'].mean(),
                'median_step_time_ms': df['execution_time_ms'].median(),
                'std_step_time_ms': df['execution_time_ms'].std(),
                'min_step_time_ms': df['execution_time_ms'].min(),
                'max_step_time_ms': df['execution_time_ms'].max(),
                'q25_step_time_ms': df['execution_time_ms'].quantile(0.25),
                'q75_step_time_ms': df['execution_time_ms'].quantile(0.75),
            },
            'memory_analysis': {
                'total_memory_delta_mb': df['memory_delta_mb'].sum(),
                'avg_memory_delta_mb': df['memory_delta_mb'].mean(),
                'memory_growth_rate_mb_per_step': df['memory_delta_mb'].sum() / len(df),
            },
            'cpu_analysis': {
                'avg_cpu_percent': df['cpu_avg_percent'].mean(),
                'max_cpu_percent': df['cpu_avg_percent'].max(),
                'cpu_utilization_efficiency': df['cpu_avg_percent'].mean() / 100,
            },
            'performance_trends': {
                'step_time_trend': self._calculate_trend(df['execution_time_ms']),
                'memory_trend': self._calculate_trend(df['memory_delta_mb']),
                'cpu_trend': self._calculate_trend(df['cpu_avg_percent']),
            },
            'bottleneck_analysis': self._identify_bottlenecks(df),
            'recommendations': self._generate_recommendations(df),
        }
        
        return analysis
        
    def _calculate_trend(self, values: pd.Series) -> Dict[str, float]:
        """Calculate linear trend of values."""
        x = np.arange(len(values))
        y = values.values
        
        # Linear regression
        slope = np.polyfit(x, y, 1)[0]
        correlation = np.corrcoef(x, y)[0, 1]
        
        return {
            'slope': slope,
            'correlation': correlation,
            'trend': 'increasing' if slope > 0.01 else 'decreasing' if slope < -0.01 else 'stable',
        }
        
    def _identify_bottlenecks(self, df: pd.DataFrame) -> List[str]:
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        # High step time variation indicates potential bottlenecks
        cv = df['execution_time_ms'].std() / df['execution_time_ms'].mean()
        if cv > 0.3:
            bottlenecks.append(
                f"High step time variation (coefficient of variation: {cv:.3f}) - "
                f"possible performance instability or bottlenecks"
            )
        
        # Memory growth trend
        memory_trend = self._calculate_trend(df['memory_delta_mb'])
        if memory_trend['trend'] == 'increasing' and abs(memory_trend['slope']) > 0.1:
            bottlenecks.append(
                f"Memory leak detected (slope: {memory_trend['slope']:.3f} MB/step)"
            )
        
        # High CPU utilization
        if df['cpu_avg_percent'].mean() > 80:
            bottlenecks.append(
                f"High CPU utilization (avg: {df['cpu_avg_percent'].mean():.1f}%)"
            )
        
        # Step time outliers
        q1 = df['execution_time_ms'].quantile(0.25)
        q3 = df['execution_time_ms'].quantile(0.75)
        iqr = q3 - q1
        outliers = df[(df['execution_time_ms'] < q1 - 1.5 * iqr) | 
                     (df['execution_time_ms'] > q3 + 1.5 * iqr)]
        
        if len(outliers) > len(df) * 0.1:  # More than 10% outliers
            bottlenecks.append(
                f"Many step time outliers detected ({len(outliers)} steps, {len(outliers)/len(df)*100:.1f}%)"
            )
        
        return bottlenecks
        
    def _generate_recommendations(self, df: pd.DataFrame) -> List[str]:
        """Generate performance improvement recommendations."""
        recommendations = []
        
        # Step time optimization
        avg_step_time = df['execution_time_ms'].mean()
        if avg_step_time > 10:  # More than 10ms per step
            recommendations.append(
                "Step time is high (>10ms). Consider:")
            recommendations.append("  - Optimizing neuron computations")
            recommendations.append("  - Reducing memory allocations")
            recommendations.append("  - Using more efficient data structures")
            
        # Memory optimization
        if df['memory_delta_mb'].sum() > 100:  # More than 100MB growth
            recommendations.append(
                "Memory growth is significant (>100MB). Consider:")
            recommendations.append("  - Implementing better memory management")
            recommendations.append("  - Using object pooling")
            recommendations.append("  - Reducing temporary allocations")
            
        # CPU optimization
        if df['cpu_avg_percent'].mean() > 70:
            recommendations.append(
                "High CPU utilization. Consider:")
            recommendations.append("  - Parallelizing computations")
            recommendations.append("  - Using SIMD optimizations")
            recommendations.append("  - Reducing computation complexity")
            
        # Stability improvement
        if df['execution_time_ms'].std() / df['execution_time_ms'].mean() > 0.5:
            recommendations.append(
                "High step time variation. Consider:")
            recommendations.append("  - Identifying and eliminating sources of variation")
            recommendations.append("  - Implementing better caching")
            recommendations.append("  - Optimizing resource management")
            
        if not recommendations:
            recommendations.append("Performance is within acceptable ranges.")
            
        return recommendations
    
    def export_metrics(self, filepath: str, format: str = "json") -> Any:
    """
    Function: Export metrics to file.
    """""
        filepath = Path(filepath)
        
        if format == "json":
            data = {
                'timings': self.metrics.timings,
                'memory_samples': self.metrics.memory_samples,
                'cpu_samples': self.metrics.cpu_samples,
                'system_samples': self.metrics.system_samples,
                'export_time': time.time(),
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
                
        elif format == "csv":
            # Export as CSV for analysis
            dataframes = self.metrics.to_dataframe()
            
            for name, df in dataframes.items():
                csv_path = filepath.with_suffix(f'_{name}.csv')
                df.to_csv(csv_path, index=False)
                
        elif format == "parquet":
            try:
                import pyarrow as pa
                import pyarrow.parquet as pq
                
                dataframes = self.metrics.to_dataframe()
                
                for name, df in dataframes.items():
                    parquet_path = filepath.with_suffix(f'_{name}.parquet')
                    table = pa.Table.from_pandas(df)
                    pq.write_table(table, parquet_path)
                    
            # Handle ImportError exception
                raise ImportError("pyarrow required for parquet export")
                
        # print(f"✓ Metrics exported to {filepath}")
        
    def load_metrics(self, filepath: str, format: str = "json") -> Any:
    """
    Function: Load metrics from file.
    """""
        filepath = Path(filepath)
        
        if format == "json":
            with open(filepath, 'r') as f:
                data = json.load(f)
                
            self.metrics = PerformanceMetrics()
            self.metrics.timings = data.get('timings', [])
            self.metrics.memory_samples = data.get('memory_samples', [])
            self.metrics.cpu_samples = data.get('cpu_samples', [])
            self.metrics.system_samples = data.get('system_samples', [])
            
        # print(f"✓ Metrics loaded from {filepath}")
class ComparativeProfiler:
    """
    Class: 
    Class: 
    Class: Profiler for comparing different NLM configurations.
    
    
    """""""""
    
    def __init__(self):
        self.benchmark_results = {}
        
    def run_configuration_comparison(self, configurations: List[Dict],
                                    simulation_func: Callable,
                                    steps: int) -> Dict[str, Any]:
        """Compare different configurations."""
        # print(f"Comparing {len(configurations)} configurations...")
        
        results = {}
        
        for i, config in enumerate(configurations):
            # print(f"  Running configuration {i+1}: {config.get('name', 'Unnamed')}")
            
            # Create profiler for this configuration
            profiler = NLMProfiler(ProfilingMode.DETAILED)
            
            # Run simulation
            benchmark_result = profiler.run_benchmark(
                simulation_func, steps, description=config.get('name', f'Config_{i}')
            )
            
            results[config.get('name', f'Config_{i}')] = {
                'profiler': profiler,
                'benchmark_result': benchmark_result,
                'config': config
            }
            
        self.benchmark_results = results
        
        # Generate comparison report
        comparison = self._generate_comparison_report(results)
        
        return comparison
        
    def _generate_comparison_report(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Generate comparison report."""
        if not results:
            return {}
            
        # Extract key metrics
        comparison_data = []
        
        for name, result in results.items():
            benchmark = result['benchmark_result']
            comparison_data.append({
                'configuration': name,
                'avg_step_time_ms': benchmark['avg_step_time_ms'],
                'std_step_time_ms': benchmark['std_step_time_ms'],
                'steps_per_second': benchmark['steps_per_second'],
                'total_time_s': benchmark['total_time_s'],
                'memory_delta_mb': benchmark['memory_delta_mb'],
                'avg_cpu_percent': benchmark['avg_cpu_percent'],
                'config_params': result['config'],
            })
        
        df = pd.DataFrame(comparison_data)
        
        # Calculate rankings
        df['rank_step_time'] = df['avg_step_time_ms'].rank()
        df['rank_performance'] = df['steps_per_second'].rank(ascending=False)
        df['rank_efficiency'] = df['steps_per_second'].rank(ascending=False)
        
        # Generate insights
        insights = self._generate_comparison_insights(df)
        
        comparison = {
            'data': df.to_dict('records'),
            'insights': insights,
            'recommendations': self._generate_configuration_recommendations(df),
        }
        
        return comparison
        
    def _generate_comparison_insights(self, df: pd.DataFrame) -> List[str]:
        """Generate insights from configuration comparison."""
        insights = []
        
        # Best performance configuration
        best_performance = df.loc[df['steps_per_second'].idxmax()]
        insights.append(
            f"Best performance: {best_performance['configuration']} "
            f"({best_performance['steps_per_second']:.1f} steps/s)"
        )
        
        # Most efficient configuration
        best_efficiency = df.loc[df['rank_efficiency'].idxmin()]
        insights.append(
            f"Most efficient: {best_efficiency['configuration']} "
            f"(rank {best_efficiency['rank_efficiency']:.0f})"
        )
        
        # Configuration with lowest step time variation
        most_stable = df.loc[df['std_step_time_ms'].idxmin()]
        insights.append(
            f"Most stable: {most_stable['configuration']} "
            f"(std: {most_stable['std_step_time_ms']:.2f} ms)"
        )
        
        # Configuration with lowest memory usage
        least_memory = df.loc[df['memory_delta_mb'].idxmin()]
        insights.append(
            f"Lowest memory growth: {least_memory['configuration']} "
            f"({least_memory['memory_delta_mb']:.1f} MB)"
        )
        
        return insights
        
    def _generate_configuration_recommendations(self, df: pd.DataFrame) -> List[str]:
        """Generate configuration recommendations."""
        recommendations = []
        
        # Find best overall configuration
        df['composite_score'] = (
            df['rank_step_time'] * 0.4 + 
            df['rank_performance'] * 0.4 + 
            df['rank_efficiency'] * 0.2
        )
        
        best_overall = df.loc[df['composite_score'].idxmin()]
        
        recommendations.append(
            f"Recommended configuration: {best_overall['configuration']}"
        )
        recommendations.append(
            f"  - Average step time: {best_overall['avg_step_time_ms']:.2f} ms"
        )
        recommendations.append(
            f"  - Performance: {best_overall['steps_per_second']:.1f} steps/s"
        )
        
        # Identify configurations that excel in specific areas
        if df['steps_per_second'].max() / df['steps_per_second'].min() > 2:
            recommendations.append(
                "Significant performance variation detected. "
                "Consider configuration-specific optimizations."
            )
            
        if df['std_step_time_ms'].max() / df['std_step_time_ms'].min() > 3:
            recommendations.append(
                "Stability varies significantly across configurations. "
                "Focus on consistency optimization."
            )
            
        return recommendations
# Convenience functions
class NLMPerformanceSuite:
    """
    Class: 
    Class: 
    Class: High-level interface for NLM performance testing.
    
    
    """""""""
    
    @staticmethod
    def create_profiler(mode: str = "basic") -> NLMProfiler:
        """Create a profiler with specified mode."""
        mode_map = {
            'basic': ProfilingMode.BASIC,
            'detailed': ProfilingMode.DETAILED,
            'cpu': ProfilingMode.CPU,
            'memory': ProfilingMode.MEMORY,
            'custom': ProfilingMode.CUSTOM,
        }
        
        if mode not in mode_map:
            raise ValueError(f"Unknown profiling mode: {mode}")
            
        return NLMProfiler(mode_map[mode])
        
    @staticmethod
    def create_comparative_profiler() -> ComparativeProfiler:
        """Create a comparative profiler."""
        return ComparativeProfiler()
        
    @staticmethod
    def run_simple_benchmark(simulation_func: Callable, 
                           steps: int,
                           description: str = "Simple Benchmark") -> Dict[str, Any]:
        """Run simple benchmark for a simulation function."""
        profiler = NLMProfiler(ProfilingMode.BASIC)
        return profiler.run_benchmark(simulation_func, steps, description=description)
        
    @staticmethod
    def profile_function(func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """Profile a single function call."""
        profiler = NLMProfiler(ProfilingMode.DETAILED)
        profiler.start_profiling()
        
        try:
            result = profiler.profile_function(func, *args, **kwargs)
            
            # Get profiling data
            metrics = profiler.metrics.to_dataframe()
            
            return {
                'result': result,
                'metrics': metrics,
                'profiler': profiler,
            }
            
        finally:
            profiler.stop_profiling()
# Main example function
def main() -> Any:
    """
    Function: Example usage of NLM profiling tools.
    """""
    # print("NLM Profiling and Benchmarking Module Examples")
    # print("=" * 50)
    
    # Example 1: Simple profiler
    # print("\n1. Basic Profiler Example:")
    profiler = NLMProfiler(ProfilingMode.BASIC)
    profiler.start_profiling()
    
    # Simulate some work
    for i in range(10):
        time.sleep(0.001)  # Simulate work
        profiler.add_timing("iteration", time.time())
        
    profiler.stop_profiling()
    # print("   ✓ Basic profiling completed")
    
    # Example 2: Function profiling
    # print("\n2. Function Profiling Example:")
    def sample_function(n):
        result = 0
        for i in range(n):
            result += i * i
        return result
        
    profiled = NLMPerformanceSuite.profile_function(sample_function, 1000)
    # print(f"   ✓ Function profiled: result = {profiled['result']}")
    
    # Example 3: Benchmarking
    # print("\n3. Benchmarking Example:")
    
    def sample_simulation(steps) -> Any:
    """
    Function: Sample simulation function.
    """""
        total = 0
        for step in range(steps):
            # Simulate computation
            total += step * step
            time.sleep(0.0001)  # Simulate processing time
        return total
        
    benchmark_result = NLMPerformanceSuite.run_simple_benchmark(
        sample_simulation, 100, "Sample Benchmark"
    )
    
    # print(f"   ✓ Benchmark completed:")
    # print(f"     - Total steps: {benchmark_result['steps']}")
    # print(f"     - Total time: {benchmark_result['total_time_s']:.2f}s")
    # print(f"     - Avg step time: {benchmark_result['avg_step_time_ms']:.2f}ms")
    # print(f"     - Performance: {benchmark_result['steps_per_second']:.1f} steps/s")
    
    # Example 4: Comparative profiling
    # print("\n4. Comparative Profiling Example:")
    
    configurations = [
        {'name': 'Baseline', 'param1': 1, 'param2': 100},
        {'name': 'Optimized', 'param1': 2, 'param2': 200},
        {'name': 'Aggressive', 'param1': 3, 'param2': 300},
    ]
    
    def simulation_with_config(config) -> Any:
    """
    Function: Simulation that uses configuration.
    """""
        steps = config.get('param2', 100)
        factor = config.get('param1', 1)
        
        total = 0
        for step in range(steps):
            total += step * factor
            time.sleep(0.0001)
        return total
        
    comparative = NLMPerformanceSuite.create_comparative_profiler()
    comparison = comparative.run_configuration_comparison(
        configurations, simulation_with_config, 50
    )
    
    # print(f"   ✓ Comparative analysis completed:")
    # print(f"     - Configurations compared: {len(comparison['data'])}")
    # print(f"     - Key insights: {len(comparison['insights'])} insights generated")
    
    # Print insights
    for insight in comparison['insights']:
        # print(f"     • {insight}")
        
    # print("\n" + "=" * 50)
    # print("Profiling examples completed successfully!")
    # print("\nKey features demonstrated:")
    # print("✓ Basic performance profiling")
    # print("✓ Function-level profiling")
    # print("✓ Comprehensive benchmarking")
    # print("✓ Configuration comparison")
    # print("✓ Performance analysis and insights")
    # print("✓ Bottleneck identification")
    # print("✓ Optimization recommendations")
if __name__ == "__main__":
    main()
