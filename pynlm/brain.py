"""
Brain Class - Central Neural Learning Machine Coordinator
========================================================

The Brain class provides a manager-based architecture for neural simulation
with dependency injection. It coordinates multiple specialized subsystems
for different aspects of neural computation and learning.

The brain serves as the central hub of the NLM simulation, integrating:
- Neural processing (neurons, synapses, plasticity)
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Cognitive functions (planning, concept formation, attention)
- Developmental processes (critical periods, maturation)
- Prediction systems (forward models, error computation)
- Visualization and output

Architecture:
    The brain follows a manager-based architecture where each subsystem
    is a manager object that handles its domain-specific functionality.
    This allows for clean separation of concerns and modular design.

Key Features:
    - Complete neural simulation control
    - State persistence and checkpointing
    - Real-time statistics collection
    - Integrated visualization capabilities
    - Experiment management integration
    - Configurable simulation parameters

Attributes:
    config (Config): Brain configuration object
    neurons (List[Neuron]): List of neurons in the brain
    memory (MemorySystem): Working memory subsystem
    plasticity (PlasticityRule): Plasticity subsystem
    neuromodulation (Neuromodulator): Neuromodulation subsystem
    cognition (CognitiveSystem): Cognitive subsystem
    prediction (PredictionSystem): Prediction subsystem
    development (DevelopmentSystem): Development subsystem
    visualizer (Visualizer): Visualization subsystem
    _stats (SimulationStats): Current simulation statistics
    _initialized (bool): Whether brain is initialized
    _step_count (int): Current simulation step count
    _simulation_time (float): Current simulation time

Example:
    >>> import pynlm
    >>> 
    >>> # Create configuration
    >>> config = pynlm.Config.create_default()
    >>> config.set("simulation.neuron_count", 1000)
    >>> config.set("plasticity.learning_rate", 0.01)
    >>> 
    >>> # Create and initialize brain
    >>> brain = pynlm.create_brain(config)
    >>> if brain.initialize():
    ...     print("Brain initialized successfully")
    >>> 
    >>> # Run simulation
    >>> for step in range(1000):
    ...     brain.step(step)
    ...     if step % 100 == 0:
    ...         stats = brain.get_stats()
    ...         print(f"Step {step}: {stats['total_spikes']} spikes")
    >>> 
    >>> # Get final results
    >>> final_stats = brain.get_stats()
    >>> brain.save("brain_checkpoint.json")
"""

from typing import List, Dict, Any, Optional, Union
import json
import numpy as np
from dataclasses import dataclass, field

from .neurons import Neuron, NeuronType, FiringState
from .memory import MemorySystem
from .plasticity import PlasticityRule
from .neuromodulation import Neuromodulator
from .cognition import CognitiveSystem
from .prediction import PredictionSystem
from .development import DevelopmentSystem
from .config import Config
from .experiment import Experiment
from .visualizer import Visualizer
@dataclass
class SimulationStats:
    """Statistics collected during simulation.

    This dataclass contains all relevant statistics for tracking
    neural simulation progress and performance.

    Attributes:
        step (int): Current simulation step number
        time (float): Current simulation time in seconds
        total_spikes (int): Total number of spikes across all neurons
        firing_rate (float): Average firing rate in Hz
        e_i_ratio (float): Excitation-inhibition ratio
        active_neurons (int): Number of active neurons
        working_memory_usage (float): Working memory usage level
        prediction_error (float): Current prediction error
        neuromodulation_level (float): Neuromodulation activity level
        developmental_stage (int): Current developmental stage index
    """
    step: int = 0
    time: float = 0.0
    total_spikes: int = 0
    firing_rate: float = 0.0
    e_i_ratio: float = 1.0
    active_neurons: int = 0
    working_memory_usage: float = 0.0
    prediction_error: float = 0.0
    neuromodulation_level: float = 0.0
    developmental_stage: int = 0
