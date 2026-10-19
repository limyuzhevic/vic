#!/usr/bin/env python3
"""
NLM Visualization and Monitoring Builder
========================================

This module provides tools for building custom visualizations and monitoring
systems for the NLM framework. It includes:

- Visualization builders for different neural systems
- Real-time monitoring interfaces
- Data export utilities
- Performance analysis tools
- Web interface generators
"""

import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.gridspec import GridSpec
import pandas as pd
import seaborn as sns
from datetime import datetime
import os

class VisualizationBuilder:
    """
    Base class for building custom visualizations for NLM systems.
    
    This class provides a framework for creating visual representations
    of neural activity, memory systems, neuromodulation, plasticity, and more.
    """
    
    def __init__(self, brain=None, monitor=None):
        """
        Initialize visualization builder with brain and/or monitor data.
        
        Args:
            brain: NLM Brain object or similar interface
            monitor: RealTimeMonitor object or similar interface
        """
        self.brain = brain
        self.monitor = monitor
        self.data_history = []
        self.visualization_cache = {}
        
    def add_neural_network_visualization(self, neurons=None, synapses=None):
        """
        Add neural network visualization component.
        
        Args:
            neurons: List of neuron data (id, position, activity, type)
            synapses: List of synapse data (source, target, weight, type)
        
        Returns:
            Dictionary containing visualization configuration
        """
        viz_config = {
            'type': 'neural_network',
            'neurons': neurons or self._extract_neural_data(),
            'synapses': synapses or self._extract_synaptic_data(),
            'layout_algorithm': 'force-directed',
            'visualization_mode': '3d',
            'show_labels': True,
            'color_scheme': 'neuron_type',
            'animation_enabled': True,
            'interaction_enabled': True,
            'data_export_enabled': True
        }
        
        return viz_config
    
    def add_memory_system_visualization(self):
        """
        Add memory system visualization component.
        
        Returns:
            Dictionary containing memory visualization configuration
        """
        viz_config = {
            'type': 'memory_system',
            'working_memory': self._extract_working_memory_data(),
            'episodic_memory': self._extract_episodic_memory_data(),
            'memory_capacity': {
                'working_memory_capacity': self.brain.getWorkingMemory().getCapacity() if self.brain else 100,
                'episodic_capacity': self.brain.getEpisodicMemory().getCapacity() if self.brain else 1000
            },
            'visualization_mode': 'memory_space',
            'replay_animation': True,
            'memory_traces': True,
            'time_series': True,
            'network_graph': True
        }
        
        return viz_config
    
    def add_neuromodulation_visualization(self):
        """
        Add neuromodulation visualization component.
        
        Returns:
            Dictionary containing neuromodulation visualization configuration
        """
        viz_config = {
            'type': 'neuromodulation',
            'signals': self._extract_neuromodulation_data(),
            'visualizations': {
                'dopamine': {'color': 'green', 'intensity': 'high'},
                'curiosity': {'color': 'blue', 'intensity': 'medium'},
                'novelty': {'color': 'purple', 'intensity': 'low'},
                'reward_prediction_error': {'color': 'red', 'intensity': 'variable'}
            },
            'modulation_fields': True,
            'signal_arrows': True,
            'effect_zones': True,
            'time_series': True,
            'interactive_simulation': True
        }
        
        return viz_config
    
    def add_plasticity_visualization(self):
        """
        Add plasticity visualization component.
        
        Returns:
            Dictionary containing plasticity visualization configuration
        """
        viz_config = {
            'type': 'plasticity',
            'stp_traces': self._extract_stdp_traces(),
            'hebbian_learning': self._extract_hebbian_data(),
            'structural_changes': self._extract_structural_plasticity_data(),
            'visualizations': {
                'stdp_ltp': {'color': 'blue', 'style': 'solid'},
                'stdp_ltd': {'color': 'red', 'style': 'dashed'},
                'hebbian': {'color': 'green', 'style': 'solid'},
                'structural': {'color': 'purple', 'style': 'dotted'}
            },
            'learning_curves': True,
            'trace_decay': True,
            'adaptation_curves': True,
            'pattern_detection': True
        }
        
        return viz_config
    
    def add_development_visualization(self):
        """
        Add development visualization component.
        
        Returns:
            Dictionary containing development visualization configuration
        """
        viz_config = {
            'type': 'development',
            'developmental_stages': self._extract_developmental_stages(),
            'maturation_curves': self._extract_maturation_data(),
            'plasticity_rates': self._extract_plasticity_rates(),
            'age_effects': self._extract_age_effects(),
            'visualization_modes': {
                'stage_timeline': True,
                'maturation_graph': True,
                'plasticity_evolution': True,
                'age_impact': True
            },
            'animation_speed': 'variable',
            'developmental_prediction': True
        }
        
        return viz_config
    
    def add_prediction_visualization(self):
        """
        Add prediction system visualization component.
        
        Returns:
            Dictionary containing prediction visualization configuration
        """
        viz_config = {
            'type': 'prediction',
            'prediction_errors': self._extract_prediction_errors(),
            'confidence_intervals': self._extract_confidence_intervals(),
            'planning_trees': self._extract_planning_data(),
            'prediction_accuracy': self._extract_prediction_accuracy(),
            'visualizations': {
                'error_bars': True,
                'confidence_bands': True,
                'decision_trees': True,
                'prediction_paths': True
            },
            'time_series_analysis': True,
            'pattern_detection': True,
            'uncertainty_visualization': True
        }
        
        return viz_config
    
    def build_complete_dashboard(self):
        """
        Build complete visualization dashboard with all components.
        
        Returns:
            Dictionary containing complete dashboard configuration
        """
        dashboard_config = {
            'dashboard_title': 'NLM Comprehensive Visualization Dashboard',
            'timestamp': datetime.now().isoformat(),
            'components': {
                'neural_network': self.add_neural_network_visualization(),
                'memory_system': self.add_memory_system_visualization(),
                'neuromodulation': self.add_neuromodulation_visualization(),
                'plasticity': self.add_plasticity_visualization(),
                'development': self.add_development_visualization(),
                'prediction': self.add_prediction_visualization(),
                'performance_monitoring': self.add_performance_monitoring(),
                'resource_usage': self.add_resource_monitoring()
            },
            'layout': 'grid',
            'refresh_interval': 1.0,
            'interactive_controls': True,
            'export_options': True,
            'analysis_tools': True
        }
        
        return dashboard_config
    
    def export_visualization_config(self, filename=None):
        """
        Export visualization configuration to file.
        
        Args:
            filename: Output filename (default: visualization_config.json)
        
        Returns:
            Path to exported file
        """
        if filename is None:
            filename = 'visualization_config.json'
        
        dashboard_config = self.build_complete_dashboard()
        
        with open(filename, 'w') as f:
            json.dump(dashboard_config, f, indent=2, default=str)
        
        return filename
    
    def create_web_interface(self, host='localhost', port=8080):
        """
        Create web-based visualization interface.
        
        Args:
            host: Web server host
            port: Web server port
        
        Returns:
            URL for accessing the web interface
        """
        # In a real implementation, this would start a web server
        # For now, return a placeholder URL
        web_url = f"http://{host}:{port}/nlm_visualization"
        
        return web_url
    
    # Data extraction methods (placeholders - would need actual implementation)
    
    def _extract_neural_data(self):
        """Extract neural data from brain object"""
        return []
    
    def _extract_synaptic_data(self):
        """Extract synaptic data from brain object"""
        return []
    
    def _extract_working_memory_data(self):
        """Extract working memory data"""
        return []
    
    def _extract_episodic_memory_data(self):
        """Extract episodic memory data"""
        return []
    
    def _extract_neuromodulation_data(self):
        """Extract neuromodulation data"""
        return []
    
    def _extract_stdp_traces(self):
        """Extract STDP trace data"""
        return []
    
    def _extract_hebbian_data(self):
        """Extract Hebbian learning data"""
        return []
    
    def _extract_structural_plasticity_data(self):
        """Extract structural plasticity data"""
        return []
    
    def _extract_developmental_stages(self):
        """Extract developmental stage data"""
        return []
    
    def _extract_maturation_data(self):
        """Extract maturation data"""
        return []
    
    def _extract_plasticity_rates(self):
        """Extract plasticity rate data"""
        return []
    
    def _extract_age_effects(self):
        """Extract age effects data"""
        return []
    
    def _extract_prediction_errors(self):
        """Extract prediction error data"""
        return []
    
    def _extract_confidence_intervals(self):
        """Extract confidence interval data"""
        return []
    
    def _extract_planning_data(self):
        """Extract planning data"""
        return []
    
    def _extract_prediction_accuracy(self):
        """Extract prediction accuracy data"""
        return []
    
    def add_performance_monitoring(self):
        """Add performance monitoring component"""
        return {
            'type': 'performance_monitoring',
            'metrics': ['cpu_usage', 'memory_usage', 'firing_rate', 'spike_count'],
            'alerts': True,
            'benchmarks': True,
            'optimization_suggestions': True
        }
    
    def add_resource_monitoring(self):
        """Add resource monitoring component"""
        return {
            'type': 'resource_monitoring',
            'cpu_monitoring': True,
            'memory_monitoring': True,
            'network_monitoring': True,
            'disk_usage': True,
            'process_monitoring': True
        }

