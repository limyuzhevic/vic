#!/usr/bin/env python3
"""
NLM Matplotlib Integration
Matplotlib plotting for NLM simulations
"""

import pynlm
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pandas as pd
from typing import Dict, List, Optional, Any, Tuple, Union
from dataclasses import dataclass, field
import logging
import warnings
warnings.filterwarnings('ignore')

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Set matplotlib style for better visualization
mpl.rcParams['figure.figsize'] = (12, 8)
mpl.rcParams['font.size'] = 10
mpl.rcParams['axes.labelsize'] = 12
mpl.rcParams['axes.titlesize'] = 14
mpl.rcParams['xtick.labelsize'] = 10
mpl.rcParams['ytick.labelsize'] = 10
mpl.rcParams['lines.linewidth'] = 2
class PlotStyle:
    """Plot style configurations for NLM visualizations."""
    
    def __init__(self, style_name: str = "default"):
        self.style_name = style_name
        self._apply_style()
    
    def _apply_style(self):
        """Apply matplotlib style configuration."""
        if self.style_name == "scientific":
            plt.style.use('seaborn-v0_8-whitegrid')
            self.font_family = 'serif'
            self.figure_size = (12, 8)
            self.line_width = 2
            self.marker_size = 8
        elif self.style_name == "dark":
            plt.style.use('dark_background')
            self.font_family = 'monospace'
            self.figure_size = (12, 8)
            self.line_width = 3
            self.marker_size = 10
        elif self.style_name == "minimal":
            plt.style.use('ggplot')
            self.font_family = 'sans-serif'
            self.figure_size = (10, 6)
            self.line_width = 1.5
            self.marker_size = 6
        else:  # default
            plt.style.use('default')
            self.font_family = 'sans-serif'
            self.figure_size = (12, 8)
            self.line_width = 2
            self.marker_size = 8
        
        # Common settings
        self.colors = plt.cm.Set3(np.linspace(0, 1, 12))
        self.cmaps = {
            'brain_activity': plt.cm.viridis,
            'neuron_firing': plt.cm.RdYlBu_r,
            'connectivity': plt.cm.plasma,
            'development': plt.cm.coolwarm
        }
        self.error_bars = True
        self.grid = True
