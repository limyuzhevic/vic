"""
Performance Monitoring and Profiling for NLM

This module provides comprehensive performance monitoring and profiling capabilities
for NLM simulations. It includes real-time metrics collection, bottleneck identification,
and optimization suggestions.
"""

import time
import psutil
import threading
import pandas as pd
import numpy as np
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass, field
from enum import Enum
import matplotlib.pyplot as plt
from datetime import datetime
import json
import os

@dataclass
class PerformanceMetrics:
    """Data structure for performance metrics collection."""
    
    timestamp: float
    cpu_usage: float
    memory_usage: float
    memory_available: float
    disk_io_read: float
    disk_io_write: float
    network_io_sent: float
    network_io_recv: float
    simulation_steps: int
    spike_count: int
    active_neurons: int
    firing_rate: float
    synaptic_operations: int
    plasticity_updates: int

class PerformanceProfiler:
    """
    Comprehensive performance profiler for NLM simulations.
    
    This profiler collects detailed performance metrics and provides analysis
    for optimization and bottleneck identification.
    """
    
    def __init__(self, simulation_brain: Any = None):
        self.simulation_brain = simulation_brain
        self.metrics_history: List[PerformanceMetrics] = []
        self.profiling_active = False
        self.profiling_thread = None
        self.sampling_interval = 0.1  # seconds
        self.start_time = time.time()
        self.total_steps = 0
        self.total_spikes = 0
        self.synaptic_ops = 0
        self.plasticity_updates = 0
        
    def start_profiling(self, sampling_interval: float = 0.1):
        """
        Start continuous performance profiling.
        
        Args:
            sampling_interval: Interval between metric collections (seconds)
        """
        self.sampling_interval = sampling_interval
        self.profiling_active = True
        self.start_time = time.time()
        self.metrics_history.clear()
        
        self.profiling_thread = threading.Thread(target=self._collect_metrics)
        self.profiling_thread.daemon = True
        self.profiling_thread.start()
        
    def stop_profiling(self) -> Dict[str, Any]:
        """
        Stop performance profiling and return analysis results.
        
        Returns:
            Dictionary containing profiling analysis results
        """
        self.profiling_active = False
        if self.profiling_thread:
            self.profiling_thread.join(timeout=1.0)
            self.profiling_thread = None
        
        return self._analyze_performance()
    
    def _collect_metrics(self):
        """Continuous metrics collection in background thread."""
        while self.profiling_active:
            metrics = self._collect_current_metrics()
            if metrics:
                self.metrics_history.append(metrics)
            time.sleep(self.sampling_interval)
    
    def _collect_current_metrics(self) -> Optional[PerformanceMetrics]:
        """Collect current performance metrics."""
        if not self.simulation_brain:
            return None
            
        current_time = time.time()
        process = psutil.Process()
        
        # Get system metrics
        cpu_percent = process.cpu_percent()
        memory_info = process.memory_info()
        memory_percent = process.memory_percent()
        memory_available = process.memory_info().available
        
        # Get disk I/O
        disk_io = process.io_counters() if hasattr(process, 'io_counters') else None
        disk_read = disk_io.read_bytes if disk_io else 0
        disk_write = disk_io.write_bytes if disk_io else 0
        
        # Get network I/O
        network_io = process.connections() if hasattr(process, 'connections') else None
        network_sent = sum(conn.status for conn in network_io) if network_io else 0
        network_recv = 0  # Simplified for now
        
        # Get brain-specific metrics
        steps = getattr(self.simulation_brain, 'getTotalSpikeCount', lambda: 0)()
        spike_count = getattr(self.simulation_brain, 'getTotalSpikeCount', lambda: 0)()
        active_neurons = getattr(self.simulation_brain, 'getActiveNeuronCount', lambda: 0)()
        firing_rate = getattr(self.simulation_brain, 'getAverageFiringRate', lambda: 0)()
        
        return PerformanceMetrics(
            timestamp=current_time,
            cpu_usage=cpu_percent,
            memory_usage=memory_percent,
            memory_available=memory_available,
            disk_io_read=disk_read,
            disk_io_write=disk_write,
            network_io_sent=network_sent,
            network_io_recv=network_recv,
            simulation_steps=steps,
            spike_count=spike_count,
            active_neurons=active_neurons,
            firing_rate=firing_rate,
            synaptic_operations=self.synaptic_ops,
            plasticity_updates=self.plasticity_updates
        )
    
    def record_synaptic_operation(self, count: int = 1):
        """Record synaptic operations for performance tracking."""
        self.synaptic_ops += count
    
    def record_plasticity_update(self, count: int = 1):
        """Record plasticity updates for performance tracking."""
        self.plasticity_updates += count
    
    def _analyze_performance(self) -> Dict[str, Any]:
        """Analyze collected performance metrics."""
        if not self.metrics_history:
            return {"error": "No metrics collected"}
        
        metrics_df = pd.DataFrame([vars(m) for m in self.metrics_history])
        
        analysis = {
            "duration": time.time() - self.start_time,
            "total_steps": self.total_steps,
            "total_spikes": self.total_spikes,
            "average_cpu_usage": metrics_df['cpu_usage'].mean(),
            "peak_cpu_usage": metrics_df['cpu_usage'].max(),
            "average_memory_usage": metrics_df['memory_usage'].mean(),
            "peak_memory_usage": metrics_df['memory_usage'].max(),
            "spike_rate": self.total_spikes / (len(self.metrics_history) * self.sampling_interval) if self.metrics_history else 0,
            "efficiency_ratio": self.total_spikes / max(1, self.synaptic_ops),
            "plasticity_overhead": self.plasticity_updates / max(1, self.total_steps),
            "system_stability": self._calculate_stability(metrics_df),
            "bottleneck_analysis": self._identify_bottlenecks(metrics_df),
            "optimization_recommendations": self._generate_recommendations(metrics_df)
        }
        
        return analysis
    
    def _calculate_stability(self, metrics_df: pd.DataFrame) -> Dict[str, float]:
        """Calculate system stability metrics."""
        cpu_stability = 1.0 / (1.0 + metrics_df['cpu_usage'].std())
        memory_stability = 1.0 / (1.0 + metrics_df['memory_usage'].std())
        
        return {
            "cpu_stability": cpu_stability,
            "memory_stability": memory_stability,
            "overall_stability": (cpu_stability + memory_stability) / 2
        }
    
    def _identify_bottlenecks(self, metrics_df: pd.DataFrame) -> Dict[str, Any]:
        """Identify potential performance bottlenecks."""
        bottlenecks = {}
        
        # CPU bottleneck
        if metrics_df['cpu_usage'].mean() > 80:
            bottlenecks['cpu'] = {
                "severity": "high" if metrics_df['cpu_usage'].max() > 90 else "medium",
                "recommendation": "Consider reducing computation complexity or increasing cores"
            }
        
        # Memory bottleneck
        if metrics_df['memory_usage'].mean() > 85:
            bottlenecks['memory'] = {
                "severity": "high" if metrics_df['memory_usage'].max() > 95 else "medium",
                "recommendation": "Optimize memory usage or increase available memory"
            }
        
        # I/O bottleneck
        if len(metrics_df) > 10:
            io_variance = metrics_df['disk_io_read'].std() / metrics_df['disk_io_read'].mean()
            if io_variance > 2:
                bottlenecks['io'] = {
                    "severity": "medium",
                    "recommendation": "Consider caching frequently accessed data"
                }
        
        return bottlenecks
    
    def _generate_recommendations(self, metrics_df: pd.DataFrame) -> List[str]:
        """Generate optimization recommendations."""
        recommendations = []
        
        avg_cpu = metrics_df['cpu_usage'].mean()
        avg_memory = metrics_df['memory_usage'].mean()
        
        if avg_cpu > 75:
            recommendations.append("CPU usage is high. Consider parallelizing computations or reducing simulation complexity.")
        
        if avg_memory > 80:
            recommendations.append("Memory usage is high. Consider optimizing data structures or reducing memory footprint.")
        
        if self.total_steps > 0:
            efficiency = self.total_spikes / max(1, self.total_steps)
            if efficiency < 10:
                recommendations.append("Low synaptic efficiency. Consider optimizing neuron connectivity patterns.")
        
        if not recommendations:
            recommendations.append("Performance is within acceptable ranges. Continue monitoring for optimization opportunities.")
        
        return recommendations
    
    def export_metrics(self, filepath: str):
        """
        Export collected metrics to file.
        
        Args:
            filepath: Path to export file (CSV or JSON)
        """
        if filepath.endswith('.csv'):
            df = pd.DataFrame([vars(m) for m in self.metrics_history])
            df.to_csv(filepath, index=False)
        elif filepath.endswith('.json'):
            data = [vars(m) for m in self.metrics_history]
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
        else:
            raise ValueError("Unsupported file format. Use .csv or .json")
    
    def import_metrics(self, filepath: str):
        """
        Import metrics from file.
        
        Args:
            filepath: Path to import file (CSV or JSON)
        """
        if filepath.endswith('.csv'):
            df = pd.read_csv(filepath)
            self.metrics_history = [PerformanceMetrics(**row) for _, row in df.iterrows()]
        elif filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                data = json.load(f)
            self.metrics_history = [PerformanceMetrics(**row) for row in data]
        else:
            raise ValueError("Unsupported file format. Use .csv or .json")
    
    def generate_performance_report(self, output_path: str):
        """
        Generate comprehensive performance report.
        
        Args:
            output_path: Path to output report file
        """
        analysis = self._analyze_performance()
        
        report = {
            "timestamp": datetime.now().isoformat(),
            "analysis": analysis,
            "metrics_summary": {
                "samples_collected": len(self.metrics_history),
                "duration_seconds": analysis["duration"],
                "data_quality": self._assess_data_quality()
            }
        }
        
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        # Also create human-readable summary
        self._create_human_readable_report(report, output_path.replace('.json', '_readable.txt'))
    
    def _assess_data_quality(self) -> Dict[str, Any]:
        """Assess quality of collected metrics."""
        if not self.metrics_history:
            return {"status": "no_data", "score": 0.0}
        
        # Check for missing values
        missing_values = sum(1 for m in self.metrics_history 
                           for key, value in vars(m).items() if value is None)
        
        # Check sampling consistency
        time_diffs = []
        for i in range(1, len(self.metrics_history)):
            time_diffs.append(self.metrics_history[i].timestamp - self.metrics_history[i-1].timestamp)
        
        avg_sampling = np.mean(time_diffs) if time_diffs else 0
        
        quality_score = max(0, 100 - (missing_values * 10) - (abs(avg_sampling - self.sampling_interval) * 100))
        
        return {
            "status": "good" if quality_score > 80 else "fair" if quality_score > 60 else "poor",
            "score": quality_score,
            "missing_values": missing_values,
            "sampling_consistency": abs(avg_sampling - self.sampling_interval) / self.sampling_interval if avg_sampling > 0 else 0
        }
    
    def _create_human_readable_report(self, analysis: Dict[str, Any], filepath: str):
        """Create human-readable performance report."""
        with open(filepath, 'w') as f:
            f.write("NLM Performance Analysis Report\n")
            f.write("=" * 50 + "\n\n")
            
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"Analysis Duration: {analysis['duration']:.2f} seconds\n\n")
            
            f.write("Performance Metrics:\n")
            f.write(f"- Average CPU Usage: {analysis['average_cpu_usage']:.1f}%\n")
            f.write(f"- Peak CPU Usage: {analysis['peak_cpu_usage']:.1f}%\n")
            f.write(f"- Average Memory Usage: {analysis['average_memory_usage']:.1f}%\n")
            f.write(f"- Peak Memory Usage: {analysis['peak_memory_usage']:.1f}%\n")
            f.write(f"- Spike Rate: {analysis['spike_rate']:.2f} spikes/second\n")
            f.write(f"- Efficiency Ratio: {analysis['efficiency_ratio']:.4f} spikes/synaptic operation\n\n")
            
            f.write("System Stability:\n")
            stability = analysis['system_stability']
            f.write(f"- CPU Stability: {stability['cpu_stability']:.3f}\n")
            f.write(f"- Memory Stability: {stability['memory_stability']:.3f}\n")
            f.write(f"- Overall Stability: {stability['overall_stability']:.3f}\n\n")
            
            f.write("Bottleneck Analysis:\n")
            for bottleneck, info in analysis['bottleneck_analysis'].items():
                f.write(f"- {bottleneck.upper()}: {info['severity']} - {info['recommendation']}\n")
            
            f.write("\nOptimization Recommendations:\n")
            for i, recommendation in enumerate(analysis['optimization_recommendations'], 1):
                f.write(f"{i}. {recommendation}\n")


