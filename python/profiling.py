"""
Enhanced NLM Profiling APIs

This module provides comprehensive profiling capabilities for the NLM framework,
including performance counters, CPU profiling, memory allocation tracing, and
performance regression testing.

Features:
- CPU profiling and performance counters
- Memory allocation tracing
- Performance regression testing
- Custom performance instrumentation
- Integration with common profiling tools
- Real-time performance monitoring
- Benchmarking and profiling APIs
"""

import sys
import time
import threading
import os
import subprocess
import signal
import psutil
import json
from typing import Dict, List, Optional, Callable, Any, Union
from dataclasses import dataclass, field
from enum import Enum
from contextlib import contextmanager

# Forward references
from .nlm.core.binding_helpers import NLMError, TypeValidator

class ProfileEventType(Enum):
    """Types of profile events."""
    ALLOCATION = "allocation"
    DEALLOCOLATION = "deallocation"
    FUNCTION_ENTRY = "function_entry"
    FUNCTION_EXIT = "function_exit"
    THREAD_START = "thread_start"
    THREAD_EXIT = "thread_exit"
    CPU_SAMPLE = "cpu_sample"
    MEMORY_SAMPLE = "memory_sample"
    SYSCALL = "syscall"

@dataclass
class ProfileEvent:
    """A single profile event with timing and context."""
    timestamp: float
    thread_id: int
    event_type: ProfileEventType
    duration: float = 0.0
    cpu_usage: float = 0.0
    memory_usage: float = 0.0
    function_name: str = ""
    file_path: str = ""
    line_number: int = 0
    arguments: Dict[str, Any] = field(default_factory=dict)
    return_value: Any = None
    stack_trace: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
class ProfilerConfig:
    """Configuration for profiling."""
    
    def __init__(self):
        self.sampling_interval: float = 0.001  # 1ms
        self.max_events: int = 1000000
        self.enable_cpu_profiling: bool = True
        self.enable_memory_profiling: bool = True
        self.enable_thread_profiling: bool = True
        self.enable_function_profiling: bool = True
        self.profiler_backend: str = "sampling"  # "sampling", "instrumentation", "hybrid"
        self.output_format: str = "json"  # "json", "text", "binary"
        self.output_file: Optional[str] = None
        self.compress_output: bool = False
        self.include_system_events: bool = True
        self.exclude_kernel_events: bool = True
