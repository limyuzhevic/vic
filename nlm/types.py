# Types module defining all NLM type definitions

from __future__ import annotations

import uuid
from dataclasses import dataclass, field
from enum import Enum
from typing import Any, Dict, Optional, Union
from datetime import datetime

# Basic identifiers
@dataclass(frozen=True)
class NeuronId:
    """Unique identifier for a neuron."""
    
    value: int = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    @property
    def index(self) -> int:
        """Get the neuron index."""
        return self.value
    
    def __str__(self) -> str:
        return f"NeuronId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()

@dataclass(frozen=True)
class SynapseId:
    """Unique identifier for a synapse."""
    
    value: int = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    @property
    def index(self) -> int:
        """Get the synapse index."""
        return self.value
    
    def __str__(self) -> str:
        return f"SynapseId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()

@dataclass(frozen=True)
class RegionId:
    """Unique identifier for a brain region."""
    
    value: int = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    @property
    def index(self) -> int:
        """Get the region index."""
        return self.value
    
    def __str__(self) -> str:
        return f"RegionId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()

@dataclass(frozen=True)
class PopulationId:
    """Unique identifier for a neuron population."""
    
    value: int = field(default_factory=lambda: int(uuid.uuid4().hex[:8], 16))
    
    def __str__(self) -> str:
        return f"PopulationId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()

# Enum types
class NeuronType(Enum):
    """Neuron type enumeration."""
    
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"
class SynapseType(Enum):
    """Synapse type enumeration."""
    
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"
class DevelopmentalStage(Enum):
    """Developmental stage enumeration."""
    
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
class FiringState(Enum):
    """Neuron firing state enumeration."""
    
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"
class ActionType(Enum):
    """Action type enumeration."""
    
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"
class MotorCommand(Enum):
    """Motor command enumeration."""
    
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"
class WorldObjectType(Enum):
    """World object type enumeration."""
    
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
class ActionResult:
    """Result from an action in the world."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        self.reward = reward
        self.success = success
        self.message = message
        self.timestamp = datetime.now()
    
    def __str__(self) -> str:
        status = "SUCCESS" if self.success else "FAILURE"
        return f"ActionResult(reward={self.reward}, success={self.success}, message='{self.message}')"
    
    def __repr__(self) -> str:
        return self.__str__()

# Constants
INVALID_NEURON_ID = NeuronId(0)
INVALID_SYNAPSE_ID = SynapseId(0)
INVALID_REGION_ID = RegionId(0)
INVALID_POPULATION_ID = PopulationId(0)

__all__ = [
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    "ActionResult",
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
]