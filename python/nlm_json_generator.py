#!/usr/bin/env python3
"""
NLM JSON Configuration File Generator

This module provides functionality to generate JSON configuration files
for NLM, compatible with the standard .cfg format but in JSON format
for better programmatic access and validation.
"""

import json
import os
import sys
from typing import Dict, Any, List, Optional
from dataclasses import dataclass, asdict
import argparse
from datetime import datetime

@dataclass
class NLMConfigSchema:
    """Schema for NLM configuration"""
    random_seed: int = 42
    simulation_timestep: float = 0.001
    neuron_count: int = 1000
    region_count: int = 2
    connection_probability: float = 0.1
    plasticity_learning_rate: float = 0.01
    stdp_ltp_weight: float = 0.01
    stdp_ltd_weight: float = 0.012
    stdp_time_constant: float = 20.0
    synaptogenesis_rate: float = 0.001
    pruning_rate: float = 0.0001
    maturation_rate: float = 0.01
    dopamine_baseline: float = 0.1
    curiosity_baseline: float = 0.0
    novelty_baseline: float = 0.1
    reward_discount_factor: float = 0.99
    environment_name: str = "GridWorld"
    environment_width: int = 10
    environment_height: int = 10
    log_level: str = "INFO"
    log_to_file: bool = False
    log_filename: str = "nlm.log"
    max_simulation_steps: int = 10000
    simulation_time_limit: float = 0.0
    visualization_enabled: bool = False
    visualization_update_rate: float = 30.0

