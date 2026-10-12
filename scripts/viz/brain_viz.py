#!/usr/bin/env python3
"""
Brain Visualization Tool

Real-time brain visualization with neuron states, network connectivity graphs,
synaptic weight visualization, activity heatmaps and time series.
Supports different visualization modes (network, activity, connectivity).

Features:
- Real-time brain visualization with neuron states
- Network connectivity graphs with interactive exploration
- Synaptic weight visualization with heatmaps
- Activity heatmaps and time series analysis
- Multiple visualization modes for different perspectives
- Interactive 3D brain visualization
- Web-based real-time dashboards
- Export capabilities in multiple formats

Usage Examples:
    # Basic connectivity visualization
    python scripts/viz/brain_viz.py --mode network --input data/sim_data.json --output viz.png

    # Activity heatmap visualization
    python scripts/viz/brain_viz.py --mode activity --input data/sim_data.json --output activity_heatmap.png

    # Real-time visualization from simulation
    python scripts/viz/brain_viz.py --mode realtime --simulation brain_sim.py --output realtime_viz.mp4

    # 3D connectivity graph
    python scripts/viz/brain_viz.py --mode 3d --input data/sim_data.json --output 3d_connectivity.html

API References:
    BrainViz.create_connectivity_graph(neurons, connections, output_path, format='png')
    BrainViz.create_activity_heatmap(data, output_path, format='png')
    BrainViz.create_synaptic_weights(weights, output_path, format='png')
    BrainViz.create_time_series(activity_data, output_path, format='png')
    BrainViz.create_interactive_dashboard(data, host='localhost', port=8080)
    BrainViz.create_3d_visualization(neurons, connections, output_path, format='html')
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import networkx as nx
import pandas as pd
from pathlib import Path
import argparse
import json
import sys
import time
from typing import Dict, List, Any, Optional, Tuple, Union
from datetime import datetime
import seaborn as sns
from scipy import stats
import os
from concurrent.futures import ThreadPoolExecutor
import threading
from dataclasses import dataclass
from enum import Enum
import warnings
warnings.filterwarnings('ignore')

# Optional imports for enhanced visualization
try:
    import plotly.graph_objects as go
    import plotly.express as px
    PLOTLY_AVAILABLE = True
except ImportError:
    PLOTLY_AVAILABLE = False

try:
    from mpl_toolkits.mplot3d import Axes3D
    THREE_D_AVAILABLE = True
except ImportError:
    THREE_D_AVAILABLE = False


class VisualizationMode(Enum):
    """Available visualization modes."""
    NETWORK = "network"
    ACTIVITY = "activity"
    CONNECTIVITY = "connectivity"
    TIME_SERIES = "time_series"
    HEATMAP = "heatmap"
    3D = "3d"
    REALTIME = "realtime"
    INTERACTIVE = "interactive"


class ColorScheme(Enum):
    """Color schemes for visualization."""
    NEURAL = "neural"
    RAINBOW = "rainbow"
    MONOCHROME = "monochrome"
    BRAIN = "brain"
    CURIOSITY = "curiosity"


@dataclass
class NeuronData:
    """Represents neuron data for visualization."""
    id: str
    x: float
    y: float
    z: float = 0.0
    type: str = "regular"
    layer: int = 0
    activation: float = 0.0
    state: str = "active"
    connections: List[str] = None
    properties: Dict[str, Any] = None
    metadata: Dict[str, Any] = None

    def __post_init__(self):
        if self.connections is None:
            self.connections = []
        if self.properties is None:
            self.properties = {}
        if self.metadata is None:
            self.metadata = {}


@dataclass
class ConnectionData:
    """Represents connection between neurons."""
    source: str
    target: str
    weight: float = 1.0
    type: str = "excitatory"
    strength: float = 0.5
    delay: float = 0.0
    active: bool = True
    properties: Dict[str, Any] = None

    def __post_init__(self):
        if self.properties is None:
            self.properties = {}


@dataclass
class ActivityData:
    """Represents neural activity over time."""
    time_step: int
    neurons: Dict[str, float]
    global_activity: float = 0.0
    neuromodulation: Dict[str, float] = None
    memory_activity: Dict[str, float] = None

    def __post_init__(self):
        if self.neuromodulation is None:
            self.neuromodulation = {}
        if self.memory_activity is None:
            self.memory_activity = {}


class BrainViz:
    """Comprehensive brain visualization tool for NLM simulations."""
    
    def __init__(self, style: str = "default", color_scheme: str = "neural",
                 interactive: bool = False, real_time: bool = False):
        """Initialize BrainViz.
        
        Args:
            style: Matplotlib style to use
            color_scheme: Color scheme for visualization
            interactive: Enable interactive plots
            real_time: Enable real-time visualization mode
        """
        self.style = style
        self.color_scheme = color_scheme
        self.interactive = interactive
        self.real_time = real_time
        self.current_mode = VisualizationMode.NETWORK
        self.current_frame = 0
        self.simulation_running = False
        self.update_lock = threading.Lock()
        
        # Set style
        plt.style.use(style)
        
        # Set color scheme
        self.color_map = self._get_color_scheme(color_scheme)
        
        # Initialize containers
        self.neurons: List[NeuronData] = []
        self.connections: List[ConnectionData] = []
        self.activities: List[ActivityData] = []
        self.frames: List[Dict[str, Any]] = []
        
        # Setup visualization backend
        self._setup_backend()
    
    def _get_color_scheme(self, scheme: str) -> Dict:
        """Get color scheme based on name."""
        schemes = {
            "neural": {
                "excitatory": "#FF4444",      # Red
                "inhibitory": "#4444FF",     # Blue
                "interneuron": "#44FF44",    # Green
                "regular": "#FFA500",        # Orange
                "background": "#1A1A1A",     # Dark background
                "grid": "#333333",          # Grid lines
                "activity_high": "#FF6B6B",
                "activity_medium": "#4ECDC4",
                "activity_low": "#95E1D3"
            },
            "rainbow": {
                "excitatory": "#FF0000",
                "inhibitory": "#0000FF",
                "interneuron": "#00FF00",
                "regular": "#FFFF00",
                "background": "#000000",
                "grid": "#333333",
                "activity_high": "#FF6B6B",
                "activity_medium": "#4ECDC4",
                "activity_low": "#95E1D3"
            },
            "monochrome": {
                "excitatory": "#000000",
                "inhibitory": "#404040",
                "interneuron": "#808080",
                "regular": "#C0C0C0",
                "background": "#FFFFFF",
                "grid": "#E0E0E0",
                "activity_high": "#000000",
                "activity_medium": "#808080",
                "activity_low": "#C0C0C0"
            },
            "brain": {
                "excitatory": "#FF3E3E",      # Brain red
                "inhibitory": "#3E5AFF",     # Brain blue
                "interneuron": "#3AFF3E",    # Brain green
                "regular": "#FFA23E",        # Brain orange
                "background": "#1A0F2E",     # Brain purple background
                "grid": "#3A2E5E",          # Brain grid
                "activity_high": "#FF6B6B",
                "activity_medium": "#4ECDC4",
                "activity_low": "#95E1D3"
            },
            "curiosity": {
                "excitatory": "#FF9F43",     # Curiosity orange
                "inhibitory": "#00D2D3",     # Curiosity cyan
                "interneuron": "#A29BFE",    # Curiosity purple
                "regular": "#FFEAA7",        # Curiosity yellow
                "background": "#2D3436",     # Curiosity dark
                "grid": "#636E72",          # Curiosity medium
                "activity_high": "#FF6B6B",
                "activity_medium": "#4ECDC4",
                "activity_low": "#95E1D3"
            }
        }
        return schemes.get(scheme, schemes["neural"])
    
    def _setup_backend(self):
        """Setup visualization backend based on capabilities."""
        if self.interactive and PLOTLY_AVAILABLE:
            self.backend = "plotly"
        elif THREE_D_AVAILABLE:
            self.backend = "mpl_toolkits"
        else:
            self.backend = "matplotlib"
    
    def add_neurons(self, neurons: List[Dict[str, Any]]):
        """Add neurons to visualization.
        
        Args:
            neurons: List of neuron dictionaries
        """
        for neuron_dict in neurons:
            neuron = NeuronData(
                id=str(neuron_dict.get('id', f"neuron_{len(self.neurons)}")),
                x=float(neuron_dict.get('x', np.random.uniform(0, 10))),
                y=float(neuron_dict.get('y', np.random.uniform(0, 10))),
                z=float(neuron_dict.get('z', 0.0)),
                type=str(neuron_dict.get('type', 'regular')),
                layer=int(neuron_dict.get('layer', 0)),
                activation=float(neuron_dict.get('activation', 0.0)),
                state=str(neuron_dict.get('state', 'active')),
                connections=neuron_dict.get('connections', []),
                properties=neuron_dict.get('properties', {}),
                metadata=neuron_dict.get('metadata', {})
            )
            self.neurons.append(neuron)
    
    def add_connections(self, connections: List[Dict[str, Any]]):
        """Add connections to visualization.
        
        Args:
            connections: List of connection dictionaries
        """
        for conn_dict in connections:
            connection = ConnectionData(
                source=str(conn_dict.get('source')),
                target=str(conn_dict.get('target')),
                weight=float(conn_dict.get('weight', 1.0)),
                type=str(conn_dict.get('type', 'excitatory')),
                strength=float(conn_dict.get('strength', 0.5)),
                delay=float(conn_dict.get('delay', 0.0)),
                active=bool(conn_dict.get('active', True)),
                properties=conn_dict.get('properties', {})
            )
            self.connections.append(connection)
    
    def add_activity(self, activity: Dict[str, Any]):
        """Add activity data to visualization.
        
        Args:
            activity: Activity data dictionary
        """
        activity_data = ActivityData(
            time_step=activity.get('time_step', len(self.activities)),
            neurons=activity.get('neurons', {}),
            global_activity=activity.get('global_activity', 0.0),
            neuromodulation=activity.get('neuromodulation', {}),
            memory_activity=activity.get('memory_activity', {})
        )
        self.activities.append(activity_data)
    
    def create_connectivity_graph(self, neurons: List[Dict[str, Any]], 
                                 connections: List[Dict[str, Any]],
                                 output_path: Optional[str] = None,
                                 format: str = "png") -> str:
        """Create neuron connectivity graph visualization.
        
        Args:
            neurons: List of neuron data dictionaries
            connections: List of connection data dictionaries
            output_path: Output file path (auto-generated if None)
            format: Output format (png, svg, pdf, html)
            
        Returns:
            Path to saved visualization
        """
        # Add data to internal containers
        self.neurons = []
        self.connections = []
        self.add_neurons(neurons)
        self.add_connections(connections)
        
        # Choose backend
        if self.backend == "plotly" and PLOTLY_AVAILABLE:
            return self._create_plotly_connectivity(output_path, format)
        elif self.backend == "mpl_toolkits" and THREE_D_AVAILABLE:
            return self._create_3d_connectivity(output_path, format)
        else:
            return self._create_matplotlib_connectivity(output_path, format)
    
    def _create_matplotlib_connectivity(self, output_path: Optional[str], 
                                      format: str) -> str:
        """Create 2D connectivity graph using matplotlib."""
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"connectivity_graph_{timestamp}.{format}"
        
        fig, axes = plt.subplots(1, 2, figsize=(18, 12))
        fig.suptitle('NLM Brain Connectivity Graph', fontsize=20, fontweight='bold')
        
        # Create network graph
        G = nx.Graph()
        
        # Add nodes with position attributes
        node_positions = {}
        node_colors = []
        node_sizes = []
        node_labels = {}
        
        for neuron in self.neurons:
            # Determine node color based on type
            node_color = self.color_map.get(neuron.type, "gray")
            
            # Node size based on activation
            base_size = 100
            activation_factor = neuron.activation * 500
            size = base_size + activation_factor
            
            node_positions[neuron.id] = (neuron.x, neuron.y)
            node_colors.append(node_color)
            node_sizes.append(size)
            node_labels[neuron.id] = f"{neuron.id}\n({neuron.layer})"
        
        # Add nodes to graph
        G.add_nodes_from(node_positions.keys())
        
        # Add edges
        for conn in self.connections:
            if conn.active:  # Only show active connections
                weight = conn.weight
                G.add_edge(conn.source, conn.target, 
                          weight=weight, type=conn.type)
        
        # Draw network - Part 1: Standard view
        ax = axes[0]
        
        # Draw nodes
        nx.draw_networkx_nodes(
            G, node_positions, node_color=node_colors, 
            node_size=node_sizes, alpha=0.8, ax=ax
        )
        
        # Draw edges with width based on weight
        edge_widths = [G[u][v]['weight'] * 3 for u, v in G.edges()]
        edge_colors = [G[u][v]['type'] for u, v in G.edges()]
        
        # Map edge colors to actual colors
        color_mapping = {
            'excitatory': self.color_map['excitatory'],
            'inhibitory': self.color_map['inhibitory'],
            'modulatory': self.color_map['interneuron'],
            'recurrent': self.color_map['regular'],
            'feedforward': self.color_map['excitatory'],
            'feedback': self.color_map['inhibitory']
        }
        
        mapped_edge_colors = [color_mapping.get(c, 'gray') for c in edge_colors]
        
        nx.draw_networkx_edges(
            G, node_positions, width=edge_widths, 
            edge_color=mapped_edge_colors, alpha=0.6, ax=ax
        )
        
        nx.draw_networkx_labels(
            G, node_positions, labels=node_labels,
            font_size=8, ax=ax
        )
        
        ax.set_title("Neuron Connectivity Network", fontsize=14, fontweight='bold')
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')
        
        # Draw network - Part 2: Zoomed-in view of high-density areas
        ax = axes[1]
        
        # Calculate clustering and highlight communities
        if len(G.nodes()) > 0:
            try:
                communities = nx.community.louvain_communities(G, resolution=1.0)
                
                # Assign colors to communities
                community_colors = plt.cm.Set3(np.linspace(0, 1, len(communities)))
                community_color_map = {}
                
                for i, community in enumerate(communities):
                    for node in community:
                        community_color_map[node] = community_colors[i]
                
                # Draw communities
                for node, color in community_color_map.items():
                    x, y = node_positions[node]
                    ax.scatter(x, y, color=color, s=node_sizes[list(G.nodes()).index(node)], 
                              alpha=0.7, edgecolors='black', linewidths=1)
                
                # Draw edges within communities (thicker)
                for u, v in G.edges():
                    if u in community_color_map and v in community_color_map:
                        if community_color_map[u] == community_color_map[v]:
                            # Edge within same community
                            edge_width = G[u][v]['weight'] * 5
                            ax.plot([node_positions[u][0], node_positions[v][0]],
                                   [node_positions[u][1], node_positions[v][1]],
                                   color='black', linewidth=edge_width, alpha=0.8)
                        else:
                            # Edge between communities
                            edge_width = G[u][v]['weight'] * 2
                            ax.plot([node_positions[u][0], node_positions[v][0]],
                                   [node_positions[u][1], node_positions[v][1]],
                                   color='gray', linewidth=edge_width, alpha=0.4, linestyle='--')
                
                # Draw connections to community centers
                for i, community in enumerate(communities):
                    if community:
                        center_x = np.mean([node_positions[node][0] for node in community])
                        center_y = np.mean([node_positions[node][1] for node in community])
                        ax.scatter(center_x, center_y, color='red', marker='*', s=200, 
                                  alpha=0.8, edgecolors='darkred', linewidths=2)
                        ax.text(center_x, center_y + 0.5, f"Community {i}", 
                               fontsize=10, ha='center', fontweight='bold')
            except Exception:
                # Fallback if community detection fails
                pass
        
        ax.set_title("Community Structure", fontsize=14, fontweight='bold')
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')
        
        plt.tight_layout()
        
        # Save figure
        plt.savefig(output_path, format=format, dpi=300, bbox_inches='tight',
                   facecolor='white', edgecolor='none')
        plt.close()
        
        return output_path
    
    def _create_3d_connectivity(self, output_path: Optional[str], 
                              format: str) -> str:
        """Create 3D connectivity graph visualization."""
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"connectivity_graph_3d_{timestamp}.{format}"
        
        # For HTML output with 3D visualization
        if format == "html" and PLOTLY_AVAILABLE:
            return self._create_interactive_3d(output_path)
        
        # 3D matplotlib visualization
        fig = plt.figure(figsize=(16, 12))
        ax = fig.add_subplot(111, projection='3d')
        
        # Prepare 3D data
        xs = [neuron.x for neuron in self.neurons]
        ys = [neuron.y for neuron in self.neurons]
        zs = [neuron.z for neuron in self.neurons]
        
        # Determine colors based on layer and type
        colors = []
        for neuron in self.neurons:
            if neuron.type == "excitatory":
                color = self.color_map["excitatory"]
            elif neuron.type == "inhibitory":
                color = self.color_map["inhibitory"]
            elif neuron.type == "interneuron":
                color = self.color_map["interneuron"]
            else:
                color = self.color_map["regular"]
            
            colors.append(color)
        
        # Scatter plot neurons in 3D
        scatter = ax.scatter(xs, ys, zs, c=colors, s=100, alpha=0.8, depthshade=True)
        
        # Draw connections in 3D
        for conn in self.connections:
            if conn.active:
                source_neuron = next((n for n in self.neurons if n.id == conn.source), None)
                target_neuron = next((n for n in self.neurons if n.id == conn.target), None)
                
                if source_neuron and target_neuron:
                    ax.plot([source_neuron.x, target_neuron.x],
                           [source_neuron.y, target_neuron.y],
                           [source_neuron.z, target_neuron.z],
                           color='gray', alpha=0.5, linewidth=conn.weight * 2)
        
        ax.set_title("3D Brain Connectivity Network", fontsize=16, fontweight='bold')
        ax.set_xlabel("X Position")
        ax.set_ylabel("Y Position")
        ax.set_zlabel("Z Position")
        
        # Add colorbar
        cbar = plt.colorbar(scatter, ax=ax, shrink=0.5)
        cbar.set_label('Neuron Type')
        
        plt.tight_layout()
        
        # Save figure
        if format == "png":
            plt.savefig(output_path, format=format, dpi=300, bbox_inches='tight')
        elif format in ["svg", "pdf"]:
            plt.savefig(output_path, format=format, bbox_inches='tight')
        
        plt.close()
        
        return output_path
    
    def _create_plotly_connectivity(self, output_path: Optional[str], 
                                  format: str) -> str:
        """Create interactive connectivity graph using plotly."""
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"connectivity_graph_interactive_{timestamp}.html"
        
        # Create plotly figure
        fig = go.Figure()
        
        # Add nodes
        node_x = [neuron.x for neuron in self.neurons]
        node_y = [neuron.y for neuron in self.neurons]
        node_colors = []
        
        for neuron in self.neurons:
            if neuron.type == "excitatory":
                color = self.color_map["excitatory"]
            elif neuron.type == "inhibitory":
                color = self.color_map["inhibitory"]
            elif neuron.type == "interneuron":
                color = self.color_map["interneuron"]
            else:
                color = self.color_map["regular"]
            
            node_colors.append(color)
        
        # Add node trace
        fig.add_trace(go.Scatter(
            x=node_x,
            y=node_y,
            mode='markers+text',
            marker=dict(
                size=[max(10, neuron.activation * 50) for neuron in self.neurons],
                color=node_colors,
                line=dict(color='black', width=1)
            ),
            text=[f"{neuron.id}\nL{neuron.layer}" for neuron in self.neurons],
            textposition="top center",
            hovertemplate='<b>%{text}</b><br>Activation: %{marker.size:.2f}<extra></extra>'
        ))
        
        # Add edges
        edge_x = []
        edge_y = []
        edge_info = []
        
        for conn in self.connections:
            if conn.active:
                source_neuron = next((n for n in self.neurons if n.id == conn.source), None)
                target_neuron = next((n for n in self.neurons if n.id == conn.target), None)
                
                if source_neuron and target_neuron:
                    edge_x.extend([source_neuron.x, target_neuron.x, None])
                    edge_y.extend([source_neuron.y, target_neuron.y, None])
                    edge_info.append(f"{conn.source} → {conn.target} (w={conn.weight:.2f}, t={conn.type})")
        
        if edge_x:
            fig.add_trace(go.Scatter(
                x=edge_x,
                y=edge_y,
                mode='lines',
                line=dict(color='gray', width=1),
                hoverinfo='text',
                text=edge_info,
                showlegend=False
            ))
        
        # Update layout
        fig.update_layout(
            title='Interactive NLM Brain Connectivity Graph',
            showlegend=False,
            hovermode='closest',
            margin=dict(b=0, l=0, r=0, t=0),
            xaxis=dict(showgrid=True, zeroline=False, showticklabels=False),
            yaxis=dict(showgrid=True, zeroline=False, showticklabels=False),
            height=800
        )
        
        # Save to HTML
        fig.write_html(output_path)
        
        return output_path
    
    def create_interactive_3d(self, output_path: Optional[str] = None,
                             host: str = 'localhost', port: int = 8080) -> str:
        """Create interactive 3D visualization.
        
        Args:
            output_path: Output file path (auto-generated if None)
            host: Host for web server
            port: Port for web server
            
        Returns:
            Path to saved visualization or web server URL
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"interactive_3d_{timestamp}.html"
        
        if not PLOTLY_AVAILABLE:
            raise ImportError("Plotly is required for interactive 3D visualization")
        
        # Create 3D scatter plot
        fig = go.Figure(data=[go.Scatter3d(
            x=[neuron.x for neuron in self.neurons],
            y=[neuron.y for neuron in self.neurons],
            z=[neuron.z for neuron in self.neurons],
            mode='markers',
            marker=dict(
                size=[max(5, neuron.activation * 30) for neuron in self.neurons],
                color=[
                    self.color_map.get(n.type, 'gray') if hasattr(n, 'type') else 'gray'
                    for n in self.neurons
                ],
                opacity=0.8,
                cmin=0, cmax=1
            ),
            text=[f"Neuron: {neuron.id}<br>Layer: {neuron.layer}<br>Activation: {neuron.activation:.3f}" 
                  for neuron in self.neurons],
            hovertemplate='<b>%{text}</b><extra></extra>'
        )])
        
        # Add connections as lines
        for conn in self.connections:
            if conn.active:
                source_neuron = next((n for n in self.neurons if n.id == conn.source), None)
                target_neuron = next((n for n in self.neurons if n.id == conn.target), None)
                
                if source_neuron and target_neuron:
                    fig.add_trace(go.Scatter3d(
                        x=[source_neuron.x, target_neuron.x],
                        y=[source_neuron.y, target_neuron.y],
                        z=[source_neuron.z, target_neuron.z],
                        mode='lines',
                        line=dict(color='gray', width=conn.weight * 2),
                        hoverinfo='skip',
                        showlegend=False
                    ))
        
        fig.update_layout(
            title='Interactive 3D NLM Brain Visualization',
            scene=dict(
                xaxis_title='X Position',
                yaxis_title='Y Position',
                zaxis_title='Z Position',
                camera=dict(up=dict(x=0, y=0, z=1), eye=dict(x=1.5, y=1.5, z=1.5))
            ),
            margin=dict(l=0, r=0, t=0, b=0),
            height=800
        )
        
        # Save to HTML
        fig.write_html(output_path)
        
        return output_path
    
    def create_activity_heatmap(self, activity_data: Dict[str, Any],
                               output_path: Optional[str] = None,
                               format: str = "png") -> str:
        """Create activity heatmap visualization.
        
        Args:
            activity_data: Dictionary with activity data (neurons, time, etc.)
            output_path: Output file path (auto-generated if None)
            format: Output format (png, svg, pdf)
            
        Returns:
            Path to saved visualization
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"activity_heatmap_{timestamp}.{format}"
        
        # Extract data
        neurons = activity_data.get('neurons', {})
        time_steps = activity_data.get('time', [])
        
        # Create activation matrix (neurons x time)
        if neurons and time_steps:
            neuron_ids = list(neurons.keys())
            time_steps = list(range(len(time_steps)))
            
            # Create matrix
            activation_matrix = np.zeros((len(neuron_ids), len(time_steps)))
            
            for t, time_val in enumerate(time_steps):
                for i, neuron_id in enumerate(neuron_ids):
                    if neuron_id in neurons:
                        # Get activity for this neuron at this time step
                        neuron_activity = neurons[neuron_id]
                        if isinstance(neuron_activity, list) and t < len(neuron_activity):
                            activation_matrix[i, t] = neuron_activity[t]
                        elif isinstance(neuron_activity, (int, float)):
                            activation_matrix[i, t] = neuron_activity
            
            # Create heatmaps
            fig, axes = plt.subplots(2, 2, figsize=(16, 12))
            fig.suptitle('NLM Activity Heatmap Analysis', fontsize=20, fontweight='bold')
            
            # Plot 1: Full activation heatmap
            ax = axes[0, 0]
            im1 = ax.imshow(activation_matrix.T, aspect='auto', cmap='RdYlBu_r', 
                           interpolation='nearest')
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Neuron ID')
            ax.set_title('Activation Heatmap (Neurons × Time)', fontsize=14)
            plt.colorbar(im1, ax=ax, label='Activation Level')
            
            # Set y-axis labels
            ax.set_yticks(np.linspace(0, len(neuron_ids)-1, min(10, len(neuron_ids))))
            ax.set_yticklabels([neuron_ids[int(i)] if int(i) < len(neuron_ids) else '' 
                              for i in ax.get_yticks()])
            
            # Plot 2: Average activity over time
            ax = axes[0, 1]
            avg_activity = np.mean(activation_matrix, axis=0)
            ax.plot(time_steps, avg_activity, color='red', linewidth=2, label='Average Activity')
            ax.fill_between(time_steps, 0, avg_activity, alpha=0.3, color='red')
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Average Activation')
            ax.set_title('Global Activity Trend', fontsize=14)
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            # Plot 3: Neuron activity ranking
            ax = axes[1, 0]
            neuron_activity_sums = np.sum(activation_matrix, axis=1)
            sorted_indices = np.argsort(neuron_activity_sums)[::-1]
            sorted_sums = neuron_activity_sums[sorted_indices]
            sorted_neuron_ids = [neuron_ids[i] for i in sorted_indices]
            
            # Show top 20 neurons
            n_top = min(20, len(neuron_ids))
            bars = ax.barh(range(n_top), sorted_sums[:n_top], color='steelblue', alpha=0.7)
            ax.set_xlabel('Total Activity')
            ax.set_ylabel('Neuron (sorted by activity)')
            ax.set_title('Top Active Neurons', fontsize=14)
            ax.set_yticks(range(n_top))
            ax.set_yticklabels([f"{sorted_neuron_ids[i][:10]}..." for i in range(n_top)])
            
            # Add value labels
            for i, bar in enumerate(bars):
                width = bar.get_width()
                ax.text(width, bar.get_y() + bar.get_height()/2, 
                       f'{width:.2f}', ha='left', va='center')
            
            # Plot 4: Activity clustering
            ax = axes[1, 1]\n            if len(neuron_ids) > 5:
                # Perform clustering on neuron activity patterns
                from sklearn.cluster import KMeans
                from sklearn.preprocessing import StandardScaler
                
                # Standardize data
                X_scaled = StandardScaler().fit_transform(activation_matrix.T)
                
                # Apply KMeans clustering
                kmeans = KMeans(n_clusters=min(5, len(neuron_ids)), random_state=42)
                cluster_labels = kmeans.fit_predict(X_scaled)
                
                # Plot clusters
                scatter = ax.scatter(time_steps, np.arange(len(neuron_ids)), 
                                   c=cluster_labels, cmap='tab10', alpha=0.6)
                ax.set_xlabel('Time Step')
                ax.set_ylabel('Neuron ID')
                ax.set_title('Activity Clustering by Time', fontsize=14)
                plt.colorbar(scatter, ax=ax, label='Cluster')
            
            plt.tight_layout()
            
            # Save figure
            plt.savefig(output_path, format=format, dpi=300, bbox_inches='tight',
                       facecolor='white', edgecolor='none')
            plt.close()
            
            return output_path
        
        # Fallback if no activity data
        raise ValueError("No valid activity data provided for heatmap generation")
    
    def create_time_series(self, time_data: Dict[str, Any],
                          output_path: Optional[str] = None,
                          format: str = "png") -> str:
        """Create time series visualization.
        
        Args:
            time_data: Dictionary with time series data
            output_path: Output file path (auto-generated if None)
            format: Output format (png, svg, pdf)
            
        Returns:
            Path to saved visualization
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"time_series_{timestamp}.{format}"
        
        fig, axes = plt.subplots(4, 2, figsize=(20, 16))
        fig.suptitle('NLM Time Series Analysis', fontsize=20, fontweight='bold')
        
        time_points = time_data.get('time', [])
        if not time_points:
            raise ValueError("No time data provided for time series visualization")
        
        # Plot 1: Neuron activity time series
        ax = axes[0, 0]
        neurons = time_data.get('neurons', {})
        for neuron_id, activity in neurons.items():
            if isinstance(activity, list):
                ax.plot(time_points, activity, alpha=0.7, label=f"{neuron_id[:15]}...")
            else:
                # Single value
                ax.axhline(y=activity, alpha=0.7, label=f"{neuron_id[:15]}...={activity:.3f}")
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Activity Level')
        ax.set_title('Individual Neuron Activity', fontsize=14)
        ax.legend(loc='upper right', fontsize='small')
        ax.grid(True, alpha=0.3)
        
        # Plot 2: Global activity with moving average
        ax = axes[0, 1]
        global_activity = time_data.get('global_activity', [])
        if global_activity:
            ax.plot(time_points, global_activity, color='red', linewidth=2, label='Global Activity')
            # Add moving average
            if len(global_activity) > 10:
                window = min(10, len(global_activity) // 4)
                moving_avg = pd.Series(global_activity).rolling(window=window, center=True).mean()
                ax.plot(time_points, moving_avg, color='blue', linewidth=2, 
                       label=f'Moving Average (window={window})', alpha=0.8)
            
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Global Activity')
            ax.set_title('Global Activity with Moving Average', fontsize=14)
            ax.legend()
            ax.grid(True, alpha=0.3)
        
        # Plot 3: Neuromodulation dynamics
        ax = axes[1, 0]
        neuromodulation = time_data.get('neuromodulation', {})
        for modulator, values in neuromodulation.items():
            if isinstance(values, list):
                ax.plot(time_points, values, alpha=0.8, label=modulator)
            elif isinstance(values, (int, float)):
                ax.axhline(y=values, alpha=0.8, label=f"{modulator}={values:.3f}")
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Modulation Strength')
        ax.set_title('Neuromodulation Dynamics', fontsize=14)
        ax.legend(loc='upper right', fontsize='small')
        ax.grid(True, alpha=0.3)
        
        # Plot 4: Memory traces
        ax = axes[1, 1]
        memory = time_data.get('memory', {})
        for memory_type, trace in memory.items():
            if isinstance(trace, list):
                ax.plot(time_points, trace, alpha=0.8, label=memory_type)
            elif isinstance(trace, (int, float)):
                ax.axhline(y=trace, alpha=0.8, label=f"{memory_type}={trace:.3f}")
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Memory Strength')
        ax.set_title('Memory Dynamics', fontsize=14)
        ax.legend(loc='upper right', fontsize='small')
        ax.grid(True, alpha=0.3)
        
        # Plot 5: Synaptic weight evolution
        ax = axes[2, 0]
        synaptic_weights = time_data.get('synaptic_weights', [])
        if synaptic_weights:
            # Convert to numpy array for better handling
            weights_array = np.array(synaptic_weights)
            
            # Plot first 50 connections
            n_plot = min(50, weights_array.shape[1])
            for i in range(min(10, n_plot)):  # Show first 10 connections
                if weights_array.shape[0] > 0:
                    ax.plot(range(len(weights_array)), weights_array[:, i], 
                           alpha=0.5, linewidth=1)
            
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Synaptic Weight')
            ax.set_title('Synaptic Weight Evolution (Sample Connections)', fontsize=14)
            ax.grid(True, alpha=0.3)
        
        # Plot 6: Firing rate distribution
        ax = axes[2, 1]
        firing_rates = time_data.get('firing_rates', {})
        if firing_rates:
            for neuron_id, rates in firing_rates.items():
                if isinstance(rates, list):
                    ax.hist(rates, alpha=0.5, bins=30, label=f"{neuron_id[:15]}...")
                else:
                    # Single value
                    ax.axvline(x=rates, alpha=0.8, label=f"{neuron_id[:15]}...={rates:.2f}")
            
            ax.set_xlabel('Firing Rate (Hz)')
            ax.set_ylabel('Count')
            ax.set_title('Firing Rate Distribution', fontsize=14)
            ax.legend(loc='upper right', fontsize='small')
            ax.grid(True, alpha=0.3)
        
        # Plot 7: Activity correlation heatmap
        ax = axes[3, 0]
        if neurons and len(time_points) > 1:
            # Create correlation matrix between neurons
            neuron_data = []
            for neuron_id, activity in neurons.items():
                if isinstance(activity, list) and len(activity) == len(time_points):
                    neuron_data.append(activity)
            
            if len(neuron_data) > 1:
                corr_matrix = np.corrcoef(neuron_data)
                
                # Plot correlation heatmap for top 10 neurons
                n_show = min(10, len(neuron_ids) if 'neuron_ids' in locals() else len(neuron_data))
                im = ax.imshow(corr_matrix[:n_show, :n_show], cmap='RdBu_r', 
                              vmin=-1, vmax=1, interpolation='nearest')
                ax.set_xticks(range(n_show))
                ax.set_yticks(range(n_show))
                ax.set_xlabel('Neuron ID')
                ax.set_ylabel('Neuron ID')
                ax.set_title('Activity Correlation (Top Neurons)', fontsize=14)
                plt.colorbar(im, ax=ax, label='Correlation')
                
                # Add neuron labels
                neuron_labels = [f"{neuron_id[:8]}..." for neuron_id in neuron_ids[:n_show]]
                ax.set_xticklabels(neuron_labels, rotation=45)
                ax.set_yticklabels(neuron_labels)
        
        # Plot 8: Simulation statistics
        ax = axes[3, 1]
        stats = time_data.get('statistics', {})
        if stats:
            metrics = []
            values = []
            
            for metric, value in stats.items():
                if isinstance(value, (int, float)):
                    metrics.append(metric)
                    values.append(value)
            
            if metrics:
                bars = ax.barh(range(len(metrics)), values, color='skyblue', alpha=0.7)
                ax.set_yticks(range(len(metrics)))
                ax.set_yticklabels(metrics)
                ax.set_xlabel('Value')
                ax.set_title('Simulation Statistics', fontsize=14)
                ax.grid(True, alpha=0.3)
                
                # Add value labels
                for i, bar in enumerate(bars):
                    width = bar.get_width()
                    ax.text(width, bar.get_y() + bar.get_height()/2, 
                           f'{width:.3f}', ha='left', va='center')
        
        plt.tight_layout()
        
        # Save figure
        plt.savefig(output_path, format=format, dpi=300, bbox_inches='tight',
                   facecolor='white', edgecolor='none')
        plt.close()
        
        return output_path
    
    def create_simulation_animation(self, frame_data: List[Dict[str, Any]],
                                   output_path: Optional[str] = None,
                                   format: str = "mp4", fps: int = 10) -> str:
        """Create animation from simulation frames.
        
        Args:
            frame_data: List of frame data dictionaries
            output_path: Output file path (auto-generated if None)
            format: Output format (mp4, gif)
            fps: Frames per second
            
        Returns:
            Path to saved animation
        """
        if not frame_data:
            raise ValueError("No frame data provided for animation generation")
        
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"simulation_animation_{timestamp}.{format}"
        
        # Create figure for animation
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        fig.suptitle('NLM Simulation Animation', fontsize=20, fontweight='bold')
        
        # Initialize plot elements
        neuron_plots = []
        connection_plots = []
        activity_plots = []
        neuromod_plots = []
        
        # Setup subplots
        ax1 = axes[0, 0]  # Network view
        ax2 = axes[0, 1]  # Activity heatmap
        ax3 = axes[1, 0]  # Neuromodulation
        ax4 = axes[1, 1]  # Time series
        
        # Initialize network visualization
        network_plot, = ax1.plot([], [], 'o-', alpha=0.7)
        connection_plots.append(network_plot)
        ax1.set_xlim(0, 10)
        ax1.set_ylim(0, 10)
        ax1.set_title("Network Structure")
        ax1.grid(True, alpha=0.3)
        ax1.set_aspect('equal')
        
        # Initialize activity heatmap
        activity_im = ax2.imshow(np.zeros((5, 5)), cmap='hot', interpolation='nearest', 
                               vmin=0, vmax=1)
        activity_plots.append(activity_im)
        ax2.set_title("Activity Heatmap")
        
        # Initialize neuromodulation plot
        neuromod_plot, = ax3.plot([], [], 'o-', linewidth=2)
        neuromod_plots.append(neuromod_plot)
        ax3.set_title("Neuromodulation")
        ax3.set_ylim(0, 2)
        ax3.grid(True, alpha=0.3)
        
        # Initialize time series
        time_plot, = ax4.plot([], [], 'o-', alpha=0.7)
        activity_plots.append(time_plot)
        ax4.set_title("Activity Time Series")
        ax4.set_ylim(0, 1.5)
        ax4.grid(True, alpha=0.3)
        
        # Animation update function
        def update(frame):
            if frame >= len(frame_data):
                return connection_plots + activity_plots + neuromod_plots
            
            frame_info = frame_data[frame]
            
            # Update network structure
            neurons = frame_info.get('neurons', {})
            connections = frame_info.get('connections', {})
            
            if neurons:
                # Extract neuron positions and activations
                x_coords = [neuron.get('x', np.random.uniform(0, 10)) for neuron in neurons.values()]
                y_coords = [neuron.get('y', np.random.uniform(0, 10)) for neuron in neurons.values()]
                activations = [neuron.get('activation', 0.5) for neuron in neurons.values()]
                sizes = [max(10, act * 50) for act in activations]
                colors = plt.cm.viridis(np.array(activations))
                
                # Update network plot
                network_plot.set_data(x_coords, y_coords)
                network_plot.set_color(colors)
                network_plot.set_markersize(sizes)
            
            # Update activity heatmap
            if 'activity_map' in frame_info:
                activity_data = frame_info['activity_map']
                if isinstance(activity_data, np.ndarray):
                    activity_im.set_data(activity_data)
            
            # Update neuromodulation
            neuromod = frame_info.get('neuromodulation', {})
            if neuromod:
                mod_times = list(neuromod.keys())
                mod_values = list(neuromod.values())
                neuromod_plot.set_data(mod_times, mod_values)
            
            # Update time series
            time_series_data = frame_info.get('time_series', {})
            if time_series_data:
                for key, values in time_series_data.items():
                    if isinstance(values, list):
                        ax4.plot(len(values), values[-1], 'o', color='red', markersize=8)
            
            ax1.set_title(f"Network Structure - Frame {frame}")
            
            return connection_plots + activity_plots + neuromod_plots
        
        # Create animation
        anim = animation.FuncAnimation(
            fig, update, frames=len(frame_data), 
            interval=1000/fps, blit=True, repeat=False
        )
        
        # Save animation
        try:
            anim.save(output_path, writer='ffmpeg', fps=fps, 
                     extra_args=['-vcodec', 'libx264'] if format == 'mp4' else [])
        except Exception as e:
            print(f"Error saving animation: {e}")
            # Try saving as GIF
            if format == 'mp4':
                gif_path = output_path.replace('.mp4', '.gif')
                anim.save(gif_path, writer='pillow', fps=fps)
                return gif_path
            raise
        
        plt.close()
        return output_path
    
    def export_visualization(self, neurons: List[Dict[str, Any]],
                           connections: List[Dict[str, Any]],
                           activities: List[Dict[str, Any]],
                           output_format: str = "json") -> Dict[str, Any]:
        """Export visualization data in specified format.
        
        Args:
            neurons: List of neuron data
            connections: List of connection data
            activities: List of activity data
            output_format: Output format (json, csv, pickle)
            
        Returns:
            Dictionary with visualization data
        """
        export_data = {
            "metadata": {
                "export_time": datetime.now().isoformat(),
                "neuron_count": len(neurons),
                "connection_count": len(connections),
                "activity_count": len(activities)
            },
            "neurons": neurons,
            "connections": connections,
            "activities": activities
        }
        
        # Save to file
        if output_format.lower() == "json":
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"visualization_export_{timestamp}.json"
            with open(output_path, 'w') as f:
                json.dump(export_data, f, indent=2, default=str)
            
        elif output_format.lower() == "csv":
            import pandas as pd
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"visualization_export_{timestamp}.csv"
            
            # Export neurons
            df_neurons = pd.DataFrame(neurons)
            df_neurons.to_csv(output_path.replace('.csv', '_neurons.csv'), index=False)
            
            # Export connections
            df_connections = pd.DataFrame(connections)
            df_connections.to_csv(output_path.replace('.csv', '_connections.csv'), index=False)
            
            # Export activities
            df_activities = pd.DataFrame(activities)
            df_activities.to_csv(output_path.replace('.csv', '_activities.csv'), index=False)
            
            output_path = output_path.replace('.csv', '_combined.csv')
            
        elif output_format.lower() == "pickle":
            import pickle
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"visualization_export_{timestamp}.pkl"
            with open(output_path, 'wb') as f:
                pickle.dump(export_data, f)
        
        return export_data
    
    def parse_simulation_file(self, file_path: str) -> Dict[str, Any]:
        """Parse simulation data from file.
        
        Args:
            file_path: Path to simulation data file
            
        Returns:
            Parsed simulation data
        """
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
            
            # Parse and add data to visualization
            if 'neurons' in data:
                self.add_neurons(data['neurons'])
            
            if 'connections' in data:
                self.add_connections(data['connections'])
            
            if 'activities' in data:
                for activity in data['activities']:
                    self.add_activity(activity)
            
            return data
            
        except Exception as e:
            print(f"Error parsing simulation file: {e}")
            return {}
    
    def set_mode(self, mode: str):
        """Set visualization mode.
        
        Args:
            mode: Visualization mode (network, activity, connectivity, etc.)
        """
        try:
            self.current_mode = VisualizationMode(mode.lower())
        except ValueError:
            print(f"Warning: Unknown mode '{mode}', using default 'network'")
            self.current_mode = VisualizationMode.NETWORK
    
    def get_visualization_stats(self) -> Dict[str, Any]:
        """Get statistics about current visualization.
        
        Returns:
            Dictionary with visualization statistics
        """
        stats = {
            "neuron_count": len(self.neurons),
            "connection_count": len(self.connections),
            "activity_count": len(self.activities),
            "current_mode": self.current_mode.value,
            "backend": self.backend,
            "timestamp": datetime.now().isoformat()
        }
        
        # Add neuron statistics
        if self.neurons:
            activations = [neuron.activation for neuron in self.neurons]
            stats["neuron_stats"] = {
                "avg_activation": np.mean(activations),
                "max_activation": np.max(activations),
                "min_activation": np.min(activations),
                "std_activation": np.std(activations)
            }
            
            # Count neuron types
            neuron_types = {}
            for neuron in self.neurons:
                neuron_types[neuron.type] = neuron_types.get(neuron.type, 0) + 1
            stats["neuron_types"] = neuron_types
        
        # Add connection statistics
        if self.connections:
            weights = [conn.weight for conn in self.connections if conn.active]
            stats["connection_stats"] = {
                "active_connections": len([c for c in self.connections if c.active]),
                "inactive_connections": len([c for c in self.connections if not c.active]),
                "avg_weight": np.mean(weights) if weights else 0,
                "max_weight": np.max(weights) if weights else 0,
                "min_weight": np.min(weights) if weights else 0
            }
        
        return stats


def main():
    """Main entry point for BrainViz."""
    parser = argparse.ArgumentParser(description="NLM Brain Visualization Tool")
    parser.add_argument("--input", "-i", help="Input simulation data file (JSON)")
    parser.add_argument("--output", "-o", help="Output file path")
    parser.add_argument("--mode", "-m", choices=[m.value for m in VisualizationMode],
                       default="network", help="Visualization mode")
    parser.add_argument("--format", "-f", choices=["png", "svg", "pdf", "html", "mp4", "gif"],
                       default="png", help="Output format")
    parser.add_argument("--style", choices=["default", "classic", "ggplot", "seaborn"],
                       default="default", help="Plot style")
    parser.add_argument("--color", choices=[c.value for c in ColorScheme],
                       default="neural", help="Color scheme")
    parser.add_argument("--interactive", action="store_true", help="Enable interactive visualization")
    parser.add_argument("--realtime", action="store_true", help="Enable real-time visualization")
    parser.add_argument("--fps", type=int, default=10, help="Frames per second for animations")
    parser.add_argument("--components", nargs="+", default=["connectivity", "timeseries"],
                       help="Components to visualize")
    
    args = parser.parse_args()
    
    # Initialize BrainViz
    viz = BrainViz(
        style=args.style,
        color_scheme=args.color,
        interactive=args.interactive,
        real_time=args.realtime
    )
    
    # Parse input file if provided
    if args.input:
        data = viz.parse_simulation_file(args.input)
        if not data:
            print(f"Error: Could not parse input file {args.input}")
            sys.exit(1)
    else:
        # Generate sample data if no input provided
        print("No input file provided. Generating sample data...")
        data = viz._generate_sample_data()
    
    # Generate requested visualizations
    output_files = []
    
    for component in args.components:
        try:
            if component == "connectivity":
                output = viz.create_connectivity_graph(
                    data.get('neurons', []),
                    data.get('connections', []),
                    args.output,
                    args.format
                )
                output_files.append(output)
                
            elif component == "activity":
                output = viz.create_activity_heatmap(
                    data,
                    args.output,
                    args.format
                )
                output_files.append(output)
                
            elif component == "timeseries":
                output = viz.create_time_series(
                    data,
                    args.output,
                    args.format
                )
                output_files.append(output)
                
            elif component == "animation":
                frames = data.get('frames', [])
                if frames:
                    output = viz.create_simulation_animation(
                        frames,
                        args.output,
                        args.format,
                        args.fps
                    )
                    output_files.append(output)
                else:
                    print("Warning: No frame data found for animation")
                    
        except Exception as e:
            print(f"Error generating {component} visualization: {e}")
    
    # Print summary
    if output_files:
        print("=" * 60)
        print("BRAINVIZ SUMMARY")
        print("=" * 60)
        print(f"Input file: {args.input or 'Sample data'}")
        print(f"Output format: {args.format}")
        print(f"Visualization mode: {args.mode}")
        print(f"Components generated: {len(output_files)}")
        print()
        
        for i, output_file in enumerate(output_files, 1):
            print(f"  {i}. {output_file}")
        
        print("=" * 60)
    else:
        print("No visualizations generated successfully")
        sys.exit(1)


if __name__ == "__main__":
    main()


def _generate_sample_data(self) -> Dict[str, Any]:
    """Generate sample data for demonstration purposes."""
    # Generate sample neurons
    neurons = []
    for i in range(20):
        neuron = {
            "id": f"neuron_{i}",
            "x": np.random.uniform(0, 10),
            "y": np.random.uniform(0, 10),
            "z": np.random.uniform(-1, 1),
            "type": np.random.choice(["excitatory", "inhibitory", "interneuron", "regular"]),
            "layer": np.random.randint(0, 5),
            "activation": np.random.uniform(0, 1),
            "state": "active" if np.random.random() > 0.2 else "inactive"
        }
        neurons.append(neuron)
    
    # Generate sample connections
    connections = []
    for i in range(30):
        conn = {
            "source": f"neuron_{np.random.randint(0, 20)}",
            "target": f"neuron_{np.random.randint(0, 20)}",
            "weight": np.random.uniform(0.1, 1.0),
            "type": np.random.choice(["excitatory", "inhibitory", "modulatory"]),
            "strength": np.random.uniform(0, 1),
            "active": np.random.random() > 0.3
        }
        connections.append(conn)
    
    # Generate sample activities
    activities = []
    for t in range(10):
        activity = {
            "time_step": t,
            "neurons": {},
            "global_activity": np.random.uniform(0, 1),
            "neuromodulation": {
                "dopamine": np.random.uniform(0, 1),
                "curiosity": np.random.uniform(0, 1),
                "novelty": np.random.uniform(0, 1)
            },
            "memory_activity": {
                "working_memory": np.random.uniform(0, 1),
                "episodic_memory": np.random.uniform(0, 1)
            }
        }
        
        # Add individual neuron activities
        for neuron in neurons:
            activity["neurons"][neuron["id"]] = np.random.uniform(0, 1)
        
        activities.append(activity)
    
    return {
        "neurons": neurons,
        "connections": connections,
        "activities": activities,
        "frames": [{"neurons": neurons, "connections": connections, 
                   "activity_map": np.random.rand(5, 5),
                   "neuromodulation": {"dopamine": np.random.uniform(0, 1)},
                   "time_series": {"global": np.random.uniform(0, 1)}} 
                  for _ in range(5)]
    }