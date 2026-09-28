# Configuration Validation and Management

This document provides comprehensive tools for configuration validation, management, and optimization in NLM.

## Overview

Configuration validation and management are critical for ensuring reliable and optimal operation of NLM systems. This document provides:

1. **Configuration validation tools** for detecting and correcting configuration errors
2. **Configuration management** for creating, storing, and comparing configurations
3. **Optimization tools** for finding optimal parameter combinations
4. **Version control** for managing configuration changes
5. **Documentation** for maintaining configuration knowledge

## Configuration Validation

### Configuration Validator

```python
import pynlm
import json
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
import numpy as np

class ConfigurationValidator:
    """Comprehensive configuration validation for NLM systems."""
    
    def __init__(self):
        self.validation_rules = self._load_validation_rules()
        self.error_categories = {
            'critical': [],
            'warning': [],
            'info': []
        }
    
    def _load_validation_rules(self) -> Dict[str, Any]:
        """Load validation rules from configuration file."""
        rules = {
            'brain_parameters': {
                'neuron_count': {
                    'type': 'positive_integer',
                    'min': 10,
                    'max': 100000,
                    'recommended_range': (100, 5000),
                    'critical': True
                },
                'region_count': {
                    'type': 'positive_integer',
                    'min': 1,
                    'max': 1000,
                    'recommended_range': (2, 20),
                    'critical': True
                },
                'connection_probability': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 1.0,
                    'recommended_range': (0.05, 0.2),
                    'critical': False
                },
                'initial_weight_mean': {
                    'type': 'float',
                    'min': -1.0,
                    'max': 1.0,
                    'recommended_range': (-0.5, 0.5),
                    'critical': False
                },
                'initial_weight_std': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 1.0,
                    'recommended_range': (0.1, 0.3),
                    'critical': False
                }
            },
            'plasticity_parameters': {
                'plasticity_stdp_enable': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'plasticity_stdp_learning_rate': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 1.0,
                    'recommended_range': (0.001, 0.01),
                    'critical': False
                },
                'plasticity_hebbian_enable': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'plasticity_hebbian_learning_rate': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 1.0,
                    'recommended_range': (0.001, 0.01),
                    'critical': False
                },
                'plasticity_structural_enable': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'synaptogenesis_rate': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 0.1,
                    'recommended_range': (0.0001, 0.01),
                    'critical': False
                },
                'pruning_rate': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 0.01,
                    'recommended_range': (0.00001, 0.001),
                    'critical': False
                }
            },
            'neuromodulation_parameters': {
                'neuromod.dopamine.scale': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 10.0,
                    'recommended_range': (0.5, 2.0),
                    'critical': False
                },
                'neuromod.curiosity.enable': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'neuromod.curiosity.threshold': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 1.0,
                    'recommended_range': (0.3, 0.7),
                    'critical': False
                },
                'neuromod.novelty.enable': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'neuromod.novelty.decay_rate': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 1.0,
                    'recommended_range': (0.9, 0.99),
                    'critical': False
                }
            },
            'development_parameters': {
                'development.enable': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'development.plasticity_modifier_high': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 3.0,
                    'recommended_range': (0.8, 1.2),
                    'critical': False
                },
                'development.plasticity_modifier_low': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 1.0,
                    'recommended_range': (0.1, 0.3),
                    'critical': False
                }
            },
            'memory_parameters': {
                'memory.enable_working_memory': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'memory.enable_episodic_memory': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'memory.enable_associative_memory': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'memory.enable_procedural_memory': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                }
            },
            'prediction_parameters': {
                'prediction.enable_prediction': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'prediction.error_gain': {
                    'type': 'float',
                    'min': 0.0,
                    'max': 10.0,
                    'recommended_range': (0.1, 1.0),
                    'critical': False
                }
            },
            'performance_parameters': {
                'performance.use_memory_pool': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'performance.enable_simd_optimization': {
                    'type': 'boolean',
                    'recommended_value': True,
                    'critical': False
                },
                'performance.max_memory_mb': {
                    'type': 'integer',
                    'min': 100,
                    'max': 10000,
                    'recommended_range': (500, 5000),
                    'critical': False
                }
            }
        }
        
        return rules
    
    def validate_configuration(self, config: pynlm.Config) -> Dict[str, Any]:
        """Validate a configuration and return results."""
        
        validation_results = {
            'valid': True,
            'critical_errors': [],
            'warnings': [],
            'info_messages': [],
            'summary': {
                'total_rules': 0,
                'passed_rules': 0,
                'failed_rules': 0,
                'warnings_count': 0
            }
        }
        
        # Get all configuration keys
        config_keys = config.getKeys()
        
        # Validate each rule
        for category, parameters in self.validation_rules.items():
            for param_name, param_rules in parameters.items():
                validation_results['summary']['total_rules'] += 1
                
                # Check if parameter exists in config
                if not config.has(param_name):
                    if param_rules.get('critical', False):
                        error_msg = f"Critical parameter '{param_name}' is missing from configuration"
                        validation_results['critical_errors'].append(error_msg)
                        validation_results['valid'] = False
                    else:
                        warning_msg = f"Parameter '{param_name}' is missing but has recommended value: {param_rules.get('recommended_value', 'N/A')}"
                        validation_results['warnings'].append(warning_msg)
                    continue
                
                # Get parameter value
                param_value = self._get_parameter_value(config, param_name)
                
                # Validate parameter type
                is_valid, error_msg = self._validate_parameter_type(param_value, param_rules)
                if not is_valid:
                    if param_rules.get('critical', False):
                        validation_results['critical_errors'].append(f"Critical parameter '{param_name}': {error_msg}")
                        validation_results['valid'] = False
                    else:
                        validation_results['warnings'].append(f"Parameter '{param_name}': {error_msg}")
                    continue
                
                # Validate parameter range
                is_valid, error_msg = self._validate_parameter_range(param_value, param_rules)
                if not is_valid:
                    if param_rules.get('critical', False):
                        validation_results['critical_errors'].append(f"Critical parameter '{param_name}': {error_msg}")
                        validation_results['valid'] = False
                    else:
                        validation_results['warnings'].append(f"Parameter '{param_name}': {error_msg}")
                    continue
                
                # Check recommended values
                is_recommended, suggestion = self._check_recommended_values(param_value, param_rules)
                if not is_recommended:
                    validation_results['warnings'].append(f"Parameter '{param_name}': {suggestion}")
                
                validation_results['summary']['passed_rules'] += 1
        
        validation_results['summary']['failed_rules'] = (
            len(validation_results['critical_errors']) + len(validation_results['warnings'])
        )
        validation_results['summary']['warnings_count'] = len(validation_results['warnings'])
        
        return validation_results
    
    def _get_parameter_value(self, config: pynlm.Config, param_name: str) -> Any:
        """Get parameter value from configuration."""
        
        # Handle nested parameters (e.g., "neuromod.dopamine.scale")
        if '.' in param_name:
            parts = param_name.split('.')
            current = config
            for part in parts:
                if current.has(part):
                    current = current.get(part)
                else:
                    return None
            return current
        else:
            # Simple parameter
            return config.getOr(param_name, None)
    
    def _validate_parameter_type(self, value: Any, param_rules: Dict) -> Tuple[bool, str]:
        """Validate parameter type."""
        
        if value is None:
            return False, "Parameter value is None"
        
        expected_type = param_rules['type']
        
        if expected_type == 'positive_integer':
            if not isinstance(value, int) or value < 0:
                return False, f"Expected positive integer, got {type(value).__name__}: {value}"
        
        elif expected_type == 'integer':
            if not isinstance(value, int):
                return False, f"Expected integer, got {type(value).__name__}: {value}"
        
        elif expected_type == 'float':
            if not isinstance(value, (int, float)):
                return False, f"Expected float, got {type(value).__name__}: {value}"
        
        elif expected_type == 'boolean':
            if not isinstance(value, bool):
                return False, f"Expected boolean, got {type(value).__name__}: {value}"
        
        elif expected_type == 'string':
            if not isinstance(value, str):
                return False, f"Expected string, got {type(value).__name__}: {value}"
        
        elif expected_type == 'list':
            if not isinstance(value, list):
                return False, f"Expected list, got {type(value).__name__}: {value}"
        
        elif expected_type == 'dict':
            if not isinstance(value, dict):
                return False, f"Expected dict, got {type(value).__name__}: {value}"
        
        return True, ""
    
    def _validate_parameter_range(self, value: Any, param_rules: Dict) -> Tuple[bool, str]:
        """Validate parameter range."""
        
        if value is None:
            return False, "Parameter value is None"
        
        min_val = param_rules.get('min')
        max_val = param_rules.get('max')
        
        if min_val is not None and value < min_val:
            return False, f"Value {value} is below minimum {min_val}"
        
        if max_val is not None and value > max_val:
            return False, f"Value {value} is above maximum {max_val}"
        
        return True, ""
    
    def _check_recommended_values(self, value: Any, param_rules: Dict) -> Tuple[bool, str]:n:
        """Check if parameter value matches recommended values."""
        
        if value is None:
            return False, "Parameter value is None"
        
        recommended_range = param_rules.get('recommended_range')
        recommended_value = param_rules.get('recommended_value')
        
        if recommended_range is not None:
            min_val, max_val = recommended_range
            if value < min_val or value > max_val:
                return False, f"Recommended range: [{min_val}, {max_val}], got {value}"
        
        if recommended_value is not None:
            if value != recommended_value:
                return False, f"Recommended value: {recommended_value}, got {value}"
        
        return True, "Value is within recommended range"
    
    def generate_configuration_report(self, config: pynlm.Config) -> str:
        """Generate a comprehensive configuration report."""
        
        validation_results = self.validate_configuration(config)
        
        report = f"""# NLM Configuration Validation Report

## Configuration Summary
- **Configuration ID:** {id(config)}
- **Validation Status:** {"✓ PASS" if validation_results['valid'] else "✗ FAIL"}
- **Total Rules:** {validation_results['summary']['total_rules']}
- **Passed Rules:** {validation_results['summary']['passed_rules']}
- **Failed Rules:** {validation_results['summary']['failed_rules']}
- **Warnings:** {validation_results['summary']['warnings_count']}

## Critical Errors
{chr(10).join(['- ' + error for error in validation_results['critical_errors']]) if validation_results['critical_errors'] else 'None'}

## Warnings
{chr(10).join(['- ' + warning for warning in validation_results['warnings']]) if validation_results['warnings'] else 'None'}

## Configuration Details
{self._format_configuration_details(config)}

## Recommendations
{self._generate_recommendations(validation_results)}

---
*Report generated on: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}*
"""
        
        return report
    
    def _format_configuration_details(self, config: pynlm.Config) -> str:
        """Format configuration details for report."""
        
        details = []
        config_keys = config.getKeys()
        
        for key in config_keys:
            value = self._get_parameter_value(config, key)
            details.append(f"**{key}:** {value}")
        
        return '\n\n'.join(details)
    
    def _generate_recommendations(self, validation_results: Dict) -> str:
        """Generate recommendations based on validation results."""
        
        recommendations = []
        
        if validation_results['critical_errors']:
            recommendations.append("Resolve critical errors before using the configuration.")
        
        if validation_results['warnings']:
            recommendations.append("Address warnings to optimize configuration performance.")
        
        # Check for specific problematic parameters
        for error in validation_results['critical_errors']:
            if 'neuron_count' in error:
                recommendations.append("- Increase neuron_count to at least 10 (recommended: 100-5000)")
            elif 'region_count' in error:
                recommendations.append("- Set region_count to at least 1 (recommended: 2-20)")
            elif 'connection_probability' in error:
                recommendations.append("- Adjust connection_probability to [0.05, 0.2] range")
        
        if not recommendations:
            recommendations.append("Configuration is ready for use.")
        
        return '\n'.join(['- ' + rec for rec in recommendations])

class ConfigurationOptimizer:
    """Configuration optimization for NLM systems."""
    
    def __init__(self):
        self.performance_cache = {}
    
    def optimize_configuration(self, base_config: pynlm.Config, 
                             optimization_targets: Dict[str, float] = None) -> Dict[str, Any]:
        """Optimize configuration based on performance targets."""
        
        if optimization_targets is None:
            optimization_targets = {
                'performance': 0.8,
                'efficiency': 0.7,
                'accuracy': 0.75,
                'stability': 0.8
            }
        
        # Get current performance
        current_performance = self._evaluate_configuration_performance(base_config)
        
        # Generate optimized configuration
        optimized_config = self._generate_optimized_configuration(
            base_config, current_performance, optimization_targets
        )
        
        # Validate optimized configuration
        validator = ConfigurationValidator()
        validation_results = validator.validate_configuration(optimized_config)
        
        return {
            'original_performance': current_performance,
            'optimized_performance': self._evaluate_configuration_performance(optimized_config),
            'optimization_targets': optimization_targets,
            'validation_results': validation_results,
            'optimized_config': optimized_config
        }
    
    def _evaluate_configuration_performance(self, config: pynlm.Config) -> Dict[str, float]:
        """Evaluate configuration performance."""
        
        # This would typically involve running simulations
        # For demonstration, we'll use a simplified evaluation
        
        performance_metrics = {}
        
        # Get configuration parameters
        neuron_count = config.getOr("brain.neuron_count", 1000)
        region_count = config.getOr("brain.region_count", 3)
        connection_prob = config.getOr("brain.connection_probability", 0.1)
        
        # Calculate performance score (simplified)
        performance = (
            min(neuron_count / 5000, 1.0) * 0.3 +  # Neuron count efficiency
            min(region_count / 20, 1.0) * 0.2 +   # Region count efficiency
            connection_prob * 0.2 +              # Connection probability
            min(config.getOr("plasticity.stdp.enable", True), 1.0) * 0.1 +  # Plasticity
            min(config.getOr("neuromod.curiosity.enable", True), 1.0) * 0.1 +  # Neuromodulation
            min(config.getOr("development.enable", True), 1.0) * 0.1          # Development
        )
        
        performance_metrics = {
            'overall_performance': performance,
            'neuron_efficiency': min(neuron_count / 5000, 1.0),
            'region_efficiency': min(region_count / 20, 1.0),
            'plasticity_enabled': min(config.getOr("plasticity.stdp.enable", True), 1.0),
            'neuromodulation_enabled': min(config.getOr("neuromod.curiosity.enable", True), 1.0),
            'development_enabled': min(config.getOr("development.enable", True), 1.0)
        }
        
        return performance_metrics
    
    def _generate_optimized_configuration(self, base_config: pynlm.Config,
                                         current_performance: Dict, targets: Dict) -> pynlm.Config:
        """Generate optimized configuration."""
        
        optimized_config = pynlm.createDefaultConfig()
        
        # Copy brain parameters with optimizations
        optimized_config.set("brain.neuron_count", 
                           self._optimize_parameter(
                               base_config.getOr("brain.neuron_count", 1000),
                               targets['performance'] * 5000,
                               (100, 5000)
                           ))
        
        optimized_config.set("brain.region_count",
                           self._optimize_parameter(
                               base_config.getOr("brain.region_count", 3),
                               targets['performance'] * 20,
                               (1, 20)
                           ))
        
        optimized_config.set("brain.connection_probability",
                           self._optimize_parameter(
                               base_config.getOr("brain.connection_probability", 0.1),
                               targets['performance'] * 0.2,
                               (0.05, 0.2)
                           ))
        
        # Enable all recommended features
        optimized_config.set("plasticity.stdp.enable", True)
        optimized_config.set("plasticity.hebbian.enable", True)
        optimized_config.set("plasticity.structural.enable", True)
        
        optimized_config.set("neuromod.curiosity.enable", True)
        optimized_config.set("neuromod.novelty.enable", True)
        
        optimized_config.set("development.enable", True)
        optimized_config.set("memory.enable_working_memory", True)
        optimized_config.set("memory.enable_episodic_memory", True)
        
        optimized_config.set("performance.use_memory_pool", True)
        optimized_config.set("performance.enable_simd_optimization", True)
        
        return optimized_config
    
    def _optimize_parameter(self, current_value: float, target_value: float,
                           valid_range: Tuple[float, float]) -> float:
        """Optimize parameter value based on target."""
        
        # Simple optimization: move towards target within valid range
        if target_value < valid_range[0]:
            return valid_range[0]
        elif target_value > valid_range[1]:
            return valid_range[1]
        else:
            return target_value

class ConfigurationManager:
    """Configuration management for NLM systems."""
    
    def __init__(self, storage_path: str = "configurations"):
        self.storage_path = Path(storage_path)
        self.storage_path.mkdir(exist_ok=True)
        self.config_history = {}
    
    def save_configuration(self, config: pynlm.Config, name: str, description: str = ""):
        """Save configuration to file."""
        
        # Create configuration data
        config_data = {
            'name': name,
            'description': description,
            'timestamp': datetime.now().isoformat(),
            'config': self._config_to_dict(config)
        }
        
        # Save to file
        filename = self.storage_path / f"{name}.json"
        with open(filename, 'w') as f:
            json.dump(config_data, f, indent=2, default=str)
        
        # Add to history
        self.config_history[name] = config_data
        
        print(f"Configuration '{name}' saved to: {filename}")
    
    def load_configuration(self, name: str) -> pynlm.Config:
        """Load configuration from file."""
        
        # Check if in history
        if name in self.config_history:
            config_data = self.config_history[name]
        else:
            # Load from file
            filename = self.storage_path / f"{name}.json"
            if not filename.exists():
                raise FileNotFoundError(f"Configuration '{name}' not found")
            
            with open(filename, 'r') as f:
                config_data = json.load(f)
            
            # Add to history
            self.config_history[name] = config_data
        
        # Convert to NLM config
        config = self._dict_to_config(config_data['config'])
        
        return config
    
    def list_configurations(self) -> List[Dict[str, Any]]:
        """List all saved configurations."""
        
        config_list = []
        
        # Get from history
        for name, data in self.config_history.items():
            config_list.append({
                'name': name,
                'description': data.get('description', ''),
                'timestamp': data.get('timestamp', ''),
                'source': 'history'
            })
        
        # Check for files not in history
        for filename in self.storage_path.glob("*.json"):
            name = filename.stem
            if name not in self.config_history:
                with open(filename, 'r') as f:
                    data = json.load(f)
                
                config_list.append({
                    'name': name,
                    'description': data.get('description', ''),
                    'timestamp': data.get('timestamp', ''),
                    'source': 'file'
                })
        
        return sorted(config_list, key=lambda x: x['timestamp'], reverse=True)
    
    def compare_configurations(self, config1: pynlm.Config, config2: pynlm.Config) -> Dict[str, Any]:
        """Compare two configurations and highlight differences."""
        
        dict1 = self._config_to_dict(config1)
        dict2 = self._config_to_dict(config2)
        
        differences = {
            'identical': dict1 == dict2,
            'differences': {},
            'similarity_score': self._calculate_similarity(dict1, dict2)
        }
        
        # Find differences
        all_keys = set(dict1.keys()) | set(dict2.keys())
        
        for key in all_keys:
            val1 = dict1.get(key)
            val2 = dict2.get(key)
            
            if val1 != val2:
                differences['differences'][key] = {
                    'config1': val1,
                    'config2': val2,
                    'change': f"'{val2}'" if val1 is None else f"{val1} -> {val2}"
                }
        
        return differences
    
    def backup_configurations(self, backup_name: str = None):
        """Create backup of all configurations."""
        
        if backup_name is None:
            backup_name = f"backup_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        
        backup_path = Path(f"backups/{backup_name}")
        backup_path.mkdir(parents=True, exist_ok=True)
        
        # Save each configuration
        for name, data in self.config_history.items():
            filename = backup_path / f"{name}.json"
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
        
        print(f"Backup created: {backup_path}")
    
    def _config_to_dict(self, config: pynlm.Config) -> Dict[str, Any]:
        """Convert NLM config to dictionary."""
        
        config_dict = {}
        keys = config.getKeys()
        
        for key in keys:
            value = self._get_nested_config_value(config, key)
            if value is not None:
                config_dict[key] = value
        
        return config_dict
    
    def _get_nested_config_value(self, config: pynlm.Config, key: str) -> Any:
        """Get nested configuration value."""
        
        if '.' in key:
            parts = key.split('.')
            current = config
            for part in parts:
                if current.has(part):
                    current = current.get(part)
                else:
                    return None
            return current
        else:
            return config.getOr(key, None)
    
    def _dict_to_config(self, config_dict: Dict[str, Any]) -> pynlm.Config:
        """Convert dictionary to NLM config."""
        
        config = pynlm.createDefaultConfig()
        
        for key, value in config_dict.items():
            self._set_nested_config_value(config, key, value)
        
        return config
    
    def _set_nested_config_value(self, config: pynlm.Config, key: str, value: Any):
        """Set nested configuration value."""
        
        if '.' in key:
            parts = key.split('.')
            current = config
            
            # Navigate to parent
            for part in parts[:-1]:
                if not current.has(part):
                    current.set(part, pynlm.createDefaultConfig())
                current = current.get(part)
            
            # Set the final value
            final_key = parts[-1]
            current.set(final_key, value)
        else:
            config.set(key, value)
    
    def _calculate_similarity(self, dict1: Dict, dict2: Dict) -> float:
        """Calculate similarity between two configuration dictionaries."""
        
        if not dict1 and not dict2:
            return 1.0
        
        keys1 = set(dict1.keys())
        keys2 = set(dict2.keys())
        
        intersection = keys1 & keys2
        union = keys1 | keys2
        
        if not union:
            return 1.0
        
        similarity = len(intersection) / len(union)
        
        return similarity
```

