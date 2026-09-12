"""Advanced performance profiling tools for NLM simulation metrics."""

import time
import psutil
import tracemalloc
import pandas as pd
import numpy as np
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, asdict
from datetime import datetime
import json
import os
from pathlib import Path

from pynlm import Brain, SimulationStep, Timestamp

@dataclass
class StepMetrics:
    """Metrics collected for a single simulation step."""
    step: int
    timestamp: float
    cpu_percent: float
    memory_mb: float
    step_time_ms: float
    neurons_firing: int
    total_spikes: int
    average_firing_rate: float
    e_i_ratio: float
    memory_allocated_mb: float
    memory_peak_mb: float
    thread_count: int
    context_switches: int
    disk_io_read: int
    disk_io_write: int
    active_threads: List[str]

class PerformanceProfiler:
    """Performance profiling tools for NLM simulation metrics.
    
    Provides comprehensive profiling of simulation steps including CPU usage,
    memory consumption, and timing metrics. Collects detailed metrics for
    performance analysis and optimization.
    
    Attributes:
        brain: Optional brain instance being profiled
        profiling_enabled: Whether profiling is currently active
        metrics_history: List of collected step metrics
        start_time: Overall profiling start time
        baseline_metrics: Baseline system metrics
    """
    
    def __init__(self, brain: Optional[Brain] = None):
        """Initialize performance profiler.
        
        Args:
            brain: Optional brain instance to profile
        """
        self.brain = brain
        self.profiling_enabled = False
        self.metrics_history: List[StepMetrics] = []
        self.start_time = time.time()
        self.baseline_metrics = self._get_system_baseline()
        
        # Initialize memory tracking
        tracemalloc.start()
    
    def _get_system_baseline(self) -> Dict[str, float]:
        """Get baseline system metrics for normalization."""
        process = psutil.Process()
        return {
            'cpu_percent': process.cpu_percent(),
            'memory_mb': process.memory_info().rss / (1024 * 1024),
            'thread_count': process.num_threads(),
            'disk_io_read': process.io_counters().read_bytes,
            'disk_io_write': process.io_counters().write_bytes
        }
    
    def start_step(self, step: Optional[int] = None) -> None:
        """Start timing and collecting metrics for a simulation step.
        
        Args:
            step: Step number (optional, will be auto-incremented if not provided)
        """
        self.profiling_enabled = True
        self._step_start_time = time.perf_counter()
        self._step_start_cpu = psutil.cpu_percent(interval=None)
        self._step_start_memory = psutil.Process().memory_info().rss / (1024 * 1024)
        
        # Start memory tracking for this step
        self._current_snapshot = tracemalloc.take_snapshot()
        
        if step is not None:
            self._current_step = step
        elif hasattr(self, '_current_step'):
            self._current_step += 1
        else:
            self._current_step = 0
    
    def end_step(self) -> StepMetrics:
        """End timing and collect metrics for the current step.
        
        Returns:
            StepMetrics: Metrics collected for the step
            
        Raises:
            RuntimeError: If step was not started
        """
        if not self.profiling_enabled:
            raise RuntimeError("Step was not started. Call start_step() first.")
        
        step_end_time = time.perf_counter()
        step_duration = (step_end_time - self._step_start_time) * 1000  # ms
        
        # Get current system metrics
        process = psutil.Process()
        current_cpu = psutil.cpu_percent(interval=None)
        current_memory = process.memory_info().rss / (1024 * 1024)
        
        # Calculate step-specific metrics
        step_metrics = StepMetrics(
            step=self._current_step,
            timestamp=time.time() - self.start_time,
            cpu_percent=current_cpu,
            memory_mb=current_memory,
            step_time_ms=step_duration,
            neurons_firing=self.brain.getFiringNeuronCount() if self.brain else 0,
            total_spikes=self.brain.getTotalSpikeCount() if self.brain else 0,
            average_firing_rate=self.brain.getAverageFiringRate() if self.brain else 0.0,
            e_i_ratio=self.brain.getExcitationInhibitionRatio() if self.brain else 0.0,
            memory_allocated_mb=0.0,  # Will be calculated from memory tracking
            memory_peak_mb=0.0,
            thread_count=process.num_threads(),
            context_switches=process.num_ctx_switches(),
            disk_io_read=process.io_counters().read_bytes - self.baseline_metrics['disk_io_read'],
            disk_io_write=process.io_counters().write_bytes - self.baseline_metrics['disk_io_write'],
            active_threads=[]  # Would need thread introspection
        )
        
        # Calculate memory metrics using tracemalloc
        current_snapshot = tracemalloc.take_snapshot()
        step_snapshot = current_snapshot.compare_to(self._current_snapshot, 'lineno')
        
        total_allocated = sum(stat.size_diff for stat in step_snapshot if stat.size_diff > 0)
        total_peak = sum(stat.size for stat in step_snapshot if stat.size > 0)
        
        step_metrics.memory_allocated_mb = total_allocated / (1024 * 1024)
        step_metrics.memory_peak_mb = total_peak / (1024 * 1024)
        
        # Add to history
        self.metrics_history.append(step_metrics)
        
        # Reset profiling state
        self.profiling_enabled = False
        
        return step_metrics
    
    def get_step_metrics(self, step: int) -> Optional[StepMetrics]:
        """Get metrics for a specific step.
        
        Args:
            step: Step number
            
        Returns:
            StepMetrics: Metrics for the step, or None if not found
        """
        for metrics in self.metrics_history:
            if metrics.step == step:
                return metrics
        return None
    
    def get_summary_stats(self) -> Dict[str, Any]:
        """Get summary statistics for all profiled steps.
        
        Returns:
            Dict[str, Any]: Summary statistics
        """
        if not self.metrics_history:
            return {}
        
        # Convert to DataFrame for easy analysis
        df = pd.DataFrame([asdict(m) for m in self.metrics_history])
        
        summary = {
            'total_steps': len(self.metrics_history),
            'total_duration_ms': (df['step_time_ms'].sum()),
            'average_step_time_ms': df['step_time_ms'].mean(),
            'median_step_time_ms': df['step_time_ms'].median(),
            'min_step_time_ms': df['step_time_ms'].min(),
            'max_step_time_ms': df['step_time_ms'].max(),
            'std_step_time_ms': df['step_time_ms'].std(),
            'cpu_avg_percent': df['cpu_percent'].mean(),
            'cpu_max_percent': df['cpu_percent'].max(),
            'memory_avg_mb': df['memory_mb'].mean(),
            'memory_max_mb': df['memory_mb'].max(),
            'memory_allocated_total_mb': df['memory_allocated_mb'].sum(),
            'memory_peak_total_mb': df['memory_peak_mb'].sum(),
            'neurons_firing_avg': df['neurons_firing'].mean(),
            'neurons_firing_max': df['neurons_firing'].max(),
            'total_spikes': df['total_spikes'].sum(),
            'average_firing_rate': df['average_firing_rate'].mean(),
            'e_i_ratio_avg': df['e_i_ratio'].mean(),
            'throughput_steps_per_sec': len(self.metrics_history) / (df['step_time_ms'].sum() / 1000),
            'efficiency_percent': (len(self.metrics_history) / (df['step_time_ms'].sum() / 1000)) * 100
        }
        
        return summary
    
    def generate_report(self, filename: Optional[str] = None) -> str:
        """Generate HTML performance report.
        
        Args:
            filename: Output file path (optional)
            
        Returns:
            str: HTML report content
        """
        summary = self.get_summary_stats()
        
        html_report = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .metrics {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin: 20px 0; }}
        .metric-card {{ background-color: #f9f9f9; padding: 15px; border-radius: 5px; border-left: 4px solid #3498db; }}
        .metric-value {{ font-size: 24px; font-weight: bold; color: #2c3e50; }}
        .metric-label {{ color: #7f8c8d; font-size: 14px; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background-color: #f2f2f2; }}
        .plot {{ margin: 20px 0; text-align: center; }}
        .step-metrics {{ margin: 10px 0; padding: 10px; background-color: #f8f9fa; border-radius: 3px; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Report</h1>
        <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        <p>Total Steps Profiled: {summary.get('total_steps', 0)}</p>
        <p>Total Duration: {summary.get('total_duration_ms', 0):.2f} ms</p>
    </div>
    
    <h2>Summary Statistics</h2>
    <div class="metrics">
        <div class="metric-card">
            <div class="metric-value">{summary.get('throughput_steps_per_sec', 0):.2f}</div>
            <div class="metric-label">Steps per Second</div>
        </div>
        <div class="metric-card">
            <div class="metric-value">{summary.get('average_step_time_ms', 0):.2f} ms</div>
            <div class="metric-label">Average Step Time</div>
        </div>
        <div class="metric-card">
            <div class="metric-value">{summary.get('cpu_avg_percent', 0):.1f}%</div>
            <div class="metric-label">Average CPU Usage</div>
        </div>
        <div class="metric-card">
            <div class="metric-value">{summary.get('memory_avg_mb', 0):.1f} MB</div>
            <div class="metric-label">Average Memory Usage</div>
        </div>
        <div class="metric-card">
            <div class="metric-value">{summary.get('efficiency_percent', 0):.1f}%</div>
            <div class="metric-label">Efficiency</div>
        </div>
    </div>
    
    <h2>Detailed Metrics</h2>
    <table>
        <tr>
            <th>Metric</th>
            <th>Value</th>
            <th>Unit</th>
        </tr>
        <tr>
            <td>Total Steps</td>
            <td>{summary.get('total_steps', 0)}</td>
            <td>steps</td>
        </tr>
        <tr>
            <td>Total Duration</td>
            <td>{summary.get('total_duration_ms', 0):.2f}</td>
            <td>ms</td>
        </tr>
        <tr>
            <td>Average Step Time</td>
            <td>{summary.get('average_step_time_ms', 0):.2f}</td>
            <td>ms</td>
        </tr>
        <tr>
            <td>Median Step Time</td>
            <td>{summary.get('median_step_time_ms', 0):.2f}</td>
            <td>ms</td>
        </tr>
        <tr>
            <td>Max Step Time</td>
            <td>{summary.get('max_step_time_ms', 0):.2f}</td>
            <td>ms</td>
        </tr>
        <tr>
            <td>Min Step Time</td>
            <td>{summary.get('min_step_time_ms', 0):.2f}</td>
            <td>ms</td>
        </tr>
        <tr>
            <td>CPU Average</td>
            <td>{summary.get('cpu_avg_percent', 0):.1f}</td>
            <td>%</td>
        </tr>
        <tr>
            <td>CPU Max</td>
            <td>{summary.get('cpu_max_percent', 0):.1f}</td>
            <td>%</td>
        </tr>
        <tr>
            <td>Memory Average</td>
            <td>{summary.get('memory_avg_mb', 0):.1f}</td>
            <td>MB</td>
        </tr>
        <tr>
            <td>Memory Max</td>
            <td>{summary.get('memory_max_mb', 0):.1f}</td>
            <td>MB</td>
        </tr>
        <tr>
            <td>Total Memory Allocated</td>
            <td>{summary.get('memory_allocated_total_mb', 0):.1f}</td>
            <td>MB</td>
        </tr>
        <tr>
            <td>Total Memory Peak</td>
            <td>{summary.get('memory_peak_total_mb', 0):.1f}</td>
            <td>MB</td>
        </tr>
        <tr>
            <td>Neurons Firing Average</td>
            <td>{summary.get('neurons_firing_avg', 0):.1f}</td>
            <td>neurons</td>
        </tr>
        <tr>
            <td>Neurons Firing Max</td>
            <td>{summary.get('neurons_firing_max', 0):.1f}</td>
            <td>neurons</td>
        </tr>
        <tr>
            <td>Total Spikes</td>
            <td>{summary.get('total_spikes', 0)}</td>
            <td>spikes</td>
        </tr>
        <tr>
            <td>Average Firing Rate</td>
            <td>{summary.get('average_firing_rate', 0):.2f}</td>
            <td>Hz</td>
        </tr>
        <tr>
            <td>E/I Ratio Average</td>
            <td>{summary.get('e_i_ratio_avg', 0):.3f}</td>
            <td>ratio</td>
        </tr>
    </table>
    
    <h2>Step-by-Step Metrics</h2>
"""
        
        # Add step-by-step data in a table
        html_report += """
    <table>
        <tr>
            <th>Step</th>
            <th>Time (ms)</th>
            <th>CPU (%)</th>
            <th>Memory (MB)</th>
            <th>Neurons Firing</th>
            <th>Spikes</th>
            <th>Firing Rate (Hz)</th>
            <th>E/I Ratio</th>
            <th>Memory Alloc (MB)</th>
        </tr>
        """
        
        for metrics in self.metrics_history[-50:]:  # Show last 50 steps
            html_report += f"""
        <tr>
            <td>{metrics.step}</td>
            <td>{metrics.step_time_ms:.2f}</td>
            <td>{metrics.cpu_percent:.1f}</td>
            <td>{metrics.memory_mb:.1f}</td>
            <td>{metrics.neurons_firing}</td>
            <td>{metrics.total_spikes}</td>
            <td>{metrics.average_firing_rate:.2f}</td>
            <td>{metrics.e_i_ratio:.3f}</td>
            <td>{metrics.memory_allocated_mb:.2f}</td>
        </tr>
        """
        
        html_report += """
    </table>
    
    <h2>Performance Analysis</h2>
    <p>Performance profiling completed with {len(self.metrics_history)} steps analyzed.</p>
    <p>Average step time: {summary.get('average_step_time_ms', 0):.2f} ms</p>
    <p>Throughput: {summary.get('throughput_steps_per_sec', 0):.2f} steps/second</p>
    <p>Efficiency: {summary.get('efficiency_percent', 0):.1f}%</p>
    
    </body>
    </html>
        """
        
        if filename:
            Path(filename).parent.mkdir(parents=True, exist_ok=True)
            with open(filename, 'w') as f:
                f.write(html_report)
        
        return html_report
    
    def save_metrics(self, filename: str) -> None:
        """Save performance metrics to file.
        
        Args:
            filename: Output file path (JSON format)
        """
        metrics_data = [asdict(m) for m in self.metrics_history]
        
        with open(filename, 'w') as f:
            json.dump(metrics_data, f, indent=2)
    
    def load_metrics(self, filename: str) -> None:
        """Load performance metrics from file.
        
        Args:
            filename: Input file path (JSON format)
        """
        with open(filename, 'r') as f:
            metrics_data = json.load(f)
        
        # Clear existing metrics
        self.metrics_history.clear()
        
        # Load metrics
        for data in metrics_data:
            metrics = StepMetrics(**data)
            self.metrics_history.append(metrics)
        
        # Update baseline
        self.baseline_metrics = self._get_system_baseline()
    
    def close(self) -> None:
        """Close profiler and release resources."""
        self.profiling_enabled = False
        tracemalloc.stop()
        self.metrics_history.clear()


# Convenience function
def create_performance_profiler(brain: Optional[Brain] = None) -> PerformanceProfiler:
    """Create a performance profiler.
    
    Args:
        brain: Optional brain instance to profile
        
    Returns:
        PerformanceProfiler: Profiler instance
    """
    return PerformanceProfiler(brain)
