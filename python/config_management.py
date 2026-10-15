#!/usr/bin/env python3
"""
NLM Configuration Management

Configuration management utilities for NLM including validation, migration,
templates, and automated configuration generation.
"""

import json
import os
import sys
import yaml
import copy
import threading
from pathlib import Path
from typing import Dict, List, Optional, Any, Union, Callable
from dataclasses import dataclass, asdict
import jsonschema
from jsonschema import validate, ValidationError

# Import NLM modules
import pynlm
from python.convenience_api import configureFromPreset

__all__ = [
    'ConfigValidator',
    'ConfigMigrator',
    'ConfigTemplateManager',
    'AutoConfigGenerator',
    'ConfigDiff',
    'createConfigValidator',
    'createConfigMigrator',
    'createTemplateManager',
    'createAutoGenerator'
]

@dataclass
class ConfigProfile:
    """Configuration profile for NLM."""
    name: str
    description: str
    config: Dict[str, Any]
    tags: List[str]
    created: float
    version: str
    author: str = "NLM"
    dependencies: Optional[List[str]] = None

@dataclass
class ValidationResult:
    """Configuration validation result."""
    is_valid: bool
    errors: List[str]
    warnings: List[str]
    schema_version: str
    recommended_actions: List[str]

@dataclass
class MigrationResult:
    """Configuration migration result."""
    success: bool
    migrated_config: Dict[str, Any]
    warnings: List[str]
    breaking_changes: List[str]
    manual_intervention_required: bool

