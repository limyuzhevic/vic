#!/usr/bin/env python3

"""
NLM Types - Neural Network Type Definitions

This module provides type definitions for the NLM system:
- NeuronId: Unique identifier for neurons
- SynapseId: Unique identifier for synapses
- RegionId: Unique identifier for brain regions
- PopulationId: Unique identifier for neuron populations
- NeuronType: Enumeration of neuron types
- SynapseType: Enumeration of synapse types
- ActionType: Enumeration of action types
- MotorCommand: Enumeration of motor commands
- DevelopmentalStage: Enumeration of developmental stages
- FiringState: Enumeration of neuron firing states
"""

from typing import Dict, List, Optional, Any, Union
import time

class NLMError(Exception):
    """Base exception for NLM operations."""
    
    def __init__(self, message: str, error_code: str = "NLM_ERROR", details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = time.time()
    
    def __str__(self):
        return f"[{self.error_code}] {self.message}"

# Neuron and Synapse Identifiers
class NeuronId:
    """Unique identifier for a neuron."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    @property
    def index(self) -> int:
        """Get the index of the neuron."""
        return self.value
    
    def __eq__(self, other):
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<NeuronId: {self.value}>"

class SynapseId:
    """Unique identifier for a synapse."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    @property
    def index(self) -> int:
        """Get the index of the synapse."""
        return self.value
    
    def __eq__(self, other):
        if isinstance(other, SynapseId):
            return self.value == other.value
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<SynapseId: {self.value}>"

class RegionId:
    """Unique identifier for a brain region."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    @property
    def index(self) -> int:
        """Get the index of the region."""
        return self.value
    
    def __eq__(self, other):
        if isinstance(other, RegionId):
            return self.value == other.value
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<RegionId: {self.value}>"

class PopulationId:
    """Unique identifier for a neuron population."""
    
    def __init__(self, value: int = 0):
        self.value = value
    
    def __eq__(self, other):
        if isinstance(other, PopulationId):
            return self.value == other.value
        return False
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)

# Enumerations
class NeuronType:
    """Neuron type enumeration."""
    
    EXCITATORY = 1
    INHIBITORY = 2
    MODULATORY = 3
    SENSORY = 4
    MOTOR = 5
    INTERNAL = 6

class SynapseType:
    """Synapse type enumeration."""
    
    EXCITATORY = 1
    INHIBITORY = 2
    MODULATORY = 3
    ELECTRICAL = 4
    GAP_JUNCTION = 5

class DevelopmentalStage:
    """Developmental stage enumeration."""
    
    INITIAL = 1
    CRITICAL_PERIOD = 2
    MATURATION = 3
    ADULT = 4
    AGING = 5

class FiringState:
    """Neuron firing state enumeration."""
    
    RESTING = 1
    ACTIVE = 2
    REFRACTORY = 3
    INHIBITED = 4

class ActionType:
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
    """Low-level motor command enumeration."""
    
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class WorldObjectType:
    """World object type enumeration."""
    
    EMPTY = 1
    RESOURCE = 2
    HAZARD = 3

# Type aliases
NeuronIndex = NeuronId
SynapseIndex = SynapseId
RegionIndex = RegionId
PopulationIndex = PopulationId

# Utility functions for type handling
def create_neuron_id(value: int = 0) -> NeuronId:
    """Create a neuron ID."""
    return NeuronId(value)

def create_synapse_id(value: int = 0) -> SynapseId:
    """Create a synapse ID."""
    return SynapseId(value)

def create_region_id(value: int = 0) -> RegionId:
    """Create a region ID."""
    return RegionId(value)

def create_population_id(value: int = 0) -> PopulationId:
    """Create a population ID."""
    return PopulationId(value)

# Type validation functions
def validate_neuron_id(neuron_id: NeuronId) -> bool:
    """Validate a neuron ID."""
    return isinstance(neuron_id, NeuronId) and neuron_id.value >= 0

def validate_synapse_id(synapse_id: SynapseId) -> bool:
    """Validate a synapse ID."""
    return isinstance(synapse_id, SynapseId) and synapse_id.value >= 0

class NLMContextManager:
    """Context manager for NLM resources."""
    
    def __init__(self, auto_save: bool = True, auto_profiling: bool = False):
        self.auto_save = auto_save
        self.auto_profiling = auto_profiling
        self.session_id = id(self)
        self.start_time = None
        self.brain = None
        self.experiment = None
        self.monitor = None
    
    def __enter__(self):
        self.start_time = time.time()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        duration = time.time() - self.start_time if self.start_time else 0
        
        if exc_type:
            print(f"NLM Session {self.session_id} ended with error: {exc_val}")
        else:
            print(f"NLM Session {self.session_id} completed successfully in {duration:.2f}s")
        
        if self.auto_save and self.brain:
            self.save_session()
    
    def create_brain(self, config: 'Config') -> 'Brain':
        """Create a brain for this session."""
        from nlm.core.brain import Brain
        self.brain = Brain(config)
        self.brain.initialize()
        return self.brain
    
    def create_monitor(self) -> 'RealTimeMonitor':
        """Create a monitor for this session."""
        from nlm.monitoring.real_time import RealTimeMonitor
        self.monitor = RealTimeMonitor()
        self.monitor.start()
        return self.monitor
    
    def save_session(self, prefix: str = "session") -> tuple:
        """Save current session state."""
        timestamp = time.time()
        brain_file = f"{prefix}_{self.session_id}_brain.pkl"
        
        if self.brain:
            from nlm.core.brain import save_brain
            save_brain(self.brain, brain_file)
        
        return brain_file
    
    def load_session(self, brain_file: str) -> 'Brain':
        """Load session state."""
        from nlm.core.brain import load_brain
        if brain_file:
            self.brain = load_brain(brain_file)
        
        return self.brain

# Import types
import time
from nlm.core.brain import Brain, save_brain, load_brain
from nlm.core.config import Config

__all__ = [
    "NeuronId",
    "SynapseId", 
    "RegionId",
    "PopulationId",
    "NeuronType",
    "SynapseType",
    "ActionType",
    "MotorCommand",
    "DevelopmentalStage",
    "FiringState",
    "WorldObjectType",
    "NeuronIndex",
    "SynapseIndex",
    "RegionIndex",
    "PopulationIndex",
    "NLMError",
    "NLMContextManager",
    "create_neuron_id",
    "create_synapse_id",
    "create_region_id",
    "create_population_id",
    "validate_neuron_id",
    "validate_synapse_id",
    "__version__"
]