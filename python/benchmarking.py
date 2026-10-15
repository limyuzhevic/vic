#!/usr/bin/env python3
"""
NLM Benchmarking and Profiling Tools

Advanced benchmarking and profiling tools for NLM performance analysis
and optimization.
"""

import time
import threading
import json
import psutil
import numpy as np
from typing import Dict, List, Optional, Any, Callable
from collections import defaultdict, deque
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import seaborn as sns
from dataclasses import dataclass
import sys

# Import NLM modules
import pynlm
from python.convenience_api import createQuickBrain

__all__ = [
    'BenchmarkSuite',
    'Profiler',
    'PerformanceAnalyzer',
    'StressTester',
    'createBenchmarkSuite',
    'createProfiler',
    'createPerformanceAnalyzer',
    'createStressTester'
]

@dataclass
class BenchmarkResult:
    """Benchmark result data."""
    name: str
    duration: float
    iterations: int
    success_rate: float
    metrics: Dict[str, float]
    memory_usage: Dict[str, float]
    cpu_usage: Dict[str, float]
    timestamp: float
    config: Optional[Dict] = None
@dataclass
class ProfileData:
    """Profile data for NLM operations."""
    name: str
    total_time: float
    call_count: int
    avg_time: float
    min_time: float
    max_time: float
    time_history: List[float]
    memory_samples: List[float]
    cpu_samples: List[float]
    start_time: float
    end_time: float
@dataclass
class PerformanceMetrics:
    """Performance metrics for optimization."""
    brain_id: str
    neuron_count: int
    synapse_count: int
    memory_usage_mb: float
    cpu_usage_percent: float
    simulation_speed_steps_per_sec: float
    efficiency_score: float
    bottleneck_identified: bool
    recommendations: List[str]

