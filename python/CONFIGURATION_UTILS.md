# NLM Python Bindings - Configuration Management Utilities

This guide provides configuration management utilities and tools for Python users working with NLM configurations.

## Configuration Management Overview

NLM configurations are powerful tools that control every aspect of the neural simulation, from brain structure to learning behavior. However, managing configurations effectively can be challenging, especially for complex applications. This guide provides utilities to simplify configuration management, validation, and deployment.

## Configuration Management Classes

### 1. ConfigurationManager Class

```python
import pynlm
import json
import yaml
from pathlib import Path
from typing import Dict, Any, Optional, Union
import copy

class ConfigurationManager:
    """Comprehensive configuration management for NLM applications."""
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize configuration manager.
        
        Args:
            config_file: Path to configuration file
        """
        self.config = pynlm.createDefaultConfig()
        self.config_history = []
        self.validation_rules = self._load_default_validation_rules()
        
        if config_file:
            self.load_from_file(config_file)
    
    def _load_default_validation_rules(self) -> Dict[str, Any]:
        """Load default validation rules for NLM configurations."""
        return {
            'brain': {
                'neuron_count': {'type': 'int', 'min': 100, 'max': 10000},
                'synapse_density': {'type': 'float', 'min': 0.0, 'max': 1.0},
                'connection_probability': {'type': 'float', 'min': 0.0, 'max': 1.0},
                'stdp_ltp_weight': {'type': 'float', 'min': 0.0, 'max': 1.0},
                'stdp_ltd_weight': {'type': 'float', 'min': 0.0, 'max': 1.0},
            },
            'plasticity': {
                'stdp': {'type': 'bool'},
                'hebbian': {'type': 'bool'},
                'structural': {'type': 'bool'},
            },
            'neuromodulation': {
                'dopamine': {'type': 'dict', 'required_keys': ['scale']},
                'curiosity': {'type': 'dict', 'required_keys': ['enable']},
                'novelty': {'type': 'dict', 'required_keys': ['enable']},
            },
            'development': {
                'enable': {'type': 'bool'},
                'stages': {
                    'critical_period_start': {'type': 'int', 'min': 0},
                    'critical_period_end': {'type': 'int', 'min': 0},
                }
            }
        }
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from file (JSON or YAML).
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            path = Path(filepath)
            
            if path.suffix.lower() == '.json':
                with open(path, 'r') as f:
                    config_data = json.load(f)
            elif path.suffix.lower() in ['.yaml', '.yml']:
                with open(path, 'r') as f:
                    config_data = yaml.safe_load(f)
            else:
                raise ValueError(f"Unsupported file format: {path.suffix}")
            
            # Convert to NLM configuration format
            self._apply_config_data(config_data)
            
            # Save to history
            self.config_history.append({
                'timestamp': datetime.now().isoformat(),
                'action': 'load_from_file',
                'filepath': filepath,
                'config': copy.deepcopy(self.config.getKeys())
            })
            
            print(f"Successfully loaded configuration from {filepath}")
            return True
            
        except Exception as e:
            print(f"Error loading configuration from {filepath}: {e}")
            return False
    
    def load_from_dict(self, config_dict: Dict[str, Any]) -> bool:
        """Load configuration from dictionary.
        
        Args:
            config_dict: Configuration dictionary
            
        Returns:
            True if successful, False otherwise
        """
        try:
            self._apply_config_data(config_dict)
            
            # Save to history
            self.config_history.append({
                'timestamp': datetime.now().isoformat(),
                'action': 'load_from_dict',
                'config': copy.deepcopy(self.config.getKeys())
            })
            
            print("Successfully loaded configuration from dictionary")
            return True
            
        except Exception as e:
            print(f"Error loading configuration from dictionary: {e}")
            return False
    
    def _apply_config_data(self, config_data: Dict[str, Any]):
        """Apply configuration data to the config object."""
        # Handle nested configuration structure
        if 'brain' in config_data:
            for key, value in config_data['brain'].items():
                self.config.set(f"brain.{key}", value)
        
        if 'plasticity' in config_data:
            for key, value in config_data['plasticity'].items():
                self.config.set(f"plasticity.{key}", value)
        
        if 'neuromodulation' in config_data:
            for key, value in config_data['neuromodulation'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        self.config.set(f"neuromodulation.{key}.{subkey}", subvalue)
                else:
                    self.config.set(f"neuromodulation.{key}", value)
        
        if 'development' in config_data:
            for key, value in config_data['development'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        self.config.set(f"development.{key}.{subkey}", subvalue)
                else:
                    self.config.set(f"development.{key}", value)
        
        # Handle top-level configuration
        for key, value in config_data.items():
            if key not in ['brain', 'plasticity', 'neuromodulation', 'development']:
                self.config.set(key, value)
    
    def save_to_file(self, filepath: str, format: str = 'json') -> bool:
        """Save configuration to file.
        
        Args:
            filepath: Path to save configuration
            format: File format ('json' or 'yaml')
            
        Returns:
            True if successful, False otherwise
        """
        try:
            path = Path(filepath)
            
            # Convert config to dictionary
            config_dict = self._config_to_dict()
            
            if format.lower() == 'json':
                with open(path, 'w') as f:
                    json.dump(config_dict, f, indent=2)
            elif format.lower() in ['yaml', 'yml']:
                with open(path, 'w') as f:
                    yaml.dump(config_dict, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            # Save to history
            self.config_history.append({
                'timestamp': datetime.now().isoformat(),
                'action': 'save_to_file',
                'filepath': filepath,
                'format': format
            })
            
            print(f"Successfully saved configuration to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving configuration to {filepath}: {e}")
            return False
    
    def _config_to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        config_dict = {}
        
        # Get all configuration keys and convert to nested structure
        keys = self.config.getKeys()
        
        for key in keys:
            value = self.config.get(key)
            
            # Group related keys
            if key.startswith('brain.'):
                section = key.split('.')[0]
                subkey = key.split('.')[1]
                
                if section not in config_dict:
                    config_dict[section] = {}
                
                config_dict[section][subkey] = value
            
            elif key.startswith('plasticity.'):
                section = key.split('.')[0]
                subkey = key.split('.')[1]
                
                if section not in config_dict:
                    config_dict[section] = {}
                
                config_dict[section][subkey] = value
            
            elif key.startswith('neuromodulation.'):
                parts = key.split('.')
                if len(parts) >= 3:
                    section, subkey, subsubkey = parts[0], parts[1], parts[2]
                    
                    if section not in config_dict:
                        config_dict[section] = {}
                    
                    if subkey not in config_dict[section]:
                        config_dict[section][subkey] = {}
                    
                    config_dict[section][subkey][subsubkey] = value
                else:
                    if section not in config_dict:
                        config_dict[section] = {}
                    
                    config_dict[section][subkey] = value
            
            elif key.startswith('development.'):
                parts = key.split('.')
                if len(parts) >= 3:
                    section, subkey, subsubkey = parts[0], parts[1], parts[2]
                    
                    if section not in config_dict:
                        config_dict[section] = {}
                    
                    if subkey not in config_dict[section]:
                        config_dict[section][subkey] = {}
                    
                    config_dict[section][subkey][subsubkey] = value
                else:
                    if section not in config_dict:
                        config_dict[section] = {}
                    
                    config_dict[section][subkey] = value
            
            else:
                # Top-level configuration
                config_dict[key] = value
        
        return config_dict
    
    def validate(self) -> Dict[str, Any]:
        """Validate current configuration.
        
        Returns:
            Validation results dictionary
        """
        validation_result = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': []
        }
        
        # Check validation rules
        for section, rules in self.validation_rules.items():
            if section not in validation_result:
                validation_result[section] = {}
            
            for key, rule in rules.items():
                config_key = f"{section}.{key}"
                
                # Check if key exists in config
                if self.config.has(config_key):
                    value = self.config.get(config_key)
                    
                    # Validate type
                    if rule['type'] == 'int' and not isinstance(value, int):
                        validation_result['errors'].append(
                            f"Invalid type for {config_key}: expected int, got {type(value)}"
                        )
                    
                    elif rule['type'] == 'float' and not isinstance(value, float):
                        validation_result['errors'].append(
                            f"Invalid type for {config_key}: expected float, got {type(value)}"
                        )
                    
                    elif rule['type'] == 'bool' and not isinstance(value, bool):
                        validation_result['errors'].append(
                            f"Invalid type for {config_key}: expected bool, got {type(value)}"
                        )
                    
                    # Validate range
                    if 'min' in rule and isinstance(value, (int, float)):
                        if value < rule['min']:
                            validation_result['warnings'].append(
                                f"Value {config_key} ({value}) is below minimum ({rule['min']})"
                            )
                    
                    if 'max' in rule and isinstance(value, (int, float)):
                        if value > rule['max']:
                            validation_result['warnings'].append(
                                f"Value {config_key} ({value}) is above maximum ({rule['max']})"
                            )
                
                else:
                    # Check if required
                    if 'required' in rule and rule['required']:
                        validation_result['errors'].append(
                            f"Missing required configuration: {config_key}"
                        )
        
        # Overall validation
        validation_result['valid'] = len(validation_result['errors']) == 0
        
        # Add suggestions if valid
        if validation_result['valid']:
            validation_result['suggestions'] = self._generate_suggestions()
        
        return validation_result
    
    def _generate_suggestions(self) -> list:
        """Generate suggestions based on current configuration."""
        suggestions = []
        
        # Check for likely performance issues
        if self.config.has("brain.neuron_count") and self.config.get("brain.neuron_count") > 5000:
            suggestions.append("Consider using neuron_count <= 5000 for better performance")
        
        if self.config.has("plasticity.structural.enable") and self.config.get("plasticity.structural.enable"):
            suggestions.append("Structural plasticity may impact performance significantly")
        
        # Check for likely learning issues
        if self.config.has("neuromod.dopamine.scale") and self.config.get("neuromod.dopamine.scale") > 1.0:
            suggestions.append("High dopamine scale may affect learning stability")
        
        if self.config.has("neuromod.curiosity.enable") and self.config.get("neuromod.curiosity.enable"):
            suggestions.append("Curiosity-driven exploration may increase simulation time")
        
        return suggestions
    
    def get_configuration_summary(self) -> str:
        """Get human-readable configuration summary."""
        summary = "NLM Configuration Summary:\n"
        
        keys = self.config.getKeys()
        
        for key in keys:
            value = self.config.get(key)
            summary += f"  {key}: {value}\n"
        
        return summary
    
    def create_experiment_config(self, experiment_type: str, **kwargs) -> 'ConfigurationManager':
        """Create configuration for specific experiment type.
        
        Args:
            experiment_type: Type of experiment ('development', 'learning', 'memory', 'social')
            **kwargs: Experiment-specific parameters
            
        Returns:
            New ConfigurationManager with experiment configuration
        """
        new_config = ConfigurationManager()
        
        if experiment_type == 'development':
            # Development-focused configuration
            new_config.config.set("brain.neuron_count", 1000)
            new_config.config.set("brain.synapse_density", 0.15)
            new_config.config.set("plasticity.structural.enable", True)
            new_config.config.set("development.enable", True)
            new_config.config.set("neuromod.curiosity.enable", True)
            
            # Override with user parameters
            for key, value in kwargs.items():
                new_config.config.set(key, value)
        
        elif experiment_type == 'learning':
            # Learning-focused configuration
            new_config.config.set("brain.neuron_count", 2000)
            new_config.config.set("brain.synapse_density", 0.2)
            new_config.config.set("plasticity.stdp.enable", True)
            new_config.config.set("plasticity.hebbian.enable", True)
            new_config.config.set("neuromod.dopamine.scale", 1.0)
            new_config.config.set("neuromod.curiosity.enable", True)
            
            # Override with user parameters
            for key, value in kwargs.items():
                new_config.config.set(key, value)
        
        elif experiment_type == 'memory':
            # Memory-focused configuration
            new_config.config.set("brain.neuron_count", 1500)
            new_config.config.set("brain.synapse_density", 0.1)
            new_config.config.set("plasticity.structural.enable", True)
            new_config.config.set("neuromod.novelty.enable", True)
            new_config.config.set("neuromod.curiosity.enable", False)
            
            # Override with user parameters
            for key, value in kwargs.items():
                new_config.config.set(key, value)
        
        elif experiment_type == 'social':
            # Social learning configuration
            new_config.config.set("brain.neuron_count", 3000)
            new_config.config.set("brain.synapse_density", 0.25)
            new_config.config.set("plasticity.stdp.enable", True)
            new_config.config.set("plasticity.hebbian.enable", True)
            new_config.config.set("neuromod.dopamine.scale", 0.8)
            new_config.config.set("neuromod.curiosity.enable", True)
            new_config.config.set("neuromod.novelty.enable", True)
            
            # Override with user parameters
            for key, value in kwargs.items():
                new_config.config.set(key, value)
        
        else:
            raise ValueError(f"Unknown experiment type: {experiment_type}")
        
        return new_config
    
    def clone(self) -> 'ConfigurationManager':
        """Create a copy of the current configuration."""
        new_config = ConfigurationManager()
        
        # Copy all configuration keys
        keys = self.config.getKeys()
        for key in keys:
            new_config.config.set(key, self.config.get(key))
        
        return new_config
    
    def reset_to_default(self):
        """Reset configuration to defaults."""
        self.config = pynlm.createDefaultConfig()
        
        # Save to history
        self.config_history.append({
            'timestamp': datetime.now().isoformat(),
            'action': 'reset_to_default'
        })
        
        print("Configuration reset to defaults")
    
    def get_history(self, limit: int = 10) -> list:
        """Get configuration history.
        
        Args:
            limit: Maximum number of history entries to return
            
        Returns:
            List of history entries
        """
        return self.config_history[-limit:]

# Global configuration manager instance
_global_config = ConfigurationManager()

def get_config_manager() -> ConfigurationManager:
    """Get the global configuration manager instance."""
    return _global_config

def load_config(filepath: str) -> bool:
    """Load configuration from file using global manager.
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        True if successful, False otherwise
    """
    return _global_config.load_from_file(filepath)

def save_config(filepath: str, format: str = 'json') -> bool:
    """Save configuration to file using global manager.
    
    Args:
        filepath: Path to save configuration
        format: File format ('json' or 'yaml')
        
    Returns:
        True if successful, False otherwise
    """
    return _global_config.save_to_file(filepath, format)

def validate_config() -> Dict[str, Any]:
    """Validate configuration using global manager.
    
    Returns:
        Validation results
    """
    return _global_config.validate()

def create_experiment_config(experiment_type: str, **kwargs) -> ConfigurationManager:
    """Create experiment configuration.
    
    Args:
        experiment_type: Type of experiment
        **kwargs: Experiment-specific parameters
        
    Returns:
        ConfigurationManager with experiment configuration
    """
    return _global_config.create_experiment_config(experiment_type, **kwargs)

# Import for datetime
from datetime import datetime
```

