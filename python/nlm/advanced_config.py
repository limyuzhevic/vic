"""
Advanced configuration management for NLM

This module provides enhanced configuration capabilities for NLM (Neural Learning Machine)
including validation, schema management, and advanced parameter handling.

Key Features:
- Configuration validation and schema checking
- Parameter groups and categories
- Configuration templates
- Configuration comparison
- Configuration versioning
- Advanced parameter types and validation rules
"""

import json
import yaml
import hashlib
import time
from typing import Dict, Any, List, Optional, Union
from pathlib import Path
from dataclasses import dataclass, asdict
from enum import Enum


class ValidationLevel(Enum):
    """Configuration validation levels."""
    STRICT = "strict"
    LOOSY = "loosy"
    NONE = "none"


class ConfigFormat(Enum):
    """Configuration file formats."""
    JSON = "json"
    YAML = "yaml"
    TOML = "toml"


@dataclass
class ConfigSchema:
    """Configuration schema definition."""
    name: str
    version: str
    description: str
    parameters: Dict[str, Dict[str, Any]]
    validation_rules: Optional[Dict[str, Any]] = None
    groups: Optional[Dict[str, List[str]]] = None


class ConfigManager:
    """Advanced configuration management with validation and schema support."""
    
    def __init__(self):
        self.config = {}
        self.schema = None
        self.version = "1.0"
        self.validation_level = ValidationLevel.STRICT
        self.history = []
    
    def load_config(self, filepath: str, format: Optional[ConfigFormat] = None) -> bool:
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            format: File format (auto-detected if None)
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if format is None:
                format = self._detect_format(filepath)
            
            if format == ConfigFormat.JSON:
                with open(filepath, 'r') as f:
                    self.config = json.load(f)
            elif format == ConfigFormat.YAML:
                with open(filepath, 'r') as f:
                    import yaml
                    self.config = yaml.safe_load(f)
            elif format == ConfigFormat.TOML:
                with open(filepath, 'r') as f:
                    import toml
                    self.config = toml.load(f)
            
            self._validate_config()
            self._add_to_history("load", filepath)
            return True
            
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
    
    def save_config(self, filepath: str, format: Optional[ConfigFormat] = None) -> bool:
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration
            format: File format (auto-detected if None)
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if format is None:
                format = self._detect_format(filepath)
            
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            if format == ConfigFormat.JSON:
                with open(filepath, 'w') as f:
                    json.dump(self.config, f, indent=2)
            elif format == ConfigFormat.YAML:
                with open(filepath, 'w') as f:
                    import yaml
                    yaml.dump(self.config, f, default_flow_style=False)
            elif format == ConfigFormat.TOML:
                with open(filepath, 'w') as f:
                    import toml
                    toml.dump(self.config, f)
            
            self._add_to_history("save", filepath)
            return True
            
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def set_schema(self, schema: ConfigSchema):
        """Set configuration schema for validation."""
        self.schema = schema
    
    def validate_config(self, level: Optional[ValidationLevel] = None) -> List[str]:
        """
        Validate current configuration.
        
        Args:
            level: Validation level (uses instance default if None)
            
        Returns:
            List of validation errors (empty if valid)
        """
        if level is None:
            level = self.validation_level
        
        errors = []
        
        if level == ValidationLevel.STRICT and self.schema:
            errors.extend(self._validate_strict())
        elif level == ValidationLevel.LOOSY and self.schema:
            errors.extend(self._validate_loosy())
        
        return errors
    
    def compare_configs(self, other_config: 'ConfigManager') -> Dict[str, Any]:
        """
        Compare two configurations.
        
        Args:
            other_config: Configuration to compare with
            
        Returns:
            Dictionary with comparison results
        """
        diff = {
            'added': {},
            'removed': {},
            'modified': {},
            'common': {}
        }
        
        all_keys = set(self.config.keys()) | set(other_config.config.keys())
        
        for key in all_keys:
            if key in self.config and key in other_config.config:
                if self.config[key] != other_config.config[key]:
                    diff['modified'][key] = {
                        'old': other_config.config[key],
                        'new': self.config[key]
                    }
                else:
                    diff['common'][key] = self.config[key]
            elif key in self.config:
                diff['added'][key] = self.config[key]
            else:
                diff['removed'][key] = other_config.config[key]
        
        return diff
    
    def get_parameter(self, key: str, default: Any = None) -> Any:
        """Get parameter value with support for nested keys."""
        keys = key.split('.')
        value = self.config
        
        for k in keys:
            if isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default
        
        return value
    
    def set_parameter(self, key: str, value: Any):
        """Set parameter value with support for nested keys."""
        keys = key.split('.')
        target = self.config
        
        for i, k in enumerate(keys[:-1]):
            if k not in target:
                target[k] = {}
            elif not isinstance(target[k], dict):
                raise ValueError(f"Cannot set nested property on non-dict key: {k}")
            target = target[k]
        
        target[keys[-1]] = value
    
    def remove_parameter(self, key: str) -> bool:
        """Remove parameter."""
        try:
            keys = key.split('.')
            target = self.config
            
            for i, k in enumerate(keys[:-1]):
                if isinstance(target, dict) and k in target:
                    target = target[k]
                else:
                    return False
            
            if keys[-1] in target:
                del target[keys[-1]]
                return True
            return False
        except Exception:
            return False
    
    def get_all_parameters(self) -> Dict[str, Any]:
        """Get all parameters as a flattened dictionary."""
        return self.config.copy()
    
    def import_from_dict(self, config_dict: Dict[str, Any]):
        """Import configuration from dictionary."""
        self.config.update(config_dict)
        self._validate_config()
    
    def export_to_dict(self) -> Dict[str, Any]:
        """Export configuration to dictionary."""
        return self.config.copy()
    
    def get_version(self) -> str:
        """Get configuration version."""
        return self.version
    
    def set_version(self, version: str):
        """Set configuration version."""
        self.version = version
    
    def reset(self):
        """Reset to default configuration."""
        self.config = {}
        self.history.clear()
    
    def _detect_format(self, filepath: str) -> ConfigFormat:
        """Detect file format from extension."""
        ext = Path(filepath).suffix.lower()
        
        if ext == '.json':
            return ConfigFormat.JSON
        elif ext in ['.yaml', '.yml']:
            return ConfigFormat.YAML
        elif ext == '.toml':
            return ConfigFormat.TOML
        else:
            raise ValueError(f"Unsupported file format: {ext}")
    
    def _validate_strict(self) -> List[str]:
        """Strictly validate against schema."""
        errors = []
        
        if not self.schema:
            return errors
        
        # Check required parameters
        for param_name, param_def in self.schema.parameters.items():
            if param_def.get('required', False) and param_name not in self.config:
                errors.append(f"Missing required parameter: {param_name}")
        
        # Check parameter types
        for key, value in self.config.items():
            if key in self.schema.parameters:
                expected_type = self.schema.parameters[key].get('type')
                if expected_type and not self._check_type(value, expected_type):
                    errors.append(f"Invalid type for parameter {key}: expected {expected_type}, got {type(value).__name__}")
        
        return errors
    
    def _validate_loosy(self) -> List[str]:
        """Loosely validate configuration."""
        errors = []
        
        # Basic structure validation
        if not isinstance(self.config, dict):
            errors.append("Configuration must be a dictionary")
            return errors
        
        # Check for invalid values
        for key, value in self.config.items():
            if isinstance(value, dict):
                # Recursively check nested dictionaries
                for nested_key, nested_value in value.items():
                    if not self._is_valid_value(nested_value):
                        errors.append(f"Invalid value for nested parameter {key}.{nested_key}: {nested_value}")
            else:
                if not self._is_valid_value(value):
                    errors.append(f"Invalid value for parameter {key}: {value}")
        
        return errors
    
    def _check_type(self, value: Any, expected_type: str) -> bool:
        """Check if value matches expected type."""
        type_map = {
            'int': int,
            'float': float,
            'bool': bool,
            'str': str,
            'list': list,
            'dict': dict,
        }
        
        if expected_type in type_map:
            return isinstance(value, type_map[expected_type])
        
        return True
    
    def _is_valid_value(self, value: Any) -> bool:
        """Check if value is valid."""
        if isinstance(value, (str, int, float, bool, type(None))):
            return True
        elif isinstance(value, (list, dict)):
            return all(self._is_valid_value(v) for v in value)
        else:
            return False
    
    def _validate_config(self):
        """Validate configuration after changes."""
        errors = self.validate_config()
        if errors:
            print(f"Configuration validation errors: {errors}")
    
    def _add_to_history(self, action: str, details: str):
        """Add action to history."""
        history_entry = {
            'timestamp': time.time(),
            'action': action,
            'details': details
        }
        self.history.append(history_entry)


