"""
High-level processors for NLM Python API
========================================

This module contains high-level Python classes that simplify common NLM operations.
"""

import numpy as np
from typing import Any, Dict, List, Optional, Union, Callable
from dataclasses import dataclass, field
import json
import warnings

from . import NLMError, ValidationError, ConfigurationError


try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False


@dataclass
class ExperimentResults:
    """
    Container for experiment results.
    
    Attributes:
        metrics: Dictionary of performance metrics
        timestamps: List of timestamps for each step
        data_points: List of data points recorded during experiment
    """
    metrics: Dict[str, float] = field(default_factory=dict)
    timestamps: List[float] = field(default_factory=list)
    data_points: List[Dict[str, Any]] = field(default_factory=list)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert results to dictionary."""
        return {
            'metrics': self.metrics,
            'timestamps': self.timestamps,
            'data_points': self.data_points,
        }
    
    def __str__(self) -> str:
        return f"ExperimentResults(metrics={self.metrics}, steps={len(self.timestamps)})"


class NLMBrain:
    """
    Simplified brain interface for common neural simulation operations.
    
    This class provides a high-level interface to NLM brains with convenience
    methods for initialization, simulation, and monitoring.
    
    Example:
        ```python
        brain = NLMBrain(neuron_count=1000, complexity=0.7)
        brain.initialize()
        
        for i in range(100):
            brain.step(i)
            print(f"Firing rate: {brain.get_average_firing_rate()}")
        ```
    
    Args:
        neuron_count: Number of neurons in the brain
        complexity: Brain complexity (0.0-1.0, where 1.0 is most complex)
        config: Optional configuration dict (overrides defaults)
    """
    
    def __init__(self, neuron_count: int = 1000, complexity: float = 0.5, 
                 config: Optional[Dict[str, Any]] = None):
        if not HAS_PYNLM:
            raise ImportError("NLM C++ bindings not found. Please build pynlm.")
        
        if neuron_count < 1:
            raise ValidationError(f"Neuron count must be positive, got {neuron_count}")
        if not 0.0 <= complexity <= 1.0:
            raise ValidationError(f"Complexity must be in [0.0, 1.0], got {complexity}")
        
        self._neuron_count = neuron_count
        self._complexity = complexity
        self._config = config or {}
        self._brain = None
        self._is_initialized = False
        
        # Initialize the underlying C++ brain
        self._create_brain()
    
    def _create_brain(self):
        """Create the underlying C++ brain."""
        # Create configuration
        cpp_config = pynlm.createDefaultConfig()
        
        # Set brain parameters based on complexity
        if self._complexity > 0.7:
            cpp_config.set("brain.excitatory_neurons", int(self._neuron_count * 0.8))
            cpp_config.set("brain.inhibitory_neurons", int(self._neuron_count * 0.2))
        elif self._complexity > 0.3:
            cpp_config.set("brain.excitatory_neurons", int(self._neuron_count * 0.6))
            cpp_config.set("brain.inhibitory_neurons", int(self._neuron_count * 0.4))
        else:
            cpp_config.set("brain.excitatory_neurons", int(self._neuron_count * 0.5))
            cpp_config.set("brain.inhibitory_neurons", int(self._neuron_count * 0.5))
        
        # Apply user config if provided
        for key, value in self._config.items():
            if isinstance(value, dict):
                # Handle nested config
                for subkey, subvalue in value.items():
                    cpp_config.set(f"{key}.{subkey}", subvalue)
            else:
                cpp_config.set(key, value)
        
        # Create brain
        self._brain = pynlm.createBrain(cpp_config)
    
    def initialize(self):
        """
        Initialize the brain for simulation.
        
        This method should be called before any simulation steps.
        """
        if self._brain is None:
            raise RuntimeError("Brain not properly initialized")
        
        self._brain.initialize()
        self._is_initialized = True
    
    def step(self, step_number: int, current_time: Optional[float] = None):
        """
        Perform one simulation step.
        
        Args:
            step_number: The step number (used for learning rules)
            current_time: Optional current time in simulation
        """
        if not self._is_initialized:
            self.initialize()
        
        if current_time is not None:
            self._brain.step(step_number, current_time)
        else:
            self._brain.step(step_number)
    
    def reset(self):
        """Reset the brain to its initial state."""
        if self._brain:
            self._brain.reset()
        self._is_initialized = False
    
    # Convenience methods
    
    def get_neuron_count(self) -> int:
        """Get the number of neurons in the brain."""
        return self._neuron_count
    
    def get_complexity(self) -> float:
        """Get the brain complexity (0.0-1.0)."""
        return self._complexity
    
    def get_total_neurons(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount() if self._brain else 0
    
    def get_total_synapses(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount() if self._brain else 0
    
    def get_active_neurons(self) -> int:
        """Get count of currently active neurons."""
        return self._brain.getActiveNeuronCount() if self._brain else 0
    
    def get_firing_neurons(self) -> int:
        """Get count of currently firing neurons."""
        return self._brain.getFiringNeuronCount() if self._brain else 0
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self._brain.getAverageFiringRate() if self._brain else 0.0
    
    def get_total_spikes(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount() if self._brain else 0
    
    def get_developmental_stage(self):
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage() if self._brain else None
    
    def set_developmental_stage(self, stage):
        """Set developmental stage."""
        if self._brain:
            self._brain.setDevelopmentalStage(stage)
    
    def inject_current(self, neuron_id, current: float):
        """
        Inject current into a specific neuron.
        
        Args:
            neuron_id: Neuron ID to inject current into
            current: Current value to inject
        """
        if self._brain:
            self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_to_neurons(self, neuron_type: str, current: float):
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons (e.g., 'Excitatory', 'Inhibitory')
            current: Current value to inject
        """
        if self._brain:
            self._brain.injectCurrentToNeurons(neuron_type, current)
    
    def produce_action(self):
        """Produce motor action based on neural activity."""
        if self._brain:
            return self._brain.produceAction()
        return None
    
    def receive_sensory_input(self, sensory_input):
        """
        Inject sensory input into the brain.
        
        Args:
            sensory_input: Sensory input object
        """
        if self._brain:
            self._brain.receiveSensoryInput(sensory_input)
    
    def log_status(self):
        """Log brain status information."""
        if self._brain:
            self._brain.logStatus()
    
    def save(self, filepath: str):
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save file
        """
        if self._brain:
            self._brain.save(filepath)
    
    def load(self, filepath: str):
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load file
        """
        if self._brain:
            self._brain.load(filepath)
        self._is_initialized = False
    
    def get_regions(self) -> List:
        """Get all brain regions."""
        return self._brain.getRegions() if self._brain else []
    
    def get_region_count(self) -> int:
        """Get number of brain regions."""
        return self._brain.getRegionCount() if self._brain else 0
    
    def get_config(self):
        """Get brain configuration."""
        return self._brain.getConfig() if self._brain else None
    
    def __str__(self) -> str:
        return (f"NLMBrain(neuron_count={self._neuron_count}, "
                f"complexity={self._complexity}, "
                f"initialized={self._is_initialized})")
    
    def __repr__(self) -> str:
        return self.__str__()


