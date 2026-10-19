"""
Core NLM configuration utilities for Python bindings.
"""

from typing import Dict, Any, List, Optional
from dataclasses import dataclass
import json
import os

@dataclass
class Config:
    """
    Configuration for NLM neural simulation.
    
    Provides key-value storage with type safety and serialization.
    """
    
    def __init__(self):
        self._data: Dict[str, Any] = {}
        self._defaults: Dict[str, Any] = {}
        self._set_defaults()
    
    def _set_defaults(self):
        """Set default configuration values."""
        self._defaults = {
            'neuron_count': 500,
            'region_count': 2,
            'simulation_timestep': 0.001,
            'random_seed': 42,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'stdp_tau': 20.0,
            'synaptogenesis_rate': 0.0001,
            'pruning_rate': 0.00001,
            'enable_development': True,
            'enable_checkpointing': True,
            'enable_replay': True,
            'learning_rate': 0.01,
            'novelty_threshold': 0.5,
            'curiosity_threshold': 0.3,
            'max_steps': 1000,
            'max_episodes': 5,
            'vision_width': 16,
            'vision_height': 16,
            'vision_channels': 3,
            'environment_width': 20,
            'environment_height': 20,
            'audio_sample_rate': 44100,
            'audio_num_samples': 16384,
        }
    
    def set(self, key: str, value: Any, source: str = "python") -> None:
        """
        Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            source: Source of the configuration (python, file, command_line)
        """
        self._data[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        return self._data.get(key, self._defaults.get(key, default))
    
    def has(self, key: str) -> bool:
        """
        Check if configuration has a key.
        
        Args:
            key: Configuration key to check
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._data or key in self._defaults
    
    def get_keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return list(set(self._data.keys()) | set(self._defaults.keys()))
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self._data.clear()
    
    def summary(self) -> str:
        """
        Get a summary of the configuration.
        
        Returns:
            String summary
        """
        lines = ["Configuration Summary:"]
        for key in sorted(self.get_keys()):
            value = self._data.get(key, self._defaults.get(key, "(no value)"))
            lines.append(f"  {key}: {value}")
        return "\n".join(lines)
    
    def load_from_file(self, filepath: str) -> bool:
        """
        Load configuration from a JSON file.
        
        Args:
            filepath: Path to JSON configuration file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            for key, value in data.items():
                self.set(key, value, "file")
            return True
        except Exception:
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """
        Save configuration to a JSON file.
        
        Args:
            filepath: Path to save JSON configuration file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            with open(filepath, 'w') as f:
                json.dump(self._data, f, indent=2)
            return True
        except Exception:
            return False
    
    def __repr__(self) -> str:
        return f"<Config with {len(self._data)} custom values>"

    @staticmethod
    def create_default() -> 'Config':
        """
        Create a default configuration.
        
        Returns:
            Default configuration
        """
        config = Config()
        for key, value in config._defaults.items():
            config.set(key, value, "default")
        return config

__all__ = ['Config']