### Configuration Usage Examples

```python
import pynlm
from configuration_management import ConfigurationValidator, ConfigurationOptimizer, ConfigurationManager

# Example 1: Configuration validation
print("=== Configuration Validation ===")

validator = ConfigurationValidator()

# Create a test configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 5)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Validate configuration
validation_results = validator.validate_configuration(config)

print(f"Validation Status: {"✓ PASS" if validation_results['valid'] else "✗ FAIL"}")
print(f"Total Rules: {validation_results['summary']['total_rules']}")
print(f"Passed Rules: {validation_results['summary']['passed_rules']}")
print(f"Failed Rules: {validation_results['summary']['failed_rules']}")

if validation_results['critical_errors']:
    print("\nCritical Errors:")
    for error in validation_results['critical_errors']:
        print(f"  - {error}")

if validation_results['warnings']:
    print("\nWarnings:")
    for warning in validation_results['warnings']:
        print(f"  - {warning}")

# Generate detailed report
report = validator.generate_configuration_report(config)
with open("configuration_validation_report.txt", 'w') as f:
    f.write(report)
print("\nDetailed report saved to: configuration_validation_report.txt")

# Example 2: Configuration optimization
print("\n=== Configuration Optimization ===")

optimizer = ConfigurationOptimizer()

# Define optimization targets
targets = {
    'performance': 0.8,
    'efficiency': 0.7,
    'accuracy': 0.75,
    'stability': 0.8
}

# Optimize configuration
optimization_results = optimizer.optimize_configuration(config, targets)

opt_config = optimization_results['optimized_config']

print(f"Original Performance: {optimization_results['original_performance']['overall_performance']:.3f}")
print(f"Optimized Performance: {optimization_results['optimized_performance']['overall_performance']:.3f}")
print(f"Performance Improvement: {optimization_results['optimized_performance']['overall_performance'] - optimization_results['original_performance']['overall_performance']:.3f}")

# Example 3: Configuration management
print("\n=== Configuration Management ===")

manager = ConfigurationManager()

# Save current configuration
manager.save_configuration(config, "research_baseline", "Baseline configuration for research experiments")

# Save optimized configuration
manager.save_configuration(opt_config, "optimized_for_performance", "Configuration optimized for performance")

# List all configurations
print("Saved configurations:")
for cfg_info in manager.list_configurations():
    print(f"  - {cfg_info['name']}: {cfg_info['description']} (saved: {cfg_info['timestamp']})")

# Load a specific configuration
loaded_config = manager.load_configuration("research_baseline")
print(f"\nLoaded configuration 'research_baseline': {loaded_config.getOr('brain.neuron_count', 'N/A')} neurons")

# Compare configurations
comparison = manager.compare_configurations(config, opt_config)
print(f"\nConfiguration Comparison:")
print(f"Identical: {comparison['identical']}")
print(f"Similarity Score: {comparison['similarity_score']:.3f}")

if comparison['differences']:
    print(f"\nDifferences:")
    for param, diff in comparison['differences'].items():
        print(f"  - {param}: {diff['change']}")

# Example 4: Complete workflow
print("\n=== Complete Configuration Workflow ===")

# Create comprehensive configuration
comprehensive_config = pynlm.createAdvancedConfig(
    neuronCount=1500,
    regionCount=4,
    connectionProbability=0.12,
    enableMemorySystems=True,
    enablePrediction=True,
    enableDevelopment=True
)

# Validate comprehensive configuration
validation = validator.validate_configuration(comprehensive_config)
if validation['valid']:
    print("✓ Comprehensive configuration validated successfully")
else:
    print(f"✗ Configuration validation failed with {len(validation['critical_errors'])} critical errors")

# Optimize comprehensive configuration
optimized_comprehensive = optimizer.optimize_configuration(
    comprehensive_config,
    {'performance': 0.8, 'efficiency': 0.7}
)

# Save multiple versions
manager.save_configuration(comprehensive_config, "research_comprehensive", "Comprehensive research configuration")
manager.save_configuration(optimized_comprehensive['optimized_config'], "research_optimized", "Optimized research configuration")

# Create backup
manager.backup_configurations()

print("\n=== Configuration Management Complete ===")
print("All configurations saved, optimized, and validated successfully!")
```

