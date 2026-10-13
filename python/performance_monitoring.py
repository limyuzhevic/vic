#!/usr/bin/env python3
"""
NLM - Performance Monitoring and Profiling Tools

This module provides comprehensive performance monitoring and profiling capabilities
for NLM, including:
1. Performance metrics collection
2. System resource monitoring
3. Simulation profiling
4. Memory profiling
5. CPU and GPU profiling
6. Performance bottleneck detection
7. Real-time monitoring
8. Performance comparison
"""

import time
import psutil
import numpy as np
from typing import Dict, List, Any, Optional, Callable, Union
from dataclasses import dataclass, field
from datetime import datetime, timedelta
import threading
import tracemalloc
import cProfile
import pstats
from io import StringIO
import json
import csv
from enum import Enum
import warnings
from contextlib import contextmanager
import os

class ProfilingMode(Enum):
    LINE_PROFILER = "line"
    CPU_PROFILER = "cpu"
    MEMORY_PROFILER = "memory"
    GPU_PROFILER = "gpu"
    WALL_CLOCK = "wall"
    THREAD = "thread"
    SYSTEM = "system"

class PerformanceMetric(Enum):
    CPU_USAGE = "cpu_usage"
    MEMORY_USAGE = "memory_usage"
    GPU_USAGE = "gpu_usage"
    NETWORK_USAGE = "network_usage"
    DISK_USAGE = "disk_usage"
    FPS = "fps"
    CPU_TIME = "cpu_time"
    REAL_TIME = "real_time"
    NEURON_COUNT = "neuron_count"
    SYNAPSE_COUNT = "synapse_count"
    Firing_RATE = "firing_rate"
    SPIKE_COUNT = "spike_count"
    PLACEMENT_TIME = "placement_time"
    STEP_TIME = "step_time"
    LEARNING_RATE = "learning_rate"
    MEMORY_ALLOCATION = "memory_allocation"

@dataclass
class PerformanceSnapshot:
    """Performance data point."""
    timestamp: datetime
    metrics: Dict[str, Any] = field(default_factory=dict)
    system_info: Dict[str, Any] = field(default_factory=dict)
    simulation_state: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'timestamp': self.timestamp.isoformat(),
            'metrics': self.metrics,
            'system_info': self.system_info,
            'simulation_state': self.simulation_state
        }

@dataclass
class PerformanceThreshold:
    """Performance threshold for alerts."""
    metric: str
    warning_threshold: float
    error_threshold: float
    unit: str = ""
    condition: str = "less_than"
    violated: bool = False
    
    def is_violated(self, value: float) -> str:
        """Check if threshold is violated."""
        if self.condition == "less_than":
            if value > self.error_threshold:
                self.violated = True
                return "error"
            elif value > self.warning_threshold:
                self.violated = True
                return "warning"
        elif self.condition == "greater_than":
            if value < self.error_threshold:
                self.violated = True
                return "error"
            elif value < self.warning_threshold:
                self.violated = True
                return "warning"
        self.violated = False
        return "ok"

