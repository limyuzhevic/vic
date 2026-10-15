#!/usr/bin/env python3
"""
NLM Visualization and Debugging Utilities

Advanced visualization and debugging tools for NLM development and analysis.
"""

import sys
import json
import time
import threading
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from typing import Dict, List, Optional, Any, Tuple, Callable
from dataclasses import dataclass, field
from enum import Enum
import subprocess
import os

# Import NLM modules
import pynlm
from python.convenience_api import createQuickBrain

__all__ = [
    'VisualizationManager',
    'Debugger',
    'NeuralVisualizer',
    'MemoryDebugger',
    'PerformanceDebugger',
    'TimelineDebugger',
    'createVisualizationManager',
    'createDebugger',
    'createNeuralVisualizer',
    'createMemoryDebugger',
    'createPerformanceDebugger'
]

class DebugLevel(Enum):
    """Debug levels."""
    ERROR = 1
    WARNING = 2
    INFO = 3
    DEBUG = 4
    TRACE = 5
class LogEntry:
    """Log entry for debugging."""
    
    def __init__(self, level: DebugLevel, message: str, 
                 timestamp: float = None, context: Dict = None,
                 stack_trace: str = None):
        self.level = level
        self.message = message
        self.timestamp = timestamp or time.time()
        self.context = context or {}
        self.stack_trace = stack_trace
        self.thread_id = threading.current_thread().ident
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'level': self.level.value,
            'level_name': self.level.name,
            'message': self.message,
            'timestamp': self.timestamp,
            'context': self.context,
            'thread_id': self.thread_id
        }
