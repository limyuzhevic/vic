"""Visualization tools for NLM.

This module provides visualization tools for the NLM (Neural Learning Machine)
framework for analyzing and visualizing brain activity, simulation results,
and agent behavior. It builds upon matplotlib and other visualization
libraries to provide comprehensive visualization capabilities.

Key features:
- Firing dynamics plotting
- Data export/import (JSON, CSV)
- Brain activity visualization
- Simulation result visualization
- Configuration visualization
- Interactive plots and analysis

Usage examples:

    from pynlm.enhanced.visualization import VisualizationTools

    # Plot firing dynamics
    VisualizationTools.plot_firing_dynamics(brain._state_history)
    
    # Export data to JSON
    VisualizationTools.export_to_json(brain._state_history, "brain_history.json")
    
    # Export data to CSV
    VisualizationTools.export_to_csv(brain._state_history, "brain_data.csv")
    
    # Create brain activity visualization
    brain_viz = VisualizationTools.create_brain_activity_plot(brain)
    brain_viz.show()

The visualization module includes:

1. VisualizationTools class - Comprehensive visualization tools
2. Plot creation functions for various data types
3. Data export/import functions
4. Brain activity visualization
5. Simulation result visualization
6. Configuration visualization
7. Interactive plotting capabilities
"""

from typing import Dict, Any, Optional, List, Union
import json
import csv
from pathlib import Path

# Import matplotlib for plotting
try:
    import matplotlib.pyplot as plt
    import matplotlib
    matplotlib_available = True
except ImportError:
    matplotlib_available = False
    plt = None

# Import numpy for data processing
try:
    import numpy as np
    numpy_available = True
except ImportError:
    numpy_available = False
    np = None

# Import seaborn for enhanced plots
try:
    import seaborn as sns
    seaborn_available = True
except ImportError:
    seaborn_available = False
    sns = None

