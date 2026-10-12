"""
NLM Python Utilities
====================

Utility functions and helper classes for the NLM (Neural Learning Machine) Python bindings.

This module provides convenient utilities for working with NLM simulations, including
tools for simulation management, progress tracking, experiment control, and data handling.
"""

from typing import Callable, Optional, Dict, Any, List
import json
import time
import os

# Import from pynlm
from .pynlm import (
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio, 
    InternalSignals, Action, SensoryPercept, WorldObject,
    ActionResult, AgentBody, NeuronId, SynapseId, RegionId,
    PopulationId, NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType,
    NLMException, NLMConfigError, NLMBrainError, NLMWorldError,
    NLMSimulationError, NLMMemoryError, ProgressTracker, INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
)
class Experiment:
    """Experiment management utility for NLM simulations.
    
    This class provides a convenient way to manage NLM experiments, including
    setup, execution, monitoring, and results collection.
    
    Example:
        >>> import pynlm
        >>> import pynlm.utils as nlm
        >>> 
        >>> # Create experiment configuration
        >>> config = nlm.create_default_config()
        >>> config.set("learning_rate", 0.01)
        >>> config.set("num_neurons", 1000)
        >>> 
        >>> # Create experiment
        >>> experiment = nlm.Experiment(config)
        >>> 
        >>> # Define progress callback
        >>> def progress_callback(progress, message):
        ...     print(f"Progress: {progress:.1%}: {message}")
        >>> 
        >>> # Run experiment
        >>> experiment.run(world=None, progress_callback=progress_callback)
        >>> 
        >>> # Analyze results
        >>> stats = experiment.get_statistics()
        >>> print(f"Final spike count: {stats['total_spikes']}")
    """
    
    def __init__(self, config: Config, name: Optional[str] = None):
        """Initialize a new experiment.
        
        Args:
            config: Configuration for the experiment
            name: Optional name for the experiment
        """
        self.config = config
        self.name = name or f"experiment_{int(time.time())}"
        self.brain: Optional[Brain] = None
        self.world: Optional[SimpleWorld] = None
        self.agent_brain: Optional[AgentBrain] = None
        self.steps_completed = 0
        self.results: Dict[str, Any] = {}
        self.start_time: Optional[float] = None
        self.end_time: Optional[float] = None
    
    def setup(self, world: bool = False, agent_brain: bool = False):
        """Setup the experiment components.
        
        Args:
            world: Whether to create a SimpleWorld instance
            agent_brain: Whether to create an AgentBrain interface
        """
        self.brain = Brain(self.config)
        self.brain.initialize()
        
        if world:
            self.world = SimpleWorld()
        
        if agent_brain and self.brain:
            self.agent_brain = AgentBrain(self.brain)
    
    def run(self, world: Optional[SimpleWorld] = None, 
            progress_callback: Optional[Callable[[float, str], None]] = None,
            steps: int = 100, timestep: float = 1.0):
        """Run the experiment.
        
        Args:
            world: Optional world instance to use
            progress_callback: Optional callback for progress updates
            steps: Number of simulation steps to run
            timestep: Duration of each timestep
        """
        if not self.brain:
            raise NLMException("Experiment not setup. Call setup() first.")
        
        # Use provided world or instance variable
        if world:
            self.world = world
        
        if not self.world:
            raise NLMException("No world available. Either create one during setup or pass it to run().")
        
        self.start_time = time.time()
        self.steps_completed = 0
        
        # Create progress tracker if callback is provided
        tracker = None
        if progress_callback:
            tracker = ProgressTracker()
            tracker.setCallback(progress_callback)
            tracker.setMaxSteps(steps)
        
        # Reset and initialize
        self.brain.reset()
        self.world.reset()
        
        # Run simulation steps
        for step in range(steps):
            if tracker:
                tracker.incrementStep(f"Step {step + 1}")
            
            # Process sensory input if world is available
            if self.world:
                percept = self.world.getSensoryPercept()
                if self.agent_brain:
                    self.agent_brain.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # World update
            if self.world:
                self.world.update(timestep)
            
            self.steps_completed = step + 1
        
        self.end_time = time.time()
        
        # Store results
        self.results['steps_completed'] = self.steps_completed
        self.results['duration'] = self.end_time - self.start_time
        self.results['brain_stats'] = self._collect_brain_statistics()
        
        return self
    
    def _collect_brain_statistics(self) -> Dict[str, Any]:
        """Collect brain statistics for the experiment."""
        if not self.brain:
            return {}
        
        return {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'ei_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': str(self.brain.getDevelopmentalStage())
        }
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get experiment statistics.
        
        Returns:
            Dictionary containing experiment statistics
        """
        stats = {
            'name': self.name,
            'steps_completed': self.steps_completed,
            'duration_seconds': self.end_time - self.start_time if self.end_time and self.start_time else None,
            'results': self.results
        }
        stats['brain_stats'] = self._collect_brain_statistics()
        return stats
    
    def save(self, filepath: str):
        """Save experiment results and brain state.
        
        Args:
            filepath: Path to save experiment data
        """
        if not self.brain:
            raise NLMException("No brain to save. Run experiment first.")
        
        # Create results directory if it doesn't exist
        os.makedirs(os.path.dirname(os.path.abspath(filepath)), exist_ok=True)
        
        # Save experiment metadata
        metadata = self.get_statistics()
        
        # Save brain state
        brain_filepath = os.path.join(os.path.dirname(filepath), "brain_state.nml")
        if self.brain.save(brain_filepath):
            metadata['brain_saved'] = True
            metadata['brain_filepath'] = brain_filepath
        else:
            metadata['brain_saved'] = False
        
        # Save JSON results
        json_filepath = os.path.join(os.path.dirname(filepath), "experiment_results.json")
        with open(json_filepath, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        metadata['results_filepath'] = json_filepath
        
        return metadata
    
    def load(self, filepath: str):
        """Load experiment data from file.
        
        Args:
            filepath: Path to experiment data file
        """
        # This is a placeholder - actual implementation would depend on format
        raise NotImplementedError("Loading experiments from file is not yet implemented")
class DataHandler:
    """Data handling utilities for NLM simulations.
    
    This class provides tools for saving, loading, and manipulating simulation data,
    including spike trains, neural activity patterns, and sensory inputs.
    """
    
    @staticmethod
    def save_simulation_data(brain: Brain, filepath: str, 
                           include_spikes: bool = True,
                           include_sensory: bool = True,
                           include_neurons: bool = False):
        """Save simulation data to file.
        
        Args:
            brain: Brain instance to extract data from
            filepath: Output file path
            include_spikes: Whether to include spike data
            include_sensory: Whether to include sensory data
            include_neurons: Whether to include detailed neuron data
            
        Returns:
            bool: True if save was successful
        """
        try:
            os.makedirs(os.path.dirname(os.path.abspath(filepath)), exist_ok=True)
            
            data = {
                'timestamp': time.time(),
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'ei_ratio': brain.getExcitationInhibitionRatio()
            }
            
            if include_spikes:
                # Note: This would require access to spike data which may not be directly available
                data['spike_data'] = f"Spike data for {brain.getTotalSpikeCount()} spikes"
            
            if include_sensory:
                # This would require tracking sensory input over time
                data['sensory_data'] = "Sensory data placeholder"
            
            if include_neurons:
                data['neuron_data'] = f"Neuron data for {brain.getTotalNeuronCount()} neurons"
            
            json_filepath = filepath if filepath.endswith('.json') else filepath + '.json'
            with open(json_filepath, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
        except Exception as e:
            raise NLMMemoryError(f"Failed to save simulation data: {str(e)}")
    
    @staticmethod
    def load_simulation_data(filepath: str) -> Dict[str, Any]:
        """Load simulation data from file.
        
        Args:
            filepath: Path to data file
            
        Returns:
            Dictionary containing loaded data
        """
        try:
            if filepath.endswith('.json'):
                with open(filepath, 'r') as f:
                    return json.load(f)
            else:
                # Try to load as numpy file or other formats
                raise ValueError(f"Unsupported file format: {filepath}")
        except Exception as e:
            raise NLMMemoryError(f"Failed to load simulation data: {str(e)}")
    
    @staticmethod
    def analyze_spike_pattern(spike_data: List[Dict], window_size: int = 100) -> Dict[str, Any]:
        """Analyze spike pattern statistics.
        
        Args:
            spike_data: List of spike events
            window_size: Analysis window size
            
        Returns:
            Dictionary containing spike pattern analysis
        """
        if not spike_data:
            return {'error': 'No spike data provided'}
        
        # Placeholder implementation
        return {
            'total_spikes': len(spike_data),
            'average_rate': len(spike_data) / window_size if window_size > 0 else 0,
            'pattern_type': 'unknown',
            'burst_events': 0
        }
class Visualization:
    """Visualization utilities for NLM simulations.
    
    This class provides tools for visualizing neural activity, brain states,
    and simulation results. It includes both simple text-based and
    matplotlib-based visualizations.
    """
    
    @staticmethod
    def print_brain_summary(brain: Brain):
        """Print a text-based summary of brain state.
        
        Args:
            brain: Brain instance to visualize
        """
        print("=" * 50)
        print("NLM Brain Summary")
        print("=" * 50)
        print(f"Total Neurons: {brain.getTotalNeuronCount()}")
        print(f"Total Synapses: {brain.getTotalSynapseCount()}")
        print(f"Active Neurons: {brain.getActiveNeuronCount()}")
        print(f"Firing Neurons: {brain.getFiringNeuronCount()}")
        print(f"Total Spikes: {brain.getTotalSpikeCount()}")
        print(f"Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"Developmental Stage: {brain.getDevelopmentalStage()}")
        print("=" * 50)
    
    @staticmethod
    def print_world_status(world: SimpleWorld):
        """Print a text-based summary of world state.
        
        Args:
            world: World instance to visualize
        """
        print("=" * 50)
        print("NLM World Status")
        print("=" * 50)
        print(f"World Size: {world.getWidth()} x {world.getHeight()}")
        print(f"Max Energy: {world.getMaxEnergy()}")
        print(f"Energy Decay Rate: {world.getEnergyDecayRate()}")
        print(f"Simulation Time: {world.getSimulationTime():.2f}")
        print(f"Random Seed: {world.getRandomSeed()}")
        print("=" * 50)
    
    @staticmethod
    def plot_neural_activity(brain: Brain, neurons: Optional[List[NeuronId]] = None,
                            time_window: int = 100):
        """Create a simple neural activity plot.
        
        Args:
            brain: Brain instance to visualize
            neurons: Optional list of specific neurons to plot
            time_window: Time window for activity
            
        Returns:
            matplotlib.figure.Figure or None if matplotlib not available
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # This is a placeholder for actual plotting implementation
            # In a real implementation, you would plot actual neural activity
            
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
            
            # Plot firing rate over time
            time_steps = range(time_window)
            firing_rates = np.random.rand(time_window) * brain.getAverageFiringRate()
            
            ax1.plot(time_steps, firing_rates)
            ax1.set_xlabel('Time Step')
            ax1.set_ylabel('Firing Rate (Hz)')
            ax1.set_title('Neural Firing Rate Over Time')
            ax1.grid(True, alpha=0.3)
            
            # Plot network statistics
            categories = ['Neurons', 'Synapses', 'Active', 'Firing']
            values = [
                brain.getTotalNeuronCount(),
                brain.getTotalSynapseCount(),
                brain.getActiveNeuronCount(),
                brain.getFiringNeuronCount()
            ]
            
            ax2.bar(categories, values, color=['skyblue', 'lightgreen', 'salmon', 'gold'])
            ax2.set_ylabel('Count')
            ax2.set_title('Current Network Statistics')
            ax2.grid(True, alpha=0.3, axis='y')
            
            plt.tight_layout()
            return fig
            
        except ImportError:
            print("Matplotlib not available. Install with: pip install matplotlib")
            return None
    
    @staticmethod
    def plot_activity_over_time(brain_history: List[Dict], metric: str = 'firing_rate'):
        """Plot neural activity metrics over time.
        
        Args:
            brain_history: List of brain state dictionaries over time
            metric: Metric to plot (e.g., 'firing_rate', 'spike_count', 'activity')
            
        Returns:
            matplotlib.figure.Figure or None if matplotlib not available
        """
        try:
            import matplotlib.pyplot as plt
            
            if not brain_history:
                print("No brain history data provided")
                return None
            
            time_steps = range(len(brain_history))
            
            # Extract metric values
            if metric == 'firing_rate':
                values = [state.get('average_firing_rate', 0) for state in brain_history]
                ylabel = 'Firing Rate (Hz)'
            elif metric == 'spike_count':
                values = [state.get('total_spikes', 0) for state in brain_history]
                ylabel = 'Total Spikes'
            elif metric == 'activity':
                # Calculate activity as a combination metric
                values = []
                for state in brain_history:
                    active_ratio = state.get('active_neurons', 0) / max(state.get('total_neurons', 1), 1)
                    firing_ratio = state.get('firing_neurons', 0) / max(state.get('active_neurons', 1), 1)
                    values.append(active_ratio * firing_ratio * 100)
                ylabel = 'Activity Index (0-100)'
            else:
                raise ValueError(f"Unknown metric: {metric}")
            
            fig, ax = plt.subplots(figsize=(10, 6))
            ax.plot(time_steps, values)
            ax.set_xlabel('Time Step')
            ax.set_ylabel(ylabel)
            ax.set_title(f'Neural Activity: {metric.replace("_", " ").title()}')
            ax.grid(True, alpha=0.3)
            
            plt.tight_layout()
            return fig
            
        except ImportError:
            print("Matplotlib not available. Install with: pip install matplotlib")
            return None
