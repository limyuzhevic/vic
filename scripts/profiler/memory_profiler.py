#!/usr/bin/env python3
"""
Memory Profiler for NLM Brain Simulation

Monitors and analyzes memory usage patterns in NLM (Neural Language Model) brain simulations.
Tracks memory allocation/deallocation, identifies memory leaks, analyzes memory usage patterns,
and provides memory optimization recommendations.

Features:
- Real-time memory monitoring with byte-level tracking
- Memory leak detection and analysis
- Allocation/deallocation tracking
- Memory usage pattern analysis
- Heap profiling and garbage collection monitoring
- Memory optimization recommendations
- Integration with performance profiling

Usage:
    # Basic memory profiling
    python memory_profiler.py --target simulate_brain --monitor 60
    
    # Track memory leaks
    python memory_profiler.py --target simulate_brain --leak-detection --samples 100
    
    # Analyze memory patterns
    python memory_profiler.py --analyze --output patterns.html
    
    # Profile memory usage over time
    python memory_profiler.py --continuous --interval 1.0

Examples:
    >>> from memory_profiler import MemoryProfiler
    >>> profiler = MemoryProfiler()
    >>> profiler.monitor_function(memory_intensive_operation)
    
    >>> profiler.analyze_memory_usage(simulation_memory_samples)
    >>> profiler.generate_memory_report("memory_analysis.html")

See Also:
    performance_profiler.py - For CPU and time profiling
    benchmark_suite.py - For comparative memory benchmarking
    load_tester.py - For memory stress testing

API Reference:
    MemoryProfiler - Main memory profiling class
    analyze_memory_usage() - Analyze memory usage patterns
    detect_memory_leaks() - Detect memory leaks
    generate_memory_report() - Generate memory reports

Author: NLM Team
Version: 1.0.0
License: MIT

For support and documentation, visit: https://github.com/your-repo/nlm-tools
"""

import psutil
import os
import sys
import argparse
import tracemalloc
import gc
import time
from typing import Dict, List, Any, Optional, Tuple
from pathlib import Path
import statistics
import json
from datetime import datetime
import threading
from collections import defaultdict, Counter
import matplotlib.pyplot as plt
import numpy as np
from memory_profiler import memory_usage as mem_usage
import objgraph
import pympler.asizeof as pympler_sizeof

# Import for line_profiler (if available)
try:
    from line_profiler import LineProfiler
    LINE_PROFILER_AVAILABLE = True
except ImportError:
    LINE_PROFILER_AVAILABLE = False


class AllocationTracker:
    """Track memory allocations and deallocations."""
    
    def __init__(self):
        self.allocations = []
        self.deallocations = []
        self.current_heap = []
        self.allocation_counts = defaultdict(int)
        self.deallocation_counts = defaultdict(int)
        
    def track_allocation(self, frame, size: int, count: int):
        """Track a memory allocation event."""
        self.allocations.append({
            'timestamp': time.time(),
            'thread_id': threading.get_ident(),
            'size': size,
            'count': count,
            'frame': frame,
            'backtrace': self._get_backtrace(frame)
        })
        self.allocation_counts[threading.get_ident()] += count
        self.current_heap.append(size)
        
    def track_deallocation(self, frame, size: int, count: int):
        """Track a memory deallocation event."""
        self.deallocations.append({
            'timestamp': time.time(),
            'thread_id': threading.get_ident(),
            'size': size,
            'count': count,
            'frame': frame,
            'backtrace': self._get_backtrace(frame)
        })
        self.deallocation_counts[threading.get_ident()] += count
        if self.current_heap:
            self.current_heap.pop()
            
    def _get_backtrace(self, frame) -> List[str]:
        """Get backtrace information for a frame."""
        import inspect
        try:
            frame_info = inspect.getframeinfo(frame)
            return [
                f"{frame_info.filename}:{frame_info.lineno}",
                frame_info.function
            ]
        except:
            return ["unknown"]


