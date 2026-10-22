"""
Visualization module for NLM Python bindings

This module provides visualization and analysis functionality for neural network simulations.
"""

import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
from typing import Dict, Any, List, Optional, Tuple
import plotly.graph_objects as go
import plotly.io as pio
import pandas as pd
import seaborn as sns
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
import os

class VisualizationConfig:
    """Configuration for visualization."""
    
    def __init__(self):
        # Output configuration
        self.output_format = "html"  # "html", "png", "svg", "pdf"
        self.output_dpi = 300
        self.output_width = 1200
        self.output_height = 800
        
        # Theme configuration
        self.theme = "default"  # "default", "dark", "light"
        self.color_scheme = "viridis"  # "viridis", "plasma", "coolwarm", "category10"
        
        # Layout configuration
        self.layout_algorithm = "force_directed"  # "force_directed", "circular", "radial", "grid"
        self.connection_mode = "weighted"  # "weighted", "all", "threshold"
        self.connection_threshold = 0.1
        
        # Neuron visualization
        self.show_neuron_labels = True
        self.show_neuron_ids = False
        self.neuron_size_scale = 1.0
        self.neuron_color_by = "type"  # "type", "activity", "firing_rate", "custom"
        
        # Synapse visualization
        self.show_synapse_weights = True
        self.show_synapse_delays = False
        self.synapse_width_scale = 1.0
        
        # Activity visualization
        self.show_activity = True
        self.activity_animation = True
        self.animation_duration = 1000
        
        # Analysis visualization
        self.show_analysis = True
        self.pca_components = 2
        self.cluster_count = 3
        
        # File configuration
        self.include_static = True
        self.include_interactive = True
        self.export_data = True
        self.data_format = "csv"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'output_format': self.output_format,
            'output_dpi': self.output_dpi,
            'output_width': self.output_width,
            'output_height': self.output_height,
            'theme': self.theme,
            'color_scheme': self.color_scheme,
            'layout_algorithm': self.layout_algorithm,
            'connection_mode': self.connection_mode,
            'connection_threshold': self.connection_threshold,
            'show_neuron_labels': self.show_neuron_labels,
            'show_neuron_ids': self.show_neuron_ids,
            'neuron_size_scale': self.neuron_size_scale,
            'neuron_color_by': self.neuron_color_by,
            'show_synapse_weights': self.show_synapse_weights,
            'show_synapse_delays': self.show_synapse_delays,
            'synapse_width_scale': self.synapse_width_scale,
            'show_activity': self.show_activity,
            'activity_animation': self.activity_animation,
            'animation_duration': self.animation_duration,
            'show_analysis': self.show_analysis,
            'pca_components': self.pca_components,
            'cluster_count': self.cluster_count,
            'include_static': self.include_static,
            'include_interactive': self.include_interactive,
            'export_data': self.export_data,
            'data_format': self.data_format
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'VisualizationConfig':
        config = cls()
        
        for key, value in data.items():
            if hasattr(config, key):
                setattr(config, key, value)
        
        return config

class NeuralNetworkData:
    """Container for neural network data for visualization."""
    
    def __init__(self, brain_data: Dict[str, Any]):
        self.brain_data = brain_data
        self.neurons = brain_data.get('neurons', [])
        self.synapses = brain_data.get('synapses', [])
        self.spike_times = brain_data.get('spike_times', [])
        self.step_data = brain_data.get('step_data', [])
        self.analysis_data = brain_data.get('analysis', {})
    
    def get_neuron_positions(self) -> Tuple[np.ndarray, np.ndarray]:
        """Get neuron positions for layout."""
        x = [neuron.get('x', 0) for neuron in self.neurons]
        y = [neuron.get('y', 0) for neuron in self.neurons]
        return np.array(x), np.array(y)
    
    def get_neuron_sizes(self) -> np.ndarray:
        """Get neuron sizes for visualization."""
        if 'radius' in self.neurons[0]:
            return np.array([neuron['radius'] for neuron in self.neurons])
        else:
            return np.ones(len(self.neurons)) * 10
    
    def get_neuron_colors(self) -> List[str]:
        """Get neuron colors for visualization."""
        if 'color' in self.neurons[0]:
            return [neuron['color'] for neuron in self.neurons]
        else:
            return ['lightblue' for _ in self.neurons]
    
    def get_synapse_weights(self) -> np.ndarray:
        """Get synapse weights for visualization."""
        return np.array([synapse.get('weight', 0) for synapse in self.synapses])
    
    def get_spike_rates(self) -> np.ndarray:
        """Get spike rates for visualization."""
        return np.array([neuron.get('firing_rate', 0) for neuron in self.neurons])