class NLMVisualizer:
    """
    Simple visualization interface for NLM simulations.
    
    This class provides basic visualization capabilities for brain states,
    simulation progress, and data visualization.
    
    Example:
        ```python
        visualizer = NLMVisualizer()
        brain = NLMBrain(1000, 0.7)
        brain.initialize()
        
        visualizer.show_firing_rate(brain.get_average_firing_rate())
        visualizer.update_brain_state(brain.get_developmental_stage())
        ```
    """
    
    def __init__(self, backend: str = "console"):
        """
        Initialize visualizer.
        
        Args:
            backend: Visualization backend ('console', 'plotly', 'matplotlib')
        """
        self._backend = backend.lower()
        self._history = []
        self._is_enabled = True
        
        if self._backend not in ['console', 'plotly', 'matplotlib']:
            warnings.warn(f"Backend '{self._backend}' not fully supported, using console mode")
            self._backend = 'console'
    
    def enable(self):
        """Enable visualization output."""
        self._is_enabled = True
    
    def disable(self):
        """Disable visualization output."""
        self._is_enabled = False
    
    def show_firing_rate(self, firing_rate: float, step: int = 0):
        """
        Display firing rate information.
        
        Args:
            firing_rate: Firing rate to display
            step: Current step number
        """
        if not self._is_enabled:
            return
        
        if self._backend == 'console':
            print(f"Step {step}: Firing rate = {firing_rate:.4f}")
            self._history.append({
                'type': 'firing_rate',
                'value': firing_rate,
                'step': step,
                'timestamp': None  # Would use time.time() in real implementation
            })
    
    def show_neuron_stats(self, brain):
        """
        Display brain statistics.
        
        Args:
            brain: NLMBrain or Brain object
        """
        if not self._is_enabled:
            return
        
        if self._backend == 'console':
            stats = {
                'total_neurons': brain.get_total_neurons(),
                'active_neurons': brain.get_active_neurons(),
                'firing_neurons': brain.get_firing_neurons(),
                'average_firing_rate': brain.get_average_firing_rate(),
                'total_spikes': brain.get_total_spikes(),
            }
            
            print("Brain Statistics:")
            for key, value in stats.items():
                print(f"  {key}: {value}")
            
            self._history.append({
                'type': 'neuron_stats',
                'value': stats,
                'step': None,  # Would track step number
                'timestamp': None
            })
    
    def update_brain_state(self, developmental_stage):
        """
        Display brain developmental state.
        
        Args:
            developmental_stage: Current developmental stage
        """
        if not self._is_enabled:
            return
        
        if self._backend == 'console':
            print(f"Developmental Stage: {developmental_stage}")
    
    def show_simulation_progress(self, current_step: int, total_steps: int,
                                 metrics: Optional[Dict] = None):
        """
        Display simulation progress.
        
        Args:
            current_step: Current simulation step
            total_steps: Total simulation steps
            metrics: Optional additional metrics to display
        """
        if not self._is_enabled:
            return
        
        if self._backend == 'console':
            progress = current_step / total_steps * 100 if total_steps > 0 else 0
            print(f"\nProgress: {progress:.1f}% ({current_step}/{total_steps})")
            
            if metrics:
                print("  Metrics:", metrics)
            
            self._history.append({
                'type': 'progress',
                'value': {
                    'current_step': current_step,
                    'total_steps': total_steps,
                    'progress': progress,
                    'metrics': metrics,
                },
                'step': current_step,
                'timestamp': None
            })
    
    def get_history(self) -> List[Dict[str, Any]]:
        """Get visualization history."""
        return self._history.copy()
    
    def clear_history(self):
        """Clear visualization history."""
        self._history.clear()
    
    def export_plots(self, filepath: str):
        """
        Export plots to file (when supported by backend).
        
        Args:
            filepath: Path to save plots
        """
        if self._backend not in ['plotly', 'matplotlib']:
            warnings.warn("Plot export requires plotly or matplotlib backend")
            return
        
        # Implementation would depend on backend
        warnings.warn(f"Plot export to {filepath} not implemented for {self._backend} backend")
    
    def show(self):
        """Display accumulated visualizations."""
        if self._backend == 'console' and self._is_enabled:
            print(f"\n=== Visualization Summary ===")
            print(f"Total events recorded: {len(self._history)}")
            
            event_types = {}
            for event in self._history:
                event_type = event['type']
                event_types[event_type] = event_types.get(event_type, 0) + 1
            
            print("Event types:")
            for event_type, count in event_types.items():
                print(f"  {event_type}: {count}")