class VisualizationManager:
    """
    Visualization manager for NLM.
    
    This class provides visualization capabilities including:
    - Real-time visualization
    - Historical visualization
    - Comparative visualization
    - Interactive visualization
    """
    
    def __init__(self, brain: pynlm.Brain, figsize: Tuple[int, int] = (12, 8)):
        """
        Initialize visualization manager.
        
        Args:
            brain: Brain to visualize
            figsize: Default figure size
        """
        self.brain = brain
        self.figsize = figsize
        self.visualizations = {}
        self.real_time_updates = []
        self.visualization_queue = []
        self.monitoring = False
        self.monitor_thread = None
        self.fig = None
        self.axes = None
    
    def create_brain_visualization(self, visualization_type: str = "network"):
        """
        Create brain visualization.
        
        Args:
            visualization_type: Type of visualization
            
        Returns:
            Visualization figure
        """
        if visualization_type == "network":
            return self._create_network_visualization()
        elif visualization_type == "activity":
            return self._create_activity_visualization()
        elif visualization_type == "topology":
            return self._create_topology_visualization()
        elif visualization_type == "connectivity":
            return self._create_connectivity_visualization()
        else:
            raise ValueError(f"Unknown visualization type: {visualization_type}")
    
    def _create_network_visualization(self):
        """Create neural network visualization."""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        # Plot 1: Region distribution
        self._plot_region_distribution(axes[0])
        
        # Plot 2: Neuron types
        self._plot_neuron_types(axes[1])
        
        # Plot 3: Activity over time
        self._plot_activity_time_series(axes[2])
        
        # Plot 4: Connectivity matrix
        self._plot_connectivity_matrix(axes[3])
        
        plt.tight_layout()
        return fig
    
    def _create_activity_visualization(self):
        """Create activity visualization."""
        fig, axes = plt.subplots(3, 1, figsize=self.figsize, sharex=True)
        
        # Plot 1: Firing rate
        self._plot_firing_rate(axes[0])
        
        # Plot 2: Spike count
        self._plot_spike_count(axes[1])
        
        # Plot 3: E/I ratio
        self._plot_ei_ratio(axes[2])
        
        plt.tight_layout()
        return fig
    
    def _create_topology_visualization(self):
        """Create topology visualization."""
        fig = plt.figure(figsize=self.figsize)
        
        # Create networkx graph if available
        try:
            import networkx as nx
            
            # Create graph
            G = nx.Graph()
            
            # Add regions as nodes
            region_ids = self.brain.getRegionIds()
            for region_id in region_ids:
                G.add_node(f"Region {region_id}")
            
            # Add connections
            for region_id in region_ids:
                region = self.brain.getRegion(region_id)
                if region:
                    neurons = region.getAllNeurons()
                    for neuron in neurons:
                        # Add neuron connections (simplified)
                        pass
            
            # Draw graph
            pos = nx.spring_layout(G)
            nx.draw_networkx_nodes(G, pos, node_color='lightblue', node_size=700)
            nx.draw_networkx_edges(G, pos, alpha=0.5)
            nx.draw_networkx_labels(G, pos)
            
            plt.title("Neural Network Topology")
            
        except ImportError:
            # Fallback to matplotlib scatter plot
            self._create_topology_fallback(fig)
        
        return fig
    
    def _create_connectivity_visualization(self):
        """Create connectivity visualization."""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        # Plot 1: Connection density
        self._plot_connection_density(axes[0])
        
        # Plot 2: Connection types
        self._plot_connection_types(axes[1])
        
        # Plot 3: Connection strength
        self._plot_connection_strength(axes[2])
        
        # Plot 4: Connection patterns
        self._plot_connection_patterns(axes[3])
        
        plt.tight_layout()
        return fig
    
    def _plot_region_distribution(self, ax):
        """Plot region distribution."""
        region_ids = self.brain.getRegionIds()
        neuron_counts = []
        
        for region_id in region_ids:
            region = self.brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                neuron_counts.append(len(neurons))
            else:
                neuron_counts.append(0)
        
        colors = ['lightblue', 'lightgreen', 'lightcoral', 'gold', 'plum']
        bars = ax.bar(range(len(region_ids)), neuron_counts, 
                     color=[colors[i % len(colors)] for i in range(len(region_ids))])
        
        ax.set_xlabel("Region ID")
        ax.set_ylabel("Neuron Count")
        ax.set_title("Region Distribution")
        ax.set_xticks(range(len(region_ids)))
        ax.set_xticklabels([str(rid) for rid in region_ids])
        
        # Add value labels
        for bar, count in zip(bars, neuron_counts):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{count}', ha='center', va='bottom')
    
    def _plot_neuron_types(self, ax):
        """Plot neuron types."""
        # Get neuron type counts (simplified)
        neuron_types = ['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Modulatory', 'Internal']
        counts = [100, 50, 20, 10, 5, 15]  # Placeholder data
        
        colors = ['red', 'blue', 'green', 'orange', 'purple', 'brown']
        
        ax.pie(counts, labels=neuron_types, colors=colors, autopct='%1.1f%%')
        ax.set_title("Neuron Type Distribution")
    
    def _plot_activity_time_series(self, ax):
        """Plot activity time series."""
        # Generate sample data
        time_steps = np.arange(0, 100, 1)
        firing_rate = np.random.normal(50, 10, len(time_steps))
        spikes = np.cumsum(np.random.poisson(2, len(time_steps)))
        
        # Plot firing rate
        ax.plot(time_steps, firing_rate, label='Firing Rate', color='blue')
        ax.set_ylabel('Firing Rate')
        ax.set_title('Activity Over Time')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_connectivity_matrix(self, ax):
        """Plot connectivity matrix."""
        region_ids = self.brain.getRegionIds()
        n_regions = len(region_ids)
        
        # Create sample connectivity matrix
        connectivity = np.random.rand(n_regions, n_regions) * 0.1
        np.fill_diagonal(connectivity, 0)
        
        # Plot heatmap
        sns.heatmap(connectivity, annot=True, fmt='.2f', cmap='Blues',
                   xticklabels=[str(rid) for rid in region_ids],
                   yticklabels=[str(rid) for rid in region_ids],
                   ax=ax)
        ax.set_title("Connectivity Matrix")
        ax.set_xlabel("Target Region")
        ax.set_ylabel("Source Region")
    
    def _plot_firing_rate(self, ax):
        """Plot firing rate."""
        # Generate sample data
        time_steps = np.arange(0, 100, 1)
        firing_rates = np.random.normal(50, 10, len(time_steps))
        
        ax.plot(time_steps, firing_rates, color='blue')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Firing Rate")
        ax.set_title("Firing Rate Over Time")
        ax.grid(True, alpha=0.3)
    
    def _plot_spike_count(self, ax):
        """Plot spike count."""
        # Generate sample data
        time_steps = np.arange(0, 100, 1)
        spike_counts = np.cumsum(np.random.poisson(3, len(time_steps)))
        
        ax.plot(time_steps, spike_counts, color='red')
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Spike Count")
        ax.set_title("Spike Count Over Time")
        ax.grid(True, alpha=0.3)
    
    def _plot_ei_ratio(self, ax):
        """Plot E/I ratio."""
        # Generate sample data
        time_steps = np.arange(0, 100, 1)
        ei_ratio = np.random.normal(1.0, 0.2, len(time_steps))
        
        ax.plot(time_steps, ei_ratio, color='green')
        ax.axhline(y=1.0, color='gray', linestyle='--', alpha=0.5)
        ax.set_xlabel("Time Step")
        ax.set_ylabel("E/I Ratio")
        ax.set_title("Excitation/Inhibition Ratio")
        ax.grid(True, alpha=0.3)
    
    def _plot_connection_density(self, ax):
        """Plot connection density."""
        region_ids = self.brain.getRegionIds()
        
        # Calculate connection densities
        densities = []
        for region_id in region_ids:
            region = self.brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                density = len(neurons) / max(1, len(neuron_ids) * len(neurons))
                densities.append(density)
            else:
                densities.append(0.0)
        
        ax.bar(range(len(region_ids)), densities, color='lightblue')
        ax.set_xlabel("Region ID")
        ax.set_ylabel("Connection Density")
        ax.set_title("Connection Density per Region")
        ax.set_xticks(range(len(region_ids)))
        ax.set_xticklabels([str(rid) for rid in region_ids])
    
    def _plot_connection_types(self, ax):
        """Plot connection types."""
        connection_types = ['Excitatory', 'Inhibitory', 'Electrical', 'Gap Junction']
        counts = [60, 30, 10, 5]  # Placeholder data
        
        colors = ['red', 'blue', 'green', 'orange']
        
        ax.pie(counts, labels=connection_types, colors=colors, autopct='%1.1f%%')
        ax.set_title("Connection Type Distribution")
    
    def _plot_connection_strength(self, ax):
        """Plot connection strength."""
        # Generate sample data
        region_ids = self.brain.getRegionIds()
        strength_data = np.random.rand(len(region_ids), len(region_ids)) * 0.5
        
        sns.heatmap(strength_data, annot=True, fmt='.2f', cmap='RdYlBu_r',
                   xticklabels=[str(rid) for rid in region_ids],
                   yticklabels=[str(rid) for rid in region_ids],
                   ax=ax)
        ax.set_title("Connection Strength")
        ax.set_xlabel("Target Region")
        ax.set_ylabel("Source Region")
    
    def _plot_connection_patterns(self, ax):
        """Plot connection patterns."""
        # Generate sample data
        time_steps = np.arange(0, 50, 1)
        connection_patterns = np.random.rand(len(time_steps), 3)
        
        for i in range(connection_patterns.shape[1]):
            ax.plot(time_steps, connection_patterns[:, i], 
                   label=f'Pattern {i+1}')
        
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Connection Pattern")
        ax.set_title("Connection Patterns Over Time")
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _create_topology_fallback(self, fig):
        """Create topology visualization fallback."""
        # Simple scatter plot of regions
        region_ids = self.brain.getRegionIds()
        neuron_counts = []
        
        for region_id in region_ids:
            region = self.brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                neuron_counts.append(len(neurons))
            else:
                neuron_counts.append(0)
        
        ax = fig.add_subplot(111)
        ax.scatter(range(len(region_ids)), neuron_counts, s=np.array(neuron_counts) * 10, alpha=0.6)
        ax.set_xlabel("Region ID")
        ax.set_ylabel("Neuron Count")
        ax.set_title("Neural Network Topology (Simplified)")
        ax.set_xticks(range(len(region_ids)))
        ax.set_xticklabels([str(rid) for rid in region_ids])

