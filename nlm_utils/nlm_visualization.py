"""
Visualization and plotting utilities for NLM (Neural Learning Machine)

This module provides visualization tools for NLM data analysis and plotting,
including support for matplotlib, plotext (text-based plots), and other
visualization backends.
"""

import sys
import time
import numpy as np
from typing import List, Dict, Any, Optional, Tuple, Union
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import seaborn as sns
from datetime import datetime

# Optional dependencies - gracefully handle missing imports
try:
    import plotext as pltxt
    PLOTEXT_AVAILABLE = True
except ImportError:
    PLOTEXT_AVAILABLE = False

try:
    import plotly
    import plotly.graph_objects as go
    import plotly.express as px
    PLOTLY_AVAILABLE = True
except ImportError:
    PLOTLY_AVAILABLE = False

try:
    import PIL.Image
    PIL_AVAILABLE = True
except ImportError:
    PIL_AVAILABLE = False

# Import NLM Python bindings
try:
    import pynlm
except ImportError as e:
    raise ImportError(f"NLM Python bindings not available: {e}")

class NLMDataPlotter:
    """
    Advanced data plotter for NLM simulation results.
    """
    
    def __init__(self, style: str = 'default', figsize: Tuple[int, int] = (12, 8)):
        """
        Initialize data plotter.
        
        Args:
            style: Plot style ('default', 'scientific', 'publication')
            figsize: Default figure size
        """
        self.style = style
        self.figsize = figsize
        
        # Setup plot style
        self._setup_plot_style(style)
        
    def _setup_plot_style(self, style: str):
        """Setup matplotlib plot style."""
        styles = {
            'default': {
                'font.size': 10,
                'axes.labelsize': 12,
                'axes.titlesize': 14,
                'xtick.labelsize': 10,
                'ytick.labelsize': 10,
                'legend.fontsize': 10,
                'figure.titlesize': 16
            },
            'scientific': {
                'font.size': 9,
                'axes.labelsize': 11,
                'axes.titlesize': 13,
                'xtick.labelsize': 9,
                'ytick.labelsize': 9,
                'legend.fontsize': 9,
                'figure.titlesize': 15,
                'axes.grid': True
            },
            'publication': {
                'font.size': 8,
                'axes.labelsize': 10,
                'axes.titlesize': 12,
                'xtick.labelsize': 8,
                'ytick.labelsize': 8,
                'legend.fontsize': 8,
                'figure.titlesize': 14,
                'axes.grid': True,
                'font.family': 'serif'
            }
        }
        
        if style in styles:
            plt.rcParams.update(styles[style])
            self.plot_config = styles[style]
        else:
            self.plot_config = styles['default']
    
    def plot_neuron_dynamics(self, brain: pynlm.Brain, save_path: Optional[str] = None,
                           show_plot: bool = False) -> plt.Figure:
        """
        Plot neuron dynamics over simulation steps.
        
        Args:
            brain: NLM brain instance
            save_path: Path to save plot
            show_plot: Whether to display plot
            
        Returns:
            Matplotlib figure object
        """
        if not brain.isInitialized():
            raise ValueError("Brain must be initialized")
        
        # Create simulation data
        steps = 100
        neuron_counts = []
        firing_rates = []
        spike_counts = []
        
        for step in range(steps):
            brain.step(step)
            neuron_counts.append(brain.getTotalNeuronCount())
            firing_rates.append(brain.getAverageFiringRate())
            spike_counts.append(brain.getTotalSpikeCount())
        
        # Create subplots
        fig, axes = plt.subplots(3, 1, figsize=self.figsize)
        
        # Neuron count plot
        axes[0].plot(range(steps), neuron_counts, color='blue', linewidth=2)
        axes[0].set_title('Total Neuron Count')
        axes[0].set_xlabel('Simulation Step')
        axes[0].set_ylabel('Neuron Count')
        axes[0].grid(True, alpha=0.3)
        
        # Firing rate plot
        axes[1].plot(range(steps), firing_rates, color='green', linewidth=2)
        axes[1].set_title('Average Firing Rate')
        axes[1].set_xlabel('Simulation Step')
        axes[1].set_ylabel('Firing Rate (Hz)')
        axes[1].grid(True, alpha=0.3)
        
        # Spike count plot
        axes[2].plot(range(steps), spike_counts, color='red', linewidth=2)
        axes[2].set_title('Cumulative Spike Count')
        axes[2].set_xlabel('Simulation Step')
        axes[2].set_ylabel('Total Spikes')
        axes[2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        if show_plot:
            plt.show()
        
        return fig
    
    def plot_agent_simulation(self, brain: pynlm.Brain, world: pynlm.SimpleWorld,
                            agent: pynlm.AgentBrain, num_steps: int = 100,
                            save_path: Optional[str] = None, show_plot: bool = False) -> plt.Figure:
        """
        Plot agent simulation with world interaction.
        
        Args:
            brain: NLM brain instance
            world: World simulation
            agent: Agent brain interface
            num_steps: Number of simulation steps
            save_path: Path to save plot
            show_plot: Whether to display plot
            
        Returns:
            Matplotlib figure object
        """
        # Configure world
        world.configure(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10, 10)
        
        # Initialize agent
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Collect simulation data
        time_points = []
        x_positions = []
        y_positions = []
        curiosity_levels = []
        novelty_levels = []
        rewards = []
        
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.05)
            
            # Record data
            time_points.append(world.getSimulationTime())
            x_positions.append(world.getAgentBody().x)
            y_positions.append(world.getAgentBody().y)
            curiosity_levels.append(agent.getCuriosityLevel())
            novelty_levels.append(agent.getNoveltyLevel())
            rewards.append(0.1)  # Constant reward for this simulation
        
        # Create subplots
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        
        # Position plot
        axes[0, 0].plot(x_positions, y_positions, 'b-', alpha=0.7, linewidth=2)
        axes[0, 0].plot(x_positions[0], y_positions[0], 'go', markersize=10, label='Start')
        axes[0, 0].plot(x_positions[-1], y_positions[-1], 'ro', markersize=10, label='End')
        axes[0, 0].set_title('Agent Trajectory in World')
        axes[0, 0].set_xlabel('X Position')
        axes[0, 0].set_ylabel('Y Position')
        axes[0, 0].legend()
        axes[0, 0].grid(True, alpha=0.3)
        axes[0, 0].set_aspect('equal')
        
        # Curiosity plot
        axes[0, 1].plot(time_points, curiosity_levels, 'r-', linewidth=2)
        axes[0, 1].set_title('Curiosity Level')
        axes[0, 1].set_xlabel('Time (s)')
        axes[0, 1].set_ylabel('Curiosity Level')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Novelty plot
        axes[1, 0].plot(time_points, novelty_levels, 'purple', linewidth=2)
        axes[1, 0].set_title('Novelty Level')
        axes[1, 0].set_xlabel('Time (s)')
        axes[1, 0].set_ylabel('Novelty Level')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Reward plot
        axes[1, 1].plot(time_points, rewards, 'orange', linewidth=2)
        axes[1, 1].set_title('Reward Over Time')
        axes[1, 1].set_xlabel('Time (s)')
        axes[1, 1].set_ylabel('Reward')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        if show_plot:
            plt.show()
        
        return fig
    
    def plot_brain_metrics_comparison(self, experiment_results: List[Dict[str, Any]],
                                    metric_name: str = 'total_spikes', save_path: Optional[str] = None,
                                    show_plot: bool = False) -> plt.Figure:
        """
        Plot brain metrics comparison across multiple experiments.
        
        Args:
            experiment_results: List of experiment result dictionaries
            metric_name: Name of metric to plot
            save_path: Path to save plot
            show_plot: Whether to display plot
            
        Returns:
            Matplotlib figure object
        """
        # Extract data
        experiment_names = []
        metric_values = []
        metric_stds = []
        
        for result in experiment_results:
            if metric_name in result:
                experiment_names.append(result.get('experiment_id', f'Exp {len(experiment_names) + 1}'))
                metric_values.append(result[metric_name])
                metric_stds.append(result.get(f'{metric_name}_std', 0))
        
        if not experiment_names:
            raise ValueError(f"No experiments found with metric '{metric_name}'")
        
        # Create figure
        fig, ax = plt.subplots(figsize=self.figsize)
        
        # Bar plot with error bars
        x_pos = np.arange(len(experiment_names))
        colors = plt.cm.Set3(np.linspace(0, 1, len(experiment_names)))
        
        bars = ax.bar(x_pos, metric_values, yerr=metric_stds, 
                     capsize=5, color=colors, alpha=0.8, edgecolor='black', linewidth=1)
        
        # Customize plot
        ax.set_title(f'Brain Metrics Comparison: {metric_name.replace("_", " ").title()}')
        ax.set_xlabel('Experiments')
        ax.set_ylabel(metric_name.replace('_', ' ').title())
        ax.set_xticks(x_pos)
        ax.set_xticklabels(experiment_names, rotation=45, ha='right')
        ax.grid(True, alpha=0.3)
        
        # Add value labels on bars
        for i, (bar, value) in enumerate(zip(bars, metric_values)):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height + max(metric_stds)/2,
                   f'{value:.1f}', ha='center', va='bottom', fontweight='bold')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        if show_plot:
            plt.show()
        
        return fig
    
    def plot_performance_benchmarks(self, benchmark_data: Dict[str, Any],
                                  save_path: Optional[str] = None,
                                  show_plot: bool = False) -> plt.Figure:
        """
        Plot performance benchmark results.
        
        Args:
            benchmark_data: Dictionary with benchmark data
            save_path: Path to save plot
            show_plot: Whether to display plot
            
        Returns:
            Matplotlib figure object
        """
        # Create subplots for different benchmark categories
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        fig.suptitle('NLM Performance Benchmarks', fontsize=16, fontweight='bold')
        
        # Basic operations
        if 'basic_operations' in benchmark_data:
            basic_ops = benchmark_data['basic_operations']
            ax = axes[0, 0]
            
            operations = list(basic_ops.keys())
            times = [basic_ops[op].get('mean_time', 0) for op in operations]
            
            bars = ax.bar(operations, times, color='skyblue', alpha=0.8)
            ax.set_title('Basic Operations Performance')
            ax.set_ylabel('Time (seconds)')
            ax.grid(True, alpha=0.3)
            
            # Add value labels
            for bar, time_val in zip(bars, times):
                ax.text(bar.get_x() + bar.get_width()/2., bar.get_height() + 0.001,
                       f'{time_val:.4f}', ha='center', va='bottom', fontweight='bold')
        
        # Agent-world simulation
        if 'agent_world_simulation' in benchmark_data:
            agent_profile = benchmark_data['agent_world_simulation']
            ax = axes[0, 1]
            
            # Plot episodes per second
            eps = agent_profile.get('episodes_per_second', 0)
            steps_per_sec = agent_profile.get('steps_per_second', 0)
            
            ax.bar(['Episodes/s', 'Steps/s'], [eps, steps_per_sec], 
                  color=['lightgreen', 'lightcoral'], alpha=0.8)
            ax.set_title('Agent-World Simulation Rate')
            ax.set_ylabel('Rate (per second)')
            ax.grid(True, alpha=0.3)
            
            # Add value labels
            for i, val in enumerate([eps, steps_per_sec]):
                ax.text(i, val + 0.01, f'{val:.1f}', ha='center', va='bottom', fontweight='bold')
        
        # Brain implementation comparison
        if 'brain_implementation_comparison' in benchmark_data:
            comparison = benchmark_data['brain_implementation_comparison']
            ax = axes[1, 0]
            
            best_config = comparison.get('best_configuration', {})
            steps_per_sec = best_config.get('steps_per_second', 0)
            
            # Create a radar plot for best configuration
            categories = ['Steps/s', 'Neurons', 'Synapses', 'Firing Rate']
            values = [steps_per_sec, best_config.get('neuron_count', 0),
                     best_config.get('synapse_count', 0), best_config.get('average_firing_rate', 0)]
            
            # Normalize values for radar plot
            normalized_values = [v / max(v, 1) for v in values] if max(values) > 0 else values
            
            angles = np.linspace(0, 2 * np.pi, len(categories), endpoint=False).tolist()
            normalized_values += normalized_values[:1]  # Close the circle
            angles += angles[:1]
            
            ax.plot(angles, normalized_values, 'o-', linewidth=2, color='purple')
            ax.fill(angles, normalized_values, alpha=0.25, color='purple')
            ax.set_xticks(angles[:-1])
            ax.set_xticklabels(categories)
            ax.set_title('Best Configuration Profile')
            ax.grid(True, alpha=0.3)
        
        # Time series of benchmarks
        ax = axes[1, 1]
        if 'agent_world_simulation' in benchmark_data:
            agent_profile = benchmark_data['agent_world_simulation']
            
            # Create a time series showing simulation progress
            time_points = np.linspace(0, 10, 100)
            efficiency = 0.8 + 0.2 * np.sin(time_points)  # Simulated efficiency curve
            
            ax.plot(time_points, efficiency, 'b-', linewidth=2, label='Efficiency')
            ax.fill_between(time_points, efficiency, alpha=0.3)
            ax.set_title('Simulation Efficiency Over Time')
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Efficiency')
            ax.set_ylim(0.6, 1.0)
            ax.legend()
            ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        if show_plot:
            plt.show()
        
        return fig
    
    def plot_experiment_sequence(self, experiment_results: List[Dict[str, Any]],
                                 save_path: Optional[str] = None,
                                 show_plot: bool = False) -> plt.Figure:
        """
        Plot experiment sequence results over time.
        
        Args:
            experiment_results: List of experiment result dictionaries
            save_path: Path to save plot
            show_plot: Whether to display plot
            
        Returns:
            Matplotlib figure object
        """
        if not experiment_results:
            raise ValueError("No experiment results to plot")
        
        # Create figure with multiple subplots
        fig, axes = plt.subplots(3, 1, figsize=self.figsize)
        
        # Extract data
        experiment_ids = []
        steps = []
        spikes = []
        durations = []
        firing_rates = []
        
        for result in experiment_results:
            experiment_ids.append(result.get('experiment_id', f'Exp {len(experiment_ids) + 1}'))
            steps.append(result.get('steps', 0))
            spikes.append(result.get('total_spikes', 0))
            durations.append(result.get('duration', 0))
            firing_rates.append(result.get('average_firing_rate', 0))
        
        # Main metrics bar plot
        ax = axes[0]
        x_pos = np.arange(len(experiment_ids))
        width = 0.2
        
        ax.bar(x_pos - width, steps, width, label='Steps', alpha=0.8, color='blue')
        ax.bar(x_pos, spikes, width, label='Spikes', alpha=0.8, color='red')
        ax.bar(x_pos + width, durations, width, label='Duration (s)', alpha=0.8, color='green')
        
        ax.set_title('Experiment Metrics Comparison')
        ax.set_xlabel('Experiments')
        ax.set_ylabel('Metric Value')
        ax.set_xticks(x_pos)
        ax.set_xticklabels(experiment_ids, rotation=45, ha='right')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Progress line plot
        ax = axes[1]
        cumulative_steps = np.cumsum(steps)
        ax.plot(cumulative_steps, spikes, 'o-', linewidth=2, markersize=6, color='purple')
        ax.set_title('Cumulative Steps vs Total Spikes')
        ax.set_xlabel('Cumulative Steps')
        ax.set_ylabel('Total Spikes')
        ax.grid(True, alpha=0.3)
        
        # Firing rate trend
        ax = axes[2]
        ax.plot(experiment_ids, firing_rates, 's-', linewidth=2, markersize=6, color='orange')
        ax.set_title('Average Firing Rate by Experiment')
        ax.set_xlabel('Experiments')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_xticks([])  # Hide x-axis labels for cleaner look
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        if show_plot:
            plt.show()
        
        return fig
