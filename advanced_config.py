#!/usr/bin/env python3
"""
Advanced configuration management for NLM Python bindings.

This module provides enhanced configuration capabilities including:
- Advanced configuration presets
- Configuration comparison and diffing
- Configuration inheritance and merging
- Configuration validation rules
- Advanced configuration management
"""

import json
import tempfile
import os
from typing import Dict, List, Any, Optional, Union
import pynlm

class AdvancedConfig:
    """
    Advanced configuration manager for NLM with enhanced features.
    
    This class provides advanced configuration management capabilities beyond
    the basic Config class, including configuration presets, validation rules,
    configuration comparison, and inheritance.
    """
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        """
        Initialize advanced configuration manager.
        
        Args:
            config: Optional existing NLM config to wrap
        """
        if config is None:
            self.config = pynlm.createDefaultConfig()
        else:
            self.config = config
        
        # Configuration validation rules
        self.validation_rules: Dict[str, Dict[str, Any]] = {}
        
        # Configuration inheritance
        self.parent_config: Optional['AdvancedConfig'] = None
        
        # Configuration metadata
        self.metadata: Dict[str, Any] = {
            'created': 0.0,
            'modified': 0.0,
            'version': '1.0',
            'description': ''
        }
        
        # Advanced configuration presets
        self.presets = self._initialize_presets()
    
    def _initialize_presets(self) -> Dict[str, Dict[str, Any]]:
        """Initialize built-in configuration presets."""
        return {
            'development': {
                'description': 'Development configuration for testing and debugging',
                'settings': {
                    'brain.neuron_count': 100,
                    'brain.synapse_density': 0.05,
                    'plasticity.stdp.enable': True,
                    'plasticity.hebbian.enable': True,
                    'plasticity.structural.enable': True,
                    'neuromod.dopamine.scale': 1.0,
                    'neuromod.curiosity.enable': True,
                    'neuromod.novelty.enable': True,
                    'brain.v_thresh': -40.0,
                    'brain.v_rest': -60.0,
                    'brain.tau_mem': 30.0,
                }
            },
            'production': {
                'description': 'Production configuration for real simulations',
                'settings': {
                    'brain.neuron_count': 1000,
                    'brain.synapse_density': 0.1,
                    'plasticity.stdp.enable': True,
                    'plasticity.hebbian.enable': False,
                    'plasticity.structural.enable': False,
                    'neuromod.dopamine.scale': 2.0,
                    'neuromod.curiosity.enable': False,
                    'neuromod.novelty.enable': False,
                    'brain.v_thresh': -50.0,
                    'brain.v_rest': -70.0,
                    'brain.tau_mem': 20.0,
                    'brain.tau_ref': 5.0,
                }
            },
            'research': {
                'description': 'Research configuration for scientific experiments',
                'settings': {
                    'brain.neuron_count': 5000,
                    'brain.synapse_density': 0.15,
                    'plasticity.stdp.enable': True,
                    'plasticity.hebbian.enable': True,
                    'plasticity.structural.enable': True,
                    'neuromod.dopamine.scale': 1.5,
                    'neuromod.curiosity.enable': True,
                    'neuromod.novelty.enable': True,
                    'brain.development.enabled': True,
                    'brain.development.stage_duration': 600.0,
                    'plasticity.stdp.tau_plus': 30.0,
                    'plasticity.stdp.tau_minus': 25.0,
                }
            },
            'minimal': {
                'description': 'Minimal configuration for fastest startup',
                'settings': {
                    'brain.neuron_count': 50,
                    'brain.synapse_density': 0.01,
                    'plasticity.stdp.enable': False,
                    'plasticity.hebbian.enable': False,
                    'plasticity.structural.enable': False,
                    'neuromod.dopamine.scale': 0.5,
                    'neuromod.curiosity.enable': False,
                    'neuromod.novelty.enable': False,
                    'brain.development.enabled': False,
                }
            }
        }
    
    def apply_preset(self, preset_name: str) -> bool:
        """
        Apply a predefined configuration preset.
        
        Args:
            preset_name: Name of the preset to apply
            
        Returns:
            True if preset was applied successfully, False otherwise
            
        Raises:
            ValueError: If preset name is not recognized
        """
        if preset_name not in self.presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available: {list(self.presets.keys())}")
        
        preset = self.presets[preset_name]
        print(f"Applying preset '{preset_name}': {preset['description']}")
        
        for key, value in preset['settings'].items():
            if isinstance(value, int):
                self.config.set_int(key, value)
            elif isinstance(value, float):
                self.config.set_float(key, value)
            elif isinstance(value, str):
                self.config.set_string(key, value)
            elif isinstance(value, bool):
                self.config.set_bool(key, value)
        
        self.metadata['modified'] = self.metadata['created']  # Simplified timestamp
        return True
    
    def get_preset_info(self, preset_name: str) -> Dict[str, Any]:
        """
        Get information about a specific preset.
        
        Args:
            preset_name: Name of the preset
            
        Returns:
            Dictionary with preset information
            
        Raises:
            ValueError: If preset name is not recognized
        """
        if preset_name not in self.presets:
            raise ValueError(f"Unknown preset: {preset_name}")
        
        return {
            'name': preset_name,
            'description': self.presets[preset_name]['description'],
            'settings_count': len(self.presets[preset_name]['settings'])
        }
    
    def list_presets(self) -> List[Dict[str, Any]]:
        """
        List all available presets.
        
        Returns:
            List of preset information dictionaries
        """
        return [self.get_preset_info(name) for name in self.presets.keys()]
    
    def add_validation_rule(self, key: str, rule_type: str, value: Any, description: str = ""):
        """
        Add a validation rule for a configuration key.
        
        Args:
            key: Configuration key to validate
            rule_type: Type of validation ('range', 'choice', 'regex', 'custom')
            value: Validation value (depends on rule_type)
            description: Description of the validation rule
        """
        if rule_type not in ['range', 'choice', 'regex', 'custom']:
            raise ValueError(f"Invalid rule type: {rule_type}")
        
        self.validation_rules[key] = {
            'type': rule_type,
            'value': value,
            'description': description
        }
    
    def validate_with_rules(self) -> List[str]:
        """
        Validate configuration against all rules.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []
        
        for key, rule in self.validation_rules.items():
            if not self.config.has(key):
                errors.append(f"Missing required key: {key}")
                continue
            
            value = self._get_config_value(key)
            
            if rule['type'] == 'range':
                min_val, max_val = rule['value']
                if not (min_val <= value <= max_val):
                    errors.append(f"Key '{key}' value {value} outside range [{min_val}, {max_val}]")
            
            elif rule['type'] == 'choice':
                if value not in rule['value']:
                    errors.append(f"Key '{key}' value '{value}' not in allowed choices: {rule['value']}")
            
            elif rule['type'] == 'regex':
                import re
                if not re.match(rule['value'], str(value)):
                    errors.append(f"Key '{key}' value '{value}' does not match regex pattern")
        
        return errors
    
    def merge_config(self, other_config: 'AdvancedConfig', strategy: str = 'override'):
        """
        Merge with another configuration.
        
        Args:
            other_config: Configuration to merge with
            strategy: Merge strategy ('override', 'inherit', 'combine')
        """
        if strategy == 'override':
            # Override with other's values
            for key in other_config.config.getKeys():
                if other_config.config.has(key):
                    self._set_config_value(key, self._get_config_value(key, other_config))
        
        elif strategy == 'inherit':
            # Inherit missing values from other
            for key in other_config.config.getKeys():
                if not self.config.has(key) and other_config.config.has(key):
                    self._set_config_value(key, other_config.config.get_keys(key))
        
        elif strategy == 'combine':
            # Combine non-conflicting values
            for key in other_config.config.getKeys():
                if not self.config.has(key) and other_config.config.has(key):
                    self._set_config_value(key, other_config.config.get_keys(key))
        
        self.metadata['modified'] = self.metadata['created']
    
    def get_config_diff(self, other_config: 'AdvancedConfig') -> Dict[str, Any]:
        """
        Get differences between this configuration and another.
        
        Args:
            other_config: Configuration to compare with
            
        Returns:
            Dictionary with differences
        """
        diff = {
            'added': {},
            'removed': {},
            'changed': {},
            'identical': True
        }
        
        self_keys = set(self.config.getKeys())
        other_keys = set(other_config.config.getKeys())
        
        diff['added'] = {k: self._get_config_value(k) for k in self_keys - other_keys}
        diff['removed'] = {k: other_config.config.get_int(k) for k in other_keys - self_keys}
        
        for key in self_keys & other_keys:
            self_val = self._get_config_value(key)
            other_val = other_config.config.get_int(key)
            
            if self_val != other_val:
                diff['changed'][key] = {
                    'from': other_val,
                    'to': self_val
                }
                diff['identical'] = False
        
        return diff
    
    def export_config(self, format_type: str = 'json', filepath: Optional[str] = None) -> Any:
        """
        Export configuration to file or return as data.
        
        Args:
            format_type: Export format ('json', 'python', 'ini')
            filepath: Optional file path to save to
            
        Returns:
            Exported configuration data or file path
        """
        export_data = {
            'config': {},
            'metadata': self.metadata,
            'validation_rules': self.validation_rules
        }
        
        # Export configuration values
        for key in self.config.getKeys():
            export_data['config'][key] = self._get_config_value(key)
        
        if filepath:
            if format_type == 'json':
                with open(filepath, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format_type == 'python':
                # Generate Python code
                code = f"# Configuration exported from NLM AdvancedConfig\n"
                code += f"config = {export_data['config']}\n"
                with open(filepath, 'w') as f:
                    f.write(code)
            elif format_type == 'ini':
                # Generate INI-style format
                from configparser import ConfigParser
                cp = ConfigParser()
                cp['DEFAULT'] = export_data['config']
                with open(filepath, 'w') as f:
                    cp.write(f)
            
            return filepath
        
        return export_data
    
    def import_config(self, data: Any, format_type: str = 'json'):
        """
        Import configuration from data.
        
        Args:
            data: Configuration data
            format_type: Format of the data ('json', 'python', 'ini')
        """
        if format_type == 'json':
            self.config = pynlm.createDefaultConfig()
            if isinstance(data, dict) and 'config' in data:
                # Import from exported format
                for key, value in data['config'].items():
                    if isinstance(value, int):
                        self.config.set_int(key, value)
                    elif isinstance(value, float):
                        self.config.set_float(key, value)
                    elif isinstance(value, str):
                        self.config.set_string(key, value)
                    elif isinstance(value, bool):
                        self.config.set_bool(key, value)
            elif isinstance(data, dict):
                # Direct config dict
                for key, value in data.items():
                    if isinstance(value, int):
                        self.config.set_int(key, value)
                    elif isinstance(value, float):
                        self.config.set_float(key, value)
                    elif isinstance(value, str):
                        self.config.set_string(key, value)
                    elif isinstance(value, bool):
                        self.config.set_bool(key, value)
        
        elif format_type == 'python':
            # Execute Python code to get config dict
            import types
            namespace = {}
            exec(data, namespace)
            self.config = namespace.get('config', pynlm.createDefaultConfig())
        
        self.metadata['modified'] = self.metadata['created']
    
    def duplicate(self) -> 'AdvancedConfig':
        """
        Create a duplicate of this configuration.
        
        Returns:
            Duplicate AdvancedConfig instance
        """
        duplicate = AdvancedConfig()
        duplicate.metadata = self.metadata.copy()
        duplicate.validation_rules = {k: v.copy() for k, v in self.validation_rules.items()}
        
        # Copy configuration values
        for key in self.config.getKeys():
            value = self._get_config_value(key)
            if isinstance(value, int):
                duplicate.config.set_int(key, value)
            elif isinstance(value, float):
                duplicate.config.set_float(key, value)
            elif isinstance(value, str):
                duplicate.config.set_string(key, value)
            elif isinstance(value, bool):
                duplicate.config.set_bool(key, value)
        
        return duplicate
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get configuration statistics.
        
        Returns:
            Dictionary with configuration statistics
        """
        keys = self.config.getKeys()
        
        return {
            'total_keys': len(keys),
            'int_keys': sum(1 for k in keys if isinstance(self._get_config_value(k), int)),
            'float_keys': sum(1 for k in keys if isinstance(self._get_config_value(k), float)),
            'string_keys': sum(1 for k in keys if isinstance(self._get_config_value(k), str)),
            'bool_keys': sum(1 for k in keys if isinstance(self._get_config_value(k), bool)),
            'validation_rules_count': len(self.validation_rules),
            'has_parent': self.parent_config is not None
        }
    
    def _get_config_value(self, key: str, fallback_config: Optional['AdvancedConfig'] = None) -> Any:
        """
        Get configuration value with type handling.
        
        Args:
            key: Configuration key
            fallback_config: Fallback configuration
            
        Returns:
            Configuration value
        """
        if fallback_config and not self.config.has(key):
            fallback_config = self.parent_config
        
        if self.config.has(key):
            if isinstance(self._get_config_value(key), int):
                return self.config.get_int(key, 0)
            elif isinstance(self._get_config_value(key), float):
                return self.config.get_float(key, 0.0)
            elif isinstance(self._get_config_value(key), str):
                return self.config.get_string(key, "")
            elif isinstance(self._get_config_value(key), bool):
                return self.config.get_bool(key, False)
        
        return None
    
    def _set_config_value(self, key: str, value: Any):
        """
        Set configuration value with type handling.
        
        Args:
            key: Configuration key
            value: Value to set
        """
        if isinstance(value, int):
            self.config.set_int(key, value)
        elif isinstance(value, float):
            self.config.set_float(key, value)
        elif isinstance(value, str):
            self.config.set_string(key, value)
        elif isinstance(value, bool):
            self.config.set_bool(key, value)
