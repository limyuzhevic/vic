"""
Pythonic configuration wrapper for NLM.

This module provides enhanced Pythonic APIs for working with NLM configuration,
featuring type-safe access, dictionary-style operations, and comprehensive
error handling.
"""

import sys
from typing import Any, Dict, List, Optional, Union
import json

# Add parent directory to path for imports
if len(sys.path) == 1:
    sys.path.append("..")

from nlm import Config, ConfigValue

class ConfigError(Exception):
    """Exception raised for configuration errors."""
    pass

class ConfigWrapper:
    """
    Enhanced Pythonic wrapper for NLM configuration.
    
    Provides type-safe access, dictionary-style operations, and comprehensive
    error handling for NLM configuration management.
    """
    
    def __init__(self, config: Optional[Config] = None):
        """
        Initialize configuration wrapper.
        
        Args:
            config: Existing NLM Config object to wrap. If None, creates default.
        """
        if config is None:
            self.config = Config()
        else:
            self.config = config
    
    def set(self, key: str, value: Any, source: str = "Runtime") -> None:
        """
        Set configuration value with automatic type conversion.
        
        Args:
            key: Configuration key
            value: Value to set (int, float, str, bool, list)
            source: Configuration source ("Runtime", "Default", "File", "CommandLine")
        
        Raises:
            ConfigError: If value cannot be converted
        """
        try:
            # Convert Python value to ConfigValue
            if value is None:
                config_value = ConfigValue()
            elif isinstance(value, bool):
                config_value = ConfigValue(value)
            elif isinstance(value, int):
                config_value = ConfigValue(value)
            elif isinstance(value, float):
                config_value = ConfigValue(value)
            elif isinstance(value, str):
                config_value = ConfigValue(value)
            elif isinstance(value, list):
                # Determine list element type
                if value and isinstance(value[0], int):
                    config_value = ConfigValue(value)
                elif value and isinstance(value[0], float):
                    config_value = ConfigValue(value)
                elif value and isinstance(value[0], str):
                    config_value = ConfigValue(value)
                else:
                    # Default to string list
                    config_value = ConfigValue(value)
            else:
                config_value = ConfigValue(str(value))
            
            self.config.set(key, config_value, source)
            
        except Exception as e:
            raise ConfigError(f"Failed to set configuration key '{key}': {e}")
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value with type inference.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
        
        Returns:
            Configuration value or default
        """
        try:
            value = self.config.get<ConfigValue>(key)
            if value.has_value():
                return self._config_value_to_python(value.value())
            return default
        except:
            return default
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get integer value from configuration."""
        value = self.get(key, default)
        return int(value) if value is not None else default
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get floating-point value from configuration."""
        value = self.get(key, default)
        return float(value) if value is not None else default
    
    def get_bool(self, key: str, default: bool = False) -> bool:
        """Get boolean value from configuration."""
        value = self.get(key, default)
        return bool(value) if value is not None else default
    
    def get_string(self, key: str, default: str = "") -> str:
        """Get string value from configuration."""
        value = self.get(key, default)
        return str(value) if value is not None else default
    
    def get_int_list(self, key: str) -> List[int]:
        """Get list of integers from configuration."""
        value = self.get(key, [])
        return list(value) if isinstance(value, list) else []
    
    def get_float_list(self, key: str) -> List[float]:
        """Get list of floats from configuration."""
        value = self.get(key, [])
        return list(value) if isinstance(value, list) else []
    
    def get_string_list(self, key: str) -> List[str]:
        """Get list of strings from configuration."""
        value = self.get(key, [])
        return list(value) if isinstance(value, list) else []
    
    def has(self, key: str) -> bool:
        """Check if key exists in configuration."""
        return self.config.has(key)
    
    def remove(self, key: str) -> None:
        """Remove key from configuration."""
        self.config.remove(key)
    
    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return self.config.get_keys()
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self.config.clear()
    
    def summary(self) -> str:
        """Get configuration summary."""
        return self.config.summary()
    
    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to save configuration
        
        Returns:
            True if successful, False otherwise
        """
        return self.config.save_to_file(filepath)
    
    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to load configuration from
        
        Returns:
            True if successful, False otherwise
        """
        return self.config.load_from_file(filepath)
    
    # Pythonic operator overloads
    def __getitem__(self, key: str) -> Any:
        """Get value using dictionary-style access."""
        return self.get(key)
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Set value using dictionary-style access."""
        self.set(key, value)
    
    def __contains__(self, key: str) -> bool:
        """Check if key exists using 'in' operator."""
        return self.has(key)
    
    def __len__(self) -> int:
        """Get number of configuration keys."""
        return len(self.keys())
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<ConfigWrapper: {len(self)} keys>"
    
    def __str__(self) -> str:
        """String representation."""
        return self.summary()
    
    @staticmethod
    def _config_value_to_python(value: ConfigValue) -> Any:
        """Convert ConfigValue to Python object."""
        if isinstance(value, int):
            return value
        elif isinstance(value, float):
            return value
        elif isinstance(value, bool):
            return value
        elif isinstance(value, str):
            return value
        elif isinstance(value, list):
            return value
        else:
            return str(value)
    
    @classmethod
    def create_default(cls) -> 'ConfigWrapper':
        """Create a new default configuration wrapper."""
        return cls()