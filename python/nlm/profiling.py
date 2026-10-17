#!/usr/bin/env python3
"""
Performance Profiling Tools for Neural Simulation

This module provides comprehensive performance profiling tools for neural
simulation, including timing, memory profiling, memory leak detection,
performance benchmarking, and code profiling utilities.
"""

import time
import threading
import tracemalloc
import psutil
import os
import sys
from typing import List, Dict, Tuple, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
import warnings
import gc
import functools
import traceback

warnings.filterwarnings('ignore')

class ProfilingTarget(Enum):
    """Profiling target types"""
    FUNCTION = "function"
    THREAD = "thread"
    PROCESS = "process"
    MEMORY = "memory"
    CPU = "cpu"
    I/O = "io"

@dataclass
class ProfilingResult:
    """Container for profiling results"""
    target: ProfilingTarget
    name: str
    total_time: float
    call_count: int
    average_time: float
    min_time: float
    max_time: float
    median_time: float
    std_deviation: float
    memory_usage: Optional[float] = None
    memory_peak: Optional[float] = None
    cpu_time: Optional[float] = None
    samples: Optional[List[Dict[str, Any]]] = None
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class MemoryUsage:
    """Memory usage information"""
    process_id: int
    memory_rss: float  # Resident Set Size
    memory_vms: float  # Virtual Memory Size
    memory_shared: float
    memory_text: float
    memory_data: float
    memory_dirty: float
    memory_lib: float
    memory_swap: float
    cpu_percent: float

class Profiler:
    """Base profiler for timing and resource usage"""
    
    def __init__(self):
        self.profiles: Dict[str, ProfilingResult] = {}
        self.start_times: Dict[str, float] = {}
        self.call_counts: Dict[str, int] = {}
        self.total_times: Dict[str, float] = {}
        self.min_times: Dict[str, float] = {}
        self.max_times: Dict[str, float] = {}
        self.time_samples: Dict[str, List[float]] = {}
    
    def start(self, name: str):
        """Start profiling a target"""
        self.start_times[name] = time.time()
    
    def stop(self, name: str) -> float:
        """Stop profiling a target and return elapsed time"""
        if name not in self.start_times:
            return 0.0
        
        elapsed = time.time() - self.start_times[name]
        del self.start_times[name]
        
        return elapsed
    
    def record(self, name: str, elapsed: Optional[float] = None):
        """Record a profiling result"""
        if elapsed is None:
            elapsed = self.stop(name)
        
        if name not in self.profiles:
            self.profiles[name] = ProfilingResult(
                target=ProfilingTarget.FUNCTION,
                name=name,
                total_time=0.0,
                call_count=0,
                average_time=0.0,
                min_time=float('inf'),
                max_time=0.0,
                median_time=0.0,
                std_deviation=0.0
            )
        
        profile = self.profiles[name]
        profile.call_count += 1
        profile.total_time += elapsed
        profile.min_time = min(profile.min_time, elapsed)
        profile.max_time = max(profile.max_time, elapsed)
        
        if name not in self.time_samples:
            self.time_samples[name] = []
        self.time_samples[name].append(elapsed)
    
    def get_result(self, name: str) -> Optional[ProfilingResult]:
        """Get profiling result for a specific target"""
        if name not in self.profiles:
            return None
        
        profile = self.profiles[name]
        
        # Update statistics
        samples = self.time_samples.get(name, [])
        if samples:
            profile.average_time = profile.total_time / profile.call_count
            profile.median_time = np.median(samples) if len(samples) > 0 else 0
            profile.std_deviation = np.std(samples) if len(samples) > 1 else 0
        
        return profile
    
    def get_all_results(self) -> Dict[str, ProfilingResult]:
        """Get all profiling results"""
        return self.profiles.copy()
    
    def reset(self):
        """Reset all profiling data"""
        self.profiles.clear()
        self.start_times.clear()
        self.call_counts.clear()
        self.total_times.clear()
        self.min_times.clear()
        self.max_times.clear()
        self.time_samples.clear()