class AdvancedExperimentRunner:
    """Advanced experiment control and management."""
    
    def __init__(self):
        self.experiments = {}
        self.results = []
    
    def create_experiment(self, name: str, description: str, config: Dict[str, Any]) -> str:
        """
        Create a new experiment.
        
        Args:
            name: Experiment name
            description: Experiment description
            config: Experiment configuration
            
        Returns:
            Experiment ID
        """
        import uuid
        experiment_id = str(uuid.uuid4())
        
        self.experiments[experiment_id] = {
            'name': name,
            'description': description,
            'config': config,
            'created_at': time.time(),
            'status': 'pending'
        }
        
        return experiment_id
    
    def run_experiment(self, experiment_id: str, steps: int = 1000) -> Dict[str, Any]:
        """
        Run an experiment.
        
        Args:
            experiment_id: Experiment ID
            steps: Number of simulation steps
            
        Returns:
            Experiment results
        """
        if experiment_id not in self.experiments:
            raise ValueError(f"Experiment not found: {experiment_id}")
        
        experiment = self.experiments[experiment_id]
        experiment['status'] = 'running'
        
        try:
            # Import here to avoid circular imports
            from nlm import createDefaultConfig, createBrain
            
            # Create configuration
            config = createDefaultConfig()
            config.set("neuron_count", experiment['config'].get('neuron_count', 1000))
            config.set("connection_probability", experiment['config'].get('connection_probability', 0.1))
            
            # Create brain
            brain = createBrain(config)
            brain.initialize()
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            # Collect results
            result = {
                'experiment_id': experiment_id,
                'status': 'completed',
                'start_time': experiment['created_at'],
                'end_time': end_time,
                'total_time': end_time - start_time,
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
                'config': experiment['config'],
                'steps': steps
            }
            
            self.results.append(result)
            experiment['status'] = 'completed'
            
            return result
            
        except Exception as e:
            experiment['status'] = 'failed'
            error_result = {
                'experiment_id': experiment_id,
                'status': 'failed',
                'error': str(e),
                'start_time': experiment['created_at'],
                'end_time': time.time(),
                'config': experiment['config']
            }
            self.results.append(error_result)
            raise
    
    def get_experiment_results(self, experiment_id: str) -> Dict[str, Any]:
        """Get experiment results."""
        for result in self.results:
            if result['experiment_id'] == experiment_id:
                return result
        raise ValueError(f"Experiment results not found: {experiment_id}")
    
    def list_experiments(self) -> List[Dict[str, Any]]:
        """List all experiments."""
        return list(self.experiments.values())
    
    def compare_experiments(self, experiment_ids: List[str]) -> Dict[str, Any]:
        """
        Compare multiple experiments.
        
        Args:
            experiment_ids: List of experiment IDs to compare
            
        Returns:
            Comparison results
        """
        experiments_data = []
        for exp_id in experiment_ids:
            try:
                result = self.get_experiment_results(exp_id)
                experiments_data.append(result)
            except ValueError:
                pass
        
        if len(experiments_data) < 2:
            raise ValueError("Need at least 2 experiments to compare")
        
        comparison = {
            'experiments': experiments_data,
            'metrics': self._extract_metrics(experiments_data),
            'summary': self._generate_summary(experiments_data)
        }
        
        return comparison
    
    def _extract_metrics(self, experiments_data: List[Dict[str, Any]]) -> Dict[str, List[Any]]:
        """Extract metrics from experiment data."""
        metrics = {}
        
        if experiments_data:
            metrics['neuron_counts'] = [e.get('neuron_count', 0) for e in experiments_data]
            metrics['steps_per_second'] = [e.get('total_time', 1) / e.get('steps', 1) 
                                          for e in experiments_data]
            metrics['spikes'] = [e.get('spikes', 0) for e in experiments_data]
            metrics['firing_rates'] = [e.get('firing_rate', 0) for e in experiments_data]
            metrics['exc_inh_ratios'] = [e.get('exc_inh_ratio', 0) for e in experiments_data]
        
        return metrics
    
    def _generate_summary(self, experiments_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Generate summary of experiment comparison."""
        if not experiments_data:
            return {}
        
        summary = {
            'num_experiments': len(experiments_data),
            'best_performance': None,
            'average_neuron_count': sum(e.get('neuron_count', 0) for e in experiments_data) / len(experiments_data),
            'average_firing_rate': sum(e.get('firing_rate', 0) for e in experiments_data) / len(experiments_data),
        }
        
        # Find best performing experiment (highest firing rate)
        best_idx = max(range(len(experiments_data)), 
                      key=lambda i: experiments_data[i].get('firing_rate', 0))
        summary['best_performance'] = experiments_data[best_idx]['experiment_id']
        
        return summary


class ConfigTemplateManager:
    """Manage configuration templates."""
    
    def __init__(self):
        self.templates = {}
    
    def add_template(self, name: str, description: str, config: Dict[str, Any]):
        """Add a configuration template."""
        self.templates[name] = {
            'description': description,
            'config': config,
            'created_at': time.time()
        }
    
    def get_template(self, name: str) -> Optional[Dict[str, Any]]:
        """Get template by name."""
        return self.templates.get(name)
    
    def list_templates(self) -> List[str]:
        """List all template names."""
        return list(self.templates.keys())
    
    def apply_template(self, name: str, config: ConfigManager) -> bool:
        """Apply template to configuration manager."""
        if name not in self.templates:
            return False
        
        template = self.templates[name]
        config.import_from_dict(template['config'])
        return True


# Global instances for backward compatibility
_global_config_manager = ConfigManager()
_global_experiment_runner = AdvancedExperimentRunner()
_global_template_manager = ConfigTemplateManager()

# Backward compatibility functions
def load_config(filepath: str) -> bool:
    """Load configuration using global config manager."""
    return _global_config_manager.load_config(filepath)

def save_config(filepath: str) -> bool:
    """Save configuration using global config manager."""
    return _global_config_manager.save_config(filepath)

def get_parameter(key: str, default: Any = None) -> Any:
    """Get parameter using global config manager."""
    return _global_config_manager.get_parameter(key, default)

def set_parameter(key: str, value: Any):
    """Set parameter using global config manager."""
    _global_config_manager.set_parameter(key, value)

def create_advanced_experiment(name: str, description: str, config: Dict[str, Any]) -> str:
    """Create advanced experiment using global experiment runner."""
    return _global_experiment_runner.create_experiment(name, description, config)

def run_advanced_experiment(experiment_id: str, steps: int = 1000) -> Dict[str, Any]:
    """Run advanced experiment using global experiment runner."""
    return _global_experiment_runner.run_experiment(experiment_id, steps)

def get_experiment_results(experiment_id: str) -> Dict[str, Any]:
    """Get experiment results using global experiment runner."""
    return _global_experiment_runner.get_experiment_results(experiment_id)

def apply_template(template_name: str, config: ConfigManager) -> bool:
    """Apply template using global template manager."""
    return _global_template_manager.apply_template(template_name, config)


if __name__ == "__main__":
    # Example usage
    print("NLM Advanced Configuration Management")
    print("=====================================")
    
    # Create config manager
    config = ConfigManager()
    config.set_version("1.0")
    
    # Set some parameters
    config.set_parameter("brain.neuron_count", 1000)
    config.set_parameter("brain.connection_probability", 0.1)
    config.set_parameter("plasticity.enable", True)
    
    # Save configuration
    config.save_config("test_config.json")
    print("Configuration saved!")
    
    # Load configuration
    if load_config("test_config.json"):
        print("Configuration loaded!")
        print(f"Neuron count: {get_parameter('brain.neuron_count')}")
    
    # Create experiment
    exp_id = create_advanced_experiment(
        "Test Experiment",
        "A simple test experiment",
        {"neuron_count": 500, "plasticity_enabled": True}
    )
    print(f"Created experiment: {exp_id}")
    
    # Run experiment
    try:
        result = run_advanced_experiment(exp_id, 100)
        print(f"Experiment completed: {result}")
    except Exception as e:
        print(f"Experiment failed: {e}")
    
    # Compare experiments
    try:
        if len(_global_experiment_runner.results) >= 2:
            comparison = _global_experiment_runner.compare_experiments(
                [r['experiment_id'] for r in _global_experiment_runner.results]
            )
            print(f"Comparison: {comparison}")
    except Exception as e:
        print(f"Comparison failed: {e}")