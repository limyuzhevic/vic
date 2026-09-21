"""
NLM Configuration System - Advanced Configuration Management

This module provides enhanced configuration capabilities for the NLM system,
building on the basic Config class with additional features and convenience
functions for advanced users and automation.
"""

import json
import yaml
import os
import sys
from pathlib import Path
from typing import Dict, Any, Optional, Union, List
from enum import Enum
import logging

# Configure logging
logger = logging.getLogger(__name__)

class ConfigSource(Enum):
    """Configuration source enumeration"""
    DEFAULT = 0
    CONFIG_FILE = 1
    COMMAND_LINE = 2
    ENVIRONMENT = 3
    PROGRAMMATIC = 4

class NLMConfigManager:
    """
    Advanced configuration manager for NLM with support for multiple file formats,
    environment variables, and sophisticated configuration validation.
    """
    
    def __init__(self, config_file: Optional[str] = None, 
                 auto_load: bool = True):
        """
        Initialize the advanced configuration manager.
        
        Args:
            config_file: Path to configuration file (supports .json, .yaml, .yml, .cfg, .ini)
            auto_load: Whether to automatically load configuration from file
        """
        self.config_file = config_file
        self.config_data: Dict[str, Any] = {}
        self.overrides: Dict[str, Any] = {}
        self.defaults: Dict[str, Any] = {}
        
        if auto_load and config_file and os.path.exists(config_file):
            self.load_config_file(config_file)
        elif auto_load and config_file:
            logger.warning(f"Config file not found: {config_file}")
    
    def load_config_file(self, filepath: str) -> bool:
        """
        Load configuration from a file with automatic format detection.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if not os.path.exists(filepath):
                logger.error(f"Configuration file not found: {filepath}")
                return False
            
            # Detect file format
            if filepath.endswith('.json'):
                return self._load_json(filepath)
            elif filepath.endswith(('.yaml', '.yml')):
                return self._load_yaml(filepath)
            elif filepath.endswith(('.cfg', '.ini')):
                return self._load_ini(filepath)
            else:
                # Try to auto-detect
                return self._auto_load(filepath)
                
        except Exception as e:
            logger.error(f"Failed to load configuration from {filepath}: {e}")
            return False
    
    def _load_json(self, filepath: str) -> bool:
        """Load JSON configuration file"""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            self.config_data.update(data)
            logger.info(f"Loaded JSON configuration from {filepath}")
            return True
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON in {filepath}: {e}")
            return False
        except Exception as e:
            logger.error(f"Error reading JSON file {filepath}: {e}")
            return False
    
    def _load_yaml(self, filepath: str) -> bool:
        """Load YAML configuration file"""
        try:
            import yaml
            with open(filepath, 'r') as f:
                data = yaml.safe_load(f)
            if data:
                self.config_data.update(data)
            logger.info(f"Loaded YAML configuration from {filepath}")
            return True
        except ImportError:
            logger.error("PyYAML not installed. Install with: pip install pyyaml")
            return False
        except Exception as e:
            logger.error(f"Error reading YAML file {filepath}: {e}")
            return False
    
    def _load_ini(self, filepath: str) -> bool:
        """Load INI-style configuration file"""
        try:
            import configparser
            config = configparser.ConfigParser()
            config.read(filepath)
            
            for section in config.sections():
                for key, value in config.items(section):
                    self.config_data[f"{section}.{key}"] = value
            
            logger.info(f"Loaded INI configuration from {filepath}")
            return True
        except ImportError:
            logger.error("configparser not available. Install with: pip install configparser")
            return False
        except Exception as e:
            logger.error(f"Error reading INI file {filepath}: {e}")
            return False
    
    def _auto_load(self, filepath: str) -> bool:
        """Auto-detect and load configuration file format"""
        # Try each format in order
        formats = [
            ('.json', self._load_json),
            ('.yaml', self._load_yaml),
            ('.yml', self._load_yaml),
            ('.cfg', self._load_ini),
            ('.ini', self._load_ini),
        ]
        
        for ext, loader in formats:
            if filepath.endswith(ext):
                return loader(filepath)
        
        # If no format matches, try JSON first
        return self._load_json(filepath)
    
    def set_default(self, key: str, value: Any, description: str = ""):
        """
        Set a default configuration value.
        
        Args:
            key: Configuration key (supports dot notation for nested values)
            value: Default value
            description: Human-readable description of the setting
        """
        self.defaults[key] = value
        logger.debug(f"Set default configuration: {key} = {value}")
    
    def set_override(self, key: str, value: Any):
        """
        Set an override configuration value (takes precedence over defaults and files).
        
        Args:
            key: Configuration key
            value: Override value
        """
        self.overrides[key] = value
        logger.debug(f"Set override configuration: {key} = {value}")
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value with support for dot notation and environment variables.
        
        Args:
            key: Configuration key (supports dot notation for nested access)
            default: Default value if key not found
            
        Returns:
            Any: Configuration value
        """
        # Check overrides first (highest priority)
        if key in self.overrides:
            return self.overrides[key]
        
        # Check environment variables
        env_key = key.replace('.', '_').upper()
        if env_key in os.environ:
            env_value = os.environ[env_key]
            # Convert to appropriate type
            if isinstance(default, bool):
                return env_value.lower() in ('true', '1', 'yes', 'on')
            elif isinstance(default, int):
                return int(env_value)
            elif isinstance(default, float):
                return float(env_value)
            return env_value
        
        # Check main configuration
        if key in self.config_data:
            return self.config_data[key]
        
        # Check nested keys with dot notation
        keys = key.split('.')
        current = self.config_data
        for k in keys:
            if isinstance(current, dict) and k in current:
                current = current[k]
            else:
                return default
        
        # Check defaults
        if key in self.defaults:
            return self.defaults[key]
        
        return default
    
    def get_nested(self, key_path: str, default: Any = None) -> Any:
        """
        Get nested configuration value.
        
        Args:
            key_path: Path to nested value (e.g., "brain.neuron_count")
            default: Default value if path not found
            
        Returns:
            Any: Nested configuration value
        """
        return self.get(key_path, default)
    
    def set_nested(self, key_path: str, value: Any, description: str = ""):
        """
        Set nested configuration value.
        
        Args:
            key_path: Path to nested value (e.g., "brain.neuron_count")
            value: Value to set
            description: Description for the setting
        """
        # Create nested structure if it doesn't exist
        keys = key_path.split('.')
        current = self.config_data
        
        for k in keys[:-1]:
            if k not in current:
                current[k] = {}
            current = current[k]
        
        current[keys[-1]] = value
        logger.debug(f"Set nested configuration: {key_path} = {value}")
    
    def validate_config(self, schema: Optional[Dict[str, Any]] = None) -> List[str]:
        """
        Validate configuration against a schema or basic rules.
        
        Args:
            schema: Optional validation schema
            
        Returns:
            List[str]: List of validation errors (empty if valid)
        """
        errors = []
        
        if schema:
            errors.extend(self._validate_against_schema(schema))
        else:
            errors.extend(self._validate_basic_rules())
        
        return errors
    
    def _validate_against_schema(self, schema: Dict[str, Any]) -> List[str]:
        """Validate configuration against a JSON schema-like structure"""
        errors = []
        
        for key, rules in schema.items():
            value = self.get(key.split('.')[-1]) if '.' in key else self.get(key)
            
            if rules.get('required', False) and value is None:
                errors.append(f"Required configuration key missing: {key}")
            
            if 'type' in rules:
                expected_type = rules['type']
                if not isinstance(value, expected_type) and value is not None:
                    errors.append(f"Configuration key '{key}' expected type {expected_type}, got {type(value)}")
            
            if 'range' in rules and value is not None:
                min_val, max_val = rules['range']
                if not (min_val <= value <= max_val):
                    errors.append(f"Configuration key '{key}' value {value} outside range [{min_val}, {max_val}]")
        
        return errors
    
    def _validate_basic_rules(self) -> List[str]:
        """Apply basic validation rules"""
        errors = []
        
        # Common NLM configuration validations
        brain_neuron_count = self.get('brain.neuron_count')
        if isinstance(brain_neuron_count, (int, float)) and brain_neuron_count <= 0:
            errors.append("brain.neuron_count must be positive")
        
        connection_prob = self.get('brain.connection_probability')
        if isinstance(connection_prob, (int, float)) and not (0 <= connection_prob <= 1):
            errors.append("brain.connection_probability must be between 0 and 1")
        
        return errors
    
    def save_config(self, filepath: str, format: str = 'json') -> bool:
        """
        Save configuration to a file.
        
        Args:
            filepath: Output file path
            format: Output format ('json', 'yaml', 'ini')
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if format == 'json':
                with open(filepath, 'w') as f:
                    json.dump(self.config_data, f, indent=2)
            elif format == 'yaml':
                import yaml
                with open(filepath, 'w') as f:
                    yaml.dump(self.config_data, f, default_flow_style=False)
            elif format == 'ini':
                import configparser
                config = configparser.ConfigParser()
                for section_key, value in self.config_data.items():
                    if '.' in section_key:
                        section, key = section_key.split('.', 1)
                        if section not in config.sections():
                            config.add_section(section)
                        config.set(section, key, str(value))
                
                with open(filepath, 'w') as f:
                    config.write(f)
            else:
                logger.error(f"Unsupported format: {format}")
                return False
            
            logger.info(f"Configuration saved to {filepath}")
            return True
        except Exception as e:
            logger.error(f"Error saving configuration to {filepath}: {e}")
            return False
    
    def export_for_environment(self, prefix: str = 'NLM_') -> Dict[str, str]:
        """
        Export configuration for use as environment variables.
        
        Args:
            prefix: Prefix for environment variables
            
        Returns:
            Dict[str, str]: Environment variable assignments
        """
        env_vars = {}
        
        for key, value in self.config_data.items():
            env_key = f"{prefix}{key.replace('.', '_').upper()}"
            if isinstance(value, bool):
                env_vars[env_key] = '1' if value else '0'
            elif isinstance(value, (int, float)):
                env_vars[env_key] = str(value)
            else:
                env_vars[env_key] = str(value)
        
        return env_vars
    
    def merge_configs(self, other_configs: List['NLMConfigManager']) -> 'NLMConfigManager':
        """
        Merge multiple configuration managers.
        
        Args:
            other_configs: List of other configuration managers to merge
            
        Returns:
            NLMConfigManager: New configuration manager with merged data
        """
        merged = NLMConfigManager()
        
        # Collect all configuration data
        all_configs = [self] + other_configs
        for config in all_configs:
            merged.config_data.update(config.config_data)
            merged.overrides.update(config.overrides)
            merged.defaults.update(config.defaults)
        
        return merged
    
    def __repr__(self) -> str:
        """String representation of configuration"""
        return f"NLMConfigManager(keys={len(self.config_data)}, overrides={len(self.overrides)}, defaults={len(self.defaults)})"
    
    def __str__(self) -> str:
        """Human-readable configuration summary"""
        return f"NLM Configuration: {len(self.config_data)} main, {len(self.overrides)} overrides, {len(self.defaults)} defaults"


# Global configuration instance for backward compatibility
global_config = NLMConfigManager()

# Convenience functions for backward compatibility
def load_config(filepath: str) -> bool:
    """Load configuration from file (backward compatibility)"""
    return global_config.load_config_file(filepath)

def get_config(key: str, default: Any = None) -> Any:
    """Get configuration value (backward compatibility)"""
    return global_config.get(key, default)

def set_config(key: str, value: Any):
    """Set configuration value (backward compatibility)"""
    global_config.set_override(key, value)

def save_config(filepath: str, format: str = 'json') -> bool:
    """Save configuration to file (backward compatibility)"""
    return global_config.save_config(filepath, format)
