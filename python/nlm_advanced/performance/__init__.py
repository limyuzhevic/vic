"""
Performance monitoring and metrics collection for NLM simulations.

Provides comprehensive performance monitoring tools including resource usage tracking,
simulation progress tracking, custom metrics collection, and automated tuning suggestions.
"""

import time
import threading
import psutil
import numpy as np
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from collections import defaultdict, deque
import json
from enum import Enum

class ConfigSource(Enum):
    """Configuration source enumeration."""
    Default = "Default"
    File = "File"
    CommandLine = "CommandLine"
    Runtime = "Runtime"

@dataclass
class MetricSample:
    """Single metric sample."""
    timestamp: float
    step: int
    cpu_usage: float
    memory_usage: float
    disk_io: float
    network_io: float
    simulation_fps: float
    step_time: float
    custom_metrics: Dict[str, Any] = field(default_factory=dict)

class ResourceMonitor:
    """Resource usage monitoring for simulations."""
    
    def __init__(self):
        self.samples = deque(maxlen=1000)
        self.monitoring = False
        self.monitor_thread = None
        self.sample_interval = 0.1
        self.process = psutil.Process()
        self.last_disk_io = self.process.disk_io_counters()
        self.last_net_io = self.process.net_io_counters()
    
    def start_monitoring(self, interval: float = 0.1):
        """Start resource monitoring."""
        self.monitoring = True
        self.sample_interval = interval
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop resource monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
    
    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.monitoring:
            self._collect_sample()
            time.sleep(self.sample_interval)
    
    def _collect_sample(self):
        """Collect current resource metrics."""
        timestamp = time.time()
        
        # CPU usage
        cpu_percent = psutil.cpu_percent(interval=0.01)
        
        # Memory usage
        memory_info = self.process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        
        # Disk I/O
        current_disk_io = self.process.disk_io_counters()
        disk_read = (current_disk_io.read_bytes - self.last_disk_io.read_bytes) / self.sample_interval
        disk_write = (current_disk_io.write_bytes - self.last_disk_io.write_bytes) / self.sample_interval
        self.last_disk_io = current_disk_io
        
        # Network I/O
        current_net_io = self.process.net_io_counters()
        net_sent = (current_net_io.bytes_sent - self.last_net_io.bytes_sent) / self.sample_interval
        net_recv = (current_net_io.bytes_recv - self.last_net_io.bytes_recv) / self.sample_interval
        self.last_net_io = current_net_io
        
        # Create sample (step will be added by simulation tracker)
        sample = MetricSample(
            timestamp=timestamp,
            step=0,  # Will be set by tracker
            cpu_usage=cpu_percent,
            memory_usage=memory_mb,
            disk_io=disk_read + disk_write,
            network_io=net_sent + net_recv,
            simulation_fps=0.0,  # Will be set by simulation tracker
            step_time=0.0,  # Will be set by simulation tracker
            custom_metrics={}
        )
        
        self.samples.append(sample)
    
    def get_resource_stats(self) -> Dict:
        """Get comprehensive resource statistics."""
        if not self.samples:
            return {}
        
        samples = list(self.samples)
        
        stats = {
            'current': {
                'cpu': samples[-1].cpu_usage,
                'memory_mb': samples[-1].memory_usage,
                'disk_io': samples[-1].disk_io,
                'network_io': samples[-1].network_io
            },
            'average': {
                'cpu': np.mean([s.cpu_usage for s in samples]),
                'memory_mb': np.mean([s.memory_usage for s in samples]),
                'disk_io': np.mean([s.disk_io for s in samples]),
                'network_io': np.mean([s.network_io for s in samples])
            },
            'max': {
                'cpu': np.max([s.cpu_usage for s in samples]),
                'memory_mb': np.max([s.memory_usage for s in samples]),
                'disk_io': np.max([s.disk_io for s in samples]),
                'network_io': np.max([s.network_io for s in samples])
            },
            'min': {
                'cpu': np.min([s.cpu_usage for s in samples]),
                'memory_mb': np.min([s.memory_usage for s in samples]),
                'disk_io': np.min([s.disk_io for s in samples]),
                'network_io': np.min([s.network_io for s in samples])
            },
            'samples': len(samples)
        }
        
        return stats

