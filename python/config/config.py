"""NLM Configuration Module

This module provides configuration utilities and presets for the NLM framework.
"""

import json
from typing import Dict, Any, Optional, List
import logging

logger = logging.getLogger(__name__)

class Config:
    """Configuration class for NLM system with enhanced Python integration.
    
    Provides comprehensive configuration management with parameter validation,
    type-safe operations, and serialization utilities.
    """
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        """Initialize configuration from dictionary.
        
        Args:
            config_dict: Initial configuration dictionary
        """
        self._config = {}
        self._defaults = self._get_default_config()
        
        if config_dict:
            self.update(config_dict)
            
    def _get_default_config(self):
        """Get default configuration values."""
        return {
            'neuron_count': 1000,
            'synapse_density': 0.1,
            'learning_rate': 0.01,
            'time_step': 0.1,
            'world_width': 100.0,
            'world_height': 100.0,
            'vision_width': 84,
            'vision_height': 84,
            'enable_structural_plasticity': True,
            'enable_reward_modulation': True,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_prediction_error': True,
            'max_neurons_per_region': 100,
            'random_seed': 42
        }
        
    def update(self, config_dict: Dict[str, Any]):
        """Update configuration with dictionary.
        
        Args:
            config_dict: Configuration dictionary to update
        """
        for key, value in config_dict.items():
            self.set(key, value)
            
    def set(self, key: str, value: Any):
        """Set configuration value with validation.
        
        Args:
            key: Configuration key
            value: Configuration value
            
        Raises:
            ValueError: If key is not valid or value is invalid
        """
        if key not in self._defaults:
            raise ValueError(f"Invalid configuration key: {key}")
            
        # Validate value based on key
        if key == 'neuron_count' and (not isinstance(value, int) or value <= 0):
            raise ValueError(f"neuron_count must be positive integer, got {value}")
        elif key == 'synapse_density' and (not isinstance(value, (int, float)) or not (0.0 < value <= 1.0)):
            raise ValueError(f"synapse_density must be between 0 and 1, got {value}")
        elif key == 'learning_rate' and (not isinstance(value, (int, float)) or not (0.0 < value <= 1.0)):
            raise ValueError(f"learning_rate must be between 0 and 1, got {value}")
        elif key == 'time_step' and (not isinstance(value, (int, float)) or not (0.0 < value <= 1.0)):
            raise ValueError(f"time_step must be between 0 and 1, got {value}")
        elif key in ['world_width', 'world_height'] and (not isinstance(value, (int, float)) or value <= 0):
            raise ValueError(f"{key} must be positive, got {value}")
            
        self._config[key] = value
        
    def get(self, key: str, default=None):
        """Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        return self._config.get(key, self._defaults.get(key, default))
        
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._config or key in self._defaults
        
    def keys(self):
        """Get all configuration keys.
        
        Returns:
            List of all configuration keys
        """
        return list(set(self._config.keys()) | set(self._defaults.keys()))
        
    def to_dict(self):
        """Convert configuration to dictionary.
        
        Returns:
            Dictionary with all configuration values
        """
        return self._config.copy()
        
    def validate(self):
        """Validate configuration for common issues.
        
        Returns:
            List of validation errors, empty if valid
        """
        errors = []
        
        # Check required parameters
        if self.get('neuron_count', 0) <= 0:
            errors.append("neuron_count must be positive")
            
        if self.get('learning_rate', 1.0) <= 0:
            errors.append("learning_rate must be positive")
            
        if self.get('synapse_density', 0.0) <= 0 or self.get('synapse_density', 0.0) > 1.0:
            errors.append("synapse_density must be between 0 and 1")
            
        return errors
        
    def summary(self):
        """Get configuration summary.
        
        Returns:
            String summary of configuration
        """
        neuron_count = self.get('neuron_count', 'N/A')
        learning_rate = self.get('learning_rate', 'N/A')
        world_size = f"{self.get('world_width', 'N/A')}x{self.get('world_height', 'N/A')}"
        
        return f"Config(neurons={neuron_count}, lr={learning_rate}, world={world_size})"
        
    def __repr__(self):
        """String representation of configuration."""
        return f"<Config: {self.summary()}>"
        
    def __str__(self):
        """String representation of configuration."""
        return self.summary()
        
    def copy(self):
        """Create a copy of configuration.
        
        Returns:
            New Config instance
        """
        new_config = Config()
        new_config._config = self._config.copy()
        new_config._defaults = self._defaults.copy()
        return new_config
        
    def merge(self, other):
        """Merge with another configuration.
        
        Args:
            other: Other Config instance to merge
            
        Returns:
            New merged Config instance
        """
        new_config = self.copy()
        for key, value in other.to_dict().items():
            new_config.set(key, value)
        return new_config
        
    def diff(self, other):
        """Get difference with another configuration.
        
        Args:
            other: Other Config instance to compare
            
        Returns:
            Dictionary with differences
        """
        differences = {}
        
        for key in self.keys():
            self_value = self.get(key)
            other_value = other.get(key)
            if self_value != other_value:
                differences[key] = {
                    'this': self_value,
                    'other': other_value
                }
                
        return differences
        
    def save_to_file(self, filepath):
        """Save configuration to file.
        
        Args:
            filepath: File path to save to
        """
        with open(filepath, 'w') as f:
            json.dump(self.to_dict(), f, indent=2)
            
    def load_from_file(self, filepath):
        """Load configuration from file.
        
        Args:
            filepath: File path to load from
        """
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
            self.update(config_dict)
            
    def export_for_python(self):
        """Export configuration for Python usage.
        
        Returns:
            Dictionary suitable for Python usage
        """
        return self.to_dict()
        
    def import_from_python(self, config_dict):
        """Import configuration from Python dictionary.
        
        Args:
            config_dict: Python dictionary with configuration
        """
        self.update(config_dict)