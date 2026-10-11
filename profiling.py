"""
Performance profiling and monitoring for NLM

This module provides comprehensive performance profiling capabilities
including real-time monitoring, memory usage analysis, and optimization recommendations.
"""

import time
import threading
import psutil
import os
from typing import Dict, List, Any, Optional, Union, Callable
from enum import Enum
from dataclasses import dataclass
from collections import defaultdict, deque
from datetime import datetime

class ProfileLevel(Enum):
    """Performance profiling levels."""
    BASIC = "basic"
    DETAILED = "detailed"
    COMPREHENSIVE = "comprehensive"
    REAL_TIME = "real_time"

@dataclass
class ProfileSample:
    """Performance profile sample."""
    
    timestamp: float
    cpu_usage: float
    memory_usage: float
    memory_available: float
    disk_io_read: float
    disk_io_write: float
    network_io_sent: float
    network_io_received: float
    thread_count: int
    process_id: int
    
    # Custom metrics (can be extended)
    custom_metrics: Dict[str, float] = None
    
    def __post_init__(self):
        if self.custom_metrics is None:
            self.custom_metrics = {}

class ProfileResult:
    """Performance profiling results."""
    
    def __init__(self):
        self.start_time = time.time()
        self.samples: List[ProfileSample] = []
        self.duration = 0.0
        self.max_cpu = 0.0
        self.max_memory = 0.0
        self.total_cpu_time = 0.0
        self.total_memory_usage = 0.0
        self.peak_memory = 0.0
        self.operation_times: Dict[str, List[float]] = defaultdict(list)
        self.function_times: Dict[str, float] = {}
        self.allocation_stats: Dict[str, Any] = {}

    def add_sample(self, sample: ProfileSample):
        """Add a profile sample."""
        self.samples.append(sample)
        self.max_cpu = max(self.max_cpu, sample.cpu_usage)
        self.max_memory = max(self.max_memory, sample.memory_usage)
        self.total_memory_usage += sample.memory_usage
        self.peak_memory = max(self.peak_memory, sample.memory_usage)
    
    def add_operation_time(self, operation: str, duration: float):
        """Add operation time to profile."""
        self.operation_times[operation].append(duration)
    
    def add_function_time(self, function: str, duration: float):
        """Add function execution time to profile."""
        if function not in self.function_times:
            self.function_times[function] = 0.0
        self.function_times[function] += duration
    
    def finish(self):
        """Finish profiling and calculate duration."""
        self.duration = time.time() - self.start_time
        
        # Calculate statistics
        if self.samples:
            self.total_cpu_time = sum(sample.cpu_usage for sample in self.samples)

    def get_statistics(self) -> Dict[str, Any]:
        """Get profiling statistics.
        
        Returns:
            Dictionary with performance statistics
        """
        if not self.samples:
            return {}
        
        stats = {
            'duration': self.duration,
            'sample_count': len(self.samples),
            'max_cpu_usage': self.max_cpu,
            'avg_cpu_usage': sum(s.cpu_usage for s in self.samples) / len(self.samples),
            'max_memory_usage': self.max_memory,
            'avg_memory_usage': self.total_memory_usage / len(self.samples),
            'peak_memory': self.peak_memory,
            'total_cpu_time': self.total_cpu_time,
        }
        
        # Add operation statistics
        for operation, times in self.operation_times.items():
            if times:
                stats[f'{operation}_count'] = len(times)
                stats[f'{operation}_avg_time'] = sum(times) / len(times)
                stats[f'{operation}_min_time'] = min(times)
                stats[f'{operation}_max_time'] = max(times)
        
        # Add function statistics
        for function, total_time in self.function_times.items():
            stats[f'{function}_total_time'] = total_time
        
        return stats

    def get_recommendations(self) -> List[str]:
        """Get optimization recommendations based on profile data.
        
        Returns:
            List of optimization recommendations
        """
        recommendations = []
        
        # CPU-related recommendations
        if self.max_cpu > 90:
            recommendations.append("CPU usage is high (>90%). Consider reducing thread count or optimizing algorithms.")
        
        if self.max_cpu > 80 and len(self.samples) > 100:
            recommendations.append("High CPU usage with many samples. Profile specific operations for bottlenecks.")
        
        # Memory-related recommendations
        if self.peak_memory > (psutil.virtual_memory().total * 0.8):
            recommendations.append("High memory usage (>80% of available). Consider memory optimization techniques.")
        
        # Operation-specific recommendations
        for operation, times in self.operation_times.items():
            if times and (sum(times) / len(times)) > 1.0:
                recommendations.append(f"Operation '{operation}' has high average execution time (>1s). Consider optimization.")
        
        # Function-specific recommendations
        for function, total_time in self.function_times.items():
            if total_time > self.duration * 0.5:
                recommendations.append(f"Function '{function}' consumes significant time (>50% of total). Profile for optimization.")
        
        return recommendations

