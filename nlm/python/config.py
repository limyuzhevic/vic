#!/usr/bin/env python3
"""
NLM Configuration Module

Provides configuration management for NLM (Neural Learning Machine) simulations.
"""

import json
import os
from typing import Dict, Any, Optional

class Config:
    """
    High-level configuration manager for NLM simulations.
    
    This class provides a unified interface for managing NLM configuration
    with sensible defaults for different simulation scenarios.
    """
    
    def __init__(self):
        """Create a new configuration with sensible defaults."""
        self._settings = {
            "neuron_count": 1000,
            "simulation_timestep": 0.001,
            "random_seed": 42,
            "max_neurons_per_region": 100,
            "synapse_decay_rate": 0.001,
            "plasticity_enabled": True,
            "development_enabled": True,
            "learning_rate": 0.1,
            "memory_capacity": 1000,
            "world_width": 100,
            "world_height": 100,
        }
    
    def set(self, key: str, value: Any):
        """Set a configuration value."""
        self._settings[key] = value
    
    def get(self, key: str, default: Any = None):
        """Get a configuration value."""
        return self._settings.get(key, default)
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get an integer configuration value."""
        value = self._settings.get(key, default)
        try:
            return int(value)
        except (ValueError, TypeError):
            return default
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get a float configuration value."""
        value = self._settings.get(key, default)
        try:
            return float(value)
        except (ValueError, TypeError):
            return default
    
    def get_keys(self):
        """Get all configuration keys."""
        return list(self._settings.keys())
    
    def get_summary(self):
        """Get a summary of configuration settings."""
        summary = "NLM Configuration Summary:\n"
        for key, value in self._settings.items():
            summary += f"  {key}: {value}\n"
        return summary

    def __str__(self):
        return f"Config(settings={len(self._settings)} items)"

def create_config_from_dict(settings_dict: Dict[str, Any]) -> Config:
    """Create a Config instance from a dictionary."""
    config = Config()
    for key, value in settings_dict.items():
        config.set(key, value)
    return config

def create_config_from_json(json_file: str) -> Config:
    """Create a Config instance from a JSON file."""
    with open(json_file, 'r') as f:
        settings_dict = json.load(f)
    return create_config_from_dict(settings_dict)
