#!/usr/bin/env python3
"""
Performance Profiler for NLM Brain Simulation

Profiles and analyzes the performance of NLM (Neural Language Model) brain simulations.
Provides detailed metrics on execution time, memory usage, CPU utilization,
and function-level profiling capabilities.

Features:
- Time profiling with millisecond precision
- Memory usage tracking with peak detection
- CPU usage monitoring
- Function/method-specific profiling
- Performance bottleneck identification
- Visualization and report generation
- Integration with benchmarking suites

Usage:
    # Basic performance profiling
    python performance_profiler.py --target simulate_brain --iterations 100
    
    # Profile specific functions
    python performance_profiler.py --func neuron_process --samples 1000
    
    # Generate detailed reports
    python performance_profiler.py --output results.html --verbose
    
    # Profile memory usage
    python performance_profiler.py --memory --track-leaks

Examples:
    >>> from performance_profiler import PerformanceProfiler
    >>> profiler = PerformanceProfiler()
    >>> profiler.profile_function(neural_network.train, epochs=100)
    
    >>> profiler.profile_simulation(simulate_brain, config=large_config)
    >>> profiler.generate_report("simulation_results.html")

See Also:
    memory_profiler.py - For detailed memory analysis
    benchmark_suite.py - For comparative benchmarking
    performance_analyzer.py - For bottleneck analysis

API Reference:
    PerformanceProfiler - Main profiling class
    generate_report() - Generate HTML performance reports
    profile_function() - Profile specific functions
    profile_simulation() - Profile entire simulation runs

Author: NLM Team
Version: 1.0.0
License: MIT

For support and documentation, visit: https://github.com/your-repo/nlm-tools
"""

import time
import psutil
import os
import sys
import argparse
import tracemalloc
import functools
from typing import Dict, List, Any, Optional, Callable, Union
from pathlib import Path
import statistics
import json
from datetime import datetime
import threading
import gc
import pandas as pd
import matplotlib.pyplot as plt
from contextlib import contextmanager

# Import for GPU monitoring (if available)
try:
    import GPUtil
    GPU_AVAILABLE = True
except ImportError:
    GPU_AVAILABLE = False

# Import for line_profiler (if available)
try:
    from line_profiler import LineProfiler
    LINE_PROFILER_AVAILABLE = True
except ImportError:
    LINE_PROFILER_AVAILABLE = False


class MemoryTracker:
    """Track memory allocation and deallocation details."""
    
    def __init__(self):
        self.snapshots = []
        self.allocations = []
        self.current_peak = 0
        self.total_allocated = 0
        
    def take_snapshot(self, label: str = ""):
        """Take a memory snapshot with current allocation details."""
        if tracemalloc:
            snapshot = tracemalloc.take_snapshot()
            self.snapshots.append({
                'label': label,
                'timestamp': time.time(),
                'snapshot': snapshot,
                'total_allocated': tracemalloc.get_traced_memory()[1],
                'current_allocated': tracemalloc.get_traced_memory()[0]
            })
            
    def get_memory_diff(self, snapshot1: Any, snapshot2: Any) -> Dict[str, Any]:
        """Get memory difference between two snapshots."""
        if not snapshot1 or not snapshot2:
            return {}
            
        stats = snapshot2.compare_to(snapshot1, 'lineno')
        return {
            'total_size': sum(stat.size_diff for stat in stats),
            'total_count': sum(stat.count_diff for stat in stats),
            'top_allocations': [
                {
                    'file': stat.traceback.format()[-1].strip(),
                    'line': stat.size_diff,
                    'count': stat.count_diff,
                    'size': stat.size_diff
                }
                for stat in stats[:10]
            ]
        }


