"""
NLM Python Bindings - Core Types Module

This module defines the core data types and enumerations used by the NLM Python bindings.
These types correspond to the C++ types used in the NLM neural simulation framework.
"""

from enum import Enum
from typing import Dict, Any
import numpy as np
class NeuronType(Enum):
    """Neuron types in the NLM brain."""
    
    Sensory = 0    # Sensory neurons for input processing
    Internal = 1   # Internal processing neurons
    Motor = 2      # Motor neurons for action output
    
    @classmethod
    def from_value(cls, value: int) -> 'NeuronType':
        """Convert integer value to NeuronType."""
        return cls(value)
    
    def to_value(self) -> int:
        """Convert NeuronType to integer value."""
        return self.value


class FiringState(Enum):
    """Firing states of neurons."""
    
    Resting = 0     # Neuron at rest
    Active = 1      # Neuron actively firing
    Refractory = 2  # Neuron in refractory period
    
    @classmethod
    def from_value(cls, value: int) -> 'FiringState':
        """Convert integer value to FiringState."""
        return cls(value)
    
    def to_value(self) -> int:
        """Convert FiringState to integer value."""
        return self.value


class DevelopmentalStage(Enum):
    """Developmental stages of the brain."""
    
    Initial = 0          # Initial developmental stage (high plasticity)
    CriticalPeriod = 1   # Critical period (sensitive learning)
    Maturation = 2       # Maturation phase (reduced plasticity)
    Adult = 3            # Adult stage (stable, low plasticity)
    
    @classmethod
    def from_value(cls, value: int) -> 'DevelopmentalStage':
        """Convert integer value to DevelopmentalStage."""
        return cls(value)
    
    def to_value(self) -> int:
        """Convert DevelopmentalStage to integer value."""
        return self.value

    def get_plasticity_factor(self) -> float:
        """Get plasticity factor for this developmental stage."""
        factors = {
            DevelopmentalStage.Initial: 1.0,      # High plasticity
            DevelopmentalStage.CriticalPeriod: 0.8,  # Reduced plasticity
            DevelopmentalStage.Maturation: 0.5,   # Low plasticity
            DevelopmentalStage.Adult: 0.2,       # Minimal plasticity
        }
        return factors.get(self, 1.0)


class ActionType(Enum):
    """Types of actions that can be produced by the brain."""
    
    Wait = 0            # No action, wait
    MoveForward = 1     # Move forward
    TurnLeft = 2        # Turn left
    TurnRight = 3       # Turn right
    
    @classmethod
    def from_value(cls, value: int) -> 'ActionType':
        """Convert integer value to ActionType."""
        return cls(value)
    
    def to_value(self) -> int:
        """Convert ActionType to integer value."""
        return self.value

    def get_name(self) -> str:
        """Get human-readable name of action."""
        names = {
            ActionType.Wait: "Wait",
            ActionType.MoveForward: "Move Forward",
            ActionType.TurnLeft: "Turn Left",
            ActionType.TurnRight: "Turn Right",
        }
        return names.get(self, "Unknown")


class RegionId:
    """Identifier for neural regions."""
    
    def __init__(self, value: int):
        self.value = value
        
    def __int__(self):
        return self.value
        
    def __eq__(self, other):
        if isinstance(other, RegionId):
            return self.value == other.value
        return self.value == other
        
    def __hash__(self):
        return hash(self.value)
        
    def __str__(self):
        return f"RegionId({self.value})"
        
    def __repr__(self):
        return self.__str__()


class NeuronId:
    """Identifier for neurons."""
    
    def __init__(self, value: int):
        self.value = value
        
    def __int__(self):
        return self.value
        
    def __eq__(self, other):
        if isinstance(other, NeuronId):
            return self.value == other.value
        return self.value == other
        
    def __hash__(self):
        return hash(self.value)
        
    def __str__(self):
        return f"NeuronId({self.value})"
        
    def __repr__(self):
        return self.__str__()


