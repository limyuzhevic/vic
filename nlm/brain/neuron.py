"""
Neuron class for NLM neural simulation framework.

This module provides a Python interface to the C++ Neuron class with enhanced
Python features including automatic numpy conversion, error formatting, and
time measurement utilities.
"""

import time
import numpy as np
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass
import traceback

# Python utilities
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
)

# Forward references
NeuronId = Any
NeuronType = Any
FiringState = Any

@dataclass
class NeuronState:
    """State information for neuron"""
    id: NeuronId = NeuronId()
    type: NeuronType = NeuronType.Excitatory
    region_id: NeuronId = NeuronId()
    active: bool = False
    firing_state: FiringState = FiringState.Resting
    potential: float = 0.0
    last_spike_time: float = 0.0
    firing_rate: float = 0.0

class Neuron:
    """
    Neuron class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ Neuron class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self, neuron_id: NeuronId = None, neuron_type: NeuronType = None):
        """
        Initialize neuron.
        
        Args:
            neuron_id: Neuron ID (optional)
            neuron_type: Neuron type (optional)
        """
        self._logger = PythonSessionLogger()
        self._state = NeuronState()
        
        # Set ID and type
        if neuron_id is not None:
            self._state.id = neuron_id
        if neuron_type is not None:
            self._state.type = neuron_type
        
        # Log initialization
        self._logger.logEntry(f"Neuron initialized: id={self._state.id.value}, type={self._state.type.name}")
    
    # ID and type properties
    
    @property
    def id(self) -> NeuronId:
        """Get neuron ID."""
        return self._state.id
    
    @property
    def type(self) -> NeuronType:
        """Get neuron type."""
        return self._state.type
    
    @type.setter
    def type(self, value: NeuronType) -> None:
        """
        Set neuron type.
        
        Args:
            value: Neuron type
        """
        self._state.type = value
    
    @property
    def region_id(self) -> NeuronId:
        """Get region ID."""
        return self._state.region_id
    
    @region_id.setter
    def region_id(self, value: NeuronId) -> None:
        """
        Set region ID.
        
        Args:
            value: Region ID
        """
        self._state.region_id = value
    
    # State properties
    
    @property
    def active(self) -> bool:
        """Check if neuron is active."""
        return self._state.active
    
    @property
    def firing_state(self) -> FiringState:
        """Get neuron firing state."""
        return self._state.firing_state
    
    @property
    def potential(self) -> float:
        """Get membrane potential."""
        return self._state.potential
    
    @potential.setter
    def potential(self, value: float) -> None:
        """
        Set membrane potential.
        
        Args:
            value: Membrane potential
        """
        self._state.potential = max(-1.0, min(1.0, value))
    
    @property
    def last_spike_time(self) -> float:
        """Get last spike time."""
        return self._state.last_spike_time
    
    @property
    def firing_rate(self) -> float:
        """Get firing rate."""
        return self._state.firing_rate
    
    # State management methods
    
    def activate(self) -> None:
        """Activate neuron."""
        self._state.active = True
        self._state.firing_state = FiringState.Active
        self._logger.logEntry(f"Neuron activated: id={self._state.id.value}")
    
    def deactivate(self) -> None:
        """Deactivate neuron."""
        self._state.active = False
        self._state.firing_state = FiringState.Resting
        self._logger.logEntry(f"Neuron deactivated: id={self._state.id.value}")
    
    def spike(self) -> None:
        """Make neuron spike."""
        self._state.active = True
        self._state.firing_state = FiringState.Active
        self._state.last_spike_time = time.time()
        self._state.firing_rate += 1.0
        self._logger.logEntry(f"Neuron spiked: id={self._state.id.value}")
    
    def reset(self) -> None:
        """Reset neuron state."""
        self._state.active = False
        self._state.firing_state = FiringState.Resting
        self._state.potential = 0.0
        self._logger.logEntry(f"Neuron reset: id={self._state.id.value}")
    
    def update(self, dt: float) -> None:
        """
        Update neuron state.
        
        Args:
            dt: Time step
        """
        # Simple neuron dynamics (placeholder for C++ implementation)
        if self._state.active:
            # Decay potential
            self._state.potential *= 0.99
            
            # Update firing rate
            self._state.firing_rate = max(0.0, self._state.firing_rate * 0.99)
    
    def getState(self) -> Dict[str, Any]:
        """
        Get neuron state as dictionary.
        
        Returns:
            Dictionary containing neuron state
        """
        return {
            "id": self._state.id.value,
            "type": self._state.type.name,
            "region_id": self._state.region_id.value,
            "active": self._state.active,
            "firing_state": self._state.firing_state.name,
            "potential": self._state.potential,
            "last_spike_time": self._state.last_spike_time,
            "firing_rate": self._state.firing_rate,
        }
    
    def __repr__(self) -> str:
        """String representation of neuron."""
        return f"<Neuron: id={self._state.id.value}, type={self._state.type.name}>"
    
    def __str__(self) -> str:
        """String representation of neuron."""
        return f"NLM Neuron {self._state.id.value} ({self._state.type.name})"

__all__ = ['Neuron']
