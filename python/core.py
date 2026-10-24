"""
Core NLM functionality - Configuration, random generation, and simulation management
"""

import json
import time
from typing import Dict, Any, Optional, Union

# Import from the C++ bindings
from .pynlm import NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags
from .pynlm import MembranePotential, SynapticWeight, FiringRate, FiringState
from .pynlm import NeuronType, RegionId, PopulationId, ActionType, ObservationData
from .pynlm import Config, RandomGenerator, SimulationClock, Logger

__all__ = [
    'Config', 'RandomGenerator', 'SimulationClock', 'Logger',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'PlasticityFlags', 'MembranePotential', 'SynapticWeight', 'FiringRate', 'FiringState',
    'NeuronType', 'RegionId', 'PopulationId', 'ActionType', 'ObservationData',
]

# Constants
DEFAULT_NEURON_COUNT = 1000
DEFAULT_REGION_COUNT = 2
PLASTICITY_LEARNING_RATE = 0.01
STDP_LTP_WEIGHT = 0.01
STDP_LTD_WEIGHT = 0.012

class NLMConfig:
    """Enhanced configuration manager with validation and presets"""
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize NLMConfig
        
        Args:
            config_file: Path to configuration file
        """
        self._config = Config()
        self._config_file = config_file
        
        if config_file:
            self.load_from_file(config_file)
        else:
            self.load_from_preset('default')
    
    def load_from_preset(self, preset_name: str) -> bool:
        """Load configuration from preset
        
        Args:
            preset_name: Name of preset ('default', 'quick', 'detailed', 'fast')
            
        Returns:
            True if loaded successfully
        """
        presets = {
            'default': {
                'random_seed': 42,
                'simulation_timestep': 0.001,
                'neuron_count': DEFAULT_NEURON_COUNT,
                'region_count': DEFAULT_REGION_COUNT,
                'connection_probability': 0.1,
                'plasticity_learning_rate': PLASTICITY_LEARNING_RATE,
                'plasticity_stdp_ltp_weight': STDP_LTP_WEIGHT,
                'plasticity_stdp_ltd_weight': STDP_LTD_WEIGHT,
                'plasticity_stdp_time_constant': 20.0,
                'development_synaptogenesis_rate': 0.001,
                'development_pruning_rate': 0.0001,
                'dopamine_baseline': 0.0,
                'reward_discount_factor': 0.99,
                'environment_name': 'GridWorld',
                'environment_width': 10,
                'environment_height': 10,
                'log_level': 'INFO',
                'log_to_file': False,
                'log_filename': 'nlm.log',
                'max_simulation_steps': 10000,
                'simulation_time_limit': 0.0,
                'visualization_enabled': False,
                'visualization_update_rate': 30.0,
            },
            'quick': {
                'random_seed': 42,
                'simulation_timestep': 0.001,
                'neuron_count': 100,
                'region_count': 2,
                'connection_probability': 0.2,
                'plasticity_learning_rate': 0.005,
                'plasticity_stdp_ltp_weight': 0.005,
                'plasticity_stdp_ltd_weight': 0.01,
                'plasticity_stdp_time_constant': 10.0,
                'development_synaptogenesis_rate': 0.0005,
                'development_pruning_rate': 0.00005,
                'dopamine_baseline': 0.0,
                'reward_discount_factor': 0.95,
                'environment_name': 'SimpleWorld',
                'environment_width': 5,
                'environment_height': 5,
                'log_level': 'WARN',
                'log_to_file': False,
                'max_simulation_steps': 1000,
            },
            'detailed': {
                'random_seed': 42,
                'simulation_timestep': 0.0001,
                'neuron_count': 10000,
                'region_count': 5,
                'connection_probability': 0.05,
                'plasticity_learning_rate': 0.02,
                'plasticity_stdp_ltp_weight': 0.02,
                'plasticity_stdp_ltd_weight': 0.024,
                'plasticity_stdp_time_constant': 50.0,
                'development_synaptogenesis_rate': 0.002,
                'development_pruning_rate': 0.0002,
                'dopamine_baseline': 0.1,
                'reward_discount_factor': 0.999,
                'environment_name': 'ComplexWorld',
                'environment_width': 20,
                'environment_height': 20,
                'log_level': 'DEBUG',
                'log_to_file': True,
                'log_filename': 'nlm_detailed.log',
                'max_simulation_steps': 100000,
                'simulation_time_limit': 1000.0,
                'visualization_enabled': True,
                'visualization_update_rate': 60.0,
            },
            'fast': {
                'random_seed': 42,
                'simulation_timestep': 0.01,
                'neuron_count': 50,
                'region_count': 1,
                'connection_probability': 0.5,
                'plasticity_learning_rate': 0.001,
                'plasticity_stdp_ltp_weight': 0.001,
                'plasticity_stdp_ltd_weight': 0.002,
                'plasticity_stdp_time_constant': 5.0,
                'development_synaptogenesis_rate': 0.0001,
                'development_pruning_rate': 0.00001,
                'dopamine_baseline': 0.0,
                'reward_discount_factor': 0.9,
                'environment_name': 'GridWorld',
                'environment_width': 3,
                'environment_height': 3,
                'log_level': 'ERROR',
                'log_to_file': False,
                'max_simulation_steps': 100,
            }
        }
        
        if preset_name not in presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available: {list(presets.keys())}")
        
        preset_config = presets[preset_name]
        for key, value in preset_config.items():
            self.set(key, value)
        
        return True
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from file
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            True if loaded successfully
        """
        try:
            with open(filepath, 'r') as f:
                config_data = json.load(f)
            
            for key, value in config_data.items():
                self.set(key, value)
            
            self._config_file = filepath
            return True
        except Exception as e:
            print(f"Error loading config from {filepath}: {e}")
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to file
        
        Args:
            filepath: Path to save configuration file
            
        Returns:
            True if saved successfully
        """
        try:
            config_data = {}
            for key in self._config.getKeys():
                config_data[key] = self.get(key)
            
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
            
            self._config_file = filepath
            return True
        except Exception as e:
            print(f"Error saving config to {filepath}: {e}")
            return False
    
    def set(self, key: str, value: Any) -> bool:
        """Set configuration value with validation
        
        Args:
            key: Configuration key
            value: Configuration value
            
        Returns:
            True if set successfully
        """
        # Validate common parameters
        if key == 'neuron_count' and (value < 1 or value > 1000000):
            raise ValueError("neuron_count must be between 1 and 1,000,000")
        
        if key == 'region_count' and (value < 1 or value > 100):
            raise ValueError("region_count must be between 1 and 100")
        
        if key == 'simulation_timestep' and value <= 0:
            raise ValueError("simulation_timestep must be positive")
        
        if key == 'plasticity_learning_rate' and (value < 0 or value > 1):
            raise ValueError("plasticity_learning_rate must be between 0 and 1")
        
        if key == 'connection_probability' and (value < 0 or value > 1):
            raise ValueError("connection_probability must be between 0 and 1")
        
        try:
            if isinstance(value, bool):
                self._config.set(key, value)
            elif isinstance(value, int):
                self._config.set(key, value)
            elif isinstance(value, float):
                self._config.set(key, value)
            elif isinstance(value, str):
                self._config.set(key, value)
            else:
                # Try to convert to string
                self._config.set(key, str(value))
            return True
        except Exception as e:
            print(f"Error setting config {key}={value}: {e}")
            return False
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        value = self._config.get(key)
        return value.value if value else default
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists
        """
        return self._config.has(key)
    
    def get_keys(self) -> list:
        """Get all configuration keys
        
        Returns:
            List of configuration keys
        """
        return self._config.getKeys()
    
    def get_summary(self) -> str:
        """Get configuration summary
        
        Returns:
            Configuration summary string
        """
        return self._config.summary()
    
    def clear(self) -> None:
        """Clear all configuration values"""
        self._config.clear()

# Create default config instance for convenience
_default_config = NLMConfig()
def get_config() -> NLMConfig:
    """Get the default configuration instance
    
    Returns:
        NLMConfig instance
    """
    return _default_config
