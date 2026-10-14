# Configuration module for NLM settings

from __future__ import annotations

import json
import os
from pathlib import Path
from typing import Any, Dict, List, Optional, Union
from dataclasses import dataclass, field
from enum import Enum

class LogLevel(Enum):
    """Logging levels."""
    
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"
@dataclass
class Config:
    """Configuration class for NLM system."""
    
    # Random seed for reproducible experiments
    random_seed: int = 42
    
    # Simulation timestep in seconds
    simulation_timestep: float = 0.001
    
    # Number of neurons in the brain
    neuron_count: int = 1000
    
    # Number of brain regions
    region_count: int = 2
    
    # Connection probability for random connectivity
    connection_probability: float = 0.1
    
    # Plasticity settings
    plasticity_learning_rate: float = 0.01
    stdp_ltp_weight: float = 0.01
    stdp_ltd_weight: float = 0.012
    stdp_time_constant: float = 20.0
    
    # Development settings
    development_synaptogenesis_rate: float = 0.001
    development_pruning_rate: float = 0.0001
    
    # Neuromodulation settings
    dopamine_baseline: float = 0.1
    
    # Reward settings
    reward_discount_factor: float = 0.99
    
    # Environment settings
    environment_name: str = "GridWorld"
    environment_width: int = 10
    environment_height: int = 10
    
    # Logging settings
    log_level: LogLevel = LogLevel.INFO
    log_to_file: bool = False
    log_filename: str = "nlm.log"
    
    # Simulation settings
    max_simulation_steps: int = 10000
    simulation_time_limit: float = 0.0
    
    # Visualization settings
    visualization_enabled: bool = False
    visualization_update_rate: float = 30.0
    
    def __post_init__(self) -> None:
        """Validate configuration after initialization."""
        self._validate()
    
    def _validate(self) -> None:
        """Validate configuration parameters."""
        if self.neuron_count <= 0:
            raise ValueError("neuron_count must be positive")
        if self.region_count <= 0:
            raise ValueError("region_count must be positive")
        if not 0.0 <= self.connection_probability <= 1.0:
            raise ValueError("connection_probability must be between 0.0 and 1.0")
        if not 0.0 <= self.plasticity_learning_rate <= 1.0:
            raise ValueError("plasticity_learning_rate must be between 0.0 and 1.0")
        if self.simulation_timestep <= 0:
            raise ValueError("simulation_timestep must be positive")
        if self.max_simulation_steps < 0:
            raise ValueError("max_simulation_steps must be non-negative")
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        result = {}
        for key, value in self.__dict__.items():
            if not key.startswith('_'):
                if hasattr(value, 'value'):  # Enum
                    result[key] = value.value
                else:
                    result[key] = value
        return result
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> Config:
        """Create configuration from dictionary."""
        # Convert enum values back to enum instances
        if 'log_level' in data and isinstance(data['log_level'], str):
            data['log_level'] = LogLevel(data['log_level'])
        
        # Create instance
        return cls(**data)
    
    def load_from_file(self, filepath: Union[str, Path]) -> bool:
        """Load configuration from a JSON file."""
        try:
            filepath = Path(filepath)
            if not filepath.exists():
                return False
            
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            # Convert to appropriate types
            config_dict = {}
            for key, value in data.items():
                if key == 'log_level' and isinstance(value, str):
                    config_dict[key] = LogLevel(value)
                else:
                    config_dict[key] = value
            
            # Update instance
            for key, value in config_dict.items():
                if hasattr(self, key):
                    setattr(self, key, value)
            
            self._validate()
            return True
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
    
    def load_from_args(self, argc: int, argv: List[str]) -> bool:
        """Load configuration from command line arguments."""
        try:
            i = 1
            while i < argc:
                if argv[i].startswith('--'):
                    key = argv[i][2:].replace('-', '_')
                    if i + 1 < argc and not argv[i + 1].startswith('--'):
                        # Convert string values to appropriate types
                        value_str = argv[i + 1]
                        if key in ['random_seed', 'neuron_count', 'region_count', 
                                 'max_simulation_steps']:
                            value = int(value_str)
                        elif key in ['simulation_timestep', 'connection_probability',
                                   'plasticity_learning_rate', 'stdp_ltp_weight',
                                   'stdp_ltd_weight', 'stdp_time_constant',
                                   'development_synaptogenesis_rate',
                                   'development_pruning_rate', 'dopamine_baseline',
                                   'reward_discount_factor',
                                   'visualization_update_rate',
                                   'simulation_time_limit']:
                            value = float(value_str)
                        elif key == 'log_to_file' or key == 'visualization_enabled':
                            value = value_str.lower() in ('true', '1', 'yes', 'on')
                        elif key == 'log_level':
                            value = LogLevel(value_str.upper())
                        else:
                            value = value_str
                        
                        setattr(self, key, value)
                        i += 2
                    else:
                        i += 1
                else:
                    i += 1
            
            self._validate()
            return True
        except Exception as e:
            print(f"Error loading configuration from args: {e}")
            return False
    
    def save_to_file(self, filepath: Union[str, Path]) -> bool:
        """Save configuration to a JSON file."""
        try:
            filepath = Path(filepath)
            
            with open(filepath, 'w') as f:
                json.dump(self.to_dict(), f, indent=2)
            
            return True
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        return hasattr(self, key)
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return [key for key in self.__dict__.keys() if not key.startswith('_')]
    
    def clear(self) -> None:
        """Clear all configuration entries to defaults."""
        # Reset to default values
        self.__init__()
    
    def summary(self) -> str:
        """Get a summary string of the configuration."""
        lines = []
        lines.append("NLM Configuration Summary:")
        lines.append("=" * 50)
        
        for key in self.get_keys():
            value = getattr(self, key)
            if hasattr(value, 'value'):  # Enum
                lines.append(f"{key:30} = {value.value}")
            else:
                lines.append(f"{key:30} = {value}")
        
        return "\n".join(lines)
    
    def __str__(self) -> str:
        return self.summary()
    
    def __repr__(self) -> str:
        return f"<Config: {self.neuron_count} neurons, {self.region_count} regions>"

# Create default configuration
def create_default_config() -> Config:
    """Create a default configuration."""
    return Config()

__all__ = ["Config", "LogLevel", "create_default_config"]