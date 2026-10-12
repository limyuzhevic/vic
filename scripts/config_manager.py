#!/usr/bin/env python3
"""NLM Configuration Validator and Schema System"""

import json
import re
import sys
from pathlib import Path
from typing import Dict, Any, List, Optional, Union

class NLMConfigSchema:
    """Schema validation for NLM configuration"""
    
    DEFAULT_SCHEMA = {
        "type": "object",
        "properties": {
            # Core simulation parameters
            "random_seed": {"type": "integer", "minimum": 0},
            "simulation_timestep": {"type": "number", "minimum": 0.0001, "maximum": 1.0},
            "neuron_count": {"type": "integer", "minimum": 1, "maximum": 1000000},
            "region_count": {"type": "integer", "minimum": 1, "maximum": 100},
            
            # Connectivity and structure
            "connection_probability": {"type": "number", "minimum": 0.0, "maximum": 1.0},
            
            # Plasticity parameters
            "plasticity_learning_rate": {"type": "number", "minimum": 0.0, "maximum": 1.0},
            "stdp_ltp_weight": {"type": "number", "minimum": 0.0, "maximum": 1.0},
            "stdp_ltd_weight": {"type": "number", "minimum": 0.0, "maximum": 1.0},
            "stdp_time_constant": {"type": "number", "minimum": 1.0, "maximum": 100.0},
            
            # Development parameters
            "development_synaptogenesis_rate": {"type": "number", "minimum": 0.0, "maximum": 1.0},
            "development_pruning_rate": {"type": "number", "minimum": 0.0, "maximum": 1.0},
            
            # Neuromodulation
            "dopamine_baseline": {"type": "number", "minimum": 0.0, "maximum": 10.0},
            
            # Learning
            "reward_discount_factor": {"type": "number", "minimum": 0.0, "maximum": 1.0},
            
            # Environment
            "environment_name": {"type": "string"},
            "environment_width": {"type": "integer", "minimum": 1},
            "environment_height": {"type": "integer", "minimum": 1},
            
            # Logging
            "log_level": {"type": "string", "enum": ["DEBUG", "INFO", "WARNING", "ERROR"]},
            "log_to_file": {"type": "boolean"},
            "log_filename": {"type": "string"},
            
            # Simulation limits
            "max_simulation_steps": {"type": "integer", "minimum": 1},
            "simulation_time_limit": {"type": "number", "minimum": 0.0},
            
            # Visualization
            "visualization_enabled": {"type": "boolean"},
            "visualization_update_rate": {"type": "number", "minimum": 1.0, "maximum": 1000.0}
        },
        "required": [
            "random_seed",
            "simulation_timestep", 
            "neuron_count",
            "region_count",
            "connection_probability"
        ]
    }
    
    @staticmethod
    def validate_config(config_data: Dict[str, Any]) -> List[str]:
        """Validate configuration against schema"""
        errors = []
        
        # Check required fields
        for required_field in NLMConfigSchema.DEFAULT_SCHEMA["required"]:
            if required_field not in config_data:
                errors.append(f"Missing required field: {required_field}")
        
        # Check for unknown fields
        allowed_fields = set(NLMConfigSchema.DEFAULT_SCHEMA["properties"].keys())
        for field in config_data.keys():
            if field not in allowed_fields:
                errors.append(f"Unknown configuration field: {field}")
        
        # Type and range validation
        for field, schema in NLMConfigSchema.DEFAULT_SCHEMA["properties"].items():
            if field in config_data:
                value = config_data[field]
                
                # Type checking
                if schema["type"] == "integer" and not isinstance(value, int):
                    errors.append(f"Field '{field}' must be integer, got {type(value).__name__}")
                elif schema["type"] == "number" and not isinstance(value, (int, float)):
                    errors.append(f"Field '{field}' must be number, got {type(value).__name__}")
                elif schema["type"] == "string" and not isinstance(value, str):
                    errors.append(f"Field '{field}' must be string, got {type(value).__name__}")
                elif schema["type"] == "boolean" and not isinstance(value, bool):
                    errors.append(f"Field '{field}' must be boolean, got {type(value).__name__}")
                
                # Range checking
                if isinstance(value, (int, float)):
                    if "minimum" in schema and value < schema["minimum"]:
                        errors.append(f"Field '{field}' must be >= {schema['minimum']}, got {value}")
                    if "maximum" in schema and value > schema["maximum"]:
                        errors.append(f"Field '{field}' must be <= {schema['maximum']}, got {value}")
                
                # Enum checking
                if "enum" in schema and value not in schema["enum"]:
                    errors.append(f"Field '{field}' must be one of {schema['enum']}, got '{value}'")
        
        return errors
    
    @staticmethod
    def get_default_config() -> Dict[str, Any]:
        """Get default configuration values"""
        return {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_time_constant": 20.0,
            "development_synaptogenesis_rate": 0.001,
            "development_pruning_rate": 0.0001,
            "dopamine_baseline": 0.1,
            "reward_discount_factor": 0.99,
            "environment_name": "GridWorld",
            "environment_width": 10,
            "environment_height": 10,
            "log_level": "INFO",
            "log_to_file": False,
            "log_filename": "nlm.log",
            "max_simulation_steps": 10000,
            "simulation_time_limit": 0.0,
            "visualization_enabled": False,
            "visualization_update_rate": 30.0
        }
