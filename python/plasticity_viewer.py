#!/usr/bin/env python3
"""
Plasticity Viewer Tool

Visualization of synaptic changes over time in neural networks.
Provides comprehensive visualization of plasticity mechanisms,
weight changes, and synaptic dynamics.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import TwoSlopeNorm
from matplotlib import cm
import seaborn as sns
import pandas as pd
from datetime import datetime, timedelta
import sys
import os

@dataclass
class PlasticityViewerConfig:
    """Configuration for plasticity viewer"""
    time_window: float = 20.0  # seconds
    refresh_rate: float = 50.0  # Hz
    show_weight_matrix: bool = True
    show_weight_changes: bool = True
    show_plasticity_rules: bool = True
    show_optimization_history: bool = True
    show_neuron_connectivity: bool = False
    animate: bool = True
    color_scheme: str = "diverging"  # "linear", "diverging", "sequential", "cyclic"
    show_labels: bool = True
    show_statistics: bool = True
    save_format: str = "png"  # "png", "gif", "mp4", "html"
    output_path: str = "plasticity_viewer"
    
    # Visualization modes
    view_mode: str = "matrix"  # "matrix", "network", "timeline", "heatmap"
    show_3d: bool = False
    figure_size: Tuple[int, int] = (16, 12)
    dpi: int = 150
    animation_interval: int = 100  # milliseconds
    
    # Filtering and analysis
    filter_type: str = None  # "moving_average", "exponential", "butterworth"
    filter_window: float = 1.0  # seconds
    show_effective_change: bool = True
    show_plasticity_trace: bool = True
    
    # Display settings
    show_legend: bool = True
    show_colorbar: bool = True
    smooth_changes: bool = True
    interpolation: str = "linear"  # "none", "linear", "cubic"
class PlasticityViewer:
    """
    Visualization tool for synaptic plasticity dynamics in neural networks.
    
    Features:
    - Real-time weight matrix visualization
    - Plasticity rule tracking and visualization
    - Synaptic change trajectory plotting
    - Optimization history and convergence analysis
    - Network connectivity evolution
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: PlasticityViewerConfig = None):
        self.config = config or PlasticityViewerConfig()
        self.weight_history = []  # List of weight matrices over time
        self.plasticity_history = []  # Plasticity rule applications over time
        self.optimization_history = []  # Optimization metrics over time
        self.neuron_activities = []  # Neural activity history
        self.current_time = 0.0
        self.is_running = False
        self.animation_obj = None
        self.fig = None
        self.axes = None
        
        # Initialize viewer components
        self._initialize_components()
    
    def _initialize_components(self):
        """Initialize viewer components and data structures"""
        self.weight_matrices = []  # List of current weight matrices
        self.weight_deltas = []  # Weight change matrices
        self.plasticity_weights = []  # Plasticity rule weights
        self.neuron_regions = []  # Neuron region assignments
        self.connection_types = []  # Connection type classifications
        
        # Initialize colormaps
        self._initialize_colormaps()
        
        # Setup data buffers
        self._setup_data_buffers()
    
    def _initialize_colormaps(self):
        """Initialize colormaps for different plasticity visualizations"""
        # Diverging colormap for weight changes (red to blue)
        self.diverging_cmap = TwoSlopeNorm(vmin=-1, vcenter=0, vmax=1)
        
        # Linear colormap for positive weights
        self.positive_cmap = plt.cm.Blues_r
        
        # Sequential colormap for plasticity intensity
        self.sequential_cmap = plt.cm.YlOrRd
        
        # Cyclic colormap for optimization progress
        self.cyclic_cmap = plt.cm.hot
        
        # Region colormap
        self.region_cmap = plt.cm.tab20
    
    def _setup_data_buffers(self):
        """Setup data buffers for visualization"""
        self.max_history = 1000  # Maximum number of time steps to store
        self.weight_buffer = []
        self.delta_buffer = []
        self.plasticity_buffer = []
    
    def add_plasticity_data(self, 
                            weight_matrix: np.ndarray,
                            weight_delta: np.ndarray,
                            plasticity_metrics: Dict[str, Any],
                            neuron_activities: Optional[np.ndarray] = None,
                            time_step: float = 1.0):
        """
        Add plasticity data for visualization.
        
        Args:
            weight_matrix: Current weight matrix (N x N)
            weight_delta: Weight change matrix (N x N)
            plasticity_metrics: Dictionary of plasticity rule metrics
            neuron_activities: Optional neural activity matrix
            time_step: Current time step
        """
        # Validate input shapes
        n_neurons = weight_matrix.shape[0]
        if weight_matrix.shape != (n_neurons, n_neurons):
            raise ValueError("Weight matrix must be square (N x N)")
        
        if weight_delta.shape != (n_neurons, n_neurons):
            raise ValueError("Weight delta must match weight matrix dimensions")
        
        # Store data
        self.weight_matrices.append(weight_matrix.copy())
        self.weight_deltas.append(weight_delta.copy())
        self.plasticity_history.append(plasticity_metrics.copy())
        
        if neuron_activities is not None:
            self.neuron_activities.append(neuron_activities.copy())
        
        # Update buffers
        self.weight_buffer.append(weight_matrix)
        self.delta_buffer.append(weight_delta)
        self.plasticity_buffer.append(plasticity_metrics)
        
        # Maintain buffer size
        if len(self.weight_buffer) > self.max_history:
            self.weight_buffer.pop(0)
            self.delta_buffer.pop(0)
            self.plasticity_buffer.pop(0)
        
        # Update current time
        self.current_time += time_step
    
    def start_real_time(self, initial_data: Dict[str, Any] = None):
        """
        Start real-time plasticity visualization.
        
        Args:
            initial_data: Optional initial plasticity data
        """
        if initial_data:
            # Add initial data
            for i, (W, dW, metrics, t) in enumerate(zip(
                initial_data['weight_matrices'],
                initial_data['weight_deltas'],
                initial_data['plasticity_metrics'],
                initial_data.get('time_steps', [1.0] * len(initial_data['weight_matrices']))
            )):
                self.add_plasticity_data(W, dW, metrics, time_step=t)
        
        self.is_running = True
        
        # Create visualization
        self.fig = plt.figure(figsize=self.config.figure_size)
        
        # Create subplot layout based on view mode
        if self.config.view_mode == "matrix":
            self._create_matrix_view()
        elif self.config.view_mode == "network":
            self._create_network_view()
        elif self.config.view_mode == "timeline":
            self._create_timeline_view()
        elif self.config.view_mode == "heatmap":
            self._create_heatmap_view()
        else:
            self._create_matrix_view()
        
        # Setup animation
        self.animation_obj = animation.FuncAnimation(
            self.fig, self._update_plot, interval=self.config.animation_interval,
            blit=False, cache_frame_data=False
        )
        
        plt.show()
    
    def _create_matrix_view(self):
        """Create matrix view visualization"""
        # Create grid of subplots
        gs = plt.GridSpec(3, 4, hspace=0.3, wspace=0.3)
        
        # Weight matrix display
        self.ax_matrix = plt.subplot(gs[0, :2])
        
        # Weight change display
        self.ax_change = plt.subplot(gs[0, 2:])
        
        # Plasticity rules visualization
        self.ax_plasticity = plt.subplot(gs[1, :])
        
        # Optimization history
        self.ax_optimization = plt.subplot(gs[2, :2])
        
        # Neuron activity heatmap
        self.ax_activity = plt.subplot(gs[2, 2:])
        
        # Initialize plot elements
        self._initialize_matrix_elements()
    
    def _create_network_view(self):
        """Create network view visualization"""
        # Create 2x2 grid for network visualization
        gs = plt.GridSpec(2, 2, hspace=0.3, wspace=0.3)
        
        # Network connectivity
        self.ax_network = plt.subplot(gs[0, 0])
        
        # Weighted network
        self.ax_weighted = plt.subplot(gs[0, 1])
        
        # Plasticity hotspots
        self.ax_hotspots = plt.subplot(gs[1, 0])
        
        # Optimization convergence
        self.ax_convergence = plt.subplot(gs[1, 1])
        
        # Initialize network elements
        self._initialize_network_elements()
    
    def _create_timeline_view(self):
        """Create timeline view visualization"""
        # Create multi-panel timeline view
        gs = plt.GridSpec(4, 3, hspace=0.4, wspace=0.3)
        
        # Weight evolution over time
        self.ax_weight_evolution = plt.subplot(gs[0, :])
        
        # Plasticity rule evolution
        self.ax_plasticity_evolution = plt.subplot(gs[1, :])
        
        # Network connectivity evolution
        self.ax_connectivity = plt.subplot(gs[2, 0])
        
        # Activity evolution
        self.ax_activity_evolution = plt.subplot(gs[2, 1])
        
        # Optimization trajectory
        self.ax_optimization_traj = plt.subplot(gs[2, 2])
        
        # Summary statistics
        self.ax_stats = plt.subplot(gs[3, :])
        
        # Initialize timeline elements
        self._initialize_timeline_elements()
    
    def _create_heatmap_view(self):
        """Create heatmap view visualization"""
        # Create single heatmap display with multiple panels
        gs = plt.GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # Current weight matrix heatmap
        self.ax_weight_heatmap = plt.subplot(gs[0, :2])
        
        # Weight change heatmap
        self.ax_change_heatmap = plt.subplot(gs[0, 2])
        
        # Plasticity intensity heatmap
        self.ax_plasticity_heatmap = plt.subplot(gs[1, :2])
        
        # Optimization heatmap
        self.ax_optimization_heatmap = plt.subplot(gs[1, 2])
        
        # Neuron activity heatmap
        self.ax_activity_heatmap = plt.subplot(gs[2, :2])
        
        # Network property heatmap
        self.ax_network_heatmap = plt.subplot(gs[2, 2])
        
        # Initialize heatmap elements
        self._initialize_heatmap_elements()
    
    def _initialize_matrix_elements(self):
        """Initialize matrix view plot elements"""
        if hasattr(self, 'ax_matrix'):
            # Weight matrix display
            self.matrix_im = self.ax_matrix.imshow(
                np.zeros((10, 10)), cmap=self.positive_cmap, aspect='auto'
            )
            self.ax_matrix.set_title("Weight Matrix", fontsize=12, fontweight='bold')
            self.ax_matrix.set_xlabel("Target Neuron")
            self.ax_matrix.set_ylabel("Source Neuron")
            
            # Add colorbar
            self.matrix_cbar = plt.colorbar(self.matrix_im, ax=self.ax_matrix, shrink=0.8)
            self.matrix_cbar.set_label("Weight Value")
        
        if hasattr(self, 'ax_change'):
            # Weight change display
            self.change_im = self.ax_change.imshow(
                np.zeros((10, 10)), cmap=self.diverging_cmap, aspect='auto', vmin=-1, vmax=1
            )
            self.ax_change.set_title("Weight Change", fontsize=12, fontweight='bold')
            self.ax_change.set_xlabel("Target Neuron")
            self.ax_change.set_ylabel("Source Neuron")
            
            # Add colorbar
            self.change_cbar = plt.colorbar(self.change_im, ax=self.ax_change, shrink=0.8)
            self.change_cbar.set_label("Change Value")
        
        if hasattr(self, 'ax_plasticity'):
            # Plasticity rules display
            self.plasticity_bars = None
            self.ax_plasticity.set_title("Plasticity Rules", fontsize=12, fontweight='bold')
            self.ax_plasticity.set_xlabel("Rule")
            self.ax_plasticity.set_ylabel("Strength")
            self.ax_plasticity.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_optimization'):
            # Optimization history display
            self.optimization_plot, = self.ax_optimization.plot([], [], 'b-', linewidth=2)
            self.ax_optimization.set_title("Optimization History", fontsize=12, fontweight='bold')
            self.ax_optimization.set_xlabel("Time Step")
            self.ax_optimization.set_ylabel("Objective Value")
            self.ax_optimization.grid(True, alpha=0.3)
            self.ax_optimization.set_ylim(0, 1)
        
        if hasattr(self, 'ax_activity'):
            # Activity heatmap display
            self.activity_im = self.ax_activity.imshow(
                np.zeros((10, 10)), cmap='hot', aspect='auto'
            )
            self.ax_activity.set_title("Neuron Activity", fontsize=12, fontweight='bold')
            self.ax_activity.set_xlabel("Neuron Index")
            self.ax_activity.set_ylabel("Time Step")
            
            # Add colorbar
            self.activity_cbar = plt.colorbar(self.activity_im, ax=self.ax_activity, shrink=0.8)
            self.activity_cbar.set_label("Activity Level")
    
    def _initialize_network_elements(self):
        """Initialize network view plot elements"""
        if hasattr(self, 'ax_network'):
            # Network connectivity display
            self.network_scatter = None
            self.network_conn = None
            self.ax_network.set_title("Network Connectivity", fontsize=12, fontweight='bold')
            self.ax_network.set_xlabel("X Position")
            self.ax_network.set_ylabel("Y Position")
            self.ax_network.grid(True, alpha=0.3)
            self.ax_network.axis('equal')
        
        if hasattr(self, 'ax_weighted'):
            # Weighted network display
            self.weighted_scatter = None
            self.weighted_conn = None
            self.ax_weighted.set_title("Weighted Connections", fontsize=12, fontweight='bold')
            self.ax_weighted.set_xlabel("X Position")
            self.ax_weighted.set_ylabel("Y Position")
            self.ax_weighted.grid(True, alpha=0.3)
            self.ax_weighted.axis('equal')
        
        if hasattr(self, 'ax_hotspots'):
            # Plasticity hotspots display
            self.hotspots_im = self.ax_hotspots.imshow(
                np.zeros((10, 10)), cmap='hot', aspect='auto'
            )
            self.ax_hotspots.set_title("Plasticity Hotspots", fontsize=12, fontweight='bold')
            self.ax_hotspots.set_xlabel("X Position")
            self.ax_hotspots.set_ylabel("Y Position")
            self.ax_hotspots.set_xticks([])
            self.ax_hotspots.set_yticks([])
        
        if hasattr(self, 'ax_convergence'):
            # Optimization convergence display
            self.convergence_plot, = self.ax_convergence.plot([], [], 'g-', linewidth=2)
            self.ax_convergence.set_title("Convergence", fontsize=12, fontweight='bold')
            self.ax_convergence.set_xlabel("Iteration")
            self.ax_convergence.set_ylabel("Learning Rate")
            self.ax_convergence.grid(True, alpha=0.3)
    
    def _initialize_timeline_elements(self):
        """Initialize timeline view plot elements"""
        if hasattr(self, 'ax_weight_evolution'):
            # Weight evolution display
            self.weight_evolution_lines = []
            self.ax_weight_evolution.set_title("Weight Evolution", fontsize=12, fontweight='bold')
            self.ax_weight_evolution.set_xlabel("Time Step")
            self.ax_weight_evolution.set_ylabel("Average Weight")
            self.ax_weight_evolution.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_plasticity_evolution'):
            # Plasticity rule evolution display
            self.plasticity_evolution_bars = None
            self.ax_plasticity_evolution.set_title("Plasticity Rule Evolution", fontsize=12, fontweight='bold')
            self.ax_plasticity_evolution.set_xlabel("Time Step")
            self.ax_plasticity_evolution.set_ylabel("Rule Strength")
            self.ax_plasticity_evolution.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_connectivity'):
            # Connectivity evolution display
            self.connectivity_plot, = self.ax_connectivity.plot([], [], 'r-', linewidth=2)
            self.ax_connectivity.set_title("Connectivity Evolution", fontsize=12, fontweight='bold')
            self.ax_connectivity.set_xlabel("Time Step")
            self.ax_connectivity.set_ylabel("Connection Density")
            self.ax_connectivity.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_activity_evolution'):
            # Activity evolution display
            self.activity_evolution_plot, = self.ax_activity_evolution.plot([], [], 'b-', linewidth=2)
            self.ax_activity_evolution.set_title("Activity Evolution", fontsize=12, fontweight='bold')
            self.ax_activity_evolution.set_xlabel("Time Step")
            self.ax_activity_evolution.set_ylabel("Average Activity")
            self.ax_activity_evolution.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_optimization_traj'):
            # Optimization trajectory display
            self.optimization_traj = None
            self.ax_optimization_traj.set_title("Optimization Trajectory", fontsize=12, fontweight='bold')
            self.ax_optimization_traj.set_xlabel("Parameter 1")
            self.ax_optimization_traj.set_ylabel("Parameter 2")
            self.ax_optimization_traj.grid(True, alpha=0.3)
        
        if hasattr(self, 'ax_stats'):
            # Statistics display
            self.stats_text = self.ax_stats.text(0.05, 0.95, '', transform=self.ax_stats.transAxes,
                                               fontsize=10, verticalalignment='top',
                                               bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
            self.ax_stats.set_xlim(0, 1)
            self.ax_stats.set_ylim(0, 1)
            self.ax_stats.axis('off')
    
    def _initialize_heatmap_elements(self):
        """Initialize heatmap view plot elements"""
        # Initialize all heatmap displays
        if hasattr(self, 'ax_weight_heatmap'):
            self.weight_heatmap = self.ax_weight_heatmap.imshow(
                np.zeros((10, 10)), cmap=self.positive_cmap, aspect='auto'
            )
            self.ax_weight_heatmap.set_title("Weight Matrix", fontsize=10, fontweight='bold')
            self.ax_weight_heatmap.set_xticks([])
            self.ax_weight_heatmap.set_yticks([])
        
        if hasattr(self, 'ax_change_heatmap'):
            self.change_heatmap = self.ax_change_heatmap.imshow(
                np.zeros((10, 10)), cmap=self.diverging_cmap, aspect='auto', vmin=-1, vmax=1
            )
            self.ax_change_heatmap.set_title("Weight Change", fontsize=10, fontweight='bold')
            self.ax_change_heatmap.set_xticks([])
            self.ax_change_heatmap.set_yticks([])
        
        # Add colorbars for heatmaps
        if hasattr(self, 'ax_weight_heatmap') and hasattr(self, 'fig'):
            self.weight_cbar = plt.colorbar(self.weight_heatmap, ax=self.ax_weight_heatmap, shrink=0.8)
            self.weight_cbar.set_label("Weight")
        
        if hasattr(self, 'ax_change_heatmap') and hasattr(self, 'fig'):
            self.change_cbar = plt.colorbar(self.change_heatmap, ax=self.ax_change_heatmap, shrink=0.8)
            self.change_cbar.set_label("Change")
    
    def _update_plot(self, frame):
        """Update plot for animation"""
        # Update all visualization elements based on current time
        if self.config.view_mode == "matrix":
            self._update_matrix_view(frame)
        elif self.config.view_mode == "network":
            self._update_network_view(frame)
        elif self.config.view_mode == "timeline":
            self._update_timeline_view(frame)
        elif self.config.view_mode == "heatmap":
            self._update_heatmap_view(frame)
    
    def _update_matrix_view(self, frame):
        """Update matrix view elements"""
        if not self.weight_matrices:
            return
        
        # Get current weight matrix (or interpolate between recent matrices)
        current_idx = min(frame, len(self.weight_matrices) - 1)
        weight_matrix = self.weight_matrices[current_idx]
        weight_delta = self.weight_deltas[current_idx]
        plasticity_metrics = self.plasticity_history[current_idx]
        
        # Update weight matrix display
        if hasattr(self, 'ax_matrix') and hasattr(self, 'matrix_im'):
            self.matrix_im.set_data(weight_matrix)
            self.matrix_im.set_clim(weight_matrix.min(), weight_matrix.max())
        
        # Update weight change display
        if hasattr(self, 'ax_change') and hasattr(self, 'change_im'):
            self.change_im.set_data(weight_delta)
        
        # Update plasticity rules display
        if hasattr(self, 'ax_plasticity'):
            self._update_plasticity_rules(plasticity_metrics)
        
        # Update optimization history
        if hasattr(self, 'ax_optimization') and hasattr(self, 'optimization_plot'):
            self._update_optimization_history(plasticity_metrics)
        
        # Update activity heatmap
        if hasattr(self, 'ax_activity') and hasattr(self, 'activity_im') and self.neuron_activities:
            current_activities = self.neuron_activities[current_idx]
            self.activity_im.set_data(current_activities.T)
    
    def _update_network_view(self, frame):
        """Update network view elements"""
        if not self.weight_matrices:
            return
        
        current_idx = min(frame, len(self.weight_matrices) - 1)
        weight_matrix = self.weight_matrices[current_idx]
        
        # Generate dummy network positions for visualization
        n_neurons = weight_matrix.shape[0]
        np.random.seed(42)
        positions = np.random.randn(n_neurons, 2)
        
        # Update network connectivity
        if hasattr(self, 'ax_network') and hasattr(self, 'network_conn'):
            self._update_network_connections(positions, weight_matrix, self.ax_network)
        
        # Update weighted network
        if hasattr(self, 'ax_weighted') and hasattr(self, 'weighted_conn'):
            self._update_weighted_connections(positions, weight_matrix, self.ax_weighted)
        
        # Update plasticity hotspots
        if hasattr(self, 'ax_hotspots') and hasattr(self, 'hotspots_im'):
            self._update_plasticity_hotspots(weight_matrix)
        
        # Update convergence plot
        if hasattr(self, 'ax_convergence') and hasattr(self, 'convergence_plot'):
            self._update_convergence_plot(frame)
    
    def _update_timeline_view(self, frame):
        """Update timeline view elements"""
        if not self.weight_matrices:
            return
        
        current_idx = min(frame, len(self.weight_matrices) - 1)
        weight_matrix = self.weight_matrices[current_idx]
        
        # Update weight evolution
        if hasattr(self, 'ax_weight_evolution'):
            self._update_weight_evolution(frame)
        
        # Update plasticity evolution
        if hasattr(self, 'ax_plasticity_evolution'):
            self._update_plasticity_evolution(frame)
        
        # Update connectivity evolution
        if hasattr(self, 'ax_connectivity'):
            self._update_connectivity_evolution(frame)
        
        # Update activity evolution
        if hasattr(self, 'ax_activity_evolution'):
            self._update_activity_evolution(frame)
        
        # Update optimization trajectory
        if hasattr(self, 'ax_optimization_traj'):
            self._update_optimization_trajectory(frame)
        
        # Update statistics
        if hasattr(self, 'ax_stats'):
            self._update_statistics(frame)
    
    def _update_heatmap_view(self, frame):
        """Update heatmap view elements"""
        if not self.weight_matrices:
            return
        
        current_idx = min(frame, len(self.weight_matrices) - 1)
        weight_matrix = self.weight_matrices[current_idx]
        weight_delta = self.weight_deltas[current_idx]
        
        # Update all heatmaps
        if hasattr(self, 'ax_weight_heatmap') and hasattr(self, 'weight_heatmap'):
            self.weight_heatmap.set_data(weight_matrix)
            self.weight_heatmap.set_clim(weight_matrix.min(), weight_matrix.max())
        
        if hasattr(self, 'ax_change_heatmap') and hasattr(self, 'change_heatmap'):
            self.change_heatmap.set_data(weight_delta)
        
        # Update other heatmaps based on configuration
        # (Implementation would continue here...)
    
    def _update_plasticity_rules(self, plasticity_metrics: Dict[str, Any]):
        """Update plasticity rules visualization"""
        if not plasticity_metrics:
            return
        
        # Extract plasticity rule strengths
        rules = []
        strengths = []
        
        for rule_name, rule_data in plasticity_metrics.items():
            if isinstance(rule_data, dict):
                rules.append(rule_name)
                strengths.append(rule_data.get('strength', 0.0))
            else:
                rules.append(rule_name)
                strengths.append(float(rule_data))
        
        if rules and hasattr(self, 'ax_plasticity'):
            if hasattr(self, 'plasticity_bars') and self.plasticity_bars is not None:
                # Update existing bars
                self.plasticity_bars.set_height(strengths)
                self.plasticity_bars.set_x(rules)
            else:
                # Create new bars
                x = np.arange(len(rules))
                self.plasticity_bars = self.ax_plasticity.bar(x, strengths, alpha=0.7)
                self.ax_plasticity.set_xticks(x)
                self.ax_plasticity.set_xticklabels(rules, rotation=45, ha='right')
    
    def _update_optimization_history(self, plasticity_metrics: Dict[str, Any]):
        """Update optimization history visualization"""
        if not plasticity_metrics or not hasattr(self, 'ax_optimization'):
            return
        
        # Extract objective value if available
        objective_value = plasticity_metrics.get('objective_value', 
                                               plasticity_metrics.get('loss', 
                                                                     plasticity_metrics.get('error', 0.5)))
        
        # Update line plot
        if hasattr(self, 'optimization_plot'):
            x_data = list(range(len(self.optimization_history)))
            y_data = [h.get('objective_value', 0.5) for h in self.optimization_history]
            self.optimization_plot.set_data(x_data, y_data)
            
            # Adjust x-axis limits
            self.ax_optimization.set_xlim(0, max(100, len(x_data)))
    
    def _update_network_connections(self, positions: np.ndarray, 
                                   weight_matrix: np.ndarray, 
                                   ax):
        """Update network connections visualization"""
        n_neurons = len(positions)
        
        # Filter connections based on weight threshold
        threshold = np.percentile(weight_matrix[weight_matrix > 0], 75)
        
        # Plot connections
        for i in range(n_neurons):
            for j in range(i + 1, n_neurons):
                weight = weight_matrix[i, j] if weight_matrix[i, j] > 0 else weight_matrix[j, i]
                
                if weight > threshold:
                    alpha = min(0.8, weight / weight_matrix.max())
                    linewidth = 1 + 5 * (weight / weight_matrix.max())
                    
                    ax.plot([positions[i, 0], positions[j, 0]],
                           [positions[i, 1], positions[j, 1]],
                           'k-', linewidth=linewidth, alpha=alpha, zorder=1)
        
        # Plot neurons
        ax.scatter(positions[:, 0], positions[:, 1], 
                  s=50, c='blue', alpha=0.7, edgecolors='black', linewidths=1)
    
    def _update_weighted_connections(self, positions: np.ndarray, 
                                    weight_matrix: np.ndarray, 
                                    ax):
        """Update weighted network connections visualization"""
        n_neurons = len(positions)
        
        # Normalize weights for visualization
        if weight_matrix.max() > weight_matrix.min():
            norm_weights = (weight_matrix - weight_matrix.min()) / (weight_matrix.max() - weight_matrix.min())
        else:
            norm_weights = np.zeros_like(weight_matrix)
        
        # Plot connections with weighted colors
        for i in range(n_neurons):
            for j in range(i + 1, n_neurons):
                weight = weight_matrix[i, j] if weight_matrix[i, j] > 0 else weight_matrix[j, i]
                norm_weight = norm_weights[i, j] if norm_weights[i, j] > 0 else norm_weights[j, i]
                
                color = plt.cm.Reds(norm_weight)
                linewidth = 2 + 8 * norm_weight
                
                ax.plot([positions[i, 0], positions[j, 0]],
                       [positions[i, 1], positions[j, 1]],
                       '-', linewidth=linewidth, color=color, alpha=0.7, zorder=1)
        
        # Plot neurons
        ax.scatter(positions[:, 0], positions[:, 1], 
                  s=50, c='red', alpha=0.8, edgecolors='darkred', linewidths=2)
    
    def _update_plasticity_hotspots(self, weight_matrix: np.ndarray):
        """Update plasticity hotspots visualization"""
        # Calculate plasticity hotspots (neurons with high weight changes)
        if weight_matrix.size > 0:
            # Flatten and find hotspots
            flat_weights = weight_matrix.flatten()
            
            # Create heatmap of plasticity activity
            heatmap_size = int(np.ceil(np.sqrt(len(flat_weights))))
            heatmap = np.zeros((heatmap_size, heatmap_size))
            
            for i, weight in enumerate(flat_weights):
                row = i // heatmap_size
                col = i % heatmap_size
                if row < heatmap_size and col < heatmap_size:
                    heatmap[row, col] = weight
            
            self.hotspots_im.set_data(heatmap)
            self.hotspots_im.set_clim(heatmap.min(), heatmap.max())
    
    def _update_convergence_plot(self, frame):
        """Update optimization convergence plot"""
        if not hasattr(self, 'convergence_plot'):
            return
        
        # Generate dummy convergence data
        x_data = list(range(min(frame + 1, 100)))
        y_data = np.linspace(1.0, 0.01, len(x_data)) * (1 - frame / 100) + 0.01
        
        self.convergence_plot.set_data(x_data, y_data)
        
        # Set limits
        self.ax_convergence.set_xlim(0, 100)
        self.ax_convergence.set_ylim(0, 1.0)
    
    def _update_weight_evolution(self, frame):
        """Update weight evolution visualization"""
        if not self.weight_matrices or not hasattr(self, 'ax_weight_evolution'):
            return
        
        # Calculate average weight over time
        avg_weights = []
        for i in range(min(frame + 1, len(self.weight_matrices))):
            W = self.weight_matrices[i]
            # Only consider positive weights
            avg_weights.append(W[W > 0].mean() if np.any(W > 0) else 0.0)
        
        if hasattr(self, 'weight_evolution_lines'):
            # Update existing lines
            for line in self.weight_evolution_lines:
                x, y = line.get_data()
                line.set_data(x + [frame], [y[-1] if y else 0] if len(y) == len(x) else [0])
        else:
            # Create new line
            x_data = list(range(len(avg_weights)))
            y_data = avg_weights
            line, = self.ax_weight_evolution.plot(x_data, y_data, 'b-', linewidth=2)
            self.weight_evolution_lines.append(line)
        
        # Set limits
        self.ax_weight_evolution.set_xlim(0, max(100, len(avg_weights)))
        if avg_weights:
            self.ax_weight_evolution.set_ylim(0, max(avg_weights) * 1.2 if max(avg_weights) > 0 else 1)
    
    def _update_plasticity_evolution(self, frame):
        """Update plasticity rule evolution visualization"""
        if not self.plasticity_history or not hasattr(self, 'ax_plasticity_evolution'):
            return
        
        # Calculate rule evolution
        if frame < len(self.plasticity_history):
            metrics = self.plasticity_history[frame]
            
            rules = list(metrics.keys())
            strengths = []
            
            for rule in rules:
                if isinstance(metrics[rule], dict):
                    strengths.append(metrics[rule].get('strength', 0.0))
                else:
                    strengths.append(float(metrics[rule]))
            
            if rules:
                if hasattr(self, 'plasticity_evolution_bars') and self.plasticity_evolution_bars is not None:
                    # Update existing bars
                    self.plasticity_evolution_bars.set_height(strengths)
                    self.plasticity_evolution_bars.set_x(list(range(len(rules))))
                else:
                    # Create new bars
                    x = np.arange(len(rules))
                    self.plasticity_evolution_bars = self.ax_plasticity_evolution.bar(x, strengths, alpha=0.7)
                    self.ax_plasticity_evolution.set_xticks(x)
                    self.ax_plasticity_evolution.set_xticklabels(rules, rotation=45, ha='right')
    
    def _update_connectivity_evolution(self, frame):
        """Update connectivity evolution visualization"""
        if not self.weight_matrices or not hasattr(self, 'ax_connectivity'):
            return
        
        # Calculate connection density over time
        densities = []
        for i in range(min(frame + 1, len(self.weight_matrices))):
            W = self.weight_matrices[i]
            # Calculate connection density (percentage of non-zero connections)
            total_connections = W.size
            active_connections = np.count_nonzero(W > 0)
            density = active_connections / total_connections if total_connections > 0 else 0.0
            densities.append(density)
        
        if hasattr(self, 'connectivity_plot'):
            x_data = list(range(len(densities)))
            self.connectivity_plot.set_data(x_data, densities)
            
            # Set limits
            self.ax_connectivity.set_xlim(0, max(100, len(densities)))
            self.ax_connectivity.set_ylim(0, 1.0)
    
    def _update_activity_evolution(self, frame):
        """Update activity evolution visualization"""
        if not hasattr(self, 'ax_activity_evolution'):
            return
        
        # Calculate average activity over time
        avg_activities = []
        for i in range(min(frame + 1, len(self.weight_matrices))):
            if self.neuron_activities and i < len(self.neuron_activities):
                avg_activities.append(np.mean(self.neuron_activities[i]))
            else:
                avg_activities.append(0.5)  # Default value
        
        if hasattr(self, 'activity_evolution_plot'):
            x_data = list(range(len(avg_activities)))
            self.activity_evolution_plot.set_data(x_data, avg_activities)
            
            # Set limits
            self.ax_activity_evolution.set_xlim(0, max(100, len(avg_activities)))
            self.ax_activity_evolution.set_ylim(0, max(avg_activities) * 1.2 if avg_activities else 1)
    
    def _update_optimization_trajectory(self, frame):
        """Update optimization trajectory visualization"""
        if not hasattr(self, 'ax_optimization_traj'):
            return
        
        # Generate dummy optimization trajectory
        if frame > 0:
            x = np.linspace(-2, 2, frame)
            y = np.sin(x * np.pi / 4) * np.exp(-x**2 / 8)
            
            if self.optimization_traj is not None:
                self.optimization_traj.set_data(x, y)
            else:
                self.optimization_traj, = self.ax_optimization_traj.plot(x, y, 'g-', linewidth=2)
            
            self.ax_optimization_traj.set_xlim(-2.5, 2.5)
            self.ax_optimization_traj.set_ylim(-1.2, 1.2)
    
    def _update_statistics(self, frame):
        """Update statistics panel"""
        if not self.weight_matrices or not hasattr(self, 'ax_stats'):
            return
        
        # Calculate current statistics
        current_idx = min(frame, len(self.weight_matrices) - 1)
        weight_matrix = self.weight_matrices[current_idx]
        
        # Compute statistics
        stats_text = f"Time Step: {current_idx}\n"
        stats_text += f"Average Weight: {np.mean(weight_matrix):.4f}\n"
        stats_text += f"Max Weight: {np.max(weight_matrix):.4f}\n"
        stats_text += f"Min Weight: {np.min(weight_matrix):.4f}\n"
        
        if np.any(weight_matrix > 0):
            stats_text += f"Non-zero Connections: {np.count_nonzero(weight_matrix > 0)}\n"
            stats_text += f"Connection Density: {np.count_nonzero(weight_matrix > 0) / weight_matrix.size:.3f}\n"
        
        if self.neuron_activities and current_idx < len(self.neuron_activities):
            activity = self.neuron_activities[current_idx]
            stats_text += f"Avg Neuron Activity: {np.mean(activity):.3f}\n"
            stats_text += f"Activity Range: {np.min(activity):.3f} - {np.max(activity):.3f}\n"
        
        # Update text
        self.stats_text.set_text(stats_text)
    
    def create_static_visualization(self, output_file: str = None) -> Dict[str, Any]:
        """
        Create static visualization for analysis and reporting.
        
        Args:
            output_file: Optional output filename
            
        Returns:
            Dictionary with visualization data and results
        """
        result = {
            "success": False,
            "data": {},
            "plots": {},
            "statistics": {},
            "output_files": []
        }
        
        try:
            if not self.weight_matrices:
                result["error"] = "No plasticity data available"
                return result
            
            # Create comprehensive visualization
            fig = self._create_comprehensive_visualization()
            
            # Save figure
            if output_file is None:
                output_file = f"{self.config.output_path}.{self.config.save_format}"
            
            os.makedirs(os.path.dirname(output_file) if os.path.dirname(output_file) else '.', exist_ok=True)
            fig.savefig(output_file, dpi=self.config.dpi, bbox_inches='tight')
            
            # Compute statistics
            stats = self._compute_plasticity_statistics()
            
            result["plots"] = "comprehensive plasticity visualization"
            result["statistics"] = stats
            result["output_files"].append(output_file)
            result["success"] = True
            
            result["figure"] = fig
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_comprehensive_visualization(self):
        """Create comprehensive visualization with multiple panels"""
        # Create a comprehensive figure based on view mode
        if self.config.view_mode == "matrix":
            return self._create_matrix_comprehensive()
        elif self.config.view_mode == "network":
            return self._create_network_comprehensive()
        elif self.config.view_mode == "timeline":
            return self._create_timeline_comprehensive()
        elif self.config.view_mode == "heatmap":
            return self._create_heatmap_comprehensive()
        else:
            return self._create_matrix_comprehensive()
    
    def _create_matrix_comprehensive(self):
        """Create comprehensive matrix view"""
        fig = plt.figure(figsize=(20, 15))
        gs = plt.GridSpec(4, 5, hspace=0.3, wspace=0.3)
        
        # Panel 1: Weight matrix evolution
        self._plot_weight_matrix_evolution(fig, gs[0, :3])
        
        # Panel 2: Weight change heatmap
        self._plot_weight_change_heatmap(fig, gs[0, 3:])
        
        # Panel 3: Plasticity rules over time
        self._plot_plasticity_rules_time(fig, gs[1, :3])
        
        # Panel 4: Optimization trajectory
        self._plot_optimization_trajectory(fig, gs[1, 3:])
        
        # Panel 5: Network statistics
        self._plot_network_statistics(fig, gs[2, :3])
        
        # Panel 6: Convergence analysis
        self._plot_convergence_analysis(fig, gs[2, 3:])
        
        # Panel 7: Final weight matrix
        self._plot_final_weight_matrix(fig, gs[3, :3])
        
        # Panel 8: Rule effectiveness
        self._plot_rule_effectiveness(fig, gs[3, 3:])
        
        fig.suptitle("Comprehensive Plasticity Visualization", fontsize=16, fontweight='bold')
        
        return fig
    
    def _plot_weight_matrix_evolution(self, fig, ax):
        """Plot weight matrix evolution"""
        if len(self.weight_matrices) > 0:
            # Display final weight matrix
            W = self.weight_matrices[-1]
            im = ax.imshow(W, cmap=self.positive_cmap, aspect='auto')
            ax.set_title("Final Weight Matrix", fontsize=12, fontweight='bold')
            ax.set_xlabel("Target Neuron")
            ax.set_ylabel("Source Neuron")
            
            # Add colorbar
            plt.colorbar(im, ax=ax, shrink=0.8)
    
    def _plot_weight_change_heatmap(self, fig, ax):
        """Plot weight change heatmap"""
        if len(self.weight_deltas) > 0:
            dW = self.weight_deltas[-1]
            im = ax.imshow(dW, cmap=self.diverging_cmap, aspect='auto', 
                          vmin=-1, vmax=1)
            ax.set_title("Final Weight Changes", fontsize=12, fontweight='bold')
            ax.set_xlabel("Target Neuron")
            ax.set_ylabel("Source Neuron")
            
            # Add colorbar
            plt.colorbar(im, ax=ax, shrink=0.8)
    
    def _plot_plasticity_rules_time(self, fig, ax):
        """Plot plasticity rules over time"""
        if self.plasticity_history:
            # Aggregate rule strengths over time
            rule_strengths = {}
            for metrics in self.plasticity_history:
                for rule_name, rule_data in metrics.items():
                    if rule_name not in rule_strengths:
                        rule_strengths[rule_name] = []
                    
                    if isinstance(rule_data, dict):
                        strength = rule_data.get('strength', 0.0)
                    else:
                        strength = float(rule_data)
                    
                    rule_strengths[rule_name].append(strength)
            
            # Plot each rule
            time_steps = list(range(len(self.plasticity_history)))
            for i, (rule_name, strengths) in enumerate(rule_strengths.items()):
                alpha = 0.7 if i < 10 else 0.3  # Fade out less important rules
                ax.plot(time_steps, strengths, label=rule_name, alpha=alpha, linewidth=2)
            
            ax.set_title("Plasticity Rule Evolution", fontsize=12, fontweight='bold')
            ax.set_xlabel("Time Step")
            ax.set_ylabel("Rule Strength")
            ax.legend(fontsize=8, ncol=2)
            ax.grid(True, alpha=0.3)
    
    def _plot_optimization_trajectory(self, fig, ax):
        """Plot optimization trajectory"""
        # Generate dummy optimization trajectory
        x = np.linspace(-2, 2, 100)
        y = np.sin(x * np.pi / 4) * np.exp(-x**2 / 8)
        
        ax.plot(x, y, 'g-', linewidth=3, alpha=0.7)
        ax.scatter([x[49]], [y[49]], color='red', s=100, marker='*', label='Current')
        
        ax.set_title("Optimization Trajectory", fontsize=12, fontweight='bold')
        ax.set_xlabel("Parameter 1")
        ax.set_ylabel("Parameter 2")
        ax.legend()
        ax.grid(True, alpha=0.3)
        ax.set_xlim(-2.5, 2.5)
        ax.set_ylim(-1.2, 1.2)
    
    def _plot_network_statistics(self, fig, ax):
        """Plot network statistics"""
        if self.weight_matrices:
            # Calculate statistics
            final_W = self.weight_matrices[-1]
            
            stats_text = f"Network Statistics:\n\n"
            stats_text += f"Neurons: {final_W.shape[0]} x {final_W.shape[1]}\n"
            stats_text += f"Total Connections: {final_W.size}\n"
            stats_text += f"Active Connections: {np.count_nonzero(final_W > 0)}\n"
            stats_text += f"Connection Density: {np.count_nonzero(final_W > 0) / final_W.size:.3f}\n"
            stats_text += f"Average Weight: {np.mean(final_W):.4f}\n"
            stats_text += f"Max Weight: {np.max(final_W):.4f}\n"
            stats_text += f"Min Weight: {np.min(final_W):.4f}\n"
            
            if self.neuron_activities:
                activity = self.neuron_activities[-1]
                stats_text += f"Avg Activity: {np.mean(activity):.3f}\n"
                stats_text += f"Activity Range: {np.min(activity):.3f} - {np.max(activity):.3f}\n"
            
            ax.text(0.05, 0.95, stats_text, transform=ax.transAxes, fontsize=11,
                   verticalalignment='top', fontfamily='monospace',
                   bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.7))
            
            ax.set_title("Network Statistics", fontsize=12, fontweight='bold')
            ax.axis('off')
    
    def _plot_convergence_analysis(self, fig, ax):
        """Plot convergence analysis"""
        # Generate convergence curve
        iterations = np.arange(1, 101)
        learning_rate = np.linspace(0.1, 0.001, 100)
        error = 0.5 * np.exp(-iterations / 20)
        
        ax.plot(iterations, learning_rate, 'r-', label='Learning Rate', linewidth=2)
        ax2 = ax.twinx()
        ax2.plot(iterations, error, 'b-', label='Error', linewidth=2, alpha=0.7)
        
        ax.set_title("Convergence Analysis", fontsize=12, fontweight='bold')
        ax.set_xlabel("Iteration")
        ax.set_ylabel('Learning Rate (red)', color='red')
        ax2.set_ylabel('Error (blue)', color='blue')
        ax.grid(True, alpha=0.3)
        
        # Combine legends
        lines_1, labels_1 = ax.get_legend_handles_labels()
        lines_2, labels_2 = ax2.get_legend_handles_labels()
        ax.legend(lines_1 + lines_2, labels_1 + labels_2, loc='upper right')
    
    def _plot_final_weight_matrix(self, fig, ax):
        """Plot final weight matrix"""
        if len(self.weight_matrices) > 0:
            W = self.weight_matrices[-1]
            
            # Display in heatmap format
            im = ax.imshow(W, cmap='hot', aspect='auto')
            ax.set_title("Final Weight Matrix", fontsize=12, fontweight='bold')
            ax.set_xlabel("Target Neuron")
            ax.set_ylabel("Source Neuron")
            
            # Add colorbar
            plt.colorbar(im, ax=ax, shrink=0.8)
    
    def _plot_rule_effectiveness(self, fig, ax):
        """Plot rule effectiveness"""
        if self.plasticity_history:
            # Calculate rule effectiveness based on weight changes
            rule_effectiveness = {}
            
            for metrics in self.plasticity_history:
                for rule_name, rule_data in metrics.items():
                    if rule_name not in rule_effectiveness:
                        rule_effectiveness[rule_name] = []
                    
                    if isinstance(rule_data, dict):
                        effectiveness = rule_data.get('effectiveness', rule_data.get('strength', 0.0))
                    else:
                        effectiveness = float(rule_data)
                    
                    rule_effectiveness[rule_name].append(effectiveness)
            
            # Calculate average effectiveness
            avg_effectiveness = {rule: np.mean(values) for rule, values in rule_effectiveness.items()
                               if values}
            
            if avg_effectiveness:
                rules = list(avg_effectiveness.keys())
                values = list(avg_effectiveness.values())
                
                # Sort by effectiveness
                sorted_indices = np.argsort(values)[::-1]
                rules = [rules[i] for i in sorted_indices]
                values = [values[i] for i in sorted_indices]
                
                # Create horizontal bar chart
                y_pos = np.arange(len(rules))
                ax.barh(y_pos, values, alpha=0.7, color='steelblue')
                ax.set_yticks(y_pos)
                ax.set_yticklabels(rules)
                ax.set_xlabel("Average Effectiveness")
                ax.set_title("Rule Effectiveness Ranking", fontsize=12, fontweight='bold')
                ax.grid(True, alpha=0.3, axis='x')
                ax.set_xlim(0, max(values) * 1.1 if values else 1)
    
    def _compute_plasticity_statistics(self) -> Dict[str, Any]:
        """Compute comprehensive plasticity statistics"""
        stats = {
            "time_steps": len(self.weight_matrices),
            "final_weight_matrix_shape": self.weight_matrices[-1].shape if self.weight_matrices else (0, 0),
            "plasticity_rules_count": len(set(k for m in self.plasticity_history for k in m.keys())) if self.plasticity_history else 0,
            "optimization_iterations": sum(1 for m in self.plasticity_history if 'optimization' in m) if self.plasticity_history else 0,
            "network_statistics": {},
            "plasticity_statistics": {},
            "convergence_metrics": {}
        }
        
        # Network statistics
        if self.weight_matrices:
            final_W = self.weight_matrices[-1]
            stats["network_statistics"] = {
                "total_neurons": final_W.shape[0],
                "total_connections": final_W.size,
                "active_connections": int(np.count_nonzero(final_W > 0)),
                "connection_density": float(np.count_nonzero(final_W > 0) / final_W.size),
                "average_weight": float(np.mean(final_W)),
                "max_weight": float(np.max(final_W)),
                "min_weight": float(np.min(final_W)),
                "weight_std": float(np.std(final_W)),
                "weight_range": float(np.max(final_W) - np.min(final_W))
            }
        
        # Plasticity statistics
        if self.plasticity_history:
            stats["plasticity_statistics"] = {
                "rules_applied": len(self.plasticity_history),
                "average_rule_strength": np.mean([np.mean(list(m.values())) 
                                                 for m in self.plasticity_history 
                                                 if m and any(isinstance(v, (int, float)) for v in m.values())]),
                "rule_variance": np.var([np.mean(list(m.values())) 
                                        for m in self.plasticity_history 
                                        if m and any(isinstance(v, (int, float)) for v in m.values())])
            }
        
        # Convergence metrics
        if self.plasticity_history:
            objective_values = []
            for metrics in self.plasticity_history:
                if 'objective_value' in metrics:
                    objective_values.append(metrics['objective_value'])
                elif 'loss' in metrics:
                    objective_values.append(metrics['loss'])
                elif 'error' in metrics:
                    objective_values.append(metrics['error'])
            
            if objective_values:
                stats["convergence_metrics"] = {
                    "initial_objective": objective_values[0] if objective_values else 0.0,
                    "final_objective": objective_values[-1] if objective_values else 0.0,
                    "objective_improvement": objective_values[-1] - objective_values[0] if len(objective_values) > 1 else 0.0,
                    "improvement_rate": (objective_values[0] - objective_values[-1]) / objective_values[0] 
                                         if objective_values[0] > 0 and len(objective_values) > 1 else 0.0
                }
        
        return stats
    
    def export_plasticity_data(self, filename: str) -> str:
        """
        Export plasticity data to file.
        
        Args:
            filename: Output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        export_data = {
            "config": asdict(self.config),
            "weight_history": [W.tolist() for W in self.weight_matrices],
            "weight_deltas": [dW.tolist() for dW in self.weight_deltas],
            "plasticity_history": self.plasticity_history,
            "neuron_activities": [A.tolist() for A in self.neuron_activities] if self.neuron_activities else [],
            "statistics": self._compute_plasticity_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return filename


def create_plasticity_viewer(config_dict: Dict[str, Any] = None) -> PlasticityViewer:
    """
    Factory function to create PlasticityViewer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured PlasticityViewer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = PlasticityViewerConfig(**config_dict)
    return PlasticityViewer(config)