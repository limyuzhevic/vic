#!/usr/bin/env python3
"""
Configuration utilities and validation for NLM

This module provides configuration management utilities including:
- Configuration validation
- Configuration schemas
- Configuration templates
- Configuration migration
- Configuration comparison
- Advanced configuration utilities
"""

import json
import os
from typing import Dict, Any, List, Optional, Callable, Union
from dataclasses import dataclass, asdict
from enum import Enum
from pathlib import Path

class ConfigValidationError(Exception):
    """Exception raised when configuration validation fails"""
    def __init__(self, message: str, field: str = "", value: Any = None):
        self.field = field
        self.value = value
        super().__init__(f"{field}: {message}" if field else message)

class ValidationRule:
    """Base class for configuration validation rules"""
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
    
    def validate(self, value: Any, field: str = "") -> List[str]:
        """Validate a value. Returns list of error messages (empty if valid)"""
        raise NotImplementedError

class RequiredRule(ValidationRule):
    """Validation rule for required fields"""
    
    def __init__(self, description: str = "Field is required"):
        super().__init__("required", description)
    
    def validate(self, value: Any, field: str = "") -> List[str]:
        errors = []
        if value is None or (isinstance(value, str) and not value.strip()):
            errors.append(f"{field}: {self.description}")
        return errors

class TypeRule(ValidationRule):
    """Validation rule for type checking"""
    
    def __init__(self, expected_type: type, description: str = ""):
        super().__init__("type", description or f"Must be of type {expected_type.__name__}")
        self.expected_type = expected_type
    
    def validate(self, value: Any, field: str = "") -> List[str]:
        errors = []
        if not isinstance(value, self.expected_type):
            errors.append(f"{field}: Expected {self.expected_type.__name__}, got {type(value).__name__}")
        return errors

class RangeRule(ValidationRule):
    """Validation rule for numeric ranges"""
    
    def __init__(self, min_value: Optional[float] = None, max_value: Optional[float] = None, description: str = ""):
        super().__init__("range", description or f"Value must be between {min_value} and {max_value}")
        self.min_value = min_value
        self.max_value = max_value
    
    def validate(self, value: Any, field: str = "") -> List[str]:
        errors = []
        if not isinstance(value, (int, float)):
            return errors
        
        if self.min_value is not None and value < self.min_value:
            errors.append(f"{field}: Value {value} is less than minimum {self.min_value}")
        
        if self.max_value is not None and value > self.max_value:
            errors.append(f"{field}: Value {value} is greater than maximum {self.max_value}")
        
        return errors

class ChoiceRule(ValidationRule):
    """Validation rule for choice from allowed values"""
    
    def __init__(self, allowed_values: List[Any], description: str = ""):
        super().__init__("choice", description or f"Value must be one of {allowed_values}")
        self.allowed_values = allowed_values
    
    def validate(self, value: Any, field: str = "") -> List[str]:
        errors = []
        if value not in self.allowed_values:
            errors.append(f"{field}: Value '{value}' is not in allowed values {self.allowed_values}")
        return errors

class ListRule(ValidationRule):
    """Validation rule for list items"""
    
    def __init__(self, item_rule: ValidationRule, min_length: Optional[int] = None, max_length: Optional[int] = None, description: str = ""):
        super().__init__("list", description or "Invalid list")
        self.item_rule = item_rule
        self.min_length = min_length
        self.max_length = max_length
    
    def validate(self, value: Any, field: str = "") -> List[str]:
        errors = []
        if not isinstance(value, list):
            errors.append(f"{field}: Expected list, got {type(value).__name__}")
            return errors
        
        # Check length constraints
        if self.min_length is not None and len(value) < self.min_length:
            errors.append(f"{field}: List has {len(value)} items, minimum is {self.min_length}")
        
        if self.max_length is not None and len(value) > self.max_length:
            errors.append(f"{field}: List has {len(value)} items, maximum is {self.max_length}")
        
        # Validate each item
        for i, item in enumerate(value):
            item_errors = self.item_rule.validate(item, f"{field}[{i}]")
            errors.extend(item_errors)
        
        return errors