class LeakDetector:
    """Detect memory leaks using various techniques."""
    
    def __init__(self, samples: int = 10):
        self.samples = samples
        self.memory_samples = []
        self.heap_samples = []
        self.allocation_samples = []
        
    def collect_memory_samples(self, func, *args, **kwargs):
        """Collect memory samples over time."""
        # Take initial sample
        gc.collect()
        self.memory_samples.append({
            'timestamp': time.time(),
            'memory_mb': psutil.Process(os.getpid()).memory_info().rss / 1024 / 1024,
            'gc_collections': len(gc.get_stats())
        })
        
        # Execute function and take samples
        if tracemalloc:
            tracemalloc.start(25)  # 25MB traceback allocation
            initial_trace = tracemalloc.take_snapshot()
            
        result = func(*args, **kwargs)
        
        # Take final sample
        gc.collect()
        if tracemalloc:
            final_trace = tracemalloc.take_snapshot()
            tracemalloc.stop()
            
        self.memory_samples.append({
            'timestamp': time.time(),
            'memory_mb': psutil.Process(os.getpid()).memory_info().rss / 1024 / 1024,
            'gc_collections': len(gc.get_stats()),
            'memory_diff': self._compare_traces(initial_trace, final_trace) if tracemalloc else 0
        })
        
        return result
        
    def _compare_traces(self, trace1, trace2) -> Dict[str, Any]:
        """Compare two memory traces to find leaks."""
        if not trace1 or not trace2:
            return {}
            
        stats = trace2.compare_to(trace1, 'lineno')
        top_allocations = []
        
        for stat in stats[:10]:
            top_allocations.append({
                'file': stat.traceback.format()[-1].strip() if stat.traceback else "unknown",
                'line': stat.size_diff,
                'count': stat.count_diff,
                'size': stat.size_diff
            })
            
        return {
            'total_allocated': sum(stat.size_diff for stat in stats),
            'total_count': sum(stat.count_diff for stat in stats),
            'top_allocations': top_allocations,
            'has_leak': any(stat.size_diff > 0 for stat in stats)
        }
        
    def detect_leak_pattern(self) -> Dict[str, Any]:
        """Detect leak patterns from collected samples."""
        if len(self.memory_samples) < 2:
            return {'status': 'insufficient_data'}
            
        # Check for increasing memory trend
        memory_values = [sample['memory_mb'] for sample in self.memory_samples]
        
        if len(memory_values) >= 3:
            # Calculate linear regression slope
            x = np.arange(len(memory_values))
            y = np.array(memory_values)
            slope = np.polyfit(x, y, 1)[0]
            
            # Check for increasing trend (positive slope)
            increasing = slope > 0.01  # 0.01 MB per sample threshold
            
            # Check variance
            memory_variance = np.var(memory_values)
            
            # Check memory growth
            growth_rate = (memory_values[-1] - memory_values[0]) / memory_values[0] * 100
            
            return {
                'status': 'leaked' if increasing else 'stable',
                'slope': slope,
                'increasing': increasing,
                'memory_variance': memory_variance,
                'growth_rate_percent': growth_rate,
                'memory_samples': memory_values
            }
            
        return {'status': 'insufficient_data'}


class GarbageCollectionProfiler:
    """Profile garbage collection activity."""
    
    def __init__(self):
        self.gc_stats = []
        self.collection_events = []
        self.gc_enabled = gc.isenabled()
        
    def enable_monitoring(self):
        """Enable garbage collection monitoring."""
        if not self.gc_enabled:
            gc.enable()
            self.gc_enabled = True
            
    def monitor_collections(self, func, *args, **kwargs):
        """Monitor GC activity during function execution."""
        # Record pre-collection stats
        pre_stats = gc.get_stats()
        pre_collections = len(gc.get_stats())
        
        # Enable collection monitoring
        self.enable_monitoring()
        
        # Execute function
        result = func(*args, **kwargs)
        
        # Record post-collection stats
        gc.collect()
        post_stats = gc.get_stats()
        post_collections = len(gc.get_stats())
        
        self.gc_stats.append({
            'timestamp': time.time(),
            'pre_collections': pre_collections,
            'post_collections': post_collections,
            'collection_delta': post_collections - pre_collections,
            'pre_stats': pre_stats,
            'post_stats': post_stats
        })
        
        return result