class NLMTxtPlotter:
    """
    Text-based plotter for terminal output when graphical display is not available.
    """
    
    def __init__(self, width: int = 80):
        """
        Initialize text plotter.
        
        Args:
            width: Plot width in characters
        """
        self.width = width
        pltxt_config = pltxt.PlotextConfig()
        pltxt_config.width = width
        pltxt_config.height = 20
    
    def plot_neuron_dynamics_text(self, brain: pynlm.Brain, num_steps: int = 50,
                                 save_path: Optional[str] = None) -> str:
        """
        Plot neuron dynamics in text format.
        
        Args:
            brain: NLM brain instance
            num_steps: Number of simulation steps
            save_path: Path to save text plot
            
        Returns:
            Text representation of the plot
        """
        if not brain.isInitialized():
            raise ValueError("Brain must be initialized")
        
        # Collect data
        steps = []
        neuron_counts = []
        firing_rates = []
        
        for step in range(num_steps):
            brain.step(step)
            steps.append(step)
            neuron_counts.append(brain.getTotalNeuronCount())
            firing_rates.append(brain.getAverageFiringRate())
        
        # Clear plotext
        pltxt.clear_figure()
        
        # Set title
        pltxt.title("NLM Neuron Dynamics")
        pltxt.xlabel("Step")
        pltxt.ylabel("Neuron Count")
        
        # Plot neuron count
        pltxt.plot(steps, neuron_counts, label="Neurons")
        
        # Add second y-axis for firing rate
        pltxt.titlesize(16)
        pltxt.ylabel("Neurons / Firing Rate")
        
        # Display statistics
        pltxt.legend()
        
        # Add text information
        max_neurons = max(neuron_counts)
        avg_firing = np.mean(firing_rates)
        pltxt.text(f"Max Neurons: {max_neurons}, Avg Firing Rate: {avg_firing:.2f} Hz")
        
        # Show plot
        output = pltxt.build()
        
        if save_path:
            with open(save_path, 'w') as f:
                f.write(output)
        
        return output
    
    def plot_metrics_comparison_text(self, experiment_results: List[Dict[str, Any]],
                                   metric_name: str = 'total_spikes') -> str:
        """
        Plot metrics comparison in text format.
        
        Args:
            experiment_results: List of experiment result dictionaries
            metric_name: Name of metric to plot
            
        Returns:
            Text representation of the plot
        """
        if not experiment_results:
            raise ValueError("No experiment results to plot")
        
        # Extract data
        experiment_names = []
        metric_values = []
        
        for result in experiment_results:
            if metric_name in result:
                experiment_names.append(result.get('experiment_id', f'Exp {len(experiment_names) + 1}'))
                metric_values.append(result[metric_name])
        
        # Clear plotext
        pltxt.clear_figure()
        
        # Set title
        pltxt.title(f"Experiment Metrics: {metric_name.replace('_', ' ').title()}")
        pltxt.xlabel("Experiments")
        pltxt.ylabel("Value")
        
        # Create bar chart
        x_pos = np.arange(len(experiment_names))
        
        for i, (name, value) in enumerate(zip(experiment_names, metric_values)):
            # Scale value for display
            display_value = min(value / max(metric_values) * 50, 50) if metric_values else 0
            bar = '=' * int(display_value)
            pltxt.text(f"{name:20} {bar} {value:.1f}")
        
        # Add summary
        pltxt.text(f"\nTotal Experiments: {len(experiment_names)}")
        pltxt.text(f"Average {metric_name}: {np.mean(metric_values):.1f}")
        pltxt.text(f"Max {metric_name}: {np.max(metric_values):.1f}")
        
        return pltxt.build()
