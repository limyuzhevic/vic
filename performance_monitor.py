#!/usr/bin/env python3

"""
Performance monitoring and profiling tools for NLM (Neural Learning Machine)

This module provides comprehensive profiling capabilities for NLM simulations,
including:
- Performance timing and metrics
- Memory usage tracking
- Bottleneck identification
- Profiler integration
- Memory profiling
- System resource monitoring

The performance monitoring system provides:
1. Real-time performance metrics collection
2. Detailed profiling of simulation phases
3. Resource usage tracking
4. Bottleneck detection and analysis
5. Performance comparison tools
6. Integration with external profiling tools
"""

import time
import sys
import os
import json
import psutil
import threading
import statistics
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from collections import defaultdict, deque
from enum import Enum
import resource

class PerformanceLevel(Enum):
    TRACE = "trace"
    DEBUG = "debug"
    INFO = "info"
    WARN = "warn"
    ERROR = "error"

@dataclass
class PerformanceMetric:
    """Base class for performance metrics"""
    name: str
    value: float
    unit: str
    timestamp: float
    context: Dict[str, Any] = field(default_factory=dict)

@dataclass
class TimingMetric(PerformanceMetric):
    """Timing-based performance metric"""
    def __post_init__(self):
        if self.unit == "":
            self.unit = "seconds"

@dataclass
class MemoryMetric(PerformanceMetric):
    """Memory-based performance metric"""
    def __post_init__(self):
        if self.unit == "":
            self.unit = "bytes"

@dataclass
class CountMetric(PerformanceMetric):
    """Count-based performance metric"""
    def __post_init__(self):
        if self.unit == "":
            self.unit = "count"

@dataclass
class SimulationPhase:
    """Represents a simulation phase with performance metrics"""
    name: str
    start_time: float
    end_time: float
    duration: float
    metrics: List[PerformanceMetric] = field(default_factory=list)
    steps_completed: int = 0
    neurons_active: int = 0
    spikes_generated: int = 0

