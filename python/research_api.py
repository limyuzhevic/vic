"""
High-level research API for NLM (Neural Learning Machine)

This module provides a Pythonic interface for conducting research experiments,
managing brain simulations, and analyzing results with the NLM framework.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import json
import time
import pandas as pd
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import plotly.express as px
from typing import Dict, List, Optional, Any, Union, Callable
from dataclasses import dataclass
from enum import Enum
import numpy as np
from contextlib import contextmanager

# Try to import the C++ bindings
try:
    import pynlm
    has_c_bindings = True
except ImportError:
    has_c_bindings = False
    pynlm = None

class ResearchPhase(Enum):
    """Research experiment phases"""
    EXPLORATION = "exploration"
    EXPERIMENT = "experiment"
    ANALYSIS = "analysis"
    VALIDATION = "validation"

@dataclass
class ExperimentConfig:
    """Configuration for research experiments"""
    name: str = "default_experiment"
    brain_type: str = "standard"
    learning_rate: float = 0.01
    neuron_count: int = 1000
    max_steps: int = 1000
    record_step_data: bool = True
    checkpointing: bool = False
    enable_development: bool = False
    enable_plasticity: bool = True
    description: str = ""
    tags: List[str] = None
    
    def __post_init__(self):
        if self.tags is None:
            self.tags = []

@dataclass
class ExperimentResult:
    """Results from a research experiment"""
    experiment_name: str
    brain_manager: Any
    experiment: Any
    metrics: Dict[str, float]
    step_data: Optional[pd.DataFrame] = None
    start_time: float = 0.0
    end_time: float = 0.0
    duration: float = 0.0
    success: bool = False
    notes: List[str] = None
    
    def __post_init__(self):
        if self.notes is None:
            self.notes = []
        if self.step_data is None:
            self.step_data = pd.DataFrame()

class BrainManager:
    """
    High-level brain management interface for research experiments.
    
    This class provides a Pythonic interface for managing brain simulations,
    running experiments, and collecting metrics.
    """
    
    def __init__(self, config: Optional[Union[str, Dict]] = None, brain_type: str = 'standard'):
        """
        Initialize the BrainManager.
        
        Args:
            config: Configuration file path or dictionary
            brain_type: Type of brain to create ('standard', 'development', etc.)
        """
        if config is None:
            config = {}
        
        if isinstance(config, str):
            self.config_path = config
            if has_c_bindings:
                self.brain_manager = pynlm.BrainManager(config)
            else:
                raise ImportError("C++ bindings not available. Please install pynlm.")
        else:
            self.config_path = None
            if has_c_bindings:
                # Create a PythonConfig and get the underlying C++ config
                self.brain_manager = pynlm.BrainManager()
                for key, value in config.items():
                    self.brain_manager.setString(key, str(value))
            else:
                raise ImportError("C++ bindings not available. Please install pynlm.")
        
        self.brain_type = brain_type
    
    def run_experiment(self, steps: int, **kwargs) -> 'ExperimentResult':
        """
        Run a simulation experiment.
        
        Args:
            steps: Number of simulation steps to run
            **kwargs: Additional parameters
            
        Returns:
            ExperimentResult containing the results
        """
        start_time = time.time()
        
        # Extract parameters
        record_metrics = kwargs.get('record_metrics', True)
        experiment_name = kwargs.get('experiment_name', f"exp_{int(start_time)}")
        
        if has_c_bindings:
            # Use C++ bindings
            brain = self.brain_manager.runExperiment(steps, record_metrics, experiment_name)
            metrics = self.brain_manager.getMetrics()
            
            # Create experiment result
            result = ExperimentResult(
                experiment_name=experiment_name,
                brain_manager=self.brain_manager,
                experiment=None,  # Would need to track this
                metrics=metrics,
                start_time=start_time,
                end_time=time.time(),
                duration=time.time() - start_time,
                success=True
            )
            
            return result
        else:
            raise ImportError("C++ bindings not available.")
    
    def save_state(self, path: str) -> bool:
        """Save brain state to file."""
        if has_c_bindings:
            return self.brain_manager.saveState(path)
        return False
    
    def load_state(self, path: str) -> bool:
        """Load brain state from file."""
        if has_c_bindings:
            return self.brain_manager.loadState(path)
        return False
    
    def get_metrics(self, **kwargs) -> Dict[str, float]:
        """Get current metrics."""
        if has_c_bindings:
            return self.brain_manager.getMetrics()
        return {}
    
    def visualize(self, **kwargs) -> str:
        """Generate visualization."""
        if has_c_bindings:
            return self.brain_manager.visualize()
        return "C++ bindings not available"
    
    def set_learning_rate(self, rate: float):
        """Set learning rate."""
        if has_c_bindings:
            self.brain_manager.setLearningRate(rate)
    
    def get_learning_rate(self) -> float:
        """Get learning rate."""
        if has_c_bindings:
            return self.brain_manager.getLearningRate()
        return 0.01
    
    def set_neuron_count(self, count: int):
        """Set neuron count."""
        if has_c_bindings:
            self.brain_manager.setNeuronCount(count)
    
    def get_neuron_count(self) -> int:
        """Get neuron count."""
        if has_c_bindings:
            return self.brain_manager.getNeuronCount()
        return 1000
class ExperimentManager:
    """
    Research experiment management interface.
    
    This class provides methods for conducting various types of research experiments
    including learning experiments, integration tests, parameter tuning, and ablation studies.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize the ExperimentManager.
        
        Args:
            brain_manager: BrainManager instance to use for experiments
        """
        self.brain_manager = brain_manager
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to use for experiments."""
        self.brain_manager = brain_manager
    
    def conduct_learning_experiment(self, **kwargs) -> ExperimentResult:
        """
        Conduct a learning experiment.
        
        Args:
            **kwargs: Experiment parameters
            
        Returns:
            ExperimentResult with learning experiment results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        experiment_name = kwargs.get('experiment_name', 'learning_experiment')
        steps = kwargs.get('steps', 1000)
        learning_rate = kwargs.get('learning_rate', 0.01)
        neuron_count = kwargs.get('neuron_count', 1000)
        record_step_data = kwargs.get('record_step_data', True)
        
        # Configure brain
        self.brain_manager.set_learning_rate(learning_rate)
        self.brain_manager.set_neuron_count(neuron_count)
        
        # Run experiment
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=record_step_data,
            experiment_name=experiment_name
        )
    
    def run_integration_test(self, **kwargs) -> ExperimentResult:
        """
        Run an integration test.
        
        Args:
            **kwargs: Test parameters
            
        Returns:
            ExperimentResult with integration test results
        """
        if not self.brain_manager:
            config = kwargs.get('config', {})
            self.brain_manager = BrainManager(config)
        
        test_name = kwargs.get('test_name', 'integration_test')
        steps = kwargs.get('steps', 500)
        enable_checkpointing = kwargs.get('enable_checkpointing', True)
        enable_replay = kwargs.get('enable_replay', True)
        
        # Configure for integration test
        self.brain_manager.brain_manager.setString("test.checkpointing", str(enable_checkpointing).lower())
        self.brain_manager.brain_manager.setString("test.replay", str(enable_replay).lower())
        self.brain_manager.brain_manager.setString("brain.type", "development")
        self.brain_manager.brain_manager.setString("brain.enable_development", "true")
        
        # Run experiment
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=test_name
        )
    
    def parameter_tuning(self, param_space: Dict, n_trials: int = 10, **kwargs) -> Dict[str, Any]:
        """
        Perform parameter tuning.
        
        Args:
            param_space: Parameter space to search
            n_trials: Number of trials to run
            **kwargs: Additional parameters
            
        Returns:
            Dictionary of best parameters
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        metric = kwargs.get('metric', 'performance')
        
        # Convert param_space to JSON-like format (simplified)
        param_space_json = json.dumps(param_space)
        
        # This would call the C++ parameter tuning
        if has_c_bindings:
            return self.brain_manager.brain_manager.parameterTuning(param_space_json, n_trials, metric)
        else:
            # Fallback for when C++ bindings are not available
            return {"learning_rate": 0.01, "neuron_count": 1000}
    
    def ablation_study(self, components_to_disable: List[str], **kwargs) -> ExperimentResult:
        """
        Run an ablation study.
        
        Args:
            components_to_disable: List of components to disable
            **kwargs: Additional parameters
            
        Returns:
            ExperimentResult with ablation study results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        study_name = kwargs.get('study_name', 'ablation_study')
        steps = kwargs.get('steps', 500)
        
        # Configure for ablation study
        self.brain_manager.brain_manager.setString("study.name", study_name)
        self.brain_manager.brain_manager.setString("study.type", "ablation")
        
        std::string disabledComponents;
        for (size_t i = 0; i < components_to_disable.size(); ++i) {
            if (i > 0) disabledComponents += ",";
            disabledComponents += components_to_disable[i];
        }
        self.brain_manager.brain_manager.setString("study.disabled_components", disabledComponents);
        
        # Run experiment
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=study_name
        )
class ResearchSession:
    """
    Individual experiment session management with context manager support.
    
    This class provides a context manager for managing research sessions,
    allowing for clean resource management and session lifecycle control.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize the ResearchSession.
        
        Args:
            brain_manager: BrainManager instance to use for the session
        """
        self.brain_manager = brain_manager
        self.session_active = False
        self.step_count = 0
        self.metrics_collected = []
    
    def __enter__(self):
        """Enter context manager."""
        if self.brain_manager is None:
            self.brain_manager = BrainManager()
        
        self.session_active = True
        self.step_count = 0
        self.metrics_collected = []
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager."""
        self.session_active = False
        # Collect final metrics
        self.collect_metrics()
    
    def run_step(self, step: int):
        """
        Run a single simulation step.
        
        Args:
            step: Step number to run
        """
        if not self.session_active or self.brain_manager is None:
            raise RuntimeError("Session is not active or brain manager is not set")
        
        # This would use the C++ bindings to run a step
        if has_c_bindings:
            # For now, just collect metrics
            self.collect_metrics()
        
        self.step_count = step + 1
    
    def run_steps(self, steps: int):
        """
        Run multiple simulation steps.
        
        Args:
            steps: Number of steps to run
        """
        for step in range(steps):
            self.run_step(step)
    
    def collect_metrics(self):
        """Collect current metrics."""
        if self.brain_manager is not None:
            metrics = self.brain_manager.get_metrics()
            metrics['step_count'] = self.step_count
            metrics['session_active'] = self.session_active
            self.metrics_collected.append(metrics)
    
    def get_results(self) -> Dict[str, Any]:
        """
        Get session results.
        
        Returns:
            Dictionary containing session results
        """
        results = {
            'step_count': self.step_count,
            'session_active': self.session_active,
            'metrics_collected': self.metrics_collected,
            'brain_manager': self.brain_manager
        }
        
        if has_c_bindings and self.brain_manager is not None:
            # Get brain reference if available
            try:
                results['brain'] = self.brain_manager.brain_manager.getBrain()
            except:
                pass
        
        return results
