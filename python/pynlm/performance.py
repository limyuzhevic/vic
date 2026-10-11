# Performance monitoring and optimization for NLM
from typing import Optional, List, Dict, Any, Callable
import time
import threading
import sys
from enum import Enum
from dataclasses import dataclass
from collections import defaultdict, deque
import statistics
from contextlib import contextmanager

class ProfilingLevel(Enum):
    """Profiling levels for performance monitoring."""
    OFF = 0
    BASIC = 1
    DETAILED = 2
    TRACING = 3
    CUSTOM = 4

@dataclass
class PerformanceMetrics:
    """Performance metrics collection."""
    
    step_count: int = 0
    total_time: float = 0.0
    avg_step_time: float = 0.0
    min_step_time: float = 0.0
    max_step_time: float = 0.0
    steps_per_second: float = 0.0
    spike_count: int = 0
    spike_frequency: float = 0.0
    neuron_state_count: int = 0
    memory_usage: float = 0.0
    cpu_usage: float = 0.0
    
    # Custom metrics
    custom_metrics: Dict[str, List[float]] = None
    
    def __post_init__(self):
        if self.custom_metrics is None:
            self.custom_metrics = {}

class TimingProfiler:
    """High-resolution timing profiler for NLM operations."""
    
    def __init__(self, name: str):
        self.name = name
        self.times = deque(maxlen=1000)
        self.count = 0
        self.total_time = 0.0
        self.start_time = time.perf_counter()
        self.enabled = True
    
    def __enter__(self):
        if self.enabled:
            self.start_time = time.perf_counter()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.enabled:
            elapsed = time.perf_counter() - self.start_time
            self.record(elapsed)
    
    def record(self, elapsed_time: float) -> None:
        """Record elapsed time."""
        if self.enabled:
            self.times.append(elapsed_time)
            self.count += 1
            self.total_time += elapsed_time
    
    def get_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.times:
            return {
                'name': self.name,
                'count': 0,
                'total_time': 0.0,
                'avg_time': 0.0,
                'min_time': 0.0,
                'max_time': 0.0,
                'std_time': 0.0
            }
        
        times_list = list(self.times)
        return {
            'name': self.name,
            'count': self.count,
            'total_time': self.total_time,
            'avg_time': statistics.mean(times_list),
            'min_time': min(times_list),
            'max_time': max(times_list),
            'std_time': statistics.stdev(times_list) if len(times_list) > 1 else 0.0,
            'current_rate': 1.0 / statistics.mean(times_list) if times_list else 0.0
        }
    
    def enable(self, enable: bool) -> None:
        """Enable or disable profiling."""
        self.enabled = enable

class SystemProfiler:
    """System-level profiling for NLM operations."""
    
    def __init__(self):
        self.timers = {}
        self.system_metrics = defaultdict(list)
        self.enabled = True
        self.lock = threading.RLock()
    
    def get_timer(self, name: str) -> TimingProfiler:
        """Get or create a timer."""
        with self.lock:
            if name not in self.timers:
                self.timers[name] = TimingProfiler(name)
            return self.timers[name]
    
    def start_timer(self, name: str) -> TimingProfiler:
        """Start a timer."""
        timer = self.get_timer(name)
        return timer
    
    def record_system_metrics(self) -> None:
        """Record system metrics (CPU, memory, etc.)."""
        if not self.enabled:
            return
        
        try:
            # Get memory usage
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            self.system_metrics['memory_rss'].append(memory_info.rss / 1024 / 1024)  # MB
            self.system_metrics['memory_vms'].append(memory_info.vms / 1024 / 1024)  # MB
            
            # Get CPU usage
            cpu_percent = process.cpu_percent()
            self.system_metrics['cpu_percent'].append(cpu_percent)
            
        except ImportError:
            # psutil not available
            self.system_metrics['memory_rss'].append(0.0)
            self.system_metrics['cpu_percent'].append(0.0)
        
        except Exception as e:
            # Other error
            print(f"Error recording system metrics: {e}")
    
    def get_system_stats(self) -> Dict[str, Any]:
        """Get system performance statistics."""
        stats = {}
        
        for metric_name, values in self.system_metrics.items():
            if values:
                stats[metric_name] = {
                    'current': values[-1],
                    'avg': statistics.mean(values),
                    'min': min(values),
                    'max': max(values),
                    'std': statistics.stdev(values) if len(values) > 1 else 0.0
                }
        
        return stats
    
    def get_all_stats(self) -> Dict[str, Any]:
        """Get all profiling statistics."""
        timer_stats = {}
        for name, timer in self.timers.items():
            timer_stats[name] = timer.get_stats()
        
        system_stats = self.get_system_stats()
        
        return {
            'timers': timer_stats,
            'system': system_stats,
            'total_timers': len(self.timers)
        }
    
    def enable(self, enable: bool) -> None:
        """Enable or disable profiling."""
        with self.lock:
            self.enabled = enable
            for timer in self.timers.values():
                timer.enable(enable)

