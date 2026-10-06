#!/usr/bin/env python3
"""
NLM - Performance Debugging Tool

Performance profiling and visualization with sophisticated capabilities for researchers.

Features:
- Execution time profiling
- CPU usage visualization
- Memory usage visualization
- I/O performance visualization
- Network performance visualization
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
from typing import Dict, List, Optional, Tuple, Any
import time
import threading
from dataclasses import dataclass
from enum import Enum
import json
from datetime import datetime
import os
import psutil
import tracemalloc
from collections import defaultdict

@dataclass
class PerformanceDebugConfig:
    """Configuration for performance debugging."""
    profile_cpu: bool = True
    profile_memory: bool = True
    profile_io: bool = True
    profile_network: bool = True
    real_time_updates: bool = True
    profiling_interval: float = 0.1  # seconds
    history_size: int = 1000
    cpu_threshold: float = 80.0  # %
    memory_threshold: float = 80.0  # %
    io_threshold: float = 100.0  # MB/s
    network_threshold: float = 10.0  # MB/s
    window_size: Tuple[int, int] = (1400, 900)
    save_path: Optional[str] = None
    auto_save: bool = False
    save_interval: int = 10

class PerformanceDebugState(Enum):
    """Performance debugging state enumeration."""
    IDLE = "idle"
    PROFILING = "profiling"
    ANALYZING = "analyzing"
    EXPORTING = "exporting"

class NLPerformanceProfiler:
    """
    Performance profiling and visualization tool.
    
    Provides sophisticated performance profiling capabilities including execution time
    profiling, CPU usage visualization, memory usage visualization, I/O performance
    visualization, and network performance visualization with comprehensive analysis
    and reporting.
    """
    
    def __init__(self, config: Optional[PerformanceDebugConfig] = None):
        self.config = config or PerformanceDebugConfig()
        self.state = PerformanceDebugState.IDLE
        self.performance_history: List[Dict[str, Any]] = []
        self.cpu_samples: List[Dict[str, Any]] = []
        self.memory_samples: List[Dict[str, Any]] = []
        self.io_samples: List[Dict[str, Any]] = []
        self.network_samples: List[Dict[str, Any]] = []
        self.current_time = 0.0
        self.animation_obj = None
        self.fig = None
        self.axes = None
        self.cpu_plots = []
        self.memory_plots = []
        self.io_plots = []
        self.network_plots = []
        
        # Setup visualization
        self._setup_visualization()
    
    def _setup_visualization(self):
        """Setup the matplotlib visualization."""
        plt.style.use('seaborn-v0_8')
        self.fig, self.axes = plt.subplots(
            2, 2, figsize=self.config.window_size, 
            gridspec_kw={'hspace': 0.4, 'wspace': 0.3}
        )
        
        # Setup subplots
        self.cpu_plots = [self.axes[0, 0], self.axes[0, 1]]
        self.memory_plots = [self.axes[1, 0]]
        self.io_plots = [self.axes[1, 1]]
        
        for ax in self.axes.flatten():
            ax.set_facecolor('#f8f9fa')
            ax.grid(True, alpha=0.3)
        
        self.cpu_plots[0].set_title('CPU Usage', fontsize=12, fontweight='bold')
        self.cpu_plots[1].set_title('CPU Load Trend', fontsize=12, fontweight='bold')
        self.memory_plots[0].set_title('Memory Usage', fontsize=12, fontweight='bold')
        self.io_plots[0].set_title('I/O Performance', fontsize=12, fontweight='bold')
        
        plt.tight_layout()
    
    def start_profiling(self):
        """Start performance profiling."""
        self.state = PerformanceDebugState.PROFILING
        
        # Clear any existing animation
        if self.animation_obj:
            self.animation_obj.event_source.stop()
        
        # Initialize tracking
        self._initialize_tracking()
        
        # Start profiling thread
        self.profiling_thread = threading.Thread(target=self._profiling_loop)
        self.profiling_thread.daemon = True
        self.profiling_thread.start()
        
        # Start memory profiling
        tracemalloc.start()
        
        print(f"Starting performance profiling...")
        print(f"CPU profiling: {self.config.profile_cpu}")
        print(f"Memory profiling: {self.config.profile_memory}")
        print(f"I/O profiling: {self.config.profile_io}")
        print(f"Network profiling: {self.config.profile_network}")
        print(f"Real-time updates: {self.config.real_time_updates}")
        print(f"State: {self.state.value}")
    
    def _initialize_tracking(self):
        """Initialize performance tracking."""
        self.performance_history = []
        self.cpu_samples = []
        self.memory_samples = []
        self.io_samples = []
        self.network_samples = []
        
        # Initialize system metrics
        self.process = psutil.Process()
        self.current_time = 0.0
        
        # Get network interface
        self.net_io = psutil.net_io_counters()
        self.disk_io = psutil.disk_io_counters()
    
    def _profiling_loop(self):
        """Main profiling loop for performance monitoring."""
        while self.state == PerformanceDebugState.PROFILING:
            # Update performance metrics
            self._update_performance_metrics()
            
            # Render plots
            self._render_plots()
            
            # Save if auto-save is enabled
            if self.config.auto_save and len(self.performance_history) % self.config.save_interval == 0:
                self._save_current_frame()
            
            # Control profiling speed
            time.sleep(self.config.profiling_interval)
    
    def _update_performance_metrics(self):
        """Update performance metrics."""
        self.current_time += self.config.profiling_interval * 1000  # Convert to ms
        
        # Get current CPU metrics
        if self.config.profile_cpu:
            cpu_percent = self.process.cpu_percent()
            cpu_times = self.process.cpu_times()
            
            cpu_sample = {
                'time': self.current_time,
                'cpu_percent': cpu_percent,
                'user_time': cpu_times.user,
                'system_time': cpu_times.system,
                'cpu_count': psutil.cpu_count()
            }
            self.cpu_samples.append(cpu_sample)
        
        # Get current memory metrics
        if self.config.profile_memory:
            memory_info = self.process.memory_info()
            memory_percent = self.process.memory_percent()
            
            # Get memory snapshot
            current, peak = tracemalloc.get_traced_memory()
            
            memory_sample = {
                'time': self.current_time,
                'rss': memory_info.rss / 1024 / 1024,  # MB
                'vms': memory_info.vms / 1024 / 1024,  # MB
                'memory_percent': memory_percent,
                'tracemalloc_current': current / 1024 / 1024,  # MB
                'tracemalloc_peak': peak / 1024 / 1024  # MB
            }
            self.memory_samples.append(memory_sample)
        
        # Get current I/O metrics
        if self.config.profile_io:
            disk_io = psutil.disk_io_counters()
            
            # Calculate I/O rates
            if self.disk_io:
                read_diff = disk_io.read_bytes - self.disk_io.read_bytes
                write_diff = disk_io.write_bytes - self.disk_io.write_bytes
                read_time_diff = (time.time() - self.current_time) * 1000  # ms
                write_time_diff = read_time_diff
                
                read_rate = read_diff / read_time_diff if read_time_diff > 0 else 0
                write_rate = write_diff / write_time_diff if write_time_diff > 0 else 0
                total_rate = read_rate + write_rate
                
                self.disk_io = disk_io
            else:
                read_rate = write_rate = total_rate = 0
            
            io_sample = {
                'time': self.current_time,
                'read_rate': read_rate / 1024 / 1024,  # MB/s
                'write_rate': write_rate / 1024 / 1024,  # MB/s
                'total_rate': total_rate / 1024 / 1024  # MB/s
            }
            self.io_samples.append(io_sample)
        
        # Get current network metrics
        if self.config.profile_network:
            net_io = psutil.net_io_counters()
            
            # Calculate network rates
            if self.net_io:
                read_diff = net_io.bytes_recv - self.net_io.bytes_recv
                write_diff = net_io.bytes_sent - self.net_io.bytes_sent
                time_diff = (time.time() - self.current_time) * 1000  # ms
                
                read_rate = read_diff / time_diff if time_diff > 0 else 0
                write_rate = write_diff / time_diff if time_diff > 0 else 0
                total_rate = read_rate + write_rate
                
                self.net_io = net_io
            else:
                read_rate = write_rate = total_rate = 0
            
            network_sample = {
                'time': self.current_time,
                'read_rate': read_rate / 1024 / 1024,  # MB/s
                'write_rate': write_rate / 1024 / 1024,  # MB/s
                'total_rate': total_rate / 1024 / 1024  # MB/s
            }
            self.network_samples.append(network_sample)
        
        # Combine all samples into performance history
        latest_sample = {
            'time': self.current_time
        }
        
        if self.cpu_samples:
            latest_sample.update(self.cpu_samples[-1])
        if self.memory_samples:
            latest_sample.update(self.memory_samples[-1])
        if self.io_samples:
            latest_sample.update(self.io_samples[-1])
        if self.network_samples:
            latest_sample.update(self.network_samples[-1])
        
        self.performance_history.append(latest_sample)
        
        # Maintain history size
        if len(self.performance_history) > self.config.history_size:
            self.performance_history.pop(0)
            if len(self.cpu_samples) > self.config.history_size:
                self.cpu_samples.pop(0)
            if len(self.memory_samples) > self.config.history_size:
                self.memory_samples.pop(0)
            if len(self.io_samples) > self.config.history_size:
                self.io_samples.pop(0)
            if len(self.network_samples) > self.config.history_size:
                self.network_samples.pop(0)
    
    def _render_plots(self):
        """Render the current visualization plots."""
        if not self.fig:
            return
        
        # Clear axes
        for ax in self.axes.flatten():
            ax.clear()
        
        # Plot CPU usage if enabled
        if self.config.profile_cpu and self.cpu_samples:
            self._plot_cpu_usage()
        
        # Plot memory usage if enabled
        if self.config.profile_memory and self.memory_samples:
            self._plot_memory_usage()
        
        # Plot I/O performance if enabled
        if self.config.profile_io and self.io_samples:
            self._plot_io_performance()
        
        # Plot network performance if enabled
        if self.config.profile_network and self.network_samples:
            self._plot_network_performance()
        
        # Update plot labels
        self._update_plot_labels()
        
        # Refresh the figure
        self.fig.canvas.draw()
    
    def _plot_cpu_usage(self):
        """Plot CPU usage visualization."""
        if len(self.cpu_samples) < 10:
            return
        
        time_series = [sample['time'] for sample in self.cpu_samples]
        cpu_percent_series = [sample['cpu_percent'] for sample in self.cpu_samples]
        
        # Plot current CPU usage
        self.cpu_plots[0].plot(
            time_series, cpu_percent_series,
            linewidth=2, color='red', alpha=0.8
        )
        
        # Plot threshold line
        self.cpu_plots[0].axhline(
            y=self.config.cpu_threshold, color='orange', linestyle='--',
            alpha=0.5, label=f'CPU Threshold ({self.config.cpu_threshold}%)'
        )
        
        # Highlight high CPU periods
        high_cpu_events = [sample for sample in self.cpu_samples if sample['cpu_percent'] > self.config.cpu_threshold]
        if high_cpu_events:
            for event in high_cpu_events[-3:]:
                self.cpu_plots[0].axvspan(
                    event['time'] - 100, event['time'],
                    alpha=0.3, color='red'
                )
        
        self.cpu_plots[0].set_ylim(0, max(cpu_percent_series) * 1.1 if cpu_percent_series else 100)
        self.cpu_plots[0].set_ylabel('CPU (%)')
        self.cpu_plots[0].legend(loc='upper right', fontsize=8)
        
        # Plot CPU load trend (rolling average)
        if len(cpu_percent_series) >= 10:
            rolling_avg = np.convolve(cpu_percent_series[-50:], np.ones(10)/10, mode='valid')
            rolling_time = time_series[-len(rolling_avg):]
            
            self.cpu_plots[1].plot(
                rolling_time, rolling_avg,
                linewidth=2, color='blue', label='10-sample average'
            )
            
            self.cpu_plots[1].set_ylabel('CPU (%)')
            self.cpu_plots[1].legend(loc='upper right', fontsize=8)
    
    def _plot_memory_usage(self):
        """Plot memory usage visualization."""
        if len(self.memory_samples) < 10:
            return
        
        time_series = [sample['time'] for sample in self.memory_samples]
        rss_series = [sample['rss'] for sample in self.memory_samples]
        
        # Plot RSS memory
        self.memory_plots[0].plot(
            time_series, rss_series,
            linewidth=2, color='green', label='RSS', alpha=0.8
        )
        
        # Plot threshold line
        self.memory_plots[0].axhline(
            y=self.config.memory_threshold, color='orange', linestyle='--',
            alpha=0.5, label=f'Memory Threshold ({self.config.memory_threshold}%)'
        )
        
        self.memory_plots[0].set_ylabel('Memory (MB)')
        self.memory_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_io_performance(self):
        """Plot I/O performance visualization."""
        if len(self.io_samples) < 10:
            return
        
        time_series = [sample['time'] for sample in self.io_samples]
        total_rate_series = [sample['total_rate'] for sample in self.io_samples]
        
        # Plot I/O rate
        self.io_plots[0].plot(
            time_series, total_rate_series,
            linewidth=2, color='blue', label='Total I/O Rate', alpha=0.8
        )
        
        # Plot threshold line
        self.io_plots[0].axhline(
            y=self.config.io_threshold, color='red', linestyle='--',
            alpha=0.5, label=f'I/O Threshold ({self.config.io_threshold} MB/s)'
        )
        
        self.io_plots[0].set_ylabel('I/O Rate (MB/s)')
        self.io_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_network_performance(self):
        """Plot network performance visualization."""
        if len(self.network_samples) < 10:
            return
        
        time_series = [sample['time'] for sample in self.network_samples]
        total_rate_series = [sample['total_rate'] for sample in self.network_samples]
        
        # Plot network rate
        self.network_plots[0].plot(
            time_series, total_rate_series,
            linewidth=2, color='purple', label='Network Rate', alpha=0.8
        )
        
        # Plot threshold line
        self.network_plots[0].axhline(
            y=self.config.network_threshold, color='orange', linestyle='--',
            alpha=0.5, label=f'Network Threshold ({self.config.network_threshold} MB/s)'
        )
        
        self.network_plots[0].set_ylabel('Network Rate (MB/s)')
        self.network_plots[0].legend(loc='upper right', fontsize=8)
    
    def _update_plot_labels(self):
        """Update plot labels with current performance statistics."""
        current_time_ms = self.current_time
        
        # Update titles with current statistics
        if self.performance_history:
            latest_sample = self.performance_history[-1]
            
            # Update CPU title
            if 'cpu_percent' in latest_sample:
                cpu_percent = latest_sample['cpu_percent']
                self.cpu_plots[0].set_title(
                    f'CPU Usage (Current: {cpu_percent:.1f}%)',
                    fontsize=12, fontweight='bold'
                )
            
            # Update memory title
            if 'memory_percent' in latest_sample:
                memory_percent = latest_sample['memory_percent']
                self.memory_plots[0].set_title(
                    f'Memory Usage (Current: {memory_percent:.1f}%)',
                    fontsize=12, fontweight='bold'
                )
        
        # Update time in title
        time_str = f'Time: {current_time_ms:.0f} ms'
        self.fig.suptitle(time_str, fontsize=14, fontweight='bold')
    
    def _save_current_frame(self):
        """Save current visualization frame."""
        if self.config.save_path:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"{self.config.save_path}/performance_frame_{timestamp}.png"
            
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            
            try:
                self.fig.savefig(filename, dpi=150, bbox_inches='tight')
                print(f"Saved performance frame: {filename}")
            except Exception as e:
                print(f"Warning: Could not save frame: {e}")
    
    def stop(self):
        """Stop the performance profiling."""
        self.state = PerformanceDebugState.IDLE
        
        # Stop profiling thread
        if hasattr(self, 'profiling_thread') and self.profiling_thread.is_alive():
            self.profiling_thread.join(timeout=5.0)
        
        # Stop memory profiling
        tracemalloc.stop()
        
        print("Performance profiling stopped")
    
    def export_data(self, output_path: str, format: str = "json"):
        """
        Export performance profiling data to file.
        
        Args:
            output_path: Path to output file
            format: Export format (json, csv, npy)
        """
        self.state = PerformanceDebugState.EXPORTING
        
        try:
            if format.lower() == "json":
                export_data = {
                    'config': self.config.__dict__,
                    'performance_history': self.performance_history,
                    'cpu_samples': self.cpu_samples,
                    'memory_samples': self.memory_samples,
                    'io_samples': self.io_samples,
                    'network_samples': self.network_samples,
                    'current_time': self.current_time,
                    'timestamp': datetime.now().isoformat()
                }
                
                with open(output_path, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                    
            elif format.lower() == "csv":
                import pandas as pd
                
                # Combine all samples into DataFrame
                df_data = []
                for sample in self.performance_history:
                    df_data.append(sample)
                
                df = pd.DataFrame(df_data)
                df.to_csv(output_path, index=False)
                
            elif format.lower() == "npy":
                np.savez(output_path, 
                        performance_history=np.array(self.performance_history, dtype=object),
                        current_time=self.current_time,
                        config=self.config.__dict__)
            
            print(f"Performance profiling data exported to: {output_path}")
            
        except Exception as e:
            print(f"Error exporting data: {e}")
            raise
        
        finally:
            self.state = PerformanceDebugState.IDLE

def main():
    """Main entry point for nlx-debug-performance tool."""
    print("NLM - Neural Performance Debugging Tool")
    print("=" * 50)
    print("Performance profiling and visualization with sophisticated capabilities")
    print()
    
    # Example usage
    config = PerformanceDebugConfig(
        profile_cpu=True,
        profile_memory=True,
        profile_io=True,
        profile_network=True,
        real_time_updates=True,
        profiling_interval=0.1,
        cpu_threshold=80.0,
        memory_threshold=80.0,
        io_threshold=100.0,
        network_threshold=10.0,
        window_size=(1400, 900)
    )
    
    # Create performance profiler
    profiler = NLPerformanceProfiler(config)
    
    print("Example configuration created successfully!")
    print(f"CPU profiling: {config.profile_cpu}")
    print(f"Memory profiling: {config.profile_memory}")
    print(f"I/O profiling: {config.profile_io}")
    print(f"Network profiling: {config.profile_network}")
    print(f"Update rate: {1.0/config.profiling_interval} Hz")
    print(f"Window size: {config.window_size}")
    print()
    print("To use the profiler:")
    print("  profiler = NLPerformanceProfiler(config)")
    print("  profiler.start_profiling()")
    print("  profiler.stop()   # to stop")
    print("  profiler.export_data('output.json', 'json')  # to export")

if __name__ == "__main__":
    main()
