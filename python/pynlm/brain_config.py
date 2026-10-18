"""
Brain configuration management utilities.
"""

from typing import Dict, Any, Optional, Union
import json
import tempfile
import os

from .nlm_exception import ConfigurationError


class BrainConfig:
    """
    Helper class for creating and managing brain configurations.
    
    Provides utilities for:
    - Creating configurations with common parameter sets
    - Loading/saving configurations from files
    - Parameter validation
    - Configuration merging and inheritance
    """
    
    @staticmethod
    def create_default() -> 'Config':
        """Create a default brain configuration.
        
        Returns:
            Default configuration object
        """
        from pynlm_core import createDefaultConfig
        return createDefaultConfig()
    
    @staticmethod
    def with_parameters(
        neuron_count: int = 1000,
        synapse_density: float = 0.1,
        learning_rate: float = 0.001,
        v_thresh: float = -50.0,
        v_rest: float = -70.0,
        v_reset: float = -75.0,
        tau_mem: float = 20.0,
        tau_ref: float = 2.0,
        stdp_enable: bool = True,
        stdp_learning_rate: float = 0.001,
        hebbian_enable: bool = True,
        structural_enable: bool = True,
        neuromod_dopamine_scale: float = 1.0,
        neuromod_curiosity_enable: bool = True,
        neuromod_novelty_enable: bool = True,
        **kwargs
    ) -> 'Config':
        """
        Create a custom brain configuration with specified parameters.
        
        Args:
            neuron_count: Total number of neurons
            synapse_density: Synapse connectivity density (0.0-1.0)
            learning_rate: Default learning rate
            v_thresh: Neuron threshold potential (mV)
            v_rest: Resting potential (mV)
            v_reset: Reset potential after spike (mV)
            tau_mem: Membrane time constant (ms)
            tau_ref: Refractory period (ms)
            stdp_enable: Enable STDP plasticity
            stdp_learning_rate: STDP learning rate
            hebbian_enable: Enable Hebbian plasticity
            structural_enable: Enable structural plasticity
            neuromod_dopamine_scale: Dopamine modulation scale
            neuromod_curiosity_enable: Enable curiosity-driven exploration
            neuromod_novelty_enable: Enable novelty detection
            **kwargs: Additional configuration parameters
            
        Returns:
            Configured brain object
            
        Raises:
            ConfigurationError: If invalid parameters are provided
        """
        from pynlm_core import createBrain
        
        # Create default configuration
        config = BrainConfig.create_default()
        
        # Set basic brain parameters
        if neuron_count <= 0:
            raise ConfigurationError(f"Invalid neuron count: {neuron_count}. Must be positive.")
        if not (0.0 <= synapse_density <= 1.0):
            raise ConfigurationError(f"Invalid synapse density: {synapse_density}. Must be between 0.0 and 1.0.")
        
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", synapse_density)
        config.set("learning.rate", learning_rate)
        
        # Set neuron parameters
        config.set("brain.v_thresh", v_thresh)
        config.set("brain.v_rest", v_rest)
        config.set("brain.v_reset", v_reset)
        config.set("brain.tau_mem", tau_mem)
        config.set("brain.tau_ref", tau_ref)
        
        # Set plasticity parameters
        config.set("plasticity.stdp.enable", stdp_enable)
        config.set("plasticity.stdp.learning_rate", stdp_learning_rate)
        config.set("plasticity.hebbian.enable", hebbian_enable)
        config.set("plasticity.structural.enable", structural_enable)
        
        # Set neuromodulation parameters
        config.set("neuromod.dopamine.scale", neuromod_dopamine_scale)
        config.set("neuromod.curiosity.enable", neuromod_curiosity_enable)
        config.set("neuromod.novelty.enable", neuromod_novelty_enable)
        
        # Set additional parameters
        for key, value in kwargs.items():
            config.set(key, value)
        
        return createBrain(config)
    
    @staticmethod
    def from_file(filepath: str) -> 'Config':
        """
        Load configuration from a JSON file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            Loaded configuration object
            
        Raises:
            ConfigurationError: If file cannot be loaded
        """
        from pynlm_core import createBrain, createDefaultConfig
        
        # Check if file exists
        if not os.path.exists(filepath):
            raise ConfigurationError(f"Configuration file not found: {filepath}")
        
        # Load configuration
        config = createDefaultConfig()
        
        try:
            if not config.loadFromFile(filepath):
                raise ConfigurationError(f"Failed to load configuration from {filepath}")
        except Exception as e:
            raise ConfigurationError(f"Error loading configuration from {filepath}: {e}")
        
        return config
    
    @staticmethod
    def save_to_file(config: 'Config', filepath: str) -> bool:
        """
        Save configuration to a JSON file.
        
        Args:
            config: Configuration object to save
            filepath: Path to output file
            
        Returns:
            True if successful, False otherwise
            
        Raises:
            ConfigurationError: If file cannot be saved
        """
        try:
            return config.saveToFile(filepath)
        except Exception as e:
            raise ConfigurationError(f"Error saving configuration to {filepath}: {e}")
    
    @staticmethod
    def validate_config(config: 'Config') -> list:
        """
        Validate configuration parameters.
        
        Args:
            config: Configuration object to validate
            
        Returns:
            List of validation error messages (empty if valid)
        """
        errors = []
        
        # Check required keys
        required_keys = ["brain.neuron_count", "brain.synapse_density"]
        for key in required_keys:
            if not config.has(key):
                errors.append(f"Missing required key: {key}")
        
        # Validate neuron count
        if config.has("brain.neuron_count"):
            neuron_count = config.get("brain.neuron_count")
            if neuron_count is not None and neuron_count <= 0:
                errors.append(f"Invalid neuron count: {neuron_count}. Must be positive.")
        
        # Validate synapse density
        if config.has("brain.synapse_density"):
            synapse_density = config.get("brain.synapse_density")
            if synapse_density is not None and not (0.0 <= synapse_density <= 1.0):
                errors.append(f"Invalid synapse density: {synapse_density}. Must be between 0.0 and 1.0.")
        
        return errors
    
    @staticmethod
    def merge_configs(base_config: 'Config', override_config: 'Config') -> 'Config':
        """
        Merge two configurations (override takes precedence).
        
        Args:
            base_config: Base configuration
            override_config: Override configuration
            
        Returns:
            Merged configuration
        """
        # Create new config and copy all keys from base
        merged = BrainConfig.create_default()
        
        # Copy all keys from base
        for key in base_config.getKeys():
            merged.set(key, base_config.get(key))
        
        # Override with keys from override config
        for key in override_config.getKeys():
            merged.set(key, override_config.get(key))
        
        return merged
    
    @staticmethod
    def get_config_summary(config: 'Config') -> str:
        """
        Get a human-readable summary of configuration.
        
        Args:
            config: Configuration object
            
        Returns:
            Summary string
        """
        try:
            return config.summary()
        except Exception:
            # Fallback if summary fails
            keys = config.getKeys()
            summary_lines = ["Configuration Summary:", "=" * 40]
            
            for key in sorted(keys):
                try:
                    value = config.get(key)
                    summary_lines.append(f"{key}: {value}")
                except:
                    summary_lines.append(f"{key}: <unreadable>")
            
            return "\n".join(summary_lines)


# Create a simple Config class alias for convenience
# This will work with the actual pynlm module
Config = 'Config'  # This is just a placeholder, actual type is handled in __init__.py

__all__ = ['BrainConfig', 'Config']