"""
Advanced configuration utilities for NLM neural simulations.

Provides configuration comparison and diffing, validation and schema checking,
configuration backup and restore, and configuration migration tools.
"""

import json
import yaml
import hashlib
import shutil
import tempfile
import os
from typing import Dict, List, Any, Optional, Union
from datetime import datetime
import copy
import difflib
from pathlib import Path
from dataclasses import dataclass, asdict
import jsonschema
from jsonschema import validate

@dataclass
class ConfigDiff:
    """Represents a difference between two configurations."""
    key_path: str
    old_value: Any
    new_value: Any
    change_type: str  # 'added', 'removed', 'modified'
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'key_path': self.key_path,
            'old_value': self.old_value,
            'new_value': self.new_value,
            'change_type': self.change_type
        }
class ConfigurationUtils:
    """
    Advanced configuration utilities for NLM neural simulations.
    
    Features:
    - Configuration comparison and diffing
    - Configuration validation and schema checking
    - Configuration backup and restore
    - Configuration migration tools
    """
    
    def __init__(self):
        self.config_schemas: Dict[str, Dict[str, Any]] = {}
        self.backup_dir = Path("config_backups")
        self.backup_dir.mkdir(exist_ok=True)
        
    def load_config(self, filepath: str) -> Dict[str, Any]:
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            Loaded configuration dictionary
        """
        filepath = Path(filepath)
        
        if filepath.suffix in ['.json', '.JSON']:
            with open(filepath, 'r') as f:
                return json.load(f)
        elif filepath.suffix in ['.yaml', '.yml', '.YAML', '.YML']:
            with open(filepath, 'r') as f:
                return yaml.safe_load(f)
        else:
            raise ValueError(f"Unsupported file format: {filepath.suffix}")
            
    def save_config(self, config: Dict[str, Any], filepath: str):
        """
        Save configuration to file.
        
        Args:
            config: Configuration dictionary
            filepath: Output file path
        """
        filepath = Path(filepath)
        
        if filepath.suffix in ['.json', '.JSON']:
            with open(filepath, 'w') as f:
                json.dump(config, f, indent=2)
        elif filepath.suffix in ['.yaml', '.yml', '.YAML', '.YML']:
            with open(filepath, 'w') as f:
                yaml.dump(config, f, default_flow_style=False)
        else:
            raise ValueError(f"Unsupported file format: {filepath.suffix}")
            
    def compute_hash(self, config: Dict[str, Any]) -> str:
        """
        Compute hash of configuration for integrity checking.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            SHA256 hash string
        """
        config_str = json.dumps(config, sort_keys=True, separators=(',', ':'))
        return hashlib.sha256(config_str.encode()).hexdigest()
        
    def compare_configs(self, config1: Dict[str, Any], config2: Dict[str, Any],
                       show_differences: bool = True) -> Dict[str, Any]:
        """
        Compare two configurations and identify differences.
        
        Args:
            config1: First configuration
            config2: Second configuration
            show_differences: Whether to include detailed differences
            
        Returns:
            Comparison results including summary and differences
        """
        results = {
            'identical': config1 == config2,
            'hash1': self.compute_hash(config1),
            'hash2': self.compute_hash(config2),
            'differences': []
        }
        
        if not show_differences or config1 == config2:
            return results
            
        # Use recursive comparison to find differences
        def _compare_dicts(d1: Dict[str, Any], d2: Dict[str, Any],
                          path: str = "") -> List[ConfigDiff]:
            differences = []
            
            # Check keys in both dictionaries
            all_keys = set(d1.keys()) | set(d2.keys())
            
            for key in sorted(all_keys):
                current_path = f"{path}.{key}" if path else key
                
                if key in d1 and key in d2:
                    # Both have this key
                    if isinstance(d1[key], dict) and isinstance(d2[key], dict):
                        # Recursively compare nested dictionaries
                        differences.extend(_compare_dicts(d1[key], d2[key], current_path))
                    elif d1[key] != d2[key]:
                        # Values differ
                        differences.append(ConfigDiff(
                            key_path=current_path,
                            old_value=d1[key],
                            new_value=d2[key],
                            change_type='modified'
                        ))
                elif key in d1:
                    # Only in first config
                    differences.append(ConfigDiff(
                        key_path=current_path,
                        old_value=d1[key],
                        new_value=None,
                        change_type='removed'
                    ))
                else:
                    # Only in second config
                    differences.append(ConfigDiff(
                        key_path=current_path,
                        old_value=None,
                        new_value=d2[key],
                        change_type='added'
                    ))
            
            return differences
            
        results['differences'] = [_compare_dict.to_dict() for _compare_dict in
                                 _compare_dicts(config1, config2)]
                                 
        return results
        
    def validate_config(self, config: Dict[str, Any], schema: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Validate configuration against schema.
        
        Args:
            config: Configuration to validate
            schema: JSON schema to validate against (optional)
            
        Returns:
            Validation results with errors and warnings
        """
        results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'schema_used': schema is not None
        }
        
        # Basic validation
        if not isinstance(config, dict):
            results['valid'] = False
            results['errors'].append('Configuration must be a dictionary')
            return results
            
        # Required sections check
        required_sections = ['brain', 'network', 'simulation', 'plasticity']
        for section in required_sections:
            if section not in config:
                results['warnings'].append(f"Configuration missing recommended section: {section}")
                
        # Validate against schema if provided
        if schema:
            try:
                validate(instance=config, schema=schema)
            except jsonschema.ValidationError as e:
                results['valid'] = False
                results['errors'].append(f"Schema validation error: {e.message}")
                
        return results
        
    def add_schema(self, name: str, schema: Dict[str, Any]):
        """
        Add a configuration schema for validation.
        
        Args:
            name: Schema name
            schema: JSON schema dictionary
        """
        self.config_schemas[name] = schema
        
    def validate_against_schema(self, config: Dict[str, Any], schema_name: str) -> Dict[str, Any]:
        """
        Validate configuration against named schema.
        
        Args:
            config: Configuration to validate
            schema_name: Name of schema to use
            
        Returns:
            Validation results
        """
        if schema_name not in self.config_schemas:
            return {
                'valid': False,
                'errors': [f"Schema '{schema_name}' not found"],
                'warnings': [],
                'schema_used': True
            }
            
        return self.validate_config(config, self.config_schemas[schema_name])
        
    def create_backup(self, config: Dict[str, Any], name: Optional[str] = None) -> str:
        """
        Create backup of configuration.
        
        Args:
            config: Configuration to backup
            name: Backup name (optional, will use timestamp if not provided)
            
        Returns:
            Path to backup file
        """
        if name is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            name = f"config_backup_{timestamp}"
            
        backup_file = self.backup_dir / f"{name}.json"
        
        backup_data = {
            'timestamp': datetime.now().isoformat(),
            'config': config,
            'hash': self.compute_hash(config)
        }
        
        with open(backup_file, 'w') as f:
            json.dump(backup_data, f, indent=2)
            
        return str(backup_file)
        
    def restore_backup(self, backup_path: str, backup_name: Optional[str] = None) -> Dict[str, Any]:
        """
        Restore configuration from backup.
        
        Args:
            backup_path: Path to backup file or directory
            backup_name: Specific backup name (optional, will use latest if not provided)
            
        Returns:
            Restored configuration
        """
        backup_path = Path(backup_path)
        
        if backup_path.is_file():
            # Single backup file
            with open(backup_path, 'r') as f:
                backup_data = json.load(f)
            return backup_data['config']
        elif backup_path.is_dir():
            # Backup directory
            backup_files = list(backup_path.glob("*.json"))
            if not backup_files:
                raise FileNotFoundError(f"No backup files found in {backup_path}")
                
            if backup_name:
                # Find specific backup
                target_file = backup_path / f"{backup_name}.json"
                if not target_file.exists():
                    raise FileNotFoundError(f"Backup '{backup_name}' not found")
                target_path = target_file
            else:
                # Use latest backup
                target_path = max(backup_files, key=lambda p: p.stat().st_mtime)
                
            with open(target_path, 'r') as f:
                backup_data = json.load(f)
            return backup_data['config']
        else:
            raise FileNotFoundError(f"Backup path not found: {backup_path}")
            
    def list_backups(self, backup_dir: Optional[str] = None) -> List[Dict[str, Any]]:
        """
        List available backups.
        
        Args:
            backup_dir: Backup directory (optional, uses default if not provided)
            
        Returns:
            List of backup information dictionaries
        """
        if backup_dir is None:
            backup_dir = self.backup_dir
        else:
            backup_dir = Path(backup_dir)
            
        backups = []
        for backup_file in backup_dir.glob("*.json"):
            with open(backup_file, 'r') as f:
                backup_data = json.load(f)
                
            backups.append({
                'name': backup_file.stem,
                'path': str(backup_file),
                'timestamp': backup_data['timestamp'],
                'hash': backup_data['hash'],
                'neuron_count': backup_data['config'].get('network', {}).get('neurons', {}).get('count', 0) if 'config' in backup_data else 0
            })
            
        return sorted(backups, key=lambda x: x['timestamp'], reverse=True)
        
    def migrate_config(self, config: Dict[str, Any], migration_rules: Dict[str, Any]) -> Dict[str, Any]:"""
        """
        Migrate configuration according to migration rules.
        
        Args:
            config: Configuration to migrate
            migration_rules: Migration rules dictionary
            
        Returns:
            Migrated configuration
        """
        migrated = copy.deepcopy(config)
        
        for rule_name, rule_config in migration_rules.items():
            if rule_config.get('enabled', False):
                migration_func = getattr(self, f"_migrate_{rule_name}", None)
                if migration_func:
                    migrated = migration_func(migrated, rule_config)
                    
        return migrated
        
    def _migrate_config_v1_to_v2(self, config: Dict[str, Any], rules: Dict[str, Any]) -> Dict[str, Any]:
        """Migration from config version 1 to 2."""
        migrated = copy.deepcopy(config)
        
        # Add new fields with defaults if missing
        if 'performance' not in migrated:
            migrated['performance'] = {}
            
        if 'network' in migrated and 'connectivity' not in migrated['network']:
            migrated['network']['connectivity'] = 'auto'
            
        # Update version
        if 'version' in migrated:
            migrated['version'] = '2.0'
            
        return migrated
        
    def _migrate_config_v2_to_v3(self, config: Dict[str, Any], rules: Dict[str, Any]) -> Dict[str, Any]:
        """Migration from config version 2 to 3."""
        migrated = copy.deepcopy(config)
        
        # Add new fields with defaults if missing
        if 'neuromodulation' not in migrated:
            migrated['neuromodulation'] = {
                'dopamine': {'enabled': False, 'range': [0.0, 1.0]},
                'curiosity': {'enabled': False, 'range': [0.0, 1.0]},
                'novelty': {'enabled': False, 'range': [0.0, 1.0]}
            }
            
        # Update version
        if 'version' in migrated:
            migrated['version'] = '3.0'
            
        return migrated
        
    def generate_config_schema(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Generate a JSON schema from a sample configuration.
        
        Args:
            config: Sample configuration
            
        Returns:
            Generated JSON schema
        """
        # This is a simple implementation that creates a basic schema
        # In practice, you'd want to use a more sophisticated schema generation
        schema = {
            "$schema": "http://json-schema.org/draft-07/schema#",
            "type": "object",
            "properties": {},
            "required": []
        }
        
        def _generate_schema(obj: Any, path: str = "") -> Dict[str, Any]:
            if isinstance(obj, dict):
                properties = {}
                required = []
                
                for key, value in obj.items():
                    current_path = f"{path}.{key}" if path else key
                    properties[key] = _generate_schema(value, current_path)
                    
                    # Mark fields as required if they have non-null values
                    if value is not None and not isinstance(value, (list, dict)):
                        required.append(key)
                        
                return {
                    "type": "object",
                    "properties": properties,
                    "required": required if required else []
                }
            elif isinstance(obj, list):
                if obj:
                    return {
                        "type": "array",
                        "items": _generate_schema(obj[0])
                    }
                else:
                    return {
                        "type": "array",
                        "items": {"type": "object"}
                    }
            elif isinstance(obj, str):
                return {"type": "string"}
            elif isinstance(obj, (int, float)):
                return {"type": "number"}
            elif isinstance(obj, bool):
                return {"type": "boolean"}
            else:
                return {"type": "string"}
                
        schema["properties"] = _generate_schema(config)
        return schema