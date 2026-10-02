"""
Performance monitoring and optimization for NLM.

This module provides comprehensive performance monitoring and optimization
for NLM neural simulations including profiling, resource monitoring,
and performance analysis.
"""

import pybind11
import pybind11.stl
from .types import *
from .core import Config, Brain, create_default_config
import psutil
import time
import os
import gc
import threading
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import tracemalloc
import psutil
from datetime import datetime
from typing import Dict, List, Any, Optional, Callable, Union
import warnings
warnings.filterwarnings('ignore')

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Performance monitoring classes

class ResourceMonitor:
    """
    Monitor system resources during simulation.

    This class monitors CPU, memory, and other system resources
    to track resource usage during NLM simulations.
    """

    def __init__(self, monitor_interval: float = 1.0):
        """
        Initialize resource monitor.

        Args:
            monitor_interval: Interval between measurements in seconds
        """
        self.monitor_interval = monitor_interval
        self.readings = []
        self.monitoring = False
        self.monitor_thread = None

    def start_monitoring(self) -> bool:
        """
        Start monitoring system resources.

        Returns:
            bool: True if monitoring started successfully
        """
        try:
            self.monitoring = True
            self.monitor_thread = threading.Thread(target=self._monitor_loop)
            self.monitor_thread.daemon = True
            self.monitor_thread.start()
            logger.info("Resource monitoring started")
            return True
        except Exception as e:
            logger.error(f"Failed to start monitoring: {e}")
            return False

    def stop_monitoring(self) -> List[Dict[str, Any]]:
        """
        Stop monitoring and return readings.

        Returns:
            List[Dict[str, Any]]: Resource readings
        """
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None

        readings = self.readings.copy()
        self.readings = []
        logger.info(f"Resource monitoring stopped, collected {len(readings)} readings")
        return readings

    def _monitor_loop(self) -> None:
        """Main monitoring loop."""
        while self.monitoring:
            reading = self._take_reading()
            self.readings.append(reading)
            time.sleep(self.monitor_interval)

    def _take_reading(self) -> Dict[str, Any]:
        """Take a single resource reading."""
        try:
            # CPU usage
            cpu_percent = psutil.cpu_percent(interval=0.1)
            cpu_count = psutil.cpu_count()

            # Memory usage
            memory = psutil.virtual_memory()
            memory_used = memory.used
            memory_available = memory.available
            memory_percent = memory.percent

            # Swap usage
            swap = psutil.swap_memory()
            swap_used = swap.used
            swap_percent = swap.percent

            # Disk usage
            disk = psutil.disk_usage('/')
            disk_used = disk.used
            disk_percent = disk.percent

            # Process-specific metrics (would be updated by simulation)
            process = psutil.Process(os.getpid())
            process_memory = process.memory_info()
            process_cpu_percent = process.cpu_percent()

            return {
                'timestamp': datetime.now().isoformat(),
                'cpu_percent': cpu_percent,
                'cpu_count': cpu_count,
                'memory_used': memory_used,
                'memory_available': memory_available,
                'memory_percent': memory_percent,
                'swap_used': swap_used,
                'swap_percent': swap_percent,
                'disk_used': disk_used,
                'disk_percent': disk_percent,
                'process_memory_rss': process_memory.rss,
                'process_cpu_percent': process_cpu_percent,
                'thread_count': process.num_threads()
            }
        except Exception as e:
            logger.error(f"Error taking resource reading: {e}")
            return {
                'timestamp': datetime.now().isoformat(),
                'error': str(e)
            }

    def get_summary_stats(self) -> Dict[str, Any]:
        """
        Get summary statistics of collected readings.

        Returns:
            Dict[str, Any]: Summary statistics
        """
        if not self.readings:
            return {}

        valid_readings = [r for r in self.readings if 'error' not in r]

        if not valid_readings:
            return {}

        cpu_percentages = [r['cpu_percent'] for r in valid_readings]
        memory_usages = [r['memory_percent'] for r in valid_readings]
        process_memory = [r['process_memory_rss'] for r in valid_readings]

        return {
            'readings_count': len(valid_readings),
            'cpu_percent_avg': np.mean(cpu_percentages),
            'cpu_percent_min': np.min(cpu_percentages),
            'cpu_percent_max': np.max(cpu_percentages),
            'memory_percent_avg': np.mean(memory_usages),
            'memory_percent_min': np.min(memory_usages),
            'memory_percent_max': np.max(memory_usages),
            'process_memory_rss_avg': np.mean(process_memory),
            'process_memory_rss_min': np.min(process_memory),
            'process_memory_rss_max': np.max(process_memory),
            'start_time': valid_readings[0]['timestamp'],
            'end_time': valid_readings[-1]['timestamp']
        }

    def __repr__(self) -> str:
        return f"<ResourceMonitor: {self.monitor_interval}s interval, {len(self.readings)} readings>"