class PerformanceProfiler:
    """
    Comprehensive performance profiler for NLM simulations.
    
    Tracks timing, memory usage, system resources, and provides detailed
    analysis capabilities for performance optimization.
    """
    
    def __init__(self, level: PerformanceLevel = PerformanceLevel.INFO):
        self.level = level
        self.phase_times: Dict[str, float] = {}
        self.metrics: List[PerformanceMetric] = []
        self.active_phases: Dict[str, SimulationPhase] = {}
        self.memory_samples: List[MemoryMetric] = []
        self.system_samples: List[Dict] = []
        self.bottleneck_analysis: Dict[str, float] = {}
        self.start_time = time.time()
        self.session_id = id(self)
        self.profiling_enabled = True
        self.max_memory_samples = 1000
        self.sampling_interval = 0.1
        
        if self.level.value == "trace":
            self.sampling_interval = 0.01
        
        self._sampling_thread = None
        self._sampling_running = False
        
    def enable_profiling(self, enabled: bool = True):
        """Enable or disable profiling"""
        self.profiling_enabled = enabled
        if not enabled:
            self.stop_sampling()
    
    def log_metric(self, name: str, value: float, unit: str = "",
                   context: Optional[Dict] = None, metric_type: str = "generic"):
        """Log a performance metric"""
        if not self.profiling_enabled:
            return
            
        timestamp = time.time() - self.start_time
        
        if metric_type == "timing":
            metric = TimingMetric(name, value, unit, timestamp, context or {})
        elif metric_type == "memory":
            metric = MemoryMetric(name, value, unit, timestamp, context or {})
        elif metric_type == "count":
            metric = CountMetric(name, value, unit, timestamp, context or {})
        else:
            metric = PerformanceMetric(name, value, unit, timestamp, context or {})
        
        self.metrics.append(metric)
        
        # Log based on level
        if self.level.value in ["trace", "debug"]:
            self._log_detailed(metric)
        
    def start_phase(self, name: str, context: Optional[Dict] = None):
        """Start a new simulation phase"""
        if not self.profiling_enabled:
            return
            
        phase = SimulationPhase(name, time.time(), 0.0, 0.0, context or {})
        self.active_phases[name] = phase
        
        self.log_metric(
            f"phase_{name}_started",
            1.0,
            "count",
            {"phase_name": name},
            "count"
        )
    
    def end_phase(self, name: str, additional_metrics: Optional[Dict] = None):
        """End a simulation phase"""
        if not self.profiling_enabled or name not in self.active_phases:
            return
            
        phase = self.active_phases[name]
        phase.end_time = time.time()
        phase.duration = phase.end_time - phase.start_time
        
        self.phase_times[name] = phase.duration
        
        # Log additional metrics
        if additional_metrics:
            for metric_name, value in additional_metrics.items():
                phase.metrics.append(
                    PerformanceMetric(
                        f"{name}_{metric_name}",
                        value,
                        "",
                        time.time() - self.start_time,
                        {"phase": name}
                    )
                )
        
        self.log_metric(
            f"phase_{name}_completed",
            phase.duration,
            "seconds",
            {"phase_name": name, "duration": phase.duration},
            "timing"
        )
        
        del self.active_phases[name]
    
    def _log_detailed(self, metric: PerformanceMetric):
        """Log metric in detail mode"""
        timestamp = metric.timestamp
        print(f"[{timestamp:.3f}s] {metric.name}: {metric.value} {metric.unit}")
        if metric.context:
            print(f"  Context: {metric.context}")
    
    def start_memory_sampling(self, interval: float = 0.1):
        """Start continuous memory sampling in background thread"""
        if self._sampling_running:
            return
            
        self.sampling_interval = interval
        self._sampling_running = True
        self._sampling_thread = threading.Thread(target=self._memory_sampler, daemon=True)
        self._sampling_thread.start()
    
    def stop_memory_sampling(self):
        """Stop memory sampling"""
        self._sampling_running = False
        if self._sampling_thread:
            self._sampling_thread.join(timeout=1.0)
    
    def _memory_sampler(self):
        """Background thread for memory sampling"""
        while self._sampling_running:
            try:
                # Get memory usage
                process = psutil.Process()
                memory_info = process.memory_info()
                memory_mb = memory_info.rss / (1024 * 1024)
                
                metric = MemoryMetric(
                    "memory_usage",
                    memory_mb,
                    "MB",
                    time.time() - self.start_time,
                    {"process_id": os.getpid()}
                )
                
                self.memory_samples.append(metric)
                
                # Keep only recent samples
                if len(self.memory_samples) > self.max_memory_samples:
                    self.memory_samples = self.memory_samples[-self.max_memory_samples:]
                
                time.sleep(self.sampling_interval)
                
            except Exception as e:
                if self.level.value in ["trace", "debug"]:
                    print(f"Memory sampling error: {e}")
                break
    
    def collect_system_metrics(self):
        """Collect current system metrics"""
        try:
            process = psutil.Process()
            system_info = {
                "timestamp": time.time() - self.start_time,
                "cpu_percent": process.cpu_percent(),
                "memory_percent": process.memory_percent(),
                "memory_rss_mb": process.memory_info().rss / (1024 * 1024),
                "memory_vms_mb": process.memory_info().vms / (1024 * 1024),
                "num_threads": process.num_threads(),
                "num_fds": len(process.open_files()),
            }
            
            self.system_samples.append(system_info)
            return system_info
            
        except Exception as e:
            if self.level.value in ["trace", "debug"]:
                print(f"System metrics collection error: {e}")
            return {}
    
    def analyze_bottlenecks(self, metrics_window: int = 100):
        """
        Analyze performance bottlenecks based on recent metrics.
        
        Identifies slow operations, memory issues, and CPU bottlenecks.
        """
        self.bottleneck_analysis.clear()
        
        # Analyze timing metrics
        timing_metrics = [m for m in self.metrics if isinstance(m, TimingMetric)]
        
        if len(timing_metrics) > 0:
            # Find longest operations
            phase_durations = {m.context.get("phase", "unknown"): m.value 
                             for m in timing_metrics if "phase" in m.context}
            
            if phase_durations:
                max_phase = max(phase_durations.items(), key=lambda x: x[1])
                self.bottleneck_analysis["longest_phase"] = max_phase[0]
                self.bottleneck_analysis["longest_phase_duration"] = max_phase[1]
        
        # Analyze memory metrics
        if self.memory_samples:
            recent_memory = [m.value for m in self.memory_samples[-metrics_window:]]
            if recent_memory:
                self.bottleneck_analysis["memory_trend"] = (
                    recent_memory[-1] - recent_memory[0] / max(len(recent_memory) - 1, 1)
                )
                self.bottleneck_analysis["peak_memory"] = max(recent_memory)
                self.bottleneck_analysis["memory_samples"] = len(recent_memory)
        
        # Analyze system metrics
        if self.system_samples:
            recent_system = self.system_samples[-metrics_window:]
            if recent_system:
                avg_cpu = sum(s.get("cpu_percent", 0) for s in recent_system) / len(recent_system)
                self.bottleneck_analysis["avg_cpu_usage"] = avg_cpu
                
                if avg_cpu > 80.0:
                    self.bottleneck_analysis["cpu_bottleneck"] = True
                    self.bottleneck_analysis["cpu_issue"] = "High CPU usage detected"
                
                memory_samples = [s.get("memory_percent", 0) for s in recent_system]
                if memory_samples and max(memory_samples) > 90.0:
                    self.bottleneck_analysis["memory_bottleneck"] = True
                    self.bottleneck_analysis["memory_issue"] = "High memory usage detected"
        
        return self.bottleneck_analysis
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary"""
        timing_metrics = [m for m in self.metrics if isinstance(m, TimingMetric)]
        memory_metrics = [m for m in self.metrics if isinstance(m, MemoryMetric)]
        count_metrics = [m for m in self.metrics if isinstance(m, CountMetric)]
        
        summary = {
            "session_id": self.session_id,
            "total_metrics": len(self.metrics),
            "total_time": time.time() - self.start_time,
            "active_phases": len(self.active_phases),
            "phase_times": self.phase_times,
            "bottleneck_analysis": self.bottleneck_analysis,
        }
        
        # Timing summary
        if timing_metrics:
            times = [m.value for m in timing_metrics]
            summary["timing_summary"] = {
                "total_time": sum(times),
                "avg_time": statistics.mean(times) if times else 0,
                "min_time": min(times) if times else 0,
                "max_time": max(times) if times else 0,
                "std_time": statistics.stdev(times) if len(times) > 1 else 0,
            }
        
        # Memory summary
        if memory_metrics:
            memories = [m.value for m in memory_metrics]
            summary["memory_summary"] = {
                "total_memory_samples": len(memories),
                "current_memory": memories[-1] if memories else 0,
                "avg_memory": statistics.mean(memories) if memories else 0,
                "peak_memory": max(memories) if memories else 0,
                "memory_growth": (memories[-1] - memories[0]) / max(len(memories) - 1, 1) if len(memories) > 1 else 0,
            }
        
        # Count summary
        if count_metrics:
            counts = [m.value for m in count_metrics]
            summary["count_summary"] = {
                "total_events": sum(counts),
                "avg_events": statistics.mean(counts) if counts else 0,
                "max_events": max(counts) if counts else 0,
            }
        
        # System metrics summary
        if self.system_samples:
            recent_system = self.system_samples[-100:]  # Last 100 samples
            if recent_system:
                summary["system_summary"] = {
                    "avg_cpu_usage": sum(s.get("cpu_percent", 0) for s in recent_system) / len(recent_system),
                    "avg_memory_usage": sum(s.get("memory_percent", 0) for s in recent_system) / len(recent_system),
                    "max_memory_usage": max(s.get("memory_percent", 0) for s in recent_system),
                    "sample_count": len(recent_system),
                }
        
        return summary
    
    def export_metrics(self, filepath: str):
        """Export performance metrics to a JSON file"""
        data = {
            "summary": self.get_performance_summary(),
            "metrics": [
                {
                    "name": m.name,
                    "value": m.value,
                    "unit": m.unit,
                    "timestamp": m.timestamp,
                    "context": m.context,
                    "type": type(m).__name__
                }
                for m in self.metrics
            ],
            "phase_times": self.phase_times,
            "bottleneck_analysis": self.bottleneck_analysis,
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        return filepath
    
    def import_metrics(self, filepath: str):
        """Import performance metrics from a JSON file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Clear existing metrics
        self.metrics.clear()
        self.phase_times.clear()
        self.bottleneck_analysis.clear()
        
        # Import metrics
        for metric_data in data.get("metrics", []):
            context = metric_data.get("context", {})
            timestamp = metric_data.get("timestamp", 0)
            
            if metric_data["type"] == "TimingMetric":
                metric = TimingMetric(
                    metric_data["name"],
                    metric_data["value"],
                    metric_data["unit"],
                    timestamp,
                    context
                )
            elif metric_data["type"] == "MemoryMetric":
                metric = MemoryMetric(
                    metric_data["name"],
                    metric_data["value"],
                    metric_data["unit"],
                    timestamp,
                    context
                )
            elif metric_data["type"] == "CountMetric":
                metric = CountMetric(
                    metric_data["name"],
                    metric_data["value"],
                    metric_data["unit"],
                    timestamp,
                    context
                )
            else:
                metric = PerformanceMetric(
                    metric_data["name"],
                    metric_data["value"],
                    metric_data["unit"],
                    timestamp,
                    context
                )
            
            self.metrics.append(metric)
        
        self.phase_times = data.get("phase_times", {})
        self.bottleneck_analysis = data.get("bottleneck_analysis", {})
        
        return filepath
    
    def generate_report(self) -> str:
        """Generate a human-readable performance report"""
        summary = self.get_performance_summary()
        
        report = ["=" * 60]
        report.append("NLM PERFORMANCE REPORT")
        report.append("=" * 60)
        report.append(f"Session ID: {summary['session_id']}")
        report.append(f"Total Duration: {summary['total_time']:.3f} seconds")
        report.append(f"Total Metrics Collected: {summary['total_metrics']}")
        report.append(f"Active Phases at End: {summary['active_phases']}")
        report.append("")
        
        # Phase times
        if summary["phase_times"]:
            report.append("PHASE EXECUTION TIMES:")
            for phase, duration in summary["phase_times"].items():
                report.append(f"  {phase}: {duration:.3f} seconds")
            report.append("")
        
        # Timing summary
        if "timing_summary" in summary:
            timing = summary["timing_summary"]
            report.append("TIMING STATISTICS:")
            report.append(f"  Total Time: {timing['total_time']:.3f} seconds")
            report.append(f"  Average Time: {timing['avg_time']:.3f} seconds")
            report.append(f"  Min Time: {timing['min_time']:.3f} seconds")
            report.append(f"  Max Time: {timing['max_time']:.3f} seconds")
            report.append(f"  Standard Deviation: {timing['std_time']:.3f} seconds")
            report.append("")
        
        # Memory summary
        if "memory_summary" in summary:
            memory = summary["memory_summary"]
            report.append("MEMORY STATISTICS:")
            report.append(f"  Total Samples: {memory['total_memory_samples']}")
            report.append(f"  Current Usage: {memory['current_memory']:.1f} MB")
            report.append(f"  Average Usage: {memory['avg_memory']:.1f} MB")
            report.append(f"  Peak Usage: {memory['peak_memory']:.1f} MB")
            report.append(f"  Memory Growth: {memory['memory_growth']:.1f} MB/s")
            report.append("")
        
        # Bottleneck analysis
        if summary["bottleneck_analysis"]:
            bottlenecks = summary["bottleneck_analysis"]
            report.append("BOTTLENECK ANALYSIS:")
            for bottleneck, value in bottlenecks.items():
                if isinstance(value, bool):
                    status = "YES" if value else "NO"
                    report.append(f"  {bottleneck}: {status}")
                else:
                    report.append(f"  {bottleneck}: {value}")
            report.append("")
        
        report.append("=" * 60)
        return "\n".join(report)


