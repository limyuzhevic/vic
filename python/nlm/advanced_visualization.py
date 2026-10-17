#!/usr/bin/env python3
"""
Advanced Visualization Tools for NLM Neural Data

This module provides comprehensive advanced visualization tools for neural simulation data,
including interactive plots, network visualization, activation mapping, 3D visualization,
real-time streaming, and multi-panel dashboards.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation
import matplotlib.cm as cm
import plotly.graph_objects as go
import plotly.subplots as sp
from plotly.offline import download_plotlyjs, init_notebook_mode, iplot
import plotly.express as px
import networkx as nx
import pyvis as pv
from typing import List, Dict, Tuple, Optional, Any, Union, Callable
from dataclasses import dataclass, field
from enum import Enum
import warnings
import json
import time
from collections import defaultdict
import threading
import queue
from abc import ABC, abstractmethod

warnings.filterwarnings('ignore')

class VisualizationType(Enum):
    """Types of visualization"""
    INTERACTIVE = "interactive"
    NETWORK = "network"
    ACTIVATION_MAP = "activation_map"
    HEATMAP = "heatmap"
    CONTOUR = "contour"
    THREE_D = "3d"
    STREAMING = "streaming"
    DASHBOARD = "dashboard"

class PlotBackend(Enum):
    """Plotting backends"""
    MATPLOTLIB = "matplotlib"
    PLOTLY = "plotly"
    BOKEH = "bokeh"
    SEABORN = "seaborn"
    CYTOSCAPE = "cytoscape"

@dataclass
class VisualizationConfig:
    """Configuration for advanced visualization"""
    width: int = 1200
    height: int = 800
    dpi: int = 150
    style: str = "seaborn-v0_8"
    color_scheme: str = "viridis"
    font_size: int = 10
    title_fontsize: int = 16
    label_fontsize: int = 12
    theme: str = "light"
    interactive: bool = True
    backend: PlotBackend = PlotBackend.PLOTLY
    max_plotly_height: int = 600
    enable_animations: bool = True
    animation_interval: int = 100

@dataclass
class InteractivePlotData:
    """Data container for interactive plots"""
    x: Union[List[float], np.ndarray]
    y: Union[List[float], np.ndarray]
    z: Optional[Union[List[float], np.ndarray]] = None
    color: Optional[Union[str, List[str]]] = None
    size: Optional[Union[float, List[float]]] = None
    label: Optional[str] = None
    mode: str = "lines"
    marker_symbol: Optional[str] = None

@dataclass
class NetworkGraphData:
    """Data for network visualization"""
    nodes: List[Dict[str, Any]]
    edges: List[Dict[str, Any]]
    layout: str = "spring_layout"
    node_size: Optional[List[float]] = None
    node_color: Optional[List[str]] = None
    edge_width: Optional[List[float]] = None
    node_labels: Optional[List[str]] = None
    community_labels: Optional[List[int]] = None

@dataclass
class ActivationMapData:
    """Data for activation mapping"""
    activation_values: np.ndarray
    spatial_coords: np.ndarray
    time_points: Optional[np.ndarray] = None
    neuron_types: Optional[List[str]] = None
    activation_threshold: float = 0.5

@dataclass
class HeatmapData:
    """Data for heatmap visualization"""
    data_matrix: np.ndarray
    x_labels: Optional[List[str]] = None
    y_labels: Optional[List[str]] = None
    colormap: str = "viridis"
    show_values: bool = True
    annotation_size: int = 8

@dataclass
class ContourData:
    """Data for contour plot"""
    x: np.ndarray
    y: np.ndarray
    z: np.ndarray
    contours: Optional[int] = None
    levels: Optional[List[float]] = None
    filled: bool = True

@dataclass
class ThreeDData:
    """Data for 3D visualization"""
    x: np.ndarray
    y: np.ndarray
    z: np.ndarray
    values: Optional[np.ndarray] = None
    point_size: Optional[np.ndarray] = None
    color: Optional[np.ndarray] = None
    opacity: float = 1.0

@dataclass
class StreamingData:
    """Data for streaming visualization"""
    data_queue: queue.Queue
    window_size: int = 1000
    x_range: Tuple[float, float] = (0, 100)
    y_range: Tuple[float, float] = (0, 100)
    update_callback: Optional[Callable] = None

class AdvancedNeuralVisualizer:
    """Comprehensive advanced visualization for neural data"""
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
        self._setup_matplotlib_style()
        self._init_plotly_config()
        
        # Streaming visualization management
        self.streaming_plots: Dict[str, Any] = {}
        self.dashboard_panels: List[Dict[str, Any]] = []
    
    def _setup_matplotlib_style(self):
        """Setup matplotlib style for neural visualization"""
        if self.config.backend == PlotBackend.MATPLOTLIB:
            plt.style.use(self.config.style)
            plt.rcParams['figure.figsize'] = (self.config.width / 100, 
                                             self.config.height / 100)
            plt.rcParams['font.size'] = self.config.font_size
            plt.rcParams['axes.labelsize'] = self.config.label_fontsize
            plt.rcParams['axes.titlesize'] = self.config.title_fontsize
            plt.rcParams['xtick.labelsize'] = self.config.font_size
            plt.rcParams['ytick.labelsize'] = self.config.font_size
            plt.rcParams['legend.fontsize'] = self.config.font_size
    
    def _init_plotly_config(self):
        """Initialize plotly configuration"""
        if self.config.backend == PlotBackend.PLOTLY:
            try:
                import plotly.io as pio
                if self.config.theme == "dark":
                    pio.templates.default = "plotly_dark"
                else:
                    pio.templates.default = "plotly"
            except ImportError:
                print("Plotly not installed. Using matplotlib backend.")
                self.config.backend = PlotBackend.MATPLOTLIB

    def create_interactive_time_series(self, data: InteractivePlotData, 
                                     title: str = "Interactive Time Series",
                                     save_path: Optional[str] = None) -> Union[plt.Figure, Dict[str, Any]]:
        """Create interactive time series plot"""
        if self.config.backend == PlotBackend.PLOTLY:
            return self._create_plotly_interactive_time_series(data, title)
        else:
            return self._create_matplotlib_interactive_time_series(data, title, save_path)
    
    def _create_plotly_interactive_time_series(self, data: InteractivePlotData, 
                                             title: str) -> Dict[str, Any]:
        """Create interactive time series using plotly"""
        # Create figure
        fig = go.Figure()
        
        # Add trace
        if data.z is not None and data.z.shape == data.x.shape:
            # 3D line plot
            fig.add_trace(go.Scatter3d(
                x=data.x, y=data.y, z=data.z,
                mode='lines',
                name=data.label or 'Data',
                line=dict(color=data.color, width=2),
                hovertemplate='<br>'.join([
                    'X: %{x:.3f}',
                    'Y: %{y:.3f}',
                    'Z: %{z:.3f}',
                    '<extra></extra>'
                ])
            ))
        elif isinstance(data.color, list) and len(data.color) == len(data.x):
            # Colored lines based on color array
            for i, (x_val, y_val, color_val) in enumerate(zip(data.x, data.y, data.color)):
                fig.add_trace(go.Scatter(
                    x=[x_val], y=[y_val],
                    mode='markers',
                    marker=dict(color=color_val, size=data.size or 10),
                    name=f'Point {i}',
                    hovertemplate='<br>'.join([
                        f'X: {x_val:.3f}',
                        f'Y: {y_val:.3f}',
                        f'Color: {color_val}',
                        '<extra></extra>'
                    ])
                ))
        else:
            # Regular line plot
            fig.add_trace(go.Scatter(
                x=data.x, y=data.y,
                mode=data.mode,
                name=data.label or 'Data',
                line=dict(color=data.color, width=2),
                marker=dict(symbol=data.marker_symbol or 'circle', size=data.size or 6),
                hovertemplate='<br>'.join([
                    'X: %{x:.3f}',
                    'Y: %{y:.3f}',
                    '<extra></extra>'
                ])
            ))
        
        # Update layout
        fig.update_layout(
            title=title,
            xaxis_title='X',
            yaxis_title='Y',
            width=self.config.width,
            height=self.config.height,
            showlegend=True,
            hovermode='closest',
            template='plotly_dark' if self.config.theme == 'dark' else 'plotly'
        )
        
        if data.z is None:
            # Add range slider for interactive x-axis zoom
            fig.update_xaxes(rangeslider_visible=True)
        
        return fig
    
    def _create_matplotlib_interactive_time_series(self, data: InteractivePlotData,
                                                 title: str, save_path: Optional[str]) -> plt.Figure:
        """Create interactive time series using matplotlib"""
        fig, ax = plt.subplots(figsize=(self.config.width/100, self.config.height/100))
        
        if data.z is not None and data.z.shape == data.x.shape:
            # 3D plot
            from mpl_toolkits.mplot3d import Axes3D
            ax = plt.axes(projection='3d')
            ax.plot(data.x, data.y, data.z, color=data.color or 'blue', linewidth=2)
        else:
            # 2D plot
            ax.plot(data.x, data.y, color=data.color or 'blue', 
                   linewidth=2, label=data.label or 'Data')
            
            if isinstance(data.color, list) and len(data.color) == len(data.x):
                ax.scatter(data.x, data.y, c=data.color, s=data.size or 50, alpha=0.7)
            elif data.marker_symbol:
                ax.scatter(data.x, data.y, marker=data.marker_symbol, 
                          s=data.size or 50, alpha=0.7)
        
        ax.set_title(title)
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.grid(True, alpha=0.3)
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

    def create_network_visualization(self, graph_data: NetworkGraphData,
                                    title: str = "Neural Network Visualization",
                                    save_path: Optional[str] = None) -> Union[plt.Figure, Dict[str, Any]]:
        """Create interactive network visualization"""
        if self.config.backend == PlotBackend.PLOTLY:
            return self._create_plotly_network_visualization(graph_data, title)
        elif self.config.backend == PlotBackend.CYTOSCAPE:
            return self._create_cytoscape_network_visualization(graph_data, title, save_path)
        else:
            return self._create_matplotlib_network_visualization(graph_data, title, save_path)
    
    def _create_plotly_network_visualization(self, graph_data: NetworkGraphData, title: str) -> Dict[str, Any]:
        """Create network visualization using plotly"""
        # Create networkx graph
        G = nx.Graph()
        
        # Add nodes
        for i, node in enumerate(graph_data.nodes):
            G.add_node(i, **node)
            
            # Apply node properties
            if graph_data.node_labels and i < len(graph_data.node_labels):
                G.nodes[i]['label'] = graph_data.node_labels[i]
            
            if graph_data.node_color and i < len(graph_data.node_color):
                G.nodes[i]['color'] = graph_data.node_color[i]
            
            if graph_data.node_size and i < len(graph_data.node_size):
                G.nodes[i]['size'] = graph_data.node_size[i]
        
        # Add edges
        for edge in graph_data.edges:
            src = edge.get('source', edge.get('from', 0))
            dst = edge.get('target', edge.get('to', 1))
            weight = edge.get('weight', 1.0)
            G.add_edge(src, dst, weight=weight)
        
        # Determine layout
        if graph_data.layout == 'spring_layout':
            pos = nx.spring_layout(G, k=1, iterations=50, seed=42)
        elif graph_data.layout == 'kamada_kawai':
            pos = nx.kamada_kawai_layout(G)
        elif graph_data.layout == 'circular':
            pos = nx.circular_layout(G)
        elif graph_data.layout == 'erdos_renyi':
            pos = nx.erdos_renyi_graph(n=len(graph_data.nodes), p=0.3)
            pos = nx.spring_layout(pos, k=1, iterations=50, seed=42)
        else:
            pos = nx.spring_layout(G, k=1, iterations=50, seed=42)
        
        # Prepare plotly data
        node_x = []
        node_y = []
        node_text = []
        node_color = []
        node_size = []
        
        for node in G.nodes():
            x, y = pos[node]
            node_x.append(x)
            node_y.append(y)
            
            # Create hover text
            node_info = G.nodes[node]
            text = f"Node {node}<br>"
            for key, value in node_info.items():
                if key != 'label':
                    text += f"{key}: {value}<br>"
            node_text.append(text)
            
            # Determine node color
            if 'color' in node_info:
                node_color.append(node_info['color'])
            else:
                node_color.append('lightblue')
            
            # Determine node size
            if 'size' in node_info:
                node_size.append(node_info['size'])
            else:
                node_size.append(10)
        
        edge_x = []
        edge_y = []
        edge_width = []
        edge_color = []
        
        for edge in G.edges():
            x0, y0 = pos[edge[0]]
            x1, y1 = pos[edge[1]]
            
            edge_x.extend([x0, x1, None])
            edge_y.extend([y0, y1, None])
            
            # Edge properties
            weight = G.edges[edge].get('weight', 1.0)
            edge_width.append(weight * 3)
            edge_color.append('rgba(128, 128, 128, 0.3)')
        
        # Create plotly figure
        fig = go.Figure()
        
        # Add edges
        fig.add_trace(go.Scatter(
            x=edge_x, y=edge_y,
            line=dict(width=edge_width, color=edge_color),
            hoverinfo='none',
            mode='lines',
            marker=dict(color='rgba(128, 128, 128, 0.3)')
        ))
        
        # Add nodes
        fig.add_trace(go.Scatter(
            x=node_x, y=node_y,
            mode='markers',
            hoverinfo='text',
            text=node_text,
            marker=dict(
                color=node_color,
                size=node_size,
                line=dict(color='black', width=1)
            )
        ))
        
        # Add community coloring if available
        if graph_data.community_labels:
            unique_communities = list(set(graph_data.community_labels))
            community_colors = px.colors.qualitative.Plotly
            
            for i, community in enumerate(graph_data.community_labels):
                if community < len(community_colors):
                    # Update node color for this community
                    if i < len(node_color):
                        node_color[i] = community_colors[community]
            
            fig.update_traces(marker=dict(color=node_color))
        
        # Update layout
        fig.update_layout(
            title=title,
            showlegend=False,
            hovermode='closest',
            margin=dict(b=0, l=0, r=0, t=0),
            xaxis=dict(showgrid=False, zeroline=False, showticklabels=False),
            yaxis=dict(showgrid=False, zeroline=False, showticklabels=False),
            width=self.config.width,
            height=self.config.height
        )
        
        return fig
    
    def _create_matplotlib_network_visualization(self, graph_data: NetworkGraphData,
                                               title: str, save_path: Optional[str]) -> plt.Figure:
        """Create network visualization using matplotlib"""
        fig, ax = plt.subplots(figsize=(self.config.width/100, self.config.height/100))
        
        # Create networkx graph
        G = nx.Graph()
        
        # Add nodes with attributes
        for i, node in enumerate(graph_data.nodes):
            G.add_node(i, **node)
        
        # Add edges
        for edge in graph_data.edges:
            src = edge.get('source', edge.get('from', 0))
            dst = edge.get('target', edge.get('to', 1))
            weight = edge.get('weight', 1.0)
            G.add_edge(src, dst, weight=weight)
        
        # Determine layout
        if graph_data.layout == 'spring_layout':
            pos = nx.spring_layout(G, k=1, iterations=50, seed=42)
        elif graph_data.layout == 'kamada_kawai':
            pos = nx.kamada_kawai_layout(G)
        elif graph_data.layout == 'circular':
            pos = nx.circular_layout(G)
        elif graph_data.layout == 'erdos_renyi':
            pos = nx.erdos_renyi_graph(n=len(graph_data.nodes), p=0.3)
            pos = nx.spring_layout(pos, k=1, iterations=50, seed=42)
        else:
            pos = nx.spring_layout(G, k=1, iterations=50, seed=42)
        
        # Draw edges
        edge_widths = []
        for edge in graph_data.edges:
            weight = edge.get('weight', 1.0)
            edge_widths.append(weight)
        
        if edge_widths:
            max_weight = max(edge_widths)
            edge_widths = [w / max_weight * 5 for w in edge_widths]  # Normalize
        
        nx.draw_networkx_edges(G, pos, ax=ax, alpha=0.6, width=edge_widths,
                              edge_color='gray', style='solid')
        
        # Draw nodes
        node_colors = []
        node_sizes = []
        
        for i in graph_data.nodes:
            if graph_data.node_color and i < len(graph_data.node_color):
                node_colors.append(graph_data.node_color[i])
            else:
                node_colors.append('lightblue')
            
            if graph_data.node_size and i < len(graph_data.node_size):
                node_sizes.append(graph_data.node_size[i])
            else:
                node_sizes.append(100)
        
        nx.draw_networkx_nodes(G, pos, ax=ax, node_color=node_colors,
                              node_size=node_sizes, alpha=0.8, edgecolors='black')
        
        # Draw labels
        if graph_data.node_labels:
            nx.draw_networkx_labels(G, pos, graph_data.node_labels, 
                                   font_size=self.config.font_size - 2)
        
        ax.set_title(title)
        ax.axis('off')
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

    def _create_cytoscape_network_visualization(self, graph_data: NetworkGraphData,
                                              title: str, save_path: Optional[str]) -> Dict[str, Any]:
        """Create network visualization using cytoscape"""
        try:
            import pyvis as pv
            
            # Create network graph
            G = nx.Graph()
            
            # Add nodes
            for i, node in enumerate(graph_data.nodes):
                node_data = {
                    'id': f'node_{i}',
                    'label': node.get('label', f'Node {i}'),
                    'color': node.get('color', 'lightblue'),
                    'size': node.get('size', 10)
                }
                G.add_node(i, **node_data)
            
            # Add edges
            for edge in graph_data.edges:
                src = edge.get('source', edge.get('from', 0))
                dst = edge.get('target', edge.get('to', 1))
                G.add_edge(src, dst, weight=edge.get('weight', 1.0))
            
            # Create pyvis network
            net = pv.Network(width='100%', height='600px')
            net.from_nx(G)
            
            # Set options
            net.set_options("""
            {
                "nodes": {
                    "shape": "dot",
                    "color": {
                        "background": "#222222",
                        "highlight": {
                            "border": "#2a5aa6",
                            "background": "#00bcd4"
                        }
                    },
                    "font": {"color": "#ffffff"}
                },
                "edges": {
                    "color": {"color": "#ffffff", "highlight": "#00bcd4"},
                    "arrows": {"to": {"enabled": true}}
                },
                "physics": {
                    "hierarchyLimit": 1000,
                    "barnesHut": {"theta": 0.5}
                }
            }
            """)
            
            # Save to HTML
            if save_path:
                html_file = save_path.replace('.png', '.html').replace('.jpg', '.html')
                net.save_graph(html_file)
            
            # Return the PyVis network object
            return {
                'network': net,
                'graph': G,
                'html_file': save_path.replace('.png', '.html').replace('.jpg', '.html') if save_path else None
            }
            
        except ImportError:
            print("PyVis not installed. Install with: pip install pyvis")
            return {'error': 'PyVis not available'}

    def create_activation_map(self, activation_data: ActivationMapData,
                             title: str = "Neural Activation Map",
                             save_path: Optional[str] = None) -> Union[plt.Figure, Dict[str, Any]]:
        """Create neural activation map"""
        if self.config.backend == PlotBackend.PLOTLY:
            return self._create_plotly_activation_map(activation_data, title)
        else:
            return self._create_matplotlib_activation_map(activation_data, title, save_path)
    
    def _create_plotly_activation_map(self, activation_data: ActivationMapData, title: str) -> Dict[str, Any]:
        """Create activation map using plotly"""
        # Create 3D scatter plot for activation data
        fig = go.Figure()
        
        # Extract activation values and coordinates
        values = activation_data.activation_values
        coords = activation_data.spatial_coords
        
        # Create color scale based on activation values
        if len(values) > 0:
            colors = np.interp(values, (values.min(), values.max()), (0, 1))
            color_scale = ['blue', 'cyan', 'green', 'yellow', 'red']
            color_map = px.colors.sequential.Viridis
        else:
            colors = np.zeros(len(coords))
            color_map = 'Viridis'
        
        # Create hover text
        hover_text = []
        for i, (coord, val) in enumerate(zip(coords, values)):
            if activation_data.neuron_types and i < len(activation_data.neuron_types):
                neuron_type = activation_data.neuron_types[i]
            else:
                neuron_type = 'Unknown'
            
            hover_text.append(
                f'Neuron {i}<br>'
                f'Activation: {val:.4f}<br>'
                f'Coordinates: {coord}<br>'
                f'Neuron Type: {neuron_type}<br>'
                f'Above Threshold: {val > activation_data.activation_threshold}'
            )
        
        # Create scatter plot
        if coords.shape[1] == 2:
            # 2D activation map
            fig.add_trace(go.Scatter(
                x=coords[:, 0],
                y=coords[:, 1],
                mode='markers',
                marker=dict(
                    size=15,
                    color=colors,
                    colorscale=color_map,
                    colorbar=dict(title='Activation Level'),
                    opacity=0.8,
                    showscale=True
                ),
                text=hover_text,
                hovertemplate='<br>'.join([
                    '%{text}',
                    '<extra></extra>'
                ])
            ))
        
        elif coords.shape[1] == 3:
            # 3D activation map
            fig.add_trace(go.Scatter3d(
                x=coords[:, 0],
                y=coords[:, 1],
                z=coords[:, 2],
                mode='markers',
                marker=dict(
                    size=8,
                    color=colors,
                    colorscale=color_map,
                    colorbar=dict(title='Activation Level'),
                    opacity=0.9,
                    showscale=True
                ),
                text=hover_text,
                hovertemplate='<br>'.join([
                    '%{text}',
                    '<extra></extra>'
                ])
            ))
        
        # Add threshold contour if in 2D
        if coords.shape[1] == 2 and activation_data.activation_threshold > 0:
            threshold_mask = values > activation_data.activation_threshold
            if np.any(threshold_mask):
                threshold_coords = coords[threshold_mask]
                threshold_values = values[threshold_mask]
                
                fig.add_trace(go.Scatter(
                    x=threshold_coords[:, 0],
                    y=threshold_coords[:, 1],
                    mode='markers',
                    marker=dict(
                        size=20,
                        color='red',
                        symbol='x',
                        opacity=0.6
                    ),
                    name='Above Threshold',
                    hovertemplate='<br>'.join([
                        'Neuron above activation threshold<br>',
                        '<extra></extra>'
                    ])
                ))
        
        # Update layout
        fig.update_layout(
            title=title,
            showlegend=True,
            hovermode='closest',
            template='plotly_dark' if self.config.theme == 'dark' else 'plotly',
            width=self.config.width,
            height=self.config.height
        )
        
        return fig
    
    def _create_matplotlib_activation_map(self, activation_data: ActivationMapData,
                                        title: str, save_path: Optional[str]) -> plt.Figure:
        """Create activation map using matplotlib"""
        fig = plt.figure(figsize=(self.config.width/100, self.config.height/100))
        
        if activation_data.spatial_coords.shape[1] == 2:
            # 2D activation map
            if activation_data.activation_threshold > 0:
                # Show only activated neurons
                mask = activation_data.activation_values > activation_data.activation_threshold
                plot_coords = activation_data.spatial_coords[mask]
                plot_values = activation_data.activation_values[mask]
                
                scatter = plt.scatter(plot_coords[:, 0], plot_coords[:, 1],
                                     c=plot_values, cmap=self.config.color_scheme,
                                     s=activation_data.activation_values * 100, alpha=0.7)
                
                plt.colorbar(scatter, label='Activation Level')
                
                # Mark threshold neurons
                if np.any(mask):
                    plt.scatter(plot_coords[:, 0], plot_coords[:, 1],
                               c='red', marker='x', s=200, alpha=0.6, label='Above Threshold')
            else:
                # Show all neurons
                scatter = plt.scatter(activation_data.spatial_coords[:, 0],
                                    activation_data.spatial_coords[:, 1],
                                    c=activation_data.activation_values,
                                    cmap=self.config.color_scheme,
                                    s=activation_data.activation_values * 100, alpha=0.7)
                plt.colorbar(scatter, label='Activation Level')
        
        elif activation_data.spatial_coords.shape[1] == 3:
            # 3D activation map
            ax = fig.add_subplot(111, projection='3d')
            
            scatter = ax.scatter(activation_data.spatial_coords[:, 0],
                                activation_data.spatial_coords[:, 1],
                                activation_data.spatial_coords[:, 2],
                                c=activation_data.activation_values,
                                cmap=self.config.color_scheme,
                                s=activation_data.activation_values * 50, alpha=0.7)
            
            ax.set_xlabel('X')
            ax.set_ylabel('Y')
            ax.set_zlabel('Z')
            plt.colorbar(scatter, label='Activation Level')
        
        plt.title(title)
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

    def create_heatmap(self, heatmap_data: HeatmapData,
                      title: str = "Neural Data Heatmap",
                      save_path: Optional[str] = None) -> Union[plt.Figure, Dict[str, Any]]:
        """Create heatmap visualization"""
        if self.config.backend == PlotBackend.PLOTLY:
            return self._create_plotly_heatmap(heatmap_data, title)
        else:
            return self._create_matplotlib_heatmap(heatmap_data, title, save_path)
    
    def _create_plotly_heatmap(self, heatmap_data: HeatmapData, title: str) -> Dict[str, Any]:
        """Create heatmap using plotly"""
        # Prepare data
        z = heatmap_data.data_matrix
        
        # Create heatmap trace
        fig = go.Figure(data=go.Heatmap(
            x=heatmap_data.x_labels or [f"Col {i}" for i in range(z.shape[1])],
            y=heatmap_data.y_labels or [f"Row {i}" for i in range(z.shape[0])],
            z=z,
            colorscale=getattr(px.colors.sequential, heatmap_data.colormap),
            showscale=True,
            colorbar=dict(title='Value'),
            hoverongaps=False
        ))
        
        # Add annotations if requested
        if heatmap_data.show_values:
            annotations = []
            for i in range(z.shape[0]):
                for j in range(z.shape[1]):
                    annotations.append({
                        'x': heatmap_data.x_labels[i] if heatmap_data.x_labels else j,
                        'y': heatmap_data.y_labels[i] if heatmap_data.y_labels else i,
                        'text': f"{z[i, j]:.2f}",
                        'font': {'size': heatmap_data.annotation_size},
                        'showarrow': False
                    })
            
            fig.update_layout(annotations=annotations)
        
        # Update layout
        fig.update_layout(
            title=title,
            xaxis=dict(title='Columns'),
            yaxis=dict(title='Rows', autorange='reversed'),
            width=self.config.width,
            height=self.config.height,
            template='plotly_dark' if self.config.theme == 'dark' else 'plotly'
        )
        
        return fig
    
    def _create_matplotlib_heatmap(self, heatmap_data: HeatmapData,
                                 title: str, save_path: Optional[str]) -> plt.Figure:
        """Create heatmap using matplotlib"""
        fig, ax = plt.subplots(figsize=(self.config.width/100, self.config.height/100))
        
        # Create heatmap
        im = ax.imshow(heatmap_data.data_matrix, 
                      cmap=getattr(plt.cm, heatmap_data.colormap),
                      aspect='auto', interpolation='nearest')
        
        # Add annotations
        if heatmap_data.show_values:
            for i in range(heatmap_data.data_matrix.shape[0]):
                for j in range(heatmap_data.data_matrix.shape[1]):
                    value = heatmap_data.data_matrix[i, j]
                    text = ax.text(j, i, f"{value:.2f}",
                                 ha="center", va="center",
                                 fontsize=heatmap_data.annotation_size,
                                 color="white" if value > np.max(heatmap_data.data_matrix)/2 else "black")
        
        # Set labels
        if heatmap_data.x_labels:
            ax.set_xticks(range(len(heatmap_data.x_labels)))
            ax.set_xticklabels(heatmap_data.x_labels, rotation=45, ha='right')
        else:
            ax.set_xticks(range(heatmap_data.data_matrix.shape[1]))
        
        if heatmap_data.y_labels:
            ax.set_yticks(range(len(heatmap_data.y_labels)))
            ax.set_yticklabels(heatmap_data.y_labels)
        else:
            ax.set_yticks(range(heatmap_data.data_matrix.shape[0]))
        
        ax.set_title(title)
        plt.colorbar(im, ax=ax)
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

    def create_contour_plot(self, contour_data: ContourData,
                           title: str = "Neural Activity Contour",
                           save_path: Optional[str] = None) -> Union[plt.Figure, Dict[str, Any]]:
        """Create contour plot"""
        if self.config.backend == PlotBackend.PLOTLY:
            return self._create_plotly_contour(contour_data, title)
        else:
            return self._create_matplotlib_contour(contour_data, title, save_path)
    
    def _create_plotly_contour(self, contour_data: ContourData, title: str) -> Dict[str, Any]:
        """Create contour plot using plotly"""
        # Create contour trace
        fig = go.Figure()
        
        if contour_data.contours is not None:
            # Create contour lines
            fig.add_trace(go.Contour(
                x=contour_data.x,
                y=contour_data.y,
                z=contour_data.z,
                contours=dict(size=contour_data.contours),
                showscale=True,
                colorbar=dict(title='Value'),
                line=dict(color='black', width=1),
                fills='none'
            ))
        else:
            # Create filled contours
            levels = contour_data.levels or 20
            if isinstance(levels, int):
                z_min, z_max = np.min(contour_data.z), np.max(contour_data.z)
                levels = np.linspace(z_min, z_max, levels)
            
            fig.add_trace(go.Contour(
                x=contour_data.x,
                y=contour_data.y,
                z=contour_data.z,
                contours=dict(showlabels=False),
                showscale=True,
                colorbar=dict(title='Value'),
                line=dict(color='black', width=0.5),
                fills='contour' if contour_data.filled else 'none',
                contours_colorbar=True,
                type='contour'
            ))
        
        # Update layout
        fig.update_layout(
            title=title,
            xaxis_title='X',
            yaxis_title='Y',
            width=self.config.width,
            height=self.config.height,
            template='plotly_dark' if self.config.theme == 'dark' else 'plotly'
        )
        
        return fig
    
    def _create_matplotlib_contour(self, contour_data: ContourData,
                                 title: str, save_path: Optional[str]) -> plt.Figure:
        """Create contour plot using matplotlib"""
        fig, ax = plt.subplots(figsize=(self.config.width/100, self.config.height/100))
        
        if contour_data.contours is not None:
            # Create contour lines
            contour = ax.contour(contour_data.x, contour_data.y, contour_data.z,
                               levels=contour_data.contours,
                               colors='black', linewidths=0.5)
            ax.clabel(contour, inline=True, fontsize=8)
        else:
            # Create filled contours
            levels = contour_data.levels or 20
            if isinstance(levels, int):
                z_min, z_max = np.min(contour_data.z), np.max(contour_data.z)
                levels = np.linspace(z_min, z_max, levels)
            
            contourf = ax.contourf(contour_data.x, contour_data.y, contour_data.z,
                                  levels=levels, cmap=self.config.color_scheme)
            plt.colorbar(contourf, ax=ax)
        
        ax.set_title(title)
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

    def create_3d_visualization(self, data: ThreeDData,
                               title: str = "3D Neural Visualization",
                               save_path: Optional[str] = None) -> Union[plt.Figure, Dict[str, Any]]:
        """Create 3D visualization"""
        if self.config.backend == PlotBackend.PLOTLY:
            return self._create_plotly_3d(data, title)
        else:
            return self._create_matplotlib_3d(data, title, save_path)
    
    def _create_plotly_3d(self, data: ThreeDData, title: str) -> Dict[str, Any]:
        """Create 3D plot using plotly"""
        fig = go.Figure()
        
        # Create scatter plot
        fig.add_trace(go.Scatter3d(
            x=data.x,
            y=data.y,
            z=data.z,
            mode='markers',
            marker=dict(
                size=data.point_size or 5,
                color=data.color or 'blue',
                opacity=data.opacity,
                colorbar=dict(title='Value') if data.color is not None else None
            ),
            text=[f"Point {i}<br>X: {x}<br>Y: {y}<br>Z: {z}<br>Value: {v}"
                  for i, (x, y, z, v) in enumerate(zip(data.x, data.y, data.z, 
                                                    data.values if data.values is not None else [0] * len(data.x)))],
            hovertemplate='<br>'.join([
                '%{text}',
                '<extra></extra>'
            ])
        ))
        
        # Update layout
        fig.update_layout(
            title=title,
            scene=dict(
                xaxis_title='X',
                yaxis_title='Y',
                zaxis_title='Z'
            ),
            width=self.config.width,
            height=self.config.height,
            template='plotly_dark' if self.config.theme == 'dark' else 'plotly'
        )
        
        return fig
    
    def _create_matplotlib_3d(self, data: ThreeDData, title: str, save_path: Optional[str]) -> plt.Figure:
        """Create 3D plot using matplotlib"""
        fig = plt.figure(figsize=(self.config.width/100, self.config.height/100))
        ax = fig.add_subplot(111, projection='3d')
        
        # Create scatter plot
        if data.color is not None:
            scatter = ax.scatter(data.x, data.y, data.z,
                               c=data.color, s=data.point_size or 25,
                               alpha=data.opacity, cmap=self.config.color_scheme)
            plt.colorbar(scatter, ax=ax, label='Value')
        else:
            scatter = ax.scatter(data.x, data.y, data.z,
                               s=data.point_size or 25, alpha=data.opacity)
        
        ax.set_title(title)
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

    def create_dashboard(self, panels: List[Dict[str, Any]],
                        title: str = "Neural Data Dashboard",
                        save_path: Optional[str] = None) -> Union[plt.Figure, Dict[str, Any]]:
        """Create multi-panel dashboard"""
        if self.config.backend == PlotBackend.PLOTLY:
            return self._create_plotly_dashboard(panels, title, save_path)
        else:
            return self._create_matplotlib_dashboard(panels, title, save_path)
    
    def _create_plotly_dashboard(self, panels: List[Dict[str, Any]],
                                title: str, save_path: Optional[str]) -> Dict[str, Any]:
        """Create dashboard using plotly"""
        # Determine grid layout
        n_panels = len(panels)
        cols = min(3, n_panels)
        rows = (n_panels + cols - 1) // cols
        
        # Create subplot figure
        fig = sp.make_subplots(
            rows=rows, cols=cols,
            subplot_titles=[panel.get('title', f'Panel {i+1}') for i, panel in enumerate(panels)],
            horizontal_spacing=0.1, vertical_spacing=0.1
        )
        
        # Add each panel
        for i, panel in enumerate(panels):
            row = i // cols
            col = i % cols
            
            panel_type = panel.get('type', 'plot')
            panel_data = panel.get('data')
            panel_config = panel.get('config', {})
            
            try:
                if panel_type == 'heatmap' and isinstance(panel_data, HeatmapData):
                    # Add heatmap to subplot
                    heatmap_fig = go.Figure()
                    heatmap_trace = go.Heatmap(
                        x=panel_data.x_labels or [f"Col {i}" for i in range(panel_data.data_matrix.shape[1])],
                        y=panel_data.y_labels or [f"Row {i}" for i in range(panel_data.data_matrix.shape[0])],
                        z=panel_data.data_matrix,
                        colorscale='Viridis'
                    )
                    heatmap_fig.add_trace(heatmap_trace)
                    heatmap_fig.update_layout(height=self.config.max_plotly_height)
                    
                    # Convert to plotly trace
                    for trace in heatmap_fig['data']:
                        fig.add_trace(trace, row=row+1, col=col+1)
                
                elif panel_type == 'scatter' and isinstance(panel_data, InteractivePlotData):
                    # Add scatter plot to subplot
                    scatter_fig = self.create_interactive_time_series(panel_data, 
                                                                   f"Dashboard Panel {i+1}")
                    if isinstance(scatter_fig, dict) and 'data' in scatter_fig:
                        for trace in scatter_fig['data']:
                            fig.add_trace(trace, row=row+1, col=col+1)
                
                elif panel_type == 'text':
                    # Add text panel
                    text_content = panel_data if isinstance(panel_data, str) else str(panel_data)
                    fig.add_annotation(
                        text=text_content,
                        xref="paper", yref="paper",
                        x=row / rows, y=(rows - row - 1) / rows,
                        showarrow=False,
                        fontsize=10,
                        align="center"
                    )
                
                else:
                    # Default empty panel
                    fig.add_annotation(
                        text=f"Panel {i+1}: {panel_type}",
                        xref="paper", yref="paper",
                        x=row / rows, y=(rows - row - 1) / rows,
                        showarrow=False,
                        fontsize=12,
                        align="center"
                    )
                    
            except Exception as e:
                # Error panel
                fig.add_annotation(
                    text=f"Error: {str(e)}",
                    xref="paper", yref="paper",
                    x=row / rows, y=(rows - row - 1) / rows,
                    showarrow=False,
                    fontsize=10,
                    align="center",
                    font=dict(color="red")
                )
        
        # Update layout
        fig.update_layout(
            title=title,
            showlegend=False,
            width=self.config.width,
            height=self.config.height * rows,
            template='plotly_dark' if self.config.theme == 'dark' else 'plotly'
        )
        
        return fig
    
    def _create_matplotlib_dashboard(self, panels: List[Dict[str, Any]],
                                    title: str, save_path: Optional[str]) -> plt.Figure:
        """Create dashboard using matplotlib"""
        # Determine grid layout
        n_panels = len(panels)
        cols = min(3, n_panels)
        rows = (n_panels + cols - 1) // cols
        
        # Create subplots
        fig, axes = plt.subplots(rows, cols, figsize=(self.config.width/100, 
                                                    self.config.height * rows / 100))
        
        if rows == 1 and cols == 1:
            axes = [axes]
        elif rows == 1 or cols == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        # Add each panel
        for i, (panel, ax) in enumerate(zip(panels, axes)):
            panel_type = panel.get('type', 'plot')
            panel_data = panel.get('data')
            
            try:
                if panel_type == 'heatmap' and isinstance(panel_data, HeatmapData):
                    # Plot heatmap
                    im = ax.imshow(panel_data.data_matrix, 
                                  cmap=self.config.color_scheme, aspect='auto')
                    if panel_data.show_values:
                        # Add text annotations
                        for j in range(panel_data.data_matrix.shape[0]):
                            for k in range(panel_data.data_matrix.shape[1]):
                                text = ax.text(k, j, f"{panel_data.data_matrix[j, k]:.2f}",
                                             ha="center", va="center",
                                             fontsize=panel_data.annotation_size,
                                             color="white" if panel_data.data_matrix[j, k] > np.max(panel_data.data_matrix)/2 else "black")
                    plt.colorbar(im, ax=ax)
                
                elif panel_type == 'scatter' and isinstance(panel_data, InteractivePlotData):
                    # Plot scatter
                    if panel_data.z is not None and panel_data.z.shape == panel_data.x.shape:
                        # 3D scatter
                        from mpl_toolkits.mplot3d import Axes3D
                        ax = fig.add_subplot(rows, cols, i+1, projection='3d')
                        ax.scatter(panel_data.x, panel_data.y, panel_data.z,
                                  c=panel_data.color, s=panel_data.size or 25)
                    else:
                        # 2D scatter
                        ax.scatter(panel_data.x, panel_data.y,
                                  c=panel_data.color, s=panel_data.size or 25,
                                  label=panel_data.label)
                        ax.set_xlabel('X')
                        ax.set_ylabel('Y')
                
                else:
                    # Default empty panel
                    ax.text(0.5, 0.5, f"Panel {i+1}: {panel_type}",
                           ha='center', va='center', transform=ax.transAxes,
                           fontsize=12)
                
                ax.set_title(panel.get('title', f'Panel {i+1}'))
                ax.grid(True, alpha=0.3)
                
            except Exception as e:
                # Error panel
                ax.text(0.5, 0.5, f"Error: {str(e)}",
                       ha='center', va='center', transform=ax.transAxes,
                       fontsize=10, color='red')
        
        # Hide unused subplots
        for i in range(n_panels, len(axes)):
            axes[i].axis('off')
        
        plt.suptitle(title)
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

    def start_streaming_visualization(self, stream_id: str, initial_data: Any) -> Dict[str, Any]:
        """Start streaming visualization"""
        if not self.config.enable_animations:
            return {'error': 'Streaming visualization is disabled'}
        
        # Create streaming queue
        stream_queue = queue.Queue()
        stream_queue.put(initial_data)
        
        # Create streaming data object
        streaming_data = StreamingData(
            data_queue=stream_queue,
            window_size=self.config.width,
            update_callback=None
        )
        
        # Store streaming plot
        self.streaming_plots[stream_id] = streaming_data
        
        return {
            'stream_id': stream_id,
            'status': 'started',
            'data_queue': stream_queue,
            'initial_data': initial_data
        }
    
    def update_streaming_data(self, stream_id: str, new_data: Any) -> bool:
        """Update streaming visualization with new data"""
        if stream_id not in self.streaming_plots:
            return False
        
        stream_data = self.streaming_plots[stream_id]
        stream_data.data_queue.put(new_data)
        
        # Notify update callback if set
        if stream_data.update_callback:
            try:
                stream_data.update_callback(stream_data, new_data)
            except Exception as e:
                print(f"Update callback error: {e}")
        
        return True
    
    def stop_streaming_visualization(self, stream_id: str) -> bool:
        """Stop streaming visualization"""
        if stream_id in self.streaming_plots:
            del self.streaming_plots[stream_id]
            return True
        return False
    
    def get_visualization_stats(self) -> Dict[str, Any]:
        """Get visualization statistics"""
        stats = {
            'streaming_plots': len(self.streaming_plots),
            'dashboard_panels': len(self.dashboard_panels),
            'config': {
                'backend': self.config.backend.value,
                'theme': self.config.theme,
                'width': self.config.width,
                'height': self.config.height,
                'interactive': self.config.interactive
            }
        }
        
        return stats

# Convenience functions

def create_visualizer(config: Optional[VisualizationConfig] = None) -> AdvancedNeuralVisualizer:
    """Convenience function to create visualizer"""
    return AdvancedNeuralVisualizer(config)

def create_interactive_plot(data: InteractivePlotData, title: str = "Interactive Plot"):
    """Convenience function to create interactive plot"""
    visualizer = AdvancedNeuralVisualizer()
    return visualizer.create_interactive_time_series(data, title)

def create_network_graph(graph_data: NetworkGraphData, title: str = "Network Graph"):
    """Convenience function to create network graph"""
    visualizer = AdvancedNeuralVisualizer()
    return visualizer.create_network_visualization(graph_data, title)

def create_dashboard(panels: List[Dict[str, Any]], title: str = "Dashboard"):
    """Convenience function to create dashboard"""
    visualizer = AdvancedNeuralVisualizer()
    return visualizer.create_dashboard(panels, title)

# Export all classes and functions
__all__ = [
    'VisualizationType', 'PlotBackend', 'VisualizationConfig',
    'InteractivePlotData', 'NetworkGraphData', 'ActivationMapData',
    'HeatmapData', 'ContourData', 'ThreeDData', 'StreamingData',
    'AdvancedNeuralVisualizer', 'create_visualizer', 'create_interactive_plot',
    'create_network_graph', 'create_dashboard'
]