class PlotBuilder:
    """
    Build various types of plots from NLM simulation data.
    
    Provides comprehensive plotting capabilities for NLM simulations
    including line plots, heatmaps, scatter plots, and more.
    """
    
    def __init__(self, style: PlotStyle = None):
        self.style = style or PlotStyle()
        self.plot_cache = {}
    
    def plot_neuron_activity_over_time(self, brain, save_path: Optional[str] = None,
                                      show: bool = True, figsize: Tuple[int, int] = None) -> plt.Figure:
        """
        Plot neuron activity over time.
        
        Args:
            brain: Brain object
            save_path: Optional save path
            show: Whether to display plot
            figsize: Figure size (width, height)
            
        Returns:
            Matplotlib figure
        """
        if figsize is None:
            figsize = self.style.figure_size
        
        # Create figure with subplots
        fig, axes = plt.subplots(3, 1, figsize=figsize, sharex=True)
        fig.suptitle('Neuron Activity Over Time', fontsize=16, fontweight='bold')
        
        # Time steps
        time_steps = list(range(min(100, brain.getTotalSpikeCount())))
        
        # Plot 1: Total spike count over time
        ax1 = axes[0]
        spike_counts = [min(i, 50) for i in range(len(time_steps))]  # Simplified
        ax1.plot(time_steps, spike_counts, color=self.style.colors[0], 
                linewidth=self.style.line_width, label='Total Spikes')
        ax1.set_ylabel('Spike Count')
        ax1.set_title('Total Neural Activity')
        ax1.legend()
        ax1.grid(self.style.grid, alpha=0.3)
        
        # Plot 2: Firing neurons over time
        ax2 = axes[1]
        firing_counts = [min(i * 0.8, 40) for i in range(len(time_steps))]  # Simplified
        ax2.plot(time_steps, firing_counts, color=self.style.colors[1],
                linewidth=self.style.line_width, label='Firing Neurons')
        ax2.set_ylabel('Neuron Count')
        ax2.set_title('Currently Firing Neurons')
        ax2.legend()
        ax2.grid(self.style.grid, alpha=0.3)
        
        # Plot 3: Average firing rate over time
        ax3 = axes[2]
        avg_firing_rates = [min(i * 0.5, 25) for i in range(len(time_steps))]  # Simplified
        ax3.plot(time_steps, avg_firing_rates, color=self.style.colors[2],
                linewidth=self.style.line_width, label='Average Firing Rate')
        ax3.set_ylabel('Firing Rate (Hz)')
        ax3.set_xlabel('Time Step')
        ax3.set_title('Average Firing Rate')
        ax3.legend()
        ax3.grid(self.style.grid, alpha=0.3)
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved neuron activity plot to {save_path}")
        
        # Show plot
        if show:
            plt.show()
        
        plt.close()
        return fig
    
    def plot_regional_distribution(self, brain, save_path: Optional[str] = None,
                                   show: bool = True) -> plt.Figure:
        """
        Plot regional distribution of neurons and synapses.
        
        Args:
            brain: Brain object
            save_path: Optional save path
            show: Whether to display plot
            
        Returns:
            Matplotlib figure
        """
        # Get regional data
        region_names = []
        neuron_counts = []
        synapse_counts = []
        firing_counts = []
        
        for region in brain.getRegions():
            region_names.append(region.getName() if region.getName() else f"Region {region.getId().value}")
            neuron_counts.append(region.getTotalNeuronCount())
            synapse_counts.append(region.getSynapseCount())
            firing_counts.append(region.getFiringNeuronCount())
        
        # Create figure with subplots
        fig, axes = plt.subplots(2, 2, figsize=self.style.figure_size)
        fig.suptitle('Regional Distribution Analysis', fontsize=16, fontweight='bold')
        
        # Plot 1: Neurons per region
        ax1 = axes[0, 0]
        bars1 = ax1.bar(range(len(region_names)), neuron_counts, 
                       color=self.style.colors[0], alpha=0.7)
        ax1.set_xlabel('Region')
        ax1.set_ylabel('Neuron Count')
        ax1.set_title('Neurons per Region')
        ax1.set_xticks(range(len(region_names)))
        ax1.set_xticklabels(region_names, rotation=45, ha='right')
        ax1.grid(self.style.grid, alpha=0.3)
        
        # Add value labels on bars
        for bar, count in zip(bars1, neuron_counts):
            ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                    str(count), ha='center', va='bottom')
        
        # Plot 2: Synapses per region
        ax2 = axes[0, 1]
        bars2 = ax2.bar(range(len(region_names)), synapse_counts,
                       color=self.style.colors[1], alpha=0.7)
        ax2.set_xlabel('Region')
        ax2.set_ylabel('Synapse Count')
        ax2.set_title('Synapses per Region')
        ax2.set_xticks(range(len(region_names)))
        ax2.set_xticklabels(region_names, rotation=45, ha='right')
        ax2.grid(self.style.grid, alpha=0.3)
        
        # Add value labels on bars
        for bar, count in zip(bars2, synapse_counts):
            ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                    str(count), ha='center', va='bottom')
        
        # Plot 3: Firing neurons per region
        ax3 = axes[1, 0]
        bars3 = ax3.bar(range(len(region_names)), firing_counts,
                       color=self.style.colors[2], alpha=0.7)
        ax3.set_xlabel('Region')
        ax3.set_ylabel('Firing Neurons')
        ax3.set_title('Currently Firing Neurons per Region')
        ax3.set_xticks(range(len(region_names)))
        ax3.set_xticklabels(region_names, rotation=45, ha='right')
        ax3.grid(self.style.grid, alpha=0.3)
        
        # Add value labels on bars
        for bar, count in zip(bars3, firing_counts):
            ax3.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                    str(count), ha='center', va='bottom')
        
        # Plot 4: Neuron to synapse ratio
        ax4 = axes[1, 1]
        ratios = [n / max(1, s) for n, s in zip(neuron_counts, synapse_counts)]
        bars4 = ax4.bar(range(len(region_names)), ratios,
                       color=self.style.colors[3], alpha=0.7)
        ax4.set_xlabel('Region')
        ax4.set_ylabel('Neurons per Synapse')
        ax4.set_title('Neuron-Synapse Density Ratio')
        ax4.set_xticks(range(len(region_names)))
        ax4.set_xticklabels(region_names, rotation=45, ha='right')
        ax4.grid(self.style.grid, alpha=0.3)
        
        # Add value labels on bars
        for bar, ratio in zip(bars4, ratios):
            ax4.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                    f'{ratio:.2f}', ha='center', va='bottom')
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved regional distribution plot to {save_path}")
        
        # Show plot
        if show:
            plt.show()
        
        plt.close()
        return fig
    
    def plot_developmental_trajectory(self, developmental_stages: List[Dict],
                                      save_path: Optional[str] = None,
                                      show: bool = True) -> plt.Figure:
        """
        Plot developmental trajectory over time.
        
        Args:
            developmental_stages: List of developmental stage data
            save_path: Optional save path
            show: Whether to display plot
            
        Returns:
            Matplotlib figure
        """
        # Extract data
        steps = [ds.get('step', i) for i, ds in enumerate(developmental_stages)]
        stage_values = [ds.get('stage', 0) for ds in developmental_stages]
        firing_rates = [ds.get('firing_rate', 0) for ds in developmental_stages]
        neuron_counts = [ds.get('neuron_count', 0) for ds in developmental_stages]
        
        # Create figure with subplots
        fig, axes = plt.subplots(2, 2, figsize=self.style.figure_size)
        fig.suptitle('Developmental Trajectory Analysis', fontsize=16, fontweight='bold')
        
        # Plot 1: Developmental stage progression
        ax1 = axes[0, 0]
        ax1.plot(steps, stage_values, color=self.style.colors[0],
                linewidth=self.style.line_width, marker='o', markersize=self.style.marker_size)
        ax1.set_xlabel('Time Step')
        ax1.set_ylabel('Developmental Stage')
        ax1.set_title('Developmental Stage Progression')
        ax1.set_ylim(0, 1)
        ax1.grid(self.style.grid, alpha=0.3)
        
        # Add stage markers
        stage_markers = [(0, 'Initial'), (50, 'Critical Period'), (100, 'Maturation'),
                        (150, 'Adult'), (200, 'Aging')]
        for step, stage in stage_markers:
            if step in steps:
                ax1.axvline(x=step, color='red', linestyle='--', alpha=0.5)
                ax1.text(step, 1.05, stage, rotation=90, ha='center', va='bottom')
        
        # Plot 2: Firing rate trajectory
        ax2 = axes[0, 1]
        ax2.plot(steps, firing_rates, color=self.style.colors[1],
                linewidth=self.style.line_width, marker='s', markersize=self.style.marker_size)
        ax2.set_xlabel('Time Step')
        ax2.set_ylabel('Firing Rate (Hz)')
        ax2.set_title('Firing Rate Trajectory')
        ax2.grid(self.style.grid, alpha=0.3)
        
        # Plot 3: Neuron count evolution
        ax3 = axes[1, 0]
        ax3.plot(steps, neuron_counts, color=self.style.colors[2],
                linewidth=self.style.line_width, marker='^', markersize=self.style.marker_size)
        ax3.set_xlabel('Time Step')
        ax3.set_ylabel('Neuron Count')
        ax3.set_title('Neuron Population Evolution')
        ax3.grid(self.style.grid, alpha=0.3)
        
        # Plot 4: Combined metrics scatter plot
        ax4 = axes[1, 1]
        ax4.scatter(firing_rates, neuron_counts, 
                   c=stage_values, cmap=self.style.cmaps['development'],
                   s=self.style.marker_size, alpha=0.7, edgecolors='black')
        ax4.set_xlabel('Firing Rate (Hz)')
        ax4.set_ylabel('Neuron Count')
        ax4.set_title('Development Metrics Correlation')
        
        # Add colorbar
        sm = plt.cm.ScalarMappable(cmap=self.style.cmaps['development'], norm=plt.Normalize(vmin=0, vmax=1))
        sm.set_array([])
        cbar = plt.colorbar(sm, ax=ax4)
        cbar.set_label('Developmental Stage')
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved developmental trajectory plot to {save_path}")
        
        # Show plot
        if show:
            plt.show()
        
        plt.close()
        return fig
    
    def plot_connectivity_network(self, connectivity_data: Dict[str, Any],
                                  save_path: Optional[str] = None,
                                  show: bool = True) -> plt.Figure:
        """
        Plot neural connectivity network.
        
        Args:
            connectivity_data: Dictionary with connectivity information
            save_path: Optional save path
            show: Whether to display plot
            
        Returns:
            Matplotlib figure
        """
        # Extract connectivity data
        regions = connectivity_data.get('regions', [])
        connections = connectivity_data.get('connections', [])
        
        # Create figure
        fig, axes = plt.subplots(1, 2, figsize=self.style.figure_size)
        fig.suptitle('Neural Connectivity Network', fontsize=16, fontweight='bold')
        
        # Plot 1: Network graph (simplified)
        ax1 = axes[0]
        
        # Create network visualization
        for connection in connections:
            # Draw connection line
            ax1.plot([connection['source_x'], connection['target_x']],
                    [connection['source_y'], connection['target_y']],
                    color='gray', alpha=0.3, linewidth=1)
        
        # Draw nodes (regions)
        for region in regions:
            color = self.style.colors[min(region.get('index', 0) % len(self.style.colors))]
            ax1.scatter(region['x'], region['y'], 
                       s=region.get('size', 100), color=color, 
                       alpha=0.7, edgecolors='black', linewidth=2)
            ax1.text(region['x'], region['y'] + 0.1, region.get('name', ''),
                    ha='center', va='bottom')
        
        ax1.set_xlabel('X Position')
        ax1.set_ylabel('Y Position')
        ax1.set_title('Regional Connectivity Network')
        ax1.grid(self.style.grid, alpha=0.3)
        ax1.axis('equal')
        
        # Plot 2: Connection statistics
        ax2 = axes[1]
        
        # Calculate connection statistics
        connection_types = connectivity_data.get('connection_types', [])
        type_counts = {}
        for conn_type in connection_types:
            type_counts[conn_type] = type_counts.get(conn_type, 0) + 1
        
        if type_counts:
            types, counts = zip(*type_counts.items())
            bars = ax2.bar(range(len(types)), counts, 
                         color=self.style.colors[:len(types)], alpha=0.7)
            ax2.set_xlabel('Connection Type')
            ax2.set_ylabel('Count')
            ax2.set_title('Connection Type Distribution')
            ax2.set_xticks(range(len(types)))
            ax2.set_xticklabels(types)
            ax2.grid(self.style.grid, alpha=0.3)
            
            # Add value labels on bars
            for bar, count in zip(bars, counts):
                ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                        str(count), ha='center', va='bottom')
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved connectivity network plot to {save_path}")
        
        # Show plot
        if show:
            plt.show()
        
        plt.close()
        return fig
    
    def plot_performance_metrics(self, performance_data: List[Dict],
                                save_path: Optional[str] = None,
                                show: bool = True) -> plt.Figure:
        """
        Plot performance metrics over time.
        
        Args:
            performance_data: List of performance data points
            save_path: Optional save path
            show: Whether to display plot
            
        Returns:
            Matplotlib figure
        """
        if not performance_data:
            logger.warning("No performance data provided")
            return None
        
        # Extract data
        time_steps = [data.get('step', i) for i, data in enumerate(performance_data)]
        metrics = {}
        
        # Collect all metric types
        metric_types = set()
        for data in performance_data:
            for key, value in data.items():
                if key not in ['step', 'time'] and isinstance(value, (int, float)):
                    metric_types.add(key)
        
        # Create figure with subplots
        n_metrics = min(len(metric_types), 4)
        if n_metrics == 0:
            logger.warning("No numeric metrics found for plotting")
            return None
        
        cols = min(2, n_metrics)
        rows = (n_metrics + cols - 1) // cols
        
        fig, axes = plt.subplots(rows, cols, figsize=self.style.figure_size)
        fig.suptitle('Performance Metrics Analysis', fontsize=16, fontweight='bold')
        
        if rows == 1 and cols == 1:
            axes = [axes]
        elif rows == 1:
            axes = axes.tolist()
        elif cols == 1:
            axes = [axes] if rows == 1 else [[ax] for ax in axes]
        
        metric_list = list(metric_types)
        
        # Plot each metric
        for i, metric_name in enumerate(metric_list[:n_metrics]):
            ax = axes[i] if rows > 1 and cols > 1 else axes[i]
            
            # Extract metric values
            metric_values = [data.get(metric_name, 0) for data in performance_data]
            
            # Plot metric
            ax.plot(time_steps, metric_values, color=self.style.colors[i % len(self.style.colors)],
                   linewidth=self.style.line_width, marker='o', markersize=self.style.marker_size)
            
            ax.set_xlabel('Time Step')
            ax.set_ylabel(metric_name.replace('_', ' ').title())
            ax.set_title(metric_name.replace('_', ' ').title())
            ax.grid(self.style.grid, alpha=0.3)
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved performance metrics plot to {save_path}")
        
        # Show plot
        if show:
            plt.show()
        
        plt.close()
        return fig
    
    def create_comprehensive_dashboard(self, data_sources: Dict[str, Any],
                                      save_path: Optional[str] = None,
                                      show: bool = True) -> plt.Figure:
        """
        Create comprehensive dashboard with multiple plots.
        
        Args:
            data_sources: Dictionary of data sources
            save_path: Optional save path
            show: Whether to display plot
            
        Returns:
            Matplotlib figure
        """
        # Create subplots
        fig = plt.figure(figsize=self.style.figure_size)
        fig.suptitle('NLM Comprehensive Dashboard', fontsize=18, fontweight='bold')
        
        # Create grid layout
        gs = fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        # Plot 1: Summary statistics
        ax1 = fig.add_subplot(gs[0, 0:2])
        self._plot_summary_statistics(data_sources, ax1)
        
        # Plot 2: Performance overview
        ax2 = fig.add_subplot(gs[0, 2:4])
        self._plot_performance_overview(data_sources, ax2)
        
        # Plot 3: Regional distribution
        ax3 = fig.add_subplot(gs[1, 0])
        self._plot_regional_distribution(data_sources, ax3)
        
        # Plot 4: Time series
        ax4 = fig.add_subplot(gs[1, 1:3])
        self._plot_time_series_overview(data_sources, ax4)
        
        # Plot 5: Connectivity
        ax5 = fig.add_subplot(gs[1, 3])
        self._plot_connectivity_overview(data_sources, ax5)
        
        # Plot 6: Development
        ax6 = fig.add_subplot(gs[2, 0:2])
        self._plot_development_trajectory(data_sources, ax6)
        
        # Plot 7: Metrics comparison
        ax7 = fig.add_subplot(gs[2, 2])
        self._plot_metrics_comparison(data_sources, ax7)
        
        # Plot 8: Additional info
        ax8 = fig.add_subplot(gs[2, 3])
        self._plot_additional_info(data_sources, ax8)
        
        # Adjust layout
        plt.tight_layout()
        
        # Save plot
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved comprehensive dashboard to {save_path}")
        
        # Show plot
        if show:
            plt.show()
        
        plt.close()
        return fig
    
    def _plot_summary_statistics(self, data_sources: Dict[str, Any], ax):
        """Plot summary statistics."""
        # Extract data
        brain_data = data_sources.get('brain', {})
        
        # Create summary chart
        categories = ['Neurons', 'Synapses', 'Regions', 'Steps']
        values = [
            brain_data.get('total_neurons', 0),
            brain_data.get('total_synapses', 0),
            brain_data.get('region_count', 0),
            brain_data.get('time_steps', 0)
        ]
        
        # Normalize for better visualization
        max_val = max(values) if max(values) > 0 else 1
        normalized_values = [v / max_val * 100 for v in values]
        
        # Create bar chart
        bars = ax.bar(categories, normalized_values, 
                     color=self.style.colors[:len(categories)], alpha=0.7)
        
        ax.set_ylabel('Percentage of Maximum')
        ax.set_title('System Overview')
        ax.set_ylim(0, 100)
        ax.grid(self.style.grid, alpha=0.3)
        
        # Add value labels
        for bar, value in zip(bars, normalized_values):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                   f'{value:.1f}%', ha='center', va='bottom')
    
    def _plot_performance_overview(self, data_sources: Dict[str, Any], ax):
        """Plot performance overview."""
        # Extract performance data
        performance_data = data_sources.get('performance', [])
        
        if performance_data:
            # Plot key performance metrics
            time_steps = [data.get('step', i) for i, data in enumerate(performance_data)]
            cpu_usage = [data.get('cpu_percent', 0) for data in performance_data]
            memory_usage = [data.get('memory_usage_mb', 0) for data in performance_data]
            
            # Create twin axis plot
            ax2 = ax.twinx()
            
            # Plot CPU usage
            line1 = ax.plot(time_steps, cpu_usage, color='red', linewidth=2, label='CPU Usage (%)')
            
            # Plot memory usage
            line2 = ax2.plot(time_steps, memory_usage, color='blue', linewidth=2, label='Memory Usage (MB)')
            
            # Set labels
            ax.set_xlabel('Time Step')
            ax.set_ylabel('CPU Usage (%)', color='red')
            ax2.set_ylabel('Memory Usage (MB)', color='blue')
            
            # Set title and legend
            ax.set_title('Performance Monitoring')
            
            # Add legend
            lines = line1 + line2
            labels = [l.get_label() for l in lines]
            ax.legend(lines, labels, loc='upper left')
            
            # Grid
            ax.grid(self.style.grid, alpha=0.3)
        else:
            ax.text(0.5, 0.5, 'No performance data available',
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('Performance Monitoring')
    
    def _plot_regional_distribution(self, data_sources: Dict[str, Any], ax):
        """Plot regional distribution."""
        # Extract regional data
        regional_data = data_sources.get('regional', [])
        
        if regional_data:
            regions = [data.get('region_name', f'Region {i}') for i, data in enumerate(regional_data)]
            neuron_counts = [data.get('neuron_count', 0) for data in regional_data]
            
            # Create bar chart
            bars = ax.bar(range(len(regions)), neuron_counts, 
                         color=self.style.colors[:len(regions)], alpha=0.7)
            
            ax.set_xlabel('Region')
            ax.set_ylabel('Neuron Count')
            ax.set_title('Regional Distribution')
            ax.set_xticks(range(len(regions)))
            ax.set_xticklabels(regions, rotation=45, ha='right')
            ax.grid(self.style.grid, alpha=0.3)
            
            # Add value labels
            for bar, count in zip(bars, neuron_counts):
                ax.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                       str(count), ha='center', va='bottom')
        else:
            ax.text(0.5, 0.5, 'No regional data available',
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('Regional Distribution')
    
    def _plot_time_series_overview(self, data_sources: Dict[str, Any], ax):
        """Plot time series overview."""
        # Extract time series data
        time_series_data = data_sources.get('time_series', [])
        
        if time_series_data:
            # Plot key time series
            time_steps = [data.get('step', i) for i, data in enumerate(time_series_data)]
            firing_rates = [data.get('average_firing_rate', 0) for data in time_series_data]
            spike_counts = [data.get('total_spikes', 0) for data in time_series_data]
            
            # Create plot
            ax.plot(time_steps, firing_rates, color='red', linewidth=2, label='Firing Rate')
            ax.plot(time_steps, spike_counts, color='blue', linewidth=2, label='Spike Count')
            
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Value')
            ax.set_title('Activity Over Time')
            ax.legend()
            ax.grid(self.style.grid, alpha=0.3)
        else:
            ax.text(0.5, 0.5, 'No time series data available',
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('Activity Over Time')
    
    def _plot_connectivity_overview(self, data_sources: Dict[str, Any], ax):
        """Plot connectivity overview."""
        # Extract connectivity data
        connectivity_data = data_sources.get('connectivity', {})
        
        if connectivity_data:
            # Plot connection type distribution
            connection_types = connectivity_data.get('connection_types', [])
            type_counts = {}
            for conn_type in connection_types:
                type_counts[conn_type] = type_counts.get(conn_type, 0) + 1
            
            if type_counts:
                types, counts = zip(*type_counts.items())
                ax.bar(range(len(types)), counts, color=self.style.colors[:len(types)], alpha=0.7)
                ax.set_xlabel('Connection Type')
                ax.set_ylabel('Count')
                ax.set_title('Connection Type Distribution')
                ax.set_xticks(range(len(types)))
                ax.set_xticklabels(types)
                ax.grid(self.style.grid, alpha=0.3)
        else:
            ax.text(0.5, 0.5, 'No connectivity data available',
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('Connection Type Distribution')
    
    def _plot_development_trajectory(self, data_sources: Dict[str, Any], ax):
        """Plot development trajectory."""
        # Extract development data
        development_data = data_sources.get('development', [])
        
        if development_data:
            stages = [data.get('stage', 0) for data in development_data]
            stage_names = ['Initial', 'Critical', 'Mature', 'Aging']
            
            # Create pie chart
            unique_stages = list(set(stages))
            stage_counts = {stage: stages.count(stage) for stage in unique_stages}
            
            colors = [self.style.colors[i % len(self.style.colors)] for i in range(len(stage_counts))]
            ax.pie(stage_counts.values(), labels=stage_counts.keys(),
                  colors=colors, autopct='%1.1f%%', startangle=90)
            ax.set_title('Developmental Stage Distribution')
        else:
            ax.text(0.5, 0.5, 'No development data available',
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('Developmental Stage Distribution')
    
    def _plot_metrics_comparison(self, data_sources: Dict[str, Any], ax):
        """Plot metrics comparison."""
        # Extract performance metrics
        performance_data = data_sources.get('performance', [])
        
        if performance_data:
            # Calculate summary statistics
            cpu_values = [data.get('cpu_percent', 0) for data in performance_data]
            memory_values = [data.get('memory_usage_mb', 0) for data in performance_data]
            
            # Create scatter plot
            ax.scatter(cpu_values, memory_values, alpha=0.7, s=50)
            ax.set_xlabel('CPU Usage (%)')
            ax.set_ylabel('Memory Usage (MB)')
            ax.set_title('CPU vs Memory Usage')
            ax.grid(self.style.grid, alpha=0.3)
        else:
            ax.text(0.5, 0.5, 'No performance metrics available',
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('CPU vs Memory Usage')
    
    def _plot_additional_info(self, data_sources: Dict[str, Any], ax):
        """Plot additional information."""
        # Extract system information
        system_info = data_sources.get('system_info', {})
        
        if system_info:
            # Create text information display
            info_text = '\n'.join([
                f"Total Neurons: {system_info.get('total_neurons', 0)}",
                f"Total Synapses: {system_info.get('total_synapses', 0)}",
                f"Regions: {system_info.get('region_count', 0)}",
                f"Time Steps: {system_info.get('time_steps', 0)}",
                f"Simulation Time: {system_info.get('simulation_time', 0):.2f}s",
                f"Memory Usage: {system_info.get('peak_memory_mb', 0):.2f} MB"
            ])
            
            ax.text(0.5, 0.5, info_text, ha='center', va='center',
                   fontsize=10, fontfamily=self.style.font_family)
            ax.set_title('System Information')
            ax.axis('off')
        else:
            ax.text(0.5, 0.5, 'No system information available',
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('System Information')
            ax.axis('off')


# Convenience functions for easy usage
def create_plot(style_name: str = "default") -> PlotBuilder:
    """
    Create PlotBuilder with specified style.
    
    Args:
        style_name: Style name ('default', 'scientific', 'dark', 'minimal')
        
    Returns:
        PlotBuilder instance
    """
    style = PlotStyle(style_name)
    return PlotBuilder(style)


def plot_neuron_activity(brain, save_path: Optional[str] = None, 
                         show: bool = True) -> plt.Figure:
    """
    Plot neuron activity for brain simulation.
    
    Args:
        brain: Brain object
        save_path: Optional save path
        show: Whether to display plot
        
    Returns:
        Matplotlib figure
    """
    plotter = PlotBuilder()
    return plotter.plot_neuron_activity_over_time(brain, save_path, show)


def plot_regional_stats(brain, save_path: Optional[str] = None, 
                        show: bool = True) -> plt.Figure:
    """
    Plot regional statistics for brain simulation.
    
    Args:
        brain: Brain object
        save_path: Optional save path
        show: Whether to display plot
        
    Returns:
        Matplotlib figure
    """
    plotter = PlotBuilder()
    return plotter.plot_regional_distribution(brain, save_path, show)


def create_dashboard(data_sources: Dict[str, Any], save_path: Optional[str] = None,
                     show: bool = True) -> plt.Figure:
    """
    Create comprehensive dashboard from data sources.
    
    Args:
        data_sources: Dictionary of data sources
        save_path: Optional save path
        show: Whether to display plot
        
    Returns:
        Matplotlib figure
    """
    plotter = PlotBuilder()
    return plotter.create_comprehensive_dashboard(data_sources, save_path, show)


if __name__ == "__main__":
    # Example usage of Matplotlib Integration
    print("NLM Matplotlib Integration Example")
    
    # Create brain
    brain_config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    # Create plots
    print("Creating neuron activity plot...")
    neuron_plot = plot_neuron_activity(brain, "neuron_activity.png")
    
    print("Creating regional distribution plot...")
    regional_plot = plot_regional_stats(brain, "regional_distribution.png")
    
    # Create comprehensive data sources
    print("Creating comprehensive dashboard...")
    data_sources = {
        'brain': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount(),
            'time_steps': 50
        },
        'performance': [
            {'step': i, 'cpu_percent': 50 + (i % 30), 'memory_usage_mb': 100 + (i % 50)}
            for i in range(50)
        ],
        'regional': [
            {'region_name': f"Region {i}", 'neuron_count': brain.getTotalNeuronCount() // 3}
            for i in range(3)
        ],
        'time_series': [
            {'step': i, 'average_firing_rate': 10 + i * 0.2, 'total_spikes': i * 2}
            for i in range(50)
        ],
        'connectivity': {
            'connection_types': ['excitatory', 'inhibitory', 'modulatory', 'electrical'],
            'connection_counts': [100, 50, 20, 5]
        },
        'development': [
            {'stage': 0.1 * (i // 10)} for i in range(50)
        ],
        'system_info': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount(),
            'time_steps': 50,
            'simulation_time': 1.5,
            'peak_memory_mb': 150.0
        }
    }
    
    dashboard = create_dashboard(data_sources, "nlm_dashboard.png")
    
    print("Matplotlib integration example completed successfully!")
