"""NLM Python Bindings - Visualization and Debugging Tools

This module provides comprehensive visualization and debugging tools for the NLM Python bindings.
It includes neural activity visualization, memory trace visualization, system monitoring,
and debugging utilities for brain simulation systems.

Features:
1. Neural activity visualization (2D/3D)
2. Memory trace visualization
3. System monitoring and profiling
4. Debug output and logging
5. Performance analysis
6. Interactive debugging tools
7. Export capabilities
8. Comparison and analysis tools
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.patches import Circle, Rectangle
from matplotlib.collections import PatchCollection
import matplotlib.patches as mpatches
from typing import Dict, Any, List, Optional, Tuple, Callable
import time
import threading
import json
import csv
from enum import Enum
from dataclasses import dataclass
import sys
import os
import pyqt5.QtWidgets as QtWidgets
import pyqt5.QtCore as QtCore
import pyqt5.QtGui as QtGui
from pyqt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QWidget, QLabel, QPushButton, QTabWidget, QTextEdit, QSlider, QComboBox, QGroupBox, QScrollArea
from pyqt5.QtCore import Qt, QTimer

class VisualizationType(Enum):
    """Types of visualizations."""
    NEURAL_ACTIVITY = "neural_activity"
    MEMORY_TRACES = "memory_traces"
    SYSTEM_MONITORING = "system_monitoring"
    PLOTTING = "plotting"
    ANIMATION = "animation"
    INTERACTIVE = "interactive"

class DebugLevel(Enum):
    """Debug log levels."""
    ERROR = "ERROR"
    WARNING = "WARNING"
    INFO = "INFO"
    DEBUG = "DEBUG"
    TRACE = "TRACE"

@dataclass
class VisualizationConfig:
    """Configuration for visualization."""
    
    visualization_type: VisualizationType = VisualizationType.NEURAL_ACTIVITY
    width: int = 800
    height: int = 600
    show_grid: bool = True
    show_labels: bool = True
    animate: bool = False
    animation_interval: int = 100
    colormap: str = "viridis"
    max_neurons_display: int = 1000
    neuron_size: float = 2.0
    synapse_width: float = 0.5
    background_color: str = "black"
    neuron_color_scheme: str = "activity"
    save_path: Optional[str] = None
    interactive: bool = False
    real_time_update: bool = False

@dataclass
class DebugLogEntry:
    """Entry in debug log."""
    
    timestamp: float
    level: DebugLevel
    source: str
    message: str
    data: Optional[Dict[str, Any]] = None
    stack_trace: Optional[str] = None

class NeuralActivityVisualizer:
    """Visualizer for neural activity."""
    
    def __init__(self, config: VisualizationConfig):
        self.config = config
        self.fig = None
        self.ax = None
        self.neuron_circles = []
        self.synapse_lines = []
        self.animation_obj = None
        self.current_frame = 0
        
        # Neural data
        self.neuron_positions = []
        self.neuron_activations = []
        self.neuron_types = []
        self.synapse_connections = []
        self.region_boundaries = []
    
    def setup_figure(self):
        """Setup matplotlib figure."""
        self.fig, self.ax = plt.subplots(figsize=(self.config.width / 100, 
                                                 self.config.height / 100))
        self.ax.set_aspect('equal')
        self.ax.set_facecolor(self.config.background_color)
        
        if self.config.show_grid:
            self.ax.grid(True, alpha=0.3)
        
        # Initialize neural elements
        self._initialize_neural_elements()
    
    def _initialize_neural_elements(self):
        """Initialize neural element containers."""
        self.neuron_circles = []
        self.synapse_lines = []
        self.region_boundaries = []
    
    def update_neural_data(self, neuron_data: Dict[str, Any]):
        """Update neural data for visualization."""
        self.neuron_positions = neuron_data.get('positions', [])
        self.neuron_activations = neuron_data.get('activations', [])
        self.neuron_types = neuron_data.get('types', [])
        self.synapse_connections = neuron_data.get('synapses', [])
        self.region_boundaries = neuron_data.get('regions', [])
    
    def draw_neural_network(self):
        """Draw the neural network visualization."""
        if not self.neuron_positions:
            return
        
        # Clear previous elements
        self.ax.clear()
        
        # Draw region boundaries
        for region in self.region_boundaries:
            self._draw_region_boundary(region)
        
        # Draw synapses
        self._draw_synapses()
        
        # Draw neurons
        self._draw_neurons()
        
        # Set plot limits
        all_x = [pos[0] for pos in self.neuron_positions]
        all_y = [pos[1] for pos in self.neuron_positions]
        
        if all_x and all_y:
            x_margin = (max(all_x) - min(all_x)) * 0.1 if len(all_x) > 1 else 1.0
            y_margin = (max(all_y) - min(all_y)) * 0.1 if len(all_y) > 1 else 1.0
            
            self.ax.set_xlim(min(all_x) - x_margin, max(all_x) + x_margin)
            self.ax.set_ylim(min(all_y) - y_margin, max(all_y) + y_margin)
        
        # Add legend
        self._add_legend()
    
    def _draw_region_boundary(self, region: Dict[str, Any]):
        """Draw a region boundary."""
        if 'center' in region and 'radius' in region:
            circle = Circle(region['center'], region['radius'], 
                          facecolor='none', edgecolor='gray', alpha=0.5, linewidth=1)
            self.ax.add_patch(circle)
    
    def _draw_synapses(self):
        """Draw synaptic connections."""
        for synapse in self.synapse_connections:
            if 'source' in synapse and 'target' in synapse:
                source_idx = synapse['source']
                target_idx = synapse['target']
                
                if (0 <= source_idx < len(self.neuron_positions) and 
                    0 <= target_idx < len(self.neuron_positions)):
                    source_pos = self.neuron_positions[source_idx]
                    target_pos = self.neuron_positions[target_idx]
                    
                    # Get activation-based color
                    activation = 0.0
                    if source_idx < len(self.neuron_activations):
                        activation = self.neuron_activations[source_idx]
                    
                    color = plt.cm.viridis(activation)
                    linewidth = synapse.get('weight', 1.0) * self.config.synapse_width
                    
                    line = self.ax.plot([source_pos[0], target_pos[0]],
                                      [source_pos[1], target_pos[1]],
                                      'o-', linewidth=linewidth, 
                                      alpha=0.6, color=color, markersize=0)[0]
                    self.synapse_lines.append(line)
    
    def _draw_neurons(self):
        """Draw neurons."""
        for i, (pos, activation, neuron_type) in enumerate(zip(
            self.neuron_positions, self.neuron_activations, self.neuron_types)):
            
            # Determine neuron color based on type and activation
            if self.config.neuron_color_scheme == "activity":
                color = plt.cm.viridis(activation)
            elif self.config.neuron_color_scheme == "type":
                color_map = {
                    0: 'red',      # Excitatory
                    1: 'blue',     # Inhibitory
                    2: 'green',    # Modulatory
                    3: 'orange',   # Sensory
                    4: 'purple',   # Motor
                    5: 'cyan'      # Internal
                }
                color = color_map.get(neuron_type, 'gray')
            else:
                color = 'gray'
            
            # Calculate neuron size based on activation
            size = self.config.neuron_size * (1.0 + activation * 2.0)
            
            # Draw neuron
            circle = Circle(pos, size/2, facecolor=color, 
                          edgecolor='white', alpha=0.8, linewidth=1)
            self.ax.add_patch(circle)
            self.neuron_circles.append(circle)
    
    def _add_legend(self):
        """Add legend to the plot."""
        if not self.config.show_labels:
            return
        
        legend_elements = []
        
        # Add neuron type legend
        neuron_type_legend = [
            mpatches.Patch(facecolor='red', label='Excitatory'),
            mpatches.Patch(facecolor='blue', label='Inhibitory'),
            mpatches.Patch(facecolor='green', label='Modulatory'),
            mpatches.Patch(facecolor='orange', label='Sensory'),
            mpatches.Patch(facecolor='purple', label='Motor'),
            mpatches.Patch(facecolor='cyan', label='Internal')
        ]
        
        legend_elements.extend(neuron_type_legend)
        
        # Add activation legend
        activation_legend = []
        for activation in [0.0, 0.25, 0.5, 0.75, 1.0]:
            color = plt.cm.viridis(activation)
            activation_legend.append(
                mpatches.Patch(facecolor=color, label=f'{activation:.1f}')
            )
        
        # Add region boundary legend
        region_patch = mpatches.Patch(facecolor='none', edgecolor='gray', alpha=0.5, 
                                    label='Neural Region')
        legend_elements.append(region_patch)
        
        # Create legend
        legend = self.ax.legend(handles=legend_elements, loc='upper right', 
                              bbox_to_anchor=(1.15, 1), fontsize=8)
        legend.get_frame().set_alpha(0.9)
    
    def save_visualization(self, filepath: str, dpi: int = 300):
        """Save visualization to file."""
        if self.fig:
            self.fig.savefig(filepath, dpi=dpi, bbox_inches='tight')
    
    def show(self):
        """Display visualization."""
        plt.show()
    
    def animate_neural_activity(self, frames: int = 100, interval: int = 100):
        """Create animation of neural activity."""
        if not self.animation_obj:
            # Create animation function
            def animate(frame):
                # Update neural data (simulated)
                np.random.seed(frame)
                new_activations = np.random.random(len(self.neuron_activations))
                self.neuron_activations = new_activations.tolist()
                
                # Redraw
                self.draw_neural_network()
                return self.neuron_circles + self.synapse_lines
            
            # Create animation
            self.animation_obj = animation.FuncAnimation(
                self.fig, animate, frames=frames, 
                interval=interval, blit=True
            )
        
        return self.animation_obj
class MemoryTraceVisualizer:
    """Visualizer for memory traces."""
    
    def __init__(self, config: VisualizationConfig):
        self.config = config
        self.fig = None
        self.axes = []
        self.time_data = []
        self.memory_data = {}
        self.memory_types = []
    
    def setup_figure(self):
        """Setup figure with subplots."""
        self.fig, self.axes = plt.subplots(2, 2, figsize=(15, 10))
        self.fig.suptitle("Memory Trace Visualization", fontsize=16)
        
        # Flatten axes for easier access
        self.axes = self.axes.flatten()
    
    def update_memory_data(self, memory_data: Dict[str, Any]):
        """Update memory data for visualization."""
        self.time_data = memory_data.get('time', [])
        self.memory_data = memory_data.get('traces', {})
        self.memory_types = list(self.memory_data.keys())
    
    def draw_memory_traces(self):
        """Draw memory trace plots."""
        if not self.time_data or not self.memory_data:
            return
        
        # Clear all subplots
        for ax in self.axes:
            ax.clear()
        
        # Plot each memory type
        colors = plt.cm.Set3(np.linspace(0, 1, len(self.memory_types)))
        
        for i, (memory_type, traces) in enumerate(self.memory_data.items()):
            ax = self.axes[i]
            
            # Plot memory traces
            for trace_idx, trace in enumerate(traces):
                ax.plot(self.time_data, trace, 
                       color=colors[i], alpha=0.7, linewidth=1, label=f"Trace {trace_idx}")
            
            # Plot average trace
            if traces:
                avg_trace = np.mean(traces, axis=0)
                ax.plot(self.time_data, avg_trace, 
                       color='black', linewidth=2, label='Average')
            
            ax.set_title(memory_type.replace('_', ' ').title())
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Activation')
            ax.legend(fontsize=8)
            ax.grid(True, alpha=0.3)
        
        # Hide unused subplots
        for i in range(len(self.memory_types), len(self.axes)):
            self.axes[i].set_visible(False)
    
    def create_heatmap(self, memory_type: str, timestep: int):
        """Create heatmap visualization of memory at specific timestep."""
        if memory_type not in self.memory_data:
            return None
        
        traces = self.memory_data[memory_type]
        if not traces:
            return None
        
        # Create heatmap
        fig, ax = plt.subplots(figsize=(10, 8))
        
        # Get data at timestep
        timestep_data = []
        for trace in traces:
            if timestep < len(trace):
                timestep_data.append(trace[timestep])
        
        if not timestep_data:
            return None
        
        # Reshape for heatmap (assuming traces are stored as time series)
        data_matrix = np.array(timestep_data).reshape(1, -1)
        
        im = ax.imshow(data_matrix, aspect='auto', cmap=self.config.colormap,
                      extent=[0, len(timestep_data), 0, 1])
        
        ax.set_title(f"{memory_type.replace('_', ' ').title()} - Timestep {timestep}")
        ax.set_xlabel("Trace Index")
        ax.set_ylabel("Memory Type")
        
        plt.colorbar(im, ax=ax)
        
        return fig, ax
    
    def save_visualization(self, filepath: str, dpi: int = 300):
        """Save visualization to file."""
        if self.fig:
            self.fig.savefig(filepath, dpi=dpi, bbox_inches='tight')
    
    def show(self):
        """Display visualization."""
        plt.show()
class SystemMonitor:
    """System monitoring and profiling tool."""
    
    def __init__(self):
        self.metrics_history: List[Dict[str, Any]] = []
        self.monitor_thread = None
        self.is_monitoring = False
        self.update_interval = 1.0
        
        # System metrics
        self.cpu_usage = []
        self.memory_usage = []
        self.disk_usage = []
        self.network_io = []
        
        # Application-specific metrics
        self.brain_metrics = []
        self.neuron_count = 0
        self.synapse_count = 0
        self.firing_rate = 0.0
        self.active_neurons = 0
    
    def start_monitoring(self, interval: float = 1.0):
        """Start system monitoring."""
        self.is_monitoring = True
        self.update_interval = interval
        
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop system monitoring."""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
    
    def _monitor_loop(self):
        """Monitoring loop running in separate thread."""
        while self.is_monitoring:
            self._collect_metrics()
            time.sleep(self.update_interval)
    
    def _collect_metrics(self):
        """Collect system metrics."""
        import psutil
        import os
        
        # System metrics
        self.cpu_usage.append(psutil.cpu_percent(interval=None))
        memory = psutil.virtual_memory()
        self.memory_usage.append(memory.percent)
        disk = psutil.disk_usage('/')
        self.disk_usage.append(disk.percent)
        
        # Network I/O
        net_io = psutil.net_io_counters()
        self.network_io.append({
            'bytes_sent': net_io.bytes_sent,
            'bytes_recv': net_io.bytes_recv,
            'packets_sent': net_io.packets_sent,
            'packets_recv': net_io.packets_recv
        })
        
        # Application metrics
        brain_metric = {
            'timestamp': time.time(),
            'cpu_usage': self.cpu_usage[-1],
            'memory_usage': self.memory_usage[-1],
            'disk_usage': self.disk_usage[-1],
            'neuron_count': self.neuron_count,
            'synapse_count': self.synapse_count,
            'firing_rate': self.firing_rate,
            'active_neurons': self.active_neurons
        }
        
        self.brain_metrics.append(brain_metric)
        
        # Keep only last hour of data
        current_time = time.time()
        cutoff_time = current_time - 3600
        
        self.metrics_history = [m for m in self.metrics_history if m['timestamp'] > cutoff_time]
        self.metrics_history.append(brain_metric)
    
    def update_brain_metrics(self, brain: Any):
        """Update brain-specific metrics."""
        self.neuron_count = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        self.synapse_count = brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0
        self.firing_rate = brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0.0
        self.active_neurons = brain.getActiveNeuronCount() if hasattr(brain, 'getActiveNeuronCount') else 0
    
    def create_performance_plot(self):
        """Create performance monitoring plots."""
        if not self.metrics_history:
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle("System Performance Monitoring", fontsize=16)
        
        time_data = [m['timestamp'] for m in self.metrics_history]
        
        # CPU Usage
        ax1 = axes[0, 0]
        ax1.plot(time_data, self.cpu_usage, color='red', linewidth=2)
        ax1.set_title("CPU Usage (%)\n", fontsize=12)
        ax1.set_ylabel("CPU Usage")
        ax1.grid(True, alpha=0.3)
        ax1.set_ylim(0, 100)
        
        # Memory Usage
        ax2 = axes[0, 1]
        ax2.plot(time_data, self.memory_usage, color='blue', linewidth=2)
        ax2.set_title("Memory Usage (%)\n", fontsize=12)
        ax2.set_ylabel("Memory Usage")
        ax2.grid(True, alpha=0.3)
        ax2.set_ylim(0, 100)
        
        # Disk Usage
        ax3 = axes[1, 0]
        ax3.plot(time_data, self.disk_usage, color='green', linewidth=2)
        ax3.set_title("Disk Usage (%)\n", fontsize=12)
        ax3.set_ylabel("Disk Usage")
        ax3.set_xlabel("Time")
        ax3.grid(True, alpha=0.3)
        ax3.set_ylim(0, 100)
        
        # Brain Performance
        ax4 = axes[1, 1]
        neurons = [m['neuron_count'] for m in self.metrics_history]
        synapses = [m['synapse_count'] for m in self.metrics_history]
        
        ax4.plot(time_data, neurons, color='orange', label='Neurons', linewidth=2)
        ax4.plot(time_data, synapses, color='purple', label='Synapses', linewidth=2)
        ax4.set_title("Brain System Size\n", fontsize=12)
        ax4.set_ylabel("Count")
        ax4.set_xlabel("Time")
        ax4.legend()
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def create_system_status_report(self) -> Dict[str, Any]:
        """Create system status report."""
        if not self.metrics_history:
            return {'error': 'No monitoring data available'}
        
        latest_metrics = self.metrics_history[-1]
        
        # Calculate statistics
        if self.cpu_usage:
            avg_cpu = np.mean(self.cpu_usage)
            max_cpu = np.max(self.cpu_usage)
        else:
            avg_cpu = max_cpu = 0
        
        if self.memory_usage:
            avg_memory = np.mean(self.memory_usage)
            max_memory = np.max(self.memory_usage)
        else:
            avg_memory = max_memory = 0
        
        if self.disk_usage:
            avg_disk = np.mean(self.disk_usage)
            max_disk = np.max(self.disk_usage)
        else:
            avg_disk = max_disk = 0
        
        return {
            'current_status': latest_metrics,
            'statistics': {
                'cpu': {'average': avg_cpu, 'maximum': max_cpu},
                'memory': {'average': avg_memory, 'maximum': max_memory},
                'disk': {'average': avg_disk, 'maximum': max_disk}
            },
            'brain_performance': {
                'total_neurons': latest_metrics.get('neuron_count', 0),
                'total_synapses': latest_metrics.get('synapse_count', 0),
                'average_firing_rate': latest_metrics.get('firing_rate', 0),
                'active_neuron_count': latest_metrics.get('active_neurons', 0)
            },
            'monitoring_duration': len(self.metrics_history) * self.update_interval,
            'data_points': len(self.metrics_history)
        }
    
    def export_metrics(self, filepath: str):
        """Export metrics to CSV file."""
        if not self.metrics_history:
            return False
        
        try:
            with open(filepath, 'w', newline='') as csvfile:
                fieldnames = ['timestamp', 'cpu_usage', 'memory_usage', 
                            'disk_usage', 'neuron_count', 'synapse_count',
                            'firing_rate', 'active_neurons']
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                
                writer.writeheader()
                for metrics in self.metrics_history:
                    row = {
                        'timestamp': metrics['timestamp'],
                        'cpu_usage': metrics['cpu_usage'],
                        'memory_usage': metrics['memory_usage'],
                        'disk_usage': metrics['disk_usage'],
                        'neuron_count': metrics.get('neuron_count', 0),
                        'synapse_count': metrics.get('synapse_count', 0),
                        'firing_rate': metrics.get('firing_rate', 0),
                        'active_neurons': metrics.get('active_neurons', 0)
                    }
                    writer.writerow(row)
            
            return True
        except Exception as e:
            print(f"Error exporting metrics: {e}")
            return False
