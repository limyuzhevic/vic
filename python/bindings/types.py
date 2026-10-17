"""
NLM Python Bindings: Type Definitions

This module contains type definitions and enumerations that are exposed to Python.
It provides type hints and helper functions for working with NLM types in Python.
"""

from typing import Optional, List, Dict, Any, Union, Tuple
import numpy as np

# Import C++ type bindings from pybind11
from ._pynlm_types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType
)

# Type aliases for better Python readability
MembranePotential = float
SynapticWeight = float
Timestamp = float
TimestepDuration = float
FiringRate = float
Delay = int

# Vector types for convenience
Vector2D = Tuple[float, float]
Vector3D = Tuple[float, float, float]
Matrix = List[List[float]]

# Configuration types
ConfigValue = Union[int, float, str, bool, List[Any], Dict[str, Any]]
ConfigDict = Dict[str, ConfigValue]

# Neuron state types
NeuronStateDict = Dict[str, Any]
SynapseStateDict = Dict[str, Any]

# Memory types
MemoryTrace = Tuple[NeuronId, float]  # (neuron_id, activation_strength)
EpisodeData = Dict[str, Any]

# Action types
ActionParams = List[float]
MotorCommandParams = Dict[str, float]

# World object types
WorldObjectData = Dict[str, Any]

# Utility functions for type conversion and validation

def validate_neuron_type(neuron_type: NeuronType) -> NeuronType:
    """Validate neuron type."""
    valid_types = [NeuronType.Excitatory, NeuronType.Inhibitory, 
                   NeuronType.Modulatory, NeuronType.Sensory, 
                   NeuronType.Motor, NeuronType.Internal]
    if neuron_type not in valid_types:
        raise ValueError(f"Invalid neuron type: {neuron_type}")
    return neuron_type

def validate_synapse_type(synapse_type: SynapseType) -> SynapseType:
    """Validate synapse type."""
    valid_types = [SynapseType.Excitatory, SynapseType.Inhibitory,
                   SynapseType.Modulatory, SynapseType.Electrical,
                   SynapseType.GapJunction]
    if synapse_type not in valid_types:
        raise ValueError(f"Invalid synapse type: {synapse_type}")
    return synapse_type

def clamp_value(value: float, min_val: float, max_val: float) -> float:
    """Clamp a value between minimum and maximum bounds."""
    return max(min_val, min(max_val, value))

def normalize_array(values: List[float], min_val: float = 0.0, max_val: float = 1.0) -> List[float]:
    """Normalize an array of values to specified range."""
    if not values:
        return []
    
    min_original = min(values)
    max_original = max(values)
    
    if max_original == min_original:
        return [min_val] * len(values)
    
    return [
        clamp_value((v - min_original) / (max_original - min_original), min_val, max_val)
        for v in values
    ]

def create_neuron_id(value: int) -> NeuronId:
    """Create a NeuronId with validation."""
    if value < 0:
        raise ValueError("Neuron ID must be non-negative")
    return NeuronId(value)

def create_synapse_id(value: int) -> SynapseId:
    """Create a SynapseId with validation."""
    if value < 0:
        raise ValueError("Synapse ID must be non-negative")
    return SynapseId(value)

def create_region_id(value: int) -> RegionId:
    """Create a RegionId with validation."""
    if value < 0:
        raise ValueError("Region ID must be non-negative")
    return RegionId(value)

# Constants for common values
DEFAULT_NEURON_COUNT = 1000
DEFAULT_REGION_COUNT = 1
DEFAULT_CONNECTION_PROBABILITY = 0.1
DEFAULT_SIMULATION_TIMESTEP = 0.001
DEFAULT_REFACTORY_PERIOD = 5
DEFAULT_RESTING_POTENTIAL = -70.0
DEFAULT_THRESHOLD = -55.0
DEFAULT_RESET_POTENTIAL = -70.0
DEFAULT_LEAK_CONDUCTANCE = 10.0

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"

# Re-export commonly used types
__all__ = [
    # Core types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage",
    "FiringState", "ActionType", "MotorCommand", "WorldObjectType",
    
    # Pythonic type aliases
    "MembranePotential", "SynapticWeight", "Timestamp", "TimestepDuration",
    "FiringRate", "Delay", "Vector2D", "Vector3D", "Matrix",
    
    # Configuration types
    "ConfigValue", "ConfigDict",
    
    # Utility functions
    "validate_neuron_type", "validate_synapse_type",
    "clamp_value", "normalize_array",
    "create_neuron_id", "create_synapse_id", "create_region_id",
    
    # Constants
    "DEFAULT_NEURON_COUNT", "DEFAULT_REGION_COUNT",
    "DEFAULT_CONNECTION_PROBABILITY", "DEFAULT_SIMULATION_TIMESTEP",
    "DEFAULT_REFACTORY_PERIOD", "DEFAULT_RESTING_POTENTIAL",
    "DEFAULT_THRESHOLD", "DEFAULT_RESET_POTENTIAL", "DEFAULT_LEAK_CONDUCTANCE",
]