"""Brain factory for creating NLM brains with different configurations."""

from typing import Dict, Any, List
from ..config import Config
from .brain import Brain
from .exceptions import NLMException


class BrainFactory:
    """Factory for creating NLM brains with different configurations."""
    
    _presets = {}
    
    @classmethod
    def register_preset(cls, name: str, config: Config):
        """
        Register a brain configuration preset.
        
        Args:
            name: Preset name
            config: Configuration object
        """
        cls._presets[name] = config
    
    @classmethod
    def get_preset(cls, name: str) -> Config:
        """
        Get a registered preset configuration.
        
        Args:
            name: Preset name
            
        Returns:
            Configuration object
            
        Raises:
            NLMException: If preset not found
        """
        if name not in cls._presets:
            raise NLMException(f"Preset '{name}' not found")
        return cls._presets[name]
    
    @classmethod
    def list_presets(cls) -> List[str]:
        """List all registered preset names."""
        return list(cls._presets.keys())
    
    @classmethod
    def create(cls, config: Config = None, preset: str = None) -> Brain:
        """
        Create a brain.
        
        Args:
            config: Configuration (overrides preset if both provided)
            preset: Preset name to use
            
        Returns:
            Brain object
            
        Raises:
            NLMException: If neither config nor preset provided
        """
        if config is None and preset is None:
            raise NLMException("Either config or preset must be provided")
        
        if config is None:
            config = cls.get_preset(preset)
        
        return Brain(config)
    
    @classmethod
    def create_default(cls) -> Brain:
        """Create a brain with default configuration."""
        return Brain()
    
    @classmethod
    def create_explorer_brain(cls) -> Brain:
        """Create a brain optimized for exploration."""
        config = Config()
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.curiosity.enable", True)
        config.set("plasticity.novelty.enable", True)
        return Brain(config)
    
    @classmethod
    def create_expert_brain(cls) -> Brain:
        """Create a brain optimized for expert performance."""
        config = Config()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.scale", 2.0)
        return Brain(config)
    
    @classmethod
    def create_developer_brain(cls) -> Brain:
        """Create a brain with strong developmental plasticity."""
        config = Config()
        config.set("brain.neuron_count", 3000)
        config.set("plasticity.development.enabled", True)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.development.critical_period", True)
        return Brain(config)


# Convenience functions
def create_simple_brain(neuron_count: int = 1000) -> Brain:
    """Create a simple brain with specified neuron count."""
    config = Config()
    config.set("brain.neuron_count", neuron_count)
    return Brain(config)


def create_small_brain() -> Brain:
    """Create a small, fast brain for testing."""
    config = Config()
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.05)
    return Brain(config)


def create_large_brain() -> Brain:
    """Create a large, complex brain for research."""
    config = Config()
    config.set("brain.neuron_count", 10000)
    config.set("brain.synapse_density", 0.3)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    return Brain(config)


# Preset configurations
def create_explorer_preset() -> Config:
    """Create explorer brain preset."""
    config = Config()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.curiosity.enable", True)
    config.set("plasticity.novelty.enable", True)
    return config


def create_researcher_preset() -> Config:
    """Create researcher brain preset."""
    config = Config()
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    return config