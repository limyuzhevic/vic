# Python NLM Module
# Enhanced Python interface for NLM neural simulation framework

"""
NLM (Neural Learning Machine) Python Interface
===============================================

This module provides a comprehensive Python interface for the NLM neural simulation
framework, enabling researchers and developers to create, simulate, and experiment
with artificial neural systems using Python.

Key Features:
- Real spiking neural computation with LIF neurons
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Sensory-motor integration
- Memory systems (working, episodic, associative)
- Developmental processes
- Neuromodulation (dopamine, curiosity, novelty)
- Action planning and decision making

Quick Start Example:
```python
import pynlm

# Create a default configuration
config = pynlm.Config()

# Create a neural brain
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i, i * 0.001)
    
# Check learning
print(f"Excitation/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.3f}")
```

Installation:
```bash
pip install pynlm
```
"""

import sys
import json
import logging
from typing import Dict, List, Any, Optional, Union, Tuple
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Global NLM exception
class NLMError(Exception):
    """Base exception for NLM-related errors."""
    pass

class NeuronId:
    """Unique identifier for a neuron with Pythonic interface."""
    
    def __init__(self, value: int = 0):
        """
        Create a neuron identifier.
        
        Args:
            value: The unique identifier value for the neuron.
            
        Examples:
            >>> NeuronId(123)
            <NeuronId: 123>
            >>> id = NeuronId()
            >>> id.value
            0
        """
        self._value = value
    
    @property
    def value(self) -> int:
        """The unique identifier value for this neuron (read-write)."""
        return self._value
    
    @value.setter
    def value(self, v: int) -> None:
        self._value = v
    
    def index(self) -> int:
        """
        Get the integer index representation of this neuron identifier.
        
        Returns:
            int: The integer index (same as value).
            
        Examples:
            >>> id = NeuronId(999)
            >>> id.index()
            999
        """
        return self._value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality with another neuron identifier."""
        if isinstance(other, NeuronId):
            return self._value == other._value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality with another neuron identifier."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Return hash of the neuron identifier for use in dictionaries and sets."""
        return hash(self._value)
    
    def __repr__(self) -> str:
        """Return string representation of the neuron identifier."""
        return f"<NeuronId: {self._value}>"
    
    def __str__(self) -> str:
        """Return string representation of the neuron identifier value."""
        return str(self._value)

class SynapseId:
    """Unique identifier for a synapse with Pythonic interface."""
    
    def __init__(self, value: int = 0):
        """
        Create a synapse identifier.
        
        Args:
            value: The unique identifier value for the synapse.
            
        Examples:
            >>> SynapseId(123)
            <SynapseId: 123>
            >>> id = SynapseId()
            >>> id.value
            0
        """
        self._value = value
    
    @property
    def value(self) -> int:
        """The unique identifier value for this synapse (read-write)."""
        return self._value
    
    @value.setter
    def value(self, v: int) -> None:
        self._value = v
    
    def index(self) -> int:
        """Get the integer index representation."""
        return self._value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality."""
        if isinstance(other, SynapseId):
            return self._value == other._value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Return hash for dictionaries/sets."""
        return hash(self._value)
    
    def __repr__(self) -> str:
        """Return string representation."""
        return f"<SynapseId: {self._value}>"
    
    def __str__(self) -> str:
        """Return string representation of value."""
        return str(self._value)

class RegionId:
    """Unique identifier for a brain region with Pythonic interface."""
    
    def __init__(self, value: int = 0):
        """
        Create a region identifier.
        
        Args:
            value: The unique identifier value for the region.
        """
        self._value = value
    
    @property
    def value(self) -> int:
        """The unique identifier value (read-write)."""
        return self._value
    
    @value.setter
    def value(self, v: int) -> None:
        self._value = v
    
    def index(self) -> int:
        """Get the integer index representation."""
        return self._value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality."""
        if isinstance(other, RegionId):
            return self._value == other._value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Return hash for dictionaries/sets."""
        return hash(self._value)
    
    def __repr__(self) -> str:
        """Return string representation."""
        return f"<RegionId: {self._value}>"
    
    def __str__(self) -> str:
        """Return string representation of value."""
        return str(self._value)

class PopulationId:
    """Unique identifier for a neuron population with Pythonic interface."""
    
    def __init__(self, value: int = 0):
        """
        Create a population identifier.
        
        Args:
            value: The unique identifier value for the population.
        """
        self._value = value
    
    @property
    def value(self) -> int:
        """The unique identifier value (read-write)."""
        return self._value
    
    @value.setter
    def value(self, v: int) -> None:
        self._value = v
    
    def index(self) -> int:
        """Get the integer index representation."""
        return self._value
    
    def __eq__(self, other: Any) -> bool:
        """Check equality."""
        if isinstance(other, PopulationId):
            return self._value == other._value
        return False
    
    def __ne__(self, other: Any) -> bool:
        """Check inequality."""
        return not self.__eq__(other)
    
    def __hash__(self) -> int:
        """Return hash for dictionaries/sets."""
        return hash(self._value)
    
    def __repr__(self) -> str:
        """Return string representation."""
        return f"<PopulationId: {self._value}>"
    
    def __str__(self) -> str:
        """Return string representation of value."""
        return str(self._value)

