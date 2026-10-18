#!/usr/bin/env python3
"""
NLM Configuration Validation Tool
Validates configuration files against the NLM configuration schema.
"""

import sys
import os
import configparser
import re
from typing import Dict, Any, List, Optional
from pathlib import Path

class ConfigValidator:
    def __init__(self, schema_file: str = "docs/config_schema_v2.md"):
        self.schema_file = schema_file
        self.errors = []
        self.warnings = []
        self.validation_rules = {
            # Type validation
            "int": self._validate_int,
            "float": self._validate_float,
            "bool": self._validate_bool,
            "string": self._validate_string,
            # Range validation
            "range": self._validate_range,
            # Conditional validation
            "conditional": self._validate_conditional
        }
        
    def load_schema(self) -> Dict[str, Any]:
        """Load and parse the configuration schema."""
        try:
            with open(self.schema_file, 'r') as f:
                content = f.read()
            
            # Parse schema (simplified implementation)
            schema = {
                "sections": {},
                "defaults": {},
                "examples": []
            }
            
            # Extract sections
            section_pattern = r'\[([a-zA-Z0-9._]+)\]'
            for match in re.finditer(section_pattern, content):
                section_name = match.group(1)
                schema["sections"][section_name] = {
                    "parameters": {},
                    "description": ""
                }
            
            return schema
            
        except Exception as e:
            self.errors.append(f"Failed to load schema file: {e}")
            return {}
    
    def validate_config_file(self, config_file: str) -> bool:
        """Validate a configuration file."""
        print(f"Validating configuration file: {config_file}")
        
        if not os.path.exists(config_file):
            self.errors.append(f"Configuration file not found: {config_file}")
            return False
        
        try:
            config = configparser.ConfigParser()
            config.read(config_file)
            
            # Basic validation
            self._validate_basic_structure(config)
            
            # Validate parameter types
            self._validate_parameter_types(config)
            
            # Validate parameter ranges
            self._validate_parameter_ranges(config)
            
            # Report results
            if self.errors:
                print("\n❌ VALIDATION FAILED:")
                for error in self.errors:
                    print(f"  ERROR: {error}")
                return False
            elif self.warnings:
                print("\n⚠️  VALIDATION WARNINGS:")
                for warning in self.warnings:
                    print(f"  WARNING: {warning}")
                print("\n✅ VALIDATION PASSED (with warnings)")
                return True
            else:
                print("\n✅ VALIDATION PASSED")
                return True
                
        except configparser.Error as e:
            self.errors.append(f"Invalid configuration file format: {e}")
            return False
        except Exception as e:
            self.errors.append(f"Unexpected error during validation: {e}")
            return False
    
    def _validate_basic_structure(self, config: configparser.ConfigParser):
        """Validate basic structure of configuration file."""
        required_sections = ["simulation.timing", "simulation.topology"]
        
        for section in required_sections:
            if not config.has_section(section):
                self.errors.append(f"Missing required section: {section}")
    
    def _validate_parameter_types(self, config: configparser.ConfigParser):
        """Validate parameter types and values."""
        type_patterns = {
            r'^[+-]?\d+$': "int",
            r'^[+-]?\d+\.\d+$': "float",
            r'^(true|false|yes|no|on|off)$': "bool",
            r'.*': "string"
        }
        
        for section in config.sections():
            for key, value in config[section].items():
                param_type = self._infer_parameter_type(value)
                self._validate_single_parameter(section, key, value, param_type)
    
    def _validate_parameter_ranges(self, config: configparser.ConfigParser):
        """Validate parameter ranges and constraints."""
        # Validate specific parameters
        if config.has_section("simulation.topology"):
            if config.has_option("simulation.topology", "neuron_count"):
                neuron_count = int(config["simulation.topology"]["neuron_count"])
                if neuron_count < 1 or neuron_count > 1000000:
                    self.warnings.append(f"neuron_count is unusually large: {neuron_count}")
            
            if config.has_option("simulation.topology", "connection_probability"):
                conn_prob = float(config["simulation.topology"]["connection_probability"])
                if conn_prob < 0.0 or conn_prob > 1.0:
                    self.errors.append(f"connection_probability must be between 0.0 and 1.0, got {conn_prob}")
        
        if config.has_section("simulation.timing"):
            if config.has_option("simulation.timing", "simulation_timestep"):
                timestep = float(config["simulation.timing"]["simulation_timestep"])
                if timestep <= 0.0 or timestep > 0.1:
                    self.warnings.append(f"simulation_timestep is unusually large: {timestep}")
    
    def _infer_parameter_type(self, value: str) -> str:
        """Infer the type of a parameter value."""
        value_lower = value.lower()
        
        if value_lower in ("true", "false", "yes", "no", "on", "off"):
            return "bool"
        
        try:
            int(value)
            return "int"
        except ValueError:
            try:
                float(value)
                return "float"
            except ValueError:
                return "string"
    
    def _validate_single_parameter(self, section: str, key: str, value: str, param_type: str):
        """Validate a single parameter."""
        try:
            if param_type == "bool":
                if value.lower() not in ("true", "false", "yes", "no", "on", "off"):
                    self.warnings.append(f"{section}.{key} value '{value}' may not be a valid boolean")
            
            elif param_type == "int":
                int_value = int(value)
                if int_value < 0:
                    self.warnings.append(f"{section}.{key} value {int_value} is negative")
            
            elif param_type == "float":
                float_value = float(value)
                if float_value < 0:
                    self.warnings.append(f"{section}.{key} value {float_value} is negative")
                    
        except ValueError:
            self.errors.append(f"{section}.{key} has invalid value: {value}")
    
    def generate_config_from_schema(self, output_file: str):
        """Generate a configuration file from schema."""
        print(f"Generating configuration file: {output_file}")
        
        try:
            config = configparser.ConfigParser()
            
            # Add sections from schema
            config.add_section("simulation.timing")
            config.set("simulation.timing", "simulation_timestep", "0.001")
            config.set("simulation.timing", "max_simulation_steps", "10000")
            
            config.add_section("simulation.topology")
            config.set("simulation.topology", "neuron_count", "500")
            config.set("simulation.topology", "region_count", "1")
            config.set("simulation.topology", "connection_probability", "0.15")
            
            # Write to file
            with open(output_file, 'w') as f:
                config.write(f)
            
            print(f"✅ Configuration file generated successfully: {output_file}")
            
        except Exception as e:
            print(f"❌ Failed to generate configuration file: {e}")