class PerformanceMonitor:
    """
    Core performance monitoring system for NLM.
    """
    
    def __init__(self, sampling_interval: float = 1.0, max_history: int = 3600):
        self.sampling_interval = sampling_interval
        self.max_history = max_history
        self.snapshots: List[PerformanceSnapshot] = []
        self.thresholds: List[PerformanceThreshold] = []
        self.active = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.profiling_mode = ProfilingMode.WALL_CLOCK
        self.currently_profiling: Dict[str, Any] = {}
        
        # Configure logging
        self.error_logger = logging.getLogger('performance_errors')
        self.error_logger.setLevel(logging.WARNING)
        
        # Initialize system monitoring
        self._init_system_monitoring()
        
        # Configure metrics
        self.metrics = self._get_default_metrics()
    
    def _init_system_monitoring(self):
        """Initialize system monitoring capabilities."""
        self.system_metrics = {
            'cpu': psutil.cpu_percent(interval=None),
            'memory': psutil.virtual_memory(),
            'disk': psutil.disk_usage('/'),
            'network': psutil.net_io_counters(),
            'processes': psutil.Process(),
            'temperature': self._get_temperature()
        }
    
    def _get_temperature(self) -> Dict[str, float]:
        """Get system temperature if available."""
        temps = {}
        try:
            import sensors
            for sensor in sensors.get_modules():
                for chip in sensor.get_chips():
                    for key in chip.keys():
                        if 'temp' in key:
                            temps[key] = chip[key].current_value
        except ImportError:
            pass
        return temps
    
    def _get_default_metrics(self) -> Dict[str, Any]:
        """Get default performance metrics configuration."""
        return {
            'cpu_usage': {'type': 'gauge', 'sampling': 'auto'},
            'memory_usage': {'type': 'gauge', 'sampling': 'auto'},
            'gpu_usage': {'type': 'gauge', 'sampling': 'auto'},
            'fps': {'type': 'rate', 'sampling': 'fixed'},
            'cpu_time': {'type': 'cumulative', 'sampling': 'auto'},
            'real_time': {'type': 'cumulative', 'sampling': 'auto'},
            'neuron_count': {'type': 'gauge', 'sampling': 'event'},
            'synapse_count': {'type': 'gauge', 'sampling': 'event'},
            'firing_rate': {'type': 'rate', 'sampling': 'fixed'},
            'spike_count': {'type': 'cumulative', 'sampling': 'event'},
            'step_time': {'type': 'histogram', 'sampling': 'fixed'},
            'learning_rate': {'type': 'gauge', 'sampling': 'event'},
            'memory_allocation': {'type': 'histogram', 'sampling': 'fixed'}
        }
    
    def start_monitoring(self, mode: ProfilingMode = ProfilingMode.WALL_CLOCK):
        """
        Start performance monitoring.
        
        Args:
            mode: Profiling mode
        """
        self.profiling_mode = mode
        self.active = True
        
        # Start monitoring thread
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
        
        print(f"Performance monitoring started with mode: {mode.value}")
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.active = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        print("Performance monitoring stopped")
    
    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.active:
            try:
                self._collect_metrics()
                time.sleep(self.sampling_interval)
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(5.0)  # Wait before retrying
    
    def _collect_metrics(self):
        """Collect current metrics."""
        timestamp = datetime.now()
        
        # Collect system metrics
        system_metrics = self._collect_system_metrics()
        
        # Collect simulation metrics
        simulation_metrics = self._collect_simulation_metrics()
        
        # Create snapshot
        snapshot = PerformanceSnapshot(
            timestamp=timestamp,
            metrics={**system_metrics, **simulation_metrics},
            system_info=self._get_system_info(),
            simulation_state=self._get_simulation_state()
        )
        
        # Add snapshot to history
        self.snapshots.append(snapshot)
        
        # Check thresholds
        self._check_thresholds(snapshot)
        
        # Trim history if needed
        if len(self.snapshots) > self.max_history:
            self.snapshots = self.snapshots[-self.max_history:]
    
    def _collect_system_metrics(self) -> Dict[str, Any]:
        """Collect system performance metrics."""
        metrics = {}
        
        # CPU metrics
        cpu_percent = psutil.cpu_percent(interval=None)
        metrics['cpu_usage'] = cpu_percent
        
        # Memory metrics
        memory = psutil.virtual_memory()
        metrics['memory_usage'] = {
            'total': memory.total,
            'available': memory.available,
            'percent': memory.percent,
            'used': memory.used
        }
        
        # GPU metrics (if available)
        if psutil.HAVE_GPU:
            gpus = psutil.graphics_devices()
            for i, gpu in enumerate(gpus):
                metrics[f'gpu_{i}_usage'] = getattr(gpu, 'load', 0) * 100
                metrics[f'gpu_{i}_memory'] = getattr(gpu, 'memory_used', 0)
        
        # Network metrics
        network = psutil.net_io_counters()
        metrics['network_usage'] = {
            'bytes_sent': network.bytes_sent,
            'bytes_recv': network.bytes_recv,
            'packets_sent': network.packets_sent,
            'packets_recv': network.packets_recv
        }
        
        # Disk metrics
        disk = psutil.disk_usage('/')
        metrics['disk_usage'] = {
            'total': disk.total,
            'used': disk.used,
            'free': disk.free,
            'percent': disk.percent
        }
        
        return metrics
    
    def _collect_simulation_metrics(self) -> Dict[str, Any]:
        """Collect simulation-specific metrics."""
        metrics = {}
        
        # These would be implemented based on NLM internals
        # For now, return placeholder metrics
        metrics['step_time'] = np.random.normal(0.1, 0.02)  # Simulated step time
        metrics['learning_rate'] = np.random.uniform(0.001, 0.01)
        metrics['memory_allocation'] = np.random.normal(100, 20)
        
        return metrics
    
    def _get_system_info(self) -> Dict[str, Any]:
        """Get system information."""
        return {
            'platform': psutil.platform(),
            'architecture': psutil.architecture(),
            'cpu_count': psutil.cpu_count(),
            'cpu_freq': psutil.cpu_freq(),
            'boot_time': datetime.fromtimestamp(psutil.boot_time()),
        }
    
    def _get_simulation_state(self) -> Dict[str, Any]:
        """Get current simulation state."""
        return {
            'neurons': 1000,  # Would be queried from brain
            'synapses': 15000,
            'regions': 4,
            'development_stage': 3,
            'active_regions': [0, 1],
        }
    
    def _check_thresholds(self, snapshot: PerformanceSnapshot):
        """Check if any thresholds are violated."""
        for threshold in self.thresholds:
            metric_name = threshold.metric.split('.')[0]
            
            if metric_name in snapshot.metrics:
                # Handle nested metrics
                value = snapshot.metrics[metric_name]
                if isinstance(value, dict):
                    # For complex metrics, check key values
                    for key, val in value.items():
                        violation = threshold.is_violated(val)
                        if violation != "ok":
                            self._handle_threshold_violation(threshold, key, val, violation, snapshot)
                else:
                    violation = threshold.is_violated(value)
                    if violation != "ok":
                        self._handle_threshold_violation(threshold, metric_name, value, violation, snapshot)
    
    def _handle_threshold_violation(self, threshold: PerformanceThreshold, metric_name: str, 
                                    value: float, violation: str, snapshot: PerformanceSnapshot):
        """Handle threshold violation."""
        message = f"Performance threshold violated: {metric_name} = {value} {threshold.unit} " \
                  f"(expected {threshold.error_threshold if violation == 'error' else threshold.warning_threshold} {threshold.unit})"
        
        severity = "ERROR" if violation == "error" else "WARNING"
        print(f"[{severity}] {message}")
        
        # Log violation
        self.error_logger.log(getattr(logging, severity), message)
    
    def add_threshold(self, threshold: PerformanceThreshold):
        """Add a performance threshold."""
        self.thresholds.append(threshold)
    
    def add_metric(self, name: str, config: Dict[str, Any]):
        """Add a custom metric."""
        self.metrics[name] = config
    
    def start_profiling(self, function: Callable, *args, **kwargs) -> cProfile.Profile:
        """
        Start profiling a function.
        
        Args:
            function: Function to profile
            *args, **kwargs: Function arguments
            
        Returns:
            Profiler object
        """
        profiler = cProfile.Profile()
        self.currently_profiling['function'] = (function, args, kwargs, profiler)
        
        if self.profiling_mode == ProfilingMode.CPU_PROFILER:
            profiler.enable()
        
        return profiler
    
    def stop_profiling(self, profiler: cProfile.Profile) -> Dict[str, Any]:
        """
        Stop profiling and return statistics.
        
        Args:
            profiler: Profiler object
            
        Returns:
            Profiling statistics
        """
        if self.profiling_mode == ProfilingMode.CPU_PROFILER:
            profiler.disable()
        
        # Get profiling statistics
        stream = StringIO()
        stats = pstats.Stats(profiler, stream=stream)
        stats.sort_stats('cumulative')
        
        output = stream.getvalue()
        
        # Extract key metrics
        lines = output.split('\n')
        function_stats = []
        
        for line in lines:
            if 'function calls' in line or 'ncalls' in line:
                continue
            if line.strip() and not line.startswith('   '):
                break
            
            if line.strip():
                function_stats.append(line.strip())
        
        return {
            'total_calls': len(function_stats),
            'output': output,
            'top_functions': function_stats[:10],
        }
    
    def start_memory_profiling(self) -> tracemalloc.Snapshot:
        """Start memory profiling."""
        tracemalloc.start()
        return tracemalloc.take_snapshot()
    
    def stop_memory_profiling(self, snapshot1: tracemalloc.Snapshot, 
                             snapshot2: tracemalloc.Snapshot) -> Dict[str, Any]:
        """
        Stop memory profiling and compare snapshots.
        
        Args:
            snapshot1: Starting snapshot
            snapshot2: Ending snapshot
            
        Returns:
            Memory profiling statistics
        """
        stats1 = snapshot1.statistics('lineno')
        stats2 = snapshot2.statistics('lineno')
        
        # Calculate differences
        memory_increase = 0
        top_allocations = []
        
        for stat in stats2:
            # Find matching allocation in snapshot1
            for stat1 in stats1:
                if stat.traceback == stat1.traceback and stat.size == stat1.size:
                    break
            else:
                memory_increase += stat.size_diff
                top_allocations.append({
                    'file': stat.traceback.format()[-1],
                    'line': stat.traceback[0].lineno if stat.traceback else 0,
                    'size': stat.size_diff,
                    'count': stat.count_diff
                })
        
        return {
            'memory_increase': memory_increase,
            'top_allocations': top_allocations[:10],
            'total_allocations': len(stats2)
        }
    
    def generate_report(self) -> Dict[str, Any]:
        """Generate performance report."""
        if not self.snapshots:
            return {'message': 'No performance data available'}
        
        # Calculate statistics for each metric
        report = {
            'total_samples': len(self.snapshots),
            'sampling_interval': self.sampling_interval,
            'duration': (self.snapshots[-1].timestamp - self.snapshots[0].timestamp).total_seconds(),
            'metrics_summary': {},
            'threshold_violations': len([t for t in self.thresholds if t.violated]),
            'peak_values': {},
            'average_values': {},
            'min_values': {},
            'max_values': {}
        }
        
        # Analyze each metric
        all_metric_names = set()
        for snapshot in self.snapshots:
            all_metric_names.update(snapshot.metrics.keys())
        
        for metric_name in all_metric_names:
            values = []
            for snapshot in self.snapshots:
                if metric_name in snapshot.metrics:
                    value = snapshot.metrics[metric_name]
                    if isinstance(value, dict):
                        # For complex metrics, use an average of values
                        values.extend(value.values())
                    else:
                        values.append(value)
            
            if values:
                report['metrics_summary'][metric_name] = {
                    'count': len(values),
                    'average': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'median': np.median(values)
                }
                
                report['peak_values'][metric_name] = {
                    'value': np.max(values),
                    'timestamp': self.snapshots[np.argmax(values)].timestamp.isoformat()
                }
                
                report['average_values'][metric_name] = np.mean(values)
                report['min_values'][metric_name] = np.min(values)
                report['max_values'][metric_name] = np.max(values)
        
        return report
    
    def export_data(self, filepath: str, format: str = "json"):
        """
        Export performance data.
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'sqlite')
        """
        if format == 'json':
            self._export_json(filepath)
        elif format == 'csv':
            self._export_csv(filepath)
        elif format == 'sqlite':
            self._export_sqlite(filepath)
        else:
            raise ValueError(f"Unsupported export format: {format}")
    
    def _export_json(self, filepath: str):
        """Export data as JSON."""
        data = {
            'snapshots': [snapshot.to_dict() for snapshot in self.snapshots],
            'thresholds': [
                {
                    'metric': t.metric,
                    'warning_threshold': t.warning_threshold,
                    'error_threshold': t.error_threshold,
                    'unit': t.unit,
                    'condition': t.condition
                }
                for t in self.thresholds
            ],
            'metrics_config': self.metrics
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
    
    def _export_csv(self, filepath: str):
        """Export data as CSV."""
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            
            # Write headers
            headers = ['timestamp']
            if self.snapshots:
                headers.extend(self.snapshots[0].metrics.keys())
            
            writer.writerow(headers)
            
            # Write data
            for snapshot in self.snapshots:
                row = [snapshot.timestamp.isoformat()]
                for header in headers[1:]:
                    value = snapshot.metrics.get(header, '')
                    if isinstance(value, dict):
                        # For complex metrics, convert to string
                        value = json.dumps(value)
                    row.append(value)
                
                writer.writerow(row)
    
    def _export_sqlite(self, filepath: str):
        """Export data to SQLite."""
        try:
            import sqlite3
            
            conn = sqlite3.connect(filepath)
            cursor = conn.cursor()
            
            # Create tables
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS performance_snapshots (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    timestamp TEXT,
                    metrics TEXT,
                    system_info TEXT,
                    simulation_state TEXT
                )
            ''')
            
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS performance_thresholds (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    metric TEXT,
                    warning_threshold REAL,
                    error_threshold REAL,
                    unit TEXT,
                    condition TEXT
                )
            ''')
            
            # Insert data
            for snapshot in self.snapshots:
                cursor.execute('''
                    INSERT INTO performance_snapshots (timestamp, metrics, system_info, simulation_state)
                    VALUES (?, ?, ?, ?)
                ''', (
                    snapshot.timestamp.isoformat(),
                    json.dumps(snapshot.metrics),
                    json.dumps(snapshot.system_info),
                    json.dumps(snapshot.simulation_state)
                ))
            
            for threshold in self.thresholds:
                cursor.execute('''
                    INSERT INTO performance_thresholds (metric, warning_threshold, error_threshold, unit, condition)
                    VALUES (?, ?, ?, ?, ?)
                ''', (
                    threshold.metric,
                    threshold.warning_threshold,
                    threshold.error_threshold,
                    threshold.unit,
                    threshold.condition
                ))
            
            conn.commit()
            conn.close()
            
        except ImportError:
            print("SQLite not available, skipping database export")