class MemoryProfiler:
    """
    Profile memory usage during simulation.

    This class profiles memory usage to identify memory leaks and
    optimize memory efficiency in NLM simulations.
    """

    def __init__(self):
        """Initialize memory profiler."""
        self.snapshots = []
        tracemalloc.start()

    def take_snapshot(self, label: str = "snapshot") -> None:
        """
        Take memory snapshot.

        Args:
            label: Label for the snapshot
        """
        snapshot = tracemalloc.take_snapshot()
        self.snapshots.append({
            'label': label,
            'timestamp': datetime.now().isoformat(),
            'snapshot': snapshot
        })

    def compare_snapshots(self, label1: str, label2: str) -> Dict[str, Any]:
        """
        Compare two memory snapshots.

        Args:
            label1: First snapshot label
            label2: Second snapshot label

        Returns:
            Dict[str, Any]: Comparison results
        """
        snap1 = next((s for s in self.snapshots if s['label'] == label1), None)
        snap2 = next((s for s in self.snapshots if s['label'] == label2), None)

        if not snap1 or not snap2:
            return {'error': 'Snapshots not found'}

        # Compare snapshots
        top_stats = snap2['snapshot'].compare_to(snap1['snapshot'], 'lineno')

        results = {
            'total_memory_increase': 0,
            'top_increases': []
        }

        for stat in top_stats[:10]:  # Top 10 memory increases
            if stat.size_diff > 0:
                results['top_increases'].append({
                    'traceback': stat.traceback,
                    'size_diff': stat.size_diff,
                    'count_diff': stat.count_diff,
                    'filename': stat.filename,
                    'lineno': stat.lineno,
                    'line': stat.line
                })
                results['total_memory_increase'] += stat.size_diff

        return results

    def get_memory_usage(self) -> Dict[str, Any]:
        """
        Get current memory usage.

        Returns:
            Dict[str, Any]: Memory usage information
        """
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        memory_percent = process.memory_percent()

        return {
            'rss': memory_info.rss,
            'vms': memory_info.vms,
            'shared': memory_info.shared,
            'text': memory_info.text,
            'data': memory_info.data,
            'dirty': memory_info.dirty,
            'percent': memory_percent
        }

    def stop(self) -> None:
        """Stop memory profiling."""
        tracemalloc.stop()

    def __repr__(self) -> str:
        return f"<MemoryProfiler: {len(self.snapshots)} snapshots>"


