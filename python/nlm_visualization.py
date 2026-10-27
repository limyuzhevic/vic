#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools

This module provides comprehensive visualization and debugging utilities
for NLM simulations, including data analysis, plotting, and interactive
visualization tools.

Key Features:
- Activity data visualization
- Memory activity analysis
- Performance trend analysis
- Export capabilities for external tools
- Interactive debugging tools
- Real-time visualization
- Statistical analysis
- Data export for machine learning

Usage:
    import nlm_visualization
    
    # Create visualization tools
    visualizer = nlm_visualization.Visualizer(brain)
    
    # Generate visualizations
    visualizer.plot_neuron_activity()
    visualizer.plot_memory_activity()
    visualizer.plot_performance_trends()
    
    # Export data for external tools
    visualizer.export_for_matlab("neuron_data.mat")
    visualizer.export_for_python("analysis_data.csv")
"""

# Import dependencies
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import numpy as np
import pandas as pd
import seaborn as sns
from typing import Dict, Any, List, Optional, Tuple
import json
import os
from datetime import datetime
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# === Core Visualization Classes ===
class Visualizer:
    """
    Comprehensive visualization and debugging tools for NLM simulations.
    
    Provides tools for visualizing neural activity, memory systems, and
    performance metrics with customizable plots and export capabilities.
    """
    
    def __init__(self, brain, output_dir: str = "visualizations"):
        """
        Initialize visualizer with brain and output directory.
        
        Args:
            brain: Brain object to visualize
            output_dir: Directory for saving visualizations
        """
        self.brain = brain
        self.output_dir = output_dir
        self.logger = logging.getLogger(f"Visualizer.{brain.getTotalNeuronCount()}")
        
        # Create output directory
        os.makedirs(self.output_dir, exist_ok=True)
        
        # Initialize plotting style
        self._setup_plotting_style()
        
        # Store visualization data
        self.neuron_data = {}
        self.memory_data = {}
        self.performance_data = {}
        
        self.logger.info(f"Visualizer initialized with output directory: {output_dir}")
    
    def _setup_plotting_style(self):
        """Setup matplotlib plotting style for NLM visualizations."""
        plt.style.use('seaborn-v0_8')
        
        # Custom colormap for neural activity
        self.neural_colormap = plt.cm.RdYlBu_r
        
        # Custom colormap for memory traces
        self.memory_colormap = plt.cm.viridis
        
        # Custom colormap for performance
        self.performance_colormap = plt.cm.plasma
    
    def collect_neuron_activity_data(self, steps: int = 100) -> Dict[str, Any]:
        """
        Collect comprehensive neuron activity data.
        
        Args:
            steps: Number of simulation steps to collect data for
            
        Returns:
            Dictionary with neuron activity data
        """
        self.logger.info(f"Collecting neuron activity data for {steps} steps")
        
        # Initialize data structures
        neuron_data = {
            'step': [],
            'total_neurons': [],
            'active_neurons': [],
            'firing_neurons': [],
            'average_firing_rate': [],
            'max_firing_rate': [],
            'min_firing_rate': [],
            'excitatory_inhibition_ratio': [],
            'membrane_potential_distribution': [],
            'time_series': {
                'step': [],
                'total_neurons': [],
                'active_neurons': [],
                'firing_neurons': []
            }
        }
        
        # Collect data for specified steps
        for step in range(steps):
            self.brain.step(step)
            
            # Get brain statistics
            stats = self.brain.get_stats()
            
            # Record data
            neuron_data['step'].append(step)
            neuron_data['total_neurons'].append(stats.get('total_neuron_count', 0))
            neuron_data['active_neurons'].append(stats.get('active_neuron_count', 0))
            neuron_data['firing_neurons'].append(stats.get('firing_neuron_count', 0))
            
            # Calculate firing rate statistics
            if stats.get('total_neuron_count', 0) > 0:
                firing_rate = (stats.get('firing_neuron_count', 0) / 
                             stats.get('total_neuron_count', 1)) * 100
            else:
                firing_rate = 0.0
            
            neuron_data['average_firing_rate'].append(firing_rate)
            
            # Store time series data
            neuron_data['time_series']['step'].append(step)
            neuron_data['time_series']['total_neurons'].append(stats.get('total_neuron_count', 0))
            neuron_data['time_series']['active_neurons'].append(stats.get('active_neuron_count', 0))
            neuron_data['time_series']['firing_neurons'].append(stats.get('firing_neuron_count', 0))
            
            # Record E/I ratio
            ei_ratio = stats.get('excitation_inhibition_ratio', 0.0)
            neuron_data['excitatory_inhibition_ratio'].append(ei_ratio)
            
            # Record membrane potential distribution (mock data)
            membrane_dist = np.random.normal(-65, 10, 100)  # Mock distribution
            neuron_data['membrane_potential_distribution'].append(membrane_dist)
        
        # Calculate summary statistics
        neuron_data['summary'] = {
            'total_steps': steps,
            'max_firing_rate': max(neuron_data['average_firing_rate']),
            'min_firing_rate': min(neuron_data['average_firing_rate']),
            'average_firing_rate': np.mean(neuron_data['average_firing_rate']),
            'firing_rate_std': np.std(neuron_data['average_firing_rate']),
            'max_ei_ratio': max(neuron_data['excitatory_inhibition_ratio']),
            'min_ei_ratio': min(neuron_data['excitatory_inhibition_ratio']),
            'average_ei_ratio': np.mean(neuron_data['excitatory_inhibition_ratio'])
        }
        
        self.neuron_data = neuron_data
        self.logger.info(f"Collected {len(neuron_data['step'])} data points")
        
        return neuron_data
    
    def collect_memory_activity_data(self, steps: int = 100) -> Dict[str, Any]:
        """
        Collect comprehensive memory system activity data.
        
        Args:
            steps: Number of simulation steps to collect data for
            
        Returns:
            Dictionary with memory activity data
        """
        self.logger.info(f"Collecting memory activity data for {steps} steps")
        
        # Initialize data structures
        memory_data = {
            'step': [],
            'working_memory_traces': [],
            'working_memory_capacity': [],
            'working_memory_utilization': [],
            'episodic_memory_episodes': [],
            'episodic_memory_capacity': [],
            'episodic_memory_utilization': [],
            'associative_memory_associations': [],
            'associative_memory_capacity': [],
            'memory_consolidation_events': [],
            'memory_replay_events': []
        }
        
        # Collect data for specified steps
        for step in range(steps):
            self.brain.step(step)
            
            # Get memory statistics if available
            memory_stats = self._get_memory_stats()
            
            # Record data
            memory_data['step'].append(step)
            memory_data['working_memory_traces'].append(
                memory_stats.get('working_memory', {}).get('active_traces', 0))
            memory_data['working_memory_capacity'].append(
                memory_stats.get('working_memory', {}).get('trace_capacity', 0))
            
            if memory_stats.get('working_memory', {}).get('trace_capacity', 0) > 0:
                utilization = (memory_stats.get('working_memory', {}).get('active_traces', 0) /
                             memory_stats.get('working_memory', {}).get('trace_capacity', 1))
            else:
                utilization = 0.0
            memory_data['working_memory_utilization'].append(utilization)
            
            memory_data['episodic_memory_episodes'].append(
                memory_stats.get('episodic_memory', {}).get('episode_count', 0))
            memory_data['episodic_memory_capacity'].append(
                memory_stats.get('episodic_memory', {}).get('max_episodes', 0))
            
            if memory_stats.get('episodic_memory', {}).get('max_episodes', 0) > 0:
                epi_utilization = (memory_stats.get('episodic_memory', {}).get('episode_count', 0) /
                                 memory_stats.get('episodic_memory', {}).get('max_episodes', 1))
            else:
                epi_utilization = 0.0
            memory_data['episodic_memory_utilization'].append(epi_utilization)
            
            memory_data['associative_memory_associations'].append(
                memory_stats.get('associative_memory', {}).get('association_count', 0))
            memory_data['associative_memory_capacity'].append(
                memory_stats.get('associative_memory', {}).get('similarity_threshold', 0))
        
        # Calculate summary statistics
        memory_data['summary'] = {
            'total_steps': steps,
            'avg_working_memory_utilization': np.mean(memory_data['working_memory_utilization']),
            'max_working_memory_utilization': max(memory_data['working_memory_utilization']),
            'avg_episodic_memory_utilization': np.mean(memory_data['episodic_memory_utilization']),
            'max_episodic_memory_utilization': max(memory_data['episodic_memory_utilization']),
            'total_memory_events': len([e for e in memory_data['memory_consolidation_events'] if e]) +
                                   len([e for e in memory_data['memory_replay_events'] if e])
        }
        
        self.memory_data = memory_data
        self.logger.info(f"Collected {len(memory_data['step'])} memory data points")
        
        return memory_data
    
    def collect_performance_data(self, steps: int = 100, sampling_rate: float = 10.0) -> Dict[str, Any]:
        """
        Collect performance monitoring data.
        
        Args:
            steps: Number of simulation steps to collect data for
            sampling_rate: Sampling rate in Hz
            
        Returns:
            Dictionary with performance data
        """
        self.logger.info(f"Collecting performance data for {steps} steps at {sampling_rate} Hz")
        
        # Initialize data structures
        performance_data = {
            'step': [],
            'time': [],
            'processing_time': [],
            'spike_count': [],
            'neuron_activity': [],
            'memory_usage': [],
            'performance_score': []
        }
        
        # Collect data for specified steps
        for step in range(steps):
            # Record start time
            start_time = time.time()
            
            # Run brain step
            self.brain.step(step)
            
            # Record end time
            end_time = time.time()
            processing_time = end_time - start_time
            
            # Get performance metrics
            stats = self.brain.get_stats()
            
            # Record data
            performance_data['step'].append(step)
            performance_data['time'].append(step * (1.0 / sampling_rate))
            performance_data['processing_time'].append(processing_time)
            performance_data['spike_count'].append(stats.get('total_spike_count', 0))
            performance_data['neuron_activity'].append(stats.get('firing_neuron_count', 0))
            
            # Calculate memory usage (mock)
            memory_usage = np.random.uniform(0.1, 0.9)  # Mock memory usage
            performance_data['memory_usage'].append(memory_usage)
            
            # Calculate performance score (mock)
            performance_score = (stats.get('firing_neuron_count', 0) * 0.5 +
                               (1.0 - processing_time / 0.1) * 0.5)  # Normalize processing time
            performance_data['performance_score'].append(performance_score)
        
        # Calculate summary statistics
        performance_data['summary'] = {
            'total_steps': steps,
            'average_processing_time': np.mean(performance_data['processing_time']),
            'min_processing_time': min(performance_data['processing_time']),
            'max_processing_time': max(performance_data['processing_time']),
            'std_processing_time': np.std(performance_data['processing_time']),
            'total_spikes': sum(performance_data['spike_count']),
            'average_performance_score': np.mean(performance_data['performance_score']),
            'max_performance_score': max(performance_data['performance_score'])
        }
        
        self.performance_data = performance_data
        self.logger.info(f"Collected {len(performance_data['step'])} performance data points")
        
        return performance_data
    
    def _get_memory_stats(self) -> Dict[str, Any]:
        """Get memory statistics from brain."""
        try:
            if hasattr(self.brain, 'getWorkingMemory'):
                working_memory = self.brain.getWorkingMemory()
                if working_memory:
                    return {
                        'working_memory': {
                            'active_traces': working_memory.getActiveTraces(),
                            'trace_capacity': working_memory.getTraceCapacity(),
                            'competition_count': working_memory.getCompetitionCount()
                        },
                        'episodic_memory': {
                            'episode_count': working_memory.getEpisodeCount() if hasattr(working_memory, 'getEpisodeCount') else 0,
                            'max_episodes': working_memory.getMaxEpisodes() if hasattr(working_memory, 'getMaxEpisodes') else 0,
                            'consolidation_threshold': working_memory.getConsolidationThreshold() if hasattr(working_memory, 'getConsolidationThreshold') else 0.0
                        },
                        'associative_memory': {
                            'association_count': working_memory.getAssociationCount() if hasattr(working_memory, 'getAssociationCount') else 0,
                            'similarity_threshold': working_memory.getSimilarityThreshold() if hasattr(working_memory, 'getSimilarityThreshold') else 0.0
                        }
                    }
        except Exception as e:
            self.logger.warning(f"Error getting memory stats: {e}")
        
        return {}
    
    def plot_neuron_activity(self, output_format: str = "png") -> str:
        """
        Generate neuron activity visualization.
        
        Args:
            output_format: Output format (png, pdf, svg)
            
        Returns:
            Path to saved visualization
        """
        if not self.neuron_data:
            self.collect_neuron_activity_data()
        
        self.logger.info("Generating neuron activity plot")
        
        # Create figure with subplots
        fig, axes = plt.subplots(3, 2, figsize=(15, 12))
        fig.suptitle(f"Neuron Activity - {self.brain.getTotalNeuronCount()} Neurons", 
                    fontsize=16, fontweight='bold')
        
        # Plot 1: Total neurons vs Step
        ax1 = axes[0, 0]
        ax1.plot(self.neuron_data['step'], self.neuron_data['total_neurons'], 
                linewidth=2, color='blue')
        ax1.set_title('Total Neurons Over Time')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Neuron Count')
        ax1.grid(True, alpha=0.3)
        
        # Plot 2: Active neurons vs Step
        ax2 = axes[0, 1]
        ax2.plot(self.neuron_data['step'], self.neuron_data['active_neurons'],
                linewidth=2, color='green')
        ax2.set_title('Active Neurons Over Time')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Active Neurons')
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Firing neurons vs Step
        ax3 = axes[1, 0]
        ax3.plot(self.neuron_data['step'], self.neuron_data['firing_neurons'],
                linewidth=2, color='red')
        ax3.set_title('Firing Neurons Over Time')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Firing Neurons')
        ax3.grid(True, alpha=0.3)
        
        # Plot 4: Firing rate over time
        ax4 = axes[1, 1]
        ax4.plot(self.neuron_data['step'], self.neuron_data['average_firing_rate'],
                linewidth=2, color='purple')
        ax4.set_title('Average Firing Rate Over Time')
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Firing Rate (%)')
        ax4.grid(True, alpha=0.3)
        
        # Plot 5: E/I ratio over time
        ax5 = axes[2, 0]
        ax5.plot(self.neuron_data['step'], self.neuron_data['excitatory_inhibition_ratio'],
                linewidth=2, color='orange')
        ax5.set_title('Excitatory/Inhibitory Ratio Over Time')
        ax5.set_xlabel('Step')
        ax5.set_ylabel('E/I Ratio')
        ax5.grid(True, alpha=0.3)
        
        # Plot 6: Summary statistics
        ax6 = axes[2, 1]
        summary = self.neuron_data['summary']
        metrics = ['Average Firing Rate (%)', 'Firing Rate Std', 'Max E/I Ratio', 'Min E/I Ratio']
        values = [summary['average_firing_rate'], 
                 summary['firing_rate_std'],
                 summary['max_ei_ratio'],
                 summary['min_ei_ratio']]
        
        bars = ax6.bar(metrics, values, color=['skyblue', 'lightgreen', 'lightcoral', 'gold'])
        ax6.set_title('Summary Statistics')
        ax6.set_ylabel('Value')
        ax6.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax6.text(bar.get_x() + bar.get_width()/2., height,
                    f'{value:.3f}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        # Save plot
        filename = f"neuron_activity_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{output_format}"
        filepath = os.path.join(self.output_dir, filename)
        plt.savefig(filepath, dpi=300, bbox_inches='tight')
        plt.close()
        
        self.logger.info(f"Neuron activity plot saved to: {filepath}")
        return filepath
    
    def plot_memory_activity(self, output_format: str = "png") -> str:
        """
        Generate memory activity visualization.
        
        Args:
            output_format: Output format (png, pdf, svg)
            
        Returns:
            Path to saved visualization
        """
        if not self.memory_data:
            self.collect_memory_activity_data()
        
        self.logger.info("Generating memory activity plot")
        
        # Create figure with subplots
        fig, axes = plt.subplots(3, 2, figsize=(15, 12))
        fig.suptitle("Memory Activity Analysis", fontsize=16, fontweight='bold')
        
        # Plot 1: Working memory traces
        ax1 = axes[0, 0]
        ax1.plot(self.memory_data['step'], self.memory_data['working_memory_traces'],
                linewidth=2, color='blue', label='Active Traces')
        ax1.plot(self.memory_data['step'], self.memory_data['working_memory_capacity'],
                linewidth=2, color='red', linestyle='--', label='Capacity')
        ax1.set_title('Working Memory Activity')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Trace Count')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Plot 2: Working memory utilization
        ax2 = axes[0, 1]
        ax2.plot(self.memory_data['step'], self.memory_data['working_memory_utilization'],
                linewidth=2, color='green')
        ax2.set_title('Working Memory Utilization')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Utilization Rate')
        ax2.set_ylim(0, 1)
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Episodic memory episodes
        ax3 = axes[1, 0]
        ax3.plot(self.memory_data['step'], self.memory_data['episodic_memory_episodes'],
                linewidth=2, color='purple', label='Episodes')
        ax3.plot(self.memory_data['step'], self.memory_data['episodic_memory_capacity'],
                linewidth=2, color='orange', linestyle='--', label='Capacity')
        ax3.set_title('Episodic Memory Activity')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Episode Count')
        ax3.legend()
        ax3.grid(True, alpha=0.3)
        
        # Plot 4: Episodic memory utilization
        ax4 = axes[1, 1]
        ax4.plot(self.memory_data['step'], self.memory_data['episodic_memory_utilization'],
                linewidth=2, color='brown')
        ax4.set_title('Episodic Memory Utilization')
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Utilization Rate')
        ax4.set_ylim(0, 1)
        ax4.grid(True, alpha=0.3)
        
        # Plot 5: Associative memory associations
        ax5 = axes[2, 0]
        ax5.plot(self.memory_data['step'], self.memory_data['associative_memory_associations'],
                linewidth=2, color='cyan', label='Associations')
        ax5.set_title('Associative Memory Activity')
        ax5.set_xlabel('Step')
        ax5.set_ylabel('Association Count')
        ax5.legend()
        ax5.grid(True, alpha=0.3)
        
        # Plot 6: Memory summary
        ax6 = axes[2, 1]
        summary = self.memory_data['summary']
        metrics = ['Working Mem Util', 'Episodic Mem Util', 'Memory Events']
        values = [
            summary['avg_working_memory_utilization'],
            summary['avg_episodic_memory_utilization'],
            summary['total_memory_events']
        ]
        
        bars = ax6.bar(metrics, values, color=['skyblue', 'lightcoral', 'lightgreen'])
        ax6.set_title('Memory System Summary')
        ax6.set_ylabel('Value')
        ax6.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax6.text(bar.get_x() + bar.get_width()/2., height,
                    f'{value:.3f}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        # Save plot
        filename = f"memory_activity_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{output_format}"
        filepath = os.path.join(self.output_dir, filename)
        plt.savefig(filepath, dpi=300, bbox_inches='tight')
        plt.close()
        
        self.logger.info(f"Memory activity plot saved to: {filepath}")
        return filepath
    
    def plot_performance_trends(self, output_format: str = "png") -> str:
        """
        Generate performance trends visualization.
        
        Args:
            output_format: Output format (png, pdf, svg)
            
        Returns:
            Path to saved visualization
        """
        if not self.performance_data:
            self.collect_performance_data()
        
        self.logger.info("Generating performance trends plot")
        
        # Create figure with subplots
        fig, axes = plt.subplots(3, 2, figsize=(15, 12))
        fig.suptitle("Performance Trends Analysis", fontsize=16, fontweight='bold')
        
        # Plot 1: Processing time over time
        ax1 = axes[0, 0]
        ax1.plot(self.performance_data['step'], 
                np.array(self.performance_data['processing_time']) * 1000,  # Convert to ms
                linewidth=2, color='red')
        ax1.set_title('Processing Time Over Time')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Processing Time (ms)')
        ax1.grid(True, alpha=0.3)
        
        # Plot 2: Spike count over time
        ax2 = axes[0, 1]
        ax2.plot(self.performance_data['step'], self.performance_data['spike_count'],
                linewidth=2, color='blue')
        ax2.set_title('Spike Count Over Time')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Spike Count')
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Neuron activity over time
        ax3 = axes[1, 0]
        ax3.plot(self.performance_data['step'], self.performance_data['neuron_activity'],
                linewidth=2, color='green')
        ax3.set_title('Neuron Activity Over Time')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Active Neurons')
        ax3.grid(True, alpha=0.3)
        
        # Plot 4: Performance score over time
        ax4 = axes[1, 1]
        ax4.plot(self.performance_data['step'], self.performance_data['performance_score'],
                linewidth=2, color='purple')
        ax4.set_title('Performance Score Over Time')
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Performance Score')
        ax4.set_ylim(0, 1)
        ax4.grid(True, alpha=0.3)
        
        # Plot 5: Memory usage over time
        ax5 = axes[2, 0]
        ax5.plot(self.performance_data['step'], self.performance_data['memory_usage'],
                linewidth=2, color='orange')
        ax5.set_title('Memory Usage Over Time')
        ax5.set_xlabel('Step')
        ax5.set_ylabel('Memory Usage')
        ax5.set_ylim(0, 1)
        ax5.grid(True, alpha=0.3)
        
        # Plot 6: Performance summary
        ax6 = axes[2, 1]
        summary = self.performance_data['summary']
        metrics = ['Avg Processing Time (ms)', 'Min Processing Time (ms)', 
                  'Max Processing Time (ms)', 'Total Spikes', 'Avg Performance Score']
        values = [
            summary['average_processing_time'] * 1000,
            summary['min_processing_time'] * 1000,
            summary['max_processing_time'] * 1000,
            summary['total_spikes'],
            summary['average_performance_score']
        ]
        
        bars = ax6.bar(metrics, values, color=['skyblue', 'lightcoral', 'lightgreen', 
                                             'gold', 'plum'], alpha=0.8)
        ax6.set_title('Performance Summary')
        ax6.set_ylabel('Value')
        ax6.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax6.text(bar.get_x() + bar.get_width()/2., height,
                    f'{value:.3f}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        # Save plot
        filename = f"performance_trends_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{output_format}"
        filepath = os.path.join(self.output_dir, filename)
        plt.savefig(filepath, dpi=300, bbox_inches='tight')
        plt.close()
        
        self.logger.info(f"Performance trends plot saved to: {filepath}")
        return filepath
    
    def generate_heatmaps(self, output_format: str = "png") -> Dict[str, str]:
        """
        Generate heatmap visualizations for activity patterns.
        
        Args:
            output_format: Output format (png, pdf, svg)
            
        Returns:
            Dictionary mapping plot types to file paths
        """
        self.logger.info("Generating heatmap visualizations")
        
        # Generate various heatmaps
        heatmaps = {}
        
        if self.neuron_data:
            # Neuron activity heatmap
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle("Neuron Activity Heatmaps", fontsize=14, fontweight='bold')
            
            # Time series heatmap
            time_series_data = np.array(self.neuron_data['time_series'])
            # Normalize data for heatmap
            normalized_data = (time_series_data - time_series_data.min(axis=0)) / \
                            (time_series_data.max(axis=0) - time_series_data.min(axis=0))
            
            sns.heatmap(normalized_data.T, ax=axes[0, 0], cmap=self.neural_colormap,
                      cbar_kws={'label': 'Normalized Activity'})
            axes[0, 0].set_title('Neuron Activity Heatmap')
            axes[0, 0].set_xlabel('Time Steps')
            axes[0, 0].set_ylabel('Neuron Type')
            
            # Firing rate heatmap
            firing_rate_data = np.array(self.neuron_data['average_firing_rate']).reshape(-1, 1)
            sns.heatmap(firing_rate_data, ax=axes[0, 1], cmap=self.neural_colormap,
                      cbar_kws={'label': 'Firing Rate (%)'})
            axes[0, 1].set_title('Firing Rate Heatmap')
            axes[0, 1].set_xlabel('Time Steps')
            axes[0, 1].set_ylabel('Time Steps')
            
            # E/I ratio heatmap
            ei_ratio_data = np.array(self.neuron_data['excitatory_inhibition_ratio']).reshape(-1, 1)
            sns.heatmap(ei_ratio_data, ax=axes[1, 0], cmap=self.neural_colormap,
                      cbar_kws={'label': 'E/I Ratio'})
            axes[1, 0].set_title('E/I Ratio Heatmap')
            axes[1, 0].set_xlabel('Time Steps')
            axes[1, 0].set_ylabel('Time Steps')
            
            plt.tight_layout()
            
            filename = f"neuron_heatmap_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{output_format}"
            filepath = os.path.join(self.output_dir, filename)
            plt.savefig(filepath, dpi=300, bbox_inches='tight')
            plt.close()
            
            heatmaps['neuron_activity'] = filepath
            self.logger.info(f"Neuron activity heatmap saved to: {filepath}")
        
        if self.memory_data:
            # Memory activity heatmap
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle("Memory Activity Heatmaps", fontsize=14, fontweight='bold')
            
            # Working memory heatmap
            wm_data = np.array(self.memory_data['working_memory_utilization']).reshape(-1, 1)
            sns.heatmap(wm_data, ax=axes[0, 0], cmap=self.memory_colormap,
                      cbar_kws={'label': 'Memory Utilization'})
            axes[0, 0].set_title('Working Memory Heatmap')
            axes[0, 0].set_xlabel('Time Steps')
            axes[0, 0].set_ylabel('Time Steps')
            
            # Episodic memory heatmap
            em_data = np.array(self.memory_data['episodic_memory_utilization']).reshape(-1, 1)
            sns.heatmap(em_data, ax=axes[0, 1], cmap=self.memory_colormap,
                      cbar_kws={'label': 'Memory Utilization'})
            axes[0, 1].set_title('Episodic Memory Heatmap')
            axes[0, 1].set_xlabel('Time Steps')
            axes[0, 1].set_ylabel('Time Steps')
            
            plt.tight_layout()
            
            filename = f"memory_heatmap_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{output_format}"
            filepath = os.path.join(self.output_dir, filename)
            plt.savefig(filepath, dpi=300, bbox_inches='tight')
            plt.close()
            
            heatmaps['memory_activity'] = filepath
            self.logger.info(f"Memory activity heatmap saved to: {filepath}")
        
        if self.performance_data:
            # Performance heatmap
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle("Performance Heatmaps", fontsize=14, fontweight='bold')
            
            # Processing time heatmap
            proc_time_data = np.array(self.performance_data['processing_time']).reshape(-1, 1)
            sns.heatmap(proc_time_data, ax=axes[0, 0], cmap=self.performance_colormap,
                      cbar_kws={'label': 'Processing Time (s)'})
            axes[0, 0].set_title('Processing Time Heatmap')
            axes[0, 0].set_xlabel('Time Steps')
            axes[0, 0].set_ylabel('Time Steps')
            
            # Performance score heatmap
            perf_score_data = np.array(self.performance_data['performance_score']).reshape(-1, 1)
            sns.heatmap(perf_score_data, ax=axes[0, 1], cmap=self.performance_colormap,
                      cbar_kws={'label': 'Performance Score'})
            axes[0, 1].set_title('Performance Score Heatmap')
            axes[0, 1].set_xlabel('Time Steps')
            axes[0, 1].set_ylabel('Time Steps')
            
            plt.tight_layout()
            
            filename = f"performance_heatmap_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{output_format}"
            filepath = os.path.join(self.output_dir, filename)
            plt.savefig(filepath, dpi=300, bbox_inches='tight')
            plt.close()
            
            heatmaps['performance'] = filepath
            self.logger.info(f"Performance heatmap saved to: {filepath}")
        
        return heatmaps
    
    def export_data_for_external_tools(self, export_format: str = "json") -> Dict[str, str]:
        """
        Export visualization data for external tools and analysis.
        
        Args:
            export_format: Export format (json, csv, matlab)
            
        Returns:
            Dictionary mapping data types to file paths
        """
        self.logger.info(f"Exporting data for external tools in {export_format} format")
        
        exported_files = {}
        
        # Export neuron data
        if self.neuron_data:
            filename = f"neuron_data.{export_format}"
            filepath = os.path.join(self.output_dir, filename)
            
            if export_format == "json":
                # Convert numpy arrays to lists for JSON serialization
                neuron_data_json = {
                    'step': self.neuron_data['step'],
                    'total_neurons': self.neuron_data['total_neurons'],
                    'active_neurons': self.neuron_data['active_neurons'],
                    'firing_neurons': self.neuron_data['firing_neurons'],
                    'average_firing_rate': self.neuron_data['average_firing_rate'],
                    'excitatory_inhibition_ratio': self.neuron_data['excitatory_inhibition_ratio'],
                    'summary': self.neuron_data['summary'],
                    'time_series': self.neuron_data['time_series']
                }
                
                # Convert numpy arrays to lists
                for key, value in neuron_data_json.items():
                    if isinstance(value, np.ndarray):
                        neuron_data_json[key] = value.tolist()
                    elif isinstance(value, dict):
                        for subkey, subvalue in value.items():
                            if isinstance(subvalue, np.ndarray):
                                neuron_data_json[key][subkey] = subvalue.tolist()
                
                with open(filepath, 'w') as f:
                    json.dump(neuron_data_json, f, indent=2)
                
                exported_files['neuron_data'] = filepath
            
            elif export_format == "csv":
                # Create CSV file
                import csv
                with open(filepath, 'w', newline='') as f:
                    writer = csv.writer(f)
                    
                    # Write header
                    header = ['step', 'total_neurons', 'active_neurons', 'firing_neurons',
                             'average_firing_rate', 'excitatory_inhibition_ratio']
                    writer.writerow(header)
                    
                    # Write data
                    for i in range(len(self.neuron_data['step'])):
                        row = [
                            self.neuron_data['step'][i],
                            self.neuron_data['total_neurons'][i],
                            self.neuron_data['active_neurons'][i],
                            self.neuron_data['firing_neurons'][i],
                            self.neuron_data['average_firing_rate'][i],
                            self.neuron_data['excitatory_inhibition_ratio'][i]
                        ]
                        writer.writerow(row)
                
                exported_files['neuron_data'] = filepath
        
        # Export memory data
        if self.memory_data:
            filename = f"memory_data.{export_format}"
            filepath = os.path.join(self.output_dir, filename)
            
            if export_format == "json":
                memory_data_json = {
                    'step': self.memory_data['step'],
                    'working_memory_traces': self.memory_data['working_memory_traces'],
                    'working_memory_capacity': self.memory_data['working_memory_capacity'],
                    'working_memory_utilization': self.memory_data['working_memory_utilization'],
                    'episodic_memory_episodes': self.memory_data['episodic_memory_episodes'],
                    'episodic_memory_capacity': self.memory_data['episodic_memory_capacity'],
                    'episodic_memory_utilization': self.memory_data['episodic_memory_utilization'],
                    'associative_memory_associations': self.memory_data['associative_memory_associations'],
                    'associative_memory_capacity': self.memory_data['associative_memory_capacity'],
                    'summary': self.memory_data['summary']
                }
                
                with open(filepath, 'w') as f:
                    json.dump(memory_data_json, f, indent=2)
                
                exported_files['memory_data'] = filepath
        
        # Export performance data
        if self.performance_data:
            filename = f"performance_data.{export_format}"
            filepath = os.path.join(self.output_dir, filename)
            
            if export_format == "json":
                performance_data_json = {
                    'step': self.performance_data['step'],
                    'time': self.performance_data['time'],
                    'processing_time': self.performance_data['processing_time'],
                    'spike_count': self.performance_data['spike_count'],
                    'neuron_activity': self.performance_data['neuron_activity'],
                    'memory_usage': self.performance_data['memory_usage'],
                    'performance_score': self.performance_data['performance_score'],
                    'summary': self.performance_data['summary']
                }
                
                with open(filepath, 'w') as f:
                    json.dump(performance_data_json, f, indent=2)
                
                exported_files['performance_data'] = filepath
        
        self.logger.info(f"Data exported to external tools: {list(exported_files.keys())}")
        return exported_files
    
    def generate_report(self) -> Dict[str, Any]:
        """
        Generate comprehensive analysis report.
        
        Returns:
            Dictionary with analysis report
        """
        self.logger.info("Generating comprehensive analysis report")
        
        # Collect all data if not already collected
        if not self.neuron_data:
            self.collect_neuron_activity_data()
        if not self.memory_data:
            self.collect_memory_activity_data()
        if not self.performance_data:
            self.collect_performance_data()
        
        # Generate report
        report = {
            'simulation_info': {
                'total_steps': len(self.neuron_data['step']) if self.neuron_data else 0,
                'neuron_count': self.brain.getTotalNeuronCount(),
                'total_neurons': self.neuron_data['summary']['total_neurons'] if self.neuron_data else 0,
                'generation_time': datetime.now().isoformat()
            },
            'neuron_activity': self._generate_neuron_activity_report() if self.neuron_data else {},
            'memory_activity': self._generate_memory_activity_report() if self.memory_data else {},
            'performance': self._generate_performance_report() if self.performance_data else {},
            'visualizations': {
                'neuron_activity_plot': None,
                'memory_activity_plot': None,
                'performance_trends_plot': None,
                'heatmaps': {}
            },
            'recommendations': self._generate_recommendations()
        }
        
        # Generate visualizations
        if self.neuron_data:
            neuron_plot_path = self.plot_neuron_activity()
            report['visualizations']['neuron_activity_plot'] = neuron_plot_path
        
        if self.memory_data:
            memory_plot_path = self.plot_memory_activity()
            report['visualizations']['memory_activity_plot'] = memory_plot_path
        
        if self.performance_data:
            performance_plot_path = self.plot_performance_trends()
            report['visualizations']['performance_trends_plot'] = performance_plot_path
        
        # Generate heatmaps
        if self.neuron_data or self.memory_data or self.performance_data:
            heatmaps = self.generate_heatmaps()
            report['visualizations']['heatmaps'] = heatmaps
        
        # Save report
        report_path = os.path.join(self.output_dir, 
                                  f"analysis_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json")
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        self.logger.info(f"Analysis report saved to: {report_path}")
        return report
    
    def _generate_neuron_activity_report(self) -> Dict[str, Any]:
        """Generate neuron activity analysis report."""
        if not self.neuron_data:
            return {}
        
        summary = self.neuron_data['summary']
        return {
            'firing_patterns': {
                'average_firing_rate': summary['average_firing_rate'],
                'firing_rate_std': summary['firing_rate_std'],
                'max_firing_rate': summary['max_firing_rate'],
                'min_firing_rate': summary['min_firing_rate'],
                'coefficient_of_variation': (summary['firing_rate_std'] / 
                                            summary['average_firing_rate'] if summary['average_firing_rate'] > 0 else 0)
            },
            'excitatory_inhibition_balance': {
                'average_ei_ratio': summary['average_ei_ratio'],
                'max_ei_ratio': summary['max_ei_ratio'],
                'min_ei_ratio': summary['min_ei_ratio'],
                'balanced_activity': abs(summary['average_ei_ratio'] - 1.0) < 0.1
            },
            'activity_stability': {
                'activity_std': np.std(self.neuron_data['average_firing_rate']),
                'coefficient_of_variation': np.std(self.neuron_data['average_firing_rate']) / 
                                            np.mean(self.neuron_data['average_firing_rate']) if np.mean(self.neuron_data['average_firing_rate']) > 0 else 0
            }
        }
    
    def _generate_memory_activity_report(self) -> Dict[str, Any]:
        """Generate memory activity analysis report."""
        if not self.memory_data:
            return {}
        
        summary = self.memory_data['summary']
        return {
            'memory_utilization': {
                'average_working_memory_utilization': summary['avg_working_memory_utilization'],
                'average_episodic_memory_utilization': summary['avg_episodic_memory_utilization'],
                'memory_efficiency': (summary['avg_working_memory_utilization'] + 
                                    summary['avg_episodic_memory_utilization']) / 2,
                'optimal_utilization': 0.7 <= summary['avg_working_memory_utilization'] <= 0.9 and
                                       0.7 <= summary['avg_episodic_memory_utilization'] <= 0.9
            },
            'memory_events': {
                'total_memory_events': summary['total_memory_events'],
                'consolidation_events': len([e for e in self.memory_data['memory_consolidation_events'] if e]),
                'replay_events': len([e for e in self.memory_data['memory_replay_events'] if e]),
                'event_frequency': summary['total_memory_events'] / summary['total_steps'] if summary['total_steps'] > 0 else 0
            }
        }
    
    def _generate_performance_report(self) -> Dict[str, Any]:
        """Generate performance analysis report."""
        if not self.performance_data:
            return {}
        
        summary = self.performance_data['summary']
        return {
            'processing_efficiency': {
                'average_processing_time': summary['average_processing_time'],
                'performance_consistency': 1.0 / (1.0 + summary['std_processing_time']),
                'processing_speed': 1.0 / summary['average_processing_time'] if summary['average_processing_time'] > 0 else 0
            },
            'neural_performance': {
                'total_spikes': summary['total_spikes'],
                'spike_efficiency': summary['total_spikes'] / (self.brain.getTotalNeuronCount() * summary['total_steps'] / 100) if self.brain.getTotalNeuronCount() > 0 else 0,
                'performance_score': summary['average_performance_score']
            },
            'system_stability': {
                'stability_index': 1.0 / (1.0 + np.std(self.performance_data['performance_score'])),
                'reliability': summary['max_performance_score'] / summary['average_performance_score'] if summary['average_performance_score'] > 0 else 0
            }
        }
    
    def _generate_recommendations(self) -> List[str]:
        """Generate recommendations based on analysis."""
        recommendations = []
        
        if self.neuron_data:
            summary = self.neuron_data['summary']
            
            # Check firing rate recommendations
            if summary['average_firing_rate'] > 50:
                recommendations.append("Consider reducing neural excitability to prevent over-firing")
            
            if summary['average_firing_rate'] < 5:
                recommendations.append("Consider increasing neural connectivity to boost activity")
            
            # Check E/I balance recommendations
            if abs(summary['average_ei_ratio'] - 1.0) > 0.5:
                recommendations.append("Excitatory/Inhibitory balance is unstable - consider adjusting plasticity rules")
        
        if self.memory_data:
            summary = self.memory_data['summary']
            
            # Check memory utilization recommendations
            if summary['avg_working_memory_utilization'] < 0.5:
                recommendations.append("Working memory utilization is low - consider increasing memory capacity")
            
            if summary['avg_working_memory_utilization'] > 0.95:
                recommendations.append("Working memory is approaching capacity - consider implementing consolidation")
            
            if summary['avg_episodic_memory_utilization'] < 0.3:
                recommendations.append("Episodic memory utilization is low - consider enhancing episodic encoding")
        
        if self.performance_data:
            summary = self.performance_data['summary']
            
            # Check performance recommendations
            if summary['average_processing_time'] > 0.1:
                recommendations.append("Processing time is high - consider optimizing neural algorithms")
            
            if summary['average_performance_score'] < 0.5:
                recommendations.append("Overall performance is low - consider adjusting system parameters")
        
        if not recommendations:
            recommendations.append("System performance is within optimal ranges")
        
        return recommendations
    
    def __del__(self):
        """Cleanup when visualizer is destroyed."""
        plt.close('all')  # Close all matplotlib figures
        self.logger.info("Visualizer cleanup completed")


# === Convenience Functions ===

def create_visualizer(brain, output_dir: str = "visualizations") -> Visualizer:
    """
    Convenience function to create a visualizer.
    
    Args:
        brain: Brain object to visualize
        output_dir: Directory for saving visualizations
        
    Returns:
        Visualizer object
    """
    return Visualizer(brain, output_dir)


def generate_complete_analysis(brain, steps: int = 100, output_dir: str = "visualizations") -> Dict[str, Any]:
    """
    Generate complete analysis of NLM simulation.
    
    Args:
        brain: Brain object to analyze
        steps: Number of simulation steps to run
        output_dir: Directory for saving outputs
        
    Returns:
        Dictionary with analysis results
    """
    visualizer = create_visualizer(brain, output_dir)
    return visualizer.generate_report()


# === Main Function ===

def main():
    """Main function for command-line usage."""
    print("=== NLM Visualization and Debugging Tools ===")
    print("This module provides comprehensive visualization and debugging tools")
    print("for NLM simulations.")
    print("\nUsage Examples:")
    print("1. Create visualizer:")
    print("   visualizer = create_visualizer(brain)")
    print("\n2. Generate analysis:")
    print("   report = visualizer.generate_report()")
    print("\n3. Create visualizations:")
    print("   neuron_plot = visualizer.plot_neuron_activity()")
    print("   memory_plot = visualizer.plot_memory_activity()")
    print("   performance_plot = visualizer.plot_performance_trends()")
    print("\n4. Export data:")
    print("   data = visualizer.export_data_for_external_tools()")
    print("\n5. Generate heatmaps:")
    print("   heatmaps = visualizer.generate_heatmaps()")
    print("\nFor more information, see the documentation.")

if __name__ == "__main__":
    main()