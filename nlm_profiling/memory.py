#!/usr/bin/env python3
"""
Memory Profiler Module

Memory usage monitoring and analysis tools for NLM brain simulations.
Provides comprehensive memory profiling capabilities with multiple analysis modes.
"""

import time
import logging
import psutil
import tracemalloc
from typing import Dict, List, Optional, Any, Tuple
from dataclasses import dataclass, field
from enum import Enum
from collections import defaultdict, Counter

from .base import PerformanceMetrics, BenchmarkResult, ProfilingFramework, OutputFormat

logger = logging.getLogger("nlm_profiling.memory")


class MemoryAnalysisMode(Enum):
    """Memory analysis mode enumeration."""
    SNAPSHOT = "snapshot"
    LEAK_DETECTION = "leak_detection"
    GROWTH_ANALYSIS = "growth_analysis"
    PEAK_ANALYSIS = "peak_analysis"


class MemoryTrackingConfig:
    """Configuration for memory tracking."""
    
    def __init__(self, steps: int = 100, tracking_interval: int = 10,
                 analysis_mode: MemoryAnalysisMode = MemoryAnalysisMode.GROWTH_ANALYSIS,
                 track_allocated: bool = True, track_freed: bool = True):
        self.steps = steps
        self.tracking_interval = tracking_interval
        self.analysis_mode = analysis_mode
        self.track_allocated = track_allocated
        self.track_freed = track_freed
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "steps": self.steps,
            "tracking_interval": self.tracking_interval,
            "analysis_mode": self.analysis_mode.value,
            "track_allocated": self.track_allocated,
            "track_freed": self.track_freed
        }


class MemoryProfileSnapshot:
    """Memory profile snapshot at a specific point in time."""
    
    def __init__(self, timestamp: float, step: int):
        self.timestamp = timestamp
        self.step = step
        self.process_memory_rss = 0.0
        self.process_memory_vms = 0.0
        self.process_memory_shared = 0.0
        self.process_memory_text = 0.0
        self.process_memory_data = 0.0
        self.process_memory_dirty = 0.0
        self.python_objects = 0
        self.python_object_size = 0.0
        self.total_allocations = 0
        self.total_freed = 0
        self.allocated_memory = 0.0
        self.freed_memory = 0.0
        
        # Custom tracking
        self.custom_metrics: Dict[str, Any] = {}
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "timestamp": self.timestamp,
            "step": self.step,
            "process_memory_rss": self.process_memory_rss,
            "process_memory_vms": self.process_memory_vms,
            "process_memory_shared": self.process_memory_shared,
            "process_memory_text": self.process_memory_text,
            "process_memory_data": self.process_memory_data,
            "process_memory_dirty": self.process_memory_dirty,
            "python_objects": self.python_objects,
            "python_object_size": self.python_object_size,
            "total_allocations": self.total_allocations,
            "total_freed": self.total_freed,
            "allocated_memory": self.allocated_memory,
            "freed_memory": self.freed_memory,
            "custom_metrics": self.custom_metrics
        }