## Configuration File Formats

### JSON Configuration Format

```json
{
    "name": "Research Configuration",
    "description": "Configuration optimized for research experiments",
    "timestamp": "2026-01-15T10:30:00.000Z",
    "config": {
        "brain.neuron_count": 2000,
        "brain.region_count": 5,
        "brain.connection_probability": 0.15,
        "plasticity.stdp.enable": true,
        "neuromod.curiosity.enable": true,
        "development.enable": true
    }
}
```

### Configuration Schema

The configuration system supports nested parameters using dot notation:

- **brain.neuron_count** - Number of neurons in the brain
- **brain.region_count** - Number of brain regions
- **brain.connection_probability** - Probability of connections between neurons
- **plasticity.stdp.enable** - Enable STDP plasticity
- **neuromod.curiosity.enable** - Enable curiosity neuromodulation
- **development.enable** - Enable developmental processes

### Validation Rules

#### Brain Parameters
- **neuron_count**: Positive integer (10-100000), recommended: 100-5000
- **region_count**: Positive integer (1-1000), recommended: 2-20
- **connection_probability**: Float (0.0-1.0), recommended: 0.05-0.2

#### Plasticity Parameters
- **plasticity.stdp.enable**: Boolean (default: true)
- **plasticity.hebbian.enable**: Boolean (default: true)
- **plasticity.structural.enable**: Boolean (default: true)