class SimulationProfiler:
    """
    Profile simulation performance.

    This class profiles simulation performance including timing,
    memory usage, and computational efficiency.
    """

    def __init__(self, brain: Brain):
        """
        Initialize simulation profiler.

        Args:
            brain: Brain instance to profile
        """
        self.brain = brain
        self.step_times = []
        self.initialization_time = 0.0
        self.start_time = None
        self.end_time = None
        self.resource_monitor = ResourceMonitor()
        self.memory_profiler = MemoryProfiler()

    def start_profiling(self) -> None:
        """Start profiling simulation."""
        self.start_time = time.time()
        self.step_times = []
        self.initialization_time = 0.0

        # Start resource monitoring
        self.resource_monitor.start_monitoring()

        logger.info("Simulation profiling started")

    def stop_profiling(self) -> 'PerformanceMetrics':
        """
        Stop profiling and return performance metrics.

        Returns:
            PerformanceMetrics: Performance metrics
        """
        self.end_time = time.time()

        # Stop resource monitoring
        resource_readings = self.resource_monitor.stop_monitoring()

        # Get memory snapshot
        self.memory_profiler.take_snapshot("end")

        # Calculate performance metrics
        metrics = self._calculate_metrics(resource_readings)

        logger.info(f"Simulation profiling completed: {metrics.simulation_time:.2f}s")
        return metrics

    def profile_step(self, step_num: int, step_time: float) -> None:
        """
        Profile a single simulation step.

        Args:
            step_num: Step number
            step_time: Time taken for step
        """
        self.step_times.append({
            'step': step_num,
            'time': step_time,
            'timestamp': datetime.now().isoformat()
        })

    def _calculate_metrics(self, resource_readings: List[Dict[str, Any]]) -> 'PerformanceMetrics':
        """
        Calculate performance metrics from profiling data.

        Args:
            resource_readings: Resource monitoring readings

        Returns:
            PerformanceMetrics: Calculated metrics
        """
        metrics = PerformanceMetrics()

        # Basic timing
        if self.start_time and self.end_time:
            metrics.simulation_time = self.end_time - self.start_time

        # Step performance
        if self.step_times:
            metrics.total_steps = len(self.step_times)
            step_times = [s['time'] for s in self.step_times]
            metrics.average_step_time = np.mean(step_times)
            metrics.min_step_time = np.min(step_times)
            metrics.max_step_time = np.max(step_times)

        # Spike performance
        metrics.total_spikes = self.brain.get_total_spike_count()
        if metrics.total_steps > 0:
            metrics.spikes_per_second = metrics.total_spikes / metrics.simulation_time

        # Neuron and synapse counts
        metrics.neuron_count = self.brain.get_total_neuron_count()
        metrics.synapse_count = self.brain.get_total_synapse_count()

        # Resource metrics
        if resource_readings:
            valid_readings = [r for r in resource_readings if 'error' not in r]
            if valid_readings:
                cpu_percents = [r['cpu_percent'] for r in valid_readings]
                memory_percents = [r['memory_percent'] for r in valid_readings]
                process_memory = [r['process_memory_rss'] for r in valid_readings]

                metrics.cpu_usage = np.mean(cpu_percents)
                metrics.memory_usage = np.mean(memory_percents)

                # Convert memory from bytes to MB
                metrics.memory_usage_mb = np.mean(process_memory) / (1024 * 1024)
                metrics.peak_memory_mb = np.max(process_memory) / (1024 * 1024)

        # Brain status
        brain_status = self.brain.log_status()

        return metrics

    def analyze_performance_trends(self) -> Dict[str, Any]:
        """
        Analyze performance trends.

        Returns:
            Dict[str, Any]: Performance analysis
        """
        if not self.step_times:
            return {}

        step_numbers = [s['step'] for s in self.step_times]
        step_durations = [s['time'] for s in self.step_times]

        # Calculate moving averages
        window_size = min(10, len(step_durations))
        if window_size > 1:
            moving_avg = np.convolve(step_durations, np.ones(window_size)/window_size, mode='valid')
        else:
            moving_avg = step_durations

        # Detect performance issues (slow steps)
        mean_duration = np.mean(step_durations)
        std_duration = np.std(step_durations)
        slow_step_threshold = mean_duration + 2 * std_duration

        slow_steps = [s for s in self.step_times if s['time'] > slow_step_threshold]

        return {
            'total_steps': len(self.step_times),
            'mean_step_time': np.mean(step_durations),
            'std_step_time': np.std(step_durations),
            'min_step_time': np.min(step_durations),
            'max_step_time': np.max(step_durations),
            'slow_step_count': len(slow_steps),
            'slow_step_percentage': len(slow_steps) / len(self.step_times) * 100,
            'performance_trend': 'improving' if len(moving_avg) > 1 and np.mean(moving_avg[-5:]) < np.mean(moving_avg[:-5]) else 'stable'
        }

    def create_performance_report(self, output_dir: str = "./performance_reports") -> bool:
        """
        Create performance report.

        Args:
            output_dir: Output directory

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import os
            os.makedirs(output_dir, exist_ok=True)

            # Create performance report
            report = {
                'simulation_summary': {
                    'start_time': self.start_time,
                    'end_time': self.end_time,
                    'total_duration': self.end_time - self.start_time if self.start_time and self.end_time else 0,
                    'total_steps': len(self.step_times)
                },
                'step_performance': self.step_times,
                'performance_analysis': self.analyze_performance_trends()
            }

            # Save report
            import json
            report_file = f"{output_dir}/performance_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
            with open(report_file, 'w') as f:
                json.dump(report, f, indent=2, default=str)

            logger.info(f"Performance report saved to {report_file}")
            return True

        except Exception as e:
            logger.error(f"Error creating performance report: {e}")
            return False

    def __repr__(self) -> str:
        return f"<SimulationProfiler: {len(self.step_times)} steps profiled>"


class PerformanceMonitor:
    """
    Comprehensive performance monitoring for NLM simulations.

    This class provides comprehensive performance monitoring including
    resource monitoring, memory profiling, and simulation profiling.
    """

    def __init__(self, brain: Optional[Brain] = None):
        """
        Initialize performance monitor.

        Args:
            brain: Brain instance to monitor (uses global if None)
        """
        self.brain = brain
        self.resource_monitor = ResourceMonitor()
        self.memory_profiler = MemoryProfiler()
        self.simulation_profiler = None
        self.monitoring = False

    def set_brain(self, brain: Brain) -> None:
        """
        Set brain to monitor.

        Args:
            brain: Brain instance
        """
        self.brain = brain

    def start_monitoring(self) -> bool:
        """
        Start comprehensive monitoring.

        Returns:
            bool: True if monitoring started successfully
        """
        try:
            self.monitoring = True

            # Start resource monitoring
            self.resource_monitor.start_monitoring()

            # Start memory profiling
            self.memory_profiler = MemoryProfiler()

            # Start simulation profiling if brain available
            if self.brain:
                self.simulation_profiler = SimulationProfiler(self.brain)
                self.simulation_profiler.start_profiling()

            logger.info("Performance monitoring started")
            return True

        except Exception as e:
            logger.error(f"Failed to start monitoring: {e}")
            return False

    def stop_monitoring(self) -> 'PerformanceMetrics':
        """
        Stop monitoring and return performance metrics.

        Returns:
            PerformanceMetrics: Performance metrics
        """
        metrics = PerformanceMetrics()

        try:
            # Stop resource monitoring
            resource_readings = self.resource_monitor.stop_monitoring()

            # Get memory snapshot
            self.memory_profiler.take_snapshot("end")
            memory_info = self.memory_profiler.get_memory_usage()

            # Stop simulation profiling
            if self.simulation_profiler:
                sim_metrics = self.simulation_profiler.stop_profiling()
                metrics.update(sim_metrics)

            # Combine resource metrics
            if resource_readings:
                valid_readings = [r for r in resource_readings if 'error' not in r]
                if valid_readings:
                    cpu_percents = [r['cpu_percent'] for r in valid_readings]
                    memory_percents = [r['memory_percent'] for r in valid_readings]
                    process_memory = [r['process_memory_rss'] for r in valid_readings]

                    metrics.cpu_usage = np.mean(cpu_percents)
                    metrics.memory_usage = np.mean(memory_percents)
                    metrics.memory_usage_mb = np.mean(process_memory) / (1024 * 1024)
                    metrics.peak_memory_mb = np.max(process_memory) / (1024 * 1024)

            # Add memory profiler info
            metrics.total_memory_usage = memory_info['rss'] / (1024 * 1024)

            self.monitoring = False
            logger.info(f"Performance monitoring stopped: {metrics.simulation_time:.2f}s")
            return metrics

        except Exception as e:
            logger.error(f"Error stopping monitoring: {e}")
            self.monitoring = False
            return metrics

    def profile_step(self, step_num: int, step_time: float) -> None:
        """
        Profile a simulation step.

        Args:
            step_num: Step number
            step_time: Step duration
        """
        if self.simulation_profiler:
            self.simulation_profiler.profile_step(step_num, step_time)

    def get_resource_usage(self) -> Dict[str, Any]:
        """
        Get current resource usage.

        Returns:
            Dict[str, Any]: Resource usage
        """
        if self.resource_monitor:
            reading = self.resource_monitor._take_reading()
            return reading
        return {}

    def get_memory_profile(self) -> Dict[str, Any]:
        """
        Get memory profile.

        Returns:
            Dict[str, Any]: Memory profile
        """
        if self.memory_profiler:
            return self.memory_profiler.get_memory_usage()
        return {}

    def analyze_performance(self) -> Dict[str, Any]:
        """
        Analyze performance data.

        Returns:
            Dict[str, Any]: Performance analysis
        """
        analysis = {}

        if self.simulation_profiler:
            analysis['simulation'] = self.simulation_profiler.analyze_performance_trends()

        if self.resource_monitor:
            analysis['resources'] = self.resource_monitor.get_summary_stats()

        if self.memory_profiler and len(self.memory_profiler.snapshots) > 1:
            analysis['memory'] = self.memory_profiler.compare_snapshots(
                self.memory_profiler.snapshots[0]['label'],
                self.memory_profiler.snapshots[-1]['label']
            )

        return analysis

    def create_performance_dashboard(self, output_file: str = "performance_dashboard.html") -> bool:
        """
        Create interactive performance dashboard.

        Args:
            output_file: Output file path

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import plotly.graph_objects as go
            import plotly.subplots as sp

            analysis = self.analyze_performance()

            fig = sp.make_subplots(
                rows=2, cols=2,
                subplot_titles=('Resource Usage', 'Memory Usage', 
                               'Performance Trends', 'System Metrics'),
                specs=[[{'type': 'scatter'}, {'type': 'bar'}],
                       [{'type': 'scatter'}, {'type': 'scatter'}]]
            )

            # Resource usage plot
            if 'resources' in analysis:
                resources = analysis['resources']
                fig.add_trace(
                    go.Scatter(
                        x=list(range(resources.get('readings_count', 0))),
                        y=[resources.get('cpu_percent_avg', 0)] * resources.get('readings_count', 0),
                        mode='lines',
                        name='CPU %'
                    ),
                    row=1, col=1
                )

            # Memory usage plot
            if 'memory' in analysis:
                memory_data = analysis['memory']
                fig.add_trace(
                    go.Bar(
                        x=['Before', 'After'],
                        y=[memory_data.get('total_memory_increase', 0) / (1024 * 1024)],  # MB
                        name='Memory Change (MB)'
                    ),
                    row=1, col=2
                )

            # Performance trends
            if 'simulation' in analysis:
                sim_data = analysis['simulation']
                fig.add_trace(
                    go.Scatter(
                        x=list(range(sim_data.get('total_steps', 0))),
                        y=[sim_data.get('mean_step_time', 0)] * sim_data.get('total_steps', 0),
                        mode='lines',
                        name='Mean Step Time (s)'
                    ),
                    row=2, col=1
                )

            # System metrics
            if 'resources' in analysis and 'memory' in analysis:
                fig.add_trace(
                    go.Scatter(
                        x=list(range(4)),
                        y=[analysis['resources'].get('cpu_percent_avg', 0),
                           analysis['resources'].get('memory_percent_avg', 0),
                           analysis['memory'].get('total_memory_increase', 0) / (1024 * 1024),
                           analysis.get('simulation', {}).get('mean_step_time', 0)],
                        mode='markers',
                        name='Metrics'
                    ),
                    row=2, col=2
                )

            fig.update_layout(
                title='NLM Performance Dashboard',
                showlegend=True,
                height=800,
                width=1000
            )

            fig.write_html(output_file)
            logger.info(f"Performance dashboard saved to {output_file}")
            return True

        except Exception as e:
            logger.error(f"Error creating performance dashboard: {e}")
            return False

    def __repr__(self) -> str:
        return f"<PerformanceMonitor: {'active' if self.monitoring else 'inactive'}>"


