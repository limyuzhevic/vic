"""Advanced configuration management tools for NLM."""

import json
import os
from typing import Dict, List, Optional, Any, Union
from dataclasses import dataclass, asdict
from pathlib import Path
import copy

from pynlm import Config

@dataclass
class TemplateConfig:
    """Template configuration with metadata."""
    name: str
    description: str
    config: Dict[str, Any]
    tags: List[str] = None
    version: str = "1.0.0"
    
    def __post_init__(self):
        if self.tags is None:
            self.tags = []

@dataclass
class ValidationError:
    """Configuration validation error."""
    key: str
    message: str
    severity: str = "error"  # "error", "warning", "info"
    
@dataclass
class ValidationResult:
    """Configuration validation result."""
    valid: bool = True
    errors: List[ValidationError] = None
    warnings: List[ValidationError] = None
    suggestions: List[str] = None
    
    def __post_init__(self):
        if self.errors is None:
            self.errors = []
        if self.warnings is None:
            self.warnings = []
        if self.suggestions is None:
            self.suggestions = []

@dataclass
class ConfigDiff:
    """Difference between two configurations."""
    added_keys: List[str] = None
    removed_keys: List[str] = None
    modified_keys: Dict[str, Dict[str, Any]] = None
    unchanged_keys: List[str] = None
    
    def __post_init__(self):
        if self.added_keys is None:
            self.added_keys = []
        if self.removed_keys is None:
            self.removed_keys = []
        if self.modified_keys is None:
            self.modified_keys = {}
        if self.unchanged_keys is None:
            self.unchanged_keys = []

