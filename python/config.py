"""
Configuration module for NLM.
Provides configuration management for the neural simulation framework.
"""

from typing import Dict, Any, Optional, List
import json
import os

# Import the C++ Config class
from ..bindings import Config

__all__ = ['ConfigManager', 'validate_config', 'get_default_config']

class ConfigManager:
    """
    High-level configuration manager for NLM.
    
    Provides Python-friendly interface to NLM configuration with validation
    and helper methods for common configuration patterns.
    """
    
    def __init__(self, config: Optional['Config'] = None):
        """
        Initialize configuration manager.
        
        Args:
            config: Optional existing Config object. If None, creates default.
        """
        self._config = config or Config()
    
    def load_from_file(self, filepath: str) -> None:
        """
        Load configuration from file.
        
        Args:
            filepath: Path to JSON configuration file.
        """
        self._config.loadFromFile(filepath)
    
    def save_to_file(self, filepath: str) -> None:
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save JSON configuration file.
        """
        self._config.saveToFile(filepath)
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value by key.
        
        Args:
            key: Configuration key.
            default: Default value if key doesn't exist.
            
        Returns:
            Configuration value or default.
        """
        return self._config.has(key) and self._config.get(key) or default
    
    def set(self, key: str, value: Any) -> None:
        """
        Set configuration value.
        
        Args:
            key: Configuration key.
            value: Configuration value.
        """
        # Note: This would need to be implemented based on Config class capabilities
        pass
    
    def get_config(self) -> 'Config':
        """
        Get the underlying Config object.
        
        Returns:
            The Config object.
        """
        return self._config

def validate_config(config_dict: Dict[str, Any]) -> Dict[str, Any]:
    """
    Validate configuration dictionary.
    
    Args:
        config_dict: Configuration dictionary to validate.
        
    Returns:
        Validated configuration dictionary.
        
    Raises:
        ValueError: If configuration is invalid.
    """
    # Basic validation for common NLM configuration keys
    valid_keys = {
        'brain', 'world', 'simulation', 'memory', 'neuromodulation',
        'development', 'cognition', 'sensory', 'motor', 'plasticity'
    }
    
    for key in config_dict:
        if key not in valid_keys:
            raise ValueError(f"Invalid configuration key: {key}")
    
    return config_dict

def get_default_config() -> Dict[str, Any]:
    """
    Get default configuration for NLM.
    
    Returns:
        Default configuration dictionary.
    """
    return {
        'simulation': {
            'dt': 0.01,
            'max_steps': 10000,
            'checkpoint_interval': 1000
        },
        'brain': {
            'neuron_count': 1000,
            'region_count': 5,
            'learning_rate': 0.01
        },
        'world': {
            'width': 100,
            'height': 100,
            'vision_width': 20,
            'vision_height': 20
        }
    }

# Create global config instance for convenience
_global_config: Optional[ConfigManager] = None

def get_global_config() -> ConfigManager:
    """
    Get global configuration instance.
    
    Returns:
        Global ConfigManager instance.
    """
    global _global_config
    if _global_config is None:
        _global_config = ConfigManager()
    return _global_config