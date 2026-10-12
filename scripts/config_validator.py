#!/usr/bin/env python3
"""Advanced configuration validation and analysis for NLM"""

import json
import sys
import os
from pathlib import Path
from typing import Dict, Any, List, Optional, Tuple
from datetime import datetime

class NLMConfigValidator:
    """Advanced configuration validation with dependency checking"""
    
    # Configuration dependencies and constraints
    CONSTRAINTS = {
        # Parameter ranges and relationships
        "simulation_timestep": {"min": 0.0001, "max": 1.0},
        "neuron_count": {"min": 1, "max": 1000000},
        "region_count": {"min": 1, "max": 100},
        "connection_probability": {"min": 0.0, "max": 1.0},
        "plasticity_learning_rate": {"min": 0.0, "max": 1.0},
        "stdp_ltp_weight": {"min": 0.0, "max": 1.0},
        "stdp_ltd_weight": {"min": 0.0, "max": 1.0},
        "stdp_time_constant": {"min": 1.0, "max": 100.0},
        "development_synaptogenesis_rate": {"min": 0.0, "max": 1.0},
        "development_pruning_rate": {"min": 0.0, "max": 1.0},
        "dopamine_baseline": {"min": 0.0, "max": 10.0},
        "reward_discount_factor": {"min": 0.0, "max": 1.0},
        "environment_width": {"min": 1, "max": 1000},
        "environment_height": {"min": 1, "max": 1000},
        "max_simulation_steps": {"min": 1, "max": 1000000},
        "simulation_time_limit": {"min": 0.0, "max": 1000000.0},
        "visualization_update_rate": {"min": 1.0, "max": 1000.0}
    }
    
    # Configuration dependencies (parameter1 must be set for parameter2 to be valid)
    DEPENDENCIES = {
        "visualization_enabled": ["visualization_update_rate"],  # If enabled, update rate matters
        "log_to_file": ["log_filename"],  # If logging to file, filename must be set
        "development_synaptogenesis_rate": ["development_pruning_rate"],  # Both development params
        "stdp_ltp_weight": ["stdp_ltd_weight"],  # Both STDP params
        "stdp_ltd_weight": ["stdp_ltp_weight"],
        "dopamine_baseline": ["reward_discount_factor"],  # Both neuromodulation params
        "reward_discount_factor": ["dopamine_baseline"]
    }
    
    # Configuration categories for analysis
    CATEGORIES = {
        "core_simulation": ["random_seed", "simulation_timestep", "neuron_count", 
                           "region_count", "connection_probability"],
        "plasticity": ["plasticity_learning_rate", "stdp_ltp_weight", 
                      "stdp_ltd_weight", "stdp_time_constant"],
        "development": ["development_synaptogenesis_rate", "development_pruning_rate"],
        "neuromodulation": ["dopamine_baseline", "reward_discount_factor"],
        "environment": ["environment_name", "environment_width", "environment_height"],
        "logging": ["log_level", "log_to_file", "log_filename"],
        "simulation": ["max_simulation_steps", "simulation_time_limit"],
        "visualization": ["visualization_enabled", "visualization_update_rate"]
    }
    
    @staticmethod
    def validate_configuration(config_data: Dict[str, Any]) -> Tuple[List[str], List[str], List[Dict]]:
        """Validate configuration with detailed error reporting"""
        errors = []
        warnings = []
        analysis = []
        
        # Validate against constraints
        for param, constraints in NLMConfigValidator.CONSTRAINTS.items():
            if param in config_data:
                value = config_data[param]
                
                # Type checking
                if isinstance(value, int) and not isinstance(value, bool):
                    if value < constraints["min"] or value > constraints["max"]:
                        errors.append(f"Parameter '{param}': value {value} is out of range [{constraints['min']}, {constraints['max']}]")
                elif isinstance(value, float):
                    if value < constraints["min"] or value > constraints["max"]:
                        errors.append(f"Parameter '{param}': value {value} is out of range [{constraints['min']}, {constraints['max']}]")
                else:
                    warnings.append(f"Parameter '{param}': expected numeric value, got {type(value).__name__}")
        
        # Validate dependencies
        for param, dependencies in NLMConfigValidator.DEPENDENCIES.items():
            if param in config_data:
                for dep in dependencies:
                    if dep not in config_data:
                        warnings.append(f"Parameter '{param}': recommends setting '{dep}' for full functionality")
        
        # Configuration analysis
        for category, params in NLMConfigValidator.CATEGORIES.items():
            category_params = []
            for param in params:
                if param in config_data:
                    category_params.append({
                        'name': param,
                        'value': config_data[param],
                        'type': type(config_data[param]).__name__
                    })
            
            if category_params:
                analysis.append({
                    'category': category,
                    'parameters': category_params,
                    'count': len(category_params)
                })
        
        # Special validation checks
        NLMConfigValidator._special_validation(config_data, errors, warnings)
        
        return errors, warnings, analysis
    
    @staticmethod
    def _special_validation(config_data: Dict[str, Any], errors: List[str], warnings: List[str]):
        """Perform special validation checks"""
        # Check for unrealistic combinations
        if 'neuron_count' in config_data and 'max_simulation_steps' in config_data:
            neurons = config_data['neuron_count']
            steps = config_data['max_simulation_steps']
            if neurons > 10000 and steps < 1000:
                warnings.append("High neuron count with low step count may result in insufficient activity")
        
        # Check timestep vs step count
        if 'simulation_timestep' in config_data and 'max_simulation_steps' in config_data:
            timestep = config_data['simulation_timestep']
            steps = config_data['max_simulation_steps']
            total_time = timestep * steps
            if total_time < 1.0:
                warnings.append(f"Total simulation time ({total_time:.3f}s) is very short for learning")
            elif total_time > 1000.0:
                warnings.append(f"Total simulation time ({total_time:.1f}s) is very long, consider reducing steps")
        
        # Check connection probability
        if 'connection_probability' in config_data:
            conn_prob = config_data['connection_probability']
            if conn_prob < 0.01:
                warnings.append("Very low connection probability may result in disconnected network")
            elif conn_prob > 0.5:
                warnings.append("High connection probability may result in overly connected network")
        
        # Check STDP parameters
        if 'stdp_ltp_weight' in config_data and 'stdp_ltd_weight' in config_data:
            ltp = config_data['stdp_ltp_weight']
            ltd = config_data['stdp_ltd_weight']
            if abs(ltp - ltd) < 0.001:
                warnings.append("LTP and LTD weights are nearly equal, may balance to no net change")
        
        # Check development rates
        if 'development_synaptogenesis_rate' in config_data and 'development_pruning_rate' in config_data:
            synap = config_data['development_synaptogenesis_rate']
            prune = config_data['development_pruning_rate']
            if synap < prune:
                warnings.append("Synaptogenesis rate is lower than pruning rate, net synapse loss")
        
        # Environment consistency
        if 'environment_name' in config_data and 'environment_width' in config_data:
            env_width = config_data['environment_width']
            if env_width < 5:
                warnings.append("Environment width is small, may limit behavioral complexity")
    
    @staticmethod
    def validate_config_file(filepath: str) -> bool:
        """Validate configuration from file"""
        try:
            with open(filepath, 'r') as f:
                if filepath.endswith('.json'):
                    config_data = json.load(f)
                else:
                    # Parse simple key=value format
                    config_data = {}
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            
                            # Try to convert to appropriate type
                            if value.lower() in ('true', 'false'):
                                config_data[key] = value.lower() == 'true'
                            elif value.isdigit():
                                config_data[key] = int(value)
                            else:
                                try:
                                    config_data[key] = float(value)
                                except ValueError:
                                    config_data[key] = value
            
            errors, warnings, analysis = NLMConfigValidator.validate_configuration(config_data)
            
            print(f"Configuration validation for: {filepath}")
            print("=" * 60)
            
            if errors:
                print("ERRORS:")
                for error in errors:
                    print(f"  ✗ {error}")
            
            if warnings:
                print("\nWARNINGS:")
                for warning in warnings:
                    print(f"  ⚠ {warning}")
            
            if not errors and not warnings:
                print("✓ Configuration is valid")
            
            print("\nConfiguration Analysis:")
            print(f"  Categories configured: {len(analysis)}")
            for category in analysis:
                print(f"    {category['category']}: {category['count']} parameters")
                for param in category['parameters']:
                    print(f"      {param['name']}: {param['value']} ({param['type']})")
            
            return len(errors) == 0
            
        except FileNotFoundError:
            print(f"Error: Configuration file not found: {filepath}")
            return False
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in configuration file: {e}")
            return False
        except Exception as e:
            print(f"Error: {e}")
            return False
    
    @staticmethod
    def generate_config_report(config_data: Dict[str, Any], output_file: Optional[str] = None):
        """Generate detailed configuration report"""
        errors, warnings, analysis = NLMConfigValidator.validate_configuration(config_data)
        
        report = {
            'timestamp': datetime.now().isoformat(),
            'validation_summary': {
                'errors': len(errors),
                'warnings': len(warnings),
                'is_valid': len(errors) == 0
            },
            'configuration_analysis': analysis,
            'errors': errors,
            'warnings': warnings
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
            print(f"Configuration report saved to: {output_file}")
        else:
            print(json.dumps(report, indent=2))
    
    @staticmethod
    def compare_configs(file1: str, file2: str):
        """Compare two configuration files"""
        try:
            # Load both configurations
            config1 = NLMConfigValidator._load_config_file(file1)
            config2 = NLMConfigValidator._load_config_file(file2)
            
            print(f"Comparison: {Path(file1).name} vs {Path(file2).name}")
            print("=" * 60)
            
            # Find all unique parameters
            all_params = set(config1.keys()) | set(config2.keys())
            
            differences = []
            similarities = []
            
            for param in sorted(all_params):
                val1 = config1.get(param, "<MISSING>")
                val2 = config2.get(param, "<MISSING>")
                
                if val1 == val2:
                    similarities.append(param)
                else:
                    differences.append((param, val1, val2))
            
            print(f"Similarities: {len(similarities)} parameters")
            print(f"Differences: {len(differences)} parameters")
            
            if differences:
                print("\nDifferences:")
                for param, val1, val2 in differences[:20]:  # Show first 20
                    print(f"  {param}:")
                    print(f"    {file1}: {val1}")
                    print(f"    {file2}: {val2}")
                
                if len(differences) > 20:
                    print(f"  ... and {len(differences) - 20} more differences")
            
            if similarities:
                print(f"\nSimilarities (first 10):")
                for param in list(similarities)[:10]:
                    print(f"  {param}: {config1.get(param, 'N/A')}")
                
                if len(similarities) > 10:
                    print(f"  ... and {len(similarities) - 10} more similarities")
            
            return differences
            
        except Exception as e:
            print(f"Error comparing configurations: {e}")
            return []
    
    @staticmethod
    def _load_config_file(filepath: str) -> Dict[str, Any]:
        """Load configuration from file"""
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                return json.load(f)
        else:
            config_data = {}
            with open(filepath, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#') and '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        if value.lower() in ('true', 'false'):
                            config_data[key] = value.lower() == 'true'
                        elif value.isdigit():
                            config_data[key] = int(value)
                        else:
                            try:
                                config_data[key] = float(value)
                            except ValueError:
                                config_data[key] = value
            return config_data
if __name__ == "__main__":
    # Command line interface for configuration validation
    if len(sys.argv) < 2:
        print("Usage: python config_validator.py <command> [options]")
        print("\nCommands:")
        print("  validate <file>              Validate configuration file")
        print("  compare <file1> <file2>      Compare two configuration files")
        print("  report <file> [output]       Generate configuration report")
        sys.exit(1)
    
    command = sys.argv[1]
    
    if command == "validate":
        if len(sys.argv) < 3:
            print("Error: Please specify configuration file to validate")
            sys.exit(1)
        NLMConfigValidator.validate_config_file(sys.argv[2])
    
    elif command == "compare":
        if len(sys.argv) < 4:
            print("Error: Please specify two configuration files to compare")
            sys.exit(1)
        NLMConfigValidator.compare_configs(sys.argv[2], sys.argv[3])
    
    elif command == "report":
        if len(sys.argv) < 3:
            print("Error: Please specify configuration file to analyze")
            sys.exit(1)
        output_file = sys.argv[3] if len(sys.argv) > 3 else None
        config_data = NLMConfigValidator._load_config_file(sys.argv[2])
        NLMConfigValidator.generate_config_report(config_data, output_file)
    
    else:
        print(f"Error: Unknown command '{command}'")
        print("Available commands: validate, compare, report")
        sys.exit(1)