class SimulationProgressTracker:
    """Track simulation progress and completion."""
    
    def __init__(self, total_steps: int, total_time: Optional[float] = None):
        self.total_steps = total_steps
        self.total_time = total_time
        self.completed_steps = 0
        self.start_time = time.time()
        self.step_times = []
        self.progress_callbacks = []
        self.completion_callbacks = []
    
    def step_completed(self, step_time: Optional[float] = None):
        """Record completion of a simulation step."""
        self.completed_steps += 1
        
        if step_time is not None:
            self.step_times.append(step_time)
        
        # Estimate remaining time
        estimated_total_time = self._estimate_total_time()
        time_elapsed = time.time() - self.start_time
        time_remaining = estimated_total_time - time_elapsed
        
        progress_info = {
            'completed_steps': self.completed_steps,
            'total_steps': self.total_steps,
            'progress_percentage': (self.completed_steps / self.total_steps) * 100,
            'time_elapsed': time_elapsed,
            'time_remaining': max(0, time_remaining),
            'estimated_completion': self.start_time + estimated_total_time,
            'avg_step_time': np.mean(self.step_times) if self.step_times else 0,
            'eta_per_step': time_remaining / max(1, self.total_steps - self.completed_steps)
        }
        
        # Call progress callbacks
        for callback in self.progress_callbacks:
            callback(progress_info)
        
        # Check for completion
        if self.completed_steps >= self.total_steps:
            self._notify_completion()
        
        return progress_info
    
    def _estimate_total_time(self) -> float:
        """Estimate total simulation time based on current progress."""
        if not self.step_times or self.completed_steps == 0:
            return self.total_time or 3600.0  # Default 1 hour
        
        avg_step_time = np.mean(self.step_times)
        remaining_steps = self.total_steps - self.completed_steps
        return (avg_step_time * remaining_steps) + (self.total_time or 0)
    
    def _notify_completion(self):
        """Notify completion and call completion callbacks."""
        completion_info = {
            'completed_steps': self.completed_steps,
            'total_steps': self.total_steps,
            'total_time': time.time() - self.start_time,
            'avg_step_time': np.mean(self.step_times) if self.step_times else 0
        }
        
        for callback in self.completion_callbacks:
            callback(completion_info)
    
    def add_progress_callback(self, callback: Callable[[Dict], None]):
        """Add a callback for progress updates."""
        self.progress_callbacks.append(callback)
    
    def add_completion_callback(self, callback: Callable[[Dict], None]):
        """Add a callback for completion notification."""
        self.completion_callbacks.append(callback)
    
    def get_progress(self) -> Dict:
        """Get current progress information."""
        return self.step_completed()  # This will update and return current progress

class MetricsCollector:
    """Custom metrics collection for simulations."""
    
    def __init__(self):
        self.metrics_history = defaultdict(list)
        self.custom_metrics = {}
        self.collection_active = False
        self.collection_thread = None
        self.sample_interval = 0.1
    
    def start_collection(self, interval: float = 0.1):
        """Start collecting custom metrics."""
        self.collection_active = True
        self.sample_interval = interval
        self.collection_thread = threading.Thread(target=self._collection_loop, daemon=True)
        self.collection_thread.start()
    
    def stop_collection(self):
        """Stop collecting custom metrics."""
        self.collection_active = False
        if self.collection_thread:
            self.collection_thread.join(timeout=5.0)
            self.collection_thread = None
    
    def _collection_loop(self):
        """Main collection loop."""
        while self.collection_active:
            self._collect_custom_metrics()
            time.sleep(self.sample_interval)
    
    def _collect_custom_metrics(self):
        """Collect custom metrics."""
        timestamp = time.time()
        
        for metric_name, metric_func in self.custom_metrics.items():
            try:
                value = metric_func()
                self.metrics_history[metric_name].append({
                    'timestamp': timestamp,
                    'value': value
                })
            except Exception as e:
                print(f"Error collecting metric {metric_name}: {e}")
    
    def add_metric(self, name: str, func: Callable, interval: float = None):
        """Add a custom metric to collect."""
        if interval:
            # Create a specialized version of the function
            def interval_func():
                return func()
            self.custom_metrics[name] = interval_func
        else:
            self.custom_metrics[name] = func
    
    def get_metric_stats(self, name: str) -> Dict:
        """Get statistics for a specific metric."""
        if name not in self.metrics_history:
            return {}
        
        values = [item['value'] for item in self.metrics_history[name]]
        
        if not values:
            return {}
        
        return {
            'current': values[-1],
            'average': np.mean(values),
            'max': np.max(values),
            'min': np.min(values),
            'std': np.std(values),
            'count': len(values)
        }
    
    def get_all_metrics(self) -> Dict:
        """Get statistics for all collected metrics."""
        all_stats = {}
        for name in self.custom_metrics:
            all_stats[name] = self.get_metric_stats(name)
        return all_stats
    
    def get_time_series(self, name: str, limit: int = None) -> List[Dict]:
        """Get time series data for a metric."""
        if name not in self.metrics_history:
            return []
        
        data = self.metrics_history[name]
        if limit:
            data = data[-limit:]
        
        return data

