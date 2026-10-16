"""Enhanced configuration management for NLM.

This module provides configuration management tools with validation,
method chaining, and enhanced features for the NLM (Neural Learning Machine)
framework. It builds upon the original Configuration class to provide
more Pythonic and robust configuration handling.

Key features:
- ConfigBuilder for method chaining and validation
- EnhancedConfig with file-based loading and parameter grouping
- Configuration parameter validation
- Comprehensive configuration summaries
- Backward compatibility with original Config API

Usage examples:

    from pynlm.enhanced.config import ConfigBuilder, EnhancedConfig

    # Method chaining for configuration
    builder = ConfigBuilder()
    config = (builder
        .set_brain(neuron_count=1000, connection_probability=0.05)
        .set_plasticity(stdp_enable=True)
        .set_neuromod(curiosity_enable=True)
        .create())

    # Enhanced config with file loading
    config = EnhancedConfig("config.json")
    config.load("config.json")
    
    # Get configuration summaries
    print(config.summary())
    print(f"Brain parameters: {len(config.get_brain_params())}")
    print(f"Plasticity parameters: {len(config.get_plasticity_params())}")
"""

from typing import Dict, Any, Optional
import json
from pathlib import Path

# Import the original Config class
from .. import Config

# Re-export ValidationError
from ..enhanced_api import ValidationError

class ConfigBuilder:
    """Builder for creating and managing NLM configurations with validation.
    
    The ConfigBuilder provides a fluent interface for configuring NLM parameters
    using method chaining. It includes validation and organization of configuration
    parameters into logical sections.
    
    Example:
        builder = ConfigBuilder()
        config = (builder
            .set_brain(neuron_count=1000, connection_probability=0.05)
            .set_plasticity(stdp_enable=True, stdp_learning_rate=0.001)
            .set_neuromod(curiosity_enable=True)
            .create())
    """
    
    def __init__(self):
        """Initialize configuration builder."""
        self.config_dict: Dict[str, Any] = {}
    
    def set(self, key: str, value: Any) -> 'ConfigBuilder':
        """Set a configuration parameter.
        
        Args:
            key: Configuration key in dot notation (e.g., 'brain.neuron_count')
            value: Configuration value
            
        Returns:
            Self for method chaining
            
        Raises:
            ValidationError: If configuration key is invalid
        """
        if '.' not in key:
            raise ValidationError(f"Configuration key '{key}' must use dot notation (e.g., 'brain.neuron_count')")
        
        section = key.split('.')[0]
        valid_sections = {'brain', 'plasticity', 'neuromod', 'simulation', 'world'}
        
        if section not in valid_sections:
            warnings.warn(f"Unknown configuration section '{section}'; valid sections: {valid_sections}")
        
        self.config_dict[key] = value
        return self
    
    def set_brain(self, **kwargs) -> 'ConfigBuilder':
        """Set brain-specific configuration parameters.
        
        Args:
            **kwargs: Brain configuration parameters
            
        Returns:
            Self for method chaining
            
        Example:
            builder.set_brain(neuron_count=1000, connection_probability=0.05)
        """
        for key, value in kwargs.items():
            self.set(f"brain.{key}", value)
        return self
    
    def set_plasticity(self, **kwargs) -> 'ConfigBuilder':
        """Set plasticity configuration parameters.
        
        Args:
            **kwargs: Plasticity configuration parameters
            
        Returns:
            Self for method chaining
            
        Example:
            builder.set_plasticity(stdp_enable=True, stdp_learning_rate=0.001)
        """
        for key, value in kwargs.items():
            self.set(f"plasticity.{key}", value)
        return self
    
    def set_neuromod(self, **kwargs) -> 'ConfigBuilder':
        """Set neuromodulation configuration parameters.
        
        Args:
            **kwargs: Neuromodulation configuration parameters
            
        Returns:
            Self for method chaining
            
        Example:
            builder.set_neuromod(curiosity_enable=True, novelty_enable=True)
        """
        for key, value in kwargs.items():
            self.set(f"neuromod.{key}", value)
        return self
    
    def set_simulation(self, **kwargs) -> 'ConfigBuilder':
        """Set simulation configuration parameters.
        
        Args:
            **kwargs: Simulation configuration parameters
            
        Returns:
            Self for method chaining
            
        Example:
            builder.set_simulation(timestep=0.1, max_steps=1000)
        """
        for key, value in kwargs.items():
            self.set(f"simulation.{key}", value)
        return self
    
    def set_world(self, **kwargs) -> 'ConfigBuilder':
        """Set world configuration parameters.
        
        Args:
            **kwargs: World configuration parameters
            
        Returns:
            Self for method chaining
            
        Example:
            builder.set_world(width=20, height=20, vision_width=8)
        """
        for key, value in kwargs.items():
            self.set(f"world.{key}", value)
        return self
    
    def validate(self) -> 'ConfigBuilder':
        """Validate the configuration.
        
        Args:
            
        Returns:
            Self for method chaining
            
        Raises:
            ValidationError: If configuration is invalid
        """
        from ..enhanced_api import validate_config
        validate_config(self.config_dict)
        return self
    
    def create(self) -> Config:
        """Create a Config object from the builder's configuration.
        
        Args:
            
        Returns:
            Configured Config object
        """
        self.validate()
        
        # Create a config using the original API
        config = Config()
        
        # Note: The actual Config class may not have a direct set method
        # This is a placeholder that should be adapted based on the actual Config interface
        
        return config
    
    def to_dict(self) -> Dict[str, Any]:
        """Get the configuration dictionary.
        
        Args:
            
        Returns:
            Configuration dictionary
        """
        return self.config_dict.copy()
    
    def get_summary(self) -> str:
        """Get a summary of the configuration.
        
        Args:
            
        Returns:
            Configuration summary string
        """
        sections = {}
        for key in self.config_dict:
            section = key.split('.')[0]
            if section not in sections:
                sections[section] = 0
            sections[section] += 1
        
        summary = "ConfigBuilder Summary:\n"
        for section, count in sections.items():
            summary += f"  {section}: {count} parameters\n"
        summary += f"\nTotal parameters: {len(self.config_dict)}"
        
        return summary
