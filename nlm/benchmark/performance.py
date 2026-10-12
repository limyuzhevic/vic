#!/usr/bin/env python3
"""
NLM Performance Module - Real-time Performance Monitoring

This module provides real-time performance monitoring with millisecond precision,
including step timing, memory usage, and neural activity metrics.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Tuple
import time
import statistics
from collections import deque

class PerformanceMonitor:
    """Real-time performance monitoring for neural simulation.
    
    Tracks and analyzes simulation performance with millisecond precision,
    including step timing, memory usage, and neural activity metrics.
    
    Features:
    * Real-time step timing
    * Memory usage tracking
    * Neural activity monitoring
    * Performance stability analysis
    * Bottleneck detection
    * Performance profiling
    """
    
    def __init__(self, sampling_interval_ms: float = 100.0, window_size: int = 1000):
        self.sampling_interval_ms = sampling_interval_ms
        self.window_size = window_size
        
        # Performance data storage
        self.step_times = deque(maxlen=window_size)
        self.spike_counts = deque(maxlen=window_size)
        self.active_neuron_counts = deque(maxlen=window_size)
        self.memory_usage = deque(maxlen=window_size)
        self.step_timestamps = deque(maxlen=window_size)
        
        # Monitoring state
        self.is_monitoring = False
        self.start_time = None
        self.total_steps_recorded = 0
        self.performance_threshold = 0.1  # For stability detection
    
    def start(self):
        """Start performance monitoring."""
        self.is_monitoring = True
        self.start_time = time.time()
        self.total_steps_recorded = 0
        self.step_times.clear()
        self.spike_counts.clear()
        self.active_neuron_counts.clear()
        self.memory_usage.clear()
        self.step_timestamps.clear()
        
        print("Performance monitoring started")
    
    def stop(self):
        """Stop performance monitoring."""
        self.is_monitoring = False
        duration = time.time() - self.start_time if self.start_time else 0
        
        print(f"Performance monitoring stopped. Duration: {duration:.2f}s")
        print(f"Total steps recorded: {self.total_steps_recorded}")
    
    def record_step(self, step_time_ms: float, spikes: int, active_neurons: int):
        """Record a simulation step's performance data.
        
        Args:
            step_time_ms: Time taken for this step in milliseconds
            spikes: Number of spikes recorded in this step
            active_neurons: Number of active neurons in this step
        """
        if not self.is_monitoring:
            return
        
        current_time = time.time()
        
        self.step_times.append(step_time_ms)
        self.spike_counts.append(spikes)
        self.active_neuron_counts.append(active_neurons)
        self.step_timestamps.append(current_time)
        
        # Estimate memory usage based on neural activity
        estimated_memory = self._estimate_memory_usage(spikes, active_neurons)
        self.memory_usage.append(estimated_memory)
        
        self.total_steps_recorded += 1
    
    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current performance metrics (averaged over recent window)."""
        if not self.step_times:
            return self._get_empty_metrics()
        
        metrics = {
            "total_steps": self.total_steps_recorded,
            "current_step_time_ms": self.step_times[-1] if self.step_times else 0,
            "avg_step_time_ms": statistics.mean(self.step_times) if self.step_times else 0,
            "std_step_time_ms": statistics.stdev(self.step_times) if len(self.step_times) > 1 else 0,
            "min_step_time_ms": min(self.step_times),
            "max_step_time_ms": max(self.step_times),
            
            "current_spikes": self.spike_counts[-1] if self.spike_counts else 0,
            "avg_spikes_per_step": statistics.mean(self.spike_counts) if self.spike_counts else 0,
            "max_spikes_per_step": max(self.spike_counts) if self.spike_counts else 0,
            
            "current_active_neurons": self.active_neuron_counts[-1] if self.active_neuron_counts else 0,
            "avg_active_neurons": statistics.mean(self.active_neuron_counts) if self.active_neuron_counts else 0,
            "max_active_neurons": max(self.active_neuron_counts) if self.active_neuron_counts else 0,
            
            "current_memory_mb": self.memory_usage[-1] if self.memory_usage else 0,
            "avg_memory_mb": statistics.mean(self.memory_usage) if self.memory_usage else 0,
            "max_memory_mb": max(self.memory_usage) if self.memory_usage else 0,
            
            "duration_seconds": time.time() - self.start_time if self.start_time else 0,
            "steps_per_second": self.total_steps_recorded / (time.time() - self.start_time) 
                                if self.start_time and (time.time() - self.start_time) > 0 else 0,
            
            "monitoring_active": self.is_monitoring,
            "data_points": len(self.step_times)
        }
        
        return metrics
    
    def get_snapshot(self) -> Dict[str, Any]:
        """Get performance metrics snapshot."""
        snapshot = self.get_current_metrics()
        snapshot["snapshot_timestamp"] = time.time()
        return snapshot
    
    def is_stable(self, window_size: int = 100, threshold: float = 0.1) -> bool:
        """Check if performance is stable.
        
        Args:
            window_size: Number of recent steps to analyze
            threshold: Coefficient of variation threshold (std/mean)
            
        Returns:
            True if performance is stable, False otherwise
        """
        if len(self.step_times) < window_size:
            return False
        
        # Get recent data
        recent_times = list(self.step_times)[-window_size:]
        
        if len(recent_times) < 2:
            return True
        
        mean_time = statistics.mean(recent_times)
        std_time = statistics.stdev(recent_times) if len(recent_times) > 1 else 0
        
        # Calculate coefficient of variation
        cv = (std_time / mean_time) if mean_time > 0 else 0
        
        return cv <= threshold
    
    def get_step_times(self) -> List[float]:
        """Get all recorded step times."""
        return list(self.step_times)
    
    def get_spike_counts(self) -> List[int]:
        """Get all recorded spike counts."""
        return list(self.spike_counts)
    
    def get_active_neuron_counts(self) -> List[int]:
        """Get all recorded active neuron counts."""
        return list(self.active_neuron_counts)
    
    def reset(self):
        """Reset all monitoring data."""
        print("Resetting performance monitoring data")
        self.step_times.clear()
        self.spike_counts.clear()
        self.active_neuron_counts.clear()
        self.memory_usage.clear()
        self.step_timestamps.clear()
        self.total_steps_recorded = 0
    
    def get_duration(self) -> float:
        """Get monitoring duration in seconds."""
        if not self.start_time:
            return 0.0
        return time.time() - self.start_time
    
    def get_total_steps(self) -> int:
        """Get total number of steps recorded."""
        return self.total_steps_recorded
    
    def get_performance_score(self) -> float:
        """Get overall performance score (0-100)."""
        if not self.step_times:
            return 0.0
        
        # Calculate score based on step time consistency and speed
        mean_time = statistics.mean(self.step_times)
        std_time = statistics.stdev(self.step_times) if len(self.step_times) > 1 else 0
        
        # Higher score for faster and more consistent steps
        time_score = min(100.0, (1000.0 / mean_time) * 20)  # Normalize to 0-100
        consistency_score = max(0.0, 100.0 - (std_time / mean_time) * 100) if mean_time > 0 else 50.0
        
        # Weighted average
        final_score = (time_score * 0.7) + (consistency_score * 0.3)
        
        return final_score
    
    def get_bottleneck_analysis(self) -> Dict[str, Any]:
        """Analyze potential bottlenecks in performance."""
        if len(self.step_times) < 10:
            return {"error": "Insufficient data for bottleneck analysis"}
        
        analysis = {
            "slow_steps": [],
            "consistent_performance": True,
            "recommendations": []
        }
        
        # Identify slow steps (top 10%)
        threshold = statistics.quantiles(self.step_times, n=10)[-1]  # 90th percentile
        analysis["slow_steps"] = [t for t in self.step_times if t > threshold]
        
        # Check consistency
        mean_time = statistics.mean(self.step_times)
        std_time = statistics.stdev(self.step_times) if len(self.step_times) > 1 else 0
        cv = (std_time / mean_time) if mean_time > 0 else 0
        
        analysis["consistent_performance"] = cv < 0.2  # Coefficient of variation < 20%
        
        # Generate recommendations
        if analysis["slow_steps"]:
            analysis["recommendations"].append(
                "Consider optimizing neural dynamics for slow steps"
            )
        
        if cv > 0.2:
            analysis["recommendations"].append(
                "Performance varies significantly - check for unstable dynamics"
            )
        
        if len(self.memory_usage) > 5:
            recent_memory_trend = self._calculate_trend(self.memory_usage[-5:])
            if recent_memory_trend > 0:  # Increasing trend
                analysis["recommendations"].append(
                    "Memory usage increasing - check for memory leaks"
                )
        
        return analysis
    
    def _estimate_memory_usage(self, spikes: int, active_neurons: int) -> float:
        """Estimate memory usage based on neural activity."""
        # Base memory for neural state
        base_memory = 50.0  # MB
        
        # Additional memory for spikes
        spike_memory = (spikes * 0.01)  # 0.01 MB per spike
        
        # Additional memory for active neurons
        neuron_memory = (active_neurons * 0.001)  # 0.001 MB per active neuron
        
        # Memory for synaptic state
        synaptic_memory = (active_neurons * 0.05)  # 0.05 MB per active neuron for synapses
        
        total_memory = base_memory + spike_memory + neuron_memory + synaptic_memory
        return max(0.0, total_memory)
    
    def _calculate_trend(self, values: List[float]) -> float:
        """Calculate trend (positive = increasing, negative = decreasing)."""
        if len(values) < 2:
            return 0.0
        
        # Simple linear regression
        n = len(values)
        x = list(range(n))
        y = values
        
        sum_x = sum(x)
        sum_y = sum(y)
        sum_xy = sum(xi * yi for xi, yi in zip(x, y))
        sum_x2 = sum(xi * xi for xi in x)
        
        if sum_x2 == 0:
            return 0.0
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
        return slope
    
    def _get_empty_metrics(self) -> Dict[str, Any]:
        """Get empty metrics dictionary."""
        return {
            "total_steps": 0,
            "current_step_time_ms": 0,
            "avg_step_time_ms": 0,
            "std_step_time_ms": 0,
            "min_step_time_ms": 0,
            "max_step_time_ms": 0,
            
            "current_spikes": 0,
            "avg_spikes_per_step": 0,
            "max_spikes_per_step": 0,
            
            "current_active_neurons": 0,
            "avg_active_neurons": 0,
            "max_active_neurons": 0,
            
            "current_memory_mb": 0,
            "avg_memory_mb": 0,
            "max_memory_mb": 0,
            
            "duration_seconds": 0,
            "steps_per_second": 0,
            
            "monitoring_active": self.is_monitoring,
            "data_points": 0
        }

