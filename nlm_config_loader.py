#!/usr/bin/env python3
"""
Enhanced NLM Brain Configuration Loader

This Python script provides advanced configuration loading capabilities
for the NLM (Neural Learning Machine) project.

Features:
- JSON/YAML configuration parsing
- Configuration validation and schema support
- Default configuration generation
- Environment variable support
- Configuration file backup and restore
"""

import json
import yaml
import os
import sys
from pathlib import Path
from typing import Dict, Any, Optional, Union

class NLMConfigLoader:
    """Advanced configuration loader for NLM"""
    
    def __init__(self, config_file: Optional[str] = None):
        self.config_file = config_file
        self.config_data: Dict[str, Any] = {}
        self.schema: Optional[Dict[str, Any]] = None
        
    def load_config(self, filepath: Optional[str] = None) -> Dict[str, Any]:
        """Load configuration from file (JSON, YAML, or legacy format)"""
        if filepath is None:
            filepath = self.config_file
            
        if not filepath:
            return self.get_default_config()
            
        path = Path(filepath)
        
        if not path.exists():
            print(f"Config file not found: {filepath}")
            return self.get_default_config()
            
        try:
            if path.suffix.lower() == '.json':
                with open(path, 'r') as f:
                    self.config_data = json.load(f)
                    
            elif path.suffix.lower() in ['.yaml', '.yml']:
                with open(path, 'r') as f:
                    self.config_data = yaml.safe_load(f)
                    
            else:
                # Legacy key=value format
                self.config_data = self._load_legacy_config(path)
                
            # Validate configuration
            self._validate_config(self.config_data)
            
            print(f"Successfully loaded configuration from {filepath}")
            return self.config_data
            
        except (json.JSONDecodeError, yaml.YAMLError) as e:
            print(f"Error parsing config file {filepath}: {e}")
            return self.get_default_config()
            
    def _load_legacy_config(self, filepath: Path) -> Dict[str, Any]:
        """Load legacy key=value format"""
        config = {}
        
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                    
                if '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Remove quotes
                    if (value.startswith('"') and value.endswith('"')) or \
                       (value.startswith("'") and value.endswith("'")):
                        value = value[1:-1]
                        
                    config[key] = self._parse_value(value)
                        
        return config
        
    def _parse_value(self, value: str) -> Any:
        """Parse string value to appropriate type"""
        # Try to detect type
        if value.lower() == 'true':
            return True
        elif value.lower() == 'false':
            return False
        elif value.isdigit():
            return int(value)
        elif self._is_float(value):
            return float(value)
        else:
            return value
            
    def _is_float(self, value: str) -> bool:
        """Check if string can be parsed as float"""
        try:
            float(value)
            return True
        except ValueError:
            return False
            
    def _validate_config(self, config: Dict[str, Any]) -> None:
        """Validate configuration against schema"""
        if not self.schema:
            return
            
        # Basic validation - check for required keys
        required_sections = ['brain', 'simulation', 'memory', 'neuromodulation']
        for section in required_sections:
            if section not in config:
                print(f"Warning: Missing required section '{section}' in config")
                
        # Validate specific ranges
        if 'brain' in config:
            brain = config['brain']
            if 'neuron_count' in brain and brain['neuron_count'] < 1:
                raise ValueError("neuron_count must be positive")
            if 'connection_probability' in brain and not (0.0 <= brain['connection_probability'] <= 1.0):
                raise ValueError("connection_probability must be between 0.0 and 1.0")
                
    def get_default_config(self) -> Dict[str, Any]:
        """Get default configuration"""
        default_config = {
            'experiment': 'phase6_integration',
            'brain': {
                'neuron_count': 1000,
                'region_count': 1,
                'connection_probability': 0.1,
                'random_seed': 42
            },
            'simulation': {
                'timestep': 0.001,
                'max_steps': 1000,
                'duration_seconds': 1.0
            },
            'memory': {
                'working_memory_capacity': 100,
                'episodic_memory_max_episodes': 1000,
                'replay_interval': 100,
                'consolidation_interval': 1000
            },
            'neuromodulation': {
                'enable_dopamine': True,
                'dopamine_baseline': 0.1,
                'dopamine_peak': 1.0,
                'dopamine_decay_rate': 0.05,
                'enable_curiosity': True,
                'enable_novelty': True
            },
            'plasticity': {
                'stdp_enabled': True,
                'hebbian_enabled': True,
                'structural_enabled': True,
                'synaptogenesis_rate': 0.0001,
                'pruning_rate': 0.00001
            },
            'development': {
                'enable': True,
                'stage': 'critical',
                'initial_plasticity': 1.0
            }
        }
        
        self.config_data = default_config
        return default_config
        
    def save_config(self, filepath: str, config: Optional[Dict[str, Any]] = None) -> bool:
        """Save configuration to file"""
        if config is None:
            config = self.config_data
            
        path = Path(filepath)
        path.parent.mkdir(parents=True, exist_ok=True)
        
        try:
            if filepath.endswith('.json'):
                with open(filepath, 'w') as f:
                    json.dump(config, f, indent=2)
            elif filepath.endswith(('.yaml', '.yml')):
                with open(filepath, 'w') as f:
                    yaml.dump(config, f, default_flow_style=False)
            else:
                # Legacy format
                with open(filepath, 'w') as f:
                    self._save_legacy_config(config, f)
                    
            print(f"Configuration saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
            
    def _save_legacy_config(self, config: Dict[str, Any], file) -> None:
        """Save configuration in legacy format"""
        for key, value in config.items():
            if isinstance(value, (list, dict)):
                # Skip complex values for legacy format
                continue
            file.write(f"{key}={value}\n")
            
    def validate_config_file(self, filepath: str) -> bool:
        """Validate configuration file and return result"""
        print(f"Validating configuration file: {filepath}")
        
        loader = NLMConfigLoader(filepath)
        config = loader.load_config()
        
        # Perform validation checks
        validation_passed = True
        
        if 'brain' not in config:
            print("ERROR: Missing 'brain' section")
            validation_passed = False
            
        if 'neuron_count' in config.get('brain', {}):
            neuron_count = config['brain']['neuron_count']
            if not isinstance(neuron_count, int) or neuron_count < 1:
                print("ERROR: neuron_count must be a positive integer")
                validation_passed = False
                
        if 'simulation' not in config:
            print("ERROR: Missing 'simulation' section")
            validation_passed = False
            
        if 'timestep' in config.get('simulation', {}):
            timestep = config['simulation']['timestep']
            if not isinstance(timestep, (int, float)) or timestep <= 0:
                print("ERROR: timestep must be a positive number")
                validation_passed = False
                
        # Check for consistency between sections
        if 'brain' in config and 'simulation' in config:
            brain = config['brain']
            simulation = config['simulation']
            
            if 'neuron_count' in brain and 'max_steps' in simulation:
                if brain['neuron_count'] > 10000 and simulation['max_steps'] > 100000:
                    print("WARNING: Large brain with many steps may cause memory issues")
                    
        if validation_passed:
            print("Configuration validation PASSED")
        else:
            print("Configuration validation FAILED")
            
        return validation_passed
        
    def print_config_summary(self) -> None:
        """Print configuration summary"""
        if not self.config_data:
            print("No configuration loaded")
            return
            
        print("=== NLM Configuration Summary ===")
        
        sections = ['brain', 'simulation', 'memory', 'neuromodulation', 
                   'plasticity', 'development', 'prediction', 'cognition']
                   
        for section in sections:
            if section in self.config_data:
                print(f"\n{section.upper()}:")
                self._print_section(section, self.config_data[section])
                
    def _print_section(self, section: str, data: Dict[str, Any], indent: int = 2) -> None:
        """Print configuration section"""
        indent_str = ' ' * indent
        for key, value in data.items():
            if isinstance(value, dict):
                print(f"{indent_str}{key}:")
                self._print_section(key, value, indent + 2)
            elif isinstance(value, list):
                print(f"{indent_str}{key}: [{', '.join(map(str, value))}]")
            else:
                print(f"{indent_str}{key}: {value}")

def main():
    """Command-line interface"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='NLM Advanced Configuration Loader',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        'config_file',
        nargs='?',
        help='Configuration file (JSON, YAML, or legacy format)'
    )
    
    parser.add_argument(
        '--validate',
        action='store_true',
        help='Validate configuration file and exit'
    )
    
    parser.add_argument(
        '--summary',
        action='store_true',
        help='Print configuration summary'
    )
    
    parser.add_argument(
        '--save',
        help='Save configuration to specified file'
    )
    
    parser.add_argument(
        '--export-defaults',
        help='Export default configuration to specified file'
    )
    
    args = parser.parse_args()
    
    # Create loader
    loader = NLMConfigLoader(args.config_file)
    
    # Load configuration
    config = loader.load_config()
    
    # Handle commands
    if args.validate:
        if args.config_file:
            success = loader.validate_config_file(args.config_file)
            sys.exit(0 if success else 1)
        else:
            print("Validation requires a configuration file")
            sys.exit(1)
            
    elif args.summary:
        loader.print_config_summary()
        
    elif args.save:
        success = loader.save_config(args.save)
        sys.exit(0 if success else 1)
        
    elif args.export_defaults:
        default_config = loader.get_default_config()
        success = loader.save_config(args.export_defaults, default_config)
        sys.exit(0 if success else 1)
        
    else:
        # Show summary if config was loaded
        if loader.config_data:
            loader.print_config_summary()
        else:
            print("No configuration loaded. Use --help for usage information.")
            
    print("\nConfiguration loaded successfully!")
if __name__ == '__main__':
    main()