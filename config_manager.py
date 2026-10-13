#!/usr/bin/env bash
# Advanced Configuration Manager
# Supports JSON, YAML, and custom formats with validation and schema checking

import os
import json
import sys
from pathlib import Path
from typing import Dict, Any, Optional, Union
from dataclasses import dataclass

@dataclass
class ConfigValidationError:
    field: str
    message: str
    value: Any = None

@dataclass
class ConfigSchema:
    fields: Dict[str, Dict[str, Any]]
    required_fields: list
    validation_rules: Dict[str, callable]

class AdvancedConfigManager:
    def __init__(self):
        self.config: Dict[str, Any] = {}
        self.schema: Optional[ConfigSchema] = None
        self.file_path: Optional[str] = None
    
    def load_config(self, file_path: str) -> bool:
        """Load configuration from file with format detection"""
        self.file_path = file_path
        
        if not os.path.exists(file_path):
            print(f"Warning: Config file '{file_path}' not found, using defaults")
            return False
        
        # Detect file format
        if file_path.endswith('.json'):
            return self._load_json(file_path)
        elif file_path.endswith('.yaml') or file_path.endswith('.yml'):
            return self._load_yaml(file_path)
        elif file_path.endswith('.cfg') or file_path.endswith('.ini'):
            return self._load_simple_config(file_path)
        else:
            print(f"Warning: Unknown file format '{file_path}', trying simple config")
            return self._load_simple_config(file_path)
    
    def _load_json(self, file_path: str) -> bool:
        """Load JSON configuration file"""
        try:
            with open(file_path, 'r') as f:
                self.config = json.load(f)
            print(f"Loaded JSON config from: {file_path}")
            return True
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in '{file_path}': {e}")
            return False
        except Exception as e:
            print(f"Error reading JSON config: {e}")
            return False
    
    def _load_yaml(self, file_path: str) -> bool:
        """Load YAML configuration file"""
        try:
            import yaml
            with open(file_path, 'r') as f:
                self.config = yaml.safe_load(f) or {}
            print(f"Loaded YAML config from: {file_path}")
            return True
        except ImportError:
            print("Error: PyYAML not installed. Install with: pip install PyYAML")
            return False
        except Exception as e:
            print(f"Error reading YAML config: {e}")
            return False
    
    def _load_simple_config(self, file_path: str) -> bool:
        """Load simple key=value configuration file"""
        try:
            with open(file_path, 'r') as f:
                for line in f:
                    line = line.strip()
                    if not line or line.startswith('#') or line.startswith('//'):
                        continue
                    
                    if '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Convert value to appropriate type
                        converted_value = self._convert_value(value)
                        self.config[key] = converted_value
            
            print(f"Loaded config from: {file_path}")
            return True
        except Exception as e:
            print(f"Error reading config file: {e}")
            return False
    
    def _convert_value(self, value: str) -> Any:
        """Convert string value to appropriate type"""
        # Remove quotes
        if value.startswith('"') and value.endswith('"'):
            value = value[1:-1]
        elif value.startswith("'") and value.endswith("'"):
            value = value[1:-1]
        
        # Try to convert to boolean
        if value.lower() in ('true', 'yes', '1'):
            return True
        elif value.lower() in ('false', 'no', '0'):
            return False
        
        # Try to convert to int
        try:
            if '.' not in value:
                return int(value)
        except ValueError:
            pass
        
        # Try to convert to float
        try:
            return float(value)
        except ValueError:
            pass
        
        # Return as string
        return value
    
    def validate_config(self) -> list:
        """Validate configuration against schema"""
        errors = []
        
        if not self.schema:
            return errors
        
        # Check required fields
        for field in self.schema.required_fields:
            if field not in self.config:
                errors.append(ConfigValidationError(
                    field, f"Required field '{field}' is missing"
                ))
        
        # Validate each field
        for field, field_config in self.schema.fields.items():
            if field in self.config:
                value = self.config[field]
                
                # Check type
                expected_type = field_config.get('type')
                if expected_type and not self._check_type(value, expected_type):
                    errors.append(ConfigValidationError(
                        field, f"Expected type '{expected_type}', got '{type(value).__name__}'"
                    ))
                
                # Check validation rules
                if 'validation' in field_config:
                    validator = field_config['validation']
                    if not validator(value):
                        errors.append(ConfigValidationError(
                            field, f"Validation failed for field '{field}'"
                        ))
                
                # Check min/max for numeric types
                if 'min' in field_config and isinstance(value, (int, float)):
                    if value < field_config['min']:
                        errors.append(ConfigValidationError(
                            field, f"Value {value} is less than minimum {field_config['min']}"
                        ))
                
                if 'max' in field_config and isinstance(value, (int, float)):
                    if value > field_config['max']:
                        errors.append(ConfigValidationError(
                            field, f"Value {value} is greater than maximum {field_config['max']}"
                        ))
        
        return errors
    
    def _check_type(self, value: Any, expected_type: str) -> bool:
        """Check if value matches expected type"""
        type_mapping = {
            'int': int,
            'float': (int, float),
            'string': str,
            'bool': bool,
            'list': list,
            'dict': dict
        }
        
        expected_py_type = type_mapping.get(expected_type)
        if expected_py_type:
            return isinstance(value, expected_py_type)
        
        return False
    
    def set_schema(self, schema_file: Optional[str] = None, 
                   schema_dict: Optional[Dict] = None) -> bool:
        """Load or set configuration schema"""
        if schema_file:
            try:
                if schema_file.endswith('.json'):
                    import json
                    with open(schema_file, 'r') as f:
                        schema_dict = json.load(f)
                elif schema_file.endswith('.yaml') or schema_file.endswith('.yml'):
                    import yaml
                    with open(schema_file, 'r') as f:
                        schema_dict = yaml.safe_load(f)
            except Exception as e:
                print(f"Error loading schema file: {e}")
                return False
        
        if not schema_dict:
            return False
        
        try:
            self.schema = ConfigSchema(
                fields=schema_dict.get('fields', {}),
                required_fields=schema_dict.get('required_fields', []),
                validation_rules=schema_dict.get('validation_rules', {})
            )
            return True
        except Exception as e:
            print(f"Error creating schema: {e}")
            return False
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with default"""
        return self.config.get(key, default)
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value"""
        self.config[key] = value
    
    def get_section(self, section: str) -> Dict[str, Any]:
        """Get configuration section (for dotted notation)"""
        result = {}
        for key, value in self.config.items():
            if key.startswith(section + '.'):
                result[key[len(section) + 1:]] = value
        return result
    
    def merge_with(self, other_config: Dict[str, Any]) -> None:
        """Merge with another configuration"""
        # Deep merge for nested dictionaries
        for key, value in other_config.items():
            if key in self.config and isinstance(self.config[key], dict) and isinstance(value, dict):
                self._deep_merge(self.config[key], value)
            else:
                self.config[key] = value
    
    def _deep_merge(self, target: Dict, source: Dict) -> None:
        """Deep merge two dictionaries"""
        for key, value in source.items():
            if key in target and isinstance(target[key], dict) and isinstance(value, dict):
                self._deep_merge(target[key], value)
            else:
                target[key] = value
    
    def export(self, format: str = 'json', output_file: Optional[str] = None) -> Union[str, bool]:
        """Export configuration to different formats"""
        if format == 'json':
            result = json.dumps(self.config, indent=2)
        elif format == 'yaml':
            try:
                import yaml
                result = yaml.dump(self.config, default_flow_style=False)
            except ImportError:
                print("Error: PyYAML not installed for YAML export")
                return False
        elif format == 'ini':
            result = self._export_ini()
        else:
            print(f"Error: Unsupported export format: {format}")
            return False
        
        if output_file:
            try:
                with open(output_file, 'w') as f:
                    f.write(result)
                print(f"Configuration exported to: {output_file}")
                return True
            except Exception as e:
                print(f"Error writing to file: {e}")
                return False
        
        return result
    
    def _export_ini(self) -> str:
        """Export configuration to INI format"""
        lines = []
        for section in set(key.split('.')[0] for key in self.config.keys()):
            lines.append(f"[{section}]")
            for key, value in self.config.items():
                if key.startswith(section + '.'):
                    subkey = key[len(section) + 1:]
                    lines.append(f"{subkey} = {value}")
            lines.append("")
        return "\n".join(lines)
    
    def get_validation_errors(self) -> list:
        """Get validation errors for the current configuration"""
        return self.validate_config()
    
    def is_valid(self) -> bool:
        """Check if configuration is valid"""
        return len(self.get_validation_errors()) == 0
    
    def display(self, section: Optional[str] = None) -> None:
        """Display configuration"""
        if section:
            config_to_show = self.get_section(section)
        else:
            config_to_show = self.config
        
        print("Configuration:")
        for key, value in config_to_show.items():
            print(f"  {key}: {value} ({type(value).__name__})")
        
        # Show validation errors if schema is set
        if self.schema and not self.is_valid():
            print("\nValidation errors:")
            for error in self.get_validation_errors():
                print(f"  {error.field}: {error.message}")