class ConfigSchema:
    """Configuration schema definition"""
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.fields: Dict[str, Dict[str, Any]] = {}
        self.required_fields: List[str] = []
    
    def add_field(self, name: str, field_type: type, required: bool = False, 
                  description: str = "", validation_rules: Optional[List[ValidationRule]] = None,
                  default_value: Any = None):
        """Add a field to the schema"""
        field_def = {
            'type': field_type,
            'required': required,
            'description': description,
            'validation_rules': validation_rules or [],
            'default_value': default_value
        }
        
        self.fields[name] = field_def
        if required:
            self.required_fields.append(name)
        
        return self
    
    def add_int_field(self, name: str, required: bool = False, min_value: Optional[int] = None, 
                      max_value: Optional[int] = None, description: str = "", default_value: Optional[int] = None):
        """Add an integer field with optional range validation"""
        rules = []
        if min_value is not None or max_value is not None:
            rules.append(RangeRule(min_value, max_value))
        
        return self.add_field(name, int, required, description, rules, default_value)
    
    def add_float_field(self, name: str, required: bool = False, min_value: Optional[float] = None, 
                        max_value: Optional[float] = None, description: str = "", default_value: Optional[float] = None):
        """Add a float field with optional range validation"""
        rules = []
        if min_value is not None or max_value is not None:
            rules.append(RangeRule(min_value, max_value))
        
        return self.add_field(name, float, required, description, rules, default_value)
    
    def add_str_field(self, name: str, required: bool = False, choices: Optional[List[str]] = None,
                      description: str = "", default_value: Optional[str] = None):
        """Add a string field with optional choice validation"""
        rules = []
        if choices:
            rules.append(ChoiceRule(choices))
        
        return self.add_field(name, str, required, description, rules, default_value)
    
    def add_bool_field(self, name: str, required: bool = False, description: str = "", default_value: Optional[bool] = None):
        """Add a boolean field"""
        return self.add_field(name, bool, required, description, [], default_value)
    
    def add_list_field(self, name: str, item_type: type, required: bool = False,
                       min_length: Optional[int] = None, max_length: Optional[int] = None,
                       description: str = "", default_value: Optional[List[Any]] = None):
        """Add a list field with item type validation"""
        item_rule = TypeRule(item_type)
        rules = ListRule(item_rule, min_length, max_length)
        
        return self.add_field(name, list, required, description, [rules], default_value)
    
    def validate_config(self, config: Dict[str, Any], strict: bool = True) -> List[ConfigValidationError]:
        """Validate a configuration against the schema"""
        errors = []
        
        # Check required fields
        if strict:
            for field_name in self.required_fields:
                if field_name not in config:
                    errors.append(ConfigValidationError(
                        f"Missing required field", field_name
                    ))
        
        # Validate all fields
        for field_name, field_def in self.fields.items():
            if field_name not in config:
                if field_def.get('default_value') is not None:
                    continue  # Use default value
                elif strict:
                    errors.append(ConfigValidationError(
                        f"Missing optional field", field_name
                    ))
                continue
            
            value = config[field_name]
            
            # Type validation
            expected_type = field_def['type']
            if not isinstance(value, expected_type):
                errors.append(ConfigValidationError(
                    f"Expected {expected_type.__name__}, got {type(value).__name__}",
                    field_name,
                    value
                ))
                continue  # Skip further validation if type is wrong
            
            # Custom validation rules
            for rule in field_def['validation_rules']:
                try:
                    rule_errors = rule.validate(value, field_name)
                    for error_msg in rule_errors:
                        # Extract the actual error message from the rule
                        if error_msg.startswith(f"{field_name}: "):
                            error_msg = error_msg[len(f"{field_name}: "):]
                        errors.append(ConfigValidationError(error_msg, field_name, value))
                except Exception as e:
                    errors.append(ConfigValidationError(
                        f"Validation rule error: {str(e)}",
                        field_name,
                        value
                    ))
        
        return errors

