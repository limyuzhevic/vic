"""
Validation utilities for NLM CLI
"""

import re
import os
from pathlib import Path
from typing import Dict, Any, List, Tuple
from datetime import datetime

class NLMValidator:
    def __init__(self):
        self.errors = []
        self.warnings = []
    
    def validate_file_path(self, filepath: str, must_exist: bool = True) -> Tuple[bool, List[str]]:
        """Validate a file path"""
        errors = []
        warnings = []
        
        try:
            path = Path(filepath)
            
            # Check if parent directory exists
            if not path.parent.exists():
                warnings.append(f"Directory {path.parent} does not exist, will be created")
                path.parent.mkdir(parents=True, exist_ok=True)
            
            # Check if file exists
            if must_exist and not path.exists():
                errors.append(f"File does not exist: {filepath}")
            
            # Check file extension
            if path.suffix and not self._is_valid_extension(path.suffix.lower()):
                warnings.append(f"Unexpected file extension: {path.suffix}")
            
            return len(errors) == 0, errors + warnings
            
        except Exception as e:
            errors.append(f"Error validating file path {filepath}: {e}")
            return False, errors
    
    def _is_valid_extension(self, extension: str) -> bool:
        """Check if file extension is valid"""
        valid_extensions = {
            '.json', '.yaml', '.yml', '.cfg', '.txt', '.bin', 
            '.py', '.cpp', '.hpp', '.md', '.txt', '.csv'
        }
        return extension in valid_extensions
    
    def validate_config_values(self, config: Dict[str, Any]) -> Tuple[bool, List[str], List[str]]:
        """Validate configuration values"""
        errors = []
        warnings = []
        
        # Validate numeric ranges
        numeric_validations = {
            'steps': (1, 1000000, "steps"),
            'neuron_count': (1, 1000000, "neuron_count"),
            'region_count': (1, 1000, "region_count"),
            'parallel': (1, 64, "parallel"),
            'repeats': (1, 1000, "repeats"),
            'random_seed': (0, 2**31 - 1, "random_seed"),
            'simulation_timestep': (0.0001, 1.0, "simulation_timestep")
        }
        
        for key, (min_val, max_val, name) in numeric_validations.items():
            if key in config:
                value = config[key]
                if not isinstance(value, (int, float)):
                    errors.append(f"{name} must be a number, got {type(value).__name__}")
                elif value < min_val or value > max_val:
                    errors.append(f"{name} must be between {min_val} and {max_val}, got {value}")
        
        # Validate boolean values
        boolean_fields = ['visualization_enabled', 'log_to_file']
        for field in boolean_fields:
            if field in config and not isinstance(config[field], bool):
                errors.append(f"{field} must be a boolean (true/false)")
        
        # Validate string values
        if 'environment_name' in config and not isinstance(config['environment_name'], str):
            errors.append("environment_name must be a string")
        
        # Check for reasonable values
        if 'connection_probability' in config:
            prob = config['connection_probability']
            if not isinstance(prob, (int, float)) or prob < 0 or prob > 1:
                errors.append("connection_probability must be between 0 and 1")
        
        return len(errors) == 0, errors, warnings
    
    def validate_command_args(self, command: str, args: Dict[str, Any]) -> Tuple[bool, List[str], List[str]]:
        """Validate command line arguments"""
        errors = []
        warnings = []
        
        # Command-specific validation
        if command == 'simulate':
            if 'steps' in args and args['steps'] <= 0:
                errors.append("steps must be positive")
            
            if 'neuron_count' in args and args['neuron_count'] <= 0:
                errors.append("neuron_count must be positive")
            
            if 'output' in args:
                valid, file_errors = self.validate_file_path(args['output'], must_exist=False)
                if not valid:
                    warnings.extend(file_errors)
        
        elif command == 'create':
            if 'neurons' in args and args['neurons'] <= 0:
                errors.append("neurons must be positive")
            
            if 'output' in args:
                valid, file_errors = self.validate_file_path(args['output'], must_exist=False)
                if not valid:
                    warnings.extend(file_errors)
        
        elif command == 'explore':
            if 'file' in args:
                valid, file_errors = self.validate_file_path(args['file'], must_exist=True)
                if not valid:
                    errors.extend(file_errors)
        
        elif command == 'batch':
            if 'file' in args:
                valid, file_errors = self.validate_file_path(args['file'], must_exist=True)
                if not valid:
                    errors.extend(file_errors)
            
            if 'parallel' in args and args['parallel'] <= 0:
                errors.append("parallel must be positive")
        
        return len(errors) == 0, errors, warnings
    
    def validate_config_file(self, filepath: str) -> Tuple[bool, List[str], List[str]]:
        """Validate a configuration file"""
        errors = []
        warnings = []
        
        # First validate file path
        valid, path_errors = self.validate_file_path(filepath, must_exist=True)
        if not valid:
            errors.extend(path_errors)
            return False, errors, warnings
        
        try:
            # Try to parse based on extension
            if filepath.endswith('.json'):
                with open(filepath, 'r') as f:
                    config = json.load(f)
            elif filepath.endswith(('.yaml', '.yml')):
                with open(filepath, 'r') as f:
                    config = yaml.safe_load(f)
            elif filepath.endswith('.cfg'):
                config = self._parse_simple_config(filepath)
            else:
                errors.append(f"Unsupported config file format: {filepath}")
                return False, errors, warnings
            
            # Validate the parsed configuration
            valid, config_errors, config_warnings = self.validate_config_values(config)
            if not valid:
                errors.extend(config_errors)
            warnings.extend(config_warnings)
            
        except Exception as e:
            errors.append(f"Error reading config file {filepath}: {e}")
        
        return len(errors) == 0, errors, warnings
    
    def _parse_simple_config(self, filepath: str) -> Dict[str, Any]:
        """Parse simple key=value config file"""
        config = {}
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Type conversion
                    if value.lower() in ('true', 'false'):
                        value = value.lower() == 'true'
                    elif value.isdigit():
                        value = int(value)
                    elif '.' in value and value.replace('.', '', 1).isdigit():
                        value = float(value)
                    
                    config[key] = value
        return config
    
    def validate_output_format(self, format: str) -> bool:
        """Validate output format"""
        return format.lower() in ['text', 'json', 'yaml']
    
    def validate_environment(self) -> List[str]:
        """Validate system environment"""
        warnings = []
        
        # Check available memory (mock)
        try:
            import psutil
            available_memory = psutil.virtual_memory().available / (1024 * 1024 * 1024)  # GB
            if available_memory < 2:
                warnings.append(f"Low memory available: {available_memory:.1f}GB. Consider reducing parallel processing.")
        except ImportError:
            pass
        
        # Check for required dependencies
        required_modules = ['pynlm', 'yaml', 'json']
        for module in required_modules:
            try:
                __import__(module)
            except ImportError:
                warnings.append(f"Missing optional module: {module}")
        
        return warnings

# Import json and yaml for use
import json
import yaml
