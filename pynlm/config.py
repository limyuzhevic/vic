from typing import Dict, Any, List, Optional, Union
import json
from dataclasses import dataclass, field

# Mock jsonschema for now
try:
    import jsonschema
    from jsonschema import validate
    JSONSCHEMA_AVAILABLE = True
except ImportError:
    JSONSCHEMA_AVAILABLE = False
    def validate(instance, schema):
        return {}

@dataclass
class ParameterValidationResult:
    is_valid: bool = True
    errors: List[str] = field(default_factory=list)
    warnings: List[str] = field(default_factory=list)

class Config:
    DEFAULT_SCHEMA = {
        "properties": {
            "simulation": {
                "properties": {
                    "timestep": {"type": "number", "default": 0.001},
                    "duration": {"type": "number", "default": 100.0},
                    "neuron_count": {"type": "integer", "default": 1000},
                    "region_count": {"type": "integer", "default": 1}
                }
            },
            "plasticity": {
                "properties": {
                    "learning_rate": {"type": "number", "default": 0.01}
                }
            }
        }
    }
    
    def __init__(self, schema=None, initial_config=None, preset=None):
        self.schema = schema or self.DEFAULT_SCHEMA
        self.parameters = {}
        
        if preset:
            self.update({"preset": preset})
            
        if initial_config:
            self.update(initial_config)
    
    def has(self, key):
        return key in self.parameters
    
    def get(self, key, default=None):
        return self.parameters.get(key, default)
    
    def get_or(self, key, default):
        return self.parameters.get(key, default)
    
    def set(self, key, value):
        self.parameters[key] = value
    
    def update(self, config):
        self.parameters.update(config)
    
    def validate(self):
        result = ParameterValidationResult()
        return result
    
    def save_to_file(self, filepath):
        with open(filepath, 'w') as f:
            json.dump(self.parameters, f, indent=2)
    
    def load_from_file(self, filepath):
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        self.update(config_data)
    
    def get_keys(self):
        return list(self.parameters.keys())
    
    def clear(self):
        self.parameters.clear()
    
    def summary(self):
        if not self.parameters:
            return "Empty configuration"
        return "\n".join([f"  {k}: {v}" for k, v in sorted(self.parameters.items())])
    
    def __repr__(self):
        return f"<Config keys={len(self.parameters)}>"
    
    @staticmethod
    def create_default():
        return Config()
    
    @staticmethod
    def from_dict(config_dict):
        return Config(initial_config=config_dict)
    
    @staticmethod
    def from_preset(preset_name):
        return Config(preset=preset_name)
    
    @staticmethod
    def load_from_file(filepath):
        config = Config()
        config.load_from_file(filepath)
        return config
