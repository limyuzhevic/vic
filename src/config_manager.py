#!/usr/bin/env python3
"""
NLM Configuration Manager

Advanced configuration management system for NLM with support for:
- Configuration inheritance and merging
- Validation and schema checking
- Configuration templates
- Preset management
- Configuration comparison and diffing
- Import/export utilities
"""

import json
import yaml
import copy
from pathlib import Path
from typing import Dict, Any, List, Optional, Union, Set
from enum import Enum
import argparse
import sys

class ConfigError(Exception):
    """Base exception for configuration errors."""
    pass

class ValidationError(ConfigError):
    """Configuration validation error."""
    pass

class ConfigSource(Enum):
    """Configuration source enumeration."""
    DEFAULT = "default"
    FILE = "file"
    COMMAND_LINE = "command_line"
    RUNTIME = "runtime"
    TEMPLATE = "template"
    PRESET = "preset"

class ConfigEntry:
    """Configuration entry with metadata."""
    
    def __init__(self, key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME, 
                 description: str = "", validation_rules: Optional[Dict] = None):
        self.key = key
        self.value = value
        self.source = source
        self.description = description
        self.validation_rules = validation_rules or {}
        self.timestamp = None  # Could be added for tracking changes
    
    def to_dict(self) -> Dict:
        """Convert to dictionary representation."""
        return {
            'key': self.key,
            'value': self.value,
            'source': self.source.value,
            'description': self.description,
            'validation_rules': self.validation_rules
        }
    
    @classmethod
    def from_dict(cls, data: Dict) -> 'ConfigEntry':
        """Create from dictionary representation."""
        return cls(
            key=data['key'],
            value=data['value'],
            source=ConfigSource(data['source']),
            description=data.get('description', ''),
            validation_rules=data.get('validation_rules', {})
        )
class ConfigSchema:
    """Configuration schema for validation."""
    
    def __init__(self, name: str = "default"):
        self.name = name
        self.fields: Dict[str, Dict] = {}
        self.dependencies: Dict[str, List[str]] = {}
        self.groups: Dict[str, List[str]] = {}
    
    def add_field(self, key: str, field_type: str, required: bool = False, 
                  default: Any = None, description: str = "", 
                  validation_rules: Optional[Dict] = None, group: str = "default"):
        """Add a field to the schema."""
        self.fields[key] = {
            'type': field_type,
            'required': required,
            'default': default,
            'description': description,
            'validation_rules': validation_rules or {},
            'group': group
        }
        
        if group not in self.groups:
            self.groups[group] = []
        self.groups[group].append(key)
    
    def add_dependency(self, key: str, dependent_keys: List[str]):
        """Add dependency relationships between fields."""
        self.dependencies[key] = dependent_keys
    
    def validate(self, config: Dict) -> List[str]:
        """Validate configuration against schema. Returns list of errors."""
        errors = []
        
        # Check required fields
        for key, field_info in self.fields.items():
            if field_info['required'] and key not in config:
                errors.append(f"Required field '{key}' is missing")
        
        # Validate field types and values
        for key, value in config.items():
            if key in self.fields:
                field_info = self.fields[key]
                error = self._validate_field(key, value, field_info)
                if error:
                    errors.append(error)
        
        # Check dependencies
        for key, dependents in self.dependencies.items():
            if key in config:
                for dependent in dependents:
                    if dependent in config:
                        error = self._validate_dependency(key, dependent, config, field_info)
                        if error:
                            errors.append(error)
        
        return errors
    
    def _validate_field(self, key: str, value: Any, field_info: Dict) -> Optional[str]:
        """Validate a single field."""
        expected_type = field_info['type']
        
        # Type checking
        if expected_type == 'int' and not isinstance(value, int):
            return f"Field '{key}' must be an integer, got {type(value).__name__}"
        elif expected_type == 'float' and not isinstance(value, (int, float)):
            return f"Field '{key}' must be a number, got {type(value).__name__}"
        elif expected_type == 'bool' and not isinstance(value, bool):
            return f"Field '{key}' must be a boolean, got {type(value).__name__}"
        elif expected_type == 'string' and not isinstance(value, str):
            return f"Field '{key}' must be a string, got {type(value).__name__}"
        elif expected_type == 'list' and not isinstance(value, list):
            return f"Field '{key}' must be a list, got {type(value).__name__}"
        
        # Value validation based on validation rules
        validation_rules = field_info.get('validation_rules', {})
        
        if 'min' in validation_rules and value < validation_rules['min']:
            return f"Field '{key}' value {value} is below minimum {validation_rules['min']}"
        
        if 'max' in validation_rules and value > validation_rules['max']:
            return f"Field '{key}' value {value} exceeds maximum {validation_rules['max']}"
        
        if 'choices' in validation_rules and value not in validation_rules['choices']:
            return f"Field '{key}' value '{value}' is not one of {validation_rules['choices']}"
        
        return None
    
    def _validate_dependency(self, key: str, dependent: str, config: Dict, field_info: Dict) -> Optional[str]:npass: Optional[str]:
        """Validate dependency relationship."""
        # Implement dependency validation logic
        return None