class ConfigManager:
    """
    Global configuration manager for NLM.
    
    This class provides a centralized configuration management system
    that can be used across different parts of the application.
    """
    
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._initialized = False
        return cls._instance
    
    def __init__(self):
        if not self._initialized:
            self._configs: Dict[str, AdvancedConfig] = {}
            self._initialized = True
    
    def create_config(self, name: str, preset: Optional[str] = None) -> AdvancedConfig:
        """
        Create a new configuration with optional preset.
        
        Args:
            name: Configuration name
            preset: Preset to apply (optional)
            
        Returns:
            AdvancedConfig instance
        """
        if name in self._configs:
            raise ValueError(f"Configuration '{name}' already exists")
        
        config = AdvancedConfig()
        
        if preset:
            config.apply_preset(preset)
        
        self._configs[name] = config
        return config
    
    def get_config(self, name: str) -> AdvancedConfig:
        """
        Get a configuration by name.
        
        Args:
            name: Configuration name
            
        Returns:
            AdvancedConfig instance
            
        Raises:
            KeyError: If configuration not found
        """
        if name not in self._configs:
            raise KeyError(f"Configuration '{name}' not found")
        return self._configs[name]
    
    def list_configs(self) -> List[str]:
        """
        List all configuration names.
        
        Returns:
            List of configuration names
        """
        return list(self._configs.keys())
    
    def delete_config(self, name: str):
        """
        Delete a configuration.
        
        Args:
            name: Configuration name
        """
        if name in self._configs:
            del self._configs[name]
    
    def export_all_configs(self, directory: str = ".", format_type: str = 'json'):
        """
        Export all configurations to files.
        
        Args:
            directory: Directory to export to
            format_type: Export format
        """
        os.makedirs(directory, exist_ok=True)
        
        for name, config in self._configs.items():
            filepath = os.path.join(directory, f"{name}.{format_type}")
            config.export_config(format_type, filepath)
    
    def import_config_from_file(self, filepath: str, name: Optional[str] = None, format_type: str = 'json'):
        """
        Import configuration from file.
        
        Args:
            filepath: File path to import from
            name: Configuration name (optional, will be derived from filename if not provided)
            format_type: Import format
        """
        if name is None:
            name = os.path.splitext(os.path.basename(filepath))[0]
        
        with open(filepath, 'r') as f:
            if format_type == 'json':
                data = json.load(f)
            elif format_type == 'python':
                # For Python files, execute them to get the config dict
                namespace = {}
                exec(f.read(), namespace)
                data = namespace.get('config', {})
            else:
                raise ValueError(f"Unsupported import format: {format_type}")
        
        config = AdvancedConfig()
        config.import_config(data, format_type)
        self._configs[name] = config
