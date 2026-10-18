"""NLM Python Bindings - Configuration Module

This module provides the Config class for configuring the NLM brain system.
Created as part of the modularization effort to improve maintainability.
"""

from __future__ import annotations
from typing import Dict, Any, Optional, Union
import json
import sys
import os

# Import the main NLM module
import pynlm as nlm

class Config:
    """Configuration class for NLM system.
    
    This class manages configuration parameters for the NLM brain simulation.
    It provides methods to load/save configurations from files and command-line
    arguments, and to access individual configuration values.
    """
    
    def __init__(self):
        """Initialize an empty configuration."""
        self._config: Dict[str, Any] = {}
        self._initialized = False
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file
            
        Returns:
            True if loading succeeded, False otherwise
        """
        try:
            with open(filepath, 'r') as f:
                self._config = json.load(f)
            self._initialized = True
            return True
        except Exception as e:
            print(f"Error loading config from {filepath}: {e}")
            return False
    
    def load_from_args(self, argc: int, argv: list[str]) -> bool:
        """Load configuration from command line arguments.
        
        Args:
            argc: Argument count
            argv: Argument list
            
        Returns:
            True if loading succeeded, False otherwise
        """
        try:
            # Parse command line arguments for configuration
            for i in range(1, argc):
                if argv[i].startswith("--"):
                    key = argv[i][2:]
                    if i + 1 < argc and not argv[i + 1].startswith("--"):
                        # Parse value based on type
                        value_str = argv[i + 1]
                        
                        # Try to parse as different types
                        if "." in value_str:
                            try:
                                value = float(value_str)
                            except ValueError:
                                value = value_str
                        else:
                            try:
                                value = int(value_str)
                            except ValueError:
                                value = value_str
                        
                        self._config[key] = value
                        i += 1  # Skip next argument
            
            self._initialized = True
            return True
        except Exception as e:
            print(f"Error loading config from args: {e}")
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to a JSON file.
        
        Args:
            filepath: Path to save the JSON configuration file
            
        Returns:
            True if saving succeeded, False otherwise
        """
        try:
            with open(filepath, 'w') as f:
                json.dump(self._config, f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving config to {filepath}: {e}")
            return False
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists.
        
        Args:
            key: Configuration key to check
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._config
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value by key.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if not found
        """
        return self._config.get(key, default)
    
    def get_keys(self) -> list[str]:
        """Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return list(self._config.keys())
    
    def clear(self):
        """Clear all configuration entries."""
        self._config.clear()
        self._initialized = False
    
    def summary(self) -> str:
        """Get a summary string of the configuration.
        
        Returns:
            Summary string
        """
        if not self._config:
            return "Empty configuration"
        
        summary_lines = [f"{key}: {value}" for key, value in self._config.items()]
        return "Configuration with " + str(len(self._config)) + " entries:\n" + "\n".join(summary_lines)
    
    def set(self, key: str, value: Any):
        """Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        self._config[key] = value
        self._initialized = True
    
    def __repr__(self):
        return f"<Config: {self.summary()}>"
    
    def __eq__(self, other):
        if not isinstance(other, Config):
            return False
        return self._config == other._config

# Factory function for creating default config

def create_default_config() -> Config:
    """Create a default configuration for NLM.
    
    Returns:
        New Config instance with default values
    """
    config = Config()
    
    # Set some reasonable default values
    config.set("neuron_count", 1000)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01)
    config.set("stdp_ltd_weight", 0.012)
    config.set("synaptogenesis_rate", 0.0001)
    config.set("pruning_rate", 0.00001)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    return config