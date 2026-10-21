#!/usr/bin/env python3
"""
Brain Visualizer Tool

Interactive 2D/3D visualization of neural networks.
Provides real-time visualization of brain structure, neuron connections,
and activity patterns.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, asdict
from enum import Enum
import json
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import plotly.graph_objects as go
import plotly.express as px
from scipy import spatial
import sys
import os

@dataclass
class BrainVisualizationConfig:
    """Configuration for brain visualization"""
    visualization_type: str = "3d"  # "2d", "3d", "interactive"
    neuron_size: float = 10.0
    connection_alpha: float = 0.3
    show_labels: bool = True
    animate: bool = False
    resolution: str = "high"  # "low", "medium", "high"
    color_scheme: str = "default"  # "default", "activity", "type", "region"
    save_format: str = "png"  # "png", "html", "json"
    output_path: str = "brain_visualization"
    
    # Filtering and highlighting
    highlight_neurons: List[int] = None
    neuron_types: Dict[int, str] = None
    regions: Dict[int, str] = None
    time_window: Optional[Tuple[float, float]] = None
    
    def __post_init__(self):
        if self.highlight_neurons is None:
            self.highlight_neurons = []
        if self.neuron_types is None:
            self.neuron_types = {}
        if self.regions is None:
            self.regions = {}
class BrainVisualizer:
    """
    Interactive brain visualization tool for NLM.
    
    Features:
    - 2D and 3D neural network visualization
    - Real-time activity visualization
    - Connection strength visualization
    - Interactive plots with hover information
    - Export capabilities for scientific analysis
    """
    
    def __init__(self, config: BrainVisualizationConfig = None):
        self.config = config or BrainVisualizationConfig()
        self.fig = None
        self.ax = None
        
        # Initialize visualization backend
        self._initialize_backend()
    
    def _initialize_backend(self):
        """Initialize visualization backend based on configuration"""
        if self.config.visualization_type == "interactive":
            try:
                import plotly
                self.backend = "plotly"
            except ImportError:
                print("Warning: Plotly not installed, falling back to matplotlib")
                self.config.visualization_type = "2d"
                self.backend = "matplotlib"
        else:
            self.backend = "matplotlib"
    
    def visualize_network(self, 
                         neuron_positions: np.ndarray,
                         connections: np.ndarray,
                         neuron_activities: Optional[np.ndarray] = None,
                         metadata: Optional[Dict[str, Any]] = None,
                         title: str = "Neural Network Visualization"):
        """
        Create visualization of neural network structure and activity.
        
        Args:
            neuron_positions: Array of neuron positions (N x 2 or N x 3)
            connections: Adjacency matrix or list of connections
            neuron_activities: Optional activity levels for each neuron
            metadata: Optional metadata about neurons (regions, types, etc.)
            title: Visualization title
            
        Returns:
            Dictionary with visualization data and results
        """
        if metadata is None:
            metadata = {}
        
        result = {
            "title": title,
            "success": False,
            "data": {},
            "stats": {},
            "visualization": None
        }
        
        try:
            # Validate input data
            if len(neuron_positions.shape) != 2:
                raise ValueError("neuron_positions must be 2D array")
            
            n_neurons = neuron_positions.shape[0]
            
            if connections is not None:
                if isinstance(connections, np.ndarray):
                    if connections.shape != (n_neurons, n_neurons):
                        raise ValueError("Connection matrix must match neuron count")
                elif isinstance(connections, list):
                    connections = np.array(connections)
                    if connections.shape[0] != connections.shape[1] or connections.shape[1] != n_neurons:
                        raise ValueError("Connection list dimensions must match neuron count")
            
            if neuron_activities is not None:
                if len(neuron_activities) != n_neurons:
                    raise ValueError("Neuron activities must match neuron count")
                if np.any(neuron_activities < 0) or np.any(neuron_activities > 1):
                    print("Warning: Neuron activities outside [0,1] range, normalizing")
                    neuron_activities = np.clip(neuron_activities, 0, 1)
            
            # Create visualization
            visualization = self._create_visualization(
                neuron_positions, connections, neuron_activities, metadata, title
            )
            
            # Compute statistics
            stats = self._compute_visualization_stats(neuron_positions, connections, neuron_activities)
            
            result["visualization"] = visualization
            result["stats"] = stats
            result["data"] = {
                "neuron_positions": neuron_positions.tolist(),
                "connections": connections.tolist() if connections is not None else None,
                "neuron_activities": neuron_activities.tolist() if neuron_activities is not None else None,
                "metadata": metadata
            }
            result["success"] = True
            
        except Exception as e:
            result["error"] = str(e)
            result["success"] = False
        
        return result
    
    def _create_visualization(self, 
                              neuron_positions: np.ndarray,
                              connections: np.ndarray,
                              neuron_activities: Optional[np.ndarray],
                              metadata: Dict[str, Any],
                              title: str):
        """Create actual visualization based on configuration"""
        if self.config.visualization_type == "3d" and neuron_positions.shape[1] == 3:
            return self._create_3d_visualization(
                neuron_positions, connections, neuron_activities, metadata, title
            )
        elif self.config.visualization_type == "2d" or neuron_positions.shape[1] == 2:
            return self._create_2d_visualization(
                neuron_positions, connections, neuron_activities, metadata, title
            )
        else:
            return self._create_2d_visualization(
                neuron_positions[:, :2], connections, neuron_activities, metadata, title
            )
    
    def _create_2d_visualization(self,
                                 neuron_positions: np.ndarray,
                                 connections: np.ndarray,
                                 neuron_activities: Optional[np.ndarray],
                                 metadata: Dict[str, Any],
                                 title: str):
        """Create 2D visualization"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Plot connections if provided
        if connections is not None:
            self._plot_connections_2d(ax, neuron_positions, connections)
        
        # Plot neurons
        self._plot_neurons_2d(ax, neuron_positions, neuron_activities, metadata)
        
        # Customize plot
        ax.set_aspect('equal')
        ax.set_title(title, fontsize=16, fontweight='bold')
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
        ax.grid(True, alpha=0.3)
        
        # Add colorbar if activities are shown
        if neuron_activities is not None:
            sm = plt.cm.ScalarMappable(
                cmap=self._get_activity_colormap(),
                norm=plt.Normalize(vmin=0, vmax=1)
            )
            sm.set_array([])
            cbar = plt.colorbar(sm, ax=ax)
            cbar.set_label('Activity Level')
        
        plt.tight_layout()
        return fig
    
    def _create_3d_visualization(self,
                                 neuron_positions: np.ndarray,
                                 connections: np.ndarray,
                                 neuron_activities: Optional[np.ndarray],
                                 metadata: Dict[str, Any],
                                 title: str):
        """Create 3D visualization"""
        fig = plt.figure(figsize=(14, 12))
        ax = fig.add_subplot(111, projection='3d')
        
        # Plot connections if provided
        if connections is not None:
            self._plot_connections_3d(ax, neuron_positions, connections)
        
        # Plot neurons
        self._plot_neurons_3d(ax, neuron_positions, neuron_activities, metadata)
        
        # Customize plot
        ax.set_title(title, fontsize=16, fontweight='bold')
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
        ax.set_zlabel("Z Position")
        
        # Add colorbar if activities are shown
        if neuron_activities is not None:
            sm = plt.cm.ScalarMappable(
                cmap=self._get_activity_colormap(),
                norm=plt.Normalize(vmin=0, vmax=1)
            )
            sm.set_array([])
            cbar = plt.colorbar(sm, ax=ax, shrink=0.6)
            cbar.set_label('Activity Level')
        
        plt.tight_layout()
        return fig
    
    def _plot_connections_2d(self, ax, positions: np.ndarray, connections: np.ndarray):
        """Plot connections as lines or arrows"""
        # Threshold for showing connections
        threshold = np.percentile(connections[connections > 0], 75)
        
        for i in range(len(positions)):
            for j in range(i + 1, len(positions)):
                weight = connections[i, j] if connections[i, j] > 0 else connections[j, i]
                
                if weight > threshold:
                    # Normalize weight for line width
                    alpha = min(0.8, weight / connections.max())
                    linewidth = 1 + 5 * (weight / connections.max())
                    
                    # Plot connection line
                    ax.plot([positions[i, 0], positions[j, 0]],
                           [positions[i, 1], positions[j, 1]],
                           'k-', linewidth=linewidth, alpha=alpha, zorder=1)
    
    def _plot_connections_3d(self, ax, positions: np.ndarray, connections: np.ndarray):
        """Plot connections in 3D"""
        # Threshold for showing connections
        threshold = np.percentile(connections[connections > 0], 75)
        
        for i in range(len(positions)):
            for j in range(i + 1, len(positions)):
                weight = connections[i, j] if connections[i, j] > 0 else connections[j, i]
                
                if weight > threshold:
                    alpha = min(0.8, weight / connections.max())
                    linewidth = 1 + 5 * (weight / connections.max())
                    
                    # Plot connection line
                    ax.plot([positions[i, 0], positions[j, 0]],
                           [positions[i, 1], positions[j, 1]],
                           [positions[i, 2], positions[j, 2]],
                           'k-', linewidth=linewidth, alpha=alpha, zorder=1)
    
    def _plot_neurons_2d(self,
                         ax, positions: np.ndarray,
                         activities: Optional[np.ndarray],
                         metadata: Dict[str, Any]):
        """Plot neurons as points with appropriate coloring"""
        n_neurons = len(positions)
        
        # Determine colors based on configuration
        if self.config.color_scheme == "activity" and activities is not None:
            colors = self._get_activity_colormap()(activities)
        elif self.config.color_scheme == "type" and 'neuron_types' in metadata:
            colors = self._get_type_colormap(metadata['neuron_types'])
        elif self.config.color_scheme == "region" and 'regions' in metadata:
            colors = self._get_region_colormap(metadata['regions'])
        else:
            colors = self._get_default_colormap(n_neurons)
        
        # Highlight specified neurons
        if self.config.highlight_neurons:
            colors = colors.copy()
            colors[self.config.highlight_neurons] = 'red'
        
        # Plot neurons
        for i in range(n_neurons):
            size = self.config.neuron_size
            if activities is not None:
                # Scale size based on activity
                activity = activities[i]
                size = self.config.neuron_size * (0.5 + 0.5 * activity)
            
            scatter = ax.scatter(
                positions[i, 0], positions[i, 1],
                s=size, c=[colors[i]], alpha=0.7,
                edgecolors='black', linewidths=1
            )
            
            # Add label if enabled and not too many neurons
            if self.config.show_labels and n_neurons <= 100:
                ax.annotate(f"{i}", (positions[i, 0], positions[i, 1]),
                           xytext=(5, 5), textcoords='offset points',
                           fontsize=8, alpha=0.7)
    
    def _plot_neurons_3d(self,
                         ax, positions: np.ndarray,
                         activities: Optional[np.ndarray],
                         metadata: Dict[str, Any]):
        """Plot neurons in 3D"""
        n_neurons = len(positions)
        
        # Determine colors based on configuration
        if self.config.color_scheme == "activity" and activities is not None:
            colors = self._get_activity_colormap()(activities)
        elif self.config.color_scheme == "type" and 'neuron_types' in metadata:
            colors = self._get_type_colormap(metadata['neuron_types'])
        elif self.config.color_scheme == "region" and 'regions' in metadata:
            colors = self._get_region_colormap(metadata['regions'])
        else:
            colors = self._get_default_colormap(n_neurons)
        
        # Highlight specified neurons
        if self.config.highlight_neurons:
            colors = colors.copy()
            colors[self.config.highlight_neurons] = 'red'
        
        # Plot neurons
        for i in range(n_neurons):
            size = self.config.neuron_size
            if activities is not None:
                activity = activities[i]
                size = self.config.neuron_size * (0.5 + 0.5 * activity)
            
            scatter = ax.scatter(
                positions[i, 0], positions[i, 1], positions[i, 2],
                s=size, c=[colors[i]], alpha=0.7,
                edgecolors='black', linewidths=1
            )
    
    def _get_activity_colormap(self):
        """Get colormap for activity visualization"""
        return plt.cm.RdYlBu_r
    
    def _get_type_colormap(self, neuron_types: Dict[int, str]):
        """Get colormap for neuron types"""
        from matplotlib.colors import ListedColormap
        
        type_colors = {
            'excitatory': '#FF6B6B',
            'inhibitory': '#4ECDC4',
            'modulatory': '#45B7D1',
            'sensory': '#96CEB4',
            'motor': '#FFEAA7',
            'associative': '#DDA0DD'
        }
        
        color_map = {i: type_colors.get(t, '#CCCCCC') for i, t in neuron_types.items()}
        return color_map
    
    def _get_region_colormap(self, regions: Dict[int, str]):
        """Get colormap for regions"""
        from matplotlib.colors import ListedColormap
        import hashlib
        
        color_map = {}
        for i, region in regions.items():
            # Generate deterministic color based on region name
            hash_obj = hashlib.md5(region.encode())
            hex_code = '#' + hash_obj.hexdigest()[:6]
            color_map[i] = hex_code
        
        return color_map
    
    def _get_default_colormap(self, n_neurons: int):
        """Get default colormap"""
        colors = plt.cm.Set3(np.linspace(0, 1, min(n_neurons, 20)))
        
        # Extend colors if more neurons than colormap entries
        if n_neurons > len(colors):
            colors = np.vstack([colors, plt.cm.tab20(np.linspace(0, 1, n_neurons - len(colors)))])
        
        return colors[:n_neurons]
    
    def _compute_visualization_stats(self,
                                     positions: np.ndarray,
                                     connections: Optional[np.ndarray],
                                     activities: Optional[np.ndarray]):
        """Compute statistics about the visualization"""
        stats = {
            "neuron_count": len(positions),
            "dimensions": positions.shape[1],
            "has_activities": activities is not None,
            "has_connections": connections is not None,
            "density": 0.0
        }
        
        if connections is not None:
            total_possible_connections = len(positions) * (len(positions) - 1) / 2
            actual_connections = np.count_nonzero(np.triu(connections > 0.5, k=1))
            stats["density"] = actual_connections / total_possible_connections if total_possible_connections > 0 else 0.0
            stats["connection_count"] = actual_connections
            stats["max_connection_weight"] = np.max(connections[connections > 0])
            stats["mean_connection_weight"] = np.mean(connections[connections > 0]) if np.any(connections > 0) else 0.0
        
        if activities is not None:
            stats["activity_stats"] = {
                "mean": float(np.mean(activities)),
                "std": float(np.std(activities)),
                "min": float(np.min(activities)),
                "max": float(np.max(activities)),
                "active_neurons": int(np.sum(activities > 0.5))
            }
        
        # Compute spatial statistics
        if positions.shape[1] == 2:
            from scipy import stats
            stats["spatial_stats_2d"] = {
                "x_mean": float(np.mean(positions[:, 0])),
                "x_std": float(np.std(positions[:, 0])),
                "y_mean": float(np.mean(positions[:, 1])),
                "y_std": float(np.std(positions[:, 1]))
            }
        elif positions.shape[1] == 3:
            stats["spatial_stats_3d"] = {
                "x_mean": float(np.mean(positions[:, 0])),
                "x_std": float(np.std(positions[:, 0])),
                "y_mean": float(np.mean(positions[:, 1])),
                "y_std": float(np.std(positions[:, 1])),
                "z_mean": float(np.mean(positions[:, 2])),
                "z_std": float(np.std(positions[:, 2]))
            }
        
        return stats

    def save_visualization(self, fig, filename: str = None) -> str:
        """
        Save visualization to file.
        
        Args:
            fig: matplotlib figure to save
            filename: output filename (uses config output_path if None)
            
        Returns:
            Path to saved file
        """
        if filename is None:
            filename = f"{self.config.output_path}.{self.config.save_format}"
        
        # Ensure directory exists
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        if self.config.save_format == "html" and hasattr(fig, 'write_html'):
            fig.write_html(filename)
        else:
            plt.savefig(filename, dpi=300, bbox_inches='tight')
        
        return filename
    
    def export_visualization_data(self, result: Dict[str, Any], filename: str) -> str:
        """
        Export visualization data to JSON file.
        
        Args:
            result: visualization result dictionary
            filename: output filename
            
        Returns:
            Path to saved file
        """
        os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        with open(filename, 'w') as f:
            json.dump(result, f, indent=2)
        
        return filename


def create_brain_visualizer(config_dict: Dict[str, Any] = None) -> BrainVisualizer:
    """
    Factory function to create BrainVisualizer with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured BrainVisualizer instance
    """
    if config_dict is None:
        config_dict = {}
    
    config = BrainVisualizationConfig(**config_dict)
    return BrainVisualizer(config)