#!/usr/bin/env python3
"""
Advanced Configuration Management for NLM

Provides advanced configuration management features including:
- Configuration schema validation
- Configuration templates and presets
- Configuration comparison and diffing
- Configuration inheritance and merging
- Configuration validation with detailed error messages
- Configuration history and versioning
"""

import json
import copy
from typing import Dict, Any, List, Optional, Union
from pathlib import Path
import hashlib
import time

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not found. Some configuration features will not work.")
class ConfigSchema:
    """Configuration schema definition for validation"""
    
    def __init__(self, name: str = "NLM Configuration"):
        self.name = name
        self.fields: Dict[str, Dict[str, Any]] = {}
        self.required_fields: List[str] = []
        self.allowed_values: Dict[str, List[Any]] = {}
        self.field_types: Dict[str, type] = {}
        self.validation_rules: Dict[str, callable] = {}
    
    def add_field(self, name: str, field_type: type, required: bool = False,
                 description: str = "", allowed_values: Optional[List[Any]] = None,
                 validation_rule: Optional[callable] = None):
        """Add a configuration field to the schema"""
        self.fields[name] = {
            "type": field_type,
            "required": required,
            "description": description,
            "default": None
        }
        
        if required:
            self.required_fields.append(name)
        
        if allowed_values:
            self.allowed_values[name] = allowed_values
        
        self.field_types[name] = field_type
        
        if validation_rule:
            self.validation_rules[name] = validation_rule
    
    def validate(self, config_dict: Dict[str, Any]) -> List[str]:
        """Validate configuration against schema, return list of errors"""
        errors = []
        
        # Check required fields
        for field in self.required_fields:
            if field not in config_dict:
                errors.append(f"Missing required field: {field}")
        
        # Validate field types and values
        for field, value in config_dict.items():
            if field not in self.fields:
                errors.append(f"Unknown field: {field}")
                continue
            
            field_info = self.fields[field]
            field_type = field_info["type"]
            
            # Check type
            if not isinstance(value, field_type):
                errors.append(f"Field '{field}' must be of type {field_type.__name__}, "
                            f"got {type(value).__name__}")
            
            # Check allowed values
            if field in self.allowed_values:
                if value not in self.allowed_values[field]:
                    errors.append(f"Field '{field}' value '{value}' is not allowed. "
                                f"Allowed values: {self.allowed_values[field]}")
            
            # Run custom validation
            if field in self.validation_rules:
                try:
                    if not self.validation_rules[field](value):
                        errors.append(f"Validation failed for field '{field}'")
                except Exception as e:
                    errors.append(f"Validation error for field '{field}': {e}")
        
        return errors
class ConfigTemplate:
    """Configuration template for common NLM setups"""
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.config: Dict[str, Any] = {}
        self.tags: List[str] = []
    
    def add_config(self, config_dict: Dict[str, Any]):
        """Add configuration values to the template"""
        self.config.update(config_dict)
    
    def get_config(self) -> Dict[str, Any]:
        """Get template configuration"""
        return copy.deepcopy(self.config)
    
    def merge_with(self, base_config: Dict[str, Any]) -> Dict[str, Any]:
        """Merge template with base configuration"""
        merged = copy.deepcopy(base_config)
        merged.update(self.config)
        return merged
