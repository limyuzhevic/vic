# Enhanced configuration management
# This module provides enhanced configuration management with validation and utilities

import json
import os
from typing import Dict, Any, List, Optional, Union
from enum import Enum
from .core import NLMError, ValidationError, ConfigError

class ConfigurationManager:
    """
    Enhanced configuration manager with validation and utilities.
    
    This class provides advanced configuration management features including:
    - Configuration validation
    - Error reporting
    - Export/import utilities
    - Schema validation
    """
    
    def __init__(self):
        """Initialize configuration manager."""
        self._config = None
        
    def create_config(self) -> 'Config':
        """
        Create a new configuration.
        
        Returns:
            Config object
        """
        from .brain import BrainSession
        # This would create actual Config object
        return Config()
        
    def load_from_file(self, filepath: str) -> 'Config':
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON configuration file
            
        Returns:
            Config object
            
        Raises:
            ConfigError: If loading fails
        """
        try:
            config = Config()
            if not config.loadFromFile(filepath):
                raise ConfigError(f"Failed to load configuration from {filepath}")
            return config
        except Exception as e:
            raise ConfigError(f"Failed to load configuration: {e}")
            
    def save_to_file(self, config: 'Config', filepath: str) -> bool:
        """
        Save configuration to JSON file.
        
        Args:
            config: Configuration to save
            filepath: Path to save file
            
        Returns:
            True if save succeeded, False otherwise
        """
        try:
            return config.saveToFile(filepath)
        except Exception as e:
            raise ConfigError(f"Failed to save configuration: {e}")
            
    def validate_config(self, config: 'Config') -> Tuple[List[str], List[str]]:
        """
        Validate configuration and return errors and warnings.
        
        Args:
            config: Configuration to validate
            
        Returns:
            Tuple of (errors, warnings)
        """
        errors = []
        warnings = []
        
        # Basic validation
        if config.has("neural.region_count") and config.get[int64_t]("neural.region_count") <= 0:
            errors.append("neural.region_count must be positive")
            
        if config.has("performance.simulation_steps") and config.get[int64_t]("performance.simulation_steps") < 0:
            errors.append("performance.simulation_steps cannot be negative")
            
        if config.has("brain.boundary_check.enabled") and not config.get[bool]("brain.boundary_check.enabled"):
            warnings.append("Boundary checking is disabled - may cause simulation errors")
            
        # Check for conflicting settings
        if (config.has("neuromodulation.dopamine.enabled") and 
            config.has("neuromodulation.curiosity.enabled") and
            config.get[bool]("neuromodulation.dopamine.enabled") and
            config.get[bool]("neuromodulation.curiosity.enabled")):
            warnings.append("Both dopamine and curiosity modulation enabled - may cause conflict")
            
        return errors, warnings
        
    def get_config_schema(self) -> Dict[str, Any]:
        """
        Get configuration schema for validation.
        
        Returns:
            Dictionary describing valid configuration keys and types
        """
        return {
            "neural": {
                "region_count": int,
                "neuron_per_region": int,
                "connection_probability": float,
            },
            "performance": {
                "simulation_steps": int,
                "optimization_level": str,
                "thread_count": int,
                "vectorization": bool,
            },
            "plasticity": {
                "stdp.enabled": bool,
                "hebbian.enabled": bool,
                "structural.enabled": bool,
            },
            "neuromodulation": {
                "dopamine.enabled": bool,
                "curiosity.enabled": bool,
                "novelty.enabled": bool,
            },
            "memory": {
                "working.enabled": bool,
                "working.capacity": int,
                "episodic.enabled": bool,
            },
            "cognition": {
                "attention.enabled": bool,
                "planning.horizon": int,
            },
            "development": {
                "enabled": bool,
                "stages": int,
            },
        }
        
    def migrate_config(self, old_config: 'Config', version_mapping: Dict[str, str]) -> 'Config':
        """
        Migrate configuration from older version.
        
        Args:
            old_config: Old configuration
            version_mapping: Mapping of old keys to new keys
            
        Returns:
            Migrated configuration
        """
        new_config = Config()
        
        # Copy all values from old config
        for key in old_config.getKeys():
            value = old_config.getAny(key)
            if key in version_mapping:
                new_config.set(version_mapping[key], value)
            else:
                new_config.set(key, value)
                
        return new_config
class ConfigPresets:
    """
    Pre-configured configuration presets for common use cases.
    
    This class provides ready-to-use configuration presets for different
    simulation scenarios and research purposes.
    """
    
    @staticmethod
    def get_default() -> Dict[str, Any]:
        """Get default configuration."""
        return {
            "neural": {
                "region_count": 5,
                "neuron_per_region": 100,
                "connection_probability": 0.1,
            },
            "performance": {
                "simulation_steps": 1000,
                "optimization_level": "medium",
                "thread_count": -1,
                "vectorization": True,
            },
            "plasticity": {
                "stdp.enabled": True,
                "hebbian.enabled": True,
                "structural.enabled": False,
            },
            "neuromodulation": {
                "dopamine.enabled": True,
                "curiosity.enabled": True,
                "novelty.enabled": False,
            },
            "memory": {
                "working.enabled": True,
                "working.capacity": 50,
                "episodic.enabled": False,
            },
            "cognition": {
                "attention.enabled": True,
                "planning.horizon": 10,
            },
            "development": {
                "enabled": True,
                "stages": 5,
            },
        }
        
    @staticmethod
    def get_research() -> Dict[str, Any]:
        """Get research configuration (more flexible, less optimized)."""
        config = ConfigPresets.get_default().copy()
        config.update({
            "performance": {
                "optimization_level": "low",
                "thread_count": 1,
                "vectorization": False,
            },
            "plasticity": {
                "structural.enabled": True,
                "stdp.enabled": False,
                "hebbian.enabled": False,
            },
            "memory": {
                "episodic.enabled": True,
            },
        })
        return config
        
    @staticmethod
    def get_optimization() -> Dict[str, Any]:
        """Get optimization configuration (maximum performance)."""
        config = ConfigPresets.get_default().copy()
        config.update({
            "performance": {
                "optimization_level": "high",
                "thread_count": -1,
                "vectorization": True,
            },
            "plasticity": {
                "stdp.enabled": True,
                "hebbian.enabled": True,
                "structural.enabled": True,
            },
            "neuromodulation": {
                "novelty.enabled": True,
            },
            "memory": {
                "working.capacity": 200,
            },
        })
        return config
        
    @staticmethod
    def get_development() -> Dict[str, Any]:
        """Get development configuration (learning focused)."""
        config = ConfigPresets.get_default().copy()
        config.update({
            "development": {
                "enabled": True,
                "stages": 10,
            },
            "plasticity": {
                "structural.enabled": True,
            },
            "cognition": {
                "planning.horizon": 20,
            },
            "memory": {
                "episodic.enabled": True,
            },
        })
        return config
        
    @staticmethod
    def get_replay() -> Dict[str, Any]:
        """Get replay configuration (memory consolidation focused)."""
        config = ConfigPresets.get_default().copy()
        config.update({
            "memory": {
                "working.enabled": False,
                "episodic.enabled": True,
            },
            "plasticity": {
                "stdp.enabled": False,
                "hebbian.enabled": False,
            },
            "development": {
                "stages": 3,
            },
        })
        return config
class ConfigurationValidator:
    """
    Advanced configuration validation utilities.
    
    This class provides comprehensive validation of configuration settings,
    including type checking, range validation, and consistency checking.
    """
    
    @staticmethod
    def validate_types(config: 'Config', schema: Dict[str, Any]) -> List[str]:
        """
        Validate configuration types against schema.
        
        Args:
            config: Configuration to validate
            schema: Schema defining expected types
            
        Returns:
            List of validation errors
        """
        errors = []
        
        for category, settings in schema.items():
            for key, expected_type in settings.items():
                full_key = f"{category}.{key}" if category else key
                if config.has(full_key):
                    value = config.getAny(full_key)
                    if not isinstance(value, expected_type):
                        errors.append(
                            f"Config '{full_key}' expects type {expected_type}, "
                            f"got {type(value)}"
                        )
                        
        return errors
        
    @staticmethod
    def validate_ranges(config: 'Config') -> List[str]:
        """
        Validate configuration ranges.
        
        Args:
            config: Configuration to validate
            
        Returns:
            List of validation errors
        """
        errors = []
        
        # Validate neural parameters
        if config.has("neural.region_count") and config.get[int64_t]("neural.region_count") < 1:
            errors.append("neural.region_count must be at least 1")
            
        if config.has("neural.neuron_per_region") and config.get[int64_t]("neural.neuron_per_region") < 1:
            errors.append("neural.neuron_per_region must be at least 1")
            
        # Validate performance parameters
        if config.has("performance.thread_count") and config.get[int64_t]("performance.thread_count") < -1:
            errors.append("performance.thread_count must be >= -1 (use all cores)")
            
        # Validate probability parameters
        if config.has("neural.connection_probability"):
            prob = config.get[float]("neural.connection_probability")
            if not 0.0 <= prob <= 1.0:
                errors.append("neural.connection_probability must be between 0.0 and 1.0")
                
        return errors
        
    @staticmethod
    def validate_consistency(config: 'Config') -> List[str]:
        """
        Validate configuration consistency.
        
        Args:
            config: Configuration to validate
            
        Returns:
            List of validation warnings
        """
        warnings = []
        
        # Check for conflicting enabled systems
        if (config.has("neuromodulation.dopamine.enabled") and
            config.has("neuromodulation.curiosity.enabled") and
            config.get[bool]("neuromodulation.dopamine.enabled") and
            config.get[bool]("neuromodulation.curiosity.enabled")):
            warnings.append("Both dopamine and curiosity modulation enabled - may cause conflict")
            
        # Check for disabled essential systems
        if (config.has("memory.working.enabled") and
            config.has("memory.episodic.enabled") and
            not config.get[bool]("memory.working.enabled") and
            not config.get[bool]("memory.episodic.enabled")):
            warnings.append("Both working and episodic memory disabled - may limit learning")
            
        # Check for optimization vs accuracy tradeoff
        if (config.has("performance.optimization_level") and
            config.get[str]("performance.optimization_level") == "high" and
            config.has("plasticity.stdp.enabled") and
            not config.get[bool]("plasticity.stdp.enabled")):
            warnings.append("High optimization with STDP disabled - may reduce learning quality")
            
        return warnings
        
    @staticmethod
    def generate_recommendations(config: 'Config') -> List[str]:
        """
        Generate configuration recommendations.
        
        Args:
            config: Configuration to analyze
            
        Returns:
            List of recommendations
        """
        recommendations = []
        
        # Performance recommendations
        if config.has("performance.thread_count") and config.get[int64_t]("performance.thread_count") == 1:
            recommendations.append("Consider using more threads for better performance (set to -1 for all cores)")
            
        # Learning recommendations
        if config.has("plasticity.stdp.enabled") and not config.get[bool]("plasticity.stdp.enabled"):
            recommendations.append("STDP is recommended for temporal learning and memory formation")
            
        # Memory recommendations
        if config.has("memory.working.enabled") and not config.get[bool]("memory.working.enabled"):
            recommendations.append("Working memory is essential for maintaining active representations")
            
        return recommendations