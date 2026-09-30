#!/usr/bin/env python3
"""
Performance monitoring and profiling tools for NLM Python API

This module provides comprehensive performance monitoring, profiling,
and benchmarking tools for the NLM neural simulation framework.

Usage:
    import nlm
    from nlm.profiling import PerformanceProfiler, MemoryMonitor, BenchmarkSuite
    
    # Create performance profiler
    profiler = PerformanceProfiler()
    
    # Run benchmark
    result = profiler.benchmark(brain_function, num_iterations=100)
"""

import time
import psutil
import tracemalloc
import threading
import numpy as np
from typing import Dict, List, Any, Optional, Callable, Union
from datetime import datetime
import os
import gc
from dataclasses import dataclass, asdict
from enum import Enum

# Import NLM core classes
from .pynlm import (
    Config, 
    Brain, 
    SimpleWorld, 
    Action,
    ActionType
)

class PerformanceLevel(Enum):
    """Performance monitoring levels."""
    BASIC = "basic"
    DETAILED = "detailed"
    COMPREHENSIVE = "comprehensive"

@dataclass
class MemoryMetrics:
    """Memory usage metrics."""
    timestamp: float
    rss_mb: float
    vms_mb: float
    shared_mb: float
    text_mb: float
    lib_mb: float
    data_mb: float
    dirty_mb: float
    python_objects: int
    numpy_arrays: int

@dataclass
class CPUMetrics:
    """CPU usage metrics."""
    timestamp: float
    cpu_percent: float
    cpu_count: int
    cpu_frequency_mhz: float
    load_1min: float
    load_5min: float
    load_15min: float

@dataclass
class SimulationMetrics:
    """Neural simulation metrics."""
    timestamp: float
    simulation_time: float
    step_time_ms: float
    total_spike_count: int
    active_neuron_count: int
    firing_neuron_count: int
    average_firing_rate: float
    excitation_inhibition_ratio: float
    memory_usage_mb: float
    energy_consumed: float

@dataclass
class BenchmarkResult:
    """Benchmark test result."""
    test_name: str
    execution_time: float
    memory_usage: float
    cpu_usage: float
    success: bool
    error_message: Optional[str] = None
    metrics: Optional[Dict] = None