class NLMConfigConverter:
    """Convert between old and new configuration formats"""
    
    # Mapping from new field names to old field names
    FIELD_MAPPING = {
        "plasticity_learning_rate": "plasticity_learning_rate",
        "stdp_ltp_weight": "stdp_ltp_weight",
        "stdp_ltd_weight": "stdp_ltd_weight",
        "stdp_time_constant": "stdp_time_constant",
        "development_synaptogenesis_rate": "development_synaptogenesis_rate",
        "development_pruning_rate": "development_pruning_rate",
        "dopamine_baseline": "dopamine_baseline",
        "reward_discount_factor": "reward_discount_factor",
        "environment_name": "environment_name",
        "environment_width": "environment_width",
        "environment_height": "environment_height",
        "log_level": "log_level",
        "log_to_file": "log_to_file",
        "log_filename": "log_filename",
        "max_simulation_steps": "max_simulation_steps",
        "simulation_time_limit": "simulation_time_limit",
        "visualization_enabled": "visualization_enabled",
        "visualization_update_rate": "visualization_update_rate"
    }
    
    @staticmethod
    def convert_old_to_new(old_config: Dict[str, Any]) -> Dict[str, Any]:
        """Convert old configuration format to new format"""
        new_config = NLMConfigSchema.get_default_config()
        
        # Copy all valid fields
        for new_field, old_field in NLMConfigConverter.FIELD_MAPPING.items():
            if old_field in old_config:
                new_config[new_field] = old_config[old_field]
        
        return new_config
    
    @staticmethod
    def convert_new_to_old(new_config: Dict[str, Any]) -> Dict[str, Any]:
        """Convert new configuration format to old format (for backward compatibility)"""
        old_config = {}
        
        for old_field in NLMConfigConverter.FIELD_MAPPING.values():
            if old_field in new_config:
                old_config[old_field] = new_config[old_field]
        
        return old_config