# Global configuration manager instance
config_manager = ConfigManager()
def get_config_manager() -> ConfigManager:
    """Get the global configuration manager instance."""
    return config_manager
def create_advanced_config(name: str, preset: Optional[str] = None) -> AdvancedConfig:
    """
    Convenience function to create an advanced configuration.
    
    Args:
        name: Configuration name
        preset: Preset to apply (optional)
        
    Returns:
        AdvancedConfig instance
    """
    return config_manager.create_config(name, preset)
def get_advanced_config(name: str) -> AdvancedConfig:
    """
    Convenience function to get an advanced configuration.
    
    Args:
        name: Configuration name
        
    Returns:
        AdvancedConfig instance
        
    Raises:
        KeyError: If configuration not found
    """
    return config_manager.get_config(name)
# Example usage and demonstration
if __name__ == "__main__":
    print("=== NLM Advanced Configuration Demo ===\n")
    
    # Create default config
    config1 = AdvancedConfig()
    config1.apply_preset('development')
    print(f"1. Applied 'development' preset")
    
    # Create config with different preset
    config2 = AdvancedConfig()
    config2.apply_preset('production')
    print(f"2. Applied 'production' preset")
    
    # Create config manager and use it
    manager = get_config_manager()
    manager.create_config('dev_env', 'development')
    manager.create_config('prod_env', 'production')
    
    print(f"\n3. Available configs: {manager.list_configs()}")
    
    # Compare configs
    dev_config = manager.get_config('dev_env')
    prod_config = manager.get_config('prod_env')
    
    diff = dev_config.get_config_diff(prod_config)
    print(f"\n4. Config differences:")
    print(f"   Added keys: {list(diff['added'].keys())}")
    print(f"   Changed keys: {list(diff['changed'].keys())}")
    
    # Export and import
    dev_config.export_config('json', 'dev_config.json')
    print(f"\n5. Exported dev config to dev_config.json")
    
    # Import back
    imported_config = AdvancedConfig()
    with open('dev_config.json', 'r') as f:
        imported_data = json.load(f)
    imported_config.import_config(imported_data, 'json')
    
    print(f"6. Imported config keys: {len(imported_config.config.getKeys())}")
    
    # Validation
    dev_config.add_validation_rule('brain.neuron_count', 'range', (50, 1000))
    dev_config.add_validation_rule('brain.synapse_density', 'choice', 
                                   [0.01, 0.05, 0.1, 0.15])
    
    errors = dev_config.validate_with_rules()
    print(f"\n7. Validation errors: {len(errors)}")
    
    # Stats
    stats = dev_config.get_stats()
    print(f"8. Config stats: {stats}")
    
    print("\n=== Demo Complete ===")
