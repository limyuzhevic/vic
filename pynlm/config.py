# Config module - Enhanced Python interface for configuration
"""
Enhanced Python interface for configuration management with improved
error handling and additional functionality.
"""

from typing import Dict, Any, List, Optional, Union
import json
import os
from enum import Enum

class ConfigSource(Enum):
    """Configuration source enumeration."""
    Default = 0
    File = 1
    CommandLine = 2
    Environment = 3

class Config:
    """Enhanced Python interface for NLM configuration.
    
    This class provides a more Pythonic interface for configuration management,
    with improved error handling, type safety, and additional functionality.
    """
    
    def __init__(self, cpp_config=None):
        """Initialize configuration.
        
        Args:
            cpp_config: C++ Config instance (internal use).
        """
        if cpp_config is not None:
            self._config = cpp_config
        else:
            # Create a default C++ config
            from . import _pynlm
            self._config = _pynlm.createDefaultConfig()
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from file.
        
        Args:
            filepath: Path to configuration file.
            
        Returns:
            True if loaded successfully, False otherwise.
        """
        if not os.path.exists(filepath):
            return False
        
        try:
            with open(filepath, 'r') as f:
                # Try to parse as JSON first
                try:
                    data = json.load(f)
                    self._load_dict(data)
                    return True
                except json.JSONDecodeError:
                    # Fall back to simple key=value format
                    f.seek(0)
                    return self._config.loadFromFile(filepath)
        except Exception:
            return False
    
    def load_from_dict(self, data: Dict[str, Any]) -> bool:
        """Load configuration from dictionary.
        
        Args:
            data: Configuration data.
            
        Returns:
            True if loaded successfully.
        """
        self._load_dict(data)
        return True
    
    def load_from_env(self, prefix: str = "NLM_"):
        """Load configuration from environment variables.
        
        Args:
            prefix: Prefix for environment variables.
        """
        for key, value in os.environ.items():
            if key.startswith(prefix):
                config_key = key[len(prefix):].lower()
                self._set_config_value(config_key, value)
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to file.
        
        Args:
            filepath: Path to save file.
            
        Returns:
            True if saved successfully.
        """
        try:
            with open(filepath, 'w') as f:
                self._save_dict(f)
            return True
        except Exception:
            return False
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key.
            
        Returns:
            True if key exists, False otherwise.
        """
        return self._config.has(key)
    
    def get(self, key: str, default_value: Any = None):
        """Get configuration value.
        
        Args:
            key: Configuration key.
            default_value: Default value if key not found.
            
        Returns:
            Configuration value or default.
        """
        if default_value is None:
            value = self._config.get(key)
            if value is not None:
                return value
            return None
        
        return self._config.getOr(key, default_value)
    
    def get_int(self, key: str, default_value: int = 0) -> int:
        """Get integer configuration value.
        
        Args:
            key: Configuration key.
            default_value: Default value.
            
        Returns:
            Integer value.
        """
        return self._config.getOr<int64_t>(key, default_value)
    
    def get_float(self, key: str, default_value: float = 0.0) -> float:
        """Get float configuration value.
        
        Args:
            key: Configuration key.
            default_value: Default value.
            
        Returns:
            Float value.
        """
        return self._config.getOr<double>(key, default_value)
    
    def get_bool(self, key: str, default_value: bool = False) -> bool:
        """Get boolean configuration value.
        
        Args:
            key: Configuration key.
            default_value: Default value.
            
        Returns:
            Boolean value.
        """
        return self._config.getOr<bool>(key, default_value)
    
    def get_string(self, key: str, default_value: str = "") -> str:
        """Get string configuration value.
        
        Args:
            key: Configuration key.
            default_value: Default value.
            
        Returns:
            String value.
        """
        return self._config.getOr<string>(key, default_value)
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.Default):
        """Set configuration value.
        
        Args:
            key: Configuration key.
            value: Configuration value.
            source: Configuration source.
        """
        self._set_config_value(key, value, source)
    
    def remove(self, key: str):
        """Remove configuration key.
        
        Args:
            key: Configuration key to remove.
        """
        self._config.remove(key)
    
    def clear(self):
        """Clear all configuration."""
        self._config.clear()
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of configuration keys.
        """
        return self._config.getKeys()
    
    def get_summary(self) -> str:
        """Get configuration summary.
        
        Returns:
            Summary string.
        """
        return self._config.summary()
    
    def validate(self, schema: Optional[Dict[str, Any]] = None) -> List[str]:
        """Validate configuration against schema.
        
        Args:
            schema: Validation schema (optional).
            
        Returns:
            List of validation errors (empty if valid).
        """
        errors = []
        
        # Default schema validation
        required_keys = ['random_seed', 'simulation_timestep', 'neuron_count']
        for key in required_keys:
            if not self.has(key):
                errors.append(f"Missing required key: {key}")
        
        # Type validation
        if self.has('random_seed') and not isinstance(self.get('random_seed'), (int, float)):
            errors.append("random_seed must be numeric")
        
        if self.has('simulation_timestep') and self.get('simulation_timestep', 0) <= 0:
            errors.append("simulation_timestep must be positive")
        
        if self.has('neuron_count') and self.get('neuron_count', 0) <= 0:
            errors.append("neuron_count must be positive")
        
        # Custom schema validation
        if schema:
            for key, expected_type in schema.items():
                if self.has(key):
                    value = self.get(key)
                    if not isinstance(value, expected_type):
                        errors.append(f"Key {key} has wrong type. Expected {expected_type}, got {type(value)}")
        
        return errors
    
    def clone(self) -> 'Config':
        """Create a copy of this configuration.
        
        Returns:
            New Config instance.
        """
        from . import _pynlm
        new_cpp_config = _pynlm.createDefaultConfig()
        # Copy all entries (simplified)
        for key in self.get_keys():
            new_cpp_config._set_config_value(key, self.get(key))
        return Config(new_cpp_config)
    
    def merge(self, other: 'Config', override: bool = True):
        """Merge another configuration.
        
        Args:
            other: Configuration to merge.
            override: Whether to override existing values.
        """
        for key in other.get_keys():
            if override or not self.has(key):
                self.set(key, other.get(key))
    
    def get_section(self, section_prefix: str) -> Dict[str, Any]:
        """Get configuration section.
        
        Args:
            section_prefix: Prefix for keys in section.
            
        Returns:
            Dictionary of configuration keys in section.
        """
        section = {}
        for key in self.get_keys():
            if key.startswith(section_prefix + '_'):
                short_key = key[len(section_prefix) + 1:]
                section[short_key] = self.get(key)
        return section
    
    def set_section(self, section_prefix: str, section: Dict[str, Any], source: ConfigSource = ConfigSource.Default):
        """Set configuration section.
        
        Args:
            section_prefix: Prefix for keys in section.
            section: Configuration section dictionary.
            source: Configuration source.
        """
        for key, value in section.items():
            full_key = f"{section_prefix}_{key}"
            self.set(full_key, value, source)
    
    def apply_profile(self, profile_name: str) -> bool:
        """Apply predefined configuration profile.
        
        Args:
            profile_name: Name of profile to apply.
            
        Returns:
            True if profile applied successfully.
        """
        profiles = {
            'default': {
                'random_seed': 42,
                'simulation_timestep': 0.001,
                'neuron_count': 1000,
                'region_count': 2,
                'connection_probability': 0.1,
                'plasticity_learning_rate': 0.01,
                'dopamine_baseline': 0.1,
                'log_level': 'INFO',
            },
            'fast': {
                'random_seed': 42,
                'simulation_timestep': 0.01,
                'neuron_count': 500,
                'region_count': 1,
                'connection_probability': 0.15,
                'plasticity_learning_rate': 0.02,
                'dopamine_baseline': 0.1,
                'log_level': 'WARN',
                'max_simulation_steps': 1000,
            },
            'detailed': {
                'random_seed': 42,
                'simulation_timestep': 0.0001,
                'neuron_count': 5000,
                'region_count': 4,
                'connection_probability': 0.05,
                'plasticity_learning_rate': 0.005,
                'dopamine_baseline': 0.2,
                'log_level': 'DEBUG',
                'max_simulation_steps': 100000,
            },
        }
        
        if profile_name not in profiles:
            return False
        
        self.clear()
        self.load_from_dict(profiles[profile_name])
        return True
    
    def get_config_stats(self) -> Dict[str, Any]:
        """Get configuration statistics.
        
        Returns:
            Dictionary with configuration statistics.
        """
        keys = self.get_keys()
        stats = {
            'total_keys': len(keys),
            'by_source': {},
            'types': {
                'int': 0,
                'float': 0,
                'bool': 0,
                'string': 0,
                'other': 0,
            }
        }
        
        # Count by source and type
        for key in keys:
            # This would need actual implementation to get source info
            value = self.get(key)
            if isinstance(value, bool):
                stats['types']['bool'] += 1
            elif isinstance(value, int):
                stats['types']['int'] += 1
            elif isinstance(value, float):
                stats['types']['float'] += 1
            elif isinstance(value, str):
                stats['types']['string'] += 1
            else:
                stats['types']['other'] += 1
        
        return stats
    
    # Internal helper methods
    
    def _load_dict(self, data: Dict[str, Any]):
        """Load configuration from dictionary (internal)."""
        for key, value in data.items():
            if isinstance(value, bool):
                self._config.set(key, value, ConfigSource.File)
            elif isinstance(value, int):
                self._config.set(key, value, ConfigSource.File)
            elif isinstance(value, float):
                self._config.set(key, value, ConfigSource.File)
            elif isinstance(value, str):
                self._config.set(key, value, ConfigSource.File)
            else:
                # Try to convert to string
                self._config.set(key, str(value), ConfigSource.File)
    
    def _set_config_value(self, key: str, value: Any, source: ConfigSource = ConfigSource.Default):
        """Set configuration value (internal)."""
        if isinstance(value, bool):
            self._config.set(key, value, source)
        elif isinstance(value, int):
            self._config.set(key, value, source)
        elif isinstance(value, float):
            self._config.set(key, value, source)
        elif isinstance(value, str):
            self._config.set(key, value, source)
        else:
            # Try to convert to string
            self._config.set(key, str(value), source)
    
    def _save_dict(self, file_handle):
        """Save configuration to file (internal)."""
        # Simple key=value format for backward compatibility
        for key in self.get_keys():
            value = self.get(key)
            file_handle.write(f"{key} = {value}\n")
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<Config: {len(self.get_keys())} keys>"
    
    def __str__(self) -> str:
        """String representation."""
        return self.get_summary()


# Configuration factory functions

def createDefaultConfig() -> Config:
    """Create default configuration.
    
    Returns:
        Config instance with default values.
    """
    return Config()


def loadConfigFromFile(filepath: str) -> Config:
    """Load configuration from file.
    
    Args:
        filepath: Path to configuration file.
        
    Returns:
        Config instance.
    """
    config = createDefaultConfig()
    config.load_from_file(filepath)
    return config


def createConfigFromDict(data: Dict[str, Any]) -> Config:
    """Create configuration from dictionary.
    
    Args:
        data: Configuration data.
        
    Returns:
        Config instance.
    """
    config = createDefaultConfig()
    config.load_from_dict(data)
    return config


# Re-export for convenience
Config = Config