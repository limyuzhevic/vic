# Configuration management for NLM Python API

"""
Configuration class for NLM brain system

Provides a Pythonic interface to the NLM configuration system,
with type hints, better error handling, and convenient methods.
"""

import json
from typing import Dict, List, Any, Optional, Union, Iterator
from pathlib import Path

# Import the C++ binding
from pynlm import Config as CppConfig

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class ConfigValidationError(NLMError):
    """Raised when configuration validation fails"""
    pass

class ConfigLoadError(NLMError):
    """Raised when configuration loading fails"""
    pass

class Config:
    """
    NLM Configuration Manager
    
    Provides a Pythonic interface to manage NLM brain configuration.
    Supports loading from files, command line, and programmatic access.
    
    Examples:
        >>> config = Config()
        >>> config.set("brain.regionCount", 5)
        >>> config.set("brain.plasticityHebbianRate", 0.01)
        >>> config.save("my_brain.json")
    """
    
    def __init__(self, config_path: Optional[str] = None):
        """
        Initialize configuration.
        
        Args:
            config_path: Optional path to configuration file to load
        """
        self._cpp_config = CppConfig()
        
        if config_path:
            self.load_from_file(config_path)
    
    def has(self, key: str) -> bool:
        """
        Check if configuration key exists.
        
        Args:
            key: Configuration key to check
            
        Returns:
            True if key exists, False otherwise
            
        Examples:
            >>> config = Config()
            >>> if config.has("brain.regionCount"):
            ...     print("Region count configured")
        """
        return self._cpp_config.has(key)
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value with optional default.
        
        Args:
            key: Configuration key to retrieve
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default
            
        Raises:
            KeyError: If key doesn't exist and no default provided
            
        Examples:
            >>> config = Config()
            >>> neurons = config.get("brain.regionCount", 1)
            >>> rate = config["brain.plasticityHebbianRate"]
        """
        if not self.has(key):
            if default is not None:
                return default
            raise KeyError(f"Configuration key '{key}' not found")
        
        # Try to get as appropriate type based on key prefix
        if key.startswith("brain.regionCount"):
            return self._cpp_config.getInt(key)
        elif key.startswith("brain.plasticity"):
            return self._cpp_config.getFloat(key)
        elif key.startswith("brain.development"):
            return self._cpp_config.getBool(key)
        elif key.startswith("world."):
            return self._cpp_config.getInt(key)
        else:
            return self._cpp_config.getString(key)
    
    def __getitem__(self, key: str) -> Any:
        """
        Dictionary-style access to configuration.
        
        Args:
            key: Configuration key
            
        Returns:
            Configuration value
            
        Raises:
            KeyError: If key doesn't exist
            
        Examples:
            >>> config = Config()
            >>> print(config["brain.regionCount"])
        """
        return self.get(key)
    
    def __setitem__(self, key: str, value: Any) -> None:
        """
        Dictionary-style setting of configuration.
        
        Args:
            key: Configuration key
            value: Configuration value
            
        Examples:
            >>> config = Config()
            >>> config["brain.regionCount"] = 5
            >>> config["brain.plasticityHebbianRate"] = 0.01
        """
        self.set(key, value)
    
    def set(self, key: str, value: Any) -> None:
        """
        Set configuration value with automatic type conversion.
        
        Args:
            key: Configuration key
            value: Configuration value (int, float, bool, str, etc.)
            
        Examples:
            >>> config = Config()
            >>> config.set("brain.regionCount", 5)
            >>> config.set("brain.plasticityHebbianRate", 0.01)
            >>> config.set("world.width", 100)
        """
        # Auto-detect type from Python type
        if isinstance(value, bool):
            self._cpp_config.setBool(key, value)
        elif isinstance(value, int):
            self._cpp_config.setInt(key, value)
        elif isinstance(value, float):
            self._cpp_config.setFloat(key, value)
        elif isinstance(value, str):
            self._cpp_config.setString(key, value)
        else:
            # Try to handle complex types
            try:
                if hasattr(value, 'to_dict'):
                    self._cpp_config.setDict(key, value.to_dict())
                elif isinstance(value, (list, tuple)):
                    self._cpp_config.setList(key, list(value))
                else:
                    self._cpp_config.setString(key, str(value))
            except Exception as e:
                raise ConfigValidationError(f"Failed to set configuration '{key}' = '{value}': {e}")
    
    def get_keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of all configuration keys
            
        Examples:
            >>> config = Config()
            >>> keys = config.get_keys()
            >>> for key in keys:
            ...     print(f"{key}: {config[key]}")
        """
        return self._cpp_config.getKeys()
    
    def clear(self) -> None:
        """
        Clear all configuration entries.
        
        Examples:
            >>> config = Config()
            >>> config.clear()  # Start with fresh config
        """
        self._cpp_config.clear()
    
    def load_from_file(self, filepath: str) -> None:
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON configuration file
            
        Raises:
            ConfigLoadError: If file cannot be loaded or is invalid
            
        Examples:
            >>> config = Config()
            >>> config.load_from_file("my_brain.json")
        """
        try:
            self._cpp_config.loadFromFile(filepath)
        except Exception as e:
            raise ConfigLoadError(f"Failed to load config from '{filepath}': {e}")
    
    def save_to_file(self, filepath: str) -> None:
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to save configuration file
            
        Raises:
            ConfigLoadError: If file cannot be saved
            
        Examples:
            >>> config = Config()
            >>> config.save_to_file("my_brain.json")
        """
        try:
            self._cpp_config.saveToFile(filepath)
        except Exception as e:
            raise ConfigLoadError(f"Failed to save config to '{filepath}': {e}")
    
    def load_from_args(self, args: Optional[List[str]] = None) -> bool:
        """
        Load configuration from command line arguments.
        
        Args:
            args: List of arguments (defaults to sys.argv)
            
        Returns:
            True if configuration was loaded successfully, False otherwise
            
        Examples:
            >>> config = Config()
            >>> config.load_from_args(["nlm", "--brain.regionCount", "10"])
        """
        import sys
        if args is None:
            args = sys.argv
        
        return self._cpp_config.loadFromArgs(len(args), args)
    
    def summary(self) -> str:
        """
        Get configuration summary as string.
        
        Returns:
            Configuration summary string
            
        Examples:
            >>> config = Config()
            >>> print(config.summary())
        """
        return self._cpp_config.summary()
    
    def __repr__(self) -> str:
        """String representation of configuration"""
        return f"<Config with {len(self.get_keys())} keys>"
    
    def __str__(self) -> str:
        """String representation of configuration"""
        return self.summary()

