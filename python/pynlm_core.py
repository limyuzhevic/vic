"""
pynlm_core - Low-level C++ bindings interface for NLM

This module provides the raw C++ bindings that are automatically generated
from the pybind11 bindings in bindings.cpp. This is an internal module that
should not be imported directly by users. Use `pynlm` instead.

The module exposes the C++ classes and functions with pybind11's default
binding interface. Type hints and additional functionality are provided
by the main `pynlm` module.
"""

import sys
import importlib

# Import the compiled C++ extension module
try:
    import pynlm_core
    _PYNLMC_AVAILABLE = True
except ImportError as e:
    _PYNLMC_AVAILABLE = False
    _PYNLMC_ERROR = e

if _PYNLMC_AVAILABLE:
    # Re-export all symbols from the C++ module
    from pynlm_core import *
else:
    # Define placeholder classes and functions for when the C++ extension is not available
    # This is for development and testing purposes
    
    # Exception classes
    class RuntimeError(Exception):
        pass

    # Placeholder type definitions
    class NeuronId:
        def __init__(self, value=0):
            self.value = value
        
        def __eq__(self, other):
            return isinstance(other, NeuronId) and self.value == other.value
        
        def __ne__(self, other):
            return not self.__eq__(other)
        
        def __hash__(self):
            return hash(self.value)
        
        def index(self):
            return self.value
        
        def __repr__(self):
            return f"<NeuronId: {self.value}>"

    class SynapseId(NeuronId):
        def __init__(self, value=0):
            super().__init__(value)

    class RegionId(NeuronId):
        def __init__(self, value=0):
            super().__init__(value)

    class PopulationId(NeuronId):
        def __init__(self, value=0):
            super().__init__(value)

    # Enum placeholders
    from enum import Enum
    
    class NeuronType(Enum):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Sensory = 3
        Motor = 4
        Internal = 5

    class SynapseType(Enum):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Electrical = 3
        GapJunction = 4

    class DevelopmentalStage(Enum):
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4

    class FiringState(Enum):
        Resting = 0
        Active = 1
        Refractory = 2
        Inhibited = 3

    class ActionType(Enum):
        MoveForward = 0
        MoveBackward = 1
        MoveLeft = 2
        MoveRight = 3
        TurnLeft = 4
        TurnRight = 5
        Look = 6
        LookUp = 7
        LookDown = 8
        Interact = 9
        Eat = 10
        Drink = 11
        Rest = 12
        Wait = 13
        Custom = 14

    class MotorCommand(Enum):
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        LookLeft = 4
        LookRight = 5
        Interact = 6
        Wait = 7

    class WorldObjectType(Enum):
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4

    # Invalid IDs
    INVALID_NEURON_ID = NeuronId(2**64 - 1)
    INVALID_SYNAPSE_ID = SynapseId(2**64 - 1)
    INVALID_REGION_ID = RegionId(2**64 - 1)
    INVALID_POPULATION_ID = PopulationId(2**64 - 1)

    # Factory function placeholders
    def createDefaultConfig():
        """Create a default configuration."""
        raise RuntimeError("C++ extension not available. Install pynlm_core.")

    def createBrain(config):
        """Create a brain with the given configuration."""
        raise RuntimeError("C++ extension not available. Install pynlm_core.")

    def createAgentBrain(brain):
        """Create an agent brain interface."""
        raise RuntimeError("C++ extension not available. Install pynlm_core.")

    def createSimpleWorld():
        """Create a simple world for simulation."""
        raise RuntimeError("C++ extension not available. Install pynlm_core.")

# Export all symbols
__all__ = [
    # C++ extension availability
    '_PYNLMC_AVAILABLE',
    '_PYNLMC_ERROR',
    
    # Exception classes
    'RuntimeError',
    
    # Type aliases
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Enums
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Invalid IDs
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createAgentBrain', 'createSimpleWorld',
]

# Module metadata
__version__ = '0.1.0'
__author__ = 'NLM Project'
__license__ = 'MIT'

if not _PYNLMC_AVAILABLE:
    # Log warning when C++ extension is not available
    import warnings
    warnings.warn(
        "NLM C++ extension not available. Using placeholder implementations. "
        "Install pynlm_core to use the full functionality. "
        f"Original error: {_PYNLMC_ERROR}",
        ImportWarning,
        stacklevel=2
    )