class ConfigPreset:
    """Configuration preset for common setups."""
    
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
        self.config: Dict[str, Any] = {}
    
    def add_config(self, key: str, value: Any):
        """Add configuration value to preset."""
        self.config[key] = value
    
    def get_config(self) -> Dict[str, Any]:
        """Get the configuration as a dictionary."""
        return copy.deepcopy(self.config)
    
    def merge_with(self, other_config: Dict, override: bool = True) -> Dict[str, Any]:
        """Merge preset with another configuration."""
        result = copy.deepcopy(self.config)
        
        for key, value in other_config.items():
            if override or key not in result:
                result[key] = value
        
        return result
class ConfigTemplate:
    """Configuration template with variable substitution."""
    
    def __init__(self, name: str, template: Dict[str, Any]):
        self.name = name
        self.template = template
        self.variables: Dict[str, Any] = {}
    
    def set_variable(self, name: str, value: Any):
        """Set a template variable."""
        self.variables[name] = value
    
    def render(self) -> Dict[str, Any]:
        """Render template with current variables."""
        result = copy.deepcopy(self.template)
        
        for key, value in result.items():
            if isinstance(value, str) and value.startswith('${') and value.endswith('}'):
                var_name = value[2:-1]
                if var_name in self.variables:
                    result[key] = self.variables[var_name]
        
        return result