class MemoryProfilerModule(ProfilingFramework):
    """Memory Profiler for monitoring memory usage patterns."""
    
    def __init__(self, config: Optional[MemoryTrackingConfig] = None):
        super().__init__("memory_profiler")
        self.config = config or MemoryTrackingConfig()
        self.snapshots: List[MemoryProfileSnapshot] = []
        self.tracestack_snapshot = None
        self.initial_memory = None
    
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Profile memory usage."""
        try:
            import pynlm
            
            steps = kwargs.get('steps', self.config.steps)
            tracking_interval = kwargs.get('tracking_interval', self.config.tracking_interval)
            
            # Start memory tracking
            tracemalloc.start()
            self.initial_memory = tracemalloc.take_snapshot()
            
            # Create brain for profiling
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            self.logger.info(f"Starting memory profiling: {steps} steps, interval: {tracking_interval}")
            
            # Take initial snapshot
            self._take_snapshot(0, brain)
            
            # Profile over specified steps
            for step in range(1, steps + 1):
                brain.step(step)
                
                # Take snapshot at intervals
                if step % tracking_interval == 0:
                    self._take_snapshot(step, brain)
                
                # Log progress
                if step % 100 == 0:
                    self.logger.info(f"Step {step}/{steps} completed")
            
            # Take final snapshot
            self._take_snapshot(steps, brain)
            
            # Stop memory tracking
            tracemalloc.stop()
            final_memory = tracemalloc.take_snapshot()
            
            # Analyze memory usage
            self._analyze_memory_usage(final_memory)
            
            # Generate metrics
            metrics = self._generate_memory_metrics()
            
            result = BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=metrics,
                duration=steps * 0.001,  # Estimated duration
                success=True
            )
            
            self.results = {
                "snapshots": [s.to_dict() for s in self.snapshots],
                "metrics": [m.to_dict() for m in metrics]
            }
            return result
            
        except Exception as e:
            self.logger.error(f"Error in memory profiler: {e}")
            raise
    
    def _take_snapshot(self, step: int, brain) -> None:
        """Take a memory snapshot at the current point."""
        try:
            timestamp = time.time()
            snapshot = MemoryProfileSnapshot(timestamp, step)
            
            # Get process memory information
            process = psutil.Process()
            memory_info = process.memory_info()
            
            snapshot.process_memory_rss = memory_info.rss / (1024 * 1024)  # MB
            snapshot.process_memory_vms = memory_info.vms / (1024 * 1024)  # MB
            if hasattr(memory_info, 'shared'):
                snapshot.process_memory_shared = memory_info.shared / (1024 * 1024)  # MB
            if hasattr(memory_info, 'text'):
                snapshot.process_memory_text = memory_info.text / (1024 * 1024)  # MB
            if hasattr(memory_info, 'data'):
                snapshot.process_memory_data = memory_info.data / (1024 * 1024)  # MB
            if hasattr(memory_info, 'dirty'):
                snapshot.process_memory_dirty = memory_info.dirty / (1024 * 1024)  # MB
            
            # Get Python object information
            snapshot.python_objects = len(gc.get_objects()) if 'gc' in sys.modules else 0
            
            # Get memory allocation information
            if self.tracestack_snapshot is None:
                self.tracestack_snapshot = tracemalloc.take_snapshot()
            else:
                current_snapshot = tracemalloc.take_snapshot()
                
                # Compare snapshots to find allocations
                top_stats = current_snapshot.compare_to(self.tracestack_snapshot, 'lineno')
                
                snapshot.total_allocations = len(top_stats)
                
                for stat in top_stats[:10]:  # Top 10 allocations
                    snapshot.allocated_memory += stat.size_diff / (1024 * 1024)  # MB
                
                self.tracestack_snapshot = current_snapshot
            
            self.snapshots.append(snapshot)
            
        except Exception as e:
            self.logger.error(f"Error taking memory snapshot: {e}")
    
    def _analyze_memory_usage(self, final_snapshot) -> None:
        """Analyze memory usage patterns."""
        try:
            if not self.snapshots:
                return
            
            # Calculate memory growth
            initial_rss = self.snapshots[0].process_memory_rss
            final_rss = self.snapshots[-1].process_memory_rss
            memory_growth = final_rss - initial_rss
            
            # Calculate peak memory
            peak_rss = max(snapshot.process_memory_rss for snapshot in self.snapshots)
            
            # Calculate memory stability (coefficient of variation)
            rss_values = [snapshot.process_memory_rss for snapshot in self.snapshots]
            if len(rss_values) > 1 and statistics.mean(rss_values) > 0:
                stddev_rss = statistics.stdev(rss_values)
                mean_rss = statistics.mean(rss_values)
                cv_rss = stddev_rss / mean_rss
            else:
                cv_rss = 0.0
            
            # Store analysis results
            self.memory_analysis = {
                "initial_rss_mb": initial_rss,
                "final_rss_mb": final_rss,
                "memory_growth_mb": memory_growth,
                "peak_rss_mb": peak_rss,
                "rss_coefficient_of_variation": cv_rss,
                "total_snapshots": len(self.snapshots)
            }
            
        except Exception as e:
            self.logger.error(f"Error analyzing memory usage: {e}")
    
    def _generate_memory_metrics(self) -> List[PerformanceMetrics]:
        """Generate memory metrics from snapshots."""
        metrics = []
        
        if not self.snapshots:
            return metrics
        
        # Calculate overall memory metrics
        total_growth = sum(s.process_memory_rss for s in self.snapshots) - len(self.snapshots) * self.snapshots[0].process_memory_rss
        
        metrics.append(PerformanceMetrics(
            timestamp=time.time(),
            metric_name="memory_growth",
            value=total_growth,
            unit="MB"
        ))
        
        metrics.append(PerformanceMetrics(
            timestamp=time.time(),
            metric_name="peak_memory",
            value=max(s.process_memory_rss for s in self.snapshots),
            unit="MB"
        ))
        
        metrics.append(PerformanceMetrics(
            timestamp=time.time(),
            metric_name="memory_stability_cv",
            value=sum(s.process_memory_rss for s in self.snapshots) / len(self.snapshots),
            unit=""
        ))
        
        # Add per-snapshot metrics if needed
        if len(self.snapshots) > 1:
            rss_values = [s.process_memory_rss for s in self.snapshots]
            if len(rss_values) > 1:
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="memory_growth_rate",
                    value=(rss_values[-1] - rss_values[0]) / (len(rss_values) - 1),
                    unit="MB/step"
                ))
        
        return metrics
    
    def generate_memory_insights(self, result: Dict[str, Any]) -> Dict[str, Any]:
        """Generate insights from memory profiling results."""
        insights = {}
        
        try:
            if "metrics" not in result:
                return insights
            
            # Extract key metrics
            memory_growth = next((m["value"] for m in result["metrics"] 
                                if m["metric_name"] == "memory_growth"), 0)
            peak_memory = next((m["value"] for m in result["metrics"] 
                              if m["metric_name"] == "peak_memory"), 0)
            avg_memory = next((m["value"] for m in result["metrics"] 
                             if m["metric_name"] == "memory_stability_cv"), 0)
            
            insights["memory_growth_mb"] = memory_growth
            insights["peak_memory_mb"] = peak_memory
            insights["average_memory_mb"] = avg_memory
            
            # Memory analysis
            if memory_growth > 50:
                insights["memory_analysis"] = "High memory growth detected - potential memory leak"
            elif memory_growth > 10:
                insights["memory_analysis"] = "Moderate memory growth - acceptable for simulation"
            else:
                insights["memory_analysis"] = "Low memory growth - efficient memory usage"
            
            if avg_memory > 0.2:
                insights["memory_stability_analysis"] = "High memory variability detected"
            elif avg_memory > 0.1:
                insights["memory_stability_analysis"] = "Moderate memory variability"
            else:
                insights["memory_stability_analysis"] = "Stable memory usage detected"
            
            if peak_memory > 500:
                insights["peak_memory_analysis"] = "High peak memory usage - consider memory optimization"
            elif peak_memory > 200:
                insights["peak_memory_analysis"] = "Moderate peak memory usage"
            else:
                insights["peak_memory_analysis"] = "Low peak memory usage - good memory efficiency"
            
            insights["recommendations"] = self._generate_memory_recommendations(insights)
            
        except Exception as e:
            self.logger.error(f"Error generating memory insights: {e}")
            insights["error"] = f"Failed to generate insights: {e}"
        
        return insights
    
    def _generate_memory_recommendations(self, insights: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on memory analysis."""
        recommendations = []
        
        if "memory_analysis" in insights:
            if "leak" in insights["memory_analysis"].lower():
                recommendations.append("Implement proper cleanup and garbage collection")
                recommendations.append("Review memory allocation patterns in brain components")
                recommendations.append("Consider using weak references for temporary objects")
            elif "acceptable" in insights["memory_analysis"].lower():
                recommendations.append("Memory usage is within acceptable range")
                recommendations.append("Monitor memory usage over longer runs")
            else:
                recommendations.append("Excellent memory efficiency achieved")
        
        if "memory_stability_analysis" in insights:
            if "variability" in insights["memory_stability_analysis"].lower():
                recommendations.append("Consider using memory pools or object reuse")
                recommendations.append("Optimize memory allocation patterns")
                recommendations.append("Review simulation parameters for stability")
        
        if "peak_memory_analysis" in insights:
            if "optimization" in insights["peak_memory_analysis"].lower():
                recommendations.append("Consider using memory-efficient data structures")
                recommendations.append("Implement lazy loading for large components")
                recommendations.append("Review memory-intensive operations")
        
        return recommendations

    def export_results(self, format: OutputFormat, filepath: Optional[str] = None) -> str:
        """Export memory profiling results."""
        if not self.results:
            return "No results to export"
        
        # Add memory insights to results
        if isinstance(self.results, dict):
            insights = self.generate_memory_insights(self.results)
            self.results["memory_insights"] = insights
        
        return super().export_results(format, filepath)


