#!/usr/bin/env python3
"""
Standalone Config Implementation for NLM Config CLI

This provides a standalone Config class implementation that doesn't depend
on the pynlm Python bindings module, allowing the CLI tools to work even
when the Python bindings are not built.
"""

import json
import yaml
import os
from typing import Any, Dict, List, Optional, Union


class Config:
    """Simple configuration class that mimics pynlm.Config interface"""
    
    def __init__(self, data: Optional[Dict[str, Any]] = None):
        """Initialize config with optional data"""
        self._data: Dict[str, Any] = data or {}
    
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value"""
        self._data[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value"""
        return self._data.get(key, default)
    
    def get_string(self, key: str) -> str:
        """Get configuration value as string"""
        value = self._data.get(key)
        if value is None:
            raise ValueError(f"Key '{key}' not found")
        return str(value)
    
    def get_int(self, key: str) -> int:
        """Get configuration value as integer"""
        value = self._data.get(key)
        if value is None:
            raise ValueError(f"Key '{key}' not found")
        try:
            return int(value)
        except (ValueError, TypeError):
            raise ValueError(f"Key '{key}' is not an integer")
    
    def get_float(self, key: str) -> float:
        """Get configuration value as float"""
        value = self._data.get(key)
        if value is None:
            raise ValueError(f"Key '{key}' not found")
        try:
            return float(value)
        except (ValueError, TypeError):
            raise ValueError(f"Key '{key}' is not a float")
    
    def get_bool(self, key: str) -> bool:
        """Get configuration value as boolean"""
        value = self._data.get(key)
        if value is None:
            raise ValueError(f"Key '{key}' not found")
        if isinstance(value, bool):
            return value
        if isinstance(value, str):
            return value.lower() in ('true', 'yes', '1', 'on')
        return bool(value)
    
    def has(self, key: str) -> bool:
        """Check if key exists"""
        return key in self._data
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys"""
        return list(self._data.keys())
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with default"""
        return self._data.get(key, default)
    
    def __contains__(self, key: str) -> bool:
        """Check if key exists (in operator)"""
        return self.has(key)
    
    def __getitem__(self, key: str) -> Any:
        """Get item using indexing syntax"""
        if key in self._data:
            return self._data[key]
        raise KeyError(f"Key '{key}' not found")
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Set item using indexing syntax"""
        self.set(key, value)
    
    def summary(self) -> str:
        """Get configuration summary"""
        return f"Config with {len(self._data)} keys: {list(self._data.keys())}"


def load_config_from_dict(config_dict: Dict[str, Any]) -> Config:
    """Load configuration from dictionary"""
    return Config(config_dict)


def save_config_to_dict(config: Config) -> Dict[str, Any]:
    """Save configuration to dictionary"""
    return config._data.copy()


# Common preset configurations

def get_debug_config() -> Dict[str, Any]:
    """Get a configuration suitable for debugging"""
    return {
        "neuron_count": 100,
        "region_count": 2,
        "simulation_timestep": 0.01,
        "random_seed": 42,
        "working_memory_capacity": 20,
        "episodic_memory_max_episodes": 100,
        "enable_structural_plasticity": True,
        "enable_development": False,  # Speed up for debugging
        "stdp_ltp_weight": 0.05,
        "stdp_ltd_weight": 0.055,
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001,
    }


def get_performance_config() -> Dict[str, Any]:
    """Get a configuration optimized for performance"""
    return {
        "neuron_count": 5000,
        "region_count": 1,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "working_memory_capacity": 500,
        "episodic_memory_max_episodes": 5000,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
    }


def get_creative_config() -> Dict[str, Any]:
    """Get a configuration optimized for creative exploration"""
    return {
        "neuron_count": 2000,
        "region_count": 3,
        "simulation_timestep": 0.001,
        "random_seed": 12345,
        "working_memory_capacity": 200,
        "episodic_memory_max_episodes": 2000,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_reward_modulation": False,  # More exploration
        "enable_curiosity": True,
        "enable_novelty_detection": True,
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.018,
        "stdp_tau": 30.0,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002,
    }


# Helper functions

def load_config_from_env(prefix: str = "NLM_") -> Config:
    """Load configuration from environment variables with the given prefix"""
    config = Config()
    
    for key, value in os.environ.items():
        if key.startswith(prefix):
            config_key = key[len(prefix):].lower()
            
            try:
                # Try to convert to appropriate type
                if value.lower() in ("true", "false"):
                    config.set(config_key, value.lower() == "true")
                elif value.isdigit():
                    config.set(config_key, int(value))
                elif _is_float(value):
                    config.set(config_key, float(value))
                else:
                    config.set(config_key, value)
            except ValueError:
                continue
                
    return config


def _is_float(value: str) -> bool:
    """Check if string can be converted to float."""
    try:
        float(value)
        return True
    except ValueError:
        return False


# Error handling


class NLMError(Exception):
    """NLM error class"""
    pass


class ConfigValidator:
    """Configuration validator with comprehensive checking"""
    
    def __init__(self, config: Config):
        self.config = config
        self.errors = []
        self.warnings = []
        
    def validate_types(self):
        """Validate configuration types."""
        # Basic type validation
        type_checks = {
            "neuron_count": int,
            "region_count": int,
            "simulation_timestep": float,
            "random_seed": int,
            "connection_probability": float,
        }
        
        for key, expected_type in type_checks.items():
            if self.config.has(key):
                try:
                    value = self.config.get(key)
                    if not isinstance(value, expected_type):
                        self.errors.append(f"Key '{key}' has invalid type for {expected_type.__name__}")
                except Exception:
                    self.errors.append(f"Key '{key}' cannot be validated for {expected_type.__name__}")
                    
        return len(self.errors) == 0
        
    def validate_ranges(self):
        """Validate configuration value ranges."""
        range_checks = {
            "neuron_count": {"min": 10, "max": 100000},
            "region_count": {"min": 1, "max": 1000},
            "simulation_timestep": {"min": 0.0001, "max": 1.0},
            "random_seed": {"min": 0, "max": 2**31 - 1},
            "connection_probability": {"min": 0.0, "max": 1.0},
        }
        
        for key, limits in range_checks.items():
            if self.config.has(key):
                try:
                    value = self.config.get(key)
                    if value < limits["min"]:
                        self.errors.append(f"Key '{key}' value {value} < minimum {limits['min']}")
                    if value > limits["max"]:
                        self.errors.append(f"Key '{key}' value {value} > maximum {limits['max']}")
                except Exception:
                    self.errors.append(f"Key '{key}' cannot be validated for range checking")
                    
        return len(self.errors) == 0
        
    def get_report(self):
        """Get validation report."""
        return {
            "config": self.config.summary(),
            "errors": self.errors,
            "warnings": self.warnings,
            "valid": len(self.errors) == 0,
        }
        
    def validate_all(self):
        """Run all validation checks."""
        self.validate_types()
        self.validate_ranges()
        return len(self.errors) == 0


def compare_configs(config1: Config, config2: Config) -> Dict[str, Any]:
    """Compare two configurations and report differences."""
    keys1 = set(config1.get_keys())
    keys2 = set(config2.get_keys())
    
    common_keys = keys1.intersection(keys2)
    only_in_1 = keys1 - keys2
    only_in_2 = keys2 - keys1
    
    differences = {}
    for key in common_keys:
        try:
            val1 = config1.get(key)
            val2 = config2.get(key)
            if val1 != val2:
                differences[key] = {"from": val1, "to": val2}
        except:
            # Different types or values
            differences[key] = {"from": "N/A", "to": "N/A"}
            
    return {
        "only_in_first": list(only_in_1),
        "only_in_second": list(only_in_2),
        "common_keys": list(common_keys),
        "differences": differences,
        "summary": f"Config 1 has {len(config1.get_keys())} keys, Config 2 has {len(config2.get_keys())} keys",
    }