### 2. Experiment Configuration Builder

```python
import pynlm
import json
from typing import Dict, Any, List

class ExperimentConfigurationBuilder:
    """Builder class for creating experiment configurations."""
    
    def __init__(self):
        self.config = {}
    
    def with_brain(self, neuron_count: int = 1000, synapse_density: float = 0.1,
                    connection_probability: float = 0.05, **kwargs) -> 'ExperimentConfigurationBuilder':
        """Configure brain parameters."""
        self.config['brain'] = {
            'neuron_count': neuron_count,
            'synapse_density': synapse_density,
            'connection_probability': connection_probability
        }
        
        for key, value in kwargs.items():
            self.config['brain'][key] = value
        
        return self
    
    def with_plasticity(self, stdp: bool = True, hebbian: bool = True,
                         structural: bool = True, **kwargs) -> 'ExperimentConfigurationBuilder':
        """Configure plasticity parameters."""
        self.config['plasticity'] = {
            'stdp': {'enable': stdp},
            'hebbian': {'enable': hebbian},
            'structural': {'enable': structural}
        }
        
        for key, value in kwargs.items():
            if key in self.config['plasticity']:
                if isinstance(self.config['plasticity'][key], dict):
                    self.config['plasticity'][key].update(value)
                else:
                    self.config['plasticity'][key] = value
            else:
                self.config['plasticity'][key] = value
        
        return self
    
    def with_neuromodulation(self, dopamine_scale: float = 1.0,
                              curiosity: bool = True, novelty: bool = True,
                              **kwargs) -> 'ExperimentConfigurationBuilder':
        """Configure neuromodulation parameters."""
        self.config['neuromodulation'] = {
            'dopamine': {'scale': dopamine_scale},
            'curiosity': {'enable': curiosity},
            'novelty': {'enable': novelty}
        }
        
        for key, value in kwargs.items():
            if key in self.config['neuromodulation']:
                if isinstance(self.config['neuromodulation'][key], dict):
                    self.config['neuromodulation'][key].update(value)
                else:
                    self.config['neuromodulation'][key] = value
            else:
                self.config['neuromodulation'][key] = value
        
        return self
    
    def with_development(self, enable: bool = False, critical_period_start: int = 10,
                         critical_period_end: int = 100, **kwargs) -> 'ExperimentConfigurationBuilder':
        """Configure development parameters."""
        self.config['development'] = {
            'enable': enable,
            'stages': {
                'critical_period_start': critical_period_start,
                'critical_period_end': critical_period_end
            }
        }
        
        for key, value in kwargs.items():
            if key in self.config['development']:
                if isinstance(self.config['development'][key], dict):
                    self.config['development'][key].update(value)
                else:
                    self.config['development'][key] = value
            else:
                self.config['development'][key] = value
        
        return self
    
    def with_world(self, width: int = 20, height: int = 20,
                   vision_width: int = 8, vision_height: int = 8,
                   max_energy: float = 100.0, **kwargs) -> 'ExperimentConfigurationBuilder':
        """Configure world parameters."""
        self.config['world'] = {
            'width': width,
            'height': height,
            'vision_width': vision_width,
            'vision_height': vision_height,
            'max_energy': max_energy
        }
        
        for key, value in kwargs.items():
            self.config['world'][key] = value
        
        return self
    
    def with_agent(self, reward_modulation: bool = True,
                   structural_plasticity: bool = True, development: bool = True,
                   curiosity: bool = True, **kwargs) -> 'ExperimentConfigurationBuilder':
        """Configure agent parameters."""
        self.config['agent'] = {
            'enable_reward_modulation': reward_modulation,
            'enable_structural_plasticity': structural_plasticity,
            'enable_development': development,
            'enable_curiosity': curiosity
        }
        
        for key, value in kwargs.items():
            self.config['agent'][key] = value
        
        return self
    
    def with_experimental_parameters(self, learning_rate: float = 0.001,
                                      exploration_rate: float = 0.1,
                                      discount_factor: float = 0.95, **kwargs) -> 'ExperimentConfigurationBuilder':
        """Configure experimental parameters."""
        self.config['experimental'] = {
            'learning_rate': learning_rate,
            'exploration_rate': exploration_rate,
            'discount_factor': discount_factor
        }
        
        for key, value in kwargs.items():
            self.config['experimental'][key] = value
        
        return self
    
    def build(self) -> pynlm.Config:
        """Build the configuration and return it."""
        # Create NLM config
        nlm_config = pynlm.createDefaultConfig()
        
        # Apply configuration
        if 'brain' in self.config:
            for key, value in self.config['brain'].items():
                nlm_config.set(f"brain.{key}", value)
        
        if 'plasticity' in self.config:
            for key, value in self.config['plasticity'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        nlm_config.set(f"plasticity.{key}.{subkey}", subvalue)
                else:
                    nlm_config.set(f"plasticity.{key}", value)
        
        if 'neuromodulation' in self.config:
            for key, value in self.config['neuromodulation'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        nlm_config.set(f"neuromodulation.{key}.{subkey}", subvalue)
                else:
                    nlm_config.set(f"neuromodulation.{key}", value)
        
        if 'development' in self.config:
            for key, value in self.config['development'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        nlm_config.set(f"development.{key}.{subkey}", subvalue)
                else:
                    nlm_config.set(f"development.{key}", value)
        
        if 'world' in self.config:
            for key, value in self.config['world'].items():
                nlm_config.set(f"world.{key}", value)
        
        if 'agent' in self.config:
            for key, value in self.config['agent'].items():
                nlm_config.set(f"agent.{key}", value)
        
        if 'experimental' in self.config:
            for key, value in self.config['experimental'].items():
                nlm_config.set(f"experimental.{key}", value)
        
        return nlm_config
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert builder configuration to dictionary."""
        return self.config.copy()
    
    def from_dict(self, config_dict: Dict[str, Any]) -> 'ExperimentConfigurationBuilder':
        """Load configuration from dictionary."""
        self.config = config_dict.copy()
        return self
    
    def validate(self) -> Dict[str, Any]:
        """Validate configuration."""
        # Basic validation
        validation_result = {
            'valid': True,
            'errors': [],
            'warnings': []
        }
        
        # Check for required sections
        if 'brain' not in self.config:
            validation_result['errors'].append("Missing 'brain' configuration")
            validation_result['valid'] = False
        
        # Validate brain parameters
        if 'brain' in self.config:
            brain_config = self.config['brain']
            if 'neuron_count' in brain_config and brain_config['neuron_count'] < 100:
                validation_result['warnings'].append("neuron_count is below recommended minimum (100)")
            
            if 'synapse_density' in brain_config:
                if brain_config['synapse_density'] < 0 or brain_config['synapse_density'] > 1:
                    validation_result['errors'].append("synapse_density must be between 0 and 1")
                    validation_result['valid'] = False
        
        return validation_result

# Quick configuration presets

def get_development_experiment_config():
    """Get configuration for developmental studies."""
    return ExperimentConfigurationBuilder()\
        .with_brain(neuron_count=1000, synapse_density=0.15)\
        .with_plasticity(structural=True, stdp=True)\
        .with_neuromodulation(curiosity=True, novelty=True)\
        .with_development(enable=True)\
        .build()

def get_learning_experiment_config():
    """Get configuration for learning studies."""
    return ExperimentConfigurationBuilder()\
        .with_brain(neuron_count=2000, synapse_density=0.2)\
        .with_plasticity(stdp=True, hebbian=True)\
        .with_neuromodulation(dopamine_scale=1.0, curiosity=True)\
        .with_experimental_parameters(learning_rate=0.001, exploration_rate=0.1)\
        .build()

def get_memory_experiment_config():
    """Get configuration for memory studies."""
    return ExperimentConfigurationBuilder()\
        .with_brain(neuron_count=1500, synapse_density=0.1)\
        .with_plasticity(structural=True)\
        .with_neuromodulation(novelty=True)\
        .build()

def get_social_experiment_config():
    """Get configuration for social learning studies."""
    return ExperimentConfigurationBuilder()\
        .with_brain(neuron_count=3000, synapse_density=0.25)\
        .with_plasticity(stdp=True, hebbian=True)\
        .with_neuromodulation(dopamine_scale=0.8, curiosity=True, novelty=True)\
        .build()

def get_quick_test_config():
    """Get configuration for quick testing."""
    return ExperimentConfigurationBuilder()\
        .with_brain(neuron_count=100, synapse_density=0.05)\
        .with_plasticity(stdp=True, hebbian=True, structural=False)\
        .with_neuromodulation(curiosity=False, novelty=False)\
        .with_development(enable=False)\
        .build()
```