class MemoryMonitor:
    """Real-time memory monitoring for NLM systems.
    
    This class provides continuous monitoring of memory usage,
    including Python objects, NumPy arrays, and system memory.
    
    Attributes:
        sampling_interval (float): Sampling interval in seconds
        metrics_history (list): Historical memory metrics
        monitoring (bool): Whether monitoring is active
    """
    
    def __init__(self, sampling_interval: float = 0.1):
        """Initialize memory monitor.
        
        Args:
            sampling_interval (float): Sampling interval in seconds
        """
        self.sampling_interval = sampling_interval
        self.metrics_history: List[MemoryMetrics] = []
        self.monitoring = False
        self.monitor_thread = None
        
        # Start memory tracing
        tracemalloc.start()
    
    def get_current_memory(self) -> MemoryMetrics:
        """Get current memory usage metrics.
        
        Returns:
            MemoryMetrics: Current memory metrics
        """
        process = psutil.Process()
        
        # Get system memory info
        system_memory = psutil.virtual_memory()
        
        # Get process memory info
        memory_info = process.memory_info()
        
        # Get memory breakdown
        memory_full_info = process.memory_full_info()
        
        # Get Python object count
        python_objects = len(gc.get_objects())
        
        # Get NumPy array count
        numpy_arrays = len([obj for obj in gc.get_objects() 
                           if isinstance(obj, np.ndarray)])
        
        # Get traceback snapshot for memory tracking
        current, peak = tracemalloc.get_traced_memory()
        
        return MemoryMetrics(
            timestamp=time.time(),
            rss_mb=memory_info.rss / 1024 / 1024,
            vms_mb=memory_info.vms / 1024 / 1024,
            shared_mb=memory_info.shared / 1024 / 1024 if hasattr(memory_info, 'shared') else 0,
            text_mb=0,  # Not available in Python
            lib_mb=0,   # Not available in Python
            data_mb=memory_info.data / 1024 / 1024 if hasattr(memory_info, 'data') else 0,
            dirty_mb=memory_full_info.dirty / 1024 / 1024 if hasattr(memory_full_info, 'dirty') else 0,
            python_objects=python_objects,
            numpy_arrays=numpy_arrays
        )
    
    def start_monitoring(self):
        """Start continuous memory monitoring."""
        if self.monitoring:
            return
        
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print("Memory monitoring started")
    
    def stop_monitoring(self):
        """Stop memory monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        
        print("Memory monitoring stopped")
    
    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.monitoring:
            metrics = self.get_current_memory()
            self.metrics_history.append(metrics)
            time.sleep(self.sampling_interval)
    
    def get_average_memory(self) -> MemoryMetrics:
        """Get average memory metrics from history.
        
        Returns:
            MemoryMetrics: Average memory metrics
        """
        if not self.metrics_history:
            return self.get_current_memory()
        
        # Calculate averages
        avg_rss = np.mean([m.rss_mb for m in self.metrics_history])
        avg_vms = np.mean([m.vms_mb for m in self.metrics_history])
        avg_shared = np.mean([m.shared_mb for m in self.metrics_history])
        avg_data = np.mean([m.data_mb for m in self.metrics_history])
        avg_dirty = np.mean([m.dirty_mb for m in self.metrics_history])
        avg_python_objects = np.mean([m.python_objects for m in self.metrics_history])
        avg_numpy_arrays = np.mean([m.numpy_arrays for m in self.metrics_history])
        
        return MemoryMetrics(
            timestamp=time.time(),
            rss_mb=avg_rss,
            vms_mb=avg_vms,
            shared_mb=avg_shared,
            text_mb=0,
            lib_mb=0,
            data_mb=avg_data,
            dirty_mb=avg_dirty,
            python_objects=int(avg_python_objects),
            numpy_arrays=int(avg_numpy_arrays)
        )
    
    def get_peak_memory(self) -> MemoryMetrics:
        """Get peak memory metrics from history.
        
        Returns:
            MemoryMetrics: Peak memory metrics
        """
        if not self.metrics_history:
            return self.get_current_memory()
        
        max_rss = max(m.rss_mb for m in self.metrics_history)
        max_vms = max(m.vms_mb for m in self.metrics_history)
        max_shared = max(m.shared_mb for m in self.metrics_history)
        max_data = max(m.data_mb for m in self.metrics_history)
        max_dirty = max(m.dirty_mb for m in self.metrics_history)
        max_python_objects = max(m.python_objects for m in self.metrics_history)
        max_numpy_arrays = max(m.numpy_arrays for m in self.metrics_history)
        
        return MemoryMetrics(
            timestamp=time.time(),
            rss_mb=max_rss,
            vms_mb=max_vms,
            shared_mb=max_shared,
            text_mb=0,
            lib_mb=0,
            data_mb=max_data,
            dirty_mb=max_dirty,
            python_objects=max_python_objects,
            numpy_arrays=max_numpy_arrays
        )
    
    def clear_history(self):
        """Clear memory metrics history."""
        self.metrics_history.clear()
    
    def get_history_size(self) -> int:
        """Get size of metrics history."""
        return len(self.metrics_history)
    
    def save_history(self, filepath: str):
        """Save memory metrics history to file.
        
        Args:
            filepath (str): Path to save history
        """
        import json
        
        history_data = [asdict(metrics) for metrics in self.metrics_history]
        
        with open(filepath, 'w') as f:
            json.dump(history_data, f, indent=2, default=str)
    
    def load_history(self, filepath: str):
        """Load memory metrics history from file.
        
        Args:
            filepath (str): Path to history file
        """
        import json
        
        with open(filepath, 'r') as f:
            history_data = json.load(f)
        
        # Convert back to MemoryMetrics objects
        self.metrics_history = [MemoryMetrics(**data) for data in history_data]
    
    def __enter__(self):
        """Context manager entry."""
        self.start_monitoring()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.stop_monitoring()

class CPUMonitor:
    """Real-time CPU monitoring for NLM systems.
    
    This class provides continuous monitoring of CPU usage,
    including per-core statistics and system load.
    
    Attributes:
        sampling_interval (float): Sampling interval in seconds
        metrics_history (list): Historical CPU metrics
        monitoring (bool): Whether monitoring is active
    """
    
    def __init__(self, sampling_interval: float = 0.1):
        """Initialize CPU monitor.
        
        Args:
            sampling_interval (float): Sampling interval in seconds
        """
        self.sampling_interval = sampling_interval
        self.metrics_history: List[CPUMetrics] = []
        self.monitoring = False
        self.monitor_thread = None
    
    def get_current_cpu(self) -> CPUMetrics:
        """Get current CPU usage metrics.
        
        Returns:
            CPUMetrics: Current CPU metrics
        """
        # Get CPU usage
        cpu_percent = psutil.cpu_percent(interval=0.1)
        
        # Get CPU info
        cpu_info = psutil.cpu_freq()
        
        # Get load averages
        load_1min, load_5min, load_15min = psutil.getloadavg()
        
        return CPUMetrics(
            timestamp=time.time(),
            cpu_percent=cpu_percent,
            cpu_count=psutil.cpu_count(),
            cpu_frequency_mhz=cpu_info.current if cpu_info else 0,
            load_1min=load_1min,
            load_5min=load_5min,
            load_15min=load_15min
        )
    
    def start_monitoring(self):
        """Start continuous CPU monitoring."""
        if self.monitoring:
            return
        
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print("CPU monitoring started")
    
    def stop_monitoring(self):
        """Stop CPU monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        
        print("CPU monitoring stopped")
    
    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.monitoring:
            metrics = self.get_current_cpu()
            self.metrics_history.append(metrics)
            time.sleep(self.sampling_interval)
    
    def get_average_cpu(self) -> CPUMetrics:
        """Get average CPU metrics from history.
        
        Returns:
            CPUMetrics: Average CPU metrics
        """
        if not self.metrics_history:
            return self.get_current_cpu()
        
        # Calculate averages
        avg_cpu = np.mean([m.cpu_percent for m in self.metrics_history])
        avg_load_1min = np.mean([m.load_1min for m in self.metrics_history])
        avg_load_5min = np.mean([m.load_5min for m in self.metrics_history])
        avg_load_15min = np.mean([m.load_15min for m in self.metrics_history])
        
        return CPUMetrics(
            timestamp=time.time(),
            cpu_percent=avg_cpu,
            cpu_count=psutil.cpu_count(),
            cpu_frequency_mhz=0,
            load_1min=avg_load_1min,
            load_5min=avg_load_5min,
            load_15min=avg_load_15min
        )
    
    def clear_history(self):
        """Clear CPU metrics history."""
        self.metrics_history.clear()
    
    def __enter__(self):
        """Context manager entry."""
        self.start_monitoring()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.stop_monitoring()