#### Neuromodulation Parameters
- **neuromod.curiosity.enable**: Boolean (default: true)
- **neuromod.curiosity.threshold**: Float (0.0-1.0), recommended: 0.3-0.7

#### Development Parameters
- **development.enable**: Boolean (default: true)
- **development.plasticity_modifier**: Float (0.0-3.0), recommended: 0.8-1.2

## Configuration Templates

### Research Template
```python
research_config = pynlm.createAdvancedConfig(
    neuronCount=2000,
    regionCount=5,
    connectionProbability=0.15,
    enableMemorySystems=True,
    enablePrediction=True,
    enableDevelopment=True
)
```

### Performance Template
```python
performance_config = pynlm.createAdvancedConfig(
    neuronCount=1000,
    regionCount=3,
    connectionProbability=0.1,
    enableMemorySystems=False,
    enablePrediction=False,
    enableDevelopment=False
)
```

### Development Template
```python
development_config = pynlm.createAdvancedConfig(
    neuronCount=1500,
    regionCount=4,
    connectionProbability=0.12,
    enableMemorySystems=True,
    enablePrediction=True,
    enableDevelopment=True
)
```

## Configuration Best Practices

### 1. Parameter Validation
- Always validate configurations before use
- Check for critical parameters that are missing
- Verify parameter ranges and types
- Follow recommended value ranges

