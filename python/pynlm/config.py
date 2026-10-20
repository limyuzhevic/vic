"""
NLM Configuration Management

Advanced configuration system for expert users with validation, inheritance,
and environment-specific settings.
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, Any, Optional, Union, List
from enum import Enum

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

from .core import Config

class ConfigFormat(Enum):
    JSON = "json"
    YAML = "yaml"
    INI = "ini"
    TOML = "toml"

class ConfigManager:
    def __init__(self, project_dir: str = "."):
        self.project_dir = Path(project_dir)
        self.config_dir = self.project_dir / "configs"
        self.config_dir.mkdir(exist_ok=True)
        
        # Default configurations
        self.defaults = {
            "simulation": {
                "timestep": 0.001,
                "max_steps": 100000,
                "random_seed": 42,
            },
            "environment": {
                "width": 10,
                "height": 10,
                "vision_width": 8,
                "vision_height": 8,
            },
            "brain": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
            },
            "neuromodulation": {
                "dopamine_baseline": 0.1,
                "curiosity_threshold": 0.5,
            },
            "learning": {
                "plasticity_rate": 0.01,
                "memory_consolidation": True,
            }
        }
    
    def create_config(self, name: str = "default", profile: str = "default") -> Config:
        """Create a configuration with specified profile."""
        config = Config()
        
        # Load default values
        profile_config = self.defaults.get(profile, {})
        for section, values in profile_config.items():
            for key, value in values.items():
                config.set(f"{section}.{key}", value)
        
        # Load from file if exists
        config_path = self.config_dir / f"{name}.json"
        if config_path.exists():
            config.loadFromFile(str(config_path))
        
        return config
    
    def validate_config(self, config: Config) -> List[str]:
        """Validate configuration and return list of errors."""
        errors = []
        
        # Check required sections
        required_sections = ["simulation", "brain"]
        for section in required_sections:
            if not self._section_exists(config, section):
                errors.append(f"Missing required section: {section}")
        
        # Check specific values
        if config.has("simulation.timestep"):
            timestep = config.getOr("simulation.timestep", 0.0)
            if timestep <= 0:
                errors.append("Simulation timestep must be positive")
        
        if config.has("brain.neuron_count"):
            neuron_count = config.getOr("brain.neuron_count", 0)
            if neuron_count <= 0:
                errors.append("Neuron count must be positive")
        
        return errors
    
    def merge_configs(self, base_config: Config, override_config: Config) -> Config:
        """Merge two configurations, with override taking precedence."""
        merged = Config()
        
        # Copy all keys from base
        for key in base_config.getKeys():
            value = base_config.get(key)
            if value is not None:
                merged.set(key, value)
        
        # Override with values from override
        for key in override_config.getKeys():
            value = override_config.get(key)
            if value is not None:
                merged.set(key, value)
        
        return merged
    
    def export_config(self, config: Config, format: ConfigFormat = ConfigFormat.JSON) -> str:
        """Export configuration to string in specified format."""
        if format == ConfigFormat.JSON:
            return self._to_json(config)
        elif format == ConfigFormat.YAML:
            return self._to_yaml(config)
        elif format == ConfigFormat.INI:
            return self._to_ini(config)
        else:  # TOML
            return self._to_toml(config)
    
    def import_config(self, config_str: str, format: ConfigFormat = ConfigFormat.JSON) -> Config:
        """Import configuration from string in specified format."""
        config = Config()
        
        if format == ConfigFormat.JSON:
            self._from_json(config_str, config)
        elif format == ConfigFormat.YAML:
            self._from_yaml(config_str, config)
        elif format == ConfigFormat.INI:
            self._from_ini(config_str, config)
        else:  # TOML
            self._from_toml(config_str, config)
        
        return config
    
    def _section_exists(self, config: Config, section: str) -> bool:
        """Check if a section exists in config."""
        for key in config.getKeys():
            if key.startswith(f"{section}.") or key == section:
                return True
        return False
    
    def _to_json(self, config: Config) -> str:
        """Convert config to JSON string."""
        data = {}
        for key in config.getKeys():
            value = config.get(key)
            if value is not None:
                self._set_nested_value(data, key, value)
        return json.dumps(data, indent=2)
    
    def _from_json(self, json_str: str, config: Config):
        """Import config from JSON string."""
        try:
            data = json.loads(json_str)
            self._flatten_dict(data, lambda k, v: config.set(k, v))
        except json.JSONDecodeError as e:
            raise ValueError(f"Invalid JSON: {e}")
    
    def _to_yaml(self, config: Config) -> str:
        """Convert config to YAML string (placeholder - requires pyyaml)."""
        try:
            import yaml
            data = {}
            for key in config.getKeys():
                value = config.get(key)
                if value is not None:
                    self._set_nested_value(data, key, value)
            return yaml.dump(data, default_flow_style=False)
        except ImportError:
            raise ImportError("YAML support requires pyyaml package")
    
    def _from_yaml(self, yaml_str: str, config: Config):
        """Import config from YAML string (placeholder)."""
        try:
            import yaml
            data = yaml.safe_load(yaml_str)
            self._flatten_dict(data, lambda k, v: config.set(k, v))
        except ImportError:
            raise ImportError("YAML support requires pyyaml package")
        except yaml.YAMLError as e:
            raise ValueError(f"Invalid YAML: {e}")
    
    def _to_ini(self, config: Config) -> str:
        """Convert config to INI string (placeholder - requires configparser)."""
        try:
            import configparser
            cp = configparser.ConfigParser()
            
            # Group keys by section
            sections = {}
            for key in config.getKeys():
                if '.' in key:
                    section, subkey = key.split('.', 1)
                    if section not in sections:
                        sections[section] = {}
                    sections[section][subkey] = config.get(key)
                else:
                    # Top-level key
                    if 'DEFAULT' not in sections:
                        sections['DEFAULT'] = {}
                    sections['DEFAULT'][key] = config.get(key)
            
            # Write to INI
            cp.read_dict(sections)
            
            import io
            output = io.StringIO()
            cp.write(output)
            return output.getvalue()
        except ImportError:
            raise ImportError("INI support requires configparser (built-in)")
    
    def _from_ini(self, ini_str: str, config: Config):
        """Import config from INI string."""
        try:
            import configparser
            cp = configparser.ConfigParser()
            cp.read_string(ini_str)
            
            for section in cp.sections():
                for key, value in cp.items(section):
                    config.set(f"{section}.{key}", value)
            
            # Also handle DEFAULT section
            if 'DEFAULT' in cp:
                for key, value in cp['DEFAULT'].items():
                    config.set(key, value)
        except ImportError:
            raise ImportError("INI support requires configparser (built-in)")
        except configparser.Error as e:
            raise ValueError(f"Invalid INI: {e}")
    
    def _to_toml(self, config: Config) -> str:
        """Convert config to TOML string (placeholder - requires toml)."""
        try:
            import toml
            data = {}
            for key in config.getKeys():
                value = config.get(key)
                if value is not None:
                    self._set_nested_value(data, key, value)
            return toml.dumps(data)
        except ImportError:
            raise ImportError("TOML support requires toml package")
    
    def _from_toml(self, toml_str: str, config: Config):
        """Import config from TOML string (placeholder)."""
        try:
            import toml
            data = toml.loads(toml_str)
            self._flatten_dict(data, lambda k, v: config.set(k, v))
        except ImportError:
            raise ImportError("TOML support requires toml package")
        except toml.TomlDecodeError as e:
            raise ValueError(f"Invalid TOML: {e}")
    
    def _set_nested_value(self, data: Dict, key: str, value):
        """Set nested value in dictionary using dot notation."""
        parts = key.split('.')
        current = data
        for part in parts[:-1]:
            if part not in current:
                current[part] = {}
            current = current[part]
        current[parts[-1]] = value
    
    def _flatten_dict(self, data: Dict, setter_func):
        """Flatten nested dictionary and apply setter function."""
        def _flatten(d, prefix=""):
            for key, value in d.items():
                full_key = f"{prefix}.{key}" if prefix else key
                if isinstance(value, dict):
                    _flatten(value, full_key)
                else:
                    setter_func(full_key, value)
        
        _flatten(data)
class AdvancedConfig:
    def __init__(self, base_config: Config):
        self.base_config = base_config
        self._validate_config()
    
    def _validate_config(self):
        """Validate configuration and apply fixes."""
        # Expert-level validation and fixes
        if self.base_config.has("simulation.timestep"):
            timestep = self.base_config.getOr("simulation.timestep", 0.001)
            if timestep < 0.0001:
                self.base_config.set("simulation.timestep", 0.0001)
            elif timestep > 1.0:
                self.base_config.set("simulation.timestep", 1.0)
    
    def get_simulation_params(self) -> Dict[str, Any]:
        """Get all simulation parameters as a dictionary."""
        params = {}
        for key in self.base_config.getKeys():
            if key.startswith("simulation."):
                subkey = key[11:]  # Remove "simulation." prefix
                value = self.base_config.get(key)
                if value is not None:
                    params[subkey] = value
        return params
    
    def get_brain_params(self) -> Dict[str, Any]:
        """Get all brain parameters as a dictionary."""
        params = {}
        for key in self.base_config.getKeys():
            if key.startswith("brain."):
                subkey = key[6:]  # Remove "brain." prefix
                value = self.base_config.get(key)
                if value is not None:
                    params[subkey] = value
        return params
    
    def get_neuromodulation_params(self) -> Dict[str, Any]:
        """Get all neuromodulation parameters as a dictionary."""
        params = {}
        for key in self.base_config.getKeys():
            if key.startswith("neuromodulation."):
                subkey = key[16:]  # Remove "neuromodulation." prefix
                value = self.base_config.get(key)
                if value is not None:
                    params[subkey] = value
        return params
    
    def set_advanced_option(self, option: str, value: Any):
        """Set an advanced configuration option with validation."""
        # Validate based on option type
        if option.startswith("simulation."):
            self._validate_simulation_option(option, value)
        elif option.startswith("brain."):
            self._validate_brain_option(option, value)
        
        self.base_config.set(option, value)
    
    def _validate_simulation_option(self, option: str, value: Any):
        """Validate simulation-specific options."""
        if option == "simulation.timestep":
            if isinstance(value, (int, float)) and value <= 0:
                raise ValueError("Simulation timestep must be positive")
        
        elif option == "simulation.max_steps":
            if isinstance(value, int) and value <= 0:
                raise ValueError("Maximum simulation steps must be positive")
    
    def _validate_brain_option(self, option: str, value: Any):
        """Validate brain-specific options."""
        if option == "brain.neuron_count":
            if isinstance(value, int) and value <= 0:
                raise ValueError("Neuron count must be positive")
            if isinstance(value, int) and value > 100000:
                print("Warning: Large neuron count may impact performance")
    
    def export_profile(self) -> str:
        """Export configuration as a profile string."""
        import base64
        import json
        
        data = {}
        for key in self.base_config.getKeys():
            value = self.base_config.get(key)
            if value is not None:
                self._set_nested_value(data, key, value)
        
        json_str = json.dumps(data, indent=2)
        return base64.b64encode(json_str.encode()).decode()
    
    def import_profile(self, profile_str: str):
        """Import configuration from a profile string."""
        import base64
        import json
        
        try:
            json_str = base64.b64decode(profile_str.encode()).decode()
            data = json.loads(json_str)
            self._flatten_dict(data, lambda k, v: self.base_config.set(k, v))
        except Exception as e:
            raise ValueError(f"Invalid profile string: {e}")
    
    def _set_nested_value(self, data: Dict, key: str, value):
        """Set nested value in dictionary using dot notation."""
        parts = key.split('.')
        current = data
        for part in parts[:-1]:
            if part not in current:
                current[part] = {}
            current = current[part]
        current[parts[-1]] = value
    
    def _flatten_dict(self, data: Dict, setter_func):
        """Flatten nested dictionary and apply setter function."""
        def _flatten(d, prefix=""):
            for key, value in d.items():
                full_key = f"{prefix}.{key}" if prefix else key
                if isinstance(value, dict):
                    _flatten(value, full_key)
                else:
                    setter_func(full_key, value)
        
        _flatten(data)
# Global configuration manager instance
_global_config_manager: Optional[ConfigManager] = None


def get_config_manager() -> ConfigManager:
    """Get the global configuration manager instance."""
    global _global_config_manager
    if _global_config_manager is None:
        _global_config_manager = ConfigManager()
    return _global_config_manager


def set_config_manager(manager: ConfigManager):
    """Set the global configuration manager instance."""
    global _global_config_manager
    _global_config_manager = manager


def create_config(name: str = "default", profile: str = "default") -> Config:
    """Create a configuration with the specified name and profile."""
    return get_config_manager().create_config(name, profile)


def validate_config(config: Config) -> List[str]:
    """Validate a configuration and return list of errors."""
    return get_config_manager().validate_config(config)


def merge_configs(base_config: Config, override_config: Config) -> Config:
    """Merge two configurations."""
    return get_config_manager().merge_configs(base_config, override_config)


def export_config(config: Config, format: ConfigFormat = ConfigFormat.JSON) -> str:
    """Export configuration to string."""
    return get_config_manager().export_config(config, format)


def import_config(config_str: str, format: ConfigFormat = ConfigFormat.JSON) -> Config:
    """Import configuration from string."""
    return get_config_manager().import_config(config_str, format)