class SimulationProfiler:
    """Neural simulation profiling for NLM systems.
    
    This class provides profiling of neural simulation performance,
    including step timing, neuron activity, and resource usage.
    
    Attributes:
        sampling_interval (float): Sampling interval in seconds
        metrics_history (list): Historical simulation metrics
        profiling (bool): Whether profiling is active
    """
    
    def __init__(self, sampling_interval: float = 0.01):
        """Initialize simulation profiler.
        
        Args:
            sampling_interval (float): Sampling interval in seconds
        """
        self.sampling_interval = sampling_interval
        self.metrics_history: List[SimulationMetrics] = []
        self.profiling = False
        self.profiler_thread = None
    
    def get_current_simulation_metrics(self, brain: Brain, world: Optional[SimpleWorld] = None) -> SimulationMetrics:
        """Get current simulation metrics.
        
        Args:
            brain (Brain): The brain to profile
            world (SimpleWorld, optional): The world to profile
            
        Returns:
            SimulationMetrics: Current simulation metrics
        """
        start_time = time.time()
        
        # Get brain statistics
        total_spike_count = brain.getTotalSpikeCount()
        active_neuron_count = brain.getActiveNeuronCount()
        firing_neuron_count = brain.getFiringNeuronCount()
        average_firing_rate = brain.getAverageFiringRate()
        excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
        
        # Get memory usage
        memory_info = psutil.Process().memory_info()
        memory_usage_mb = memory_info.rss / 1024 / 1024
        
        # Calculate energy (simplified)
        energy_consumed = 0.0
        
        simulation_time = time.time() - start_time
        
        return SimulationMetrics(
            timestamp=time.time(),
            simulation_time=simulation_time,
            step_time_ms=simulation_time * 1000,
            total_spike_count=total_spike_count,
            active_neuron_count=active_neuron_count,
            firing_neuron_count=firing_neuron_count,
            average_firing_rate=average_firing_rate,
            excitation_inhibition_ratio=excitation_inhibition_ratio,
            memory_usage_mb=memory_usage_mb,
            energy_consumed=energy_consumed
        )
    
    def start_profiling(self):
        """Start simulation profiling."""
        if self.profiling:
            return
        
        self.profiling = True
        self.profiler_thread = threading.Thread(target=self._profile_loop)
        self.profiler_thread.daemon = True
        self.profiler_thread.start()
        
        print("Simulation profiling started")
    
    def stop_profiling(self):
        """Stop simulation profiling."""
        self.profiling = False
        if self.profiler_thread:
            self.profiler_thread.join(timeout=1.0)
            self.profiler_thread = None
        
        print("Simulation profiling stopped")
    
    def _profile_loop(self):
        """Main profiling loop."""
        while self.profiling:
            time.sleep(self.sampling_interval)
    
    def profile_step(self, brain: Brain, step_func: Callable, *args, **kwargs) -> Dict:
        """Profile a single simulation step.
        
        Args:
            brain (Brain): The brain to profile
            step_func (callable): Function to profile
            *args: Arguments to pass to step_func
            **kwargs: Keyword arguments to pass to step_func
            
        Returns:
            dict: Step profiling results
        """
        start_time = time.perf_counter()
        
        # Start memory tracing
        tracemalloc.start()
        
        # Execute step
        step_func(*args, **kwargs)
        
        # Get memory snapshot
        current, peak = tracemalloc.get_traced_memory()
        
        # Stop memory tracing
        tracemalloc.stop()
        
        end_time = time.perf_counter()
        
        execution_time = end_time - start_time
        
        # Get metrics
        metrics = self.get_current_simulation_metrics(brain)
        
        return {
            'execution_time_ms': execution_time * 1000,
            'memory_current_mb': current / 1024 / 1024,
            'memory_peak_mb': peak / 1024 / 1024,
            'metrics': asdict(metrics)
        }
    
    def profile_simulation(self, brain: Brain, world: SimpleWorld, num_steps: int) -> List[Dict]:
        """Profile a complete simulation.
        
        Args:
            brain (Brain): The brain to profile
            world (SimpleWorld): The world to simulate
            num_steps (int): Number of steps to simulate
            
        Returns:
            list: Simulation profiling results
        """
        results = []
        
        for step in range(num_steps):
            step_start = time.perf_counter()
            
            # Execute step
            world.update(0.01)
            
            step_end = time.perf_counter()
            
            step_time = step_end - step_start
            metrics = self.get_current_simulation_metrics(brain, world)
            
            results.append({
                'step': step,
                'step_time_ms': step_time * 1000,
                'execution_time_ms': metrics.simulation_time * 1000,
                'total_spike_count': metrics.total_spike_count,
                'active_neuron_count': metrics.active_neuron_count,
                'firing_neuron_count': metrics.firing_neuron_count,
                'average_firing_rate': metrics.average_firing_rate,
                'excitation_inhibition_ratio': metrics.excitation_inhibition_ratio,
                'memory_usage_mb': metrics.memory_usage_mb,
                'energy_consumed': metrics.energy_consumed
            })
            
            time.sleep(0.01)  # Small delay to match sampling interval
        
        return results
    
    def clear_history(self):
        """Clear simulation metrics history."""
        self.metrics_history.clear()
    
    def get_average_metrics(self) -> SimulationMetrics:
        """Get average simulation metrics from history.
        
        Returns:
            SimulationMetrics: Average simulation metrics
        """
        if not self.metrics_history:
            return SimulationMetrics(
                timestamp=time.time(),
                simulation_time=0,
                step_time_ms=0,
                total_spike_count=0,
                active_neuron_count=0,
                firing_neuron_count=0,
                average_firing_rate=0,
                excitation_inhibition_ratio=0,
                memory_usage_mb=0,
                energy_consumed=0
            )
        
        # Calculate averages
        avg_simulation_time = np.mean([m.simulation_time for m in self.metrics_history])
        avg_step_time = np.mean([m.step_time_ms for m in self.metrics_history])
        avg_total_spike_count = np.mean([m.total_spike_count for m in self.metrics_history])
        avg_active_neuron_count = np.mean([m.active_neuron_count for m in self.metrics_history])
        avg_firing_neuron_count = np.mean([m.firing_neuron_count for m in self.metrics_history])
        avg_average_firing_rate = np.mean([m.average_firing_rate for m in self.metrics_history])
        avg_excitation_inhibition_ratio = np.mean([m.excitation_inhibition_ratio for m in self.metrics_history])
        avg_memory_usage_mb = np.mean([m.memory_usage_mb for m in self.metrics_history])
        avg_energy_consumed = np.mean([m.energy_consumed for m in self.metrics_history])
        
        return SimulationMetrics(
            timestamp=time.time(),
            simulation_time=avg_simulation_time,
            step_time_ms=avg_step_time,
            total_spike_count=int(avg_total_spike_count),
            active_neuron_count=int(avg_active_neuron_count),
            firing_neuron_count=int(avg_firing_neuron_count),
            average_firing_rate=avg_average_firing_rate,
            excitation_inhibition_ratio=avg_excitation_inhibition_ratio,
            memory_usage_mb=avg_memory_usage_mb,
            energy_consumed=avg_energy_consumed
        )
    
    def save_results(self, filepath: str):
        """Save profiling results to file.
        
        Args:
            filepath (str): Path to save results
        """
        import json
        
        history_data = [asdict(metrics) for metrics in self.metrics_history]
        
        with open(filepath, 'w') as f:
            json.dump(history_data, f, indent=2, default=str)
    
    def load_results(self, filepath: str):
        """Load profiling results from file.
        
        Args:
            filepath (str): Path to results file
        """
        import json
        
        with open(filepath, 'r') as f:
            history_data = json.load(f)
        
        # Convert back to SimulationMetrics objects
        self.metrics_history = [SimulationMetrics(**data) for data in history_data]

