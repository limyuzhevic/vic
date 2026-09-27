#!/usr/bin/env python3
"""
NLM Performance Profiling Tools

Advanced performance monitoring and profiling tools for NLM (Neural Learning Machine) simulations.
Provides comprehensive performance analysis including memory usage tracking, 
CPU performance metrics, simulation step timing, and resource consumption monitoring.

Key Features:
- Real-time performance monitoring
- Memory usage tracking with detailed statistics
- CPU utilization monitoring
- Step-by-step timing analysis
- Performance benchmarking
- Bottleneck identification
- Performance recommendations
"""

import time
import threading
import psutil
import numpy as np
from typing import Dict, List, Optional, Any, Callable, Tuple
import logging
import sys
import os
from collections import defaultdict, deque
from dataclasses import dataclass, field
from enum import Enum
import json
import psutil
from datetime import datetime

logger = logging.getLogger(__name__)


class PerformanceMetricType(Enum):
    """Enumeration of performance metric types."""
    MEMORY_USAGE = "memory_usage"
    CPU_USAGE = "cpu_usage"
    SIMULATION_STEP = "simulation_step"
    GPU_USAGE = "gpu_usage"
    BANDWIDTH = "bandwidth"
    LATENCY = "latency"
    THROUGHPUT = "throughput"


@dataclass
class PerformanceSample:
    """Represents a single performance measurement sample."""
    timestamp: float
    value: float
    unit: str
    metric_type: PerformanceMetricType
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class PerformanceStats:
    """Statistical summary of performance data."""
    count: int
    mean: float
    median: float
    std: float
    min: float
    max: float
    p25: float
    p50: float
    p75: float
    p95: float
    p99: float
    skew: float = 0.0
    kurtosis: float = 0.0