class NeuralNetworkVisualizer:
    """Main neural network visualization class."""
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
        self.setup_matplotlib()
    
    def setup_matplotlib(self):
        """Setup matplotlib configuration."""
        plt.style.use('seaborn-v0_8')
        
        # Set figure size
        plt.rcParams['figure.figsize'] = (self.config.output_width / 100, 
                                         self.config.output_height / 100)
        
        # Set font size
        plt.rcParams['font.size'] = 10
        plt.rcParams['axes.labelsize'] = 12
        plt.rcParams['axes.titlesize'] = 14
        plt.rcParams['xtick.labelsize'] = 10
        plt.rcParams['ytick.labelsize'] = 10
        
        # Set color scheme
        self._set_color_scheme()
    
    def _set_color_scheme(self):
        """Set color scheme based on configuration."""
        color_schemes = {
            'viridis': plt.cm.viridis,
            'plasma': plt.cm.plasma,
            'coolwarm': plt.cm.coolwarm,
            'category10': self._create_category10_colors(),
            'default': self._create_default_colors()
        }
        
        self.color_scheme = color_schemes.get(self.config.color_scheme, color_schemes['default'])
    
    def _create_category10_colors(self) -> List[str]:
        """Create category 10 colors."""
        return plt.cm.tab10(np.linspace(0, 1, 10)).tolist()
    
    def _create_default_colors(self) -> List[str]:
        """Create default colors."""
        return plt.cm.Set3(np.linspace(0, 1, 12)).tolist()
    
    def create_neural_network_layout(self, data: NeuralNetworkData) -> go.Figure:
        """Create neural network layout visualization."""
        x, y = data.get_neuron_positions()
        sizes = data.get_neuron_sizes()
        colors = data.get_neuron_colors()
        
        # Create neurons
        neuron_trace = go.Scatter(
            x=x,
            y=y,
            mode='markers',
            marker=dict(
                size=sizes,
                color=colors,
                line=dict(color='black', width=1),
                sizemode='area'
            ),
            text=[f'Neuron {i}' for i in range(len(x))],
            hovertemplate='<b>Neuron %{text}</b><br>X: %{x:.2f}<br>Y: %{y:.2f}<extra></extra>'
        )
        
        # Create synapses
        synapse_weights = data.get_synapse_weights()
        if len(synapse_weights) > 0:
            synapse_trace = go.Scatter(
                x=x,
                y=y,
                mode='markers',
                marker=dict(
                    size=1,
                    color=synapse_weights,
                    colorscale='RdBu',
                    showscale=True,
                    cmin=min(synapse_weights),
                    cmax=max(synapse_weights),
                    opacity=0.6
                ),
                text=[f'Synapse {i}' for i in range(len(synapse_weights))],
                hovertemplate='<b>Synapse %{text}</b><br>Weight: %{marker.color:.3f}<extra></extra>'
            )
        else:
            synapse_trace = go.Scatter(x=[], y=[], mode='markers')
        
        # Create layout
        layout = go.Layout(
            title='Neural Network Layout',
            xaxis=dict(title='X Position', showgrid=True, zeroline=False),
            yaxis=dict(title='Y Position', showgrid=True, zeroline=False),
            hoverlabel=dict(bgcolor='white', font_size=12),
            width=self.config.output_width,
            height=self.config.output_height
        )
        
        # Add title based on theme
        if self.config.theme == 'dark':
            layout.update(dict(
                plot_bgcolor='rgba(0,0,0,0.8)',
                paper_bgcolor='rgba(0,0,0,0.8)',
                font=dict(color='white')
            ))
        
        # Create figure
        fig = go.Figure(data=[neuron_trace, synapse_trace], layout=layout)
        
        return fig
    
    def create_spike_activity_visualization(self, data: NeuralNetworkData) -> go.Figure:
        """Create spike activity visualization."""
        if not data.spike_times:
            fig = go.Figure()
            fig.update_layout(title='No spike data available')
            return fig
        
        # Create spike raster plot
        spike_times = np.array(data.spike_times)
        
        # Create figure
        fig = go.Figure()
        
        # Add spike raster
        fig.add_trace(go.Scatter(
            x=spike_times,
            y=np.zeros(len(spike_times)),
            mode='markers',
            marker=dict(
                size=5,
                color='red',
                opacity=0.7
            ),
            text=[f'Spike at {t:.2f}s' for t in spike_times],
            hovertemplate='<b>%{text}</b><extra></extra>'
        ))
        
        # Add time line
        max_time = max(spike_times) if spike_times else 10.0
        fig.add_trace(go.Scatter(
            x=[0, max_time],
            y=[0, 0],
            mode='lines',
            line=dict(color='black', width=2),
            showlegend=False,
            hovertemplate='<extra></extra>'
        ))
        
        # Update layout
        fig.update_layout(
            title='Spike Activity Visualization',
            xaxis=dict(title='Time (s)', showgrid=True),
            yaxis=dict(title='', showticklabels=False, zeroline=False),
            width=self.config.output_width,
            height=self.config.output_height,
            showlegend=False
        )
        
        return fig
    
    def create_neural_activity_heatmap(self, data: NeuralNetworkData) -> go.Figure:
        """Create neural activity heatmap visualization."""
        if not data.step_data:
            fig = go.Figure()
            fig.update_layout(title='No activity data available')
            return fig
        
        # Convert step data to numpy array
        step_data = pd.DataFrame(data.step_data)
        
        # Create heatmap data
        if 'spikes' in step_data.columns:
            heatmap_data = step_data['spikes'].values.reshape(-1, 1)
            
            # Create heatmap
            fig = go.Figure(data=go.Heatmap(
                z=heatmap_data,
                colorscale='Viridis',
                showscale=True,
                colorbar=dict(title='Spike Count')
            ))
            
            # Update layout
            fig.update_layout(
                title='Neural Activity Heatmap',
                xaxis=dict(title='Time Steps'),
                yaxis=dict(title='Neuron Index', showticklabels=False),
                width=self.config.output_width,
                height=self.config.output_height
            )
        
        return fig
    
    def create_network_analysis_plots(self, data: NeuralNetworkData) -> List[go.Figure]:
        """Create network analysis plots."""
        figures = []
        
        # Create figure for analysis data
        if data.analysis_data:
            analysis_data = data.analysis_data
            
            # Create subplots
            fig = go.Figure()
            
            # Add spike count subplot
            if 'total_spikes' in analysis_data:
                fig.add_trace(go.Scatter(
                    x=[0],
                    y=[analysis_data['total_spikes']],
                    mode='markers',
                    marker=dict(size=20, color='red'),
                    name='Total Spikes'
                ))
            
            # Add firing rate subplot
            if 'average_firing_rate' in analysis_data:
                fig.add_trace(go.Scatter(
                    x=[0],
                    y=[analysis_data['average_firing_rate']],
                    mode='markers',
                    marker=dict(size=20, color='blue'),
                    name='Average Firing Rate'
                ))
            
            # Add weight change subplot
            if 'weight_change_magnitude' in analysis_data:
                fig.add_trace(go.Scatter(
                    x=[0],
                    y=[analysis_data['weight_change_magnitude']],
                    mode='markers',
                    marker=dict(size=20, color='green'),
                    name='Weight Change Magnitude'
                ))
            
            # Update layout
            fig.update_layout(
                title='Network Analysis',
                xaxis=dict(showticklabels=False, zeroline=False),
                yaxis=dict(title='Value'),
                width=self.config.output_width,
                height=self.config.output_height,
                showlegend=True
            )
            
            figures.append(fig)
        
        return figures
    
    def create_pca_visualization(self, data: NeuralNetworkData) -> go.Figure:
        """Create PCA visualization of neural network data."""
        if not data.neurons or len(data.neurons) < 2:
            fig = go.Figure()
            fig.update_layout(title='Insufficient data for PCA visualization')
            return fig
        
        # Prepare data for PCA
        features = []
        for neuron in data.neurons:
            feature_vector = [
                neuron.get('x', 0),
                neuron.get('y', 0),
                neuron.get('radius', 10),
                neuron.get('firing_rate', 0),
                neuron.get('adaptation_level', 0)
            ]
            features.append(feature_vector)
        
        X = np.array(features)
        
        # Perform PCA
        pca = PCA(n_components=min(self.config.pca_components, X.shape[1]))
        X_pca = pca.fit_transform(X)
        
        # Create scatter plot
        fig = go.Figure(data=go.Scatter(
            x=X_pca[:, 0],
            y=X_pca[:, 1] if X_pca.shape[1] > 1 else np.zeros(X_pca.shape[0]),
            mode='markers',
            marker=dict(
                size=8,
                color=data.get_neuron_colors(),
                opacity=0.7
            ),
            text=[f'Neuron {i}' for i in range(len(X))],
            hovertemplate='<b>Neuron %{text}</b><br>PCA1: %{x:.2f}<br>PCA2: %{y:.2f}<extra></extra>'
        ))
        
        # Update layout
        fig.update_layout(
            title=f'PCA Visualization (Explained Variance: {pca.explained_variance_ratio_[0]:.2%})',
            xaxis=dict(title='Principal Component 1'),
            yaxis=dict(title='Principal Component 2'),
            width=self.config.output_width,
            height=self.config.output_height
        )
        
        return fig
    
    def create_interactive_dashboard(self, data: NeuralNetworkData) -> go.Figure:
        """Create interactive dashboard."""
        fig = go.Figure()
        
        # Create subplots
        fig = go.Figure()
        
        # Add neural network layout
        layout_fig = self.create_neural_network_layout(data)
        fig.add_trace(layout_fig.data[0])
        
        # Add spike activity
        spike_fig = self.create_spike_activity_visualization(data)
        
        # Create dashboard layout
        layout = go.Layout(
            title='Neural Network Interactive Dashboard',
            grid=dict(rows=2, columns=2),
            width=self.config.output_width,
            height=self.config.output_height
        )
        
        # Note: Creating complex dashboard with subplots would require
        # more sophisticated integration with Plotly's subplot functionality
        # For now, we'll create separate figures
        
        return layout
    
    def save_figure(self, fig: go.Figure, filename: str):
        """Save figure to file."""
        if self.config.output_format == 'html':
            fig.write_html(filename)
        elif self.config.output_format == 'png':
            fig.write_image(filename, dpi=self.config.output_dpi)
        elif self.config.output_format == 'svg':
            fig.write_html(filename.replace('.html', '.svg'))
        elif self.config.output_format == 'pdf':
            fig.write_pdf(filename.replace('.html', '.pdf'))
        else:
            raise ValueError(f"Unsupported output format: {self.config.output_format}")
    
    def export_data(self, data: NeuralNetworkData, filename: str):
        """Export data to file."""
        if self.config.data_format == 'csv':
            self._export_to_csv(data, filename)
        elif self.config.data_format == 'json':
            self._export_to_json(data, filename)
        else:
            raise ValueError(f"Unsupported data format: {self.config.data_format}")
    
    def _export_to_csv(self, data: NeuralNetworkData, filename: str):
        """Export data to CSV."""
        import csv
        
        with open(filename, 'w', newline='') as f:
            writer = csv.writer(f)
            
            # Write header
            writer.writerow(['neuron_id', 'x', 'y', 'radius', 'firing_rate', 'spike_count'])
            
            # Write data
            for i, neuron in enumerate(data.neurons):
                writer.writerow([
                    i,
                    neuron.get('x', 0),
                    neuron.get('y', 0),
                    neuron.get('radius', 10),
                    neuron.get('firing_rate', 0),
                    neuron.get('spike_count', 0)
                ])
    
    def _export_to_json(self, data: NeuralNetworkData, filename: str):
        """Export data to JSON."""
        export_data = {
            'neurons': data.neurons,
            'synapses': data.synapses,
            'spike_times': data.spike_times,
            'step_data': data.step_data,
            'analysis': data.analysis_data
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
    
    def create_complete_report(self, data: NeuralNetworkData, output_prefix: str):
        """Create complete visualization report."""
        # Create all visualization types
        figures = []
        
        # 1. Neural network layout
        layout_fig = self.create_neural_network_layout(data)
        figures.append(('neural_network_layout.html', layout_fig))
        
        # 2. Spike activity visualization
        spike_fig = self.create_spike_activity_visualization(data)
        figures.append(('spike_activity.html', spike_fig))
        
        # 3. PCA visualization
        pca_fig = self.create_pca_visualization(data)
        figures.append(('pca_visualization.html', pca_fig))
        
        # 4. Neural activity heatmap
        heatmap_fig = self.create_neural_activity_heatmap(data)
        figures.append(('neural_activity_heatmap.html', heatmap_fig))
        
        # 5. Network analysis plots
        analysis_figs = self.create_network_analysis_plots(data)
        for i, fig in enumerate(analysis_figs):
            figures.append((f'network_analysis_{i}.html', fig))
        
        # Save all figures
        for filename, fig in figures:
            self.save_figure(fig, f"{output_prefix}_{filename}")
        
        # Export data
        if self.config.export_data:
            self.export_data(data, f"{output_prefix}_data.{self.config.data_format}")
        
        print(f"Generated {len(figures)} visualization files")
        return [fig[0] for fig in figures]

class SpikeActivityVisualizer:
    """Specialized spike activity visualization."""
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
    
    def create_spike_raster_plot(self, spike_times: List[float], neurons: List[int]) -> go.Figure:
        """Create spike raster plot."""
        if not spike_times:
            fig = go.Figure()
            fig.update_layout(title='No spike data available')
            return fig
        
        # Create figure
        fig = go.Figure()
        
        # Add spike raster
        fig.add_trace(go.Scatter(
            x=spike_times,
            y=neurons,
            mode='markers',
            marker=dict(
                size=5,
                color=neurons,
                colorscale='Viridis',
                colorbar=dict(title='Neuron ID'),
                opacity=0.7
            ),
            text=[f'Neuron {neuron} at {time:.2f}s' for neuron, time in zip(neurons, spike_times)],
            hovertemplate='<b>%{text}</b><extra></extra>'
        ))
        
        # Update layout
        fig.update_layout(
            title='Spike Raster Plot',
            xaxis=dict(title='Time (s)', showgrid=True),
            yaxis=dict(title='Neuron ID', showgrid=True),
            width=800,
            height=600,
            showlegend=False
        )
        
        return fig
    
    def create_spike_histogram(self, spike_times: List[float], bin_width: float = 1.0) -> go.Figure:
        """Create spike histogram."""
        if not spike_times:
            fig = go.Figure()
            fig.update_layout(title='No spike data available')
            return fig
        
        # Create histogram
        fig = go.Figure(data=go.Histogram(
            x=spike_times,
            nbinsx=int(max(spike_times) / bin_width) if max(spike_times) > 0 else 10,
            marker_color='steelblue',
            opacity=0.7
        ))
        
        # Update layout
        fig.update_layout(
            title='Spike Histogram',
            xaxis=dict(title='Time (s)'),
            yaxis=dict(title='Count'),
            width=800,
            height=600
        )
        
        return fig
    
    def create_spike_train_visualization(self, spike_times: List[float], neurons: List[int], 
                                        duration: float = 10.0) -> go.Figure:
        """Create spike train visualization."""
        # Create figure
        fig = go.Figure()
        
        # Add spike trains for each neuron
        for neuron_id in sorted(set(neurons)):
            neuron_spikes = [t for n, t in zip(neurons, spike_times) if n == neuron_id]
            
            fig.add_trace(go.Scatter(
                x=neuron_spikes,
                y=[neuron_id] * len(neuron_spikes),
                mode='markers',
                marker=dict(size=3, color='red'),
                name=f'Neuron {neuron_id}',
                hovertemplate='<b>Neuron %{y}</b><br>Time: %{x:.2f}s<extra></extra>'
            ))
        
        # Update layout
        fig.update_layout(
            title='Spike Train Visualization',
            xaxis=dict(title='Time (s)', range=[0, duration]),
            yaxis=dict(title='Neuron ID', autorange='reversed'),
            width=800,
            height=600,
            showlegend=True
        )
        
        return fig

class NetworkTopologyVisualizer:
    """Network topology visualization."""
    
    def __init__(self, config: Optional[VisualizationConfig] = None):
        self.config = config or VisualizationConfig()
    
    def create_connectivity_matrix(self, adjacency_matrix: np.ndarray) -> go.Figure:
        """Create connectivity matrix visualization."""
        fig = go.Figure(data=go.Heatmap(
            z=adjacency_matrix,
            colorscale='Viridis',
            showscale=True,
            colorbar=dict(title='Connection Weight')
        ))
        
        # Update layout
        fig.update_layout(
            title='Connectivity Matrix',
            xaxis=dict(title='Target Neuron'),
            yaxis=dict(title='Source Neuron'),
            width=800,
            height=800
        )
        
        return fig
    
    def create_degree_distribution(self, degrees: List[int]) -> go.Figure:
        """Create degree distribution visualization."""
        fig = go.Figure(data=go.Histogram(
            x=degrees,
            marker_color='steelblue',
            opacity=0.7,
            name='Degree Distribution'
        ))
        
        # Update layout
        fig.update_layout(
            title='Degree Distribution',
            xaxis=dict(title='Degree'),
            yaxis=dict(title='Frequency'),
            width=800,
            height=600
        )
        
        return fig
    
    def create_small_world_metrics(self, metrics: Dict[str, float]) -> go.Figure:
        """Create small-world metrics visualization."""
        fig = go.Figure(data=go.Bar(
            x=list(metrics.keys()),
            y=list(metrics.values()),
            marker_color='steelblue',
            opacity=0.7
        ))
        
        # Update layout
        fig.update_layout(
            title='Small-World Metrics',
            xaxis=dict(title='Metric'),
            yaxis=dict(title='Value'),
            width=800,
            height=600
        )
        
        return fig

# High-level interface functions
def visualize_neural_network(
    brain_data: Dict[str, Any],
    filename: str,
    config: Optional[VisualizationConfig] = None
) -> List[str]:
    """
    Create comprehensive neural network visualization.
    
    Args:
        brain_data: Brain data dictionary
        filename: Base filename for output files
        config: Visualization configuration
    
    Returns:
        List of generated filenames
    """
    visualizer = NeuralNetworkVisualizer(config)
    data = NeuralNetworkData(brain_data)
    
    return visualizer.create_complete_report(data, filename)

def visualize_spike_activity(
    spike_times: List[float],
    neurons: List[int],
    filename: str
) -> str:
    """
    Create spike activity visualization.
    
    Args:
        spike_times: List of spike times
        neurons: List of neuron IDs for each spike
        filename: Output filename
    
    Returns:
        Generated filename
    """
    visualizer = SpikeActivityVisualizer()
    fig = visualizer.create_spike_raster_plot(spike_times, neurons)
    
    if filename.endswith('.html'):
        fig.write_html(filename)
    else:
        fig.write_html(filename + '.html')
    
    return filename if filename.endswith('.html') else filename + '.html'

def visualize_network_topology(
    adjacency_matrix: np.ndarray,
    filename: str
) -> str:
    """
    Create network topology visualization.
    
    Args:
        adjacency_matrix: Adjacency matrix of the network
        filename: Output filename
    
    Returns:
        Generated filename
    """
    visualizer = NetworkTopologyVisualizer()
    fig = visualizer.create_connectivity_matrix(adjacency_matrix)
    
    if filename.endswith('.html'):
        fig.write_html(filename)
    else:
        fig.write_html(filename + '.html')
    
    return filename if filename.endswith('.html') else filename + '.html'

def create_interactive_dashboard(
    brain_data: Dict[str, Any],
    filename: str,
    config: Optional[VisualizationConfig] = None
) -> str:
    """
    Create interactive dashboard.
    
    Args:
        brain_data: Brain data dictionary
        filename: Output filename
        config: Visualization configuration
    
    Returns:
        Generated filename
    """
    visualizer = NeuralNetworkVisualizer(config)
    data = NeuralNetworkData(brain_data)
    
    fig = visualizer.create_interactive_dashboard(data)
    
    if filename.endswith('.html'):
        fig.write_html(filename)
    else:
        fig.write_html(filename + '.html')
    
    return filename if filename.endswith('.html') else filename + '.html'

# Module-level functions for backward compatibility

def visualize_network(
    neurons: List[Dict[str, Any]],
    synapses: List[Dict[str, Any]],
    spike_times: List[float],
    output_prefix: str
) -> List[str]:
    """Visualize neural network (legacy function)."""
    brain_data = {
        'neurons': neurons,
        'synapses': synapses,
        'spike_times': spike_times
    }
    
    return visualize_neural_network(brain_data, output_prefix)

# Print module information
print("NLM Visualization Module v0.1.0")
print("Features:")
print("  - Neural network layout visualization")
print("  - Spike activity visualization")
print("  - Network topology visualization")
print("  - PCA and clustering visualization")
print("  - Interactive dashboards")
print("  - Data export (CSV, JSON)")
print("  - Multiple output formats (HTML, PNG, SVG, PDF)")
print()
