"""
Synapse class for NLM neural simulation framework.

This module provides a Python interface to the C++ Synapse class with enhanced
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
SynapseId = Any
SynapseType = Any
NeuronId = Any

@dataclass
class SynapseState:
    """State information for synapse"""
    id: SynapseId = SynapseId()
    type: SynapseType = SynapseType.Excitatory
    source_id: NeuronId = NeuronId()
    target_id: NeuronId = NeuronId()
    weight: float = 0.0
    delay: int = 0
    active: bool = False
    last_spike_time: float = 0.0

class Synapse:
    """
    Synapse class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ Synapse class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self, synapse_id: SynapseId = None, synapse_type: SynapseType = None):
        """
        Initialize synapse.
        
        Args:
            synapse_id: Synapse ID (optional)
            synapse_type: Synapse type (optional)
        """
        self._logger = PythonSessionLogger()
        self._state = SynapseState()
        
        # Set ID and type
        if synapse_id is not None:
            self._state.id = synapse_id
        if synapse_type is not None:
            self._state.type = synapse_type
        
        # Log initialization
        self._logger.logEntry(f"Synapse initialized: id={self._state.id.value}, type={self._state.type.name}")
    
    # ID and type properties
    
    @property
    def id(self) -> SynapseId:
        """Get synapse ID."""
        return self._state.id
    
    @property
    def type(self) -> SynapseType:
        """Get synapse type."""
        return self._state.type
    
    @type.setter
    def type(self, value: SynapseType) -> None:
        """
        Set synapse type.
        
        Args:
            value: Synapse type
        """
        self._state.type = value
    
    @property
    def source_id(self) -> NeuronId:
        """Get source neuron ID."""
        return self._state.source_id
    
    @source_id.setter
    def source_id(self, value: NeuronId) -> None:
        """
        Set source neuron ID.
        
        Args:
            value: Source neuron ID
        """
        self._state.source_id = value
    
    @property
    def target_id(self) -> NeuronId:
        """Get target neuron ID."""
        return self._state.target_id
    
    @target_id.setter
    def target_id(self, value: NeuronId) -> None:
        """
        Set target neuron ID.
        
        Args:
            value: Target neuron ID
        """
        self._state.target_id = value
    
    @property
    def weight(self) -> float:
        """Get synapse weight."""
        return self._state.weight
    
    @weight.setter
    def weight(self, value: float) -> None:
        """
        Set synapse weight.
        
        Args:
            value: Synapse weight
        """
        self._state.weight = max(0.0, min(1.0, value))
    
    @property
    def delay(self) -> int:
        """Get synapse delay."""
        return self._state.delay
    
    @delay.setter
    def delay(self, value: int) -> None:
        """
        Set synapse delay.
        
        Args:
            value: Synapse delay
        """
        self._state.delay = max(0, min(1000, value))
    
    @property
    def active(self) -> bool:
        """Check if synapse is active."""
        return self._state.active
    
    @active.setter
    def active(self, value: bool) -> None:
        """
        Set synapse active state.
        
        Args:
            value: Active state
        """
        self._state.active = value
    
    @property
    def last_spike_time(self) -> float:
        """Get last spike time."""
        return self._state.last_spike_time
    
    # Synapse state management methods
    
    def transmit(self) -> bool:
        """
        Transmit signal through synapse.
        
        Returns:
            True if transmission was successful, False otherwise
        """
        if not self._state.active:
            return False
        
        try:
            # Update last spike time
            self._state.last_spike_time = time.time()
            
            # Log transmission
            self._logger.logEntry(f"Synapse transmitted: id={self._state.id.value}, weight={self._state.weight}")
            
            return True
            
        except Exception as e:
            self._format_python_error(f"Synapse transmission failed: {e}")
            return False
    
    def reset(self) -> None:
        """Reset synapse state."""
        self._state.active = False
        self._state.last_spike_time = 0.0
        self._logger.logEntry(f"Synapse reset: id={self._state.id.value}")
    
    def update(self, dt: float) -> None:
        """
        Update synapse state.
        
        Args:
            dt: Time step
        """
        # Simple synapse dynamics (placeholder for C++ implementation)
        if self._state.active:
            # Decay weight
            self._state.weight *= 0.99
    
    def getState(self) -> Dict[str, Any]:
        """
        Get synapse state as dictionary.
        
        Returns:
            Dictionary containing synapse state
        """
        return {
            "id": self._state.id.value,
            "type": self._state.type.name,
            "source_id": self._state.source_id.value,
            "target_id": self._state.target_id.value,
            "weight": self._state.weight,
            "delay": self._state.delay,
            "active": self._state.active,
            "last_spike_time": self._state.last_spike_time,
        }
    
    def __repr__(self) -> str:
        """String representation of synapse."""
        return f"<Synapse: id={self._state.id.value}, type={self._state.type.name}>"
    
    def __str__(self) -> str:
        """String representation of synapse."""
        return f"NLM Synapse {self._state.id.value} ({self._state.type.name})"

__all__ = ['Synapse']