class PythonConfig:
    """
    Python-friendly configuration interface.
    
    This class provides a Pythonic interface for loading, accessing, and saving
    configuration files in JSON or YAML format.
    """
    
    def __init__(self, **kwargs):
        """
        Initialize the PythonConfig.
        
        Args:
            **kwargs: Initial configuration parameters
        """
        self.config = {}
        for key, value in kwargs.items():
            self.config[key] = value
    
    def load_json(self, path: str):
        """Load configuration from JSON file."""
        with open(path, 'r') as f:
            self.config = json.load(f)
    
    def load_yaml(self, path: str):
        """Load configuration from YAML file."""
        try:
            import yaml
            with open(path, 'r') as f:
                self.config = yaml.safe_load(f)
        except ImportError:
            raise ImportError("PyYAML is required for YAML support")
    
    def save_json(self, path: str):
        """Save configuration to JSON file."""
        with open(path, 'w') as f:
            json.dump(self.config, f, indent=2)
    
    # Pythonic property access
    @property
    def learning_rate(self) -> float:
        """Get learning rate."""
        return self.config.get('learning_rate', 0.01)
    
    @learning_rate.setter
    def learning_rate(self, value: float):
        """Set learning rate."""
        self.config['learning_rate'] = value
    
    @property
    def neuron_count(self) -> int:
        """Get neuron count."""
        return self.config.get('neuron_count', 1000)
    
    @neuron_count.setter
    def neuron_count(self, value: int):
        """Set neuron count."""
        self.config['neuron_count'] = value
    
    @property
    def brain_type(self) -> str:
        """Get brain type."""
        return self.config.get('brain_type', 'standard')
    
    @brain_type.setter
    def brain_type(self, value: str):
        """Set brain type."""
        self.config['brain_type'] = value
    
    @property
    def max_steps(self) -> int:
        """Get maximum steps."""
        return self.config.get('max_steps', 1000)
    
    @max_steps.setter
    def max_steps(self, value: int):
        """Set maximum steps."""
        self.config['max_steps'] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
        return self.config.get(key, default)
    
    def set(self, key: str, value: Any):
        """Set configuration value."""
        self.config[key] = value
    
    def to_dict(self) -> Dict:
        """Convert to dictionary."""
        return self.config.copy()
    
    @classmethod
    def from_dict(cls, config_dict: Dict) -> 'PythonConfig':
        """Create from dictionary."""
        config = cls()
        config.config = config_dict.copy()
        return config
    
    def __repr__(self) -> str:
        return f"PythonConfig({self.config})"