class MetricCollector:
    """Collects and manages performance metrics."""
    
    def __init__(self, name: str, metric_type: PerformanceMetricType, 
                 sampling_interval: float = 0.1, max_history: int = 10000):
        """
        Initialize metric collector.
        
        Args:
            name: Name of the metric
            metric_type: Type of metric
            sampling_interval: Sampling interval in seconds
            max_history: Maximum number of samples to store
        """
        self.name = name
        self.metric_type = metric_type
        self.sampling_interval = sampling_interval
        self.max_history = max_history
        
        self.samples: List[PerformanceSample] = deque(maxlen=max_history)
        self.is_collecting = False
        self.collection_thread: Optional[threading.Thread] = None
        self.lock = threading.Lock()
        self.logger = logging.getLogger(f"{__name__}.metric_collector")
    
    def start(self) -> None:
        """Start collecting metrics."""
        if self.is_collecting:
            return
        
        self.is_collecting = True
        self.collection_thread = threading.Thread(target=self._collect_loop, daemon=True)
        self.collection_thread.start()
        self.logger.info(f"Started collecting metric: {self.name}")
    
    def stop(self) -> None:
        """Stop collecting metrics."""
        self.is_collecting = False
        if self.collection_thread:
            self.collection_thread.join(timeout=5.0)
        self.logger.info(f"Stopped collecting metric: {self.name}")
    
    def add_sample(self, value: float, unit: str = "", metadata: Dict[str, Any] = None) -> None:
        """
        Add a performance sample.
        
        Args:
            value: Metric value
            unit: Unit of measurement
            metadata: Additional metadata
        """
        with self.lock:
            sample = PerformanceSample(
                timestamp=time.time(),
                value=value,
                unit=unit,
                metric_type=self.metric_type,
                metadata=metadata or {}
            )
            self.samples.append(sample)
    
    def _collect_loop(self) -> None:
        """Continuous collection loop."""
        while self.is_collecting:
            try:
                self._collect_sample()
                time.sleep(self.sampling_interval)
            except Exception as e:
                self.logger.error(f"Error in collection loop for {self.name}: {e}")
                time.sleep(self.sampling_interval)
    
    def _collect_sample(self) -> None:
        """Collect a single sample based on metric type."""
        if self.metric_type == PerformanceMetricType.MEMORY_USAGE:
            process = psutil.Process()
            memory_info = process.memory_info()
            self.add_sample(memory_info.rss / 1024 / 1024, "MB", {"type": "rss"})
            self.add_sample(memory_info.vms / 1024 / 1024, "MB", {"type": "vms"})
        
        elif self.metric_type == PerformanceMetricType.CPU_USAGE:
            cpu_percent = psutil.cpu_percent(interval=0.1)
            self.add_sample(cpu_percent, "%", {"core": "total"})
        
        elif self.metric_type == PerformanceMetricType.GPU_USAGE:
            try:
                import pynvml
                pynvml.nvmlInit()
                handle = pynvml.nvmlDeviceGetHandleByIndex(0)
                utilization = pynvml.nvmlDeviceGetUtilizationRates(handle)
                self.add_sample(utilization.gpu, "%", {"gpu": 0})
                self.add_sample(utilization.memory, "%", {"gpu": 0, "type": "memory"})
            except ImportError:
                pass
            except Exception as e:
                self.logger.debug(f"Could not get GPU metrics: {e}")
        
        elif self.metric_type == PerformanceMetricType.BANDWIDTH:
            net_io = psutil.net_io_counters()
            # Calculate bytes per second (simplified)
            self.add_sample(net_io.bytes_sent, "bytes", {"direction": "sent"})
            self.add_sample(net_io.bytes_recv, "bytes", {"direction": "received"})
        
        elif self.metric_type == PerformanceMetricType.LATENCY:
            # This would be populated by external metrics
            pass
    
    def get_stats(self) -> PerformanceStats:
        """
        Get statistical summary of collected samples.
        
        Returns:
            PerformanceStats object
        """
        with self.lock:
            if not self.samples:
                return PerformanceStats(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
            
            values = [s.value for s in self.samples]
            return self._calculate_stats(values)
    
    def get_samples(self, filter_func: Optional[Callable[[PerformanceSample], bool]] = None) -> List[PerformanceSample]:
        """
        Get samples, optionally filtered.
        
        Args:
            filter_func: Function to filter samples
            
        Returns:
            List of PerformanceSample objects
        """
        with self.lock:
            samples = list(self.samples)
            if filter_func:
                samples = [s for s in samples if filter_func(s)]
            return samples
    
    def get_recent_samples(self, seconds: float = 60.0) -> List[PerformanceSample]:
        """
        Get samples from the last N seconds.
        
        Args:
            seconds: Time window in seconds
            
        Returns:
            List of recent samples
        """
        cutoff = time.time() - seconds
        return [s for s in self.get_samples() if s.timestamp >= cutoff]
    
    def clear(self) -> None:
        """Clear all collected samples."""
        with self.lock:
            self.samples.clear()
    
    @staticmethod
    def _calculate_stats(values: List[float]) -> PerformanceStats:
        """Calculate statistical summary for a list of values."""
        if not values:
            return PerformanceStats(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
        
        # Basic statistics
        count = len(values)
        mean = np.mean(values)
        median = np.median(values)
        std = np.std(values)
        min_val = np.min(values)
        max_val = np.max(values)
        
        # Percentiles
        p25 = np.percentile(values, 25)
        p50 = np.percentile(values, 50)
        p75 = np.percentile(values, 75)
        p95 = np.percentile(values, 95)
        p99 = np.percentile(values, 99)
        
        # Skewness and kurtosis
        try:
            skew = scipy.stats.skew(values) if len(values) >= 3 else 0.0
            kurtosis = scipy.stats.kurtosis(values) if len(values) >= 3 else 0.0
        except ImportError:
            skew = 0.0
            kurtosis = 0.0
        
        return PerformanceStats(
            count=count,
            mean=mean,
            median=median,
            std=std,
            min=min_val,
            max=max_val,
            p25=p25,
            p50=p50,
            p75=p75,
            p95=p95,
            p99=p99,
            skew=skew,
            kurtosis=kurtosis
        )


class SimulationProfiler:
    """Profiler for NLM simulation performance."""
    
    def __init__(self, brain=None):
        """
        Initialize simulation profiler.
        
        Args:
            brain: Optional pynlm.Brain instance to profile
        """
        self.brain = brain
        self.step_metrics = {}
        self.session_start_time = None
        self.session_end_time = None
        self.step_times = deque(maxlen=10000)
        self.metrics_collectors: Dict[str, MetricCollector] = {}
        self.logger = logging.getLogger(f"{__name__}.simulation_profiler")
        
        # Initialize common metrics
        self._setup_metrics()
    
    def _setup_metrics(self) -> None:
        """Set up default metrics collectors."""
        self.metrics_collectors['memory'] = MetricCollector(
            "memory_usage", PerformanceMetricType.MEMORY_USAGE, sampling_interval=0.5
        )
        self.metrics_collectors['cpu'] = MetricCollector(
            "cpu_usage", PerformanceMetricType.CPU_USAGE, sampling_interval=1.0
        )
        self.metrics_collectors['gpu'] = MetricCollector(
            "gpu_usage", PerformanceMetricType.GPU_USAGE, sampling_interval=2.0
        )
    
    def start_profiling(self) -> None:
        """Start performance profiling."""
        self.session_start_time = time.time()
        
        for collector in self.metrics_collectors.values():
            collector.start()
        
        self.logger.info("Started performance profiling")
    
    def stop_profiling(self) -> None:
        """Stop performance profiling."""
        self.session_end_time = time.time()
        
        for collector in self.metrics_collectors.values():
            collector.stop()
        
        self.logger.info("Stopped performance profiling")
    
    def record_step(self, step_num: int, step_time: float, 
                    additional_metrics: Dict[str, Any] = None) -> None:
        """
        Record simulation step performance.
        
        Args:
            step_num: Step number
            step_time: Time taken for step in seconds
            additional_metrics: Additional metrics to record
        """
        self.step_times.append(step_time)
        
        step_metric = {
            'step': step_num,
            'timestamp': time.time(),
            'step_time': step_time,
            'cpu_percent': psutil.cpu_percent(),
            'memory_rss': psutil.Process().memory_info().rss / 1024 / 1024,
            'memory_vms': psutil.Process().memory_info().vms / 1024 / 1024
        }
        
        if additional_metrics:
            step_metric.update(additional_metrics)
        
        self.step_metrics[step_num] = step_metric
        
        self.logger.debug(f"Recorded step {step_num}: {step_time:.4f}s")
    
    def get_step_statistics(self) -> PerformanceStats:
        """
        Get step time statistics.
        
        Returns:
            PerformanceStats for step times
        """
        if not self.step_times:
            return PerformanceStats(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
        
        return self._calculate_stats_from_values(list(self.step_times))
    
    def get_memory_statistics(self) -> PerformanceStats:
        """
        Get memory usage statistics.
        
        Returns:
            PerformanceStats for memory usage
        """
        collector = self.metrics_collectors.get('memory')
        if collector:
            return collector.get_stats()
        return PerformanceStats(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
    
    def get_cpu_statistics(self) -> PerformanceStats:
        """
        Get CPU usage statistics.
        
        Returns:
            PerformanceStats for CPU usage
        """
        collector = self.metrics_collectors.get('cpu')
        if collector:
            return collector.get_stats()
        return PerformanceStats(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
    
    def get_session_duration(self) -> float:
        """
        Get total session duration.
        
        Returns:
            Session duration in seconds
        """
        if not self.session_start_time:
            return 0.0
        
        end_time = self.session_end_time or time.time()
        return end_time - self.session_start_time
    
    def benchmark_configuration(self, config_data: Dict, 
                               steps: int = 100, warmup_steps: int = 10) -> Dict:
        """
        Benchmark a configuration.
        
        Args:
            config_data: Configuration data dictionary
            steps: Number of steps to benchmark
            warmup_steps: Number of warmup steps
            
        Returns:
            Dictionary containing benchmark results
        """
        if not self.brain:
            raise ValueError("Brain instance required for benchmarking")
        
        # Reset brain to initial state
        if hasattr(self.brain, 'reset'):
            self.brain.reset()
        
        # Warmup phase
        for step_num in range(warmup_steps):
            step_start = time.time()
            step = pynlm.SimulationStep(step_num)
            self.brain.step(step)
            step_time = time.time() - step_start
            self.record_step(step_num, step_time)
        
        # Benchmark phase
        step_times = []
        for step_num in range(steps):
            step_start = time.time()
            step = pynlm.SimulationStep(step_num)
            self.brain.step(step)
            step_time = time.time() - step_start
            step_times.append(step_time)
            self.record_step(step_num + warmup_steps, step_time)
        
        # Calculate statistics
        step_stats = self.get_step_statistics()
        
        return {
            'config': config_data,
            'steps': len(step_times),
            'step_times': step_times,
            'session_duration': self.get_session_duration(),
            'step_statistics': {
                'count': step_stats.count,
                'mean': step_stats.mean,
                'std': step_stats.std,
                'median': step_stats.median,
                'min': step_stats.min,
                'max': step_stats.max,
                'p95': step_stats.p95,
                'p99': step_stats.p99
            },
            'memory_statistics': {
                'mean': self.get_memory_statistics().mean,
                'max': self.get_memory_statistics().max
            },
            'cpu_statistics': {
                'mean': self.get_cpu_statistics().mean,
                'max': self.get_cpu_statistics().max
            }
        }
    
    def compare_performance(self, profilers: List['SimulationProfiler'],
                          labels: List[str]) -> Dict:
        """
        Compare performance across multiple profilers.
        
        Args:
            profilers: List of SimulationProfiler instances
            labels: List of labels for each profiler
            
        Returns:
            Dictionary containing comparative results
        """
        if len(profilers) != len(labels):
            raise ValueError("Number of profilers must match number of labels")
        
        comparison = {
            'labels': labels,
            'session_durations': [],
            'step_statistics': [],
            'memory_statistics': [],
            'cpu_statistics': []
        }
        
        for profiler, label in zip(profilers, labels):
            comparison['session_durations'].append(profiler.get_session_duration())
            comparison['step_statistics'].append({
                'mean': profiler.get_step_statistics().mean,
                'std': profiler.get_step_statistics().std,
                'min': profiler.get_step_statistics().min,
                'max': profiler.get_step_statistics().max
            })
            comparison['memory_statistics'].append({
                'mean': profiler.get_memory_statistics().mean,
                'max': profiler.get_memory_statistics().max
            })
            comparison['cpu_statistics'].append({
                'mean': profiler.get_cpu_statistics().mean,
                'max': profiler.get_cpu_statistics().max
            })
        
        return comparison
    
    def generate_report(self) -> Dict:
        """
        Generate comprehensive performance report.
        
        Returns:
            Dictionary containing performance report
        """
        step_stats = self.get_step_statistics()
        memory_stats = self.get_memory_statistics()
        cpu_stats = self.get_cpu_statistics()
        
        report = {
            'session_info': {
                'start_time': self.session_start_time,
                'end_time': self.session_end_time,
                'duration': self.get_session_duration(),
                'total_steps': len(self.step_times)
            },
            'step_performance': {
                'count': step_stats.count,
                'mean_time': step_stats.mean,
                'median_time': step_stats.median,
                'std_time': step_stats.std,
                'min_time': step_stats.min,
                'max_time': step_stats.max,
                'p95_time': step_stats.p95,
                'p99_time': step_stats.p99,
                'throughput': 1.0 / step_stats.mean if step_stats.mean > 0 else 0.0
            },
            'memory_performance': {
                'mean_memory': memory_stats.mean,
                'max_memory': memory_stats.max,
                'memory_growth': memory_stats.max - memory_stats.mean
            },
            'cpu_performance': {
                'mean_cpu': cpu_stats.mean,
                'max_cpu': cpu_stats.max,
                'cpu_efficiency': cpu_stats.mean / 100.0
            },
            'bottleneck_analysis': self._identify_bottlenecks(),
            'recommendations': self._generate_recommendations()
        }
        
        return report
    
    def export_data(self, filepath: str, format: str = 'json') -> bool:
        """
        Export profiling data to file.
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'pickle')
            
        Returns:
            True if successful
        """
        try:
            data = {
                'step_metrics': self.step_metrics,
                'step_times': list(self.step_times),
                'metrics': {}
            }
            
            for name, collector in self.metrics_collectors.items():
                data['metrics'][name] = {
                    'samples': [(s.timestamp, s.value, s.unit, s.metadata) for s in collector.samples],
                    'stats': collector.get_stats().__dict__
                }
            
            if format == 'json':
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2, default=str)
            elif format == 'csv':
                import pandas as pd
                df = pd.DataFrame(data['step_metrics'])
                df.to_csv(filepath, index=False)
            elif format == 'pickle':
                import pickle
                with open(filepath, 'wb') as f:
                    pickle.dump(data, f)
            else:
                raise ValueError(f"Unknown format: {format}")
            
            self.logger.info(f"Exported profiling data to {filepath}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to export data to {filepath}: {e}")
            return False
    
    def import_data(self, filepath: str, format: str = 'json') -> bool:
        """
        Import profiling data from file.
        
        Args:
            filepath: Input file path
            format: Import format ('json', 'csv', 'pickle')
            
        Returns:
            True if successful
        """
        try:
            if format == 'json':
                with open(filepath, 'r') as f:
                    data = json.load(f)
            elif format == 'csv':
                import pandas as pd
                df = pd.read_csv(filepath)
                data = {'step_metrics': df.to_dict('records')}
            elif format == 'pickle':
                import pickle
                with open(filepath, 'rb') as f:
                    data = pickle.load(f)
            else:
                raise ValueError(f"Unknown format: {format}")
            
            # Load data into profiler
            self.step_metrics = data.get('step_metrics', {})
            self.step_times = deque(data.get('step_times', []), maxlen=10000)
            
            # Restore metrics collectors data (simplified)
            # In practice, this would require more complex restoration
            
            self.logger.info(f"Imported profiling data from {filepath}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to import data from {filepath}: {e}")
            return False
    
    def _calculate_stats_from_values(self, values: List[float]) -> PerformanceStats:
        """Calculate PerformanceStats from raw values."""
        if not values:
            return PerformanceStats(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
        
        return self._calculate_stats(values)
    
    def _calculate_stats(self, values: List[float]) -> PerformanceStats:
        """Calculate statistics from values."""
        if not values:
            return PerformanceStats(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
        
        return PerformanceStats(
            count=len(values),
            mean=np.mean(values),
            median=np.median(values),
            std=np.std(values),
            min=np.min(values),
            max=np.max(values),
            p25=np.percentile(values, 25),
            p50=np.percentile(values, 50),
            p75=np.percentile(values, 75),
            p95=np.percentile(values, 95),
            p99=np.percentile(values, 99)
        )
    
    def _identify_bottlenecks(self) -> List[str]:
        """Identify potential performance bottlenecks."""
        bottlenecks = []
        
        step_stats = self.get_step_statistics()
        
        # Check for high variance in step times
        if step_stats.std > step_stats.mean * 0.5:
            bottlenecks.append("High variance in step times. Consider optimizing simulation parameters.")
        
        # Check for slow average step time
        if step_stats.mean > 1.0:
            bottlenecks.append("Average step time > 1s. Consider optimization or parallel processing.")
        
        # Check for long maximum step time
        if step_stats.max > step_stats.mean * 5:
            bottlenecks.append("Maximum step time is much higher than average. Identify outliers.")
        
        # Check memory usage
        memory_stats = self.get_memory_statistics()
        if memory_stats.mean > 500:  # MB
            bottlenecks.append("High memory usage. Consider memory optimization.")
        
        # Check CPU usage
        cpu_stats = self.get_cpu_statistics()
        if cpu_stats.mean < 10:
            bottlenecks.append("Low CPU utilization. Consider increasing workload or parallelism.")
        
        return bottlenecks
    
    def _generate_recommendations(self) -> List[str]:
        """Generate performance optimization recommendations."""
        recommendations = []
        
        step_stats = self.get_step_statistics()
        
        # Step time recommendations
        if step_stats.mean > 0.1:
            recommendations.append("Consider optimizing simulation step implementation for better performance.")
        
        if step_stats.p99 > step_stats.mean * 2:
            recommendations.append("Investigate slow outliers using profiling tools.")
        
        # Memory recommendations
        memory_stats = self.get_memory_statistics()
        if memory_stats.mean > 200:
            recommendations.append("Consider using memory-efficient data structures.")
        
        if memory_stats.max - memory_stats.mean > 300:
            recommendations.append("Investigate memory spikes during simulation.")
        
        # CPU recommendations
        cpu_stats = self.get_cpu_statistics()
        if cpu_stats.mean < 20:
            recommendations.append("Consider parallel processing to better utilize CPU resources.")
        
        if cpu_stats.max < 50:
            recommendations.append("System is underutilized. Consider increasing workload complexity.")
        
        # General recommendations
        if not recommendations:
            recommendations.append("Performance looks good! Consider increasing simulation scale.")
        
        return recommendations


# High-level API functions
def benchmark_brain_performance(brain, config_data: Dict, 
                               steps: int = 100, warmup_steps: int = 10) -> Dict:
    """
    Benchmark brain performance with easy-to-use API.
    
    Args:
        brain: pynlm.Brain instance
        config_data: Configuration data
        steps: Number of steps to run
        warmup_steps: Number of warmup steps
        
    Returns:
        Dictionary containing benchmark results
    """
    profiler = SimulationProfiler(brain)
    profiler.start_profiling()
    
    result = profiler.benchmark_configuration(config_data, steps, warmup_steps)
    
    profiler.stop_profiling()
    
    return result


def profile_simulation(brain, steps: int, warmup_steps: int = 10) -> Dict:
    """
    Profile simulation with automatic performance monitoring.
    
    Args:
        brain: pynlm.Brain instance
        steps: Number of steps to profile
        warmup_steps: Number of warmup steps
        
    Returns:
        Dictionary containing performance report
    """
    profiler = SimulationProfiler(brain)
    profiler.start_profiling()
    
    # Run simulation
    for step_num in range(warmup_steps, steps):
        step = pynlm.SimulationStep(step_num)
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        profiler.record_step(step_num, step_time)
    
    profiler.stop_profiling()
    
    return profiler.generate_report()


def compare_implementations(brain_configs: List[Tuple[str, Dict, int]],
                          steps: int = 100) -> Dict:
    """
    Compare multiple implementations with the same benchmarking setup.
    
    Args:
        brain_configs: List of (name, config, steps) tuples
        steps: Number of steps per implementation
        
    Returns:
        Dictionary containing comparison results
    """
    profilers = []
    labels = []
    
    for name, config, _ in brain_configs:
        # Create brain instance (simplified - would need actual brain creation)
        brain = None  # This would be implemented based on actual brain creation
        profiler = SimulationProfiler(brain)
        profilers.append(profiler)
        labels.append(name)
    
    comparison = profilers[0].compare_performance(profilers, labels)
    return comparison


# Export public API
__all__ = [
    'PerformanceMetricType',
    'PerformanceSample',
    'PerformanceStats',
    'MetricCollector',
    'SimulationProfiler',
    'benchmark_brain_performance',
    'profile_simulation',
    'compare_implementations'
]