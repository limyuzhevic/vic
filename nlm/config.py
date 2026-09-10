# High-level configuration management with validation and defaults
from typing import Dict, Any, Optional, Union
from enum import Enum
import json
from pathlib import Path

class ConfigSource(Enum):
    DEFAULT = "default"
    FILE = "file"
    COMMAND_LINE = "command_line"
    PROGRAM = "program"

class ConfigurationError(Exception):
    """Raised when configuration is invalid or unavailable."""
    pass

class Config:
    """
    High-level configuration management with validation and type safety.
    
    Provides a Pythonic interface for NLM configuration with:
    - Type validation and conversion
    - Configuration presets and validation
    - File loading/saving with JSON support
    - Environment variable support
    - Configuration inheritance and merging
    
    Examples:
        Basic configuration:
            config = Config()
            config.set("brain.neuron_count", 1000)
            config.set("brain.connection_probability", 0.1)
        
        Loading from file:
            config = Config().load_from_file("configs/default.json")
        
        Configuration presets:
            config = Config.create_preset("research")
            config = Config.create_preset("performance", neuron_count=2000)
    """
    
    # Predefined configuration presets
    PRESETS = {
        "beginner": {
            "brain.neuron_count": 500,
            "brain.connection_probability": 0.05,
            "simulation_timestep": 0.001,
            "region_count": 1,
            "stdp_ltp_weight": 0.005,
            "stdp_ltd_weight": 0.007,
            "enable_learning": True,
            "enable_development": True,
        },
        "research": {
            "brain.neuron_count": 10000,
            "brain.connection_probability": 0.1,
            "simulation_timestep": 0.0005,
            "region_count": 4,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.5,
            "neuromod.novelty.enable": True,
            "enable_learning": True,
            "enable_development": True,
            "enable_curiosity": True,
        },
        "performance": {
            "brain.neuron_count": 2000,
            "brain.connection_probability": 0.08,
            "simulation_timestep": 0.001,
            "region_count": 2,
            "stdp_ltp_weight": 0.008,
            "stdp_ltd_weight": 0.009,
            "plasticity.structural.enable": False,  # Disable for speed
            "enable_learning": True,
            "enable_development": True,
            "enable_curiosity": True,
        },
        "simulation": {
            "brain.neuron_count": 1000,
            "brain.connection_probability": 0.1,
            "simulation_timestep": 0.001,
            "region_count": 1,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "plasticity.structural.enable": True,
            "plasticity.hebbian.enable": True,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "enable_learning": True,
            "enable_development": True,
            "enable_curiosity": True,
        }
    }
    
    def __init__(self, **kwargs):
        """
        Initialize configuration.
        
        Args:
            **kwargs: Initial configuration key-value pairs
        """
        self._config: Dict[str, Any] = {}
        self._defaults: Dict[str, Any] = {}
        self._validation_rules: Dict[str, Dict[str, Any]] = {}
        
        # Set default validation rules
        self._setup_validation_rules()
        
        # Apply initial configuration
        if kwargs:
            self.update(kwargs)
    
    def _setup_validation_rules(self):
        """Setup default validation rules for configuration keys."""
        self._validation_rules = {
            "brain.neuron_count": {"type": int, "min": 1, "max": 100000},
            "brain.connection_probability": {"type": float, "min": 0.0, "max": 1.0},
            "simulation_timestep": {"type": float, "min": 0.0001, "max": 1.0},
            "region_count": {"type": int, "min": 1, "max": 100},
            "stdp_ltp_weight": {"type": float, "min": 0.0, "max": 1.0},
            "stdp_ltd_weight": {"type": float, "min": 0.0, "max": 1.0},
            "stdp_tau": {"type": float, "min": 1.0, "max": 1000.0},
            "plasticity.structural.enable": {"type": bool},
            "plasticity.hebbian.enable": {"type": bool},
            "plasticity.stdp.enable": {"type": bool},
            "neuromod.dopamine.scale": {"type": float, "min": 0.0, "max": 10.0},
            "neuromod.curiosity.enable": {"type": bool},
            "neuromod.novelty.enable": {"type": bool},
            "enable_learning": {"type": bool},
            "enable_development": {"type": bool},
            "enable_curiosity": {"type": bool},
        }
    
    def set(self, key: str, value: Any, source: Optional[ConfigSource] = None) -> 'Config':
        """
        Set a configuration value with validation.
        
        Args:
            key: Configuration key (dot notation supported)
            value: Value to set
            source: Source of configuration (optional)
        
        Returns:
            Self for method chaining
        
        Raises:
            ConfigurationError: If value is invalid
        """
        # Validate the value
        if key in self._validation_rules:
            self._validate_key_value(key, value)
        
        # Set the value
        self._config[key] = value
        return self
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
        
        Returns:
            Configuration value or default
        """
        # Try to get the value from the config
        if key in self._config:
            return self._config[key]
        
        # Try nested keys (e.g., "brain.neuron_count")
        if "." in key:
            parts = key.split(".")
            current = self._config
            for part in parts:
                if isinstance(current, dict) and part in current:
                    current = current[part]
                else:
                    return default
            return current
        
        return default
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get configuration value as int."""
        value = self.get(key, default)
        try:
            return int(value)
        except (ValueError, TypeError):
            return default
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get configuration value as float."""
        value = self.get(key, default)
        try:
            return float(value)
        except (ValueError, TypeError):
            return default
    
    def get_bool(self, key: str, default: bool = False) -> bool:
        """Get configuration value as bool."""
        value = self.get(key, default)
        try:
            if isinstance(value, str):
                return value.lower() in ('true', '1', 'yes', 'on')
            return bool(value)
        except (ValueError, TypeError):
            return default
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
        return key in self._config or ("." in key and self._get_nested_key(key) is not None)
    
    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._config.keys())
    
    def update(self, other: Union[Dict, 'Config']) -> 'Config':
        """
        Update configuration from another dict or Config object.
        
        Args:
            other: Configuration source
        
        Returns:
            Self for method chaining
        """
        if isinstance(other, Config):
            self._config.update(other._config)
        else:
            self._config.update(other)
        return self
    
    def load_from_file(self, filepath: str, format: str = 'json') -> 'Config':
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            format: File format ('json', 'yaml', 'toml', 'cfg')
        
        Returns:
            Self for method chaining
        
        Raises:
            ConfigurationError: If file cannot be loaded
        """
        path = Path(filepath)
        if not path.exists():
            raise ConfigurationError(f"Configuration file not found: {filepath}")
        
        try:
            if format == 'json':
                with open(path, 'r') as f:
                    data = json.load(f)
                self.update(data)
            elif format == 'cfg':
                # Simple key=value format
                with open(path, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            
                            # Try to convert to appropriate type
                            if value.lower() in ('true', 'false'):
                                value = value.lower() == 'true'
                            elif value.isdigit():
                                value = int(value)
                            else:
                                try:
                                    value = float(value)
                                except ValueError:
                                    pass  # Keep as string
                            
                            self.set(key, value)
            else:
                raise ConfigurationError(f"Unsupported format: {format}")
        except json.JSONDecodeError as e:
            raise ConfigurationError(f"Invalid JSON in {filepath}: {e}")
        except Exception as e:
            raise ConfigurationError(f"Error loading {filepath}: {e}")
        
        return self
    
    def save_to_file(self, filepath: str, format: str = 'json') -> 'Config':
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration
            format: File format ('json', 'yaml', 'cfg')
        
        Returns:
            Self for method chaining
        """
        path = Path(filepath)
        
        try:
            if format == 'json':
                with open(path, 'w') as f:
                    json.dump(self._config, f, indent=2)
            elif format == 'cfg':
                with open(path, 'w') as f:
                    for key, value in self._config.items():
                        # Format based on type
                        if isinstance(value, bool):
                            value_str = 'true' if value else 'false'
                        elif isinstance(value, (int, float)):
                            value_str = str(value)
                        else:
                            value_str = f'"{value}"'
                        f.write(f"{key} = {value_str}\n")
            else:
                raise ConfigurationError(f"Unsupported format: {format}")
        except Exception as e:
            raise ConfigurationError(f"Error saving to {filepath}: {e}")
        
        return self
    
    def create_preset(self, preset_name: str, **overrides) -> 'Config':
        """
        Create configuration from preset with optional overrides.
        
        Args:
            preset_name: Name of preset ('beginner', 'research', 'performance', 'simulation')
            **overrides: Override values
        
        Returns:
            New Config instance
        
        Raises:
            ConfigurationError: If preset not found
        """
        if preset_name not in self.PRESETS:
            available = ', '.join(self.PRESETS.keys())
            raise ConfigurationError(f"Unknown preset: {preset_name}. Available: {available}")
        
        config = Config()
        config._config = self.PRESETS[preset_name].copy()
        config.update(overrides)
        return config
    
    def validate(self) -> List[str]:
        """
        Validate configuration.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []
        
        for key, value in self._config.items():
            if key in self._validation_rules:
                rule = self._validation_rules[key]
                
                # Check type
                if 'type' in rule:
                    expected_type = rule['type']
                    if not isinstance(value, expected_type):
                        errors.append(f"Key '{key}': expected type {expected_type}, got {type(value)}")
                
                # Check min/max for numeric types
                if 'min' in rule and isinstance(value, (int, float)):
                    if value < rule['min']:
                        errors.append(f"Key '{key}': value {value} is less than minimum {rule['min']}")
                
                if 'max' in rule and isinstance(value, (int, float)):
                    if value > rule['max']:
                        errors.append(f"Key '{key}': value {value} is greater than maximum {rule['max']}")
        
        return errors
    
    def merge(self, other: 'Config') -> 'Config':
        """
        Merge another configuration into this one.
        
        Args:
            other: Configuration to merge
        
        Returns:
            New merged Config instance
        """
        merged = Config()
        merged._config = {**self._config, **other._config}
        return merged
    
    def __getitem__(self, key: str) -> Any:
        """Support dict-like access."""
        return self.get(key)
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Support dict-like assignment."""
        self.set(key, value)
    
    def __contains__(self, key: str) -> bool:
        """Support 'in' operator."""
        return self.has(key)
    
    def __repr__(self) -> str:
        """String representation."""
        return f"Config({len(self._config)} settings)"
    
    def __str__(self) -> str:
        """String representation with config summary."""
        return f"Configuration with {len(self._config)} settings"
    
    def _get_nested_key(self, key: str) -> Any:
        """Get nested configuration key."""
        parts = key.split(".")
        current = self._config
        for part in parts:
            if isinstance(current, dict) and part in current:
                current = current[part]
            else:
                return None
        return current
    
    def _validate_key_value(self, key: str, value: Any):
        """Validate a single key-value pair."""
        if key not in self._validation_rules:
            return  # No validation rules for this key
        
        rule = self._validation_rules[key]
        
        # Type checking
        if 'type' in rule and not isinstance(value, rule['type']):
            raise ConfigurationError(
                f"Key '{key}': expected type {rule['type']}, got {type(value)}"
            )
        
        # Range checking
        if 'min' in rule and isinstance(value, (int, float)):
            if value < rule['min']:
                raise ConfigurationError(
                    f"Key '{key}': value {value} is less than minimum {rule['min']}"
                )
        
        if 'max' in rule and isinstance(value, (int, float)):
            if value > rule['max']:
                raise ConfigurationError(
                    f"Key '{key}': value {value} is greater than maximum {rule['max']}"
                )


# Factory functions for backward compatibility
_config_instance: Optional[Config] = None

def create_default_config() -> Config:
    """Create a default configuration."""
    global _config_instance
    if _config_instance is None:
        _config_instance = Config.create_preset("simulation")
    return _config_instance

def create_config(**kwargs) -> Config:
    """Create a custom configuration."""
    return Config(**kwargs)