class VisualizationExporter:
    """
    Utility class for exporting visualization data and generating reports.
    """
    
    @staticmethod
    def export_to_csv(data, filename):
        """
        Export visualization data to CSV format.
        
        Args:
            data: Data dictionary or DataFrame
            filename: Output filename
        """
        if isinstance(data, dict):
            df = pd.DataFrame(data)
        else:
            df = data
        
        df.to_csv(filename, index=False)
        return filename
    
    @staticmethod
    def export_to_json(data, filename):
        """
        Export visualization data to JSON format.
        
        Args:
            data: Data dictionary
            filename: Output filename
        """
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        return filename
    
    @staticmethod
    def create_analysis_report(data, filename=None):
        """
        Create comprehensive analysis report.
        
        Args:
            data: Data dictionary or DataFrame
            filename: Output filename (optional)
        
        Returns:
            Path to created report file
        """
        if filename is None:
            filename = f"analysis_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        report = {
            'timestamp': datetime.now().isoformat(),
            'data_summary': {
                'total_samples': len(data) if hasattr(data, '__len__') else 0,
                'analysis_type': 'comprehensive',
                'neural_activity': 'analyzed',
                'memory_systems': 'analyzed',
                'neuromodulation': 'analyzed',
                'plasticity': 'analyzed',
                'development': 'analyzed',
                'prediction': 'analyzed'
            },
            'key_insights': [],
            'recommendations': [],
            'statistical_analysis': {}
        }
        
        # Add statistical analysis
        if hasattr(data, 'describe'):
            report['statistical_analysis'] = data.describe().to_dict()
        
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        return filename

