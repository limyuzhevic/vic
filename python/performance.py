"""
Enhanced NLM Performance APIs

This module provides performance monitoring, memory management, and profiling capabilities
for the NLM (Neural Learning Machine) framework.

Features:
- Memory usage monitoring and tracking
- Performance counters and CPU profiling
- Real-time performance monitoring
- Benchmarking and profiling APIs
- Resource quota management
- Memory allocation tracing
- Memory leak detection
- Performance regression testing
"""

import sys
import time
import threading
import psutil
import os
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass, field
from contextlib import contextmanager
from collections import deque

# Forward references
from .nlm.core.binding_helpers import NLMError, TypeValidator

@dataclass
class MemoryUsageInfo:
    """Memory usage information for tracking and analysis."""
    total_mb: float = 0.0
    used_mb: float = 0.0
    available_mb: float = 0.0
    allocation_count: int = 0
    deallocation_count: int = 0
    current_objects: int = 0
    peak_memory_mb: float = 0.0
    memory_fragmentation: float = 0.0

@dataclass
class CPUMetrics:
    """CPU usage and performance metrics."""
    user_time: float = 0.0
    system_time: float = 0.0
    cpu_percent: float = 0.0
    frequency: float = 0.0
    cache_misses: int = 0
    cache_hits: int = 0
    context_switches: int = 0
    instructions_per_cycle: float = 0.0

@dataclass
class PerformanceMetrics:
    """General performance metrics for tracking simulation performance."""
    step_time_ms: float = 0.0
    neurons_processed: int = 0
    spikes_generated: int = 0
    synaptic_events: int = 0
    memory_usage_mb: float = 0.0
    cpu_usage_percent: float = 0.0
    timestep_efficiency: float = 0.0
    simd_efficiency: float = 0.0
    multithreading_efficiency: float = 0.0
    active_neurons: int = 0
    firing_neurons: int = 0

@dataclass
class AllocationRecord:
    """Record of memory allocation for tracing and leak detection."""
    timestamp: float = 0.0
    thread_id: int = 0
    size: int = 0
    allocator_id: int = 0
    stack_trace: List[str] = field(default_factory=list)
    allocated_from: str = ""

class MemoryProfiler:
    """Memory usage monitoring and tracking for NLM."""
    
    def __init__(self):
        self._memory_usage_history: deque = deque(maxlen=1000)
        self._allocation_history: deque = deque(maxlen=10000)
        self._allocation_count = 0
        self._deallocation_count = 0
        self._start_time = time.time()
        self._peak_memory = 0.0
        self._total_allocated = 0
        self._lock = threading.RLock()
    
    def get_memory_usage(self) -> MemoryUsageInfo:
        """Get current memory usage information."""
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        with self._lock:
            self._allocation_count = len(self._allocation_history)
            self._deallocation_count = self._allocation_count - sum(
                1 for alloc in self._allocation_history if alloc.size > 0
            )
        
        total_mb = memory_info.rss / (1024 * 1024)
        self._peak_memory = max(self._peak_memory, total_mb)
        
        return MemoryUsageInfo(
            total_mb=memory_info.rss / (1024 * 1024),
            used_mb=memory_info.rss / (1024 * 1024),
            available_mb=psutil.virtual_memory().available / (1024 * 1024),
            allocation_count=self._allocation_count,
            deallocation_count=self._deallocation_count,
            current_objects=self._allocation_count - self._deallocation_count,
            peak_memory_mb=self._peak_memory,
            memory_fragmentation=0.0  # Simplified calculation
        )
    
    def trace_allocation(self, size: int, allocator_id: int = 0, allocated_from: str = ""):
        """Trace a memory allocation."""
        with self._lock:
            import traceback
            stack_trace = traceback.format_stack()[-5:]  # Get last 5 stack frames
            self._allocation_history.append(AllocationRecord(
                timestamp=time.time() - self._start_time,
                thread_id=threading.get_ident(),
                size=size,
                allocator_id=allocator_id,
                stack_trace=[str(frame) for frame in stack_trace],
                allocated_from=allocated_from
            ))
            self._total_allocated += size
    
    def trace_deallocation(self, size: int):
        """Trace a memory deallocation."""
        with self._lock:
            # Remove a corresponding allocation record
            for i in range(len(self._allocation_history) - 1, -1, -1):
                if self._allocation_history[i].size == size:
                    self._allocation_history.pop(i)
                    break
    
    def detect_memory_leaks(self) -> List[AllocationRecord]:
        """Detect potential memory leaks by checking for un-deallocated allocations."""
        with self._lock:
            # Simple leak detection: allocations without matching deallocations
            leaked = []
            for alloc in self._allocation_history:
                if alloc.size > 0:  # Still allocated
                    leaked.append(alloc)
            return leaked
    
    def get_allocation_history(self, limit: Optional[int] = None) -> List[AllocationRecord]:
        """Get allocation history for analysis."""
        with self._lock:
            history = list(self._allocation_history)
            if limit is not None:
                history = history[-limit:]
            return history