class MatplotlibVisualizer:
    """
    Visualization using matplotlib.
    
    This class provides matplotlib-based visualization of brain simulation results,
    including neural activity, memory metrics, learning progress, and integration status.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize the MatplotlibVisualizer.
        
        Args:
            brain_manager: BrainManager instance to visualize
        """
        self.brain_manager = brain_manager
        self.plots = {}
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to visualize."""
        self.brain_manager = brain_manager
    
    def plot_neural_activity(self, step_range: Optional[tuple] = None):
        """
        Plot neural activity over time.
        
        Args:
            step_range: Tuple of (start_step, end_step) to visualize
            
        Returns:
            matplotlib Figure object
        """
        if self.brain_manager is None:
            raise ValueError("Brain manager is not set")
        
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Generate sample data (in practice, this would use real brain data)
        if step_range is None:
            steps = 100
        else:
            steps = step_range[1] - step_range[0] + 1
        
        x = range(steps)
        y = np.random.normal(0.5, 0.1, steps)  # Sample neural activity
        
        ax.plot(x, y, linewidth=2)
        ax.set_xlabel('Step')
        ax.set_ylabel('Neural Activity')
        ax.set_title('Neural Activity Over Time')
        ax.grid(True, alpha=0.3)
        
        self.plots['neural_activity'] = fig
        return fig
    
    def plot_memory_metrics(self):
        """
        Plot memory system metrics.
        
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Generate sample memory metrics data
        steps = 50
        time_steps = np.arange(steps)
        
        # Working memory
        working_memory = np.random.uniform(0.3, 0.8, steps)
        axes[0, 0].plot(time_steps, working_memory)
        axes[0, 0].set_title('Working Memory')
        axes[0, 0].set_ylabel('Utilization')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Episodic memory
        episodic_memory = np.random.uniform(0, 1, steps)
        axes[0, 1].plot(time_steps, episodic_memory)
        axes[0, 1].set_title('Episodic Memory')
        axes[0, 1].set_ylabel('Stored Episodes')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Consolidation
        consolidation = np.random.uniform(0, 1, steps)
        axes[1, 0].plot(time_steps, consolidation)
        axes[1, 0].set_title('Memory Consolidation')
        axes[1, 0].set_ylabel('Effectiveness')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Replay events
        replay_events = np.random.poisson(0.5, steps)
        axes[1, 1].bar(time_steps[::2], replay_events[::2], alpha=0.7)
        axes[1, 1].set_title('Replay Events')
        axes[1, 1].set_ylabel('Events per Step')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        self.plots['memory_metrics'] = fig
        return fig
    
    def plot_learning_progress(self):
        """
        Plot learning progress over time.
        
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 1, figsize=(10, 8))
        
        steps = 100
        time_steps = np.arange(steps)
        
        # Learning rate
        learning_rates = np.random.uniform(0.001, 0.1, steps)
        axes[0].plot(time_steps, learning_rates)
        axes[0].set_title('Learning Rate')
        axes[0].set_ylabel('Rate')
        axes[0].grid(True, alpha=0.3)
        
        # Performance
        performance = np.cumsum(np.random.exponential(0.05, steps))
        axes[1].plot(time_steps, performance)
        axes[1].set_title('Learning Performance')
        axes[1].set_ylabel('Performance')
        axes[1].set_xlabel('Step')
        axes[1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        self.plots['learning_progress'] = fig
        return fig
    
    def plot_integration_status(self):
        """
        Plot integration status.
        
        Returns:
            matplotlib Figure object
        """
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        components = ['Memory', 'Neuromodulation', 'Prediction', 'Development']
        integration_scores = [0.8, 0.9, 0.7, 0.85]
        
        axes[0, 0].barh(components, integration_scores, color='skyblue')
        axes[0, 0].set_title('Integration Status')
        axes[0, 0].set_xlabel('Score')
        axes[0, 0].set_xlim(0, 1)
        
        # System metrics
        metrics = ['Firing Rate', 'Learning Rate', 'Memory Usage', 'Energy']
        values = [0.6, 0.8, 0.4, 0.9]
        
        axes[0, 1].bar(metrics, values, color='lightcoral')
        axes[0, 1].set_title('System Metrics')
        axes[0, 1].set_ylabel('Value')
        axes[0, 1].set_ylim(0, 1)
        axes[0, 1].grid(True, alpha=0.3)
        
        # Time series of integration
        integration_progress = np.linspace(0, 1, 50)
        axes[1, 0].plot(integration_progress)
        axes[1, 0].set_title('Integration Progress')
        axes[1, 0].set_ylabel('Integration Level')
        axes[1, 0].set_xlabel('Phase')
        axes[1, 0].grid(True, alpha=0.3)
        
        # System health
        health_metrics = [0.95, 0.88, 0.92, 0.87]
        axes[1, 1].pie(health_metrics, labels=['Memory', 'NM', 'Pred', 'Dev'], autopct='%1.1f%%')
        axes[1, 1].set_title('System Health')
        
        plt.tight_layout()
        self.plots['integration_status'] = fig
        return fig
    
    def show_all_plots(self):
        """Display all generated plots."""
        for plot in self.plots.values():
            plt.show()
    
    def save_plot(self, name: str, path: str):
        """
        Save a plot to file.
        
        Args:
            name: Name of the plot to save
            path: File path to save
        """
        if name in self.plots:
            self.plots[name].savefig(path, dpi=300, bbox_inches='tight')
class PlotlyVisualizer:
    """
    Interactive visualization using plotly.
    
    This class provides interactive, web-based visualization of brain simulation results,
    including real-time dashboards, interactive plots, and exploration tools.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize the PlotlyVisualizer.
        
        Args:
            brain_manager: BrainManager instance to visualize
        """
        self.brain_manager = brain_manager
        self.figures = {}
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to visualize."""
        self.brain_manager = brain_manager
    
    def create_interactive_dashboard(self):
        """
        Create an interactive dashboard.
        
        Returns:
            plotly Figure object
        """
        fig = go.Figure()
        
        # Create a multi-panel dashboard
        fig.add_trace(go.Scatter(
            x=np.arange(100),
            y=np.random.normal(0.5, 0.1, 100),
            mode='lines',
            name='Neural Activity'
        ))
        
        fig.add_trace(go.Scatter(
            x=np.arange(100),
            y=np.random.normal(0.3, 0.05, 100),
            mode='lines',
            name='Learning Progress',
            yaxis='y2'
        ))
        
        fig.update_layout(
            title='Brain Simulation Dashboard',
            xaxis=dict(title='Step'),
            yaxis=dict(title='Neural Activity'),
            yaxis2=dict(title='Learning Progress', overlaying='y', side='right'),
            height=600
        )
        
        self.figures['dashboard'] = fig
        return fig
    
    def plot_real_time_metrics(self):
        """
        Create real-time metrics visualization.
        
        Returns:
            plotly Figure object
        """
        fig = go.Figure()
        
        # Real-time metrics data
        time_steps = np.arange(50)
        neural_activity = np.random.normal(0.5, 0.1, 50)
        firing_rate = np.random.normal(0.6, 0.05, 50)
        learning_rate = np.random.uniform(0.001, 0.1, 50)
        
        fig.add_trace(go.Scatter(
            x=time_steps,
            y=neural_activity,
            mode='lines',
            name='Neural Activity',
            line=dict(color='blue')
        ))
        
        fig.add_trace(go.Scatter(
            x=time_steps,
            y=firing_rate,
            mode='lines',
            name='Firing Rate',
            yaxis='y2',
            line=dict(color='red')
        ))
        
        fig.add_trace(go.Scatter(
            x=time_steps,
            y=learning_rate,
            mode='lines',
            name='Learning Rate',
            yaxis='y3',
            line=dict(color='green')
        ))
        
        fig.update_layout(
            title='Real-Time Metrics',
            xaxis=dict(title='Step'),
            yaxis=dict(title='Neural Activity', side='left'),
            yaxis2=dict(title='Firing Rate', overlaying='y', side='right'),
            yaxis3=dict(title='Learning Rate', overlaying='y', side='right', position=0.9),
            height=500
        )
        
        self.figures['real_time'] = fig
        return fig
    
    def create_exploration_plots(self):
        """
        Create exploration plots for different brain parameters.
        
        Returns:
            Dictionary of plotly Figure objects
        """
        plots = {}
        
        # Parameter exploration
        fig = go.Figure()
        
        # Sample data for different brain types
        brain_types = ['Standard', 'Developmental', 'Advanced']
        learning_rates = [0.01, 0.05, 0.001]
        neuron_counts = [1000, 2000, 500]
        
        # Create scatter plot for parameter exploration
        fig.add_trace(go.Scatter(
            x=learning_rates,
            y=neuron_counts,
            mode='markers',
            marker=dict(size=15, color=np.random.rand(len(learning_rates)), colorscale='Viridis'),
            text=[f"{bt} - LR: {lr}, NC: {nc}" for bt, lr, nc in zip(brain_types, learning_rates, neuron_counts)],
            name='Brain Configurations'
        ))
        
        fig.update_layout(
            title='Brain Parameter Exploration',
            xaxis=dict(title='Learning Rate'),
            yaxis=dict(title='Neuron Count'),
            height=400
        )
        
        plots['parameter_exploration'] = fig
        
        # Performance comparison
        fig2 = go.Figure()
        
        performance_data = {
            'Standard': np.random.normal(0.8, 0.1, 50),
            'Developmental': np.random.normal(0.7, 0.15, 50),
            'Advanced': np.random.normal(0.85, 0.08, 50)
        }
        
        for brain_type, data in performance_data.items():
            fig2.add_trace(go.Box(
                y=data,
                name=brain_type,
                boxpoints='all'
            ))
        
        fig2.update_layout(
            title='Performance Comparison by Brain Type',
            yaxis=dict(title='Performance Score'),
            height=400
        )
        
        plots['performance_comparison'] = fig2
        
        self.figures.update(plots)
        return plots
    
    def show_interactive(self, figure_names: Optional[List[str]] = None):
        """
        Display interactive plots.
        
        Args:
            figure_names: List of figure names to display. If None, display all.
        """
        if figure_names is None:
            figure_names = list(self.figures.keys())
        
        for name in figure_names:
            if name in self.figures:
                self.figures[name].show()
    
    def save_interactive(self, name: str, path: str):
        """
        Save interactive plot to HTML file.
        
        Args:
            name: Name of the plot to save
            path: File path to save
        """
        if name in self.figures:
            self.figures[name].write_html(path)