class RealTimeMonitor:
    """
    Real-time monitoring interface for NLM simulations.
    
    Provides live visualization and monitoring of simulation metrics
    during runtime.
    """
    
    def __init__(self, profiler: PerformanceProfiler):
        self.profiler = profiler
        self.monitor_active = False
        self.monitor_thread = None
        self.update_callbacks: List[Callable] = []
        
    def start_monitoring(self, interval: float = 1.0):
        """
        Start real-time monitoring.
        
        Args:
            interval: Update interval in seconds
        """
        self.monitor_active = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(interval,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.monitor_active = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
    
    def add_update_callback(self, callback: Callable):
        """
        Add a callback function for real-time updates.
        
        Args:
            callback: Function to call with monitoring data
        """
        self.update_callbacks.append(callback)
    
    def _monitor_loop(self, interval: float):
        """Main monitoring loop."""
        while self.monitor_active:
            # Get current metrics
            latest_metrics = None
            if self.profiler.metrics_history:
                latest_metrics = self.profiler.metrics_history[-1]
            
            # Call all callbacks
            for callback in self.update_callbacks:
                try:
                    callback(latest_metrics)
                except Exception as e:
                    print(f"Monitoring callback error: {e}")
            
            time.sleep(interval)
    
    def create_dashboard(self, width: int = 1200, height: int = 800) -> 'Dashboard':
        """
        Create a dashboard for real-time visualization.
        
        Args:
            width: Dashboard width
            height: Dashboard height
            
        Returns:
            Dashboard object for visualization
        """
        return Dashboard(self.profiler, width, height)

class Dashboard:
    """
    Real-time dashboard for NLM monitoring and visualization.
    
    Provides interactive visualization of simulation metrics and performance data.
    """
    
    def __init__(self, profiler: PerformanceProfiler, width: int = 1200, height: int = 800):
        self.profiler = profiler
        self.width = width
        self.height = height
        self.fig = None
        self.axes = {}
        
    def create_simple_dashboard(self) -> Any:
        """
        Create a simple dashboard with basic metrics.
        
        Returns:
            Matplotlib figure object
        """
        self.fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(self.width/100, self.height/100))
        
        # CPU usage plot
        self.axes['cpu'] = ax1
        ax1.set_title('CPU Usage (%)')
        ax1.set_xlabel('Time')
        ax1.set_ylabel('CPU %')
        
        # Memory usage plot
        self.axes['memory'] = ax2
        ax2.set_title('Memory Usage (%)')
        ax2.set_xlabel('Time')
        ax2.set_ylabel('Memory %')
        
        # Spike rate plot
        self.axes['spikes'] = ax3
        ax3.set_title('Spike Rate')
        ax3.set_xlabel('Time')
        ax3.set_ylabel('Spikes/sec')
        
        # System stability plot
        self.axes['stability'] = ax4
        ax4.set_title('System Stability')
        ax4.set_xlabel('Time')
        ax4.set_ylabel('Stability')
        
        return self.fig
    
    def update_dashboard(self):
        """Update dashboard with latest metrics."""
        if not self.fig or not self.axes:
            return
        
        if not self.profiler.metrics_history:
            return
        
        timestamps = [m.timestamp - self.profiler.start_time for m in self.profiler.metrics_history]
        
        # Update CPU plot
        self.axes['cpu'].clear()
        self.axes['cpu'].plot(timestamps, [m.cpu_usage for m in self.profiler.metrics_history])
        self.axes['cpu'].set_title('CPU Usage (%)')
        self.axes['cpu'].set_xlabel('Time (s)')
        self.axes['cpu'].set_ylabel('CPU %')
        self.axes['cpu'].grid(True)
        
        # Update memory plot
        self.axes['memory'].clear()
        self.axes['memory'].plot(timestamps, [m.memory_usage for m in self.profiler.metrics_history])
        self.axes['memory'].set_title('Memory Usage (%)')
        self.axes['memory'].set_xlabel('Time (s)')
        self.axes['memory'].set_ylabel('Memory %')
        self.axes['memory'].grid(True)
        
        # Update spike rate plot
        if len(self.profiler.metrics_history) > 1:
            spike_rates = [m.spike_count / (m.timestamp - self.profiler.metrics_history[0].timestamp) 
                         for m in self.profiler.metrics_history]
            self.axes['spikes'].clear()
            self.axes['spikes'].plot(timestamps, spike_rates)
            self.axes['spikes'].set_title('Spike Rate')
            self.axes['spikes'].set_xlabel('Time (s)')
            self.axes['spikes'].set_ylabel('Spikes/sec')
            self.axes['spikes'].grid(True)
        
        # Update stability plot
        if len(self.profiler.metrics_history) > 1:
            cpu_stability = [1.0 / (1.0 + m.cpu_usage) for m in self.profiler.metrics_history]
            memory_stability = [1.0 / (1.0 + m.memory_usage) for m in self.profiler.metrics_history]
            avg_stability = [(c + m) / 2 for c, m in zip(cpu_stability, memory_stability)]
            
            self.axes['stability'].clear()
            self.axes['stability'].plot(timestamps, avg_stability)
            self.axes['stability'].set_title('System Stability')
            self.axes['stability'].set_xlabel('Time (s)')
            self.axes['stability'].set_ylabel('Stability')
            self.axes['stability'].grid(True)
        
        plt.tight_layout()
        plt.pause(0.1)
    
    def save_dashboard(self, filepath: str):
        """
        Save dashboard to file.
        
        Args:
            filepath: Path to save dashboard
        """
        if self.fig:
            self.fig.savefig(filepath, dpi=150, bbox_inches='tight')
            plt.close(self.fig)
            self.fig = None
            self.axes = {}