class NLMAnalytics:
    """Advanced analytics for NLM simulations."""
    
    def __init__(self):
        self.performance_monitor = SystemProfiler()
        self.spike_times = deque(maxlen=10000)
        self.neuron_activities = defaultdict(deque)
        self.step_times = deque(maxlen=10000)
        self.memory_usage = deque(maxlen=1000)
        self.optimization_suggestions = []
        self.enabled = True
    
    def record_step_timing(self, step_time: float) -> None:
        """Record step execution time."""
        if self.enabled:
            self.step_times.append(step_time)
            self.performance_monitor.record_system_metrics()
    
    def record_spike(self, timestamp: float, neuron_id: int) -> None:
        """Record a spike event."""
        if self.enabled:
            self.spike_times.append(timestamp)
            self.neuron_activities[neuron_id].append(timestamp)
    
    def get_neuron_activity(self, neuron_id: int) -> List[float]:
        """Get activity for a specific neuron."""
        return list(self.neuron_activities.get(neuron_id, []))
    
    def get_spike_stats(self) -> Dict[str, Any]:
        """Get spike statistics."""
        if not self.spike_times:
            return {
                'spike_count': 0,
                'spike_rate': 0.0,
                'firing_frequency': 0.0
            }
        
        spike_times_list = list(self.spike_times)
        time_span = spike_times_list[-1] - spike_times_list[0] if len(spike_times_list) > 1 else 0
        
        return {
            'spike_count': len(spike_times_list),
            'spike_rate': len(spike_times_list) / time_span if time_span > 0 else 0,
            'firing_frequency': len(spike_times_list) / max(time_span, 1.0) if spike_times_list else 0
        }
    
    def get_step_stats(self) -> Dict[str, Any]:
        """Get step execution statistics."""
        if not self.step_times:
            return {
                'step_count': 0,
                'avg_step_time': 0.0,
                'min_step_time': 0.0,
                'max_step_time': 0.0,
                'stability': 0.0
            }
        
        step_times_list = list(self.step_times)
        
        return {
            'step_count': len(step_times_list),
            'avg_step_time': statistics.mean(step_times_list),
            'min_step_time': min(step_times_list),
            'max_step_time': max(step_times_list),
            'stability': 1.0 - (statistics.stdev(step_times_list) / statistics.mean(step_times_list) if len(step_times_list) > 1 else 0)
        }
    
    def generate_optimization_suggestions(self) -> List[str]:
        """Generate optimization suggestions based on performance data."""
        suggestions = []
        
        step_stats = self.get_step_stats()
        spike_stats = self.get_spike_stats()
        
        # Check for slow steps
        if step_stats['max_step_time'] > step_stats['avg_step_time'] * 3:
            suggestions.append(
                f"Step execution time varies significantly (max: {step_stats['max_step_time']*1000:.2f}ms, avg: {step_stats['avg_step_time']*1000:.2f}ms). "
                "Consider optimizing neuron synchronization or synaptic computation."
            )
        
        # Check for low firing rate
        if spike_stats['firing_frequency'] < 10:  # spikes per second
            suggestions.append(
                f"Low spike firing rate ({spike_stats['firing_frequency']:.2f} Hz). "
                "Consider increasing neuron excitability or synaptic input."
            )
        
        # Check for memory usage growth
        if len(self.memory_usage) > 10:
            recent_memory = list(self.memory_usage)[-10:]
            if max(recent_memory) > min(recent_memory) * 1.5:
                suggestions.append(
                    "Memory usage fluctuates significantly. "
                    "Consider implementing memory pools or more efficient data structures."
                )
        
        # Stability analysis
        if step_stats['stability'] < 0.8:
            suggestions.append(
                f"Simulation stability is low ({step_stats['stability']:.2f}). "
                "Consider reducing time step size or improving convergence algorithms."
            )
        
        return suggestions
    
    def enable(self, enable: bool) -> None:
        """Enable or disable analytics."""
        self.enabled = enable
        self.performance_monitor.enable(enable)