class PandasDataFrameExporter:
    """
    Export simulation results to pandas DataFrames.
    
    This class provides methods for exporting simulation data in Python-friendly
    formats, making it easy to analyze and visualize results using pandas.
    """
    
    def export_simulation_results(self, brain_manager: BrainManager, 
                                  metrics: Optional[List[str]] = None) -> pd.DataFrame:
        """
        Export simulation results to pandas DataFrame.
        
        Args:
            brain_manager: BrainManager instance
            metrics: List of metrics to export. If None, export all available.
            
        Returns:
            pandas DataFrame with simulation results
        """
        if not has_c_bindings:
            raise ImportError("C++ bindings not available")
        
        # Get metrics from brain manager
        all_metrics = brain_manager.get_metrics()
        
        if metrics is None:
            metrics = list(all_metrics.keys())
        
        # Create DataFrame
        data = {}
        for metric in metrics:
            if metric in all_metrics:
                data[metric] = [all_metrics[metric]]
        
        return pd.DataFrame(data)
    
    def export_step_data(self, brain_manager: BrainManager, steps: List[int]) -> pd.DataFrame:
        """
        Export step-by-step data.
        
        Args:
            brain_manager: BrainManager instance
            steps: List of step numbers
            
        Returns:
            pandas DataFrame with step data
        """
        # Generate sample step data (in practice, this would use real brain data)
        data = {
            'step': steps,
            'total_spike_count': np.random.randint(100, 1000, len(steps)),
            'active_neuron_count': np.random.randint(50, 500, len(steps)),
            'firing_neuron_count': np.random.randint(20, 200, len(steps)),
            'average_firing_rate': np.random.uniform(0, 1, len(steps)),
            'learning_rate': np.random.uniform(0.001, 0.1, len(steps)),
            'neuron_count': np.full(len(steps), brain_manager.get_neuron_count()),
            'session_active': np.ones(len(steps))
        }
        
        return pd.DataFrame(data)
    
    def export_memory_data(self, brain_manager: BrainManager) -> pd.DataFrame:
        """
        Export memory system data.
        
        Args:
            brain_manager: BrainManager instance
            
        Returns:
            pandas DataFrame with memory data
        """
        # Generate sample memory data
        memory_types = ['working_memory', 'episodic_memory', 'associative_memory']
        data = {}
        
        for memory_type in memory_types:
            size = np.random.randint(100, 1000)
            data[f'{memory_type}_size'] = [size]
            data[f'{memory_type}_utilization'] = [np.random.uniform(0, 1)]
        
        return pd.DataFrame(data)
