#!/usr/bin/env python3
"""
Schema validation tools for NLM configuration system.

This script provides command-line tools for validating configuration files
against schemas, generating schemas from configurations, and other schema-related tasks.
"""

import argparse
import json
import os
import sys
import yaml
from pathlib import Path
from typing import Dict, List, Any, Optional

# Add the src directory to the path so we can import NLM modules
sys.path.insert(0, str(Path(__file__).parent / "src"))

class SchemaValidator:
    """Validator for NLM configuration schemas."""
    
    def __init__(self, schema_path: Optional[str] = None):
        self.schema_path = schema_path
        self.schema = None
    
    def load_schema(self, schema_path: str = None) -> Dict:
        """Load a schema from a file."""
        if schema_path is None:
            schema_path = self.schema_path
        
        if not schema_path:
            raise ValueError("No schema path provided")
        
        if not os.path.exists(schema_path):
            raise FileNotFoundError(f"Schema file not found: {schema_path}")
        
        with open(schema_path, 'r') as f:
            if schema_path.endswith('.json'):
                self.schema = json.load(f)
            elif schema_path.endswith(('.yaml', '.yml')):
                self.schema = yaml.safe_load(f)
            else:
                raise ValueError(f"Unsupported schema format: {schema_path}")
        
        return self.schema
    
    def validate_config(self, config_path: str) -> Dict[str, Any]:
        """Validate a configuration file against the loaded schema."""
        if not self.schema:
            raise ValueError("No schema loaded")
        
        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Config file not found: {config_path}")
        
        with open(config_path, 'r') as f:
            if config_path.endswith('.json'):
                config = json.load(f)
            elif config_path.endswith(('.yaml', '.yml')):
                config = yaml.safe_load(f)
            else:
                raise ValueError(f"Unsupported config format: {config_path}")
        
        result = {
            "valid": True,
            "errors": [],
            "warnings": [],
            "validated_at": ""  # Would need datetime module
        }
        
        # Simple validation logic
        self._validate_config_against_schema(config, self.schema, result)
        
        return result
    
    def _validate_config_against_schema(self, config: Dict, schema: Dict, result: Dict):
        """Internal method to validate config against schema."""
        # Check required fields
        if "fields" in schema:
            for field in schema["fields"]:
                field_name = field.get("key")
                field_required = field.get("required", False)
                field_type = field.get("type", "string")
                
                if field_name not in config and field_required:
                    result["valid"] = False
                    result["errors"].append(f"Required field '{field_name}' is missing")
                
                if field_name in config:
                    # Check type
                    if not self._check_type(config[field_name], field_type):
                        result["valid"] = False
                        result["errors"].append(f"Field '{field_name}' has wrong type. Expected {field_type}")
                    
                    # Check value ranges if specified
                    if "min" in field and field["min"] is not None:
                        if not self._check_range(config[field_name], field["min"], field.get("max")):
                            result["valid"] = False
                            result["errors"].append(f"Field '{field_name}' value {config[field_name]} is out of range [{field['min']}, {field.get('max', '')}]")
                    
                    # Check allowed values if specified
                    if "allowed_values" in field and field["allowed_values"]:
                        if config[field_name] not in field["allowed_values"]:
                            result["valid"] = False
                            result["errors"].append(f"Field '{field_name}' value '{config[field_name]}' is not in allowed values: {field['allowed_values']}")
    
    def _check_type(self, value: Any, expected_type: str) -> bool:
        """Check if a value has the expected type."""
        if expected_type == "int":
            return isinstance(value, int)
        elif expected_type == "double":
            return isinstance(value, (int, float))
        elif expected_type == "bool":
            return isinstance(value, bool)
        elif expected_type == "string":
            return isinstance(value, str)
        elif expected_type == "vector<int>":
            return isinstance(value, list) and all(isinstance(x, int) for x in value)
        elif expected_type == "vector<double>":
            return isinstance(value, list) and all(isinstance(x, (int, float)) for x in value)
        elif expected_type == "vector<string>":
            return isinstance(value, list) and all(isinstance(x, str) for x in value)
        elif expected_type == "map":
            return isinstance(value, dict)
        else:
            return True
    
    def _check_range(self, value: Any, min_val: Any, max_val: Any) -> bool:
        """Check if a value is within the specified range."""
        try:
            if isinstance(value, (int, float)):
                if min_val is not None and value < min_val:
                    return False
                if max_val is not None and value > max_val:
                    return False
                return True
        except:
            pass
        
        return True

def validate_command(args):
    """Command line handler for validation."""
    validator = SchemaValidator(args.schema)
    
    try:
        result = validator.validate_config(args.config)
        
        print("Validation Results:")
        print(f"Valid: {result['valid']}")
        
        if result['errors']:
            print("\nErrors:")
            for error in result['errors']:
                print(f"  - {error}")
        
        if result['warnings']:
            print("\nWarnings:")
            for warning in result['warnings']:
                print(f"  - {warning}")
        
        return 0 if result['valid'] else 1
        
    except Exception as e:
        print(f"Error: {e}")
        return 1


def generate_schema_command(args):
    """Command line handler for schema generation."""
    # This would generate a schema from a configuration file
    # For now, just print a placeholder
    print("Schema generation from configuration file (placeholder)")
    print("This would generate a schema based on the structure and types found in the configuration.")
    return 0


def generate_docs_command(args):
    """Command line handler for documentation generation."""
    # This would generate documentation from a schema
    # For now, just print a placeholder
    print("Schema documentation generation (placeholder)")
    print("This would generate documentation from the schema.")
    return 0


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Configuration Schema Validation Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s validate --schema phase1_schema.json --config config.json
  %(prog)s generate-schema --input config.json --output schema.json
  %(prog)s generate-docs --schema phase1_schema.json --output docs.md
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate a configuration against a schema')
    validate_parser.add_argument('--schema', required=True, help='Path to schema file')
    validate_parser.add_argument('--config', required=True, help='Path to configuration file')
    validate_parser.set_defaults(func=validate_command)
    
    # Generate schema command
    gen_schema_parser = subparsers.add_parser('generate-schema', help='Generate a schema from a configuration file')
    gen_schema_parser.add_argument('--input', required=True, help='Path to configuration file')
    gen_schema_parser.add_argument('--output', required=True, help='Path to output schema file')
    gen_schema_parser.set_defaults(func=generate_schema_command)
    
    # Generate documentation command
    gen_docs_parser = subparsers.add_parser('generate-docs', help='Generate documentation from a schema')
    gen_docs_parser.add_argument('--schema', required=True, help='Path to schema file')
    gen_docs_parser.add_argument('--output', required=True, help='Path to output documentation file')
    gen_docs_parser.set_defaults(func=generate_docs_command)
    
    # Parse arguments and execute
    args = parser.parse_args()
    
    if not hasattr(args, 'func'):
        parser.print_help()
        return 1
    
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
