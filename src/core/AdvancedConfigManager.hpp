# Advanced Configuration System for NLM

"""
Enhanced configuration system with improved type safety, validation, and user experience.

This module provides:
- Schema-based configuration validation
- Type-safe configuration access
- Configuration inheritance and merging
- Environment variable integration
- Configuration templates and presets
- Better error messages and logging
"""

import json
import os
from typing import Dict, Any, List, Optional, Union, Set, Tuple
from pathlib import Path
import logging

# Configure logging
logger = logging.getLogger(__name__)

class ConfigurationError(Exception):
    """Base exception for configuration errors."""
    pass

class ValidationError(ConfigurationError):
    """Exception raised when configuration validation fails."""
    def __init__(self, message: str, field: str = None):
        super().__init__(message)
        self.field = field

class ConfigurationSchema:
    """
    Schema definition for configuration validation.
    
    Defines the structure, types, and constraints for configuration values.
    """
    
    def __init__(self, description: str = "", schema_version: str = "1.0.0"):
        self.description = description
        self.schema_version = schema_version
        self.fields: Dict[str, Dict[str, Any]] = {}
        self.groups: Dict[str, List[str]] = {}
        self.inheritance_chains: List[List[str]] = []
        
    def add_field(self, name: str, field_type: type, default: Any = None, 
                 required: bool = False, validation: Optional[Callable] = None,
                 description: str = "", group: str = "",
                 validation_message: str = None):
        """
        Add a field to the configuration schema.
        
        Args:
            name: Field name (dot notation supported, e.g., "brain.neuron_count")
            field_type: Expected type (int, float, str, bool, list, dict)
            default: Default value if not provided
            required: Whether the field is required
            validation: Optional validation function
            description: Human-readable description
            group: Configuration group (e.g., "brain", "plasticity", "neuromodulation")
            validation_message: Custom error message for validation failures
        """
        field_info = {
            'type': field_type,
            'default': default,
            'required': required,
            'validation': validation,
            'description': description,
            'group': group,
            'validation_message': validation_message,
        }
        
        self.fields[name] = field_info
        
        if group:
            if group not in self.groups:
                self.groups[group] = []
            self.groups[group].append(name)
    
    def validate_field(self, name: str, value: Any) -> List[str]:
        """
        Validate a field against the schema.
        
        Returns a list of error messages (empty if valid).
        """
        if name not in self.fields:
            return [f"Unknown configuration field: {name}"]
        
        field_info = self.fields[name]
        errors = []
        
        # Check if required
        if field_info['required'] and value is None:
            errors.append(f"Field '{name}' is required but not provided")
            return errors
        
        if value is None and not field_info['required']:
            return errors
        
        # Type validation
        expected_type = field_info['type']
        if not self._check_type(value, expected_type):
            errors.append(self._get_type_error(name, value, expected_type, field_info))
        
        # Custom validation
        if field_info['validation'] and not field_info['validation'](value):
            msg = field_info.get('validation_message', f"Validation failed for field '{name}'")
            errors.append(msg)
        
        return errors
    
    def _check_type(self, value: Any, expected_type: type) -> bool:
        """Check if value matches expected type."""
        if expected_type == Any:
            return True
        
        type_checks = {
            int: lambda x: isinstance(x, int) and not isinstance(x, bool),
            float: lambda x: isinstance(x, (int, float)),
            str: lambda x: isinstance(x, str),
            bool: lambda x: isinstance(x, bool),
            list: lambda x: isinstance(x, list),
            dict: lambda x: isinstance(x, dict),
        }
        
        if expected_type in type_checks:
            return type_checks[expected_type](value)
        
        return isinstance(value, expected_type)
    
    def _get_type_error(self, field: str, value: Any, expected_type: type, field_info: Dict) -> str:
        """Generate type error message."""
        actual_type = type(value).__name__
        expected_type_name = expected_type.__name__
        
        if field_info['type'] in [int, float]:
            if isinstance(value, str) and value.replace('.', '', 1).isdigit():
                return f"Field '{field}' should be numeric but is string"
        
        return f"Field '{field}' should be {expected_type_name}, got {actual_type}"
    
    def get_field_info(self, name: str) -> Dict[str, Any]:
        """Get field information from schema."""
        return self.fields.get(name, {}).copy()
    
    def get_groups(self) -> List[str]:
        """Get all configuration groups."""
        return list(self.groups.keys())
    
    def get_fields_in_group(self, group: str) -> List[str]:
        """Get all fields in a specific group."""
        return self.groups.get(group, []).copy()

