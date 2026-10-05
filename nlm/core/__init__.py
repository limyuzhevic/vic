"""
NLM Core Module

Core functionality for the NLM neural simulation framework.
Includes configuration management, type definitions, and fundamental utilities.
"""

from .config import Config
from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    MembranePotential, SimulationStep, Timestamp, Delay, PlasticityFlags
)

__all__ = [
    'Config',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'MembranePotential', 'SimulationStep', 'Timestamp', 'Delay', 'PlasticityFlags',
]

# Import configuration management utilities
from . import config
from . import types
