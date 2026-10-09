"""
NLM Configuration Management

Enhanced configuration system for NLM with property-based access and presets.
"""

from typing import Dict, Any, Optional, Union
from .exceptions import NLMConfigurationError

class EnhancedConfig:
    """
    Enhanced configuration for NLM with property-based access and presets.
    
    This class provides a more user-friendly interface for configuring
    NLM systems, with support for presets, validation, and type-safe access.
    """
    
    # Configuration presets
    PRESETS = {
        'beginner': {
            'brain': {
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'connection_probability': 0.05,
            },
            'neuromodulation': {
                'curiosity': {'enable': True},
                'novelty': {'enable': True},
                'dopamine': {'scale': 1.0},
            },
            'development': {
                'enable_development': True,
                'stage_duration': 60.0,
            },
        },
        'explorer': {
            'brain': {
                'neuron_count': 5000,
                'synapse_density': 0.05,
                'connection_probability': 0.03,
            },
            'neuromodulation': {
                'curiosity': {'enable': True},
                'novelty': {'enable': True},
                'dopamine': {'scale': 1.5},
            },
            'development': {
                'enable_development': True,
                'stage_duration': 120.0,
            },
        },
        'researcher': {
            'brain': {
                'neuron_count': 10000,
                'synapse_density': 0.01,
                'connection_probability': 0.02,
            },
            'neuromodulation': {
                'curiosity': {'enable': True},
                'novelty': {'enable': True},
                'dopamine': {'scale': 2.0},
            },
            'development': {
                'enable_development': True,
                'stage_duration': 300.0,
            },
            'plasticity': {
                'structural': {'enable': True},
            },
        },
    }
    
    def __init__(self, settings: Optional[Dict[str, Any]] = None):
        """
        Initialize configuration.
        
        Args:
            settings: Dictionary of configuration settings
        """
        self._settings = settings or {}
        self._validate_settings()
    
    def _validate_settings(self):
        """Validate configuration settings."""
        # Basic validation
        if 'brain' in self._settings:
            brain_settings = self._settings['brain']
            if 'neuron_count' in brain_settings:
                neuron_count = brain_settings['neuron_count']
                if not isinstance(neuron_count, (int, float)) or neuron_count < 100:
                    raise NLMConfigurationError(f"Invalid neuron count: {neuron_count}")
    
    def set(self, category: str, key: str, value: Any):
        """
        Set a configuration value.
        
        Args:
            category: Configuration category (e.g., 'brain', 'neuromodulation')
            key: Configuration key
            value: Configuration value
        """
        if category not in self._settings:
            self._settings[category] = {}
        self._settings[category][key] = value
        self._validate_settings()
    
    def get(self, category: str, key: str, default_value: Any = None) -> Any:
        """
        Get a configuration value.
        
        Args:
            category: Configuration category
            key: Configuration key
            default_value: Default value if not found
            
        Returns:
            Configuration value or default
        """
        category_settings = self._settings.get(category, {})
        return category_settings.get(key, default_value)
    
    def get_int(self, category: str, key: str, default_value: int = 0) -> int:
        """Get integer configuration value."""
        value = self.get(category, key, default_value)
        return int(value) if value is not None else default_value
    
    def get_float(self, category: str, key: str, default_value: float = 0.0) -> float:
        """Get float configuration value."""
        value = self.get(category, key, default_value)
        return float(value) if value is not None else default_value
    
    def get_bool(self, category: str, key: str, default_value: bool = False) -> bool:
        """Get boolean configuration value."""
        value = self.get(category, key, default_value)
        return bool(value) if value is not None else default_value
    
    def load_preset(self, preset: str):
        """
        Load a configuration preset.
        
        Args:
            preset: Preset name ('beginner', 'explorer', 'researcher')
        """
        if preset not in self.PRESETS:
            raise NLMConfigurationError(f"Unknown preset: {preset}")
        
        preset_settings = self.PRESETS[preset]
        for category, settings in preset_settings.items():
            for key, value in settings.items():
                self.set(category, key, value)
    
    def validate(self) -> bool:
        """
        Validate configuration.
        
        Returns:
            True if configuration is valid, False otherwise
        """
        try:
            self._validate_settings()
            return True
        except NLMConfigurationError:
            return False
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self._settings.copy()
    
    def from_dict(self, settings: Dict[str, Any]):
        """
        Load configuration from dictionary.
        
        Args:
            settings: Configuration settings
        """
        self._settings = settings.copy()
        self._validate_settings()
    
    def __repr__(self) -> str:
        return f"EnhancedConfig({len(self._settings)} categories)"
    
    def __str__(self) -> str:
        return f"NLM Configuration with {len(self._settings)} categories"


def create_config(settings: Optional[Dict[str, Any]] = None) -> EnhancedConfig:
    """
    Create a new NLM configuration.
    
    Args:
        settings: Optional configuration settings
        
    Returns:
        EnhancedConfig instance
    """
    return EnhancedConfig(settings)


def create_beginner_config() -> EnhancedConfig:
    """Create a beginner-friendly configuration."""
    config = EnhancedConfig()
    config.load_preset('beginner')
    return config


def create_explorer_config() -> EnhancedConfig:
    """Create an explorer configuration."""
    config = EnhancedConfig()
    config.load_preset('explorer')
    return config


def create_researcher_config() -> EnhancedConfig:
    """Create a researcher configuration."""
    config = EnhancedConfig()
    config.load_preset('researcher')
    return config