class BenchmarkSuite:
    """
    Comprehensive benchmark suite for NLM.
    
    This class provides comprehensive benchmarking including:
    - Performance benchmarking
    - Memory benchmarking
    - Scalability testing
    - Stress testing
    - Comparative benchmarking
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize benchmark suite.
        
        Args:
            brain: Brain to benchmark
        """
        self.brain = brain
        self.benchmark_results = []
        self.benchmark_history = []
        self.performance_monitors = {}
    
    def benchmark_step_performance(self, iterations: int = 100, 
                                  warmup_iterations: int = 10) -> BenchmarkResult:
        """
        Benchmark step performance.
        
        Args:
            iterations: Number of benchmark iterations
            warmup_iterations: Number of warmup iterations
            
        Returns:
            Benchmark result
        """
        # Warmup
        for _ in range(warmup_iterations):
            self.brain.step(0)
        
        # Benchmark
        times = []
        start_time = time.time()
        
        for i in range(iterations):
            step_start = time.time()
            self.brain.step(i + warmup_iterations)
            step_time = time.time() - step_start
            times.append(step_time)
        
        total_time = time.time() - start_time
        
        # Calculate metrics
        success_rate = len([t for t in times if t < 0.1]) / len(times) * 100
        avg_time = np.mean(times)
        std_time = np.std(times)
        min_time = np.min(times)
        max_time = np.max(times)
        
        # Memory usage
        memory_stats = self._get_memory_stats()
        
        # CPU usage
        cpu_stats = self._get_cpu_stats()
        
        # Create result
        result = BenchmarkResult(
            name="step_performance",
            duration=total_time,
            iterations=iterations,
            success_rate=success_rate,
            metrics={
                'avg_time': avg_time,
                'std_time': std_time,
                'min_time': min_time,
                'max_time': max_time,
                'steps_per_second': iterations / total_time,
                'efficiency': 1.0 / avg_time if avg_time > 0 else 0.0
            },
            memory_usage=memory_stats,
            cpu_usage=cpu_stats,
            timestamp=time.time(),
            config=self._get_brain_config()
        )
        
        self.benchmark_results.append(result)
        self.benchmark_history.append(result)
        
        return result
    
    def benchmark_memory_usage(self, test_duration: float = 60.0,
                             sampling_interval: float = 1.0) -> BenchmarkResult:
        """
        Benchmark memory usage over time.
        
        Args:
            test_duration: Test duration in seconds
            sampling_interval: Sampling interval in seconds
            
        Returns:
            Benchmark result
        """
        start_time = time.time()
        memory_samples = []
        cpu_samples = []
        step_count = 0
        
        while time.time() - start_time < test_duration:
            # Take memory and CPU samples
            process = psutil.Process()
            memory_info = process.memory_info()
            cpu_percent = process.cpu_percent()
            
            memory_samples.append(memory_info.rss / 1024 / 1024)  # MB
            cpu_samples.append(cpu_percent)
            
            # Run a few steps
            for _ in range(10):
                self.brain.step(step_count)
                step_count += 1
            
            time.sleep(sampling_interval)
        
        # Calculate metrics
        total_time = time.time() - start_time
        avg_memory = np.mean(memory_samples)
        max_memory = np.max(memory_samples)
        avg_cpu = np.mean(cpu_samples)
        
        result = BenchmarkResult(
            name="memory_usage",
            duration=total_time,
            iterations=step_count,
            success_rate=100.0,
            metrics={
                'avg_memory_mb': avg_memory,
                'max_memory_mb': max_memory,
                'memory_growth_mb': memory_samples[-1] - memory_samples[0] if len(memory_samples) > 1 else 0.0,
                'memory_stability': 1.0 - np.std(memory_samples) / avg_memory if avg_memory > 0 else 0.0
            },
            memory_usage={
                'avg_mb': avg_memory,
                'max_mb': max_memory,
                'samples': memory_samples
            },
            cpu_usage={
                'avg_percent': avg_cpu,
                'samples': cpu_samples
            },
            timestamp=time.time(),
            config=self._get_brain_config()
        )
        
        self.benchmark_results.append(result)
        self.benchmark_history.append(result)
        
        return result
    
    def benchmark_scalability(self, neuron_counts: List[int],
                            iterations_per_count: int = 10) -> List[BenchmarkResult]:
        """
        Benchmark scalability with different neuron counts.
        
        Args:
            neuron_counts: List of neuron counts to test
            iterations_per_count: Iterations per neuron count
            
        Returns:
            List of benchmark results
        """
        results = []
        
        for neuron_count in neuron_counts:
            print(f"Benchmarking scalability with {neuron_count} neurons...")
            
            # Create brain with specified neuron count
            config = self._get_brain_config()
            config['brain.neuron_count'] = neuron_count
            
            test_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            for key, value in config.items():
                test_brain.getConfig().set(key, value)
            
            test_brain.initialize()
            
            # Run benchmark
            result = self.benchmark_step_performance(iterations_per_count)
            result.config['brain.neuron_count'] = neuron_count
            
            results.append(result)
        
        return results
    
    def benchmark_stress_test(self, duration: float = 300.0,
                            stress_level: str = "high") -> BenchmarkResult:
        """
        Benchmark stress test.
        
        Args:
            duration: Test duration in seconds
            stress_level: Stress level ('low', 'medium', 'high')
            
        Returns:
            Benchmark result
        """
        start_time = time.time()
        step_count = 0
        errors = 0
        memory_usage = []
        cpu_usage = []
        
        # Set stress parameters
        stress_params = {
            'high': {
                'max_steps': 10000,
                'memory_limit_mb': 2048,
                'cpu_limit_percent': 90
            },
            'medium': {
                'max_steps': 5000,
                'memory_limit_mb': 1024,
                'cpu_limit_percent': 75
            },
            'low': {
                'max_steps': 1000,
                'memory_limit_mb': 512,
                'cpu_limit_percent': 50
            }
        }[stress_level]
        
        while time.time() - start_time < duration:
            try:
                # Monitor resources
                process = psutil.Process()
                current_memory = process.memory_info().rss / 1024 / 1024
                current_cpu = process.cpu_percent()
                
                memory_usage.append(current_memory)
                cpu_usage.append(current_cpu)
                
                # Run steps
                for _ in range(100):
                    if step_count >= stress_params['max_steps']:
                        break
                    
                    self.brain.step(step_count)
                    step_count += 1
                
                # Check resource limits
                if current_memory > stress_params['memory_limit_mb']:
                    print(f"Memory limit exceeded: {current_memory:.1f} MB > {stress_params['memory_limit_mb']} MB")
                    errors += 1
                
                if current_cpu > stress_params['cpu_limit_percent']:
                    print(f"CPU limit exceeded: {current_cpu:.1f}% > {stress_params['cpu_limit_percent']}%")
                    errors += 1
                
                time.sleep(0.1)  # Small delay to prevent overwhelming
            
            except Exception as e:
                errors += 1
                print(f"Error during stress test: {e}")
        
        total_time = time.time() - start_time
        success_rate = max(0, (1 - errors / max(1, step_count))) * 100
        
        result = BenchmarkResult(
            name="stress_test",
            duration=total_time,
            iterations=step_count,
            success_rate=success_rate,
            metrics={
                'steps_per_second': step_count / total_time,
                'error_rate': errors / max(1, step_count) * 100,
                'avg_memory_mb': np.mean(memory_usage) if memory_usage else 0,
                'max_memory_mb': np.max(memory_usage) if memory_usage else 0,
                'avg_cpu_percent': np.mean(cpu_usage) if cpu_usage else 0,
                'stress_level': stress_level
            },
            memory_usage={
                'avg_mb': np.mean(memory_usage) if memory_usage else 0,
                'max_mb': np.max(memory_usage) if memory_usage else 0,
                'samples': memory_usage
            },
            cpu_usage={
                'avg_percent': np.mean(cpu_usage) if cpu_usage else 0,
                'samples': cpu_usage
            },
            timestamp=time.time(),
            config=self._get_brain_config()
        )
        
        self.benchmark_results.append(result)
        self.benchmark_history.append(result)
        
        return result
    
    def benchmark_comparison(self, brains: List[pynlm.Brain],
                           benchmark_name: str) -> List[BenchmarkResult]:
        """
        Benchmark comparison of multiple brains.
        
        Args:
            brains: List of brains to compare
            benchmark_name: Name of benchmark
            
        Returns:
            List of benchmark results
        """
        results = []
        
        for i, brain in enumerate(brains):
            print(f"Benchmarking brain {i + 1}/{len(brains)}...")
            
            # Temporarily replace brain
            original_brain = self.brain
            self.brain = brain
            
            try:
                # Run benchmark
                result = self.benchmark_step_performance(100)
                result.name = f"{benchmark_name}_brain_{i}"
                result.config = self._get_brain_config(brain)
                
                results.append(result)
            finally:
                # Restore original brain
                self.brain = original_brain
        
        return results
    
    def _get_memory_stats(self) -> Dict[str, float]:
        """Get memory statistics."""
        try:
            process = psutil.Process()
            memory_info = process.memory_info()
            
            return {
                'rss_mb': memory_info.rss / 1024 / 1024,
                'vms_mb': memory_info.vms / 1024 / 1024,
                'shared_mb': memory_info.shared / 1024 / 1024 if hasattr(memory_info, 'shared') else 0.0
            }
        except:
            return {'rss_mb': 0.0, 'vms_mb': 0.0, 'shared_mb': 0.0}
    
    def _get_cpu_stats(self) -> Dict[str, float]:
        """Get CPU statistics."""
        try:
            process = psutil.Process()
            
            return {
                'cpu_percent': process.cpu_percent(),
                'cpu_times': process.cpu_times().user,
                'cpu_frequency': process.cpu_freq().current if hasattr(process, 'cpu_freq') else 0.0
            }
        except:
            return {'cpu_percent': 0.0, 'cpu_times': 0.0, 'cpu_frequency': 0.0}
    
    def _get_brain_config(self, brain: Optional[pynlm.Brain] = None) -> Dict[str, Any]:
        """Get brain configuration."""
        target_brain = brain or self.brain
        
        if hasattr(target_brain, 'getConfig'):
            config = target_brain.getConfig()
            if config:
                return {key: config.getOr(key, None) for key in config.getKeys()}
        
        return {}
    
    def generate_report(self, output_path: Optional[str] = None) -> str:
        """
        Generate benchmark report.
        
        Args:
            output_path: Path to save report
            
        Returns:
            Report as string
        """
        if not self.benchmark_results:
            return "No benchmark results available"
        
        report_lines = []
        report_lines.append("NLM Benchmark Report")
        report_lines.append("=" * 50)
        report_lines.append(f"Total benchmarks: {len(self.benchmark_results)}")
        report_lines.append(f"Test date: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report_lines.append("")
        
        for i, result in enumerate(self.benchmark_results):
            report_lines.append(f"Benchmark {i + 1}: {result.name}")
            report_lines.append(f"  Duration: {result.duration:.2f}s")
            report_lines.append(f"  Iterations: {result.iterations}")
            report_lines.append(f"  Success rate: {result.success_rate:.1f}%")
            report_lines.append(f"  Metrics: {json.dumps(result.metrics, indent=6)}")
            report_lines.append(f"  Memory usage: {json.dumps(result.memory_usage, indent=6)}")
            report_lines.append(f"  CPU usage: {json.dumps(result.cpu_usage, indent=6)}")
            report_lines.append("")
        
        report = "\n".join(report_lines)
        
        if output_path:
            with open(output_path, 'w') as f:
                f.write(report)
        
        return report

class Profiler:
    """
    Profiler for NLM operations.
    
    This class provides profiling capabilities including:
    - Function profiling
    - Memory profiling
    - CPU profiling
    - Timeline analysis
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize profiler.
        
        Args:
            brain: Brain to profile
        """
        self.brain = brain
        self.profile_data = {}
        self.profiling = False
        self.monitor_thread = None
        self.data_lock = threading.Lock()
    
    def start_profiling(self, function_name: str, profile_config: Optional[Dict] = None):
        """
        Start profiling a function.
        
        Args:
            function_name: Name of function to profile
            profile_config: Profile configuration
        """
        if profile_config is None:
            profile_config = {
                'sampling_interval': 0.1,
                'duration': 60.0,
                'include_memory': True,
                'include_cpu': True
            }
        
        profile = ProfileData(
            name=function_name,
            total_time=0.0,
            call_count=0,
            avg_time=0.0,
            min_time=0.0,
            max_time=0.0,
            time_history=[],
            memory_samples=[],
            cpu_samples=[],
            start_time=time.time(),
            end_time=0.0
        )
        
        self.profile_data[function_name] = profile
        self.profiling = True
        
        self.monitor_thread = threading.Thread(
            target=self._profile_loop,
            kwargs={'profile_name': function_name, 'config': profile_config},
            daemon=True
        )
        self.monitor_thread.start()
    
    def stop_profiling(self, function_name: str) -> ProfileData:
        """
        Stop profiling a function.
        
        Args:
            function_name: Name of function to stop profiling
            
        Returns:
            Profile data
        """
        if function_name not in self.profile_data:
            raise ValueError(f"Function '{function_name}' is not being profiled")
        
        self.profiling = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=10.0)
        
        profile = self.profile_data[function_name]
        profile.end_time = time.time()
        
        # Calculate statistics
        if profile.time_history:
            profile.total_time = sum(profile.time_history)
            profile.avg_time = profile.total_time / len(profile.time_history)
            profile.min_time = min(profile.time_history)
            profile.max_time = max(profile.time_history)
            profile.call_count = len(profile.time_history)
        
        return profile
    
    def profile_step(self, step_name: str = "step"):
        """
        Profile a single brain step.
        
        Args:
            step_name: Name of step
        """
        with self._get_lock():
            if step_name not in self.profile_data:
                return
            
            profile = self.profile_data[step_name]
            
            # Record step timing
            step_start = time.time()
            self.brain.step(step_name)
            step_time = time.time() - step_start
            
            profile.time_history.append(step_time)
            
            # Record memory usage
            if profile.memory_samples:
                try:
                    process = psutil.Process()
                    memory_info = process.memory_info()
                    profile.memory_samples.append(memory_info.rss / 1024 / 1024)
                except:
                    profile.memory_samples.append(0.0)
            
            # Record CPU usage
            if profile.cpu_samples:
                try:
                    process = psutil.Process()
                    cpu_percent = process.cpu_percent()
                    profile.cpu_samples.append(cpu_percent)
                except:
                    profile.cpu_samples.append(0.0)
    
    def _profile_loop(self, profile_name: str, config: Dict):
        """Profile loop."""
        sampling_interval = config['sampling_interval']
        duration = config['duration']
        include_memory = config['include_memory']
        include_cpu = config['include_cpu']
        
        start_time = time.time()
        
        while self.profiling:
            with self._get_lock():
                if profile_name not in self.profile_data:
                    break
                
                profile = self.profile_data[profile_name]
                
                # Record system metrics
                if include_memory:
                    try:
                        process = psutil.Process()
                        memory_info = process.memory_info()
                        profile.memory_samples.append(memory_info.rss / 1024 / 1024)
                    except:
                        profile.memory_samples.append(0.0)
                
                if include_cpu:
                    try:
                        process = psutil.Process()
                        cpu_percent = process.cpu_percent()
                        profile.cpu_samples.append(cpu_percent)
                    except:
                        profile.cpu_samples.append(0.0)
            
            # Sleep for sampling interval
            time.sleep(sampling_interval)
            
            # Check duration
            if time.time() - start_time > duration:
                break
    
    def _get_lock(self):
        """Get lock for thread safety."""
        return threading.Lock()
    
    def generate_profile_report(self, profile_name: str, 
                              output_path: Optional[str] = None) -> str:
        """
        Generate profile report.
        
        Args:
            profile_name: Name of profile
            output_path: Path to save report
            
        Returns:
            Profile report as string
        """
        if profile_name not in self.profile_data:
            return f"Profile '{profile_name}' not found"
        
        profile = self.profile_data[profile_name]
        
        if not profile.time_history:
            return f"Profile '{profile_name}' has no data"
        
        report_lines = []
        report_lines.append(f"NLM Profile Report: {profile_name}")
        report_lines.append("=" * 50)
        report_lines.append(f"Total time: {profile.total_time:.2f}s")
        report_lines.append(f"Number of calls: {profile.call_count}")
        report_lines.append(f"Average time: {profile.avg_time:.6f}s")
        report_lines.append(f"Min time: {profile.min_time:.6f}s")
        report_lines.append(f"Max time: {profile.max_time:.6f}s")
        report_lines.append(f"Time per call (μs): {profile.avg_time * 1000000:.0f}")
        report_lines.append(f"Start time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(profile.start_time))}")
        report_lines.append(f"End time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(profile.end_time))}")
        
        if profile.memory_samples:
            report_lines.append(f"\nMemory Usage:")
            report_lines.append(f"  Average: {np.mean(profile.memory_samples):.1f} MB")
            report_lines.append(f"  Max: {np.max(profile.memory_samples):.1f} MB")
            report_lines.append(f"  Min: {np.min(profile.memory_samples):.1f} MB")
            report_lines.append(f"  Std: {np.std(profile.memory_samples):.1f} MB")
        
        if profile.cpu_samples:
            report_lines.append(f"\nCPU Usage:")
            report_lines.append(f"  Average: {np.mean(profile.cpu_samples):.1f}%")
            report_lines.append(f"  Max: {np.max(profile.cpu_samples):.1f}%")
            report_lines.append(f"  Min: {np.min(profile.cpu_samples):.1f}%")
        
        report = "\n".join(report_lines)
        
        if output_path:
            with open(output_path, 'w') as f:
                f.write(report)
        
        return report

