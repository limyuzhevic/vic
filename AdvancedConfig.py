#!/usr/bin/env python3

"""
Advanced Configuration System for NLM - Power User Tools

This script provides enhanced configuration capabilities including:
- JSON/YAML configuration with validation
- Configuration schema definition
- Parameter grouping and categories
- Configuration templates
- Configuration history
- Dynamic parameter tweaking
- Configuration profiles
"""

import json
import yaml
import argparse
import os
import sys
import time
from datetime import datetime
from typing import Dict, Any, Optional, List, Union
from pathlib import Path
from dataclasses import dataclass, asdict
from enum import Enum
import numpy as np

@dataclass
class ConfigParameter:
    """Represents a single configuration parameter"""
    name: str
    type: str  # "int", "float", "string", "bool", "vector"
    default: Any
    description: str
    category: str
    min_val: Optional[float] = None
    max_val: Optional[float] = None
    step: Optional[float] = None
    allowed_values: Optional[List[Any]] = None
    validation_func: Optional[str] = None
    dependencies: Optional[List[str]] = None
    hidden: bool = False

@dataclass
class ConfigGroup:
    """Represents a group of configuration parameters"""
    name: str
    description: str
    parameters: List[ConfigParameter]
    advanced: bool = False

@dataclass
class ConfigTemplate:
    """A pre-configured template for common use cases"""
    name: str
    description: str
    groups: List[ConfigGroup]
    tags: List[str]