if __name__ == "__main__":
    # Example usage
    print("Initializing NLM Performance Profiler...")
    
    # Create profiler
    profiler = PerformanceProfiler()
    
    # Start profiling
    print("Starting performance profiling...")
    profiler.start_profiling(0.5)
    
    # Simulate some work (you would run your actual simulation here)
    for i in range(100):
        time.sleep(0.1)
        profiler.record_synaptic_operation(i % 10)
        profiler.record_plasticity_update(i % 5)
    
    # Stop profiling and get analysis
    print("Stopping performance profiling...")
    analysis = profiler.stop_profiling()
    
    print("\nPerformance Analysis Results:")
    print(f"Duration: {analysis['duration']:.2f} seconds")
    print(f"Average CPU Usage: {analysis['average_cpu_usage']:.1f}%")
    print(f"Peak CPU Usage: {analysis['peak_cpu_usage']:.1f}%")
    print(f"Average Memory Usage: {analysis['average_memory_usage']:.1f}%")
    print(f"Spike Rate: {analysis['spike_rate']:.2f} spikes/second")
    
    print("\nBottleneck Analysis:")
    for bottleneck, info in analysis['bottleneck_analysis'].items():
        print(f"- {bottleneck.upper()}: {info['severity']} - {info['recommendation']}")
    
    print("\nOptimization Recommendations:")
    for i, recommendation in enumerate(analysis['optimization_recommendations'], 1):
        print(f"{i}. {recommendation}")
    
    # Generate report
    profiler.generate_performance_report("nlm_performance_report.json")
    print("\nPerformance report saved to 'nlm_performance_report.json'")
    
    print("\nPerformance monitoring and profiling completed successfully!")
