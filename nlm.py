#!/usr/bin/env python3
"""
NLM Python Bindings - Neural Learning Machine for Python
=========================================================

This module provides Python bindings for the NLM (Neural Learning Machine) C++ library,
enabling Python developers to create and simulate artificial neural systems with
evolving brains that learn from experience.

Key Features:
- High-level Python API for easy neural system creation and simulation
- Event-driven neural computation with LIF neurons and plasticity
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental plasticity
- Full control over neural parameters and simulation
- Serialization/persistance (checkpointing)
- Comprehensive testing and verification tools

Example Usage:
    >>> import nlm
    >>> 
    >>> # Create a neural system
    >>> brain = nlm.NeuralSystem()
    >>> brain.initialize()
    >>> 
    >>> # Set up sensory input
    >>> brain.set_sensory_input([0.1, 0.5, 0.9, ...])
    >>> 
    >>> # Run simulation
    >>> brain.step()
    >>> 
    >>> # Get motor output
    >>> action = brain.get_motor_output()
    >>> 
    >>> # Check for learning
    >>> if brain.has_learning_occurred():
    >>>     print("Brain learned something!")
"""

import numpy as np
import time
from typing import Optional, List, Dict, Tuple, Callable
from enum import Enum
import json
import pickle
from dataclasses import dataclass, field

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Team"
__license__ = "MIT"

# Type aliases for better Python API
float_array = List[float]
int_array = List[int]
float_matrix = List[List[float]]


class MotorCommand(Enum):
    """Motor commands that the brain can produce"""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    LOOK_LEFT = 4
    LOOK_RIGHT = 5
    INTERACT = 6
    WAIT = 7

    def to_string(self) -> str:
        return self.name.replace('_', ' ').title()

    @classmethod
    def from_index(cls, index: int) -> 'MotorCommand':
        """Get MotorCommand from integer index"""
        return cls(index % len(cls))


class DevelopmentalStage(Enum):
    """Developmental stages of the neural system"""
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4

    @property
    def name(self) -> str:
        return self._name_ or self.name.title()


@dataclass
class SensorData:
    """Sensory input data from the environment"""
    vision: float_array = field(default_factory=list)
    touch: float_array = field(default_factory=list)
    proprioception: float_array = field(default_factory=list)
    internal: float_array = field(default_factory=list)
    audio: float_array = field(default_factory=list)
    timestamp: float = 0.0
    
    def get_all_signals(self) -> float_array:
        """Get all sensory signals concatenated for brain input"""
        return self.vision + self.touch + self.proprioception + self.internal + self.audio
    
    def get_signal_count(self) -> int:
        """Get total number of sensory signals"""
        return len(self.get_all_signals())


@dataclass
class ActionResult:
    """Result of a motor action in the environment"""
    reward: float = 0.0
    success: bool = False
    message: str = ""
    
    @classmethod
    def from_tuple(cls, reward: float, success: bool, message: str = "") -> 'ActionResult':
        return cls(reward, success, message)


@dataclass
class LearningMetrics:
    """Metrics about learning that has occurred"""
    weight_change: float = 0.0
    learning_detected: bool = False
    neurons_activated: int = 0
    spike_count: int = 0
    plasticity_events: int = 0
    
    def to_dict(self) -> Dict:
        return {
            'weight_change': self.weight_change,
            'learning_detected': self.learning_detected,
            'neurons_activated': self.neurons_activated,
            'spike_count': self.spike_count,
            'plasticity_events': self.plasticity_events
        }


