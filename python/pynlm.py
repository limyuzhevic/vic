"""
NLM (Neural Learning Machine) - Python Bindings

A Python interface to the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

This module provides a high-level API for creating and simulating neural brains
with event-driven spiking dynamics, synaptic plasticity, and world interaction.

Examples:
    >>> import pynlm
    >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
    >>> brain.initialize()
    >>> for i in range(100):
    ...     brain.step(i)
    >>> print(f"Spikes: {brain.getTotalSpikeCount()}")
"""

from typing import Any, Dict, List, Optional, Union, overload
import logging
from enum import Enum

# Custom exception classes for better error handling
class NLMError(Exception):
    """Base exception for all NLM-related errors."""
    pass

class ConfigurationError(NLMError):
    """Raised when there's an error with configuration."""
    pass

class SimulationError(NLMError):
    """Raised when there's an error during simulation."""
    pass

class ValidationError(NLMError):
    """Raised when validation fails."""
    pass

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Enums
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    
    # ID types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Utility functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'createDefaultConfig',
    
    # Error classes
    'NLMError', 'ConfigurationError', 'SimulationError',
    'ValidationError',
    
    # Version info
    '__version__', '__author__',
    
    # Module utilities
    'configure_logging', 'get_version', 'validate_config',
    'list_available_configs'
]

# Setup logging
logging.basicConfig(level=logging.WARNING)
logger = logging.getLogger(__name__)

def configure_logging(level: str = 'WARNING', format_string: Optional[str] = None) -> None:
    """
    Configure logging for NLM operations.
    
    Args:
        level: Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
        format_string: Custom format string for log messages
    """
    numeric_level = getattr(logging, level.upper(), None)
    if not isinstance(numeric_level, int):
        raise ValidationError(f"Invalid logging level: {level}")
    
    if format_string is None:
        format_string = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    
    handler = logging.StreamHandler()
    handler.setFormatter(logging.Formatter(format_string))
    
    # Remove existing handlers
    logger.handlers.clear()
    logger.addHandler(handler)
    logger.setLevel(numeric_level)
    
    logger.debug(f"Logging configured at level: {level}")

def get_version() -> str:
    """Return the NLM Python bindings version."""
    return __version__

def validate_config(config_dict: Dict[str, Any]) -> bool:
    """
    Validate a configuration dictionary.
    
    Args:
        config_dict: Configuration dictionary to validate
        
    Returns:
        True if valid, raises ValidationError if invalid
        
    Raises:
        ValidationError: If configuration is invalid
    """
    if not isinstance(config_dict, dict):
        raise ValidationError("Configuration must be a dictionary")
    
    # Basic validation - more comprehensive validation would need
    # to check against the actual Config class schema
    allowed_keys = [
        'brain.neuron_count', 'brain.synapse_density', 'brain.connection_probability',
        'brain.initial_weight_mean', 'brain.initial_weight_std',
        'brain.v_thresh', 'brain.v_rest', 'brain.v_reset',
        'brain.tau_mem', 'brain.tau_ref',
        'plasticity.stdp.enable', 'plasticity.stdp.learning_rate',
        'plasticity.stdp.tau_plus', 'plasticity.stdp.tau_minus',
        'plasticity.hebbian.enable',
        'plasticity.structural.enable',
        'neuromod.dopamine.scale',
        'neuromod.curiosity.enable', 'neuromod.novelty.enable'
    ]
    
    for key in config_dict:
        if '.' not in key and key not in allowed_keys:
            # This might be acceptable as future extensions are allowed
            logger.warning(f"Unknown configuration key: {key}")
    
    return True

def list_available_configs() -> List[str]:
    """
    List all available configuration options.
    
    Returns:
        List of configuration option strings
    """
    return [
        'brain.neuron_count', 'brain.synapse_density', 'brain.connection_probability',
        'brain.initial_weight_mean', 'brain.initial_weight_std',
        'brain.v_thresh', 'brain.v_rest', 'brain.v_reset',
        'brain.tau_mem', 'brain.tau_ref',
        'plasticity.stdp.enable', 'plasticity.stdp.learning_rate',
        'plasticity.stdp.tau_plus', 'plasticity.stdp.tau_minus',
        'plasticity.hebbian.enable',
        'plasticity.structural.enable',
        'neuromod.dopamine.scale',
        'neuromod.curiosity.enable', 'neuromod.novelty.enable'
    ]

