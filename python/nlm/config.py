"""
Configuration management for NLM simulation.

High-level Python API for managing NLM simulation parameters and settings.
"""

import json
import os
from typing import Any, Dict, Optional, Union
from pathlib import Path

import pybind11
from pybind11 import kwargs, arg

from .exceptions import NLMConfigurationError, NLMValidationError

class Config:
    """NLM Configuration manager.
    
    Manages simulation parameters and settings for NLM brains.
    Provides a high-level Python interface to the C++ Config class.
    """
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        """Initialize configuration.
        
        Args:
            config_dict: Optional dictionary of configuration parameters
        """
        try:
            from pynlm import _pynlm
            self._config = _pynlm.createDefaultConfig()
            
            if config_dict:
                for key, value in config_dict.items():
                    self.set(key, value)
                    
        except ImportError:
            raise ImportError("pynlm module not found. Please install NLM Python bindings.")
    
    def set(self, key: str, value: Any, source: str = "user") -> bool:
        """Set a configuration parameter.
        
        Args:
            key: Configuration key (e.g., "brain.neuron_count")
            value: Value to set
            source: Source of configuration (user, default, command_line)
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Convert Python types to C++ types as needed
            if isinstance(value, bool):
                return self._config.set(key, int(value), source)
            elif isinstance(value, int):
                return self._config.set(key, int64_t(value), source)
            elif isinstance(value, float):
                return self._config.set(key, float(value), source)
            elif isinstance(value, str):
                return self._config.set(key, value, source)
            else:
                # Try to handle as general type
                return self._config.set(key, value, source)
        except Exception as e:
            raise NLMConfigurationError(f"Failed to set config key '{key}': {e}")
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration parameter.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default
        """
        try:
            return self._config.get(key, default)
        except Exception:
            return default
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key to check
            
        Returns:
            True if key exists, False otherwise
        """
        return self._config.has(key)
    
    def load_from_file(self, filepath: Union[str, Path]) -> bool:
        """Load configuration from file.
        
        Args:
            filepath: Path to configuration file (JSON)
            
        Returns:
            True if successful, False otherwise
        """
        filepath = str(filepath)
        if not os.path.exists(filepath):
            raise NLMConfigurationError(f"Config file not found: {filepath}")
            
        return self._config.loadFromFile(filepath)
    
    def save_to_file(self, filepath: Union[str, Path]) -> bool:
        """Save configuration to file.
        
        Args:
            filepath: Path to save configuration file
            
        Returns:
            True if successful, False otherwise
        """
        return self._config.saveToFile(str(filepath))
    
    def get_keys(self) -> list:
        """Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return self._config.getKeys()
    
    def clear(self):
        """Clear all configuration entries."""
        self._config.clear()
    
    def summary(self) -> str:
        """Get configuration summary.
        
        Returns:
            Summary string
        """
        return self._config.summary()
    
    def __repr__(self) -> str:
        return f"<Config: {self.summary()}>"
    
    def __str__(self) -> str:
        return self.summary()
    
    # Convenience methods for common configuration patterns
    def set_neuron_count(self, count: int):
        """Set number of neurons."""
        self.set("brain.neuron_count", count)
    
    def set_synapse_density(self, density: float):
        """Set synapse density (0.0-1.0)."""
        if not 0.0 <= density <= 1.0:
            raise NLMValidationError(f"Synapse density must be between 0.0 and 1.0, got {density}")
        self.set("brain.synapse_density", density)
    
    def set_connection_probability(self, probability: float):
        """Set connection probability (0.0-1.0)."""
        if not 0.0 <= probability <= 1.0:
            raise NLMValidationError(f"Connection probability must be between 0.0 and 1.0, got {probability}")
        self.set("brain.connection_probability", probability)
    
    def set_initial_weight_mean(self, mean: float):
        """Set initial synaptic weight mean."""
        self.set("brain.initial_weight_mean", mean)
    
    def set_initial_weight_std(self, std: float):
        """Set initial synaptic weight standard deviation."""
        self.set("brain.initial_weight_std", std)
    
    def set_neuron_threshold(self, threshold: float):
        """Set neuron threshold potential."""
        self.set("brain.v_thresh", threshold)
    
    def set_neuron_resting_potential(self, potential: float):
        """Set neuron resting potential."""
        self.set("brain.v_rest", potential)
    
    def set_neuron_reset_potential(self, potential: float):
        """Set neuron reset potential."""
        self.set("brain.v_reset", potential)
    
    def set_leak_conductance(self, conductance: float):
        """Set membrane leak conductance."""
        self.set("brain.tau_mem", conductance)
    
    def set_refractory_period(self, period: float):
        """Set refractory period."""
        self.set("brain.tau_ref", period)
    
    def set_stdp_enable(self, enable: bool = True):
        """Enable/disable STDP."""
        self.set("plasticity.stdp.enable", enable)
    
    def set_hebbian_enable(self, enable: bool = True):
        """Enable/disable Hebbian learning."""
        self.set("plasticity.hebbian.enable", enable)
    
    def set_structural_plasticity_enable(self, enable: bool = True):
        """Enable/disable structural plasticity."""
        self.set("plasticity.structural.enable", enable)
    
    def set_curiosity_enable(self, enable: bool = True):
        """Enable/disable curiosity-driven exploration."""
        self.set("neuromod.dopamine.scale", 1.0 if enable else 0.0)
        self.set("neuromod.curiosity.enable", enable)
    
    def set_novelty_enable(self, enable: bool = True):
        """Enable/disable novelty detection."""
        self.set("neuromod.novelty.enable", enable)
    
    def get_brain_config(self) -> Dict[str, Any]:
        """Get brain-specific configuration parameters."""
        brain_keys = [k for k in self.get_keys() if k.startswith("brain.")]
        return {key.replace("brain.", ""): self.get(key) for key in brain_keys}
    
    def get_plasticity_config(self) -> Dict[str, Any]:
        """Get plasticity configuration parameters."""
        plasticity_keys = [k for k in self.get_keys() if k.startswith("plasticity.")]
        return {key: self.get(key) for key in plasticity_keys}
    
    def get_neuromod_config(self) -> Dict[str, Any]:
        """Get neuromodulation configuration parameters."""
        neuromod_keys = [k for k in self.get_keys() if k.startswith("neuromod.")]
        return {key: self.get(key) for key in neuromod_keys}
    
    def reset_to_defaults(self):
        """Reset configuration to defaults."""
        self.clear()
        # Set some common defaults
        self.set_neuron_count(1000)
        self.set_synapse_density(0.1)
        self.set_connection_probability(0.05)
        self.set_initial_weight_mean(0.5)
        self.set_initial_weight_std(0.1)
        self.set_neuron_threshold(-50.0)
        self.set_neuron_resting_potential(-70.0)
        self.set_neuron_reset_potential(-75.0)
        self.set_leak_conductance(20.0)
        self.set_refractory_period(2.0)
        
        # Enable common features
        self.set_stdp_enable(True)
        self.set_hebbian_enable(True)
        self.set_structural_plasticity_enable(True)
        self.set_curiosity_enable(True)
        self.set_novelty_enable(True)
    
    @classmethod
    def from_preset(cls, preset: str) -> 'Config':
        """Create configuration from preset.
        
        Args:
            preset: Preset name ("small", "medium", "large", "learning", "exploration")
            
        Returns:
            Config instance
        """
        presets = {
            "small": {
                "brain.neuron_count": 500,
                "brain.synapse_density": 0.05,
                "brain.connection_probability": 0.02,
            },
            "medium": {
                "brain.neuron_count": 2000,
                "brain.synapse_density": 0.1,
                "brain.connection_probability": 0.05,
            },
            "large": {
                "brain.neuron_count": 10000,
                "brain.synapse_density": 0.2,
                "brain.connection_probability": 0.1,
            },
            "learning": {
                "brain.neuron_count": 1000,
                "plasticity.stdp.enable": True,
                "plasticity.hebbian.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
            },
            "exploration": {
                "brain.neuron_count": 1500,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "plasticity.structural.enable": True,
            }
        }
        
        if preset not in presets:
            raise NLMConfigurationError(f"Unknown preset: {preset}. Available: {list(presets.keys())}")
            
        config = cls()
        for key, value in presets[preset].items():
            config.set(key, value)
            
        return config