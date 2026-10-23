#!/usr/bin/env python3
"""
NLM Visualization Tools

Advanced visualization tools for NLM (Neural Learning Machine) Phase 6 brain data.
Provides comprehensive plotting, exporting, and interactive visualization capabilities.

Features:
- Multi-modal visualization (neural activity, memory, plasticity)
- Real-time interactive dashboards
- Export to multiple formats (PNG, SVG, PDF, HTML)
- 3D neural network visualization
- Time series analysis and plotting
- Comparative analysis tools
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import cm
import plotly.graph_objects as go
import plotly.express as px
import pandas as pd
from datetime import datetime, timedelta
import json
import os
from pathlib import Path
import asyncio
import threading
from typing import Dict, List, Any, Optional, Tuple
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class NLMVisualizer:
    """
    Comprehensive visualization toolkit for NLM brain data.
    
    Provides methods for creating various types of visualizations including:
    - Neural activity plots (spike trains, firing rates)
    - Synaptic weight changes over time
    - Memory system visualizations
    - Neuromodulator dynamics
    - Developmental stage progression
    - Multi-panel dashboards
    """
    
    def __init__(self, config: Optional[Dict] = None):
        """Initialize visualizer with configuration.
        
        Args:
            config: Configuration dictionary for visualization settings
        """
        self.config = config or {}
        self.theme = self.config.get('theme', 'neural')
        self.export_path = Path(self.config.get('export_path', './exports'))
        self.export_path.mkdir(exist_ok=True)
        
        # Set up matplotlib style
        self._setup_matplotlib_style()
        
        # Visualization data storage
        self.visualization_data = {}
        self.current_session = datetime.now().strftime('%Y%m%d_%H%M%S')
        
        logger.info(f"NLMVisualizer initialized with session: {self.current_session}")
    
    def _setup_matplotlib_style(self):
        """Set up matplotlib style based on theme."""
        if self.theme == 'neural':
            plt.style.use('dark_background')
            self.colors = {
                'spike': '#ff6b6b',
                'firing_rate': '#4ecdc4',
                'weight_change': '#45b7d1',
                'memory': '#f9c80e',
                'neuromod': '#6c5ba7',
                'development': '#ff9f43',
                'background': '#1a1a2e',
                'grid': '#16213e',
                'accent': '#0f3460'
            }
        elif self.theme == 'scientific':
            plt.style.use('seaborn-v0_8')
            self.colors = {
                'spike': '#d62728',
                'firing_rate': '#1f77b4',
                'weight_change': '#2ca02c',
                'memory': '#9467bd',
                'neuromod': '#8c564b',
                'development': '#e377c2',
                'background': 'white',
                'grid': '#e0e0e0',
                'accent': '#7f7f7f'
            }
        else:  # dark
            plt.style.use('dark_background')
            self.colors = {
                'spike': '#ff6b6b',
                'firing_rate': '#4ecdc4',
                'weight_change': '#45b7d1',
                'memory': '#f9c80e',
                'neuromod': '#6c5ba7',
                'development': '#ff9f43',
                'background': '#0a0a0a',
                'grid': '#1a1a1a',
                'accent': '#333333'
            }
    
    def create_spike_train_plot(self, spike_data: np.ndarray, 
                              time_steps: np.ndarray,
                              neurons: Optional[List[int]] = None,
                              title: str = "Spike Train Visualization",
                              save_plot: bool = True) -> plt.Figure:
        """Create spike train visualization.
        
        Args:
            spike_data: Array of spike times or binary spike data
            time_steps: Time step values
            neurons: List of neuron indices to visualize
            title: Plot title
            save_plot: Whether to save the plot
            
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 1, figsize=(14, 10))
        
        # Determine data shape
        if spike_data.ndim == 1:
            # Binary spike data
            spike_matrix = spike_data.reshape(-1, len(time_steps))
        else:
            spike_matrix = spike_data
            
        if neurons is None:
            neurons = list(range(min(20, spike_matrix.shape[1])))
            
        # Plot selected neuron spike trains
        for i, neuron_idx in enumerate(neurons[:15]):  # Plot first 15 neurons
            if neuron_idx < spike_matrix.shape[1]:
                neuron_spikes = spike_matrix[:, neuron_idx]
                spike_times = time_steps[neuron_spikes == 1]
                
                # Plot as stick diagram
                axes[0].vlines(spike_times, i, i+0.8, 
                             color=self.colors['spike'], linewidth=1)
        
        axes[0].set_xlim(time_steps[0], time_steps[-1])
        axes[0].set_ylim(0, len(neurons[:15]))
        axes[0].set_xlabel('Time (s)')
        axes[0].set_ylabel('Neuron Index')
        axes[0].set_title(title)
        axes[0].grid(True, alpha=0.3)
        
        # Plot firing rate histogram
        firing_rates = np.sum(spike_matrix, axis=0) / len(time_steps)
        axes[1].plot(time_steps, firing_rates, 
                   color=self.colors['firing_rate'], linewidth=2)
        axes[1].set_xlabel('Time (s)')
        axes[1].set_ylabel('Firing Rate (Hz)')
        axes[1].set_title('Average Firing Rate')
        axes[1].grid(True, alpha=0.3)
        axes[1].set_yscale('log')
        
        plt.tight_layout()
        
        if save_plot:
            save_path = self.export_path / f"spike_trains_{self.current_session}.png"
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Spike train plot saved to: {save_path}")
            
        return fig
    
    def create_weight_evolution_plot(self, weight_history: List[np.ndarray],
                                   time_steps: np.ndarray,
                                   title: str = "Synaptic Weight Evolution",
                                   save_plot: bool = True) -> plt.Figure:
        """Create synaptic weight evolution visualization.
        
        Args:
            weight_history: List of weight matrices over time
            time_steps: Time step values
            title: Plot title
            save_plot: Whether to save the plot
            
        Returns:
            matplotlib Figure object
        """
        n_time_points = len(weight_history)
        n_weights = weight_history[0].size if weight_history else 0
        
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # Mean weight over time
        mean_weights = [np.mean(weights) for weights in weight_history]
        axes[0, 0].plot(time_steps, mean_weights, 
                      color=self.colors['weight_change'], linewidth=2)
        axes[0, 0].set_xlabel('Time (s)')
        axes[0, 0].set_ylabel('Mean Weight')
        axes[0, 0].set_title('Mean Synaptic Weight')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Weight distribution at start and end
        axes[0, 1].boxplot([weight_history[0], weight_history[-1]], 
                         labels=['Start', 'End'])
        axes[0, 1].set_ylabel('Weight Value')
        axes[0, 1].set_title('Weight Distribution')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Weight change histogram
        initial_weights = weight_history[0]
        final_weights = weight_history[-1]
        weight_changes = final_weights - initial_weights
        axes[1, 0].hist(weight_changes, bins=50, alpha=0.7, 
                       color=self.colors['weight_change'], edgecolor='black')
        axes[1, 0].set_xlabel('Weight Change')
        axes[1, 0].set_ylabel('Count')
        axes[1, 0].set_title('Weight Change Distribution')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Cumulative weight change
        cumulative_change = np.cumsum(weight_changes)
        axes[1, 1].plot(cumulative_change, 
                      color=self.colors['weight_change'], linewidth=2)
        axes[1, 1].set_xlabel('Synapse Index')
        axes[1, 1].set_ylabel('Cumulative Change')
        axes[1, 1].set_title('Cumulative Weight Change')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.suptitle(title, fontsize=16)
        plt.tight_layout()
        
        if save_plot:
            save_path = self.export_path / f"weight_evolution_{self.current_session}.png"
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Weight evolution plot saved to: {save_path}")
            
        return fig
    
    def create_memory_system_visualization(self, memory_data: Dict[str, Any],
                                         title: str = "Memory System Visualization",
                                         save_plot: bool = True) -> plt.Figure:
        """Create memory system visualization.
        
        Args:
            memory_data: Dictionary containing memory system data
            title: Plot title
            save_plot: Whether to save the plot
            
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(3, 2, figsize=(16, 12))
        
        # Working memory capacity
        if 'working_memory_capacity' in memory_data:
            axes[0, 0].bar(['Working Memory'], 
                          [memory_data['working_memory_capacity']],
                          color=self.colors['memory'])
            axes[0, 0].set_ylabel('Capacity')
            axes[0, 0].set_title('Working Memory Capacity')
            axes[0, 0].grid(True, alpha=0.3)
        
        # Episodic memory episodes
        if 'episodic_memory_episodes' in memory_data:
            axes[0, 1].bar(['Episodic Memory'], 
                          [memory_data['episodic_memory_episodes']],
                          color=self.colors['memory'])
            axes[0, 1].set_ylabel('Number of Episodes')
            axes[0, 1].set_title('Episodic Memory Storage')
            axes[0, 1].grid(True, alpha=0.3)
        
        # Associative memory strength
        if 'associative_memory_strength' in memory_data:
            axes[1, 0].hist(memory_data['associative_memory_strength'],
                           bins=20, alpha=0.7, color=self.colors['memory'])
            axes[1, 0].set_xlabel('Memory Strength')
            axes[1, 0].set_ylabel('Count')
            axes[1, 0].set_title('Associative Memory Strength Distribution')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Memory consolidation over time
        if 'consolidation_timeline' in memory_data:
            timeline = memory_data['consolidation_timeline']
            axes[1, 1].plot(timeline['time'], timeline['strength'],
                          color=self.colors['memory'], linewidth=2)
            axes[1, 1].set_xlabel('Time (s)')
            axes[1, 1].set_ylabel('Consolidation Strength')
            axes[1, 1].set_title('Memory Consolidation Timeline')
            axes[1, 1].grid(True, alpha=0.3)
        
        # Memory system connections
        if 'memory_connections' in memory_data:
            connections = memory_data['memory_connections']
            axes[2, 0].pie(list(connections.values()), 
                          labels=list(connections.keys()),
                          colors=[self.colors['memory']] * len(connections),
                          autopct='%1.1f%%')
            axes[2, 0].set_title('Memory System Connections')
        
        # Memory access patterns
        if 'access_patterns' in memory_data:
            patterns = memory_data['access_patterns']
            axes[2, 1].bar(range(len(patterns)), patterns,
                          color=self.colors['memory'])
            axes[2, 1].set_xlabel('Pattern Index')
            axes[2, 1].set_ylabel('Access Frequency')
            axes[2, 1].set_title('Memory Access Patterns')
            axes[2, 1].grid(True, alpha=0.3)
        
        plt.suptitle(title, fontsize=16)
        plt.tight_layout()
        
        if save_plot:
            save_path = self.export_path / f"memory_visualization_{self.current_session}.png"
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Memory visualization plot saved to: {save_path}")
            
        return fig
    
    def create_neuromodulation_plot(self, neuromod_data: Dict[str, Any],
                                   title: str = "Neuromodulation Dynamics",
                                   save_plot: bool = True) -> plt.Figure:
        """Create neuromodulation dynamics visualization.
        
        Args:
            neuromod_data: Dictionary containing neuromodulator data
            title: Plot title
            save_plot: Whether to save the plot
            
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 3, figsize=(18, 10))
        
        # Dopamine levels over time
        if 'dopamine_timeline' in neuromod_data:
            dopamine = neuromod_data['dopamine_timeline']
            axes[0, 0].plot(dopamine['time'], dopamine['level'],
                          color=self.colors['neuromod'], linewidth=2)
            axes[0, 0].set_xlabel('Time (s)')
            axes[0, 0].set_ylabel('Dopamine Level')
            axes[0, 0].set_title('Dopamine Dynamics')
            axes[0, 0].grid(True, alpha=0.3)
        
        # Curiosity levels
        if 'curiosity_timeline' in neuromod_data:
            curiosity = neuromod_data['curiosity_timeline']
            axes[0, 1].plot(curiosity['time'], curiosity['level'],
                          color=self.colors['neuromod'], linewidth=2)
            axes[0, 1].set_xlabel('Time (s)')
            axes[0, 1].set_ylabel('Curiosity Level')
            axes[0, 1].set_title('Curiosity Dynamics')
            axes[0, 1].grid(True, alpha=0.3)
        
        # Novelty detection
        if 'novelty_timeline' in neuromod_data:
            novelty = neuromod_data['novelty_timeline']
            axes[0, 2].plot(novelty['time'], novelty['detection'],
                          color=self.colors['neuromod'], linewidth=2)
            axes[0, 2].set_xlabel('Time (s)')
            axes[0, 2].set_ylabel('Novelty Detection')
            axes[0, 2].set_title('Novelty Detection')
            axes[0, 2].grid(True, alpha=0.3)
        
        # Prediction error
        if 'prediction_error_timeline' in neuromod_data:
            pred_error = neuromod_data['prediction_error_timeline']
            axes[1, 0].plot(pred_error['time'], pred_error['error'],
                          color=self.colors['neuromod'], linewidth=2)
            axes[1, 0].set_xlabel('Time (s)')
            axes[1, 0].set_ylabel('Prediction Error')
            axes[1, 0].set_title('Prediction Error Dynamics')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Neuromodulator interaction matrix
        if 'interaction_matrix' in neuromod_data:
            interaction = neuromod_data['interaction_matrix']
            im = axes[1, 1].imshow(interaction, cmap='RdYlBu_r', aspect='auto')
            axes[1, 1].set_xlabel('Target Modulator')
            axes[1, 1].set_ylabel('Source Modulator')
            axes[1, 1].set_title('Neuromodulator Interactions')
            plt.colorbar(im, ax=axes[1, 1])
        
        # Neuromodulator correlations
        if 'correlations' in neuromod_data:
            correlations = neuromod_data['correlations']
            axes[1, 2].bar(range(len(correlations)), 
                          list(correlations.values()),
                          color=self.colors['neuromod'])
            axes[1, 2].set_xlabel('Modulator Pair')
            axes[1, 2].set_ylabel('Correlation Coefficient')
            axes[1, 2].set_title('Neuromodulator Correlations')
            axes[1, 2].grid(True, alpha=0.3)
        
        plt.suptitle(title, fontsize=16)
        plt.tight_layout()
        
        if save_plot:
            save_path = self.export_path / f"neuromodulation_{self.current_session}.png"
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Neuromodulation plot saved to: {save_path}")
            
        return fig
    
    def create_development_plot(self, development_data: Dict[str, Any],
                               title: str = "Developmental Progression",
                               save_plot: bool = True) -> plt.Figure:
        """Create developmental progression visualization.
        
        Args:
            development_data: Dictionary containing developmental data
            title: Plot title
            save_plot: Whether to save the plot
            
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # Developmental stages over time
        if 'stage_timeline' in development_data:
            stage_data = development_data['stage_timeline']
            stages = stage_data.get('stages', [])
            times = stage_data.get('times', [])
            
            for i, (stage, time) in enumerate(zip(stages, times)):
                axes[0, 0].axvline(x=time, color='gray', linestyle='--', alpha=0.5)
                axes[0, 0].text(time, i, stage, fontsize=10, 
                               verticalalignment='center')
            
            axes[0, 0].set_xlabel('Time (s)')
            axes[0, 0].set_ylabel('Developmental Stage')
            axes[0, 0].set_title('Developmental Stage Timeline')
            axes[0, 0].grid(True, alpha=0.3)
        
        # Plasticity rate changes
        if 'plasticity_timeline' in development_data:
            plasticity = development_data['plasticity_timeline']
            axes[0, 1].plot(plasticity['time'], plasticity['rate'],
                          color=self.colors['development'], linewidth=2)
            axes[0, 1].set_xlabel('Time (s)')
            axes[0, 1].set_ylabel('Plasticity Rate')
            axes[0, 1].set_title('Plasticity Rate Changes')
            axes[0, 1].grid(True, alpha=0.3)
        
        # Synaptogenesis and pruning
        if 'structural_changes' in development_data:
            structural = development_data['structural_changes']
            axes[1, 0].plot(structural['time'], structural['synaptogenesis'],
                          label='Synaptogenesis', color='#4ecdc4', linewidth=2)
            axes[1, 0].plot(structural['time'], structural['pruning'],
                          label='Pruning', color='#ff6b6b', linewidth=2)
            axes[1, 0].set_xlabel('Time (s)')
            axes[1, 0].set_ylabel('Rate')
            axes[1, 0].set_title('Structural Plasticity Changes')
            axes[1, 0].legend()
            axes[1, 0].grid(True, alpha=0.3)
        
        # Development milestones
        if 'milestones' in development_data:
            milestones = development_data['milestones']
            axes[1, 1].scatter(milestones['time'], milestones['achievement'],
                              c=range(len(milestones['achievement'])),
                              cmap='viridis', s=100, alpha=0.7)
            axes[1, 1].set_xlabel('Time (s)')
            axes[1, 1].set_ylabel('Achievement Level')
            axes[1, 1].set_title('Developmental Milestones')
            axes[1, 1].grid(True, alpha=0.3)
        
        plt.suptitle(title, fontsize=16)
        plt.tight_layout()
        
        if save_plot:
            save_path = self.export_path / f"development_{self.current_session}.png"
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Development plot saved to: {save_path}")
            
        return fig
    
    def create_dashboard(self, brain_data: Dict[str, Any],
                        include_plots: List[str] = None) -> go.Figure:
        """Create interactive dashboard visualization.
        
        Args:
            brain_data: Dictionary containing brain data
            include_plots: List of plot types to include
            
        Returns:
            plotly Figure object
        """
        if include_plots is None:
            include_plots = ['spikes', 'firing_rate', 'memory', 'neuromod']
        
        fig = go.Figure()
        
        # Add subplots
        n_plots = len(include_plots)
        
        for i, plot_type in enumerate(include_plots):
            if plot_type == 'spikes' and 'spike_data' in brain_data:
                spike_data = brain_data['spike_data']
                fig.add_trace(go.Scatter(
                    x=spike_data['time'],
                    y=spike_data['neuron'],
                    mode='markers',
                    marker=dict(size=3, color='red'),
                    name='Spike Trains'
                ))
            
            elif plot_type == 'firing_rate' and 'firing_rates' in brain_data:
                rates = brain_data['firing_rates']
                fig.add_trace(go.Scatter(
                    x=rates['time'],
                    y=rates['rate'],
                    mode='lines',
                    name='Firing Rate'
                ))
            
            elif plot_type == 'memory' and 'memory_data' in brain_data:
                memory = brain_data['memory_data']
                fig.add_trace(go.Heatmap(
                    z=memory.get('matrix', []),
                    colorscale='Viridis',
                    name='Memory Matrix'
                ))
            
            elif plot_type == 'neuromod' and 'neuromod_data' in brain_data:
                neuromod = brain_data['neuromod_data']
                fig.add_trace(go.Scatter(
                    x=neuromod['time'],
                    y=neuromod['dopamine'],
                    mode='lines',
                    name='Dopamine'
                ))
        
        fig.update_layout(
            title='NLM Brain Dashboard',
            xaxis_title='Time (s)',
            yaxis_title='Activity',
            hovermode='x unified',
            template='plotly_dark',
            height=600,
            showlegend=True
        )
        
        return fig
    
    def export_data(self, data: Dict[str, Any], format: str = 'json',
                   filename: Optional[str] = None) -> str:
        """Export visualization data.
        
        Args:
            data: Dictionary containing visualization data
            format: Export format (json, csv, npy, parquet)
            filename: Optional filename (auto-generated if not provided)
            
        Returns:
            Path to exported file
        """
        if filename is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"nlm_data_{timestamp}"
        
        save_path = self.export_path / filename
        
        if format == 'json':
            with open(f"{save_path}.json", 'w') as f:
                json.dump(data, f, indent=2, default=str)
            logger.info(f"Data exported to: {save_path}.json")
        
        elif format == 'csv':
            for name, df in data.items():
                if hasattr(df, 'to_csv'):
                    df.to_csv(f"{save_path}_{name}.csv")
            logger.info(f"Data exported to CSV files in: {save_path}")
        
        elif format == 'npy':
            for name, array in data.items():
                if isinstance(array, np.ndarray):
                    np.save(f"{save_path}_{name}.npy", array)
            logger.info(f"Data exported to .npy files in: {save_path}")
        
        elif format == 'parquet':
            try:
                import pyarrow.parquet as pq
                for name, df in data.items():
                    if hasattr(df, 'to_pandas'):
                        pq.write_table(df.to_pandas(), f"{save_path}_{name}.parquet")
                logger.info(f"Data exported to parquet files in: {save_path}")
            except ImportError:
                logger.warning("pyarrow not available for parquet export")
        
        return str(save_path)
    
    def create_animation(self, frames_data: List[Dict[str, Any]],
                        output_path: Optional[str] = None,
                        fps: int = 30) -> animation.FuncAnimation:
        """Create animation from visualization frames.
        
        Args:
            frames_data: List of frame data dictionaries
            output_path: Optional output path
            fps: Frames per second
            
        Returns:
            matplotlib animation object
        """
        fig, ax = plt.subplots(figsize=(12, 8))
        
        def update_frame(frame_idx):
            frame = frames_data[frame_idx]
            ax.clear()
            
            # Create visualization based on frame data
            if 'spikes' in frame:
                spike_data = frame['spikes']
                ax.vlines(spike_data['time'], 0, 1, color='red', linewidth=1)
            
            ax.set_xlim(0, 100)
            ax.set_ylim(0, 10)
            ax.set_title(f"Frame {frame_idx + 1}")
            ax.grid(True, alpha=0.3)
        
        anim = animation.FuncAnimation(fig, update_frame, 
                                      frames=len(frames_data), fps=fps)
        
        if output_path:
            anim.save(output_path, writer='ffmpeg', fps=fps)
            logger.info(f"Animation saved to: {output_path}")
        
        return anim
    
    def generate_report(self, data: Dict[str, Any], 
                       include_charts: bool = True,
                       include_tables: bool = True) -> str:
        """Generate comprehensive visualization report.
        
        Args:
            data: Dictionary containing visualization data
            include_charts: Whether to include charts in report
            include_tables: Whether to include tables in report
            
        Returns:
            Report file path
        """
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        report_path = self.export_path / f"visualization_report_{timestamp}.html"
        
        with open(report_path, 'w') as f:
            f.write(f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>NLM Visualization Report - {timestamp}</title>
                <style>
                    body {{ font-family: Arial, sans-serif; margin: 20px; }}
                    .header {{ background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
                              color: white; padding: 20px; border-radius: 10px; }}
                    .section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 8px; }}
                    .chart {{ width: 100%; height: 400px; margin: 10px 0; }}
                    .table {{ width: 100%; border-collapse: collapse; margin: 10px 0; }}
                    .table th, .table td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
                    .table th {{ background-color: #f2f2f2; }}
                </style>
            </head>
            <body>
                <div class="header">
                    <h1>NLM Visualization Report</h1>
                    <p>Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
                    <p>Session ID: {self.current_session}</p>
                </div>
                
                <div class="section">
                    <h2>Executive Summary</h2>
                    <p>This report contains comprehensive visualizations of NLM Phase 6 brain simulation data. 
                       The visualizations include neural activity patterns, memory system dynamics, 
                       neuromodulation effects, and developmental progression.</p>
                </div>
                
                {self._create_charts_section(data) if include_charts else ''}
                
                {self._create_tables_section(data) if include_tables else ''}
                
                <div class="section">
                    <h2>Technical Details</h2>
                    <p><strong>Visualization Theme:</strong> {self.theme}</p>
                    <p><strong>Export Path:</strong> {self.export_path}</p>
                    <p><strong>Visualization Session:</strong> {self.current_session}</p>
                </div>
            </body>
            </html>
            """)
        
        logger.info(f"Report generated: {report_path}")
        return str(report_path)
    
    def _create_charts_section(self, data: Dict[str, Any]) -> str:
        """Create HTML charts section for report."""
        html = '<div class="section"><h2>Visualizations</h2>'
        
        for chart_name, chart_data in data.items():
            if isinstance(chart_data, dict) and 'figure' in chart_data:
                # This would be enhanced with actual chart generation
                html += f'''
                <div class="chart-container">
                    <h3>{chart_name.replace('_', ' ').title()}</h3>
                    <div class="chart" id="chart_{chart_name}">
                        <!-- Chart would be embedded here -->
                        <p>Chart visualization would be generated for {chart_name}</p>
                    </div>
                </div>
                '''
        
        html += '</div>'
        return html
    
    def _create_tables_section(self, data: Dict[str, Any]) -> str:
        """Create HTML tables section for report."""
        html = '<div class="section"><h2>Data Tables</h2>'
        
        for table_name, table_data in data.items():
            if isinstance(table_data, (dict, list)):
                html += f'''
                <h3>{table_name.replace('_', ' ').title()}</h3>
                <table class="table">
                    <tr>
            '''
                if isinstance(table_data, dict):
                    for key, value in table_data.items():
                        html += f'<th>{key}</th><td>{value}</td>\n'
                elif isinstance(table_data, list):
                    if table_data and isinstance(table_data[0], dict):
                        headers = table_data[0].keys()
                        html += ''.join(f'<th>{h}</th>' for h in headers) + '\n'
                        for row in table_data:
                            html += ''.join(f'<td>{row.get(h, "")}</td>' for h in headers) + '\n'
                
                html += '</table>'
        
        html += '</div>'
        return html
    
    def __repr__(self) -> str:
        """Return string representation of visualizer."""
        return f"NLMVisualizer(session='{self.current_session}', theme='{self.theme}', export_path='{self.export_path}')"
    
    def __str__(self) -> str:
        """Return string description of visualizer."""
        return f"NLM Visualization Toolkit - Session: {self.current_session}, Theme: {self.theme}, Path: {self.export_path}"