class MemoryProfiler:
    """Main memory profiler for NLM simulations."""
    
    def __init__(self, enable_leak_detection: bool = True,
                 enable_allocation_tracking: bool = True,
                 enable_gc_profiling: bool = True,
                 sampling_interval: float = 1.0):
        """Initialize the memory profiler.
        
        Args:
            enable_leak_detection: Enable memory leak detection
            enable_allocation_tracking: Enable detailed allocation tracking
            enable_gc_profiling: Enable garbage collection profiling
            sampling_interval: Time interval between samples (seconds)
        """
        self.leak_detector = LeakDetector() if enable_leak_detection else None
        self.allocation_tracker = AllocationTracker() if enable_allocation_tracking else None
        self.gc_profiler = GarbageCollectionProfiler() if enable_gc_profiling else None
        
        self.sampling_interval = sampling_interval
        self.memory_samples = []
        self.heap_samples = []
        self.allocation_samples = []
        self.continuous_monitoring = False
        self.monitoring_thread = None
        
        # Install allocation tracking if available
        if tracemalloc:
            tracemalloc.start(25)  # 25MB for tracebacks
            
    def monitor_function(self, func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """Monitor memory usage of a function.
        
        Args:
            func: Function to monitor
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Dictionary with memory profiling results
        """
        # Get initial memory baseline
        gc.collect()
        initial_memory = psutil.Process(os.getpid()).memory_info().rss / 1024 / 1024
        
        if self.leak_detector:
            result = self.leak_detector.collect_memory_samples(func, *args, **kwargs)
        else:
            result = func(*args, **kwargs)
            
        # Get final memory snapshot
        gc.collect()
        final_memory = psutil.Process(os.getpid()).memory_info().rss / 1024 / 1024
        
        # Calculate metrics
        memory_delta = final_memory - initial_memory
        
        profiling_result = {
            'function': func.__name__ if hasattr(func, '__name__') else 'anonymous',
            'module': func.__module__ if hasattr(func, '__module__') else 'unknown',
            'initial_memory_mb': initial_memory,
            'final_memory_mb': final_memory,
            'memory_delta_mb': memory_delta,
            'memory_samples': self.leak_detector.memory_samples if self.leak_detector else [],
            'allocation_tracker': self.allocation_tracker.allocations if self.allocation_tracker else [],
            'deallocation_tracker': self.allocation_tracker.deallocations if self.allocation_tracker else [],
            'gc_stats': self.gc_profiler.gc_stats if self.gc_profiler else None
        }
        
        if self.leak_detector:
            profiling_result['leak_analysis'] = self.leak_detector.detect_leak_pattern()
            
        return profiling_result
    
    def analyze_memory_usage(self, samples: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze memory usage patterns from samples.
        
        Args:
            samples: List of memory usage samples
            
        Returns:
            Dictionary with analysis results
        """
        if not samples:
            return {'status': 'no_samples'}
            
        memory_values = [s['memory_mb'] for s in samples]
        timestamps = [s['timestamp'] for s in samples]
        
        # Calculate statistics
        analysis = {
            'sample_count': len(samples),
            'mean_memory_mb': statistics.mean(memory_values),
            'median_memory_mb': statistics.median(memory_values),
            'max_memory_mb': max(memory_values),
            'min_memory_mb': min(memory_values),
            'std_memory_mb': statistics.stdev(memory_values) if len(memory_values) > 1 else 0,
            'memory_growth_rate': (memory_values[-1] - memory_values[0]) / memory_values[0] * 100 if memory_values[0] > 0 else 0,
            'memory_variance': np.var(memory_values) if len(memory_values) >= 2 else 0,
            'time_span_seconds': timestamps[-1] - timestamps[0] if len(timestamps) >= 2 else 0,
            'memory_values': memory_values,
            'timestamps': timestamps
        }
        
        # Detect memory patterns
        if len(memory_values) >= 5:
            # Check for memory spikes
            mean_memory = analysis['mean_memory_mb']
            spike_count = sum(1 for mem in memory_values if abs(mem - mean_memory) > mean_memory * 0.2)
            analysis['spike_count'] = spike_count
            
            # Check for memory leaks (increasing trend)
            if len(memory_values) >= 3:
                x = np.arange(len(memory_values))
                y = np.array(memory_values)
                slope = np.polyfit(x, y, 1)[0]
                analysis['memory_trend_slope'] = slope
                analysis['memory_trend'] = 'increasing' if slope > 0.01 else 'stable' if abs(slope) <= 0.01 else 'decreasing'
                
        return analysis
    
    def detect_memory_leaks(self, samples: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Detect memory leaks from memory samples.
        
        Args:
            samples: List of memory usage samples
            
        Returns:
            Dictionary with leak detection results
        """
        if not samples:
            return {'status': 'no_samples'}
            
        leak_detector = LeakDetector()
        leak_detector.memory_samples = samples
        return leak_detector.detect_leak_pattern()
    
    def generate_memory_report(self, output_path: str = None, 
                              analysis_results: Dict[str, Any] = None) -> str:
        """Generate memory analysis report.
        
        Args:
            output_path: Output file path. If None, auto-generates.
            analysis_results: Pre-computed analysis results
            
        Returns:
            Path to generated report
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"memory_report_{timestamp}.html"
            
        if analysis_results is None:
            analysis_results = {}
            
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Memory Analysis Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }}
        .header {{ background-color: #28a745; color: white; padding: 30px; border-radius: 5px; }}
        .section {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; }}
        .metric-card {{ background: #f8f9fa; padding: 15px; border-radius: 5px; border-left: 4px solid #28a745; }}
        .metric-value {{ font-size: 24px; font-weight: bold; color: #28a745; }}
        .metric-label {{ color: #6c757d; font-size: 14px; }}
        .leak-warning {{ background-color: #fff3cd; border: 1px solid #ffeaa7; padding: 15px; border-radius: 5px; margin: 10px 0; }}
        .stable {{ background-color: #d4edda; border: 1px solid #c3e6cb; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .allocation {{ background-color: #f8d7da; }}
        .deallocation {{ background-color: #d1ecf1; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Memory Analysis Report</h1>
        <p>Generated: {datetime.now().isoformat()}</p>
    </div>
    
    <div class="section">
        <h2>Memory Usage Analysis</h2>
    """
        
        if analysis_results:
            html_content += f"""
        <div class="metric-grid">
            <div class="metric-card">
                <div class="metric-value">{analysis_results.get('sample_count', 0)}</div>
                <div class="metric-label">Sample Count</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{analysis_results.get('mean_memory_mb', 0):.1f} MB</div>
                <div class="metric-label">Mean Memory</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{analysis_results.get('max_memory_mb', 0):.1f} MB</div>
                <div class="metric-label">Max Memory</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{analysis_results.get('memory_growth_rate', 0):.1f}%</div>
                <div class="metric-label">Growth Rate</div>
            </div>
        </div>
            """
            
        # Memory trend visualization
        if 'memory_values' in analysis_results:
            html_content += """
        <h3>Memory Usage Trend</h3>
        <canvas id="memoryChart" width="800" height="400"></canvas>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <script>
            const ctx = document.getElementById('memoryChart').getContext('2d');
            new Chart(ctx, {
                type: 'line',
                data: {
                    labels: Array.from({length: memoryData.length}, (_, i) => `Sample ${i}`),
                    datasets: [{
                        label: 'Memory Usage (MB)',
                        data: memoryData,
                        borderColor: 'rgb(75, 192, 192)',
                        backgroundColor: 'rgba(75, 192, 192, 0.2)',
                        tension: 0.1
                    }]
                },
                options: {
                    responsive: true,
                    plugins: {
                        title: { display: true, text: 'Memory Usage Over Time' }
                    },
                    scales: {
                        y: { beginAtZero: true, title: { display: true, text: 'Memory (MB)' } },
                        x: { title: { display: true, text: 'Sample Index' } }
                    }
                }
            });
        </script>
            """.replace('memoryData', str(analysis_results.get('memory_values', [])))
            
        html_content += """
    </div>
    
    <div class="section">
        <h2>Leak Detection Results</h2>
    """
        
        if 'leak_analysis' in analysis_results:
            leak_analysis = analysis_results['leak_analysis']
            status = leak_analysis.get('status', 'unknown')
            
            if status == 'leaked':
                html_content += f"""
            <div class="leak-warning">
                <h3>⚠️ MEMORY LEAK DETECTED</h3>
                <p><strong>Slope:</strong> {leak_analysis.get('slope', 0):.4f} MB per sample</p>
                <p><strong>Growth Rate:</strong> {leak_analysis.get('growth_rate_percent', 0):.1f}%</p>
                <p><strong>Variance:</strong> {leak_analysis.get('memory_variance', 0):.2f}</p>
            </div>
                """
            elif status == 'stable':
                html_content += """
            <div class="stable">
                <h3>✅ Memory Usage is STABLE</h3>
                <p>No significant memory growth detected.</p>
            </div>
                """
            else:
                html_content += """
            <div>
                <h3>ℹ️ Insufficient Data</h3>
                <p>Collect more samples to perform reliable leak detection.</p>
            </div>
                """
                
        html_content += """
    </div>
    
    <div class="section">
        <h2>Allocation/Deallocation Analysis</h2>
    """
        
        if 'allocation_tracker' in self.memory_samples[-1] if self.memory_samples else []:
            allocations = self.memory_samples[-1]['allocation_tracker']
            deallocations = self.memory_samples[-1]['deallocation_tracker']
            
            if allocations:
                html_content += f"""
            <h3>Allocations ({len(allocations)})</h3>
            <table>
                <tr>
                    <th>Timestamp</th>
                    <th>Thread ID</th>
                    <th>Size (bytes)</th>
                    <th>Count</th>
                    <th>Frame</th>
                </tr>
        """
                for alloc in allocations[:10]:
                    html_content += f"""
                <tr class="allocation">
                    <td>{alloc.get('timestamp', 0):.2f}</td>
                    <td>{alloc.get('thread_id', 0)}</td>
                    <td>{alloc.get('size', 0)}</td>
                    <td>{alloc.get('count', 0)}</td>
                    <td>{alloc.get('frame', 'unknown')}</td>
                </tr>
                    """
                html_content += """
            </table>
                """
                
            if deallocations:
                html_content += f"""
            <h3>Deallocations ({len(deallocations)})</h3>
            <table>
                <tr>
                    <th>Timestamp</th>
                    <th>Thread ID</th>
                    <th>Size (bytes)</th>
                    <th>Count</th>
                    <th>Frame</th>
                </tr>
        """
                for dealloc in deallocations[:10]:
                    html_content += f"""
                <tr class="deallocation">
                    <td>{dealloc.get('timestamp', 0):.2f}</td>
                    <td>{dealloc.get('thread_id', 0)}</td>
                    <td>{dealloc.get('size', 0)}</td>
                    <td>{dealloc.get('count', 0)}</td>
                    <td>{dealloc.get('frame', 'unknown')}</td>
                </tr>
                    """
                html_content += """
            </table>
                """
                
        html_content += """
    </div>
    
</body>
</html>
        """
        
        with open(output_path, 'w') as f:
            f.write(html_content)
            
        print(f"Memory report generated: {output_path}")
        return output_path


def profile_memory_usage(func: Callable, *args, **kwargs) -> Dict[str, Any]:
    """Convenience function to profile memory usage.
    
    Args:
        func: Function to profile
        *args: Function arguments
        **kwargs: Function keyword arguments
        
    Returns:
        Dictionary with memory profiling results
    """
    profiler = MemoryProfiler()
    return profiler.monitor_function(func, *args, **kwargs)


def detect_memory_leaks(func: Callable, iterations: int = 10) -> Dict[str, Any]:
    """Detect memory leaks by running a function multiple times.
    
    Args:
        func: Function to test for leaks
        iterations: Number of iterations to run
        
    Returns:
        Dictionary with leak detection results
    """
    profiler = MemoryProfiler()
    leak_results = []
    
    for i in range(iterations):
        result = profiler.monitor_function(func)
        leak_results.append(result)
        
    # Combine all samples
    all_samples = []
    for result in leak_results:
        all_samples.extend(result.get('memory_samples', []))
        
    analysis = profiler.analyze_memory_usage(all_samples)
    analysis['detailed_results'] = leak_results
    
    return analysis


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Profile NLM brain memory usage")
    parser.add_argument("--target", help="Target function or simulation to profile")
    parser.add_argument("--iterations", type=int, default=1, help="Number of iterations")n    parser.add_argument("--monitor", type=int, default=60, help="Continuous monitoring duration (seconds)")n    parser.add_argument("--analyze", action="store_true", help="Analyze memory patterns")
    parser.add_argument("--output", help="Output report file path")
    parser.add_argument("--leak-detection", action="store_true", help="Enable memory leak detection")
    parser.add_argument("--continuous", action="store_true", help="Continuous memory monitoring")
    parser.add_argument("--interval", type=float, default=1.0, help="Sampling interval (seconds)")
    
    args = parser.parse_args()
    
    print("Starting NLM Memory Profiling...")
    print(f"Target: {args.target}")
    print(f"Iterations: {args.iterations}")
    
    profiler = MemoryProfiler(enable_leak_detection=args.leak_detection)
    
    if args.continuous:
        print(f"Starting continuous monitoring for {args.monitor} seconds...")
        print(f"Sampling interval: {args.interval} seconds")
        
        start_time = time.time()
        profiler.continuous_monitoring = True
        
        while profiler.continuous_monitoring and (time.time() - start_time) < args.monitor:
            profiler.memory_samples.append({
                'timestamp': time.time(),
                'memory_mb': psutil.Process(os.getpid()).memory_info().rss / 1024 / 1024
            })
            time.sleep(args.interval)
            
        profiler.continuous_monitoring = False
            
    if args.target:
        # Import target function
        try:
            target_module, target_func_name = args.target.rsplit('.', 1)
            import importlib
            target_module = importlib.import_module(target_module)
            target_func = getattr(target_module, target_func_name)
            
            for i in range(args.iterations):
                print(f"Iteration {i + 1}/{args.iterations}")
                profiler.monitor_function(target_func)
                
        except ImportError as e:
            print(f"Error importing target: {e}")
            sys.exit(1)
        except AttributeError as e:
            print(f"Error finding function: {e}")
            sys.exit(1)
            
    if args.analyze and profiler.memory_samples:
        analysis = profiler.analyze_memory_usage(profiler.memory_samples)
        report_path = profiler.generate_memory_report(args.output, analysis)
        print(f"Memory analysis complete. Report saved to: {report_path}")
        
    print("Memory profiling complete.")