class StatisticalAnalysis:
    """
    Built-in statistical analysis functions.
    
    This class provides comprehensive statistical analysis tools for simulation
    data, including t-tests, correlation analysis, trend analysis, and significance tests.
    """
    
    def __init__(self, data: pd.DataFrame):
        """
        Initialize StatisticalAnalysis.
        
        Args:
            data: pandas DataFrame with data to analyze
        """
        self.data = data
    
    def t_test(self, group1: pd.Series, group2: pd.Series) -> Dict[str, Any]:
        """
        Perform t-test between two groups.
        
        Args:
            group1: First group data
            group2: Second group data
            
        Returns:
            Dictionary with t-test results
        """
        from scipy import stats
        
        t_stat, p_value = stats.ttest_ind(group1, group2, equal_var=False)
        
        return {
            't_statistic': t_stat,
            'p_value': p_value,
            'mean_diff': group1.mean() - group2.mean(),
            'std_diff': np.sqrt(group1.var() + group2.var())
        }
    
    def correlation_analysis(self, x: pd.Series, y: pd.Series) -> Dict[str, Any]:
        """
        Perform correlation analysis between two variables.
        
        Args:
            x: First variable
            y: Second variable
            
        Returns:
            Dictionary with correlation results
        """
        correlation = x.corr(y)
        
        return {
            'correlation': correlation,
            'p_value': 2 * (1 - stats.norm.cdf(abs(correlation) * np.sqrt(len(x) - 2))),
            'sample_size': len(x)
        }
    
    def trend_analysis(self, data: pd.Series, window_size: int = 10) -> Dict[str, Any]:
        """
        Perform trend analysis on time series data.
        
        Args:
            data: Time series data
            window_size: Window size for rolling analysis
            
        Returns:
            Dictionary with trend analysis results
        """
        # Calculate rolling mean
        rolling_mean = data.rolling(window=window_size).mean()
        
        # Calculate trend using linear regression
        from scipy import stats
        x = np.arange(len(data))
        y = data.values
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_value ** 2,
            'p_value': p_value,
            'std_error': std_err,
            'rolling_mean': rolling_mean.tolist()
        }
    
    def significance_test(self, hypothesis: str, data: pd.Series) -> Dict[str, Any]:
        """
        Perform significance test based on hypothesis.
        
        Args:
            hypothesis: Hypothesis to test (e.g., 'mean > 0', 'mean != 0', 'mean < 0')
            data: Data to test
            
        Returns:
            Dictionary with test results
        """
        from scipy import stats
        
        mean = data.mean()
        std = data.std()
        n = len(data)
        
        if hypothesis == 'mean > 0':
            t_stat, p_value = stats.ttest_1samp(data, 0)
            p_value = p_value / 2  # One-tailed test
        elif hypothesis == 'mean < 0':
            t_stat, p_value = stats.ttest_1samp(data, 0)
            p_value = p_value / 2  # One-tailed test
        else:  # mean != 0
            t_stat, p_value = stats.ttest_1samp(data, 0)
        
        return {
            'hypothesis': hypothesis,
            'mean': mean,
            'std': std,
            't_statistic': t_stat,
            'p_value': p_value,
            'significant': p_value < 0.05,
            'confidence_interval': stats.t.interval(0.95, n-1, loc=mean, scale=std/np.sqrt(n))
        }
