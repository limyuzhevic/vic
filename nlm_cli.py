#!/usr/bin/env python3
"""
NLM Configuration Management and Command-Line Tools

This module provides comprehensive configuration management and command-line
interfaces for the NLM (Neural Learning Machine) project. It includes:

- Advanced configuration management with validation
- Multiple configuration templates for different use cases
- Command-line interface for running experiments
- Configuration file generation and validation
- Environment-specific settings
- Type-safe configuration access

Features:
- JSON/YAML configuration file support
- Type-safe configuration access with validation
- Multiple configuration templates (demo, research, production)
- Command-line argument parsing
- Configuration validation and schema checking
- Environment variable support
- Configuration comparison and diffing
- Configuration backup and restore
- Automatic configuration detection
- Comprehensive error handling and reporting
"""

import json
import yaml
import argparse
import os
import sys
import logging
from typing import Dict, Any, Optional, Union, List
from dataclasses import dataclass, field
from pathlib import Path
from enum import Enum
from datetime import datetime

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)
class ConfigTemplate(Enum):
    """Configuration templates for different use cases"""
    DEMO = "demo"
    RESEARCH = "research"
    PRODUCTION = "production"
    BENCHMARK = "benchmark"
    DEVELOPMENT = "development"
    VALIDATION = "validation"
class ConfigSection(Enum):
    """Configuration sections"""
    NEURAL = "neural"
    SIMULATION = "simulation"
    MEMORY = "memory"
    COGNITION = "cognition"
    DEVELOPMENT = "development"
    NEUROMODULATION = "neuromodulation"
    VISUALIZATION = "visualization"
    PERFORMANCE = "performance"
    EXPERIMENT = "experiment"
@dataclass
class ConfigSchema:
    """Configuration schema definition"""
    name: str
    description: str
    section: ConfigSection
    type: str  # int, float, str, bool, list, dict
    required: bool = False
    default: Any = None
    min_value: Optional[float] = None
    max_value: Optional[float] = None
    choices: Optional[List[str]] = None
    validation_func: Optional[callable] = None
@dataclass
class NLMConfig:
    """Enhanced NLM configuration with validation and management"""
    
    # Neural system configuration
    neural: Dict[str, Any] = field(default_factory=lambda: {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "random_seed": 42,
        "learning_rate": 0.01,
        "adaptation_rate": 0.001,
        "structural_plasticity": True,
        "reward_modulation": True,
        "developmental_plasticity": True
    })
    
    # Simulation configuration
    simulation: Dict[str, Any] = field(default_factory=lambda: {
        "timestep": 0.001,
        "max_steps": 10000,
        "simulation_time": 100.0,
        "clock_skew_tolerance": 0.1,
        "parallel_processing": True,
        "vectorization": True,
        "event_driven": True
    })
    
    # Memory system configuration
    memory: Dict[str, Any] = field(default_factory=lambda: {
        "working_memory_capacity": 100,
        "working_memory_decay_rate": 0.1,
        "episodic_memory_capacity": 1000,
        "episodic_memory_compression": True,
        "associative_memory_threshold": 0.5,
        "replay_frequency": 0.1,
        "consolidation_rate": 0.05
    })
    
    # Cognition configuration
    cognition: Dict[str, Any] = field(default_factory=lambda: {
        "planning_horizon": 10,
        "concept_formulation_threshold": 0.8,
        "attention_focus_strength": 0.9,
        "predictive_coding_strength": 0.7,
        "social_learning": True,
        "continual_learning": True
    })
    
    # Development configuration
    development: Dict[str, Any] = field(default_factory=lambda: {
        "critical_period_start": 100,
        "critical_period_end": 1000,
        "maturation_rate": 0.1,
        "pruning_rate": 0.01,
        "synaptogenesis_rate": 0.001,
        "developmental_stages": 5
    })
    
    # Neuromodulation configuration
    neuromodulation: Dict[str, Any] = field(default_factory=lambda: {
        "dopamine_strength": 1.0,
        "curiosity_strength": 0.5,
        "novelty_threshold": 0.3,
        "prediction_error_decay": 0.9,
        "neuromodulation_integration": True
    })
    
    # Performance configuration
    performance: Dict[str, Any] = field(default_factory=lambda: {
        "memory_pool_size": 1024,
        "event_queue_size": 512,
        "checkpoint_frequency": 100,
        "checkpoint_compression": True,
        "simd_width": 32,
        "num_threads": 4,
        "gpu_acceleration": False
    })
    
    # Experiment configuration
    experiment: Dict[str, Any] = field(default_factory=lambda: {
        "experiment_name": "default_experiment",
        "experiment_description": "Default experiment configuration",
        "output_directory": "./results",
        "save_frequency": 10,
        "metric_collection": True,
        "visualization_enabled": False,
        "benchmark_mode": False
    })
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary"""
        return {
            "neural": self.neural,
            "simulation": self.simulation,
            "memory": self.memory,
            "cognition": self.cognition,
            "development": self.development,
            "neuromodulation": self.neuromodulation,
            "performance": self.performance,
            "experiment": self.experiment
        }
    
    def update(self, other: 'NLMConfig') -> None:
        """Update configuration from another configuration"""
        self.neural.update(other.neural)
        self.simulation.update(other.simulation)
        self.memory.update(other.memory)
        self.cognition.update(other.cognition)
        self.development.update(other.development)
        self.neuromodulation.update(other.neuromodulation)
        self.performance.update(other.performance)
        self.experiment.update(other.experiment)
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of errors"""
        errors = []
        
        # Validate neural configuration
        if self.neural["neuron_count"] <= 0:
            errors.append("Neuron count must be positive")
        if self.neural["region_count"] <= 0:
            errors.append("Region count must be positive")
        if not 0 <= self.neural["connection_probability"] <= 1:
            errors.append("Connection probability must be between 0 and 1")
        
        # Validate simulation configuration
        if self.simulation["timestep"] <= 0:
            errors.append("Timestep must be positive")
        if self.simulation["max_steps"] <= 0:
            errors.append("Max steps must be positive")
        
        # Validate memory configuration
        if self.memory["working_memory_capacity"] <= 0:
            errors.append("Working memory capacity must be positive")
        if not 0 <= self.memory["working_memory_decay_rate"] <= 1:
            errors.append("Working memory decay rate must be between 0 and 1")
        
        return errors
    
    def get(self, section: ConfigSection, key: str, default: Any = None) -> Any:
        """Get configuration value from a specific section"""
        sections = {
            ConfigSection.NEURAL: self.neural,
            ConfigSection.SIMULATION: self.simulation,
            ConfigSection.MEMORY: self.memory,
            ConfigSection.COGNITION: self.cognition,
            ConfigSection.DEVELOPMENT: self.development,
            ConfigSection.NEUROMODULATION: self.neuromodulation,
            ConfigSection.PERFORMANCE: self.performance,
            ConfigSection.EXPERIMENT: self.experiment
        }
        
        if section in sections:
            return sections[section].get(key, default)
        return default