class ConfigManager:
    """Advanced configuration manager for NLM."""
    
    def __init__(self, config_dir: Optional[str] = None):
        self.config_dir = Path(config_dir) if config_dir else Path(".") / "configs"
        self.config_dir.mkdir(exist_ok=True)
        
        self.presets: Dict[str, ConfigPreset] = {}
        self.templates: Dict[str, ConfigTemplate] = {}
        self.schemas: Dict[str, ConfigSchema] = {}
        
        # Initialize default presets
        self._initialize_default_presets()
        self._initialize_default_templates()
        self._initialize_default_schemas()
    
    def _initialize_default_presets(self):
        """Initialize default configuration presets."""
        # Small brain configuration
        small_brain = ConfigPreset("small_brain", "Small brain configuration for quick testing")
        small_brain.add_config("brain.neuron_count", 500)
        small_brain.add_config("brain.synapse_density", 0.1)
        small_brain.add_config("plasticity.stdp.enable", True)
        small_brain.add_config("plasticity.hebbian.enable", True)
        self.presets["small_brain"] = small_brain
        
        # Medium brain configuration
        medium_brain = ConfigPreset("medium_brain", "Medium brain configuration for balanced performance")
        medium_brain.add_config("brain.neuron_count", 2000)
        medium_brain.add_config("brain.synapse_density", 0.15)
        medium_brain.add_config("plasticity.stdp.enable", True)
        medium_brain.add_config("plasticity.hebbian.enable", True)
        medium_brain.add_config("neuromod.curiosity.enable", True)
        medium_brain.add_config("development.enable", True)
        self.presets["medium_brain"] = medium_brain
        
        # Large brain configuration
        large_brain = ConfigPreset("large_brain", "Large brain configuration for detailed simulations")
        large_brain.add_config("brain.neuron_count", 5000)
        large_brain.add_config("brain.synapse_density", 0.2)
        large_brain.add_config("plasticity.stdp.enable", True)
        large_brain.add_config("plasticity.hebbian.enable", True)
        large_brain.add_config("plasticity.structural.enable", True)
        large_brain.add_config("neuromod.curiosity.enable", True)
        large_brain.add_config("neuromod.novelty.enable", True)
        large_brain.add_config("development.enable", True)
        self.presets["large_brain"] = large_brain
    
    def _initialize_default_templates(self):
        """Initialize default configuration templates."""
        # Simple template
        simple_template = ConfigTemplate("simple", {
            "brain.neuron_count": "${neuron_count}",
            "brain.synapse_density": "${synapse_density}",
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True
        })
        simple_template.set_variable("neuron_count", 1000)
        simple_template.set_variable("synapse_density", 0.1)
        self.templates["simple"] = simple_template
        
        # Advanced template
        advanced_template = ConfigTemplate("advanced", {
            "brain.neuron_count": "${neuron_count}",
            "brain.synapse_density": "${synapse_density}",
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": "${structural_plasticity}",
            "neuromod.curiosity.enable": "${curiosity}",
            "neuromod.novelty.enable": "${novelty}",
            "development.enable": "${development}",
            "neuromod.dopamine.scale": "${dopamine_scale}"
        })
        advanced_template.set_variable("neuron_count", 2000)
        advanced_template.set_variable("synapse_density", 0.15)
        advanced_template.set_variable("structural_plasticity", True)
        advanced_template.set_variable("curiosity", True)
        advanced_template.set_variable("novelty", True)
        advanced_template.set_variable("development", True)
        advanced_template.set_variable("dopamine_scale", 1.0)
        self.templates["advanced"] = advanced_template
    
    def _initialize_default_schemas(self):
        """Initialize default configuration schemas."""
        # Simple schema
        simple_schema = ConfigSchema("simple")
        simple_schema.add_field("brain.neuron_count", "int", True, 1000, "Number of neurons")
        simple_schema.add_field("brain.synapse_density", "float", True, 0.1, "Synapse density")
        simple_schema.add_field("plasticity.stdp.enable", "bool", False, True, "Enable STDP")
        simple_schema.add_field("plasticity.hebbian.enable", "bool", False, True, "Enable Hebbian learning")
        self.schemas["simple"] = simple_schema
        
        # Advanced schema
        advanced_schema = ConfigSchema("advanced")
        advanced_schema.add_field("brain.neuron_count", "int", True, 1000, "Number of neurons", {"min": 100, "max": 10000})
        advanced_schema.add_field("brain.synapse_density", "float", True, 0.1, "Synapse density", {"min": 0.01, "max": 1.0})
        advanced_schema.add_field("plasticity.stdp.enable", "bool", False, True, "Enable STDP")
        advanced_schema.add_field("plasticity.hebbian.enable", "bool", False, True, "Enable Hebbian learning")
        advanced_schema.add_field("plasticity.structural.enable", "bool", False, False, "Enable structural plasticity")
        advanced_schema.add_field("neuromod.curiosity.enable", "bool", False, True, "Enable curiosity")
        advanced_schema.add_field("neuromod.novelty.enable", "bool", False, True, "Enable novelty detection")
        advanced_schema.add_field("development.enable", "bool", False, True, "Enable development")
        advanced_schema.add_field("neuromod.dopamine.scale", "float", False, 1.0, "Dopamine scale", {"min": 0.0, "max": 10.0})
        self.schemas["advanced"] = advanced_schema
    
    def add_preset(self, preset: ConfigPreset):
        """Add a configuration preset."""
        self.presets[preset.name] = preset
    
    def add_template(self, template: ConfigTemplate):
        """Add a configuration template."""
        self.templates[template.name] = template
    
    def add_schema(self, schema: ConfigSchema):
        """Add a configuration schema."""
        self.schemas[schema.name] = schema
    
    def get_preset(self, name: str) -> Optional[ConfigPreset]:
        """Get a preset by name."""
        return self.presets.get(name)
    
    def get_template(self, name: str) -> Optional[ConfigTemplate]:
        """Get a template by name."""
        return self.templates.get(name)
    
    def get_schema(self, name: str) -> Optional[ConfigSchema]:
        """Get a schema by name."""
        return self.schemas.get(name)
    
    def load_preset(self, name: str, **overrides) -> Dict[str, Any]:
        """Load a preset with optional overrides."""
        preset = self.get_preset(name)
        if not preset:
            raise ConfigError(f"Preset not found: {name}")
        
        config = preset.get_config()
        
        # Apply overrides
        for key, value in overrides.items():
            config[key] = value
        
        return config
    
    def load_template(self, name: str, **variables) -> Dict[str, Any]:
        """Load a template with variable substitution."""
        template = self.get_template(name)
        if not template:
            raise ConfigError(f"Template not found: {name}")
        
        # Set variables
        for key, value in variables.items():
            template.set_variable(key, value)
        
        return template.render()
    
    def validate_config(self, config: Dict, schema_name: str = "default") -> List[str]:
        """Validate configuration against a schema."""
        schema = self.get_schema(schema_name)
        if not schema:
            schema = self.schemas["simple"]  # Default schema
        
        return schema.validate(config)
    
    def merge_configs(self, *configs: Dict[str, Any]) -> Dict[str, Any]:
        """Merge multiple configurations, later ones take precedence."""
        result = {}
        
        for config in reversed(configs):
            for key, value in config.items():
                result[key] = value
        
        return result
    
    def export_config(self, config: Dict, filepath: str, format: str = "json"):
        """Export configuration to file."""
        path = Path(filepath)
        
        if format == "json":
            with open(path, 'w') as f:
                json.dump(config, f, indent=2)
        elif format == "yaml":
            try:
                import yaml
                with open(path, 'w') as f:
                    yaml.dump(config, f, default_flow_style=False)
            except ImportError:
                raise ConfigError("PyYAML is required for YAML export")
        else:
            raise ConfigError(f"Unsupported format: {format}")
    
    def import_config(self, filepath: str, format: str = "json") -> Dict[str, Any]:
        """Import configuration from file."""
        path = Path(filepath)
        
        if format == "json":
            with open(path, 'r') as f:
                return json.load(f)
        elif format == "yaml":
            try:
                import yaml
                with open(path, 'r') as f:
                    return yaml.safe_load(f)
            except ImportError:
                raise ConfigError("PyYAML is required for YAML import")
        else:
            raise ConfigError(f"Unsupported format: {format}")
    
    def diff_configs(self, config1: Dict, config2: Dict) -> Dict[str, Any]:
        """Compare two configurations and return differences."""
        diff = {
            "added": {},
            "removed": {},
            "modified": {}
        }
        
        keys1 = set(config1.keys())
        keys2 = set(config2.keys())
        
        # Find added keys
        diff["added"] = {k: config2[k] for k in keys2 - keys1}
        
        # Find removed keys
        diff["removed"] = {k: config1[k] for k in keys1 - keys2}
        
        # Find modified keys
        for k in keys1 & keys2:
            if config1[k] != config2[k]:
                diff["modified"][k] = {
                    "from": config1[k],
                    "to": config2[k]
                }
        
        return diff
    
    def get_config_summary(self, config: Dict) -> str:
        """Generate a human-readable summary of configuration."""
        lines = []
        lines.append("Configuration Summary:")
        lines.append("=" * 50)
        
        for key, value in sorted(config.items()):
            if isinstance(value, dict):
                lines.append(f"{key}:")
                for subkey, subvalue in value.items():
                    lines.append(f"  {subkey}: {subvalue}")
            else:
                lines.append(f"{key}: {value}")
        
        return "\n".join(lines)