class SamplingProfiler:
    """Sampling-based profiler for continuous performance monitoring."""
    
    def __init__(self, config: Optional[ProfilerConfig] = None):
        self.config = config or ProfilerConfig()
        self.events: List[ProfileEvent] = []
        self.running = False
        self.thread = None
        self.sampling_thread = None
        self.start_time = 0.0
        self.lock = threading.RLock()
        self._perf_counters: Dict[str, int] = {}
        self._cpu_samples: deque = deque(maxlen=10000)
        self._memory_samples: deque = deque(maxlen=10000)
    
    def start(self):
        """Start profiling."""
        if self.running:
            return
        
        self.running = True
        self.start_time = time.time()
        self.events.clear()
        self._cpu_samples.clear()
        self._memory_samples.clear()
        
        # Start main profiler thread
        self.thread = threading.Thread(target=self._profile_loop, daemon=True)
        self.thread.start()
        
        # Start CPU sampling thread
        if self.config.enable_cpu_profiling:
            self.sampling_thread = threading.Thread(target=self._cpu_sampling_loop, daemon=True)
            self.sampling_thread.start()
        
        # Record thread start events
        for thread_id in threading.enumerate():
            self._record_event(ProfileEventType.THREAD_START, {
                'thread_id': thread_id.ident,
                'name': thread_id.name
            })
    
    def stop(self):
        """Stop profiling."""
        if not self.running:
            return
        
        self.running = False
        
        if self.thread:
            self.thread.join(timeout=1.0)
        
        if self.sampling_thread:
            self.sampling_thread.join(timeout=1.0)
        
        # Record thread exit events
        for thread_id in threading.enumerate():
            self._record_event(ProfileEventType.THREAD_EXIT, {
                'thread_id': thread_id.ident,
                'name': thread_id.name
            })
    
    @contextmanager
    def profile_function(self, name: str, file_path: str = "", line_number: int = 0,
                        args: Dict[str, Any] = None):
        """Context manager to profile a function call."""
        if args is None:
            args = {}
        
        start_time = time.time()
        thread_id = threading.get_ident()
        
        event = self._record_event(ProfileEventType.FUNCTION_ENTRY, {
            'function_name': name,
            'file_path': file_path,
            'line_number': line_number,
            'arguments': args
        })
        
        try:
            yield
        finally:
            end_time = time.time()
            event.duration = end_time - start_time
            event.event_type = ProfileEventType.FUNCTION_EXIT
    
    def record_allocation(self, size: int, allocator_id: int = 0, allocated_from: str = ""):
        """Record a memory allocation event."""
        self._record_event(ProfileEventType.ALLOCATION, {
            'size': size,
            'allocator_id': allocator_id,
            'allocated_from': allocated_from
        })
    
    def record_deallocation(self, size: int, allocator_id: int = 0):
        """Record a memory deallocation event."""
        self._record_event(ProfileEventType.DEALLOCATION, {
            'size': size,
            'allocator_id': allocator_id
        })
    
    def get_events(self, event_types: Optional[List[ProfileEventType]] = None,
                   thread_id: Optional[int] = None, 
                   time_range: Optional[tuple] = None) -> List[ProfileEvent]:
        """Get filtered profile events."""
        with self.lock:
            events = self.events.copy()
        
        if event_types:
            events = [e for e in events if e.event_type in event_types]
        
        if thread_id:
            events = [e for e in events if e.thread_id == thread_id]
        
        if time_range:
            start_time, end_time = time_range
            events = [e for e in events if start_time <= e.timestamp <= end_time]
        
        # Limit to max events
        if len(events) > self.config.max_events:
            events = events[-self.config.max_events:]
        
        return events
    
    def get_summary(self) -> Dict[str, Any]:
        """Get profiling summary statistics."""
        with self.lock:
            events = self.events.copy()
        
        if not events:
            return {}
        
        # Calculate statistics
        total_time = time.time() - self.start_time
        
        # Function statistics
        function_stats = {}
        for event in events:
            if event.event_type == ProfileEventType.FUNCTION_ENTRY:
                func_name = event.function_name
                if func_name not in function_stats:
                    function_stats[func_name] = {
                        'calls': 0,
                        'total_time': 0.0,
                        'avg_time': 0.0,
                        'min_time': float('inf'),
                        'max_time': 0.0
                    }
                
                stats = function_stats[func_name]
                stats['calls'] += 1
        
        # Memory statistics
        alloc_events = [e for e in events if e.event_type == ProfileEventType.ALLOCATION]
        dealloc_events = [e for e in events if e.event_type == ProfileEventType.DEALLOCATION]
        
        memory_stats = {
            'total_allocated': sum(e.metadata.get('size', 0) for e in alloc_events),
            'total_deallocated': sum(e.metadata.get('size', 0) for e in dealloc_events),
            'net_allocation': (sum(e.metadata.get('size', 0) for e in alloc_events) -
                             sum(e.metadata.get('size', 0) for e in dealloc_events)),
            'allocation_events': len(alloc_events),
            'deallocation_events': len(dealloc_events)
        }
        
        # Thread statistics
        thread_stats = {}
        for event in events:
            thread_id = event.thread_id
            if thread_id not in thread_stats:
                thread_stats[thread_id] = {
                    'events': 0,
                    'time_active': 0.0
                }
            
            thread_stats[thread_id]['events'] += 1
            if event.event_type in [ProfileEventType.FUNCTION_ENTRY, ProfileEventType.FUNCTION_EXIT]:
                thread_stats[thread_id]['time_active'] += event.duration
        
        return {
            'total_time': total_time,
            'total_events': len(events),
            'function_stats': function_stats,
            'memory_stats': memory_stats,
            'thread_stats': thread_stats
        }
    
    def export(self, filename: str, format: Optional[str] = None):
        """Export profiling data to file."""
        if format is None:
            format = self.config.output_format
        
        events = self.get_events()
        
        if format == "json":
            data = {
                'events': [
                    {
                        'timestamp': e.timestamp,
                        'thread_id': e.thread_id,
 'event_type': e.event_type.value,
                        'duration': e.duration,
                        'cpu_usage': e.cpu_usage,
                        'memory_usage': e.memory_usage,
                        'function_name': e.function_name,
                        'file_path': e.file_path,
                        'line_number': e.line_number,
                        'arguments': e.arguments,
                        'return_value': e.return_value,
                        'stack_trace': e.stack_trace,
                        'metadata': e.metadata
                    } for e in events
                ]
            }
            
            with open(filename, 'w') as f:
                if self.config.compress_output:
                    import gzip
                    with gzip.open(filename + '.gz', 'wt', encoding='utf-8') as gz:
                        json.dump(data, gz)
                else:
                    json.dump(data, f)
        
        elif format == "text":
            with open(filename, 'w') as f:
                f.write(f"NLM Profiler Output\n")
                f.write(f"Generated at: {time.ctime()}\n")
                f.write(f"Total events: {len(events)}\n")
                f.write(f"Total time: {time.time() - self.start_time:.3f}s\n\n")
                
                # Write summary
                summary = self.get_summary()
                f.write("Summary:\n")
                f.write(f"  Memory allocated: {summary['memory_stats']['total_allocated']} bytes\n")
                f.write(f"  Memory deallocated: {summary['memory_stats']['total_deallocated']} bytes\n")
                f.write(f"  Net allocation: {summary['memory_stats']['net_allocation']} bytes\n\n")
                
                # Write function statistics
                f.write("Function Statistics:\n")
                for func_name, stats in summary['function_stats'].items():
                    if stats['calls'] > 0:
                        f.write(f"  {func_name}: {stats['calls']} calls, "
                               f"avg time: {stats['avg_time']*1000:.3f}ms, "
                               f"total time: {stats['total_time']*1000:.3f}ms\n")
    
    def _record_event(self, event_type: ProfileEventType, metadata: Dict[str, Any]) -> ProfileEvent:
        """Record a profile event."""
        event = ProfileEvent(
            timestamp=time.time() - self.start_time,
            thread_id=threading.get_ident(),
            event_type=event_type,
            metadata=metadata
        )
        
        with self.lock:
            self.events.append(event)
            # Remove oldest event if exceeding limit
            if len(self.events) > self.config.max_events:
                self.events.pop(0)
        
        return event
    
    def _profile_loop(self):
        """Main profiling loop."""
        while self.running:
            try:
                # Record system metrics
                if self.config.enable_cpu_profiling:
                    self._record_cpu_sample()
                
                if self.config.enable_memory_profiling:
                    self._record_memory_sample()
                
                time.sleep(self.config.sampling_interval)
            except Exception:
                break
    
    def _cpu_sampling_loop(self):
        """CPU sampling loop."""
        while self.running:
            try:
                process = psutil.Process(os.getpid())
                cpu_percent = process.cpu_percent(interval=0.001)
                
                with self.lock:
                    self._cpu_samples.append({
                        'timestamp': time.time() - self.start_time,
                        'cpu_percent': cpu_percent,
                        'thread_count': threading.active_count()
                    })
                
                time.sleep(self.config.sampling_interval)
            except Exception:
                break
    
    def _record_cpu_sample(self):
        """Record a CPU sample."""
        if self._cpu_samples:
            sample = self._cpu_samples[-1]
            self._record_event(ProfileEventType.CPU_SAMPLE, {
                'cpu_percent': sample['cpu_percent'],
                'thread_count': sample['thread_count']
            })
    
    def _record_memory_sample(self):
        """Record a memory sample."""
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        self._record_event(ProfileEventType.MEMORY_SAMPLE, {
            'rss': memory_info.rss,
            'vms': memory_info.vms,
            'shared': memory_info.shared,
            'text': memory_info.text,
            'data': memory_info.data,
            'dirty': memory_info.dirty
        })