class NeuralSystem:
    """
    Main interface for creating and simulating neural systems with NLM.
    
    This class provides a high-level Python API for working with the NLM
    neural simulation framework. It encapsulates the C++ brain and provides
    easy-to-use methods for simulation, learning, and interaction.
    
    Attributes:
        neuron_count: Number of neurons in the system (default: 1000)
        region_count: Number of brain regions (default: 1)
        learning_enabled: Whether learning is enabled (default: True)
        development_enabled: Whether developmental changes occur (default: True)
    """
    
    def __init__(self, neuron_count: int = 1000, region_count: int = 1,
                 learning_enabled: bool = True, development_enabled: bool = True,
                 random_seed: Optional[int] = None):
        """
        Initialize a new neural system.
        
        Args:
            neuron_count: Number of neurons to create (default: 1000)
            region_count: Number of brain regions (default: 1)
            learning_enabled: Enable plasticity and learning (default: True)
            development_enabled: Enable developmental changes (default: True)
            random_seed: Random seed for reproducible results (default: None)
        """
        self.neuron_count = neuron_count
        self.region_count = region_count
        self.learning_enabled = learning_enabled
        self.development_enabled = development_enabled
        self.random_seed = random_seed or int(time.time() * 1000) % 10000
        
        # Internal state
        self._is_initialized = False
        self._current_step = 0
        self._last_metrics = LearningMetrics()
        self._simulation_start_time = 0.0
        
        # Lazy initialization - brain created on first use
        self._brain = None
        self._config = None
        
    def _ensure_initialized(self) -> None:
        """Ensure the neural system is initialized"""
        if not self._is_initialized:
            self._initialize()
    
    def _initialize(self) -> None:
        """Internal initialization method"""
        try:
            # This would normally call the C++ NLM library
            # For now, we'll create a Python implementation
            self._simulation_start_time = time.time()
            self._is_initialized = True
            
        except Exception as e:
            raise RuntimeError(f"Failed to initialize neural system: {e}")
    
    def initialize(self, config_file: Optional[str] = None) -> bool:
        """
        Initialize the neural system with the given configuration.
        
        Args:
            config_file: Path to configuration file (default: None uses defaults)
            
        Returns:
            True if initialization succeeded, False otherwise
        """
        try:
            # Apply configuration
            if config_file:
                self._load_config(config_file)
            else:
                self._load_default_config()
            
            # Initialize the brain (C++ layer)
            self._initialize()
            
            return True
            
        except Exception as e:
            print(f"Initialization failed: {e}")
            return False
    
    def _load_config(self, config_file: str) -> None:
        """Load configuration from file"""
        # Implementation for loading configuration from file
        # For now, use defaults
        pass
    
    def _load_default_config(self) -> None:
        """Load default configuration"""
        # Default configuration parameters
        self.config = {
            'random_seed': self.random_seed,
            'simulation_timestep': 0.001,
            'neuron_count': self.neuron_count,
            'region_count': self.region_count,
            'connection_probability': 0.15,
            'stdp_ltp_weight': 0.02,
            'stdp_ltd_weight': 0.015,
            'plasticity_learning_rate': 0.01,
            'synaptogenesis_rate': 0.0001,
            'pruning_rate': 0.00001,
            'dopamine_baseline': 0.0,
            'reward_discount_factor': 0.99,
            'max_simulation_steps': 10000
        }
    
    def step(self) -> bool:
        """
        Run one simulation step.
        
        This advances the neural system by one timestep, processing sensory input,
        running neural dynamics, and producing motor output.
        
        Returns:
            True if step completed successfully, False if simulation should stop
        """
        self._ensure_initialized()
        
        try:
            # Process current sensory input
            if hasattr(self, '_sensory_input') and self._sensory_input:
                self._process_sensory_input(self._sensory_input)
            
            # Update neural dynamics
            self._update_neural_dynamics()
            
            # Check development
            if self.development_enabled:
                self._update_development()
            
            # Check for learning
            self._check_for_learning()
            
            # Increment step counter
            self._current_step += 1
            
            return True
            
        except Exception as e:
            print(f"Step failed: {e}")
            return False
    
    def _process_sensory_input(self, sensory_data: SensorData) -> None:
        """Process sensory input from environment"""
        # This would interface with C++ neural system
        # For now, just store for later processing
        pass
    
    def _update_neural_dynamics(self) -> None:
        """Update neural dynamics for one timestep"""
        # This would interface with C++ neural system
        # For now, generate some basic activity
        self._neural_activity = self._generate_basic_activity()
    
    def _update_development(self) -> None:
        """Update developmental changes"""
        # This would interface with C++ developmental system
        # For now, just update developmental stage
        pass
    
    def _check_for_learning(self) -> None:
        """Check if learning occurred this timestep"""
        # This would interface with C++ plasticity system
        # For now, generate some metrics
        pass
    
    def _generate_basic_activity(self) -> float_array:
        """Generate basic neural activity for testing"""
        # Simple pattern for demonstration
        activity = [0.1] * min(self.neuron_count, 100)
        return activity
    
    def set_sensory_input(self, sensory_data: SensorData) -> None:
        """
        Set the current sensory input for the neural system.
        
        Args:
            sensory_data: SensorData object containing all sensory inputs
        """
        self._ensure_initialized()
        self._sensory_input = sensory_data
    
    def set_sensory_input_from_arrays(self, vision: Optional[float_array] = None,
                                     touch: Optional[float_array] = None,
                                     proprioception: Optional[float_array] = None,
                                     internal: Optional[float_array] = None,
                                     audio: Optional[float_array] = None) -> None:
        """
        Set sensory input using individual arrays.
        
        Args:
            vision: Vision input array (16x16 = 256 values)
            touch: Touch/proximity input array
            proprioception: Body position/velocity input
            internal: Internal homeostatic signals
            audio: Audio-like signals
        """
        self._ensure_initialized()
        
        # Create SensorData from arrays
        sensory_data = SensorData(
            vision=vision or [0.0] * 256,
            touch=touch or [0.0] * 8,
            proprioception=proprioception or [0.0] * 6,
            internal=internal or [0.0] * 4,
            audio=audio or [0.0] * 0,
            timestamp=time.time()
        )
        
        self.set_sensory_input(sensory_data)
    
    def get_motor_output(self) -> MotorCommand:
        """
        Get the motor command produced by the neural system.
        
        Returns:
            MotorCommand enum value representing the action to take
        """
        self._ensure_initialized()
        
        # This would interface with C++ motor system
        # For now, return a default command based on basic activity
        if hasattr(self, '_neural_activity') and self._neural_activity:
            avg_activity = sum(self._neural_activity) / len(self._neural_activity)
            
            # Simple decision making based on activity level
            if avg_activity > 0.8:
                return MotorCommand.MOVE_FORWARD
            elif avg_activity < 0.2:
                return MotorCommand.WAIT
            else:
                return MotorCommand.INTERACT
        
        return MotorCommand.WAIT
    
    def get_action(self) -> ActionResult:
        """
        Get a complete action with reward feedback.
        
        Returns:
            ActionResult containing reward and success information
        """
        command = self.get_motor_output()
        
        # Simple reward system based on command
        reward = 0.1
        if command == MotorCommand.WAIT:
            reward = -0.1  # Small penalty for waiting
        
        success = command != MotorCommand.WAIT
        message = f"Executing {command.to_string()}"
        
        return ActionResult(reward, success, message)
    
    def get_neural_activity(self) -> float_array:
        """
        Get current neural activity levels.
        
        Returns:
            List of firing rates for all neurons
        """
        self._ensure_initialized()
        return getattr(self, '_neural_activity', [0.0] * self.neuron_count)
    
    def get_spike_count(self) -> int:
        """
        Get total number of spikes generated in current simulation.
        
        Returns:
            Total spike count
        """
        # This would interface with C++ spike system
        return 0  # Placeholder
    
    def get_learning_metrics(self) -> LearningMetrics:
        """
        Get metrics about learning that has occurred.
        
        Returns:
            LearningMetrics object containing learning information
        """
        return self._last_metrics
    
    def has_learning_occurred(self) -> bool:
        """
        Check if significant learning has occurred.
        
        Returns:
            True if learning was detected, False otherwise
        """
        return self._last_metrics.learning_detected
    
    def get_simulation_step(self) -> int:
        """
        Get current simulation step number.
        
        Returns:
            Current step number
        """
        return self._current_step
    
    def get_simulation_time(self) -> float:
        """
        Get elapsed simulation time in seconds.
        
        Returns:
            Elapsed simulation time
        """
        if self._is_initialized:
            return time.time() - self._simulation_start_time
        return 0.0
    
    def reset(self) -> None:
        """
        Reset the neural system to its initial state.
        """
        self._current_step = 0
        self._last_metrics = LearningMetrics()
        self._is_initialized = False
        self._brain = None
        self._config = None
    
    def save_checkpoint(self, filepath: str) -> bool:
        """
        Save the current neural system state to a checkpoint file.
        
        Args:
            filepath: Path to save the checkpoint
            
        Returns:
            True if save succeeded, False otherwise
        """
        try:
            checkpoint_data = {
                'version': __version__,
                'neuron_count': self.neuron_count,
                'region_count': self.region_count,
                'current_step': self._current_step,
                'last_metrics': self._last_metrics.to_dict(),
                'config': getattr(self, 'config', {}),
                'timestamp': time.time()
            }
            
            with open(filepath, 'wb') as f:
                pickle.dump(checkpoint_data, f)
            
            return True
            
        except Exception as e:
            print(f"Failed to save checkpoint: {e}")
            return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """
        Load a neural system state from a checkpoint file.
        
        Args:
            filepath: Path to the checkpoint file
            
        Returns:
            True if load succeeded, False otherwise
        """
        try:
            with open(filepath, 'rb') as f:
                checkpoint_data = pickle.load(f)
            
            # Restore state
            self.neuron_count = checkpoint_data.get('neuron_count', self.neuron_count)
            self.region_count = checkpoint_data.get('region_count', self.region_count)
            self._current_step = checkpoint_data.get('current_step', 0)
            self._last_metrics = LearningMetrics(**checkpoint_data.get('last_metrics', {}))
            self.config = checkpoint_data.get('config', {})
            
            self._is_initialized = True
            
            return True
            
        except Exception as e:
            print(f"Failed to load checkpoint: {e}")
            return False