class MemoryProfilerCLI:
    """CLI interface for memory profiler."""
    
    def __init__(self):
        self.logger = logging.getLogger("nlm_profiling.memory.cli")
    
    def run_memory_profiling(self, args) -> int:
        """Run memory profiling."""
        try:
            # Create memory tracking config
            config = MemoryTrackingConfig(
                steps=args.steps,
                tracking_interval=args.tracking_interval
            )
            
            # Create memory profiler
            profiler = MemoryProfilerModule(config)
            
            # Run profiling
            result = profiler.run_profiling(steps=args.steps)
            
            # Output results
            if args.json:
                output = profiler.export_results(OutputFormat.JSON)
                print(output)
            else:
                print(f"\n=== Memory Profiler Results ===")
                print(f"Profiling completed in {result['duration']:.2f}s")
                
                # Print memory insights
                insights = profiler.generate_memory_insights(result)
                print(f"\nMemory Insights:")
                for key, value in insights.items():
                    if key != "recommendations":
                        print(f"  {key}: {value}")
                
                print(f"\nRecommendations:")
                for recommendation in insights.get("recommendations", []):
                    print(f"  - {recommendation}")
                
                # Print key metrics
                print(f"\nKey Metrics:")
                for metric in result.get("metrics", []):
                    print(f"  {metric['metric_name']}: {metric['value']} {metric.get('unit', '')}")
            
            return 0
            
        except Exception as e:
            self.logger.error(f"Memory profiling failed: {e}")
            return 1


if __name__ == "__main__":
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description="Memory Profiler for NLM Brain Simulations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python -m nlm_profiling.memory --steps 100
  python -m nlm_profiling.memory --steps 500 --tracking-interval 50
  python -m nlm_profiling.memory --json
        """
    )
    
    parser.add_argument("--steps", type=int, default=100, help="Number of steps to profile")
    parser.add_argument("--tracking-interval", type=int, default=10,
                      help="Memory snapshot tracking interval (steps)")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Setup logging
    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level)
    
    # Create CLI handler and run profiling
    cli = MemoryProfilerCLI()
    sys.exit(cli.run_memory_profiling(args))