# Factory functions
def create_default_config() -> Config:
    """
    Create a default NLM configuration.
    
    Returns:
        New Config instance with default values
        
    Examples:
        >>> config = create_default_config()
        >>> config.set("brain.regionCount", 5)
    """
    return Config()

# Convenience constants for common configuration keys
class ConfigKeys:
    """Convenient access to common configuration key prefixes"""
    
    BRAIN = "brain"
    WORLD = "world"
    NEUROMODULATION = "neuromodulation"
    MEMORY = "memory"
    COGNITION = "cognition"
    PLASTICITY = "plasticity"
    DEVELOPMENT = "development"
    
    @classmethod
    def brain(cls, subkey: str) -> str:
        """Get brain configuration key"""
        return f"{cls.BRAIN}.{subkey}"
    
    @classmethod
    def world(cls, subkey: str) -> str:
        """Get world configuration key"""
        return f"{cls.WORLD}.{subkey}"
    
    @classmethod
    def neuromodulation(cls, subkey: str) -> str:
        """Get neuromodulation configuration key"""
        return f"{cls.NEUROMODULATION}.{subkey}"
    
    @classmethod
    def memory(cls, subkey: str) -> str:
        """Get memory configuration key"""
        return f"{cls.MEMORY}.{subkey}"
    
    @classmethod
    def cognition(cls, subkey: str) -> str:
        """Get cognition configuration key"""
        return f"{cls.COGNITION}.{subkey}"
    
    @classmethod
    def plasticity(cls, subkey: str) -> str:
        """Get plasticity configuration key"""
        return f"{cls.PLASTICITY}.{subkey}"
    
    @classmethod
    def development(cls, subkey: str) -> str:
        """Get development configuration key"""
        return f"{cls.DEVELOPMENT}.{subkey}"

# Create default config instance for convenience
_config_instance: Optional[Config] = None

def get_config() -> Config:
    """Get default configuration instance (singleton pattern)"""
    global _config_instance
    if _config_instance is None:
        _config_instance = create_default_config()
    return _config_instance

# Load default config from command line if provided
try:
    import sys
    if len(sys.argv) > 1:
        get_config().load_from_args()
except Exception:
    pass  # Silent failure - let users set config manually