class AutoTuner:
    """Automated performance tuning suggestions."""
    
    def __init__(self, brain, config):
        self.brain = brain
        self.config = config
        self.performance_history = []
        self.tuning_history = []
        self.suggestions = []
        self.tuning_rules = []
    
    def add_tuning_rule(self, rule: Dict):
        """Add a performance tuning rule."""
        self.tuning_rules.append(rule)
    
    def evaluate_performance(self, metrics: Dict) -> List[str]:
        """Evaluate performance and suggest tuning actions."""
        suggestions = []
        
        # CPU performance
        cpu_usage = metrics.get('cpu_usage', 0)
        if cpu_usage > 90:
            suggestions.append({
                'priority': 'high',
                'action': 'reduce_concurrency',
                'description': f'CPU usage is high ({cpu_usage}%). Consider reducing simulation concurrency.',
                'suggested_change': {'concurrency': max(1, self.config.get('concurrency', 4) - 1)}
            })
        
        # Memory performance
        memory_usage = metrics.get('memory_usage', 0)
        if memory_usage > 2048:  # 2GB
            suggestions.append({
                'priority': 'medium',
                'action': 'enable_compression',
                'description': f'Memory usage is high ({memory_usage}MB). Consider enabling state compression.',
                'suggested_change': {'compress_state': True}
            })
        
        # Simulation performance
        fps = metrics.get('simulation_fps', 0)
        if fps < 1.0:
            suggestions.append({
                'priority': 'high',
                'action': 'reduce_timestep',
                'description': f'Simulation FPS is low ({fps}). Consider reducing time step size.',
                'suggested_change': {'time_step_size': self.config.get('time_step_size', 0.01) * 0.5}
            })
        
        # Check tuning rules
        for rule in self.tuning_rules:
            if self._evaluate_tuning_rule(rule, metrics):
                suggestions.append({
                    'priority': rule.get('priority', 'medium'),
                    'action': rule['action'],
                    'description': rule['description'],
                    'suggested_change': rule.get('suggested_change', {})
                })
        
        self.suggestions.extend(suggestions)
        
        # Apply suggestions if requested
        applied = []
        for suggestion in suggestions:
            if suggestion.get('apply', False):
                applied.append(self._apply_tuning_suggestion(suggestion))
        
        return suggestions
    
    def _evaluate_tuning_rule(self, rule: Dict, metrics: Dict) -> bool:
        """Evaluate if a tuning rule applies."""
        condition = rule.get('condition', {})
        
        for metric, threshold in condition.items():
            value = metrics.get(metric)
            if value is not None and value > threshold:
                continue
            elif value is not None and value < threshold:
                # Handle "less than" conditions
                if 'operator' in rule and rule['operator'] == 'less_than':
                    continue
            else:
                # Metric not available
                return False
        
        return True
    
    def _apply_tuning_suggestion(self, suggestion: Dict) -> Dict:
        """Apply a tuning suggestion."""
        change = suggestion['suggested_change']
        
        # Apply configuration changes
        for key, value in change.items():
            self.config.set(key, value, ConfigSource.Runtime)
        
        # Reinitialize affected components
        self._reinitialize_components(change.keys())
        
        # Record tuning action
        tuning_action = {
            'timestamp': time.time(),
            'suggestion': suggestion,
            'applied': True
        }
        
        self.tuning_history.append(tuning_action)
        
        return tuning_action
    
    def _reinitialize_components(self, changed_keys):
        """Reinitialize components affected by configuration changes."""
        # Reinitialize brain if relevant configs changed
        relevant_keys = ['time_step_size', 'concurrency', 'compress_state']
        if any(key in changed_keys for key in relevant_keys):
            self.brain.reset()
    
    def generate_performance_report(self) -> Dict:
        """Generate a comprehensive performance report."""
        if not self.performance_history:
            return {}
        
        report = {
            'summary': {
                'total_evaluations': len(self.performance_history),
                'total_tunings': len(self.tuning_history),
                'average_cpu': np.mean([p.get('cpu_usage', 0) for p in self.performance_history]),
                'average_memory': np.mean([p.get('memory_usage', 0) for p in self.performance_history]),
                'average_fps': np.mean([p.get('simulation_fps', 0) for p in self.performance_history])
            },
            'suggestions': self.suggestions[-10:],  # Last 10 suggestions
            'tuning_history': self.tuning_history[-10:],  # Last 10 tunings
            'recommendations': self._generate_recommendations()
        }
        
        return report
    
    def _generate_recommendations(self) -> List[str]:
        """Generate performance recommendations."""
        recommendations = []
        
        if len(self.tuning_history) > 5:
            recent_tunings = self.tuning_history[-5:]
            if all(t['applied'] for t in recent_tunings):
                recommendations.append(
                    "Consider increasing simulation complexity - system is optimizing well."
                )
        
        if len(self.suggestions) > 10:
            high_priority_suggestions = [s for s in self.suggestions if s['priority'] == 'high']
            if high_priority_suggestions:
                recommendations.append(
                    f"Address {len(high_priority_suggestions)} high-priority tuning suggestions."
                )
        
        return recommendations

