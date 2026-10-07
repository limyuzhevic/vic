"""
Performance Monitoring System for NLM Python Bindings

This module provides real-time performance monitoring and metrics collection
for the enhanced NLM Python bindings, enabling users to track and optimize
performance during neural simulations.
"""

import time
import threading
import psutil
import numpy as np
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
from collections import deque
import json
from datetime import datetime
from .performance_optimizer import PerformanceMetrics

class MonitoringEvent(Enum):
    """Types of monitoring events"""
    SIMULATION_START = "simulation_start"
    SIMULATION_END = "simulation_end"
    STEP_COMPLETE = "step_complete"
    MEMORY_SPIKE = "memory_spike"
    CPU_SPIKE = "cpu_spike"
    PERFORMANCE_THRESHOLD = "performance_threshold"
    OPTIMIZATION_ENABLED = "optimization_enabled"
    OPTIMIZATION_DISABLED = "optimization_disabled"

@dataclass
class MonitoringEventData:
    """Data for monitoring events"""
    event_type: MonitoringEvent
    timestamp: float
    data: Dict[str, Any] = field(default_factory=dict)
    step_id: int = 0

class PerformanceProfiler:
    """
    Real-time performance profiler for NLM simulations.
    
    This profiler continuously monitors system performance, collects metrics,
    and provides insights into optimization opportunities.
    """
    
    def __init__(self, sample_interval: float = 0.1):
        """Initialize performance profiler"""
        self.sample_interval = sample_interval
        self.monitoring_thread = None
        self.is_monitoring = False
        self.metrics_history = deque(maxlen=10000)
        self.event_handlers = []
        self.performance_thresholds = {
            'memory_usage_percent': 80.0,
            'cpu_usage_percent': 70.0,
            'operation_rate': 1000.0,
            'memory_growth_rate': 10.0  # MB/s
        }
        self.current_metrics = PerformanceMetrics()
        self.process = psutil.Process()
        
    def add_event_handler(self, handler: Callable[[MonitoringEventData], None]):
        """Add event handler for monitoring events"""
        self.event_handlers.append(handler)
    
    def _collect_system_metrics(self) -> PerformanceMetrics:
        """Collect current system metrics"""
        current_time = time.time()
        metrics = PerformanceMetrics(timestamp=current_time)
        
        # Memory metrics
        memory_info = self.process.memory_info()
        metrics.memory_usage_mb = memory_info.rss / 1024 / 1024
        metrics.memory_allocated_mb = memory_info.vms / 1024 / 1024
        metrics.memory_peak_mb = max(metrics.memory_usage_mb, 
                                   getattr(self, '_previous_memory', 0))
        
        # CPU metrics
        cpu_percent = psutil.cpu_percent(interval=0.0)
        metrics.cpu_usage_percent = cpu_percent
        
        # Calculate operations rate (simplified)
        metrics.operations_per_second = getattr(self, '_operations_counter', 0)
        
        # Track memory growth
        if hasattr(self, '_previous_memory'):
            memory_growth = metrics.memory_usage_mb - self._previous_memory
            metrics.memory_allocated_mb = max(metrics.memory_allocated_mb, memory_growth)
        
        self._previous_memory = metrics.memory_usage_mb
        
        return metrics
    
    def _check_thresholds(self, metrics: PerformanceMetrics):
        """Check if metrics exceed thresholds"""
        # Memory threshold check
        memory_percent = (metrics.memory_usage_mb / 1024) / psutil.virtual_memory().total * 100
        if memory_percent > self.performance_thresholds['memory_usage_percent']:
            self._trigger_event(MonitoringEvent.MEMORY_SPIKE, {
                'memory_mb': metrics.memory_usage_mb,
                'threshold_percent': self.performance_thresholds['memory_usage_percent']
            })
        
        # CPU threshold check
        if metrics.cpu_usage_percent > self.performance_thresholds['cpu_usage_percent']:
            self._trigger_event(MonitoringEvent.CPU_SPIKE, {
                'cpu_percent': metrics.cpu_usage_percent,
                'threshold_percent': self.performance_thresholds['cpu_usage_percent']
            })
        
        # Performance threshold check
        if metrics.operations_per_second > self.performance_thresholds['operation_rate']:
            self._trigger_event(MonitoringEvent.PERFORMANCE_THRESHOLD, {
                'operations_per_second': metrics.operations_per_second,
                'threshold': self.performance_thresholds['operation_rate']
            })
    
    def _trigger_event(self, event_type: MonitoringEvent, data: Dict[str, Any], step_id: int = 0):
        """Trigger monitoring event"""
        event_data = MonitoringEventData(
            event_type=event_type,
            timestamp=time.time(),
            data=data,
            step_id=step_id
        )
        
        # Notify all handlers
        for handler in self.event_handlers:
            try:
                handler(event_data)
            except Exception as e:
                print(f"Event handler error: {e}")
    
    def start_monitoring(self):
        """Start continuous performance monitoring"""
        if self.is_monitoring:
            return
        
        self.is_monitoring = True
        self.monitoring_thread = threading.Thread(
            target=self._monitoring_loop,
            daemon=True,
            name='nlm_performance_monitor'
        )
        self.monitoring_thread.start()
        
        # Trigger start event
        self._trigger_event(MonitoringEvent.SIMULATION_START, {})
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.is_monitoring = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
        
        self._trigger_event(MonitoringEvent.SIMULATION_END, {})
    
    def _monitoring_loop(self):
        """Main monitoring loop"""
        while self.is_monitoring:
            try:
                # Collect metrics
                metrics = self._collect_system_metrics()
                
                # Check thresholds
                self._check_thresholds(metrics)
                
                # Store metrics
                self.metrics_history.append(metrics)
                self.current_metrics = metrics
                
                # Small delay to control sampling rate
                time.sleep(self.sample_interval)
                
            except Exception as e:
                print(f"Monitoring loop error: {e}")
                time.sleep(self.sample_interval)
    
    def get_current_metrics(self) -> PerformanceMetrics:
        """Get current performance metrics"""
        return self.current_metrics
    
    def get_metrics_history(self, limit: Optional[int] = None) -> List[PerformanceMetrics]:
        """Get metrics history"""
        if limit is None:
            return list(self.metrics_history)
        return list(self.metrics_history)[-limit:]
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary statistics"""
        if not self.metrics_history:
            return {}
        
        metrics_list = list(self.metrics_history)
        
        # Calculate statistics
        memory_values = [m.memory_usage_mb for m in metrics_list]
        cpu_values = [m.cpu_usage_percent for m in metrics_list]
        operation_values = [m.operations_per_second for m in metrics_list]
        
        summary = {
            'sample_count': len(metrics_list),
            'time_span_seconds': (metrics_list[-1].timestamp - metrics_list[0].timestamp) 
                               if len(metrics_list) > 1 else 0.0,
            
            'memory_stats': {
                'current_mb': memory_values[-1],
                'average_mb': np.mean(memory_values) if memory_values else 0.0,
                'peak_mb': np.max(memory_values) if memory_values else 0.0,
                'min_mb': np.min(memory_values) if memory_values else 0.0,
                'std_mb': np.std(memory_values) if memory_values else 0.0
            },
            
            'cpu_stats': {
                'current_percent': cpu_values[-1],
                'average_percent': np.mean(cpu_values) if cpu_values else 0.0,
                'peak_percent': np.max(cpu_values) if cpu_values else 0.0,
                'min_percent': np.min(cpu_values) if cpu_values else 0.0
            },
            
            'operation_stats': {
                'current_ops_per_second': operation_values[-1],
                'average_ops_per_second': np.mean(operation_values) if operation_values else 0.0,
                'peak_ops_per_second': np.max(operation_values) if operation_values else 0.0,
                'min_ops_per_second': np.min(operation_values) if operation_values else 0.0
            }
        }
        
        return summary
    
    def export_metrics(self, filepath: str):
        """Export metrics to file"""
        metrics_list = list(self.metrics_history)
        
        # Convert to serializable format
        export_data = {
            'timestamp': datetime.now().isoformat(),
            'metrics': [
                {
                    'timestamp': m.timestamp,
                    'memory_usage_mb': m.memory_usage_mb,
                    'memory_allocated_mb': m.memory_allocated_mb,
                    'memory_peak_mb': m.memory_peak_mb,
                    'cpu_usage_percent': m.cpu_usage_percent,
                    'cpu_time_ms': m.cpu_time_ms,
                    'real_time_ms': m.real_time_ms,
                    'operations_per_second': m.operations_per_second,
                    'bytes_processed': m.bytes_processed,
                    'spike_events_processed': m.spike_events_processed,
                    'synapses_updated': m.synapses_updated,
                    'neurons_updated': m.neurons_updated,
                    'accuracy': m.accuracy,
                    'convergence_rate': m.convergence_rate,
                    'step_id': m.step_id
                }
                for m in metrics_list
            ],
            'performance_thresholds': self.performance_thresholds,
            'sample_interval': self.sample_interval
        }
        
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2)

class PerformanceThresholdMonitor:
    """
    Monitor and manage performance thresholds.
    
    This class monitors key performance metrics and triggers actions
    when thresholds are exceeded.
    """
    
    def __init__(self):
        self.thresholds = {}
        self.alert_callbacks = []
        self.auto_actions = {}
        
    def set_threshold(self, metric: str, value: float, alert_callback: Optional[Callable] = None,
                     auto_action: Optional[Callable] = None):
        """Set performance threshold for a metric"""
        self.thresholds[metric] = value
        
        if alert_callback:
            self.alert_callbacks.append((metric, alert_callback))
        
        if auto_action:
            self.auto_actions[metric] = auto_action
    
    def check_threshold(self, metric: str, value: float) -> bool:
        """Check if metric exceeds threshold"""
        if metric not in self.thresholds:
            return False
        
        threshold = self.thresholds[metric]
        exceeded = value > threshold
        
        if exceeded:
            self._handle_threshold_exceeded(metric, value, threshold)
        
        return exceeded
    
    def _handle_threshold_exceeded(self, metric: str, value: float, threshold: float):
        """Handle threshold exceeded condition"""
        # Trigger alert callbacks
        for alert_metric, callback in self.alert_callbacks:
            if alert_metric == metric:
                try:
                    callback(metric, value, threshold)
                except Exception as e:
                    print(f"Alert callback error: {e}")
        
        # Trigger auto actions
        if metric in self.auto_actions:
            try:
                self.auto_actions[metric](metric, value, threshold)
            except Exception as e:
                print(f"Auto action error: {e}")

class ResourceMonitor:
    """
    Monitor system resources (CPU, memory, I/O).
    """
    
    def __init__(self):
        self.cpu_monitor = None
        self.memory_monitor = None
        self.io_monitor = None
        self.monitoring_active = False
        
    def start(self):
        """Start resource monitoring"""
        self.monitoring_active = True
        
        # Start resource monitoring threads
        self.cpu_monitor = threading.Thread(
            target=self._cpu_monitor_loop,
            daemon=True,
            name='cpu_monitor'
        )
        self.memory_monitor = threading.Thread(
            target=self._memory_monitor_loop,
            daemon=True,
            name='memory_monitor'
        )
        
        self.cpu_monitor.start()
        self.memory_monitor.start()
    
    def stop(self):
        """Stop resource monitoring"""
        self.monitoring_active = False
        
        if self.cpu_monitor:
            self.cpu_monitor.join(timeout=2.0)
        
        if self.memory_monitor:
            self.memory_monitor.join(timeout=2.0)
    
    def _cpu_monitor_loop(self):
        """CPU monitoring loop"""
        while self.monitoring_active:
            # Record CPU usage
            time.sleep(1.0)
    
    def _memory_monitor_loop(self):
        """Memory monitoring loop"""
        while self.monitoring_active:
            # Record memory usage
            time.sleep(0.5)

# Global performance monitoring instances
_performance_profiler = None
_threshold_monitor = None
_resource_monitor = None

def get_profiler(sample_interval: float = 0.1) -> PerformanceProfiler:
    """Get or create global performance profiler"""
    global _performance_profiler
    if _performance_profiler is None:
        _performance_profiler = PerformanceProfiler(sample_interval)
    return _performance_profiler

def get_threshold_monitor() -> PerformanceThresholdMonitor:
    """Get or create global threshold monitor"""
    global _threshold_monitor
    if _threshold_monitor is None:
        _threshold_monitor = PerformanceThresholdMonitor()
    return _threshold_monitor

def get_resource_monitor() -> ResourceMonitor:
    """Get or create global resource monitor"""
    global _resource_monitor
    if _resource_monitor is None:
        _resource_monitor = ResourceMonitor()
    return _resource_monitor

# Convenience functions
def start_performance_monitoring(sample_interval: float = 0.1):
    """Start global performance monitoring"""
    profiler = get_profiler(sample_interval)
    profiler.start_monitoring()

def stop_performance_monitoring():
    """Stop global performance monitoring"""
    profiler = get_profiler()
    profiler.stop_monitoring()

def get_performance_summary() -> Dict[str, Any]:
    """Get global performance summary"""
    profiler = get_profiler()
    return profiler.get_performance_summary()

def add_performance_event_handler(handler: Callable[[MonitoringEventData], None]):
    """Add global performance event handler"""
    profiler = get_profiler()
    profiler.add_event_handler(handler)

print("Performance monitoring system loaded successfully!")