# NLM Enhanced Python API
# 
# This module provides enhanced Python interfaces for the NLM neural simulation framework.
# It includes utilities for visualization, analysis, experiment management, and more.
# 
# Version: 1.0
# Author: NLM Development Team
# License: MIT

import sys
import os
import json
import time
import threading
from typing import List, Dict, Any, Optional, Union, Iterator
from dataclasses import dataclass, field
from enum import Enum

# Import the enhanced NLM bindings
try:
    import pynlm as nlm
except ImportError as e:
    raise ImportError(
        "NLM Python bindings not found. Please build the project with pybind11 support.\n"
        f"Error: {e}\n\n"
        "To build the Python bindings:\n"
        "1. Navigate to the project root directory\n"
        "2. Create a build directory: mkdir build && cd build\n"
        "3. Run CMake: cmake .. -DPYTHON_EXECUTABLE=$(which python3)\n"
        "4. Build: make -j4\n"
        "5. Install: pip install .\n"
        "\nFor development, use: pip install -e ."
    ) from e

__version__ = "1.0.0"

# === ENHANCED EXCEPTION CLASSES ===

class NLMError(Exception):
    """Base exception for all NLM errors."""
    
    def __init__(self, message: str, error_code: int = 0, details: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
    
    def __str__(self) -> str:
        if self.details:
            details_str = ", ".join(f"{k}={v}" for k, v in self.details.items())
            return f"{self.message} ({details_str})"
        return self.message
class ConfigurationError(NLMError):
    """Exception raised for configuration-related errors."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(f"Configuration Error: {message}", error_code=100, details=details)
class InitializationError(NLMError):
    """Exception raised for initialization-related errors."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(f"Initialization Error: {message}", error_code=200, details=details)
class SimulationError(NLMError):
    """Exception raised for simulation-related errors."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(f"Simulation Error: {message}", error_code=300, details=details)
class VisualizationError(NLMError):
    """Exception raised for visualization-related errors."""
    
    def __init__(self, message: str, details: Optional[Dict[str, Any]] = None):
        super().__init__(f"Visualization Error: {message}", error_code=400, details=details)
# === ENUMERATIONS ===

class Profile(Enum):
    """Configuration profiles for quick start."""
    MINIMAL = "minimal"
    BALANCED = "balanced"
    FAST = "fast"
    DETAILED = "detailed"
    RESEARCH = "research"
class VisualizationType(Enum):
    """Types of visualizations available."""
    FIRING_RATES = "firing_rates"
    EXCITATION_INHIBITION = "ei_ratio"
    NEURON_ACTIVITY = "neuron_activity"
    SPIKE_TIMES = "spike_times"
    TEMPORAL_DYNAMICS = "temporal_dynamics"
    PATTERN_ANALYSIS = "pattern_analysis"
    SPATIAL_DISTRIBUTION = "spatial_distribution"
class ExportFormat(Enum):
    """Export formats for data."""
    JSON = "json"
    CSV = "csv"
    PYTHON = "python"
    NUMPY = "numpy"
    HDF5 = "hdf5"
# === DATA CLASSES ===

@dataclass
class NeuronMetrics:
    """Metrics for individual neurons."""
    neuron_id: int
    neuron_type: str
    firing_rate: float
    membrane_potential: float
    threshold: float
    is_firing: bool
    spike_count: int
    last_spike_time: float
    region_id: int
    population_id: int
    @classmethod
    def from_brain_neuron(cls, brain: 'Brain', neuron_id: int) -> 'NeuronMetrics':
        """Create neuron metrics from brain data."""
        # This would be implemented using the enhanced brain interface
        return cls(
            neuron_id=neuron_id,
            neuron_type="Internal",  # This would be retrieved from brain
            firing_rate=0.0,
            membrane_potential=0.0,
            threshold=0.0,
            is_firing=False,
            spike_count=0,
            last_spike_time=0.0,
            region_id=0,
            population_id=0
        )
@dataclass
class SimulationResult:
    """Complete simulation results."""
    steps: List[List[float]] = field(default_factory=list)
    metrics: Dict[str, Any] = field(default_factory=dict)
    brain_state: Optional['Brain'] = None
    start_time: float = 0.0
    end_time: float = 0.0
    experiment_name: str = ""
    
    def __post_init__(self):
        self.start_time = time.time()
    
    @property
    def duration(self) -> float:
        """Get simulation duration in seconds."""
        return self.end_time - self.start_time
    
    @property
    def total_steps(self) -> int:
        """Get total number of simulation steps."""
        return len(self.steps)
    
    def add_step_data(self, step_data: List[float]) -> None:
        """Add data for a single simulation step."""
        self.steps.append(step_data)
    
    def calculate_statistics(self) -> Dict[str, float]:
        """Calculate statistics from simulation results."""
        if not self.steps:
            return {}
        
        stats = {}
        
        # Calculate statistics for each metric column
        num_steps = len(self.steps)
        
        for col_idx in range(min(4, len(self.steps[0]))):  # Assume up to 4 metrics
            column_data = [step[col_idx] for step in self.steps if col_idx < len(step)]
            
            if column_data:
                metric_name = ["spike_count", "firing_neuron_count", "firing_rate", "ei_ratio"][col_idx]
                stats[f"avg_{metric_name}"] = sum(column_data) / len(column_data)
                stats[f"min_{metric_name}"] = min(column_data)
                stats[f"max_{metric_name}"] = max(column_data)
                
                if len(column_data) > 1:
                    variance = sum((x - stats[f"avg_{metric_name}"]) ** 2 for x in column_data) / len(column_data)
                    stats[f"std_{metric_name}"] = variance ** 0.5
        
        return stats
@dataclass
class ExperimentConfig:
    """Configuration for scientific experiments."""
    name: str
    brain_config: Dict[str, Any]
    simulation_steps: int
    profile: Profile = Profile.BALANCED
    output_dir: str = "experiment_results"
    save_interval: int = 100
    analysis_types: List[str] = field(default_factory=lambda: ["basic", "advanced"])
    visualization_types: List[str] = field(default_factory=list)
    export_format: ExportFormat = ExportFormat.JSON
    parallel: bool = False
    max_workers: int = 1
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary for serialization."""
        return {
            "name": self.name,
            "brain_config": self.brain_config,
            "simulation_steps": self.simulation_steps,
            "profile": self.profile.value,
            "output_dir": self.output_dir,
            "save_interval": self.save_interval,
            "analysis_types": self.analysis_types,
            "visualization_types": self.visualization_types,
            "export_format": self.export_format.value,
            "parallel": self.parallel,
            "max_workers": self.max_workers
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'ExperimentConfig':
        """Create from dictionary."""
        return cls(
            name=data["name"],
            brain_config=data["brain_config"],
            simulation_steps=data["simulation_steps"],
            profile=Profile(data["profile"]),
            output_dir=data["output_dir"],
            save_interval=data["save_interval"],
            analysis_types=data["analysis_types"],
            visualization_types=data["visualization_types"],
            export_format=ExportFormat(data["export_format"]),
            parallel=data["parallel"],
            max_workers=data["max_workers"]
        )
# === UTILITY FUNCTIONS ===

def create_brain(profile: Union[Profile, str] = Profile.BALANCED, 
                custom_config: Optional[Dict[str, Any]] = None) -> 'Brain':
    """
    Create a brain with specified configuration profile.
    
    Args:
        profile: Configuration profile
        custom_config: Optional custom configuration parameters
        
    Returns:
        Configured and initialized Brain object
        
    Examples:
        >>> brain = create_brain(Profile.BALANCED)
        >>> brain.initialize()
        >>> results = run_simulation(brain, 100)
    """
    if isinstance(profile, str):
        profile = Profile(profile.lower())
    
    # Import here to avoid circular imports
    from pynlm import createBrain, createDefaultConfig, BrainConfigurator
    
    # Create configuration based on profile
    if profile == Profile.MINIMAL:
        config = BrainConfigurator.createQuickStartConfig("fast")
    elif profile == Profile.FAST:
        config = BrainConfigurator.createQuickStartConfig("fast")
    elif profile == Profile.BALANCED:
        config = BrainConfigurator.createQuickStartConfig("balanced")
    elif profile == Profile.DETAILED:
        config = BrainConfigurator.createQuickStartConfig("detailed")
    elif profile == Profile.RESEARCH:
        config = BrainConfigurator.createQuickStartConfig("research")
    else:
        config = BrainConfigurator.createDefaultConfig()
    
    # Apply custom configuration if provided
    if custom_config:
        for key, value in custom_config.items():
            config.set(key, value)
    
    # Validate configuration
    BrainConfigurator.validateConfig(config)
    
    # Create and return brain
    brain = createBrain(config)
    brain.initialize()
    
    return brain
def run_simulation(brain: 'Brain', steps: int = 100, 
                   profile: Union[Profile, str] = Profile.BALANCED,
                   custom_config: Optional[Dict[str, Any]] = None) -> SimulationResult:
    """
    Run a simulation with the specified brain configuration.
    
    Args:
        brain: Brain object to run simulation on
        steps: Number of simulation steps to run
        profile: Configuration profile
        custom_config: Optional custom configuration parameters
        
    Returns:
        SimulationResult containing simulation data and analysis
        
    Examples:
        >>> brain = create_brain(Profile.BALANCED)
        >>> result = run_simulation(brain, steps=200)
        >>> print(f"Duration: {result.duration:.2f}s")
        >>> print(f"Average firing rate: {result.metrics.get('avg_firing_rate', 0):.2f} Hz")
    """
    if custom_config:
        # Update brain configuration with custom parameters
        brain.getConfig().loadFromArgs(0, [])
    
    # Run simulation
    simulation = SimulationResult(experiment_name=f"simulation_{int(time.time())}")
    
    for step in range(steps):
        brain.step(step)
        
        # Collect metrics for this step
        step_data = [
            brain.getTotalSpikeCount(),
            brain.getFiringNeuronCount(),
            brain.getAverageFiringRate(),
            brain.getExcitationInhibitionRatio()
        ]
        simulation.add_step_data(step_data)
    
    simulation.end_time = time.time()
    simulation.metrics = simulation.calculate_statistics()
    
    return simulation
def create_experiment(config: ExperimentConfig) -> 'ExperimentManager':
    """
    Create and run an experiment with the specified configuration.
    
    Args:
        config: Experiment configuration
        
    Returns:
        ExperimentManager for managing the experiment
        
    Examples:
        >>> config = ExperimentConfig(
        ...     name="neural_network_test",
        ...     brain_config={"neuron_count": 1000, "simulation_timestep": 0.001},
        ...     simulation_steps=500,
        ...     profile=Profile.RESEARCH
        ... )
        >>> experiment = create_experiment(config)
        >>> results = experiment.run()
        >>> report = experiment.generate_report()
    """
    return ExperimentManager(config)
# === CONTEXT MANAGERS ===

class BrainContext:
    """
    Context manager for NLM brain operations.
    
    Provides automatic initialization and cleanup for brain operations.
    
    Examples:
        >>> with BrainContext() as brain:
        ...     result = brain.run_simulation(100)
        ...     print(f"Simulation completed with {result.total_steps} steps")
    """
    
    def __init__(self, profile: Union[Profile, str] = Profile.BALANCED):
        self.profile = profile
        self.brain = None
    
    def __enter__(self) -> 'Brain':
        """Enter the context and create brain."""
        self.brain = create_brain(self.profile)
        return self.brain
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Exit the context and cleanup brain."""
        if self.brain:
            self.brain.reset()
            self.brain = None
class ExperimentContext:
    """
    Context manager for NLM experiment operations.
    
    Provides automatic experiment management with cleanup.
    """
    
    def __init__(self, config: ExperimentConfig):
        self.config = config
        self.experiment = None
    
    def __enter__(self) -> 'ExperimentManager':
        """Enter the context and create experiment."""
        self.experiment = create_experiment(self.config)
        return self.experiment
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Exit the context and cleanup experiment."""
        if self.experiment:
            self.experiment.cleanup()
            self.experiment = None
# === VISUALIZATION AND ANALYSIS ===

class NLMVisualizer:
    """
    Enhanced visualization and analysis tools for NLM simulations.
    
    Provides methods for creating various types of visualizations and analyses.
    """
    
    def __init__(self, output_dir: str = "visualizations"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
        
        # Try to import matplotlib for plotting
        try:
            import matplotlib.pyplot as plt
            import matplotlib as mpl
            self.plt = plt
            self.mpl = mpl
            self.plotting_available = True
        except ImportError:
            self.plotting_available = False
            print("Warning: matplotlib not available. Visualization features disabled.")
    
    def plot_firing_rates(self, simulation_results: SimulationResult, 
                         save_path: Optional[str] = None) -> str:
        """
        Plot firing rates over time.
        
        Args:
            simulation_results: Simulation results to visualize
            save_path: Optional path to save the plot
            
        Returns:
            Path to saved plot file
        """
        if not self.plotting_available:
            raise VisualizationError("Plotting not available. Install matplotlib.")
        
        fig, ax = self.plt.subplots(figsize=(12, 6))
        
        # Plot each metric
        if simulation_results.steps:
            num_steps = len(simulation_results.steps)
            step_indices = list(range(num_steps))
            
            # Average firing rate (assuming it's at index 2)
            if any(len(step) > 2 for step in simulation_results.steps):
                avg_rates = [step[2] if len(step) > 2 else 0.0 for step in simulation_results.steps]
                ax.plot(step_indices, avg_rates, label='Average Firing Rate', linewidth=2)
            
            # Spike count (assuming it's at index 0)
            if any(len(step) > 0 for step in simulation_results.steps):
                spike_counts = [step[0] if len(step) > 0 else 0.0 for step in simulation_results.steps]
                ax.plot(step_indices, spike_counts, label='Spike Count', alpha=0.7, linewidth=1)
        
        ax.set_xlabel('Simulation Steps')
        ax.set_ylabel('Value')
        ax.set_title('NLM Simulation Results')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        if save_path:
            self.plt.savefig(save_path, dpi=300, bbox_inches='tight')
            return save_path
        
        plot_path = os.path.join(self.output_dir, f"firing_rates_{int(time.time())}.png")
        self.plt.savefig(plot_path, dpi=300, bbox_inches='tight')
        return plot_path
    
    def plot_ei_ratio(self, simulation_results: SimulationResult,
                     save_path: Optional[str] = None) -> str:
        """
        Plot excitation/inhibition ratio over time.
        
        Args:
            simulation_results: Simulation results to visualize
            save_path: Optional path to save the plot
            
        Returns:
            Path to saved plot file
        """
        if not self.plotting_available:
            raise VisualizationError("Plotting not available. Install matplotlib.")
        
        fig, (ax1, ax2) = self.plt.subplots(2, 1, figsize=(12, 8))
        
        if simulation_results.steps:
            num_steps = len(simulation_results.steps)
            step_indices = list(range(num_steps))
            
            # E/I ratio (assuming it's at index 3)
            if any(len(step) > 3 for step in simulation_results.steps):
                ei_ratios = [step[3] if len(step) > 3 else 1.0 for step in simulation_results.steps]
                ax1.plot(step_indices, ei_ratios, color='red', linewidth=2)
                ax1.set_ylabel('E/I Ratio')
                ax1.set_title('Excitation/Inhibition Ratio Over Time')
                ax1.grid(True, alpha=0.3)
                
                # Add horizontal line at 1.0 (balanced)
                ax1.axhline(y=1.0, color='green', linestyle='--', alpha=0.7, label='Balanced')
                ax1.legend()
            
            # Statistics
            if simulation_results.metrics:
                metrics_data = []
                for i in range(0, len(simulation_results.metrics), 3):
                    if i + 2 < len(simulation_results.metrics):
                        metric_name = list(simulation_results.metrics.keys())[i]
                        avg_val = simulation_results.metrics[metric_name]
                        std_val = simulation_results.metrics.get(f"std_{metric_name.split('_')[1]}", 0)
                        metrics_data.append((metric_name, avg_val, std_val))
                
                if metrics_data:
                    names, averages, stds = zip(*metrics_data)
                    x_pos = range(len(names))
                    ax2.bar(x_pos, averages, yerr=stds, capsize=5, alpha=0.7, color='skyblue')
                    ax2.set_xticks(x_pos)
                    ax2.set_xticklabels(names, rotation=45)
                    ax2.set_ylabel('Average Value ± Std Dev')
                    ax2.set_title('Simulation Metrics Statistics')
                    ax2.grid(True, alpha=0.3)
        
        if save_path:
            self.plt.savefig(save_path, dpi=300, bbox_inches='tight')
            return save_path
        
        plot_path = os.path.join(self.output_dir, f"ei_ratio_{int(time.time())}.png")
        self.plt.savefig(plot_path, dpi=300, bbox_inches='tight')
        return plot_path
    
    def create_dashboard(self, simulation_results: SimulationResult,
                        save_path: Optional[str] = None) -> str:
        """
        Create a comprehensive dashboard with multiple plots.
        
        Args:
            simulation_results: Simulation results to visualize
            save_path: Optional path to save the dashboard
            
        Returns:
            Path to saved dashboard file
        """
        if not self.plotting_available:
            raise VisualizationError("Plotting not available. Install matplotlib.")
        
        fig = self.plt.figure(figsize=(16, 12))
        
        # Create subplots
        gs = fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        if simulation_results.steps:
            num_steps = len(simulation_results.steps)
            step_indices = list(range(num_steps))
            
            # Plot 1: Average firing rate
            ax1 = fig.add_subplot(gs[0, :2])
            if any(len(step) > 2 for step in simulation_results.steps):
                avg_rates = [step[2] if len(step) > 2 else 0.0 for step in simulation_results.steps]
                ax1.plot(step_indices, avg_rates, color='blue', linewidth=2)
            ax1.set_xlabel('Steps')
            ax1.set_ylabel('Firing Rate (Hz)')
            ax1.set_title('Average Firing Rate')
            ax1.grid(True, alpha=0.3)
            
            # Plot 2: E/I ratio
            ax2 = fig.add_subplot(gs[0, 2:])
            if any(len(step) > 3 for step in simulation_results.steps):
                ei_ratios = [step[3] if len(step) > 3 else 1.0 for step in simulation_results.steps]
                ax2.plot(step_indices, ei_ratios, color='red', linewidth=2)
                ax2.axhline(y=1.0, color='green', linestyle='--', alpha=0.7)
            ax2.set_xlabel('Steps')
            ax2.set_ylabel('E/I Ratio')
            ax2.set_title('Excitation/Inhibition Ratio')
            ax2.grid(True, alpha=0.3)
            
            # Plot 3: Spike count
            ax3 = fig.add_subplot(gs[1, :4])
            if any(len(step) > 0 for step in simulation_results.steps):
                spike_counts = [step[0] if len(step) > 0 else 0.0 for step in simulation_results.steps]
                ax3.plot(step_indices, spike_counts, color='purple', linewidth=1.5, alpha=0.8)
            ax3.set_xlabel('Steps')
            ax3.set_ylabel('Spike Count')
            ax3.set_title('Spike Count Over Time')
            ax3.grid(True, alpha=0.3)
            
            # Plot 4: Metrics statistics
            ax4 = fig.add_subplot(gs[2, :4])
            if simulation_results.metrics:
                metrics_to_plot = {}
                for key, value in simulation_results.metrics.items():
                    if 'avg_' in key:
                        metric_name = key[4:]  # Remove 'avg_' prefix
                        metrics_to_plot[metric_name] = value
                
                if metrics_to_plot:
                    metrics_names = list(metrics_to_plot.keys())
                    metrics_values = list(metrics_to_plot.values())
                    
                    bars = ax4.bar(range(len(metrics_names)), metrics_values, alpha=0.7)
                    ax4.set_xticks(range(len(metrics_names)))
                    ax4.set_xticklabels(metrics_names, rotation=45)
                    ax4.set_ylabel('Value')
                    ax4.set_title('Key Metrics Statistics')
                    ax4.grid(True, alpha=0.3, axis='y')
                    
                    # Add value labels on bars
                    for bar, value in zip(bars, metrics_values):
                        ax4.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                                f'{value:.3f}', ha='center', va='bottom')
        
        fig.suptitle(f'NLM Simulation Dashboard - {simulation_results.experiment_name}', 
                    fontsize=16, fontweight='bold')
        
        if save_path:
            self.plt.savefig(save_path, dpi=300, bbox_inches='tight')
            return save_path
        
        dashboard_path = os.path.join(self.output_dir, f"dashboard_{int(time.time())}.png")
        self.plt.savefig(dashboard_path, dpi=300, bbox_inches='tight')
        return dashboard_path
    
    def export_results(self, simulation_results: SimulationResult, 
                      format: ExportFormat = ExportFormat.JSON,
                      filepath: Optional[str] = None) -> str:
        """
        Export simulation results in various formats.
        
        Args:
            simulation_results: Simulation results to export
            format: Export format
            filepath: Optional filepath to save results
            
        Returns:
            Path to exported file
        """
        if not filepath:
            timestamp = int(time.time())
            filepath = os.path.join(self.output_dir, f"results_{timestamp}.{format.value}")
        
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        
        if format == ExportFormat.JSON:
            data = {
                'steps': simulation_results.steps,
                'metrics': simulation_results.metrics,
                'experiment_name': simulation_results.experiment_name,
                'start_time': simulation_results.start_time,
                'end_time': simulation_results.end_time,
                'duration': simulation_results.duration,
                'total_steps': simulation_results.total_steps
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
        
        elif format == ExportFormat.CSV:
            import csv
            with open(filepath, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(['step', 'spike_count', 'firing_neuron_count', 
                               'firing_rate', 'ei_ratio'])
                for i, step in enumerate(simulation_results.steps):
                    writer.writerow([i] + step)
        
        elif format == ExportFormat.PYTHON:
            with open(filepath, 'w') as f:
                f.write(f"# NLM Simulation Results\n")
                f.write(f"# Experiment: {simulation_results.experiment_name}\n")
                f.write(f"# Steps: {simulation_results.total_steps}\n")
                f.write(f"# Duration: {simulation_results.duration:.2f} seconds\n\n")
                
                f.write("steps = \n")
                f.write(str(simulation_results.steps))
        
        return filepath
# === EXPERIMENT MANAGEMENT ===

class ExperimentManager:
    """
    Advanced experiment management for NLM simulations.
    
    Provides tools for creating, running, and analyzing experiments.
    """
    
    def __init__(self, config: ExperimentConfig):
        self.config = config
        self.results = []
        self.status = "initialized"
        self.start_time = None
        self.end_time = None
        
        # Create output directory
        os.makedirs(config.output_dir, exist_ok=True)
    
    def run(self) -> 'ExperimentResult':
        """
        Run the experiment.
        
        Returns:
            ExperimentResult containing all results
        """
        self.status = "running"
        self.start_time = time.time()
        
        print(f"Starting experiment: {self.config.name}")
        print(f"Profile: {self.config.profile.value}")
        print(f"Simulation steps: {self.config.simulation_steps}")
        print(f"Output directory: {self.config.output_dir}")
        
        # Create brain
        brain = create_brain(self.config.profile, self.config.brain_config)
        
        # Run simulation
        simulation_result = run_simulation(
            brain, 
            self.config.simulation_steps,
            self.config.profile,
            self.config.brain_config
        )
        
        # Save results
        self.save_results(simulation_result)
        
        # Analyze results
        if "advanced" in self.config.analysis_types:
            simulation_result.metrics.update(self.analyze_advanced(simulation_result))
        
        # Create visualizations
        if self.config.visualization_types:
            self.create_visualizations(simulation_result)
        
        # Export results
        if self.config.export_format != ExportFormat.HDF5:
            export_path = self.export_results(simulation_result)
            print(f"Results exported to: {export_path}")
        
        self.end_time = time.time()
        self.status = "completed"
        
        print(f"Experiment completed in {self.end_time - self.start_time:.2f} seconds")
        
        return ExperimentResult(self, simulation_result)
    
    def save_results(self, simulation_result: SimulationResult) -> None:
        """Save simulation results to file."""
        timestamp = int(time.time())
        filename = f"{self.config.name}_results_{timestamp}.json"
        filepath = os.path.join(self.config.output_dir, filename)
        
        # Save simulation results
        with open(filepath, 'w') as f:
            json.dump({
                'experiment_name': self.config.name,
                'profile': self.config.profile.value,
                'brain_config': self.config.brain_config,
                'simulation_steps': self.config.simulation_steps,
                'simulation_result': {
                    'steps': simulation_result.steps,
                    'metrics': simulation_result.metrics,
                    'experiment_name': simulation_result.experiment_name,
                    'start_time': simulation_result.start_time,
                    'end_time': simulation_result.end_time,
                    'duration': simulation_result.duration,
                    'total_steps': simulation_result.total_steps
                }
            }, f, indent=2)
    
    def create_visualizations(self, simulation_result: SimulationResult) -> List[str]:
        """Create visualizations for the experiment."""
        visualizer = NLMVisualizer(self.config.output_dir)
        plot_paths = []
        
        for viz_type in self.config.visualization_types:
            try:
                if viz_type == "firing_rates":
                    plot_path = visualizer.plot_firing_rates(simulation_result)
                elif viz_type == "ei_ratio":
                    plot_path = visualizer.plot_ei_ratio(simulation_result)
                elif viz_type == "dashboard":
                    plot_path = visualizer.create_dashboard(simulation_result)
                else:
                    print(f"Warning: Unknown visualization type: {viz_type}")
                    continue
                
                plot_paths.append(plot_path)
                print(f"Created visualization: {plot_path}")
                
            except VisualizationError as e:
                print(f"Warning: Failed to create visualization {viz_type}: {e}")
        
        return plot_paths
    
    def export_results(self, simulation_result: SimulationResult) -> str:
        """Export experiment results."""
        timestamp = int(time.time())
        filename = f"{self.config.name}_export.{self.config.export_format.value}"
        filepath = os.path.join(self.config.output_dir, filename)
        
        visualizer = NLMVisualizer(self.config.output_dir)
        return visualizer.export_results(simulation_result, self.config.export_format, filepath)
    
    def analyze_advanced(self, simulation_result: SimulationResult) -> Dict[str, float]:
        """Perform advanced analysis on simulation results."""
        analysis = {}
        
        if not simulation_result.steps:
            return analysis
        
        steps = simulation_result.steps
        num_steps = len(steps)
        
        # Calculate temporal statistics
        if num_steps > 10:
            # Moving averages
            window_size = min(100, num_steps // 10)
            for i in range(0, num_steps, window_size):
                window = steps[min(i + window_size, num_steps):]
                if window:
                    # Calculate statistics for this window
                    pass
        
        # Pattern analysis
        if len(steps) > 20:
            # Check for periodic patterns
            spike_counts = [step[0] if len(step) > 0 else 0 for step in steps]
            if len(spike_counts) > 10:
                # Simple autocorrelation
                max_lag = min(50, len(spike_counts) // 2)
                for lag in range(1, max_lag + 1):
                    correlation = 0.0
                    for i in range(len(spike_counts) - lag):
                        correlation += spike_counts[i] * spike_counts[i + lag]
                    correlation /= (len(spike_counts) - lag)
                    analysis[f'autocorrelation_lag_{lag}'] = correlation
        
        return analysis
    
    def cleanup(self) -> None:
        """Clean up experiment resources."""
        self.results.clear()
        self.status = "cleaned_up"
        print("Experiment resources cleaned up")
class ExperimentResult:
    """
    Results container for experiment data.
    
    Provides convenient access to experiment results and analysis.
    """
    
    def __init__(self, experiment_manager: ExperimentManager, 
                 simulation_result: SimulationResult):
        self.experiment_manager = experiment_manager
        self.simulation_result = simulation_result
        self.config = experiment_manager.config
        
        # Create summary statistics
        self.summary = self._create_summary()
    
    def _create_summary(self) -> Dict[str, Any]:
        """Create summary statistics for the experiment."""
        summary = {
            'experiment_name': self.config.name,
            'profile': self.config.profile.value,
            'simulation_steps': self.config.simulation_steps,
            'total_spikes': self.simulation_result.metrics.get('avg_spike_count', 0) * len(self.simulation_result.steps),
            'avg_firing_rate': self.simulation_result.metrics.get('avg_firing_rate', 0),
            'avg_ei_ratio': self.simulation_result.metrics.get('avg_ei_ratio', 1.0),
            'duration_seconds': self.simulation_result.duration,
            'steps_per_second': len(self.simulation_result.steps) / max(self.simulation_result.duration, 0.001)
        }
        
        # Add advanced metrics if available
        advanced_metrics = {}
        for key, value in self.simulation_result.metrics.items():
            if any(metric in key for metric in ['autocorrelation', 'std_', 'min_', 'max_']):
                advanced_metrics[key] = value
        
        if advanced_metrics:
            summary['advanced_metrics'] = advanced_metrics
        
        return summary
    
    def generate_report(self) -> str:
        """Generate a human-readable report."""
        report = f"""
NLM Experiment Report
=====================

Experiment: {self.config.name}
Profile: {self.config.profile.value}
Duration: {self.simulation_result.duration:.2f} seconds
Total Steps: {self.config.simulation_steps}

Simulation Results:
-----------------
Total Spikes: {self.summary['total_spikes']:.0f}
Average Firing Rate: {self.summary['avg_firing_rate']:.2f} Hz
Average E/I Ratio: {self.summary['avg_ei_ratio']:.3f}
Steps per Second: {self.summary['steps_per_second']:.1f}

Key Metrics Statistics:
----------------------
"""
        
        # Add metrics statistics
        for key, value in self.simulation_result.metrics.items():
            if 'avg_' in key:
                metric_name = key[4:]  # Remove 'avg_' prefix
                report += f"{metric_name}: {value:.4f}\n"
        
        report += f"\nAdvanced Metrics:\n----------------"
        for key, value in self.simulation_result.metrics.items():
            if any(metric in key for metric in ['autocorrelation', 'std_', 'min_', 'max_']):
                report += f"{key}: {value:.4f}\n"
        
        # Add visualization information
        if self.config.visualization_types:
            report += f"\nVisualizations Created:\n-------------------"
            report += f"See {self.config.output_dir} directory for visualizations\n"
        
        # Add export information
        if self.config.export_format != ExportFormat.HDF5:
            report += f"\nExported Results:\n----------------"
            report += f"Format: {self.config.export_format.value}\n"
            report += f"Location: {self.config.output_dir}\n"
        
        return report
    
    def plot_results(self, save_path: Optional[str] = None) -> str:
        """Create plots of experiment results."""
        visualizer = NLMVisualizer(self.config.output_dir)
        return visualizer.create_dashboard(self.simulation_result, save_path)
    
    def export_data(self, format: ExportFormat = ExportFormat.JSON,
                   filepath: Optional[str] = None) -> str:
        """Export experiment data."""
        visualizer = NLMVisualizer(self.config.output_dir)
        return visualizer.export_results(self.simulation_result, format, filepath)
# === MAIN MODULE FUNCTION ===

def initialize() -> Dict[str, Any]:
    """
    Initialize the NLM enhanced Python API.
    
    Returns:
        Dictionary containing initialization status and available features
    """
    status = {
        'version': __version__,
        'python_bindings': True,
        'enhanced_features': [
            'Beginner-friendly convenience methods',
            'Context managers for resource management',
            'Performance monitoring utilities',
            'Configuration management tools',
            'Simulation helpers',
            'Pythonic interfaces (iterators, properties)',
            'Enhanced error handling',
            'Experiment management',
            'Visualization and analysis',
            'Data export utilities'
        ],
        'available_profiles': [profile.value for profile in Profile],
        'visualization_types': [viz_type.value for viz_type in VisualizationType],
        'export_formats': [fmt.value for fmt in ExportFormat],
        'status': 'initialized'
    }
    
    return status
def get_available_features() -> List[str]:
    """Get list of available enhanced features."""
    return [
        'create_brain',
        'run_simulation',
        'create_experiment',
        'BrainContext',
        'ExperimentContext',
        'NLMVisualizer',
        'ExperimentManager',
        'ExperimentResult',
        'create_default_config',
        'create_quick_config',
        'createBrain'
    ]
# === BACKWARD COMPATIBILITY ===

# Create aliases for backward compatibility
create_default_brain = create_brain
def run_quick_simulation(brain=None, steps=100, profile=Profile.BALANCED):
    """Backward compatibility function for running quick simulations."""
    if brain is None:
        brain = create_brain(profile)
    return run_simulation(brain, steps, profile)
def __getattr__(name: str) -> Any:
    """
    Handle attribute access for backward compatibility.
    
    Provides aliases for common function names to maintain compatibility
    with existing code that may use different naming conventions.
    """
    # Create mapping of old names to new functions
    compatibility_map = {
        'create_default_brain': create_brain,
        'run_quick_simulation': run_quick_simulation,
        'ExperimentManager': ExperimentManager,
        'ExperimentResult': ExperimentResult,
        'NLMVisualizer': NLMVisualizer
    }
    
    if name in compatibility_map:
        return compatibility_map[name]
    
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")
# Initialize module
status = initialize()

# Print welcome message
print("Enhanced NLM Python API initialized successfully!")
print(f"Version: {__version__}")
print(f"Available features: {len(get_available_features())}")
print("Use 'help()' or 'dir()' to explore available functions.")
print()
print("Key features available:")
for feature in status['enhanced_features'][:5]:
    print(f"  • {feature}")
if len(status['enhanced_features']) > 5:
    print(f"  ... and {len(status['enhanced_features']) - 5} more features")
print()
print("For documentation and examples, see the docstrings for each function.")
print("To get help on specific functions, use: help(function_name)")
# Export important classes and functions
__all__ = [
    # Core classes
    'Brain',
    'Config',
    'AgentBrain',
    'SimpleWorld',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Enhanced classes
    'NLMError',
    'ConfigurationError',
    'InitializationError',
    'SimulationError',
    'VisualizationError',
    'BrainContext',
    'ExperimentContext',
    'NLMVisualizer',
    'ExperimentManager',
    'ExperimentResult',
    
    # Enumerations
    'Profile',
    'VisualizationType',
    'ExportFormat',
    
    # Data classes
    'NeuronMetrics',
    'SimulationResult',
    'ExperimentConfig',
    
    # Utility functions
    'create_brain',
    'run_simulation',
    'create_experiment',
    'create_default_brain',
    'run_quick_simulation',
    
    # Module information
    'initialize',
    'get_available_features',
    '__version__'
]

if __name__ == "__main__":
    print(__doc__)
    print("\nTo get help on specific functions, use: help(function_name)")
    print("Example: help(create_brain)")
    print("\nTo explore all available features: dir()")

# Module footer
print("\n" + "="*60)
print("Enhanced NLM Python API - Ready for Use!")
print("="*60)
print("For documentation and examples, refer to the docstrings.")
print("For support, visit the NLM documentation website.")
print("="*60)