### 2. Configuration Management
- Save configurations with descriptive names
- Document configuration purposes and settings
- Use version control for configuration changes
- Create backups regularly

### 3. Performance Optimization
- Optimize for specific use cases (research, performance, development)
- Balance memory usage and computational efficiency
- Consider computational resources when selecting parameters

### 4. Reproducibility
- Save configurations with timestamps
- Use descriptive names for easy identification
- Maintain configuration history
- Document experimental conditions

## Configuration Troubleshooting

### Common Issues and Solutions

1. **Missing Parameters**
   ```python
   # Problem: Configuration missing critical parameters
   config = pynlm.createDefaultConfig()
   
   # Solution: Validate and add missing parameters
   validator = ConfigurationValidator()
   validation = validator.validate_configuration(config)
   
   if validation['critical_errors']:
       # Add missing parameters
       config.set("brain.neuron_count", 1000)
       config.set("brain.region_count", 3)
   ```

2. **Parameter Range Errors**
   ```python
   # Problem: Parameter outside valid range
   config.set("brain.neuron_count", 50000)  # Too high
   
   # Solution: Clamp to valid range
   if config.getOr("brain.neuron_count", 0) > 10000:
       config.set("brain.neuron_count", 10000)
   ```

3. **Configuration Performance**
   ```python
   # Problem: Configuration performs poorly
   # Solution: Optimize using ConfigurationOptimizer
   optimizer = ConfigurationOptimizer()
   results = optimizer.optimize_configuration(config, {'performance': 0.8})
   ```