class CPUProfiler:
    """CPU profiling and performance monitoring for NLM."""
    
    def __init__(self):
        self._cpu_metrics_history: deque = deque(maxlen=1000)
        self._perf_counters: Dict[str, int] = {}
        self._start_time = time.time()
        self._lock = threading.RLock()
    
    def get_cpu_metrics(self) -> CPUMetrics:
        """Get current CPU metrics."""
        process = psutil.Process(os.getpid())
        
        with self._lock:
            # Get CPU times
            cpu_times = process.cpu_times()
            
            # Get CPU percent
            cpu_percent = process.cpu_percent(interval=0.001)
            
            return CPUMetrics(
                user_time=cpu_times.user,
                system_time=cpu_times.system,
                cpu_percent=cpu_percent,
                frequency=psutil.cpu_freq().current if psutil.cpu_freq() else 0.0,
                cache_misses=self._perf_counters.get('cache_misses', 0),
                cache_hits=self._perf_counters.get('cache_hits', 0),
                context_switches=self._perf_counters.get('context_switches', 0),
                instructions_per_cycle=0.0  # Would need hardware counters
            )
    
    def update_performance_counters(self):
        """Update performance counters."""
        # This would typically use platform-specific APIs or perf tools
        pass

class ResourceManager:
    """Resource quota management and pooling for NLM."""
    
    def __init__(self):
        self._resource_limits: Dict[str, float] = {}
        self._current_usage: Dict[str, float] = {}
        self._pool_cache: Dict[str, deque] = {}
        self._lock = threading.RLock()
    
    def set_resource_limit(self, resource_type: str, limit: float):
        """Set a resource limit (memory in MB, CPU percent, etc.)."""
        with self._lock:
            self._resource_limits[resource_type] = limit
            self._current_usage[resource_type] = 0.0
    
    def acquire_resource(self, resource_type: str, amount: float) -> bool:
        """Acquire a resource if available within limits."""
        with self._lock:
            if resource_type not in self._resource_limits:
                return False
            
            current = self._current_usage.get(resource_type, 0.0)
            if current + amount <= self._resource_limits[resource_type]:
                self._current_usage[resource_type] = current + amount
                return True
            return False
    
    def release_resource(self, resource_type: str, amount: float):
        """Release a resource."""
        with self._lock:
            current = self._current_usage.get(resource_type, 0.0)
            self._current_usage[resource_type] = max(0.0, current - amount)
    
    def check_resource_usage(self, resource_type: str) -> float:
        """Check current resource usage."""
        with self._lock:
            return self._current_usage.get(resource_type, 0.0)
    
    def get_resource_limits(self) -> Dict[str, float]:
        """Get all resource limits."""
        with self._lock:
            return self._resource_limits.copy()

