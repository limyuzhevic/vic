"""
Type definitions and hints for NLM Python API.

This module provides comprehensive type definitions and type hints
for the NLM Python API, improving IDE support and code documentation.
"""

from typing import (
    Dict, List, Optional, Tuple, Union, Any, Callable, 
    Protocol, TypeVar, Generic
)
import numpy as np

# Core NLM types
T = TypeVar('T')

class ConfigValueUnion:
    """Union type for NLM configuration values."""
    UnionType = Union[int, float, str, bool, List[int], List[float], List[str]]

class SensorData:
    """Type definition for sensor data."""
    SensorData = Dict[str, Union[List[float], np.ndarray]]

class BrainStats:
    """Type definition for brain statistics."""
    BrainStats = Dict[str, Union[int, float, str, Dict[str, Any]]]

class ActionParams:
    """Type definition for action parameters."""
    ActionParams = Dict[str, Union[float, int, str]]

class WorldObjectDict:
    """Type definition for world object dictionary."""
    WorldObjectDict = Dict[str, Union[float, int, str, bool]]

# Type aliases for common patterns
ConfigKey = str
ConfigValue = Union[int, float, str, bool, List[int], List[float], List[str]]
ConfigDict = Dict[ConfigKey, ConfigValue]

BrainID = int
NeuronID = int
SynapseID = int
RegionID = int
PopulationID = int

SimulationStep = int
Timestamp = float
MembranePotential = float

NeuronType = str
SynapseType = str
DevelopmentalStage = str
FiringState = str
ActionType = str
MotorCommand = str
WorldObjectType = str

# Function type definitions
SensoryInputFunc = Callable[[], SensorData]
ActionFunc = Callable[[], ActionParams]
StepFunc = Callable[[SimulationStep], None]
UpdateFunc = Callable[[Timestamp], None]

# Class protocol definitions
class HasBrain(Protocol):
    """Protocol for objects that have a brain."""
    
    def getBrain(self) -> Any: ...
    def get_total_neurons(self) -> int: ...
    def get_total_spikes(self) -> int: ...
    def get_average_firing_rate(self) -> float: ...

class HasWorld(Protocol):
    """Protocol for objects that have a world."""
    
    def get_width(self) -> float: ...
    def get_height(self) -> float: ...
    def get_agent_position(self) -> Tuple[float, float]: ...
    def update(self, timestep: float) -> None: ...

class HasMemory(Protocol):
    """Protocol for objects that have memory systems."""
    
    def get_working_memory(self) -> Any: ...
    def get_episodic_memory(self) -> Any: ...
    def get_associative_memory(self) -> Any: ...

class HasNeuromodulation(Protocol):
    """Protocol for objects that have neuromodulation systems."""
    
    def get_dopamine(self) -> Any: ...
    def get_curiosity(self) -> Any: ...
    def get_novelty(self) -> Any: ...
    def get_prediction_error_signal(self) -> Any: ...

# Configuration type definitions
class ConfigBuilder(Protocol):
    """Protocol for configuration builders."""
    
    def set(self, key: str, value: ConfigValue) -> 'ConfigBuilder': ...
    def get_int(self, key: str, default: int = 0) -> int: ...
    def get_float(self, key: str, default: float = 0.0) -> float: ...
    def get_string(self, key: str, default: str = "") -> str: ...
    def get_bool(self, key: str, default: bool = False) -> bool: ...
    def save_to_file(self, filepath: str) -> bool: ...
    def load_from_file(self, filepath: str) -> bool: ...

# Simulation type definitions
class SimulationResult:
    """Type definition for simulation results."""
    
    def __init__(self,
                 steps: int,
                 duration: float,
                 final_neurons: int,
                 final_spikes: int,
                 average_firing_rate: float,
                 stats: Optional[Dict[str, Any]] = None):
        self.steps = steps
        self.duration = duration
        self.final_neurons = final_neurons
        self.final_spikes = final_spikes
        self.average_firing_rate = average_firing_rate
        self.stats = stats or {}
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'steps': self.steps,
            'duration': self.duration,
            'final_neurons': self.final_neurons,
            'final_spikes': self.final_spikes,
            'average_firing_rate': self.average_firing_rate,
            'stats': self.stats
        }

# Agent type definitions
class AgentCapabilities:
    """Type definition for agent capabilities."""
    
    def __init__(self):
        self.reward_modulation = False
        self.structural_plasticity = False
        self.development = False
        self.curiosity = False
        self.social_learning = False
        self.text_encoding = False
    
    def enable(self, **kwargs) -> 'AgentCapabilities':
        """Enable agent capabilities."""
        for key, value in kwargs.items():
            if hasattr(self, key):
                setattr(self, key, value)
        return self
    
    def is_enabled(self, capability: str) -> bool:
        """Check if capability is enabled."""
        return getattr(self, capability, False)

