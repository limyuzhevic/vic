# NLM Configuration Management System
# Advanced configuration loading, validation, and management

"""
This module provides advanced configuration management capabilities for the NLM project.
It includes:
- Configuration schema validation
- Environment variable integration
- Configuration inheritance
- Configuration hot-reloading
- Configuration migration
"""

import json
import os
import logging
from typing import Dict, Any, Optional, List, Union
from pathlib import Path
from dataclasses import dataclass
from enum import Enum
import time
import threading
from contextlib import contextmanager

class ConfigurationError(Exception):
    """Raised when configuration errors occur"""
    pass

class ConfigurationSource(Enum):
    """Source of configuration value"""
    DEFAULT = "default"
    FILE = "file"
    ENVIRONMENT = "environment"
    COMMAND_LINE = "command_line"
    OVERRIDE = "override"

@dataclass
class ConfigurationEntry:
    """Configuration entry with metadata"""
    key: str
    value: Any
    source: ConfigurationSource
    description: str
    type: type = None
    required: bool = False
    default_value: Any = None

class ConfigurationValidator:
    """Validates configuration values"""
    
    def __init__(self):
        self.validators = {}
    
    def register_validator(self, key: str, validator_func, description: str = ""):
        """Register a validator function for a configuration key"""
        self.validators[key] = {
            'validator': validator_func,
            'description': description,
            'last_validated': None
        }
    
    def validate(self, key: str, value: Any) -> bool:
        """Validate a configuration value"""
        if key in self.validators:
            validator_info = self.validators[key]
            try:
                is_valid = validator_info['validator'](value)
                validator_info['last_validated'] = time.time()
                return is_valid
            except Exception as e:
                logging.warning(f"Validation failed for {key}: {e}")
                return False
        return True
    
    def get_schema(self) -> Dict[str, Dict]:
        """Get the validation schema"""
        return {
            key: {
                'validator': func.__name__ if hasattr(func, '__name__') else str(func),
                'description': info['description'],
                'last_validated': info.get('last_validated')
            }
            for key, info in self.validators.items()
        }