class ConfigValidator:
    """
    Configuration validation for NLM.
    
    This class provides comprehensive configuration validation including:
    - Schema validation
    - Constraint checking
    - Dependency validation
    - Best practices checking
    """
    
    def __init__(self, schema_path: Optional[str] = None):
        """
        Initialize configuration validator.
        
        Args:
            schema_path: Path to configuration schema file
        """
        self.schema = self._load_default_schema()
        if schema_path:
            self._load_schema_from_file(schema_path)
        
        # Validation rules
        self.validation_rules = self._load_validation_rules()
        self.dependency_checker = DependencyChecker()
    
    def _load_default_schema(self) -> Dict[str, Any]:
        """Load default configuration schema."""
        return {
            "$schema": "http://json-schema.org/draft-07/schema#",
            "type": "object",
            "properties": {
                "brain": {
                    "type": "object",
                    "properties": {
                        "neuron_count": {"type": "integer", "minimum": 1, "maximum": 100000},
                        "region_count": {"type": "integer", "minimum": 1, "maximum": 1000},
                        "connection_probability": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                        "initial_weight_mean": {"type": "number"},
                        "initial_weight_std": {"type": "number", "minimum": 0.0},
                        "v_thresh": {"type": "number"},
                        "v_rest": {"type": "number"},
                        "v_reset": {"type": "number"},
                        "tau_mem": {"type": "number", "minimum": 0.0},
                        "tau_ref": {"type": "number", "minimum": 0.0}
                    },
                    "required": ["neuron_count", "connection_probability"]
                },
                "plasticity": {
                    "type": "object",
                    "properties": {
                        "stdp": {
                            "type": "object",
                            "properties": {
                                "enable": {"type": "boolean"},
                                "learning_rate": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                                "tau_plus": {"type": "number", "minimum": 0.0},
                                "tau_minus": {"type": "number", "minimum": 0.0}
                            }
                        },
                        "hebbian": {
                            "type": "object",
                            "properties": {
                                "enable": {"type": "boolean"},
                                "learning_rate": {"type": "number", "minimum": 0.0, "maximum": 1.0}
                            }
                        },
                        "structural": {
                            "type": "object",
                            "properties": {
                                "enable": {"type": "boolean"},
                                "rate": {"type": "number", "minimum": 0.0}
                            }
                        }
                    }
                },
                "neuromod": {
                    "type": "object",
                    "properties": {
                        "dopamine": {
                            "type": "object",
                            "properties": {
                                "scale": {"type": "number", "minimum": 0.0}
                            }
                        },
                        "curiosity": {
                            "type": "object",
                            "properties": {
                                "enable": {"type": "boolean"},
                                "scale": {"type": "number", "minimum": 0.0}
                            }
                        },
                        "novelty": {
                            "type": "object",
                            "properties": {
                                "enable": {"type": "boolean"},
                                "threshold": {"type": "number", "minimum": 0.0}
                            }
                        }
                    }
                },
                "simulation": {
                    "type": "object",
                    "properties": {
                        "timestep": {"type": "number", "minimum": 0.0},
                        "max_steps": {"type": "integer", "minimum": 1},
                        "checkpoint_interval": {"type": "integer", "minimum": 0}
                    }
                }
            },
            "additionalProperties": False
        }
    
    def _load_schema_from_file(self, schema_path: str):
        """Load schema from file."""
        try:
            with open(schema_path, 'r') as f:
                file_schema = json.load(f)
            self.schema = file_schema
        except Exception as e:
            raise ValueError(f"Failed to load schema from {schema_path}: {e}")
    
    def _load_validation_rules(self) -> Dict[str, Callable]:
        """Load validation rules."""
        return {
            'config_size': self._validate_config_size,
            'neuron_region_ratio': self._validate_neuron_region_ratio,
            'plasticity_balance': self._validate_plasticity_balance,
            'neuromod_consistency': self._validate_neuromod_consistency,
            'performance_estimates': self._validate_performance_estimates
        }
    
    def validate(self, config: Union[pynlm.Config, Dict, str], 
                file_path: Optional[str] = None) -> ValidationResult:
        """
        Validate configuration.
        
        Args:
            config: Configuration to validate (Config object, dict, or file path)
            file_path: Path to config file (if config is a path)
            
        Returns:
            Validation result
        """
        errors = []
        warnings = []
        
        # Parse configuration
        try:
            if isinstance(config, pynlm.Config):
                config_dict = self._config_to_dict(config)
            elif isinstance(config, str) and file_path is None:
                # Assume it's a file path
                config_dict = self._load_config_from_file(config)
            elif isinstance(config, dict):
                config_dict = config
            else:
                raise ValueError("Unsupported config type")
        except Exception as e:
            errors.append(f"Failed to parse configuration: {e}")
            return ValidationResult(
                is_valid=False,
                errors=errors,
                warnings=warnings,
                schema_version="7.0.0",
                recommended_actions=[]
            )
        
        # Validate against schema
        try:
            validate(instance=config_dict, schema=self.schema)
        except ValidationError as e:
            errors.append(f"Schema validation failed: {e.message}")
        
        # Apply validation rules
        for rule_name, rule_func in self.validation_rules.items():
            try:
                rule_warnings, rule_errors = rule_func(config_dict)
                warnings.extend(rule_warnings)
                errors.extend(rule_errors)
            except Exception as e:
                errors.append(f"Validation rule '{rule_name}' failed: {e}")
        
        # Check dependencies
        dep_warnings, dep_errors = self.dependency_checker.check_dependencies(config_dict)
        warnings.extend(dep_warnings)
        errors.extend(dep_errors)
        
        # Validate best practices
        bp_warnings, bp_errors = self._validate_best_practices(config_dict)
        warnings.extend(bp_warnings)
        errors.extend(bp_errors)
        
        # Estimate performance
        perf_warnings = self._estimate_performance(config_dict)
        warnings.extend(perf_warnings)
        
        is_valid = len(errors) == 0
        
        return ValidationResult(
            is_valid=is_valid,
            errors=errors,
            warnings=warnings,
            schema_version=self.schema.get("$schema", "7.0.0"),
            recommended_actions=self._generate_recommended_actions(config_dict, errors, warnings)
        )
    
    def _config_to_dict(self, config: pynlm.Config) -> Dict[str, Any]:
        """Convert Config object to dictionary."""
        result = {}
        
        # Get all keys from config
        for key in config.getKeys():
            value = config.getOr(key, None)
            if value is not None:
                result[key] = value
        
        return result
    
    def _load_config_from_file(self, file_path: str) -> Dict[str, Any]:
        """Load configuration from file."""
        try:
            with open(file_path, 'r') as f:
                if file_path.endswith('.json'):
                    return json.load(f)
                elif file_path.endswith(('.yml', '.yaml')):
                    import yaml
                    return yaml.safe_load(f)
                else:
                    raise ValueError(f"Unsupported file format: {file_path}")
        except Exception as e:
            raise ValueError(f"Failed to load config from {file_path}: {e}")
    
    def _validate_config_size(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """Validate configuration size constraints."""
        warnings = []
        errors = []
        
        neuron_count = config.get('brain', {}).get('neuron_count', 0)
        
        # Check if configuration is too large for typical systems
        if neuron_count > 10000:
            warnings.append("Large neuron count (>10,000) may impact performance on smaller systems")
        
        # Check if configuration is too small for meaningful simulation
        if neuron_count < 10:
            errors.append("Very small neuron count (<10) may not produce meaningful results")
        
        return warnings, errors
    
    def _validate_neuron_region_ratio(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """Validate neuron to region ratio."""
        warnings = []
        errors = []
        
        neuron_count = config.get('brain', {}).get('neuron_count', 0)
        region_count = config.get('brain', {}).get('region_count', 1)
        
        if region_count > 0:
            neurons_per_region = neuron_count / region_count
            
            # Check if ratio is reasonable
            if neurons_per_region > 1000:
                warnings.append(f"High neurons per region ({neurons_per_region:.0f}) may impact performance")
            
            if neurons_per_region < 10:
                warnings.append(f"Low neurons per region ({neurons_per_region:.0f}) may not allow complex dynamics")
        
        return warnings, errors
    
    def _validate_plasticity_balance(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """Validate plasticity system balance."""
        warnings = []
        errors = []
        
        plasticity = config.get('plasticity', {})
        
        # Check if any plasticity system is enabled
        stdp_enabled = plasticity.get('stdp', {}).get('enable', False)
        hebbian_enabled = plasticity.get('hebbian', {}).get('enable', False)
        structural_enabled = plasticity.get('structural', {}).get('enable', False)
        
        if not (stdp_enabled or hebbian_enabled or structural_enabled):
            warnings.append("No plasticity systems enabled - brain will not learn or adapt")
        
        # Check if all are enabled (might be too much plasticity)
        if stdp_enabled and hebbian_enabled and structural_enabled:
            warnings.append("All plasticity systems enabled - may lead to unstable learning")
        
        return warnings, errors
    
    def _validate_neuromod_consistency(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """Validate neuromodulation consistency."""
        warnings = []
        errors = []
        
        neuromod = config.get('neuromod', {})
        
        # Check for extreme dopamine scaling
        dopamine_scale = neuromod.get('dopamine', {}).get('scale', 1.0)
        if dopamine_scale > 10:
            warnings.append("High dopamine scale (>10) may cause unstable behavior")
        
        # Check curiosity and novelty consistency
        curiosity_enabled = neuromod.get('curiosity', {}).get('enable', False)
        novelty_enabled = neuromod.get('novelty', {}).get('enable', False)
        
        if curiosity_enabled and not novelty_enabled:
            warnings.append("Curiosity enabled without novelty detection - may cause undirected exploration")
        
        return warnings, errors
    
    def _validate_performance_estimates(self, config: Dict[str, Any]) -> List[str]:
        """Estimate performance characteristics."""
        warnings = []
        
        neuron_count = config.get('brain', {}).get('neuron_count', 1000)
        
        # Estimate memory requirements
        estimated_memory_mb = neuron_count * 1000 / 1024  # Rough estimate in MB
        
        if estimated_memory_mb > 512:
            warnings.append(f"Estimated memory requirement: {estimated_memory_mb:.0f} MB - consider reducing neuron count for smaller systems")
        
        # Estimate computation requirements
        estimated_flops_per_second = neuron_count * 1000
        
        if estimated_flops_per_second > 1e8:
            warnings.append(f"Estimated computational requirement: {estimated_flops_per_second/1e6:.1f} MFLOPS/s - consider reducing neuron count for resource-constrained systems")
        
        return warnings
    
    def _validate_best_practices(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """Validate against best practices."""
        warnings = []
        errors = []
        
        # Check for recommended defaults
        if 'brain' not in config:
            warnings.append("Missing 'brain' section - using defaults")
        
        if 'plasticity' not in config:
            warnings.append("Missing 'plasticity' section - learning may be limited")
        
        if 'neuromod' not in config:
            warnings.append("Missing 'neuromod' section - motivation and exploration may be limited")
        
        # Check timestep
        timestep = config.get('simulation', {}).get('timestep', 0.001)
        if timestep <= 0 or timestep >= 1.0:
            warnings.append(f"Simulation timestep ({timestep}) should be between 0.001 and 1.0 for stability")
        
        return warnings, errors
    
    def _estimate_performance(self, config: Dict[str, Any]) -> List[str]:
        """Estimate performance characteristics."""
        warnings = []
        
        neuron_count = config.get('brain', {}).get('neuron_count', 1000)
        
        # Estimate simulation time for 1000 steps
        estimated_time = neuron_count * 1000 / 1e6  # Rough estimate in seconds
        
        if estimated_time > 60:
            warnings.append(f"Estimated simulation time for 1000 steps: {estimated_time:.1f}s - consider reducing neuron count")
        
        return warnings
    
    def _generate_recommended_actions(self, config: Dict[str, Any], 
                                      errors: List[str], warnings: List[str]) -> List[str]:
        """Generate recommended actions based on validation results."""
        actions = []
        
        if 'neuron_count' not in config.get('brain', {}):
            actions.append("Set brain.neuron_count for proper brain configuration")
        
        if 'connection_probability' not in config.get('brain', {}):
            actions.append("Set brain.connection_probability for proper connectivity")
        
        if not any('plasticity' in config for _ in range(1)):
            actions.append("Consider enabling plasticity systems for learning capabilities")
        
        if not any('neuromod' in config for _ in range(1)):
            actions.append("Consider enabling neuromodulation for motivation and exploration")
        
        if errors:
            actions.append("Fix validation errors listed above")
        
        if warnings:
            actions.append("Address warnings to improve configuration quality")
        
        return actions

class DependencyChecker:
    """Check configuration dependencies."""
    
    def __init__(self):
        self.dependencies = self._load_dependencies()
    
    def _load_dependencies(self) -> Dict[str, List[str]]:
        """Load dependency definitions."""
        return {
            'plasticity_stdp_requires_config': ['brain.initial_weight_mean', 'brain.initial_weight_std'],
            'neuromod_curiosity_requires_plasticity': ['plasticity.stdp.enable'],
            'neuromod_novelty_requires_plasticity': ['plasticity.hebbian.enable'],
            'structural_plasticity_requires_neuron_count': ['brain.neuron_count']
        }
    
    def check_dependencies(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """
        Check configuration dependencies.
        
        Args:
            config: Configuration to check
            
        Returns:
            Tuple of (warnings, errors)
        """
        warnings = []
        errors = []
        
        # Check each dependency
        for dep_name, required_keys in self.dependencies.items():
            missing_keys = []
            
            for key in required_keys:
                # Check if key exists in config
                if not self._check_key_exists(config, key):
                    missing_keys.append(key)
            
            if missing_keys:
                if dep_name.startswith('plasticity'):
                    warnings.append(f"Required keys for {dep_name}: {', '.join(missing_keys)}")
                else:
                    errors.append(f"Missing required keys for {dep_name}: {', '.join(missing_keys)}")
        
        return warnings, errors
    
    def _check_key_exists(self, config: Dict[str, Any], key: str) -> bool:
        """Check if key exists in config (supports dot notation)."""
        parts = key.split('.')
        current = config
        
        for part in parts:
            if part not in current:
                return False
            current = current[part]
        
        return True

class ConfigMigrator:
    """
    Configuration migrator for NLM.
    
    This class provides migration capabilities including:
    - Version migration
    - Schema evolution
    - Breaking change detection
    - Migration planning
    """
    
    def __init__(self):
        self.version_history = self._load_version_history()
        self.migration_rules = self._load_migration_rules()
    
    def _load_version_history(self) -> Dict[str, Any]:
        """Load version history."""
        return {
            'current_version': '1.0.0',
            'versions': [
                {
                    'version': '1.0.0',
                    'description': 'Initial release',
                    'breaking_changes': [],
                    'new_features': ['Basic brain simulation', 'Plasticity systems', 'Neuromodulation']
                },
                {
                    'version': '1.1.0',
                    'description': 'Enhanced features',
                    'breaking_changes': ['Changed config key naming convention'],
                    'new_features': ['Advanced plasticity rules', 'New neuromodulators', 'Better logging']
                },
                {
                    'version': '1.2.0',
                    'description': 'Phase 6 integration',
                    'breaking_changes': ['Restructured configuration hierarchy'],
                    'new_features': ['Memory systems integration', 'Development system', 'Prediction system']
                }
            ]
        }
    
    def _load_migration_rules(self) -> Dict[str, Callable]:
        """Load migration rules."""
        return {
            '1.0.0_to_1.1.0': self._migrate_1_0_0_to_1_1_0,
            '1.1.0_to_1.2.0': self._migrate_1_1_0_to_1_2_0
        }
    
    def migrate(self, from_version: str, to_version: str, config: Dict[str, Any]) -> MigrationResult:
        """
        Migrate configuration from one version to another.
        
        Args:
            from_version: Source version
            to_version: Target version
            config: Configuration to migrate
            
        Returns:
            Migration result
        """
        if from_version == to_version:
            return MigrationResult(
                success=True,
                migrated_config=config,
                warnings=["Source and target versions are the same"],
                breaking_changes=[],
                manual_intervention_required=False
            )
        
        # Check if migration is supported
        migration_key = f"{from_version}_to_{to_version}"
        if migration_key not in self.migration_rules:
            return MigrationResult(
                success=False,
                migrated_config=config,
                warnings=[f"Migration from {from_version} to {to_version} is not supported"],
                breaking_changes=[],
                manual_intervention_required=True
            )
        
        # Apply migration
        try:
            migrated_config = copy.deepcopy(config)
            warnings = []
            breaking_changes = []
            
            # Apply migration rules
            for rule_key, rule_func in self.migration_rules.items():
                if rule_key.startswith(from_version):
                    rule_warnings, rule_breaking_changes = rule_func(migrated_config)
                    warnings.extend(rule_warnings)
                    breaking_changes.extend(rule_breaking_changes)
            
            return MigrationResult(
                success=True,
                migrated_config=migrated_config,
                warnings=warnings,
                breaking_changes=breaking_changes,
                manual_intervention_required=len(breaking_changes) > 0
            )
        
        except Exception as e:
            return MigrationResult(
                success=False,
                migrated_config=config,
                warnings=[f"Migration failed: {e}"],
                breaking_changes=[],
                manual_intervention_required=True
            )
    
    def _migrate_1_0_0_to_1_1_0(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """Migrate from version 1.0.0 to 1.1.0."""
        warnings = []
        breaking_changes = []
        
        # Update configuration keys
        key_mapping = {
            'brain.synaptic_weight': 'brain.initial_weight_mean',
            'brain.synaptic_variance': 'brain.initial_weight_std',
            'plasticity.rate': 'plasticity.stdp.learning_rate',
            'neuromod.exploration': 'neuromod.curiosity.scale'
        }
        
        for old_key, new_key in key_mapping.items():
            if old_key in config:
                config[new_key] = config.pop(old_key)
                breaking_changes.append(f"Configuration key renamed: {old_key} -> {new_key}")
        
        # Update default values
        if 'plasticity.stdp.learning_rate' not in config.get('plasticity', {}):
            config['plasticity']['stdp']['learning_rate'] = 0.001
        
        if 'neuromod.curiosity.scale' not in config.get('neuromod', {}):
            config['neuromod']['curiosity']['scale'] = 1.0
        
        return warnings, breaking_changes
    
    def _migrate_1_1_0_to_1_2_0(self, config: Dict[str, Any]) -> Tuple[List[str], List[str]]:
        """Migrate from version 1.1.0 to 1.2.0."""
        warnings = []
        breaking_changes = []
        
        # Restructure configuration
        if 'plasticity' in config:
            # Move learning rates to new structure
            if 'stdp' not in config['plasticity']:
                config['plasticity']['stdp'] = {}
            
            if 'hebbian' not in config['plasticity']:
                config['plasticity']['hebbian'] = {}
            
            if 'structural' not in config['plasticity']:
                config['plasticity']['structural'] = {}
        
        # Add new default values
        if 'simulation' not in config:
            config['simulation'] = {}
        
        if 'timestep' not in config['simulation']:
            config['simulation']['timestep'] = 0.001
        
        if 'max_steps' not in config['simulation']:
            config['simulation']['max_steps'] = 1000
        
        return warnings, breaking_changes

class ConfigTemplateManager:
    """
    Configuration template manager for NLM.
    
    This class provides template capabilities including:
    - Template storage
    - Template versioning
    - Template application
    - Template comparison
    """
    
    def __init__(self, template_dir: Optional[str] = None):
        """
        Initialize template manager.
        
        Args:
            template_dir: Directory for template storage
        """
        if template_dir is None:
            template_dir = "./nlm_templates"
        
        self.template_dir = Path(template_dir)
        self.template_dir.mkdir(exist_ok=True)
        
        # Load default templates
        self.templates = self._load_default_templates()
    
    def _load_default_templates(self) -> Dict[str, ConfigProfile]:
        """Load default templates."""
        templates = {}
        
        # Quick test template
        templates['quick_test'] = ConfigProfile(
            name='Quick Test',
            description='Quick test configuration for validation',
            config={
                'brain': {
                    'neuron_count': 500,
                    'connection_probability': 0.1
                },
                'plasticity': {
                    'stdp': {'enable': True, 'learning_rate': 0.001},
                    'hebbian': {'enable': True, 'learning_rate': 0.001}
                },
                'neuromod': {
                    'curiosity': {'enable': True, 'scale': 1.0}
                }
            },
            tags=['quick', 'test', 'validation'],
            created=time.time(),
            version='1.0.0'
        )
        
        # Research template
        templates['research'] = ConfigProfile(
            name='Research',
            description='Research-grade configuration for experiments',
            config={
                'brain': {
                    'neuron_count': 5000,
                    'connection_probability': 0.05,
                    'initial_weight_mean': 0.3,
                    'initial_weight_std': 0.2
                },
                'plasticity': {
                    'stdp': {'enable': True, 'learning_rate': 0.01},
                    'hebbian': {'enable': True, 'learning_rate': 0.01},
                    'structural': {'enable': True, 'rate': 0.0001}
                },
                'neuromod': {
                    'curiosity': {'enable': True, 'scale': 2.0},
                    'novelty': {'enable': True, 'threshold': 0.5},
                    'dopamine': {'scale': 2.0}
                }
            },
            tags=['research', 'experiment', 'advanced'],
            created=time.time(),
            version='1.0.0'
        )
        
        # Production template
        templates['production'] = ConfigProfile(
            name='Production',
            description='Production-ready configuration',
            config={
                'brain': {
                    'neuron_count': 2000,
                    'connection_probability': 0.08
                },
                'plasticity': {
                    'stdp': {'enable': True, 'learning_rate': 0.005},
                    'hebbian': {'enable': False}
                },
                'neuromod': {
                    'curiosity': {'enable': False}
                }
            },
            tags=['production', 'stable', 'optimized'],
            created=time.time(),
            version='1.0.0'
        )
        
        return templates
    
    def get_template(self, name: str) -> Optional[ConfigProfile]:
        """
        Get template by name.
        
        Args:
            name: Template name
            
        Returns:
            Template or None if not found
        """
        return self.templates.get(name)
    
    def list_templates(self, tag: Optional[str] = None) -> List[ConfigProfile]:
        """
        List templates.
        
        Args:
            tag: Optional tag to filter by
            
        Returns:
            List of templates
        """
        templates = list(self.templates.values())
        
        if tag:
            templates = [t for t in templates if tag in t.tags]
        
        return templates
    
    def save_template(self, profile: ConfigProfile):
        """
        Save template.
        
        Args:
            profile: Template profile
        """
        self.templates[profile.name] = profile
        
        # Save to file
        template_file = self.template_dir / f"{profile.name}.json"
        with open(template_file, 'w') as f:
            json.dump(asdict(profile), f, indent=2)
    
    def load_template(self, name: str) -> Optional[ConfigProfile]:
        """
        Load template from file.
        
        Args:
            name: Template name
            
        Returns:
            Template or None if not found
        """
        template_file = self.template_dir / f"{name}.json"
        
        if not template_file.exists():
            return None
        
        with open(template_file, 'r') as f:
            data = json.load(f)
        
        return ConfigProfile(**data)

class AutoConfigGenerator:
    """
    Automated configuration generator for NLM.
    
    This class provides auto-configuration capabilities including:
    - Target-based generation
    - Constraint optimization
    - Performance prediction
    - Automated tuning
    """
    
    def __init__(self):
        self.performance_model = self._load_performance_model()
        self.constraint_checker = ConstraintChecker()
    
    def _load_performance_model(self) -> Dict[str, Any]:
        """Load performance model."""
        return {
            'memory_usage': lambda neurons, regions: neurons * 1000 / 1024,  # MB
            'computation_time': lambda neurons, steps: neurons * steps / 1e6,  # seconds
            'stability': lambda config: self._estimate_stability(config),
            'learning_capacity': lambda config: self._estimate_learning_capacity(config)
        }
    
    def generate(self, target_performance: Dict[str, Any], 
                constraints: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Generate configuration based on target performance.
        
        args:
            target_performance: Target performance metrics
            constraints: Configuration constraints
            
        Returns:
            Generated configuration
        """
        if constraints is None:
            constraints = {}
        
        # Initialize search
        best_config = None
        best_score = float('inf')
        
        # Generate configurations
        for neuron_count in self._generate_neuron_counts(target_performance, constraints):
            for region_count in self._generate_region_counts(neuron_count, constraints):
                for connection_probability in self._generate_connection_probabilities(constraints):
                    config = {
                        'brain': {
                            'neuron_count': neuron_count,
                            'region_count': region_count,
                            'connection_probability': connection_probability
                        }
                    }
                    
                    # Apply constraints
                    if not self.constraint_checker.check_constraints(config, constraints):
                        continue
                    
                    # Evaluate performance
                    score = self._evaluate_config(config, target_performance)
                    
                    if score < best_score:
                        best_score = score
                        best_config = config
        
        return best_config if best_config else {}
    
    def _generate_neuron_counts(self, target_performance: Dict[str, Any], 
                              constraints: Dict[str, Any]) -> List[int]:
        """Generate possible neuron counts."""
        # Based on memory constraints
        max_neurons = constraints.get('max_memory_mb', 512) * 1024 / 1000
        min_neurons = constraints.get('min_neuron_count', 100)
        
        # Based on target computation time
        target_time = target_performance.get('max_computation_time', 60)
        if target_time > 0:
            estimated_neurons = target_time * 1e6 / 1000  # Rough estimate
            max_neurons = min(max_neurons, estimated_neurons * 2)
        
        # Generate values around target
        target_neuron = target_performance.get('target_neuron_count', 1000)
        
        return [
            max(min_neurons, int(target_neuron * 0.5)),
            target_neuron,
            min(max_neurons, int(target_neuron * 2))
        ]
    
    def _generate_region_counts(self, neuron_count: int, 
                              constraints: Dict[str, Any]) -> List[int]:
        """Generate possible region counts."""
        max_regions = constraints.get('max_regions', 10)
        
        # Ensure neurons per region is reasonable
        neurons_per_region = constraints.get('min_neurons_per_region', 100)
        max_possible_regions = neuron_count // neurons_per_region
        
        return [
            max(1, min(max_regions, neuron_count // 500)),
            max(1, min(max_regions, neuron_count // 200)),
            max(1, min(max_regions, max_possible_regions))
        ]
    
    def _generate_connection_probabilities(self, constraints: Dict[str, Any]) -> List[float]:
        """Generate possible connection probabilities."""
        return [0.05, 0.1, 0.2, 0.3]
    
    def _evaluate_config(self, config: Dict[str, Any], 
                        target_performance: Dict[str, Any]) -> float:
        """Evaluate configuration against targets."""
        score = 0.0
        
        # Memory usage
        memory_usage = self.performance_model['memory_usage'](
            config['brain']['neuron_count'],
            config['brain']['region_count']
        )
        target_memory = target_performance.get('max_memory_mb', 512)
        score += abs(memory_usage - target_memory) / target_memory
        
        # Computation time
        comp_time = self.performance_model['computation_time'](
            config['brain']['neuron_count'],
            target_performance.get('target_steps', 1000)
        )
        target_time = target_performance.get('max_computation_time', 60)
        score += abs(comp_time - target_time) / target_time
        
        # Stability
        stability = self.performance_model['stability'](config)
        target_stability = target_performance.get('target_stability', 0.8)
        score += abs(stability - target_stability)
        
        # Learning capacity
        learning_capacity = self.performance_model['learning_capacity'](config)
        target_learning = target_performance.get('target_learning_capacity', 0.5)
        score += abs(learning_capacity - target_learning)
        
        return score
    
    def _estimate_stability(self, config: Dict[str, Any]) -> float:
        """Estimate configuration stability."""
        # Simplified stability estimate
        stability = 1.0
        
        # Reduce stability for extreme values
        neuron_count = config['brain']['neuron_count']
        if neuron_count > 10000:
            stability *= 0.8
        
        connection_prob = config['brain']['connection_probability']
        if connection_prob < 0.01:
            stability *= 0.9
        elif connection_prob > 0.5:
            stability *= 0.7
        
        return stability
    
    def _estimate_learning_capacity(self, config: Dict[str, Any]) -> float:
        """Estimate learning capacity."""
        # Simplified learning capacity estimate
        learning_capacity = 0.0
        
        # Add contributions from plasticity systems
        plasticity = config.get('plasticity', {})
        
        if plasticity.get('stdp', {}).get('enable', False):
            learning_capacity += 0.3
        
        if plasticity.get('hebbian', {}).get('enable', False):
            learning_capacity += 0.3
        
        if plasticity.get('structural', {}).get('enable', False):
            learning_capacity += 0.4
        
        return learning_capacity

class ConstraintChecker:
    """Check configuration constraints."""
    
    def check_constraints(self, config: Dict[str, Any], 
                         constraints: Dict[str, Any]) -> bool:
        """
        Check if configuration meets constraints.
        
        Args:
            config: Configuration to check
            constraints: Constraints to check against
            
        Returns:
            True if constraints are met, False otherwise
        """
        # Check memory constraint
        if 'max_memory_mb' in constraints:
            memory_usage = config['brain']['neuron_count'] * 1000 / 1024
            if memory_usage > constraints['max_memory_mb']:
                return False
        
        # Check computation constraint
        if 'max_computation_time' in constraints:
            comp_time = config['brain']['neuron_count'] * 1000 / 1e6
            if comp_time > constraints['max_computation_time']:
                return False
        
        # Check neuron count constraint
        if 'min_neuron_count' in constraints:
            if config['brain']['neuron_count'] < constraints['min_neuron_count']:
                return False
        
        # Check region count constraint
        if 'max_regions' in constraints:
            if config['brain']['region_count'] > constraints['max_regions']:
                return False
        
        # Check connection probability constraint
        if 'max_connection_probability' in constraints:
            if config['brain']['connection_probability'] > constraints['max_connection_probability']:
                return False
        
        return True

# Convenience functions for configuration management
def createConfigValidator(schema_path: Optional[str] = None) -> ConfigValidator:
    """Create configuration validator."""
    return ConfigValidator(schema_path)

def createConfigMigrator() -> ConfigMigrator:
    """Create configuration migrator."""
    return ConfigMigrator()

def createTemplateManager(template_dir: Optional[str] = None) -> ConfigTemplateManager:
    """Create template manager."""
    return ConfigTemplateManager(template_dir)

def createAutoGenerator() -> AutoConfigGenerator:
    """Create auto configuration generator."""
    return AutoConfigGenerator()