class ExperimentRunner:
    """
    Runner for NLM experiments and testing.
    
    This class provides utilities for running experiments, collecting metrics,
    and analyzing results from neural system simulations.
    """
    
    def __init__(self):
        self.results = []
    
    def run_experiment(self, experiment_name: str, system_factory: Callable,
                      steps: int, **kwargs) -> Dict:
        """
        Run a complete experiment with a neural system.
        
        Args:
            experiment_name: Name of the experiment
            system_factory: Callable that creates a NeuralSystem
            steps: Number of simulation steps to run
            **kwargs: Additional parameters for the experiment
            
        Returns:
            Dictionary containing experiment results
        """
        print(f"Running experiment: {experiment_name}")
        
        # Create system
        system = system_factory()
        
        # Initialize with config if provided
        if 'config_file' in kwargs:
            system.initialize(kwargs['config_file'])
        else:
            system.initialize()
        
        # Run simulation
        total_reward = 0.0
        max_steps_reached = False
        
        for step in range(steps):
            # Set up some basic sensory input for demonstration
            sensory_data = SensorData(
                vision=[0.1] * 256,
                touch=[0.0] * 8,
                proprioception=[0.0] * 6,
                internal=[0.0] * 4,
                audio=[0.0] * 0,
                timestamp=step * 0.001
            )
            system.set_sensory_input(sensory_data)
            
            # Step the system
            success = system.step()
            if not success:
                max_steps_reached = True
                break
            
            # Get reward from action
            action = system.get_action()
            total_reward += action.reward
            
            # Print progress every 100 steps
            if step % 100 == 0:
                print(f"  Step {step}/{steps}, Reward: {action.reward:.3f}")
        
        # Collect results
        metrics = system.get_learning_metrics()
        final_activity = system.get_neural_activity()
        spike_count = system.get_spike_count()
        
        result = {
            'experiment_name': experiment_name,
            'steps_completed': system.get_simulation_step(),
            'total_reward': total_reward,
            'max_steps_reached': max_steps_reached,
            'learning_detected': system.has_learning_occurred(),
            'final_metrics': {
                'weight_change': metrics.weight_change,
                'neurons_activated': metrics.neurons_activated,
                'spike_count': metrics.spike_count,
                'plasticity_events': metrics.plasticity_events
            },
            'final_activity_stats': {
                'mean': sum(final_activity) / len(final_activity) if final_activity else 0,
                'max': max(final_activity) if final_activity else 0,
                'min': min(final_activity) if final_activity else 0
            },
            'simulation_time': system.get_simulation_time(),
            'timestamp': time.time()
        }
        
        self.results.append(result)
        
        print(f"Experiment completed: {result['steps_completed']} steps, "
              f"Total reward: {total_reward:.3f}, Learning detected: {result['learning_detected']}")
        
        return result