class VisualizationTools:
    """Tools for visualizing NLM data and results.
    
    The VisualizationTools class provides comprehensive visualization
    capabilities for NLM simulations, including brain activity plotting,
    simulation result visualization, and data export/import.
    
    Example:
        from pynlm.enhanced.visualization import VisualizationTools
        
        # Plot firing dynamics
        VisualizationTools.plot_firing_dynamics(brain._state_history)
        
        # Export data
        VisualizationTools.export_to_json(brain._state_history, "brain_history.json")
    """
    
    @staticmethod
    def check_matplotlib() -> bool:
        """Check if matplotlib is available.
        
        Args:
            
        Returns:
            True if matplotlib is available, False otherwise
        """
        if not matplotlib_available:
            raise ImportError(
                "matplotlib not installed. Install with: pip install matplotlib\n"
                "Or use alternative visualization methods."
            )
        return True
    
    @staticmethod
    def check_numpy() -> bool:
        """Check if numpy is available.
        
        Args:
            
        Returns:
            True if numpy is available, False otherwise
        """
        if not numpy_available:
            raise ImportError(
                "numpy not installed. Install with: pip install numpy\n"
                "Or use alternative visualization methods."
            )
        return True
    
    @staticmethod
    def check_seaborn() -> bool:
        """Check if seaborn is available.
        
        Args:
            
        Returns:
            True if seaborn is available, False otherwise
        """
        if not seaborn_available:
            raise ImportError(
                "seaborn not installed. Install with: pip install seaborn\n"
                "Or use alternative visualization methods."
            )
        return True
    
    @staticmethod
    def plot_firing_dynamics(history: List[Dict[str, Any]], 
                            output_file: Optional[str] = None,
                            show: bool = True,
                            figsize: tuple = (10, 8)) -> Optional[plt.Figure]:
        """Plot firing dynamics over time.
        
        Args:
            history: Brain state history
            output_file: Optional output file path
            show: Whether to display the plot (default: True)
            figsize: Figure size (width, height) (default: (10, 8))
            
        Returns:
            matplotlib Figure object
            
        Raises:
            ImportError: If matplotlib is not available
            ValueError: If history is empty
        """
        VisualizationTools.check_matplotlib()
        
        if not history:
            raise ValueError("History is empty")
        
        steps = [s['step'] for s in history]
        active_neurons = [s.get('active_neurons', 0) for s in history]
        firing_rates = [s.get('average_firing_rate', 0) for s in history]
        total_spikes = [s.get('total_spikes', 0) for s in history]
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=figsize)
        
        # Active neurons over time
        ax1.plot(steps, active_neurons, 'b-', linewidth=2, label='Active Neurons')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Active Neurons', color='b')
        ax1.set_title('Active Neurons Over Time')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Firing rate over time
        ax2.plot(steps, firing_rates, 'r-', linewidth=2, label='Average Firing Rate')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Firing Rate', color='r')
        ax2.set_title('Average Firing Rate Over Time')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        # Total spikes over time
        ax3.plot(steps, total_spikes, 'g-', linewidth=2, label='Total Spikes')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Total Spikes', color='g')
        ax3.set_title('Total Spikes Over Time')
        ax3.legend()
        ax3.grid(True, alpha=0.3)
        
        # Firing rate distribution
        ax4.hist(firing_rates, bins=20, alpha=0.7, color='purple', edgecolor='black')
        ax4.set_xlabel('Firing Rate')
        ax4.set_ylabel('Frequency')
        ax4.set_title('Firing Rate Distribution')
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
        
        if show:
            plt.show()
        
        return fig
    
    @staticmethod
    def plot_neural_activity(neural_data: Dict[str, List[float]],
                            output_file: Optional[str] = None,
                            show: bool = True,
                            figsize: tuple = (12, 6)) -> Optional[plt.Figure]:
        """Plot neural activity patterns.
        
        Args:
            neural_data: Neural activity data (e.g., {'excitatory': [...], 'inhibitory': [...]})
            output_file: Optional output file path
            show: Whether to display the plot (default: True)
            figsize: Figure size (width, height) (default: (12, 6))
            
        Returns:
            matplotlib Figure object
        """
        VisualizationTools.check_matplotlib()
        
        fig, ax = plt.subplots(figsize=figsize)
        
        for activity_type, data in neural_data.items():
            ax.plot(data, label=activity_type, linewidth=2)
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Activity Level')
        ax.set_title('Neural Activity Patterns')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
        
        if show:
            plt.show()
        
        return fig
    
    @staticmethod
    def plot_brain_regions(brain_data: Dict[str, Any],
                           output_file: Optional[str] = None,
                           show: bool = True,
                           figsize: tuple = (10, 8)) -> Optional[plt.Figure]:
        """Plot brain region activity.
        
        Args:
            brain_data: Brain region data
            output_file: Optional output file path
            show: Whether to display the plot (default: True)
            figsize: Figure size (width, height) (default: (10, 8))
            
        Returns:
            matplotlib Figure object
        """
        VisualizationTools.check_matplotlib()
        
        fig, ax = plt.subplots(figsize=figsize)
        
        # Extract brain region data
        if 'regions' in brain_data:
            for region_name, region_data in brain_data['regions'].items():
                ax.bar(region_name, region_data.get('activity', 0), alpha=0.7)
        
        ax.set_xlabel('Brain Regions')
        ax.set_ylabel('Activity Level')
        ax.set_title('Brain Region Activity')
        ax.grid(True, alpha=0.3)
        ax.set_xticks(range(len(brain_data.get('regions', {}))))
        ax.set_xticklabels(list(brain_data.get('regions', {}).keys()), rotation=45)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
        
        if show:
            plt.show()
        
        return fig
    
    @staticmethod
    def plot_simulation_results(simulation_data: List[Dict[str, Any]],
                                output_file: Optional[str] = None,
                                show: bool = True,
                                figsize: tuple = (12, 8)) -> Optional[plt.Figure]:
        """Plot simulation results.
        
        Args:
            simulation_data: Simulation results data
            output_file: Optional output file path
            show: Whether to display the plot (default: True)
            figsize: Figure size (width, height) (default: (12, 8))
            
        Returns:
            matplotlib Figure object
        """
        VisualizationTools.check_matplotlib()
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=figsize)
        
        if simulation_data:
            # Extract data from simulation results
            steps = [s.get('step', i) for i, s in enumerate(simulation_data)]
            
            # Plot agent position
            ax1.plot([s.get('agent_x', 0) for s in simulation_data],
                    [s.get('agent_y', 0) for s in simulation_data], 'b-', linewidth=2)
            ax1.set_xlabel('X Position')
            ax1.set_ylabel('Y Position')
            ax1.set_title('Agent Trajectory')
            ax1.grid(True, alpha=0.3)
            ax1.axis('equal')
            
            # Plot actions
            if 'actions' in simulation_data[0]:
                action_data = [s['actions'] for s in simulation_data if 'actions' in s]
                ax2.hist(action_data, bins=len(set(action_data)), alpha=0.7)
                ax2.set_xlabel('Actions')
                ax2.set_ylabel('Frequency')
                ax2.set_title('Action Distribution')
                ax2.grid(True, alpha=0.3)
            
            # Plot rewards
            if 'rewards' in simulation_data[0]:
                rewards = [s['rewards'] for s in simulation_data if 'rewards' in s]
                ax3.plot(rewards, 'g-', linewidth=2, label='Rewards')
                ax3.set_xlabel('Step')
                ax3.set_ylabel('Reward')
                ax3.set_title('Rewards Over Time')
                ax3.legend()
                ax3.grid(True, alpha=0.3)
            
            # Plot performance metrics
            if 'performance' in simulation_data[0]:
                performance = [s['performance'] for s in simulation_data if 'performance' in s]
                ax4.plot(performance, 'r-', linewidth=2, label='Performance')
                ax4.set_xlabel('Step')
                ax4.set_ylabel('Performance')
                ax4.set_title('Performance Over Time')
                ax4.legend()
                ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
        
        if show:
            plt.show()
        
        return fig
    
    @staticmethod
    def export_to_json(data: Union[List[Dict[str, Any]], Dict[str, Any]],
                      output_file: str,
                      indent: int = 2) -> None:
        """Export data to JSON file.
        
        Args:
            data: Data to export
            output_file: Output file path
            indent: JSON indentation (default: 2)
            
        Raises:
            IOError: If file cannot be written
        """
        try:
            with open(output_file, 'w') as f:
                json.dump(data, f, indent=indent)
        except IOError as e:
            raise IOError(f"Cannot write to file {output_file}: {e}")
    
    @staticmethod
    def import_from_json(input_file: str) -> Union[List[Dict[str, Any]], Dict[str, Any]]:
        """Import data from JSON file.
        
        Args:
            input_file: Input file path
            
        Returns:
            Imported data
            
        Raises:
            IOError: If file cannot be read
            json.JSONDecodeError: If JSON is invalid
        """
        try:
            with open(input_file, 'r') as f:
                return json.load(f)
        except IOError as e:
            raise IOError(f"Cannot read file {input_file}: {e}")
    
    @staticmethod
    def export_to_csv(data: List[Dict[str, Any]],
                     output_file: str) -> None:
        """Export data to CSV file.
        
        Args:
            data: Data to export (list of dictionaries)
            output_file: Output file path
            
        Raises:
            IOError: If file cannot be written
        """
        if not data:
            raise ValueError("Data is empty")
        
        try:
            with open(output_file, 'w', newline='') as f:
                writer = csv.writer(f)
                
                # Write header
                writer.writerow(data[0].keys())
                
                # Write data
                for row in data:
                    writer.writerow(row.values())
        except IOError as e:
            raise IOError(f"Cannot write to file {output_file}: {e}")
    
    @staticmethod
    def import_from_csv(input_file: str) -> List[Dict[str, Any]]:
        """Import data from CSV file.
        
        Args:
            input_file: Input file path
            
        Returns:
            Imported data
            
        Raises:
            IOError: If file cannot be read
            csv.Error: If CSV is invalid
        """
        try:
            with open(input_file, 'r') as f:
                reader = csv.DictReader(f)
                return list(reader)
        except IOError as e:
            raise IOError(f"Cannot read file {input_file}: {e}")
    
    @staticmethod
    def create_brain_activity_plot(brain) -> Optional[plt.Figure]:
        """Create comprehensive brain activity visualization.
        
        Args:
            brain: Brain object to visualize
            
        Returns:
            matplotlib Figure object
        """
        VisualizationTools.check_matplotlib()
        
        # Check if brain has _state_history attribute
        if not hasattr(brain, '_state_history'):
            raise ValueError("Brain object does not have _state_history attribute")
        
        history = brain._state_history
        
        if not history:
            raise ValueError("Brain history is empty")
        
        return VisualizationTools.plot_firing_dynamics(
            history, 
            output_file=None,
            show=False
        )
    
    @staticmethod
    def create_simulation_plot(world, agent) -> Optional[plt.Figure]:
        """Create simulation visualization.
        
        Args:
            world: World object
            agent: Agent object
            
        Returns:
            matplotlib Figure object
        """
        VisualizationTools.check_matplotlib()
        
        # Check if world has run_simulation method
        if not hasattr(world, 'run_simulation'):
            raise ValueError("World object does not have run_simulation method")
        
        # Run simulation
        simulation_data = world.run_simulation(agent, duration=10.0, timestep=0.1)
        
        return VisualizationTools.plot_simulation_results(
            simulation_data,
            output_file=None,
            show=False
        )

# Factory functions for common visualization tasks

def create_enhanced_visualization_tools() -> VisualizationTools:
    """Create enhanced visualization tools.
    
    Args:
        
    Returns:
        VisualizationTools instance
    """
    return VisualizationTools()

# Export all public classes and functions
__all__ = [
    'VisualizationTools',
    'create_enhanced_visualization_tools',
]