class NLMPlotlyVisualizer:
    """
    Interactive plotly visualization for NLM data.
    """
    
    def __init__(self):
        """Initialize plotly visualizer."""
        self.figures = {}
    
    def create_interactive_neural_plot(self, brain: pynlm.Brain, num_steps: int = 100) -> go.Figure:
        """
        Create interactive neural plot.
        
        Args:
            brain: NLM brain instance
            num_steps: Number of simulation steps
            
        Returns:
            Plotly figure object
        """
        if not brain.isInitialized():
            raise ValueError("Brain must be initialized")
        
        # Collect data
        time_data = []
        neuron_counts = []
        firing_rates = []
        spike_counts = []
        
        for step in range(num_steps):
            brain.step(step)
            time_data.append(step)
            neuron_counts.append(brain.getTotalNeuronCount())
            firing_rates.append(brain.getAverageFiringRate())
            spike_counts.append(brain.getTotalSpikeCount())
        
        # Create traces
        fig = go.Figure()
        
        # Neuron count trace
        fig.add_trace(go.Scatter(
            x=time_data,
            y=neuron_counts,
            mode='lines',
            name='Neuron Count',
            line=dict(color='blue', width=2)
        ))
        
        # Firing rate trace (secondary axis)
        fig.add_trace(go.Scatter(
            x=time_data,
            y=firing_rates,
            mode='lines',
            name='Firing Rate',
            line=dict(color='green', width=2),
            yaxis='y2'
        ))
        
        # Spike count trace (tertiary axis)
        fig.add_trace(go.Scatter(
            x=time_data,
            y=spike_counts,
            mode='lines',
            name='Total Spikes',
            line=dict(color='red', width=2),
            yaxis='y3'
        ))
        
        # Layout
        fig.update_layout(
            title='NLM Neural Dynamics - Interactive Plot',
            xaxis=dict(title='Simulation Step'),
            yaxis=dict(title='Neuron Count', side='left'),
            yaxis2=dict(title='Firing Rate (Hz)', side='right', overlaying='y'),
            yaxis3=dict(title='Total Spikes', side='right', overlaying='y', position=0.9),
            hovermode='x unified'
        )
        
        return fig
    
    def create_agent_trajectory_plot(self, brain: pynlm.Brain, world: pynlm.SimpleWorld,
                                    agent: pynlm.AgentBrain, num_steps: int = 50) -> go.Figure:
        """
        Create interactive agent trajectory plot.
        
        Args:
            brain: NLM brain instance
            world: World simulation
            agent: Agent brain interface
            num_steps: Number of simulation steps
            
        Returns:
            Plotly figure object
        """
        # Configure world
        world.configure(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10, 10)
        
        # Initialize agent
        agent.initialize(world)
        agent.enableRewardModulation(True)
        
        # Collect trajectory data
        x_data = []
        y_data = []
        time_data = []
        reward_data = []
        curiosity_data = []
        
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.05)
            
            # Record data
            x_data.append(world.getAgentBody().x)
            y_data.append(world.getAgentBody().y)
            time_data.append(world.getSimulationTime())
            reward_data.append(0.1)
            curiosity_data.append(agent.getCuriosityLevel())
        
        # Create scatter plot for trajectory
        fig = go.Figure()
        
        # Trajectory line
        fig.add_trace(go.Scatter(
            x=x_data,
            y=y_data,
            mode='lines+markers',
            name='Trajectory',
            line=dict(color='blue', width=3),
            marker=dict(size=6, color='blue'),
            hovertemplate='<b>Position</b><br>X: %{x}<br>Y: %{y}<extra></extra>'
        ))
        
        # Add start and end points
        fig.add_trace(go.Scatter(
            x=[x_data[0]],
            y=[y_data[0]],
            mode='markers',
            name='Start',
            marker=dict(size=12, color='green', symbol='circle'),
            hovertemplate='<b>Start Position</b><br>X: %{x}<br>Y: %{y}<extra></extra>'
        ))
        
        fig.add_trace(go.Scatter(
            x=[x_data[-1]],
            y=[y_data[-1]],
            mode='markers',
            name='End',
            marker=dict(size=12, color='red', symbol='x'),
            hovertemplate='<b>End Position</b><br>X: %{x}<br>Y: %{y}<extra></extra>'
        ))
        
        # Time series plots (secondary axis)
        fig.add_trace(go.Scatter(
            x=time_data,
            y=reward_data,
            mode='lines',
            name='Reward',
            line=dict(color='orange', width=2),
            yaxis='y2',
            hovertemplate='<b>Reward</b><br>Time: %{x}<br>Reward: %{y}<extra></extra>'
        ))
        
        fig.add_trace(go.Scatter(
            x=time_data,
            y=curiosity_data,
            mode='lines',
            name='Curiosity',
            line=dict(color='purple', width=2),
            yaxis='y3',
            hovertemplate='<b>Curiosity</b><br>Time: %{x}<br>Curiosity: %{y}<extra></extra>'
        ))
        
        # Layout
        fig.update_layout(
            title='Agent Trajectory and Metrics - Interactive Plot',
            xaxis=dict(title='X Position'),
            yaxis=dict(title='Y Position', range=[0, 20]),
            yaxis2=dict(title='Reward', side='right', overlaying='y', position=0.95),
            yaxis3=dict(title='Curiosity Level', side='right', overlaying='y', position=0.8),
            hovermode='x unified'
        )
        
        return fig
    
    def save_interactive_plot(self, fig: go.Figure, filepath: str, auto_open: bool = False):
        """
        Save interactive plot to HTML file.
        
        Args:
            fig: Plotly figure object
            filepath: Output file path
            auto_open: Whether to open the plot automatically
        """
        fig.write_html(filepath, auto_open=auto_open, include_plotlyjs='cdn')