class CPUProfiler:
    """Monitor CPU usage and performance metrics."""
    
    def __init__(self):
        self.readings = []
        self.start_time = None
        self.end_time = None
        self.threads = []
        
    def start(self):
        """Start CPU monitoring."""
        self.start_time = time.time()
        self.threads.append(threading.Thread(target=self._monitor_cpu, daemon=True))
        
    def _monitor_cpu(self):
        """Background CPU monitoring thread."""
        process = psutil.Process(os.getpid())
        while self.start_time and (self.end_time is None or time.time() < self.end_time):
            self.readings.append({
                'timestamp': time.time(),
                'cpu_percent': process.cpu_percent(),
                'memory_mb': process.memory_info().rss / 1024 / 1024,
                'threads': process.num_threads(),
                'context_switches': process.num_ctx_switches()
            })
            time.sleep(0.1)
            
    def stop(self):
        """Stop CPU monitoring."""
        self.end_time = time.time()
        
    def get_stats(self) -> Dict[str, Any]:
        """Get CPU and system statistics."""
        if not self.readings:
            return {}
            
        cpu_percents = [r['cpu_percent'] for r in self.readings]
        memory_mb = [r['memory_mb'] for r in self.readings]
        
        return {
            'duration': len(self.readings) * 0.1,
            'avg_cpu_percent': statistics.mean(cpu_percents) if cpu_percents else 0,
            'max_cpu_percent': max(cpu_percents) if cpu_percents else 0,
            'min_cpu_percent': min(cpu_percents) if cpu_percents else 0,
            'std_cpu_percent': statistics.stdev(cpu_percents) if len(cpu_percents) > 1 else 0,
            'avg_memory_mb': statistics.mean(memory_mb) if memory_mb else 0,
            'max_memory_mb': max(memory_mb) if memory_mb else 0,
            'readings_count': len(self.readings),
            'thread_count': self.readings[-1]['threads'] if self.readings else 0
        }


class GPUProfiler:
    """Monitor GPU usage (if available)."""
    
    def __init__(self):
        self.metrics = {}
        self.available = GPU_AVAILABLE
        
    def get_gpu_stats(self) -> Dict[str, Any]:
        """Get current GPU statistics."""
        if not self.available:
            return {'available': False}
            
        try:
            gpus = GPUtil.getGPUs()
            if gpus:
                gpu = gpus[0]
                return {
                    'available': True,
                    'load': gpu.load,
                    'temperature': gpu.temperature,
                    'memory_used': gpu.memoryUsed,
                    'memory_total': gpu.memoryTotal,
                    'memory_percent': gpu.memoryUtil,
                    'gpu_utilization': gpu.gpuUtil,
                    'processes': len(gpu.processes)
                }
        except Exception as e:
            return {'available': True, 'error': str(e)}
            
        return {'available': False}