### 3. Configuration Compare and Merge Utilities

```python
import pynlm
from typing import Dict, Any, List, Tuple

class ConfigurationComparator:
    """Compare and merge configurations."""
    
    @staticmethod
    def compare_configs(config1: pynlm.Config, config2: pynlm.Config) -> Dict[str, Any]:
        """Compare two configurations.
        
        Args:
            config1: First configuration
            config2: Second configuration
            
        Returns:
            Comparison results
        """
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        
        all_keys = set(keys1) | set(keys2)
        
        differences = []
        similarities = []
        
        for key in sorted(all_keys):
            if key in keys1 and key in keys2:
                value1 = config1.get(key)
                value2 = config2.get(key)
                
                if value1 == value2:
                    similarities.append(key)
                else:
                    differences.append({
                        'key': key,
                        'config1_value': value1,
                        'config2_value': value2,
                        'difference': f"{value1} -> {value2}"
                    })
            elif key in keys1:
                differences.append({
                    'key': key,
                    'config1_value': config1.get(key),
                    'config2_value': '<missing>',
                    'difference': 'Added'
                })
            else:
                differences.append({
                    'key': key,
                    'config1_value': '<missing>',
                    'config2_value': config2.get(key),
                    'difference': 'Removed'
                })
        
        return {
            'similarities': similarities,
            'differences': differences,
            'summary': {
                'total_keys': len(all_keys),
                'similar_keys': len(similarities),
                'different_keys': len(differences),
                'similarity_percentage': len(similarities) / len(all_keys) * 100 if all_keys else 0
            }
        }
    
    @staticmethod
    def merge_configs(configs: List[pynlm.Config]) -> pynlm.Config:
        """Merge multiple configurations.
        
        Args:
            configs: List of configurations to merge
            
        Returns:
            Merged configuration
        """
        if not configs:
            return pynlm.createDefaultConfig()
        
        merged_config = pynlm.createDefaultConfig()
        
        # Start with first config
        first_config = configs[0]
        for key in first_config.getKeys():
            merged_config.set(key, first_config.get(key))
        
        # Merge subsequent configs
        for config in configs[1:]:
            for key in config.getKeys():
                merged_config.set(key, config.get(key))
        
        return merged_config
    
    @staticmethod
    def find_differences(config1: pynlm.Config, config2: pynlm.Config, 
                        threshold: float = 0.1) -> Dict[str, Any]:
        """Find significant differences between configurations.
        
        Args:
            config1: First configuration
            config2: Second configuration
            threshold: Threshold for considering a difference significant
            
        Returns:
            Dictionary of significant differences
        """
        differences = ConfigurationComparator.compare_configs(config1, config2)
        
        significant_differences = []
        
        for diff in differences['differences']:
            if diff['difference'] in ['Added', 'Removed']:
                significant_differences.append(diff)
            elif '->' in diff['difference']:
                # Try to quantify the difference
                try:
                    value1 = float(diff['config1_value'])
                    value2 = float(diff['config2_value'])
                    
                    relative_diff = abs(value2 - value1) / max(abs(value1), 1e-10)
                    
                    if relative_diff > threshold:
                        diff['relative_difference'] = relative_diff
                        significant_differences.append(diff)
                except (ValueError, TypeError):
                    # Non-numeric difference, still consider it significant
                    significant_differences.append(diff)
        
        return {
            'significant_differences': significant_differences,
            'total_differences': len(differences['differences']),
            'significant_count': len(significant_differences)
        }
    
    @staticmethod
    def create_configuration_family(base_config: pynlm.Config, 
                                   variations: List[Dict[str, Any]]) -> List[pynlm.Config]:
        """Create a family of related configurations.
        
        Args:
            base_config: Base configuration
            variations: List of configuration variations
            
        Returns:
            List of related configurations
        """
        config_family = [base_config]
        
        for variation in variations:
            variant_config = ConfigurationComparator.merge_configs([base_config])
            
            # Apply variation
            for key, value in variation.items():
                if isinstance(value, dict):
                    # Nested configuration
                    for subkey, subvalue in value.items():
                        variant_config.set(f"{key}.{subkey}", subvalue)
                else:
                    variant_config.set(key, value)
            
            config_family.append(variant_config)
        
        return config_family

class ConfigurationValidator:
    """Validate configuration for specific use cases."""
    
    @staticmethod
    def validate_for_simulation(config: pynlm.Config, 
                               world_size: Tuple[int, int] = (20, 20)) -> Dict[str, Any]:
        """Validate configuration for simulation.
        
        Args:
            config: Configuration to validate
            world_size: World dimensions
            
        Returns:
            Validation results
        """
        results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': []
        }
        
        width, height = world_size
        
        # Validate brain parameters
        if config.has("brain.neuron_count"):
            neuron_count = config.get("brain.neuron_count")
            if neuron_count < 100:
                results['warnings'].append("Neuron count is very low, may not produce interesting behavior")
            
            if neuron_count > 5000:
                results['warnings'].append("High neuron count may impact performance significantly")
        
        # Validate world parameters
        if config.has("world.width") and config.get("world.width") != width:
            results['warnings'].append(f"World width ({config.get('world.width')}) differs from specified ({width})")
        
        if config.has("world.height") and config.get("world.height") != height:
            results['warnings'].append(f"World height ({config.get('world.height')}) differs from specified ({height})")
        
        # Validate sensory parameters
        if config.has("world.vision_width") and config.get("world.vision_width") > width:
            results['errors'].append(f"Vision width ({config.get('world.vision_width')}) exceeds world width ({width})")
            results['valid'] = False
        
        if config.has("world.vision_height") and config.get("world.vision_height") > height:
            results['errors'].append(f"Vision height ({config.get('world.vision_height')}) exceeds world height ({height})")
            results['valid'] = False
        
        # Validate neuromodulation parameters
        if config.has("neuromod.dopamine.scale"):
            dopamine_scale = config.get("neuromod.dopamine.scale")
            if dopamine_scale < 0:
                results['errors'].append("Dopamine scale cannot be negative")
                results['valid'] = False
        
        # Validate development parameters
        if config.has("development.enable") and config.get("development.enable"):
            if not config.has("development.stages.critical_period_start"):
                results['warnings'].append("Development enabled but critical period not configured")
        
        # Performance suggestions
        if config.has("brain.neuron_count") and config.get("brain.neuron_count") > 2000:
            results['suggestions'].append("Consider reducing neuron count for better performance")
        
        if config.has("plasticity.structural.enable") and config.get("plasticity.structural.enable"):
            results['suggestions'].append("Structural plasticity may significantly impact performance")
        
        return results
    
    @staticmethod
    def validate_for_experiment(config: pynlm.Config, 
                                experiment_type: str = "general") -> Dict[str, Any]:
        """Validate configuration for specific experiment type.
        
        Args:
            config: Configuration to validate
            experiment_type: Type of experiment
            
        Returns:
            Validation results
        """
        results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': []
        }
        
        if experiment_type == "development":
            # Development experiment validation
            if not config.has("development.enable") or not config.get("development.enable"):
                results['warnings'].append("Development experiments typically require development enabled")
            
            if not config.has("neuromod.curiosity.enable") or not config.get("neuromod.curiosity.enable"):
                results['warnings'].append("Curiosity-driven exploration is important for development studies")
        
        elif experiment_type == "learning":
            # Learning experiment validation
            if not config.has("plasticity.stdp.enable") or not config.get("plasticity.stdp.enable"):
                results['warnings'].append("STDP is important for learning experiments")
            
            if not config.has("neuromod.dopamine.scale"):
                results['warnings'].append("Dopamine scale should be configured for learning experiments")
        
        elif experiment_type == "memory":
            # Memory experiment validation
            if not config.has("plasticity.structural.enable") or not config.get("plasticity.structural.enable"):
                results['warnings'].append("Structural plasticity is important for memory experiments")
            
            if not config.has("neuromod.novelty.enable") or not config.get("neuromod.novelty.enable"):
                results['warnings'].append("Novelty detection is important for memory experiments")
        
        elif experiment_type == "social":
            # Social experiment validation
            if not config.has("plasticity.hebbian.enable") or not config.get("plasticity.hebbian.enable"):
                results['warnings'].append("Hebbian learning is important for social experiments")
        
        return results

# Utility functions for configuration management

def export_config_to_file(config: pynlm.Config, filepath: str, format: str = 'json'):
    """Export configuration to file.
    
    Args:
        config: Configuration to export
        filepath: Path to save configuration
        format: File format ('json' or 'yaml')
    """
    try:
        import json
        from pathlib import Path
        
        # Convert config to dictionary
        config_dict = {}
        keys = config.getKeys()
        
        for key in keys:
            value = config.get(key)
            
            # Group by section
            if key.startswith('brain.'):
                if 'brain' not in config_dict:
                    config_dict['brain'] = {}
                config_dict['brain'][key.split('.')[1]] = value
            
            elif key.startswith('plasticity.'):
                if 'plasticity' not in config_dict:
                    config_dict['plasticity'] = {}
                
                parts = key.split('.')
                if len(parts) > 2:
                    if parts[1] not in config_dict['plasticity']:
                        config_dict['plasticity'][parts[1]] = {}
                    config_dict['plasticity'][parts[1]][parts[2]] = value
                else:
                    config_dict['plasticity'][parts[1]] = value
            
            elif key.startswith('neuromodulation.'):
                if 'neuromodulation' not in config_dict:
                    config_dict['neuromodulation'] = {}
                
                parts = key.split('.')
                if len(parts) > 2:
                    if parts[1] not in config_dict['neuromodulation']:
                        config_dict['neuromodulation'][parts[1]] = {}
                    if parts[2] not in config_dict['neuromodulation'][parts[1]]:
                        config_dict['neuromodulation'][parts[1]][parts[2]] = {}
                    config_dict['neuromodulation'][parts[1]][parts[2]][parts[3]] = value
                else:
                    config_dict['neuromodulation'][parts[1]] = value
            
            elif key.startswith('development.'):
                if 'development' not in config_dict:
                    config_dict['development'] = {}
                
                parts = key.split('.')
                if len(parts) > 2:
                    if parts[1] not in config_dict['development']:
                        config_dict['development'][parts[1]] = {}
                    config_dict['development'][parts[1]][parts[2]] = value
                else:
                    config_dict['development'][parts[1]] = value
            
            elif key.startswith('world.'):
                if 'world' not in config_dict:
                    config_dict['world'] = {}
                config_dict['world'][key.split('.')[1]] = value
            
            else:
                config_dict[key] = value
        
        path = Path(filepath)
        
        if format.lower() == 'json':
            with open(path, 'w') as f:
                json.dump(config_dict, f, indent=2)
        elif format.lower() in ['yaml', 'yml']:
            import yaml
            with open(path, 'w') as f:
                yaml.dump(config_dict, f, default_flow_style=False)
        
        print(f"Configuration exported to {filepath}")
        
    except Exception as e:
        print(f"Error exporting configuration: {e}")
        raise

def import_config_from_file(filepath: str) -> pynlm.Config:
    """Import configuration from file.
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        Imported configuration
    """
    try:
        import json
        from pathlib import Path
        
        path = Path(filepath)
        
        if path.suffix.lower() == '.json':
            with open(path, 'r') as f:
                config_dict = json.load(f)
        elif path.suffix.lower() in ['.yaml', '.yml']:
            import yaml
            with open(path, 'r') as f:
                config_dict = yaml.safe_load(f)
        else:
            raise ValueError(f"Unsupported file format: {path.suffix}")
        
        # Create NLM config
        config = pynlm.createDefaultConfig()
        
        # Apply configuration
        if 'brain' in config_dict:
            for key, value in config_dict['brain'].items():
                config.set(f"brain.{key}", value)
        
        if 'plasticity' in config_dict:
            for key, value in config_dict['plasticity'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        config.set(f"plasticity.{key}.{subkey}", subvalue)
                else:
                    config.set(f"plasticity.{key}", value)
        
        if 'neuromodulation' in config_dict:
            for key, value in config_dict['neuromodulation'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        config.set(f"neuromodulation.{key}.{subkey}", subvalue)
                else:
                    config.set(f"neuromodulation.{key}", value)
        
        if 'development' in config_dict:
            for key, value in config_dict['development'].items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        config.set(f"development.{key}.{subkey}", subvalue)
                else:
                    config.set(f"development.{key}", value)
        
        if 'world' in config_dict:
            for key, value in config_dict['world'].items():
                config.set(f"world.{key}", value)
        
        print(f"Configuration imported from {filepath}")
        return config
        
    except Exception as e:
        print(f"Error importing configuration: {e}")
        raise

def create_configuration_preset(name: str, config: pynlm.Config, description: str = ""):
    """Create a named configuration preset.
    
    Args:
        name: Preset name
        config: Configuration to save
        description: Preset description
    """
    presets_dir = Path("config_presets")
    presets_dir.mkdir(exist_ok=True)
    
    preset_file = presets_dir / f"{name}.json"
    
    # Convert config to dictionary
    config_dict = {}
    keys = config.getKeys()
    
    for key in keys:
        value = config.get(key)
        
        # Group by section
        if key.startswith('brain.'):
            if 'brain' not in config_dict:
                config_dict['brain'] = {}
            config_dict['brain'][key.split('.')[1]] = value
        
        elif key.startswith('plasticity.'):
            if 'plasticity' not in config_dict:
                config_dict['plasticity'] = {}
            
            parts = key.split('.')
            if len(parts) > 2:
                if parts[1] not in config_dict['plasticity']:
                    config_dict['plasticity'][parts[1]] = {}
                config_dict['plasticity'][parts[1]][parts[2]] = value
            else:
                config_dict['plasticity'][parts[1]] = value
        
        elif key.startswith('neuromodulation.'):
            if 'neuromodulation' not in config_dict:
                config_dict['neuromodulation'] = {}
            
            parts = key.split('.')
            if len(parts) > 2:
                if parts[1] not in config_dict['neuromodulation']:
                    config_dict['neuromodulation'][parts[1]] = {}
                if parts[2] not in config_dict['neuromodulation'][parts[1]]:
                    config_dict['neuromodulation'][parts[1]][parts[2]] = {}
                config_dict['neuromodulation'][parts[1]][parts[2]][parts[3]] = value
            else:
                config_dict['neuromodulation'][parts[1]] = value
        
        elif key.startswith('development.'):
            if 'development' not in config_dict:
                config_dict['development'] = {}
            
            parts = key.split('.')
            if len(parts) > 2:
                if parts[1] not in config_dict['development']:
                    config_dict['development'][parts[1]] = {}
                config_dict['development'][parts[1]][parts[2]] = value
            else:
                config_dict['development'][parts[1]] = value
        
        elif key.startswith('world.'):
            if 'world' not in config_dict:
                config_dict['world'] = {}
            config_dict['world'][key.split('.')[1]] = value
        
        else:
            config_dict[key] = value
    
    # Save preset
    import json
    preset_data = {
        'name': name,
        'description': description,
        'timestamp': datetime.now().isoformat(),
        'config': config_dict
    }
    
    with open(preset_file, 'w') as f:
        json.dump(preset_data, f, indent=2)
    
    print(f"Configuration preset saved: {name}")

def load_configuration_preset(name: str) -> pynlm.Config:
    """Load configuration preset.
    
    Args:
        name: Preset name
        
    Returns:
        Loaded configuration
    """
    preset_file = Path("config_presets") / f"{name}.json"
    
    try:
        import json
        with open(preset_file, 'r') as f:
            preset_data = json.load(f)
        
        # Create NLM config
        config = pynlm.createDefaultConfig()
        
        # Apply configuration
        if 'config' in preset_data:
            config_dict = preset_data['config']
            
            if 'brain' in config_dict:
                for key, value in config_dict['brain'].items():
                    config.set(f"brain.{key}", value)
            
            if 'plasticity' in config_dict:
                for key, value in config_dict['plasticity'].items():
                    if isinstance(value, dict):
                        for subkey, subvalue in value.items():
                            config.set(f"plasticity.{key}.{subkey}", subvalue)
                    else:
                        config.set(f"plasticity.{key}", value)
            
            if 'neuromodulation' in config_dict:
                for key, value in config_dict['neuromodulation'].items():
                    if isinstance(value, dict):
                        for subkey, subvalue in value.items():
                            config.set(f"neuromodulation.{key}.{subkey}", subvalue)
                    else:
                        config.set(f"neuromodulation.{key}", value)
            
            if 'development' in config_dict:
                for key, value in config_dict['development'].items():
                    if isinstance(value, dict):
                        for subkey, subvalue in value.items():
                            config.set(f"development.{key}.{subkey}", subvalue)
                    else:
                        config.set(f"development.{key}", value)
            
            if 'world' in config_dict:
                for key, value in config_dict['world'].items():
                    config.set(f"world.{key}", value)
        
        print(f"Configuration preset loaded: {name}")
        return config
        
    except Exception as e:
        print(f"Error loading configuration preset: {e}")
        raise

# Import for datetime
from datetime import datetime
```

This provides comprehensive configuration management utilities with:

1. **ConfigurationManager**: Main configuration management class
2. **ExperimentConfigurationBuilder**: Fluent API for building configurations
3. **ConfigurationComparator**: Compare and merge configurations
4. **ConfigurationValidator**: Validate configurations for specific use cases
5. **Preset Management**: Save and load configuration presets
6. **Export/Import**: Convert configurations to/from files

These utilities make it much easier to manage NLM configurations, validate them for specific use cases, and share configurations with others.