class NLMExperiment:
    """
    High-level experiment runner for NLM simulations.
    
    This class provides a high-level interface for setting up and running
    complex experiments with brains, worlds, and agents.
    
    Example:
        ```python
        experiment = NLMExperiment.create_standard(
            world_size=(20, 20),
            num_steps=1000
        )
        
        results = experiment.run()
        ```
    """
    
    @staticmethod
    def create_simple(neuron_count: int = 1000, world_size: tuple = (10, 10),
                     config: Optional[Dict[str, Any]] = None) -> 'NLMExperiment':
        """
        Create a simple experiment with basic brain and world setup.
        
        Args:
            neuron_count: Number of neurons in the brain
            world_size: Tuple of (width, height) for the world
            config: Optional configuration dict
            
        Returns:
            NLMExperiment instance
        """
        return NLMExperiment(neuron_count=neuron_count, world_size=world_size, 
                           config=config)
    
    @staticmethod
    def create_standard(world_size: tuple = (10, 10), 
                       config: Optional[Dict[str, Any]] = None) -> 'NLMExperiment':
        """
        Create a standard experiment with default settings.
        
        Args:
            world_size: Tuple of (width, height) for the world
            config: Optional configuration dict
            
        Returns:
            NLMExperiment instance
        """
        return NLMExperiment(world_size=world_size, config=config)
    
    def __init__(self, neuron_count: int = 1000, world_size: tuple = (10, 10),
                 config: Optional[Dict[str, Any]] = None):
        """
        Initialize experiment.
        
        Args:
            neuron_count: Number of neurons in the brain
            world_size: Tuple of (width, height) for the world
            config: Optional configuration dict
        """
        if not HAS_PYNLM:
            raise ImportError("NLM C++ bindings not found. Please build pynlm.")
        
        self._neuron_count = neuron_count
        self._world_size = world_size
        self._config = config or {}
        self._brain = None
        self._world = None
        self._agent = None
        self._visualizer = NLMVisualizer()
        self._results = ExperimentResults()
        
        # Initialize components
        self._setup_components()
    
    def _setup_components(self):
        """Set up brain, world, and agent components."""
        # Create brain
        cpp_config = pynlm.createDefaultConfig()
        cpp_config.set("brain.excitatory_neurons", int(self._neuron_count * 0.7))
        cpp_config.set("brain.inhibitory_neurons", int(self._neuron_count * 0.3))
        
        for key, value in self._config.items():
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    cpp_config.set(f"{key}.{subkey}", subvalue)
            else:
                cpp_config.set(key, value)
        
        self._brain = pynlm.createBrain(cpp_config)
        
        # Create world
        self._world = pynlm.createSimpleWorld()
        self._world.configure(
            width=self._world_size[0],
            height=self._world_size[1],
            visionWidth=8,
            visionHeight=8
        )
        
        # Create agent
        self._agent = pynlm.createAgentBrain(self._brain)
        
        # Initialize components
        self._brain.initialize()
        self._agent.initialize(self._world)
    
    def configure(self, **kwargs):
        """
        Configure experiment parameters.
        
        Args:
            **kwargs: Configuration options
        """
        self._config.update(kwargs)
    
    def set_visualizer(self, visualizer: NLMVisualizer):
        """Set visualizer for the experiment."""
        self._visualizer = visualizer
    
    def run(self, num_steps: int) -> ExperimentResults:
        """
        Run the experiment for a given number of steps.
        
        Args:
            num_steps: Number of simulation steps to run
            
        Returns:
            ExperimentResults object containing the results
        """
        # Reset results
        self._results = ExperimentResults()
        
        # Reset components
        self._brain.reset()
        self._world.reset()
        self._agent.reset()
        
        # Clear history
        self._visualizer.clear_history()
        
        # Run simulation
        self._world.reset()
        
        for step in range(num_steps):
            # Update world
            self._world.update(0.1)
            
            # Get sensory input
            percept = self._world.getSensoryPercept()
            
            # Process sensory input
            self._agent.processSensoryInput(percept)
            
            # Brain processes input
            self._brain.step(step)
            
            # Get action
            action = self._agent.decodeMotorCommand()
            
            # Apply action
            self._world.applyMotorCommand(action, self._world.getSimulationTime())
            
            # Record results
            self._results.timestamps.append(step)
            self._results.data_points.append({
                'step': step,
                'firing_rate': self._brain.getAverageFiringRate(),
                'active_neurons': self._brain.getActiveNeuronCount(),
                'firing_neurons': self._brain.getFiringNeuronCount(),
                'total_spikes': self._brain.getTotalSpikeCount(),
                'reward': 0.0,  # Would track actual reward in real implementation
            })
            
            # Update metrics
            self._results.metrics = {
                'final_firing_rate': self._brain.getAverageFiringRate(),
                'final_active_neurons': self._brain.getActiveNeuronCount(),
                'final_firing_neurons': self._brain.getFiringNeuronCount(),
                'final_spikes': self._brain.getTotalSpikeCount(),
            }
            
            # Visualize progress
            self._visualizer.show_firing_rate(self._brain.getAverageFiringRate(), step)
            self._visualizer.show_neuron_stats(self._brain)
            self._visualizer.show_simulation_progress(step, num_steps, 
                                                     {'reward': 0.0})  # Would track actual reward
        
        # Show final visualizer output
        self._visualizer.show()
        
        return self._results
    
    def enable_features(self, reward_modulation: bool = False,
                       structural_plasticity: bool = False,
                       development: bool = False,
                       curiosity: bool = False):
        """
        Enable optional brain features.
        
        Args:
            reward_modulation: Enable reward-based learning
            structural_plasticity: Enable connection growth
            development: Enable developmental processes
            curiosity: Enable curiosity-driven exploration
        """
        if self._agent:
            if reward_modulation:
                self._agent.enableRewardModulation(True)
            if structural_plasticity:
                self._agent.enableStructuralPlasticity(True)
            if development:
                self._agent.enableDevelopment(True)
            if curiosity:
                self._agent.enableCuriosity(True)
    
    def get_brain(self) -> Any:
        """Get the underlying brain object."""
        return self._brain
    
    def get_world(self) -> Any:
        """Get the underlying world object."""
        return self._world
    
    def get_agent(self) -> Any:
        """Get the underlying agent object."""
        return self._agent
    
    def get_results(self) -> ExperimentResults:
        """Get experiment results."""
        return self._results
    
    def __str__(self) -> str:
        return (f"NLMExperiment(neuron_count={self._neuron_count}, "
                f"world_size={self._world_size}, "
                f"steps={len(self._results.timestamps)})")
    
    def __repr__(self) -> str:
        return self.__str__()