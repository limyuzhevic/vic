"""
Real-time performance monitoring for NLM neural simulations.

Provides comprehensive performance monitoring including timing statistics,
memory usage tracking, CPU analysis, and bottleneck identification.
"""

import time
import psutil
import threading
import json
import os
from typing import Dict, List, Any, Optional, Callable
from datetime import datetime
import numpy as np
import pandas as pd

class PerformanceMetrics:
    """Container for performance metrics data."""
    
    def __init__(self):
        self.timestamp: float = time.time()
        self.step: int = 0
        self.real_time: float = 0.0
        self.cpu_usage: float = 0.0
        self.memory_usage: float = 0.0
        self.vram_usage: float = 0.0
        self.active_threads: int = 0
        self.neuron_count: int = 0
        self.synapse_count: int = 0
        self.spike_count: int = 0
        self.firing_rate: float = 0.0
        self.plasticity_updates: int = 0
        self.bottleneck_detected: bool = False
        self.bottleneck_type: Optional[str] = None
        
    def to_dict(self) -> Dict[str, Any]:
        return {
            'timestamp': self.timestamp,
            'step': self.step,
            'real_time': self.real_time,
            'cpu_usage': self.cpu_usage,
            'memory_usage': self.memory_usage,
            'vram_usage': self.vram_usage,
            'active_threads': self.active_threads,
            'neuron_count': self.neuron_count,
            'synapse_count': self.synapse_count,
            'spike_count': self.spike_count,
            'firing_rate': self.firing_rate,
            'plasticity_updates': self.plasticity_updates,
            'bottleneck_detected': self.bottleneck_detected,
            'bottleneck_type': self.bottleneck_type
        }