class NLMConfigManager:
    """Manage NLM configuration with validation and conversion"""
    
    def __init__(self):
        self.schema = NLMConfigSchema()
        self.converter = NLMConfigConverter()
        self.config_file = "configs/nlm_config.json"
        self.backup_file = "configs/nlm_config_backup.json"
    
    def load_config(self, filepath: Optional[str] = None) -> Dict[str, Any]:
        """Load configuration from file with validation"""
        if filepath is None:
            filepath = self.config_file
        
        try:
            with open(filepath, 'r') as f:
                config_data = json.load(f)
            
            # Validate configuration
            errors = self.schema.validate_config(config_data)
            if errors:
                print("Configuration validation errors:")
                for error in errors:
                    print(f"  ERROR: {error}")
                return self.schema.get_default_config()
            
            print(f"Loaded configuration from: {filepath}")
            return config_data
            
        except FileNotFoundError:
            print(f"Configuration file not found: {filepath}")
            print("Using default configuration")
            return self.schema.get_default_config()
        except json.JSONDecodeError as e:
            print(f"Invalid JSON in configuration file: {e}")
            return self.schema.get_default_config()
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return self.schema.get_default_config()
    
    def save_config(self, config_data: Dict[str, Any], filepath: Optional[str] = None) -> bool:
        """Save configuration to file with backup"""
        if filepath is None:
            filepath = self.config_file
        
        try:
            # Validate configuration
            errors = self.schema.validate_config(config_data)
            if errors:
                print("Configuration validation errors - cannot save:")
                for error in errors:
                    print(f"  ERROR: {error}")
                return False
            
            # Create backup if file exists
            if Path(filepath).exists():
                Path(self.backup_file).write_text(Path(filepath).read_text())
                print(f"Created backup at: {self.backup_file}")
            
            # Save configuration
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
            
            print(f"Configuration saved to: {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def create_template_config(self, filepath: str = "configs/nlm_config_template.json"):
        """Create a template configuration file"""
        template = self.schema.get_default_config()
        self.save_config(template, filepath)
        print(f"Created configuration template at: {filepath}")
    
    def migrate_config(self, old_filepath: str, new_filepath: Optional[str] = None):
        """Migrate old configuration format to new format"""
        if new_filepath is None:
            new_filepath = "configs/nlm_config_migrated.json"
        
        try:
            # Load old config (simple key=value format)
            old_config = {}
            with open(old_filepath, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#') and '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Try to convert to appropriate type
                        if value.lower() in ('true', 'false'):
                            old_config[key] = value.lower() == 'true'
                        elif value.isdigit():
                            old_config[key] = int(value)
                        else:
                            try:
                                old_config[key] = float(value)
                            except ValueError:
                                old_config[key] = value
            
            # Convert to new format
            new_config = self.converter.convert_old_to_new(old_config)
            
            # Save new config
            self.save_config(new_config, new_filepath)
            print(f"Migrated configuration to new format at: {new_filepath}")
            
        except Exception as e:
            print(f"Error migrating configuration: {e}")
class NLMCliHelper:
    """CLI helper for NLM with advanced features"""
    
    @staticmethod
    def print_help():
        """Print help information"""
        help_text = """
NLM - Neural Learning Machine

USAGE:
  nlm [OPTIONS]
  nlm --config <file> [OPTIONS]

OPTIONS:
  --help                          Show this help message
  --version                       Show version information
  --list-configs                  List available configuration files
  --create-template <file>        Create a new configuration template file
  --migrate <old_file> [new_file] Migrate old configuration format to new format
  --validate <file>               Validate configuration file
  --set <key>=<value>             Set configuration value
  --get <key>                     Get configuration value
  --save-config <file>             Save current configuration to file
  --verbose                        Enable verbose output
  --quiet                          Disable non-essential output

CONFIGURATION:
  --config <file>                 Configuration file (default: configs/default.cfg)
  --config-format <format>         Configuration file format (json, cfg, auto)

EXAMPLES:
  # Run with default configuration
  nlm

  # Run with custom configuration
  nlm --config my_config.json

  # Create a new configuration template
  nlm --create-template my_template.json

  # Validate configuration file
  nlm --validate my_config.json

  # Set configuration value for this run
  nlm --set neuron_count=2000 --set random_seed=123

For more information, visit: https://github.com/nlm-project/nlm
"""
        print(help_text)
    
    @staticmethod
    def parse_cli_args(argv: List[str]) -> Dict[str, Any]:
        """Parse command line arguments"""
        args = {
            'config_file': 'configs/default.cfg',
            'config_format': 'auto',
            'verbose': False,
            'quiet': False,
            'set_values': {},
            'custom_options': {}
        }
        
        i = 1
        while i < len(argv):
            arg = argv[i]
            
            if arg == '--help':
                NLMCliHelper.print_help()
                sys.exit(0)
            elif arg == '--version':
                print("NLM Version 0.1.0 (Phase 6 Final Integration)")
                sys.exit(0)
            elif arg == '--list-configs':
                NLMCliHelper.list_config_files()
                sys.exit(0)
            elif arg == '--verbose':
                args['verbose'] = True
            elif arg == '--quiet':
                args['quiet'] = True
            elif arg.startswith('--config='):
                args['config_file'] = arg.split('=', 1)[1]
            elif arg == '--config':
                i += 1
                if i < len(argv):
                    args['config_file'] = argv[i]
            elif arg.startswith('--set='):
                key_value = arg.split('=', 1)[1]
                if '=' in key_value:
                    key, value = key_value.split('=', 1)
                    args['set_values'][key] = value
                else:
                    args['set_values'][key_value] = True
            elif arg == '--set':
                i += 1
                if i < len(argv):
                    if '=' in argv[i]:
                        key, value = argv[i].split('=', 1)
                        args['set_values'][key] = value
                    else:
                        args['set_values'][argv[i]] = True
            elif arg.startswith('--'):
                args['custom_options'][arg] = True
            
            i += 1
        
        return args
    
    @staticmethod
    def list_config_files():
        """List available configuration files"""
        config_dir = Path("configs")
        if not config_dir.exists():
            print("No configs directory found")
            return
        
        print("Available configuration files:")
        for config_file in config_dir.glob("*"):
            if config_file.is_file():
                size = config_file.stat().st_size
                print(f"  {config_file.name:30} ({size} bytes)")
    
    @staticmethod
    def format_value(value: Any) -> str:
        """Format configuration value for display"""
        if isinstance(value, bool):
            return "true" if value else "false"
        elif isinstance(value, str):
            return f'"{value}"'
        else:
            return str(value)
class ConfigManager:
    """Enhanced configuration manager with CLI support"""
    
    def __init__(self):
        self.schema = NLMConfigSchema()
        self.converter = NLMConfigConverter()
        self.cli_helper = NLMCliHelper()
        self.manager = NLMConfigManager()
        self.current_config = None
    
    def load_config_with_cli(self, argv: List[str]) -> Dict[str, Any]:
        """Load configuration with CLI processing"""
        # Parse command line arguments
        cli_args = self.cli_helper.parse_cli_args(argv)
        
        # Determine config file and format
        config_file = cli_args['config_file']
        config_format = cli_args['config_format']
        
        # Load configuration based on format
        if config_format == 'auto' or config_format == 'json':
            if config_file.endswith('.json'):
                config_data = self.manager.load_config(config_file)
            else:
                # Try JSON first, then fallback to old format
                json_file = Path(config_file).with_suffix('.json')
                if json_file.exists():
                    config_data = self.manager.load_config(str(json_file))
                else:
                    config_data = self.manager.load_config(config_file)
        else:
            # Old format (key=value)
            config_data = self.manager.load_config(config_file)
        
        # Apply command line overrides
        if cli_args['set_values']:
            config_data.update(cli_args['set_values'])
            if cli_args['verbose'] or not cli_args['quiet']:
                print("Applied command line overrides:")
                for key, value in cli_args['set_values'].items():
                    print(f"  {key} = {self.cli_helper.format_value(value)}")
        
        # Validate configuration
        errors = self.schema.validate_config(config_data)
        if errors:
            print("Configuration validation errors:")
            for error in errors:
                print(f"  ERROR: {error}")
            config_data = self.schema.get_default_config()
        
        self.current_config = config_data
        return config_data
    
    def save_config_with_cli(self, filepath: Optional[str] = None) -> bool:
        """Save configuration with CLI processing"""
        if self.current_config is None:
            print("No configuration to save")
            return False
        
        return self.manager.save_config(self.current_config, filepath)
    
    class ConfigManager:
    """Enhanced configuration manager with CLI support"""
    
    def __init__(self):
        self.schema = NLMConfigSchema()
        self.converter = NLMConfigConverter()
        self.cli_helper = NLMCliHelper()
        self.manager = NLMConfigManager()
        self.current_config = None
    
    def load_config_with_cli(self, argv: List[str]) -> Dict[str, Any]:
        """Load configuration with CLI processing"""
        # Parse command line arguments
        cli_args = self.cli_helper.parse_cli_args(argv)
        
        # Determine config file and format
        config_file = cli_args['config_file']
        config_format = cli_args['config_format']
        
        # Load configuration based on format
        if config_format == 'auto' or config_format == 'json':
            if config_file.endswith('.json'):
                config_data = self.manager.load_config(config_file)
            else:
                # Try JSON first, then fallback to old format
                json_file = Path(config_file).with_suffix('.json')
                if json_file.exists():
                    config_data = self.manager.load_config(str(json_file))
                else:
                    config_data = self.manager.load_config(config_file)
        else:
            # Old format (key=value)
            config_data = self.manager.load_config(config_file)
        
        # Apply command line overrides
        if cli_args['set_values']:
            config_data.update(cli_args['set_values'])
            if cli_args['verbose'] or not cli_args['quiet']:
                print("Applied command line overrides:")
                for key, value in cli_args['set_values'].items():
                    print(f"  {key} = {self.cli_helper.format_value(value)}")
        
        # Validate configuration
        errors = self.schema.validate_config(config_data)
        if errors:
            print("Configuration validation errors:")
            for error in errors:
                print(f"  ERROR: {error}")
            config_data = self.schema.get_default_config()
        
        self.current_config = config_data
        return config_data
    
    def save_config_with_cli(self, filepath: Optional[str] = None) -> bool:
        """Save configuration with CLI processing"""
        if self.current_config is None:
            print("No configuration to save")
            return False
        
        return self.manager.save_config(self.current_config, filepath)
    
    def create_template(self, filepath: str):
        """Create configuration template"""
        self.manager.create_template_config(filepath)
    
    def migrate_config(self, old_filepath: str, new_filepath: Optional[str] = None):
        """Migrate configuration"""
        self.manager.migrate_config(old_filepath, new_filepath)
    
    def print_config_summary(self):
        """Print configuration summary"""
        if self.current_config is None:
            print("No configuration loaded")
            return
        
        print("Current Configuration Summary:")
        print("=" * 50)
        
        # Group configuration by category
        core_params = ["random_seed", "simulation_timestep", "neuron_count", 
                      "region_count", "connection_probability"]
        plasticity_params = ["plasticity_learning_rate", "stdp_ltp_weight", 
                           "stdp_ltd_weight", "stdp_time_constant"]
        development_params = ["development_synaptogenesis_rate", 
                            "development_pruning_rate"]
        neuromodulation_params = ["dopamine_baseline", "reward_discount_factor"]
        environment_params = ["environment_name", "environment_width", 
                           "environment_height"]
        logging_params = ["log_level", "log_to_file", "log_filename"]
        simulation_params = ["max_simulation_steps", "simulation_time_limit"]
        visualization_params = ["visualization_enabled", "visualization_update_rate"]
        
        for param_group in [("Core Simulation", core_params),
                           ("Plasticity", plasticity_params),
                           ("Development", development_params),
                           ("Neuromodulation", neuromodulation_params),
                           ("Environment", environment_params),
                           ("Logging", logging_params),
                           ("Simulation", simulation_params),
                           ("Visualization", visualization_params)]:
            category, params = param_group
            print(f"\n{category}:")
            for param in params:
                if param in self.current_config:
                    value = self.current_config[param]
                    print(f"  {param:35} = {self.cli_helper.format_value(value)}")
if __name__ == "__main__":
    # Command line interface for configuration management
    manager = ConfigManager()
    
    if len(sys.argv) > 1:
        if sys.argv[1] == '--create-template':
            if len(sys.argv) > 2:
                manager.create_template(sys.argv[2])
            else:
                print("Error: Please specify template filename")
                sys.exit(1)
        elif sys.argv[1] == '--migrate':
            if len(sys.argv) > 2:
                new_file = sys.argv[3] if len(sys.argv) > 3 else None
                manager.migrate_config(sys.argv[2], new_file)
            else:
                print("Error: Please specify old configuration file")
                sys.exit(1)
        elif sys.argv[1] == '--validate':
            if len(sys.argv) > 2:
                config_data = manager.load_config(sys.argv[2])
                errors = manager.schema.validate_config(config_data)
                if errors:
                    print("Configuration validation errors:")
                    for error in errors:
                        print(f"  ERROR: {error}")
                    sys.exit(1)
                else:
                    print("Configuration file is valid")
            else:
                print("Error: Please specify configuration file to validate")
                sys.exit(1)
        elif sys.argv[1] == '--list-configs':
            NLMCliHelper.list_config_files()
        else:
            # Parse other arguments
            config_data = manager.load_config_with_cli(sys.argv)
            print("Current configuration:")
            for key, value in sorted(config_data.items()):
                print(f"  {key}: {manager.cli_helper.format_value(value)}")
    else:
        # No arguments, just show help
        NLMCliHelper.print_help()

print("NLM Configuration Management System")
print("Type 'nlm --help' for more information")
