#!/usr/bin/env python3
"""
NLM Config Module - Configuration Management and Templates

This module provides advanced configuration management:
- Configuration management with template support
- Template-based configuration with variables and inheritance
- Configuration profiling and optimization
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Union, Set
import json
import os
import hashlib
import time
from pathlib import Path
from copy import deepcopy
from datetime import datetime

class ConfigError(Exception):
    """Configuration error."""
    
    def __init__(self, message: str, config_path: Optional[str] = None, field: Optional[str] = None):
        super().__init__(message)
        self.config_path = config_path
        self.field = field
        self.timestamp = datetime.now().isoformat()

class ConfigTemplate:
    """Template-based configuration with variables and inheritance.
    
    Provides template-based configuration management with variables,
    conditional settings, and template inheritance.
    
    Features:
    * Template variables
    * Conditional settings
    * Template inheritance
    * Template validation
    * Template parameters
    """
    
    def __init__(self):
        self.variables = {}
        self.inheritance_chain = []
        self.conditional_settings = {}
        self.parameters = {}
        self.validation_rules = {}
        self.is_valid = False
    
    def set_variable(self, name: str, value: Any, description: str = ""):
        """Set template variable.
        
        Args:
            name: Variable name
            value: Variable value
            description: Variable description
        """
        self.variables[name] = {
            "value": value,
            "description": description,
            "type": type(value).__name__,
            "timestamp": datetime.now().isoformat()
        }
        print(f"Set template variable: {name} = {value}")
    
    def get_variable(self, name: str) -> Any:
        """Get template variable.
        
        Args:
            name: Variable name
            
        Returns:
            Variable value
        """
        if name not in self.variables:
            raise ConfigError(f"Variable not found: {name}", "", "get_variable")
        
        return self.variables[name]["value"]
    
    def add_parameter(self, name: str, default_value: Any, description: str = ""):
        """Add template parameter.
        
        Args:
            name: Parameter name
            default_value: Default value
            description: Parameter description
        """
        self.parameters[name] = {
            "default_value": default_value,
            "description": description,
            "type": type(default_value).__name__,
            "timestamp": datetime.now().isoformat()
        }
        print(f"Added template parameter: {name} = {default_value}")
    
    def get_parameter(self, name: str) -> Any:
        """Get template parameter.
        
        Args:
            name: Parameter name
            
        Returns:
            Parameter value
        """
        if name not in self.parameters:
            raise ConfigError(f"Parameter not found: {name}", "", "get_parameter")
        
        return self.parameters[name]["default_value"]
    
    def add_inheritance(self, parent_template: Dict[str, Any]):
        """Add template inheritance.
        
        Args:
            parent_template: Parent template configuration
        """
        self.inheritance_chain.append({
            "template": deepcopy(parent_template),
            "timestamp": datetime.now().isoformat()
        })
        print(f"Added inheritance from template")
    
    def add_conditional_setting(self, condition: str, setting: Dict[str, Any]):
        """Add conditional setting.
        
        Args:
            condition: Condition expression
            setting: Setting configuration
        """
        self.conditional_settings[condition] = {
            "setting": deepcopy(setting),
            "timestamp": datetime.now().isoformat()
        }
        print(f"Added conditional setting: {condition}")
    
    def render(self) -> Dict[str, Any]:
        """Render template to final configuration.
        
        Returns:
            Rendered configuration
        """
        print("Rendering template to final configuration")
        
        # Start with base configuration
        config = {}
        
        # Apply inheritance chain
        for inheritance in self.inheritance_chain:
            parent_config = inheritance["template"]
            config = self._merge_configs(config, parent_config)
        
        # Apply variables
        for var_name, var_info in self.variables.items():
            config[var_name] = var_info["value"]
        
        # Apply conditional settings
        for condition, setting_info in self.conditional_settings.items():
            if self._evaluate_condition(condition):
                config = self._merge_configs(config, setting_info["setting"])
        
        # Apply parameters
        for param_name, param_info in self.parameters.items():
            config[param_name] = param_info["default_value"]
        
        # Validate template
        self.validate_template()
        
        return config
    
    def validate_template(self):
        """Validate template consistency."""
        print("Validating template consistency")
        
        errors = []
        
        # Check for required parameters
        required_params = ["neuron_count", "connection_probability"]
        for param in required_params:
            if param not in self.variables:
                errors.append(f"Required parameter '{param}' is missing")
        
        # Check for variable type consistency
        for var_name, var_info in self.variables.items():
            if var_info["type"] not in ["int", "float", "str", "bool", "list", "dict"]:
                errors.append(f"Variable '{var_name}' has unsupported type: {var_info['type']}")
        
        # Check for inheritance cycles
        if len(self.inheritance_chain) > 10:
            errors.append("Inheritance chain too long (possible cycle)")
        
        if errors:
            self.is_valid = False
            for error in errors:
                print(f"Validation error: {error}")
            raise ConfigError(f"Template validation failed: {len(errors)} errors")
        else:
            self.is_valid = True
            print("Template validation passed")
    
    def get_template_variables(self) -> Dict[str, Any]:
        """Get all template variables."""
        return {name: info["value"] for name, info in self.variables.items()}
    
    def get_template_parameters(self) -> Dict[str, Any]:
        """Get all template parameters."""
        return {name: info["default_value"] for name, info in self.parameters.items()}
    
    def __repr__(self):
        return f"ConfigTemplate(variables={len(self.variables)}, parameters={len(self.parameters)}, valid={self.is_valid})"
    
    def _merge_configs(self, base: Dict[str, Any], override: Dict[str, Any]) -> Dict[str, Any]:
        """Merge two configurations."""
        result = deepcopy(base)
        
        for key, value in override.items():
            if key in result and isinstance(result[key], dict) and isinstance(value, dict):
                result[key] = self._merge_configs(result[key], value)
            else:
                result[key] = value
        
        return result
    
    def _evaluate_condition(self, condition: str) -> bool:
        """Evaluate condition expression."""
        # Simplified condition evaluation
        # In a real implementation, this would parse and evaluate the condition
        try:
            # Handle simple conditions
            if "==" in condition:
                left, right = condition.split("==", 1)
                return left.strip() == right.strip()
            elif ">" in condition:
                left, right = condition.split(">", 1)
                try:
                    return float(left.strip()) > float(right.strip())
                except:
                    return False
            elif "<" in condition:
                left, right = condition.split("<", 1)
                try:
                    return float(left.strip()) < float(right.strip())
                except:
                    return False
            elif "and" in condition:
                parts = condition.split("and")
                return all(self._evaluate_condition(part.strip()) for part in parts)
            elif "or" in condition:
                parts = condition.split("or")
                return any(self._evaluate_condition(part.strip()) for part in parts)
        except:
            pass
        
        return True  # Default to true if condition cannot be evaluated

class ConfigManager:
    """Advanced configuration management for neural simulation.
    
    Provides sophisticated configuration management with template support,
    validation, comparison, history, inheritance, and profiling.
    
    Features:
    * Configuration templates
    * Configuration validation
    * Configuration comparison
    * Configuration history
    * Configuration inheritance
    * Configuration profiling
    """
    
    def __init__(self):
        self.configs = {}
        self.templates = {}
        self.config_history = []
        self.current_config_name = ""
        self.validation_rules = {}
        self.profiling_data = {}
    
    def load_template(self, name: str) -> ConfigTemplate:
        """Load configuration template.
        
        Args:
            name: Template name
            
        Returns:
            ConfigTemplate object
        """
        if name not in self.templates:
            raise ConfigError(f"Template not found: {name}", "", "load_template")
        
        template = self.templates[name]
        print(f"Loaded template: {name}")
        return template
    
    def save_template(self, name: str, config: Dict[str, Any]):
        """Save configuration as template.
        
        Args:
            name: Template name
            config: Configuration to save as template
        """
        print(f"Saving configuration as template: {name}")
        
        # Create template from configuration
        template = ConfigTemplate()
        
        # Extract variables from config
        for key, value in config.items():
            if isinstance(value, (int, float, str, bool)):
                template.set_variable(key, value, f"Configuration parameter {key}")
        
        # Add parameters for key configuration items
        key_params = ["neuron_count", "connection_probability", "v_thresh", "v_rest"]
        for param in key_params:
            if param in config:
                template.add_parameter(param, config[param], f"Parameter {param}")
        
        self.templates[name] = template
        
        # Track in history
        self._track_config_action("save_template", name, config)
        
        print(f"Template saved: {name}")
    
    def get_template(self, name: str) -> Dict[str, Any]:
        """Get template by name.
        
        Args:
            name: Template name
            
        Returns:
            Template configuration
        """
        if name not in self.templates:
            raise ConfigError(f"Template not found: {name}", "", "get_template")
        
        template = self.templates[name]
        return template.render()
    
    def list_templates(self) -> List[str]:
        """List all available templates."""
        return sorted(self.templates.keys())
    
    def delete_template(self, name: str):
        """Delete template.
        
        Args:
            name: Template name to delete
        """
        if name not in self.templates:
            raise ConfigError(f"Template not found: {name}", "", "delete_template")
        
        del self.templates[name]
        
        # Track in history
        self._track_config_action("delete_template", name, {})
        
        print(f"Template deleted: {name}")
    
    def create_from_template(self, template_name: str, customizations: Dict[str, Any] = None):
        """Create configuration from template with customizations.
        
        Args:
            template_name: Template name to use
            customizations: Customizations to apply
            
        Returns:
            Created configuration
        """
        print(f"Creating configuration from template: {template_name}")
        
        if template_name not in self.templates:
            raise ConfigError(f"Template not found: {template_name}", "", "create_from_template")
        
        # Get base configuration from template
        base_config = self.get_template(template_name)
        
        # Apply customizations
        if customizations:
            config = self._merge_configs(base_config, customizations)
        else:
            config = base_config
        
        # Save configuration
        config_name = f"{template_name}_custom_{int(time.time())}"
        self.save_config(config_name, config)
        
        print(f"Configuration created: {config_name} from template {template_name}")
        return config
    
    def validate_config(self, config: Dict[str, Any]) -> bool:
        """Validate configuration against schema.
        
        Args:
            config: Configuration to validate
            
        Returns:
            True if valid, False otherwise
        """
        print(f"Validating configuration")
        
        errors = []
        
        # Validate required fields
        required_fields = ["neuron_count", "connection_probability"]
        for field in required_fields:
            if field not in config:
                errors.append(f"Missing required field: {field}")
        
        # Validate field types
        type_checks = [
            ("neuron_count", (int,)),
            ("connection_probability", (int, float)),
            ("v_thresh", (int, float)),
            ("v_rest", (int, float)),
            ("tau_mem", (int, float)),
            ("dt", (int, float))
        ]
        
        for field, types in type_checks:
            if field in config and not isinstance(config[field], types):
                errors.append(f"Field '{field}' has invalid type: {type(config[field])}")
        
        # Validate field ranges
        if "neuron_count" in config and config["neuron_count"] <= 0:
            errors.append("neuron_count must be positive")
        
        if "connection_probability" in config and (config["connection_probability"] < 0 or config["connection_probability"] > 1):
            errors.append("connection_probability must be between 0 and 1")
        
        # Validate consistency
        if "neuron_count" in config and "connection_probability" in config:
            expected_synapses = config["neuron_count"] * config["neuron_count"] * config["connection_probability"] / 2
            if "synapse_count" in config and abs(config["synapse_count"] - expected_synapses) > expected_synapses * 0.1:
                errors.append("Synapse count inconsistent with neuron count and connection probability")
        
        if errors:
            print(f"Configuration validation failed: {len(errors)} errors")
            for error in errors:
                print(f"  - {error}")
            return False
        
        print("Configuration validation passed")
        return True
    
    def compare_configs(self, config1: Dict[str, Any], config2: Dict[str, Any]) -> Dict[str, Any]:
        """Compare two configurations.
        
        Args:
            config1: First configuration
            config2: Second configuration
            
        Returns:
            Comparison results
        """
        print("Comparing configurations")
        
        # Find common keys
        keys1 = set(config1.keys())
        keys2 = set(config2.keys())
        
        common_keys = keys1 & keys2
        only_in_1 = keys1 - keys2
        only_in_2 = keys2 - keys1
        
        # Compare values for common keys
        differences = {}
        similarities = {}
        
        for key in common_keys:
            val1 = config1[key]
            val2 = config2[key]
            
            if val1 == val2:
                similarities[key] = val1
            else:
                differences[key] = {
                    "config1": val1,
                    "config2": val2,
                    "type1": type(val1).__name__,
                    "type2": type(val2).__name__,
                    "equal": False
                }
        
        comparison = {
            "config1_keys": len(keys1),
            "config2_keys": len(keys2),
            "common_keys": len(common_keys),
            "only_in_config1": list(only_in_1),
            "only_in_config2": list(only_in_2),
            "similarities": similarities,
            "differences": differences,
            "similarity_percentage": len(similarities) / max(len(keys1), len(keys2)) * 100 if (len(keys1) + len(keys2)) > 0 else 100
        }
        
        return comparison
    
    def get_config_history(self, config_name: str) -> List[Dict[str, Any]]:
        """Get configuration change history.
        
        Args:
            config_name: Configuration name
            
        Returns:
            Configuration history entries
        """
        return [entry for entry in self.config_history if entry.get("config_name") == config_name]
    
    def get_config_diff(self, config1_name: str, config2_name: str) -> Dict[str, Any]:
        """Get differences between configurations.
        
        Args:
            config1_name: First configuration name
            config2_name: Second configuration name
            
        Returns:
            Differences between configurations
        """
        # Get configurations
        config1 = self.configs.get(config1_name, {})
        config2 = self.configs.get(config2_name, {})
        
        return self.compare_configs(config1, config2)
    
    def export_config(self, config: Dict[str, Any], format: str) -> str:
        """Export configuration to different format.
        
        Args:
            config: Configuration to export
            format: Export format (json, yaml, xml, ini)
            
        Returns:
            Exported configuration string
        """
        print(f"Exporting configuration in {format} format")
        
        if format == "json":
            return json.dumps(config, indent=2)
        elif format == "yaml":
            try:
                import yaml
                return yaml.dump(config, default_flow_style=False)
            except ImportError:
                raise ConfigError("YAML export requires pyyaml library", "", "export_config")
        elif format == "xml":
            # Simple XML generation
            xml_lines = ["<config>"]
            for key, value in config.items():
                xml_lines.append(f"  <{key}>{value}</{key}>")
            xml_lines.append("</config>")
            return "\n".join(xml_lines)
        elif format == "ini":
            # Simple INI generation
            lines = ["[config]"]
            for key, value in config.items():
                if isinstance(value, (int, float)):
                    lines.append(f"{key}={value}")
                else:
                    lines.append(f"{key}=\"{value}\"")
            return "\n".join(lines)
        else:
            raise ConfigError(f"Unsupported export format: {format}", "", "export_config")
    
    def import_config(self, data: str, format: str) -> Dict[str, Any]:
        """Import configuration from different format.
        
        Args:
            data: Configuration data
            format: Import format (json, yaml, xml, ini)
            
        Returns:
            Imported configuration
        """
        print(f"Importing configuration from {format} format")
        
        if format == "json":
            return json.loads(data)
        elif format == "yaml":
            try:
                import yaml
                return yaml.safe_load(data)
            except ImportError:
                raise ConfigError("YAML import requires pyyaml library", "", "import_config")
        elif format == "xml":
            # Simple XML parsing (limited)
            import re
            config = {}
            for match in re.finditer(r'<([^>]+)>([^<]+)</\1>', data):
                key = match.group(1)
                value = match.group(2)
                config[key] = self._parse_xml_value(value)
            return config
        elif format == "ini":
            # Simple INI parsing
            config = {}
            for line in data.split('\n'):
                line = line.strip()
                if line and not line.startswith('[') and '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip().strip('"')
                    config[key] = self._parse_ini_value(value)
            return config
        else:
            raise ConfigError(f"Unsupported import format: {format}", "", "import_config")
    
    def save_config(self, name: str, config: Dict[str, Any]):
        """Save configuration.
        
        Args:
            name: Configuration name
            config: Configuration to save
        """
        print(f"Saving configuration: {name}")
        
        # Validate configuration
        if not self.validate_config(config):
            raise ConfigError(f"Configuration validation failed for '{name}'", "", "save_config")
        
        # Save configuration
        self.configs[name] = config
        self.current_config_name = name
        
        # Track in history
        self._track_config_action("save_config", name, config)
        
        print(f"Configuration saved: {name}")
    
    def get_config(self, name: str) -> Dict[str, Any]:
        """Get configuration by name.
        
        Args:
            name: Configuration name
            
        Returns:
            Configuration
        """
        if name not in self.configs:
            raise ConfigError(f"Configuration not found: {name}", "", "get_config")
        
        print(f"Retrieved configuration: {name}")
        return self.configs[name].copy()
    
    def delete_config(self, name: str):
        """Delete configuration.
        
        Args:
            name: Configuration name to delete
        """
        if name not in self.configs:
            raise ConfigError(f"Configuration not found: {name}", "", "delete_config")
        
        del self.configs[name]
        
        # Track in history
        self._track_config_action("delete_config", name, {})
        
        print(f"Configuration deleted: {name}")
    
    def list_configs(self) -> List[str]:
        """List all configurations."""
        return sorted(self.configs.keys())
    
    def get_default_config(self) -> Dict[str, Any]:
        """Get default configuration."""
        return {
            "neuron_count": 1000,
            "connection_probability": 0.1,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "tau_mem": 20.0,
            "dt": 0.1,
            "simulation_steps": 1000,
            "random_seed": 42
        }
    
    def create_brain_from_config(self, config_name: str):
        """Create brain from configuration.
        
        Args:
            config_name: Configuration name
            
        Returns:
            Configuration dictionary
        """
        print(f"Creating brain from configuration: {config_name}")
        
        config = self.get_config(config_name)
        return config
    
    def __repr__(self):
        return f"ConfigManager(configs={len(self.configs)}, templates={len(self.templates)}, current='{self.current_config_name}')"
    
    def _merge_configs(self, base: Dict[str, Any], override: Dict[str, Any]) -> Dict[str, Any]:
        """Merge two configurations."""
        result = deepcopy(base)
        
        for key, value in override.items():
            if key in result and isinstance(result[key], dict) and isinstance(value, dict):
                result[key] = self._merge_configs(result[key], value)
            else:
                result[key] = value
        
        return result
    
    def _track_config_action(self, action: str, config_name: str, config: Dict[str, Any]):
        """Track configuration action in history."""
        entry = {
            "action": action,
            "config_name": config_name,
            "timestamp": datetime.now().isoformat(),
            "config": deepcopy(config),
            "user": "system"  # In a real implementation, this would be the actual user
        }
        
        self.config_history.append(entry)
        
        # Keep only recent history (last 1000 entries)
        if len(self.config_history) > 1000:
            self.config_history = self.config_history[-1000:]
    
    def _parse_xml_value(self, value: str) -> Any:
        """Parse XML value to appropriate type."""
        # Remove quotes if present
        if value.startswith('"') and value.endswith('"'):
            value = value[1:-1]
        
        # Try to parse as number
        try:
            if '.' in value:
                return float(value)
            else:
                return int(value)
        except ValueError:
            pass
        
        return value
    
    def _parse_ini_value(self, value: str) -> Any:
        """Parse INI value to appropriate type."""
        # Remove quotes if present
        if value.startswith('"') and value.endswith('"'):
            value = value[1:-1]
        
        # Try to parse as number
        try:
            if '.' in value:
                return float(value)
            else:
                return int(value)
        except ValueError:
            pass
        
        return value

if __name__ == "__main__":
    # Example usage
    print("Running Config Manager Example")
    
    # Create config manager
    config_manager = ConfigManager()
    
    # Create a simple configuration
    simple_config = {
        "neuron_count": 1000,
        "connection_probability": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0
    }
    
    # Save configuration
    config_manager.save_config("simple_config", simple_config)
    
    # Create and validate a new configuration
    new_config = {
        "neuron_count": 2000,
        "connection_probability": 0.05,
        "v_thresh": -45.0,
        "v_rest": -75.0,
        "tau_mem": 25.0
    }
    
    # Validate configuration
    is_valid = config_manager.validate_config(new_config)
    print(f"New configuration valid: {is_valid}")
    
    if is_valid:
        # Save the new configuration
        config_manager.save_config("new_config", new_config)
        
        # Get the configuration
        retrieved_config = config_manager.get_config("new_config")
        print(f"Retrieved config: {retrieved_config}")
    
    # Create templates
    template_config = {
        "neuron_count": 1500,
        "connection_probability": 0.08,
        "v_thresh": -55.0,
        "v_rest": -70.0
    }
    
    config_manager.save_template("medium_neural_net", template_config)
    
    # List templates
    print("Available templates:", config_manager.list_templates())
    
    # Load template
    loaded_template = config_manager.load_template("medium_neural_net")
    print(f"Loaded template: {loaded_template}")
    
    # Create configuration from template
    created_config = config_manager.create_from_template("medium_neural_net", 
                                                         {"neuron_count": 2000})
    print(f"Created config from template: {created_config}")
    
    # Compare configurations
    comparison = config_manager.compare_configs(simple_config, new_config)
    print(f"Comparison similarity: {comparison['similarity_percentage']:.1f}%")
    print(f"Differences: {len(comparison['differences'])}")
    
    # List all configurations
    print("All configurations:", config_manager.list_configs())
    
    print("Config manager example completed!")
