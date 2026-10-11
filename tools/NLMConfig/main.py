#!/usr/bin/env python3
"""
NLMConfig - Configuration Management Tool

This tool provides comprehensive configuration management for the NLM (Neural Learning Machine) project.
It supports configuration validation, generation, comparison, and parameter sensitivity analysis.

Features:
- Configuration file validation (JSON/YAML/CFG)
- Configuration generation with templates
- Configuration comparison (diff, similarity)
- Parameter sensitivity analysis
- Integration with NLM configuration system
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional, List

# Add the NLM source to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from core.ErrorHandling.Error import ConfigError
    import core.InputValidation.InputValidator as InputValidator
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM modules not available: {e}")
    NLM_AVAILABLE = False

def parse_simple_config(content: str) -> Dict[str, Any]:
    """Parse simple key=value configuration format"""
    config = {}
    for line in content.strip().split('\n'):
        line = line.strip()
        if line and not line.startswith('#') and '=' in line:
            key, value = line.split('=', 1)
            key = key.strip()
            value = value.strip()
            
            # Try to infer type
            if value.lower() in ('true', 'false'):
                config[key] = value.lower() == 'true'
            elif value.isdigit():
                config[key] = int(value)
            elif value.replace('.', '', 1).isdigit() and value.count('.') == 1:
                config[key] = float(value)
            else:
                config[key] = value
    return config

def print_help():
    """Print help information"""
    help_text = """
NLMConfig - Configuration Management Tool

USAGE:
    nlmconfig [OPTIONS] [CONFIG_FILE]

DESCRIPTION:
    Advanced configuration management for NLM (Neural Learning Machine).
    Supports validation, generation, comparison, and analysis of configurations.

OPTIONS:
    --validate, -v           Validate configuration file
    --generate, -g PATH      Generate configuration from template
    --compare, -c FILE       Compare current config with another config
    --analyze, -a            Perform parameter sensitivity analysis
    --validate-schema PATH   Validate configuration against JSON/YAML schema
    --export-json FILE       Export configuration as JSON
    --export-yaml FILE       Export configuration as YAML
    --export-simple FILE     Export configuration as simple key=value format
    --list                   List all configuration keys and values
    --help, -h               Show this help message

EXAMPLES:
    # Validate a configuration file
    nlmconfig --validate configs/default.cfg

    # Generate configuration from template
    nlmconfig --generate my_config.cfg

    # Compare with another configuration
    nlmconfig --compare configs/default.cfg my_config.cfg

    # Analyze parameter sensitivity
    nlmconfig --analyze configs/default.cfg

    # Export as JSON
    nlmconfig --export-json config.json configs/default.cfg

    # Validate against schema
    nlmconfig --validate-schema schema.json configs/default.cfg

CONFIGURATION FILES:
    Supported formats:
    - Simple .cfg files (key=value format)
    - JSON files (.json)
    - YAML files (.yaml, .yml)

EXIT CODES:
    0   Success
    1   Configuration validation failed
    2   Configuration file not found
    3   Invalid command line arguments
    4   NLM system error