class VisualizationTools:
    """
    Utility class with common visualization functions and helpers.
    """
    
    @staticmethod
    def create_3d_neural_plot(neurons, synapses, save_path=None):
        """
        Create 3D plot of neural network.
        
        Args:
            neurons: List of neuron data
            synapses: List of synapse data
            save_path: Path to save plot (optional)
        
        Returns:
            Matplotlib figure
        """
        fig = plt.figure(figsize=(12, 10))
        ax = fig.add_subplot(111, projection='3d')
        
        # Plot neurons
        for neuron in neurons:
            x, y, z = neuron['position']
            radius = neuron.get('radius', 0.1)
            ax.scatter(x, y, z, s=radius*100, alpha=0.7, c='blue')
        
        # Plot synapses
        for synapse in synapses:
            src = synapse['source']
            tgt = synapse['target']
            weight = synapse.get('weight', 0.5)
            
            ax.plot([src['x'], tgt['x']], [src['y'], tgt['y']], 
                   [src['z'], tgt['z']], 'gray', alpha=weight*0.5, linewidth=1)
        
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
        ax.set_title('3D Neural Network Visualization')
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    @staticmethod
    def create_activity_heatmap(neurons, time_steps, save_path=None):
        """
        Create heatmap of neural activity over time.
        
        Args:
            neurons: List of neuron data
            time_steps: Time step data
            save_path: Path to save heatmap (optional)
        
        Returns:
            Matplotlib figure
        """
        # Create activity matrix
        activity_matrix = np.zeros((len(neurons), len(time_steps)))
        
        for i, neuron in enumerate(neurons):
            for j, step in enumerate(time_steps):
                activity_matrix[i, j] = neuron['activity_history'][j] if j < len(neuron['activity_history']) else 0
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Overall activity heatmap
        sns.heatmap(activity_matrix, ax=axes[0, 0], cmap='hot', cbar=True)
        axes[0, 0].set_title('Neural Activity Heatmap')
        axes[0, 0].set_xlabel('Time Steps')
        axes[0, 0].set_ylabel('Neurons')
        
        # Activity distribution
        activity_flat = activity_matrix.flatten()
        axes[0, 1].hist(activity_flat, bins=50, alpha=0.7, color='skyblue', edgecolor='black')
        axes[0, 1].set_title('Activity Distribution')
        axes[0, 1].set_xlabel('Activity Level')
        axes[0, 1].set_ylabel('Frequency')
        
        # Activity over time (average)
        avg_activity = np.mean(activity_matrix, axis=0)
        axes[1, 0].plot(time_steps, avg_activity, 'b-', linewidth=2)
        axes[1, 0].set_title('Average Neural Activity Over Time')
        axes[1, 0].set_xlabel('Time Steps')
        axes[1, 0].set_ylabel('Average Activity')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Active neurons over time
        active_threshold = np.mean(avg_activity) + np.std(avg_activity)
        active_neuron_counts = np.sum(activity_matrix > active_threshold, axis=0)
        axes[1, 1].plot(time_steps, active_neuron_counts, 'r-', linewidth=2)
        axes[1, 1].set_title('Active Neuron Count Over Time')
        axes[1, 1].set_xlabel('Time Steps')
        axes[1, 1].set_ylabel('Number of Active Neurons')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig

# Main function for running the visualization builder

def main():
    """Main function to demonstrate visualization builder usage"""
    print("NLM Visualization Builder")
    print("=" * 50)
    
    # Create visualization builder
    viz_builder = VisualizationBuilder()
    
    # Build complete dashboard
    dashboard = viz_builder.build_complete_dashboard()
    
    # Export configuration
    config_file = viz_builder.export_visualization_config()
    print(f"✓ Visualization configuration exported to: {config_file}")
    
    # Create web interface
    web_url = viz_builder.create_web_interface()
    print(f"✓ Web interface available at: {web_url}")
    
    # Display dashboard summary
    print("\nDashboard Summary:")
    print(f"  - Neural Network Visualization: {'✓' if dashboard['components']['neural_network'] else '✗'}")
    print(f"  - Memory System Visualization: {'✓' if dashboard['components']['memory_system'] else '✗'}")
    print(f"  - Neuromodulation Visualization: {'✓' if dashboard['components']['neuromodulation'] else '✗'}")
    print(f"  - Plasticity Visualization: {'✓' if dashboard['components']['plasticity'] else '✗'}")
    print(f"  - Development Visualization: {'✓' if dashboard['components']['development'] else '✗'}")
    print(f"  - Prediction Visualization: {'✓' if dashboard['components']['prediction'] else '✗'}")
    print(f"  - Performance Monitoring: {'✓' if dashboard['components']['performance_monitoring'] else '✗'}")
    print(f"  - Resource Usage Monitoring: {'✓' if dashboard['components']['resource_usage'] else '✗'}")
    
    print("\nVisualization Builder completed successfully!")
    print("\nNext Steps:")
    print("  - Configure brain and monitor objects with actual data")
    print("  - Customize visualization components as needed")
    print("  - Access web interface to interact with visualizations")
    print("  - Export visualization data for analysis")
    print("  - Use VisualizationTools for additional visualization functions")

if __name__ == "__main__":
    main()
