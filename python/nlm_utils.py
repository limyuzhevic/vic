#!/usr/bin/env python3
"""
NLM Python Utilities
Helper module for NLM Python bindings
"""

import json
import os
from typing import Dict, List, Any, Optional

class ConfigError(Exception):
    """Configuration error"""
    pass

class ConfigSchemaError(ConfigError):
    """Configuration schema error"""
    pass

class ConfigValidationError(ConfigError):
    """Configuration validation error"""
    pass

class ValidationError:
    def __init__(self, key: str, message: str):
        self.key = key
        self.message = message
    
    def __str__(self):
        return f"[{self.key}] {self.message}"

class ConfigPreset:
    def __init__(self, name: str, category: str, description: str, parameters: Dict[str, str]):
        self.name = name
        self.category = category
        self.description = description
        self.parameters = parameters
        self.performance_score = 0.0
        self.required_features = []

class ConfigTemplate:
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
        self.schema = []
        self.default_values = {}
        self.optimization_parameters = {}

class ConfigManager:
    def __init__(self):
        self.config = None
        self.schemas = {}
        self.templates = {}
        self.presets = {}
        self.validation_errors = []
    
    def load(self, filepath: str) -> bool:
        """Load configuration from file"""
        try:
            if filepath.endswith('.json'):
                return self.import_from_json(filepath)
            elif filepath.endswith('.ini') or filepath.endswith('.cfg'):
                return self.config.loadFromFile(filepath)
            else:
                raise ConfigError(f"Unsupported file format: {filepath}")
        except Exception as e:
            raise ConfigError(f"Failed to load configuration: {e}")
    
    def save(self, filepath: str, include_defaults: bool = False, pretty: bool = False) -> bool:
        """Save configuration to file"""
        try:
            if filepath.endswith('.json'):
                return self.export_to_json(filepath, include_defaults, pretty)
            else:
                raise ConfigError(f"Unsupported export format: {filepath}")
        except Exception as e:
            raise ConfigError(f"Failed to save configuration: {e}")
    
    def validate(self) -> List[ValidationError]:
        """Validate configuration"""
        errors = []
        # Simple validation logic
        for key, value in self.config.items():
            if key in self.schemas:
                schema = self.schemas[key]
                if schema.get('required', False) and value is None:
                    errors.append(ValidationError(key, "Required parameter is missing"))
        
        self.validation_errors = errors
        return errors
    
    def get_errors(self) -> List[ValidationError]:
        """Get validation errors"""
        return self.validation_errors
    
    def is_valid(self) -> bool:
        """Check if configuration is valid"""
        return len(self.validation_errors) == 0

class ConfigBuilder:
    @staticmethod
    def create_default_config() -> ConfigManager:
        """Create default configuration"""
        config = ConfigManager()
        config.schemas = {
            "random_seed": {"type": "int", "min": 0, "max": 2147483647},
            "simulation_timestep": {"type": "double", "min": 0.0001, "max": 1.0},
            "neuron_count": {"type": "int", "min": 1, "max": 1000000},
            "log_level": {"type": "string", "allowed": ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]}
        }
        config.config = {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "log_level": "INFO"
        }
        return config
    
    @staticmethod
    def create_training_config() -> ConfigManager:
        """Create training configuration"""
        config = ConfigBuilder.create_default_config()
        config.config.update({
            "neuron_count": 2000,
            "plasticity_learning_rate": 0.05,
            "log_level": "DEBUG"
        })
        return config
    
    @staticmethod
    def create_research_config() -> ConfigManager:
        """Create research configuration"""
        config = ConfigBuilder.create_default_config()
        config.config.update({
            "neuron_count": 1000,
            "plasticity_learning_rate": 0.03,
            "log_level": "DEBUG",
            "profiling_enabled": True
        })
        return config
    
    @staticmethod
    def create_config_from_preset(preset_name: str) -> ConfigManager:
        """Create configuration from preset"""
        presets = {
            "training": ConfigBuilder.create_training_config(),
            "simulation": ConfigBuilder.create_default_config(),
            "research": ConfigBuilder.create_research_config(),
            "development": ConfigBuilder.create_default_config()
        }
        
        if preset_name in presets:
            return presets[preset_name]
        else:
            raise ConfigError(f"Unknown preset: {preset_name}")

class ConfigValidator:
    @staticmethod
    def create_schema(key: str, type: str, description: str, min_value: Optional[float] = None, 
                     max_value: Optional[float] = None, required: bool = False, 
                     allowed: Optional[List[str]] = None) -> Dict:
        """Create configuration schema"""
        schema = {
            "key": key,
            "type": type,
            "description": description,
            "required": required
        }
        
        if min_value is not None:
            schema["min"] = min_value
        if max_value is not None:
            schema["max"] = max_value
        if allowed:
            schema["allowed"] = allowed
        
        return schema
    
    @staticmethod
    def validate_advanced_config(config: ConfigManager) -> List[str]:
        """Validate advanced configuration"""
        errors = []
        if not config.is_valid():
            for error in config.get_errors():
                errors.append(f"{error.key}: {error.message}")
        
        # Additional validation
        if "neuron_count" in config.config:
            neuron_count = config.config["neuron_count"]
            if neuron_count < 1 or neuron_count > 1000000:
                errors.append(f"neuron_count must be between 1 and 1,000,000, got {neuron_count}")
        
        return errors

class MetricsCollector:
    def __init__(self):
        self.metrics = {}
    
    def collect_metrics(self, brain, step: int):
        """Collect metrics from brain simulation"""
        self.metrics[step] = {
            "firing_rate": brain.getAverageFiringRate(),
            "active_neurons": brain.getActiveNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount()
        }
    
    def get_all_metrics(self) -> Dict:
        """Get all collected metrics"""
        return self.metrics
    
    def export_to_csv(self, filename: str):
        """Export metrics to CSV file"""
        try:
            with open(filename, 'w') as f:
                f.write("step,firing_rate,active_neurons,total_neurons,total_synapses\n")
                for step, metrics in self.metrics.items():
                    f.write(f"{step},{metrics['firing_rate']},{metrics['active_neurons']},"
                           f"{metrics['total_neurons']},{metrics['total_synapses']}\n")
        except Exception as e:
            raise ConfigError(f"Failed to export metrics: {e}")

class FeatureDetector:
    @staticmethod
    def detect_features() -> Dict[str, bool]:
        """Detect available features"""
        features = {}
        
        # Check Python version compatibility
        import sys
        features["python38+"] = sys.version_info >= (3, 8)
        features["yaml_support"] = True  # pyyaml is optional
        features["json_support"] = True
        
        return features

# Convenience instances
config_manager = ConfigManager()
metrics_collector = MetricsCollector()
feature_detector = FeatureDetector()