class AdvancedConfigManager:
    """
    Advanced configuration manager with schema validation and inheritance.
    
    Features:
    - Schema-based validation
    - Configuration inheritance
    - Environment variable support
    - File-based configuration (JSON, YAML)
    - Command-line argument integration
    - Runtime configuration updates
    """
    
    def __init__(self, schema: Optional[ConfigurationSchema] = None):
        self._config: Dict[str, Any] = {}
        self._schema = schema or ConfigurationSchema()
        self._env_prefix = "NLM_"
        self._file_configs: List[str] = []
        self._logger = logging.getLogger(f"{__name__}.{self.__class__.__name__}")
        
        # Default configuration groups
        self._setup_default_schema()
    
    def _setup_default_schema(self):
        """Set up the default configuration schema."""
        # Brain configuration
        self._schema.add_field("brain.neuron_count", int, 1000, 
                              description="Total number of neurons in the brain", 
                              group="brain",
                              validation_message="Neuron count must be between 10 and 100000")
        
        self._schema.add_field("brain.synapse_density", float, 0.1,
                              description="Density of connections between neurons (0.0 to 1.0)",
                              group="brain",
                              validation=lambda x: 0.0 <= x <= 1.0)
        
        self._schema.add_field("brain.connection_probability", float, 0.1,
                              description="Probability of connection between any two neurons",
                              group="brain",
                              validation=lambda x: 0.0 <= x <= 1.0)
        
        self._schema.add_field("brain.initial_weight_mean", float, 0.5,
                              description="Mean of initial synaptic weights",
                              group="brain")
        
        self._schema.add_field("brain.initial_weight_std", float, 0.1,
                              description="Standard deviation of initial synaptic weights",
                              group="brain")
        
        # Plasticity configuration
        self._schema.add_field("plasticity.stdp.enable", bool, True,
                              description="Enable spike-timing dependent plasticity",
                              group="plasticity")
        
        self._schema.add_field("plasticity.stdp.learning_rate", float, 0.001,
                              description="STDP learning rate",
                              group="plasticity",
                              validation=lambda x: 0.0 <= x <= 1.0)
        
        self._schema.add_field("plasticity.stdp.tau_plus", float, 20.0,
                              description="STDP LTP time constant (ms)",
                              group="plasticity")
        
        self._schema.add_field("plasticity.stdp.tau_minus", float, 20.0,
                              description="STDP LTD time constant (ms)",
                              group="plasticity")
        
        self._schema.add_field("plasticity.hebbian.enable", bool, True,
                              description="Enable Hebbian learning",
                              group="plasticity")
        
        self._schema.add_field("plasticity.structural.enable", bool, True,
                              description="Enable structural plasticity (synaptogenesis/pruning)",
                              group="plasticity")
        
        self._schema.add_field("plasticity.structural.synaptogenesis_rate", float, 0.0001,
                              description="Rate of new synapse formation",
                              group="plasticity",
                              validation=lambda x: 0.0 <= x <= 1.0)
        
        self._schema.add_field("plasticity.structural.pruning_rate", float, 0.00001,
                              description="Rate of synapse removal",
                              group="plasticity",
                              validation=lambda x: 0.0 <= x <= 1.0)
        
        # Neuromodulation configuration
        self._schema.add_field("neuromod.dopamine.scale", float, 1.0,
                              description="Dopamine modulation scale factor",
                              group="neuromodulation")
        
        self._schema.add_field("neuromod.curiosity.enable", bool, True,
                              description="Enable curiosity-driven exploration",
                              group="neuromodulation")
        
        self._schema.add_field("neuromod.novelty.enable", bool, True,
                              description="Enable novelty detection",
                              group="neuromodulation")
        
        self._schema.add_field("neuromod.prediction_error.enable", bool, False,
                              description="Enable prediction error tracking",
                              group="neuromodulation")
        
        self._schema.add_field("neuromod.prediction_error.sensitivity", float, 0.3,
                              description="Sensitivity of prediction error detection",
                              group="neuromodulation",
                              validation=lambda x: 0.0 <= x <= 1.0)
        
        # Simulation configuration
        self._schema.add_field("simulation.timestep", float, 0.001,
                              description="Simulation timestep (seconds)",
                              group="simulation",
                              validation=lambda x: 0.0 < x <= 1.0)
        
        self._schema.add_field("simulation.max_steps", int, 10000,
                              description="Maximum simulation steps",
                              group="simulation",
                              validation=lambda x: x > 0)
        
        self._schema.add_field("simulation.random_seed", int, 42,
                              description="Random seed for reproducible results",
                              group="simulation")
        
        # Development configuration
        self._schema.add_field("development.initial_stage", str, "Initial",
                              description="Initial developmental stage",
                              group="development",
                              validation=lambda x: x in ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"])
        
        self._schema.add_field("development.plasticity_rate", float, 1.0,
                              description="Base plasticity rate that decreases with development",
                              group="development",
                              validation=lambda x: 0.0 <= x <= 2.0)
        
        # Memory configuration
        self._schema.add_field("memory.working_capacity", int, 1000,
                              description="Working memory capacity (number of traces)",
                              group="memory")
        
        self._schema.add_field("memory.episodic_max_episodes", int, 1000,
                              description="Maximum number of episodic memory episodes",
                              group="memory")
        
        self._schema.add_field("memory.associative_strength_threshold", float, 0.5,
                              description="Threshold for associative memory strength",
                              group="memory",
                              validation=lambda x: 0.0 <= x <= 1.0)
        
        # Environment configuration
        self._schema.add_field("environment.width", int, 20,
                              description="World width (units)",
                              group="environment")
        
        self._schema.add_field("environment.height", int, 20,
                              description="World height (units)",
                              group="environment")
        
        self._schema.add_field("environment.vision_width", int, 8,
                              description="Vision sensor width (cells)",
                              group="environment")
        
        self._schema.add_field("environment.vision_height", int, 8,
                              description="Vision sensor height (cells)",
                              group="environment")
        
        # Checkpoint configuration
        self._schema.add_field("checkpoint.directory", str, "./checkpoints",
                              description="Directory for checkpoint files",
                              group="checkpoint")
        
        self._schema.add_field("checkpoint.max_checkpoints", int, 10,
                              description="Maximum number of checkpoints to keep",
                              group="checkpoint")
        
        self._schema.add_field("checkpoint.save_interval", int, 1000,
                              description="Steps between automatic checkpoint saves",
                              group="checkpoint")
        
        self._schema.add_field("checkpoint.compress", bool, True,
                              description="Enable checkpoint compression",
                              group="checkpoint")
    
    def load_from_file(self, filepath: str, file_format: str = "auto") -> bool:
        """
        Load configuration from a file.
        
        Args:
            filepath: Path to configuration file
            file_format: File format ('json', 'yaml', 'auto', or 'env')
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if file_format == "auto":
                file_format = self._detect_file_format(filepath)
            
            if file_format == "env":
                return self._load_from_environment(filepath)
            
            with open(filepath, 'r') as f:
                if file_format == "json":
                    data = json.load(f)
                elif file_format == "yaml":
                    import yaml
                    data = yaml.safe_load(f)
                else:
                    raise ValueError(f"Unsupported file format: {file_format}")
            
            self._validate_and_apply_config(data, f"file: {filepath}")
            self._file_configs.append(filepath)
            
            self._logger.info(f"Loaded configuration from {filepath} ({file_format})")
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to load configuration from {filepath}: {e}")
            return False
    
    def _detect_file_format(self, filepath: str) -> str:
        """Detect file format from extension."""
        ext = Path(filepath).suffix.lower()
        
        if ext == '.json':
            return 'json'
        elif ext in ['.yaml', '.yml']:
            return 'yaml'
        elif ext == '.env':
            return 'env'
        elif ext == '.cfg':
            return 'env'  # Often environment variable files
        else:
            # Default to JSON
            return 'json'
    
    def _load_from_environment(self, filepath: str) -> bool:
        """Load configuration from environment variable file."""
        try:
            with open(filepath, 'r') as f:
                for line in f:
                    line = line.strip()
                    if not line or line.startswith('#') or line.startswith(';'):
                        continue
                    
                    if '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip().upper()
                        value = value.strip()
                        
                        # Convert value to appropriate type
                        if value.lower() in ['true', 'false']:
                            parsed_value = value.lower() == 'true'
                        elif value.isdigit():
                            parsed_value = int(value)
                        elif value.replace('.', '', 1).isdigit():
                            parsed_value = float(value)
                        else:
                            parsed_value = value
                        
                        self.set(key, parsed_value)
            
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to load environment configuration: {e}")
            return False
    
    def load_from_environment(self) -> bool:
        """Load configuration from environment variables."""
        success = True
        
        for key, value in os.environ.items():
            if key.startswith(self._env_prefix):
                config_key = key[len(self._env_prefix):].lower().replace('-', '_')
                
                try:
                    parsed_value = self._parse_env_value(value)
                    self.set(config_key, parsed_value)
                except Exception as e:
                    self._logger.warning(f"Failed to parse environment variable {key}: {e}")
                    success = False
        
        if success:
            self._logger.info("Loaded configuration from environment variables")
        
        return success
    
    def _parse_env_value(self, value: str) -> Any:
        """Parse environment variable value to appropriate type."""
        value = value.strip()
        
        if value.lower() in ['true', 'false']:
            return value.lower() == 'true'
        elif value.isdigit():
            return int(value)
        elif value.replace('.', '', 1).isdigit():
            return float(value)
        elif value.startswith('[') and value.endswith(']'):
            # Try to parse as JSON array
            try:
                return json.loads(value)
            except:
                pass
        elif value.startswith('{') and value.endswith('}'):
            # Try to parse as JSON object
            try:
                return json.loads(value)
            except:
                pass
        
        return value
    
    def load_from_preset(self, preset_name: str) -> bool:
        """
        Load a named preset configuration.
        
        Available presets: 'minimal', 'balanced', 'research', 'production'
        """
        presets = {
            'minimal': {
                'brain.neuron_count': 100,
                'brain.synapse_density': 0.05,
                'plasticity.stdp.enable': True,
                'neuromod.dopamine.scale': 0.5,
                'neuromod.curiosity.enable': False,
                'simulation.max_steps': 1000,
            },
            'balanced': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.001,
                'plasticity.hebbian.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.0,
                'simulation.max_steps': 10000,
            },
            'research': {
                'brain.neuron_count': 5000,
                'brain.synapse_density': 0.2,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.01,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.5,
                'neuromod.prediction_error.enable': True,
                'neuromod.prediction_error.sensitivity': 0.5,
                'simulation.max_steps': 100000,
            },
            'production': {
                'brain.neuron_count': 10000,
                'brain.synapse_density': 0.15,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.001,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': False,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.0,
                'neuromod.prediction_error.enable': True,
                'neuromod.prediction_error.sensitivity': 0.3,
                'simulation.timestep': 0.0005,
                'simulation.max_steps': 100000,
                'checkpoint.save_interval': 5000,
                'checkpoint.compress': True,
            }
        }
        
        if preset_name not in presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available: {list(presets.keys())}")
        
        self._validate_and_apply_config(presets[preset_name], f"preset: {preset_name}")
        self._logger.info(f"Loaded configuration preset: {preset_name}")
        
        return True
    
    def _validate_and_apply_config(self, config_data: Dict[str, Any], source: str):
        """Validate and apply configuration data."""
        errors = []
        
        for key, value in config_data.items():
            field_errors = self._schema.validate_field(key, value)
            if field_errors:
                errors.extend([f"{key}: {error}" for error in field_errors])
        
        if errors:
            error_msg = f"Configuration validation failed ({source}):\n" + "\n".join(errors)
            raise ValidationError(error_msg)
        
        # Apply validated configuration
        for key, value in config_data.items():
            self._config[key] = value
    
    def set(self, key: str, value: Any, source: str = "runtime"):
        """
        Set a configuration value.
        
        Args:
            key: Configuration key (dot notation supported)
            value: Configuration value
            source: Source of the configuration (for debugging)
        """
        # Validate the value
        errors = self._schema.validate_field(key, value)
        if errors:
            raise ValidationError(f"Cannot set '{key}': {'; '.join(errors)}", key)
        
        self._config[key] = value
        self._logger.debug(f"Set configuration '{key}' = {value} (source: {source})")
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value with type safety.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        if key in self._config:
            return self._config[key]
        elif default is not None:
            return default
        else:
            # Return schema default if available
            field_info = self._schema.get_field_info(key)
            if field_info and 'default' in field_info:
                return field_info['default']
            
            raise KeyError(f"Configuration key '{key}' not found")
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
        return key in self._config
    
    def remove(self, key: str):
        """Remove a configuration key."""
        if key in self._config:
            del self._config[key]
            self._logger.debug(f"Removed configuration key: {key}")
    
    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._config.keys())
    
    def groups(self) -> List[str]:
        """Get all configuration groups."""
        return self._schema.get_groups()
    
    def get_group(self, group: str) -> Dict[str, Any]:
        """Get all configuration values in a group."""
        group_fields = self._schema.get_fields_in_group(group)
        return {key: self._config[key] for key in group_fields if key in self._config}
    
    def validate(self) -> List[str]:
        """
        Validate current configuration.
        
        Returns a list of validation errors (empty if valid).
        """
        errors = []
        
        for key, value in self._config.items():
            field_errors = self._schema.validate_field(key, value)
            errors.extend([f"{key}: {error}" for error in field_errors])
        
        return errors
    
    def save(self, filepath: str, file_format: str = "json") -> bool:
        """
        Save configuration to a file.
        
        Args:
            filepath: Path to save configuration
            file_format: File format ('json', 'yaml', or 'env')
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if file_format == "json":
                with open(filepath, 'w') as f:
                    json.dump(self._config, f, indent=2)
            elif file_format == "yaml":
                import yaml
                with open(filepath, 'w') as f:
                    yaml.dump(self._config, f, default_flow_style=False)
            elif file_format == "env":
                with open(filepath, 'w') as f:
                    f.write("# Environment variable configuration\n")
                    f.write(f"# Generated on {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
                    
                    for key, value in self._config.items():
                        env_key = f"{self._env_prefix}{key.upper().replace('.', '_')}"
                        
                        if isinstance(value, bool):
                            env_value = 'true' if value else 'false'
                        elif isinstance(value, (int, float)):
                            env_value = str(value)
                        elif isinstance(value, str):
                            env_value = value
                        elif isinstance(value, list):
                            env_value = json.dumps(value)
                        elif isinstance(value, dict):
                            env_value = json.dumps(value)
                        else:
                            env_value = str(value)
                        
                        f.write(f"{env_key}={env_value}\n")
            else:
                raise ValueError(f"Unsupported file format: {file_format}")
            
            self._logger.info(f"Saved configuration to {filepath} ({file_format})")
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to save configuration to {filepath}: {e}")
            return False
    
    def merge(self, other_config: 'AdvancedConfigManager') -> bool:
        """
        Merge another configuration manager into this one.
        
        Args:
            other_config: Another AdvancedConfigManager instance
            
        Returns:
            True if merge was successful, False otherwise
        """
        try:
            # Validate other's configuration before merging
            other_errors = other_config.validate()
            if other_errors:
                raise ValidationError(f"Cannot merge: other configuration has errors: {other_errors}")
            
            # Merge configurations
            for key, value in other_config._config.items():
                # Validate before setting
                field_errors = self._schema.validate_field(key, value)
                if field_errors:
                    self._logger.warning(f"Skipping field '{key}' during merge: {field_errors}")
                else:
                    self._config[key] = value
            
            self._logger.info("Merged configurations")
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to merge configurations: {e}")
            return False
    
    def copy(self) -> 'AdvancedConfigManager':
        """Create a copy of this configuration manager."""
        new_config = AdvancedConfigManager(self._schema)
        new_config._config = self._config.copy()
        new_config._file_configs = self._file_configs.copy()
        return new_config
    
    def __repr__(self):
        return f"AdvancedConfigManager(keys={len(self._config)}, groups={len(self._schema.groups())})"
    
    def __str__(self):
        return f"AdvancedConfigManager with {len(self._config)} configuration keys"

# Configuration factory for easy creation of different configurations
class ConfigFactory:
    """
    Factory class for creating common configuration presets.
    
    Provides easy access to predefined configuration presets for different use cases:
    - minimal: Small, fast configurations for testing
    - balanced: General-purpose configurations for most applications
    - research: Large-scale configurations for research and experimentation
    - production: Optimized configurations for production environments
    - custom: Allow custom configuration creation
    """
    
    @staticmethod
    def create_preset(preset_name: str, customizations: Optional[Dict[str, Any]] = None) -> AdvancedConfigManager:
        """
        Create a configuration preset.
        
        Args:
            preset_name: Name of preset ('minimal', 'balanced', 'research', 'production', 'custom')
            customizations: Optional dictionary of customizations to apply
            
        Returns:
            Configured AdvancedConfigManager instance
        """
        config = AdvancedConfigManager()
        
        # Load the preset
        config.load_from_preset(preset_name)
        
        # Apply customizations if provided
        if customizations:
            config._validate_and_apply_config(customizations, "customizations")
        
        return config
    
    @staticmethod
    def create_for_testing() -> AdvancedConfigManager:
        """Create a configuration optimized for testing."""
        return ConfigFactory.create_preset('minimal')
    
    @staticmethod
    def create_for_production() -> AdvancedConfigManager:
        """Create a configuration optimized for production use."""
        return ConfigFactory.create_preset('production')
    
    @staticmethod
    def create_for_research() -> AdvancedConfigManager:
        """Create a configuration optimized for research."""
        return ConfigFactory.create_preset('research')
    
    @staticmethod
    def create_custom(**kwargs) -> AdvancedConfigManager:
        """
        Create a custom configuration with specified parameters.
        
        Args:
            **kwargs: Configuration parameters (dot notation keys)
            
        Returns:
            Configured AdvancedConfigManager instance
        """
        config = AdvancedConfigManager()
        
        # Build configuration from kwargs
        config_data = {}
        for key, value in kwargs.items():
            config_data[key] = value
        
        config._validate_and_apply_config(config_data, "custom")
        
        return config

# Example usage and demonstration
if __name__ == "__main__":
    # Example 1: Create a minimal configuration for testing
    print("Example 1: Creating minimal configuration for testing")
    test_config = ConfigFactory.create_for_testing()
    print(f"Neuron count: {test_config.get('brain.neuron_count')}")
    print(f"Synapse density: {test_config.get('brain.synapse_density')}")
    print()
    
    # Example 2: Create a production configuration
    print("Example 2: Creating production configuration")
    prod_config = ConfigFactory.create_for_production()
    print(f"Neuron count: {prod_config.get('brain.neuron_count')}")
    print(f"Synapse density: {prod_config.get('brain.synapse_density')}")
    print(f"Structural plasticity: {prod_config.get('plasticity.structural.enable')}")
    print()
    
    # Example 3: Create a custom configuration
    print("Example 3: Creating custom configuration")
    custom_config = ConfigFactory.create_custom(
        brain_neuron_count=2500,
        plasticity_stdp_learning_rate=0.005,
        neuromod_curiosity_enable=False,
        simulation_timestep=0.001,
        simulation_max_steps=50000,
        checkpoint_save_interval=2500,
        checkpoint_compress=True
    )
    print(f"Custom neuron count: {custom_config.get('brain.neuron_count')}")
    print(f"Custom learning rate: {custom_config.get('plasticity.stdp.learning_rate')}")
    print(f"Custom curiosity: {custom_config.get('neuromod.curiosity.enable')}")
    print()
    
    # Example 4: Configuration validation
    print("Example 4: Configuration validation")
    validation_errors = prod_config.validate()
    if validation_errors:
        print("Validation errors:")
        for error in validation_errors:
            print(f"  - {error}")
    else:
        print("Configuration is valid!")
    print()
    
    # Example 5: Save configuration to file
    print("Example 5: Saving configuration to file")
    prod_config.save("production_config.json", "json")
    print("Configuration saved to production_config.json")
    print()
    
    # Example 6: Load configuration from file
    print("Example 6: Loading configuration from file")
    loaded_config = AdvancedConfigManager()
    loaded_config.load_from_file("production_config.json", "json")
    print(f"Loaded neuron count: {loaded_config.get('brain.neuron_count')}")
    print()
    
    # Example 7: Environment variable integration
    print("Example 7: Environment variable integration")
    env_config = AdvancedConfigManager()
    env_config.load_from_environment()
    print(f"Configuration from environment: {env_config.get('brain.neuron_count', 'not set')}")
    print()
    
    # Example 8: Configuration groups
    print("Example 8: Configuration groups")
    config = AdvancedConfigManager()
    print("Available groups:", config.groups())
    
    brain_config = config.get_group("brain")
    print("Brain configuration:", brain_config)
    
    plasticity_config = config.get_group("plasticity")
    print("Plasticity configuration:", plasticity_config)
    print()
    
    print("All examples completed successfully!")