class PerformanceProfiler:
    """Comprehensive performance profiler for NLM systems.
    
    This class provides comprehensive performance profiling, including
    memory monitoring, CPU monitoring, simulation profiling, and
    benchmark testing.
    
    Attributes:
        memory_monitor (MemoryMonitor): Memory monitor instance
        cpu_monitor (CPUMonitor): CPU monitor instance
        simulation_profiler (SimulationProfiler): Simulation profiler instance
        benchmark_results (list): Benchmark results
    """
    
    def __init__(self, sampling_interval: float = 0.1):
        """Initialize performance profiler.
        
        Args:
            sampling_interval (float): Sampling interval for monitoring
        """
        self.memory_monitor = MemoryMonitor(sampling_interval)
        self.cpu_monitor = CPUMonitor(sampling_interval)
        self.simulation_profiler = SimulationProfiler(sampling_interval)
        self.benchmark_results: List[BenchmarkResult] = []
    
    def start_profiling(self):
        """Start comprehensive profiling."""
        self.memory_monitor.start_monitoring()
        self.cpu_monitor.start_monitoring()
        self.simulation_profiler.start_profiling()
        
        print("Performance profiling started")
    
    def stop_profiling(self):
        """Stop comprehensive profiling."""
        self.memory_monitor.stop_monitoring()
        self.cpu_monitor.stop_monitoring()
        self.simulation_profiler.stop_profiling()
        
        print("Performance profiling stopped")
    
    def benchmark_function(self, func: Callable, *args, **kwargs) -> BenchmarkResult:
        """Benchmark a single function.
        
        Args:
            func (callable): Function to benchmark
            *args: Arguments to pass to function
            **kwargs: Keyword arguments to pass to function
            
        Returns:
            BenchmarkResult: Benchmark result
        """
        start_time = time.perf_counter()
        start_memory = psutil.Process().memory_info().rss
        start_cpu = psutil.cpu_percent()
        
        try:
            result = func(*args, **kwargs)
            
            end_time = time.perf_counter()
            end_memory = psutil.Process().memory_info().rss
            end_cpu = psutil.cpu_percent()
            
            execution_time = end_time - start_time
            memory_usage = (end_memory - start_memory) / 1024 / 1024
            cpu_usage = end_cpu - start_cpu
            
            benchmark_result = BenchmarkResult(
                test_name=func.__name__,
                execution_time=execution_time,
                memory_usage=memory_usage,
                cpu_usage=cpu_usage,
                success=True,
                metrics={
                    'start_memory_mb': start_memory / 1024 / 1024,
                    'end_memory_mb': end_memory / 1024 / 1024,
                    'memory_delta_mb': memory_usage,
                    'start_cpu_percent': start_cpu,
                    'end_cpu_percent': end_cpu,
                    'cpu_delta_percent': cpu_usage
                }
            )
            
        except Exception as e:
            end_time = time.perf_counter()
            execution_time = end_time - start_time
            
            benchmark_result = BenchmarkResult(
                test_name=func.__name__,
                execution_time=execution_time,
                memory_usage=0,
                cpu_usage=0,
                success=False,
                error_message=str(e)
            )
        
        self.benchmark_results.append(benchmark_result)
        
        return benchmark_result
    
    def benchmark_simulation(self, brain: Brain, world: SimpleWorld, num_steps: int, num_runs: int = 10) -> List[BenchmarkResult]:
        """Benchmark neural simulation.
        
        Args:
            brain (Brain): Brain to simulate
            world (SimpleWorld): World to simulate
            num_steps (int): Number of steps per simulation
            num_runs (int): Number of simulation runs
            
        Returns:
            list: Benchmark results
        """
        print(f"Running simulation benchmark: {num_runs} runs of {num_steps} steps")
        
        results = []
        
        for run in range(num_runs):
            print(f"Run {run + 1}/{num_runs}")
            
            # Reset brain and world
            brain.reset()
            world.reset()
            
            # Start memory tracing
            tracemalloc.start()
            
            start_time = time.perf_counter()
            
            # Run simulation
            for step in range(num_steps):
                world.update(0.01)
                time.sleep(0.001)  # Small delay
            
            end_time = time.perf_counter()
            
            # Get memory stats
            current, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
            execution_time = end_time - start_time
            memory_usage = peak / 1024 / 1024
            
            benchmark_result = BenchmarkResult(
                test_name=f"simulation_{num_steps}steps",
                execution_time=execution_time,
                memory_usage=memory_usage,
                cpu_usage=0,  # Simplified
                success=True,
                metrics={
                    'steps': num_steps,
                    'runs': num_runs,
                    'run': run,
                    'execution_time_ms': execution_time * 1000,
                    'memory_peak_mb': memory_usage,
                    'peak_spikes': brain.getTotalSpikeCount()
                }
            )
            
            results.append(benchmark_result)
            self.benchmark_results.append(benchmark_result)
        
        return results
    
    def run_suite(self, test_functions: List[Callable], suite_name: str = "default") -> Dict:
        """Run a suite of benchmark tests.
        
        Args:
            test_functions (list): List of test functions to run
            suite_name (str): Name of the test suite
            
        Returns:
            dict: Suite benchmark results
        """
        print(f"Running benchmark suite: {suite_name}")
        print(f"Running {len(test_functions)} tests")
        
        suite_results = {
            'suite_name': suite_name,
            'timestamp': datetime.now().isoformat(),
            'tests': []
        }
        
        for i, test_func in enumerate(test_functions):
            print(f"Running test {i + 1}/{len(test_functions)}: {test_func.__name__}")
            
            result = self.benchmark_function(test_func)
            suite_results['tests'].append(asdict(result))
        
        return suite_results
    
    def compare_results(self, result1: BenchmarkResult, result2: BenchmarkResult) -> Dict:
        """Compare two benchmark results.
        
        Args:
            result1 (BenchmarkResult): First benchmark result
            result2 (BenchmarkResult): Second benchmark result
            
        Returns:
            dict: Comparison results
        """
        if not result1.success or not result2.success:
            return {
                'comparison': 'invalid',
                'error': 'One or both results are not successful'
            }
        
        comparison = {
            'test_names': [result1.test_name, result2.test_name],
            'execution_time_comparison': {
                'result1': result1.execution_time,
                'result2': result2.execution_time,
                'difference': result2.execution_time - result1.execution_time,
                'percentage_change': ((result2.execution_time - result1.execution_time) / result1.execution_time * 100) if result1.execution_time > 0 else 0
            },
            'memory_usage_comparison': {
                'result1': result1.memory_usage,
                'result2': result2.memory_usage,
                'difference': result2.memory_usage - result1.memory_usage,
                'percentage_change': ((result2.memory_usage - result1.memory_usage) / result1.memory_usage * 100) if result1.memory_usage > 0 else 0
            },
            'cpu_usage_comparison': {
                'result1': result1.cpu_usage,
                'result2': result2.cpu_usage,
                'difference': result2.cpu_usage - result1.cpu_usage,
                'percentage_change': ((result2.cpu_usage - result1.cpu_usage) / result1.cpu_usage * 100) if result1.cpu_usage > 0 else 0
            },
            'winner': None
        }
        
        # Determine winner based on overall performance
        time_better = result2.execution_time < result1.execution_time
        memory_better = result2.memory_usage < result1.memory_usage
        cpu_better = result2.cpu_usage < result1.cpu_usage
        
        if time_better and memory_better and cpu_better:
            comparison['winner'] = 2
        elif time_better and not memory_better and not cpu_better:
            comparison['winner'] = 2 if memory_better else 1
        elif memory_better and not time_better and not cpu_better:
            comparison['winner'] = 2 if time_better else 1
        elif cpu_better and not time_better and not memory_better:
            comparison['winner'] = 2 if (time_better and memory_better) else 1
        else:
            comparison['winner'] = 0  # Draw or inconclusive
        
        return comparison
    
    def get_summary(self) -> Dict:
        """Get benchmark summary.
        
        Returns:
            dict: Benchmark summary
        """
        if not self.benchmark_results:
            return {'error': 'No benchmark results available'}
        
        successful_results = [r for r in self.benchmark_results if r.success]
        failed_results = [r for r in self.benchmark_results if not r.success]
        
        summary = {
            'total_tests': len(self.benchmark_results),
            'successful_tests': len(successful_results),
            'failed_tests': len(failed_results),
            'success_rate': len(successful_results) / len(self.benchmark_results) if self.benchmark_results else 0,
            'average_execution_time': np.mean([r.execution_time for r in successful_results]) if successful_results else 0,
            'average_memory_usage': np.mean([r.memory_usage for r in successful_results]) if successful_results else 0,
            'average_cpu_usage': np.mean([r.cpu_usage for r in successful_results]) if successful_results else 0,
            'fastest_test': min(successful_results, key=lambda x: x.execution_time).test_name if successful_results else None,
            'memory_efficient_test': min(successful_results, key=lambda x: x.memory_usage).test_name if successful_results else None,
            'cpu_efficient_test': min(successful_results, key=lambda x: x.cpu_usage).test_name if successful_results else None
        }
        
        return summary
    
    def save_results(self, filepath: str):
        """Save benchmark results to file.
        
        Args:
            filepath (str): Path to save results
        """
        import json
        
        # Convert benchmark results to serializable format
        results_data = []
        for result in self.benchmark_results:
            result_dict = asdict(result)
            # Convert datetime objects to strings
            if result_dict.get('timestamp'):
                result_dict['timestamp'] = result_dict['timestamp'].isoformat()
            results_data.append(result_dict)
        
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2, default=str)
    
    def load_results(self, filepath: str):
        """Load benchmark results from file.
        
        Args:
            filepath (str): Path to results file
        """
        import json
        
        with open(filepath, 'r') as f:
            results_data = json.load(f)
        
        # Convert back to BenchmarkResult objects
        self.benchmark_results = []
        for result_data in results_data:
            # Handle timestamp conversion
            if 'timestamp' in result_data and isinstance(result_data['timestamp'], str):
                result_data['timestamp'] = datetime.fromisoformat(result_data['timestamp'])
            
            self.benchmark_results.append(BenchmarkResult(**result_data))
    
    def generate_report(self) -> str:
        """Generate a text report of benchmark results.
        
        Returns:
            str: Formatted report
        """
        summary = self.get_summary()
        
        report = f"Benchmark Report\n"
        report += f"{'='*50}\n"
        report += f"Total Tests: {summary['total_tests']}\n"
        report += f"Successful Tests: {summary['successful_tests']}\n"
        report += f"Failed Tests: {summary['failed_tests']}\n"
        report += f"Success Rate: {summary['success_rate']:.2%}\n"
        report += f"Average Execution Time: {summary['average_execution_time']:.4f}s\n"
        report += f"Average Memory Usage: {summary['average_memory_usage']:.2f}MB\n"
        report += f"Average CPU Usage: {summary['average_cpu_usage']:.2f}%\n"
        report += f"Fastest Test: {summary['fastest_test']}\n"
        report += f"Most Memory Efficient Test: {summary['memory_efficient_test']}\n"
        report += f"Most CPU Efficient Test: {summary['cpu_efficient_test']}\n"
        
        return report
    
    def __enter__(self):
        """Context manager entry."""
        self.start_profiling()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.stop_profiling()