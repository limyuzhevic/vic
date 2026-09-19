"""
NLM Configuration Manager for Advanced Configuration Management

Provides comprehensive configuration management capabilities including:
- Configuration validation with schema checking
- Configuration groups and hierarchical organization
- Configuration inheritance and merging
- Configuration serialization/deserialization (JSON, TOML, YAML)
- Configuration monitoring and change detection
- Configuration profiles and presets
- Configuration for advanced users (validation levels, logging, debugging)
- Configuration commands and scripting support
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional, Callable, Union, Set, Tuple, Type
import threading
import time
from dataclasses import dataclass, field
from enum import Enum, auto
from abc import ABC, abstractmethod


try:
    import yaml
except ImportError:
    yaml = None

try:
    import toml
except ImportError:
    toml = None


try:
    from jsonschema import validate, ValidationError
    HAS_JSONSCHEMA = True
except ImportError:
    HAS_JSONSCHEMA = False
    ValidationError = Exception


class ValidationLevel(Enum):
    """Configuration validation levels"""
    NONE = auto()
    BASIC = auto()
    STRICT = auto()
    SCHEMA = auto()


class ConfigSource(Enum):
    """Configuration source enumeration"""
    DEFAULT = "default"
    FILE = "file"
    COMMAND_LINE = "command_line"
    ENVIRONMENT = "environment"
    RUNTIME = "runtime"
    PROFILE = "profile"


@dataclass
class ConfigChange:
    """Configuration change event"""
    key: str
    old_value: Any
    new_value: Any
    source: ConfigSource
    timestamp: float
    path: str = ""


@dataclass
class ConfigEntry:
    """Configuration entry with metadata"""
    key: str
    value: Any
    source: ConfigSource
    description: str = ""
    type: str = "string"
    required: bool = False
    validation_rules: Dict[str, Any] = field(default_factory=dict)
    groups: List[str] = field(default_factory=list)
    path: str = ""
    validation_level: ValidationLevel = ValidationLevel.NONE


class ConfigSchema:
    """Configuration schema definition"""
    
    def __init__(self, schema_dict: Dict[str, Any] = None):
        self.schema = schema_dict or {}
        self._compiled_schema = self._compile_schema()
    
    def _compile_schema(self) -> Dict[str, Any]:
        """Compile schema for validation"""
        if not HAS_JSONSCHEMA:
            return {}
        
        compiled = {
            "type": "object",
            "properties": {},
            "required": [],
            "additionalProperties": False
        }
        
        for key, rules in self.schema.get("properties", {}).items():
            prop_schema = {"type": rules.get("type", "string")}
            if "minLength" in rules:
                prop_schema["minLength"] = rules["minLength"]
            if "maxLength" in rules:
                prop_schema["maxLength"] = rules["maxLength"]
            if "pattern" in rules:
                prop_schema["pattern"] = rules["pattern"]
            if "minimum" in rules:
                prop_schema["minimum"] = rules["minimum"]
            if "maximum" in rules:
                prop_schema["maximum"] = rules["maximum"]
            if "enum" in rules:
                prop_schema["enum"] = rules["enum"]
            
            compiled["properties"][key] = prop_schema
        
        compiled["required"] = self.schema.get("required", [])
        compiled["additionalProperties"] = self.schema.get("additionalProperties", False)
        
        return compiled
    
    def validate(self, data: Dict[str, Any], level: ValidationLevel) -> List[str]:
        """Validate configuration against schema"""
        errors = []
        
        if level == ValidationLevel.NONE:
            return errors
        
        if level == ValidationLevel.SCHEMA and HAS_JSONSCHEMA:
            try:
                validate(instance=data, schema=self._compiled_schema)
            except ValidationError as e:
                errors.append(f"Schema validation error: {e.message}")
        
        if level in (ValidationLevel.BASIC, ValidationLevel.STRICT):
            for key, rules in self.schema.get("properties", {}).items():
                if key not in data:
                    if rules.get("required", False):
                        errors.append(f"Required property '{key}' is missing")
                    continue
                
                value = data[key]
                prop_type = rules.get("type", "string")
                
                if level == ValidationLevel.STRICT:
                    if not self._validate_type(value, prop_type):
                        errors.append(f"Property '{key}' must be of type {prop_type}, got {type(value).__name__}")
                    
                    if prop_type == "string":
                        if "minLength" in rules and len(str(value)) < rules["minLength"]:
                            errors.append(f"Property '{key}' must be at least {rules['minLength']} characters")
                        if "maxLength" in rules and len(str(value)) > rules["maxLength"]:
                            errors.append(f"Property '{key}' must be at most {rules['maxLength']} characters")
                        if "pattern" in rules and not self._validate_pattern(str(value), rules["pattern"]):
                            errors.append(f"Property '{key}' does not match pattern {rules['pattern']}")
                    
                    if prop_type in ("number", "integer"):
                        if "minimum" in rules and float(value) < rules["minimum"]:
                            errors.append(f"Property '{key}' must be at least {rules['minimum']}")
                        if "maximum" in rules and float(value) > rules["maximum"]:
                            errors.append(f"Property '{key}' must be at most {rules['maximum']}")
                
                if "validator" in rules:
                    try:
                        if not rules["validator"](value):
                            errors.append(f"Property '{key}' validation failed")
                    except Exception as e:
                        errors.append(f"Property '{key}' validation error: {e}")
        
        return errors
    
    def _validate_type(self, value: Any, expected_type: str) -> bool:
        """Validate value type against expected type"""
        type_map = {
            "string": str,
            "number": (int, float),
            "integer": int,
            "boolean": bool,
            "array": list,
            "object": dict,
        }
        
        expected_py_type = type_map.get(expected_type)
        if expected_py_type is None:
            return True
        
        return isinstance(value, expected_py_type)
    
    def _validate_pattern(self, value: str, pattern: str) -> bool:
        """Validate value against regex pattern"""
        import re
        try:
            return bool(re.match(pattern, value))
        except re.error:
            return True


class ConfigProfile:
    """Configuration profile"""
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.defaults: Dict[str, Any] = {}
        self.overrides: Dict[str, Any] = {}
        self.validation_level = ValidationLevel.BASIC
        self.schema = None
    
    def add_default(self, key: str, value: Any, **kwargs) -> 'ConfigProfile':
        """Add a default value for a configuration key"""
        self.defaults[key] = value
        
        entry = ConfigEntry(
            key=key,
            value=value,
            source=ConfigSource.PROFILE,
            description=kwargs.get("description", ""),
            type=kwargs.get("type", "string"),
            required=kwargs.get("required", False),
            validation_rules=kwargs.get("validation_rules", {}),
            groups=kwargs.get("groups", [])
        )
        
        return self
    
    def add_override(self, key: str, value: Any) -> 'ConfigProfile':
        """Add an override for a configuration key"""
        self.overrides[key] = value
        return self
    
    def with_validation(self, level: ValidationLevel) -> 'ConfigProfile':
        """Set validation level for this profile"""
        self.validation_level = level
        return self
    
    def with_schema(self, schema: Dict[str, Any]) -> 'ConfigProfile':
        """Set schema for this profile"""
        self.schema = ConfigSchema(schema)
        return self


class ConfigGroup:
    """Configuration group for organization"""
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.entries: Dict[str, ConfigEntry] = {}
        self.subgroups: Dict[str, 'ConfigGroup'] = {}
    
    def add_entry(self, entry: ConfigEntry) -> 'ConfigGroup':
        """Add a configuration entry to the group"""
        self.entries[entry.key] = entry
        return self
    
    def add_subgroup(self, subgroup: 'ConfigGroup') -> 'ConfigGroup':
        """Add a subgroup to this group"""
        self.subgroups[subgroup.name] = subgroup
        return self
    
    def get_all_entries(self) -> Dict[str, ConfigEntry]:
        """Get all entries in this group and its subgroups"""
        all_entries = dict(self.entries)
        
        for subgroup in self.subgroups.values():
            all_entries.update(subgroup.get_all_entries())
        
        return all_entries


class ConfigMonitor:
    """Configuration monitoring and change detection"""
    
    def __init__(self):
        self.change_callbacks: List[Callable[[ConfigChange], None]] = []
        self.config_snapshot: Dict[str, Any] = {}
        self.monitoring = False
        self.check_interval = 1.0
    
    def add_callback(self, callback: Callable[[ConfigChange], None]) -> 'ConfigMonitor':
        """Add a callback for configuration changes"""
        self.change_callbacks.append(callback)
        return self
    
    def start_monitoring(self) -> 'ConfigMonitor':
        """Start configuration monitoring"""
        self.monitoring = True
        return self
    
    def stop_monitoring(self) -> 'ConfigMonitor':
        """Stop configuration monitoring"""
        self.monitoring = False
        return self
    
    def check_changes(self, current_config: Dict[str, Any]) -> List[ConfigChange]:
        """Check for configuration changes"""
        changes = []
        
        for key, new_value in current_config.items():
            old_value = self.config_snapshot.get(key)
            
            if key not in self.config_snapshot or old_value != new_value:
                change = ConfigChange(
                    key=key,
                    old_value=old_value,
                    new_value=new_value,
                    source=ConfigSource.RUNTIME,
                    timestamp=time.time()
                )
                changes.append(change)
        
        self.config_snapshot = current_config.copy()
        
        return changes


class ConfigManager:
    """
    Comprehensive Configuration Manager for advanced configuration management.
    
    Provides:
    - Configuration validation with schema checking
    - Configuration groups and hierarchical organization
    - Configuration inheritance and merging
    - Configuration serialization/deserialization (JSON, TOML, YAML)
    - Configuration monitoring and change detection
    - Configuration profiles and presets
    - Configuration for advanced users (validation levels, logging, debugging)
    - Configuration commands and scripting support
    """
    
    def __init__(self, validation_level: ValidationLevel = ValidationLevel.BASIC,
                 enable_monitoring: bool = False, log_changes: bool = False):
        self.validation_level = validation_level
        self.log_changes = log_changes
        self.config_file: Optional[str] = None
        
        # Core configuration storage
        self._config_entries: Dict[str, ConfigEntry] = {}
        self._groups: Dict[str, ConfigGroup] = {}
        self._profiles: Dict[str, ConfigProfile] = {}
        
        # Inheritance and merging
        self._parent_configs: List['ConfigManager'] = []
        self._inherited_entries: Dict[str, ConfigEntry] = {}
        
        # Monitoring
        self._monitor = ConfigMonitor() if enable_monitoring else None
        
        # Validation
        self._schemas: Dict[str, ConfigSchema] = {}
        
        # Callbacks
        self._change_callbacks: List[Callable[[ConfigChange], None]] = []
    
    def add_group(self, group: ConfigGroup) -> 'ConfigManager':
        """Add a configuration group"""
        self._groups[group.name] = group
        return self
    
    def get_group(self, name: str) -> Optional[ConfigGroup]:
        """Get a configuration group by name"""
        return self._groups.get(name)
    
    def add_profile(self, profile: ConfigProfile) -> 'ConfigManager':
        """Add a configuration profile"""
        self._profiles[profile.name] = profile
        return self
    
    def apply_profile(self, profile_name: str) -> 'ConfigManager':
        """Apply a configuration profile"""
        if profile_name not in self._profiles:
            raise ValueError(f"Profile '{profile_name}' not found")
        
        profile = self._profiles[profile_name]
        
        # Apply defaults
        for key, value in profile.defaults.items():
            self.set(key, value, ConfigSource.PROFILE)
        
        # Apply overrides
        for key, value in profile.overrides.items():
            self.set(key, value, ConfigSource.PROFILE)
        
        # Set validation level
        self.validation_level = profile.validation_level
        
        # Apply schema
        if profile.schema:
            self._schemas[profile_name] = profile.schema
        
        return self
    
    def set_schema(self, name: str, schema: Dict[str, Any]) -> 'ConfigManager':
        """Set a configuration schema"""
        self._schemas[name] = ConfigSchema(schema)
        return self
    
    def set_parent(self, parent: 'ConfigManager') -> 'ConfigManager':
        """Set a parent configuration for inheritance"""
        self._parent_configs.append(parent)
        return self
    
    def set_parent_configs(self, parents: List['ConfigManager']) -> 'ConfigManager':
        """Set multiple parent configurations for inheritance"""
        self._parent_configs.extend(parents)
        return self
    
    def inherit_from_parents(self) -> 'ConfigManager':
        """Inherit configuration from parent configs"""
        self._inherited_entries.clear()
        
        for parent in self._parent_configs:
            for key, entry in parent.get_all_entries().items():
                if key not in self._config_entries:
                    self._inherited_entries[key] = entry
        
        return self
    
    def merge_config(self, other: 'ConfigManager', strategy: str = 'overwrite') -> 'ConfigManager':
        """Merge another configuration manager into this one"""
        for key, entry in other.get_all_entries().items():
            if strategy == 'overwrite' or key not in self._config_entries:
                self._config_entries[key] = ConfigEntry(
                    key=key,
                    value=entry.value,
                    source=entry.source,
                    description=entry.description,
                    type=entry.type,
                    required=entry.required,
                    validation_rules=entry.validation_rules,
                    groups=entry.groups,
                    path=entry.path,
                    validation_level=entry.validation_level
                )
        
        return self
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME,
            description: str = "", type_hint: str = "string",
            required: bool = False, validation_rules: Dict[str, Any] = None,
            groups: List[str] = None, validation_level: ValidationLevel = None) -> 'ConfigManager':
        """
        Set a configuration value.
        
        Args:
            key: Configuration key (dot notation for nested keys)
            value: Configuration value
            source: Configuration source
            description: Description of the configuration
            type_hint: Type hint for validation
            required: Whether the configuration is required
            validation_rules: Validation rules for the configuration
            groups: Groups to organize the configuration
            validation_level: Validation level for this key
        """
        # Validate the configuration
        errors = self._validate_config(key, value)
        if errors:
            if self.log_changes:
                for error in errors:
                    print(f"Validation error: {error}")
            raise ValueError(f"Configuration validation failed: {'; '.join(errors)}")
        
        # Create configuration entry
        entry = ConfigEntry(
            key=key,
            value=value,
            source=source,
            description=description,
            type=type_hint,
            required=required,
            validation_rules=validation_rules or {},
            groups=groups or [],
            path=self._get_config_path(key),
            validation_level=validation_level or self.validation_level
        )
        
        old_value = None
        if key in self._config_entries:
            old_value = self._config_entries[key].value
        
        self._config_entries[key] = entry
        
        # Handle monitoring
        if self._monitor:
            change = ConfigChange(
                key=key,
                old_value=old_value,
                new_value=value,
                source=source,
                timestamp=time.time(),
                path=entry.path
            )
            
            # Check for changes
            changes = self._monitor.check_changes(self._get_flat_config())
            for change in changes:
                for callback in self._monitor.change_callbacks + self._change_callbacks:
                    callback(change)
        
        return self
    
    def get(self, key: str) -> Any:
        """Get a configuration value"""
        # Check parent configs first
        if key in self._inherited_entries:
            return self._inherited_entries[key].value
        
        if key in self._config_entries:
            return self._config_entries[key].value
        
        raise KeyError(f"Configuration key '{key}' not found")
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists"""
        return key in self._config_entries or key in self._inherited_entries
    
    def get_all(self) -> Dict[str, Any]:
        """Get all configuration entries"""
        config = self._get_flat_config()
        return config
    
    def get_by_group(self, group_name: str) -> Dict[str, Any]:
        """Get configuration entries by group"""
        config = {}
        
        for entry in self._config_entries.values():
            if group_name in entry.groups:
                config[entry.key] = entry.value
        
        return config
    
    def get_all_entries(self) -> Dict[str, ConfigEntry]:
        """Get all configuration entries with metadata"""
        return {**self._inherited_entries, **self._config_entries}
    
    def get_keys(self, group: Optional[str] = None) -> List[str]:
        """Get all configuration keys, optionally filtered by group"""
        keys = list(self._config_entries.keys())
        
        if group:
            keys = [key for key in keys if key in self._config_entries 
                   and group in self._config_entries[key].groups]
        
        return keys
    
    def remove(self, key: str) -> 'ConfigManager':
        """Remove a configuration key"""
        if key in self._config_entries:
            del self._config_entries[key]
        return self
    
    def clear(self) -> 'ConfigManager':
        """Clear all configuration"""
        self._config_entries.clear()
        return self
    
    def save_to_file(self, filepath: str, format: str = 'json') -> bool:
        """Save configuration to file"""
        try:
            if format == 'json':
                return self._save_json(filepath)
            elif format == 'yaml':
                if yaml:
                    return self._save_yaml(filepath)
                else:
                    raise ImportError("PyYAML is required for YAML serialization")
            elif format == 'toml':
                if toml:
                    return self._save_toml(filepath)
                else:
                    raise ImportError("Tomli is required for TOML serialization")
            else:
                raise ValueError(f"Unsupported format: {format}")
        except Exception as e:
            if self.log_changes:
                print(f"Error saving configuration: {e}")
            return False
    
    def load_from_file(self, filepath: str, format: str = 'auto',
                       validate: bool = True) -> bool:
        """Load configuration from file"""
        try:
            if format == 'auto':
                if filepath.endswith('.json'):
                    format = 'json'
                elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
                    format = 'yaml'
                elif filepath.endswith('.toml'):
                    format = 'toml'
                else:
                    raise ValueError("Cannot auto-detect format, please specify format")
            
            if format == 'json':
                data = self._load_json(filepath)
            elif format == 'yaml':
                if yaml:
                    data = self._load_yaml(filepath)
                else:
                    raise ImportError("PyYAML is required for YAML deserialization")
            elif format == 'toml':
                if toml:
                    data = self._load_toml(filepath)
                else:
                    raise ImportError("Tomli is required for TOML deserialization")
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            if validate:
                errors = self._validate_all()
                if errors:
                    raise ValueError(f"Validation failed: {'; '.join(errors)}")
            
            self.clear()
            for key, value in data.items():
                self.set(key, value, ConfigSource.FILE)
            
            return True
        except Exception as e:
            if self.log_changes:
                print(f"Error loading configuration: {e}")
            return False
    
    def load_from_string(self, content: str, format: str = 'json',
                        validate: bool = True) -> bool:
        """Load configuration from string"""
        try:
            if format == 'json':
                data = json.loads(content)
            elif format == 'yaml':
                if yaml:
                    data = yaml.safe_load(content)
                else:
                    raise ImportError("PyYAML is required for YAML deserialization")
            elif format == 'toml':
                if toml:
                    import tomli
                    data = tomli.loads(content)
                else:
                    raise ImportError("Tomli is required for TOML deserialization")
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            if not isinstance(data, dict):
                raise ValueError(f"Configuration must be a dictionary, got {type(data).__name__}")
            
            if validate:
                errors = self._validate_all()
                if errors:
                    raise ValueError(f"Validation failed: {'; '.join(errors)}")
            
            self.clear()
            for key, value in data.items():
                self.set(key, value, ConfigSource.FILE)
            
            return True
        except Exception as e:
            if self.log_changes:
                print(f"Error loading configuration from string: {e}")
            return False
    
    def get_validation_errors(self) -> List[str]:
        """Get all validation errors"""
        return self._validate_all()
    
    def add_change_callback(self, callback: Callable[[ConfigChange], None]) -> 'ConfigManager':
        """Add a callback for configuration changes"""
        self._change_callbacks.append(callback)
        return self
    
    def set_validation_level(self, level: ValidationLevel) -> 'ConfigManager':
        """Set validation level"""
        self.validation_level = level
        return self
    
    def export_config(self, keys: List[str] = None) -> Dict[str, Any]:
        """Export configuration to dictionary"""
        config = self._get_flat_config()
        
        if keys:
            return {key: config[key] for key in keys if key in config}
        
        return config
    
    def import_config(self, config: Dict[str, Any], source: ConfigSource = ConfigSource.RUNTIME) -> 'ConfigManager':
        """Import configuration from dictionary"""
        self.clear()
        for key, value in config.items():
            self.set(key, value, source)
        return self
    
    def copy(self) -> 'ConfigManager':
        """Create a copy of the configuration manager"""
        new_manager = ConfigManager(
            validation_level=self.validation_level,
            enable_monitoring=False,
            log_changes=self.log_changes
        )
        
        new_manager._config_entries = self._config_entries.copy()
        new_manager._groups = self._groups.copy()
        new_manager._profiles = self._profiles.copy()
        new_manager._parent_configs = self._parent_configs.copy()
        new_manager._inherited_entries = self._inherited_entries.copy()
        new_manager._schemas = self._schemas.copy()
        new_manager._change_callbacks = self._change_callbacks.copy()
        
        if self._monitor:
            new_manager._monitor = ConfigMonitor()
            new_manager._monitor.change_callbacks = self._monitor.change_callbacks.copy()
            new_manager._monitor.config_snapshot = self._monitor.config_snapshot.copy()
        
        return new_manager
    
    def __repr__(self) -> str:
        """String representation"""
        return f"ConfigManager(validation_level={self.validation_level.name}, entries={len(self._config_entries)}, groups={len(self._groups)})"

    def _validate_config(self, key: str, value: Any) -> List[str]:
        """Validate a single configuration entry"""
        errors = []
        
        # Check for schemas
        schema_errors = self._validate_with_schema(key, value)
        errors.extend(schema_errors)
        
        # Basic type validation
        entry_type = self._get_entry_type(key)
        if entry_type:
            type_errors = self._validate_type(value, entry_type)
            errors.extend(type_errors)
        
        return errors
    
    def _validate_all(self) -> List[str]:
        """Validate all configuration entries"""
        errors = []
        
        for key, entry in self._config_entries.items():
            errors.extend(self._validate_config(key, entry.value))
        
        return errors
    
    def _validate_with_schema(self, key: str, value: Any) -> List[str]:
        """Validate configuration against schema"""
        errors = []
        
        # Find matching schema
        schema_name = None
        for schema_name in self._schemas:
            schema = self._schemas[schema_name]
            if self._matches_schema_criteria(key, schema_name):
                schema_errors = schema.validate({key: value}, self.validation_level)
                errors.extend(schema_errors)
                break
        
        return errors
    
    def _validate_type(self, value: Any, expected_type: str) -> List[str]:
        """Validate value type"""
        errors = []
        
        if expected_type == "string" and not isinstance(value, str):
            errors.append(f"Expected string for '{value}', got {type(value).__name__}")
        elif expected_type == "number" and not isinstance(value, (int, float)):
            errors.append(f"Expected number for '{value}', got {type(value).__name__}")
        elif expected_type == "integer" and not isinstance(value, int):
            errors.append(f"Expected integer for '{value}', got {type(value).__name__}")
        elif expected_type == "boolean" and not isinstance(value, bool):
            errors.append(f"Expected boolean for '{value}', got {type(value).__name__}")
        elif expected_type == "array" and not isinstance(value, list):
            errors.append(f"Expected array for '{value}', got {type(value).__name__}")
        elif expected_type == "object" and not isinstance(value, dict):
            errors.append(f"Expected object for '{value}', got {type(value).__name__}")
        
        return errors
    
    def _get_entry_type(self, key: str) -> Optional[str]:
        """Get the type hint for a configuration key"""
        if key in self._config_entries:
            return self._config_entries[key].type
        return None
    
    def _matches_schema_criteria(self, key: str, schema_name: str) -> bool:
        """Check if a configuration key matches schema criteria"""
        # Simple implementation - can be extended for more complex criteria
        return True
    
    def _get_config_path(self, key: str) -> str:
        """Get configuration path (group prefix)"""
        path_parts = []
        
        # Find which groups this key belongs to
        for group in self._groups.values():
            for entry in group.entries.values():
                if entry.key == key:
                    path_parts.extend(entry.groups)
        
        return "/".join(path_parts) if path_parts else ""
    
    def _get_flat_config(self) -> Dict[str, Any]:
        """Get flat configuration dictionary"""
        config = {}
        
        # Add inherited entries
        for key, entry in self._inherited_entries.items():
            config[key] = entry.value
        
        # Add current entries (overwrites inherited)
        for key, entry in self._config_entries.items():
            config[key] = entry.value
        
        return config
    
    def _save_json(self, filepath: str) -> bool:
        """Save configuration as JSON"""
        with open(filepath, 'w') as f:
            json.dump(self._get_flat_config(), f, indent=2)
        return True
    
    def _load_json(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from JSON"""
        with open(filepath, 'r') as f:
            return json.load(f)
    
    def _save_yaml(self, filepath: str) -> bool:
        """Save configuration as YAML"""
        if yaml:
            with open(filepath, 'w') as f:
                yaml.dump(self._get_flat_config(), f)
            return True
        return False
    
    def _load_yaml(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from YAML"""
        if yaml:
            with open(filepath, 'r') as f:
                return yaml.safe_load(f)
        return {}
    
    def _save_toml(self, filepath: str) -> bool:
        """Save configuration as TOML"""
        if toml:
            with open(filepath, 'w') as f:
                toml.dump(self._get_flat_config(), f)
            return True
        return False
    
    def _load_toml(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from TOML"""
        if toml:
            with open(filepath, 'r') as f:
                return toml.load(f)
        return {}


def create_default_config() -> ConfigManager:
    """Create a default configuration manager with common settings"""
    config = ConfigManager(validation_level=ValidationLevel.BASIC)
    
    # Add default brain configuration
    config.set("brain.neuron_count", 1000, 
              description="Total number of neurons in the brain",
              type_hint="integer",
              required=True,
              groups=["brain"])
    
    config.set("brain.synapse_density", 0.1,
              description="Synapse connectivity density",
              type_hint="number",
              groups=["brain"])
    
    # Add default world configuration
    config.set("world.width", 20,
              description="World width in units",
              type_hint="integer",
              groups=["world"])
    
    config.set("world.height", 20,
              description="World height in units",
              type_hint="integer",
              groups=["world"])
    
    # Add default agent configuration
    config.set("agent.enable_reward_modulation", True,
              description="Enable reward modulation learning",
              type_hint="boolean",
              groups=["agent"])
    
    config.set("agent.enable_curiosity", True,
              description="Enable curiosity-driven exploration",
              type_hint="boolean",
              groups=["agent"])
    
    return config


def create_test_config() -> ConfigManager:
    """Create a test configuration"""
    config = ConfigManager(validation_level=ValidationLevel.STRICT)
    
    # Add test-specific configuration
    config.set("test.seed", 42,
              description="Random seed for tests",
              type_hint="integer")
    
    config.set("test.mode", "integration",
              description="Test mode",
              type_hint="string",
              validation_rules={"enum": ["unit", "integration", "e2e"]})
    
    return config


def create_development_config() -> ConfigManager:
    """Create a development configuration"""
    config = ConfigManager(validation_level=ValidationLevel.STRICT, log_changes=True)
    
    # Add development-specific configuration
    config.set("debug.enabled", True,
              description="Enable debug logging",
              type_hint="boolean",
              groups=["debug"])
    
    config.set("debug.log_level", "debug",
              description="Debug log level",
              type_hint="string",
              validation_rules={"enum": ["error", "warn", "info", "debug"]})
    
    config.set("profile", "development",
              description="Development profile",
              type_hint="string")
    
    return config


if __name__ == "__main__":
    # Example usage
    print("=== NLM Configuration Manager Example ===")
    
    # Create configuration manager
    config = ConfigManager(validation_level=ValidationLevel.STRICT, log_changes=True)
    
    # Set some configuration values
    config.set("brain.neuron_count", 1000,
              description="Total number of neurons",
              type_hint="integer",
              required=True,
              groups=["brain"])
    
    config.set("world.width", 20,
              description="World width",
              type_hint="integer",
              groups=["world"])
    
    # Validate configuration
    errors = config.get_validation_errors()
    if errors:
        print("Validation errors:", errors)
    else:
        print("Configuration is valid!")
    
    # Save to file
    config.save_to_file("config.json")
    print("Saved configuration to config.json")
    
    # Load from file
    config2 = ConfigManager()
    config2.load_from_file("config.json")
    print("Loaded configuration:", config2.get_all())
    
    # Create profile
    profile = ConfigProfile("production", "Production configuration")
    profile.add_default("brain.neuron_count", 5000)
    profile.add_default("world.width", 50)
    profile.with_validation(ValidationLevel.STRICT)
    
    config.add_profile("production", profile)
    config.apply_profile("production")
    print("Applied production profile")
    
    print("\n=== Configuration Summary ===")
    print(config)