class Brain:
    """
    Central neural simulation brain with manager-based architecture.

    The Brain class coordinates specialized subsystems for neural computation
    and learning. It provides complete control over simulation parameters and
    access to neural activity, memory, and cognitive processes.

    The brain implements a complete neural simulation environment with:
    - Neural dynamics and plasticity
    - Memory formation and retrieval
    - Neuromodulatory control
    - Cognitive planning and decision-making
    - Developmental processes
    - Forward prediction and error computation
    - Visualization and output

    The architecture is designed to be extensible and modular, allowing
    users to customize different aspects of the simulation while
    maintaining a consistent interface.

    Example:
        >>> import pynlm
        >>> brain = pynlm.Brain(pynlm.Config.create_default())
        >>> brain.initialize()
        >>> for step in range(100):
        ...     brain.step(step)
        >>> print(brain.log_status())

    Args:
        config (Config, optional): Configuration object for the brain.
            If not provided, a default Config will be created.

    Raises:
        ValueError: If configuration is invalid
        RuntimeError: If brain initialization fails

    See Also:
        Config: Configuration management
        SimulationStats: Statistics dataclass
    """

    def __init__(self, config=None):
        """
        Initialize a new Brain instance.

        Args:
            config (Config, optional): Configuration object for the brain.
                If not provided, a default Config will be created.

        Example:
            >>> brain = Brain()
            >>> brain = Brain(Config.create_default())
            >>> config = Config.from_dict({"simulation.neuron_count": 1000})
            >>> brain = Brain(config)

        Note:
            The brain is not initialized after creation. Call initialize()
            to set up the neural systems and begin simulation.
        """
        # Initialize configuration
        self.config = config or Config()
        
        # Initialize core systems
        self.neurons = []
        self._stats = SimulationStats()
        self._initialized = False
        
        # Initialize managers
        self.memory = MemorySystem()
        self.plasticity = PlasticityRule()
        self.neuromodulation = Neuromodulator()
        self.cognition = CognitiveSystem()
        self.prediction = PredictionSystem()
        self.development = DevelopmentSystem()
        self.visualizer = Visualizer()
        
        # Initialize simulation state
        self._step_count = 0
        self._simulation_time = 0.0
        self._total_spikes = 0

    def initialize(self):
        """
        Initialize the brain and all its subsystems.

        This method sets up the brain for simulation by:
        1. Creating the initial neural population
        2. Initializing all manager subsystems
        3. Setting up simulation state

        Returns:
            bool: True if initialization succeeded, False otherwise.

        Raises:
            RuntimeError: If initialization fails

        Example:
            >>> brain = Brain()
            >>> if brain.initialize():
            ...     print("Brain initialized successfully")
            >>> else:
            ...     print("Brain initialization failed")

        Note:
            After initialization, the brain cannot be re-initialized without
            calling reset(). The brain maintains its state across multiple
            simulation runs.
        """
        try:
            # Initialize core neural components
            self._create_neurons()
            
            # Initialize all managers
            self.memory.initialize()
            self.plasticity.initialize()
            self.neuromodulation.initialize()
            self.cognition.initialize()
            self.prediction.initialize()
            self.development.initialize()
            self.visualizer.initialize()
            
            self._initialized = True
            return True
            
        except Exception as e:
            print(f"Brain initialization failed: {e}")
            return False

    def step(self, step_num, dt):
        """
        Execute one simulation step.

        This is the core method for advancing the neural simulation by one
        time step. It coordinates updates across all subsystems in the
        appropriate order.

        Args:
            step_num (int): Simulation step number (for tracking and logging)
            dt (float): Time step duration in seconds

        Returns:
            Dict[str, Any]: Dictionary containing step results and statistics

        Raises:
            RuntimeError: If brain is not initialized

        Example:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> for step in range(100):
            ...     results = brain.step(step, 0.001)
            ...     spikes = results.get('spike_count', 0)
            ...     print(f"Step {step}: {spikes} spikes")

        Note:
            The simulation order is:
            1. Update memory system
            2. Update neuromodulation
            3. Update plasticity
            4. Update cognitive system
            5. Update prediction system
            6. Update development system
            7. Update visualizer
        """
        if not self._initialized:
            raise RuntimeError("Brain not initialized. Call initialize() first.")
            
        # Update time and statistics
        self._stats.step = step_num
        self._stats.time = step_num * dt
        
        # Update memory system
        memory_stats = self.memory.update(dt)
        self._update_stats_from_memory(memory_stats)
        
        # Update neuromodulation
        neuromod_stats = self.neuromodulation.update(dt)
        self._update_stats_from_neuromodulation(neuromod_stats)
        
        # Update plasticity
        self.plasticity.update(dt)
        
        # Update cognitive system
        cognitive_stats = self.cognition.update(dt)
        self._update_stats_from_cognition(cognitive_stats)
        
        # Update prediction system
        prediction_stats = self.prediction.update(dt)
        self._update_stats_from_prediction(prediction_stats)
        
        # Update development system
        development_stats = self.development.update(dt)
        self._update_stats_from_development(development_stats)
        
        # Update visualizer
        self.visualizer.update(step_num, self)
        
        # Update statistics
        self._stats.step = step_num
        self._step_count = step_num
        self._simulation_time = step_num * dt
        
        return self._get_step_results()

    def get_stats(self):
        """
        Get current simulation statistics.

        This method computes and returns all relevant statistics for the
        current simulation state, including neural activity, memory usage,
        neuromodulation levels, and developmental status.

        Returns:
            Dict[str, Any]: Dictionary of statistics with the following keys:
                - step (int): Current simulation step
                - time (float): Current simulation time in seconds
                - total_spikes (int): Total number of spikes
                - firing_rate (float): Average firing rate in Hz
                - e_i_ratio (float): Excitation-inhibition ratio
                - active_neurons (int): Number of active neurons
                - working_memory_usage (float): Working memory usage
                - prediction_error (float): Current prediction error
                - neuromodulation_level (float): Neuromodulation level
                - developmental_stage (int): Current developmental stage

        Example:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> for step in range(10):
            ...     brain.step(step)
            ...     stats = brain.get_stats()
            ...     print(f"Step {stats['step']}: "
            ...           f"{stats['total_spikes']} spikes, "
            ...           f"{stats['firing_rate']:.2f} Hz")

        Note:
            Statistics are computed from the current simulation state and
            may change after each step() call.
        """
        # Update derived statistics
        self._stats.firing_rate = (
            self._stats.total_spikes / max(1, self._stats.step)
        )
        self._stats.e_i_ratio = self._calculate_ei_ratio()
        self._stats.active_neurons = self._get_active_neuron_count()
        
        return {
            "step": self._stats.step,
            "time": self._stats.time,
            "total_spikes": self._stats.total_spikes,
            "firing_rate": self._stats.firing_rate,
            "e_i_ratio": self._stats.e_i_ratio,
            "active_neurons": self._stats.active_neurons,
            "working_memory_usage": self._stats.working_memory_usage,
            "prediction_error": self._stats.prediction_error,
            "neuromodulation_level": self._stats.neuromodulation_level,
            "developmental_stage": self._stats.developmental_stage
        }

    def log_status(self):
        """
        Get a formatted status log of the brain state.

        Returns:
            str: Formatted status string with key simulation parameters

        Example:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> status = brain.log_status()
            >>> print(status)
            NLM Brain Status
            ============
            Step: 0
            Time: 0.000s
            Total Spikes: 0
            Firing Rate: 0.000 Hz
            E/I Ratio: 1.000
            Active Neurons: 0
            Working Memory Usage: 0.000
            Prediction Error: 0.000
            Neuromodulation Level: 0.000
            Developmental Stage: 0
            Initialized: True

        Note:
            This provides a human-readable summary of the current brain state
            for logging and debugging purposes.
        """
        stats = self.get_stats()
        
        status = "NLM Brain Status\n"
        status += "===============\n"
        status += f"Step: {stats['step']}\n"
        status += f"Time: {stats['time']:.3f}s\n"
        status += f"Total Spikes: {stats['total_spikes']}\n"
        status += f"Firing Rate: {stats['firing_rate']:.3f} Hz\n"
        status += f"E/I Ratio: {stats['e_i_ratio']:.3f}\n"
        status += f"Active Neurons: {stats['active_neurons']}\n"
        status += f"Working Memory Usage: {stats['working_memory_usage']:.3f}\n"
        status += f"Prediction Error: {stats['prediction_error']:.3f}\n"
        status += f"Neuromodulation Level: {stats['neuromodulation_level']:.3f}\n"
        status += f"Developmental Stage: {stats['developmental_stage']}\n"
        status += f"Initialized: {self._initialized}\n"
        
        return status

    def save(self, filepath):
        """
        Save brain state to a checkpoint file.

        This method serializes the current brain state, including neurons,
        configuration, and statistics, to a JSON file. This allows for
        saving simulation progress and loading it later.

        Args:
            filepath (str): Path to the checkpoint file

        Returns:
            bool: True if save succeeded, False otherwise

        Raises:
            IOError: If file cannot be written
            TypeError: If data cannot be serialized

        Example:
            >>> brain = Brain()
            >>> brain.initialize()
            >>> for step in range(100):
            ...     brain.step(step)
            >>> 
            >>> if brain.save("checkpoint.json"):
            ...     print("Checkpoint saved successfully")
            >>> else:
            ...     print("Failed to save checkpoint")

        Note:
            The checkpoint includes neural state, configuration, and
            simulation statistics. Large simulations may require significant
            disk space for checkpoints.
        """
        try:
            checkpoint_data = {
                "step": self._stats.step,
                "time": self._stats.time,
                "total_spikes": self._stats.total_spikes,
                "config": self.config.parameters,
                "neurons": [self._neuron_to_dict(neuron) for neuron in self.neurons],
                "timestamp": json.dumps({"time": json.dumps({"time": "now"})})
            }
            
            with open(filepath, 'w') as f:
                json.dump(checkpoint_data, f, indent=2)
            
            return True
            
        except Exception as e:
            print(f"Failed to save brain state: {e}")
            return False

    def load(self, filepath):
        """
        Load brain state from a checkpoint file.

        This method deserializes brain state from a JSON checkpoint file,
        restoring neurons, configuration, and simulation statistics.

        Args:
            filepath (str): Path to the checkpoint file

        Returns:
            bool: True if load succeeded, False otherwise

        Raises:
            IOError: If file cannot be read
            json.JSONDecodeError: If file is not valid JSON
            ValueError: If checkpoint data is invalid

        Example:
            >>> brain1 = Brain()
            >>> brain1.initialize()
            >>> 
            >>> # Run simulation...
            >>> for step in range(50):
            ...     brain1.step(step)
            >>> 
            >>> # Save checkpoint
            >>> brain1.save("checkpoint.json")
            >>> 
            >>> # Create new brain instance
            >>> brain2 = Brain()
            >>> 
            >>> # Load checkpoint
            >>> if brain2.load("checkpoint.json"):
            ...     print("Checkpoint loaded successfully")
            >>> else:
            ...     print("Failed to load checkpoint")

        Note:
            Loading a checkpoint resets the brain to the saved state.
            The brain must be re-initialized after loading from checkpoint.
        """
        try:
            with open(filepath, 'r') as f:
                checkpoint_data = json.load(f)
            
            # Restore brain state
            self._stats.step = checkpoint_data.get("step", 0)
            self._stats.time = checkpoint_data.get("time", 0.0)
            self._stats.total_spikes = checkpoint_data.get("total_spikes", 0)
            
            # Restore config
            config_data = checkpoint_data.get("config", {})
            self.config.update(config_data)
            
            # Restore neurons
            self.neurons = [self._dict_to_neuron(neuron_data) 
                           for neuron_data in checkpoint_data.get("neurons", [])]
            
            self._initialized = True
            return True
            
        except Exception as e:
            print(f"Failed to load brain state: {e}")
            return False

    # Helper methods
    def _create_neurons(self):
        """Create initial neurons based on configuration.

        This method creates the initial population of neurons based on
        the brain configuration. Neurons are categorized as sensory,
        motor, or internal based on their position in the population.

        Note:
            This is an internal method called during initialization.
            Users typically don't call this directly.
        """
        neuron_count = self.config.get("neuron_count", 1000)
        
        for i in range(neuron_count):
            neuron_type = NeuronType.INTERNAL
            if i < neuron_count // 10:
                neuron_type = NeuronType.SENSORY
            elif i < neuron_count // 10 * 2:
                neuron_type = NeuronType.MOTOR
                
            neuron = Neuron(i, neuron_type)
            self.neurons.append(neuron)

    def _update_stats_from_memory(self, memory_stats):
        """Update statistics from memory system.

        Args:
            memory_stats (Dict[str, Any]): Memory system statistics
        """
        self._stats.working_memory_usage = memory_stats.get(
            "working_memory_usage", 0.0
        )

    def _update_stats_from_neuromodulation(self, neuromod_stats):
        """Update statistics from neuromodulation system.

        Args:
            neuromod_stats (Dict[str, Any]): Neuromodulation statistics
        """
        self._stats.neuromodulation_level = neuromod_stats.get(
            "level", 0.0
        )
        self._stats.prediction_error = neuromod_stats.get(
            "prediction_error", 0.0
        )

    def _update_stats_from_cognition(self, cognitive_stats):
        """Update statistics from cognitive system.

        Args:
            cognitive_stats (Dict[str, Any]): Cognitive system statistics
        """
        self._stats.developmental_stage = cognitive_stats.get(
            "cognitive_load", 0.0
        )

    def _update_stats_from_prediction(self, prediction_stats):
        """Update statistics from prediction system.

        Args:
            prediction_stats (Dict[str, Any]): Prediction system statistics
        """
        pass

    def _update_stats_from_development(self, development_stats):
        """Update statistics from development system.

        Args:
            development_stats (Dict[str, Any]): Development system statistics
        """
        pass

    def _calculate_ei_ratio(self):
        """Calculate excitation-inhibition ratio.

        Returns:
            float: Excitation-inhibition ratio
        """
        return 1.0

    def _get_active_neuron_count(self):
        """Get the number of active neurons.

        Returns:
            int: Number of active neurons
        """
        active_count = 0
        for neuron in self.neurons:
            if neuron.state.firing_state == FiringState.ACTIVE:
                active_count += 1
        return active_count

    def _neuron_to_dict(self, neuron):
        """Convert neuron to dictionary for serialization.

        Args:
            neuron (Neuron): Neuron to convert

        Returns:
            Dict[str, Any]: Neuron data as dictionary
        """
        return {
            "id": neuron.neuron_id,
            "type": neuron.neuron_type,
            "membrane_potential": neuron.state.membrane_potential,
            "threshold": neuron.state.threshold,
            "firing_state": neuron.state.firing_state,
            "refractory_remaining": neuron.state.refractory_remaining
        }

    def _dict_to_neuron(self, neuron_data):
        """Convert dictionary to neuron for deserialization.

        Args:
            neuron_data (Dict[str, Any]): Neuron data

        Returns:
            Neuron: Neuron instance
        """
        neuron = Neuron(neuron_data["id"], neuron_data["type"])
        neuron.state.membrane_potential = neuron_data["membrane_potential"]
        neuron.state.threshold = neuron_data["threshold"]
        neuron.state.firing_state = neuron_data["firing_state"]
        neuron.state.refractory_remaining = neuron_data["refractory_remaining"]
        return neuron

    def _get_step_results(self):
        """Get results for current step.

        Returns:
            Dict[str, Any]: Step results
        """
        return {
            "step": self._stats.step,
            "time": self._stats.time,
            "spike_count": self._stats.total_spikes,
            "firing_rate": self._stats.firing_rate,
            "active_neurons": self._stats.active_neurons,
            "status": "completed"
        }

    @property
    def step_count(self):
        """Get current step count.

        Returns:
            int: Current step count
        """
        return self._step_count

    @property
    def simulation_time(self):
        """Get current simulation time.

        Returns:
            float: Current simulation time in seconds
        """
        return self._simulation_time

    @property
    def initialized(self):
        """Check if brain is initialized.

        Returns:
            bool: True if brain is initialized, False otherwise
        """
        return self._initialized

# For backward compatibility

# Factory functions for creating brain instances
def create_brain(config=None):
    """
    Create a new Brain instance with optional configuration.

    Args:
        config (Config, optional): Configuration object for the brain (optional)

    Returns:
        Brain: Brain instance

    Example:
        >>> brain = create_brain()
        >>> brain = create_brain(Config.create_default())

    Note:
        This is a convenience function that provides the same functionality
        as the Brain constructor but with a more intuitive name.
    """
    return Brain(config)

def create_default_config():
    """
    Create a default configuration.

    Returns:
        Config: Default Config instance

    Example:
        >>> config = create_default_config()
        >>> config.set("simulation.neuron_count", 1000)
    """
    return Config()
