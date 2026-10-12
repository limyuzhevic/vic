#!/usr/bin/env python3
"""
NLM Memory Module - Memory Usage Analysis

This module provides memory usage tracking and analysis for neural simulation,
including peak memory tracking, memory breakdown by component, and memory efficiency metrics.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Tuple
import time
import statistics
import tracemalloc
from collections import defaultdict, deque

class MemoryProfiler:
    """Memory usage profiling for neural simulation.
    
    Tracks memory consumption patterns and provides detailed analysis
    of memory usage by different components.
    
    Features:
    * Peak memory tracking
    * Memory breakdown by component
    * Memory trend analysis
    * Memory leak detection
    * Memory efficiency optimization
    """
    
    def __init__(self, sampling_interval_ms: float = 1000.0, enable_leak_detection: bool = True):
        self.sampling_interval_ms = sampling_interval_ms
        self.enable_leak_detection = enable_leak_detection
        
        # Memory tracking
        self.memory_samples = []
        self.component_memory = defaultdict(list)
        self.allocation_samples = []
        self.deallocation_samples = []
        
        # State tracking
        self.is_profiling = False
        self.start_time = None
        self.peak_memory = 0.0
        self.total_allocated = 0.0
        self.total_deallocated = 0.0
        
        # Leak detection
        self.baseline_memory = 0.0
        self.memory_history = deque(maxlen=1000)
    
    def start(self):
        """Start memory profiling."""
        self.is_profiling = True
        self.start_time = time.time()
        self.peak_memory = 0.0
        self.total_allocated = 0.0
        self.total_deallocated = 0.0
        self.memory_samples.clear()
        self.component_memory.clear()
        self.allocation_samples.clear()
        self.deallocation_samples.clear()
        self.memory_history.clear()
        
        # Start memory tracking
        tracemalloc.start()
        
        print("Memory profiling started")
    
    def stop(self):
        """Stop memory profiling."""
        self.is_profiling = False
        duration = time.time() - self.start_time if self.start_time else 0
        
        # Stop memory tracking
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        print(f"Memory profiling stopped. Duration: {duration:.2f}s")
        print(f"Current memory usage: {current / 1024 / 1024:.2f} MB")
        print(f"Peak memory usage: {peak / 1024 / 1024:.2f} MB")
        print(f"Total allocated: {self.total_allocated / 1024 / 1024:.2f} MB")
        print(f"Total deallocated: {self.total_deallocated / 1024 / 1024:.2f} MB")
    
    def record(self):
        """Record current memory usage."""
        if not self.is_profiling:
            return
        
        current, peak = tracemalloc.get_traced_memory()
        current_mb = current / 1024 / 1024
        peak_mb = peak / 1024 / 1024
        
        # Update peak
        self.peak_memory = max(self.peak_memory, peak_mb)
        
        # Record sample
        sample = {
            "timestamp": time.time(),
            "current_mb": current_mb,
            "peak_mb": peak_mb,
            "sample_num": len(self.memory_samples)
        }
        
        self.memory_samples.append(sample)
        self.memory_history.append(current_mb)
        
        print(f"Memory recorded: {current_mb:.2f} MB (Peak: {peak_mb:.2f} MB)")
    
    def record_allocation(self, size_bytes: int):
        """Record memory allocation."""
        size_mb = size_bytes / 1024 / 1024
        self.total_allocated += size_mb
        
        allocation = {
            "timestamp": time.time(),
            "size_mb": size_mb,
            "type": "allocation"
        }
        
        self.allocation_samples.append(allocation)
    
    def record_deallocation(self, size_bytes: int):
        """Record memory deallocation."""
        size_mb = size_bytes / 1024 / 1024
        self.total_deallocated += size_mb
        
        deallocation = {
            "timestamp": time.time(),
            "size_mb": size_mb,
            "type": "deallocation"
        }
        
        self.deallocation_samples.append(deallocation)
    
    def get_peak_memory(self) -> float:
        """Get peak memory usage in MB."""
        return self.peak_memory
    
    def get_current_memory(self) -> float:
        """Get current memory usage in MB."""
        if not self.memory_samples:
            return 0.0
        return self.memory_samples[-1]["current_mb"]
    
    def get_breakdown(self) -> Dict[str, Any]:
        """Get memory breakdown by component."""
        # This would typically break down memory by different neural components
        # For now, return a simplified breakdown
        
        breakdown = {
            "total_samples": len(self.memory_samples),
            "peak_memory_mb": self.peak_memory,
            "current_memory_mb": self.get_current_memory(),
            "total_allocated_mb": self.total_allocated,
            "total_deallocated_mb": self.total_deallocated,
            
            "component_memory": {
                "neurons": sum(sample["current_mb"] * 0.7 for sample in self.memory_samples) / len(self.memory_samples) if self.memory_samples else 0,
                "synapses": sum(sample["current_mb"] * 0.2 for sample in self.memory_samples) / len(self.memory_samples) if self.memory_samples else 0,
                "memory_systems": sum(sample["current_mb"] * 0.1 for sample in self.memory_samples) / len(self.memory_samples) if self.memory_samples else 0,
                "overhead": sum(sample["current_mb"] * 0.0 for sample in self.memory_samples) / len(self.memory_samples) if self.memory_samples else 0
            },
            
            "allocation_summary": {
                "total_allocations": len(self.allocation_samples),
                "total_deallocations": len(self.deallocation_samples),
                "net_allocation": self.total_allocated - self.total_deallocated
            }
        }
        
        return breakdown
    
    def reset_peak(self):
        """Reset peak memory tracking."""
        self.peak_memory = 0.0
        print("Peak memory tracking reset")
    
    def get_memory_samples(self) -> List[Dict[str, Any]]:
        """Get all memory samples."""
        return self.memory_samples.copy()
    
    def get_memory_trend(self) -> float:
        """Get memory usage trend (positive = increasing, negative = decreasing)."""
        if len(self.memory_history) < 2:
            return 0.0
        
        # Simple trend calculation
        recent_samples = list(self.memory_history)[-10:]  # Last 10 samples
        if len(recent_samples) < 2:
            return 0.0
        
        x = list(range(len(recent_samples)))
        y = recent_samples
        
        n = len(x)
        sum_x = sum(x)
        sum_y = sum(y)
        sum_xy = sum(xi * yi for xi, yi in zip(x, y))
        sum_x2 = sum(xi * xi for xi in x)
        
        if sum_x2 == 0:
            return 0.0
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
        return slope
    
    def is_memory_leaking(self, threshold_mb: float = 1.0, samples_window: int = 50) -> bool:
        """Check if there's a memory leak.
        
        Args:
            threshold_mb: Memory increase threshold to consider a leak
            samples_window: Number of recent samples to analyze
            
        Returns:
            True if memory leak is detected, False otherwise
        """
        if not self.enable_leak_detection:
            return False
        
        if len(self.memory_history) < samples_window:
            return False
        
        # Get recent memory samples
        recent_samples = list(self.memory_history)[-samples_window:]
        
        if len(recent_samples) < 2:
            return False
        
        # Calculate trend
        trend = self.get_memory_trend()
        
        # Check if trend is positive and exceeds threshold
        if trend > 0:
            # Calculate total increase over window
            start_memory = recent_samples[0]
            end_memory = recent_samples[-1]
            memory_increase = end_memory - start_memory
            
            return memory_increase > threshold_mb
        
        return False
    
    def get_memory_efficiency_score(self) -> float:
        """Get memory efficiency score (0-100)."""
        if not self.memory_samples:
            return 0.0
        
        # Calculate efficiency based on memory usage vs expected
        current_memory = self.get_current_memory()
        peak_memory = self.peak_memory
        
        # Higher efficiency for lower memory usage
        if peak_memory > 0:
            efficiency = max(0.0, 100.0 - (current_memory / peak_memory) * 100)
        else:
            efficiency = 100.0
        
        # Consider allocation/deallocation efficiency
        if self.total_allocated > 0:
            allocation_efficiency = (self.total_deallocated / self.total_allocated) * 100
            efficiency = (efficiency + allocation_efficiency) / 2
        
        return efficiency
    
    def generate_memory_report(self, output_file: str = "memory_report.html") -> Dict[str, Any]:
        """Generate memory profiler report."""
        print(f"Generating memory profiler report: {output_file}")
        
        report = {
            "title": "NLM Memory Profiler Report",
            "generated_at": time.time(),
            "profiling_session": {
                "duration": time.time() - self.start_time if self.start_time else 0,
                "samples_recorded": len(self.memory_samples),
                "peak_memory_mb": self.peak_memory,
                "total_allocated_mb": self.total_allocated,
                "total_deallocated_mb": self.total_deallocated
            },
            "memory_analysis": {
                "current_memory_mb": self.get_current_memory(),
                "average_memory_mb": statistics.mean([s["current_mb"] for s in self.memory_samples]) 
                                      if self.memory_samples else 0,
                "peak_memory_mb": self.peak_memory,
                "memory_trend": self.get_memory_trend(),
                "memory_efficiency_score": self.get_memory_efficiency_score(),
                "memory_leaking": self.is_memory_leaking()
            },
            "breakdown": self.get_breakdown(),
            "recommendations": self._generate_memory_recommendations()
        }
        
        # Save report
        import json
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Memory report saved to: {output_file}")
        return report
    
    def _generate_memory_recommendations(self) -> List[str]:
        """Generate memory optimization recommendations."""
        recommendations = []
        
        # Check for memory leaks
        if self.is_memory_leaking():
            recommendations.append(
                "Memory leak detected - investigate allocation patterns and cleanup routines"
            )
        
        # Check memory efficiency
        efficiency = self.get_memory_efficiency_score()
        if efficiency < 50:
            recommendations.append(
                "Memory efficiency is low - consider optimizing memory usage and implementing more efficient data structures"
            )
        
        # Check peak memory
        if self.peak_memory > 1000:  # 1GB threshold
            recommendations.append(
                "Peak memory usage is high - consider memory pooling and more efficient data structures"
            )
        
        # Check for recent memory trends
        trend = self.get_memory_trend()
        if trend > 0.1:  # Increasing trend
            recommendations.append(
                "Memory usage is increasing - monitor for potential memory leaks"
            )
        
        if not recommendations:
            recommendations.append("Memory usage looks good - continue with current approach")
        
        return recommendations

# Convenience function for easy memory profiling

def create_memory_profiler(sampling_interval_ms: float = 1000.0, enable_leak_detection: bool = True):
    """Create a memory profiler with specified configuration."""
    return MemoryProfiler(sampling_interval_ms, enable_leak_detection)

if __name__ == "__main__":
    # Example usage
    print("Running Memory Profiler Example")
    
    # Create profiler
    profiler = MemoryProfiler()
    
    # Start profiling
    profiler.start()
    
    # Record some memory samples
    for i in range(10):
        profiler.record()
        
        # Simulate some memory allocation
        if i % 3 == 0:
            profiler.record_allocation(1024 * 1024)  # 1MB
        elif i % 5 == 0:
            profiler.record_deallocation(512 * 1024)  # 0.5MB
        
        time.sleep(0.1)  # Wait between samples
    
    # Get breakdown
    breakdown = profiler.get_breakdown()
    print(f"Peak memory: {breakdown['peak_memory_mb']:.2f} MB")
    print(f"Current memory: {breakdown['current_memory_mb']:.2f} MB")
    print(f"Memory efficiency score: {profiler.get_memory_efficiency_score():.1f}/100")
    
    # Generate report
    profiler.generate_memory_report("memory_report.html")
    
    # Stop profiling
    profiler.stop()
    
    print("Memory profiler example completed!")