class PerformanceAnalyzer:
    """
    Performance analyzer for NLM.
    
    This class provides performance analysis including:
    - Bottleneck detection
    - Optimization recommendations
    - Performance trend analysis
    - Comparative analysis
    """
    
    def __init__(self, benchmark_suite: BenchmarkSuite):
        """
        Initialize performance analyzer.
        
        Args:
            benchmark_suite: Benchmark suite to analyze
        """
        self.benchmark_suite = benchmark_suite
        self.analysis_results = {}
    
    def analyze_performance(self, benchmark_name: str) -> Dict[str, Any]:
        """
        Analyze performance for a benchmark.
        
        Args:
            benchmark_name: Name of benchmark to analyze
            
        Returns:
            Analysis results
        """
        results = []
        for result in self.benchmark_suite.benchmark_results:
            if result.name == benchmark_name:
                results.append(result)
        
        if not results:
            return {'error': f"No benchmark results found for '{benchmark_name}'"}
        
        analysis = {
            'benchmark_name': benchmark_name,
            'num_results': len(results),
            'total_duration': sum(r.duration for r in results),
            'avg_duration': np.mean([r.duration for r in results]),
            'std_duration': np.std([r.duration for r in results]),
            'performance_trend': self._calculate_trend(results),
            'bottleneck_indicators': self._identify_bottlenecks(results),
            'optimization_recommendations': self._generate_recommendations(results),
            'detailed_results': [self._result_to_dict(r) for r in results]
        }
        
        self.analysis_results[benchmark_name] = analysis
        
        return analysis
    
    def _calculate_trend(self, results: List[BenchmarkResult]) -> Dict[str, Any]:
        """Calculate performance trend."""
        if len(results) < 2:
            return {'trend': 'insufficient_data', 'confidence': 0.0}
        
        # Calculate trend based on time metric
        times = [r.metrics.get('avg_time', 0) for r in results]
        
        if len(times) < 2:
            return {'trend': 'insufficient_data', 'confidence': 0.0}
        
        # Simple linear regression
        x = list(range(len(times)))
        y = times
        
        correlation = np.corrcoef(x, y)[0, 1] if len(x) > 1 else 0.0
        
        if correlation > 0.7:
            trend = 'degrading'
        elif correlation < -0.7:
            trend = 'improving'
        else:
            trend = 'stable'
        
        return {
            'trend': trend,
            'correlation': correlation,
            'confidence': abs(correlation)
        }
    
    def _identify_bottlenecks(self, results: List[BenchmarkResult]) -> List[str]:
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        # Check for slow step times
        slow_steps = [r for r in results if r.metrics.get('avg_time', 0) > 0.1]
        if slow_steps:
            bottlenecks.append(f"Slow step performance: {len(slow_steps)}/{len(results)} benchmarks have avg step time > 100ms")
        
        # Check for memory issues
        high_memory = [r for r in results if r.memory_usage.get('avg_mb', 0) > 1000]
        if high_memory:
            bottlenecks.append(f"High memory usage: {len(high_memory)}/{len(results)} benchmarks use >1GB memory")
        
        # Check for CPU issues
        high_cpu = [r for r in results if r.cpu_usage.get('avg_percent', 0) > 80]
        if high_cpu:
            bottlenecks.append(f"High CPU usage: {len(high_cpu)}/{len(results)} benchmarks use >80% CPU")
        
        # Check for scalability issues
        if len(results) >= 3:
            # Simple scalability check - check if performance degrades with scale
            pass
        
        return bottlenecks
    
    def _generate_recommendations(self, results: List[BenchmarkResult]) -> List[str]:
        """Generate optimization recommendations."""
        recommendations = []
        
        # Check for memory optimization opportunities
        high_memory = [r for r in results if r.memory_usage.get('avg_mb', 0) > 500]
        if high_memory:
            recommendations.append("Optimize memory usage - consider reducing brain size or using more efficient data structures")
        
        # Check for computation optimization opportunities
        slow_steps = [r for r in results if r.metrics.get('avg_time', 0) > 0.05]
        if slow_steps:
            recommendations.append("Optimize step computation - consider vectorization or algorithmic improvements")
        
        # Check for stability issues
        unstable = [r for r in results if r.metrics.get('std_time', 0) > r.metrics.get('avg_time', 0) * 0.5]
        if unstable:
            recommendations.append("Improve performance stability - reduce variance in step times")
        
        # Check for resource utilization
        underutilized = [r for r in results if r.cpu_usage.get('avg_percent', 0) < 50]
        if underutilized:
            recommendations.append("Improve resource utilization - consider parallel processing or better load balancing")
        
        return recommendations
    
    def _result_to_dict(self, result: BenchmarkResult) -> Dict[str, Any]:
        """Convert benchmark result to dictionary."""
        return {
            'name': result.name,
            'duration': result.duration,
            'iterations': result.iterations,
            'success_rate': result.success_rate,
            'metrics': result.metrics,
            'memory_usage': result.memory_usage,
            'cpu_usage': result.cpu_usage,
            'timestamp': result.timestamp,
            'config': result.config
        }