class DebugLogger:
    """Advanced debugging logger."""
    
    def __init__(self, log_file: str = "nlm_debug.log", level: DebugLevel = DebugLevel.INFO):
        self.log_file = log_file
        self.level = level
        self.log_entries: List[DebugLogEntry] = []
        self.log_file_handle = None
        self.log_lock = threading.Lock()
        
        # Create log file
        self.log_file_handle = open(log_file, 'w')
    
    def log(self, level: DebugLevel, source: str, message: str, 
            data: Optional[Dict[str, Any]] = None, stack_trace: Optional[str] = None):
        """Log a message."""
        if level.value < self.level.value:
            return
        
        entry = DebugLogEntry(
            timestamp=time.time(),
            level=level,
            source=source,
            message=message,
            data=data,
            stack_trace=stack_trace
        )
        
        with self.log_lock:
            # Add to in-memory log
            self.log_entries.append(entry)
            
            # Write to file
            log_line = f"[{entry.timestamp:.6f}] {level.value:6s} {source:20s}: {message}\n"
            self.log_file_handle.write(log_line)
            self.log_file_handle.flush()
            
            # Print to console if level is ERROR or WARNING
            if level in (DebugLevel.ERROR, DebugLevel.WARNING):
                print(log_line.strip())
    
    def debug(self, source: str, message: str, data: Optional[Dict[str, Any]] = None):
        """Log debug message."""
        self.log(DebugLevel.DEBUG, source, message, data)
    
    def info(self, source: str, message: str, data: Optional[Dict[str, Any]] = None):
        """Log info message."""
        self.log(DebugLevel.INFO, source, message, data)
    
    def warning(self, source: str, message: str, data: Optional[Dict[str, Any]] = None):
        """Log warning message."""
        self.log(DebugLevel.WARNING, source, message, data)
    
    def error(self, source: str, message: str, 
              data: Optional[Dict[str, Any]] = None,
              stack_trace: Optional[str] = None):
        """Log error message."""
        self.log(DebugLevel.ERROR, source, message, data, stack_trace)
    
    def trace(self, source: str, message: str, data: Optional[Dict[str, Any]] = None):
        """Log trace message."""
        self.log(DebugLevel.TRACE, source, message, data)
    
    def get_logs(self, level: Optional[DebugLevel] = None, 
                 source: Optional[str] = None, 
                 since: Optional[float] = None) -> List[DebugLogEntry]:
        """Get filtered logs."""
        logs = self.log_entries
        
        if level:
            logs = [log for log in logs if log.level == level]
        
        if source:
            logs = [log for log in logs if log.source == source]
        
        if since:
            logs = [log for log in logs if log.timestamp >= since]
        
        return logs
    
    def get_log_summary(self) -> Dict[str, Any]:
        """Get log summary statistics."""
        if not self.log_entries:
            return {'total_entries': 0}
        
        level_counts = {}
        source_counts = {}
        
        for entry in self.log_entries:
            level_counts[entry.level.value] = level_counts.get(entry.level.value, 0) + 1
            source_counts[entry.source] = source_counts.get(entry.source, 0) + 1
        
        return {
            'total_entries': len(self.log_entries),
            'level_counts': level_counts,
            'source_counts': source_counts,
            'oldest_entry': min(log.timestamp for log in self.log_entries),
            'newest_entry': max(log.timestamp for log in self.log_entries)
        }
    
    def export_logs(self, filepath: str, format: str = "json"):
        """Export logs to file."""
        try:
            if format.lower() == 'json':
                data = {
                    'entries': [
                        {
                            'timestamp': entry.timestamp,
                            'level': entry.level.value,
                            'source': entry.source,
                            'message': entry.message,
                            'data': entry.data
                        }
                        for entry in self.log_entries
                    ]
                }
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)
            
            elif format.lower() == 'csv':
                import csv
                with open(filepath, 'w', newline='') as csvfile:
                    fieldnames = ['timestamp', 'level', 'source', 'message', 'data']
                    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                    writer.writeheader()
                    
                    for entry in self.log_entries:
                        row = {
                            'timestamp': entry.timestamp,
                            'level': entry.level.value,
                            'source': entry.source,
                            'message': entry.message,
                            'data': json.dumps(entry.data) if entry.data else ''
                        }
                        writer.writerow(row)
            
            return True
            
        except Exception as e:
            print(f"Error exporting logs: {e}")
            return False
    
    def close(self):
        """Close log file."""
        if self.log_file_handle:
            self.log_file_handle.close()