class PerformanceMonitor:
    """
    Real-time performance monitoring for NLM neural simulations.
    
    Features:
    - Real-time timing statistics
    - Memory usage tracking
    - CPU usage analysis
    - Bottleneck identification
    - Performance metrics storage and reporting
    """
    
    def __init__(self, brain=None, update_interval: float = 0.1):
        """
        Initialize performance monitor.
        
        Args:
            brain: Reference to the brain object (optional)
            update_interval: Monitoring interval in seconds
        """
        self.brain = brain
        self.update_interval = update_interval
        self.metrics_history: List[PerformanceMetrics] = []
        self.is_monitoring = False
        self.monitor_thread = None
        self.start_time = 0.0
        self.last_frame_time = 0.0
        
        # Performance thresholds for bottleneck detection
        self.cpu_threshold = 80.0  # Percentage
        self.memory_threshold = 80.0  # Percentage
        self.vram_threshold = 90.0  # Percentage
        self.update_frequency_threshold = 0.5  # seconds (max time per frame)
        
    def start_monitoring(self):
        """Start real-time performance monitoring."""
        self.is_monitoring = True
        self.start_time = time.time()
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
            
    def _monitor_loop(self):
        """Internal monitoring loop."""
        while self.is_monitoring:
            try:
                metrics = self._collect_metrics()
                self.metrics_history.append(metrics)
                
                # Keep only recent metrics (last 1000 entries)
                if len(self.metrics_history) > 1000:
                    self.metrics_history = self.metrics_history[-1000:]
                    
                time.sleep(self.update_interval)
            except Exception as e:
                print(f"Performance monitoring error: {e}")
                break
                
    def _collect_metrics(self) -> PerformanceMetrics:
        """Collect current performance metrics."""
        metrics = PerformanceMetrics()
        metrics.step = getattr(self.brain, 'getTotalSpikeCount', lambda: 0)() if self.brain else 0
        
        # System metrics
        process = psutil.Process(os.getpid())
        metrics.real_time = time.time() - self.start_time
        metrics.cpu_usage = process.cpu_percent()
        metrics.memory_usage = process.memory_percent()
        metrics.active_threads = process.num_threads()
        
        # VRAM usage if available
        try:
            import pynvl
            gpu = pynvl.get_gpu(0)
            metrics.vram_usage = (gpu.memory_used / gpu.memory_total) * 100
        except (ImportError, AttributeError):
            metrics.vram_usage = 0.0
            
        # Neural simulation metrics
        if self.brain:
            metrics.neuron_count = self.brain.getTotalNeuronCount()
            metrics.synapse_count = self.brain.getTotalSynapseCount()
            metrics.spike_count = self.brain.getTotalSpikeCount()
            metrics.firing_rate = self.brain.getAverageFiringRate()
            
            # Bottleneck detection
            self._detect_bottlenecks(metrics)
            
        metrics.last_frame_time = time.time() - self.last_frame_time
        self.last_frame_time = time.time()
        
        return metrics
        
    def _detect_bottlenecks(self, metrics: PerformanceMetrics):
        """Detect performance bottlenecks."""
        bottlenecks = []
        
        # CPU bottleneck
        if metrics.cpu_usage > self.cpu_threshold:
            bottlenecks.append('CPU')
            
        # Memory bottleneck
        if metrics.memory_usage > self.memory_threshold:
            bottlenecks.append('Memory')
            
        # VRAM bottleneck
        if metrics.vram_usage > self.vram_threshold:
            bottlenecks.append('VRAM')
            
        # Time bottleneck (frame rate)
        if metrics.last_frame_time > self.update_frequency_threshold:
            bottlenecks.append('Time')
            
        metrics.bottleneck_detected = len(bottlenecks) > 0
        metrics.bottleneck_type = ', '.join(bottlenecks) if bottlenecks else None
        
    def get_current_metrics(self) -> PerformanceMetrics:
        """Get current performance metrics."""
        if not self.metrics_history:
            return self._collect_metrics()
        return self.metrics_history[-1]
        
    def get_metrics_history(self, limit: int = None) -> List[PerformanceMetrics]:
        """Get performance metrics history."""
        if limit:
            return self.metrics_history[-limit:]
        return self.metrics_history.copy()
        
    def get_statistics(self) -> Dict[str, float]:
        """Get aggregated performance statistics."""
        if not self.metrics_history:
            return {}
            
        # Convert to DataFrame for easy analysis
        df = pd.DataFrame([m.to_dict() for m in self.metrics_history])
        
        stats = {
            'total_steps': len(self.metrics_history),
            'avg_cpu_usage': df['cpu_usage'].mean(),
            'max_cpu_usage': df['cpu_usage'].max(),
            'avg_memory_usage': df['memory_usage'].mean(),
            'max_memory_usage': df['memory_usage'].max(),
            'avg_firing_rate': df['firing_rate'].mean(),
            'max_firing_rate': df['firing_rate'].max(),
            'bottleneck_count': df['bottleneck_detected'].sum(),
            'total_spikes': df['spike_count'].sum()
        }
        
        return stats
        
    def export_metrics(self, filepath: str, format: str = 'json'):
        """
        Export performance metrics to file.
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'parquet')
        """
        metrics_data = [m.to_dict() for m in self.metrics_history]
        
        if format == 'json':
            with open(filepath, 'w') as f:
                json.dump(metrics_data, f, indent=2)
        elif format == 'csv':
            df = pd.DataFrame(metrics_data)
            df.to_csv(filepath, index=False)
        elif format == 'parquet':
            try:
                import pyarrow
                import pyarrow.parquet as pq
                df = pd.DataFrame(metrics_data)
                table = pyarrow.Table.from_pandas(df)
                pq.write_table(table, filepath)
            except ImportError:
                raise ImportError("pyarrow is required for parquet export")
                
    def generate_report(self) -> str:
        """Generate a performance report."""
        stats = self.get_statistics()
        
        report = f"""NLM Performance Report
=======================
Total Steps: {stats['total_steps']}
Average CPU Usage: {stats['avg_cpu_usage']:.1f}%
Max CPU Usage: {stats['max_cpu_usage']:.1f}%
Average Memory Usage: {stats['avg_memory_usage']:.1f}%
Max Memory Usage: {stats['max_memory_usage']:.1f}%
Average Firing Rate: {stats['avg_firing_rate']:.2f}
Max Firing Rate: {stats['max_firing_rate']:.2f}
Bottleneck Events: {int(stats['bottleneck_count'])}
Total Spikes: {int(stats['total_spikes'])}

Bottleneck Analysis:
"""
        
        bottleneck_metrics = [m for m in self.metrics_history if m.bottleneck_detected]
        if bottleneck_metrics:
            bottleneck_types = {}
            for m in bottleneck_metrics:
                if m.bottleneck_type:
                    for bottleneck in m.bottleneck_type.split(', '):
                        bottleneck_types[bottleneck] = bottleneck_types.get(bottleneck, 0) + 1
            
            for bottleneck, count in bottleneck_types.items():
                percentage = (count / len(bottleneck_metrics)) * 100
                report += f"  - {bottleneck}: {percentage:.1f}% of steps\n"
        else:
            report += "  No bottlenecks detected.\n"
            
        return report