# Import the actual C++ classes
# Note: In a real implementation, these would be imported from the pybind11 module
# For this example, we'll define them with type hints

# Forward declarations for type hints
class Config:
    pass

class Brain:
    pass

class AgentBrain:
    pass

class SimpleWorld:
    pass

class SensoryInput:
    pass

class Vision(SensoryInput):
    pass

class Audio(SensoryInput):
    pass

class InternalSignals(SensoryInput):
    pass

class Action:
    pass

class WorldObject:
    pass

class AgentBody:
    pass

class ActionResult:
    pass

class SensoryPercept:
    pass

# Enum definitions for Python users
class NeuronType(Enum):
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    SENSORY = "Sensory"
    MOTOR = "Motor"
    INTERNAL = "Internal"

class SynapseType(Enum):
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    ELECTRICAL = "Electrical"
    GAPJUNCTION = "GapJunction"

class DevelopmentalStage(Enum):
    INITIAL = "Initial"
    CRITICALPERIOD = "CriticalPeriod"
    MATURATION = "Maturation"
    ADULT = "Adult"
    AGING = "Aging"

class FiringState(Enum):
    RESTING = "Resting"
    ACTIVE = "Active"
    REFRACTORY = "Refractory"
    INHIBITED = "Inhibited"

class ActionType(Enum):
    MOVEFORWARD = "MoveForward"
    MOVEBACKWARD = "MoveBackward"
    MOVELEFT = "MoveLeft"
    MOVERIGHT = "MoveRight"
    TURNLEFT = "TurnLeft"
    TURNRIGHT = "TurnRight"
    LOOK = "Look"
    LOOKUP = "LookUp"
    LOOKDOWN = "LookDown"
    INTERACT = "Interact"
    EAT = "Eat"
    DRINK = "Drink"
    REST = "Rest"
    WAIT = "Wait"
    CUSTOM = "Custom"

class MotorCommand(Enum):
    MOVEFORWARD = "MoveForward"
    MOVEBACKWARD = "MoveBackward"
    TURNLEFT = "TurnLeft"
    TURNRIGHT = "TurnRight"
    LOOKLEFT = "LookLeft"
    LOOKRIGHT = "LookRight"
    INTERACT = "Interact"
    WAIT = "Wait"

class WorldObjectType(Enum):
    EMPTY = "Empty"
    RESOURCE = "Resource"
    HAZARD = "Hazard"
    WALL = "Wall"
    MARKER = "Marker"

# ID type definitions
class NeuronId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<NeuronId: {self.value}>"

class SynapseId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<SynapseId: {self.value}>"

class RegionId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, RegionId) and self.value == other.value
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<RegionId: {self.value}>"

class PopulationId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, PopulationId) and self.value == other.value
    
    def __ne__(self, other: Any) -> bool:
        return not self.__eq__(other)

# Invalid ID constants
INVALID_NEURON_ID = NeuronId(-1)
INVALID_SYNAPSE_ID = SynapseId(-1)
INVALID_REGION_ID = RegionId(-1)
INVALID_POPULATION_ID = PopulationId(-1)

# Module-level functions that wrap the C++ functions
def createDefaultConfig() -> Config:
    """Create a default configuration."""
    # This would be implemented by importing the actual C++ class
    logger.debug("Creating default configuration")
    return Config()

def createBrain(config: Config) -> Brain:
    """Create a new brain with the given configuration."""
    logger.debug("Creating brain with configuration")
    return Brain()

def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world."""
    logger.debug("Creating simple world")
    return SimpleWorld()

def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface."""
    logger.debug("Creating agent brain interface")
    return AgentBrain()

# Set up invalid IDs as module attributes
INVALID_NEURON_ID = INVALID_NEURON_ID
INVALID_SYNAPSE_ID = INVALID_SYNAPSE_ID
INVALID_REGION_ID = INVALID_REGION_ID
INVALID_POPULATION_ID = INVALID_POPULATION_ID