class AdvancedProfiler:
    """Advanced performance profiler with real-time monitoring."""
    
    def __init__(self, level: ProfileLevel = ProfileLevel.DETAILED):
        """Initialize advanced profiler.
        
        Args:
            level: Profiling level
        """
        self.level = level
        self._profiles: Dict[str, ProfileResult] = {}
        self._monitoring_thread: Optional[threading.Thread] = None
        self._monitoring_active = False
        self._monitoring_interval = 0.1  # 100ms
        self._monitored_processes: List[int] = []
        
        # Initialize system monitoring
        self._process = psutil.Process(os.getpid())
        self._monitored_processes.append(os.getpid())
        
    def start_profiling(self, name: str, level: Optional[ProfileLevel] = None) -> 'AdvancedProfiler':
        """Start profiling for a named operation.
        
        Args:
            name: Profile name
            level: Profiling level (uses default if None)
            
        Returns:
            AdvancedProfiler instance
        """
        if level is None:
            level = self.level
        
        if name not in self._profiles:
            self._profiles[name] = ProfileResult()
        
        return self
    
    def stop_profiling(self, name: str) -> Optional[ProfileResult]:
        """Stop profiling for a named operation.
        
        Args:
            name: Profile name
            
        Returns:
            ProfileResult or None if not found
        """
        if name in self._profiles:
            result = self._profiles[name]
            result.finish()
            return result
        return None
    
    def start_real_time_monitoring(self, interval: Optional[float] = None):
        """Start real-time system monitoring.
        
        Args:
            interval: Monitoring interval in seconds (uses default if None)
        """
        if interval is not None:
            self._monitoring_interval = interval
        
        if self._monitoring_active:
            return
        
        self._monitoring_active = True
        self._monitoring_thread = threading.Thread(target=self._monitoring_worker, daemon=True)
        self._monitoring_thread.start()
    
    def stop_real_time_monitoring(self):
        """Stop real-time monitoring."""
        self._monitoring_active = False
        if self._monitoring_thread:
            self._monitoring_thread.join(timeout=1.0)
    
    def enable_real_time_monitoring(self):
        """Enable real-time monitoring."""
        self.start_real_time_monitoring()
    
    def disable_real_time_monitoring(self):
        """Disable real-time monitoring."""
        self.stop_real_time_monitoring()
    
    def get_current_metrics(self) -> Dict[str, float]:
        """Get current system metrics.
        
        Returns:
            Dictionary with current metrics
        """
        try:
            cpu_percent = self._process.cpu_percent()
            memory_info = self._process.memory_info()
            memory_percent = self._process.memory_percent()
            
            return {
                'cpu_usage': cpu_percent,
                'memory_usage': memory_info.rss / (1024 * 1024),  # MB
                'memory_percent': memory_percent,
                'thread_count': self._process.num_threads(),
                'create_time': self._process.create_time(),
            }
        except Exception:
            return {}
    
    def get_profile(self, name: str) -> Optional[ProfileResult]:
        """Get profile result.
        
        Args:
            name: Profile name
            
        Returns:
            ProfileResult or None if not found
        """
        return self._profiles.get(name)
    
    def get_all_profiles(self) -> Dict[str, ProfileResult]:
        """Get all profile results.
        
        Returns:
            Dictionary of profile results
        """
        return self._profiles.copy()
    
    def clear_profile(self, name: str):
        """Clear profile.
        
        Args:
            name: Profile name
        """
        if name in self._profiles:
            del self._profiles[name]
    
    def clear_all_profiles(self):
        """Clear all profiles."""
        self._profiles.clear()
    
    def generate_report(self, name: str, output_file: Optional[str] = None) -> str:
        """Generate profiling report.
        
        Args:
            name: Profile name
            output_file: Output file path (optional)
            
        Returns:
            Generated report
        """
        profile = self.get_profile(name)
        if not profile:
            return f"Profile '{name}' not found"
        
        stats = profile.get_statistics()
        recommendations = profile.get_recommendations()
        
        report = f"Performance Profile Report: {name}\n"
        report += f"Generated at: {datetime.now().isoformat()}\n"
        report += f"Duration: {stats.get('duration', 0):.2f} seconds\n"
        report += f"Total samples: {stats.get('sample_count', 0)}\n"
        report += f"\nPerformance Statistics:\n"
        
        for key, value in stats.items():
            if isinstance(value, float) and key not in ['create_time']:
                if 'time' in key or 'cpu' in key:
                    report += f"  {key}: {value:.4f}\n"
                else:
                    report += f"  {key}: {value:.2f}\n"
            else:
                report += f"  {key}: {value}\n"
        
        if recommendations:
            report += f"\nOptimization Recommendations:\n"
            for recommendation in recommendations:
                report += f"  • {recommendation}\n"
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report)
        
        return report
    
    def profile_function(self, name: str, func: Callable, *args, **kwargs) -> Any:
        """Profile function execution.
        
        Args:
            name: Profile name
            func: Function to profile
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Function result
        """
        if name not in self._profiles:
            self._profiles[name] = ProfileResult()
        
        start_time = time.time()
        
        try:
            result = func(*args, **kwargs)
            
            execution_time = time.time() - start_time
            self._profiles[name].add_function_time(func.__name__, execution_time)
            
            return result
        
        except Exception as e:
            execution_time = time.time() - start_time
            self._profiles[name].add_function_time(f'{func.__name__}_error', execution_time)
            raise e
    
    def profile_operation(self, name: str, operation: str, duration: float = None):
        """Profile operation.
        
        Args:
            name: Profile name
            operation: Operation name
            duration: Operation duration (calculated if None)
        """
        if name not in self._profiles:
            self._profiles[name] = ProfileResult()
        
        if duration is None:
            duration = time.time() - time.time()  # This will be 0, use a placeholder
        
        self._profiles[name].add_operation_time(operation, duration)
    
    def _monitoring_worker(self):
        """Background monitoring worker."""
        while self._monitoring_active:
            try:
                timestamp = time.time()
                
                # Collect system metrics
                cpu_percent = self._process.cpu_percent()
                memory_info = self._process.memory_info()
                
                # Get disk I/O
                disk_io = self._process.io_counters() if hasattr(self._process, 'io_counters') else None
                
                # Get network I/O
                net_io = psutil.net_io_counters() if psutil else None
                
                # Create profile sample
                sample = ProfileSample(
                    timestamp=timestamp,
                    cpu_usage=cpu_percent,
                    memory_usage=memory_info.rss / (1024 * 1024),  # MB
                    memory_available=psutil.virtual_memory().available / (1024 * 1024),  # MB
                    disk_io_read=disk_io.read_bytes / (1024 * 1024) if disk_io else 0.0,  # MB
                    disk_io_write=disk_io.write_bytes / (1024 * 1024) if disk_io else 0.0,  # MB
                    network_io_sent=net_io.bytes_sent / (1024 * 1024) if net_io else 0.0,  # MB
                    network_io_received=net_io.bytes_recv / (1024 * 1024) if net_io else 0.0,  # MB
                    thread_count=self._process.num_threads(),
                    process_id=os.getpid(),
                )
                
                # Add sample to all profiles
                for profile in self._profiles.values():
                    profile.add_sample(sample)
                
                time.sleep(self._monitoring_interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(self._monitoring_interval)

# Create singleton instance for global use
_profiler_instance: Optional[AdvancedProfiler] = None

def get_profiler(level: ProfileLevel = ProfileLevel.DETAILED) -> AdvancedProfiler:
    """Get global profiler instance.
    
    Args:
        level: Profiling level
        
    Returns:
        AdvancedProfiler instance
    """
    global _profiler_instance
    if _profiler_instance is None:
        _profiler_instance = AdvancedProfiler(level)
    return _profiler_instance

def start_profiling(name: str, level: Optional[ProfileLevel] = None) -> AdvancedProfiler:
    """Start profiling using global profiler.
    
    Args:
        name: Profile name
        level: Profiling level
        
    Returns:
        AdvancedProfiler instance
    """
    profiler = get_profiler(level)
    return profiler.start_profiling(name)

def stop_profiling(name: str) -> Optional[ProfileResult]:
    """Stop profiling using global profiler.
    
    Args:
        name: Profile name
        
    Returns:
        ProfileResult or None
    """
    profiler = get_profiler()
    return profiler.stop_profiling(name)

def start_real_time_monitoring():
    """Start real-time monitoring using global profiler."""
    profiler = get_profiler()
    profiler.start_real_time_monitoring()

def stop_real_time_monitoring():
    """Stop real-time monitoring using global profiler."""
    profiler = get_profiler()
    profiler.stop_real_time_monitoring()

# Export types and classes
__all__ = [
    'ProfileLevel',
    'ProfileSample',
    'ProfileResult',
    'AdvancedProfiler',
    'get_profiler',
    'start_profiling',
    'stop_profiling',
    'start_real_time_monitoring',
    'stop_real_time_monitoring',
    'profile_function',
    'profile_operation',
]