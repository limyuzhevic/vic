# NLM Configuration - Configuration presets and utilities

This module provides configuration presets and utilities for NLM simulations,
making it easy to use common configuration patterns and custom setups.
"""

import pynlm
from typing import Dict, Any


class ConfigPresets:
    """Predefined configuration presets for common use cases."""
    
    @staticmethod
    def create_default():
        """Create a default configuration."""
        return pynlm.create_standard_config()
    
    @staticmethod
    def create_learning_agent():
        """Create configuration optimized for learning agents."""
        config = pynlm.create_standard_config()
        config.set("brain.neuron_count", "2000")
        config.set("plasticity.stdp.enable", "true")
        config.set("plasticity.hebbian.enable", "true")
        config.set("neuromod.dopamine.scale", "1.5")
        config.set("neuromod.curiosity.enable", "true")
        config.set("neuromod.novelty.enable", "true")
        config.set("plasticity.structural.enable", "true")
        return config
    
    @staticmethod
    def create_exploratory_agent():
        """Create configuration optimized for exploratory agents."""
        config = ConfigPresets.create_learning_agent()
        config.set("neuromod.curiosity.scale", "2.0")
        config.set("brain.synapse_density", "0.2")
        config.set("plasticity.hebbian.enable", "true")
        config.set("plasticity.stdp.enable", "true")
        config.set("plasticity.stdp.learning_rate", "0.002")
        return config
    
    @staticmethod
    def create_development_agent():
        """Create configuration optimized for developmental agents."""
        config = ConfigPresets.create_learning_agent()
        config.set("plasticity.development.enabled", "true")
        config.set("plasticity.development.age_modulation", "true")
        config.set("plasticity.stdp.learning_rate", "0.005")
        config.set("plasticity.structural.rate", "0.001")
        config.set("plasticity.structural.prune_rate", "0.00005")
        return config
    
    @staticmethod
    def get_all_presets():
        """Get all available configuration presets."""
        return {
            "default": ConfigPresets.create_default(),
            "learning_agent": ConfigPresets.create_learning_agent(),
            "exploratory_agent": ConfigPresets.create_exploratory_agent(),
            "development_agent": ConfigPresets.create_development_agent(),
        }


# Quick access functions for common configurations
def get_config_for_research():
    """Get configuration optimized for research simulations."""
    config = ConfigPresets.create_learning_agent()
    config.set("brain.neuron_count", "5000")
    config.set("brain.synapse_density", "0.15")
    return config


def get_config_for_artificial():
    """Get configuration optimized for artificial intelligence simulations."""
    config = ConfigPresets.create_development_agent()
    config.set("brain.neuron_count", "3000")
    config.set("plasticity.stdp.learning_rate", "0.001")
    config.set("plasticity.hebbian.enable", "true")
    return config


def get_config_for_biomimetic():
    """Get configuration optimized for biomimetic simulations."""
    config = ConfigPresets.create_learning_agent()
    config.set("brain.synapse_density", "0.05")
    config.set("plasticity.stdp.tau_plus", "30.0")
    config.set("plasticity.stdp.tau_minus", "30.0")
    return config


# Convenience validation functions
def validate_config_briefly(config):
    """
    Perform brief validation on a configuration.
    
    Args:
        config: The configuration to validate.
        
    Returns:
        List of validation warnings (empty if valid).
    """
    warnings = []
    
    try:
        neuron_count = int(config.get("brain.neuron_count", "0"))
        if neuron_count <= 0:
            warnings.append("brain.neuron_count should be positive")
        elif neuron_count > 100000:
            warnings.append("brain.neuron_count is very large (> 100,000)")
    except (ValueError, TypeError):
        warnings.append("brain.neuron_count should be a valid integer")
    
    try:
        synapse_density = float(config.get("brain.synapse_density", "0"))
        if synapse_density < 0.0 or synapse_density > 1.0:
            warnings.append("brain.synapse_density should be between 0.0 and 1.0")
    except (ValueError, TypeError):
        warnings.append("brain.synapse_density should be a valid float")
    
    return warnings


def create_custom_config(**kwargs):
    """
    Create a custom configuration with specified parameters.
    
    Args:
        **kwargs: Configuration parameters to set.
        
    Returns:
        Config: A configured NLM configuration object.
        
    Example:
        >>> config = create_custom_config(
        ...     neuron_count=3000,
        ...     learning_rate=0.001,
        ...     enable_curiosity=True
        ... )
    """
    config = pynlm.create_standard_config()
    
    param_map = {
        "neuron_count": ("brain.neuron_count", str),
        "synapse_density": ("brain.synapse_density", str),
        "learning_rate": ("plasticity.stdp.learning_rate", str),
        "enable_curiosity": ("neuromod.curiosity.enable", str),
        "enable_novelty": ("neuromod.novelty.enable", str),
        "enable_stdp": ("plasticity.stdp.enable", str),
        "enable_hebbian": ("plasticity.hebbian.enable", str),
    }
    
    for key, value in kwargs.items():
        if key in param_map:
            param_name, param_type = param_map[key]
            config.set(param_name, param_type(value))
    
    return config


# Package metadata
__version__ = "0.1.0"
__all__ = [
    "ConfigPresets",
    "get_config_for_research",
    "get_config_for_artificial",
    "get_config_for_biomimetic",
    "validate_config_briefly",
    "create_custom_config",
]