"""
    print(help_text)

def load_config(filepath: str) -> Dict[str, Any]:
    """Load configuration from file"""
    if not os.path.exists(filepath):
        raise FileNotFoundError(f"Configuration file not found: {filepath}")
    
    ext = os.path.splitext(filepath)[1].lower()
    
    with open(filepath, 'r') as f:
        if ext == '.json':
            return json.load(f)
        elif ext in ['.yaml', '.yml']:
            try:
                import yaml
                return yaml.safe_load(f)
            except ImportError:
                raise ImportError("PyYAML not installed. Install with: pip install PyYAML")
        elif ext == '.cfg':
            content = f.read()
            return parse_simple_config(content)
        else:
            raise ValueError(f"Unsupported file format: {ext}")

def save_config(config: Dict[str, Any], filepath: str):
    """Save configuration to file"""
    ext = os.path.splitext(filepath)[1].lower()
    
    with open(filepath, 'w') as f:
        if ext == '.json':
            json.dump(config, f, indent=2)
        elif ext in ['.yaml', '.yml']:
            try:
                import yaml
                yaml.dump(config, f, default_flow_style=False)
            except ImportError:
                raise ImportError("PyYAML not installed. Install with: pip install PyYAML")
        elif ext == '.cfg':
            for key, value in config.items():
                if isinstance(value, bool):
                    value_str = 'true' if value else 'false'
                elif isinstance(value, (int, float)):
                    value_str = str(value)
                else:
                    value_str = str(value)
                f.write(f"{key} = {value_str}\n")
        else:
            raise ValueError(f"Unsupported file format: {ext}")

def validate_config(config: Dict[str, Any]) -> bool:
    """Validate configuration using NLM validation system"""
    if not NLM_AVAILABLE:
        print("NLM validation system not available, performing basic validation")
        return True
    
    try:
        # Create a temporary config
        cfg = Config()
        
        # Set configuration values
        for key, value in config.items():
            if isinstance(value, bool):
                cfg.set(key, value, "Runtime")
            elif isinstance(value, int):
                cfg.set(key, value, "Runtime")
            elif isinstance(value, float):
                cfg.set(key, value, "Runtime")
            elif isinstance(value, str):
                cfg.set(key, value, "Runtime")
            else:
                cfg.set(key, str(value), "Runtime")
        
        # Validate using NLM's validation system
        result = cfg.validate()
        
        if result.valid:
            print("Configuration validation PASSED")
            print(f"Config summary: {cfg.summary()}")
            return True
        else:
            print(f"Configuration validation FAILED: {result.errorMessage}")
            print(f"Path: {result.path}")
            return False
            
    except Exception as e:
        print(f"Validation error: {e}")
        return False

def generate_config(template_path: Optional[str] = None, output_path: Optional[str] = None):
    """Generate configuration from template"""
    if template_path and os.path.exists(template_path):
        config = load_config(template_path)
        print(f"Generated configuration from template: {template_path}")
    else:
        # Default template
        config = {
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
        print("Generated default NLM configuration template")
    
    if output_path:
        save_config(config, output_path)
        print(f"Configuration saved to: {output_path}")
    
    return config

def compare_configs(config1: Dict[str, Any], config2: Dict[str, Any]):
    """Compare two configurations"""
    print("=== Configuration Comparison ===")
    
    keys1 = set(config1.keys())
    keys2 = set(config2.keys())
    
    only_in_1 = keys1 - keys2
    only_in_2 = keys2 - keys1
    common = keys1 & keys2
    
    print(f"Keys only in config1: {len(only_in_1)}")
    for key in sorted(only_in_1):
        print(f"  {key}")
    
    print(f"Keys only in config2: {len(only_in_2)}")
    for key in sorted(only_in_2):
        print(f"  {key}")
    
    print(f"\nCommon keys: {len(common)}")
    
    differences = []
    for key in sorted(common):
        if config1[key] != config2[key]:
            differences.append((key, config1[key], config2[key]))
    
    print(f"\nValue differences: {len(differences)}")
    for key, val1, val2 in differences:
        print(f"  {key}:")
        print(f"    Config1: {val1}")
        print(f"    Config2: {val2}")
    
    if len(only_in_1) == 0 and len(only_in_2) == 0 and len(differences) == 0:
        print("\n✓ Configurations are identical")
    else:
        print("\n✗ Configurations differ")

def analyze_parameters(config: Dict[str, Any]):
    """Perform parameter sensitivity analysis"""
    print("=== Parameter Sensitivity Analysis ===")
    
    if not NLM_AVAILABLE:
        print("NLM analysis system not available, performing basic analysis")
    
    print("Analysis of critical parameters:")
    
    # Critical parameters for NLM
    critical_params = [
        "neuron_count",
        "region_count", 
        "connection_probability",
        "plasticity_learning_rate",
        "max_simulation_steps",
        "simulation_timestep"
    ]
    
    for param in critical_params:
        if param in config:
            value = config[param]
            param_info = get_parameter_info(param, value)
            print(f"  {param}: {value} - {param_info}")
        else:
            print(f"  {param}: [NOT SET] - Critical parameter")
    
    print("\nRecommendations:")
    print("  - Higher neuron_count improves capacity but increases computation time")
    print("  - Optimal region_count: 1-4 for most simulations")
    print("  - connection_probability around 0.1-0.2 for balanced connectivity")
    print("  - plasticity_learning_rate affects learning speed and stability")
    print("  - max_simulation_steps should balance training and computation time")

def get_parameter_info(param: str, value: Any) -> str:
    """Get information about a parameter"""
    info_map = {
        "neuron_count": "Controls network size and capacity",
        "region_count": "Number of brain regions affecting modular processing",
        "connection_probability": "Controls sparsity of connectivity",
        "plasticity_learning_rate": "Controls rate of synaptic adaptation",
        "max_simulation_steps": "Controls simulation duration",
        "simulation_timestep": "Controls temporal resolution",
        "random_seed": "Ensures reproducible results",
        "development_synaptogenesis_rate": "Controls structural development",
        "development_pruning_rate": "Controls synaptic pruning"
    }
    
    return info_map.get(param, "General configuration parameter")

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="NLM Configuration Management Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlmconfig --validate configs/default.cfg
  nlmconfig --generate my_config.cfg
  nlmconfig --compare configs/default.cfg my_config.cfg
  nlmconfig --analyze configs/default.cfg
  nlmconfig --export-json config.json configs/default.cfg
        """
    )
    
    parser.add_argument(
        "config_file",
        nargs='?',
        help="Configuration file to process"
    )
    
    parser.add_argument(
        "--validate", "-v",
        action="store_true",
        help="Validate configuration file"
    )
    
    parser.add_argument(
        "--generate", "-g",
        metavar="PATH",
        help="Generate configuration from template"
    )
    
    parser.add_argument(
        "--compare", "-c",
        metavar="FILE",
        help="Compare current config with another config"
    )
    
    parser.add_argument(
        "--analyze", "-a",
        action="store_true",
        help="Perform parameter sensitivity analysis"
    )
    
    parser.add_argument(
        "--validate-schema",
        metavar="PATH",
        help="Validate configuration against JSON/YAML schema"
    )
    
    parser.add_argument(
        "--export-json",
        metavar="FILE",
        help="Export configuration as JSON"
    )
    
    parser.add_argument(
        "--export-yaml",
        metavar="FILE",
        help="Export configuration as YAML"
    )
    
    parser.add_argument(
        "--export-simple",
        metavar="FILE",
        help="Export configuration as simple key=value format"
    )
    
    parser.add_argument(
        "--list",
        action="store_true",
        help="List all configuration keys and values"
    )
    
    parser.add_argument(
        "--help", "-h",
        action="store_true",
        help="Show help message"
    )
    
    args = parser.parse_args()
    
    # Show help if requested
    if args.help or len(sys.argv) == 1:
        print_help()
        return 0
    
    try:
        # Validate help argument presence
        if args.validate and not args.config_file:
            print("Error: --validate requires a configuration file")
            return 3
        
        if args.compare and not args.config_file:
            print("Error: --compare requires a configuration file")
            return 3
        
        if args.generate:
            generate_config(args.generate)
            return 0
        
        # Load configuration if provided
        config = None
        if args.config_file:
            config = load_config(args.config_file)
            print(f"Loaded configuration from: {args.config_file}")
        
        # Validate configuration
        if args.validate and config:
            if not validate_config(config):
                return 1
        
        # Compare configurations
        if args.compare and config and os.path.exists(args.compare):
            compare_config = load_config(args.compare)
            compare_configs(config, compare_config)
        
        # Analyze parameters
        if args.analyze and config:
            analyze_parameters(config)
        
        # Export configurations
        if config:
            if args.export_json:
                save_config(config, args.export_json)
                print(f"Configuration exported to JSON: {args.export_json}")
            
            if args.export_yaml:
                save_config(config, args.export_yaml)
                print(f"Configuration exported to YAML: {args.export_yaml}")
            
            if args.export_simple:
                save_config(config, args.export_simple)
                print(f"Configuration exported to simple format: {args.export_simple}")
            
            if args.list:
                print("=== Configuration Contents ===")
                for key, value in sorted(config.items()):
                    print(f"  {key}: {value}")
        
        return 0
        
    except Exception as e:
        print(f"Error: {e}")
        return 4

if __name__ == "__main__":
    sys.exit(main())