class NLMVisualizer:
    """Main visualizer class combining all visualization components."""
    
    def __init__(self, config: VisualizationConfig):
        self.config = config
        self.neural_visualizer = NeuralActivityVisualizer(config)
        self.memory_visualizer = MemoryTraceVisualizer(config)
        self.system_monitor = SystemMonitor()
        self.debug_logger = DebugLogger()
        
        # Visualization state
        self.current_neural_data = {}
        self.current_memory_data = {}
        self.is_running = False
        self.update_timer = None
    
    def setup_all(self):
        """Setup all visualizers."""
        self.neural_visualizer.setup_figure()
        self.memory_visualizer.setup_figure()
    
    def update_data(self, neural_data: Dict[str, Any], memory_data: Dict[str, Any]):
        """Update data for all visualizers."""
        self.current_neural_data = neural_data
        self.current_memory_data = memory_data
        
        self.neural_visualizer.update_neural_data(neural_data)
        self.memory_visualizer.update_memory_data(memory_data)
    
    def start_system_monitoring(self, interval: float = 1.0):
        """Start system monitoring."""
        self.system_monitor.start_monitoring(interval)
    
    def stop_system_monitoring(self):
        """Stop system monitoring."""
        self.system_monitor.stop_monitoring()
    
    def update_brain_metrics(self, brain: Any):
        """Update brain metrics in system monitor."""
        self.system_monitor.update_brain_metrics(brain)
    
    def create_comprehensive_dashboard(self) -> Any:
        """Create comprehensive dashboard with all visualizations."""
        if not self.fig:
            self.fig = plt.figure(figsize=(20, 12))
            self.fig.suptitle("NLM Visualization Dashboard", fontsize=16)
        
        # Create subplots
        gs = self.fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        # Neural activity plot (top left)
        ax1 = self.fig.add_subplot(gs[0, :2])
        self._draw_neural_activity(ax1)
        
        # Memory traces (top right)
        ax2 = self.fig.add_subplot(gs[0, 2:])
        self._draw_memory_traces(ax2)
        
        # System monitoring (middle left)
        ax3 = self.fig.add_subplot(gs[1, :2])
        self._draw_system_monitoring(ax3)
        
        # Debug log (middle right)
        ax4 = self.fig.add_subplot(gs[1, 2:])
        self._draw_debug_log(ax4)
        
        # Performance metrics (bottom left)
        ax5 = self.fig.add_subplot(gs[2, :2])
        self._draw_performance_metrics(ax5)
        
        # Control panel (bottom right)
        ax6 = self.fig.add_subplot(gs[2, 2:])
        self._draw_control_panel(ax6)
        
        plt.tight_layout()
        return self.fig
    
    def _draw_neural_activity(self, ax):
        """Draw neural activity on subplot."""
        if not self.current_neural_data:
            return
        
        # Simple scatter plot of neurons
        positions = self.current_neural_data.get('positions', [])
        activations = self.current_neural_data.get('activations', [])
        
        if positions and activations:
            colors = plt.cm.viridis(np.array(activations))
            sizes = np.array(activations) * 100 + 10
            
            ax.scatter([p[0] for p in positions], [p[1] for p in positions],
                      c=colors, s=sizes, alpha=0.7, edgecolors='black', linewidth=0.5)
            
            ax.set_aspect('equal')
            ax.set_title('Neural Activity', fontsize=12)
            ax.set_xlabel('X')
            ax.set_ylabel('Y')
            ax.grid(True, alpha=0.3)
    
    def _draw_memory_traces(self, ax):
        """Draw memory traces on subplot."""
        if not self.current_memory_data:
            return
        
        time_data = self.current_memory_data.get('time', [])
        memory_data = self.current_memory_data.get('traces', {})
        
        if not time_data or not memory_data:
            return
        
        colors = plt.cm.Set3(np.linspace(0, 1, len(memory_data)))
        
        for i, (memory_type, traces) in enumerate(memory_data.items()):
            if traces:
                avg_trace = np.mean(traces, axis=0)
                ax.plot(time_data, avg_trace, 
                       color=colors[i], label=memory_type.replace('_', ' '),
                       linewidth=2)
        
        ax.set_title('Memory Traces', fontsize=12)
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Activation')
        ax.legend(fontsize=8)
        ax.grid(True, alpha=0.3)
    
    def _draw_system_monitoring(self, ax):
        """Draw system monitoring on subplot."""
        # This would show system metrics from system_monitor
        ax.text(0.5, 0.5, 'System Monitoring\n(Real-time metrics)',
               horizontalalignment='center', verticalalignment='center',
               transform=ax.transAxes, fontsize=12)
        ax.set_axis_off()
    
    def _draw_debug_log(self, ax):
        """Draw debug log on subplot."""
        logs = self.debug_logger.get_logs(since=time.time() - 60)  # Last minute
        
        if logs:
            # Show last 10 log entries
            for i, log in enumerate(logs[-10:]):
                level_symbol = {'ERROR': 'E', 'WARNING': 'W', 'INFO': 'I', 'DEBUG': 'D', 'TRACE': 'T'}.get(log.level.value, '?')
                ax.text(0.1, 0.9 - i * 0.15, 
                       f"[{level_symbol}] {log.source}: {log.message[:50]}...",
                       fontsize=8, transform=ax.transAxes)
        
        ax.set_title('Recent Debug Logs', fontsize=12)
        ax.set_axis_off()
    
    def _draw_performance_metrics(self, ax):
        """Draw performance metrics on subplot."""
        status_report = self.system_monitor.create_system_status_report()
        
        if 'current_status' in status_report:
            metrics = status_report['current_status']
            ax.bar(['CPU', 'Memory', 'Disk'], 
                   [metrics['cpu_usage'], metrics['memory_usage'], metrics['disk_usage']],
                   color=['red', 'blue', 'green'], alpha=0.7)
            ax.set_ylabel('Usage (%)')
            ax.set_title('Current System Load', fontsize=12)
            ax.set_ylim(0, 100)
            ax.grid(True, alpha=0.3)
        
        else:
            ax.text(0.5, 0.5, 'No performance data\navailable', 
                   horizontalalignment='center', verticalalignment='center',
                   transform=ax.transAxes, fontsize=12)
            ax.set_axis_off()
    
    def _draw_control_panel(self, ax):
        """Draw control panel on subplot."""
        controls = [
            'Start/Stop Visualization',
            'Toggle Animation',
            'Export Data',
            'Clear Logs',
            'Save Screenshot',
            'Show Configuration'
        ]
        
        for i, control in enumerate(controls):
            ax.text(0.1, 0.9 - i * 0.15, control,
                   fontsize=10, transform=ax.transAxes,
                   bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        ax.set_title('Controls', fontsize=12)
        ax.set_axis_off()
    
    def start_auto_update(self, interval: int = 1000):
        """Start automatic data updates."""
        if self.update_timer is None:
            self.update_timer = threading.Timer(interval / 1000.0, self._update_loop)
            self.update_timer.daemon = True
            self.update_timer.start()
    
    def _update_loop(self):
        """Update loop for real-time updates."""
        if not self.is_running:
            return
        
        # Update system monitor metrics
        self.system_monitor.update_brain_metrics(self)
        
        # Schedule next update
        self.update_timer = threading.Timer(self.config.animation_interval / 1000.0, self._update_loop)
        self.update_timer.daemon = True
        self.update_timer.start()
    
    def stop_auto_update(self):
        """Stop automatic updates."""
        if self.update_timer:
            self.update_timer.cancel()
            self.update_timer = None
    
    def export_all(self, output_dir: str):
        """Export all visualizations to files."""
        os.makedirs(output_dir, exist_ok=True)
        
        # Export neural visualization
        neural_file = os.path.join(output_dir, 'neural_activity.png')
        self.neural_visualizer.save_visualization(neural_file)
        
        # Export memory visualization
        memory_file = os.path.join(output_dir, 'memory_traces.png')
        self.memory_visualizer.save_visualization(memory_file)
        
        # Export system metrics
        metrics_file = os.path.join(output_dir, 'system_metrics.csv')
        self.system_monitor.export_metrics(metrics_file)
        
        # Export debug logs
        logs_file = os.path.join(output_dir, 'debug_logs.json')
        self.debug_logger.export_logs(logs_file, 'json')
    
    def log_debug_event(self, level: DebugLevel, source: str, message: str,
                       data: Optional[Dict[str, Any]] = None):
        """Log debug event."""
        self.debug_logger.log(level, source, message, data)

# Interactive GUI wrapper (for Qt-based interface)
class NLMInteractiveGUI(QMainWindow):
    """Interactive GUI for NLM visualization and debugging."""
    
    def __init__(self, visualizer: NLMVisualizer):
        super().__init__()
        self.visualizer = visualizer
        self.setWindowTitle("NLM Visualization and Debugging Tool")
        self.setGeometry(100, 100, 1600, 900)
        
        self.setup_ui()
    
    def setup_ui(self):
        """Setup user interface."""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        main_layout = QHBoxLayout(central_widget)
        
        # Control panel
        control_panel = self._create_control_panel()
        main_layout.addLayout(control_panel, 1)
        
        # Visualization area
        viz_area = self._create_visualization_area()
        main_layout.addWidget(viz_area, 3)
        
        # Debug log area
        debug_area = self._create_debug_area()
        main_layout.addWidget(debug_area, 2)
    
    def _create_control_panel(self) -> QHBoxLayout:
        """Create control panel."""
        panel = QGroupBox("Controls")
        layout = QVBoxLayout()
        
        # Visualization controls
        viz_group = QGroupBox("Visualization")
        viz_layout = QVBoxLayout()
        
        self.viz_type_combo = QComboBox()
        self.viz_type_combo.addItems(['Neural Activity', 'Memory Traces', 
                                    'System Monitoring', 'Comprehensive Dashboard'])
        viz_layout.addWidget(QLabel("Visualization Type:"))
        viz_layout.addWidget(self.viz_type_combo)
        
        self.animate_checkbox = QCheckBox("Auto Animate")
        viz_layout.addWidget(self.animate_checkbox)
        
        self.real_time_checkbox = QCheckBox("Real-time Updates")
        viz_layout.addWidget(self.real_time_checkbox)
        
        viz_group.setLayout(viz_layout)
        layout.addWidget(viz_group)
        
        # Control buttons
        button_group = QGroupBox("Actions")
        button_layout = QVBoxLayout()
        
        self.start_button = QPushButton("Start Visualization")
        self.start_button.clicked.connect(self.start_visualization)
        button_layout.addWidget(self.start_button)
        
        self.stop_button = QPushButton("Stop Visualization")
        self.stop_button.clicked.connect(self.stop_visualization)
        button_layout.addWidget(self.stop_button)
        
        self.export_button = QPushButton("Export All")
        self.export_button.clicked.connect(self.export_all)
        button_layout.addWidget(self.export_button)
        
        button_group.setLayout(button_layout)
        layout.addWidget(button_group)
        
        # Debug controls
        debug_group = QGroupBox("Debug Level")
        debug_layout = QVBoxLayout()
        
        self.debug_level_combo = QComboBox()
        self.debug_level_combo.addItems(['ERROR', 'WARNING', 'INFO', 'DEBUG', 'TRACE'])
        debug_layout.addWidget(QLabel("Log Level:"))
        debug_layout.addWidget(self.debug_level_combo)
        
        self.clear_logs_button = QPushButton("Clear Logs")
        self.clear_logs_button.clicked.connect(self.clear_logs)
        debug_layout.addWidget(self.clear_logs_button)
        
        debug_group.setLayout(debug_layout)
        layout.addWidget(debug_group)
        
        panel.setLayout(layout)
        return layout
    
    def _create_visualization_area(self) -> QWidget:
        """Create visualization area."""
        area = QWidget()
        layout = QVBoxLayout()
        
        # Create matplotlib canvas (simplified for demo)
        self.viz_label = QLabel("Visualization will appear here")
        self.viz_label.setAlignment(Qt.AlignCenter)
        self.viz_label.setStyleSheet("background-color: white; border: 1px solid gray;")
        layout.addWidget(self.viz_label)
        
        # Status label
        self.status_label = QLabel("Status: Inactive")
        layout.addWidget(self.status_label)
        
        area.setLayout(layout)
        return area
    
    def _create_debug_area(self) -> QWidget:
        """Create debug log area."""
        area = QWidget()
        layout = QVBoxLayout()
        
        self.debug_text = QTextEdit()
        self.debug_text.setReadOnly(True)
        layout.addWidget(QLabel("Debug Logs:"))
        layout.addWidget(self.debug_text)
        
        # Filter controls
        filter_layout = QHBoxLayout()
        filter_layout.addWidget(QLabel("Filter by level:"))
        self.level_filter = QComboBox()
        self.level_filter.addItems(['All', 'ERROR', 'WARNING', 'INFO', 'DEBUG', 'TRACE'])
        filter_layout.addWidget(self.level_filter)
        
        self.filter_button = QPushButton("Apply Filter")
        self.filter_button.clicked.connect(self.apply_log_filter)
        filter_layout.addWidget(self.filter_button)
        
        layout.addLayout(filter_layout)
        
        area.setLayout(layout)
        return area
    
    def start_visualization(self):
        """Start visualization."""
        self.visualizer.is_running = True
        self.visualizer.start_auto_update()
        self.start_button.setEnabled(False)
        self.stop_button.setEnabled(True)
        self.status_label.setText("Status: Active")
    
    def stop_visualization(self):
        """Stop visualization."""
        self.visualizer.is_running = False
        self.visualizer.stop_auto_update()
        self.start_button.setEnabled(True)
        self.stop_button.setEnabled(False)
        self.status_label.setText("Status: Inactive")
    
    def export_all(self):
        """Export all data."""
        # Simple file dialog would go here
        output_dir = "./exports"
        os.makedirs(output_dir, exist_ok=True)
        
        self.visualizer.export_all(output_dir)
        self.status_label.setText(f"Status: Exported to {output_dir}")
    
    def clear_logs(self):
        """Clear debug logs."""
        # This would clear the visualizer's debug log
        self.debug_text.clear()
    
    def apply_log_filter(self):
        """Apply log filter."""
        # This would filter the debug logs
        pass
    
    def update_visualization(self, neural_data: Dict[str, Any], memory_data: Dict[str, Any]):
        """Update visualization with new data."""
        self.visualizer.update_data(neural_data, memory_data)
        
        # Update matplotlib display (simplified)
        self.viz_label.setText("New neural and memory data received")
        
        # Update debug log
        current_logs = self.visualizer.debug_logger.get_logs()
        self.debug_text.clear()
        for log in current_logs[-20:]:  # Last 20 logs
            self.debug_text.append(f"[{log.timestamp:.2f}] {log.level.value:6s} {log.source:20s}: {log.message}")

def create_default_visualizer_config() -> VisualizationConfig:
    """Create default visualization configuration."""
    return VisualizationConfig(
        visualization_type=VisualizationType.COMPREHENSIVE,
        width=1600,
        height=900,
        show_grid=True,
        show_labels=True,
        animate=True,
        animation_interval=100,
        real_time_update=True,
        interactive=True
    )

def create_interactive_gui(brain: Any) -> NLMInteractiveGUI:
    """Create interactive GUI for NLM visualization and debugging."""
    # Create visualizer
    config = create_default_visualizer_config()
    visualizer = NLMVisualizer(config)
    
    # Start system monitoring
    visualizer.start_system_monitoring()
    
    # Update with initial brain metrics
    visualizer.update_brain_metrics(brain)
    
    # Create GUI
    app = QApplication.instance() or QApplication(sys.argv)
    gui = NLMInteractiveGUI(visualizer)
    
    return gui

# Example usage functions

def example_neural_visualization():
    """Example of neural visualization."""
    # Create sample neural data
    np.random.seed(42)
    n_neurons = 100
    
    neuron_data = {
        'positions': np.random.rand(n_neurons, 2) * 10,
        'activations': np.random.rand(n_neurons),
        'types': np.random.randint(0, 6, n_neurons)
    }
    
    # Create visualizer
    config = create_default_visualizer_config()
    visualizer = NeuralActivityVisualizer(config)
    visualizer.setup_figure()
    visualizer.update_neural_data(neuron_data)
    visualizer.draw_neural_network()
    
    return visualizer

def example_memory_visualization():
    """Example of memory trace visualization."""
    # Create sample memory data
    time_steps = 100
    n_traces = 5
    
    memory_data = {
        'time': list(range(time_steps)),
        'traces': {
            'working_memory': [np.random.rand(time_steps) * 0.5 for _ in range(n_traces)],
            'episodic_memory': [np.random.rand(time_steps) * 0.3 for _ in range(n_traces)],
            'associative_memory': [np.random.rand(time_steps) * 0.8 for _ in range(n_traces)]
        }
    }
    
    # Create visualizer
    config = create_default_visualizer_config()
    visualizer = MemoryTraceVisualizer(config)
    visualizer.setup_figure()
    visualizer.update_memory_data(memory_data)
    visualizer.draw_memory_traces()
    
    return visualizer

def example_system_monitoring():
    """Example of system monitoring."""
    monitor = SystemMonitor()
    
    # Start monitoring
    monitor.start_monitoring()
    
    # Simulate some system activity
    for i in range(60):
        time.sleep(1)
        # Simulate system metrics
        monitor.cpu_usage.append(np.random.randint(20, 80))
        monitor.memory_usage.append(np.random.randint(30, 90))
        monitor.disk_usage.append(np.random.randint(10, 60))
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Create plots
    fig = monitor.create_performance_plot()
    
    return monitor, fig

def example_debug_logging():
    """Example of debug logging."""
    logger = DebugLogger("nlm_debug_example.log", DebugLevel.DEBUG)
    
    # Log various events
    logger.debug("NeuralSimulator", "Neuron initialized", {"neuron_id": 1, "type": "excitatory"})
    logger.info("MemorySystem", "Memory allocated", {"memory_type": "working", "size": 1000})
    logger.warning("PlasticitySystem", "High plasticity detected", {"plastic": 0.95})
    logger.error("AgentSystem", "Action selection failed", {"error_code": "E001"})
    
    # Get log summary
    summary = logger.get_log_summary()
    print(f"Log summary: {summary}")
    
    # Export logs
    logger.export_logs("exported_logs.json", "json")
    
    return logger

# Main example function
def main_example():
    """Run complete example demonstrating all visualization and debugging features."""
    print("=== NLM Visualization and Debugging Example ===\n")
    
    # Example 1: Neural visualization
    print("1. Creating neural activity visualization...")
    neural_viz = example_neural_visualization()
    neural_viz.show()
    
    # Example 2: Memory visualization  
    print("2. Creating memory trace visualization...")
    memory_viz = example_memory_visualization()
    memory_viz.show()
    
    # Example 3: System monitoring
    print("3. Creating system monitoring...")
    monitor, perf_fig = example_system_monitoring()
    plt.show()
    
    # Example 4: Debug logging
    print("4. Setting up debug logging...")
    logger = example_debug_logging()
    
    # Example 5: Comprehensive dashboard
    print("5. Creating comprehensive dashboard...")
    
    # Create comprehensive visualizer
    config = create_default_visualizer_config()
    viz = NLMVisualizer(config)
    viz.setup_all()
    
    # Start monitoring
    viz.start_system_monitoring()
    
    print("\n=== Example Complete ===")
    print("All visualization and debugging examples have been demonstrated.")
    print("Check the generated files in the current directory.")

if __name__ == "__main__":
    main_example()