# Command-line interface for configuration manager
if __name__ == '__main__':
    import argparse
    
    parser = argparse.ArgumentParser(description='Advanced Configuration Manager')
    parser.add_argument('--file', '-f', help='Configuration file path')
    parser.add_argument('--schema', help='Schema file path for validation')
    
    subparsers = parser.add_subparsers(dest='command', help='Commands')
    
    # Load command
    load_parser = subparsers.add_parser('load', help='Load configuration file')
    load_parser.add_argument('file', help='Configuration file path')
    
    # Show command
    show_parser = subparsers.add_parser('show', help='Show configuration')
    show_parser.add_argument('--section', help='Configuration section')
    
    # Get command
    get_parser = subparsers.add_parser('get', help='Get configuration value')
    get_parser.add_argument('key', help='Configuration key (dot notation for sections)')
    
    # Set command
    set_parser = subparsers.add_parser('set', help='Set configuration value')
    set_parser.add_argument('key', help='Configuration key')
    set_parser.add_argument('value', help='Configuration value')
    
    # Export command
    export_parser = subparsers.add_parser('export', help='Export configuration')
    export_parser.add_argument('--format', choices=['json', 'yaml', 'ini'], default='json', help='Export format')
    export_parser.add_argument('--output', help='Output file path')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate configuration')
    
    args = parser.parse_args()
    
    # Initialize config manager
    config_manager = AdvancedConfigManager()
    
    if args.command == 'load':
        if args.schema:
            config_manager.set_schema(args.schema)
        if config_manager.load_config(args.file):
            if args.schema:
                errors = config_manager.get_validation_errors()
                if errors:
                    print("Configuration validation failed:")
                    for error in errors:
                        print(f"  {error.field}: {error.message}")
                else:
                    print("Configuration is valid")
        else:
            print("Failed to load configuration")
    
    elif args.command == 'show':
        config_manager.load_config(args.file)
        config_manager.display(args.section)
    
    elif args.command == 'get':
        config_manager.load_config(args.file)
        value = config_manager.get(args.key)
        if value is not None:
            print(f"{args.key}: {value}")
        else:
            print(f"Key '{args.key}' not found")
    
    elif args.command == 'set':
        config_manager.load_config(args.file)
        config_manager.set(args.key, args.value)
        config_manager.export(args.format, args.file)
        print(f"Configuration updated and saved to {args.file}")
    
    elif args.command == 'export':
        config_manager.load_config(args.file)
        config_manager.export(args.format, args.output)
    
    elif args.command == 'validate':
        if args.file:
            config_manager.load_config(args.file)
        if args.schema:
            config_manager.set_schema(args.schema)
        
        errors = config_manager.get_validation_errors()
        if errors:
            print("Validation failed:")
            for error in errors:
                print(f"  {error.field}: {error.message}")
        else:
            print("Configuration is valid")