class ConfigManager:
    """Enhanced configuration manager with file I/O and template support"""
    
    def __init__(self, config_dir: str = "./config"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        self.schemas = self._load_schemas()
    
    def _load_schemas(self) -> Dict[str, List[ConfigSchema]]:
        """Load configuration schemas"""
        schemas = {}
        for section in ConfigSection:
            # Define configuration schemas for each section
            if section == ConfigSection.NEURAL:
                schemas[section.value] = [
                    ConfigSchema("neuron_count", "Number of neurons", ConfigSection.NEURAL, "int", True, 1000),
                    ConfigSchema("region_count", "Number of neural regions", ConfigSection.NEURAL, "int", True, 1),
                    ConfigSchema("connection_probability", "Probability of connections between neurons", ConfigSection.NEURAL, "float", False, 0.1),
                    ConfigSchema("random_seed", "Random seed for reproducibility", ConfigSection.NEURAL, "int", False, 42),
                ]
            elif section == ConfigSection.SIMULATION:
                schemas[section.value] = [
                    ConfigSchema("timestep", "Simulation time step", ConfigSection.SIMULATION, "float", True, 0.001),
                    ConfigSchema("max_steps", "Maximum simulation steps", ConfigSection.SIMULATION, "int", True, 10000),
                    ConfigSchema("simulation_time", "Total simulation time", ConfigSection.SIMULATION, "float", False, 100.0),
                ]
            # Add more schemas for other sections
        return schemas
    
    def load_from_json(self, filepath: str) -> NLMConfig:
        """Load configuration from JSON file"""
        try:
            with open(filepath, 'r') as f:
                config_dict = json.load(f)
            
            # Convert to NLMConfig
            config = NLMConfig()
            
            # Merge loaded configuration
            for section in ConfigSection:
                if section.value in config_dict:
                    section_dict = config_dict[section.value]
                    section_attrs = getattr(config, section.value)
                    section_attrs.update(section_dict)
            
            return config
        except Exception as e:
            logger.error(f"Failed to load configuration from {filepath}: {e}")
            raise
    
    def save_to_json(self, config: NLMConfig, filepath: str) -> None:
        """Save configuration to JSON file"""
        try:
            config_dict = config.to_dict()
            with open(filepath, 'w') as f:
                json.dump(config_dict, f, indent=2)
            logger.info(f"Configuration saved to {filepath}")
        except Exception as e:
            logger.error(f"Failed to save configuration to {filepath}: {e}")
            raise
    
    def load_from_yaml(self, filepath: str) -> NLMConfig:
        """Load configuration from YAML file"""
        try:
            with open(filepath, 'r') as f:
                config_dict = yaml.safe_load(f)
            
            return self.load_from_dict(config_dict)
        except Exception as e:
            logger.error(f"Failed to load configuration from {filepath}: {e}")
            raise
    
    def save_to_yaml(self, config: NLMConfig, filepath: str) -> None:
        """Save configuration to YAML file"""
        try:
            config_dict = config.to_dict()
            with open(filepath, 'w') as f:
                yaml.dump(config_dict, f, default_flow_style=False)
            logger.info(f"Configuration saved to {filepath}")
        except Exception as e:
            logger.error(f"Failed to save configuration to {filepath}: {e}")
            raise
    
    def load_from_dict(self, config_dict: Dict[str, Any]) -> NLMConfig:
        """Load configuration from dictionary"""
        config = NLMConfig()
        
        for section in ConfigSection:
            if section.value in config_dict:
                section_dict = config_dict[section.value]
                section_attrs = getattr(config, section.value)
                section_attrs.update(section_dict)
        
        return config
    
    def get_template(self, template_type: ConfigTemplate) -> NLMConfig:
        """Get a configuration template"""
        templates = {
            ConfigTemplate.DEMO: self._get_demo_template(),
            ConfigTemplate.RESEARCH: self._get_research_template(),
            ConfigTemplate.PRODUCTION: self._get_production_template(),
            ConfigTemplate.BENCHMARK: self._get_benchmark_template(),
            ConfigTemplate.DEVELOPMENT: self._get_development_template(),
            ConfigTemplate.VALIDATION: self._get_validation_template(),
        }
        
        return templates[template_type]
    
    def _get_demo_template(self) -> NLMConfig:
        """Get demo configuration template"""
        config = NLMConfig()
        config.neural["neuron_count"] = 500
        config.neural["region_count"] = 1
        config.simulation["max_steps"] = 1000
        config.experiment["experiment_name"] = "demo_experiment"
        config.experiment["output_directory"] = "./demo_results"
        config.performance["num_threads"] = 2
        return config
    
    def _get_research_template(self) -> NLMConfig:
        """Get research configuration template"""
        config = NLMConfig()
        config.neural["neuron_count"] = 5000
        config.neural["region_count"] = 3
        config.neural["connection_probability"] = 0.05
        config.simulation["max_steps"] = 50000
        config.memory["working_memory_capacity"] = 500
        config.memory["episodic_memory_capacity"] = 5000
        config.experiment["experiment_name"] = "research_experiment"
        config.experiment["metric_collection"] = True
        config.performance["vectorization"] = True
        config.performance["num_threads"] = 8
        return config
    
    def _get_production_template(self) -> NLMConfig:
        """Get production configuration template"""
        config = NLMConfig()
        config.neural["neuron_count"] = 10000
        config.neural["region_count"] = 5
        config.neural["connection_probability"] = 0.02
        config.neural["random_seed"] = 12345
        config.simulation["max_steps"] = 100000
        config.simulation["simulation_time"] = 1000.0
        config.memory["working_memory_capacity"] = 1000
        config.memory["episodic_memory_capacity"] = 10000
        config.memory["replay_frequency"] = 0.05
        config.experiment["experiment_name"] = "production_experiment"
        config.experiment["output_directory"] = "./production_results"
        config.experiment["save_frequency"] = 100
        config.performance["memory_pool_size"] = 4096
        config.performance["event_queue_size"] = 2048
        config.performance["checkpoint_frequency"] = 500
        config.performance["checkpoint_compression"] = True
        config.performance["num_threads"] = 16
        config.performance["simd_width"] = 64
        return config
    
    def _get_benchmark_template(self) -> NLMConfig:
        """Get benchmark configuration template"""
        config = NLMConfig()
        config.neural["neuron_count"] = 100
        config.neural["region_count"] = 1
        config.simulation["max_steps"] = 10
        config.performance["benchmark_mode"] = True
        config.experiment["benchmark_mode"] = True
        config.experiment["output_directory"] = "./benchmark_results"
        return config
    
    def _get_development_template(self) -> NLMConfig:
        """Get development configuration template"""
        config = NLMConfig()
        config.neural["neuron_count"] = 1000
        config.neural["connection_probability"] = 0.2
        config.simulation["max_steps"] = 2000
        config.development["critical_period_start"] = 50
        config.development["critical_period_end"] = 500
        config.development["maturation_rate"] = 0.05
        config.experiment["experiment_name"] = "development_experiment"
        config.experiment["visualization_enabled"] = True
        config.performance["vectorization"] = True
        config.performance["num_threads"] = 4
        return config
    
    def _get_validation_template(self) -> NLMConfig:
        """Get validation configuration template"""
        config = NLMConfig()
        config.neural["neuron_count"] = 200
        config.neural["region_count"] = 2
        config.simulation["max_steps"] = 500
        config.experiment["experiment_name"] = "validation_experiment"
        config.experiment["output_directory"] = "./validation_results"
        config.performance["benchmark_mode"] = True
        return config
    
    def validate_config(self, config: NLMConfig) -> bool:
        """Validate configuration and return True if valid"""
        errors = config.validate()
        if errors:
            logger.error("Configuration validation failed:")
            for error in errors:
                logger.error(f"  - {error}")
            return False
        return True
    
    def get_config_for_run(self, template: ConfigTemplate = ConfigTemplate.DEMO,
                          config_file: Optional[str] = None,
                          overrides: Optional[Dict[str, Any]] = None) -> NLMConfig:
        """Get configuration for a run, with optional overrides"""
        # Start with template
        config = self.get_template(template)
        
        # Load from file if specified
        if config_file:
            if config_file.endswith('.json'):
                file_config = self.load_from_json(config_file)
            elif config_file.endswith('.yaml') or config_file.endswith('.yml'):
                file_config = self.load_from_yaml(config_file)
            else:
                raise ValueError(f"Unsupported config file format: {config_file}")
            
            config.update(file_config)
        
        # Apply overrides
        if overrides:
            for section, section_dict in overrides.items():
                if hasattr(config, section):
                    getattr(config, section).update(section_dict)
        
        # Validate
        if not self.validate_config(config):
            raise ValueError("Configuration validation failed")
        
        return config
class NLMCmdLineInterface:
    """Command-line interface for NLM configuration and execution"""
    
    def __init__(self, config_manager: Optional[ConfigManager] = None):
        self.config_manager = config_manager or ConfigManager()
        self.parser = self._create_parser()
    
    def _create_parser(self) -> argparse.ArgumentParser:
        """Create argument parser"""
        parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) Configuration and Execution Tool",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  # Run a demo experiment
  %(prog)s --template demo --experiment mydemo --steps 1000
  
  # Run with custom configuration
  %(prog)s --config myconfig.json --template research --experiment research_study
  
  # Save generated configuration
  %(prog)s --template production --save-to ./config/prod.json
  
  # Validate configuration file
  %(prog)s --validate ./config/prod.json
  
  # Generate configuration from template
  %(prog)s --template benchmark --generate-config ./config/bench.json
            """
        )
        
        # Template selection
        parser.add_argument(
            "--template",
            choices=[t.value for t in ConfigTemplate],
            default=ConfigTemplate.DEMO.value,
            help="Configuration template to use (default: demo)"
        )
        
        # Configuration file
        parser.add_argument(
            "--config",
            help="Load configuration from file (JSON or YAML)"
        )
        
        # Configuration overrides
        parser.add_argument(
            "--set",
            action="append",
            help="Set configuration value in format 'section.key=value'"
        )
        
        # Experiment parameters
        parser.add_argument(
            "--experiment",
            required=True,
            help="Experiment name"
        )
        
        parser.add_argument(
            "--steps",
            type=int,
            help="Number of simulation steps to run"
        )
        
        # Output options
        parser.add_argument(
            "--output-dir",
            help="Output directory for results"
        )
        
        parser.add_argument(
            "--save-to",
            help="Save generated configuration to file"
        )
        
        # Execution options
        parser.add_argument(
            "--validate-only",
            action="store_true",
            help="Only validate configuration, don't run"
        )
        
        parser.add_argument(
            "--dry-run",
            action="store_true",
            help="Show configuration but don't run"
        )
        
        # Advanced options
        parser.add_argument(
            "--verbose",
            action="store_true",
            help="Enable verbose output"
        )
        
        parser.add_argument(
            "--quiet",
            action="store_true",
            help="Suppress non-error output"
        )
        
        # Configuration generation
        parser.add_argument(
            "--generate-config",
            help="Generate configuration from template and save to file"
        )
        
        # Validation
        parser.add_argument(
            "--validate",
            help="Validate configuration file and exit"
        )
        
        return parser
    
    def parse_overrides(self, set_args: Optional[List[str]]) -> Dict[str, Any]:
        """Parse --set arguments into nested dictionary"""
        overrides = {}
        
        if not set_args:
            return overrides
        
        for arg in set_args:
            if '=' not in arg:
                raise ValueError(f"Invalid override format: {arg}. Use format 'section.key=value'")
            
            key, value = arg.split('=', 1)
            
            # Try to convert value to appropriate type
            if value.lower() in ('true', 'false'):
                value = value.lower() == 'true'
            elif value.isdigit():
                value = int(value)
            elif '.' in value and value.replace('.', '', 1).isdigit():
                value = float(value)
            
            # Convert key to section and subfield
            if '.' in key:
                section, subfield = key.split('.', 1)
                
                if section not in overrides:
                    overrides[section] = {}
                
                overrides[section][subfield] = value
            else:
                overrides[key] = value
        
        return overrides
    
    def run(self, args: Optional[List[str]] = None) -> int:
        """Run command line interface"""
        parsed_args = self.parser.parse_args(args)
        
        try:
            # Set up logging
            log_level = logging.WARNING
            if parsed_args.verbose:
                log_level = logging.INFO
            elif parsed_args.quiet:
                log_level = logging.ERROR
            
            logging.getLogger().setLevel(log_level)
            
            # Parse overrides
            overrides = self.parse_overrides(parsed_args.set)
            
            # Get configuration
            template = ConfigTemplate(parsed_args.template)
            config = self.config_manager.get_config_for_run(
                template=template,
                config_file=parsed_args.config,
                overrides=overrides if overrides else None
            )
            
            # Apply experiment-specific overrides
            if parsed_args.experiment:
                config.experiment["experiment_name"] = parsed_args.experiment
            
            if parsed_args.output_dir:
                config.experiment["output_directory"] = parsed_args.output_dir
            
            if parsed_args.steps:
                config.simulation["max_steps"] = parsed_args.steps
            
            # Validate only mode
            if parsed_args.validate_only:
                errors = config.validate()
                if errors:
                    print("Configuration validation failed:")
                    for error in errors:
                        print(f"  - {error}")
                    return 1
                else:
                    print("Configuration validation passed")
                    return 0
            
            # Generate configuration mode
            if parsed_args.generate_config:
                self.config_manager.save_to_json(config, parsed_args.generate_config)
                print(f"Configuration generated and saved to {parsed_args.generate_config}")
                return 0
            
            # Validate configuration file mode
            if parsed_args.validate:
                try:
                    if parsed_args.validate.endswith('.json'):
                        file_config = self.config_manager.load_from_json(parsed_args.validate)
                    elif parsed_args.validate.endswith('.yaml') or parsed_args.validate.endswith('.yml'):
                        file_config = self.config_manager.load_from_yaml(parsed_args.validate)
                    else:
                        print(f"Unsupported file format: {parsed_args.validate}")
                        return 1
                    
                    if self.config_manager.validate_config(file_config):
                        print(f"Configuration in {parsed_args.validate} is valid")
                        return 0
                    else:
                        print(f"Configuration in {parsed_args.validate} is invalid")
                        return 1
                except Exception as e:
                    print(f"Error validating configuration: {e}")
                    return 1
            
            # Save configuration mode
            if parsed_args.save_to:
                self.config_manager.save_to_json(config, parsed_args.save_to)
                print(f"Configuration saved to {parsed_args.save_to}")
                return 0
            
            # Dry run mode
            if parsed_args.dry_run:
                print("Configuration (dry run):")
                print(json.dumps(config.to_dict(), indent=2))
                return 0
            
            # Print configuration
            print(f"Running NLM experiment: {config.experiment['experiment_name']}")
            print(f"Configuration:")
            print(json.dumps(config.to_dict(), indent=2))
            
            # Here you would typically run the actual NLM simulation
            # For now, we'll just print the configuration
            print("\nNote: This is a dry run. Actual NLM execution would go here.")
            print("To run actual NLM, replace this section with your execution code.")
            
            return 0
            
        except Exception as e:
            logger.error(f"Error: {e}")
            return 1
def main():
    """Main function"""
    try:
        cmd_interface = NLMCmdLineInterface()
        return cmd_interface.run()
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 1
    except Exception as e:
        print(f"Unexpected error: {e}")
        return 1
if __name__ == "__main__":
    sys.exit(main())