# World configuration type definitions
class WorldConfig:
    """Type definition for world configuration."""
    
    def __init__(self,
                 width: float = 20.0,
                 height: float = 20.0,
                 vision_width: int = 8,
                 vision_height: int = 8,
                 max_energy: float = 100.0,
                 energy_decay_rate: float = 0.5):
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self.max_energy = max_energy
        self.energy_decay_rate = energy_decay_rate
    
    def validate(self) -> bool:
        """Validate world configuration."""
        return (self.width > 0 and self.height > 0 and
                self.vision_width > 0 and self.vision_height > 0 and
                self.width >= self.vision_width and self.height >= self.vision_height)

# Type checking utilities
class TypeChecker:
    """Utility class for type checking."""
    
    @staticmethod
    def is_numeric(value: Any) -> bool:
        """Check if value is numeric."""
        return isinstance(value, (int, float))
    
    @staticmethod
    def is_string(value: Any) -> bool:
        """Check if value is string."""
        return isinstance(value, str)
    
    @staticmethod
    def is_boolean(value: Any) -> bool:
        """Check if value is boolean."""
        return isinstance(value, bool)
    
    @staticmethod
    def is_list(value: Any) -> bool:
        """Check if value is list."""
        return isinstance(value, list)
    
    @staticmethod
    def is_dict(value: Any) -> bool:
        """Check if value is dictionary."""
        return isinstance(value, dict)
    
    @staticmethod
    def validate_config_value(key: str, value: Any) -> bool:
        """
        Validate configuration value based on key.
        
        Args:
            key: Configuration key
            value: Value to validate
        
        Returns:
            True if valid, False otherwise
        """
        # Common validation rules
        if 'neuron' in key.lower() and 'count' in key.lower():
            return isinstance(value, (int, float)) and value > 0
        
        if 'rate' in key.lower():
            return isinstance(value, (int, float)) and 0 <= value <= 1
        
        if 'probability' in key.lower():
            return isinstance(value, (int, float)) and 0 <= value <= 1
        
        if 'seed' in key.lower():
            return isinstance(value, (int, float)) and value >= 0
        
        return True
    
    @staticmethod
    def get_type_name(value: Any) -> str:
        """Get human-readable type name."""
        if isinstance(value, int):
            return "integer"
        elif isinstance(value, float):
            return "float"
        elif isinstance(value, str):
            return "string"
        elif isinstance(value, bool):
            return "boolean"
        elif isinstance(value, list):
            return "list"
        elif isinstance(value, dict):
            return "dictionary"
        else:
            return type(value).__name__

# Documentation strings
def get_type_documentation() -> str:
    """Get type documentation string."""
    return """
    # NLM Type Definitions
    
    Core Types:
    - Config: Configuration wrapper for NLM settings
    - Brain: Neural simulation brain with learning capabilities
    - World: Environment simulation with sensory processing
    - Agent: Interface connecting brain to world
    
    Data Types:
    - ConfigValue: Union[int, float, str, bool, List[int], List[float], List[str]]
    - SensorData: Dict[str, Union[List[float], np.ndarray]]
    - ActionParams: Dict[str, Union[float, int, str]]
    
    Configuration Keys:
    - brain.neuron_count: Number of neurons (int)
    - brain.region_count: Number of brain regions (int)
    - brain.connection_probability: Connection probability (float)
    - brain.random_seed: Random seed (int)
    - simulation.learning_rate: Learning rate (float)
    - agent.enable_curiosity: Curiosity enabled (bool)
    
    Usage Examples:
    >>> config = create_nlm_config({"brain.neuron_count": 1000})
    >>> brain = create_nlm_brain(config)
    >>> world = create_nlm_world()
    """

# Module exports
__all__ = [
    'ConfigValueUnion',
    'SensorData',
    'BrainStats',
    'ActionParams',
    'WorldObjectDict',
    'ConfigKey',
    'ConfigValue',
    'ConfigDict',
    'BrainID',
    'NeuronID',
    'SynapseID',
    'RegionID',
    'PopulationID',
    'SimulationStep',
    'Timestamp',
    'MembranePotential',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'SensoryInputFunc',
    'ActionFunc',
    'StepFunc',
    'UpdateFunc',
    'HasBrain',
    'HasWorld',
    'HasMemory',
    'HasNeuromodulation',
    'ConfigBuilder',
    'SimulationResult',
    'AgentCapabilities',
    'WorldConfig',
    'TypeChecker',
    'get_type_documentation'
]