class ConfigCLI:
    """Command-line interface for configuration management."""
    
    def __init__(self, config_manager: Optional[ConfigManager] = None):
        self.config_manager = config_manager or ConfigManager()
    
    def create_parser(self) -> argparse.ArgumentParser:
        """Create the argument parser."""
        parser = argparse.ArgumentParser(
            prog="nlm-config",
            description="NLM Configuration Manager",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm-config create --name small --output small_config.json
  nlm-config preset list
  nlm-config template show simple
  nlm-config validate --config my_config.json --schema advanced
  nlm-config merge preset1 preset2 --output merged.json
  nlm-config diff config1.json config2.json
  nlm-config export --config config.json --format yaml
  nlm-config import config.yaml --format yaml

For more information, visit: https://github.com/nlm-project/nlm
            """
        )
        
        parser.add_argument(
            "--verbose", 
            "-v", 
            action="store_true",
            help="Enable verbose output"
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Available commands")
        
        # Create command
        create_parser = subparsers.add_parser("create", help="Create a new configuration")
        create_parser.add_argument("name", help="Configuration name")
        create_parser.add_argument("output", help="Output file path")
        create_parser.add_argument("--template", choices=["simple", "advanced"], 
                                 default="simple", help="Template to use (default: simple)")
        create_parser.add_argument("--from-preset", help="Create from preset")
        create_parser.add_argument("--set", action="append", help="Set configuration values (key=value)")
        
        # Preset commands
        preset_parser = subparsers.add_parser("preset", help="Preset management")
        preset_subparsers = preset_parser.add_subparsers(dest="preset_command")
        
        preset_list_parser = preset_subparsers.add_parser("list", help="List all presets")
        preset_load_parser = preset_subparsers.add_parser("load", help="Load a preset")
        preset_load_parser.add_argument("name", help="Preset name")
        preset_load_parser.add_argument("output", help="Output file path")
        preset_load_parser.add_argument("--set", action="append", help="Set configuration values (key=value)")
        
        # Template commands
        template_parser = subparsers.add_parser("template", help="Template management")
        template_subparsers = template_parser.add_subparsers(dest="template_command")
        
        template_list_parser = template_subparsers.add_parser("list", help="List all templates")
        template_show_parser = template_subparsers.add_parser("show", help="Show template details")
        template_show_parser.add_argument("name", help="Template name")
        template_render_parser = template_subparsers.add_parser("render", help="Render template")
        template_render_parser.add_argument("name", help="Template name")
        template_render_parser.add_argument("--set", action="parse_args", help="Template variables (var=value)")
        
        # Schema commands
        schema_parser = subparsers.add_parser("schema", help="Schema management")
        schema_subparsers = schema_parser.add_subparsers(dest="schema_command")
        
        schema_list_parser = schema_subparsers.add_parser("list", help="List all schemas")
        schema_validate_parser = schema_subparsers.add_parser("validate", help="Validate configuration")
        schema_validate_parser.add_argument("config", help="Configuration file")
        schema_validate_parser.add_argument("--schema", default="default", help="Schema name (default: default)")
        
        # Configuration commands
        config_parser = subparsers.add_parser("config", help="Configuration management")
        config_subparsers = config_parser.add_subparsers(dest="config_command")
        
        config_merge_parser = config_subparsers.add_parser("merge", help="Merge configurations")
        config_merge_parser.add_argument("inputs", nargs="+", help="Input configuration files")
        config_merge_parser.add_argument("output", help="Output file path")
        
        config_diff_parser = config_subparsers.add_parser("diff", help="Compare configurations")
        config_diff_parser.add_argument("config1", help="First configuration file")
        config_diff_parser.add_argument("config2", help="Second configuration file")
        
        config_import_parser = config_subparsers.add_parser("import", help="Import configuration")
        config_import_parser.add_argument("input", help="Input file path")
        config_import_parser.add_argument("output", help="Output file path")
        config_import_parser.add_argument("--format", choices=["json", "yaml"], default="json", help="Input format (default: json)")
        
        config_export_parser = config_subparsers.add_parser("export", help="Export configuration")
        config_export_parser.add_argument("input", help="Input file path")
        config_export_parser.add_argument("output", help="Output file path")
        config_export_parser.add_argument("--format", choices=["json", "yaml"], default="json", help="Output format (default: json)")
        
        # Analysis commands
        analysis_parser = subparsers.add_parser("analyze", help="Configuration analysis")
        analysis_subparsers = analysis_parser.add_subparsers(dest="analysis_command")
        
        analysis_diff_parser = analysis_subparsers.add_parser("diff", help="Analyze differences")
        analysis_diff_parser.add_argument("config1", help="First configuration file")
        analysis_diff_parser.add_argument("config2", help="Second configuration file")
        analysis_diff_parser.add_argument("--format", choices=["text", "json"], default="text", help="Output format (default: text)")
        
        return parser
    
    def parse_set_args(self, set_args: Optional[List[str]]) -> Dict[str, Any]:
        """Parse --set arguments."""
        if not set_args:
            return {}
        
        config = {}
        for arg in set_args:
            if '=' in arg:
                key, value = arg.split('=', 1)
                
                # Try to infer type
                if value.lower() in ('true', 'false'):
                    config[key] = value.lower() == 'true'
                elif value.isdigit():
                    config[key] = int(value)
                elif value.replace('.', '', 1).isdigit():
                    config[key] = float(value)
                else:
                    config[key] = value
        
        return config
    
    def run_command(self, args):
        """Execute command."""
        if args.command == "create":
            self._handle_create_command(args)
        elif args.command == "preset":
            self._handle_preset_command(args)
        elif args.command == "template":
            self._handle_template_command(args)
        elif args.command == "schema":
            self._handle_schema_command(args)
        elif args.command == "config":
            self._handle_config_command(args)
        elif args.command == "analyze":
            self._handle_analyze_command(args)
        else:
            print("Unknown command. Use --help for available commands.")
    
    def _handle_create_command(self, args):
        """Handle create command."""
        config = {}
        
        # Load from preset if specified
        if args.from_preset:
            config = self.config_manager.load_preset(args.from_preset)
        else:
            # Load from template
            template = self.config_manager.get_template(args.template)
            if template:
                config = template.render()
        
        # Apply command line overrides
        if args.set:
            overrides = self.parse_set_args(args.set)
            config.update(overrides)
        
        # Save configuration
        self.config_manager.export_config(config, args.output, "json")
        
        print(f"Configuration '{args.name}' saved to {args.output}")
        
        # Show summary if verbose
        if args.verbose:
            print(self.config_manager.get_config_summary(config))
    
    def _handle_preset_command(self, args):
        """Handle preset command."""
        if args.preset_command == "list":
            print("Available presets:")
            for name, preset in self.config_manager.presets.items():
                print(f"  {name}: {preset.description}")
        
        elif args.preset_command == "load":
            config = self.config_manager.load_preset(args.name)
            
            # Apply command line overrides
            if args.set:
                overrides = self.parse_set_args(args.set)
                config.update(overrides)
            
            # Save to file
            output = args.output if args.output else f"{args.name}_config.json"
            self.config_manager.export_config(config, output, "json")
            
            print(f"Preset '{args.name}' loaded to {output}")
    
    def _handle_template_command(self, args):
        """Handle template command."""
        if args.template_command == "list":
            print("Available templates:")
            for name, template in self.config_manager.templates.items():
                print(f"  {name}: {template.template}")
        
        elif args.template_command == "show":
            template = self.config_manager.get_template(args.name)
            if template:
                print(f"Template '{args.name}':")
                for key, value in template.template.items():
                    print(f"  {key}: {value}")
            else:
                print(f"Template not found: {args.name}")
        
        elif args.template_command == "render":
            # Parse template variables (this would need better argument parsing)
            config = self.config_manager.load_template(args.name)
            self.config_manager.export_config(config, "rendered_config.json", "json")
            print(f"Template '{args.name}' rendered to rendered_config.json")
    
    def _handle_schema_command(self, args):
        """Handle schema command."""
        if args.schema_command == "list":
            print("Available schemas:")
            for name, schema in self.config_manager.schemas.items():
                print(f"  {name}: {len(schema.fields)} fields")
        
        elif args.schema_command == "validate":
            try:
                config = self.config_manager.import_config(args.config, "json")
                errors = self.config_manager.validate_config(config, args.schema)
                
                if errors:
                    print("Validation errors:")
                    for error in errors:
                        print(f"  - {error}")
                else:
                    print("Configuration is valid!")
            except Exception as e:
                print(f"Error: {e}")
    
    def _handle_config_command(self, args):
        """Handle config command."""
        if args.config_command == "merge":
            configs = []
            for input_file in args.inputs:
                config = self.config_manager.import_config(input_file, "json")
                configs.append(config)
            
            merged = self.config_manager.merge_configs(*configs)
            self.config_manager.export_config(merged, args.output, "json")
            print(f"Configurations merged to {args.output}")
        
        elif args.config_command == "diff":
            config1 = self.config_manager.import_config(args.config1, "json")
            config2 = self.config_manager.import_config(args.config2, "json")
            
            diff = self.config_manager.diff_configs(config1, config2)
            print("Configuration differences:")
            print(f"  Added: {list(diff['added'].keys())}")
            print(f"  Removed: {list(diff['removed'].keys())}")
            print(f"  Modified: {list(diff['modified'].keys())}")
            
            # Save diff to file if requested
            if hasattr(args, 'output') and args.output:
                self.config_manager.export_config(diff, args.output, "json")
    
    def _handle_analyze_command(self, args):
        """Handle analyze command."""
        if args.analysis_command == "diff":
            config1 = self.config_manager.import_config(args.config1, "json")
            config2 = self.config_manager.import_config(args.config2, "json")
            
            diff = self.config_manager.diff_configs(config1, config2)
            
            if args.format == "json":
                import json
                print(json.dumps(diff, indent=2))
            else:
                print("Difference Analysis:")
                print(f"  Added keys: {len(diff['added'])}")
                print(f"  Removed keys: {len(diff['removed'])}")
                print(f"  Modified keys: {len(diff['modified'])}")
                
                if diff['added']:
                    print("\nAdded keys:")
                    for key in diff['added'].keys():
                        print(f"  - {key}: {diff['added'][key]}")
                
                if diff['removed']:
                    print("\nRemoved keys:")
                    for key in diff['removed'].keys():
                        print(f"  - {key}: {diff['removed'][key]}")
                
                if diff['modified']:
                    print("\nModified keys:")
                    for key, change in diff['modified'].items():
                        print(f"  - {key}: {change['from']} -> {change['to']}")
def main():
    """Main entry point."""
    config_manager = ConfigManager()
    cli = ConfigCLI(config_manager)
    parser = cli.create_parser()
    args = parser.parse_args()
    
    try:
        cli.run_command(args)
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1
    
    return 0
if __name__ == "__main__":
    sys.exit(main())