class PerformanceMonitor:
    """
    High-level performance monitor for NLM simulation.
    
    Provides a simplified interface for monitoring and profiling
    NLM simulations with pre-configured monitoring for common use cases.
    """
    
    def __init__(self, level: PerformanceLevel = PerformanceLevel.INFO):
        self.level = level
        self.profiler = PerformanceProfiler(level)
        self.current_phase = None
        self.monitoring_active = False
    
    def start_monitoring(self):
        """Start performance monitoring"""
        self.monitoring_active = True
        self.profiler.enable_profiling(True)
        self.profiler.start_memory_sampling()
        self.profiler.collect_system_metrics()
        
        if self.level.value in ["trace", "debug"]:
            print("Performance monitoring started")
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.monitoring_active = False
        self.profiler.stop_memory_sampling()
        self.profiler.collect_system_metrics()
        
        if self.level.value in ["trace", "debug"]:
            print("Performance monitoring stopped")
    
    def start_phase(self, name: str, context: Optional[Dict] = None):
        """Start monitoring a simulation phase"""
        if not self.monitoring_active:
            return
        
        self.current_phase = name
        self.profiler.start_phase(name, context)
    
    def end_phase(self, name: Optional[str] = None, additional_metrics: Optional[Dict] = None):
        """End monitoring a simulation phase"""
        phase_name = name or self.current_phase
        if not phase_name or not self.monitoring_active:
            return
        
        self.profiler.end_phase(phase_name, additional_metrics)
        self.current_phase = None
    
    def log_metric(self, name: str, value: float, unit: str = "",
                   context: Optional[Dict] = None, metric_type: str = "generic"):
        """Log a performance metric"""
        if not self.monitoring_active:
            return
        
        self.profiler.log_metric(name, value, unit, context, metric_type)
    
    def track_step(self, step: int, neurons_active: int = 0, spikes_generated: int = 0):
        """Track performance for a simulation step"""
        if not self.monitoring_active:
            return
        
        context = {
            "step": step,
            "neurons_active": neurons_active,
            "spikes_generated": spikes_generated,
        }
        
        self.log_metric(
            f"step_{step}_neurons",
            neurons_active,
            "neurons",
            context,
            "count"
        )
        
        self.log_metric(
            f"step_{step}_spikes",
            spikes_generated,
            "spikes",
            context,
            "count"
        )
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary"""
        return self.profiler.get_performance_summary()
    
    def generate_report(self) -> str:
        """Generate performance report"""
        return self.profiler.generate_report()
    
    def export_metrics(self, filepath: str):
        """Export metrics to file"""
        return self.profiler.export_metrics(filepath)
    
    def analyze_bottlenecks(self, metrics_window: int = 100):
        """Analyze performance bottlenecks"""
        return self.profiler.analyze_bottlenecks(metrics_window)