"""
Advanced configuration management for NLM Python bindings

This module provides enhanced configuration management capabilities
including templates, comparison, merging, and advanced validation.
"""

import json
import os
import hashlib
import time
import threading
from typing import Dict, List, Any, Optional, Union, Set, Tuple, Callable
from enum import Enum
from copy import deepcopy
import jsonschema
from jsonschema import validate as jsonschema_validate
from datetime import datetime

try:
    import yaml
    HAS_YAML = True
except ImportError:
    HAS_YAML = False

class ValidationSeverity(Enum):
    """Configuration validation severity levels."""
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"

class ValidationResult:
    """Configuration validation result with severity levels."""
    
    def __init__(self, key: str, message: str, severity: ValidationSeverity = ValidationSeverity.INFO):
        self.key = key
        self.message = message
        self.severity = severity
    
    def is_valid(self) -> bool:
        """Check if this validation result is valid (not an error)."""
        return self.severity not in [ValidationSeverity.ERROR, ValidationSeverity.CRITICAL]
    
    def is_critical(self) -> bool:
        """Check if this validation result is critical."""
        return self.severity == ValidationSeverity.CRITICAL
class AdvancedConfigManager:
    """Advanced configuration manager with templates and validation."""
    
    def __init__(self, config=None):
        """Initialize advanced configuration manager.
        
        Args:
            config: Initial configuration dictionary (optional)
        """
        self._config = config or {}
        self._templates = {}
        self._validators = {}
        self._comparisons = {}
        self._config_history = []
        self._config_hash = None
        
    def load_config_from_file(self, filepath: str, format: Optional[str] = None) -> 'AdvancedConfigManager':
        """Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            format: File format ('json', 'yaml', or auto-detected)
            
        Returns:
            AdvancedConfigManager instance
        """
        if format is None:
            if filepath.endswith('.json'):
                format = 'json'
            elif filepath.endswith(('.yaml', '.yml')):
                format = 'yaml'
            else:
                raise ValueError(f"Unsupported file format: {filepath}")
        
        with open(filepath, 'r') as f:
            if format == 'json':
                self._config = json.load(f)
            elif format == 'yaml':
                if not HAS_YAML:
                    raise ImportError("PyYAML is required to load YAML files. Install with: pip install PyYAML")
                self._config = yaml.safe_load(f)
            else:
                raise ValueError(f"Unsupported format: {format}")
        
        self._update_config_hash()
        return self
    
    def create_template(self, name: str, config: Dict[str, Any]) -> 'AdvancedConfigManager':
        """Create configuration template.
        
        Args:
            name: Template name
            config: Template configuration
            
        Returns:
            AdvancedConfigManager instance
        """
        self._templates[name] = deepcopy(config)
        return self
    
    def apply_template(self, name: str) -> 'AdvancedConfigManager':
        """Apply configuration template.
        
        Args:
            name: Template name
            
        Returns:
            AdvancedConfigManager instance
            
        Raises:
            ValueError: If template doesn't exist
        """
        if name not in self._templates:
            raise ValueError(f"Template '{name}' not found")
        
        # Merge template with current config (template overrides)
        merged_config = deepcopy(self._templates[name])
        merged_config.update(deepcopy(self._config))
        
        self._config = merged_config
        self._update_config_hash()
        return self
    
    def compare_configs(self, other_config: 'AdvancedConfigManager') -> Dict[str, Any]:
        """Compare two configurations.
        
        Args:
            other_config: Other configuration to compare with
            
        Returns:
            Comparison result with differences, similarities, and recommendations
        """
        if not isinstance(other_config, AdvancedConfigManager):
            raise TypeError("Can only compare with AdvancedConfigManager instance")
        
        result = {
            'differences': {},
            'similarities': {},
            'recommendations': [],
            'config_a_keys': list(self._config.keys()),
            'config_b_keys': list(other_config.get_config().keys()),
        }
        
        # Find differences
        all_keys = set(self._config.keys()) | set(other_config.get_config().keys())
        
        for key in all_keys:
            value_a = self._config.get(key)
            value_b = other_config.get_config().get(key)
            
            if value_a != value_b:
                result['differences'][key] = {
                    'config_a': value_a,
                    'config_b': value_b,
                    'type': type(value_a).__name__ if value_a is not None else None,
                    'change': self._calculate_change(value_a, value_b)
                }
            else:
                result['similarities'][key] = value_a
        
        # Generate recommendations
        if result['differences']:
            result['recommendations'].append("Review differences for compatibility and performance impact")
        
        # Check for critical differences
        for key, diff in result['differences'].items():
            if self._is_critical_difference(key, diff):
                result['recommendations'].append(f"CRITICAL: Configuration key '{key}' has significant difference - verify compatibility")
        
        return result
    
    def merge_configs(self, other_config: 'AdvancedConfigManager', strategy: str = 'smart') -> 'AdvancedConfigManager':
        """Merge two configurations.
        
        Args:
            other_config: Other configuration to merge
            strategy: Merge strategy ('smart', 'override', 'deep')
            
        Returns:
            AdvancedConfigManager instance with merged configuration
        """
        if not isinstance(other_config, AdvancedConfigManager):
            raise TypeError("Can only merge with AdvancedConfigManager instance")
        
        merged_config = deepcopy(self._config)
        
        if strategy == 'override':
            merged_config.update(deepcopy(other_config.get_config()))
        elif strategy == 'deep':
            self._deep_merge(merged_config, other_config.get_config())
        else:  # smart
            merged_config = self._smart_merge(merged_config, other_config.get_config())
        
        result = AdvancedConfigManager(merged_config)
        result._templates = deepcopy(self._templates)
        result._validators = deepcopy(self._validators)
        result._comparisons = deepcopy(self._comparisons)
        
        return result
    
    def validate_advanced(self, validation_level: str = 'strict') -> List[ValidationResult]:
        """Advanced configuration validation.
        
        Args:
            validation_level: Validation level ('strict', 'fast', 'comprehensive')
            
        Returns:
            List of validation results
        """
        results = []
        
        # Basic validation
        if validation_level in ['strict', 'comprehensive']:
            # Type validation
            for key, value in self._config.items():
                is_valid, error_msg = self._validate_type(key, value)
                if not is_valid:
                    results.append(ValidationResult(
                        key, f"Type validation failed for '{key}': {error_msg}",
                        ValidationSeverity.ERROR
                    ))
        
        # Value validation
        if validation_level in ['strict', 'comprehensive']:
            for key, value in self._config.items():
                is_valid, error_msg = self._validate_value(key, value)
                if not is_valid:
                    severity = self._get_validation_severity(key, value)
                    results.append(ValidationResult(key, error_msg, severity))
        
        # Performance impact analysis
        if validation_level in ['comprehensive']:
            perf_issues = self._analyze_performance_impact()
            for key, message in perf_issues.items():
                results.append(ValidationResult(
                    key, f"Performance issue: {message}",
                    ValidationSeverity.WARNING
                ))
        
        # Compatibility checking
        if validation_level in ['strict', 'comprehensive']:
            compat_issues = self._check_compatibility()
            for key, message in compat_issues.items():
                results.append(ValidationResult(
                    key, f"Compatibility issue: {message}",
                    ValidationSeverity.WARNING
                ))
        
        return results
    
    def optimize_config(self, constraints: Dict[str, Any]) -> 'AdvancedConfigManager':
        """Optimize configuration based on constraints.
        
        Args:
            constraints: Optimization constraints
            
        Returns:
            AdvancedConfigManager instance with optimized configuration
        """
        optimized_config = deepcopy(self._config)
        
        # Memory optimization
        if 'memory_limit' in constraints:
            optimized_config = self._optimize_memory(optimized_config, constraints['memory_limit'])
        
        # Performance optimization
        if 'performance_target' in constraints:
            optimized_config = self._optimize_performance(optimized_config, constraints['performance_target'])
        
        # Compatibility optimization
        if 'compatibility_target' in constraints:
            optimized_config = self._optimize_compatibility(optimized_config, constraints['compatibility_target'])
        
        # Update configuration
        self._config = optimized_config
        self._update_config_hash()
        
        return self
    
    def save_config_with_validation(self, filepath: str, format: Optional[str] = None) -> bool:
        """Save configuration with validation.
        
        Args:
            filepath: Path to save configuration
            format: File format ('json', 'yaml', or auto-detected)
            
        Returns:
            True if save succeeded, False otherwise
        """
        try:
            # Validate before saving
            validation_results = self.validate_advanced()
            
            # Check for critical validation errors
            critical_errors = [r for r in validation_results if r.is_critical()]
            if critical_errors:
                raise ValueError(f"Configuration validation failed with {len(critical_errors)} critical errors")
            
            # Save configuration
            if format is None:
                if filepath.endswith('.json'):
                    format = 'json'
                elif filepath.endswith(('.yaml', '.yml')):
                    format = 'yaml'
                else:
                    format = 'json'  # default
            
            with open(filepath, 'w') as f:
                if format == 'json':
                    json.dump(self._config, f, indent=2)
                elif format == 'yaml':
                    if not HAS_YAML:
                        raise ImportError("PyYAML is required to save YAML files. Install with: pip install PyYAML")
                    yaml.dump(self._config, f, default_flow_style=False)
            
            # Save validation report
            report_file = filepath + '.validation_report'
            self._save_validation_report(report_file, validation_results)
            
            return True
        
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def get_config_history(self, limit: int = 10) -> List[Dict[str, Any]]:
        """Get configuration history.
        
        Args:
            limit: Maximum number of history entries to return
            
        Returns:
            List of configuration history entries
        """
        return self._config_history[-limit:]
    
    def is_config_changed(self, config: Optional[Dict[str, Any]] = None) -> bool:
        """Check if configuration has changed.
        
        Args:
            config: Configuration to check against (uses current if None)
            
        Returns:
            True if configuration has changed
        """
        if config is None:
            config = self._config
        
        current_hash = self._calculate_config_hash(config)
        return current_hash != self._config_hash
    
    # Getters and setters
    
    def get_config(self) -> Dict[str, Any]:
        """Get configuration dictionary.
        
        Returns:
            Configuration dictionary
        """
        return deepcopy(self._config)
    
    def set_config(self, config: Dict[str, Any]) -> 'AdvancedConfigManager':
        """Set configuration.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            AdvancedConfigManager instance
        """
        self._config = deepcopy(config)
        self._update_config_hash()
        return self
    
    def get_template(self, name: str) -> Optional[Dict[str, Any]]:
        """Get configuration template.
        
        Args:
            name: Template name
            
        Returns:
            Template configuration or None if not found
        """
        return deepcopy(self._templates.get(name))
    
    def remove_template(self, name: str) -> 'AdvancedConfigManager':
        """Remove configuration template.
        
        Args:
            name: Template name
            
        Returns:
            AdvancedConfigManager instance
        """
        if name in self._templates:
            del self._templates[name]
        return self
    
    def get_config_summary(self) -> Dict[str, Any]:
        """Get configuration summary.
        
        Returns:
            Configuration summary
        """
        summary = {
            'total_keys': len(self._config),
            'config_hash': self._config_hash,
            'template_count': len(self._templates),
            'validators_count': len(self._validators),
        }
        
        # Add category information
        categories = {}
        for key in self._config.keys():
            category = key.split('.')[0] if '.' in key else 'general'
            categories[category] = categories.get(category, 0) + 1
        
        summary['categories'] = categories
        
        return summary
    
    # Internal helper methods
    
    def _update_config_hash(self):
        """Update configuration hash."""
        self._config_hash = self._calculate_config_hash(self._config)
    
    def _calculate_config_hash(self, config: Dict[str, Any]) -> str:
        """Calculate configuration hash.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            Configuration hash
        """
        config_str = json.dumps(config, sort_keys=True)
        return hashlib.md5(config_str.encode()).hexdigest()
    
    def _calculate_change(self, value_a: Any, value_b: Any) -> str:
        """Calculate type of change between two values.
        
        Args:
            value_a: First value
            value_b: Second value
            
        Returns:
            Change description
        """
        if value_a is None and value_b is not None:
            return 'added'
        elif value_a is not None and value_b is None:
            return 'removed'
        elif type(value_a) != type(value_b):
            return 'type_changed'
        elif value_a != value_b:
            return 'value_changed'
        else:
            return 'identical'
    
    def _is_critical_difference(self, key: str, diff: Dict[str, Any]) -> bool:
        """Check if difference is critical.
        
        Args:
            key: Configuration key
            diff: Difference dictionary
            
        Returns:
            True if difference is critical
        """
        # Critical if key relates to core functionality and value changed significantly
        critical_keys = ['brain.', 'simulation.', 'learning.']
        
        if any(key.startswith(kw) for kw in critical_keys):
            return True
        
        # Check for type changes in critical components
        if diff['type'] and diff['change'] == 'type_changed':
            return True
        
        return False
    
    def _deep_merge(self, target: Dict[str, Any], source: Dict[str, Any]):
        """Deep merge two dictionaries.
        
        Args:
            target: Target dictionary
            source: Source dictionary
        """
        for key, value in source.items():
            if key in target and isinstance(target[key], dict) and isinstance(value, dict):
                self._deep_merge(target[key], value)
            else:
                target[key] = deepcopy(value)
    
    def _smart_merge(self, target: Dict[str, Any], source: Dict[str, Any]) -> Dict[str, Any]:
        """Smart merge two configurations.
        
        Args:
            target: Target configuration
            source: Source configuration
            
        Returns:
            Merged configuration
        """
        merged = deepcopy(target)
        
        # Source overrides target for simple values
        for key, value in source.items():
            if key not in target or self._should_override(key, target[key], value):
                merged[key] = deepcopy(value)
            elif isinstance(target[key], dict) and isinstance(value, dict):
                merged[key] = self._smart_merge(target[key], value)
        
        return merged
    
    def _should_override(self, key: str, target_value: Any, source_value: Any) -> bool:
        """Determine if target value should be overridden.
        
        Args:
            key: Configuration key
            target_value: Target value
            source_value: Source value
            
        Returns:
            True if should override
        """
        # Override if source value is not None and target value is None
        if target_value is None and source_value is not None:
            return True
        
        # Override if values are of different types
        if type(target_value) != type(source_value):
            return True
        
        # Override if source is a template and target is default
        if self._is_default_value(key, target_value) and not self._is_default_value(key, source_value):
            return True
        
        return False
    
    def _is_default_value(self, key: str, value: Any) -> bool:
        """Check if value is default for the key.
        
        Args:
            key: Configuration key
            value: Value to check
            
        Returns:
            True if value is default
        """
        # Simple default value checks
        default_values = {
            'brain.region_count': 1,
            'brain.neurons_per_region': 100,
            'simulation.max_steps': 1000,
            'simulation.dt': 0.01,
            'logging.level': 'info',
        }
        
        return default_values.get(key) == value
    
    def _validate_type(self, key: str, value: Any) -> Tuple[bool, str]:
        """Validate configuration type.
        
        Args:
            key: Configuration key
            value: Value to validate
            
        Returns:
            Tuple of (is_valid, error_message)
        """
        # Type validation can be extended based on specific schema
        if value is None:
            return True, ""
        
        # Basic type checks
        if key.startswith('brain.') and not isinstance(value, (int, float)):
            return False, f"Expected numeric value for brain configuration, got {type(value).__name__}"
        
        if key.startswith('simulation.') and not isinstance(value, (int, float)):
            return False, f"Expected numeric value for simulation configuration, got {type(value).__name__}"
        
        if key.startswith('logging.') and not isinstance(value, str):
            return False, f"Expected string value for logging configuration, got {type(value).__name__}"
        
        return True, ""
    
    def _validate_value(self, key: str, value: Any) -> Tuple[bool, str]:
        """Validate configuration value.
        
        Args:
            key: Configuration key
            value: Value to validate
            
        Returns:
            Tuple of (is_valid, error_message)
        """
        # Range validation for specific keys
        if key == 'simulation.dt' and isinstance(value, (int, float)):
            if value <= 0 or value > 1.0:
                return False, f"Time step must be between 0 and 1.0, got {value}"
        
        if key == 'brain.neurons_per_region' and isinstance(value, (int, float)):
            if value <= 0 or value > 10000:
                return False, f"Neuron count must be between 1 and 10000, got {value}"
        
        # Enum validation for logging level
        if key == 'logging.level' and isinstance(value, str):
            valid_levels = ['debug', 'info', 'warning', 'error', 'critical']
            if value.lower() not in valid_levels:
                return False, f"Invalid logging level '{value}'. Must be one of {valid_levels}"
        
        return True, ""
    
    def _get_validation_severity(self, key: str, value: Any) -> ValidationSeverity:
        """Get validation severity for a key-value pair.
        
        Args:
            key: Configuration key
            value: Value to check
            
        Returns:
            Validation severity
        """
        # Critical for core functionality keys
        critical_keys = ['brain.', 'simulation.', 'learning.', 'environment.']
        
        if any(key.startswith(kw) for kw in critical_keys):
            return ValidationSeverity.CRITICAL
        
        # Error for obvious value issues
        if key == 'simulation.dt' and isinstance(value, (int, float)):
            if value <= 0:
                return ValidationSeverity.ERROR
        
        # Warning for potential issues
        if key == 'logging.level' and isinstance(value, str):
            if value.lower() not in ['debug', 'info', 'warning', 'error', 'critical']:
                return ValidationSeverity.WARNING
        
        return ValidationSeverity.INFO
    
    def _analyze_performance_impact(self) -> Dict[str, str]:
        """Analyze performance impact of configuration.
        
        Returns:
            Dictionary of performance issues
        """
        issues = {}
        
        # Check for potentially expensive configurations
        if self._config.get('brain.neurons_per_region', 0) > 1000:
            issues['brain.neurons_per_region'] = "Large neuron count may impact performance"
        
        if self._config.get('simulation.max_steps', 0) > 10000:
            issues['simulation.max_steps'] = "Large step count may increase simulation time"
        
        # Check for many regions
        region_count = len([k for k in self._config.keys() if k.startswith('brain.region_')])
        if region_count > 10:
            issues['brain.region_count'] = "Many regions may impact performance"
        
        return issues
    
    def _check_compatibility(self) -> Dict[str, str]:
        """Check configuration compatibility.
        
        Returns:
            Dictionary of compatibility issues
        """
        issues = {}
        
        # Check for incompatible combinations
        if (self._config.get('learning.enable_curiosity') and 
            not self._config.get('learning.enable_reward_modulation', True)):
            issues['learning.combination'] = "Curiosity enabled without reward modulation may reduce learning effectiveness"
        
        # Check for missing dependencies
        if self._config.get('logging.enable_file') and not HAS_YAML:
            issues['logging.yaml_dependency'] = "YAML logging requires PyYAML library"
        
        return issues
    
    def _optimize_memory(self, config: Dict[str, Any], memory_limit: int) -> Dict[str, Any]:
        """Optimize configuration for memory constraints.
        
        Args:
            config: Configuration to optimize
            memory_limit: Memory limit in MB
            
        Returns:
            Optimized configuration
        """
        optimized = deepcopy(config)
        
        # Reduce neuron count if needed
        if memory_limit < 100 and optimized.get('brain.neurons_per_region', 0) > 100:
            optimized['brain.neurons_per_region'] = min(100, optimized['brain.neurons_per_region'])
        
        return optimized
    
    def _optimize_performance(self, config: Dict[str, Any], performance_target: str) -> Dict[str, Any]:
        """Optimize configuration for performance targets.
        
        Args:
            config: Configuration to optimize
            performance_target: Performance target ('fast', 'balanced', 'thorough')
            
        Returns:
            Optimized configuration
        """
        optimized = deepcopy(config)
        
        if performance_target == 'fast':
            # Reduce simulation steps
            if optimized.get('simulation.max_steps', 0) > 1000:
                optimized['simulation.max_steps'] = 1000
        
        return optimized
    
    def _optimize_compatibility(self, config: Dict[str, Any], compatibility_target: str) -> Dict[str, Any]:
        """Optimize configuration for compatibility.
        
        Args:
            config: Configuration to optimize
            compatibility_target: Compatibility target ('backward', 'forward', 'universal')
            
        Returns:
            Optimized configuration
        """
        optimized = deepcopy(config)
        
        # Add compatibility mode settings
        if compatibility_target == 'backward':
            optimized['compatibility.mode'] = 'backward_compatible'
        
        return optimized
    
    def _save_validation_report(self, filepath: str, validation_results: List[ValidationResult]):
        """Save validation report to file.
        
        Args:
            filepath: Path to save validation report
            validation_results: Validation results
        """
        report = {
            'timestamp': str(datetime.now()),
            'validation_results': [
                {
                    'key': result.key,
                    'message': result.message,
                    'severity': result.severity.value,
                    'is_valid': result.is_valid(),
                    'is_critical': result.is_critical(),
                }
                for result in validation_results
            ],
            'summary': {
                'total_validations': len(validation_results),
                'valid_count': len([r for r in validation_results if r.is_valid()]),
                'error_count': len([r for r in validation_results if r.severity == ValidationSeverity.ERROR]),
                'critical_count': len([r for r in validation_results if r.is_critical()]),
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(report, f, indent=2)