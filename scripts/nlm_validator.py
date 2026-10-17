#!/usr/bin/env python3
"""
NLM Configuration Validator
Advanced validation and error handling system for Neural Learning Machine

Features:
- Comprehensive configuration validation
- Automatic error recovery and backup
- Configuration compatibility checking
- Performance validation
- Error reporting and logging
- Automated fixes for common issues
"""

import os
import sys
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple, Union
import copy
import re
import statistics
from datetime import datetime

class ConfigurationError(Exception):
    """Configuration error exception."""
    pass

class ValidationWarning(Warning):
    """Validation warning."""
    pass

class ConfigValidator:
    """Comprehensive configuration validator for NLM."""
    
    def __init__(self, config: Dict = None):
        self.config = config or {}
        self.errors = []
        self.warnings = []
        self.fixes_applied = []
        self.backup_config = copy.deepcopy(self.config)
    
    def validate(self) -> Tuple[bool, List[str], List[str]]:
        """Validate configuration and return results."""
        self.errors.clear()
        self.warnings.clear()
        self.fixes_applied.clear()
        
        # Run all validation checks
        self._validate_structure()
        self._validate_values()
        self._validate_dependencies()
        self._validate_performance()
        self._validate_file_paths()
        
        return len(self.errors) == 0, self.errors, self.warnings
    
    def auto_fix(self) -> Tuple[int, List[str]]:
        """Automatically fix common issues."""
        fixes = 0
        applied_fixes = []
        
        # Fix 1: Validate configuration sections
        if 'sections' not in self.config:
            self.config['sections'] = ['general', 'memory', 'plasticity', 'development', 'neuromodulation']
            applied_fixes.append("Added missing 'sections' field")
            fixes += 1
        
        # Fix 2: Ensure required fields exist
        required_fields = self._get_required_fields()
        for field in required_fields:
            if not self._field_exists(field):
                default_value = self._get_default_value(field)
                if default_value is not None:
                    self._set_field_value(field, default_value)
                    applied_fixes.append(f"Added missing field: {field}")
                    fixes += 1
        
        # Fix 3: Validate field types
        type_issues = self._validate_field_types()
        for field, expected_type in type_issues:
            current_value = self._get_field_value(field)
            fixed_value = self._convert_to_type(current_value, expected_type)
            if fixed_value is not None:
                self._set_field_value(field, fixed_value)
                applied_fixes.append(f"Fixed field type: {field} -> {expected_type}")
                fixes += 1
        
        # Fix 4: Range validation
        range_issues = self._validate_value_ranges()
        for field, min_val, max_val in range_issues:
            current_value = self._get_field_value(field)
            if isinstance(current_value, (int, float)):
                if current_value < min_val or current_value > max_val:
                    # Use midpoint as fix
                    fixed_value = (min_val + max_val) / 2
                    self._set_field_value(field, fixed_value)
                    applied_fixes.append(f"Fixed field range: {field} to {fixed_value}")
                    fixes += 1
        
        self.fixes_applied = applied_fixes
        return fixes, applied_fixes
    
    def _validate_structure(self):
        """Validate configuration structure."""
        # Check if config is a dict
        if not isinstance(self.config, dict):
            self.errors.append("Configuration must be a dictionary/object")
            return
        
        # Check for empty config
        if not self.config:
            self.warnings.append("Configuration is empty - using defaults")
        
        # Validate key-value pairs
        for key, value in self.config.items():
            if not isinstance(key, str):
                self.warnings.append(f"Configuration key '{key}' should be a string")
            
            if value is None:
                self.warnings.append(f"Configuration value for '{key}' is None")
    
    def _validate_values(self):
        """Validate configuration values."""
        # Get known valid configurations
        valid_configs = self._get_valid_configurations()
        
        # Check if config matches any valid pattern
        config_signature = self._get_config_signature()
        if config_signature not in valid_configs:
            self.warnings.append("Configuration doesn't match known valid patterns")
        
        # Validate specific fields
        field_validations = self._get_field_validations()
        
        for field, validation_rules in field_validations.items():
            if field in self.config:
                value = self.config[field]
                self._validate_field_value(field, value, validation_rules)
    
    def _validate_dependencies(self):
        """Validate configuration dependencies."""
        # Check for required dependencies between fields
        dependencies = [
            ("neuron_count", "min_neuron_count", 10),
            ("simulation_timestep", "min_timestep", 0.0001),
            ("region_count", "min_region_count", 1),
            ("connection_probability", "max_connection_prob", 1.0),
            ("plasticity_learning_rate", "max_plasticity_rate", 1.0),
        ]
        
        for main_field, dep_field, min_value in dependencies:
            if main_field in self.config:
                main_value = self.config[main_field]
                if dep_field in self.config:
                    dep_value = self.config[dep_field]
                    if main_value < dep_value:
                        self.warnings.append(
                            f"{main_field} ({main_value}) should be >= {dep_field} ({dep_value})"
                        )
    
    def _validate_performance(self):
        """Validate performance-related settings."""
        perf_settings = [
            ("performance_use_memory_pools", bool),
            ("performance_enable_simd", bool),
            ("performance_lock_free_structures", bool),
            ("visualization_enabled", bool),
        ]
        
        for field, expected_type in perf_settings:
            if field in self.config:
                value = self.config[field]
                if not isinstance(value, expected_type):
                    self.warnings.append(
                        f"{field} should be {expected_type.__name__}, got {type(value).__name__}"
                    )
    
    def _validate_file_paths(self):
        """Validate file paths in configuration."""
        path_fields = [
            "checkpoint_path",
            "log_filename",
            "visualization_output_path",
        ]
        
        for field in path_fields:
            if field in self.config:
                path = self.config[field]
                if not isinstance(path, str):
                    self.warnings.append(f"{field} should be a string path")
                else:
                    # Check if path exists (if it should)
                    if "checkpoint" in field and path:
                        if not os.path.exists(path):
                            self.warnings.append(f"Checkpoint path does not exist: {path}")
    
    def _validate_field_value(self, field: str, value: Any, validation_rules: Dict):
        """Validate a single field value."""
        if "type" in validation_rules:
            expected_type = validation_rules["type"]
            if not isinstance(value, expected_type):
                self.warnings.append(
                    f"{field} should be {expected_type.__name__}, got {type(value).__name__}"
                )
        
        if "range" in validation_rules:
            min_val, max_val = validation_rules["range"]
            if isinstance(value, (int, float)):
                if value < min_val or value > max_val:
                    self.warnings.append(
                        f"{field} ({value}) should be in range [{min_val}, {max_val}]"
                    )
        
        if "enum" in validation_rules:
            if value not in validation_rules["enum"]:
                self.warnings.append(
                    f"{field} ({value}) should be one of: {validation_rules['enum']}"
                )
    
    def _get_required_fields(self) -> List[str]:
        """Get list of required configuration fields."""
        return [
            "neuron_count",
            "region_count",
            "simulation_timestep",
            "random_seed",
            "connection_probability",
            "plasticity_learning_rate",
            "stdp_ltp_weight",
            "stdp_ltd_weight",
            "stdp_time_constant",
            "development_synaptogenesis_rate",
            "development_pruning_rate",
            "dopamine_baseline",
            "reward_discount_factor",
            "environment_name",
            "log_level",
            "max_simulation_steps",
        ]
    
    def _get_field_validations(self) -> Dict[str, Dict]:
        """Get field-specific validation rules."""
        return {
            "neuron_count": {"type": int, "range": (1, 100000)},
            "region_count": {"type": int, "range": (1, 100)},
            "simulation_timestep": {"type": float, "range": (0.0001, 1.0)},
            "random_seed": {"type": int, "range": (0, 2**31-1)},
            "connection_probability": {"type": float, "range": (0.0, 1.0)},
            "plasticity_learning_rate": {"type": float, "range": (0.0, 1.0)},
            "stdp_ltp_weight": {"type": float, "range": (0.0, 1.0)},
            "stdp_ltd_weight": {"type": float, "range": (0.0, 1.0)},
            "stdp_time_constant": {"type": float, "range": (0.0, 1000.0)},
            "development_synaptogenesis_rate": {"type": float, "range": (0.0, 0.1)},
            "development_pruning_rate": {"type": float, "range": (0.0, 0.01)},
            "dopamine_baseline": {"type": float, "range": (0.0, 1.0)},
            "reward_discount_factor": {"type": float, "range": (0.0, 1.0)},
            "log_level": {"type": str, "enum": ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]},
            "max_simulation_steps": {"type": int, "range": (1, 1000000)},
        }
    
    def _get_valid_configurations(self) -> List[str]:
        """Get list of valid configuration signatures."""
        return [
            "basic_research",
            "production",
            "development",
            "performance_test",
        ]
    
    def _get_config_signature(self) -> str:
        """Generate a signature for the current configuration."""
        # Create a signature based on key configuration values
        signature_values = []
        
        key_fields = ["neuron_count", "region_count", "simulation_timestep", "random_seed"]
        for field in key_fields:
            if field in self.config:
                signature_values.append(f"{field}={self.config[field]}")
        
        return "|".join(signature_values) if signature_values else "empty"
    
    def _field_exists(self, field: str) -> bool:
        """Check if a field exists in configuration."""
        return field in self.config
    
    def _get_field_value(self, field: str) -> Any:
        """Get field value if it exists."""
        return self.config.get(field)
    
    def _set_field_value(self, field: str, value: Any):
        """Set field value."""
        self.config[field] = value
    
    def _get_default_value(self, field: str) -> Any:
        """Get default value for a field."""
        defaults = {
            "neuron_count": 1000,
            "region_count": 2,
            "simulation_timestep": 0.001,
            "random_seed": 42,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_time_constant": 20.0,
            "development_synaptogenesis_rate": 0.0001,
            "development_pruning_rate": 0.00001,
            "dopamine_baseline": 0.1,
            "reward_discount_factor": 0.99,
            "environment_name": "GridWorld",
            "log_level": "INFO",
            "max_simulation_steps": 10000,
            "performance_use_memory_pools": True,
            "performance_enable_simd": True,
            "visualization_enabled": True,
        }
        
        return defaults.get(field)
    
    def _validate_field_types(self) -> List[Tuple[str, type]]:
        """Validate field types and return issues."""
        issues = []
        field_types = self._get_field_type_expectations()
        
        for field, expected_type in field_types.items():
            if field in self.config:
                current_value = self.config[field]
                if not isinstance(current_value, expected_type):
                    issues.append((field, expected_type))
        
        return issues
    
    def _get_field_type_expectations(self) -> Dict[str, type]:
        """Get expected types for fields."""
        return {
            "neuron_count": int,
            "region_count": int,
            "simulation_timestep": float,
            "random_seed": int,
            "connection_probability": float,
            "plasticity_learning_rate": float,
            "stdp_ltp_weight": float,
            "stdp_ltd_weight": float,
            "stdp_time_constant": float,
            "development_synaptogenesis_rate": float,
            "development_pruning_rate": float,
            "dopamine_baseline": float,
            "reward_discount_factor": float,
            "log_level": str,
            "max_simulation_steps": int,
            "performance_use_memory_pools": bool,
            "performance_enable_simd": bool,
            "visualization_enabled": bool,
        }
    
    def _convert_to_type(self, value: Any, target_type: type) -> Any:
        """Convert value to target type."""
        try:
            if target_type == int:
                return int(float(value)) if isinstance(value, (int, float, str)) else None
            elif target_type == float:
                return float(value) if isinstance(value, (int, float, str)) else None
            elif target_type == str:
                return str(value)
            elif target_type == bool:
                if isinstance(value, bool):
                    return value
                elif isinstance(value, str):
                    return value.lower() in ('true', 'yes', '1', 'on')
                elif isinstance(value, (int, float)):
                    return bool(value)
            elif target_type == list:
                return list(value) if not isinstance(value, list) else value
            elif target_type == dict:
                return dict(value) if not isinstance(value, dict) else value
        except (ValueError, TypeError):
            pass
        
        return None
    
    def _validate_value_ranges(self) -> List[Tuple[str, float, float]]:
        """Validate value ranges and return issues."""
        issues = []
        range_checks = [
            ("neuron_count", 1, 100000),
            ("region_count", 1, 100),
            ("simulation_timestep", 0.0001, 1.0),
            ("random_seed", 0, 2**31-1),
            ("connection_probability", 0.0, 1.0),
            ("plasticity_learning_rate", 0.0, 1.0),
            ("stdp_ltp_weight", 0.0, 1.0),
            ("stdp_ltd_weight", 0.0, 1.0),
            ("stdp_time_constant", 0.0, 1000.0),
            ("development_synaptogenesis_rate", 0.0, 0.1),
            ("development_pruning_rate", 0.0, 0.01),
            ("dopamine_baseline", 0.0, 1.0),
            ("reward_discount_factor", 0.0, 1.0),
            ("max_simulation_steps", 1, 1000000),
        ]
        
        for field, min_val, max_val in range_checks:
            if field in self.config:
                value = self.config[field]
                if isinstance(value, (int, float)):
                    if value < min_val or value > max_val:
                        issues.append((field, min_val, max_val))
        
        return issues