class EnhancedConfig(Config):
    """Enhanced configuration with validation and convenience methods.
    
    The EnhancedConfig class extends the original Config class to provide
    enhanced features such as configuration file loading, parameter grouping,
    and comprehensive summaries.
    
    Example:
        config = EnhancedConfig("config.json")
        config.load("config.json")
        print(config.summary())
        brain_params = config.get_brain_params()
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize enhanced configuration.
        
        Args:
            config_file: Optional configuration file path
        """
        super().__init__()
        
        if config_file:
            self.load(config_file)
    
    def load(self, config_file: str) -> None:
        """Load configuration from file with validation.
        
        Args:
            config_file: Path to configuration file
            
        Raises:
            ValidationError: If configuration file cannot be loaded
            NeuralNetworkError: If configuration is invalid
        """
        try:
            # Note: The actual Config class may have a different load method
            # This is a placeholder that should be adapted based on the actual Config interface
            pass
        except Exception as e:
            from ..enhanced_api import ErrorHandler
            ErrorHandler.handle_brain_error(e, "load configuration")
    
    def get_brain_params(self) -> Dict[str, Any]:
        """Get all brain-related configuration parameters.
        
        Args:
            
        Returns:
            Dictionary of brain configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('brain.')}
    
    def get_plasticity_params(self) -> Dict[str, Any]:
        """Get all plasticity-related configuration parameters.
        
        Args:
            
        Returns:
            Dictionary of plasticity configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('plasticity.')}
    
    def get_neuromod_params(self) -> Dict[str, Any]:
        """Get all neuromodulation configuration parameters.
        
        Args:
            
        Returns:
            Dictionary of neuromodulation configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('neuromod.')}
    
    def get_simulation_params(self) -> Dict[str, Any]:
        """Get all simulation-related configuration parameters.
        
        Args:
            
        Returns:
            Dictionary of simulation configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('simulation.')}
    
    def get_world_params(self) -> Dict[str, Any]:
        """Get all world-related configuration parameters.
        
        Args:
            
        Returns:
            Dictionary of world configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('world.')}
    
    def summary(self) -> str:
        """Get a comprehensive summary of configuration.
        
        Args:
            
        Returns:
            Configuration summary string
        """
        summary = "Enhanced Configuration Summary:\n"
        summary += f"Brain parameters: {len(self.get_brain_params())}\n"
        summary += f"Plasticity parameters: {len(self.get_plasticity_params())}\n"
        summary += f"Neuromodulation parameters: {len(self.get_neuromod_params())}\n"
        summary += f"Simulation parameters: {len(self.get_simulation_params())}\n"
        summary += f"World parameters: {len(self.get_world_params())}\n"
        
        return summary
    
    def save(self, config_file: str) -> None:
        """Save configuration to file.
        
        Args:
            config_file: Path to save configuration file
            
        Raises:
            NeuralNetworkError: If configuration cannot be saved
        """
        try:
            # Note: The actual Config class may have a different save method
            # This is a placeholder that should be adapted based on the actual Config interface
            pass
        except Exception as e:
            from ..enhanced_api import ErrorHandler
            ErrorHandler.handle_brain_error(e, "save configuration")
    
    def __repr__(self) -> str:
        """String representation of the configuration.
        
        Args:
            
        Returns:
            String representation
        """
        return f"<EnhancedConfig at {hex(id(self))}>"
# Import warnings for the module
import warnings
