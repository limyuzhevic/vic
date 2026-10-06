#!/usr/bin/env python3
"""
Config Validation Tools for NLM

This module provides comprehensive validation tools for NLM configurations,
including schema validation, constraint checking, and detailed error reporting.
"""

import sys
import os
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, Any, List, Optional, Union
from datetime import datetime

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent / "python"))

try:
    from pynlm import Config
    from pynlm.helpers import (
        load_config_from_dict,
        save_config_to_dict,
        ConfigValidator,
        load_config_from_env,
        validate_config as validate_config_helper,
    )
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    print("Please ensure pynlm is properly installed.")
    sys.exit(1)

def load_config_file(file_path: str) -> Config:
    """Load configuration from a file"""
    ext = Path(file_path).suffix.lower()
    
    config = Config()
    
    try:
        if ext == '.json':
            with open(file_path, 'r') as f:
                data = json.load(f)
                config = load_config_from_dict(data)
        elif ext in ['.yaml', '.yml']:
            with open(file_path, 'r') as f:
                data = yaml.safe_load(f)
                config = load_config_from_dict(data)
        elif ext == '.txt':
            # Simple key=value format
            with open(file_path, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#') and '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Try to infer type
                        if value.lower() in ['true', 'false']:
                            config.set(key, value.lower() == 'true')
                        elif value.isdigit():
                            config.set(key, int(value))
                        elif _is_float(value):
                            config.set(key, float(value))
                        else:
                            config.set(key, value)
        else:
            raise ValueError(f"Unsupported file format: {ext}")
        
        return config
    except Exception as e:
        raise RuntimeError(f"Error loading config from {file_path}: {e}")

def _is_float(value: str) -> bool:
    """Check if string can be converted to float"""
    try:
        float(value)
        return True
    except ValueError:
        return False

def validate_config(config: Config, schema: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """Validate configuration against rules or schema"""
    validation_result = {
        "valid": True,
        "errors": [],
        "warnings": [],
        "info": [],
        "timestamp": datetime.now().isoformat(),
    }
    
    try:
        # Use existing ConfigValidator if available
        validator = ConfigValidator(config)
        
        # Run all validation checks
        type_valid = validator.validate_types()
        range_valid = validator.validate_ranges()
        all_valid = validator.validate_all()
        
        validation_result["valid"] = all_valid
        validation_result["errors"] = validator.errors
        validation_result["warnings"] = validator.warnings
        
        # Add additional validation if schema is provided
        if schema:
            schema_errors = _validate_against_schema(config, schema)
            validation_result["errors"].extend(schema_errors)
            validation_result["valid"] = validation_result["valid"] and not schema_errors
        
        # Check for missing required keys
        required_keys = schema.get('required', []) if schema else []
        for key in required_keys:
            if not config.has(key):
                validation_result["errors"].append(f"Required key missing: {key}")
                validation_result["valid"] = False
        
        # Add info about config
        validation_result["info"].append(f"Config has {len(config.get_keys())} keys")
        
    except Exception as e:
        validation_result["valid"] = False
        validation_result["errors"].append(f"Validation error: {e}")
    
    return validation_result

def _validate_against_schema(config: Config, schema: Dict[str, Any]) -> List[str]:
    """Validate config against a detailed schema"""
    errors = []
    
    for key, rules in schema.items():
        if not config.has(key):
            if rules.get('required', False):
                errors.append(f"Required key '{key}' is missing")
            continue
        
        value = config.get_string(key)
        
        # Type checking
        expected_type = rules.get('type')
        if expected_type:
            try:
                if expected_type == 'int':
                    int(value)
                elif expected_type == 'float':
                    float(value)
                elif expected_type == 'bool':
                    if value.lower() not in ['true', 'false', 'yes', 'no', '1', '0']:
                        errors.append(f"Key '{key}' should be boolean, got '{value}'")
                elif expected_type == 'str':
                    pass  # Already a string
            except ValueError:
                errors.append(f"Key '{key}' should be {expected_type}, got '{value}'")
        
        # Range checking
        if 'min' in rules and isinstance(value, (int, float)):
            if value < rules['min']:
                errors.append(f"Key '{key}' value {value} is less than minimum {rules['min']}")
        
        if 'max' in rules and isinstance(value, (int, float)):
            if value > rules['max']:
                errors.append(f"Key '{key}' value {value} exceeds maximum {rules['max']}")
        
        # Enum validation
        if 'enum' in rules and value not in rules['enum']:
            errors.append(f"Key '{key}' value '{value}' is not in allowed values: {rules['enum']}")
        
        # Custom validation
        if 'validate' in rules:
            try:
                if not rules['validate'](value):
                    errors.append(f"Key '{key}' failed custom validation")
            except Exception as e:
                errors.append(f"Key '{key}' custom validation error: {e}")
    
    return errors

def compare_configs(file1: str, file2: str, output_file: Optional[str] = None) -> Dict[str, Any]:
    """Compare two configuration files"""
    try:
        config1 = load_config_file(file1)
        config2 = load_config_file(file2)
        
        return compare_configs_objects(config1, config2, output_file)
    except Exception as e:
        raise RuntimeError(f"Error comparing configs: {e}")

def compare_configs_objects(config1: Config, config2: Config, output_file: Optional[str] = None) -> Dict[str, Any]:
    """Compare two Config objects"""
    try:
        # Use existing compare function if available
        try:
            from pynlm.helpers import compare_configs as compare_helper
            comparison = compare_helper(config1, config2)
        except ImportError:
            # Implement basic comparison
            keys1 = set(config1.get_keys())
            keys2 = set(config2.get_keys())
            
            common_keys = keys1.intersection(keys2)
            only_in_1 = keys1 - keys2
            only_in_2 = keys2 - keys1
            
            differences = {}
            for key in common_keys:
                try:
                    val1 = config1.get_string(key)
                    val2 = config2.get_string(key)
                    if val1 != val2:
                        differences[key] = {"from": val1, "to": val2}
                except:
                    differences[key] = {"from": "N/A", "to": "N/A"}
            
            comparison = {
                "only_in_first": list(only_in_1),
                "only_in_second": list(only_in_2),
                "common_keys": list(common_keys),
                "differences": differences,
                "summary": f"Config 1 has {len(config1.get_keys())} keys, Config 2 has {len(config2.get_keys())} keys",
            }
        
        # Save comparison if output file specified
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(comparison, f, indent=2)
        
        return comparison
    except Exception as e:
        raise RuntimeError(f"Error comparing configs: {e}")

def generate_validation_report(config: Config, schema: Optional[Dict[str, Any]] = None,
                              report_file: Optional[str] = None) -> str:
    """Generate a detailed validation report"""
    validation = validate_config(config, schema)
    
    report = f"NLM Configuration Validation Report\n"
    report += f"====================================\n"
    report += f"Generated: {validation['timestamp']}\n"
    report += f"Config keys: {len(config.get_keys())}\n"
    report += f"\n"
    
    if validation['valid']:
        report += "Status: ✓ VALID\n"
    else:
        report += "Status: ✗ INVALID\n"
    
    if validation['errors']:
        report += f"\nErrors ({len(validation['errors'])}):\n"
        for error in validation['errors']:
            report += f"  • {error}\n"
    
    if validation['warnings']:
        report += f"\nWarnings ({len(validation['warnings'])}):\n"
        for warning in validation['warnings']:
            report += f"  • {warning}\n"
    
    if validation['info']:
        report += f"\nInfo:\n"
        for info in validation['info']:
            report += f"  • {info}\n"
    
    # Save report if requested
    if report_file:
        with open(report_file, 'w') as f:
            f.write(report)
    
    return report

def validate_schema(schema_file: str) -> Dict[str, Any]:
    """Validate a schema file"""
    try:
        with open(schema_file, 'r') as f:
            schema = yaml.safe_load(f) if schema_file.endswith('.yaml') or schema_file.endswith('.yml') else json.load(f)
        
        validation_result = {
            "valid": True,
            "errors": [],
            "schema_keys": list(schema.keys()) if isinstance(schema, dict) else [],
        }
        
        # Basic schema validation
        if not isinstance(schema, dict):
            validation_result["valid"] = False
            validation_result["errors"].append("Schema must be a dictionary")
        else:
            for key, rules in schema.items():
                if not isinstance(rules, dict):
                    validation_result["valid"] = False
                    validation_result["errors"].append(f"Rules for key '{key}' must be a dictionary")
                elif 'type' not in rules:
                    validation_result["warnings"].append(f"Key '{key}' has no type specified")
        
        return validation_result
    except Exception as e:
        return {
            "valid": False,
            "errors": [f"Error loading schema: {e}"],
            "schema_keys": [],
        }

def load_from_environment(prefix: str = "NLM_") -> Dict[str, Any]:
    """Load configuration from environment variables"""
    try:
        config = load_config_from_env(prefix)
        config_dict = save_config_to_dict(config)
        
        return {
            "valid": True,
            "config": config_dict,
            "source": f"Environment variables with prefix '{prefix}'",
            "count": len(config_dict),
        }
    except Exception as e:
        return {
            "valid": False,
            "error": str(e),
            "source": f"Environment variables with prefix '{prefix}'",
        }

def export_to_environment(config_file: str, prefix: str = "NLM_", unset: bool = False) -> Dict[str, Any]:
    """Export configuration to environment variables"""
    try:
        config = load_config_file(config_file)
        
        if unset:
            # Unset environment variables
            for key in list(os.environ.keys()):
                if key.startswith(prefix):
                    del os.environ[key]
            
            return {
                "valid": True,
                "action": "unset",
                "prefix": prefix,
                "count": 0,
            }
        else:
            # Export to environment
            config_dict = save_config_to_dict(config)
            
            for key, value in config_dict.items():
                env_key = prefix + key.upper()
                os.environ[env_key] = str(value)
            
            return {
                "valid": True,
                "action": "export",
                "prefix": prefix,
                "count": len(config_dict),
                "variables": list(config_dict.keys()),
            }
    except Exception as e:
        return {
            "valid": False,
            "error": str(e),
            "action": "export",
            "prefix": prefix,
        }

def main():
    parser = argparse.ArgumentParser(
        description="NLM Config Validation Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Validate a configuration file
  python3 config_validation.py validate config.yaml
  
  # Validate with detailed report
  python3 config_validation.py validate --report report.txt config.yaml
  
  # Compare two configuration files
  python3 config_validation.py compare config1.yaml config2.yaml
  
  # Load from environment
  python3 config_validation.py env-load
  
  # Export to environment
  python3 config_validation.py env-export config.yaml
  
  # Validate schema
  python3 config_validation.py validate-schema schema.yaml
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate configuration file')
    validate_parser.add_argument('file', help='Configuration file to validate')
    validate_parser.add_argument('--schema', help='Schema file for validation')
    validate_parser.add_argument('--report', '-r', help='Save validation report to file')
    validate_parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    
    # Compare command
    compare_parser = subparsers.add_parser('compare', help='Compare two configuration files')
    compare_parser.add_argument('file1', help='First configuration file')
    compare_parser.add_argument('file2', help='Second configuration file')
    compare_parser.add_argument('--output', '-o', help='Save comparison to file')
    
    # Schema validation command
    schema_parser = subparsers.add_parser('validate-schema', help='Validate schema file')
    schema_parser.add_argument('file', help='Schema file to validate')
    
    # Environment load command
    env_load_parser = subparsers.add_parser('env-load', help='Load configuration from environment variables')
    env_load_parser.add_argument('--prefix', default='NLM_', help='Environment variable prefix')
    env_load_parser.add_argument('--report', '-r', help='Save loaded config to file')
    
    # Environment export command
    env_export_parser = subparsers.add_parser('env-export', help='Export configuration to environment variables')
    env_export_parser.add_argument('file', help='Configuration file to export')
    env_export_parser.add_argument('--prefix', default='NLM_', help='Environment variable prefix')
    env_export_parser.add_argument('--unset', action='store_true', help='Unset existing environment variables')
    
    args = parser.parse_args()
    
    try:
        if args.command == 'validate':
            config = load_config_file(args.file)
            
            # Load schema if provided
            schema = None
            if args.schema:
                schema = load_config_file(args.schema)
                # Convert to dict for validation
                schema_dict = save_config_to_dict(schema)
                
                # Validate against schema
                validation = validate_config(config, schema_dict)
            else:
                # Use built-in validation
                validation = validate_config(config)
            
            # Generate report if requested
            if args.report:
                report = generate_validation_report(config, schema_dict if 'schema_dict' in locals() else None, args.report)
                print(f"Validation report saved to {args.report}")
            
            # Print validation results
            if args.verbose:
                print(json.dumps(validation, indent=2))
            else:
                if validation['valid']:
                    print("✓ Configuration is valid")
                else:
                    print("✗ Configuration is invalid")
                    for error in validation['errors']:
                        print(f"  Error: {error}")
                    for warning in validation['warnings']:
                        print(f"  Warning: {warning}")
        
        elif args.command == 'compare':
            comparison = compare_configs(args.file1, args.file2, args.output)
            
            if args.output:
                print(f"Comparison saved to {args.output}")
            
            print("Comparison results:")
            print(f"  Keys only in {Path(args.file1).name}: {comparison['only_in_first']}")
            print(f"  Keys only in {Path(args.file2).name}: {comparison['only_in_second']}")
            print(f"  Common keys: {len(comparison['common_keys'])}")
            
            if comparison['differences']:
                print(f"  Differences: {len(comparison['differences'])}")
        
        elif args.command == 'validate-schema':
            result = validate_schema(args.file)
            
            if result['valid']:
                print("✓ Schema is valid")
                print(f"  Schema keys: {', '.join(result['schema_keys'])}")
            else:
                print("✗ Schema is invalid")
                for error in result['errors']:
                    print(f"  Error: {error}")
        
        elif args.command == 'env-load':
            result = load_from_environment(args.prefix)
            
            if result['valid']:
                print("✓ Environment configuration loaded successfully")
                print(f"  Source: {result['source']}")
                print(f"  Keys: {result['count']}")
                
                if args.report:
                    with open(args.report, 'w') as f:
                        json.dump(result['config'], f, indent=2)
                    print(f"  Saved to {args.report}")
            else:
                print(f"✗ Failed to load environment configuration: {result['error']}")
        
        elif args.command == 'env-export':
            result = export_to_environment(args.file, args.prefix, args.unset)
            
            if result['valid']:
                if result['action'] == 'unset':
                    print(f"✓ Unset {result['count']} environment variables with prefix '{args.prefix}'")
                else:
                    print(f"✓ Exported {result['count']} configuration keys to environment variables")
                    print(f"  Prefix: {args.prefix}")
                    print(f"  Variables: {', '.join(result['variables'])}")
            else:
                print(f"✗ Failed to export configuration: {result['error']}")
        
        else:
            parser.print_help()
    
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()