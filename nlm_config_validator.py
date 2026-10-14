# NLM Configuration Validator
# This file provides validation functions for NLM configuration files

import json
import os
from typing import Dict, Any, List, Optional
from pathlib import Path

class NLMConfigValidator:
    """Validates NLM configuration files for Phase 6 experiments."""
    
    # Default configuration schema
    DEFAULT_SCHEMA = {
        "simulation_parameters": {
            "max_simulation_steps": {"type": "int", "min": 1, "max": 1000000},
            "simulation_time_limit": {"type": "float", "min": 0.0},
            "simulation_timestep": {"type": "float", "min": 0.001, "max": 1.0},
            "random_seed": {"type": "int", "min": 0},
            "neuron_count": {"type": "int", "min": 1, "max": 100000},
            "region_count": {"type": "int", "min": 1, "max": 100},
            "connection_probability": {"type": "float", "min": 0.0, "max": 1.0}
        },
        "brain_architecture": {
            "enable_plasticity": {"type": "bool"},
            "stdp_ltp_weight": {"type": "float", "min": 0.0, "max": 1.0},
            "stdp_ltd_weight": {"type": "float", "min": 0.0, "max": 1.0},
            "stdp_time_constant": {"type": "float", "min": 1.0},
            "synaptogenesis_rate": {"type": "float", "min": 0.0, "max": 1.0},
            "pruning_rate": {"type": "float", "min": 0.0, "max": 1.0}
        },
        "memory_systems": {
            "working_memory_enabled": {"type": "bool"},
            "working_memory_capacity": {"type": "int", "min": 1},
            "episodic_memory_enabled": {"type": "bool"},
            "episodic_memory_capacity": {"type": "int", "min": 1},
            "semantic_memory_enabled": {"type": "bool"},
            "procedural_memory_enabled": {"type": "bool"},
            "replay_enabled": {"type": "bool"},
            "replay_interval": {"type": "int", "min": 1},
            "replay_strength": {"type": "float", "min": 0.0, "max": 1.0},
            "consolidation_enabled": {"type": "bool"},
            "consolidation_interval": {"type": "int", "min": 1}
        },
        "neuromodulation": {
            "dopamine_enabled": {"type": "bool"},
            "dopamine_baseline": {"type": "float", "min": 0.0, "max": 1.0},
            "dopamine_range": {"type": "float", "min": 0.0, "max": 1.0},
            "curiosity_enabled": {"type": "bool"},
            "novelty_enabled": {"type": "bool"},
            "serotonin_enabled": {"type": "bool"},
            "norepinephrine_enabled": {"type": "bool"},
            "acetylcholine_enabled": {"type": "bool"}
        },
        "development": {
            "development_enabled": {"type": "bool"},
            "enable_critical_periods": {"type": "bool"},
            "enable_maturation": {"type": "bool"},
            "enable_aging": {"type": "bool"},
            "plasticity_decay_rate": {"type": "float", "min": 0.0, "max": 1.0},
            "synapse_scaling_enabled": {"type": "bool"}
        }
    }
    
    def __init__(self):
        self.errors = []
        self.warnings = []
    
    def validate_json_config(self, config_path: str) -> Dict[str, Any]:
        """Validate a JSON configuration file for Phase 6 experiments."""
        try:
            with open(config_path, 'r') as f:
                config = json.load(f)
            
            return self._validate_config_dict(config, config_path)
        except json.JSONDecodeError as e:
            self.errors.append(f"Invalid JSON in {config_path}: {e}")
            return {}
        except Exception as e:
            self.errors.append(f"Error reading {config_path}: {e}")
            return {}
    
    def validate_cfg_config(self, config_path: str) -> Dict[str, Any]:
        """Validate a .cfg configuration file for Phase 6 experiments."""
        config = {}
        try:
            with open(config_path, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#'):
                        if '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            
                            # Try to convert to appropriate type
                            config[key] = self._convert_value(value)
        
        except Exception as e:
            self.errors.append(f"Error reading {config_path}: {e}")
            return {}
        
        return config
    
    def _validate_config_dict(self, config: Dict[str, Any], source: str) -> Dict[str, Any]:
        """Validate configuration dictionary against schema."""
        validated = {}
        
        for section, schema in self.DEFAULT_SCHEMA.items():
            if section in config:
                section_config = config[section]
                validated[section] = self._validate_section(section_config, schema, section)
            else:
                self.warnings.append(f"Section '{section}' not found in {source}")
                # Use defaults for missing sections
                validated[section] = self._get_default_section(section)
        
        return validated
    
    def _validate_section(self, section_config: Dict[str, Any], schema: Dict[str, Any], section_name: str) -> Dict[str, Any]:
        """Validate individual section against its schema."""
        validated = {}
        
        for key, schema_def in schema.items():
            if key in section_config:
                value = section_config[key]
                
                # Type validation
                if not self._validate_type(value, schema_def.get("type")):
                    self.errors.append(f"Invalid type for {section_name}.{key}: expected {schema_def.get('type')}, got {type(value)}")
                    continue
                
                # Range validation
                if "min" in schema_def and value < schema_def["min"]:
                    self.errors.append(f"Value for {section_name}.{key} too small: minimum {schema_def['min']}, got {value}")
                    continue
                
                if "max" in schema_def and value > schema_def["max"]:
                    self.errors.append(f"Value for {section_name}.{key} too large: maximum {schema_def['max']}, got {value}")
                    continue
                
                validated[key] = value
            else:
                self.warnings.append(f"Key '{key}' not found in section '{section_name}'")
                # Use default value
                validated[key] = self._get_default_value(schema_def)
        
        return validated
    
    def _validate_type(self, value: Any, expected_type: Optional[str]) -> bool:
        """Validate value type against expected type."""
        if not expected_type:
            return True
        
        type_mapping = {
            "int": int,
            "float": (float, int),  # int is also a float
            "bool": bool,
            "str": str
        }
        
        return isinstance(value, type_mapping.get(expected_type, object))
    
    def _convert_value(self, value: str) -> Any:
        """Convert string value to appropriate type."""
        value = value.strip()
        
        # Try boolean
        if value.lower() in ['true', 'false']:
            return value.lower() == 'true'
        
        # Try integer
        try:
            return int(value)
        except ValueError:
            pass
        
        # Try float
        try:
            return float(value)
        except ValueError:
            pass
        
        # Return as string
        return value
    
    def _get_default_section(self, section_name: str) -> Dict[str, Any]:
        """Get default values for a section."""
        defaults = {}
        if section_name in self.DEFAULT_SCHEMA:
            for key, schema in self.DEFAULT_SCHEMA[section_name].items():
                defaults[key] = self._get_default_value(schema)
        return defaults
    
    def _get_default_value(self, schema_def: Dict[str, Any]) -> Any:
        """Get default value based on schema definition."""
        type_map = {
            "int": 0,
            "float": 0.0,
            "bool": False,
            "str": ""
        }
        
        return type_map.get(schema_def.get("type"), None)
    
    def get_validation_report(self) -> Dict[str, List[str]]:
        """Get validation report with errors and warnings."""
        return {
            "errors": self.errors,
            "warnings": self.warnings
        }
    
    def is_valid(self) -> bool:
        """Check if configuration is valid."""
        return len(self.errors) == 0
class NLMConfigManager:
    """Manages NLM configuration files for different experiment phases."""
    
    def __init__(self, configs_dir: str = "configs"):
        self.configs_dir = Path(configs_dir)
        self.validator = NLMConfigValidator()
    
    def load_phase6_config(self, config_path: Optional[str] = None) -> Dict[str, Any]:
        """Load Phase 6 configuration with validation."""
        if config_path is None:
            config_path = str(self.configs_dir / "phase6_experiments.json")
        
        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Phase 6 configuration not found: {config_path}")
        
        # Determine file type by extension
        if config_path.endswith('.json'):
            config = self.validator.validate_json_config(config_path)
        elif config_path.endswith('.cfg'):
            config = self.validator.validate_cfg_config(config_path)
        else:
            raise ValueError(f"Unsupported configuration file format: {config_path}")
        
        if not self.validator.is_valid():
            report = self.validator.get_validation_report()
            raise ValueError(f"Invalid configuration: {report}")
        
        return config
    
    def create_default_phase6_config(self, output_path: str) -> None:
        """Create default Phase 6 configuration file."""
        default_config = {
            "experiment_id": "phase6_neural_brain",
            "version": "6.0",
            "description": "Phase 6: Integrated Neural Learning Machine with complete brain system integration",
            
            "simulation_parameters": {
                "max_simulation_steps": 10000,
                "simulation_time_limit": 0.0,
                "simulation_timestep": 0.001,
                "random_seed": 42,
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1
            },
            
            "brain_architecture": {
                "enable_plasticity": True,
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.012,
                "stdp_time_constant": 20.0,
                "synaptogenesis_rate": 0.001,
                "pruning_rate": 0.0001
            },
            
            "memory_systems": {
                "working_memory_enabled": True,
                "working_memory_capacity": 1000,
                "episodic_memory_enabled": True,
                "episodic_memory_capacity": 5000,
                "semantic_memory_enabled": True,
                "procedural_memory_enabled": True,
                "replay_enabled": True,
                "replay_interval": 100,
                "replay_strength": 0.8,
                "consolidation_enabled": True,
                "consolidation_interval": 1000
            },
            
            "neuromodulation": {
                "dopamine_enabled": True,
                "dopamine_baseline": 0.1,
                "dopamine_range": 1.0,
                "curiosity_enabled": True,
                "novelty_enabled": True,
                "serotonin_enabled": True,
                "norepinephrine_enabled": True,
                "acetylcholine_enabled": True
            },
            
            "development": {
                "development_enabled": True,
                "enable_critical_periods": True,
                "enable_maturation": True,
                "enable_aging": True,
                "plasticity_decay_rate": 0.01,
                "synapse_scaling_enabled": True
            }
        }
        
        with open(output_path, 'w') as f:
            json.dump(default_config, f, indent=4)
    
    def list_available_configs(self) -> List[str]:
        """List all available configuration files."""
        configs = []
        for file_path in self.configs_dir.glob("*.json"):
            configs.append(file_path.name)
        for file_path in self.configs_dir.glob("*.cfg"):
            configs.append(file_path.name)
        return sorted(configs)
# Global configuration manager instance
config_manager = NLMConfigManager()

# Convenience functions
def load_phase6_config(config_path: Optional[str] = None) -> Dict[str, Any]:
    """Load Phase 6 configuration with validation."""
    return config_manager.load_phase6_config(config_path)

def create_phase6_config(output_path: str) -> None:
    """Create default Phase 6 configuration file."""
    config_manager.create_default_phase6_config(output_path)

def list_configs() -> List[str]:
    """List all available configuration files."""
    return config_manager.list_available_configs()