# Performance monitoring functions


def profile_simulation(brain: Brain, steps: int) -> 'PerformanceMetrics':
    """
    Profile simulation performance.

    Args:
        brain: Brain instance to profile
        steps: Number of simulation steps

    Returns:
        PerformanceMetrics: Performance metrics
    """
    profiler = SimulationProfiler(brain)
    profiler.start_profiling()

    for step in range(steps):
        brain.step(step)
        profiler.profile_step(step, 0.0)  # Step time would be measured here

    return profiler.stop_profiling()


def monitor_resources(duration: float = 60.0) -> List[Dict[str, Any]]:
    """
    Monitor system resources for specified duration.

    Args:
        duration: Monitoring duration in seconds

    Returns:
        List[Dict[str, Any]]: Resource readings
    """
    monitor = ResourceMonitor(monitor_interval=1.0)
    monitor.start_monitoring()

    time.sleep(duration)
    return monitor.stop_monitoring()


def profile_memory_usage(duration: float = 30.0) -> Dict[str, Any]:
    """
    Profile memory usage for specified duration.

    Args:
        duration: Profiling duration in seconds

    Returns:
        Dict[str, Any]: Memory usage information
    """
    profiler = MemoryProfiler()

    # Take initial snapshot
    profiler.take_snapshot("start")

    # Run workload
    time.sleep(duration)

    # Take final snapshot
    profiler.take_snapshot("end")

    # Compare snapshots
    comparison = profiler.compare_snapshots("start", "end")

    # Get current memory usage
    current_usage = profiler.get_memory_usage()

    return {
        'comparison': comparison,
        'current_usage': current_usage,
        'duration': duration
    }