class Debugger:
    """
    Debugger for NLM.
    
    This class provides debugging capabilities including:
    - Exception debugging
    - Performance debugging
    - Memory debugging
    - State debugging
    """
    
    def __init__(self, brain: pynlm.Brain, log_file: Optional[str] = None):
        """
        Initialize debugger.
        
        Args:
            brain: Brain to debug
            log_file: Path to log file
        """
        self.brain = brain
        self.log_file = log_file
        self.log_entries = []
        self.debug_level = DebugLevel.INFO
        self.breakpoints = []
        self.watches = {}
        self.exception_handlers = {}
    
    def log(self, level: DebugLevel, message: str, context: Dict = None):
        """
        Log message.
        
        Args:
            level: Log level
            message: Message
            context: Context information
        """
        if level.value < self.debug_level.value:
            return
        
        entry = LogEntry(level, message, context=context)
        self.log_entries.append(entry)
        
        # Print to console
        level_name = level.name
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry.timestamp))
        print(f"[{timestamp}] {level_name}: {message}")
        
        # Write to file
        if self.log_file:
            with open(self.log_file, 'a') as f:
                f.write(f"[{timestamp}] {level_name}: {message}\n")
    
    def set_debug_level(self, level: DebugLevel):
        """Set debug level."""
        self.debug_level = level
    
    def add_breakpoint(self, condition: Callable):
        """Add breakpoint."""
        self.breakpoints.append(condition)
    
    def add_watch(self, name: str, expression: Callable):
        """Add watch expression."""
        self.watches[name] = expression
    
    def add_exception_handler(self, exception_type: type, handler: Callable):
        """Add exception handler."""
        self.exception_handlers[exception_type] = handler
    
    def debug_step(self, step_name: str = "step"):
        """
        Debug one step.
        
        Args:
            step_name: Name of step
        """
        self.log(DebugLevel.DEBUG, f"Debugging step: {step_name}")
        
        # Check breakpoints
        for i, condition in enumerate(self.breakpoints):
            try:
                if condition(self.brain, step_name):
                    self.log(DebugLevel.WARNING, f"Breakpoint triggered at step {step_name}")
                    input("Press Enter to continue...")
            except Exception as e:
                self.log(DebugLevel.ERROR, f"Error in breakpoint condition: {e}")
        
        # Debug step
        try:
            self.brain.step(step_name)
            self.log(DebugLevel.DEBUG, f"Step {step_name} completed")
        except Exception as e:
            self.log(DebugLevel.ERROR, f"Error in step {step_name}: {e}")
            
            # Try exception handlers
            for exc_type, handler in self.exception_handlers.items():
                if isinstance(e, exc_type):
                    try:
                        handler(e, step_name)
                        self.log(DebugLevel.INFO, f"Exception handled by {exc_type.__name__}")
                        break
                    except Exception as handler_error:
                        self.log(DebugLevel.ERROR, f"Error in exception handler: {handler_error}")
    
    def watch_expressions(self):
        """Evaluate and display watch expressions."""
        for name, expression in self.watches.items():
            try:
                result = expression(self.brain)
                self.log(DebugLevel.INFO, f"Watch '{name}': {result}")
            except Exception as e:
                self.log(DebugLevel.ERROR, f"Error evaluating watch '{name}': {e}")
    
    def inspect_brain_state(self) -> Dict[str, Any]:
        """Inspect brain state."""
        state = {
            'timestamp': time.time(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage().value if hasattr(self.brain.getDevelopmentalStage(), 'value') else str(self.brain.getDevelopmentalStage())
        }
        
        return state
    
    def generate_debug_report(self, output_path: Optional[str] = None) -> str:
        """
        Generate debug report.
        
        Args:
            output_path: Path to save report
            
        Returns:
            Debug report
        """
        report_lines = []
        report_lines.append("NLM Debug Report")
        report_lines.append("=" * 50)
        report_lines.append(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report_lines.append(f"Debug Level: {self.debug_level.name}")
        report_lines.append(f"Total Log Entries: {len(self.log_entries)}")
        report_lines.append(f"Breakpoints: {len(self.breakpoints)}")
        report_lines.append(f"Watch Expressions: {len(self.watches)}")
        report_lines.append("")
        
        # Add log entries
        report_lines.append("Log Entries:")
        for entry in self.log_entries[-20:]:  # Last 20 entries
            timestamp = time.strftime('%H:%M:%S', time.localtime(entry.timestamp))
            level_name = entry.level.name
            report_lines.append(f"  [{timestamp}] {level_name}: {entry.message}")
        
        report_lines.append("")
        
        # Add brain state
        report_lines.append("Brain State:")
        brain_state = self.inspect_brain_state()
        for key, value in brain_state.items():
            report_lines.append(f"  {key}: {value}")
        
        report = "\n".join(report_lines)
        
        if output_path:
            with open(output_path, 'w') as f:
                f.write(report)
        
        return report

class NeuralVisualizer:
    """
    Neural network visualizer for NLM.
    
    This class provides neural network visualization including:
    - Neural network diagrams
    - Activity visualization
    - Learning progress visualization
    - Performance monitoring
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize neural visualizer.
        
        Args:
            brain: Brain to visualize
        """
        self.brain = brain
        self.visualization_history = []
        self.current_visualization = None
    
    def create_neural_diagram(self, save_path: Optional[str] = None) -> plt.Figure:
        """
        Create neural diagram.
        
        Args:
            save_path: Path to save diagram
            
        Returns:
            Neural diagram figure
        """
        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        
        # Plot 1: Network structure
        self._plot_network_structure(axes[0])
        
        # Plot 2: Activity distribution
        self._plot_activity_distribution(axes[1])
        
        # Plot 3: Learning progress
        self._plot_learning_progress(axes[2])
        
        plt.suptitle("Neural Network Visualization")
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        self.visualization_history.append({
            'timestamp': time.time(),
            'type': 'neural_diagram',
            'path': save_path
        })
        
        return fig
    
    def _plot_network_structure(self, ax):
        """Plot network structure."""
        region_ids = self.brain.getRegionIds()
        
        # Create network graph
        pos = {}
        for i, region_id in enumerate(region_ids):
            pos[f"Region {region_id}"] = (i, 0)
        
        # Draw nodes
        node_colors = ['lightblue'] * len(region_ids)
        node_sizes = []
        
        for region_id in region_ids:
            region = self.brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                node_sizes.append(len(neurons) * 10)
            else:
                node_sizes.append(100)
        
        for i, (node, coords) in enumerate(pos.items()):
            ax.scatter(coords[0], coords[1], s=node_sizes[i], 
                      c=node_colors[i], alpha=0.7, edgecolors='black')
            ax.annotate(node, coords, ha='center', va='center')
        
        # Draw connections
        for i, region_id1 in enumerate(region_ids):
            for j, region_id2 in enumerate(region_ids):
                if i != j:
                    # Simple connection lines
                    ax.plot([i, j], [0, 0], 'gray', alpha=0.3, linewidth=0.5)
        
        ax.set_xlim(-1, len(region_ids))
        ax.set_ylim(-1, 1)
        ax.axis('off')
        ax.set_title("Network Structure")
    
    def _plot_activity_distribution(self, ax):
        """Plot activity distribution."""
        # Generate sample data
        neuron_types = ['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Modulatory', 'Internal']
        activities = np.random.rand(len(neuron_types)) * 100
        
        bars = ax.bar(neuron_types, activities, color=['red', 'blue', 'green', 'orange', 'purple', 'brown'])
        
        ax.set_ylabel('Activity Level')
        ax.set_title('Activity Distribution')
        ax.set_ylim(0, 100)
        
        # Add value labels
        for bar, height in zip(bars, activities):
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{height:.1f}', ha='center', va='bottom')
    
    def _plot_learning_progress(self, ax):
        """Plot learning progress."""
        # Generate sample data
        epochs = np.arange(0, 100, 1)
        performance = np.random.rand(len(epochs)) * 100
        
        ax.plot(epochs, performance, linewidth=2, color='blue')
        ax.set_xlabel('Epoch')
        ax.set_ylabel('Performance')
        ax.set_title('Learning Progress')
        ax.grid(True, alpha=0.3)
        
        # Add trend line
        z = np.polyfit(epochs, performance, 1)
        p = np.poly1d(z)
        ax.plot(epochs, p(epochs), "r--", alpha=0.5, label=f'Trend: {z[0]:.3f}x + {z[1]:.1f}')
        ax.legend()
class MemoryDebugger:
    """
    Memory debugger for NLM.
    
    This class provides memory debugging capabilities including:
    - Memory leak detection
    - Memory usage tracking
    - Garbage collection monitoring
    - Memory optimization suggestions
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize memory debugger.
        
        Args:
            brain: Brain to debug
        """
        self.brain = brain
        self.memory_history = []
        self.garbage_collection_stats = {
            'collections': 0,
            'collected_objects': 0,
            'collection_times': []
        }
        self.memory_pools = {}
    
    def track_memory_usage(self, interval: float = 1.0, duration: float = 60.0):
        """
        Track memory usage over time.
        
        Args:
            interval: Tracking interval in seconds
            duration: Total tracking duration in seconds
            
        Returns:
            Memory usage history
        """
        start_time = time.time()
        
        while time.time() - start_time < duration:
            memory_info = self._get_memory_info()
            self.memory_history.append(memory_info)
            
            time.sleep(interval)
        
        return self.memory_history
    
    def detect_memory_leaks(self) -> Dict[str, Any]:
        """
        Detect memory leaks.
        
        Returns:
            Leak detection results
        """
        if len(self.memory_history) < 10:
            return {'status': 'insufficient_data', 'leaks': []}
        
        # Analyze memory trends
        usage_trends = []
        for i in range(1, len(self.memory_history)):
            prev = self.memory_history[i-1]['rss_mb']
            curr = self.memory_history[i]['rss_mb']
            trend = curr - prev
            usage_trends.append(trend)
        
        # Detect leaks (memory increase over time)
        leaks = []
        if len(usage_trends) > 20:
            # Calculate linear regression slope
            x = list(range(len(usage_trends)))
            y = usage_trends
            
            if len(x) > 1:
                correlation = np.corrcoef(x, y)[0, 1] if len(x) > 1 else 0.0
                slope = np.polyfit(x, y, 1)[0] if len(x) > 1 else 0.0
                
                if correlation > 0.7 and slope > 0.1:
                    leaks.append({
                        'severity': 'high',
                        'slope': slope,
                        'correlation': correlation,
                        'description': 'Steady memory increase over time'
                    })
                elif correlation > 0.5 and slope > 0.05:
                    leaks.append({
                        'severity': 'medium',
                        'slope': slope,
                        'correlation': correlation,
                        'description': 'Moderate memory increase'
                    })
        
        return {
            'status': 'analyzed',
            'leaks': leaks,
            'total_samples': len(self.memory_history),
            'time_span': self.memory_history[-1]['timestamp'] - self.memory_history[0]['timestamp']
        }
    
    def optimize_memory_usage(self) -> Dict[str, Any]:
        """
        Optimize memory usage.
        
        Returns:
            Optimization results
        """
        results = {
            'before_optimization': self._get_memory_summary(),
            'optimization_actions': [],
            'after_optimization': None
        }
        
        # Collect garbage
        gc.collect()
        self.garbage_collection_stats['collections'] += 1
        
        # Clear memory pools
        for pool_name, pool in self.memory_pools.items():
            pool.clear()
            results['optimization_actions'].append(f"Cleared memory pool: {pool_name}")
        
        # Force garbage collection again
        gc.collect()
        
        results['after_optimization'] = self._get_memory_summary()
        
        return results
    
    def _get_memory_info(self) -> Dict[str, Any]:
        """Get memory information."""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            
            return {
                'timestamp': time.time(),
                'rss_mb': memory_info.rss / 1024 / 1024,
                'vms_mb': memory_info.vms / 1024 / 1024,
                'shared_mb': memory_info.shared / 1024 / 1024 if hasattr(memory_info, 'shared') else 0.0,
                'text_mb': memory_info.text / 1024 / 1024 if hasattr(memory_info, 'text') else 0.0,
                'data_mb': memory_info.data / 1024 / 1024 if hasattr(memory_info, 'data') else 0.0,
                'dirty_mb': memory_info.dirty / 1024 / 1024 if hasattr(memory_info, 'dirty') else 0.0
            }
        except:
            return {
                'timestamp': time.time(),
                'rss_mb': 0.0,
                'vms_mb': 0.0,
                'shared_mb': 0.0,
                'text_mb': 0.0,
                'data_mb': 0.0,
                'dirty_mb': 0.0
            }
    
    def _get_memory_summary(self) -> Dict[str, Any]:n        """Get memory summary."""
        if not self.memory_history:
            return {}
        
        rss_values = [entry['rss_mb'] for entry in self.memory_history]
        
        return {
            'avg_rss_mb': np.mean(rss_values),
            'max_rss_mb': np.max(rss_values),
            'min_rss_mb': np.min(rss_values),
            'std_rss_mb': np.std(rss_values),
            'total_samples': len(self.memory_history)
        }

class PerformanceDebugger:
    """
    Performance debugger for NLM.
    
    This class provides performance debugging capabilities including:
    - Performance bottleneck detection
    - Optimization recommendations
    - Performance trend analysis
    - Comparative performance analysis
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize performance debugger.
        
        Args:
            brain: Brain to debug
        """
        self.brain = brain
        self.performance_history = []
        self.benchmark_results = []
    
    def monitor_performance(self, interval: float = 1.0, duration: float = 60.0):
        """
        Monitor performance over time.
        
        Args:
            interval: Monitoring interval in seconds
            duration: Total monitoring duration in seconds
            
        Returns:
            Performance history
        """
        start_time = time.time()
        
        while time.time() - start_time < duration:
            performance = self._get_performance_metrics()
            self.performance_history.append(performance)
            
            time.sleep(interval)
        
        return self.performance_history
    
    def detect_performance_bottlenecks(self) -> Dict[str, Any]:
        """
        Detect performance bottlenecks.
        
        Returns:
            Bottleneck detection results
        """
        if len(self.performance_history) < 10:
            return {'status': 'insufficient_data', 'bottlenecks': []}
        
        bottlenecks = []
        
        # Check for slow performance
        slow_samples = [p for p in self.performance_history if p['avg_step_time'] > 0.1]
        if slow_samples:
            bottlenecks.append({
                'type': 'slow_performance',
                'severity': 'high' if len(slow_samples) > len(self.performance_history) * 0.5 else 'medium',
                'affected_samples': len(slow_samples),
                'description': 'Slow average step times (>100ms)'
            })
        
        # Check for high CPU usage
        high_cpu_samples = [p for p in self.performance_history if p['cpu_usage_percent'] > 80]
        if high_cpu_samples:
            bottlenecks.append({
                'type': 'high_cpu_usage',
                'severity': 'high' if len(high_cpu_samples) > len(self.performance_history) * 0.5 else 'medium',
                'affected_samples': len(high_cpu_samples),
                'description': 'High CPU usage (>80%)'
            })
        
        # Check for memory issues
        high_memory_samples = [p for p in self.performance_history if p['memory_usage_mb'] > 1000]
        if high_memory_samples:
            bottlenecks.append({
                'type': 'high_memory_usage',
                'severity': 'high' if len(high_memory_samples) > len(self.performance_history) * 0.5 else 'medium',
                'affected_samples': len(high_memory_samples),
                'description': 'High memory usage (>1GB)'
            })
        
        # Check for performance instability
        unstable_samples = []
        for p in self.performance_history:
            if p['avg_step_time'] > 0:
                cv = np.std(p['step_times']) / p['avg_step_time']
                if cv > 0.5:
                    unstable_samples.append(p)
        
        if unstable_samples:
            bottlenecks.append({
                'type': 'performance_instability',
                'severity': 'high' if len(unstable_samples) > len(self.performance_history) * 0.5 else 'medium',
                'affected_samples': len(unstable_samples),
                'description': 'High performance variability (CV > 0.5)'
            })
        
        return {
            'status': 'analyzed',
            'bottlenecks': bottlenecks,
            'total_samples': len(self.performance_history),
            'time_span': self.performance_history[-1]['timestamp'] - self.performance_history[0]['timestamp']
        }
    
    def generate_optimization_recommendations(self) -> List[str]:
        """
        Generate optimization recommendations.
        
        Returns:
            List of optimization recommendations
        """
        recommendations = []
        
        if not self.performance_history:
            return recommendations
        
        # Check for memory optimization opportunities
        high_memory = [p for p in self.performance_history if p['memory_usage_mb'] > 500]
        if high_memory:
            recommendations.append("Optimize memory usage - consider reducing brain size or using more efficient data structures")
        
        # Check for computation optimization opportunities
        slow_steps = [p for p in self.performance_history if p['avg_step_time'] > 0.05]
        if slow_steps:
            recommendations.append("Optimize step computation - consider vectorization or algorithmic improvements")
        
        # Check for stability improvements
        unstable = [p for p in self.performance_history if p['stability_score'] < 0.8]
        if unstable:
            recommendations.append("Improve performance stability - reduce variance in step times")
        
        # Check for resource utilization
        underutilized = [p for p in self.performance_history if p['cpu_usage_percent'] < 50]
        if underutilized:
            recommendations.append("Improve resource utilization - consider parallel processing or better load balancing")
        
        return recommendations
    
    def _get_performance_metrics(self) -> Dict[str, Any]:
        """Get performance metrics."""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            cpu_percent = process.cpu_percent()
            
            # Get brain performance metrics
            return {
                'timestamp': time.time(),
                'cpu_usage_percent': cpu_percent,
                'memory_usage_mb': memory_info.rss / 1024 / 1024,
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'avg_firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'avg_step_time': 0.1,  # Placeholder
                'step_times': [0.1, 0.12, 0.08, 0.15, 0.09],  # Placeholder
                'stability_score': 0.8,  # Placeholder
                'performance_score': 0.85  # Placeholder
            }
        except:
            return {
                'timestamp': time.time(),
                'cpu_usage_percent': 0.0,
                'memory_usage_mb': 0.0,
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'avg_firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'avg_step_time': 0.1,
                'step_times': [0.1, 0.1, 0.1, 0.1, 0.1],
                'stability_score': 0.8,
                'performance_score': 0.85
            }

class TimelineDebugger:
    """
    Timeline debugger for NLM.
    
    This class provides timeline debugging capabilities including:
    - Timeline visualization
    - Event tracking
    - Performance timeline analysis
    - Timeline comparison
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize timeline debugger.
        
        Args:
            brain: Brain to debug
        """
        self.brain = brain
        self.timeline_events = []
        self.performance_timeline = []
        self.debug_sessions = []
    
    def add_event(self, event_name: str, event_type: str, 
                 timestamp: float = None, details: Dict = None):
        """
        Add timeline event.
        
        Args:
            event_name: Event name
            event_type: Event type
            timestamp: Event timestamp
            details: Event details
        """
        if timestamp is None:
            timestamp = time.time()
        
        event = {
            'name': event_name,
            'type': event_type,
            'timestamp': timestamp,
            'details': details or {}
        }
        
        self.timeline_events.append(event)
    
    def create_performance_timeline(self, duration: float = 60.0,
                                   sampling_interval: float = 1.0):
        """
        Create performance timeline.
        
        Args:
            duration: Timeline duration in seconds
            sampling_interval: Sampling interval in seconds
            
        Returns:
            Performance timeline
        """
        start_time = time.time()
        
        while time.time() - start_time < duration:
            timeline_entry = {
                'timestamp': time.time(),
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'avg_firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                'total_spikes': self.brain.getTotalSpikeCount()
            }
            
            self.performance_timeline.append(timeline_entry)
            
            # Run step
            self.brain.step(len(self.performance_timeline))
            
            time.sleep(sampling_interval)
        
        return self.performance_timeline
    
    def start_debug_session(self, session_name: str):
        """
        Start debug session.
        
        Args:
            session_name: Session name
        """
        session = {
            'name': session_name,
            'start_time': time.time(),
            'end_time': None,
            'events': [],
            'performance_data': []
        }
        
        self.debug_sessions.append(session)
        return session
    
    def end_debug_session(self, session_name: str):
        """
        End debug session.
        
        Args:
            session_name: Session name
        """
        for session in self.debug_sessions:
            if session['name'] == session_name:
                session['end_time'] = time.time()
                return
        
        raise ValueError(f"Debug session '{session_name}' not found")
    
    def generate_timeline_report(self, output_path: Optional[str] = None) -> str:
        """
        Generate timeline report.
        
        Args:
            output_path: Path to save report
            
        Returns:
            Timeline report
        """
        report_lines = []
        report_lines.append("NLM Timeline Debug Report")
        report_lines.append("=" * 50)
        report_lines.append(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report_lines.append(f"Total Events: {len(self.timeline_events)}")
        report_lines.append(f"Total Performance Samples: {len(self.performance_timeline)}")
        report_lines.append(f"Debug Sessions: {len(self.debug_sessions)}")
        report_lines.append("")
        
        # Add timeline events
        report_lines.append("Timeline Events:")
        for event in self.timeline_events[-20:]:  # Last 20 events
            timestamp = time.strftime('%H:%M:%S', time.localtime(event['timestamp']))
            report_lines.append(f"  [{timestamp}] {event['type']}: {event['name']}")
        
        report_lines.append("")
        
        # Add performance timeline
        if self.performance_timeline:
            report_lines.append("Performance Timeline (Last 10 samples):")
            for entry in self.performance_timeline[-10:]:
                timestamp = time.strftime('%H:%M:%S', time.localtime(entry['timestamp']))
                report_lines.append(f"  [{timestamp}] "
                                 f"Neurons: {entry['neuron_count']}, "
                                 f"Firing: {entry['firing_neurons']}, "
                                 f"Avg Rate: {entry['avg_firing_rate']:.1f}")
        
        report_lines.append("")
        
        # Add debug sessions
        if self.debug_sessions:
            report_lines.append("Debug Sessions:")
            for session in self.debug_sessions:
                duration = session['end_time'] - session['start_time'] if session['end_time'] else time.time() - session['start_time']
                report_lines.append(f"  {session['name']}: "
                                 f"{duration:.1f}s, "
                                 f"{len(session['events'])} events, "
                                 f"{len(session['performance_data'])} data points")
        
        report = "\n".join(report_lines)
        
        if output_path:
            with open(output_path, 'w') as f:
                f.write(report)
        
        return report

# Convenience functions for visualization and debugging
def createVisualizationManager(brain: pynlm.Brain) -> VisualizationManager:
    """Create visualization manager."""
    return VisualizationManager(brain)

def createDebugger(brain: pynlm.Brain, log_file: Optional[str] = None) -> Debugger:
    """Create debugger."""
    return Debugger(brain, log_file)

def createNeuralVisualizer(brain: pynlm.Brain) -> NeuralVisualizer:
    """Create neural visualizer."""
    return NeuralVisualizer(brain)

def createMemoryDebugger(brain: pynlm.Brain) -> MemoryDebugger:
    """Create memory debugger."""
    return MemoryDebugger(brain)

def createPerformanceDebugger(brain: pynlm.Brain) -> PerformanceDebugger:
    """Create performance debugger."""
    return PerformanceDebugger(brain)