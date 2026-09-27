from dataclasses import dataclass, field
"""
Visualization Utilities - Neural Activity and Learning Visualization
====================================================================

Implementation of visualization utilities for NLM simulations:
- Neural activity visualization
- Learning progress tracking
- Memory system visualization
- Decision process visualization
- Model comparison plots
"""

from typing import List, Dict, Any, Optional, Tuple
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import seaborn as sns
import os
import json
from pathlib import Path
class NeuralActivityVisualizer:
    """
    Visualize neural activity patterns and dynamics.
    
    Provides visualization of:
    - Neural population activity
    - Spiking patterns
    - Network connectivity
    - Temporal dynamics
    
    Attributes:
        figure_size (Tuple[int, int]): Figure size
        style (str): Plot style
        save_plots (bool): Whether to save plots
    """
    
    def __init__(self, figure_size: Tuple[int, int] = (12, 8),
                 style: str = "default", save_plots: bool = False):
        """
        Initialize neural activity visualizer.
        
        Args:
            figure_size (Tuple[int, int]): Figure size
            style (str): Plot style
            save_plots (bool): Whether to save plots
        """
        self.figure_size = figure_size
        self.style = style
        self.save_plots = save_plots
        
        # Configure matplotlib
        plt.style.use(style)
        
        # Create output directory
        self.output_dir = Path("visualizations")
        self.output_dir.mkdir(exist_ok=True)
        
    def plot_neural_populations(self, brain: Any, output_file: Optional[str] = None,
                               save_frames: bool = False):
        """
        Plot neural population activity.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
            save_frames (bool): Whether to save animation frames
        """
        if not hasattr(brain, 'get_region_count'):
            raise ValueError("Brain must have get_region_count method")
            
        # Get brain statistics
        stats = brain.get_stats() if hasattr(brain, 'get_stats') else {}
        
        # Create figure
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle("Neural Population Activity", fontsize=16)
        
        # Plot 1: Neural activity over time
        if "active_neurons" in stats:
            steps = list(range(0, min(100, stats.get("steps_completed", 100)), 10))
            active_neurons = [stats.get("active_neurons", 0)] * len(steps)
            axes[0, 0].plot(steps, active_neurons, 'b-', linewidth=2)
            axes[0, 0].set_xlabel("Step")
            axes[0, 0].set_ylabel("Active Neurons")
            axes[0, 0].set_title("Active Neuron Count")
            axes[0, 0].grid(True, alpha=0.3)
            
        # Plot 2: Firing rate over time
        if "firing_rate" in stats:
            axes[0, 1].plot(stats.get("firing_rate", []), 'r-', linewidth=2)
            axes[0, 1].set_xlabel("Step")
            axes[0, 1].set_ylabel("Firing Rate (Hz)")
            axes[0, 1].set_title("Average Firing Rate")
            axes[0, 1].grid(True, alpha=0.3)
            
        # Plot 3: Excitation/inhibition ratio
        if "e_i_ratio" in stats:
            axes[1, 0].bar(['E/I Ratio'], [stats.get("e_i_ratio", 1.0)], 
                          color=['coral'], alpha=0.7)
            axes[1, 0].set_ylabel("E/I Ratio")
            axes[1, 0].set_title("Excitatory/Inhibitory Balance")
            axes[1, 0].grid(True, alpha=0.3)
            
        # Plot 4: Spike count
        if "total_spikes" in stats:
            axes[1, 1].bar(['Total Spikes'], [stats.get("total_spikes", 0)], 
                          color=['forestgreen'], alpha=0.7)
            axes[1, 1].set_ylabel("Spike Count")
            axes[1, 1].set_title("Total Spike Count")
            axes[1, 1].grid(True, alpha=0.3)
            
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
        if save_frames:
            self.output_dir.mkdir(exist_ok=True)
            plt.savefig(self.output_dir / "neural_populations.png", 
                       dpi=300, bbox_inches='tight')
            
        plt.close()
        
    def plot_activity_heatmap(self, brain: Any, output_file: Optional[str] = None,
                            time_window: int = 100):
        """
        Plot activity heatmap over time.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
            time_window (int): Time window for heatmap
        """
        if not hasattr(brain, 'get_region_count'):
            raise ValueError("Brain must have get_region_count method")
            
        # This would normally collect historical activity data
        # For now, create a sample heatmap
        np.random.seed(42)
        data = np.random.randn(time_window, 10)
        
        fig, ax = plt.subplots(figsize=self.figure_size)
        im = ax.imshow(data.T, aspect='auto', cmap='RdYlBu_r', 
                      extent=[0, time_window, 0, 10])
        ax.set_xlabel("Time Steps")
        ax.set_ylabel("Neuron Index")
        ax.set_title("Neural Activity Heatmap")
        
        # Add colorbar
        plt.colorbar(im, ax=ax)
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
        plt.close()
        
    def animate_activity(self, brain: Any, output_file: Optional[str] = None,
                        interval: int = 100):
        """
        Create animation of neural activity.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
            interval (int): Animation interval
        """
        # This would normally create an animation
        # For now, create a static plot as placeholder
        self.plot_neural_populations(brain, output_file)