4. **Configuration Reproducibility**
   ```python
   # Problem: Cannot reproduce previous experiments
   # Solution: Save configurations with descriptive names
   manager.save_configuration(config, "experiment_2024_01_15", "Control group configuration")
   ```

## Configuration Integration

### Integration with Python Bindings

```python
# Use optimized configurations in Python bindings
from configuration_management import ConfigurationManager

manager = ConfigurationManager()

# Load optimized configuration
opt_config = manager.load_configuration("research_optimized")

# Create brain with optimized configuration
brain = pynlm.createBrain(opt_config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

### Integration with Experimental Framework

```python
from advanced_experimental_framework import NLMExperiment
from configuration_management import ConfigurationManager

class ExperimentalConfiguration(NLMExperiment):
    def __init__(self, config_name: str):
        super().__init__("Experimental Configuration Study", "Study of configuration effects")
        self.config_manager = ConfigurationManager()
        self.config_name = config_name
    
    def run(self):
        # Load configuration
        config = self.config_manager.load_configuration(self.config_name)
        
        # Create brain with loaded configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run experiment
        results = self.run_experiment(brain)
        self.results = results
```

## Configuration File Examples

### Simple Configuration
```json
{
    "name": "Simple Brain",
    "description": "Basic brain configuration for simple tasks",
    "timestamp": "2026-01-15T10:00:00.000Z",
    "config": {
        "brain.neuron_count": 500,
        "brain.region_count": 2,
        "brain.connection_probability": 0.1,
        "plasticity.stdp.enable": true,
        "neuromod.curiosity.enable": true
    }
}
```

### Complex Configuration
```json
{
    "name": "Research Brain",
    "description": "Complex brain configuration for research experiments",
    "timestamp": "2026-01-15T14:30:00.000Z",
    "config": {
        "brain.neuron_count": 3000,
        "brain.region_count": 8,
        "brain.connection_probability": 0.2,
        "brain.initial_weight_mean": 0.0,
        "brain.initial_weight_std": 0.2,
        "plasticity.stdp.enable": true,
        "plasticity.stdp.learning_rate": 0.002,
        "plasticity.hebbian.enable": true,
        "plasticity.hebbian.learning_rate": 0.001,
        "plasticity.structural.enable": true,
        "plasticity.structural.synaptogenesis_rate": 0.01,
        "plasticity.structural.pruning_rate": 0.001,
        "neuromod.curiosity.enable": true,
        "neuromod.curiosity.threshold": 0.5,
        "neuromod.novelty.enable": true,
        "development.enable": true,
        "memory.enable_working_memory": true,
        "memory.enable_episodic_memory": true,
        "memory.enable_associative_memory": true,
        "prediction.enable_prediction": true,
        "performance.use_memory_pool": true,
        "performance.enable_simd_optimization": true
    }
}
```

## Configuration Quality Assurance

### Configuration Validation Checklist

- [ ] All critical parameters present
- [ ] Parameter types correct
- [ ] Parameter ranges valid
- [ ] Recommended values followed
- [ ] Configuration documented
- [ ] Configuration tested
- [ ] Configuration backed up
- [ ] Configuration reproducible

### Configuration Testing Checklist

- [ ] Basic functionality test
- [ ] Performance test
- [ ] Memory usage test
- [ ] Error handling test
- [ ] Configuration validation test
- [ ] Integration test
- [ ] Compatibility test

## Configuration Security

### Access Control
- Restrict access to sensitive configurations
- Use file permissions for configuration files
- Implement access logging
- Use encryption for sensitive configurations

### Integrity Verification
- Hash configuration files for integrity verification
- Use digital signatures for critical configurations
- Implement configuration validation before use
- Log configuration changes

## Configuration Documentation

### Configuration Documentation Template

```markdown
# Configuration Documentation