class ConfigurationManager:
    """Manager for NLM configuration with validation and recovery."""
    
    def __init__(self, config_file: str = None):
        self.config_file = config_file
        self.current_config = {}
        self.logger = None
        
        if config_file and Path(config_file).exists():
            self.load_config(config_file)
        
        self.validator = ConfigValidator(self.current_config)
    
    def load_config(self, config_file: str):
        """Load configuration from file."""
        try:
            if config_file.endswith('.json'):
                with open(config_file, 'r') as f:
                    self.current_config = json.load(f)
            elif config_file.endswith(('.yaml', '.yml')):
                with open(config_file, 'r') as f:
                    self.current_config = yaml.safe_load(f)
            else:
                # Try CFG file format (key=value)
                self._load_cfg_file(config_file)
            
            self.validator = ConfigValidator(self.current_config)
            print(f"✅ Configuration loaded from: {config_file}")
            
        except Exception as e:
            print(f"❌ Error loading configuration: {e}")
            self.current_config = {}
    
    def _load_cfg_file(self, config_file: str):
        """Load configuration from .cfg file format."""
        config = {}
        with open(config_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    if '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Convert value to appropriate type
                        if value.lower() == 'true':
                            config[key] = True
                        elif value.lower() == 'false':
                            config[key] = False
                        elif value.isdigit():
                            config[key] = int(value)
                        else:
                            try:
                                config[key] = float(value)
                            except ValueError:
                                config[key] = value
        
        self.current_config = config
    
    def save_config(self, config_file: str = None):
        """Save configuration to file."""
        if not config_file:
            config_file = self.config_file
        
        if not config_file:
            print("❌ No configuration file specified")
            return
        
        os.makedirs(os.path.dirname(config_file), exist_ok=True)
        
        try:
            if config_file.endswith('.json'):
                with open(config_file, 'w') as f:
                    json.dump(self.current_config, f, indent=2)
            elif config_file.endswith(('.yaml', '.yml')):
                with open(config_file, 'w') as f:
                    yaml.dump(self.current_config, f, default_flow_style=False)
            else:
                # Save as CFG file
                self._save_cfg_file(config_file)
            
            print(f"✅ Configuration saved to: {config_file}")
            
        except Exception as e:
            print(f"❌ Error saving configuration: {e}")
    
    def _save_cfg_file(self, config_file: str):
        """Save configuration as .cfg file."""
        with open(config_file, 'w') as f:
            f.write("# NLM Configuration File\n")
            f.write(f"# Generated at {datetime.now().isoformat()}\n\n")
            
            for key, value in sorted(self.current_config.items()):
                if isinstance(value, bool):
                    f.write(f"{key} = {str(value).lower()}\n")
                elif isinstance(value, (int, float)):
                    f.write(f"{key} = {value}\n")
                else:
                    f.write(f'{key} = "{value}"\n')
    
    def validate_configuration(self) -> Tuple[bool, List[str], List[str]]:
        """Validate current configuration."""
        return self.validator.validate()
    
    def auto_fix_configuration(self) -> Tuple[int, List[str]]:
        """Automatically fix configuration issues."""
        return self.validator.auto_fix()
    
    def set_value(self, key: str, value: Any):
        """Set configuration value."""
        self.current_config[key] = value
    
    def get_value(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
        return self.current_config.get(key, default)
    
    def get_all(self) -> Dict:
        """Get all configuration values."""
        return self.current_config.copy()
    
    def compare_configurations(self, other_config: Dict) -> Dict:
        """Compare two configurations and return differences."""
        diff = {
            'added': {},
            'removed': {},
            'modified': {}
        }
        
        self_keys = set(self.current_config.keys())
        other_keys = set(other_config.keys())
        
        # Find added keys
        for key in other_keys - self_keys:
            diff['added'][key] = other_config[key]
        
        # Find removed keys
        for key in self_keys - other_keys:
            diff['removed'][key] = self.current_config[key]
        
        # Find modified keys
        for key in self_keys & other_keys:
            if self.current_config[key] != other_config[key]:
                diff['modified'][key] = {
                    'old': self.current_config[key],
                    'new': other_config[key]
                }
        
        return diff

class ValidatorManager:
    """Manager for NLM configuration validation."""
    
    def __init__(self, config_file: str = None):
        self.config_manager = ConfigurationManager(config_file)
    
    def run_validation(self):
        """Run complete validation process."""
        print("🔍 NLM Configuration Validation")
        print("=" * 50)
        
        # Validate configuration
        is_valid, errors, warnings = self.config_manager.validate_configuration()
        
        print(f"\n📋 Validation Results:")
        print(f"   Status: {'✅ PASS' if is_valid else '❌ FAIL'}")
        print(f"   Errors: {len(errors)}")
        print(f"   Warnings: {len(warnings)}")
        
        if errors:
            print(f"\n❌ ERRORS:")
            for error in errors:
                print(f"   ✗ {error}")
        
        if warnings:
            print(f"\n⚠️  WARNINGS:")
            for warning in warnings:
                print(f"   ⚠ {warning}")
        
        # Auto-fix if requested
        fix_all = input(f"\nAuto-fix common issues? (y/N): ").strip().lower() == 'y'
        if fix_all:
            fixes, applied = self.config_manager.auto_fix_configuration()
            print(f"\n🔧 Auto-fix Results:")
            print(f"   Fixes applied: {fixes}")
            print(f"   Applied fixes: {len(applied)}")
            
            if applied:
                print(f"   Applied: {', '.join(applied[:5])}")
                if len(applied) > 5:
                    print(f"   ... and {len(applied) - 5} more")
        
        # Save configuration if valid
        if is_valid:
            save = input(f"\nSave corrected configuration? (y/N): ").strip().lower() == 'y'
            if save:
                config_file = input(f"Configuration file path (default: configs/validated.cfg): ")
                if not config_file:
                    config_file = "configs/validated.cfg"
                self.config_manager.save_config(config_file)
        
        return is_valid

class ValidationScript:
    """Validation script for various NLM configurations."""
    
    def __init__(self):
        self.configurations_dir = "configs/validation"
        os.makedirs(self.configurations_dir, exist_ok=True)
    
    def create_validation_configs(self):
        """Create a set of validation configurations."""
        validations = [
            {
                "name": "minimal_configuration",
                "description": "Minimal working configuration",
                "config": {
                    "neuron_count": 100,
                    "region_count": 1,
                    "simulation_timestep": 0.001,
                    "random_seed": 42,
                    "connection_probability": 0.1,
                }
            },
            {
                "name": "research_configuration",
                "description": "Configuration optimized for research",
                "config": {
                    "neuron_count": 2000,
                    "region_count": 4,
                    "simulation_timestep": 0.001,
                    "random_seed": 123,
                    "connection_probability": 0.15,
                    "plasticity_learning_rate": 0.005,
                    "performance_use_memory_pools": True,
                    "visualization_enabled": True,
                }
            },
            {
                "name": "production_configuration",
                "description": "Production-ready configuration",
                "config": {
                    "neuron_count": 5000,
                    "region_count": 8,
                    "simulation_timestep": 0.0001,
                    "random_seed": 999,
                    "connection_probability": 0.05,
                    "performance_use_memory_pools": True,
                    "performance_enable_simd": True,
                    "performance_lock_free_structures": True,
                    "visualization_enabled": False,
                }
            },
            {
                "name": "experimental_configuration",
                "description": "Configuration for experimental studies",
                "config": {
                    "neuron_count": 10000,
                    "region_count": 16,
                    "simulation_timestep": 0.0005,
                    "random_seed": 777,
                    "connection_probability": 0.02,
                    "plasticity_learning_rate": 0.001,
                    "development_synaptogenesis_rate": 0.001,
                    "development_pruning_rate": 0.0005,
                    "neuromod_curiosity_enable": True,
                    "neuromod_novelty_enable": True,
                    "prediction_enable": True,
                }
            }
        ]
        
        for validation in validations:
            config_file = Path(self.configurations_dir) / f"{validation['name']}.json"
            
            with open(config_file, 'w') as f:
                json.dump({
                    "name": validation["name"],
                    "description": validation["description"],
                    "config": validation["config"],
                    "created_at": datetime.now().isoformat()
                }, f, indent=2)
            
            print(f"✓ Created: {config_file}")
    
    def run_validation_suite(self):
        """Run validation suite on all configurations."""
        print("🧪 NLM Configuration Validation Suite")
        print("=" * 50)
        
        config_files = list(Path(self.configurations_dir).glob("*.json"))
        
        if not config_files:
            print("❌ No validation configurations found")
            return
        
        results = []
        
        for config_file in config_files:
            print(f"\n📋 Validating: {config_file.stem}")
            print("-" * 40)
            
            try:
                with open(config_file, 'r') as f:
                    validation_data = json.load(f)
                
                config = validation_data["config"]
                name = validation_data["name"]
                description = validation_data["description"]
                
                validator = ConfigValidator(config)
                is_valid, errors, warnings = validator.validate()
                
                result = {
                    "name": name,
                    "description": description,
                    "valid": is_valid,
                    "errors": errors,
                    "warnings": warnings,
                    "file": config_file
                }
                
                results.append(result)
                
                print(f"   Status: {'✅ PASS' if is_valid else '❌ FAIL'}")
                print(f"   Errors: {len(errors)}, Warnings: {len(warnings)}")
                
                if errors:
                    for error in errors[:3]:  # Show first 3 errors
                        print(f"   ✗ {error}")
                    if len(errors) > 3:
                        print(f"   ... and {len(errors) - 3} more")
                
            except Exception as e:
                print(f"   ❌ Error: {e}")
                results.append({
                    "name": config_file.stem,
                    "valid": False,
                    "error": str(e)
                })
        
        # Summary
        print(f"\n📊 Validation Suite Summary:")
        print(f"   Total configurations: {len(results)}")
        
        valid_count = sum(1 for r in results if r.get('valid', False))
        print(f"   Valid: {valid_count}")
        print(f"   Invalid: {len(results) - valid_count}")
        
        # Generate report
        self._generate_validation_report(results)
    
    def _generate_validation_report(self, results: List[Dict]):
        """Generate validation report."""
        report_file = Path(self.configurations_dir) / "validation_report.json"
        
        report = {
            "timestamp": datetime.now().isoformat(),
            "total_configurations": len(results),
            "valid_configurations": sum(1 for r in results if r.get('valid', False)),
            "invalid_configurations": sum(1 for r in results if not r.get('valid', False)),
            "results": results
        }
        
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\n📄 Validation report saved to: {report_file}")

class ValidatorManager:
    """Manager for NLM validation system."""
    
    def __init__(self):
        self.validation_script = ValidationScript()
    
    def run_validation(self, args: argparse.Namespace):
        """Run validation."""
        if args.create:
            self.validation_script.create_validation_configs()
            print("✅ Validation configurations created")
        
        if args.suite:
            self.validation_script.run_validation_suite()
        
        if not args.create and not args.suite:
            print("❌ No validation option specified")
            self.validation_script.create_validation_configs()
            print("💡 Created validation configurations")
            print("💡 Run with --suite to validate all configurations")

def main():
    validator_manager = ValidatorManager()

    parser = argparse.ArgumentParser(
        description="NLM Configuration Validator",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Create validation configurations
    create_parser = subparsers.add_parser("create", help="Create validation configurations")

    # Run validation suite
    subparsers.add_parser("suite", help="Run validation suite")

    args = parser.parse_args()

    validator_manager.run_validation(args)

if __name__ == "__main__":
    main()