# Import native NLM bindings
try:
    import pynlm_native
    
    # Re-export native classes with Pythonic enhancements
    NeuronId = pynlm_native.NeuronId
    SynapseId = pynlm_native.SynapseId
    RegionId = pynlm_native.RegionId
    PopulationId = pynlm_native.PopulationId
    
    # Import enum types
    from pynlm_native import NeuronType, SynapseType, DevelopmentalStage, FiringState, ActionType, MotorCommand, WorldObjectType
    
    logger.info("NLM native bindings loaded successfully")
    
except ImportError as e:
    logger.warning(f"Native NLM bindings not available: {e}")
    
    # Define enum types locally if native bindings not available
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

# Configuration class with Pythonic interface
class Config:
    """Configuration system for neural simulation with Pythonic interface."""
    
    def __init__(self):
        """
        Create a new configuration object with default settings.
        
        The Config class provides a key-value storage system for neural
        simulation parameters. It supports multiple value types including
        integers, floats, strings, booleans, and arrays.
        """
        try:
            import pynlm_native
            self._native = pynlm_native.Config()
        except ImportError:
            # Fallback implementation
            self._native = None
            self._data = {}
    
    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file.
            
        Returns:
            bool: True if configuration was successfully loaded.
            
        Raises:
            NLMError: If the file cannot be read or parsed.
            
        Examples:
            >>> config = Config()
            >>> success = config.load_from_file("config.json")
            >>> if success:
            ...     print("Configuration loaded")
        """
        if self._native:
            return self._native.loadFromFile(filepath)
        else:
            try:
                with open(filepath, 'r') as f:
                    self._data = json.load(f)
                return True
            except Exception as e:
                raise NLMError(f"Failed to load config from {filepath}: {e}")
    
    def load_from_args(self, argc: int, argv: List[str]) -> bool:
        """
        Load configuration from command line arguments.
        
        Args:
            argc: Argument count (typically from sys.argv).
            argv: Argument values (typically from sys.argv).
            
        Returns:
            bool: True if configuration was successfully parsed.
            
        Examples:
            >>> import sys
            >>> config = Config()
            >>> config.load_from_args(len(sys.argv), sys.argv)
        """
        if self._native:
            return self._native.loadFromArgs(argc, argv)
        else:
            # Basic argument parsing for common options
            for i in range(1, len(argv)):
                if argv[i].startswith('--config='):
                    filepath = argv[i][9:]
                    self.load_from_file(filepath)
                elif argv[i] == '--config' and i + 1 < len(argv):
                    self.load_from_file(argv[i + 1])
            return True
    
    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to a JSON file.
        
        Args:
            filepath: Path to save the JSON configuration file.
            
        Returns:
            bool: True if configuration was successfully saved.
            
        Raises:
            NLMError: If the file cannot be written.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> success = config.save_to_file("config.json")
            >>> if success:
            ...     print("Configuration saved")
        """
        if self._native:
            return self._native.saveToFile(filepath)
        else:
            try:
                with open(filepath, 'w') as f:
                    json.dump(self._data, f, indent=2)
                return True
            except Exception as e:
                raise NLMError(f"Failed to save config to {filepath}: {e}")
    
    def get(self, key: str) -> Any:
        """
        Get a configuration value by key.
        
        Args:
            key: The configuration key to retrieve.
            
        Returns:
            The configuration value. The exact type depends on what was
            stored (int, float, string, bool, or list).
            
        Raises:
            NLMError: If the key does not exist in the configuration.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> count = config.get("neuron_count")
            >>> print(count)
            100
            
            >>> config.set("learning_rate", 0.01)
            >>> rate = config.get("learning_rate")
            >>> print(f"{rate:.3f}")
            0.010
        """
        if self._native:
            return self._native.get(key)
        else:
            if key not in self._data:
                raise NLMError(f"Key not found: {key}")
            return self._data[key]
    
    def get_or(self, key: str, default_value: Any) -> Any:
        """
        Get a configuration value with a default fallback.
        
        Args:
            key: The configuration key to retrieve.
            default_value: Value to return if key does not exist.
            
        Returns:
            The configuration value if key exists, otherwise default_value.
            
        Examples:
            >>> config = Config()
            >>> count = config.get_or("neuron_count", 100)
            >>> print(count)
            100
            
            >>> config.set("neuron_count", 200)
            >>> count = config.get_or("neuron_count", 100)
            >>> print(count)
            200
        """
        if self._native:
            return self._native.getOr(key, default_value)
        else:
            return self._data.get(key, default_value)
    
    def set(self, key: str, value: Any) -> None:
        """
        Set a configuration value.
        
        Args:
            key: The configuration key to set.
            value: The value to set (int, float, str, bool, or list).
            
        Raises:
            NLMError: If the value type is not supported.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.set("learning_rate", 0.01)
            >>> config.set("enabled", True)
            >>> config.set("tags", ["exc", "inh", "mod"])
        """
        if self._native:
            self._native.set(key, value)
        else:
            self._data[key] = value
    
    def has(self, key: str) -> bool:
        """
        Check if a configuration key exists.
        
        Args:
            key: The configuration key to check.
            
        Returns:
            bool: True if the key exists, False otherwise.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.has("neuron_count")
            True
            >>> config.has("nonexistent")
            False
        """
        if self._native:
            return self._native.has(key)
        else:
            return key in self._data
    
    def remove(self, key: str) -> None:
        """
        Remove a configuration key.
        
        Args:
            key: The configuration key to remove.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.remove("neuron_count")
            >>> config.has("neuron_count")
            False
        """
        if self._native:
            self._native.remove(key)
        else:
            self._data.pop(key, None)
    
    def get_keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            list: List of all configuration keys.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.set("learning_rate", 0.01)
            >>> sorted(config.get_keys())
            ['learning_rate', 'neuron_count']
        """
        if self._native:
            return self._native.getKeys()
        else:
            return list(self._data.keys())
    
    def clear(self) -> None:
        """
        Clear all configuration entries.
        
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.clear()
            >>> config.get_keys()
            []
        """
        if self._native:
            self._native.clear()
        else:
            self._data.clear()
    
    def summary(self) -> str:
        """
        Get a summary string of the configuration.
        
        Returns:
            str: Summary of all configuration entries.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.set("learning_rate", 0.01)
            >>> print(config.summary())
            Configuration with 2 entries:
             - neuron_count: 100 (int)
             - learning_rate: 0.010000 (float)
        """
        if self._native:
            return self._native.summary()
        else:
            lines = [f"Configuration with {len(self._data)} entries:"]
            for key, value in self._data.items():
                type_name = type(value).__name__
                lines.append(f"  - {key}: {value} ({type_name})")
            return "\n".join(lines)
    
    # Pythonic interface operators
    def __contains__(self, key: str) -> bool:
        """Check if a key exists (using 'in' operator)."""
        return self.has(key)
    
    def __getitem__(self, key: str) -> Any:
        """Get a configuration value (using [] operator)."""
        return self.get(key)
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Set a configuration value (using [] operator)."""
        self.set(key, value)
    
    def __len__(self) -> int:
        """Get the number of configuration keys."""
        return len(self.get_keys())
    
    def __iter__(self):
        """Iterate over configuration keys."""
        return iter(self.get_keys())
    
    def keys(self) -> List[str]:
        """
        Get all configuration keys (alias for get_keys()).
        
        Returns:
            list: List of all configuration keys.
        """
        return self.get_keys()
    
    def items(self) -> List[Tuple[str, Any]]:
        """
        Get all configuration items as (key, value) pairs.
        
        Returns:
            list: List of (key, value) tuples.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.set("learning_rate", 0.01)
            >>> config.items()
            [('neuron_count', 100), ('learning_rate', 0.010000)]
        """
        return [(key, self.get(key)) for key in self.get_keys()]
    
    def update(self, other: Dict[str, Any]) -> None:
        """
        Update configuration with key-value pairs from another dictionary.
        
        Args:
            other: Dictionary of key-value pairs to update.
            
        Examples:
            >>> config = Config()
            >>> config.update({"neuron_count": 100, "learning_rate": 0.01})
            >>> config["neuron_count"]
            100
        """
        for key, value in other.items():
            self.set(key, value)
    
    def validate(self, required_keys: List[str], type_hints: Optional[Dict[str, str]] = None) -> bool:
        """
        Validate configuration against required keys and type constraints.
        
        Args:
            required_keys: List of configuration keys that must be present.
            type_hints: Dictionary mapping keys to expected types ("int", "float", "str", "bool").
            
        Returns:
            bool: True if configuration is valid.
            
        Raises:
            NLMError: If validation fails.
            
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.set("learning_rate", 0.01)
            >>> config.validate(["neuron_count", "learning_rate"], {})
            True
            
            >>> config.validate(["neuron_count", "nonexistent"], {})
            NLMError: Configuration validation failed:
              Missing required keys: nonexistent
        """
        if type_hints is None:
            type_hints = {}
        
        missing_keys = []
        type_errors = []
        
        for key in required_keys:
            if not self.has(key):
                missing_keys.append(key)
        
        for key, expected_type in type_hints.items():
            if self.has(key):
                value = self.get(key)
                type_ok = False
                if expected_type == "int" and isinstance(value, int):
                    type_ok = True
                elif expected_type == "float" and isinstance(value, (float, int)):
                    type_ok = True
                elif expected_type == "str" and isinstance(value, str):
                    type_ok = True
                elif expected_type == "bool" and isinstance(value, bool):
                    type_ok = True
                
                if not type_ok:
                    type_errors.append(f"Key '{key}' should be {expected_type}, got {type(value).__name__}")
        
        if missing_keys or type_errors:
            error_msg = "Configuration validation failed:\n"
            if missing_keys:
                error_msg += "Missing required keys: " + " ".join(missing_keys) + "\n"
            if type_errors:
                error_msg += "Type errors:\n" + "\n".join(f"  - {e}" for e in type_errors)
            raise NLMError(error_msg)
        
        return True
    
    def __repr__(self) -> str:
        """Return string representation of the configuration."""
        return f"<Config: {self.summary()}>"

