"""
Type definitions and enumerations for NLM Python bindings.

Provides all neural network data types, identifiers, and enumerations used
throughout the NLM system.
"""

import pybind11
from typing import NamedTuple, Union

# Import the C++ extensions
import pynlm
from pynlm import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType
)

__all__ = [
    # Identifiers
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Neural types
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
]