# Type aliases for better type hints
MembranePotential = float
SimulationStep = int
Timestamp = float
Delay = int
TimestepDuration = float


class Config:
    """Configuration for NLM brain simulation."""
    
    def __init__(self):
        self._data: Dict[str, Any] = {}
        
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value."""
        self._data[key] = value
        
    def get(self, key: str) -> Any:
        """Get a configuration value."""
        return self._data.get(key)
        
    def get_or(self, key: str, default: Any) -> Any:
        """Get a configuration value or return default if not set."""
        return self._data.get(key, default)
        
    def has(self, key: str) -> bool:
        """Check if configuration has a key."""
        return key in self._data
        
    def remove(self, key: str) -> None:
        """Remove a configuration value."""
        self._data.pop(key, None)
        
    def clear(self) -> None:
        """Clear all configuration values."""
        self._data.clear()
        
    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._data.keys())
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self._data.copy()
        
    def from_dict(self, config_dict: Dict[str, Any]) -> None:
        """Load configuration from dictionary."""
        self._data = config_dict.copy()
        
    def summary(self) -> str:
        """Get configuration summary."""
        lines = [f"Configuration ({len(self._data)} entries):"]
        for key, value in self._data.items():
            if isinstance(value, str):
                lines.append(f"  {key} = \"{value}\"")
            elif isinstance(value, list):
                lines.append(f"  {key} = {value}")
            else:
                lines.append(f"  {key} = {value}")
        return "\n".join(lines)


class PlasticityFlags:
    """Flags for synaptic plasticity rules."""
    
    def __init__(self, stdp: bool = False, hebbian: bool = False):
        self.stdp = stdp
        self.hebbian = hebbian
        
    def __str__(self) -> str:
        flags = []
        if self.stdp:
            flags.append("STDP")
        if self.hebbian:
            flags.append("Hebbian")
        return "Plasticity(" + ", ".join(flags) + ")"


# Utility functions for type conversion
def to_python_neuron_type(cpp_type: int) -> NeuronType:
    """Convert C++ neuron type to Python enum."""
    return NeuronType.from_value(cpp_type)


def to_cpp_neuron_type(python_type: NeuronType) -> int:
    """Convert Python neuron type to C++ enum."""
    return python_type.to_value()


def to_python_firing_state(cpp_state: int) -> FiringState:
    """Convert C++ firing state to Python enum."""
    return FiringState.from_value(cpp_state)


def to_cpp_firing_state(python_state: FiringState) -> int:
    """Convert Python firing state to C++ enum."""
    return python_state.to_value()


def to_python_developmental_stage(cpp_stage: int) -> DevelopmentalStage:
    """Convert C++ developmental stage to Python enum."""
    return DevelopmentalStage.from_value(cpp_stage)


def to_cpp_developmental_stage(python_stage: DevelopmentalStage) -> int:
    """Convert Python developmental stage to C++ enum."""
    return python_stage.to_value()


def to_python_action_type(cpp_action: int) -> ActionType:
    """Convert C++ action type to Python enum."""
    return ActionType.from_value(cpp_action)


def to_cpp_action_type(python_action: ActionType) -> int:
    """Convert Python action type to C++ enum."""
    return python_action.to_value()


# Module exports
__all__ = [
    'NeuronType',
    'FiringState',
    'DevelopmentalStage',
    'ActionType',
    'RegionId',
    'NeuronId',
    'MembranePotential',
    'SimulationStep',
    'Timestamp',
    'Delay',
    'TimestepDuration',
    'Config',
    'PlasticityFlags',
    'to_python_neuron_type',
    'to_cpp_neuron_type',
    'to_python_firing_state',
    'to_cpp_firing_state',
    'to_python_developmental_stage',
    'to_cpp_developmental_stage',
    'to_python_action_type',
    'to_cpp_action_type',
]