# Example usage
if __name__ == "__main__":
    # Create visualizer
    visualizer = NLMVisualizer()
    
    # Generate sample data
    np.random.seed(42)
    time_steps = np.linspace(0, 100, 1000)
    
    # Sample spike data
    spike_matrix = np.random.binomial(1, 0.01, (1000, 50))
    
    # Create spike train plot
    fig1 = visualizer.create_spike_train_plot(
        spike_data=spike_matrix,
        time_steps=time_steps,
        neurons=list(range(10)),
        title="Sample Spike Train Visualization"
    )
    
    # Create weight evolution plot
    weight_history = [np.random.randn(100) * 0.1 + 0.5 for _ in range(10)]
    
    fig2 = visualizer.create_weight_evolution_plot(
        weight_history=weight_history,
        time_steps=time_steps[:10],
        title="Sample Weight Evolution"
    )
    
    # Create memory visualization
    memory_data = {
        'working_memory_capacity': 1000,
        'episodic_memory_episodes': 500,
        'associative_memory_strength': np.random.rand(100),
        'consolidation_timeline': {
            'time': np.linspace(0, 100, 50),
            'strength': np.random.rand(50) * 0.8
        },
        'memory_connections': {'working': 0.7, 'episodic': 0.3, 'semantic': 0.5},
        'access_patterns': np.random.rand(20)
    }
    
    fig3 = visualizer.create_memory_system_visualization(
        memory_data=memory_data,
        title="Sample Memory System Visualization"
    )
    
    plt.show()
    
    print(f"Visualization toolkit initialized: {visualizer}")
    print(f"Export path: {visualizer.export_path}")
    print(f"Session ID: {visualizer.current_session}")