class ConfigValidator:
    """Configuration validator with predefined schemas"""
    
    def __init__(self):
        self.schemas: Dict[str, ConfigSchema] = {}
        self.validators: Dict[str, Callable[[Dict[str, Any]], List[ConfigValidationError]]] = {}
    
    def register_schema(self, name: str, schema: ConfigSchema):
        """Register a configuration schema"""
        self.schemas[name] = schema
    
    def register_validator(self, name: str, validator: Callable[[Dict[str, Any]], List[ConfigValidationError]]):
        """Register a custom validator function"""
        self.validators[name] = validator
    
    def validate_with_schema(self, name: str, config: Dict[str, Any], strict: bool = True) -> List[ConfigValidationError]:
        """Validate configuration against named schema"""
        if name in self.schemas:
            return self.schemas[name].validate_config(config, strict)
        elif name in self.validators:
            return self.validators[name](config)
        else:
            raise ValueError(f"Unknown schema or validator: {name}")
    
    def validate_config(self, config: Dict[str, Any], schema_name: str = "default") -> List[ConfigValidationError]:
        """Validate configuration with default schema"""
        return self.validate_with_schema(schema_name, config)
    
    def get_schema_info(self, name: str) -> Optional[Dict[str, Any]]:
        """Get information about a registered schema"""
        if name in self.schemas:
            schema = self.schemas[name]
            return {
                'name': schema.name,
                'description': schema.description,
                'fields': schema.fields,
                'required_fields': schema.required_fields
            }
        return None

class ConfigTemplate:
    """Configuration template with variables and inheritance"""
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.template: Dict[str, Any] = {}
        self.variables: Dict[str, Any] = {}
        self.parent_templates: List[str] = []
    
    def set_template(self, template: Dict[str, Any]):
        """Set the template structure"""
        self.template = template
    
    def add_variable(self, name: str, value: Any, description: str = ""):
        """Add a variable with optional description"""
        self.variables[name] = {
            'value': value,
            'description': description
        }
    
    def set_parent(self, parent_name: str):
        """Set parent template for inheritance"""
        self.parent_templates.append(parent_name)
    
    def apply_variables(self, variables: Dict[str, Any]) -> Dict[str, Any]:
        """Apply variables to template"""
        # Start with template
        config = self.template.copy()
        
        # Override with provided variables
        for key, value in variables.items():
            config[key] = value
        
        return config
    
    def validate_template(self, config: Dict[str, Any]) -> List[ConfigValidationError]:
        """Validate configuration against template structure"""
        # Basic structure validation - can be extended
        errors = []
        
        for field_name, field_value in config.items():
            # Check if field is in template
            if field_name not in self.template:
                # Check if it's a variable
                if field_name not in self.variables:
                    errors.append(ConfigValidationError(
                        f"Unknown field '{field_name}' in template",
                        field_name,
                        field_value
                    ))
        
        return errors

class ConfigMigrator:
    """Configuration migration and version management"""
    
    def __init__(self, name: str = "nlm_config"):
        self.name = name
        self.versions: Dict[str, Dict[str, Any]] = {}
        self.current_version = "1.0.0"
    
    def add_version(self, version: str, schema: Dict[str, Any], migration_rules: Optional[List[Callable]] = None):
        """Add a configuration version"""
        self.versions[version] = {
            'schema': schema,
            'migration_rules': migration_rules or []
        }
    
    def migrate_config(self, source_version: str, target_version: str, config: Dict[str, Any]) -> Dict[str, Any]:
        """Migrate configuration from source version to target version"""
        if source_version == target_version:
            return config
        
        if source_version not in self.versions:
            raise ValueError(f"Unknown source version: {source_version}")
        
        if target_version not in self.versions:
            raise ValueError(f"Unknown target version: {target_version}")
        
        # Apply migration rules
        migrated_config = config.copy()
        
        # Get migration rules from intermediate versions
        source_schema = self.versions[source_version]['schema']
        target_schema = self.versions[target_version]['schema']
        
        # Apply simple migration: add new fields with defaults, remove deprecated fields
        for field_name, field_def in target_schema.items():
            if field_name not in migrated_config and 'default' in field_def:
                migrated_config[field_name] = field_def['default']
        
        # Remove deprecated fields
        for field_name in source_schema:
            if field_name not in target_schema:
                if field_name in migrated_config:
                    del migrated_config[field_name]
        
        return migrated_config
    
    def validate_version_compatibility(self, version: str) -> List[ConfigValidationError]:
        """Validate version compatibility"""
        errors = []
        
        if version not in self.versions:
            errors.append(ConfigValidationError(f"Unknown version: {version}"))
            return errors
        
        # Basic schema validation can be added here
        return errors

