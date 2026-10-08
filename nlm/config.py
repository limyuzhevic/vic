import json
import os
from typing import Dict, Any, Optional, List
from pathlib import Path

"""
Enhanced NLM Configuration Management

Provides improved configuration handling for NLM simulations with better
file handling, validation, and user-friendly interfaces.
"""

class NLMConfigManager:
    """
    Enhanced configuration manager for NLM with better file handling,
    validation, and user-friendly interfaces.
    """
    
    def __init__(self, config_dir: str = "configs"):
        """
        Initialize configuration manager.
        
        Args:
            config_dir: Directory for configuration files
        """
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
    def create_config(self, name: str, config_data: Optional[Dict[str, Any]] = None) -> str:
        """
        Create a new configuration file.
        
        Args:
            name: Name of the configuration (without extension)
            config_data: Configuration data. If None, uses default values.
            
        Returns:
            Path to the created configuration file
        """
        if config_data is None:
            config_data = self._get_default_config()
        
        config_file = self.config_dir / f"{name}.json"
        
        with open(config_file, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        return str(config_file)
    
    def load_config(self, name: str) -> Dict[str, Any]:
        """
        Load configuration from file.
        
        Args:
            name: Name of the configuration (with or without .json extension)
            
        Returns:
            Configuration dictionary
            
        Raises:
            FileNotFoundError: If configuration file doesn't exist
            json.JSONDecodeError: If configuration file is not valid JSON
        """
        if not name.endswith('.json'):
            name += '.json'
        
        config_file = self.config_dir / name
        
        if not config_file.exists():
            raise FileNotFoundError(f"Configuration file not found: {config_file}")
        
        with open(config_file, 'r') as f:
            return json.load(f)
    
    def validate_config(self, config: Dict[str, Any]) -> bool:
        """
        Validate configuration dictionary.
        
        Args:
            config: Configuration dictionary to validate
            
        Returns:
            True if configuration is valid, False otherwise
        """
        # Required fields with their types
        required_fields = {
            'simulation_timestep': (float, int),
            'neuron_count': int,
            'region_count': int,
            'connection_probability': (float, int),
        }
        
        # Check required fields
        for field, field_type in required_fields.items():
            if field not in config:
                print(f"Missing required field: {field}")
                return False
            
            value = config[field]
            if not isinstance(value, field_type):
                print(f"Field {field} must be of type {field_type}, got {type(value)}")
                return False
        
        # Validate field ranges
        if not (0.0 <= config['connection_probability'] <= 1.0):
            print("connection_probability must be between 0.0 and 1.0")
            return False
        
        if config['neuron_count'] <= 0:
            print("neuron_count must be positive")
            return False
        
        if config['region_count'] <= 0:
            print("region_count must be positive")
            return False
        
        if config['simulation_timestep'] <= 0:
            print("simulation_timestep must be positive")
            return False
        
        return True
    
    def export_config(self, name: str, output_format: str = 'ini') -> str:
        """
        Export configuration in different formats.
        
        Args:
            name: Name of the configuration
            output_format: Output format ('ini', 'yaml', or 'json')
            
        Returns:
            String representation of the configuration in the specified format
        """
        config = self.load_config(name)
        
        if output_format.lower() == 'ini':
            return self._config_to_ini(config)
        elif output_format.lower() == 'yaml':
            import yaml
            return yaml.dump(config)
        else:  # json
            return json.dumps(config, indent=2)
    
    def _config_to_ini(self, config: Dict[str, Any]) -> str:
        """Convert configuration dictionary to INI format"""
        lines = ["[NLM Configuration]", ""]
        
        # Basic settings
        lines.append("[Basic Settings]")
        lines.append(f"simulation_timestep = {config.get('simulation_timestep', 0.001)}")
        lines.append(f"neuron_count = {config.get('neuron_count', 1000)}")
        lines.append(f"region_count = {config.get('region_count', 2)}")
        lines.append(f"connection_probability = {config.get('connection_probability', 0.1)}")
        lines.append(f"random_seed = {config.get('random_seed', 42)}")
        lines.append("")
        
        # Neural dynamics
        lines.append("[Neural Dynamics]")
        lines.append(f"lif_threshold = {config.get('lif_threshold', -55.0)}")
        lines.append(f"lif_reset_potential = {config.get('lif_reset_potential', -70.0)}")
        lines.append(f"lif_resting_potential = {config.get('lif_resting_potential', -70.0)}")
        lines.append(f"lif_leak_conductance = {config.get('lif_leak_conductance', 10.0)}")
        lines.append("")
        
        # Plasticity parameters
        lines.append("[Plasticity Parameters]")
        lines.append(f"stdp_ltp_weight = {config.get('stdp_ltp_weight', 0.01)}")
        lines.append(f"stdp_ltd_weight = {config.get('stdp_ltd_weight', 0.012)}")
        lines.append(f"stdp_tau = {config.get('stdp_tau', 20.0)}")
        lines.append("")
        
        return '\n'.join(lines)
    
    def list_configs(self) -> list:
        """
        List all available configuration files.
        
        Returns:
            List of configuration file names
        """
        config_files = list(self.config_dir.glob('*.json'))
        return [f.name for f in config_files]
    
    def show_config(self, name: str) -> None:
        """
        Display configuration contents.
        
        Args:
            name: Name of the configuration
        """
        try:
            config = self.load_config(name)
            print(f"Configuration: {name}")
            print("=" * 50)
            for key, value in config.items():
                print(f"{key}: {value}")
        except Exception as e:
            print(f"Error loading configuration: {e}")
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default configuration values"""
        return {
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "random_seed": 42,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
            "working_memory_capacity": 100,
            "episodic_memory_max_episodes": 1000,
        }

# Convenience functions for common configuration tasks
def create_default_config() -> Dict[str, Any]:
    """
    Create a default configuration dictionary.
    
    Returns:
        Default configuration dictionary
    """
    return NLMConfigManager()._get_default_config()


def create_small_config() -> Dict[str, Any]:
    """
    Create a small configuration for quick testing.
    
    Returns:
        Small configuration dictionary
    """
    return {
        "simulation_timestep": 0.001,
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.2,
        "random_seed": 42,
    }


def create_medium_config() -> Dict[str, Any]:
    """
    Create a medium configuration for standard experiments.
    
    Returns:
        Medium configuration dictionary
    """
    return {
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "random_seed": 42,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.018,
    }


def create_large_config() -> Dict[str, Any]:
    """
    Create a large configuration for intensive research.
    
    Returns:
        Large configuration dictionary
    """
    return {
        "simulation_timestep": 0.0005,
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.1,
        "random_seed": 42,
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.014,
        "synaptogenesis_rate": 0.00005,
        "pruning_rate": 0.000005,
    }

# Export commonly used functions
__all__ = [
    'NLMConfigManager',
    'create_default_config',
    'create_small_config',
    'create_medium_config',
    'create_large_config',
]

# Convenience functions for common configuration tasks
def create_default_config() -> Dict[str, Any]:
    """
    Create a default configuration dictionary.
    
    Returns:
        Default configuration dictionary
    """
    return NLMConfigManager()._get_default_config()


def create_small_config() -> Dict[str, Any]:
    """
    Create a small configuration for quick testing.
    
    Returns:
        Small configuration dictionary
    """
    return {
        "simulation_timestep": 0.001,
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.2,
        "random_seed": 42,
    }


def create_medium_config() -> Dict[str, Any]:
    """
    Create a medium configuration for standard experiments.
    
    Returns:
        Medium configuration dictionary
    """
    return {
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "random_seed": 42,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.018,
    }


def create_large_config() -> Dict[str, Any]:
    """
    Create a large configuration for intensive research.
    
    Returns:
        Large configuration dictionary
    """
    return {
        "simulation_timestep": 0.0005,
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.1,
        "random_seed": 42,
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.014,
        "synaptogenesis_rate": 0.00005,
        "pruning_rate": 0.000005,
    }

# Export commonly used functions
__all__ = [
    'NLMConfigManager',
    'create_default_config',
    'create_small_config',
    'create_medium_config',
    'create_large_config',
]
