# Configuration management for NLM
import json
import os
from typing import Dict, List, Any, Optional

class Config:
    """NLM configuration with key-value storage."""
    
    def __init__(self):
        """Create empty configuration."""
        self._config: Dict[str, Any] = {}
        self._defaults: Dict[str, Any] = self._get_default_config()
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default configuration values."""
        return {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "brain.connection_probability": 0.05,
            "brain.initial_weight_mean": 0.5,
            "brain.initial_weight_std": 0.1,
            "brain.v_thresh": -50.0,
            "brain.v_rest": -70.0,
            "brain.v_reset": -75.0,
            "brain.tau_mem": 20.0,
            "brain.tau_ref": 2.0,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.stdp.tau_plus": 20.0,
            "plasticity.stdp.tau_minus": 20.0,
            "plasticity.hebbian.enable": True,
            "plasticity.hebbian.learning_rate": 0.01,
            "plasticity.structural.enable": True,
            "plasticity.structural.synaptogenesis_rate": 0.0001,
            "plasticity.structural.pruning_rate": 0.00001,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "development.synaptogenesis_rate": 0.001,
            "development.pruning_rate": 0.0001,
            "development.maturation_rate": 0.01,
            "reward.discount_factor": 0.99,
            "environment.width": 20,
            "environment.height": 20,
            "environment.vision_width": 8,
            "environment.vision_height": 8,
            "max_energy": 100.0,
            "energy_decay_rate": 0.01,
            "log_level": "INFO",
            "log_to_file": False,
            "log_filename": "nlm.log",
            "max_simulation_steps": 10000,
            "simulation_time_limit": 0.0,
        }
    
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from a JSON file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
                self._config.update(data)
            return True
        except Exception as e:
            print(f"Warning: Could not load config from {filepath}: {e}")
            return False
    
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to a JSON file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self._config, f, indent=2)
            return True
        except Exception as e:
            print(f"Error: Could not save config to {filepath}: {e}")
            return False
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        return key in self._config or key in self._defaults
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value for key."""
        return self._config.get(key, self._defaults.get(key, default))
    
    def getOr(self, key: str, default: Any) -> Any:
        """Get configuration value with type-specific default."""
        return self._config.get(key, self._defaults.get(key, default))
    
    def set(self, key: str, value: Any, source: str = "user") -> None:
        """Set configuration value."""
        self._config[key] = value
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys."""
        keys = set()
        keys.update(self._config.keys())
        keys.update(self._defaults.keys())
        return sorted(list(keys))
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self._config.clear()
    
    def summary(self) -> str:
        """Get a summary string of the configuration."""
        lines = ["NLM Configuration Summary:"]
        lines.append(f"  Total keys: {len(self.getKeys())}")
        
        # Show some key settings
        important_keys = [
            "brain.neuron_count",
            "brain.synapse_density", 
            "plasticity.stdp.enable",
            "plasticity.hebbian.enable",
            "simulation_timestep",
            "random_seed"
        ]
        
        for key in important_keys:
            if self.has(key):
                value = self.get(key)
                lines.append(f"  {key}: {value}")
        
        return "\n".join(lines)
    
    def __repr__(self) -> str:
        """String representation of config."""
        return f"<Config keys={len(self.getKeys())}>"
    
    def __str__(self) -> str:
        """Human-readable config summary."""
        return self.summary()
# Global function to create default config
def createDefaultConfig() -> Config:
    """Create a new default configuration."""
    return Config()