## Configuration Overview
- **Name:** [Configuration Name]
- **Description:** [Description]
- **Version:** [Version]
- **Created:** [Timestamp]
- **Author:** [Author]

## Configuration Parameters
| Parameter | Value | Type | Description |
|-----------|-------|------|-------------|
| brain.neuron_count | [value] | Integer | Number of neurons |
| brain.region_count | [value] | Integer | Number of brain regions |

## Usage Instructions
1. Load the configuration
2. Initialize the brain
3. Run the simulation
4. Analyze results

## Expected Performance
- **Runtime:** [estimated time]
- **Memory Usage:** [estimated memory]
- **Throughput:** [estimated throughput]

## Dependencies
- [ ] Parameter dependencies
- [ ] Software dependencies
- [ ] Hardware dependencies

## Limitations
- [ ] Known limitations
- [ ] Performance constraints
- [ ] Compatibility issues

## Contact Information
- **Email:** [contact email]
- **Documentation:** [documentation URL]
```

## Conclusion

The Configuration Validation and Management system provides comprehensive tools for:

1. **Configuration validation** with detailed error reporting
2. **Configuration optimization** with performance tuning
3. **Configuration management** with version control and backups
4. **Configuration documentation** with comprehensive reporting
5. **Configuration security** with access control and integrity verification

This system ensures that NLM configurations are:

- **Valid** - All parameters correct and within ranges
- **Optimized** - Performance tuned for specific use cases
- **Manageable** - Easy to save, load, and compare
- **Documented** - Well-documented with usage instructions
- **Secure** - Protected from unauthorized access and modification

The configuration management system is essential for:

- **Research reproducibility** with documented configurations
- **Performance optimization** with automated tuning
- **Quality assurance** with comprehensive validation
- **Long-term maintenance** with version control
- **Team collaboration** with shared configuration management

By using this system, NLM users can:

1. Create reliable, well-documented configurations
2. Optimize performance for specific requirements
3. Maintain configuration history and reproducibility
4. Ensure quality and consistency across experiments
5. Facilitate collaboration and knowledge sharing

The enhanced NLM configuration management system makes it easier to work with complex neural learning machines, ensuring reliable operation and reproducible research results.