class StressTester:
    """
    Stress tester for NLM.
    
    This class provides stress testing capabilities including:
    - Extreme load testing
    - Resource exhaustion testing
    - Failure recovery testing
    - Edge case testing
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize stress tester.
        
        Args:
            brain: Brain to stress test
        """
        self.brain = brain
        self.stress_results = []
        self.current_stress_test = None
    
    def run_stress_test(self, test_name: str, duration: float = 60.0,
                       stress_level: str = "moderate", 
                       failure_tolerance: float = 0.1) -> Dict[str, Any]:
        """
        Run stress test.
        
        Args:
            test_name: Name of stress test
            duration: Test duration in seconds
            stress_level: Stress level ('low', 'moderate', 'high', 'extreme')
            failure_tolerance: Tolerance for failures (0.0-1.0)
            
        Returns:
            Stress test results
        """
        start_time = time.time()
        
        # Initialize stress test
        stress_config = self._get_stress_config(stress_level)
        
        # Run stress test
        test_results = self._run_stress_test(
            test_name, duration, stress_config, failure_tolerance
        )
        
        total_time = time.time() - start_time
        
        result = {
            'test_name': test_name,
            'duration': total_time,
            'stress_level': stress_level,
            'failure_tolerance': failure_tolerance,
            'start_time': start_time,
            'end_time': time.time(),
            'test_results': test_results,
            'summary': self._generate_stress_summary(test_results, stress_config)
        }
        
        self.stress_results.append(result)
        
        return result
    
    def _get_stress_config(self, stress_level: str) -> Dict[str, Any]:
        """Get stress configuration."""
        configs = {
            'low': {
                'max_steps': 1000,
                'memory_limit_mb': 512,
                'cpu_limit_percent': 50,
                'max_errors': 10,
                'sampling_interval': 1.0
            },
            'moderate': {
                'max_steps': 5000,
                'memory_limit_mb': 1024,
                'cpu_limit_percent': 75,
                'max_errors': 50,
                'sampling_interval': 0.5
            },
            'high': {
                'max_steps': 10000,
                'memory_limit_mb': 2048,
                'cpu_limit_percent': 90,
                'max_errors': 100,
                'sampling_interval': 0.1
            },
            'extreme': {
                'max_steps': 20000,
                'memory_limit_mb': 4096,
                'cpu_limit_percent': 95,
                'max_errors': 200,
                'sampling_interval': 0.05
            }
        }
        
        return configs.get(stress_level, configs['moderate'])
    
    def _run_stress_test(self, test_name: str, duration: float,
                        stress_config: Dict[str, Any], 
                        failure_tolerance: float) -> Dict[str, Any]:
        """Run stress test."""
        start_time = time.time()
        
        results = {
            'steps_completed': 0,
            'errors': 0,
            'memory_samples': [],
            'cpu_samples': [],
            'step_times': [],
            'bottlenecks': [],
            'recovery_attempts': 0,
            'successful_recoveries': 0
        }
        
        try:
            while time.time() - start_time < duration:
                # Check resource limits
                process = psutil.Process()
                current_memory = process.memory_info().rss / 1024 / 1024
                current_cpu = process.cpu_percent()
                
                results['memory_samples'].append(current_memory)
                results['cpu_samples'].append(current_cpu)
                
                # Run steps
                step_start = time.time()
                try:
                    self.brain.step(results['steps_completed'])
                    step_time = time.time() - step_start
                    results['step_times'].append(step_time)
                    results['steps_completed'] += 1
                    
                    # Check if step is too slow
                    if step_time > 1.0:
                        results['bottlenecks'].append(f"Slow step: {step_time:.3f}s")
                
                except Exception as e:
                    results['errors'] += 1
                    results['bottlenecks'].append(f"Error: {e}")
                
                # Check resource limits
                if current_memory > stress_config['memory_limit_mb']:
                    results['bottlenecks'].append(
                        f"Memory limit exceeded: {current_memory:.1f} MB > {stress_config['memory_limit_mb']} MB"
                    )
                
                if current_cpu > stress_config['cpu_limit_percent']:
                    results['bottlenecks'].append(
                        f"CPU limit exceeded: {current_cpu:.1f}% > {stress_config['cpu_limit_percent']}%"
                    )
                
                # Recovery attempt
                if results['errors'] > stress_config['max_errors'] * failure_tolerance:
                    results['recovery_attempts'] += 1
                    self._attempt_recovery()
                    if results['errors'] > 0:
                        results['successful_recoveries'] += 1
                
                # Sleep to control test pace
                time.sleep(stress_config['sampling_interval'])
        
        except KeyboardInterrupt:
            print(f"Stress test interrupted by user")
        
        return results
    
    def _attempt_recovery(self):
        """Attempt recovery from errors."""
        # Try to recover by:
        # 1. Collecting garbage
        # 2. Clearing caches
        # 3. Resetting brain state
        
        try:
            gc.collect()
            self.brain.reset()
        except Exception:
            pass
    
    def _generate_stress_summary(self, results: Dict[str, Any],
                                stress_config: Dict[str, Any]) -> Dict[str, Any]:
        """Generate stress test summary."""
        summary = {
            'test_passed': results['errors'] <= stress_config['max_errors'] * 0.1,
            'resource_utilization': self._calculate_resource_utilization(results, stress_config),
            'performance_characteristics': self._analyze_performance_characteristics(results),
            'bottleneck_severity': self._assess_bottleneck_severity(results),
            'recovery_effectiveness': self._calculate_recovery_effectiveness(results)
        }
        
        return summary
    
    def _calculate_resource_utilization(self, results: Dict[str, Any],
                                       stress_config: Dict[str, Any]) -> Dict[str, float]:
        """Calculate resource utilization."""
        if not results['memory_samples']:
            return {'memory': 0.0, 'cpu': 0.0, 'steps': 0.0}
        
        memory_util = np.mean(results['memory_samples']) / stress_config['memory_limit_mb']
        cpu_util = np.mean(results['cpu_samples']) / stress_config['cpu_limit_percent']
        
        return {
            'memory': min(memory_util, 1.0),
            'cpu': min(cpu_util, 1.0),
            'steps': results['steps_completed'] / stress_config['max_steps'] if stress_config['max_steps'] > 0 else 0.0
        }
    
    def _analyze_performance_characteristics(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze performance characteristics."""
        if not results['step_times']:
            return {'avg_step_time': 0.0, 'stability': 0.0}
        
        return {
            'avg_step_time': np.mean(results['step_times']),
            'stability': 1.0 - np.std(results['step_times']) / np.mean(results['step_times']) if np.mean(results['step_times']) > 0 else 0.0,
            'min_step_time': np.min(results['step_times']),
            'max_step_time': np.max(results['step_times'])
        }
    
    def _assess_bottleneck_severity(self, results: Dict[str, Any]) -> str:
        """Assess bottleneck severity."""
        severity_score = 0
        
        # Check for high error rate
        if results['steps_completed'] > 0:
            error_rate = results['errors'] / results['steps_completed']
            if error_rate > 0.1:
                severity_score += 3
            elif error_rate > 0.05:
                severity_score += 2
            elif error_rate > 0.01:
                severity_score += 1
        
        # Check for memory bottlenecks
        if any(m > 2048 for m in results['memory_samples']):
            severity_score += 2
        
        # Check for CPU bottlenecks
        if any(c > 90 for c in results['cpu_samples']):
            severity_score += 2
        
        # Check for performance bottlenecks
        if any(t > 1.0 for t in results['step_times']):
            severity_score += 1
        
        if severity_score >= 5:
            return 'critical'
        elif severity_score >= 3:
            return 'moderate'
        elif severity_score >= 1:
            return 'mild'
        else:
            return 'none'
    
    def _calculate_recovery_effectiveness(self, results: Dict[str, Any]) -> float:
        """Calculate recovery effectiveness."""
        if results['recovery_attempts'] == 0:
            return 1.0 if results['errors'] == 0 else 0.0
        
        return results['successful_recoveries'] / results['recovery_attempts']

# Convenience functions for benchmarking and profiling
def createBenchmarkSuite(brain: pynlm.Brain) -> BenchmarkSuite:
    """Create benchmark suite."""
    return BenchmarkSuite(brain)

def createProfiler(brain: pynlm.Brain) -> Profiler:
    """Create profiler."""
    return Profiler(brain)

def createPerformanceAnalyzer(benchmark_suite: BenchmarkSuite) -> PerformanceAnalyzer:
    """Create performance analyzer."""
    return PerformanceAnalyzer(benchmark_suite)

def createStressTester(brain: pynlm.Brain) -> StressTester:
    """Create stress tester."""
    return StressTester(brain)