def create_performance_dashboard(brain: Brain, steps: int, output_file: str = "performance_dashboard.html") -> bool:
    """
    Create comprehensive performance dashboard.

    Args:
        brain: Brain instance to profile
        steps: Number of simulation steps
        output_file: Output file path

    Returns:
        bool: True if successful, False otherwise
    """
    monitor = PerformanceMonitor(brain)
    monitor.start_monitoring()

    for step in range(steps):
        brain.step(step)
        # Step profiling would happen here

    return monitor.create_performance_dashboard(output_file)


# Global performance monitoring

_performance_monitor = None


def get_performance_monitor(brain: Optional[Brain] = None) -> PerformanceMonitor:
    """
    Get global performance monitor.

    Args:
        brain: Brain instance (creates if None)

    Returns:
        PerformanceMonitor: Performance monitor
    """
    global _performance_monitor
    if _performance_monitor is None:
        _performance_monitor = PerformanceMonitor(brain)
    elif brain is not None:
        _performance_monitor.set_brain(brain)
    return _performance_monitor


def start_performance_monitoring(brain: Optional[Brain] = None) -> bool:
    """
    Start global performance monitoring.

    Args:
        brain: Brain instance

    Returns:
        bool: True if successful, False otherwise
    """
    monitor = get_performance_monitor(brain)
    return monitor.start_monitoring()


def stop_performance_monitoring() -> 'PerformanceMetrics':
    """
    Stop global performance monitoring.

    Returns:
        PerformanceMetrics: Performance metrics
    """
    global _performance_monitor
    if _performance_monitor:
        return _performance_monitor.stop_monitoring()
    return PerformanceMetrics()


# Export key classes and functions
__all__ = [
    'ResourceMonitor', 'MemoryProfiler', 'SimulationProfiler', 'PerformanceMonitor',
    'profile_simulation', 'monitor_resources', 'profile_memory_usage',
    'create_performance_dashboard', 'get_performance_monitor',
    'start_performance_monitoring', 'stop_performance_monitoring'
]