# Global instances for convenience
_default_system: Optional[NeuralSystem] = None
_experiment_runner = ExperimentRunner()


def get_system() -> NeuralSystem:
    """Get the default neural system instance."""
    global _default_system
    if _default_system is None:
        _default_system = NeuralSystem()
    return _default_system


def initialize_system(neuron_count: int = 1000, region_count: int = 1,
                      learning_enabled: bool = True, development_enabled: bool = True) -> bool:
    """Initialize the default neural system."""
    global _default_system
    if _default_system is None:
        _default_system = NeuralSystem(neuron_count, region_count, learning_enabled, development_enabled)
    return _default_system.initialize()


def run_simple_simulation(steps: int = 100) -> Dict:
    """
    Run a simple demonstration simulation.
    
    This is a convenience function that creates a simple neural system
    and runs a demonstration simulation.
    
    Args:
        steps: Number of simulation steps to run (default: 100)
        
    Returns:
        Dictionary containing simulation results
    """
    return _experiment_runner.run_experiment(
        experiment_name="Simple Demo Simulation",
        system_factory=lambda: NeuralSystem(neuron_count=100, region_count=1),
        steps=steps
    )


def save_system_state(filepath: str) -> bool:
    """Save the current system state to a file."""
    system = get_system()
    return system.save_checkpoint(filepath)


def load_system_state(filepath: str) -> bool:
    """Load system state from a file."""
    system = get_system()
    return system.load_checkpoint(filepath)


def get_system_info() -> Dict:
    """Get information about the current system."""
    system = get_system()
    return {
        'version': __version__,
        'neuron_count': system.neuron_count,
        'region_count': system.region_count,
        'learning_enabled': system.learning_enabled,
        'development_enabled': system.development_enabled,
        'current_step': system.get_simulation_step(),
        'simulation_time': system.get_simulation_time()
    }


# Make the main class easily importable
__all__ = [
    'NeuralSystem',
    'ExperimentRunner',
    'SensorData',
    'ActionResult',
    'LearningMetrics',
    'MotorCommand',
    'DevelopmentalStage',
    'get_system',
    'initialize_system',
    'run_simple_simulation',
    'save_system_state',
    'load_system_state',
    'get_system_info'
]
