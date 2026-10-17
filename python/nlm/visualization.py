#!/usr/bin/env python3
"""
Visualization Utilities for Neural Networks and Brain Data

This module provides comprehensive visualization tools for neural simulation data,
including 2D/3D plotting, network visualization, brain activity mapping, and
interactive visualization tools for neural research and development.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation
import seaborn as sns
import networkx as nx
from typing import List, Dict, Tuple, Optional, Any, Union
from dataclasses import dataclass
import warnings
import os

warnings.filterwarnings('ignore')

# Set matplotlib backend
plt.switch_backend('Agg')

@dataclass
class VisualizationConfig:
    """Configuration for visualization settings"""
    width: int = 12
    height: int = 8
    dpi: int = 150
    style: str = "seaborn-v0_8"
    color_scheme: str = "viridis"
    font_size: int = 10
    title_fontsize: int = 14
    label_fontsize: int = 12

@dataclass
class PlotData:
    """Container for plotting data"""
    x: Union[List[float], np.ndarray]
    y: Union[List[float], np.ndarray]
    label: Optional[str] = None
    color: Optional[str] = None
    marker: Optional[str] = None
    linewidth: Optional[float] = None
    alpha: Optional[float] = None

class NeuralDataVisualizer:
    """Advanced visualization tools for neural simulation data"""
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
        self._setup_matplotlib_style()
    
    def _setup_matplotlib_style(self):
        """Setup matplotlib style"""
        plt.style.use(self.config.style)
        plt.rcParams['figure.figsize'] = (self.config.width, self.config.height)
        plt.rcParams['font.size'] = self.config.font_size
        plt.rcParams['axes.labelsize'] = self.config.label_fontsize
        plt.rcParams['axes.titlesize'] = self.config.title_fontsize
        plt.rcParams['xtick.labelsize'] = self.config.font_size
        plt.rcParams['ytick.labelsize'] = self.config.font_size
    
    def plot_neuron_firing_rates(self, firing_rates: List[float], 
                                time_steps: Optional[List[float]] = None,
                                title: str = "Neuron Firing Rates",
                                save_path: Optional[str] = None) -> plt.Figure:
        """Plot neuron firing rates over time"""
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(self.config.width, self.config.height))
        
        if time_steps is None:
            time_steps = list(range(len(firing_rates)))
        
        # Firing rates plot
        ax1.plot(time_steps, firing_rates, 'b-', linewidth=1, alpha=0.8)
        ax1.set_xlabel("Time Step")
        ax1.set_ylabel("Firing Rate (Hz)")
        ax1.set_title(f"{title} - Firing Rates")
        ax1.grid(True, alpha=0.3)
        ax1.set_facecolor('#f8f9fa')
        
        # Statistical summary plot
        ax2.hist(firing_rates, bins=min(50, len(firing_rates)//2), alpha=0.7, color='skyblue', edgecolor='black')
        ax2.set_xlabel("Firing Rate (Hz)")
        ax2.set_ylabel("Frequency")
        ax2.set_title("Firing Rate Distribution")
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig
    
    def plot_brain_activity_map(self, brain_state: Any, 
                                save_path: Optional[str] = None) -> plt.Figure:
        """Visualize brain activity across regions"""
        fig, axes = plt.subplots(2, 2, figsize=(self.config.width, self.config.height))
        axes = axes.flatten()
        
        # Extract brain data
        region_names = []
        firing_rates = []
        activity_levels = []
        
        if hasattr(brain_state, 'regions'):
            for region in brain_state.regions:
                region_names.append(region.name)
                firing_rates.append(region.get_average_firing_rate())
                activity_levels.append(region.get_active_neuron_count() / region.get_total_neuron_count() if region.get_total_neuron_count() > 0 else 0)
        
        if not region_names:
            axes[0].text(0.5, 0.5, "No brain regions available", 
                        horizontalalignment='center', verticalalignment='center',
                        transform=axes[0].transAxes)
            axes[0].set_title("Brain Activity Map")
            return fig
        
        # Region firing rate bar chart
        axes[0].bar(range(len(region_names)), firing_rates, alpha=0.7, color='coral')
        axes[0].set_xlabel("Region Index")
        axes[0].set_ylabel("Average Firing Rate (Hz)")
        axes[0].set_title("Firing Rates by Region")
        axes[0].set_xticks(range(len(region_names)))
        axes[0].set_xticklabels(region_names, rotation=45, ha='right')
        axes[0].grid(True, alpha=0.3)
        
        # Activity level heatmap
        if activity_levels:
            axes[1].imshow([activity_levels], aspect='auto', cmap='RdYlGn', vmin=0, vmax=1)
            axes[1].set_xlabel("Region")
            axes[1].set_ylabel("Activity Level")
            axes[1].set_title("Activity Level Heatmap")
            axes[1].set_xticks(range(len(region_names)))
            axes[1].set_xticklabels(region_names, rotation=45, ha='right')
        
        # Correlation heatmap
        if len(firing_rates) > 1:
            corr_matrix = np.corrcoef(firing_rates)
            sns.heatmap(corr_matrix, annot=True, fmt='.2f', cmap='coolwarm', center=0,
                       ax=axes[2], vmin=-1, vmax=1)
            axes[2].set_title("Firing Rate Correlations")
        
        # Activity over time (simulated)
        time_points = np.linspace(0, 100, 100)
        activity_curve = []
        for t in time_points:
            # Simple oscillatory activity
            activity = np.mean(firing_rates) * (1 + 0.5 * np.sin(2 * np.pi * t / 50))
            activity_curve.append(activity)
        
        axes[3].plot(time_points, activity_curve, 'g-', linewidth=2)
        axes[3].set_xlabel("Time")
        axes[3].set_ylabel("Activity Level")
        axes[3].set_title("Activity Over Time")
        axes[3].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig
    
    def plot_network_graph(self, connections: List[Tuple[int, int]], 
                          node_labels: Optional[List[str]] = None,
                          node_types: Optional[List[str]] = None,
                          save_path: Optional[str] = None) -> plt.Figure:
        """Plot neural network connectivity graph"""
        fig, ax = plt.subplots(figsize=(self.config.width, self.config.height))
        
        # Create network graph
        G = nx.Graph()
        
        # Add nodes
        n_nodes = max(max(conn) for conn in connections) + 1 if connections else 0
        for i in range(n_nodes):
            node_type = node_types[i] if node_types and i < len(node_types) else "neuron"
            G.add_node(i, type=node_type)
        
        # Add edges
        for i, (src, dst) in enumerate(connections):
            weight = 1.0  # Default weight
            G.add_edge(src, dst, weight=weight)
        
        # Determine layout
        if len(connections) < 50:
            pos = nx.spring_layout(G, seed=42, k=1, iterations=50)
        else:
            pos = nx.kamada_kawai_layout(G)
        
        # Draw nodes by type
        neuron_nodes = [n for n in G.nodes() if G.nodes[n].get('type') == 'neuron']
        excitatory_nodes = [n for n in G.nodes() if G.nodes[n].get('type') == 'excitatory']
        inhibitory_nodes = [n for n in G.nodes() if G.nodes[n].get('type') == 'inhibitory']
        
        # Draw edges
        nx.draw_networkx_edges(G, pos, alpha=0.3, edge_color='gray', ax=ax)
        
        # Draw nodes with different colors
        if excitatory_nodes:
            nx.draw_networkx_nodes(G, pos, nodelist=excitatory_nodes, 
                                 node_color='red', node_size=100, ax=ax)
        if inhibitory_nodes:
            nx.draw_networkx_nodes(G, pos, nodelist=inhibitory_nodes, 
                                 node_color='blue', node_size=100, ax=ax)
        if neuron_nodes:
            nx.draw_networkx_nodes(G, pos, nodelist=neuron_nodes, 
                                 node_color='green', node_size=50, ax=ax)
        
        # Draw labels
        if node_labels:
            nx.draw_networkx_labels(G, pos, labels=dict(zip(range(len(node_labels)), node_labels)),
                                   font_size=8, ax=ax)
        
        ax.set_title("Neural Network Connectivity")
        ax.axis('off')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig
    
    def plot_simulation_animation(self, data_frames: List[np.ndarray],
                                 duration: float = 5.0,
                                 save_path: Optional[str] = None) -> FuncAnimation:
        """Create animated visualization of simulation data"""
        if not data_frames:
            return None
        
        fig, ax = plt.subplots(figsize=(self.config.width, self.config.height))
        
        # Initialize plot
        im = ax.imshow(data_frames[0], cmap=self.config.color_scheme, aspect='auto')
        ax.set_title("Neural Simulation Animation")
        ax.axis('off')
        
        def update(frame):
            im.set_data(data_frames[frame % len(data_frames)])
            ax.set_title(f"Frame {frame % len(data_frames)}")
            return [im]
        
        animation = FuncAnimation(fig, update, frames=len(data_frames), 
                                 interval=duration * 1000 / len(data_frames), 
                                 blit=True)
        
        if save_path:
            animation.save(save_path, writer='ffmpeg', fps=30)
        
        return animation
    
    def create_multi_panel_plot(self, data_panels: List[Dict[str, Any]],
                              layout: str = "grid",
                              save_path: Optional[str] = None) -> plt.Figure:
        """Create multi-panel visualization with multiple subplots"""
        n_panels = len(data_panels)
        
        if layout == "grid":
            # Determine grid size
            cols = int(np.ceil(np.sqrt(n_panels)))
            rows = int(np.ceil(n_panels / cols))
        elif layout == "horizontal":
            rows, cols = 1, n_panels
        elif layout == "vertical":
            rows, cols = n_panels, 1
        else:
            cols = int(np.ceil(np.sqrt(n_panels)))
            rows = int(np.ceil(n_panels / cols))
        
        fig, axes = plt.subplots(rows, cols, figsize=(self.config.width * cols, self.config.height * rows))
        
        if rows == 1 and cols == 1:
            axes = [axes]
        elif rows == 1 or cols == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        for i, panel in enumerate(data_panels):
            if i >= len(axes):
                break
            
            ax = axes[i]
            plot_type = panel.get('type', 'line')
            data = panel.get('data', {})
            title = panel.get('title', f"Panel {i+1}")
            
            if plot_type == 'line' and 'x' in data and 'y' in data:
                ax.plot(data['x'], data['y'], label=data.get('label'), 
                       color=data.get('color', 'blue'), linewidth=data.get('linewidth', 1))
                ax.set_xlabel(data.get('xlabel', 'X'))
                ax.set_ylabel(data.get('ylabel', 'Y'))
            
            elif plot_type == 'scatter' and 'x' in data and 'y' in data:
                ax.scatter(data['x'], data['y'], alpha=data.get('alpha', 0.6), 
                          color=data.get('color', 'blue'), s=data.get('size', 20))
                ax.set_xlabel(data.get('xlabel', 'X'))
                ax.set_ylabel(data.get('ylabel', 'Y'))
            
            elif plot_type == 'histogram' and 'data' in data:
                ax.hist(data['data'], bins=data.get('bins', 20), 
                       alpha=data.get('alpha', 0.7), color=data.get('color', 'skyblue'),
                       edgecolor=data.get('edgecolor', 'black'))
                ax.set_xlabel(data.get('xlabel', 'Value'))
                ax.set_ylabel(data.get('ylabel', 'Frequency'))
            
            elif plot_type == 'heatmap' and 'data' in data:
                im = ax.imshow(data['data'], cmap=data.get('cmap', self.config.color_scheme),
                              vmin=data.get('vmin'), vmax=data.get('vmax'))
                plt.colorbar(im, ax=ax)
            
            ax.set_title(title)
            ax.grid(True, alpha=0.3)
        
        # Hide unused subplots
        for i in range(n_panels, len(axes)):
            axes[i].set_visible(False)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig
    
    def plot_time_series_comparison(self, time_series_list: List[Tuple[List[float], str]],
                                   title: str = "Time Series Comparison",
                                   save_path: Optional[str] = None) -> plt.Figure:
        """Compare multiple time series"""
        fig, ax = plt.subplots(figsize=(self.config.width, self.config.height))
        
        for data, label in time_series_list:
            if data:
                time_steps = list(range(len(data)))
                ax.plot(time_steps, data, label=label, linewidth=2)
        
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Value")
        ax.set_title(title)
        ax.legend()
        ax.grid(True, alpha=0.3)
        ax.set_facecolor('#f8f9fa')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

class NeuralNetworkVisualizer:
    """Specialized visualization for neural networks"""
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
        self._setup_matplotlib_style()
    
    def _setup_matplotlib_style(self):
        """Setup matplotlib style"""
        plt.style.use(self.config.style)
    
    def plot_network_architecture(self, layers: List[int], 
                                 connections: Optional[List[Tuple[int, int]]] = None,
                                 save_path: Optional[str] = None) -> plt.Figure:
        """Plot neural network architecture"""
        fig, ax = plt.subplots(figsize=(self.config.width, self.config.height))
        
        # Create network diagram
        n_layers = len(layers)
        max_neurons = max(layers)
        
        # Calculate positions
        x_positions = np.linspace(1, n_layers, n_layers)
        
        # Draw layers
        for i, (layer_idx, n_neurons) in enumerate(zip(x_positions, layers)):
            # Layer center
            layer_center = i + 1
            
            # Draw neurons
            y_positions = np.linspace(1, max_neurons, n_neurons)
            for j, y_pos in enumerate(y_positions):
                neuron = patches.Circle((layer_center, y_pos), radius=0.3, 
                                       facecolor='lightblue', edgecolor='blue', linewidth=2)
                ax.add_patch(neuron)
                
                # Add neuron label
                ax.text(layer_center, y_pos, str(j+1), 
                       horizontalalignment='center', verticalalignment='center',
                       fontsize=self.config.font_size - 2)
            
            # Draw layer label
            ax.text(layer_center, max_neurons + 0.5, f"Layer {i+1}\n{n_neurons} neurons",
                   horizontalalignment='center', verticalalignment='center',
                   fontsize=self.config.font_size)
        
        # Draw connections if provided
        if connections:
            for src, dst in connections:
                # Find neuron positions
                src_layer = src // max_neurons
                src_neuron = src % max_neurons
                dst_layer = dst // max_neurons
                dst_neuron = dst % max_neurons
                
                x1 = src_layer + 1
                y1 = src_neuron + 0.7
                x2 = dst_layer + 1
                y2 = dst_neuron + 0.7
                
                ax.plot([x1, x2], [y1, y2], 'k-', linewidth=0.5, alpha=0.5)
        
        ax.set_xlim(0.5, n_layers + 1.5)
        ax.set_ylim(0.5, max_neurons + 1.5)
        ax.set_aspect('equal')
        ax.axis('off')
        ax.set_title("Neural Network Architecture")
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig
    
    def plot_training_history(self, history: Dict[str, List[float]],
                             metrics: Optional[List[str]] = None,
                             save_path: Optional[str] = None) -> plt.Figure:
        """Plot training history (loss, accuracy, etc.)"""
        if not history:
            return plt.figure()
        
        if metrics is None:
            metrics = list(history.keys())
        
        n_metrics = len(metrics)
        cols = int(np.ceil(np.sqrt(n_metrics)))
        rows = int(np.ceil(n_metrics / cols))
        
        fig, axes = plt.subplots(rows, cols, figsize=(self.config.width * cols, self.config.height * rows))
        
        if n_metrics == 1:
            axes = [axes]
        elif rows == 1 or cols == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        for i, metric in enumerate(metrics):
            if i >= len(axes):
                break
            
            ax = axes[i]
            if metric in history:
                data = history[metric]
                epochs = list(range(len(data)))
                ax.plot(epochs, data, 'b-', linewidth=2)
                ax.set_xlabel('Epoch')
                ax.set_ylabel(metric.replace('_', ' ').title())
                ax.set_title(f'{metric.replace("_", " ").title()} Over Time')
                ax.grid(True, alpha=0.3)
        
        # Hide unused subplots
        for i in range(n_metrics, len(axes)):
            axes[i].set_visible(False)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig
    
    def plot_confusion_matrix(self, confusion_matrix: np.ndarray,
                             class_names: Optional[List[str]] = None,
                             save_path: Optional[str] = None) -> plt.Figure:
        """Plot confusion matrix"""
        fig, ax = plt.subplots(figsize=(self.config.width, self.config.height))
        
        if class_names is None:
            class_names = [f"Class {i}" for i in range(confusion_matrix.shape[0])]
        
        # Normalize confusion matrix
        cm_normalized = confusion_matrix.astype('float') / confusion_matrix.sum(axis=1)[:, np.newaxis]
        
        # Create heatmap
        im = ax.imshow(cm_normalized, cmap='Blues', vmin=0, vmax=1)
        ax.set_xlabel('Predicted Label')
        ax.set_ylabel('True Label')
        ax.set_title('Confusion Matrix')
        
        # Add ticks
        ax.set_xticks(np.arange(len(class_names)))
        ax.set_yticks(np.arange(len(class_names)))
        ax.set_xticklabels(class_names)
        ax.set_yticklabels(class_names)
        
        # Add text annotations
        for i in range(len(class_names)):
            for j in range(len(class_names)):
                text = ax.text(j, i, f'{confusion_matrix[i, j]}\n({cm_normalized[i, j]:.2f})',
                              ha="center", va="center", color="black",
                              fontsize=self.config.font_size - 1)
        
        plt.colorbar(im, ax=ax)
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.config.dpi, bbox_inches='tight')
        
        return fig

# Convenience functions

def create_neural_visualizer(config: Optional[VisualizationConfig] = None) -> NeuralDataVisualizer:
    """Convenience function to create a neural data visualizer"""
    return NeuralDataVisualizer(config)

def create_network_visualizer(config: Optional[VisualizationConfig] = None) -> NeuralNetworkVisualizer:
    """Convenience function to create a neural network visualizer"""
    return NeuralNetworkVisualizer(config)
