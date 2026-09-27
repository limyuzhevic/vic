"""
Configuration management utilities for NLM CLI
"""

import json
import yaml
from pathlib import Path
from typing import Dict, Any, Optional
from enum import Enum
import os

class ConfigFormat(Enum):
    JSON = "json"
    YAML = "yaml"
    SIMPLE = "simple"

class NLMConfigManager:
    def __init__(self):
        self.config: Dict[str, Any] = {}
        self.defaults = self._get_default_config()
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default NLM configuration"""
        return {
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
            "max_simulation_steps": 10000,
            "visualization_enabled": False,
            "log_level": "INFO",
            "log_to_file": False,
            "log_filename": "nlm.log"
        }
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from file"""
        try:
            path = Path(filepath)
            if not path.exists():
                print(f"Warning: Config file not found: {filepath}")
                return False
            
            content = path.read_text()
            
            if filepath.endswith('.json'):
                self.config = json.loads(content)
            elif filepath.endswith(('.yaml', '.yml')):
                self.config = yaml.safe_load(content)
            elif filepath.endswith('.cfg'):
                self.config = self._parse_simple_config(content)
            else:
                print(f"Error: Unsupported config file format: {filepath}")
                return False
            
            return True
        except Exception as e:
            print(f"Error loading config from {filepath}: {e}")
            return False
    
    def _parse_simple_config(self, content: str) -> Dict[str, Any]:
        """Parse simple key=value config format"""
        config = {}
        for line in content.splitlines():
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
    
    def save_to_file(self, filepath: str, format: str = "json") -> bool:
        """Save configuration to file"""
        try:
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            if format.lower() == 'json':
                with open(filepath, 'w') as f:
                    json.dump(self.config, f, indent=2)
            elif format.lower() == 'yaml':
                with open(filepath, 'w') as f:
                    yaml.dump(self.config, f, default_flow_style=False)
            elif format.lower() == 'cfg':
                with open(filepath, 'w') as f:
                    for key, value in self.config.items():
                        if isinstance(value, bool):
                            f.write(f"{key} = {str(value).lower()}\n")
                        else:
                            f.write(f"{key} = {value}\n")
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Configuration saved to {filepath}")
            return True
        except Exception as e:
            print(f"Error saving config to {filepath}: {e}")
            return False
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value"""
        return self.config.get(key, self.defaults.get(key, default))
    
    def set(self, key: str, value: Any):
        """Set configuration value"""
        self.config[key] = value
    
    def list_config(self) -> Dict[str, Any]:
        """List all configuration keys and values"""
        result = self.defaults.copy()
        result.update(self.config)
        return result
    
    def merge_with_defaults(self):
        """Merge current config with defaults"""
        self.config = self.defaults.copy()
        self.config.update(self.config)
        return self.config
    
    def validate_config(self) -> List[str]:
        """Validate configuration and return list of errors"""
        errors = []
        
        # Check required fields
        required_fields = ['random_seed', 'neuron_count', 'region_count']
        for field in required_fields:
            if field not in self.config:
                errors.append(f"Missing required field: {field}")
        
        # Validate ranges
        if self.config.get('neuron_count', 0) <= 0:
            errors.append("neuron_count must be positive")
        
        if self.config.get('region_count', 0) <= 0:
            errors.append("region_count must be positive")
        
        if self.config.get('connection_probability', 0) < 0 or self.config.get('connection_probability', 0) > 1:
            errors.append("connection_probability must be between 0 and 1")
        
        return errors

# Default configuration template
def get_default_template() -> Dict[str, Any]:
    """Get default configuration template"""
    return {
        "title": "NLM Default Configuration",
        "description": "Default configuration for NLM brain simulation",
        "version": "1.0",
        "config": NLMConfigManager()._get_default_config()
    }

# Save default template
def save_default_template(filepath: str = "configs/default.cfg"):
    """Save default configuration template"""
    Path(filepath).parent.mkdir(parents=True, exist_ok=True)
    
    config = get_default_template()
    
    # Save as .cfg file
    with open(filepath, 'w') as f:
        f.write(f"# {config['title']}\n")
        f.write(f"# {config['description']}\n")
        f.write(f"# Version: {config['version']}\n\n")
        
        for key, value in config['config'].items():
            if isinstance(value, bool):
                f.write(f"{key} = {str(value).lower()}\n")
            else:
                f.write(f"{key} = {value}\n")
    
    print(f"Default configuration template saved to {filepath}")

# Load configuration with environment variable overrides
def load_config_with_env(filepath: str = None) -> Dict[str, Any]:
    """Load configuration with environment variable overrides"""
    config_manager = NLMConfigManager()
    
    if filepath:
        config_manager.load_from_file(filepath)
    
    # Apply environment variable overrides
    env_mappings = {
        'NLM_RANDOM_SEED': 'random_seed',
        'NLM_NEURON_COUNT': 'neuron_count',
        'NLM_REGION_COUNT': 'region_count',
        'NLM_CONNECTION_PROBABILITY': 'connection_probability',
        'NLM_SIMULATION_STEPS': 'steps',
        'NLM_LOG_LEVEL': 'log_level'
    }
    
    for env_var, config_key in env_mappings.items():
        if env_var in os.environ:
            value = os.environ[env_var]
            # Try to convert to appropriate type
            if value.lower() in ('true', 'false'):
                config_manager.set(config_key, value.lower() == 'true')
            elif value.isdigit():
                config_manager.set(config_key, int(value))
            elif '.' in value and value.replace('.', '', 1).isdigit():
                config_manager.set(config_key, float(value))
            else:
                config_manager.set(config_key, value)
    
    return config_manager.list_config()