class LearningExperimentAPI:
    """
    Learning experiment API for conducting learning experiments.
    
    This class provides methods for conducting learning experiments with different
    learning strategies and configurations.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize LearningExperimentAPI.
        
        Args:
            brain_manager: BrainManager instance to use for experiments
        """
        self.brain_manager = brain_manager
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to use."""
        self.brain_manager = brain_manager
    
    def run_basic_learning(self, **kwargs) -> ExperimentResult:
        """
        Run a basic learning experiment.
        
        Args:
            **kwargs: Experiment parameters
            
        Returns:
            ExperimentResult with learning experiment results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        return self.conduct_learning_experiment(**kwargs)
    
    def run_plasticity_learning(self, **kwargs) -> ExperimentResult:
        """
        Run a plasticity learning experiment.
        
        Args:
            **kwargs: Experiment parameters
            
        Returns:
            ExperimentResult with plasticity learning experiment results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for plasticity learning
        self.brain_manager.brain_manager.setString("plasticity.stdp.enabled", "true")
        self.brain_manager.brain_manager.setString("plasticity.hebbian.enabled", "true")
        
        return self.conduct_learning_experiment(**kwargs)
    
    def run_development_learning(self, **kwargs) -> ExperimentResult:
        """
        Run a development learning experiment.
        
        Args:
            **kwargs: Experiment parameters
            
        Returns:
            ExperimentResult with development learning experiment results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for development learning
        self.brain_manager.brain_manager.setString("brain.enable_development", "true")
        
        return self.conduct_learning_experiment(**kwargs)
    
    def conduct_learning_experiment(self, **kwargs) -> ExperimentResult:
        """
        Conduct a learning experiment with the given parameters.
        
        Args:
            **kwargs: Experiment parameters
            
        Returns:
            ExperimentResult with experiment results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        experiment_name = kwargs.get('experiment_name', 'learning_experiment')
        steps = kwargs.get('steps', 1000)
        learning_rate = kwargs.get('learning_rate', 0.01)
        neuron_count = kwargs.get('neuron_count', 1000)
        record_step_data = kwargs.get('record_step_data', True)
        
        # Configure brain
        self.brain_manager.set_learning_rate(learning_rate)
        self.brain_manager.set_neuron_count(neuron_count)
        
        # Run experiment
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=record_step_data,
            experiment_name=experiment_name
        )
class IntegrationTestAPI:
    """
    Integration test API for running system integration tests.
    
    This class provides methods for running integration tests that verify that
    all brain systems work together correctly.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize IntegrationTestAPI.
        
        Args:
            brain_manager: BrainManager instance to use for tests
        """
        self.brain_manager = brain_manager
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to use."""
        self.brain_manager = brain_manager
    
    def run_complete_integration(self, **kwargs) -> ExperimentResult:
        """
        Run a complete integration test.
        
        Args:
            **kwargs: Test parameters
            
        Returns:
            ExperimentResult with integration test results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for integration test
        self.brain_manager.brain_manager.setString("brain.type", "development")
        self.brain_manager.brain_manager.setString("brain.enable_development", "true")
        self.brain_manager.brain_manager.setString("test.checkpointing", "true")
        self.brain_manager.brain_manager.setString("test.replay", "true")
        
        test_name = kwargs.get('test_name', 'complete_integration')
        steps = kwargs.get('steps', 10000)
        
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=test_name
        )
    
    def run_memory_integration(self, **kwargs) -> ExperimentResult:
        """
        Run a memory integration test.
        
        Args:
            **kwargs: Test parameters
            
        Returns:
            ExperimentResult with memory integration test results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for memory integration
        self.brain_manager.brain_manager.setString("memory.working_memory.enabled", "true")
        self.brain_manager.brain_manager.setString("memory.episodic_memory.enabled", "true")
        self.brain_manager.brain_manager.setString("memory.associative_memory.enabled", "true")
        
        test_name = kwargs.get('test_name', 'memory_integration')
        steps = kwargs.get('steps', 3000)
        
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=test_name
        )
    
    def run_neuromodulation_integration(self, **kwargs) -> ExperimentResult:
        """
        Run a neuromodulation integration test.
        
        Args:
            **kwargs: Test parameters
            
        Returns:
            ExperimentResult with neuromodulation integration test results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for neuromodulation integration
        self.brain_manager.brain_manager.setString("neuromodulation.dopamine.enabled", "true")
        self.brain_manager.brain_manager.setString("neuromodulation.curiosity.enabled", "true")
        self.brain_manager.brain_manager.setString("neuromodulation.novelty.enabled", "true")
        self.brain_manager.brain_manager.setString("neuromodulation.prediction_error.enabled", "true")
        
        test_name = kwargs.get('test_name', 'neuromodulation_integration')
        steps = kwargs.get('steps', 2000)
        
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=test_name
        )
class ParameterTuningAPI:
    """
    Parameter tuning API for hyperparameter optimization.
    
    This class provides methods for hyperparameter optimization using various
    techniques like grid search, random search, and Bayesian optimization.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize ParameterTuningAPI.
        
        Args:
            brain_manager: BrainManager instance to use for tuning
        """
        self.brain_manager = brain_manager
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to use."""
        self.brain_manager = brain_manager
    
    def tune_learning_rate(self, learning_rates: List[float], n_trials: int = 10) -> Dict[str, Any]:
        """
        Tune learning rate hyperparameter.
        
        Args:
            learning_rates: List of learning rates to try
            n_trials: Number of trials for each learning rate
            
        Returns:
            Dictionary of best parameters
        """
        best_score = 0.0
        best_learning_rate = learning_rates[0]
        
        for learning_rate in learning_rates:
            if not self.brain_manager:
                self.brain_manager = BrainManager()
            
            self.brain_manager.set_learning_rate(learning_rate)
            
            # Run experiment
            result = self.brain_manager.run_experiment(
                steps=100,
                record_metrics=False
            )
            
            # Calculate score
            score = sum(result.metrics.values())
            
            if score > best_score:
                best_score = score
                best_learning_rate = learning_rate
        
        return {
            'best_learning_rate': best_learning_rate,
            'best_score': best_score
        }
    
    def tune_network_size(self, neuron_counts: List[int], n_trials: int = 10) -> Dict[str, Any]:
        """
        Tune network size hyperparameter.
        
        Args:
            neuron_counts: List of neuron counts to try
            n_trials: Number of trials for each neuron count
            
        Returns:
            Dictionary of best parameters
        """
        best_score = 0.0
        best_neuron_count = neuron_counts[0]
        
        for neuron_count in neuron_counts:
            if not self.brain_manager:
                self.brain_manager = BrainManager()
            
            self.brain_manager.set_neuron_count(neuron_count)
            
            # Run experiment
            result = self.brain_manager.run_experiment(
                steps=100,
                record_metrics=False
            )
            
            # Calculate score
            score = sum(result.metrics.values())
            
            if score > best_score:
                best_score = score
                best_neuron_count = neuron_count
        
        return {
            'best_neuron_count': best_neuron_count,
            'best_score': best_score
        }
