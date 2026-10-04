# Performance Profiling Tools for NLM

# This file provides comprehensive performance profiling capabilities for NLM simulations.

import time
import threading
import statistics
from typing import List, Dict, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import json
from pathlib import Path

@dataclass
class ProfilingMetrics:
    """Container for performance metrics"""
    name: str
    duration: float
    start_time: float = field(default_factory=time.time)
    end_time: float = 0.0
    memory_usage: float = 0.0
    cpu_time: float = 0.0
    peak_memory: float = 0.0
    call_count: int = 0
    min_time: float = float('inf')
    max_time: float = 0.0
    
    def end(self):
        self.end_time = time.time()
        self.duration = self.end_time - self.start_time
        if self.duration < self.min_time:
            self.min_time = self.duration
        if self.duration > self.max_time:
            self.max_time = self.duration
        
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'duration': self.duration,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'memory_usage': self.memory_usage,
            'cpu_time': self.cpu_time,
            'peak_memory': self.peak_memory,
            'call_count': self.call_count,
            'min_time': self.min_time,
            'max_time': self.max_time,
            'average_time': self.duration / max(self.call_count, 1)
        }

@dataclass
class ProfilingSession:
    """Manages a profiling session"""
    name: str
    metrics: List[ProfilingMetrics] = field(default_factory=list)
    start_time: float = field(default_factory=time.time)
    is_running: bool = True
    thread_samples: List[Dict[str, Any]] = field(default_factory=list)
    
    def start(self, operation_name: str) -> ProfilingMetrics:
        metric = ProfilingMetrics(name=operation_name, start_time=time.time())
        self.metrics.append(metric)
        return metric
    
    def end(self, operation_name: str):
        for metric in reversed(self.metrics):
            if metric.name == operation_name and metric.end_time == 0:
                metric.end()
                self.is_running = any(m.end_time == 0 for m in self.metrics)
                break
    
    def add_thread_sample(self, sample: Dict[str, Any]):
        self.thread_samples.append(sample)
    
    def get_total_duration(self) -> float:
        return time.time() - self.start_time
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'start_time': self.start_time,
            'duration': self.get_total_duration(),
            'is_running': self.is_running,
            'metrics': [m.to_dict() for m in self.metrics],
            'thread_samples': self.thread_samples
        }