class LoadBalancer:
    """Load balancing utilities for parallel simulations."""
    
    def __init__(self, num_workers: int = None):
        self.num_workers = num_workers or psutil.cpu_count()
        self.work_queue = []
        self.worker_status = {}
        self.load_history = []
        self.load_threshold = 0.8
    
    def submit_task(self, task: Dict, priority: int = 0):
        """Submit a task for load balancing."""
        task['priority'] = priority
        task['submitted_at'] = time.time()
        task['status'] = 'pending'
        
        self.work_queue.append(task)
        
        # Sort by priority
        self.work_queue.sort(key=lambda x: -x['priority'])
    
    def distribute_load(self) -> Dict:
        """Distribute load across workers."""
        if not self.work_queue:
            return {'assigned': [], 'remaining': []}
        
        # Get worker capacities
        available_workers = []
        for i in range(self.num_workers):
            worker_id = f"worker_{i}"
            if worker_id not in self.worker_status:
                self.worker_status[worker_id] = {
                    'current_load': 0,
                    'capacity': 1.0,
                    'available_tasks': 0
                }
            
            available_workers.append(worker_id)
        
        # Assign tasks to workers
        assigned = []
        remaining = []
        
        for task in self.work_queue:
            if not available_workers:
                remaining.append(task)
                continue
            
            # Find least loaded worker
            worker_id = min(available_workers, 
                          key=lambda w: self.worker_status[w]['current_load'])
            
            # Assign task
            task['assigned_to'] = worker_id
            task['assigned_at'] = time.time()
            task['status'] = 'assigned'
            
            assigned.append(task)
            
            # Update worker load
            task_load = task.get('estimated_load', 1.0)
            self.worker_status[worker_id]['current_load'] += task_load
            self.worker_status[worker_id]['available_tasks'] += 1
        
        # Remove completed tasks
        self.work_queue = remaining
        
        # Update load history
        total_load = sum(w['current_load'] for w in self.worker_status.values())
        max_load = sum(w['capacity'] for w in self.worker_status.values())
        load_percentage = total_load / max_load if max_load > 0 else 0
        
        self.load_history.append({
            'timestamp': time.time(),
            'load_percentage': load_percentage,
            'assigned_tasks': len(assigned),
            'remaining_tasks': len(remaining)
        })
        
        return {
            'assigned': assigned,
            'remaining': remaining,
            'worker_status': self.worker_status,
            'load_percentage': load_percentage,
            'available_workers': available_workers
        }
    
    def get_load_stats(self) -> Dict:
        """Get current load statistics."""
        if not self.load_history:
            return {}
        
        recent_loads = [l['load_percentage'] for l in self.load_history[-10:]]
        
        return {
            'current_load': sum(w['current_load'] for w in self.worker_status.values()) / sum(w['capacity'] for w in self.worker_status.values()) if self.worker_status else 0,
            'average_load': np.mean(recent_loads) if recent_loads else 0,
            'max_load': np.max(recent_loads) if recent_loads else 0,
            'tasks_assigned': sum(1 for t in self.work_queue if t['status'] == 'assigned'),
            'workers': len(self.worker_status),
            'queue_size': len(self.work_queue)
        }

# Import required modules
from .. import pynlm