# Factory functions

def create_default_config() -> Config:
    """Create a default configuration object.
    
    Returns:
        Config: A new default configuration.
        
    Examples:
        >>> config = create_default_config()
        >>> config.set("neuron_count", 100)
    """
    return Config()


def create_brain(config: Optional[Config] = None) -> 'Brain':
    """Create a new brain with configuration.
    
    Args:
        config: Configuration to use. If None, a default config is created.
        
    Returns:
        Brain: A new brain object.
        
    Examples:
        >>> config = create_default_config()
        >>> brain = create_brain(config)
    """
    if config is None:
        config = create_default_config()
    
    try:
        import pynlm_native
        return pynlm_native.create_brain(config._native)
    except ImportError:
        # Fallback implementation
        return Brain(config)


def create_simple_world() -> 'SimpleWorld':
    """Create a new simple world for NLM simulation.
    
    Returns:
        SimpleWorld: A new simple world object.
        
    Examples:
        >>> world = create_simple_world()
        >>> world.configure(100, 100, 20, 20)
    """
    try:
        import pynlm_native
        return pynlm_native.create_simple_world()
    except ImportError:
        # Fallback implementation
        return SimpleWorld()


def create_agent_brain(brain: 'Brain') -> 'AgentBrain':
    """Create a new agent brain interface.
    
    Args:
        brain: The underlying brain object.
        
    Returns:
        AgentBrain: A new agent brain interface.
        
    Examples:
        >>> brain = create_brain()
        >>> agent_brain = create_agent_brain(brain)
    """
    try:
        import pynlm_native
        return pynlm_native.create_agent_brain(brain._native)
    except ImportError:
        # Fallback implementation
        return AgentBrain(brain)