class ConfigurationManager:
    """Advanced configuration management tools.
    
    Provides comprehensive configuration management including templates,
    validation, merging, comparison, and preset generation.
    
    Attributes:
        templates: Dictionary of saved configuration templates
        config_history: History of loaded/saved configurations
        validation_rules: Custom validation rules
        preset_templates: Built-in preset templates
    """
    
    def __init__(self):
        """Initialize configuration manager."""
        self.templates: Dict[str, TemplateConfig] = {}
        self.config_history: List[Dict[str, Any]] = []
        self.validation_rules: Dict[str, Any] = {}
        self.preset_templates: Dict[str, Dict[str, Any]] = {}
        
        # Load built-in preset templates
        self._load_builtin_presets()
    
    def _load_builtin_presets(self):
        """Load built-in preset templates."""
        self.preset_templates = {
            "training": {
                "brain": {
                    "neuron_count": 2000,
                    "synapse_density": 0.1,
                    "development": {
                        "enabled": True,
                        "stage": "CriticalPeriod"
                    }
                },
                "neuromodulation": {
                    "reward_modulation": True,
                    "curiosity": True,
                    "novelty": True
                },
                "plasticity": {
                    "stdp": {"enable": True, "learning_rate": 0.001},
                    "hebbian": {"enable": True, "learning_rate": 0.0005},
                    "structural": {"enable": True, "growth_rate": 0.01}
                }
            },
            "challenge": {
                "brain": {
                    "neuron_count": 1000,
                    "vision_width": 8,
                    "vision_height": 8
                },
                "world": {
                    "width": 300,
                    "height": 300,
                    "max_energy": 200,
                    "environment": {
                        "hazards": True,
                        "resources": True,
                        "walls": True
                    }
                },
                "agent": {
                    "curiosity": 0.8,
                    "exploration_rate": 0.3,
                    "memory_capacity": 100
                }
            },
            "exploration": {
                "brain": {
                    "neuron_count": 1500,
                    "development": {
                        "enabled": True,
                        "stage": "Maturation"
                    },
                    "plasticity": {
                        "structural": {"enable": True, "growth_rate": 0.02}
                    }
                },
                "neuromodulation": {
                    "curiosity": True,
                    "novelty": True,
                    "prediction_error": True
                },
                "world": {
                    "width": 200,
                    "height": 200,
                    "environment": {
                        "resources": True,
                        "hazards": False
                    }
                }
            },
            "minimal": {
                "brain": {
                    "neuron_count": 500,
                    "development": {"enabled": False}
                },
                "world": {
                    "width": 50,
                    "height": 50
                }
            }
        }
    
    def load_template(self, filename: str) -> TemplateConfig:
        """Load configuration template from file.
        
        Args:
            filename: Template file path
            
        Returns:
            TemplateConfig: Loaded template
            
        Raises:
            ValueError: If template file cannot be loaded
        """
        try:
            with open(filename, 'r') as f:
                data = json.load(f)
            
            # Create TemplateConfig object
            template = TemplateConfig(**data)
            self.templates[template.name] = template
            
            # Add to history
            self.config_history.append({
                'action': 'load_template',
                'filename': filename,
                'timestamp': pd.Timestamp.now().isoformat() if 'pd' in globals() else None,
                'template_name': template.name
            })
            
            return template
            
        except Exception as e:
            raise ValueError(f"Failed to load template from {filename}: {e}")
    
    def get_template(self, name: str) -> Optional[TemplateConfig]:
        """Get a saved template by name.
        
        Args:
            name: Template name
            
        Returns:
            TemplateConfig: Template if found, None otherwise
        """
        return self.templates.get(name)
    
    def create_config(self, name: Optional[str] = None, **kwargs) -> Config:
        """Create new configuration from parameters.
        
        Args:
            name: Configuration name (optional)
            **kwargs: Configuration parameters
            
        Returns:
            Config: NLM configuration object
        """
        # Create configuration from kwargs
        config = Config()
        
        # Recursively set nested values
        def set_nested(config_obj, key_path, value):
            keys = key_path.split('.')
            current = config_obj
            
            for key in keys[:-1]:
                if not current.has(key):
                    current.set(key, Config())
                current = current.get(key)
            
            current.set(keys[-1], value)
        
        for key, value in kwargs.items():
            set_nested(config, key, value)
        
        # Add to history
        self.config_history.append({
            'action': 'create_config',
            'name': name,
            'timestamp': pd.Timestamp.now().isoformat() if 'pd' in globals() else None
        })
        
        return config
    
    def validate_config(self, config: Config) -> ValidationResult:
        """Validate configuration for errors.
        
        Args:
            config: Configuration to validate
            
        Returns:
            ValidationResult: Validation result
        """
        result = ValidationResult()
        
        # Get all configuration keys
        all_keys = config.getKeys()
        
        # Validate nested structure
        self._validate_structure(config, all_keys, result)
        
        # Validate specific values
        self._validate_values(config, result)
        
        # Check for conflicting settings
        self._validate_conflicts(config, result)
        
        # Generate suggestions
        self._generate_suggestions(config, result)
        
        return result
    
    def get_validation_suggestions(self, config: Config) -> List[str]:
        """Get validation suggestions for configuration.
        
        Args:
            config: Configuration to validate
            
        Returns:
            List[str]: List of suggestions
        """
        result = self.validate_config(config)
        return result.suggestions
    
    def merge_configs(self, config_list: List[str], overrides: Optional[Dict[str, Any]] = None) -> Config:
        """Merge multiple configurations.
        
        Args:
            config_list: List of configuration file paths
            overrides: Parameter overrides
            
        Returns:
            Config: Merged configuration
        """
        # Start with empty config
        merged_config = Config()
        
        # Load and merge each config file
        for filename in config_list:
            if os.path.exists(filename):
                temp_config = Config()
                if temp_config.loadFromFile(filename):
                    # Merge this config into the merged config
                    self._merge_configs_recursive(merged_config, temp_config)
        
        # Apply overrides
        if overrides:
            for key, value in overrides.items():
                merged_config.set(key, value)
        
        # Add to history
        self.config_history.append({
            'action': 'merge_configs',
            'files': config_list,
            'overrides': overrides,
            'timestamp': pd.Timestamp.now().isoformat() if 'pd' in globals() else None
        })
        
        return merged_config
    
    def generate_preset(self, **kwargs) -> Dict[str, Any]:
        """Generate preset configuration.
        
        Args:
            **kwargs: Preset parameters
            
        Returns:
            Dict[str, Any]: Preset configuration
        """
        # Find matching preset from built-in templates
        preset_name = kwargs.get('preset', 'minimal')
        base_config = self.preset_templates.get(preset_name, {}).copy()
        
        # Override with provided parameters
        def merge_nested(base, override):
            for key, value in override.items():
                if isinstance(value, dict) and key in base and isinstance(base[key], dict):
                    merge_nested(base[key], value)
                else:
                    base[key] = value
        
        merged_config = base_config.copy()
        merge_nested(merged_config, kwargs)
        
        return merged_config
    
    def save_config(self, config: Config, filename: str) -> None:
        """Save configuration to file.
        
        Args:
            config: Configuration to save
            filename: Output file path
        """
        Path(filename).parent.mkdir(parents=True, exist_ok=True)
        
        # Convert config to dictionary for saving
        config_dict = self._config_to_dict(config)
        
        with open(filename, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        # Add to history
        self.config_history.append({
            'action': 'save_config',
            'filename': filename,
            'timestamp': pd.Timestamp.now().isoformat() if 'pd' in globals() else None
        })
    
    def load_config(self, filename: str) -> Config:
        """Load configuration from file.
        
        Args:
            filename: Input file path
            
        Returns:
            Config: Loaded configuration
            
        Raises:
            ValueError: If configuration cannot be loaded
        """
        try:
            config = Config()
            if config.loadFromFile(filename):
                # Add to history
                self.config_history.append({
                    'action': 'load_config',
                    'filename': filename,
                    'timestamp': pd.Timestamp.now().isoformat() if 'pd' in globals() else None
                })
                return config
            else:
                raise ValueError(f"Failed to load configuration from {filename}")
        except Exception as e:
            raise ValueError(f"Error loading configuration: {e}")
    
    def compare_configs(self, config1: Config, config2: Config) -> ConfigDiff:
        """Compare two configurations.
        
        Args:
            config1: First configuration
            config2: Second configuration
            
        Returns:
            ConfigDiff: Comparison results
        """
        diff = ConfigDiff()
        
        # Get all keys from both configurations
        keys1 = set(config1.getKeys())
        keys2 = set(config2.getKeys())
        
        # Find added and removed keys
        diff.added_keys = list(keys2 - keys1)
        diff.removed_keys = list(keys1 - keys2)
        
        # Find modified and unchanged keys
        common_keys = keys1 & keys2
        for key in common_keys:
            val1 = self._get_nested_value(config1, key)
            val2 = self._get_nested_value(config2, key)
            
            if val1 != val2:
                diff.modified_keys[key] = {'old': val1, 'new': val2}
            else:
                diff.unchanged_keys.append(key)
        
        return diff
    
    def get_config_diff(self, config1: Config, config2: Config) -> ConfigDiff:
        """Get differences between two configurations.
        
        Args:
            config1: First configuration
            config2: Second configuration
            
        Returns:
            ConfigDiff: Configuration differences
        """
        return self.compare_configs(config1, config2)
    
    def close(self) -> None:
        """Close configuration manager and release resources."""
        self.templates.clear()
        self.config_history.clear()
        self.validation_rules.clear()
        self.preset_templates.clear()
    
    # Private helper methods
    
    def _validate_structure(self, config: Config, keys: List[str], result: ValidationResult):
        """Validate configuration structure."""
        # Validate key names
        for key in keys:
            if not isinstance(key, str) or not key.strip():
                error = ValidationError(
                    key=key,
                    message="Invalid key name: must be a non-empty string",
                    severity="error"
                )
                result.errors.append(error)
    
    def _validate_values(self, config: Config, result: ValidationResult):
        """Validate specific configuration values."""
        # Common validation rules
        if config.has('brain.neuron_count'):
            neuron_count = config.get('brain.neuron_count')
            if not isinstance(neuron_count, (int, float)) or neuron_count <= 0:
                error = ValidationError(
                    key='brain.neuron_count',
                    message="neuron_count must be a positive number",
                    severity="error"
                )
                result.errors.append(error)
        
        if config.has('world.width') or config.has('world.height'):
            width = config.get_or('world.width', 100)
            height = config.get_or('world.height', 100)
            
            if not isinstance(width, (int, float)) or width <= 0:
                error = ValidationError(
                    key='world.width',
                    message="world.width must be a positive number",
                    severity="error"
                )
                result.errors.append(error)
            
            if not isinstance(height, (int, float)) or height <= 0:
                error = ValidationError(
                    key='world.height',
                    message="world.height must be a positive number",
                    severity="error"
                )
                result.errors.append(error)
    
    def _validate_conflicts(self, config: Config, result: ValidationResult):
        """Validate for conflicting settings."""
        # Check for conflicting development and learning settings
        dev_enabled = config.get_or('development.enabled', False)
        reward_enabled = config.get_or('neuromodulation.reward_modulation', False)
        curiosity_enabled = config.get_or('neuromodulation.curiosity', False)
        
        if dev_enabled and not (reward_enabled or curiosity_enabled):
            warning = ValidationError(
                key='development.enabled',
                message="Development enabled without reward modulation or curiosity - consider enabling learning mechanisms",
                severity="warning"
            )
            result.warnings.append(warning)
    
    def _generate_suggestions(self, config: Config, result: ValidationResult):
        """Generate validation suggestions."""
        # Generate optimization suggestions
        if config.get_or('brain.neuron_count', 1000) > 5000:
            result.suggestions.append(
                "High neuron count may impact performance - consider optimizing for your use case"
            )
        
        if not config.has('plasticity.stdp.enable'):
            result.suggestions.append(
                "STDP plasticity not enabled - consider enabling for better learning"
            )
        
        if not config.has('neuromodulation.reward_modulation'):
            result.suggestions.append(
                "Reward modulation not enabled - consider enabling for reinforcement learning"
            )
    
    def _merge_configs_recursive(self, target: Config, source: Config):
        """Recursively merge source config into target config."""
        for key in source.getKeys():
            if target.has(key):
                # If both are Config objects, merge recursively
                if isinstance(target.get(key), Config) and isinstance(source.get(key), Config):
                    self._merge_configs_recursive(target.get(key), source.get(key))
                else:
                    # Override with source value
                    target.set(key, source.get(key))
            else:
                # Add new key
                target.set(key, source.get(key))
    
    def _config_to_dict(self, config: Config) -> Dict[str, Any]:
        """Convert Config object to dictionary."""
        result = {}
        
        for key in config.getKeys():
            result[key] = self._get_nested_value(config, key)
        
        return result
    
    def _get_nested_value(self, config: Config, key_path: str) -> Any:
        """Get nested value from config."""
        keys = key_path.split('.')
        current = config
        
        for key in keys:
            if current.has(key):
                current = current.get(key)
            else:
                return None
        
        return current


# Convenience function
def create_configuration_manager() -> ConfigurationManager:
    """Create a configuration manager.
    
    Returns:
        ConfigurationManager: Configuration manager instance
    """
    return ConfigurationManager()
