#!/usr/bin/env python3
# NLM CLI Configuration Module
# 
# Configuration management tools for NLM advanced CLI

import json
import os
from pathlib import Path
from typing import Dict, Any, Optional

class NLMConfigManager:
    def __init__(self, config_dir: str = None):
        if config_dir is None:
            config_dir = Path.home() / ".nlm"
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
    
    def load_config(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from file"""
        try:
            with open(filepath, 'r') as f:
                if filepath.endswith('.json'):
                    return json.load(f)
                elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
                    import yaml
                    return yaml.safe_load(f)
                else:
                    # Try to parse as key=value format
                    config = {}
                    for line in f:
                        line = line.strip()
                        if not line or line.startswith('#'):
                            continue
                        if '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            # Try to convert to appropriate type
                            try:
                                if value.isdigit():
                                    value = int(value)
                                elif '.' in value and value.replace('.', '').isdigit():
                                    value = float(value)
                                elif value.lower() in ['true', 'false']:
                                    value = value.lower() == 'true'
                            except (ValueError, AttributeError):
                                pass
                            config[key] = value
                    return config
        except Exception as e:
            print(f"Error loading config: {e}")
            return {}
    
    def save_config(self, filepath: str, config: Dict[str, Any]) -> bool:
        """Save configuration to file"""
        try:
            if filepath.endswith('.json'):
                with open(filepath, 'w') as f:
                    json.dump(config, f, indent=2)
            elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
                import yaml
                with open(filepath, 'w') as f:
                    yaml.dump(config, f, default_flow_style=False)
            else:
                # Save in key=value format
                with open(filepath, 'w') as f:
                    for key, value in config.items():
                        f.write(f"{key} = {value}\n")
            return True
        except Exception as e:
            print(f"Error saving config: {e}")
            return False
    
    def validate_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration and return validation results"""
        validation_results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'recommendations': []
        }
        
        # Required fields
        required_fields = ['neuron_count', 'region_count', 'simulation_timestep']
        for field in required_fields:
            if field not in config:
                validation_results['errors'].append(f"Missing required field: {field}")
                validation_results['valid'] = False
        
        # Validate ranges
        if 'neuron_count' in config and config['neuron_count'] <= 0:
            validation_results['errors'].append("neuron_count must be positive")
            validation_results['valid'] = False
        
        if 'region_count' in config and config['region_count'] <= 0:
            validation_results['errors'].append("region_count must be positive")
            validation_results['valid'] = False
        
        if 'simulation_timestep' in config and config['simulation_timestep'] <= 0:
            validation_results['errors'].append("simulation_timestep must be positive")
            validation_results['valid'] = False
        
        # Add recommendations
        if 'neuron_count' in config and config['neuron_count'] > 10000:
            validation_results['recommendations'].append("Large neuron count may impact performance. Consider using smaller networks for testing.")
        
        if 'connection_probability' in config and config['connection_probability'] > 0.5:
            validation_results['warnings'].append("High connection probability may lead to synchronous firing and instability.")
        
        return validation_results
    
    def optimize_config(self, config: Dict[str, Any], target: str = 'balanced') -> Dict[str, Any]:
        """Optimize configuration for target use case"""
        optimized_config = config.copy()
        
        if target == 'performance':
            # Optimize for performance
            optimized_config.update({
                'neuron_count': min(optimized_config.get('neuron_count', 1000), 2000),
                'simulation_timestep': max(optimized_config.get('simulation_timestep', 0.001), 0.0005),
                'connection_probability': min(optimized_config.get('connection_probability', 0.15), 0.2),
                'enable_monitoring': False
            })
            
        elif target == 'research':
            # Optimize for research
            optimized_config.update({
                'neuron_count': max(optimized_config.get('neuron_count', 1000), 5000),
                'enable_monitoring': True,
                'enable_checkpointing': True,
                'replay_enabled': True,
                'development_enabled': True
            })
            
        elif target == 'balanced':
            # Keep current config, just ensure all required fields
            optimized_config.update({
                'enable_monitoring': True,
                'enable_checkpointing': False
            })
        
        return optimized_config
    
    def generate_config_template(self, preset: str = 'basic') -> Dict[str, Any]:
        """Generate configuration template based on preset"""
        if preset == 'basic':
            return {
                'neuron_count': 100,
                'region_count': 1,
                'connection_probability': 0.1,
                'simulation_timestep': 0.001,
                'random_seed': 42,
                'enable_monitoring': False,
                'enable_checkpointing': False,
                'replay_enabled': False,
                'development_enabled': False
            }
        
        elif preset == 'balanced':
            return {
                'neuron_count': 1000,
                'region_count': 2,
                'connection_probability': 0.15,
                'simulation_timestep': 0.001,
                'random_seed': 42,
                'enable_monitoring': True,
                'enable_checkpointing': False,
                'replay_enabled': True,
                'development_enabled': False,
                'memory_capacity': 500
            }
        
        elif preset == 'performance':
            return {
                'neuron_count': 2000,
                'region_count': 1,
                'connection_probability': 0.2,
                'simulation_timestep': 0.0005,
                'random_seed': 42,
                'enable_monitoring': False,
                'enable_checkpointing': False,
                'replay_enabled': False,
                'development_enabled': False
            }
        
        elif preset == 'research':
            return {
                'neuron_count': 5000,
                'region_count': 3,
                'connection_probability': 0.25,
                'simulation_timestep': 0.001,
                'random_seed': 42,
                'enable_monitoring': True,
                'enable_checkpointing': True,
                'replay_enabled': True,
                'development_enabled': True,
                'memory_capacity': 2000,
                'synaptogenesis_rate': 0.0001,
                'pruning_rate': 0.00001
            }
        
        else:
            return {
                'neuron_count': 1000,
                'region_count': 1,
                'connection_probability': 0.15,
                'simulation_timestep': 0.001,
                'random_seed': 42,
                'enable_monitoring': True,
                'enable_checkpointing': True,
                'replay_enabled': True,
                'development_enabled': True
            }

# Global config manager instance
_config_manager = None

def get_config_manager(config_dir: str = None) -> NLMConfigManager:
    """Get global config manager instance"""
    global _config_manager
    if _config_manager is None:
        _config_manager = NLMConfigManager(config_dir)
    return _config_manager

def load_config(filepath: str) -> Dict[str, Any]:
    """Convenience function to load config"""
    return get_config_manager().load_config(filepath)

def save_config(filepath: str, config: Dict[str, Any]) -> bool:
    """Convenience function to save config"""
    return get_config_manager().save_config(filepath, config)

def validate_config(config: Dict[str, Any]) -> Dict[str, Any]:
    """Convenience function to validate config"""
    return get_config_manager().validate_config(config)

def optimize_config(config: Dict[str, Any], target: str = 'balanced') -> Dict[str, Any]:
    """Convenience function to optimize config"""
    return get_config_manager().optimize_config(config, target)

if __name__ == "__main__":
    # Test the config manager
    print("Testing NLM Config Manager")
    
    # Create test config
    test_config = {
        'neuron_count': 1000,
        'region_count': 2,
        'simulation_timestep': 0.001,
        'connection_probability': 0.15
    }
    
    # Validate config
    validation = validate_config(test_config)
    print(f"Validation: {validation}")
    
    # Optimize config
    optimized = optimize_config(test_config, 'research')
    print(f"Optimized config: {optimized}")
    
    # Save config
    save_config('test_config.json', optimized)
    print("Config saved to test_config.json")
    
    # Load config
    loaded = load_config('test_config.json')
    print(f"Loaded config: {loaded}")
    
    print("Config manager test completed")