class NLMVisualReporter:
    """
    Comprehensive visual reporter for NLM experiments and simulations.
    """
    
    def __init__(self, output_dir: str = "nlm_reports"):
        """
        Initialize visual reporter.
        
        Args:
            output_dir: Directory for saving reports
        """
        self.output_dir = output_dir
        self.report_files = []
        
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
        
    def create_experiment_report(self, experiment_name: str, 
                                 experiment_results: List[Dict[str, Any]],
                                 brain: pynlm.Brain, world: pynlm.SimpleWorld,
                                 agent: pynlm.AgentBrain) -> str:
        """
        Create comprehensive experiment report.
        
        Args:
            experiment_name: Name of the experiment
            experiment_results: List of experiment results
            brain: NLM brain instance
            world: World simulation
            agent: Agent brain interface
            
        Returns:
            Path to created report
        """
        report_timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        report_filename = f"{experiment_name}_report_{report_timestamp}.html"
        report_path = os.path.join(self.output_dir, report_filename)
        
        # Create HTML report
        html_content = self._generate_html_report(
            experiment_name, experiment_results, brain, world, agent
        )
        
        # Save report
        with open(report_path, 'w') as f:
            f.write(html_content)
        
        self.report_files.append(report_path)
        return report_path
    
    def _generate_html_report(self, experiment_name: str,
                            experiment_results: List[Dict[str, Any]],
                            brain: pynlm.Brain, world: pynlm.SimpleWorld,
                            agent: pynlm.AgentBrain) -> str:
        """
        Generate HTML report content.
        """
        html_template = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Experiment Report: {experiment_name}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }}
        .container {{ max-width: 1200px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }}
        .header {{ text-align: center; color: #333; margin-bottom: 30px; }}
        .section {{ margin: 30px 0; padding: 20px; border-left: 4px solid #007acc; background-color: #f8f9fa; }}
        .metric {{ display: inline-block; margin: 10px; padding: 10px; background-color: #e9ecef; border-radius: 5px; }}
        .metric h3 {{ margin: 0 0 10px 0; color: #495057; }}
        .metric p {{ margin: 0; font-size: 24px; font-weight: bold; color: #007acc; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background-color: #007acc; color: white; }}
        tr:nth-child(even) {{ background-color: #f2f2f2; }}
        .plot-container {{ margin: 20px 0; text-align: center; }}
        .experiment-header {{ background-color: #e3f2fd; padding: 15px; border-radius: 5px; margin: 20px 0; }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NLM Experiment Report</h1>
            <h2>{experiment_name}</h2>
            <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
        </div>
        
        <div class="section">
            <h3>Experiment Overview</h3>
            <div class="metric">
                <h3>Total Experiments</h3>
                <p>{len(experiment_results)}</p>
            </div>
            <div class="metric">
                <h3>Successful Experiments</h3>
                <p>{len([r for r in experiment_results if r.get('total_spikes', 0) > 0])}</p>
            </div>
            <div class="metric">
                <h3>Total Spikes</h3>
                <p>{sum(r.get('total_spikes', 0) for r in experiment_results)}</p>
            </div>
            <div class="metric">
                <h3>Average Firing Rate</h3>
                <p>{np.mean([r.get('average_firing_rate', 0) for r in experiment_results]):.2f}</p>
            </div>
        </div>
        
        <div class="section">
            <h3>Experiment Results</h3>
            <table>
                <thead>
                    <tr>
                        <th>Experiment ID</th>
                        <th>Steps</th>
                        <th>Total Spikes</th>
                        <th>Average Firing Rate</th>
                        <th>Duration (s)</th>
                        <th>Curiosity Level</th>
                        <th>Novelty Level</th>
                    </tr>
                </thead>
                <tbody>
        """
        
        # Add experiment rows
        for result in experiment_results:
            html_template += f"""
                    <tr>
                        <td>{result.get('experiment_id', f'Exp {len(experiment_results)}')}</td>
                        <td>{result.get('steps', 0)}</td>
                        <td>{result.get('total_spikes', 0)}</td>
                        <td>{result.get('average_firing_rate', 0):.2f}</td>
                        <td>{result.get('duration', 0):.2f}</td>
                        <td>{result.get('curiosity_level', 0):.3f}</td>
                        <td>{result.get('novelty_level', 0):.3f}</td>
                    </tr>
            """
        
        html_template += """
                </tbody>
            </table>
        </div>
        
        <div class="section">
            <h3>Brain Configuration</h3>
            <div class="experiment-header">
                <h4>Current Brain State</h4>
            </div>
            <table>
                <tr><td><strong>Neurons:</strong></td><td>{brain.getTotalNeuronCount()}</td></tr>
                <tr><td><strong>Synapses:</strong></td><td>{brain.getTotalSynapseCount()}</td></tr>
                <tr><td><strong>Firing Neurons:</strong></td><td>{brain.getFiringNeuronCount()}</td></tr>
                <tr><td><strong>Total Spikes:</strong></td><td>{brain.getTotalSpikeCount()}</td></tr>
                <tr><td><strong>Average Firing Rate:</strong></td><td>{brain.getAverageFiringRate():.2f}</td></tr>
                <tr><td><strong>Developmental Stage:</strong></td><td>{getattr(brain, 'getDevelopmentalStage', lambda: 'N/A')()}</td></tr>
                <tr><td><strong>Initialized:</strong></td><td>{'Yes' if brain.isInitialized() else 'No'}</td></tr>
            </table>
        </div>
        
        <div class="section">
            <h3>Agent Configuration</h3>
            <div class="experiment-header">
                <h4>Current Agent State</h4>
            </div>
            <table>
        """
        
        # Add agent metrics
        if hasattr(agent, 'getCuriosityLevel'):
            html_template += f"""
                <tr><td><strong>Curiosity Level:</strong></td><td>{agent.getCuriosityLevel():.3f}</td></tr>
            """
        if hasattr(agent, 'getNoveltyLevel'):
            html_template += f"""
                <tr><td><strong>Novelty Level:</strong></td><td>{agent.getNoveltyLevel():.3f}</td></tr>
            """
        if hasattr(agent, 'getPredictionError'):
            html_template += f"""
                <tr><td><strong>Prediction Error:</strong></td><td>{agent.getPredictionError():.3f}</td></tr>
            """
        if hasattr(agent, 'getNeuromodulationLevel'):
            html_template += f"""
                <tr><td><strong>Neuromodulation Level:</strong></td><td>{agent.getNeuromodulationLevel():.3f}</td></tr>
            """
        
        html_template += """
            </table>
        </div>
        
        <div class="section">
            <h3>World Configuration</h3>
            <div class="experiment-header">
                <h4>World State</h4>
            </div>
            <table>
                <tr><td><strong>Width:</strong></td><td>{world.getWidth()}</td></tr>
                <tr><td><strong>Height:</strong></td><td>{world.getHeight()}</td></tr>
                <tr><td><strong>Simulation Time:</strong></td><td>{world.getSimulationTime():.3f}</td></tr>
                <tr><td><strong>Max Energy:</strong></td><td>{world.getMaxEnergy()}</td></tr>
                <tr><td><strong>Energy Decay Rate:</strong></td><td>{world.getEnergyDecayRate():.3f}</td></tr>
                <tr><td><strong>Random Seed:</strong></td><td>{world.getRandomSeed()}</td></tr>
            </table>
        </div>
        
        <div class="section">
            <h3>System Information</h3>
            <div class="experiment-header">
                <h4>Python API Version</h4>
            </div>
            <table>
                <tr><td><strong>Version:</strong></td><td>{pynlm.__version__ if hasattr(pynlm, '__version__') else 'N/A'}</td></tr>
                <tr><td><strong>Author:</strong></td><td>{pynlm.__author__ if hasattr(pynlm, '__author__') else 'NLM Research Team'}</td></tr>
                <tr><td><strong>Description:</strong></td><td>{pynlm.__description__ if hasattr(pynlm, '__description__') else 'NLM Python bindings'}</td></tr>
            </table>
        </div>
        
        <div class="section">
            <h3>Analysis Summary</h3>
            <p>This report contains comprehensive analysis of NLM experiment results. 
            The data has been collected and processed using the enhanced NLM Python API with 
            advanced visualization and analysis capabilities.</p>
            
            <p><strong>Key Insights:</strong></p>
            <ul>
                <li>Total experiments conducted: {len(experiment_results)}</li>
                <li>Average performance metrics calculated and analyzed</li>
                <li>Brain dynamics visualized and documented</li>
                <li>Agent behavior patterns identified and summarized</li>
            </ul>
        </div>
    </div>
</body>
</html>
"""
        
        return html_template
# Convenience functions for backward compatibility
def plot_neuron_dynamics(brain: pynlm.Brain, save_path: Optional[str] = None,
                       show_plot: bool = False) -> plt.Figure:
    """Plot neuron dynamics (backward compatibility)."""
    plotter = NLMDataPlotter()
    return plotter.plot_neuron_dynamics(brain, save_path, show_plot)

def create_interactive_plot(brain: pynlm.Brain, num_steps: int = 100) -> go.Figure:
    """Create interactive neural plot (backward compatibility)."""
    visualizer = NLMPlotlyVisualizer()
    return visualizer.create_interactive_neural_plot(brain, num_steps)

# Module exports
__all__ = [
    'NLMDataPlotter',
    'NLMTxtPlotter', 
    'NLMPlotlyVisualizer',
    'NLMVisualReporter',
    'plot_neuron_dynamics',
    'create_interactive_plot'
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Research Team"
__description__ = "Advanced visualization and plotting utilities for NLM Python bindings"

# Auto-validation on import
if __name__ != '__main__':
    try:
        # Quick test of available backends
        backends = {
            'matplotlib': 'available',
            'plotext': 'available' if PLOTEXT_AVAILABLE else 'not available',
            'plotly': 'available' if PLOTLY_AVAILABLE else 'not available',
            'PIL': 'available' if PIL_AVAILABLE else 'not available'
        }
        
        missing_backends = [k for k, v in backends.items() if v == 'not available']
        if missing_backends:
            print(f"NLM Visualization: Note - some backends not available: {missing_backends}")
    except Exception as e:
        print(f"NLM Visualization initialization warning: {e}")
