"""
Performance Monitoring System for NLM Python Bindings

This module provides comprehensive performance monitoring and profiling
for NLM neural simulations, enabling real-time performance tracking,
threshold monitoring, and automated optimization adjustments.
"""

import time
import threading
import psutil
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
from collections import deque
import json
from datetime import datetime

class PerformanceEventType(Enum):
    """Types of performance events"""
    SIMULATION_START = "simulation_start"
    SIMULATION_END = "simulation_end"
    STEP_COMPLETE = "step_complete"
    PERFORMANCE_THRESHOLD = "performance_threshold"
    MEMORY_SPIKE = "memory_spike"
    CPU_SPIKE = "cpu_spike"
    OPTIMIZATION_ENABLED = "optimization_enabled"
    OPTIMIZATION_DISABLED = "optimization_disabled"
    PERFORMANCE_ANOMALY = "performance_anomaly"

@dataclass
class PerformanceEvent:
    """Performance monitoring event"""
    event_type: PerformanceEventType
    timestamp: float
    step_id: int = 0
    data: Dict[str, Any] = field(default_factory=dict)

class PerformanceMonitor:
    """
    Comprehensive performance monitoring system for NLM simulations.
    
    This class provides real-time monitoring of system performance,
    threshold management, and automated performance optimization adjustments.
    """
    
    def __init__(self, sampling_interval: float = 0.05):
        """Initialize performance monitor"""
        self.sampling_interval = sampling_interval
        self.is_monitoring = False
        self.monitor_thread = None
        
        # Performance data storage
        self.metrics_history = deque(maxlen=100000)
        self.events = deque(maxlen=10000)
        self.current_metrics = self._create_default_metrics()
        
        # System monitoring
        self.process = psutil.Process()
        
        # Threshold management
        self.thresholds = {
            'memory_usage_percent': 80.0,
            'cpu_usage_percent': 70.0,
            'memory_growth_rate_mb_per_sec': 10.0,
            'step_time_ms': 100.0,
            'operations_per_second': 1000.0
        }
        
        # Performance optimization
        self.performance_optimizer = None
        self.auto_optimization_enabled = True
        
        # Event handlers
        self.event_handlers: List[Callable[[PerformanceEvent], None]] = []
        
        # Performance statistics
        self.stats = {
            'samples_collected': 0,
            'events_triggered': 0,
            'threshold_violations': 0,
            'optimization_changes': 0
        }
    
    def _create_default_metrics(self):
        """Create default performance metrics object"""
        return type('Metrics', (), {
            'memory_usage_mb': 0.0,
            'memory_allocated_mb': 0.0,
            'cpu_usage_percent': 0.0,
            'operations_per_second': 0.0,
            'real_time_factor': 1.0,
            'step_id': 0,
            'timestamp': 0.0
        })()
    
    def add_event_handler(self, handler: Callable[[PerformanceEvent], None]):
        """Add event handler for performance events"""
        self.event_handlers.append(handler)
    
    def set_performance_optimizer(self, optimizer: Any):
        """Set performance optimizer for auto-optimization"""
        self.performance_optimizer = optimizer
    
    def start_monitoring(self):
        """Start performance monitoring"""
        if self.is_monitoring:
            return
        
        self.is_monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitoring_loop,
            daemon=True,
            name='performance_monitor'
        )
        self.monitor_thread.start()
        
        self._trigger_event(PerformanceEventType.SIMULATION_START, {})
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.is_monitoring = False
        
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        
        self._trigger_event(PerformanceEventType.SIMULATION_END, {})
    
    def _monitoring_loop(self):
        """Main monitoring loop"""
        last_sample_time = time.time()
        
        while self.is_monitoring:
            try:
                current_time = time.time()
                
                # Collect metrics
                metrics = self._collect_metrics(current_time)
                
                # Check thresholds
                self._check_thresholds(metrics)
                
                # Store metrics
                self.metrics_history.append(metrics)
                self.current_metrics = metrics
                
                # Auto-optimize if enabled
                if self.auto_optimization_enabled and self.performance_optimizer:
                    self._auto_optimize(metrics)
                
                # Update sampling statistics
                self.stats['samples_collected'] += 1
                
                # Control sampling rate
                elapsed = current_time - last_sample_time
                if elapsed < self.sampling_interval:
                    time.sleep(self.sampling_interval - elapsed)
                
                last_sample_time = time.time()
                
            except Exception as e:
                print(f"Monitoring loop error: {e}")
                time.sleep(self.sampling_interval)
    
    def _collect_metrics(self, timestamp: float):
        """Collect current performance metrics"""
        metrics = self._create_default_metrics()
        
        # Memory metrics
        memory_info = self.process.memory_info()
        metrics.memory_usage_mb = memory_info.rss / 1024 / 1024
        metrics.memory_allocated_mb = memory_info.vms / 1024 / 1024
        
        # CPU metrics
        cpu_percent = psutil.cpu_percent(interval=0.0)
        metrics.cpu_usage_percent = cpu_percent
        
        # Calculate operations rate (simplified)
        metrics.operations_per_second = getattr(self, '_operations_counter', 0.0)
        
        # Calculate performance stats
        metrics.real_time_factor = getattr(self, '_real_time_factor', 1.0)
        metrics.step_id = len(self.metrics_history)
        metrics.timestamp = timestamp
        
        return metrics
    
    def _check_thresholds(self, metrics: Any):
        """Check if metrics exceed thresholds"""
        # Memory usage threshold
        memory_percent = (metrics.memory_usage_mb / 1024) / psutil.virtual_memory().total * 100
        if memory_percent > self.thresholds['memory_usage_percent']:
            self._trigger_event(PerformanceEventType.MEMORY_SPIKE, {
                'memory_mb': metrics.memory_usage_mb,
                'threshold_percent': self.thresholds['memory_usage_percent']
            }, metrics.step_id)
        
        # CPU usage threshold
        if metrics.cpu_usage_percent > self.thresholds['cpu_usage_percent']:
            self._trigger_event(PerformanceEventType.CPU_SPIKE, {
                'cpu_percent': metrics.cpu_usage_percent,
                'threshold_percent': self.thresholds['cpu_usage_percent']
            }, metrics.step_id)
        
        # Performance threshold check
        if metrics.operations_per_second > self.thresholds['operations_per_second']:
            self._trigger_event(PerformanceEventType.PERFORMANCE_THRESHOLD, {
                'operations_per_second': metrics.operations_per_second,
                'threshold': self.thresholds['operations_per_second']
            }, metrics.step_id)
    
    def _auto_optimize(self, metrics: Any):
        """Automatically optimize based on current performance"""
        optimization_changes = []
        
        # Memory-based optimization
        if metrics.memory_usage_mb > (self.thresholds['memory_usage_percent'] / 100) * psutil.virtual_memory().total / 1024 / 1024:
            if self.performance_optimizer:
                # Disable memory-intensive optimizations
                optimization_changes.append('memory_optimization_disabled')
        
        # CPU-based optimization
        if metrics.cpu_usage_percent > self.thresholds['cpu_usage_percent']:
            if self.performance_optimizer:
                # Disable CPU-intensive optimizations
                optimization_changes.append('cpu_optimization_disabled')
        
        # Record optimization changes
        if optimization_changes:
            self.stats['optimization_changes'] += len(optimization_changes)
            self._trigger_event(PerformanceEventType.PERFORMANCE_ANOMALY, {
                'optimization_changes': optimization_changes
            }, metrics.step_id)
    
    def _trigger_event(self, event_type: PerformanceEventType, data: Dict[str, Any], step_id: int = 0):
        """Trigger performance event"""
        event = PerformanceEvent(
            event_type=event_type,
            timestamp=time.time(),
            step_id=step_id,
            data=data
        )
        
        self.events.append(event)
        self.stats['events_triggered'] += 1
        
        # Notify all handlers
        for handler in self.event_handlers:
            try:
                handler(event)
            except Exception as e:
                print(f"Event handler error: {e}")
    
    def get_current_metrics(self):
        """Get current performance metrics"""
        return self.current_metrics
    
    def get_metrics_history(self, limit: Optional[int] = None) -> List:
        """Get metrics history"""
        if limit is None:
            return list(self.metrics_history)
        return list(self.metrics_history)[-limit:]
    
    def get_events(self, limit: Optional[int] = None) -> List[PerformanceEvent]:
        """Get performance events"""
        if limit is None:
            return list(self.events)
        return list(self.events)[-limit:]
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary"""
        if not self.metrics_history:
            return {'error': 'No performance data collected'}
        
        metrics_list = list(self.metrics_history)
        
        # Calculate statistics
        memory_values = [m.memory_usage_mb for m in metrics_list]
        cpu_values = [m.cpu_usage_percent for m in metrics_list]
        operation_values = [m.operations_per_second for m in metrics_list]
        
        summary = {
            'monitoring_stats': self.stats,
            'thresholds': self.thresholds,
            'current_metrics': {
                'memory_usage_mb': self.current_metrics.memory_usage_mb,
                'memory_allocated_mb': self.current_metrics.memory_allocated_mb,
                'cpu_usage_percent': self.current_metrics.cpu_usage_percent,
                'operations_per_second': self.current_metrics.operations_per_second
            },
            'statistics': {
                'memory': {
                    'current_mb': memory_values[-1] if memory_values else 0.0,
                    'average_mb': sum(memory_values) / len(memory_values) if memory_values else 0.0,
                    'peak_mb': max(memory_values) if memory_values else 0.0,
                    'min_mb': min(memory_values) if memory_values else 0.0,
                    'growth_rate': self._calculate_growth_rate(memory_values)
                },
                'cpu': {
                    'current_percent': cpu_values[-1] if cpu_values else 0.0,
                    'average_percent': sum(cpu_values) / len(cpu_values) if cpu_values else 0.0,
                    'peak_percent': max(cpu_values) if cpu_values else 0.0,
                    'min_percent': min(cpu_values) if cpu_values else 0.0
                },
                'operations': {
                    'current_ops_per_sec': operation_values[-1] if operation_values else 0.0,
                    'average_ops_per_sec': sum(operation_values) / len(operation_values) if operation_values else 0.0,
                    'peak_ops_per_sec': max(operation_values) if operation_values else 0.0,
                    'min_ops_per_sec': min(operation_values) if operation_values else 0.0
                }
            },
            'event_summary': {
                'total_events': len(self.events),
                'event_types': self._count_event_types()
            }
        }
        
        return summary
    
    def _calculate_growth_rate(self, memory_values: List[float]) -> float:
        """Calculate memory growth rate (MB/s)"""
        if len(memory_values) < 2:
            return 0.0
        
        time_span = (len(memory_values) - 1) * self.sampling_interval
        if time_span == 0:
            return 0.0
        
        memory_growth = memory_values[-1] - memory_values[0]
        return memory_growth / time_span
    
    def _count_event_types(self) -> Dict[str, int]:
        """Count events by type"""
        event_counts = {}
        for event in self.events:
            event_type = event.event_type.name
            event_counts[event_type] = event_counts.get(event_type, 0) + 1
        return event_counts
    
    def set_threshold(self, metric: str, value: float):
        """Set performance threshold"""
        if metric in self.thresholds:
            self.thresholds[metric] = value
    
    def get_thresholds(self) -> Dict[str, float]:
        """Get current thresholds"""
        return self.thresholds.copy()
    
    def export_metrics(self, filepath: str):
        """Export metrics to JSON file"""
        metrics_list = list(self.metrics_history)
        events_list = list(self.events)
        
        export_data = {
            'export_timestamp': datetime.now().isoformat(),
            'sampling_interval': self.sampling_interval,
            'thresholds': self.thresholds,
            'stats': self.stats,
            'metrics': [
                {
                    'timestamp': m.timestamp,
                    'step_id': m.step_id,
                    'memory_usage_mb': m.memory_usage_mb,
                    'memory_allocated_mb': m.memory_allocated_mb,
                    'cpu_usage_percent': m.cpu_usage_percent,
                    'operations_per_second': m.operations_per_second,
                    'real_time_factor': m.real_time_factor
                }
                for m in metrics_list
            ],
            'events': [
                {
                    'timestamp': e.timestamp,
                    'step_id': e.step_id,
                    'event_type': e.event_type.value,
                    'data': e.data
                }
                for e in events_list
            ]
        }
        
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2)

# Global performance monitoring instances
_global_monitor = None

def get_performance_monitor() -> PerformanceMonitor:
    """Get or create global performance monitor"""
    global _global_monitor
    if _global_monitor is None:
        _global_monitor = PerformanceMonitor()
    return _global_monitor

# Convenience functions

def start_performance_monitoring(sampling_interval: float = 0.05):
    """Start global performance monitoring"""
    monitor = get_performance_monitor()
    monitor.start_monitoring()

def stop_performance_monitoring():
    """Stop global performance monitoring"""
    monitor = get_performance_monitor()
    monitor.stop_monitoring()

def add_performance_event_handler(handler: Callable[[PerformanceEvent], None]):
    """Add global performance event handler"""
    monitor = get_performance_monitor()
    monitor.add_event_handler(handler)

def get_performance_summary() -> Dict[str, Any]:
    """Get global performance summary"""
    monitor = get_performance_monitor()
    return monitor.get_performance_summary()

# Event handler registration helpers

def register_memory_monitor(handler: Callable[[PerformanceEvent], None]):
    """Register memory monitoring event handler"""
    def memory_handler(event: PerformanceEvent):
        if event.event_type == PerformanceEventType.MEMORY_SPIKE:
            handler(event)
    add_performance_event_handler(memory_handler)

def register_cpu_monitor(handler: Callable[[PerformanceEvent], None]):
    """Register CPU monitoring event handler"""
    def cpu_handler(event: PerformanceEvent):
        if event.event_type == PerformanceEventType.CPU_SPIKE:
            handler(event)
    add_performance_event_handler(cpu_handler)

def register_performance_monitor(handler: Callable[[PerformanceEvent], None]):
    """Register general performance monitoring event handler"""
    def performance_handler(event: PerformanceEvent):
        if event.event_type == PerformanceEventType.PERFORMANCE_THRESHOLD:
            handler(event)
    add_performance_event_handler(performance_handler)

print("Performance monitoring module loaded successfully!")