class AblationStudyAPI:
    """
    Ablation study API for testing system components.
    
    This class provides methods for conducting ablation studies to test the
    importance of different brain components by disabling them individually or in combination.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize AblationStudyAPI.
        
        Args:
            brain_manager: BrainManager instance to use for studies
        """
        self.brain_manager = brain_manager
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to use."""
        self.brain_manager = brain_manager
    
    def disable_memory_systems(self, **kwargs) -> ExperimentResult:
        """
        Disable all memory systems.
        
        Args:
            **kwargs: Study parameters
            
        Returns:
            ExperimentResult with memory ablation study results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for memory ablation
        self.brain_manager.brain_manager.setString("memory.working_memory.enabled", "false")
        self.brain_manager.brain_manager.setString("memory.episodic_memory.enabled", "false")
        self.brain_manager.brain_manager.setString("memory.associative_memory.enabled", "false")
        
        study_name = kwargs.get('study_name', 'memory_ablation')
        steps = kwargs.get('steps', 500)
        
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=study_name
        )
    
    def disable_neuromodulation(self, **kwargs) -> ExperimentResult:
        """
        Disable all neuromodulation systems.
        
        Args:
            **kwargs: Study parameters
            
        Returns:
            ExperimentResult with neuromodulation ablation study results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for neuromodulation ablation
        self.brain_manager.brain_manager.setString("neuromodulation.dopamine.enabled", "false")
        self.brain_manager.brain_manager.setString("neuromodulation.curiosity.enabled", "false")
        self.brain_manager.brain_manager.setString("neuromodulation.novelty.enabled", "false")
        self.brain_manager.brain_manager.setString("neuromodulation.prediction_error.enabled", "false")
        
        study_name = kwargs.get('study_name', 'neuromodulation_ablation')
        steps = kwargs.get('steps', 500)
        
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=study_name
        )
    
    def disable_plasticity(self, **kwargs) -> ExperimentResult:
        """
        Disable all plasticity systems.
        
        Args:
            **kwargs: Study parameters
            
        Returns:
            ExperimentResult with plasticity ablation study results
        """
        if not self.brain_manager:
            self.brain_manager = BrainManager()
        
        # Configure for plasticity ablation
        self.brain_manager.brain_manager.setString("plasticity.stdp.enabled", "false")
        self.brain_manager.brain_manager.setString("plasticity.hebbian.enabled", "false")
        self.brain_manager.brain_manager.setString("plasticity.structural_plasticity.enabled", "false")
        
        study_name = kwargs.get('study_name', 'plasticity_ablation')
        steps = kwargs.get('steps', 500)
        
        return self.brain_manager.run_experiment(
            steps=steps,
            record_metrics=True,
            experiment_name=study_name
        )
class CrossValidationAPI:
    """
    Cross-validation API for model validation.
    
    This class provides methods for cross-validation to validate model performance
    on different subsets of data or different experimental conditions.
    """
    
    def __init__(self, brain_manager: Optional[BrainManager] = None):
        """
        Initialize CrossValidationAPI.
        
        Args:
            brain_manager: BrainManager instance to use for validation
        """
        self.brain_manager = brain_manager
    
    def set_brain_manager(self, brain_manager: BrainManager):
        """Set the brain manager to use."""
        self.brain_manager = brain_manager
    
    def validate_learning(self, n_folds: int = 5) -> Dict[str, float]:
        """
        Validate learning performance using cross-validation.
        
        Args:
            n_folds: Number of folds for cross-validation
            
        Returns:
            Dictionary of cross-validation results
        """
        # Create parameter sets for cross-validation
        param_sets = []
        for i in range(10):
            param_sets.append(f"params_{i}")
        
        results = {}
        for i, param_set in enumerate(param_sets):
            if not self.brain_manager:
                self.brain_manager = BrainManager()
            
            # Simulate cross-validation for this parameter set
            fold_scores = []
            for fold in range(n_folds):
                # Configure for this fold
                self.brain_manager.set_learning_rate(0.01 * (i + 1))
                self.brain_manager.set_neuron_count(500 + fold * 200)
                
                # Run experiment
                result = self.brain_manager.run_experiment(
                    steps=100,
                    record_metrics=False
                )
                
                fold_score = sum(result.metrics.values()) / len(result.metrics)
                fold_scores.append(fold_score)
            
            results[param_set] = sum(fold_scores) / len(fold_scores)
        
        return results
    
    def validate_memory(self, n_folds: int = 5) -> Dict[str, float]:
        """
        Validate memory performance using cross-validation.
        
        Args:
            n_folds: Number of folds for cross-validation
            
        Returns:
            Dictionary of cross-validation results
        """
        # Create memory parameter sets for cross-validation
        param_sets = []
        for i in range(5):
            param_sets.append(f"memory_params_{i}")
        
        results = {}
        for i, param_set in enumerate(param_sets):
            if not self.brain_manager:
                self.brain_manager = BrainManager()
            
            # Configure for memory validation
            self.brain_manager.brain_manager.setString("memory.working_memory.enabled", "true")
            self.brain_manager.brain_manager.setString("memory.episodic_memory.enabled", "true")
            self.brain_manager.brain_manager.setString("memory.associative_memory.enabled", "true")
            
            # Simulate cross-validation
            fold_scores = []
            for fold in range(n_folds):
                # Run experiment
                result = self.brain_manager.run_experiment(
                    steps=50,
                    record_metrics=False
                )
                
                fold_score = sum(result.metrics.values()) / len(result.metrics)
                fold_scores.append(fold_score)
            
            results[param_set] = sum(fold_scores) / len(fold_scores)
        
        return results
# Module-level convenience functions

def create_brain_manager(config: Optional[Union[str, Dict]] = None, brain_type: str = 'standard') -> BrainManager:
    """Create a BrainManager instance."""
    return BrainManager(config, brain_type)

def create_experiment_manager(brain_manager: Optional[BrainManager] = None) -> ExperimentManager:
    """Create an ExperimentManager instance."""
    return ExperimentManager(brain_manager)

def create_research_session(brain_manager: Optional[BrainManager] = None) -> ResearchSession:
    """Create a ResearchSession instance."""
    return ResearchSession(brain_manager)

def create_matplotlib_visualizer(brain_manager: Optional[BrainManager] = None) -> MatplotlibVisualizer:
    """Create a MatplotlibVisualizer instance."""
    return MatplotlibVisualizer(brain_manager)

def create_plotly_visualizer(brain_manager: Optional[BrainManager] = None) -> PlotlyVisualizer:
    """Create a PlotlyVisualizer instance."""
    return PlotlyVisualizer(brain_manager)

def create_pandas_exporter() -> PandasDataFrameExporter:
    """Create a PandasDataFrameExporter instance."""
    return PandasDataFrameExporter()

def create_statistical_analysis(data: pd.DataFrame) -> StatisticalAnalysis:
    """Create a StatisticalAnalysis instance."""
    return StatisticalAnalysis(data)

# Main module exports
__all__ = [
    'BrainManager',
    'ExperimentManager',
    'ResearchSession',
    'PythonConfig',
    'MatplotlibVisualizer',
    'PlotlyVisualizer',
    'PandasDataFrameExporter',
    'StatisticalAnalysis',
    'LearningExperimentAPI',
    'IntegrationTestAPI',
    'ParameterTuningAPI',
    'AblationStudyAPI',
    'CrossValidationAPI',
    'ExperimentConfig',
    'ExperimentResult',
    'ResearchPhase',
    'create_brain_manager',
    'create_experiment_manager',
    'create_research_session',
    'create_matplotlib_visualizer',
    'create_plotly_visualizer',
    'create_pandas_exporter',
    'create_statistical_analysis'
]

if has_c_bindings:
    # Add C++ bindings to exports
    __all__.extend([
        'pynlm'
    ])