class ConfigurationManager:
    """Advanced configuration manager for NLM"""
    
    def __init__(self, config_file: str = None, environment_prefix: str = "NLM_",
                 schema_file: str = None, hot_reload: bool = False):
        """
        Initialize configuration manager
        
        Args:
            config_file: Path to configuration file
            environment_prefix: Prefix for environment variables
            schema_file: Path to configuration schema file
            hot_reload: Whether to enable hot reloading
        """
        self.config_file = config_file
        self.environment_prefix = environment_prefix
        self.schema_file = schema_file
        self.hot_reload = hot_reload
        self.validator = ConfigurationValidator()
        self.config_entries: Dict[str, ConfigurationEntry] = {}
        self._reload_thread = None
        self._last_modified = 0
        
        # Initialize with default configuration
        self._initialize_default_config()
        
        # Load from schema if provided
        if schema_file:
            self._load_schema(schema_file)
        
        # Load from environment variables
        self._load_environment_config()
        
        # Load from file if provided
        if config_file:
            self._load_config_file()
    
    def _initialize_default_config(self):
        """Initialize with default configuration values"""
        default_config = {
            # Core settings
            "neuron_count": ConfigurationEntry(
                "neuron_count", 1000, ConfigurationSource.DEFAULT,
                "Number of neurons in the brain", int, True, 1000
            ),
            "simulation_timestep": ConfigurationEntry(
                "simulation_timestep", 0.001, ConfigurationSource.DEFAULT,
                "Simulation timestep in seconds", float, True, 0.001
            ),
            "region_count": ConfigurationEntry(
                "region_count", 2, ConfigurationSource.DEFAULT,
                "Number of brain regions", int, True, 2
            ),
            
            # Connectivity
            "connection_probability": ConfigurationEntry(
                "connection_probability", 0.1, ConfigurationSource.DEFAULT,
                "Connection probability for random connectivity", float, True, 0.1
            ),
            
            # Plasticity
            "plasticity_learning_rate": ConfigurationEntry(
                "plasticity_learning_rate", 0.01, ConfigurationSource.DEFAULT,
                "Base plasticity learning rate", float, True, 0.01
            ),
            
            # STDP
            "stdp_ltp_weight": ConfigurationEntry(
                "stdp_ltp_weight", 0.01, ConfigurationSource.DEFAULT,
                "STDP Long-Term Potentiation weight", float, True, 0.01
            ),
            "stdp_ltd_weight": ConfigurationEntry(
                "stdp_ltd_weight", 0.012, ConfigurationSource.DEFAULT,
                "STDP Long-Term Depression weight", float, True, 0.012
            ),
            "stdp_time_constant": ConfigurationEntry(
                "stdp_time_constant", 20.0, ConfigurationSource.DEFAULT,
                "STDP time constant in milliseconds", float, True, 20.0
            ),
            
            # Development
            "synaptogenesis_rate": ConfigurationEntry(
                "synaptogenesis_rate", 0.001, ConfigurationSource.DEFAULT,
                "Synaptogenesis rate", float, True, 0.001
            ),
            "pruning_rate": ConfigurationEntry(
                "pruning_rate", 0.0001, ConfigurationSource.DEFAULT,
                "Pruning rate", float, True, 0.0001
            ),
            
            # Neuromodulation
            "curiosity_baseline": ConfigurationEntry(
                "curiosity_baseline", 0.1, ConfigurationSource.DEFAULT,
                "Curiosity baseline level", float, True, 0.1
            ),
            "reward_discount_factor": ConfigurationEntry(
                "reward_discount_factor", 0.99, ConfigurationSource.DEFAULT,
                "Reward discount factor", float, True, 0.99
            ),
            
            # Environment
            "environment_name": ConfigurationEntry(
                "environment_name", "GridWorld", ConfigurationSource.DEFAULT,
                "Environment name", str, True, "GridWorld"
            ),
            "environment_width": ConfigurationEntry(
                "environment_width", 10, ConfigurationSource.DEFAULT,
                "Environment width", int, True, 10
            ),
            "environment_height": ConfigurationEntry(
                "environment_height", 10, ConfigurationSource.DEFAULT,
                "Environment height", int, True, 10
            ),
            
            # Logging
            "log_level": ConfigurationEntry(
                "log_level", "INFO", ConfigurationSource.DEFAULT,
                "Log level (DEBUG, INFO, WARNING, ERROR)", str, True, "INFO"
            ),
            "log_to_file": ConfigurationEntry(
                "log_to_file", False, ConfigurationSource.DEFAULT,
                "Whether to log to file", bool, True, False
            ),
            "log_filename": ConfigurationEntry(
                "log_filename", "nlm.log", ConfigurationSource.DEFAULT,
                "Log filename", str, True, "nlm.log"
            ),
            
            # Simulation
            "max_simulation_steps": ConfigurationEntry(
                "max_simulation_steps", 10000, ConfigurationSource.DEFAULT,
                "Maximum simulation steps", int, True, 10000
            ),
            "simulation_time_limit": ConfigurationEntry(
                "simulation_time_limit", 0.0, ConfigurationSource.DEFAULT,
                "Simulation time limit in seconds", float, True, 0.0
            ),
            
            # Visualization
            "visualization_enabled": ConfigurationEntry(
                "visualization_enabled", False, ConfigurationSource.DEFAULT,
                "Whether visualization is enabled", bool, True, False
            ),
            "visualization_update_rate": ConfigurationEntry(
                "visualization_update_rate", 30.0, ConfigurationSource.DEFAULT,
                "Visualization update rate in Hz", float, True, 30.0
            ),
        }
        
        for entry in default_config.values():
            self.config_entries[entry.key] = entry
    
    def _load_schema(self, schema_file: str):
        """Load configuration schema from file"""
        try:
            with open(schema_file, 'r') as f:
                schema = json.load(f)
            
            # Register validators from schema
            for key, schema_info in schema.items():
                if 'validator' in schema_info and 'type' in schema_info:
                    self._register_validator_from_schema(key, schema_info)
        except Exception as e:
            logging.warning(f"Failed to load schema file {schema_file}: {e}")
    
    def _register_validator_from_schema(self, key: str, schema_info: Dict):
        """Register validator from schema"""
        validator_type = schema_info.get('type', 'generic')
        
        if validator_type == 'range':
            def range_validator(value):
                min_val = schema_info.get('min', float('-inf'))
                max_val = schema_info.get('max', float('inf'))
                if isinstance(value, (int, float)):
                    return min_val <= value <= max_val
                return False
            
            self.register_validator(key, range_validator, 
                                  f"Value must be between {schema_info.get('min')} and {schema_info.get('max')}")
        
        elif validator_type == 'enum':
            def enum_validator(value):
                allowed_values = schema_info.get('allowed_values', [])
                return value in allowed_values
            
            self.register_validator(key, enum_validator,
                                  f"Value must be one of: {schema_info.get('allowed_values', [])}")
        
        elif validator_type == 'positive':
            def positive_validator(value):
                return value > 0 if isinstance(value, (int, float)) else False
            
            self.register_validator(key, positive_validator, "Value must be positive")
        
        elif validator_type == 'non_negative':
            def non_negative_validator(value):
                return value >= 0 if isinstance(value, (int, float)) else False
            
            self.register_validator(key, non_negative_validator, "Value must be non-negative")
    
    def _load_environment_config(self):
        """Load configuration from environment variables"""
        for key, entry in self.config_entries.items():
            env_key = f"{self.environment_prefix}{key.upper()}"
            env_value = os.environ.get(env_key)
            
            if env_value is not None:
                try:
                    # Convert environment variable to appropriate type
                    converted_value = self._convert_environment_value(entry.type, env_value)
                    self.set(key, converted_value, ConfigurationSource.ENVIRONMENT)
                    logging.info(f"Loaded {key} from environment: {env_value}")
                except ValueError as e:
                    logging.warning(f"Failed to parse environment variable {env_key}: {e}")
    
    def _load_config_file(self):
        """Load configuration from file"""
        if not os.path.exists(self.config_file):
            logging.warning(f"Configuration file not found: {self.config_file}")
            return
        
        # Check if file was modified recently for hot reloading
        current_mtime = os.path.getmtime(self.config_file)
        if current_mtime <= self._last_modified and self.hot_reload:
            return
        
        try:
            with open(self.config_file, 'r') as f:
                config_data = json.load(f) if self.config_file.endswith('.json') else self._parse_config_file(f)
            
            for key, value in config_data.items():
                if key in self.config_entries:
                    self.set(key, value, ConfigurationSource.FILE)
                    logging.info(f"Loaded {key} from config file: {value}")
            
            self._last_modified = current_mtime
        except Exception as e:
            logging.error(f"Failed to load configuration file {self.config_file}: {e}")
    
    def _parse_config_file(self, file):
        """Parse simple key=value configuration file"""
        config_data = {}
        for line in file:
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                config_data[key] = value
        return config_data
    
    def _convert_environment_value(self, type_hint: type, value: str) -> Any:
        """Convert environment variable value to appropriate type"""
        if type_hint is int:
            return int(value)
        elif type_hint is float:
            return float(value)
        elif type_hint is bool:
            return value.lower() in ('true', '1', 'yes', 'on')
        elif type_hint is str:
            return value
        else:
            return value
    
    def register_validator(self, key: str, validator_func, description: str = ""):
        """Register a validator function for a configuration key"""
        self.validator.register_validator(key, validator_func, description)
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value"""
        if key in self.config_entries:
            return self.config_entries[key].value
        elif default is not None:
            return default
        else:
            raise KeyError(f"Configuration key not found: {key}")
    
    def set(self, key: str, value: Any, source: ConfigurationSource = ConfigurationSource.RUNTIME):
        """Set a configuration value"""
        if key in self.config_entries:
            # Validate the value
            if self.validator.validate(key, value):
                self.config_entries[key].value = value
                self.config_entries[key].source = source
                logging.debug(f"Set {key} = {value} (source: {source.value})")
            else:
                raise ConfigurationError(f"Invalid value for {key}: {value}")
        else:
            raise KeyError(f"Configuration key not found: {key}")
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists"""
        return key in self.config_entries
    
    def remove(self, key: str):
        """Remove a configuration key"""
        if key in self.config_entries:
            del self.config_entries[key]
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys"""
        return list(self.config_entries.keys())
    
    def get_config_dict(self) -> Dict[str, Any]:
        """Get configuration as dictionary"""
        return {
            key: entry.value for key, entry in self.config_entries.items()
        }
    
    def get_config_summary(self) -> str:
        """Get configuration summary"""
        summary = "Configuration Summary:\n"
        summary += f"Total keys: {len(self.config_entries)}\n\n"
        
        for key, entry in sorted(self.config_entries.items()):
            summary += f"{key}: {entry.value} (source: {entry.source.value})\n"
        
        return summary
    
    def validate_all(self) -> Dict[str, bool]:
        """Validate all configuration values"""
        results = {}
        for key, entry in self.config_entries.items():
            results[key] = self.validator.validate(key, entry.value)
        
        return results
    
    def get_validation_report(self) -> Dict:
        """Get detailed validation report"""
        report = {
            'total_keys': len(self.config_entries),
            'valid_keys': 0,
            'invalid_keys': 0,
            'validation_errors': {},
            'schema': self.validator.get_schema()
        }
        
        for key, entry in self.config_entries.items():
            if self.validator.validate(key, entry.value):
                report['valid_keys'] += 1
            else:
                report['invalid_keys'] += 1
                report['validation_errors'][key] = f"Invalid value: {entry.value}"
        
        return report
    
    def save_config(self, filepath: str):
        """Save configuration to file"""
        config_dict = self.get_config_dict()
        
        if filepath.endswith('.json'):
            with open(filepath, 'w') as f:
                json.dump(config_dict, f, indent=2)
        else:
            with open(filepath, 'w') as f:
                for key, value in config_dict.items():
                    if isinstance(value, str):
                        f.write(f'{key} = "{value}"\n')
                    else:
                        f.write(f'{key} = {value}\n')
        
        logging.info(f"Configuration saved to {filepath}")
    
    def hot_reload_check(self):
        """Check if configuration should be reloaded (hot reload)"""
        if not self.hot_reload or not self.config_file:
            return
        
        current_mtime = os.path.getmtime(self.config_file)
        if current_mtime > self._last_modified:
            logging.info("Configuration file modified, reloading...")
            self._load_config_file()
    
    def register_schema_file(self, schema_file: str):
        """Register a new schema file"""
        self._load_schema(schema_file)
    
    def update_from_dict(self, config_dict: Dict[str, Any], source: ConfigurationSource = ConfigurationSource.OVERRIDE):
        """Update configuration from dictionary"""
        for key, value in config_dict.items():
            if key in self.config_entries:
                self.set(key, value, source)
            else:
                logging.warning(f"Unknown configuration key: {key}")

# Global configuration instance
_global_config = ConfigurationManager()

def get_config() -> ConfigurationManager:
    """Get the global configuration instance"""
    return _global_config

def reset_config():
    """Reset the global configuration instance"""
    global _global_config
    _global_config = ConfigurationManager()

# Convenience functions for common configuration operations
def load_config(config_file: str = None) -> ConfigurationManager:
    """Load configuration from file"""
    if config_file:
        global _global_config
        _global_config = ConfigurationManager(config_file=config_file)
    return _global_config

def set_config_value(key: str, value: Any):
    """Set a configuration value in the global configuration"""
    global _global_config
    _global_config.set(key, value)

def get_config_value(key: str, default: Any = None) -> Any:
    """Get a configuration value from the global configuration"""
    global _global_config
    return _global_config.get(key, default)

if __name__ == "__main__":
    # Example usage
    logging.basicConfig(level=logging.INFO)
    
    # Load configuration from file
    config = load_config("config.json")
    
    # Set configuration values
    config.set("neuron_count", 2000)
    config.set("log_level", "DEBUG")
    
    # Get configuration values
    print(f"Neuron count: {config.get('neuron_count')}")
    print(f"Log level: {config.get('log_level')}")
    
    # Validate configuration
    validation_results = config.validate_all()
    print(f"Validation results: {validation_results}")
    
    # Get configuration summary
    print(f"Configuration summary:\n{config.get_config_summary()}")
    
    # Save configuration
    config.save_config("config_output.json")
