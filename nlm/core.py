# NLM Core Package
# Core functionality for NLM (Neural Learning Machine)

from __future__ import annotations

import json
import time
import numpy as np
from typing import Any, Dict, List, Optional, Union, Callable
from enum import Enum

# Basic types module
class NeuronId:
    """Unique identifier for a neuron."""
    
    def __init__(self, value: int = None):
        if value is None:
            import uuid
            value = int(uuid.uuid4().hex[:8], 16)
        self.value = value
    
    @property
    def index(self) -> int:
        """Get the neuron index."""
        return self.value
    
    def __str__(self) -> str:
        return f"NeuronId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    def __eq__(self, other) -> bool:
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)
class SynapseId:
    """Unique identifier for a synapse."""
    
    def __init__(self, value: int = None):
        if value is None:
            import uuid
            value = int(uuid.uuid4().hex[:8], 16)
        self.value = value
    
    @property
    def index(self) -> int:
        """Get the synapse index."""
        return self.value
    
    def __str__(self) -> str:
        return f"SynapseId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    def __eq__(self, other) -> bool:
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)
class RegionId:
    """Unique identifier for a brain region."""
    
    def __init__(self, value: int = None):
        if value is None:
            import uuid
            value = int(uuid.uuid4().hex[:8], 16)
        self.value = value
    
    @property
    def index(self) -> int:
        """Get the region index."""
        return self.value
    
    def __str__(self) -> str:
        return f"RegionId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    def __eq__(self, other) -> bool:
        return isinstance(other, RegionId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)
class PopulationId:
    """Unique identifier for a neuron population."""
    
    def __init__(self, value: int = None):
        if value is None:
            import uuid
            value = int(uuid.uuid4().hex[:8], 16)
        self.value = value
    
    def __str__(self) -> str:
        return f"PopulationId({self.value})"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    def __eq__(self, other) -> bool:
        return isinstance(other, PopulationId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)
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
        self.timestamp = time.time()
    
    def __str__(self) -> str:
        status = "SUCCESS" if self.success else "FAILURE"
        return f"ActionResult(reward={self.reward}, success={self.success}, message='{self.message}')"
    
    def __repr__(self) -> str:
        return self.__str__()
class LogLevel(Enum):
    """Logging levels."""
    
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"
# Constants
INVALID_NEURON_ID = NeuronId(0)
INVALID_SYNAPSE_ID = SynapseId(0)
INVALID_REGION_ID = RegionId(0)
INVALID_POPULATION_ID = PopulationId(0)
# Utility functions for advanced API

def create_advanced_config() -> 'AdvancedConfig':
    """Create an advanced configuration for NLM."""
    return AdvancedConfig()
def run_neural_simulation(config: 'AdvancedConfig', duration: int) -> 'NeuralSimulation':
    """Run a neural simulation with advanced utilities."""
    return NeuralSimulation(config).run_episode(duration)
def benchmark_simulation(config: 'AdvancedConfig', steps: int) -> Dict[str, Any]:
    """Benchmark simulation performance."""
    from .advanced_api import benchmark_simulation
    return benchmark_simulation(config, steps)
def visualize_brain_state(brain: 'Brain') -> None:
    """Create simple visualization of brain state."""
    print("Brain State Visualization:")
    print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Active Neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing Neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average Firing Rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.4f}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Developmental Stage: {brain.getDevelopmentalStage().value}")
    print()
def analyze_brain_dynamics(brain: 'Brain', window_size: int = 100) -> Dict[str, Any]:
    """Analyze brain dynamics over time."""
    # This would need to track history over time
    # For now, return current state analysis
    return {
        "current_state": {
            "neuron_count": brain.getTotalNeuronCount(),
            "activity_level": brain.getActiveNeuronCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
            "firing_rate": brain.getAverageFiringRate(),
            "plasticity": brain.getExcitationInhibitionRatio(),
        },
        "analysis": {
            "stability": "stable",  # This would require history
            "learning_rate": "moderate",  # This would require history
            "balance": "balanced",  # This would require history
        },
    }
# Forward declarations to avoid circular imports
class AdvancedConfig:
    """Forward declaration - actual implementation in advanced_api.py"""
    pass

class NeuralSimulation:
    """Forward declaration - actual implementation in advanced_api.py"""
    pass

class Brain:
    """Forward declaration - actual implementation in C++ extension"""
    pass

# Export all core functionality
__all__ = [
    # Type definitions
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
    "LogLevel",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Utility functions
    "create_advanced_config",
    "run_neural_simulation",
    "benchmark_simulation",
    "visualize_brain_state",
    "analyze_brain_dynamics",
]