class ConfigComparator:
    """Configuration comparison and diffing"""
    
    @staticmethod
    def compare_configs(config1: Dict[str, Any], config2: Dict[str, Any]) -> Dict[str, Any]:
        """Compare two configurations and return differences"""
        diff = {
            'added': {},
            'removed': {},
            'changed': {},
            'common': {}
        }
        
        # Get all keys from both configs
        all_keys = set(config1.keys()) | set(config2.keys())
        
        for key in all_keys:
            if key in config1 and key in config2:
                # Both configs have this key
                if config1[key] != config2[key]:
                    # Values differ
                    diff['changed'][key] = {
                        'from': config1[key],
                        'to': config2[key]
                    }
                else:
                    # Values are the same
                    diff['common'][key] = config1[key]
            elif key in config1:
                # Key only in first config
                diff['removed'][key] = config1[key]
            else:
                # Key only in second config
                diff['added'][key] = config2[key]
        
        return diff
    
    @staticmethod
    def format_diff(diff: Dict[str, Any], format_type: str = "text") -> str:
        """Format diff in specified format"""
        if format_type == "text":
            lines = ["Configuration Differences:"]
            
            if diff['added']:
                lines.append("\nAdded keys:")
                for key, value in diff['added'].items():
                    lines.append(f"  + {key}: {value}")
            
            if diff['removed']:
                lines.append("\nRemoved keys:")
                for key, value in diff['removed'].items():
                    lines.append(f"  - {key}: {value}")
            
            if diff['changed']:
                lines.append("\nChanged keys:")
                for key, change in diff['changed'].items():
                    lines.append(f"  ~ {key}:")
                    lines.append(f"    From: {change['from']}")
                    lines.append(f"    To:   {change['to']}")
            
            return "\n".join(lines)
        
        elif format_type == "json":
            return json.dumps(diff, indent=2)
        
        else:
            return f"Diff: {diff}"

