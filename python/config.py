"""
NLM Config class with enhanced Pythonic interface.

This module provides the Config class for managing NLM configuration
parameters. The class provides both low-level C++ bindings and a
Pythonic interface for easier use.
"""

import json
from typing import Any, Dict, List, Optional, Union

from .brain import Brain

# Import C++ bindings
from .bindings import (
    Config as _Config,
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
class EnhancedConfig:
    """Enhanced Config class with better Pythonic interface.
    
    This class provides a Pythonic interface to the NLM configuration system,
    making it easier to work with configuration parameters through both
    dictionary-like access and direct key-value operations.
    
    The enhanced config provides:
    - Dictionary-like access to configuration parameters
    - Type-safe parameter setting with validation
    - File I/O operations for configuration
    - Configuration summaries and debugging
    - Support for both string and typed access
    
    Example:
        >>> config = EnhancedConfig()
        >>> config.set("brain.neuron_count", 1000)
        >>> config.set("brain.synapse_density", 0.15)
        >>> brain = createBrain(config)
        >>> brain.initialize()
    """
    
    def __init__(self, config: Optional[Any] = None):
        """Initialize with optional existing config.
        
        Args:
            config: Existing Config object to wrap. If None, a new config
                   is created.
                   
        Raises:
            TypeError: If config is not a valid Config object.
        """
        if config is None:
            self._config = _Config()
        else:
            if hasattr(config, '_config'):
                self._config = config._config
            else:
                self._config = config
    
    def set(self, key: str, value: Any, source: str = "runtime") -> None:
        """Set a configuration value.
        
        Args:
            key: Configuration key (e.g., "brain.neuron_count")
            value: Value to set
            source: Source of configuration (default: "runtime")
            
        Raises:
            ConfigurationError: If the key is invalid or value is incompatible.
            TypeError: If key is not a string.
        """
        if not isinstance(key, str):
            raise TypeError(f"Configuration key must be a string, got {type(key)}")
        
        try:
            self._config.set(key, value, source)
        except Exception as e:
            raise ConfigurationError(f"Failed to set configuration key '{key}': {e}")
    
    def get(self, key: str) -> Optional[Any]:
        """Get a configuration value.
        
        Args:
            key: Configuration key
            
        Returns:
            Value or None if key doesn't exist.
            
        Raises:
            TypeError: If key is not a string.
        """
        if not isinstance(key, str):
            raise TypeError(f"Configuration key must be a string, got {type(key)}")
        
        return self._config.get(key)
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise.
            
        Raises:
            TypeError: If key is not a string.
        """
        if not isinstance(key, str):
            raise TypeError(f"Configuration key must be a string, got {type(key)}")
        
        return self._config.has(key)
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of configuration keys.
        """
        return self._config.get_keys()
    
    def get_or(self, key: str, default: Any = None) -> Any:
        """Get configuration value with default.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Value or default.
        """
        return self._config.get_or(key, default)
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self._config.clear()
    
    def save_to_file(self, filepath: str) -> None:
        """Save configuration to JSON file.
        
        Args:
            filepath: Path to save file
            
        Raises:
            SerializationError: If file I/O fails.
        """
        try:
            self._config.saveToFile(filepath)
        except Exception as e:
            raise SerializationError(f"Failed to save configuration to '{filepath}': {e}")
    
    def load_from_file(self, filepath: str) -> None:
        """Load configuration from JSON file.
        
        Args:
            filepath: Path to load file
            
        Raises:
            SerializationError: If file I/O fails.
        """
        try:
            self._config.loadFromFile(filepath)
        except Exception as e:
            raise SerializationError(f"Failed to load configuration from '{filepath}': {e}")
    
    def summary(self) -> str:
        """Get configuration summary.
        
        Returns:
            Configuration summary string.
        """
        return self._config.summary()
    
    def __repr__(self) -> str:
        return f"EnhancedConfig({self.summary()})"
    
    def __str__(self) -> str:
        return self.summary()
    
    def validate(self) -> List[str]:
        """Validate configuration.
        
        Returns:
            List of error messages, empty if configuration is valid.
        """
        errors = []
        
        # Basic validation
        if not self.has("brain.neuron_count"):
            errors.append("Missing 'brain.neuron_count' configuration")
        
        if not self.has("brain.synapse_density"):
            errors.append("Missing 'brain.synapse_density' configuration")
        
        # Check value ranges
        neuron_count = self.get("brain.neuron_count")
        if neuron_count is not None:
            if not isinstance(neuron_count, (int, float)) or neuron_count <= 0:
                errors.append("brain.neuron_count must be a positive number")
            elif neuron_count > 1000000:
                errors.append("brain.neuron_count should not exceed 1,000,000")
        
        synapse_density = self.get("brain.synapse_density")
        if synapse_density is not None:
            if not isinstance(synapse_density, (int, float)) or synapse_density < 0 or synapse_density > 1:
                errors.append("brain.synapse_density must be between 0 and 1")
        
        return errors
class Config(EnhancedConfig):
    """Config class for NLM simulation configuration.
    
    This is the main configuration class for the NLM system, providing
    access to all configuration parameters needed for brain simulation.
    
    Configuration parameters include:
    - Brain parameters (neuron count, synapse density, etc.)
    - Sensory system parameters (vision, audio, touch)
    - Motor system parameters (action types, commands)
    - World simulation parameters (size, energy decay, etc.)
    - Development parameters (plasticity, stages)
    - Neuromodulation parameters (curiosity, novelty, reward)
    
    The class can be loaded from JSON files or command line arguments,
    and can be saved to files for later use.
    
    Example:
        >>> # Create default config
        >>> config = Config()
        >>> 
        >>> # Load from file
        >>> config.load_from_file("config.json")
        >>> 
        >>> # Modify configuration
        >>> config.set("brain.neuron_count", 1000)
        >>> config.set("world.width", 20)
        >>> config.set("world.height", 20)
        >>> 
        >>> # Save configuration
        >>> config.save_to_file("my_config.json")
        >>> 
        >>> # Validate configuration
        >>> errors = config.validate()
        >>> if errors:
        ...     print(f"Configuration errors: {errors}")
    """
    
    def __init__(self):
        """Initialize a new configuration with default values."""
        super().__init__()
    
    def loadFromFile(self, filepath: str) -> None:
        """Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file.
        """
        self.load_from_file(filepath)
    
    def saveToFile(self, filepath: str) -> None:
        """Save configuration to a JSON file.
        
        Args:
            filepath: Path to save the JSON configuration file.
        """
        self.save_to_file(filepath)
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists.
        
        Args:
            key: Configuration key to check.
            
        Returns:
            True if the key exists, False otherwise.
        """
        return super().has(key)
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of all configuration keys.
        """
        return self.get_keys()
    
    def __repr__(self) -> str:
        return f"Config({self.summary()})"


# Type aliases for better documentation
Number = Union[int, float]
String = str
Bool = bool
ListAny = List[Any]
DictAny = Dict[str, Any]

__all__ = ["Config", "EnhancedConfig", "Number", "String", "Bool", "ListAny", "DictAny"]