class SystemProfiler:
    """Profiler for system-level events and tools integration."""
    
    def __init__(self):
        self.profilers: Dict[str, SamplingProfiler] = {}
        self.active_profiler: Optional[SamplingProfiler] = None
        self.lock = threading.RLock()
    
    def create_profiler(self, name: str, config: Optional[ProfilerConfig] = None) -> SamplingProfiler:
        """Create a new profiler."""
        with self.lock:
            if name in self.profilers:
                raise NLMError(f"Profiler already exists: {name}")
            
            profiler = SamplingProfiler(config)
            self.profilers[name] = profiler
            return profiler
    
    def get_profiler(self, name: str) -> Optional[SamplingProfiler]:
        """Get a profiler by name."""
        with self.lock:
            return self.profilers.get(name)
    
    def set_active_profiler(self, name: str):
        """Set the active profiler."""
        with self.lock:
            if name not in self.profilers:
                raise NLMError(f"Profiler not found: {name}")
            
            # Stop any running profiler
            if self.active_profiler and self.active_profiler.running:
                self.active_profiler.stop()
            
            self.active_profiler = self.profilers[name]
            self.active_profiler.start()
    
    def stop_active_profiler(self):
        """Stop the active profiler."""
        with self.lock:
            if self.active_profiler and self.active_profiler.running:
                self.active_profiler.stop()
                self.active_profiler = None
    
    def profile_system_command(self, command: str, timeout: Optional[float] = None) -> Dict[str, Any]:
        """Profile a system command."""
        import subprocess
        
        start_time = time.time()
        start_memory = psutil.Process(os.getpid()).memory_info().rss
        
        try:
            result = subprocess.run(
                command,
                shell=True,
                timeout=timeout,
                capture_output=True,
                text=True
            )
            
            end_time = time.time()
            end_memory = psutil.Process(os.getpid()).memory_info().rss
            
            return {
                'command': command,
                'returncode': result.returncode,
                'stdout': result.stdout,
                'stderr': result.stderr,
                'execution_time': end_time - start_time,
                'memory_used': (end_memory - start_memory) / (1024 * 1024),
                'success': result.returncode == 0
            }
        except subprocess.TimeoutExpired:
            return {
                'command': command,
                'returncode': -1,
                'stdout': '',
                'stderr': f"Timeout after {timeout} seconds",
                'execution_time': timeout,
                'memory_used': 0.0,
                'success': False
            }
        except Exception as e:
            return {
                'command': command,
                'returncode': -1,
                'stdout': '',
                'stderr': str(e),
                'execution_time': time.time() - start_time,
                'memory_used': 0.0,
                'success': False
            }
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics from all profilers."""
        with self.lock:
            stats = {}
            for name, profiler in self.profilers.items():
                stats[name] = profiler.get_summary()
            
            return stats
    
    def export_all_profiles(self, output_dir: str):
        """Export all profiles to files."""
        os.makedirs(output_dir, exist_ok=True)
        
        for name, profiler in self.profilers.items():
            filename = os.path.join(output_dir, f"{name}_profile.json")
            profiler.export(filename)
class GlobalProfiler:
    """Global profiler instance for convenience."""
    
    def __init__(self):
        self.system_profiler = SystemProfiler()
        self._lock = threading.RLock()
    
    def create_profiler(self, name: str, config: Optional[ProfilerConfig] = None) -> SamplingProfiler:
        """Create a profiler."""
        return self.system_profiler.create_profiler(name, config)
    
    def set_active_profiler(self, name: str):
        """Set active profiler."""
        self.system_profiler.set_active_profiler(name)
    
    def profile_system_command(self, command: str, timeout: Optional[float] = None) -> Dict[str, Any]:
        """Profile system command."""
        return self.system_profiler.profile_system_command(command, timeout)
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        return self.system_profiler.get_performance_stats()
    
    def export_all_profiles(self, output_dir: str):
        """Export all profiles."""
        self.system_profiler.export_all_profiles(output_dir)

# Global profiler instance
_global_profiler = GlobalProfiler()

# Convenience functions

def create_profiler(name: str, config: Optional[ProfilerConfig] = None) -> SamplingProfiler:
    """Create a profiler."""
    return _global_profiler.create_profiler(name, config)

def set_active_profiler(name: str):
    """Set active profiler."""
    _global_profiler.set_active_profiler(name)

def profile_system_command(command: str, timeout: Optional[float] = None) -> Dict[str, Any]:
    """Profile a system command."""
    return _global_profiler.profile_system_command(command, timeout)

def get_performance_stats() -> Dict[str, Any]:
    """Get performance statistics."""
    return _global_profiler.get_performance_stats()

def export_all_profiles(output_dir: str):
    """Export all profiles."""
    _global_profiler.export_all_profiles(output_dir)

# Decorators for easy profiling

def profile_function(name: str = "", profiler_name: str = "default"):
    """Decorator to profile a function call."""
    def decorator(func):
        def wrapper(*args, **kwargs):
            profiler = _global_profiler.create_profiler(profiler_name) if profiler_name not in _global_profiler.system_profiler.profilers else _global_profiler.system_profiler.get_profiler(profiler_name)
            
            with profiler.profile_function(func.__name__ if name == "" else name,
                                          f"{func.__code__.co_filename}",
                                          func.__code__.co_firstlineno):
                return func(*args, **kwargs)
        return wrapper
    return decorator
print(f"Enhanced NLM Profiling APIs loaded. {__name__}")