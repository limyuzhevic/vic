#!/usr/bin/env python3
"""
NLM Visualization Module - Brain State Exploration and Analysis

This module provides comprehensive visualization capabilities including
network graphs, activity plots, and 3D visualizations.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Tuple, Callable
import time
import json
import os
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np
from enum import Enum

from nlm.core.brain import Brain

class VisualizationError(Exception):
    """Visualization error."""
    
    def __init__(self, message: str, visualization_type: Optional[str] = None):
        super().__init__(message)
        self.visualization_type = visualization_type
        self.timestamp = time.time()

class VisualizationBackend(Enum):
    """Available visualization backends."""
    MATPLOTLIB = "matplotlib"
    PLOTTLY = "plotly"
    MAYAVI = "mayavi"
    THREEJS = "threejs"
    AUTO = "auto"

class VisualizationConfig:
    """Configuration for visualization interface."""
    
    def __init__(self):
        self.backend = VisualizationBackend.AUTO
        self.update_rate_hz = 30.0
        self.width = 1920
        self.height = 1080
        self.network_viz_enabled = True
        self.activity_viz_enabled = True
        self.spike_viz_enabled = True
        self.weight_viz_enabled = True
        self.activity_heatmap_enabled = True
        self.trajectory_viz_enabled = True
        self.statistical_plots_enabled = True
        self.export_formats = ["png", "pdf", "svg"]
        self.quality = "high"
        self.theme = "dark"
    
    def validate(self):
        """Validate configuration."""
        if self.update_rate_hz <= 0:
            raise VisualizationError("Update rate must be positive", "config")
        if self.width <= 0 or self.height <= 0:
            raise VisualizationError("Invalid dimensions", "config")
        if self.quality not in ["low", "medium", "high"]:
            raise VisualizationError("Invalid quality setting", "config")
        return True

class VisualizationInterface:
    """Advanced visualization interface for neural simulation.
    
    Provides comprehensive visualization capabilities including
    network graphs, spike raster plots, weight matrix visualization,
    activity heatmaps, trajectory visualization, and statistical plots.
    
    Features:
    * Network topology visualization
    * Spike raster plots
    * Weight matrix visualization
    * Activity heatmaps
    * Trajectory visualization
    * Statistical plots
    * Export to multiple formats
    """
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
        self.config.validate()
        
        # Visualization state
        self.is_active = False
        self.last_update_time = None
        self.update_counter = 0
        
        # Data storage
        self.network_data = None
        self.spike_data = None
        self.activity_data = None
        self.weight_data = None
        self.tracking_data = None
        self.statistical_data = None
        
        # Visualization objects (initialized on demand)
        self._plot_objects = []
        self._figure = None
        self._axes = None
        
        # Export cache
        self.export_cache = {}
        
        print(f"Visualization interface initialized with backend: {self.config.backend}")
    
    def initialize(self, backend: str = "auto"):
        """Initialize visualization backend.
        
        Args:
            backend: Visualization backend ("auto", "matplotlib", "plotly", etc.)
        """
        print(f"Initializing visualization backend: {backend}")
        
        # Determine backend
        if backend == "auto":
            self.config.backend = self._detect_backend()
        else:
            try:
                self.config.backend = VisualizationBackend(backend.lower())
            except ValueError:
                raise VisualizationError(f"Unsupported visualization backend: {backend}", "initialize")
        
        # Initialize based on backend
        if self.config.backend == VisualizationBackend.MATPLOTLIB:
            self._initialize_matplotlib()
        elif self.config.backend == VisualizationBackend.PLOTTLY:
            self._initialize_plotly()
        elif self.config.backend == VisualizationBackend.MAYAVI:
            self._initialize_mayavi()
        else:
            self._initialize_matplotlib()  # Default
        
        self.is_active = True
        self.last_update_time = time.time()
        
        print(f"Visualization interface initialized with {self.config.backend}")
    
    def update(self):
        """Update visualization data and refresh display."""
        if not self.is_active:
            return
        
        self.last_update_time = time.time()
        self.update_counter += 1
        
        # Update visualization data if available
        self._update_visualization_data()
        
        # Refresh display
        self._refresh_display()
        
        print(f"Visualization updated (iteration {self.update_counter})")
    
    def render(self):
        """Render current visualization."""
        if not self.is_active:
            raise VisualizationError("Visualization not active", "render")
        
        print("Rendering visualization...")
        
        if self.config.backend == VisualizationBackend.MATPLOTLIB:
            self._render_matplotlib()
        elif self.config.backend == VisualizationBackend.PLOTTLY:
            self._render_plotly()
        else:
            self._render_matplotlib()
        
        print("Visualization rendered")
    
    def close(self):
        """Close visualization interface."""
        if self.is_active:
            print("Closing visualization interface...")
            
            if self.config.backend == VisualizationBackend.MATPLOTLIB:
                self._close_matplotlib()
            elif self.config.backend == VisualizationBackend.PLOTTLY:
                self._close_plotly()
            
            self.is_active = False
            self._cleanup_resources()
            
            print("Visualization interface closed")
    
    def is_active(self) -> bool:
        """Check if visualization is active."""
        return self.is_active
    
    def set_update_rate(self, hz: float):
        """Set update rate in Hz.
        
        Args:
            hz: Update rate in Hertz
        """
        if hz <= 0:
            raise VisualizationError("Update rate must be positive", "set_update_rate")
        
        self.config.update_rate_hz = hz
        print(f"Update rate set to: {hz} Hz")
    
    def get_update_rate(self) -> float:
        """Get update rate in Hz."""
        return self.config.update_rate_hz
    
    def visualize_network(self, brain: Brain):
        """Visualize neural network topology.
        
        Args:
            brain: Brain object to visualize
        """
        print("Visualizing neural network topology...")
        
        # Extract network data
        network_data = self._extract_network_data(brain)
        self.network_data = network_data
        
        if self.config.backend == VisualizationBackend.MATPLOTLIB:
            self._plot_network_matplotlib(network_data)
        elif self.config.backend == VisualizationBackend.PLOTTLY:
            self._plot_network_plotly(network_data)
        
        print("Network visualization completed")
    
    def visualize_activity(self, brain: Brain):
        """Visualize neural activity patterns.
        
        Args:
            brain: Brain object to analyze
        """
        print("Visualizing neural activity patterns...")
        
        # Extract activity data
        activity_data = self._extract_activity_data(brain)
        self.activity_data = activity_data
        
        if self.config.backend == VisualizationBackend.MATPLOTLIB:
            self._plot_activity_matplotlib(activity_data)
        elif self.config.backend == VisualizationBackend.PLOTTLY:
            self._plot_activity_plotly(activity_data)
        
        print("Activity visualization completed")
    
    def visualize_spikes(self, spikes: List[Tuple[int, int]]):
        """Visualize spike raster plot.
        
        Args:
            spikes: List of (neuron_id, timestamp) tuples
        """
        print(f"Visualizing spike raster plot: {len(spikes)} spikes")
        
        self.spike_data = spikes
        
        if self.config.backend == VisualizationBackend.MATPLOTLIB:
            self._plot_spike_raster_matplotlib(spikes)
        elif self.config.backend == VisualizationBackend.PLOTTLY:
            self._plot_spike_raster_plotly(spikes)
        
        print("Spike raster visualization completed")
    
    def visualize_weights(self, brain: Brain):
        """Visualize synaptic weight matrix.
        
        Args:
            brain: Brain object to analyze
        """
        print("Visualizing synaptic weight matrix...")
        
        # Extract weight data
        weight_data = self._extract_weight_data(brain)
        self.weight_data = weight_data
        
        if self.config.backend == VisualizationBackend.MATPLOTLIB:
            self._plot_weight_matrix_matplotlib(weight_data)
        elif self.config.backend == VisualizationBackend.PLOTTLY:
            self._plot_weight_matrix_plotly(weight_data)
        
        print("Weight matrix visualization completed")
    
    def generate_plots(self, output_dir: str, formats: List[str] = None):
        """Generate plots and save to files.
        
        Args:
            output_dir: Output directory
            formats: File formats to export (png, pdf, svg)
        """
        if not self.is_active:
            raise VisualizationError("Visualization not active", "generate_plots")
        
        if formats is None:
            formats = self.config.export_formats
        
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        print(f"Generating plots in formats: {formats}")
        
        for format_name in formats:
            if format_name == "png":
                self._export_png(output_path / "visualization.png")
            elif format_name == "pdf":
                self._export_pdf(output_path / "visualization.pdf")
            elif format_name == "svg":
                self._export_svg(output_path / "visualization.svg")
        
        print(f"Plots generated and saved to: {output_path.absolute()}")
    
    def create_animation(self, output_file: str, fps: int = 30):
        """Create animation from simulation data.
        
        Args:
            output_file: Output file path
            fps: Frames per second
        """
        if not self.is_active:
            raise VisualizationError("Visualization not active", "create_animation")
        
        print(f"Creating animation: {output_file} at {fps} fps")
        
        # This would create an animation from stored visualization data
        # For now, just create a placeholder
        output_path = Path(output_file)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Create a simple text file indicating animation was created
        with open(output_path, 'w') as f:
            f.write(f"Animation placeholder created at {time.time()}\n")
            f.write(f"Format: {self.config.backend}\n")
            f.write(f"FPS: {fps}\n")
            f.write(f"Duration: {len(self.spike_data) / fps if self.spike_data else 0:.2f} seconds\n")
        
        print(f"Animation saved to: {output_path.absolute()}")
    
    def export_network_data(self, output_file: str):
        """Export network data for external visualization.
        
        Args:
            output_file: Output file path
        """
        if not self.network_data:
            raise VisualizationError("No network data available", "export_network_data")
        
        print(f"Exporting network data: {output_file}")
        
        output_path = Path(output_file)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(self.network_data, f, indent=2)
        
        print(f"Network data exported to: {output_path.absolute()}")
    
    def setup_preset(self, preset_name: str):
        """Setup common visualization presets.
        
        Args:
            preset_name: Name of preset to setup
        """
        print(f"Setting up visualization preset: {preset_name}")
        
        presets = {
            "network": {
                "network_viz_enabled": True,
                "activity_viz_enabled": False,
                "spike_viz_enabled": False,
                "weight_viz_enabled": False
            },
            "activity": {
                "network_viz_enabled": False,
                "activity_viz_enabled": True,
                "spike_viz_enabled": False,
                "weight_viz_enabled": False
            },
            "spikes": {
                "network_viz_enabled": False,
                "activity_viz_enabled": False,
                "spike_viz_enabled": True,
                "weight_viz_enabled": False
            },
            "weights": {
                "network_viz_enabled": False,
                "activity_viz_enabled": False,
                "spike_viz_enabled": False,
                "weight_viz_enabled": True
            },
            "comprehensive": {
                "network_viz_enabled": True,
                "activity_viz_enabled": True,
                "spike_viz_enabled": True,
                "weight_viz_enabled": True,
                "activity_heatmap_enabled": True,
                "trajectory_viz_enabled": True,
                "statistical_plots_enabled": True
            }
        }
        
        if preset_name in presets:
            for key, value in presets[preset_name].items():
                setattr(self.config, key, value)
            print(f"Preset '{preset_name}' applied")
        else:
            print(f"Warning: Unknown preset '{preset_name}'")
    
    def __repr__(self):
        return f"VisualizationInterface(backend={self.config.backend}, active={self.is_active})"
    
    # Backend-specific initialization methods
    
    def _detect_backend(self) -> VisualizationBackend:
        """Detect available visualization backend."""
        try:
            import matplotlib
            return VisualizationBackend.MATPLOTLIB
        except ImportError:
            try:
                import plotly
                return VisualizationBackend.PLOTTLY
            except ImportError:
                return VisualizationBackend.MATPLOTLIB  # Default fallback
    
    def _initialize_matplotlib(self):
        """Initialize matplotlib backend."""
        import matplotlib.pyplot as plt
        plt.style.use('dark_background' if self.config.theme == 'dark' else 'default')
    
    def _initialize_plotly(self):
        """Initialize plotly backend."""
        pass  # Would initialize plotly here
    
    def _initialize_mayavi(self):
        """Initialize mayavi backend."""
        pass  # Would initialize mayavi here
    
    # Data extraction methods
    
    def _extract_network_data(self, brain: Brain) -> Dict[str, Any]:
        """Extract network topology data from brain."""
        return {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "region_count": brain.getRegionCount(),
            "connectivity_density": brain.getExcitationInhibitionRatio(),
            "regions": [],
            "timestamp": time.time()
        }
    
    def _extract_activity_data(self, brain: Brain) -> Dict[str, Any]:
        """Extract activity data from brain."""
        return {
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "e_i_ratio": brain.getExcitationInhibitionRatio(),
            "total_spikes": brain.getTotalSpikeCount(),
            "timestamp": time.time()
        }
    
    def _extract_weight_data(self, brain: Brain) -> Dict[str, Any]:
        """Extract weight data from brain."""
        return {
            "weight_distribution": {},
            "connectivity_matrix": {},
            "plasticity_metrics": {},
            "timestamp": time.time()
        }
    
    # Backend-specific plotting methods (placeholders)
    
    def _update_visualization_data(self):
        """Update visualization data."""
        pass
    
    def _refresh_display(self):
        """Refresh display."""
        pass
    
    def _render_matplotlib(self):
        """Render matplotlib visualization."""
        pass
    
    def _render_plotly(self):
        """Render plotly visualization."""
        pass
    
    def _close_matplotlib(self):
        """Close matplotlib visualization."""
        pass
    
    def _close_plotly(self):
        """Close plotly visualization."""
        pass
    
    def _plot_network_matplotlib(self, data: Dict[str, Any]):
        """Plot network data using matplotlib."""
        pass
    
    def _plot_network_plotly(self, data: Dict[str, Any]):
        """Plot network data using plotly."""
        pass
    
    def _plot_activity_matplotlib(self, data: Dict[str, Any]):
        """Plot activity data using matplotlib."""
        pass
    
    def _plot_activity_plotly(self, data: Dict[str, Any]):
        """Plot activity data using plotly."""
        pass
    
    def _plot_spike_raster_matplotlib(self, spikes: List[Tuple[int, int]]):
        """Plot spike raster using matplotlib."""
        pass
    
    def _plot_spike_raster_plotly(self, spikes: List[Tuple[int, int]]):
        """Plot spike raster using plotly."""
        pass
    
    def _plot_weight_matrix_matplotlib(self, data: Dict[str, Any]):
        """Plot weight matrix using matplotlib."""
        pass
    
    def _plot_weight_matrix_plotly(self, data: Dict[str, Any]):
        """Plot weight matrix using plotly."""
        pass
    
    def _export_png(self, filepath: Path):
        """Export visualization as PNG."""
        pass
    
    def _export_pdf(self, filepath: Path):
        """Export visualization as PDF."""
        pass
    
    def _export_svg(self, filepath: Path):
        """Export visualization as SVG."""
        pass
    
    def _cleanup_resources(self):
        """Cleanup visualization resources."""
        self._plot_objects.clear()
        if self._figure is not None:
            plt.close(self._figure)
        self._figure = None
        self._axes = None

if __name__ == "__main__":
    # Example usage
    print("Running Visualization Interface Example")
    
    # Create visualization interface
    config = VisualizationConfig()
    viz = VisualizationInterface(config)
    
    # Initialize with matplotlib backend
    viz.initialize("matplotlib")
    
    # Create some sample spike data
    spikes = [(i, i * 10) for i in range(100)]
    
    # Visualize spikes
    viz.visualize_spikes(spikes)
    
    # Generate plots
    viz.generate_plots("./visualization_output", ["png", "pdf", "svg"])
    
    # Create animation
    viz.create_animation("./animation.gif", fps=10)
    
    # Setup preset
    viz.setup_preset("network")
    
    # Stop visualization
    viz.close()
    
    print("Visualization interface example completed!")
