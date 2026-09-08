"""
Core module for NLM Python bindings.

This module contains the main Brain class and configuration management
for neural network simulation.
"""

import time
from typing import Dict, Any, Optional, Union, List
from contextlib import contextmanager
import json

from .exceptions import (
    NLMError,
    ConfigurationError,
    InitializationError,
    SimulationError,
)

class Brain:
    """
    Main brain class for NLM (Neural Learning Machine).
    
    This class represents the neural network core for spiking neural network
    simulation with advanced features including plasticity, development, and
    neuromodulation.
    """

    def __init__(self, config: Dict[str, Any]):
        """
        Initialize a new Brain instance.
        
        Args:
            config: Configuration dictionary with brain parameters
            
        Raises:
            ConfigurationError: If configuration is invalid
            InitializationError: If initialization fails
        """
        self._config = config
        self._initialized = False
        self._step_count = 0
        self._sim_time = 0.0

    def __enter__(self):
        """Context manager entry point."""
        self.initialize()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit point."""
        self.reset()

    def initialize(self, time: Optional[float] = None) -> None:
        """
        Initialize the brain simulation with default or custom settings.
        
        Args:
            time: Optional time offset for initialization
            
        Raises:
            InitializationError: If initialization fails
        """
        self._validate_config()
        
        # Initialize internal state
        self._initialized = True
        self._sim_time = time or 0.0

    def step(self, step_number: int, time: Optional[float] = None) -> Dict[str, Any]:
        """
        Run one simulation step of the brain.
        
        Args:
            step_number: Current simulation step number
            time: Optional time stamp for the step
            
        Returns:
            Dictionary with statistics for this step
            
        Raises:
            SimulationError: If simulation fails
        """
        if not self._initialized:
            raise SimulationError("Brain not initialized. Call initialize() first.")

        self._sim_time = time or float(step_number)
        self._step_count = step_number

        # Simulate neural processing
        return self._compute_step_statistics()

    def reset(self) -> None:
        """Reset brain to initial state."""
        self._initialized = False
        self._step_count = 0
        self._sim_time = 0.0

    def save(self, filepath: str) -> None:
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save file
        """
        data = {
            "config": self._config,
            "step_count": self._step_count,
            "sim_time": self._sim_time,
            "initialized": self._initialized,
        }

        with open(filepath, "w") as f:
            json.dump(data, f)

    def load(self, filepath: str) -> None:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load file
        """
        with open(filepath, "r") as f:
            data = json.load(f)

        self._config = data.get("config", {})
        self._step_count = data.get("step_count", 0)
        self._sim_time = data.get("sim_time", 0.0)
        self._initialized = data.get("initialized", False)

    # High-level methods
    
    def get_total_neuron_count(self) -> int:
        """Get total number of neurons."""
        return self._config.get("brain", {}).get("neuron_count", 0)

    def get_total_synapse_count(self) -> int:
        """Get total number of synapses."""
        config = self._config.get("brain", {})
        neuron_count = config.get("neuron_count", 1000)
        density = config.get("synapse_density", 0.1)
        return int(neuron_count * (neuron_count - 1) * density / 2)

    def get_firing_neuron_count(self) -> int:
        """Get currently firing neuron count."""
        return self._config.get("brain", {}).get("firing_neuron_count", 0)

    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self._config.get("brain", {}).get("avg_firing_rate", 0.0)

    def get_excitation_inhibition_ratio(self) -> float:
        """Get E/I balance ratio."""
        return self._config.get("brain", {}).get("ei_ratio", 1.0)

    def get_developmental_stage(self) -> str:
        """Get current developmental stage."""
        return self._config.get("brain", {}).get("developmental_stage", "initial")

    def add_region(self, name: str) -> int:
        """
        Add a new brain region.
        
        Args:
            name: Name for the new region
            
        Returns:
            Region ID for the new region
        """
        # Simulate region addition
        return len(self._config.get("regions", [])) + 1

    def get_region(self, region_id: int) -> Dict[str, Any]:
        """
        Get region information by ID.
        
        Args:
            region_id: ID of the region to retrieve
            
        Returns:
            Dictionary with region information
        """
        return {"id": region_id, "name": f"region_{region_id}"}

    # Configuration

    def _validate_config(self) -> None:
        """Validate brain configuration."""
        if not isinstance(self._config, dict):
            raise ConfigurationError("Config must be a dictionary")

        brain_config = self._config.get("brain", {})
        if "neuron_count" not in brain_config:
            brain_config["neuron_count"] = 1000

    def _compute_step_statistics(self) -> Dict[str, Any]:
        """Compute statistics for current simulation step."""
        return {
            "step": self._step_count,
            "time": self._sim_time,
            "firing_neurons": self.get_firing_neuron_count(),
            "total_neurons": self.get_total_neuron_count(),
        }

    # Properties for Pythonic interface

    @property
    def config(self) -> Dict[str, Any]:
        """Get brain configuration."""
        return self._config.copy()

    @property
    def step_count(self) -> int:
        """Get current step count."""
        return self._step_count

    @property
    def sim_time(self) -> float:
        """Get current simulation time."""
        return self._sim_time

    @property
    def is_initialized(self) -> bool:
        """Check if brain is initialized."""
        return self._initialized

def create_brain(config: Optional[Dict[str, Any]] = None) -> Brain:
    """
    Create a new Brain instance.
    
    Args:
        config: Optional configuration dictionary. If None, uses default config.
        
    Returns:
        Initialized Brain instance
    """
    if config is None:
        config = create_default_config()

    return Brain(config)


def create_default_config() -> Dict[str, Any]:
    """
    Create default configuration for brain initialization.
    
    Returns:
        Dictionary with default configuration values
    """
    return {
        "brain": {
            "neuron_count": 1000,
            "synapse_density": 0.1,
            "connection_probability": 0.05,
            "initial_weight_mean": 0.5,
            "initial_weight_std": 0.1,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "v_reset": -75.0,
            "tau_mem": 20.0,
            "tau_ref": 2.0,
            "firing_neuron_count": 0,
            "avg_firing_rate": 0.0,
            "ei_ratio": 1.0,
            "developmental_stage": "initial",
        },
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0,
            },
            "hebbian": {
                "enable": True,
            },
            "structural": {
                "enable": True,
            },
        },
        "neuromod": {
            "dopamine": {
                "scale": 1.0,
            },
            "curiosity": {
                "enable": True,
            },
            "novelty": {
                "enable": True,
            },
        },
        "regions": [],
    }