class OptimizationTools:
    """Optimization tools for NLM simulations."""
    
    @staticmethod
    def profile_function(func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """Profile a function's execution."""
        import time
        
        start_time = time.perf_counter()
        try:
            result = func(*args, **kwargs)
            success = True
            error = None
        except Exception as e:
            result = None
            success = False
            error = str(e)
        finally:
            end_time = time.perf_counter()
        
        elapsed = end_time - start_time
        
        return {
            'success': success,
            'elapsed_time': elapsed,
            'result': result,
            'error': error,
            'function_name': func.__name__
        }
    
    @staticmethod
    def benchmark_neuron_system(brain, num_steps: int) -> Dict[str, Any]:
        """Benchmark neuron system performance."""
        import time
        
        start_time = time.perf_counter()
        
        for step in range(num_steps):
            brain.step(step)
        
        end_time = time.perf_counter()
        elapsed = end_time - start_time
        
        return {
            'steps_per_second': num_steps / elapsed,
            'avg_step_time': elapsed / num_steps,
            'total_steps': num_steps,
            'total_time': elapsed,
            'stability': 1.0 - (0.1)  # Placeholder
        }
    
    @staticmethod
    def analyze_spike_patterns(spike_times: List[float]) -> Dict[str, Any]:
        """Analyze spike patterns."""
        if len(spike_times) < 2:
            return {
                'pattern_type': 'insufficient_data',
                'firing_rate': 0.0,
                'burst_count': 0,
                'regularity': 0.0
            }
        
        intervals = [spike_times[i] - spike_times[i-1] for i in range(1, len(spike_times))]
        
        # Calculate firing rate
        time_span = spike_times[-1] - spike_times[0]
        firing_rate = len(spike_times) / time_span if time_span > 0 else 0
        
        # Check for bursts (short intervals)
        burst_threshold = statistics.mean(intervals) * 2
        burst_count = sum(1 for interval in intervals if interval < burst_threshold)
        
        # Calculate regularity (coefficient of variation)
        mean_interval = statistics.mean(intervals)
        std_interval = statistics.stdev(intervals) if len(intervals) > 1 else 0
        regularity = 1.0 - (std_interval / mean_interval) if mean_interval > 0 else 0
        
        return {
            'pattern_type': 'regular' if regularity > 0.8 else 'irregular' if regularity < 0.3 else 'mixed',
            'firing_rate': firing_rate,
            'burst_count': burst_count,
            'regularity': regularity,
            'mean_interval': mean_interval,
            'std_interval': std_interval
        }

# Factory functions

def create_performance_monitor(level: ProfilingLevel = ProfilingLevel.BASIC) -> NLMAnalytics:
    """Create a performance monitor."""
    analytics = NLMAnalytics()
    analytics.performance_monitor.performance_profiler = SystemProfiler()
    analytics.performance_monitor.set_profiling_level(level)
    return analytics

def create_timing_profiler(name: str) -> TimingProfiler:
    """Create a timing profiler."""
    return TimingProfiler(name)

def create_system_profiler() -> SystemProfiler:
    """Create a system profiler."""
    return SystemProfiler()

# Context managers for easy profiling

@contextmanager
def profile_operation(name: str):
    """Context manager for profiling a specific operation."""
    profiler = TimingProfiler(name)
    with profiler:
        yield profiler

@contextmanager
def profile_step(brain, step_num: int):
    """Context manager for profiling a simulation step."""
    start_time = time.perf_counter()
    yield
    end_time = time.perf_counter()
    
    step_time = end_time - start_time
    brain._analytics.record_step_timing(step_time)

@contextmanager
def profile_simulation(brain, num_steps: int):
    """Context manager for profiling a simulation."""
    start_time = time.perf_counter()
    
    for step in range(num_steps):
        with profile_step(brain, step):
            brain.step(step)
    
    end_time = time.perf_counter()
    
    total_time = end_time - start_time
    brain._analytics.performance_monitor.timers['total_simulation'].record(total_time)
