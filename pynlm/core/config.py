# NLM Python Config Implementation

import json
from typing import Dict, Any, Optional, List

class Config:
    """Configuration for NLM brain."""
    
    def __init__(self, **kwargs):
        self._config = {}
        self._config.update(kwargs)
        
        # Set defaults
        defaults = {
            'random_seed': 42,
            'simulation_timestep': 0.001,
            'neuron_count': 1000,
            'region_count': 2,
            'connection_probability': 0.1,
            'plasticity_learning_rate': 0.01,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'stdp_time_constant': 20.0,
            'development_synaptogenesis_rate': 0.001,
            'development_pruning_rate': 0.0001,
            'dopamine_baseline': 0.1,
            'reward_discount_factor': 0.99,
            'environment_name': 'GridWorld',
            'environment_width': 10,
            'environment_height': 10,
            'log_level': 'INFO',
            'log_to_file': False,
            'log_filename': 'nlm.log',
            'max_simulation_steps': 10000,
            'visualization_enabled': False,
            'visualization_update_rate': 30.0
        }
        
        for key, value in defaults.items():
            if key not in self._config:
                self._config[key] = value
    
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value."""
        self._config[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value."""
        return self._config.get(key, default)
    
    def has(self, key: str) -> bool:
        """Check if configuration has a key."""
        return key in self._config
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._config.keys())
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        try:
            with open(filepath, 'r') as f:
                loaded_config = json.load(f)
            self._config.update(loaded_config)
            return True
        except Exception:
            # Try to parse as simple key=value format
            try:
                with open(filepath, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            # Convert numeric values
                            try:
                                if '.' in value:
                                    value = float(value)
                                else:
                                    value = int(value)
                            except ValueError:
                                pass
                            self._config[key] = value
                return True
            except Exception:
                return False
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to JSON file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self._config, f, indent=2)
            return True
        except Exception:
            return False
    
    def summary(self) -> str:
        """Get configuration summary."""
        lines = [f"Configuration ({len(self._config)} entries):"]
        for key, value in self._config.items():
            lines.append(f"  {key} = {value}")
        return "\n".join(lines)
    
    @classmethod
    def create_default_config(cls) -> 'Config':
        """Create a default configuration."""
        return cls()

# Module-level functions for compatibility
def create_default_config() -> Config:
    """Create default configuration."""
    return Config.create_default_config()