class PerformanceProfiler:
    """Main profiling class for NLM performance analysis"""
    
    def __init__(self, name: str = "NLM_Profiler"):
        self.name = name
        self.sessions: Dict[str, ProfilingSession] = {}
        self.global_metrics: List[ProfilingMetrics] = []
        self.cpu_samples: List[Dict[str, Any]] = []
        self.memory_samples: List[Dict[str, Any]] = []
    
    def start_session(self, session_name: str) -> ProfilingSession:
        """Start a new profiling session"""
        if session_name in self.sessions:
            raise ValueError(f"Session '{session_name}' already exists")
        
        session = ProfilingSession(session_name)
        self.sessions[session_name] = session
        return session
    
    def end_session(self, session_name: str):
        """End a profiling session"""
        if session_name not in self.sessions:
            raise ValueError(f"Session '{session_name}' does not exist")
        
        session = self.sessions[session_name]
        session.end = True
        self.global_metrics.extend(session.metrics)
    
    def profile_operation(self, session_name: str, operation_name: str) -> Callable:
        """Decorator to profile an operation"""
        def decorator(func: Callable) -> Callable:
            def wrapper(*args, **kwargs):
                if session_name not in self.sessions:
                    session = self.start_session(session_name)
                else:
                    session = self.sessions[session_name]
                
                metric = session.start(operation_name)
                try:
                    start_time = time.time()
                    result = func(*args, **kwargs)
                    end_time = time.time()
                    
                    metric.duration = end_time - start_time
                    metric.call_count += 1
                    if metric.duration < metric.min_time:
                        metric.min_time = metric.duration
                    if metric.duration > metric.max_time:
                        metric.max_time = metric.duration
                    
                    return result
                finally:
                    metric.end()
            return wrapper
        return decorator
    
    def profile_function(self, func: Callable, session_name: str = None, operation_name: str = None) -> Callable:
        """Profile a function with automatic naming"""
        if operation_name is None:
            operation_name = func.__name__
        if session_name is None:
            session_name = self.name
        
        return self.profile_operation(session_name, operation_name)(func)
    
    def sample_memory(self, session_name: str, label: str = None):
        """Sample current memory usage"""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            
            sample = {
                'timestamp': time.time(),
                'rss': memory_info.rss / 1024 / 1024,  # MB
                'vms': memory_info.vms / 1024 / 1024,  # MB
                'label': label or f"memory_sample_{len(self.memory_samples)}"
            }
            
            if session_name in self.sessions:
                self.sessions[session_name].memory_samples.append(sample)
            self.memory_samples.append(sample)
            
        except ImportError:
            print("psutil not available, skipping memory sampling")
    
    def sample_cpu(self, session_name: str, label: str = None):
        """Sample current CPU usage"""
        try:
            import psutil
            process = psutil.Process()
            
            sample = {
                'timestamp': time.time(),
                'cpu_percent': process.cpu_percent(),
                'label': label or f"cpu_sample_{len(self.cpu_samples)}"
            }
            
            if session_name in self.sessions:
                self.sessions[session_name].cpu_samples.append(sample)
            self.cpu_samples.append(sample)
            
        except ImportError:
            print("psutil not available, skipping CPU sampling")
    
    def profile_brain_simulation(self, brain, steps: int, session_name: str = "brain_simulation") -> Dict[str, Any]:
        """Profile a complete brain simulation"""
        if session_name not in self.sessions:
            self.start_session(session_name)
        
        session = self.sessions[session_name]
        self.sample_memory(session_name, "before_simulation")
        self.sample_cpu(session_name, "before_simulation")
        
        @self.profile_operation(session_name, "total_simulation_time")
        def run_simulation():
            for step in range(steps):
                @self.profile_operation(session_name, f"step_{step}")
                def step_brain():
                    brain.step(step)
                    self.sample_memory(session_name, f"step_{step}_memory")
                    self.sample_cpu(session_name, f"step_{step}_cpu")
                
                step_brain()
        
        run_simulation()
        self.sample_memory(session_name, "after_simulation")
        self.sample_cpu(session_name, "after_simulation")
        
        return self.get_session_results(session_name)
    
    def get_session_results(self, session_name: str) -> Dict[str, Any]:
        """Get results for a specific session"""
        if session_name not in self.sessions:
            raise ValueError(f"Session '{session_name}' does not exist")
        
        session = self.sessions[session_name]
        
        # Calculate statistics
        durations = [m.duration for m in session.metrics if m.end_time > 0]
        if durations:
            avg_duration = statistics.mean(durations)
            std_duration = statistics.stdev(durations) if len(durations) > 1 else 0.0
            min_duration = min(durations)
            max_duration = max(durations)
        else:
            avg_duration = std_duration = min_duration = max_duration = 0.0
        
        return {
            'session_name': session_name,
            'total_duration': session.get_total_duration(),
            'num_operations': len(session.metrics),
            'avg_operation_time': avg_duration,
            'std_operation_time': std_duration,
            'min_operation_time': min_duration,
            'max_operation_time': max_duration,
            'operations': [m.to_dict() for m in session.metrics],
            'memory_samples': session.memory_samples,
            'cpu_samples': session.cpu_samples,
            'thread_samples': session.thread_samples
        }
    
    def export_results(self, session_name: str, filepath: str):
        """Export profiling results to a file"""
        results = self.get_session_results(session_name)
        
        with open(filepath, 'w') as f:
            if filepath.endswith('.json'):
                json.dump(results, f, indent=2)
            elif filepath.endswith('.csv'):
                self._export_to_csv(results, f)
            else:
                # Default to JSON
                json.dump(results, f, indent=2)
    
    def _export_to_csv(self, results: Dict[str, Any], stream):
        """Export results to CSV format"""
        import csv
        
        # Write operations
        writer = csv.writer(stream)
        writer.writerow(['Operation', 'Duration', 'Min_Time', 'Max_Time', 'Avg_Time', 'Call_Count'])
        for op in results['operations']:
            writer.writerow([
                op['name'],
                op['duration'],
                op['min_time'],
                op['max_time'],
                op['average_time'],
                op['call_count']
            ])
    
    def generate_report(self, session_name: str) -> str:
        """Generate a human-readable report"""
        results = self.get_session_results(session_name)
        
        report = f"NLM Performance Profiling Report\n"
        report += f"====================================\n"
        report += f"Session: {results['session_name']}\n"
        report += f"Total Duration: {results['total_duration']:.2f} seconds\n"
        report += f"Number of Operations: {results['num_operations']}\n\n"
        
        report += f"Performance Statistics:\n"
        report += f"  Average Operation Time: {results['avg_operation_time']:.4f} seconds\n"
        report += f"  Standard Deviation: {results['std_operation_time']:.4f} seconds\n"
        report += f"  Min Operation Time: {results['min_operation_time']:.4f} seconds\n"
        report += f"  Max Operation Time: {results['max_operation_time']:.4f} seconds\n\n"
        
        report += f"Operation Breakdown:\n"
        for op in results['operations'][:10]:  # Top 10 operations
            report += f"  {op['name']}: {op['duration']:.4f}s (avg: {op['average_time']:.4f}s)\n"
        
        if len(results['operations']) > 10:
            report += f"  ... and {len(results['operations']) - 10} more operations\n\n"
        
        return report

# Convenience profiling functions
def profile_brain_simulation(brain, steps: int, session_name: str = "brain_simulation"):
    """Convenience function to profile a brain simulation"""
    profiler = PerformanceProfiler()
    return profiler.profile_brain_simulation(brain, steps, session_name)

def profile_function(func: Callable, *args, **kwargs):
    """Profile a single function call"""
    profiler = PerformanceProfiler("single_function")
    decorated_func = profiler.profile_function(func, "single_function")
    return decorated_func(*args, **kwargs)

# Global profiler instance
_profiler = PerformanceProfiler("default")

# Global convenience functions
def start_global_session(session_name: str):
    """Start a global profiling session"""
    return _profiler.start_session(session_name)

def profile_global_operation(session_name: str, operation_name: str):
    """Profile an operation in the global session"""
    return _profiler.profile_operation(session_name, operation_name)

def get_global_results(session_name: str) -> Dict[str, Any]:
    """Get results from global session"""
    return _profiler.get_session_results(session_name)

# Export functions
__all__ = [
    'ProfilingMetrics',
    'ProfilingSession', 
    'PerformanceProfiler',
    'profile_brain_simulation',
    'profile_function',
    'start_global_session',
    'profile_global_operation',
    'get_global_results',
    '_profiler'
]