class ConfigManager:
    """Advanced configuration manager for NLM"""
    
    def __init__(self, schema: Optional[ConfigSchema] = None):
        self.config: Dict[str, Any] = {}
        self.schema = schema or self._get_default_schema()
        self.history: List[Dict[str, Any]] = []
        self.version = "1.0"
        self.created_at = time.time()
        self.last_modified = time.time()
    
    def _get_default_schema(self) -> ConfigSchema:
        """Get default NLM configuration schema"""
        schema = ConfigSchema("NLM Configuration")
        
        # Basic simulation parameters
        schema.add_field("neuron_count", int, True,
                        "Total number of neurons", [100, 500, 1000, 2000, 5000])
        schema.add_field("region_count", int, False,
                        "Number of neural regions", [1, 2, 3, 4, 5])
        
        # Connection parameters
        schema.add_field("connection_probability", float, False,
                        "Probability of connection between neurons", [0.01, 0.05, 0.1, 0.15, 0.2])
        schema.add_field("initial_weight_mean", float, False,
                        "Mean initial synaptic weight")
        schema.add_field("initial_weight_std", float, False,
                        "Standard deviation of initial weights")
        
        # Neuron parameters
        schema.add_field("v_thresh", float, False,
                        "Neuron threshold potential (mV)")
        schema.add_field("v_rest", float, False,
                        "Resting potential (mV)")
        schema.add_field("v_reset", float, False,
                        "Reset potential after spike (mV)")
        schema.add_field("tau_mem", float, False,
                        "Membrane time constant (ms)")
        schema.add_field("tau_ref", float, False,
                        "Refractory period (ms)")
        
        # Plasticity parameters
        schema.add_field("plasticity_stdp_enable", bool, False,
                        "Enable STDP plasticity")
        schema.add_field("plasticity_stdp_learning_rate", float, False,
                        "STDP learning rate")
        schema.add_field("plasticity_stdp_tau_plus", float, False,
                        "STDP LTP time constant (ms)")
        schema.add_field("plasticity_stdp_tau_minus", float, False,
                        "STDP LTD time constant (ms)")
        
        schema.add_field("plasticity_hebbian_enable", bool, False,
                        "Enable Hebbian plasticity")
        schema.add_field("plasticity_structural_enable", bool, False,
                        "Enable structural plasticity")
        
        # Development parameters
        schema.add_field("development_initial_plasticity", float, False,
                        "Initial plasticity rate")
        schema.add_field("development_plasticity_decay", float, False,
                        "Plasticity decay rate")
        
        # Neuromodulation parameters
        schema.add_field("neuromod_dopamine_scale", float, False,
                        "Dopamine modulation scale")
        schema.add_field("neuromod_curiosity_enable", bool, False,
                        "Enable curiosity-driven exploration")
        schema.add_field("neuromod_novelty_enable", bool, False,
                        "Enable novelty detection")
        
        # Simulation parameters
        schema.add_field("simulation_timestep", float, False,
                        "Simulation time step (ms)")
        
        # Random seed
        schema.add_field("random_seed", int, False,
                        "Random seed for reproducibility")
        
        return schema
    
    def load_from_file(self, filepath: str, validate: bool = True) -> bool:
        """Load configuration from JSON file"""
        try:
            with open(filepath, 'r') as f:
                config_dict = json.load(f)
            
            if validate:
                errors = self.schema.validate(config_dict)
                if errors:
                    print("Configuration validation failed:")
                    for error in errors:
                        print(f"  - {error}")
                    return False
            
            self._load_config(config_dict)
            print(f"Configuration loaded from {filepath}")
            return True
            
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to JSON file"""
        try:
            # Ensure directory exists
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            with open(filepath, 'w') as f:
                json.dump(self.config, f, indent=2)
            
            print(f"Configuration saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def _load_config(self, config_dict: Dict[str, Any]):
        """Load configuration dictionary into manager"""
        old_config = copy.deepcopy(self.config)
        self.config = copy.deepcopy(config_dict)
        
        # Create history entry
        history_entry = {
            "timestamp": time.time(),
            "version": self.version,
            "config": old_config,
            "changes": self._detect_changes(old_config, self.config)
        }
        self.history.append(history_entry)
        
        self.last_modified = time.time()
    
    def _detect_changes(self, old_config: Dict[str, Any], new_config: Dict[str, Any]) -> List[str]:
        """Detect changes between old and new configurations"""
        changes = []
        
        all_keys = set(old_config.keys()) | set(new_config.keys())
        
        for key in all_keys:
            old_value = old_config.get(key, "<removed>")
            new_value = new_config.get(key, "<removed>")
            
            if old_value != new_value:
                changes.append(f"{key}: {old_value} -> {new_value}")
        
        return changes
    
    def set(self, key: str, value: Any, source: str = "manual"):
        """Set a configuration value"""
        if key not in self.schema.fields:
            raise ValueError(f"Unknown configuration field: {key}")
        
        old_value = self.config.get(key)
        self.config[key] = value
        
        # Validate the new value
        errors = self.schema.validate({key: value})
        if errors:
            self.config[key] = old_value
            raise ValueError(f"Invalid value for field '{key}': {errors}")
        
        # Create history entry
        history_entry = {
            "timestamp": time.time(),
            "version": self.version,
            "config": copy.deepcopy(self.config),
            "changes": [f"{key}: {old_value} -> {value} (source: {source})"]
        }
        self.history.append(history_entry)
        
        self.last_modified = time.time()
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value"""
        return self.config.get(key, default)
    
    def get_all(self) -> Dict[str, Any]:
        """Get all configuration values"""
        return copy.deepcopy(self.config)
    
    def validate(self) -> List[str]:
        """Validate current configuration"""
        return self.schema.validate(self.config)
    
    def compare(self, other_config: Dict[str, Any]) -> Dict[str, Any]:
        """Compare current configuration with another"""
        diff = {
            "added": [],
            "removed": [],
            "modified": [],
            "unchanged": []
        }
        
        current_keys = set(self.config.keys())
        other_keys = set(other_config.keys())
        
        diff["added"] = list(other_keys - current_keys)
        diff["removed"] = list(current_keys - other_keys)
        
        for key in current_keys & other_keys:
            if self.config[key] != other_config[key]:
                diff["modified"].append(key)
            else:
                diff["unchanged"].append(key)
        
        return diff
    
    def create_config_from_template(self, template: ConfigTemplate) -> Dict[str, Any]:
        """Create configuration from a template"""
        return template.merge_with(self.config)
    
    def get_config_hash(self) -> str:
        """Get hash of current configuration"""
        config_str = json.dumps(self.config, sort_keys=True)
        return hashlib.md5(config_str.encode()).hexdigest()
    
    def get_config_history(self) -> List[Dict[str, Any]]:
        """Get configuration history"""
        return copy.deepcopy(self.history)
    
    def revert_to_version(self, timestamp: float) -> bool:
        """Revert to configuration at given timestamp"""
        for entry in reversed(self.history):
            if entry["timestamp"] <= timestamp:
                self.config = copy.deepcopy(entry["config"])
                self.last_modified = entry["timestamp"]
                print(f"Reverted to configuration from {time.ctime(entry['timestamp'])}")
                return True
        
        print("No configuration found with that timestamp")
        return False
    
    def export_config(self) -> str:
        """Export configuration as JSON string"""
        return json.dumps(self.config, indent=2)
    
    def import_config(self, config_str: str, validate: bool = True) -> bool:
        """Import configuration from JSON string"""
        try:
            config_dict = json.loads(config_str)
            
            if validate:
                errors = self.schema.validate(config_dict)
                if errors:
                    print("Configuration validation failed:")
                    for error in errors:
                        print(f"  - {error}")
                    return False
            
            self._load_config(config_dict)
            return True
            
        except Exception as e:
            print(f"Error importing configuration: {e}")
            return False
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get configuration statistics"""
        stats = {
            "version": self.version,
            "created_at": self.created_at,
            "last_modified": self.last_modified,
            "num_fields": len(self.config),
            "field_count": len(self.config),
            "has_history": len(self.history) > 0,
            "history_size": len(self.history)
        }
        
        # Field type statistics
        type_counts = {}
        for key, value in self.config.items():
            if key in self.schema.fields:
                field_type = self.schema.fields[key]["type"].__name__
                type_counts[field_type] = type_counts.get(field_type, 0) + 1
        
        stats["field_types"] = type_counts
        
        return stats
class PresetManager:
    """Manager for configuration presets"""
    
    def __init__(self, presets_dir: str = "presets"):
        self.presets_dir = Path(presets_dir)
        self.presets_dir.mkdir(exist_ok=True)
        self.presets: Dict[str, ConfigTemplate] = {}
        self._load_presets()
    
    def _load_presets(self):
        """Load presets from directory"""
        for file in self.presets_dir.glob("*.json"):
            try:
                with open(file, 'r') as f:
                    preset_data = json.load(f)
                
                template = ConfigTemplate(
                    preset_data.get("name", file.stem),
                    preset_data.get("description", "")
                )
                template.add_config(preset_data.get("config", {}))
                template.tags = preset_data.get("tags", [])
                
                self.presets[template.name] = template
                
            except Exception as e:
                print(f"Warning: Could not load preset {file}: {e}")
    
    def save_preset(self, name: str, description: str = "",
                   config: Optional[Dict[str, Any]] = None,
                   tags: Optional[List[str]] = None):
        """Save a configuration as a preset"""
        if config is None:
            print("No configuration provided to save as preset")
            return False
        
        template = ConfigTemplate(name, description)
        template.add_config(config)
        template.tags = tags or []
        
        self.presets[name] = template
        
        # Save to file
        preset_file = self.presets_dir / f"{name.lower().replace(' ', '_')}.json"
        preset_data = {
            "name": name,
            "description": description,
            "config": config,
            "tags": tags or []
        }
        
        with open(preset_file, 'w') as f:
            json.dump(preset_data, f, indent=2)
        
        print(f"Preset saved: {name}")
        return True
    
    def load_preset(self, name: str) -> Optional[ConfigTemplate]:
        """Load a preset by name"""
        return self.presets.get(name)
    
    def get_preset_names(self) -> List[str]:
        """Get all preset names"""
        return list(self.presets.keys())
    
    def get_presets_by_tag(self, tag: str) -> List[ConfigTemplate]:
        """Get presets by tag"""
        return [preset for preset in self.presets.values() if tag in preset.tags]
    
    def delete_preset(self, name: str) -> bool:
        """Delete a preset"""
        if name in self.presets:
            del self.presets[name]
            
            # Delete file
            preset_file = self.presets_dir / f"{name.lower().replace(' ', '_')}.json"
            if preset_file.exists():
                preset_file.unlink()
            
            print(f"Preset deleted: {name}")
            return True
        
        print(f"Preset not found: {name}")
        return False
class AdvancedConfig:
    """Main class for advanced configuration management"""
    
    def __init__(self):
        self.manager = ConfigManager()
        self.preset_manager = PresetManager()
        self.current_config: Optional[ConfigTemplate] = None
    
    def load(self, filepath: str) -> bool:
        """Load configuration from file"""
        return self.manager.load_from_file(filepath)
    
    def save(self, filepath: str) -> bool:
        """Save configuration to file"""
        return self.manager.save_to_file(filepath)
    
    def set(self, key: str, value: Any, source: str = "manual"):
        """Set a configuration value"""
        self.manager.set(key, value, source)
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value"""
        return self.manager.get(key, default)
    
    def validate(self) -> List[str]:
        """Validate current configuration"""
        return self.manager.validate()
    
    def create_from_preset(self, preset_name: str) -> bool:
        """Create configuration from preset"""
        preset = self.preset_manager.load_preset(preset_name)
        if preset:
            self.current_config = preset
            # Apply template to current config
            for key, value in preset.get_config().items():
                self.manager.set(key, value, f"preset:{preset_name}")
            print(f"Configuration created from preset: {preset_name}")
            return True
        else:
            print(f"Preset not found: {preset_name}")
            return False
    
    def save_preset(self, name: str, description: str = "",
                   tags: Optional[List[str]] = None) -> bool:
        """Save current configuration as preset"""
        return self.preset_manager.save_preset(
            name, description, self.manager.get_all(), tags
        )
    
    def compare_with(self, other_config: Dict[str, Any]):
        """Compare with another configuration"""
        diff = self.manager.compare(other_config)
        
        print("Configuration Comparison:")
        if diff["added"]:
            print("  Added fields:", diff["added"])
        if diff["removed"]:
            print("  Removed fields:", diff["removed"])
        if diff["modified"]:
            print("  Modified fields:", diff["modified"])
        if diff["unchanged"]:
            print("  Unchanged fields:", diff["unchanged"])
        
        return diff
    
    def get_history(self):
        """Get configuration history"""
        return self.manager.get_config_history()
    
    def revert_to_version(self, timestamp: float) -> bool:
        """Revert to configuration at timestamp"""
        return self.manager.revert_to_version(timestamp)
    
    def export(self) -> str:
        """Export configuration"""
        return self.manager.export_config()
    
    def import_config(self, config_str: str) -> bool:
        """Import configuration from string"""
        return self.manager.import_config(config_str)
    
    def get_statistics(self):
        """Get configuration statistics"""
        return self.manager.get_statistics()

