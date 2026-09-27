"""
NLM Core types and configuration.

This module contains the fundamental data types and configuration classes
used throughout the NLM system.
"""

from typing import Any, Dict, List, Optional, Union

from .config import Config
from .types import (
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
)

__all__ = [
    "Config",
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
]