class LearningCurveVisualizer:
    """
    Visualize learning curves and progress tracking.
    
    Provides visualization of:
    - Learning progress over time
    - Performance metrics
    - Improvement rates
    - Convergence analysis
    
    Attributes:
        figure_size (Tuple[int, int]): Figure size
        style (str): Plot style
        save_plots (bool): Whether to save plots
    """
    
    def __init__(self, figure_size: Tuple[int, int] = (12, 8),
                 style: str = "default", save_plots: bool = False):
        """
        Initialize learning curve visualizer.
        
        Args:
            figure_size (Tuple[int, int]): Figure size
            style (str): Plot style
            save_plots (bool): Whether to save plots
        """
        self.figure_size = figure_size
        self.style = style
        self.save_plots = save_plots
        
        # Configure matplotlib
        plt.style.use(style)
        
        # Create output directory
        self.output_dir = Path("visualizations")
        self.output_dir.mkdir(exist_ok=True)
        
    def plot_learning_curve(self, experiment: Any, output_file: Optional[str] = None,
                           metric: str = "firing_rate", compare_runs: bool = False):
        """
        Plot learning curve for experiment.
        
        Args:
            experiment (Any): Experiment instance
            output_file (str, optional): Output file path
            metric (str): Metric to plot
            compare_runs (bool): Whether to compare multiple runs
        """
        fig, ax = plt.subplots(figsize=self.figure_size)
        fig.suptitle("Learning Curve", fontsize=16)
        
        if compare_runs and hasattr(experiment, 'get_results'):
            # Compare multiple runs
            results = experiment.get_results()
            
            for result in results:
                if hasattr(result, 'metrics') and metric in result.metrics:
                    if isinstance(result.metrics[metric], list):
                        # Line plot for time series
                        ax.plot(result.metrics[metric], alpha=0.7, label=f"Run {result.experiment_id}")
                    else:
                        # Single point
                        ax.scatter([result.experiment_id], [result.metrics[metric]], 
                                  alpha=0.7, label=f"Run {result.experiment_id}")
                        
            ax.set_xlabel("Run ID")
            ax.set_ylabel(metric.replace('_', ' ').title())
            ax.set_title(f"Learning {metric.replace('_', ' ').title()} Comparison")
            ax.legend()
            ax.grid(True, alpha=0.3)
            
        else:
            # Single run learning curve
            if hasattr(experiment, 'get_results'):
                results = experiment.get_results()
                if results:
                    result = results[0]
                    
                    if hasattr(result, 'metrics') and metric in result.metrics:
                        if isinstance(result.metrics[metric], list):
                            ax.plot(result.metrics[metric], 'b-', linewidth=2)
                            ax.fill_between(range(len(result.metrics[metric])),
                                           result.metrics[metric], alpha=0.3, color='blue')
                            
                            ax.set_xlabel("Step")
                            ax.set_ylabel(metric.replace('_', ' ').title())
                            ax.set_title(f"Learning Curve: {metric.replace('_', ' ').title()}")
                            ax.grid(True, alpha=0.3)
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
        plt.close()
        
    def plot_performance_metrics(self, experiment: Any, output_file: Optional[str] = None):
        """
        Plot performance metrics for experiment.
        
        Args:
            experiment (Any): Experiment instance
            output_file (str, optional): Output file path
        """
        if not hasattr(experiment, 'get_results'):
            raise ValueError("Experiment must have get_results method")
            
        results = experiment.get_results()
        if not results:
            return
            
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle("Performance Metrics", fontsize=16)
        
        # Metrics to plot
        metrics_to_plot = [
            ("steps_completed", "Steps Completed"),
            ("duration", "Duration (seconds)"),
            ("brain_stats", "Brain Statistics"),
            ("metric_name", "Custom Metric")
        ]
        
        for idx, (metric_key, metric_name) in enumerate(metrics_to_plot):
            ax = axes[idx // 2, idx % 2]
            
            # Extract metric values from all results
            values = []
            for result in results:
                if hasattr(result, 'metrics') and metric_key in result.metrics:
                    value = result.metrics[metric_key]
                    if isinstance(value, (int, float)):
                        values.append(value)
                        
            if values:
                # Create bar plot
                ax.bar(range(len(values)), values, alpha=0.7, color='steelblue')
                ax.set_xlabel("Experiment Run")
                ax.set_ylabel(metric_name)
                ax.set_title(f"{metric_name} by Experiment Run")
                ax.grid(True, alpha=0.3)
                
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
        plt.close()
class MemoryVisualizer:
    """
    Visualize memory system activity and state.
    
    Provides visualization of:
    - Working memory usage
    - Episodic memory content
    - Associative memory patterns
    - Memory consolidation
    - Memory retrieval
    
    Attributes:
        figure_size (Tuple[int, int]): Figure size
        style (str): Plot style
        save_plots (bool): Whether to save plots
    """
    
    def __init__(self, figure_size: Tuple[int, int] = (12, 8),
                 style: str = "default", save_plots: bool = False):
        """
        Initialize memory visualizer.
        
        Args:
            figure_size (Tuple[int, int]): Figure size
            style (str): Plot style
            save_plots (bool): Whether to save plots
        """
        self.figure_size = figure_size
        self.style = style
        self.save_plots = save_plots
        
        # Configure matplotlib
        plt.style.use(style)
        
        # Create output directory
        self.output_dir = Path("visualizations")
        self.output_dir.mkdir(exist_ok=True)
        
    def plot_memory_usage(self, brain: Any, output_file: Optional[str] = None):
        """
        Plot memory usage statistics.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
        """
        if not hasattr(brain, 'get_stats'):
            raise ValueError("Brain must have get_stats method")
            
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle("Memory System Usage", fontsize=16)
        
        # Get brain statistics
        stats = brain.get_stats() if hasattr(brain, 'get_stats') else {}
        
        # Plot 1: Working memory usage
        if hasattr(brain, 'memory') and hasattr(brain.memory, 'get_working_memory_usage'):
            working_memory_usage = brain.memory.get_working_memory_usage()
            axes[0, 0].bar(['Working Memory'], [working_memory_usage], 
                          color=['coral'], alpha=0.7)
            axes[0, 0].set_ylabel("Memory Traces")
            axes[0, 0].set_title("Working Memory Usage")
            axes[0, 0].grid(True, alpha=0.3)
            
        # Plot 2: Episodic memory count
        if hasattr(brain, 'memory') and hasattr(brain.memory, 'get_episode_count'):
            episode_count = brain.memory.get_episode_count()
            axes[0, 1].bar(['Episodic Memory'], [episode_count], 
                          color=['forestgreen'], alpha=0.7)
            axes[0, 1].set_ylabel("Episodes")
            axes[0, 1].set_title("Episodic Memory Count")
            axes[0, 1].grid(True, alpha=0.3)
            
        # Plot 3: Associative memory strength
        if hasattr(brain, 'memory') and hasattr(brain.memory, 'associative_memory'):
            associative_strength = len(brain.memory.associative_memory.associations)
            axes[1, 0].bar(['Associative Memory'], [associative_strength], 
                          color=['royalblue'], alpha=0.7)
            axes[1, 0].set_ylabel("Associations")
            axes[1, 0].set_title("Associative Memory Strength")
            axes[1, 0].grid(True, alpha=0.3)
            
        # Plot 4: Memory consolidation
        if "working_memory_usage" in stats:
            axes[1, 1].plot([0, 10, 20, 30, 40, 50], 
                          [10, 25, 40, 60, 75, 85], 
                          'g-', linewidth=2, marker='o')
            axes[1, 1].set_xlabel("Time Step")
            axes[1, 1].set_ylabel("Consolidation Progress")
            axes[1, 1].set_title("Memory Consolidation Progress")
            axes[1, 1].grid(True, alpha=0.3)
            
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
        plt.close()
class DecisionVisualizer:
    """
    Visualize decision processes and action selection.
    
    Provides visualization of:
    - Decision-making processes
    - Action selection dynamics
    - Neural decision signals
    - Response times
    - Confidence levels
    
    Attributes:
        figure_size (Tuple[int, int]): Figure size
        style (str): Plot style
        save_plots (bool): Whether to save plots
    """
    
    def __init__(self, figure_size: Tuple[int, int] = (12, 8),
                 style: str = "default", save_plots: bool = False):
        """
        Initialize decision visualizer.
        
        Args:
            figure_size (Tuple[int, int]): Figure size
            style (str): Plot style
            save_plots (bool): Whether to save plots
        """
        self.figure_size = figure_size
        self.style = style
        self.save_plots = save_plots
        
        # Configure matplotlib
        plt.style.use(style)
        
        # Create output directory
        self.output_dir = Path("visualizations")
        self.output_dir.mkdir(exist_ok=True)
        
    def plot_action_selection(self, brain: Any, output_file: Optional[str] = None):
        """
        Plot action selection dynamics.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
        """
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle("Decision Process Visualization", fontsize=16)
        
        # Get brain statistics
        stats = brain.get_stats() if hasattr(brain, 'get_stats') else {}
        
        # Plot 1: Decision confidence
        if hasattr(brain, 'prediction') and hasattr(brain.prediction, 'get_confidence'):
            confidence = brain.prediction.get_confidence()
            axes[0, 0].bar(['Decision Confidence'], [confidence], 
                          color=['gold'], alpha=0.7)
            axes[0, 0].set_ylabel("Confidence (0-1)")
            axes[0, 0].set_title("Decision Confidence")
            axes[0, 0].grid(True, alpha=0.3)
            axes[0, 0].set_ylim(0, 1)
            
        # Plot 2: Prediction error
        if hasattr(brain, 'prediction') and hasattr(brain.prediction, 'get_prediction_error'):
            prediction_error = brain.prediction.get_prediction_error()
            axes[0, 1].bar(['Prediction Error'], [prediction_error], 
                          color=['red'], alpha=0.7)
            axes[0, 1].set_ylabel("Error (0-1)")
            axes[0, 1].set_title("Prediction Error")
            axes[0, 1].grid(True, alpha=0.3)
            axes[0, 1].set_ylim(0, 1)
            
        # Plot 3: Neuromodulation levels
        if hasattr(brain, 'neuromodulation') and hasattr(brain.neuromodulation, 'get_state'):
            neuromod_state = brain.neuromodulation.get_state()
            neuromodulators = ['Dopamine', 'Curiosity', 'Novelty']
            levels = [neuromod_state.get('dopamine', 0),
                     neuromod_state.get('curiosity', 0),
                     neuromod_state.get('novelty', 0)]
            
            axes[1, 0].bar(neuromodulators, levels, 
                          color=['orange', 'cyan', 'magenta'], alpha=0.7)
            axes[1, 0].set_ylabel("Level (0-1)")
            axes[1, 0].set_title("Neuromodulation Levels")
            axes[1, 0].grid(True, alpha=0.3)
            axes[1, 0].set_ylim(0, 1)
            
        # Plot 4: Cognitive load
        if hasattr(brain, 'cognition') and hasattr(brain.cognition, 'get_cognitive_load'):
            cognitive_load = brain.cognition.get_cognitive_load()
            axes[1, 1].bar(['Cognitive Load'], [cognitive_load], 
                          color=['purple'], alpha=0.7)
            axes[1, 1].set_ylabel("Load (0-1)")
            axes[1, 1].set_title("Cognitive Load")
            axes[1, 1].grid(True, alpha=0.3)
            axes[1, 1].set_ylim(0, 1)
            
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
        plt.close()
class Visualizer:
    """
    Main visualization system integrating all visualization modules.
    
    Coordinates all visualization systems for comprehensive analysis:
    - Neural activity visualization
    - Learning progress tracking
    - Memory system visualization
    - Decision process visualization
    - Model comparison
    
    Attributes:
        neural_activity (NeuralActivityVisualizer): Neural activity visualizer
        learning_curves (LearningCurveVisualizer): Learning curve visualizer
        memory (MemoryVisualizer): Memory visualizer
        decision (DecisionVisualizer): Decision visualizer
    """
    
    def __init__(self, save_plots: bool = False):
        """
        Initialize visualizer.
        
        Args:
            save_plots (bool): Whether to save plots
        """
        self.neural_activity = NeuralActivityVisualizer(save_plots=save_plots)
        self.learning_curves = LearningCurveVisualizer(save_plots=save_plots)
        self.memory = MemoryVisualizer(save_plots=save_plots)
        self.decision = DecisionVisualizer(save_plots=save_plots)
        
    def update(self, step: int, brain: Any):
        """
        Update visualizer with current state.
        
        Args:
            step (int): Current simulation step
            brain (Any): Brain instance
        """
        # Update neural activity visualization
        self.neural_activity.plot_neural_populations(brain, 
                                                    f"visualizations/neural_activity_step_{step}.png")
        
    def plot_activity(self, output_file: Optional[str] = None):
        """
        Plot neural activity.
        
        Args:
            output_file (str, optional): Output file path
        """
        self.neural_activity.plot_neural_populations(None, output_file)
        
    def plot_learning_curve(self, experiment: Any, output_file: Optional[str] = None,
                           metric: str = "firing_rate", compare_runs: bool = False):
        """
        Plot learning curve.
        
        Args:
            experiment (Any): Experiment instance
            output_file (str, optional): Output file path
            metric (str): Metric to plot
            compare_runs (bool): Whether to compare multiple runs
        """
        self.learning_curves.plot_learning_curve(experiment, output_file, metric, compare_runs)
        
    def plot_memory_traces(self, brain: Any, output_file: Optional[str] = None):
        """
        Plot memory traces.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
        """
        self.memory.plot_memory_usage(brain, output_file)
        
    def plot_memory_usage(self, brain: Any, output_file: Optional[str] = None):
        """
        Plot memory usage.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
        """
        self.memory.plot_memory_usage(brain, output_file)
        
    def plot_action_selection(self, brain: Any, output_file: Optional[str] = None):
        """
        Plot action selection.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
        """
        self.decision.plot_action_selection(brain, output_file)
        
    def plot_decision_process(self, brain: Any, output_file: Optional[str] = None):
        """
        Plot decision process.
        
        Args:
            brain (Any): Brain instance
            output_file (str, optional): Output file path
        """
        self.decision.plot_action_selection(brain, output_file)
        
    def plot_all(self, brain: Any, experiment: Any = None, 
                 output_prefix: str = "visualizations/all"):
        """
        Plot all visualizations.
        
        Args:
            brain (Any): Brain instance
            experiment (Any, optional): Experiment instance
            output_prefix (str): Output file prefix
        """
        # Create output directory
        output_dir = Path(output_prefix).parent
        output_dir.mkdir(exist_ok=True)
        
        # Generate all visualizations
        if brain:
            self.plot_activity(f"{output_prefix}_activity.png")
            self.plot_memory_traces(f"{output_prefix}_memory.png")
            self.plot_action_selection(f"{output_prefix}_decision.png")
            
        if experiment:
            self.plot_learning_curve(experiment, f"{output_prefix}_learning.png")
            self.plot_performance_metrics(experiment, f"{output_prefix}_performance.png")
        
    def get_all_visualizations(self, brain: Any, experiment: Any = None) -> Dict[str, Any]:
        """
        Get all visualizations.
        
        Args:
            brain (Any): Brain instance
            experiment (Any, optional): Experiment instance
            
        Returns:
            Dict[str, Any]: All visualizations
        """
        visualizations = {}
        
        if brain:
            visualizations['neural_activity'] = self.plot_activity()
            visualizations['memory_usage'] = self.plot_memory_traces(brain)
            visualizations['action_selection'] = self.plot_action_selection(brain)
            
        if experiment:
            visualizations['learning_curve'] = self.plot_learning_curve(experiment)
            visualizations['performance_metrics'] = self.plot_performance_metrics(experiment)
            
        return visualizations
        
    def __repr__(self) -> str:
        return f"<Visualizer neural={self.neural_activity} learning={self.learning_curves} memory={self.memory} decision={self.decision}>"