class NLMJSONConfigGenerator:
    """Generate JSON configuration files for NLM"""
    
    def __init__(self, schema: Optional[NLMConfigSchema] = None):
        self.schema = schema or NLMConfigSchema()
        self.template_definitions = self.load_template_definitions()
    
    def load_template_definitions(self) -> Dict[str, Any]:
        """Load template definitions"""
        return {
            "basic": {
                "name": "Basic Configuration",
                "description": "Minimal configuration for basic NLM functionality",
                "random_seed": 42,
                "simulation_timestep": 0.001,
                "neuron_count": 500,
                "region_count": 1,
                "connection_probability": 0.15,
                "plasticity_learning_rate": 0.01,
                "log_level": "INFO",
                "max_simulation_steps": 5000
            },
            "advanced": {
                "name": "Advanced Configuration",
                "description": "Advanced configuration with enhanced features",
                "random_seed": 12345,
                "simulation_timestep": 0.0005,
                "neuron_count": 5000,
                "region_count": 5,
                "connection_probability": 0.05,
                "plasticity_learning_rate": 0.005,
                "stdp_ltp_weight": 0.02,
                "stdp_ltd_weight": 0.018,
                "stdp_time_constant": 15.0,
                "synaptogenesis_rate": 0.0005,
                "pruning_rate": 0.00005,
                "maturation_rate": 0.02,
                "dopamine_baseline": 0.2,
                "curiosity_baseline": 0.3,
                "novelty_baseline": 0.1,
                "reward_discount_factor": 0.95,
                "environment_name": "ComplexWorld",
                "environment_width": 20,
                "environment_height": 20,
                "log_level": "DEBUG",
                "log_to_file": True,
                "log_filename": "nlm_advanced.log",
                "max_simulation_steps": 50000,
                "simulation_time_limit": 100.0,
                "visualization_enabled": True,
                "visualization_update_rate": 60.0
            },
            "research": {
                "name": "Research Configuration",
                "description": "Configuration optimized for research experiments",
                "random_seed": 999,
                "simulation_timestep": 0.0001,
                "neuron_count": 10000,
                "region_count": 10,
                "connection_probability": 0.02,
                "plasticity_learning_rate": 0.001,
                "stdp_ltp_weight": 0.005,
                "stdp_ltd_weight": 0.004,
                "stdp_time_constant": 10.0,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001,
                "maturation_rate": 0.005,
                "dopamine_baseline": 0.5,
                "curiosity_baseline": 0.7,
                "novelty_baseline": 0.3,
                "reward_discount_factor": 0.9,
                "environment_name": "ResearchWorld",
                "environment_width": 50,
                "environment_height": 50,
                "log_level": "WARNING",
                "log_to_file": True,
                "log_filename": "nlm_research.log",
                "max_simulation_steps": 200000,
                "simulation_time_limit": 500.0,
                "visualization_enabled": False,
                "visualization_update_rate": 10.0
            }
        }
    
    def generate_config(self, filename: str, schema: Optional[NLMConfigSchema] = None,
                       template: Optional[str] = None, custom_values: Optional[Dict[str, Any]] = None) -> bool:
        """
        Generate a JSON configuration file
        
        Args:
            filename: Output filename
            schema: Configuration schema (uses default if None)
            template: Template name (basic, advanced, research)
            custom_values: Custom values to override schema
            
        Returns:
            True if successful, False otherwise
        """
        try:
            working_schema = schema or self.schema
            
            # Apply template if specified
            if template and template in self.template_definitions:
                template_data = self.template_definitions[template]
                for key, value in template_data.items():
                    if hasattr(working_schema, key):
                        setattr(working_schema, key, value)
            
            # Create configuration dictionary
            config = self._schema_to_dict(working_schema)
            
            # Apply custom values
            if custom_values:
                for key, value in custom_values.items():
                    if key in config:
                        config[key] = value
            
            # Add metadata
            config["metadata"] = {
                "generated_by": "NLM JSON Config Generator",
                "version": "1.0.0",
                "generated_at": datetime.now().isoformat(),
                "schema_version": "1.0",
                "template_used": template if template else "custom"
            }
            
            # Write to file
            with open(filename, 'w') as f:
                json.dump(config, f, indent=2)
            
            print(f"Configuration generated successfully: {filename}")
            return True
            
        except Exception as e:
            print(f"Error generating configuration: {e}")
            return False
    
    def _schema_to_dict(self, schema: NLMConfigSchema) -> Dict[str, Any]:n        """Convert schema to dictionary"""
        result = {}
        for field_name in schema.__dataclass_fields__:
            if field_name != "metadata":
                result[field_name] = getattr(schema, field_name)
        return result
    
    def generate_from_preset(self, preset_name: str, filename: str) -> bool:
        """
        Generate configuration from a preset
        
        Args:
            preset_name: Name of preset (basic, advanced, research)
            filename: Output filename
            
        Returns:
            True if successful, False otherwise
        """
        if preset_name not in self.template_definitions:
            print(f"Error: Unknown preset '{preset_name}'")
            return False
        
        return self.generate_config(filename, template=preset_name)
    
    def generate_with_modifications(self, base_preset: str, modifications: Dict[str, Any], 
                                    filename: str) -> bool:
        """
        Generate configuration with modifications to a base preset
        
        Args:
            base_preset: Base preset name
            modifications: Dictionary of modifications
            filename: Output filename
            
        Returns:
            True if successful, False otherwise
        """
        if base_preset not in self.template_definitions:
            print(f"Error: Unknown base preset '{base_preset}'")
            return False
        
        # Start with base preset
        success = self.generate_config(filename, template=base_preset)
        if not success:
            return False
        
        # Apply modifications
        try:
            with open(filename, 'r') as f:
                config = json.load(f)
            
            # Remove metadata for modification
            metadata = config.pop("metadata", {})
            
            # Apply modifications
            for key, value in modifications.items():
                if key in config:
                    config[key] = value
                else:
                    print(f"Warning: Key '{key}' not found in configuration")
            
            # Restore metadata
            config["metadata"] = metadata
            config["metadata"]["modified"] = True
            config["metadata"]["modification_notes"] = 
                f"Modified from {base_preset} preset"
            
            # Write back
            with open(filename, 'w') as f:
                json.dump(config, f, indent=2)
            
            print(f"Configuration generated and modified: {filename}")
            return True
            
        except Exception as e:
            print(f"Error modifying configuration: {e}")
            return False
    
    def validate_config(self, filename: str) -> bool:
        """
        Validate a JSON configuration file
        
        Args:
            filename: Configuration file to validate
            
        Returns:
            True if valid, False otherwise
        """
        try:
            with open(filename, 'r') as f:
                config = json.load(f)
            
            # Check required fields
            required_fields = [
                "random_seed", "simulation_timestep", "neuron_count", 
                "region_count", "connection_probability"
            ]
            
            for field in required_fields:
                if field not in config:
                    print(f"Error: Missing required field '{field}'")
                    return False
            
            # Validate field types
            if not isinstance(config["random_seed"], int):
                print("Error: random_seed must be an integer")
                return False
            
            if not isinstance(config["simulation_timestep"], (int, float)):
                print("Error: simulation_timestep must be a number")
                return False
            
            if not isinstance(config["neuron_count"], int):
                print("Error: neuron_count must be an integer")
                return False
            
            if not isinstance(config["region_count"], int):
                print("Error: region_count must be an integer")
                return False
            
            if not isinstance(config["connection_probability"], (int, float)):
                print("Error: connection_probability must be a number")
                return False
            
            # Check value ranges
            if config["random_seed"] < 0:
                print("Error: random_seed must be non-negative")
                return False
            
            if config["simulation_timestep"] <= 0 or config["simulation_timestep"] > 1.0:
                print("Error: simulation_timestep must be between 0 and 1.0")
                return False
            
            if config["neuron_count"] <= 0:
                print("Error: neuron_count must be positive")
                return False
            
            if config["region_count"] <= 0 or config["region_count"] > 20:
                print("Error: region_count must be between 1 and 20")
                return False
            
            if config["connection_probability"] < 0 or config["connection_probability"] > 1:
                print("Error: connection_probability must be between 0 and 1")
                return False
            
            print("Configuration is valid")
            return True
            
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in configuration file: {e}")
            return False
        except Exception as e:
            print(f"Error validating configuration: {e}")
            return False
    
    def list_presets(self):
        """List available configuration presets"""
        print("Available configuration presets:")
        for preset_name, preset_data in self.template_definitions.items():
            print(f"\n{preset_name.upper()}:")
            print(f"  Name: {preset_data['name']}")
            print(f"  Description: {preset_data['description']}")
            print(f"  Key Parameters:")
            for key in ["random_seed", "neuron_count", "region_count", "log_level"]:
                if key in preset_data:
                    print(f"    {key}: {preset_data[key]}")
    
    def convert_to_legacy_format(self, json_filename: str, legacy_filename: str) -> bool:
        """
        Convert JSON configuration to legacy .cfg format
        
        Args:
            json_filename: Input JSON file
            legacy_filename: Output .cfg file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            with open(json_filename, 'r') as f:
                config = json.load(f)
            
            with open(legacy_filename, 'w') as f:
                f.write("# NLM Configuration File (Converted from JSON)\n")
                f.write(f"# Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
                f.write(f"# Source: {json_filename}\n")
                f.write("\n")
                
                for key, value in config.items():
                    if key == "metadata":
                        continue
                    f.write(f"{key} = {value}\n")
            
            print(f"Configuration converted to legacy format: {legacy_filename}")
            return True
            
        except Exception as e:
            print(f"Error converting configuration: {e}")
            return False
    
    def generate_experiment_config(self, experiment_name: str, parameters: Dict[str, Any], 
                                   filename: str) -> bool:
        """
        Generate configuration for a specific experiment
        
        Args:
            experiment_name: Name of the experiment
            parameters: Experiment-specific parameters
            filename: Output filename
            
        Returns:
            True if successful, False otherwise
        """
        # Use a basic template as base
        config = self.template_definitions["basic"].copy()
        
        # Update with experiment parameters
        config.update(parameters)
        
        # Add experiment metadata
        config["metadata"] = {
            "generated_by": "NLM Experiment Config Generator",
            "experiment_name": experiment_name,
            "generated_at": datetime.now().isoformat(),
            "purpose": "experimental setup",
            "parameters": parameters
        }
        
        # Write to file
        with open(filename, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Experiment configuration generated: {filename}")
        print(f"Experiment: {experiment_name}")
        return True
    
    def validate_experiment_setup(self, experiment_name: str, config: Dict[str, Any]) -> bool:
        """
        Validate experiment setup parameters
        
        Args:
            experiment_name: Name of the experiment
            config: Configuration dictionary
            
        Returns:
            True if valid, False otherwise
        """
        print(f"Validating experiment setup: {experiment_name}")
        
        # Check required parameters for experiment
        required_params = ["neuron_count", "region_count", "simulation_timestep"]
        
        for param in required_params:
            if param not in config:
                print(f"Error: Missing required parameter '{param}'")
                return False
        
        # Validate parameter ranges
        if config["neuron_count"] <= 0:
            print("Error: neuron_count must be positive")
            return False
        
        if config["simulation_timestep"] <= 0 or config["simulation_timestep"] > 0.1:
            print("Error: simulation_timestep must be between 0 and 0.1")
            return False
        
        # Additional experiment-specific validation
        if experiment_name == "plasticity_study":
            if "plasticity_learning_rate" not in config:
                print("Error: plasticity_learning_rate required for plasticity_study")
                return False
        
        elif experiment_name == "memory_test":
            if "working_memory" not in config and "episodic_memory" not in config:
                print("Error: memory systems required for memory_test")
                return False
        
        elif experiment_name == "neuromodulation_experiment":
            if "dopamine_baseline" not in config:
                print("Error: dopamine_baseline required for neuromodulation_experiment")
                return False
        
        print(f"Experiment '{experiment_name}' setup is valid")
        return True

class NLMJSONConfigCLI:
    """Command-line interface for NLM JSON configuration generator"""
    
    def __init__(self):
        self.generator = NLMJSONConfigGenerator()
    
    def run_command(self, args):
        """Run command with given arguments"""
        if len(args) < 1:
            print("Error: No command specified")
            return False
        
        command = args[0]
        remaining_args = args[1:]
        
        try:
            if command == "generate":
                self.handle_generate(remaining_args)
            elif command == "validate":
                self.handle_validate(remaining_args)
            elif command == "list":
                self.handle_list(remaining_args)
            elif command == "convert":
                self.handle_convert(remaining_args)
            elif command == "experiment":
                self.handle_experiment(remaining_args)
            else:
                print(f"Unknown command: {command}")
                self.print_help()
                return False
                
        except Exception as e:
            print(f"Error: {e}")
            return False
        
        return True
    
    def handle_generate(self, args):
        """Handle generate command"""
        if len(args) < 1:
            print("Error: Missing filename argument")
            return False
        
        filename = args[0]
        template = None
        custom_values = {}
        
        i = 1
        while i < len(args):
            if args[i] == "--template":
                if i + 1 < len(args):
                    template = args[i + 1]
                    i += 2
                else:
                    print("Error: Missing template name")
                    return False
            elif args[i] == "--modify":
                if i + 2 < len(args):
                    custom_values[args[i + 1]] = self.parse_value(args[i + 2])
                    i += 3
                else:
                    print("Error: Invalid --modify arguments")
                    return False
            else:
                break
        
        if template:
            print(f"Generating configuration from template '{template}'...")
            success = self.generator.generate_from_preset(template, filename)
        else:
            print(f"Generating configuration...")
            success = self.generator.generate_config(filename, custom_values=custom_values)
        
        return success
    
    def handle_validate(self, args):
        """Handle validate command"""
        if len(args) < 1:
            print("Error: Missing filename argument")
            return False
        
        filename = args[0]
        print(f"Validating configuration: {filename}")
        return self.generator.validate_config(filename)
    
    def handle_list(self, args):
        """Handle list command"""
        self.generator.list_presets()
        return True
    
    def handle_convert(self, args):
        """Handle convert command"""
        if len(args) < 2:
            print("Error: Missing source and output filenames")
            return False
        
        json_file = args[0]
        legacy_file = args[1]
        print(f"Converting {json_file} to {legacy_file}")
        return self.generator.convert_to_legacy_format(json_file, legacy_file)
    
    def handle_experiment(self, args):
        """Handle experiment command"""
        if len(args) < 2:
            print("Error: Missing experiment name and filename")
            return False
        
        experiment_name = args[0]
        filename = args[1]
        
        # Parse additional parameters
        parameters = {}
        i = 2
        while i < len(args):
            if args[i] == "--param":
                if i + 2 < len(args):
                    parameters[args[i + 1]] = self.parse_value(args[i + 2])
                    i += 3
                else:
                    print("Error: Invalid experiment parameter")
                    return False
            else:
                break
        
        print(f"Generating experiment configuration: {experiment_name}")
        return self.generator.generate_experiment_config(experiment_name, parameters, filename)
    
    def parse_value(self, value_str):
        """Parse value string to appropriate type"""
        # Try boolean
        if value_str.lower() == "true":
            return True
        elif value_str.lower() == "false":
            return False
        
        # Try integer
        try:
            return int(value_str)
        except ValueError:
            pass
        
        # Try float
        try:
            return float(value_str)
        except ValueError:
            pass
        
        # Return string
        return value_str
    
    def print_help(self):
        """Print help information"""
        print("NLM JSON Configuration Generator")
        print("=" * 40)
        print("\nCommands:")
        print("  generate <filename> [--template <name>] [--modify <key> <value> ...]")
        print("  validate <filename>")
        print("  list")
        print("  convert <json_file> <legacy_file>")
        print("  experiment <name> <filename> [--param <key> <value> ...]")
        print("\nExamples:")
        print("  nlm-config-generator generate config.json --template advanced")
        print("  nlm-config-generator validate config.json")
        print("  nlm-config-generator convert config.json config.cfg")
        print("  nlm-config-generator experiment plasticity_study --param neuron_count 1000")

# Standalone execution
def main():
    """Main function for standalone execution"""
    cli = NLMJSONConfigCLI()
    
    if len(sys.argv) < 2:
        cli.print_help()
        return 1
    
    success = cli.run_command(sys.argv[1:])
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