# Convenience function for easy performance monitoring

def create_performance_monitor(sampling_interval_ms: float = 100.0, window_size: int = 1000):
    """Create a performance monitor with specified configuration."""
    return PerformanceMonitor(sampling_interval_ms, window_size)

if __name__ == "__main__":
    # Example usage
    print("Running Performance Monitor Example")
    
    # Create monitor
    monitor = PerformanceMonitor()
    
    # Start monitoring
    monitor.start()
    
    # Simulate some steps
    for step in range(100):
        # Simulate step time (decreasing then increasing)
        step_time = 10.0 + (5.0 if step % 20 == 0 else 0.0)
        
        # Simulate neural activity
        spikes = 50 + (10 if step % 10 == 0 else 0)
        active_neurons = 100 + (20 if step % 15 == 0 else 0)
        
        # Record step
        monitor.record_step(step_time, spikes, active_neurons)
        
        # Check stability periodically
        if step % 50 == 0:
            stable = monitor.is_stable(window_size=20, threshold=0.15)
            print(f"Step {step}: Performance {'stable' if stable else 'unstable'}")
    
    # Get metrics
    metrics = monitor.get_current_metrics()
    print(f"Average step time: {metrics['avg_step_time_ms']:.2f} ms")
    print(f"Neurons per second: {metrics['steps_per_second']:.0f}")
    print(f"Performance score: {metrics.get('performance_score', 0):.1f}/100")
    
    # Stop monitoring
    monitor.stop()
    
    print("Performance monitoring example completed!")