# Convenience functions for easy usage
def create_default_config() -> AdvancedConfig:
    """Create a new advanced configuration"""
    return AdvancedConfig()

def load_config_from_file(filepath: str) -> AdvancedConfig:
    """Load configuration from file"""
    config = AdvancedConfig()
    config.load(filepath)
    return config

def create_quick_preset(name: str = "quick_simulation", neurons: int = 500):
    """Create a quick preset with specified neuron count"""
    preset_manager = PresetManager()
    
    config = {
        "neuron_count": neurons,
        "region_count": 1,
        "connection_probability": 0.1,
        "plasticity_stdp_enable": True,
        "neuromod_curiosity_enable": True,
        "simulation_timestep": 0.001
    }
    
    preset_manager.save_preset(
        name,
        f"Quick preset with {neurons} neurons",
        config,
        ["quick", "simulation", "fast"]
    )
    
    return preset_manager.load_preset(name)

# Example usage and presets
if __name__ == "__main__":
    # Create quick presets
    create_quick_preset("small_simulation", 100)
    create_quick_preset("medium_simulation", 500)
    create_quick_preset("large_simulation", 2000)
    
    print("Advanced Configuration Management for NLM")
    print("Available presets:")
    
    preset_manager = PresetManager()
    for name in preset_manager.get_preset_names():
        preset = preset_manager.load_preset(name)
        print(f"  - {name}: {preset.description}")
    
    print("\nUsage examples:")
    print("  config = create_default_config()")
    print("  config.load('config.json')")
    print("  config.set('neuron_count', 1000)")
    print("  config.validate()")
    print("  config.save('my_config.json')")
    print("  config.create_from_preset('quick_simulation')")