def create_default_config() -> Config:
    """Create a default NLM configuration.
    
    Returns:
        Config: Default configuration object
        
    Example:
        >>> import pynlm.utils as nlm
        >>> config = nlm.create_default_config()
        >>> config.set("learning_rate", 0.01)
        >>> config.set("num_neurons", 1000)
    """
    config = Config()
    # Set reasonable defaults
    config.set("learning_rate", 0.01)
    config.set("num_neurons", 100)
    config.set("population_size", [10, 20, 30])
    config.set("max_energy", 100.0)
    config.set("energy_decay_rate", 0.01)
    config.set("random_seed", 42)
    return config
def run_quick_test():
    """Run a quick test of NLM functionality.
    
    This function runs a minimal simulation to verify that the NLM
    bindings are working correctly.
    
    Returns:
        bool: True if test passed, False otherwise
        
    Example:
        >>> import pynlm.utils as nlm
        >>> if nlm.run_quick_test():
        ...     print("NLM bindings are working correctly!")
    """
    try:
        # Create basic components
        config = create_default_config()
        brain = Brain(config)
        world = SimpleWorld()
        
        # Initialize
        brain.initialize()
        world.reset()
        
        # Run a few steps
        for step in range(10):
            brain.step(step)
            world.update(0.1)
        
        # Check results
        total_neurons = brain.getTotalNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        
        print(f"Quick test passed!")
        print(f"Total neurons: {total_neurons}")
        print(f"Total spikes: {total_spikes}")
        
        return True
        
    except Exception as e:
        print(f"Quick test failed: {str(e)}")
        return False