class BenchmarkSuite:
    """Comprehensive benchmarking for performance regression testing."""
    
    def __init__(self):
        self._benchmark_results: List[Dict[str, Any]] = []
        self._lock = threading.RLock()
    
    @contextmanager
    def benchmark(self, name: str, iterations: int = 1):
        """Benchmark a specific operation."""
        start_time = time.time()
        start_memory = psutil.Process(os.getpid()).memory_info().rss
        
        for _ in range(iterations):
            yield
        
        end_time = time.time()
        end_memory = psutil.Process(os.getpid()).memory_info().rss
        
        with self._lock:
            self._benchmark_results.append({
                'name': name,
                'iterations': iterations,
                'total_time': end_time - start_time,
                'avg_time': (end_time - start_time) / iterations,
                'total_memory': (end_memory - start_memory) / (1024 * 1024),
                'avg_memory': (end_memory - start_memory) / (iterations * 1024 * 1024),
                'timestamp': time.time()
            })
    
    def get_benchmark_results(self, name: Optional[str] = None) -> List[Dict[str, Any]]:
        """Get benchmark results."""
        with self._lock:
            if name:
                return [r for r in self._benchmark_results if r['name'] == name]
            return self._benchmark_results.copy()
    
    def compare_to_baseline(self, name: str, baseline_time: float, 
                           baseline_memory: float) -> Dict[str, float]:
        """Compare current results to a baseline."""
        current_results = self.get_benchmark_results(name)
        if not current_results:
            raise NLMError(f"No benchmark results found for '{name}'")
        
        latest = current_results[-1]
        
        time_regression = (latest['avg_time'] - baseline_time) / baseline_time * 100
        memory_regression = (latest['avg_memory'] - baseline_memory) / baseline_memory * 100
        
        return {
            'name': name,
            'time_regression_percent': time_regression,
            'memory_regression_percent': memory_regression,
            'performance_status': 'PASS' if abs(time_regression) < 5 else 'FAIL',
            'memory_status': 'PASS' if abs(memory_regression) < 5 else 'FAIL'
        }

# Global instances for convenience
_global_memory_profiler = MemoryProfiler()
_global_cpu_profiler = CPUProfiler()
_global_resource_manager = ResourceManager()
_global_benchmark_suite = BenchmarkSuite()

# Convenience functions for global profiling

def get_memory_usage() -> MemoryUsageInfo:
    """Get current memory usage from global profiler."""
    return _global_memory_profiler.get_memory_usage()

def trace_allocation(size: int, allocator_id: int = 0, allocated_from: str = ""):
    """Trace a memory allocation using global profiler."""
    _global_memory_profiler.trace_allocation(size, allocator_id, allocated_from)

def trace_deallocation(size: int):
    """Trace a memory deallocation using global profiler."""
    _global_memory_profiler.trace_deallocation(size)

def detect_memory_leaks() -> List[AllocationRecord]:
    """Detect memory leaks using global profiler."""
    return _global_memory_profiler.detect_memory_leaks()

def get_cpu_metrics() -> CPUMetrics:
    """Get CPU metrics from global profiler."""
    return _global_cpu_profiler.get_cpu_metrics()

def get_performance_metrics() -> PerformanceMetrics:
    """Get performance metrics (to be implemented by brain system)."""
    # This would be implemented by the brain system to provide actual metrics
    return PerformanceMetrics()

# Performance monitoring decorator

def profile_operation(name: str = ""):
    """Decorator to profile a function call."""
    def decorator(func):
        def wrapper(*args, **kwargs):
            start_time = time.time()
            try:
                result = func(*args, **kwargs)
                return result
            finally:
                end_time = time.time()
                if name:
                    with _global_benchmark_suite.benchmark(name, iterations=1):
                        pass
        return wrapper
    return decorator

# Resource quota management decorators

def require_memory(amount_mb: float):
    """Decorator to require memory for a function call."""
    def decorator(func):
        def wrapper(*args, **kwargs):
            if _global_resource_manager.acquire_resource('memory', amount_mb):
                try:
                    return func(*args, **kwargs)
                finally:
                    _global_resource_manager.release_resource('memory', amount_mb)
            else:
                raise NLMError(f"Insufficient memory: required {amount_mb}MB")
        return wrapper
    return decorator

print(f"Enhanced NLM Performance APIs loaded. {__name__}")