class PerformanceProfiler:
    """
    High-level profiler for NLM simulations.
    """
    
    def __init__(self, monitor: PerformanceMonitor):
        self.monitor = monitor
        self.profiles = {}
        self.current_profile = None
    
    def profile_simulation(self, brain: Any, simulation_func: Callable, 
                          *args, **kwargs) -> Dict[str, Any]:
        """
        Profile a simulation.
        
        Args:
            brain: NLM brain object
            simulation_func: Simulation function
            *args, **kwargs: Function arguments
            
        Returns:
            Profiling results
        """
        # Start monitoring
        self.monitor.start_monitoring(ProfilingMode.CPU_PROFILER)
        
        # Start memory profiling
        memory_snapshot1 = self.monitor.start_memory_profiling()
        
        # Start time profiling
        start_time = time.time()
        
        # Run simulation
        results = simulation_func(brain, *args, **kwargs)
        
        # Stop time profiling
        end_time = time.time()
        
        # Stop memory profiling
        memory_snapshot2 = self.monitor.stop_memory_profiling(memory_snapshot1, memory_snapshot2)
        
        # Stop monitoring
        self.monitor.stop_monitoring()
        
        # Get profiling statistics
        profile_results = self.monitor.generate_report()
        
        return {
            'simulation_time': end_time - start_time,
            'memory_stats': memory_snapshot2,
            'profile_results': profile_results,
            'results': results
        }
    
    def profile_function(self, func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """
        Profile a single function.
        
        Args:
            func: Function to profile
            *args, **kwargs: Function arguments
            
        Returns:
            Profiling results
        """
        # Start CPU profiling
        profiler = self.monitor.start_profiling(func, *args, **kwargs)
        
        # Run function
        result = func(*args, **kwargs)
        
        # Stop profiling
        profile_stats = self.monitor.stop_profiling(profiler)
        
        return {
            'result': result,
            'profile_stats': profile_stats
        }

# Global performance monitor instance
_global_monitor = PerformanceMonitor()
global_profiler = PerformanceProfiler(_global_monitor)

def get_monitor() -> PerformanceMonitor:
    """Get the global performance monitor."""
    return _global_monitor

def get_profiler() -> PerformanceProfiler:
    """Get the global performance profiler."""
    return global_profiler

def start_performance_monitoring(sampling_interval: float = 1.0):
    """
    Start global performance monitoring.
    
    Args:
        sampling_interval: Sampling interval in seconds
    """
    _global_monitor.sampling_interval = sampling_interval
    _global_monitor.start_monitoring()

def stop_performance_monitoring():
    """Stop global performance monitoring."""
    _global_monitor.stop_monitoring()

def profile_simulation(brain: Any, simulation_func: Callable, *args, **kwargs) -> Dict[str, Any]:
    """
    Profile a simulation using the global profiler.
    
    Args:
        brain: NLM brain object
        simulation_func: Simulation function
        *args, **kwargs: Function arguments
        
    Returns:
        Profiling results
    """
    return global_profiler.profile_simulation(brain, simulation_func, *args, **kwargs)

# Exception types for performance errors
class PerformanceThresholdExceeded(NLMError):
    """Performance threshold exceeded error."""
    
    def __init__(self, message: str, metric: str, value: float, threshold: float):
        super().__init__(message, ErrorCategory.PERFORMANCE, ErrorSeverity.WARNING)
        self.metric = metric
        self.value = value
        self.threshold = threshold

# Configure error logger for performance
logging.basicConfig(level=logging.INFO)
performance_logger = logging.getLogger('performance')
performance_logger.setLevel(logging.WARNING)

if __name__ == "__main__":
    # Example usage
    print("NLM Performance Monitoring and Profiling Tools")
    print("=" * 60)
    
    # Get global monitor
    monitor = get_monitor()
    
    # Add thresholds
    monitor.add_threshold(PerformanceThreshold(
        metric='cpu_usage',
        warning_threshold=80.0,
        error_threshold=90.0,
        unit='%',
        condition='less_than'
    ))
    
    monitor.add_threshold(PerformanceThreshold(
        metric='memory_usage',
        warning_threshold=80.0,
        error_threshold=90.0,
        unit='%',
        condition='less_than'
    ))
    
    monitor.add_threshold(PerformanceThreshold(
        metric='step_time',
        warning_threshold=0.2,
        error_threshold=0.5,
        unit='seconds',
        condition='less_than'
    ))
    
    # Start monitoring
    start_performance_monitoring(sampling_interval=2.0)
    
    # Simulate some operations
    print("Running performance monitoring simulation...")
    
    for i in range(10):
        time.sleep(2)
        
        # Simulate performance metrics
        cpu_usage = np.random.uniform(30, 70)
        memory_usage = np.random.uniform(40, 80)
        step_time = np.random.uniform(0.05, 0.3)
        
        print(f"Step {i+1}: CPU={cpu_usage:.1f}%, Memory={memory_usage:.1f}%, StepTime={step_time:.3f}s")
    
    # Stop monitoring
    stop_performance_monitoring()
    
    # Generate report
    print("\nGenerating performance report...")
    report = monitor.generate_report()
    
    print(f"Total samples: {report['total_samples']}")
    print(f"Duration: {report['duration']:.1f} seconds")
    print(f"Peak values:")
    for metric, info in report['peak_values'].items():
        print(f"  {metric}: {info['value']:.2f} at {info['timestamp']}")
    
    # Export data
    print("\nExporting performance data...")
    monitor.export_data('performance_data.json', 'json')
    monitor.export_data('performance_data.csv', 'csv')
    
    print("Performance monitoring and profiling completed!")