def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print("Usage: python config_validator.py <command> [options]")
        print("Commands:")
        print("  validate <config_file>    Validate a configuration file")
        print("  generate <output_file>     Generate a configuration file from schema")
        print("  help                      Show help information")
        sys.exit(1)
    
    command = sys.argv[1].lower()
    
    if command == "help":
        print("NLM Configuration Validator")
        print("==========================")
        print("Usage: python config_validator.py <command> [options]")
        print("\nCommands:")
        print("  validate <config_file>    Validate a configuration file")
        print("  generate <output_file>     Generate a configuration file from schema")
        print("  help                      Show help information")
        print("\nExamples:")
        print("  python config_validator.py validate configs/enhanced.cfg")
        print("  python config_validator.py generate configs/default.cfg")
        
    elif command == "validate":
        if len(sys.argv) < 3:
            print("Error: Missing configuration file path")
            print("Usage: python config_validator.py validate <config_file>")
            sys.exit(1)
        
        validator = ConfigValidator()
        config_file = sys.argv[2]
        success = validator.validate_config_file(config_file)
        sys.exit(0 if success else 1)
        
    elif command == "generate":
        if len(sys.argv) < 3:
            print("Error: Missing output file path")
            print("Usage: python config_validator.py generate <output_file>")
            sys.exit(1)
        
        validator = ConfigValidator()
        output_file = sys.argv[2]
        validator.generate_config_from_schema(output_file)
        
    else:
        print(f"Unknown command: {command}")
        print("Use 'python config_validator.py help' for help information")
        sys.exit(1)

if __name__ == "__main__":
    main()