# Global constants
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()

# Main class definitions (simplified fallbacks)
class Brain:
    """Central neural simulation brain class (fallback implementation)."""
    
    def __init__(self, config: Config):
        """Initialize brain with configuration."""
        self._config = config
        self._step_count = 0
    
    def initialize(self) -> bool:
        """Initialize the brain with configuration."""
        logger.info("Brain initialized with config")
        return True
    
    def step(self, current_step: int, current_time: float = 0.0) -> None:
        """Perform a simulation step."""
        self._step_count = current_step
        logger.info(f"Simulation step {current_step} at time {current_time:.3f}")
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        return 1.0
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return 0
    
    def reset(self) -> None:
        """Reset brain state."""
        self._step_count = 0
    
    def log_status(self) -> None:
        """Log brain status."""
        logger.info(f"Brain status: step={self._step_count}")

class SimpleWorld:
    """Simple 2D world for NLM simulation (fallback implementation)."""
    
    def __init__(self):
        """Create a new simple world."""
        self.width = 100
        self.height = 100
        self.vision_width = 20
        self.vision_height = 20
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """Configure world dimensions."""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
    
    def reset(self) -> None:
        """Reset world state."""
        pass
    
    def update(self, timestep: float) -> None:
        """Update world state."""
        pass

class AgentBrain:
    """Agent brain interface (fallback implementation)."""
    
    def __init__(self, brain: Brain):
        """Initialize agent brain with brain object."""
        self._brain = brain
    
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize with world."""
        pass
    
    def process_sensory_input(self, percept: Any) -> None:
        """Process sensory percept and inject into brain."""
        pass
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        pass

# Module exports
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'SimpleWorld',
    'AgentBrain',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Enum types
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Factory functions
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Exceptions
    'NLMError',
]