class PerformanceProfiler:
    """Main performance profiler for NLM simulations."""
    
    def __init__(self, enable_memory_tracking: bool = True, 
                 enable_cpu_monitoring: bool = True,
                 enable_gpu_monitoring: bool = False,
                 enable_line_profiling: bool = False):
        """Initialize the performance profiler.
        
        Args:
            enable_memory_tracking: Enable detailed memory tracking
            enable_cpu_monitoring: Enable CPU usage monitoring
            enable_gpu_monitoring: Enable GPU usage monitoring (if available)
            enable_line_profiling: Enable line-by-line profiling (if available)
        """
        self.memory_tracker = MemoryTracker() if enable_memory_tracking else None
        self.cpu_profiler = CPUProfiler() if enable_cpu_monitoring else None
        self.gpu_profiler = GPUProfiler() if enable_gpu_monitoring else None
        self.line_profiler = LineProfiler() if enable_line_profiling and LINE_PROFILER_AVAILABLE else None
        
        self.results = {}
        self.start_time = None
        self.end_time = None
        self.function_profiles = {}
        
    @contextmanager
    def profile_context(self, label: str = "default"):
        """Context manager for profiling code blocks."""
        self.start_time = time.time()
        if self.memory_tracker:
            self.memory_tracker.take_snapshot(f"{label}_start")
        if self.cpu_profiler:
            self.cpu_profiler.start()
            
        if self.line_profiler:
            self.line_profiler.add_function(inspect.currentframe().f_back.f_code)
            
        try:
            yield
        finally:
            if self.line_profiler:
                self.line_profiler.print_stats()
                
            self.end_time = time.time()
            if self.cpu_profiler:
                self.cpu_profiler.stop()
                
            if self.memory_tracker:
                self.memory_tracker.take_snapshot(f"{label}_end")
                
            self._record_results(label)
            
    def profile_function(self, func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """Profile a specific function.
        
        Args:
            func: Function to profile
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Dictionary with profiling results
        """
        import inspect
        func_name = func.__name__
        module_name = func.__module__
        
        # Start profiling
        if self.memory_tracker:
            self.memory_tracker.take_snapshot(f"{module_name}.{func_name}_start")
        if self.cpu_profiler:
            self.cpu_profiler.start()
            
        start_time = time.time()
        
        # Execute function
        result = func(*args, **kwargs)
        
        # Stop profiling
        end_time = time.time()
        if self.cpu_profiler:
            self.cpu_profiler.stop()
        if self.memory_tracker:
            self.memory_tracker.take_snapshot(f"{module_name}.{func_name}_end")
            
        # Calculate metrics
        duration = end_time - start_time
        metrics = {
            'function': func_name,
            'module': module_name,
            'duration': duration,
            'start_time': start_time,
            'end_time': end_time
        }
        
        # Add CPU metrics
        if self.cpu_profiler:
            metrics.update(self.cpu_profiler.get_stats())
            
        # Add memory metrics
        if self.memory_tracker and len(self.memory_tracker.snapshots) >= 2:
            start_snapshot = self.memory_tracker.snapshots[-2]
            end_snapshot = self.memory_tracker.snapshots[-1]
            metrics['memory_diff'] = self.memory_tracker.get_memory_diff(
                start_snapshot['snapshot'], 
                end_snapshot['snapshot']
            )
            metrics['peak_memory'] = self.memory_tracker.current_peak
            
        # Store profile
        self.function_profiles[func_name] = metrics
        
        return metrics
    
    def profile_simulation(self, simulation_func: Callable, 
                          *args, **kwargs) -> Dict[str, Any]:
        """Profile an entire simulation run.
        
        Args:
            simulation_func: Simulation function to run
            *args: Simulation arguments
            **kwargs: Simulation keyword arguments
            
        Returns:
            Dictionary with simulation profiling results
        """
        with self.profile_context("simulation"):
            result = simulation_func(*args, **kwargs)
            
        # Compile comprehensive results
        simulation_metrics = {
            'simulation_result': result,
            'duration': self.end_time - self.start_time if self.end_time else 0,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'function_profiles': self.function_profiles,
            'memory_peak': self.memory_tracker.current_peak if self.memory_tracker else None,
            'system_cpu': self.cpu_profiler.get_stats() if self.cpu_profiler else None,
            'gpu_stats': self.gpu_profiler.get_gpu_stats() if self.gpu_profiler else None
        }
        
        return simulation_metrics
    
    def _record_results(self, label: str):
        """Record profiling results for a context."""
        if not self.start_time or not self.end_time:
            return
            
        duration = self.end_time - self.start_time
        
        result = {
            'label': label,
            'duration': duration,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'timestamp': datetime.now().isoformat()
        }
        
        # Add CPU metrics
        if self.cpu_profiler:
            result['cpu_stats'] = self.cpu_profiler.get_stats()
            
        # Add memory metrics
        if self.memory_tracker and len(self.memory_tracker.snapshots) >= 2:
            start_snapshot = self.memory_tracker.snapshots[-2]
            end_snapshot = self.memory_tracker.snapshots[-1]
            result['memory_diff'] = self.memory_tracker.get_memory_diff(
                start_snapshot['snapshot'], 
                end_snapshot['snapshot']
            )
            result['peak_memory'] = self.memory_tracker.current_peak
            
        # Add GPU metrics
        if self.gpu_profiler:
            result['gpu_stats'] = self.gpu_profiler.get_gpu_stats()
            
        self.results[label] = result
        
    def generate_report(self, output_path: str = None, 
                       format: str = "html") -> str:
        """Generate performance report.
        
        Args:
            output_path: Output file path. If None, auto-generates.
            format: Report format ('html', 'json', 'markdown')
            
        Returns:
            Path to generated report
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"performance_report_{timestamp}.{format}"
            
        if format == "html":
            self._generate_html_report(output_path)
        elif format == "json":
            self._generate_json_report(output_path)
        elif format == "markdown":
            self._generate_markdown_report(output_path)
            
        return output_path
    
    def _generate_html_report(self, output_path: str):
        """Generate HTML performance report."""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Profile Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }}
        .header {{ background-color: #2c3e50; color: white; padding: 30px; border-radius: 5px; }}
        .section {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; }}
        .metric-card {{ background: #f8f9fa; padding: 15px; border-radius: 5px; border-left: 4px solid #007bff; }}
        .metric-value {{ font-size: 24px; font-weight: bold; color: #007bff; }}
        .metric-label {{ color: #6c757d; font-size: 14px; }}
        .function-table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        .function-table th, .function-table td {{ border: 1px solid #ddd; padding: 12px; text-align: left; }}
        .function-table th {{ background-color: #f2f2f2; }}
        .top-row {{ background-color: #fff3cd; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .highlight {{ background-color: #d4edda; }}
        .warning {{ background-color: #f8d7da; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Profile Report</h1>
        <p>Generated: {datetime.now().isoformat()}</p>
        <p>Total Profiles: {len(self.results)}</p>
    </div>
    
    <div class="section">
        <h2>Overview</h2>
        <div class="metric-grid">
            <div class="metric-card">
                <div class="metric-value">{len(self.results)}</div>
                <div class="metric-label">Total Profiles</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{sum(r.get('duration', 0) for r in self.results.values()):.3f}s</div>
                <div class="metric-label">Total Duration</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{self.results.get('simulation', {}).get('memory_peak', 0):.1f} MB</div>
                <div class="metric-label">Peak Memory Usage</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{self.results.get('simulation', {}).get('cpu_stats', {}).get('max_cpu_percent', 0):.1f}%</div>
                <div class="metric-label">Max CPU Usage</div>
            </div>
        </div>
    </div>
    
    <div class="section">
        <h2>Detailed Results</h2>
        <table>
            <tr>
                <th>Profile Label</th>
                <th>Duration (s)</th>
                <th>Memory Peak (MB)</th>
                <th>CPU Usage (%)</th>
                <th>GPU Utilization (%)</th>
            </tr>
    """
        
        for label, result in self.results.items():
            is_highlight = label in ['simulation', 'main']
            highlight_class = 'highlight' if is_highlight else ''
            
            cpu_max = result.get('cpu_stats', {}).get('max_cpu_percent', 0) if 'cpu_stats' in result else 0
            gpu_util = result.get('gpu_stats', {}).get('gpu_utilization', 0) if 'gpu_stats' in result and result['gpu_stats'].get('available') else 0
            
            html_content += f"""
            <tr class="{highlight_class}">
                <td><strong>{label}</strong></td>
                <td>{result.get('duration', 0):.3f}</td>
                <td>{result.get('memory_peak', 0):.1f}</td>
                <td>{cpu_max:.1f}</td>
                <td>{gpu_util:.1f}</td>
            </tr>
            """
            
        html_content += """
        </table>
    </div>
    
    <div class="section">
        <h2>Function Profiles</h2>
        <table class="function-table">
            <tr>
                <th>Function Name</th>
                <th>Duration (s)</th>
                <th>Module</th>
                <th>CPU Usage (%)</th>
                <th>Memory Delta (MB)</th>
            </tr>
    """
        
        for func_name, func_data in self.function_profiles.items():
            is_top = func_data.get('duration', 0) > 0.1
            top_class = 'top-row' if is_top else ''
            
            html_content += f"""
            <tr class="{top_class}">
                <td><strong>{func_name}</strong></td>
                <td>{func_data.get('duration', 0):.3f}</td>
                <td>{func_data.get('module', '')}</td>
                <td>{func_data.get('avg_cpu_percent', 0):.1f}</td>
                <td>{func_data.get('memory_diff', {}).get('total_size', 0):.1f}</td>
            </tr>
            """
            
        html_content += """
        </table>
    </div>
    
    <div class="section">
        <h2>System Resources</h2>
    """
        
        if self.gpu_profiler and self.gpu_profiler.get_gpu_stats().get('available'):
            html_content += """
            <h3>GPU Statistics</h3>
            <table>
                <tr>
                    <th>Metric</th>
                    <th>Value</th>
                </tr>
            """
            gpu_stats = self.gpu_profiler.get_gpu_stats()
            for key, value in gpu_stats.items():
                if key != 'available':
                    html_content += f"""
                    <tr>
                        <td>{key.replace('_', ' ').title()}</td>
                        <td>{value:.2f}</td>
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
            
        print(f"HTML performance report generated: {output_path}")
        
    def _generate_json_report(self, output_path: str):
        """Generate JSON performance report."""
        report_data = {
            'timestamp': datetime.now().isoformat(),
            'profiles': self.results,
            'function_profiles': self.function_profiles,
            'system_info': {
                'cpu_count': psutil.cpu_count(),
                'memory_total': psutil.virtual_memory().total / 1024 / 1024 / 1024,
                'platform': sys.platform
            }
        }
        
        with open(output_path, 'w') as f:
            json.dump(report_data, f, indent=2, default=str)
            
        print(f"JSON performance report generated: {output_path}")
        
    def _generate_markdown_report(self, output_path: str):
        """Generate Markdown performance report."""
        md_content = f"""# NLM Performance Profile Report

Generated: {datetime.now().isoformat()}

## Overview

- **Total Profiles**: {len(self.results)}
- **Total Duration**: {sum(r.get('duration', 0) for r in self.results.values()):.3f}s
- **Peak Memory**: {self.results.get('simulation', {}).get('memory_peak', 0):.1f} MB
- **Max CPU Usage**: {self.results.get('simulation', {}).get('cpu_stats', {}).get('max_cpu_percent', 0):.1f}%

## Detailed Results

| Profile Label | Duration (s) | Memory Peak (MB) | CPU Usage (%) | GPU Utilization (%) |
|---------------|--------------|------------------|---------------|----------------------|
"""
        
        for label, result in self.results.items():
            cpu_max = result.get('cpu_stats', {}).get('max_cpu_percent', 0) if 'cpu_stats' in result else 0
            gpu_util = result.get('gpu_stats', {}).get('gpu_utilization', 0) if 'gpu_stats' in result and result['gpu_stats'].get('available') else 0
            
            md_content += f"""| {label} | {result.get('duration', 0):.3f} | {result.get('memory_peak', 0):.1f} | {cpu_max:.1f} | {gpu_util:.1f} |
"""
            
        md_content += """
## Function Profiles

| Function Name | Duration (s) | Module | CPU Usage (%) | Memory Delta (MB) |
|---------------|--------------|--------|---------------|--------------------|
"""
        
        for func_name, func_data in self.function_profiles.items():
            md_content += f"""| {func_name} | {func_data.get('duration', 0):.3f} | {func_data.get('module', '')} | {func_data.get('avg_cpu_percent', 0):.1f} | {func_data.get('memory_diff', {}).get('total_size', 0):.1f} |
"""
            
        with open(output_path, 'w') as f:
            f.write(md_content)
            
        print(f"Markdown performance report generated: {output_path}")


def profile_brain_simulation(simulation_func: Callable, *args, **kwargs) -> Dict[str, Any]:
    """Convenience function to profile brain simulations.
    
    Args:
        simulation_func: Brain simulation function to profile
        *args: Simulation arguments
        **kwargs: Simulation keyword arguments
        
    Returns:
        Dictionary with profiling results
    """
    profiler = PerformanceProfiler()
    return profiler.profile_simulation(simulation_func, *args, **kwargs)


def profile_function(func: Callable, *args, **kwargs) -> Dict[str, Any]:
    """Convenience function to profile a single function.
    
    Args:
        func: Function to profile
        *args: Function arguments
        **kwargs: Function keyword arguments
        
    Returns:
        Dictionary with profiling results
    """
    profiler = PerformanceProfiler()
    return profiler.profile_function(func, *args, **kwargs)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Profile NLM brain simulations")
    parser.add_argument("--target", help="Target function or simulation to profile")
    parser.add_argument("--iterations", type=int, default=1, help="Number of iterations")
    parser.add_argument("--output", help="Output report file path")
    parser.add_argument("--format", choices=["html", "json", "markdown"], default="html", help="Report format")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    parser.add_argument("--memory", action="store_true", help="Enable memory tracking")
    parser.add_argument("--cpu", action="store_true", help="Enable CPU monitoring")
    parser.add_argument("--gpu", action="store_true", help="Enable GPU monitoring")
    parser.add_argument("--line-profiler", action="store_true", help="Enable line-by-line profiling")
    
    args = parser.parse_args()
    
    print("Starting NLM Performance Profiling...")
    print(f"Target: {args.target}")
    print(f"Iterations: {args.iterations}")
    
    profiler = PerformanceProfiler(
        enable_memory_tracking=args.memory,
        enable_cpu_monitoring=args.cpu,
        enable_gpu_monitoring=args.gpu,
        enable_line_profiling=args.line_profiler
    )
    
    if args.target:
        # Import target function
        try:
            target_module, target_func_name = args.target.rsplit('.', 1)
            import importlib
            target_module = importlib.import_module(target_module)
            target_func = getattr(target_module, target_func_name)
            
            for i in range(args.iterations):
                print(f"Iteration {i + 1}/{args.iterations}")
                profiler.profile_function(target_func)
                
        except ImportError as e:
            print(f"Error importing target: {e}")
            sys.exit(1)
        except AttributeError as e:
            print(f"Error finding function: {e}")
            sys.exit(1)
    
    # Generate report
    output_path = profiler.generate_report(args.output, args.format)
    print(f"Performance profiling complete. Report saved to: {output_path}")