class FunctionProfiler:
    """Profiler for individual functions with decorators"""
    
    def __init__(self, profiler: Optional[Profiler] = None):
        self.profiler = profiler or Profiler()
    
    def profile(self, func: Callable) -> Callable:
        """Decorator to profile a function"""
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            name = f"{func.__module__}.{func.__name__}"
            self.profiler.start(name)
            try:
                result = func(*args, **kwargs)
                return result
            finally:
                self.profiler.record(name)
        
        return wrapper
    
    def profile_memory(self, func: Callable) -> Callable:
        """Decorator to profile function memory usage"""
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            name = f"{func.__module__}.{func.__name__}"
            
            # Start memory tracking
            tracemalloc.start()
            gc.collect()
            
            # Record start memory
            start_memory = self._get_memory_usage()
            
            # Run function
            start_time = time.time()
            result = func(*args, **kwargs)
            elapsed = time.time() - start_time
            
            # Record end memory
            gc.collect()
            end_memory = self._get_memory_usage()
            current, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
            # Record profiling data
            memory_delta = end_memory - start_memory
            self.profiler.record(name, elapsed)
            
            return result
        
        return wrapper
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB"""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / 1024 / 1024
class MemoryProfiler:
    """Profiler for memory usage and leak detection"""
    
    def __init__(self, sampling_interval: float = 0.1):
        self.sampling_interval = sampling_interval
        self.memory_samples: List[Tuple[float, MemoryUsage]] = []
        self.baseline_memory: Optional[float] = None
        self.profiling = False
        self.monitor_thread: Optional[threading.Thread] = None
    
    def start(self):
        """Start memory profiling"""
        self.profiling = True
        self.memory_samples = []
        self.monitor_thread = threading.Thread(target=self._monitor_memory)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop(self) -> List[Tuple[float, MemoryUsage]]:
        """Stop memory profiling and return samples"""
        self.profiling = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        
        return self.memory_samples
    
    def _monitor_memory(self):
        """Monitor memory usage"""
        while self.profiling:
            try:
                process = psutil.Process(os.getpid())
                mem_info = process.memory_info()
                
                usage = MemoryUsage(
                    process_id=os.getpid(),
                    memory_rss=mem_info.rss / 1024 / 1024,
                    memory_vms=mem_info.vms / 1024 / 1024,
                    memory_shared=mem_info.shared / 1024 / 1024,
                    memory_text=mem_info.text / 1024 / 1024,
                    memory_data=mem_info.data / 1024 / 1024,
                    memory_dirty=mem_info.dirty / 1024 / 1024,
                    memory_lib=mem_info.lib / 1024 / 1024,
                    memory_swap=mem_info.swap / 1024 / 1024,
                    cpu_percent=process.cpu_percent()
                )
                
                self.memory_samples.append((time.time(), usage))
                time.sleep(self.sampling_interval)
            
            except Exception:
                break
    
    def detect_leaks(self, samples: List[Tuple[float, MemoryUsage]], 
                    threshold: float = 1.0) -> Dict[str, Any]:
        """Detect memory leaks from memory samples"""
        if len(samples) < 10:
            return {"error": "Insufficient samples for leak detection"}
        
        # Extract memory usage over time
        times = [sample[0] for sample in samples]
        rss_values = [sample[1].memory_rss for sample in samples]
        
        # Calculate baseline (first 10% of samples)
        baseline_idx = int(len(samples) * 0.1)
        baseline_memory = np.mean(rss_values[:baseline_idx])
        
        # Check for continuous increase
        increasing_segments = 0
        for i in range(1, len(rss_values)):
            if rss_values[i] > rss_values[i-1] + threshold:
                increasing_segments += 1
        
        # Calculate trend using linear regression
        if len(times) > 2:
            coeffs = np.polyfit(times, rss_values, 1)
            trend = coeffs[0]  # Slope
        else:
            trend = 0
        
        return {
            "baseline_memory": baseline_memory,
            "final_memory": rss_values[-1],
            "memory_increase": rss_values[-1] - baseline_memory,
            "trend_slope": trend,
            "increasing_segments": increasing_segments,
            "total_samples": len(samples),
            "has_leak": trend > 0.01 or increasing_segments > len(samples) * 0.3
        }
    
    def get_peak_memory(self, samples: List[Tuple[float, MemoryUsage]]) -> float:
        """Get peak memory usage from samples"""
        if not samples:
            return 0.0
        
        return max(sample[1].memory_rss for sample in samples)
    
    def get_memory_trend(self, samples: List[Tuple[float, MemoryUsage]], 
                        window_size: int = 10) -> List[float]:
        """Get memory trend using rolling average"""
        if len(samples) < window_size:
            window_size = len(samples)
        
        rss_values = [sample[1].memory_rss for sample in samples]
        
        # Calculate rolling average
        trend = []
        for i in range(window_size - 1, len(rss_values)):
            window = rss_values[i - window_size + 1:i + 1]
            trend.append(np.mean(window))
        
        return trend
class SimulationProfiler:
    """Profiler for neural simulation performance"""
    
    def __init__(self):
        self.step_times: List[float] = []
        self.memory_usage: List[float] = []
        self.start_time: Optional[float] = None
        self.step_count: int = 0
        self.total_steps: int = 0
    
    def start(self):
        """Start simulation profiling"""
        self.start_time = time.time()
        self.step_times = []
        self.memory_usage = []
        self.step_count = 0
    
    def record_step(self, step_time: Optional[float] = None):
        """Record simulation step performance"""
        if step_time is None:
            step_time = time.time() - (self.start_time or time.time())
        
        self.step_times.append(step_time)
        self.memory_usage.append(self._get_current_memory())
        self.step_count += 1
    
    def stop(self) -> Dict[str, Any]:
        """Stop simulation profiling and return results"""
        if self.start_time is None:
            return {"error": "Profiler not started"}
        
        total_time = time.time() - self.start_time
        
        if self.step_times:
            step_times_array = np.array(self.step_times)
            memory_array = np.array(self.memory_usage)
            
            return {
                "total_time": total_time,
                "steps_completed": self.step_count,
                "average_step_time": np.mean(step_times_array),
                "min_step_time": np.min(step_times_array),
                "max_step_time": np.max(step_times_array),
                "std_step_time": np.std(step_times_array),
                "average_memory": np.mean(memory_array),
                "peak_memory": np.max(memory_array),
                "memory_trend": self._calculate_memory_trend(memory_array),
                "steps_per_second": self.step_count / total_time if total_time > 0 else 0,
                "time_per_step_ms": (total_time / self.step_count * 1000) if self.step_count > 0 else 0
            }
        
        return {
            "total_time": total_time,
            "steps_completed": self.step_count,
            "error": "No step data recorded"
        }
    
    def _get_current_memory(self) -> float:
        """Get current memory usage in MB"""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / 1024 / 1024
    
    def _calculate_memory_trend(self, memory_array: np.ndarray) -> List[float]:
        """Calculate memory trend using rolling average"""
        window_size = min(10, len(memory_array))
        if window_size < 2:
            return [memory_array[0]] if len(memory_array) > 0 else []
        
        trend = []
        for i in range(window_size - 1, len(memory_array)):
            window = memory_array[i - window_size + 1:i + 1]
            trend.append(np.mean(window))
        
        return trend
class Benchmarker:
    """Benchmark neural simulation performance"""
    
    def __init__(self, profiler: Optional[Profiler] = None):
        self.profiler = profiler or Profiler()
        self.benchmark_results: List[Dict[str, Any]] = []
    
    def run_benchmark(self, brain: Any, world: Any, 
                     duration_seconds: float,
                     target_fps: int = 60) -> Dict[str, Any]:
        """Run a performance benchmark"""
        import time
        
        # Simulate brain steps
        steps_completed = 0
        start_time = time.time()
        
        try:
            for step in range(int(duration_seconds * target_fps)):
                current_time = step / target_fps
                
                # Record step time
                step_start = time.time()
                
                # Simulate brain step (in real implementation, call brain.step())
                if hasattr(brain, 'step'):
                    brain.step(step, current_time)
                if hasattr(world, 'update'):
                    world.update(1.0 / target_fps)
                
                step_time = time.time() - step_start
                self.profiler.record(f"step_{step}", step_time)
                
                steps_completed = step + 1
        
        except Exception as e:
            pass
        
        total_time = time.time() - start_time
        
        # Get profiling results
        all_results = self.profiler.get_all_results()
        
        # Create benchmark result
        benchmark_result = {
            "duration_seconds": duration_seconds,
            "target_fps": target_fps,
            "actual_fps": steps_completed / duration_seconds if duration_seconds > 0 else 0,
            "steps_completed": steps_completed,
            "total_time": total_time,
            "average_step_time": total_time / steps_completed if steps_completed > 0 else 0,
            "memory_usage": self._get_memory_usage(),
            "cpu_usage": psutil.cpu_percent(),
            "step_times": self.profiler.get_result("step_0").samples if self.profiler.get_result("step_0") else None,
            "profiling_results": {}
        }
        
        # Extract key metrics
        step_profiles = {k: v for k, v in all_results.items() if k.startswith('step_')}
        if step_profiles:
            avg_step_time = np.mean([p.total_time / p.call_count for p in step_profiles.values()])
            benchmark_result["average_step_time_ms"] = avg_step_time * 1000
            benchmark_result["throughput_steps_per_second"] = 1.0 / avg_step_time if avg_step_time > 0 else 0
        
        self.benchmark_results.append(benchmark_result)
        
        return benchmark_result
    
    def _get_memory_usage(self) -> Dict[str, float]:
        """Get current memory usage"""
        process = psutil.Process(os.getpid())
        mem_info = process.memory_info()
        
        return {
            "rss_mb": mem_info.rss / 1024 / 1024,
            "vms_mb": mem_info.vms / 1024 / 1024,
            "shared_mb": mem_info.shared / 1024 / 1024,
            "text_mb": mem_info.text / 1024 / 1024,
            "data_mb": mem_info.data / 1024 / 1024,
            "dirty_mb": mem_info.dirty / 1024 / 1024,
        }
    
    def compare_benchmarks(self, benchmark1: Dict[str, Any], 
                           benchmark2: Dict[str, Any]) -> Dict[str, Any]:
        """Compare two benchmarks"""
        comparison = {}
        
        # Compare performance
        for key in benchmark1:
            if key in benchmark2 and isinstance(benchmark1[key], (int, float)):
                val1 = benchmark1[key]
                val2 = benchmark2[key]
                comparison[f"{key}_improvement"] = (val2 - val1) / val1 * 100 if val1 != 0 else 0
        
        return comparison

# Convenience functions

def create_profiler() -> Profiler:
    """Convenience function to create a profiler"""
    return Profiler()

def create_function_profiler(profiler: Optional[Profiler] = None) -> FunctionProfiler:
    """Convenience function to create a function profiler"""
    return FunctionProfiler(profiler)

def create_memory_profiler(sampling_interval: float = 0.1) -> MemoryProfiler:
    """Convenience function to create a memory profiler"""
    return MemoryProfiler(sampling_interval)

def create_simulation_profiler() -> SimulationProfiler:
    """Convenience function to create a simulation profiler"""
    return SimulationProfiler()

def create_benchmarker(profiler: Optional[Profiler] = None) -> Benchmarker:
    """Convenience function to create a benchmarker"""
    return Benchmarker(profiler)