class AdvancedConfigManager:
    """Advanced configuration manager with validation and schema support"""
    
    def __init__(self, config_dir: str = "./configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
        # Configuration groups
        self.groups: Dict[str, ConfigGroup] = {}
        self.templates: Dict[str, ConfigTemplate] = {}
        self.config_history: List[Dict[str, Any]] = []
        
        # Initialize default configuration
        self._initialize_default_config()
    
    def _initialize_default_config(self):
        """Initialize the default configuration structure"""
        # Create advanced configuration groups
        self._create_advanced_groups()
        self._create_preset_templates()
    
    def _create_advanced_groups(self):
        """Create advanced configuration groups"""
        
        # Neural Network Configuration
        self.groups["neural_network"] = ConfigGroup(
            name="neural_network",
            description="Core neural network parameters",
            advanced=True,
            parameters=[
                ConfigParameter("neuron_count", "int", 1000, "Number of neurons in the network", "Core", 100, 10000, 100),
                ConfigParameter("region_count", "int", 1, "Number of brain regions", "Core", 1, 10, 1),
                ConfigParameter("connection_probability", "float", 0.1, "Probability of synaptic connections", "Core", 0.01, 0.5, 0.01),
                ConfigParameter("mean_synaptic_weight", "float", 0.2, "Mean synaptic weight magnitude", "Core", -1.0, 1.0, 0.1),
                ConfigParameter("weight_variance", "float", 0.1, "Variance of synaptic weights", "Core", 0.01, 0.5, 0.01),
            ]
        )
        
        # Memory System Configuration
        self.groups["memory_system"] = ConfigGroup(
            name="memory_system",
            description="Memory system parameters",
            advanced=True,
            parameters=[
                ConfigParameter("working_memory_capacity", "int", 100, "Working memory capacity in neurons", "Memory", 10, 1000, 10),
                ConfigParameter("episodic_memory_max_episodes", "int", 1000, "Maximum number of episodic memories", "Memory", 100, 10000, 100),
                ConfigParameter("decay_rate", "float", 0.01, "Memory decay rate per timestep", "Memory", 0.001, 0.1, 0.001),
                ConfigParameter("replay_interval", "int", 100, "Memory replay interval (timesteps)", "Memory", 10, 1000, 10),
                ConfigParameter("consolidation_interval", "int", 1000, "Memory consolidation interval", "Memory", 100, 10000, 100),
            ]
        )
        
        # Neuromodulation Configuration
        self.groups["neuromodulation"] = ConfigGroup(
            name="neuromodulation",
            description="Neuromodulation system parameters",
            advanced=True,
            parameters=[
                ConfigParameter("dopamine_baseline", "float", 0.0, "Dopamine baseline level", "Neuromodulation", 0.0, 1.0, 0.01),
                ConfigParameter("dopamine_peak", "float", 1.0, "Dopamine peak response level", "Neuromodulation", 0.0, 1.0, 0.01),
                ConfigParameter("dopamine_decay_rate", "float", 0.1, "Dopamine decay rate", "Neuromodulation", 0.01, 0.5, 0.01),
                ConfigParameter("curiosity_novelty_weight", "float", 0.5, "Weight of novelty in curiosity computation", "Neuromodulation", 0.0, 1.0, 0.1),
                ConfigParameter("curiosity_prediction_error_weight", "float", 0.5, "Weight of prediction error in curiosity", "Neuromodulation", 0.0, 1.0, 0.1),
                ConfigParameter("novelty_threshold", "float", 0.3, "Novelty detection threshold", "Neuromodulation", 0.0, 1.0, 0.1),
            ]
        )
        
        # Plasticity Configuration
        self.groups["plasticity"] = ConfigGroup(
            name="plasticity",
            description="Synaptic plasticity parameters",
            advanced=True,
            parameters=[
                ConfigParameter("stdp_ltp_weight", "float", 0.01, "STDP Long-Term Potentiation weight", "Plasticity", 0.0, 0.1, 0.001),
                ConfigParameter("stdp_ltd_weight", "float", 0.012, "STDP Long-Term Depression weight", "Plasticity", 0.0, 0.1, 0.001),
                ConfigParameter("stdp_tau", "float", 20.0, "STDP time constant (ms)", "Plasticity", 1.0, 100.0, 1.0),
                ConfigParameter("hebbian_learning_rate", "float", 0.1, "Hebbian learning rate", "Plasticity", 0.0, 1.0, 0.1),
                ConfigParameter("synaptogenesis_rate", "float", 0.0001, "Rate of new synapse formation", "Plasticity", 0.00001, 0.001, 0.00001),
                ConfigParameter("pruning_rate", "float", 0.00001, "Rate of synapse pruning", "Plasticity", 0.000001, 0.0001, 0.000001),
            ]
        )
        
        # Development Configuration
        self.groups["development"] = ConfigGroup(
            name="development",
            description="Developmental system parameters",
            advanced=True,
            parameters=[
                ConfigParameter("initial_plasticity", "float", 1.0, "Initial plasticity factor", "Development", 0.1, 2.0, 0.1),
                ConfigParameter("critical_period_plasticity", "float", 0.8, "Plasticity during critical period", "Development", 0.1, 1.0, 0.1),
                ConfigParameter("maturation_plasticity", "float", 0.5, "Plasticity during maturation", "Development", 0.1, 1.0, 0.1),
                ConfigParameter("adult_plasticity", "float", 0.2, "Plasticity in adulthood", "Development", 0.0, 0.5, 0.1),
                ConfigParameter("development_step_interval", "int", 1000, "Development update interval", "Development", 100, 10000, 100),
            ]
        )
        
        # Performance Configuration
        self.groups["performance"] = ConfigGroup(
            name="performance",
            description="Performance and optimization parameters",
            advanced=True,
            parameters=[
                ConfigParameter("simulation_timestep", "float", 0.001, "Simulation timestep (seconds)", "Performance", 0.0001, 0.1, 0.0001),
                ConfigParameter("checkpoint_interval", "int", 1000, "Checkpoint saving interval", "Performance", 100, 10000, 100),
                ConfigParameter("checkpoint_compression", "int", 5, "Checkpoint compression level", "Performance", 0, 9, 1),
                ConfigParameter("enable_vectorization", "bool", True, "Enable SIMD vectorization", "Performance", None, None, None),
                ConfigParameter("enable_parallel_processing", "bool", False, "Enable parallel processing", "Performance", None, None, None),
            ]
        )
    
    def _create_preset_templates(self):
        """Create preset configuration templates"""
        
        # High Performance Template
        high_perf_group = ConfigGroup(
            name="neural_network",
            description="Core neural network parameters",
            parameters=[
                ConfigParameter("neuron_count", "int", 500, "Number of neurons", "Core", 100, 10000, 100),
                ConfigParameter("connection_probability", "float", 0.05, "Connection probability", "Core", 0.01, 0.5, 0.01),
                ConfigParameter("enable_vectorization", "bool", True, "SIMD vectorization", "Performance", None, None, None),
                ConfigParameter("enable_parallel_processing", "bool", True, "Parallel processing", "Performance", None, None, None),
            ]
        )
        
        self.templates["high_performance"] = ConfigTemplate(
            name="high_performance",
            description="Optimized for maximum performance",
            groups=[high_perf_group],
            tags=["fast", "optimized", "simulation"]
        )
        
        # Stable Learning Template
        stable_group = ConfigGroup(
            name="plasticity",
            description="Synaptic plasticity parameters",
            parameters=[
                ConfigParameter("stdp_ltp_weight", "float", 0.005, "STDP LTP weight", "Plasticity", 0.0, 0.1, 0.001),
                ConfigParameter("stdp_ltd_weight", "float", 0.006, "STDP LTD weight", "Plasticity", 0.0, 0.1, 0.001),
                ConfigParameter("stdp_tau", "float", 50.0, "STDP time constant", "Plasticity", 1.0, 100.0, 1.0),
            ]
        )
        
        memory_group = ConfigGroup(
            name="memory_system",
            description="Memory system parameters",
            parameters=[
                ConfigParameter("working_memory_capacity", "int", 200, "Working memory capacity", "Memory", 10, 1000, 10),
                ConfigParameter("decay_rate", "float", 0.005, "Memory decay rate", "Memory", 0.001, 0.1, 0.001),
            ]
        )
        
        self.templates["stable_learning"] = ConfigTemplate(
            name="stable_learning",
            description="Optimized for stable learning and memory",
            groups=[stable_group, memory_group],
            tags=["stable", "learning", "memory", "consolidation"]
        )
        
        # Rapid Exploration Template
        curiosity_group = ConfigGroup(
            name="neuromodulation",
            description="Neuromodulation system parameters",
            parameters=[
                ConfigParameter("curiosity_novelty_weight", "float", 0.7, "Novelty weight in curiosity", "Neuromodulation", 0.0, 1.0, 0.1),
                ConfigParameter("curiosity_prediction_error_weight", "float", 0.3, "Prediction error weight", "Neuromodulation", 0.0, 1.0, 0.1),
                ConfigParameter("novelty_threshold", "float", 0.5, "Novelty detection threshold", "Neuromodulation", 0.0, 1.0, 0.1),
            ]
        )
        
        development_group = ConfigGroup(
            name="development",
            description="Developmental system parameters",
            parameters=[
                ConfigParameter("initial_plasticity", "float", 1.5, "Initial plasticity factor", "Development", 0.1, 2.0, 0.1),
                ConfigParameter("adult_plasticity", "float", 0.3, "Adult plasticity level", "Development", 0.0, 0.5, 0.1),
            ]
        )
        
        self.templates["rapid_exploration"] = ConfigTemplate(
            name="rapid_exploration",
            description="Optimized for rapid exploration and adaptation",
            groups=[curiosity_group, development_group],
            tags=["exploration", "adaptation", "curiosity", "plasticity"]
        )
    
    def create_config_file(self, filename: str, parameters: Dict[str, Any], 
                          format: str = "json", template: Optional[str] = None) -> bool:
        """Create a configuration file with validation"""
        try:
            # Validate parameters
            if not self._validate_config(parameters):
                return False
            
            # Create config entry
            config_entry = {
                "timestamp": datetime.now().isoformat(),
                "parameters": parameters,
                "template": template,
                "version": "1.0"
            }
            
            # Save in specified format
            config_path = self.config_dir / filename
            
            if format.lower() == "json":
                with open(config_path, 'w') as f:
                    json.dump(config_entry, f, indent=2)
            elif format.lower() in ["yaml", "yml"]:
                with open(config_path, 'w') as f:
                    yaml.dump(config_entry, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            # Add to history
            self.config_history.append(config_entry)
            
            print(f"Configuration saved to {config_path}")
            return True
            
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def _validate_config(self, parameters: Dict[str, Any]) -> bool:
        """Validate configuration parameters"""
        # Validate each parameter against its schema
        for group in self.groups.values():
            for param in group.parameters:
                if param.name in parameters:
                    if not self._validate_parameter(param, parameters[param.name]):
                        print(f"Invalid value for parameter '{param.name}': {parameters[param.name]}")
                        return False
        return True
    
    def _validate_parameter(self, param: ConfigParameter, value: Any) -> bool:
        """Validate a single parameter"""
        # Type validation
        if param.type == "int" and not isinstance(value, int):
            return False
        elif param.type == "float" and not isinstance(value, (int, float)):
            return False
        elif param.type == "bool" and not isinstance(value, bool):
            return False
        elif param.type == "string" and not isinstance(value, str):
            return False
        elif param.type == "vector" and not isinstance(value, list):
            return False
        
        # Range validation
        if isinstance(value, (int, float)):
            if param.min_val is not None and value < param.min_val:
                return False
            if param.max_val is not None and value > param.max_val:
                return False
        
        # Allowed values validation
        if param.allowed_values is not None and value not in param.allowed_values:
            return False
        
        return True
    
    def load_config_file(self, filename: str) -> Optional[Dict[str, Any]]:
        """Load configuration from file"""
        config_path = self.config_dir / filename
        
        if not config_path.exists():
            print(f"Configuration file not found: {filename}")
            return None
        
        try:
            with open(config_path, 'r') as f:
                if filename.endswith('.json'):
                    config = json.load(f)
                elif filename.endswith(('.yaml', '.yml')):
                    config = yaml.safe_load(f)
                else:
                    print(f"Unsupported format: {filename}")
                    return None
            
            # Validate loaded configuration
            if "parameters" in config and not self._validate_config(config["parameters"]):
                print("Invalid configuration in file")
                return None
            
            return config
            
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return None
    
    def get_config_summary(self, config: Dict[str, Any]) -> str:
        """Get a human-readable summary of configuration"""
        if not config or "parameters" not in config:
            return "Invalid configuration"
        
        parameters = config["parameters"]
        summary = ["Configuration Summary:", "===================\n"]
        
        # Group parameters by category
        grouped_params = {}
        for group in self.groups.values():
            grouped_params[group.name] = []
        
        for param_name, value in parameters.items():
            # Find which group this parameter belongs to
            for group in self.groups.values():
                for param in group.parameters:
                    if param.name == param_name:
                        grouped_params[group.name].append((param_name, value))
                        break
        
        # Add groups to summary
        for group_name, params in grouped_params.items():
            if params:
                summary.append(f"\n{group_name.upper()} PARAMETERS:")
                summary.append("-" * (len(group_name) + 13))
                for name, value in params:
                    summary.append(f"  {name}: {value}")
        
        # Add metadata
        if "template" in config:
            summary.append(f"\nTemplate: {config['template']}")
        if "timestamp" in config:
            summary.append(f"Created: {config['timestamp']}")
        
        return "\n".join(summary)
    
    def list_available_templates(self) -> str:
        """List all available configuration templates"""
        result = ["Available Templates:", "====================\n"]
        
        for name, template in self.templates.items():
            result.append(f"\nTemplate: {name}")
            result.append(f"Description: {template.description}")
            result.append(f"Tags: {', '.join(template.tags)}")
        
        return "\n".join(result)
    
    def export_config(self, config: Dict[str, Any], export_path: str, 
                     include_history: bool = False, format: str = "json") -> bool:
        """Export configuration with additional metadata"""
        try:
            export_data = {
                "config": config,
                "exported_at": datetime.now().isoformat(),
                "nlm_version": "Phase 6 - Final Integration",
                "export_info": {
                    "tool": "NLM Advanced Configuration System",
                    "version": "1.0",
                    "author": "NLM Research Team"
                }
            }
            
            if include_history and self.config_history:
                export_data["history"] = self.config_history[-10:]  # Last 10 entries
            
            export_path_obj = Path(export_path)
            
            if format.lower() == "json":
                with open(export_path_obj, 'w') as f:
                    json.dump(export_data, f, indent=2)
            elif format.lower() in ["yaml", "yml"]:
                with open(export_path_obj, 'w') as f:
                    yaml.dump(export_data, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Configuration exported to {export_path}")
            return True
            
        except Exception as e:
            print(f"Error exporting configuration: {e}")
            return False

def main():
    """Main function for command-line interface"""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Configuration System - Power User Tools"
    )
    
    parser.add_argument(
        "action",
        choices=["create", "load", "list-templates", "validate", "export", "profile"],
        help="Action to perform"
    )
    
    parser.add_argument(
        "--config-dir",
        default="./configs",
        help="Configuration directory (default: ./configs)"
    )
    
    parser.add_argument(
        "--output",
        help="Output filename for create/export actions"
    )
    
    parser.add_argument(
        "--format",
        choices=["json", "yaml"],
        default="json",
        help="Configuration file format (default: json)"
    )
    
    parser.add_argument(
        "--template",
        help="Template name to use for configuration creation"
    )
    
    parser.add_argument(
        "--profile",
        help="Configuration profile name for profile mode"
    )
    
    parser.add_argument(
        "--validate-only",
        action="store_true",
        help="Only validate existing configuration files"
    )
    
    parser.add_argument(
        "--export-history",
        action="store_true",
        help="Include configuration history in export"
    )
    
    parser.add_argument(
        "--file",
        help="Configuration file to load or validate"
    )
    
    args = parser.parse_args()
    
    # Create advanced configuration manager
    config_manager = AdvancedConfigManager(args.config_dir)
    
    # Define some default configuration parameters for testing
    default_config = {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "mean_synaptic_weight": 0.2,
        "weight_variance": 0.1,
        "working_memory_capacity": 100,
        "episodic_memory_max_episodes": 1000,
        "decay_rate": 0.01,
        "replay_interval": 100,
        "consolidation_interval": 1000,
        "dopamine_baseline": 0.0,
        "dopamine_peak": 1.0,
        "dopamine_decay_rate": 0.1,
        "curiosity_novelty_weight": 0.5,
        "curiosity_prediction_error_weight": 0.5,
        "novelty_threshold": 0.3,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "hebbian_learning_rate": 0.1,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "initial_plasticity": 1.0,
        "critical_period_plasticity": 0.8,
        "maturation_plasticity": 0.5,
        "adult_plasticity": 0.2,
        "development_step_interval": 1000,
        "simulation_timestep": 0.001,
        "checkpoint_interval": 1000,
        "checkpoint_compression": 5,
        "enable_vectorization": True,
        "enable_parallel_processing": False,
    }
    
    try:
        if args.action == "create":
            if not args.output:
                print("Error: --output filename required for create action")
                return 1
            
            config_manager.create_config_file(args.output, default_config, args.format, args.template)
        
        elif args.action == "load":
            if not args.file:
                print("Error: --file filename required for load action")
                return 1
            
            config = config_manager.load_config_file(args.file)
            if config:
                print(config_manager.get_config_summary(config))
            else:
                print("Failed to load configuration")
                return 1
        
        elif args.action == "list-templates":
            print(config_manager.list_available_templates())
        
        elif args.action == "validate":
            if not args.file:
                print("Error: --file filename required for validate action")
                return 1
            
            config = config_manager.load_config_file(args.file)
            if config:
                if args.validate_only:
                    print("Configuration validation: PASSED")
                else:
                    print(config_manager.get_config_summary(config))
            else:
                print("Invalid configuration")
                return 1
        
        elif args.action == "export":
            if not args.file:
                print("Error: --file filename required for export action")
                return 1
            
            if not args.output:
                print("Error: --output filename required for export action")
                return 1
            
            config = config_manager.load_config_file(args.file)
            if config:
                config_manager.export_config(config, args.output, args.export_history, args.format)
            else:
                print("Failed to load configuration for export")
                return 1
        
        elif args.action == "profile":
            print("\n" + "="*60)
            print("NLM CONFIGURATION PROFILES")
            print("="*60)
            
            profiles = {
                "default": default_config,
                "high_performance": {
                    **default_config,
                    "enable_vectorization": True,
                    "enable_parallel_processing": True,
                    "neuron_count": 500,
                    "connection_probability": 0.05
                },
                "stable_learning": {
                    **default_config,
                    "stdp_ltp_weight": 0.005,
                    "stdp_ltd_weight": 0.006,
                    "stdp_tau": 50.0,
                    "working_memory_capacity": 200,
                    "decay_rate": 0.005
                },
                "rapid_exploration": {
                    **default_config,
                    "curiosity_novelty_weight": 0.7,
                    "curiosity_prediction_error_weight": 0.3,
                    "novelty_threshold": 0.5,
                    "initial_plasticity": 1.5,
                    "adult_plasticity": 0.3
                }
            }
            
            if args.profile:
                if args.profile in profiles:
                    print(f"\nProfile: {args.profile.upper()}")
                    print("-" * (len(args.profile) + 8))
                    print(config_manager.get_config_summary({"parameters": profiles[args.profile], "template": args.profile}))
                else:
                    print(f"Error: Unknown profile '{args.profile}'")
                    print(f"Available profiles: {', '.join(profiles.keys())}")
                    return 1
            else:
                print("\nAvailable profiles:")
                for name, config in profiles.items():
                    print(f"  {name}: {config.get('description', 'No description')}")
                
                print(f"\nUse --profile <name> to view a specific profile")
        
        return 0
        
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())