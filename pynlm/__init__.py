"""
NLM Core Configuration
"""

import json
import os
from typing import Any, Dict, List, Optional, Union

__version__ = "0.1.0"
class ConfigError(Exception):
    """Raised when configuration operations fail."""
    pass
class Config:
    """Configuration class for NLM system.
    
    Manages configuration values with support for different data types and sources.
    Provides file I/O, command-line argument parsing, and runtime configuration.
    """
    
    def __init__(self):
        """Create a new configuration instance."""
        self._values = {}
        self._sources = {}
        self._descriptions = {}
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file
            
        Returns:
            True if configuration was loaded successfully, False otherwise
        """
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            for key, value in data.items():
                self._set_value(key, value, "File")
            
            return True
        except (json.JSONDecodeError, IOError, OSError) as e:
            raise ConfigError(f"Failed to load configuration from {filepath}: {e}")
        except Exception as e:
            raise ConfigError(f"Unexpected error loading configuration: {e}")
    
    def load_from_args(self, argv: List[str]) -> bool:
        """Load configuration from command line arguments.
        
        Supports both --key=value and -k value formats.
        
        Args:
            argv: List of command line arguments (typically sys.argv)
            
        Returns:
            True if parsing completed (not necessarily success)
        """
        if len(argv) < 1:
            return True
            
        try:
            i = 1
            while i < len(argv):
                arg = argv[i]
                
                # Handle --key=value format
                if arg.startswith("--") and "=" in arg:
                    key = arg[2:arg.index("=")]
                    value = arg[arg.index("=") + 1:]
                    self._set_value(key, value, "CommandLine")
                    i += 1
                # Handle -key value format
                elif arg.startswith("-") and i + 1 < len(argv):
                    key = arg[1:]
                    value = argv[i + 1]
                    self._set_value(key, value, "CommandLine")
                    i += 2
                else:
                    i += 1
            
            return True
        except Exception as e:
            raise ConfigError(f"Error parsing command line arguments: {e}")
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to a JSON file.
        
        Args:
            filepath: Path where to save the configuration
            
        Returns:
            True if configuration was saved successfully, False otherwise
        """
        try:
            with open(filepath, 'w') as f:
                # Convert to JSON-serializable format
                json_data = {}
                for key, value in self._values.items():
                    json_data[key] = value
                
                json.dump(json_data, f, indent=2)
            
            return True
        except (IOError, OSError) as e:
            raise ConfigError(f"Failed to save configuration to {filepath}: {e}")
        except Exception as e:
            raise ConfigError(f"Unexpected error saving configuration: {e}")
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists.
        
        Args:
            key: Configuration key to check
            
        Returns:
            True if the key exists, False otherwise
        """
        return key in self._values
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of all configuration keys
        """
        return list(self._values.keys())
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self._values.clear()
        self._sources.clear()
        self._descriptions.clear()
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value by key.
        
        Args:
            key: Configuration key to retrieve
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if not found
        """
        return self._values.get(key, default)
    
    def get_or(self, key: str, default: Any) -> Any:
        """Get a configuration value or default if not found.
        
        Args:
            key: Configuration key to retrieve
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if not found
        """
        return self._values.get(key, default)
    
    def set(self, key: str, value: Any, source: str = "Runtime") -> None:
        """Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            source: Source of the configuration (Runtime, File, CommandLine)
        """
        self._set_value(key, value, source)
    
    def remove(self, key: str) -> None:
        """Remove a configuration key.
        
        Args:
            key: Configuration key to remove
        """
        if key in self._values:
            del self._values[key]
            if key in self._sources:
                del self._sources[key]
    
    def summary(self) -> str:
        """Get a summary string of the configuration.
        
        Returns:
            Formatted configuration summary
        """
        if not self._values:
            return "Configuration (0 entries): Empty"
        
        lines = [f"Configuration ({len(self._values)} entries):"]
        for key, value in self._values.items():
            source = self._sources.get(key, "Unknown")
            lines.append(f"  {key} = {value} ({source})")
        
        return "\n".join(lines)
    
    def __repr__(self) -> str:
        return f"<Config: {self.summary()}>"
    
    def __contains__(self, key: str) -> bool:
        return self.has(key)
    
    def _set_value(self, key: str, value: Any, source: str) -> None:
        """Internal method to set configuration value with validation."""
        if not isinstance(key, str):
            raise ConfigError(f"Configuration key must be string, got {type(key)}")
        
        # Handle different value types
        if isinstance(value, (int, float, str, bool)):
            self._values[key] = value
        elif isinstance(value, list):
            # Convert lists to JSON-compatible format
            self._values[key] = value
        else:
            # Try to convert to string for other types
            try:
                str_value = str(value)
                self._values[key] = str_value
            except Exception:
                raise ConfigError(f"Cannot convert value to string for key '{key}': {value}")
        
        self._sources[key] = source
def createDefaultConfig() -> Config:
    """Create a default configuration for NLM.
    
    Returns:
        Default configuration instance
    """
    return Config()
def createBrain(config: Config) -> 'Brain':
    """Create a brain with configuration.
    
    Args:
        config: Configuration for the brain
        
    Returns:
        Brain instance
    """
    return Brain(config)
def createSimpleWorld() -> 'SimpleWorld':
    """Create a simple world for NLM simulation.
    
    Returns:
        SimpleWorld instance
    """
    return SimpleWorld()
def createAgentBrain(brain: 'Brain') -> 'AgentBrain':
    """Create an agent brain interface.
    
    Args:
        brain: Underlying brain instance
        
    Returns:
        AgentBrain instance
    """
    return AgentBrain(brain)
# Enum classes
class NeuronType:
    """Neuron type enumeration."""
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    SENSORY = "Sensory"
    MOTOR = "Motor"
    INTERNAL = "Internal"
class SynapseType:
    """Synapse type enumeration."""
    EXCITATORY = "Excitatory"
    INHIBITORY = "Inhibitory"
    MODULATORY = "Modulatory"
    ELECTRICAL = "Electrical"
    GAPJUNCTION = "GapJunction"
class DevelopmentalStage:
    """Developmental stage enumeration."""
    INITIAL = "Initial"
    CRITICALPERIOD = "CriticalPeriod"
    MATURATION = "Maturation"
    ADULT = "Adult"
    AGING = "Aging"
class ActionType:
    """Action type enumeration."""
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
class MotorCommand:
    """Low-level motor command enumeration."""
    MOVEFORWARD = "MoveForward"
    MOVEBACKWARD = "MoveBackward"
    TURNLEFT = "TurnLeft"
    TURNRIGHT = "TurnRight"
    LOOKLEFT = "LookLeft"
    LOOKRIGHT = "LookRight"
    INTERACT = "Interact"
    WAIT = "Wait"
class WorldObjectType:
    """World object type enumeration."""
    EMPTY = "Empty"
    RESOURCE = "Resource"
    HAZARD = "Hazard"
    WALL = "Wall"
    MARKER = "Marker"
# Global constants
INVALID_NEURON_ID = -1
INVALID_SYNAPSE_ID = -1
INVALID_REGION_ID = -1
INVALID_POPULATION_ID = -1