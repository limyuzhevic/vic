#!/usr/bin/env python3
"""
Schema generation and validation tools for NLM configuration system.

This script provides comprehensive schema generation, validation, and analysis tools.
"""

import argparse
import json
import os
import sys
import yaml
from pathlib import Path
from typing import Dict, List, Any, Optional, Set
import re

class SchemaGenerator:
    """Generate schemas from configuration files."""
    
    def __init__(self):
        self.generated_schema = {
            "version": "1.0.0",
            "fields": []
        }
    
    def analyze_config(self, config_path: str) -> Dict:
        """Analyze a configuration file and generate schema recommendations."""
        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Config file not found: {config_path}")
        
        with open(config_path, 'r') as f:
            if config_path.endswith('.json'):
                config = json.load(f)
            elif config_path.endswith(('.yaml', '.yml')):
                config = yaml.safe_load(f)
            else:
                raise ValueError(f"Unsupported config format: {config_path}")
        
        return self._analyze_config_recursive(config, "")
    
    def _analyze_config_recursive(self, config: Any, path: str) -> Dict:
        """Recursively analyze configuration structure."""
        result = {
            "fields": [],
            "nested": {}
        }
        
        if isinstance(config, dict):
            for key, value in config.items():
                full_path = f"{path}.{key}" if path else key
                field_info = self._infer_field_info(key, value)
                
                # Add field
                result["fields"].append({
                    "key": key,
                    "type": field_info["type"],
                    "required": True,  # In generated schemas, all fields are typically required
                    "description": field_info["description"],
                    "min": field_info.get("min"),
                    "max": field_info.get("max"),
                    "allowed_values": field_info.get("allowed_values")
                })
                
                # Recursively analyze nested structures
                if isinstance(value, dict) and not self._is_primitive(value):
                    nested_result = self._analyze_config_recursive(value, full_path)
                    if nested_result["fields"]:
                        result["nested"][full_path] = nested_result
        
        return result
    
    def _infer_field_info(self, key: str, value: Any) -> Dict:
        """Infer field information from a configuration value."""
        field_info = {
            "type": self._infer_type(value),
            "description": self._generate_description(key),
            "min": None,
            "max": None,
            "allowed_values": None
        }
        
        # Infer type-specific information
        if field_info["type"] in ["int", "double"]:
            field_info["min"] = self._infer_min(value)
            field_info["max"] = self._infer_max(value)
        
        return field_info
    
    def _infer_type(self, value: Any) -> str:
        """Infer the type of a configuration value."""
        if isinstance(value, bool):
            return "bool"
        elif isinstance(value, int):
            return "int"
        elif isinstance(value, float):
            return "double"
        elif isinstance(value, str):
            # Check if it's an enum-like string
            return "string"
        elif isinstance(value, list):
            if len(value) == 0:
                return "vector<string>"
            elif all(isinstance(x, int) for x in value):
                return "vector<int>"
            elif all(isinstance(x, (int, float)) for x in value):
                return "vector<double>"
            elif all(isinstance(x, str) for x in value):
                return "vector<string>"
            else:
                return "string"
        elif isinstance(value, dict):
            return "map"
        else:
            return "string"
    
    def _generate_description(self, key: str) -> str:
        """Generate a description for a configuration key."""
        # Convert key from snake_case to human-readable description
        key = key.replace('_', ' ')
        key = key.replace('-', ' ')
        
        # Handle common patterns
        descriptions = {
            "random_seed": "Random seed for reproducible experiments",
            "simulation_timestep": "Simulation timestep in seconds",
            "neuron_count": "Number of neurons in the brain",
            "region_count": "Number of brain regions",
            "connection_probability": "Connection probability for random connectivity",
            "log_level": "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)",
            "visualization_enabled": "Enable visualization",
            "max_simulation_steps": "Maximum simulation steps",
            "environment_name": "Environment name"
        }
        
        if key in descriptions:
            return descriptions[key]
        
        # Capitalize first letter and add question
        return f"Configuration for {key.replace(' ', '')}"
    
    def _infer_min(self, value: Any) -> Any:
        """Infer minimum value from a configuration value."""
        if isinstance(value, (int, float)):
            return value * 0.5 if value > 0 else value / 2
        return None
    
    def _infer_max(self, value: Any) -> Any:
        """Infer maximum value from a configuration value."""
        if isinstance(value, (int, float)):
            return value * 2
        return None
    
    def _is_primitive(self, value: Any) -> bool:
        """Check if a value is a primitive type."""
        return isinstance(value, (bool, int, float, str))
    
    def generate_schema(self, config_path: str, output_path: str) -> str:
        """Generate a schema from a configuration file."""
        analysis = self.analyze_config(config_path)
        
        # Build the complete schema
        schema = {
            "version": "1.0.0",
            "fields": analysis["fields"]
        }
        
        # Save the schema
        with open(output_path, 'w') as f:
            if output_path.endswith('.json'):
                json.dump(schema, f, indent=2)
            elif output_path.endswith(('.yaml', '.yml')):
                yaml.dump(schema, f, default_flow_style=False)
        
        return output_path


def validate_schema_command(args):
    """Command line handler for schema generation."""
    generator = SchemaGenerator()
    
    try:
        output_path = generator.generate_schema(args.input, args.output)
        print(f"Schema generated successfully: {output_path}")
        return 0
        
    except Exception as e:
        print(f"Error: {e}")
        return 1


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Schema Generator",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Generate schema command
    gen_schema_parser = subparsers.add_parser('generate-schema', help='Generate a schema from a configuration file')
    gen_schema_parser.add_argument('--input', required=True, help='Path to configuration file')
    gen_schema_parser.add_argument('--output', required=True, help='Path to output schema file')
    gen_schema_parser.set_defaults(func=validate_schema_command)
    
    # Parse arguments and execute
    args = parser.parse_args()
    
    if not hasattr(args, 'func'):
        parser.print_help()
        return 1
    
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