class ConfigManager:
    """Main configuration manager with all utilities"""
    
    def __init__(self, name: str = "nlm_config"):
        self.name = name
        self.validator = ConfigValidator()
        self.migrator = ConfigMigrator(name)
        self.comparator = ConfigComparator()
        self.templates: Dict[str, ConfigTemplate] = {}
        self.configs: Dict[str, Dict[str, Any]] = {}
        
        # Register default schemas
        self._register_default_schemas()
    
    def _register_default_schemas(self):
        """Register default configuration schemas"""
        # Default NLM configuration schema
        default_schema = ConfigSchema("nlm_default", "Default NLM configuration schema")
        
        default_schema.add_int_field("world_width", True, min_value=1, max_value=100, 
                                    description="World width in units")
        default_schema.add_int_field("world_height", True, min_value=1, max_value=100,
                                    description="World height in units")
        default_schema.add_int_field("vision_width", False, min_value=1, max_value=50,
                                    description="Vision width in pixels", default_value=8)
        default_schema.add_int_field("vision_height", False, min_value=1, max_value=50,
                                    description="Vision height in pixels", default_value=8)
        default_schema.add_float_field("timestep", False, min_value=0.001, max_value=1.0,
                                     description="Simulation timestep in seconds", default_value=0.05)
        default_schema.add_int_field("max_steps", False, min_value=1, max_value=10000,
                                    description="Maximum simulation steps", default_value=1000)
        default_schema.add_int_field("random_seed", False, min_value=0, max_value=2**31 - 1,
                                    description="Random seed for reproducibility", default_value=None)
        default_schema.add_bool_field("enable_reward_modulation", False,
                                    description="Enable reward-based neuromodulation", default_value=True)
        default_schema.add_bool_field("enable_structural_plasticity", False,
                                    description="Enable structural plasticity", default_value=False)
        default_schema.add_bool_field("enable_development", False,
                                    description="Enable developmental stages", default_value=True)
        default_schema.add_bool_field("enable_curiosity", False,
                                    description="Enable curiosity-driven exploration", default_value=True)
        default_schema.add_float_field("start_x", False, min_value=0, max_value=1000,
                                    description="Agent start X position", default_value=2.0)
        default_schema.add_float_field("start_y", False, min_value=0, max_value=1000,
                                    description="Agent start Y position", default_value=2.0)
        default_schema.add_int_field("log_interval", False, min_value=1, max_value=10000,
                                    description="Log interval in steps", default_value=50)
        
        self.validator.register_schema("default", default_schema)
    
    def create_config(self, name: str, config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Create a new configuration"""
        if config is None:
            config = {}
        
        # Validate configuration
        errors = self.validator.validate_config(config, "default")
        if errors:
            error_messages = [str(error) for error in errors]
            raise ValueError(f"Configuration validation failed: {', '.join(error_messages)}")
        
        self.configs[name] = config
        return config.copy()
    
    def get_config(self, name: str) -> Optional[Dict[str, Any]]:
        """Get configuration by name"""
        return self.configs.get(name)
    
    def update_config(self, name: str, config: Dict[str, Any]) -> Dict[str, Any]:
        """Update existing configuration"""
        if name not in self.configs:
            raise ValueError(f"Configuration '{name}' not found")
        
        # Validate configuration
        errors = self.validator.validate_config(config, "default")
        if errors:
            error_messages = [str(error) for error in errors]
            raise ValueError(f"Configuration validation failed: {', '.join(error_messages)}")
        
        self.configs[name] = config
        return config.copy()
    
    def validate_config(self, name: str) -> List[ConfigValidationError]:
        """Validate existing configuration"""
        if name not in self.configs:
            raise ValueError(f"Configuration '{name}' not found")
        
        return self.validator.validate_config(self.configs[name], "default")
    
    def apply_template(self, template_name: str, variables: Dict[str, Any]) -> Dict[str, Any]:
        """Apply template with variables"""
        if template_name not in self.templates:
            raise ValueError(f"Template '{template_name}' not found")
        
        template = self.templates[template_name]
        config = template.apply_variables(variables)
        
        # Validate template
        errors = template.validate_template(config)
        if errors:
            error_messages = [str(error) for error in errors]
            raise ValueError(f"Template validation failed: {', '.join(error_messages)}")
        
        return config
    
    def save_config(self, name: str, filepath: str):
        """Save configuration to file"""
        if name not in self.configs:
            raise ValueError(f"Configuration '{name}' not found")
        
        Path(filepath).parent.mkdir(parents=True, exist_ok=True)
        
        with open(filepath, 'w') as f:
            json.dump(self.configs[name], f, indent=2)
        
        print(f"Configuration '{name}' saved to {filepath}")
    
    def load_config(self, name: str, filepath: str) -> Dict[str, Any]:
        """Load configuration from file"""
        with open(filepath, 'r') as f:
            config = json.load(f)
        
        self.create_config(name, config)
        print(f"Configuration '{name}' loaded from {filepath}")
        return config
    
    def compare_configs(self, name1: str, name2: str) -> Dict[str, Any]:
        """Compare two configurations"""
        if name1 not in self.configs:
            raise ValueError(f"Configuration '{name1}' not found")
        
        if name2 not in self.configs:
            raise ValueError(f"Configuration '{name2}' not found")
        
        return self.comparator.compare_configs(self.configs[name1], self.configs[name2])
    
    def export_config(self, name: str) -> Dict[str, Any]:
        """Export configuration as dictionary"""
        if name not in self.configs:
            raise ValueError(f"Configuration '{name}' not found")
        
        return self.configs[name].copy()
    
    def import_config(self, name: str, config: Dict[str, Any]):
        """Import configuration from dictionary"""
        self.create_config(name, config)
    
    def list_configs(self) -> List[str]:
        """List all configuration names"""
        return list(self.configs.keys())
    
    def list_schemas(self) -> List[str]:
        """List all registered schemas"""
        return list(self.validator.schemas.keys())
    
    def list_templates(self) -> List[str]:
        """List all templates"""
        return list(self.templates.keys())

# Convenience functions
def create_default_config() -> Dict[str, Any]:
    """Create a default NLM configuration"""
    config_manager = ConfigManager()
    return config_manager.create_config("default")

def validate_nlm_config(config: Dict[str, Any]) -> List[ConfigValidationError]:
    """Validate NLM configuration"""
    config_manager = ConfigManager()
    return config_manager.validator.validate_config(config, "default")

def create_scenario_config(scenario: str) -> Dict[str, Any]:
    """Create scenario-specific configuration"""
    config = create_default_config()
    
    if scenario == "phase3":
        config.update({
            'world_width': 30,
            'world_height': 30,
            'vision_width': 8,
            'vision_height': 8,
            'max_steps': 500,
            'enable_reward_modulation': True,
            'enable_development': True,
            'enable_curiosity': False,
            'enable_structural_plasticity': False,
            'log_interval': 100
        })
    
    elif scenario == "phase4":
        config.update({
            'world_width': 40,
            'world_height': 40,
            'vision_width': 12,
            'vision_height': 12,
            'max_steps': 800,
            'enable_reward_modulation': True,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_structural_plasticity': True,
            'log_interval': 100
        })
    
    elif scenario == "phase6":
        config.update({
            'world_width': 50,
            'world_height': 50,
            'vision_width': 15,
            'vision_height': 15,
            'timestep': 0.03,
            'max_steps': 1200,
            'enable_reward_modulation': True,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_structural_plasticity': True,
            'log_interval': 100
        })
    
    return config

def example_usage():
    """Example showing how to use the configuration utilities"""
    print("=== Example: Configuration Utilities Usage ===")
    
    # Create configuration manager
    config_manager = ConfigManager()
    
    # Create a configuration
    config = config_manager.create_config("my_config", {
        "world_width": 30,
        "world_height": 30,
        "vision_width": 8,
        "vision_height": 8,
        "timestep": 0.05,
        "max_steps": 500
    })
    
    print(f"Created configuration: {config}")
    
    # Validate configuration
    errors = config_manager.validate_config("my_config")
    if errors:
        print(f"Validation errors: {errors}")
    else:
        print("Configuration is valid!")
    
    # Update configuration
    config_manager.update_config("my_config", {
        "world_width": 40,
        "world_height": 40,
        "max_steps": 1000
    })
    
    print(f"Updated configuration: {config_manager.get_config('my_config')}")
    
    # Save configuration
    config_manager.save_config("my_config", "configs/my_config.json")
    
    # Load configuration
    loaded_config = config_manager.load_config("loaded_config", "configs/my_config.json")
    print(f"Loaded configuration: {loaded_config}")
    
    # Create scenario configuration
    phase4_config = create_scenario_config("phase4")
    print(f"Phase 4 configuration: {phase4_config}")
    
    # Validate scenario configuration
    scenario_errors = validate_nlm_config(phase4_config)
    if scenario_errors:
        print(f"Scenario validation errors: {scenario_errors}")
    else:
        print("Scenario configuration is valid!")
    
    print("\nConfiguration utilities complete!")
    print("\nKey features:")
    print("1. Schema-based validation")
    print("2. Template-based configuration")
    print("3. Configuration migration and versioning")
    print("4. Configuration comparison and diffing")
    print("5. File-based configuration storage")
    print("6. Scenario-specific configurations")
    print("7. Comprehensive error reporting")

if __name__ == "__main__":
    example_usage()