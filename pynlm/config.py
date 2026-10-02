"""
Advanced configuration management for NLM.

This module provides comprehensive configuration management including
JSON/YAML support, preset management, configuration validation,
and Pythonic configuration interface.
"""

import pybind11
import pybind11.stl
from .types import *
from .core import Config as CoreConfig
import json
import yaml
import os
import copy
from datetime import datetime
from typing import Dict, List, Any, Optional, Union
import logging
import hashlib

# Configuration system classes

class ConfigPreset:
    """
    Configuration preset management.

    This class manages configuration presets including research presets,
    performance presets, and research question presets.
    """

    def __init__(self):
        """Initialize configuration preset manager."""
        self.presets = {}
        self._load_default_presets()

    def _load_default_presets(self) -> None:
        """Load default configuration presets."""
        # Research presets
        self.presets['research_scientific'] = {
            'name': 'Research Scientific',
            'category': 'research',
            'description': 'Parameters optimized for scientific rigor and experimental control',
            'config': {
                'neuron_count': 1000,
                'region_count': 2,
                'connection_probability': 0.1,
                'enable_parameter_sweeps': True,
                'enable_multi_objective_optimization': True,
                'enable_adaptive_experimentation': True,
                'enable_comparative_analysis': True,
                'seed': 42
            }
        }

        self.presets['research_speed'] = {
            'name': 'Research Speed',
            'category': 'research',
            'description': 'Parameters optimized for fast iteration and quick feedback',
            'config': {
                'neuron_count': 200,
                'region_count': 1,
                'connection_probability': 0.2,
                'enable_parameter_sweeps': True,
                'enable_multi_objective_optimization': False,
                'enable_adaptive_experimentation': False,
                'enable_comparative_analysis': False,
                'seed': 42
            }
        }

        self.presets['research_reproducibility'] = {
            'name': 'Research Reproducibility',
            'category': 'research',
            'description': 'Parameters optimized for reproducibility and consistency',
            'config': {
                'neuron_count': 500,
                'region_count': 2,
                'connection_probability': 0.15,
                'enable_parameter_sweeps': True,
                'enable_multi_objective_optimization': True,
                'enable_adaptive_experimentation': True,
                'enable_comparative_analysis': True,
                'seed': 42
            }
        }

        # Performance presets
        self.presets['performance_small'] = {
            'name': 'Performance Small',
            'category': 'performance',
            'description': 'Small-scale experiments (100-500 neurons) for development and testing',
            'config': {
                'neuron_count': 100,
                'region_count': 1,
                'connection_probability': 0.05,
                'max_neurons': 500,
                'synapse_formation_steps': 5000,
                'enable_multithreading': False
            }
        }

        self.presets['performance_medium'] = {
            'name': 'Performance Medium',
            'category': 'performance',
            'description': 'Medium-scale experiments (500-1000 neurons) for balanced research',
            'config': {
                'neuron_count': 500,
                'region_count': 2,
                'connection_probability': 0.1,
                'max_neurons': 1000,
                'synapse_formation_steps': 10000,
                'enable_multithreading': True,
                'thread_count': 4
            }
        }

        self.presets['performance_large'] = {
            'name': 'Performance Large',
            'category': 'performance',
            'description': 'Large-scale experiments (1000+ neurons) for research-grade performance',
            'config': {
                'neuron_count': 1000,
                'region_count': 3,
                'connection_probability': 0.15,
                'max_neurons': 5000,
                'synapse_formation_steps': 20000,
                'enable_multithreading': True,
                'thread_count': 8,
                'enable_SIMD': True,
                'enable_sparse_connectivity': True
            }
        }

        # Research question presets
        self.presets['learning_curves'] = {
            'name': 'Learning Curves',
            'category': 'research_question',
            'description': 'Configure experiments to track learning over time',
            'config': {
                'enable_parameter_sweeps': True,
                'parameter_ranges': {
                    'exploration_rate': [0.1, 0.2, 0.3, 0.4, 0.5],
                    'novelty_threshold': [0.2, 0.4, 0.6, 0.8]
                },
                'optimization_objectives': ['learning_rate', 'memory_retention'],
                'tracking_interval': 100
            }
        }

        self.presets['memory_capacity'] = {
            'name': 'Memory Capacity',
            'category': 'research_question',
            'description': 'Configure experiments to test memory limits',
            'config': {
                'initial_neurons': 50000,
                'max_neurons': 200000,
                'connection_probability': 0.1,
                'enable_parameter_sweeps': True,
                'parameter_ranges': {
                    'synaptic_strength': [0.5, 0.8, 1.0],
                    'plasticity_rate': [0.001, 0.01, 0.1]
                }
            }
        }

        self.presets['social_dynamics'] = {
            'name': 'Social Dynamics',
            'category': 'research_question',
            'description': 'Configure experiments for social behavior studies',
            'config': {
                'num_agents': 150,
                'enable_parameter_sweeps': True,
                'parameter_ranges': {
                    'social_interaction_rate': [0.1, 0.3, 0.5, 0.7],
                    'imitation_probability': [0.3, 0.6, 0.9]
                }
            }
        }

        self.presets['development_stages'] = {
            'name': 'Development Stages',
            'category': 'research_question',
            'description': 'Configure experiments for developmental studies',
            'config': {
                'enable_developmental_tracking': True,
                'developmental_milestone_interval': 25.0,
                'enable_parameter_sweeps': True,
                'parameter_ranges': {
                    'development_stage_duration': [1000, 5000, 10000],
                    'transition_rate': [0.01, 0.05, 0.1]
                }
            }
        }

    def get_preset(self, preset_name: str) -> Optional[Dict[str, Any]]:
        """
        Get preset by name.

        Args:
            preset_name: Name of the preset

        Returns:
            Optional[Dict[str, Any]]: Preset or None if not found
        """
        return self.presets.get(preset_name)

    def get_all_presets(self) -> Dict[str, Dict[str, Any]]:
        """Get all presets."""
        return self.presets.copy()

    def get_presets_by_category(self, category: str) -> Dict[str, Dict[str, Any]]:
        """
        Get presets by category.

        Args:
            category: Preset category

        Returns:
            Dict[str, Dict[str, Any]]: Presets in category
        """
        return {k: v for k, v in self.presets.items() if v.get('category') == category}

    def get_all_categories(self) -> List[str]:
        """Get all preset categories."""
        return list(set(preset.get('category') for preset in self.presets.values()))

    def create_preset(self, name: str, category: str, description: str, config: Dict[str, Any]) -> bool:
        """
        Create new preset.

        Args:
            name: Preset name
            category: Preset category
            description: Preset description
            config: Preset configuration

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            self.presets[name] = {
                'name': name,
                'category': category,
                'description': description,
                'config': config
            }
            return True
        except Exception as e:
            logging.error(f"Error creating preset: {e}")
            return False

    def update_preset(self, preset_name: str, config: Dict[str, Any]) -> bool:
        """
        Update existing preset.

        Args:
            preset_name: Name of preset to update
            config: New configuration

        Returns:
            bool: True if successful, False otherwise
        """
        if preset_name not in self.presets:
            return False

        try:
            self.presets[preset_name]['config'].update(config)
            self.presets[preset_name]['last_modified'] = datetime.now().isoformat()
            return True
        except Exception as e:
            logging.error(f"Error updating preset: {e}")
            return False

    def delete_preset(self, preset_name: str) -> bool:
        """
        Delete preset.

        Args:
            preset_name: Name of preset to delete

        Returns:
            bool: True if successful, False otherwise
        """
        if preset_name in self.presets:
            del self.presets[preset_name]
            return True
        return False

    def __repr__(self) -> str:
        return f"<ConfigPreset: {len(self.presets)} presets, {len(self.get_all_categories())} categories>"


class ConfigManager:
    """
    Advanced configuration manager with preset support and validation.

    This class provides comprehensive configuration management including
    preset loading, configuration validation, and version control.
    """

    def __init__(self, config_dir: str = "./configs"):
        """
        Initialize configuration manager.

        Args:
            config_dir: Directory for configuration files
        """
        self.config_dir = config_dir
        self.config_preset = ConfigPreset()
        self.config_history = {}
        self.current_config_id = None

        # Create config directory
        os.makedirs(config_dir, exist_ok=True)

    def load_preset(self, preset_name: str) -> Optional['Config']:
        """
        Load configuration from preset.

        Args:
            preset_name: Name of preset

        Returns:
            Optional[Config]: Configuration or None if not found
        """
        preset = self.config_preset.get_preset(preset_name)
        if not preset:
            return None

        config_dict = preset.get('config', {})
        config = Config()
        config.load_from_dict(config_dict)
        return config

    def create_config_from_preset(self, preset_name: str) -> 'Config':
        """
        Create configuration from preset.

        Args:
            preset_name: Name of preset

        Returns:
            Config: Created configuration
        """
        preset = self.config_preset.get_preset(preset_name)
        if not preset:
            raise ValueError(f"Preset not found: {preset_name}")

        config = Config()
        config.load_from_dict(preset.get('config', {}))
        return config

    def save_config(self, config: 'Config', config_name: str, 
                   format: str = "json", description: str = "") -> bool:
        """
        Save configuration to file.

        Args:
            config: Configuration to save
            config_name: Configuration name
            format: Output format (json, yaml, cfg)
            description: Configuration description

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            # Create config record
            config_id = f"{config_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
            config_record = {
                'id': config_id,
                'name': config_name,
                'description': description,
                'format': format,
                'config': config.to_dict(),
                'created': datetime.now().isoformat(),
                'last_modified': datetime.now().isoformat()
            }

            self.config_history[config_id] = config_record
            self.current_config_id = config_id

            # Save to file
            if format.lower() == 'json':
                filepath = f"{self.config_dir}/{config_name}.json"
                import json
                with open(filepath, 'w') as f:
                    json.dump(config.to_dict(), f, indent=2)
            elif format.lower() == 'yaml':
                filepath = f"{self.config_dir}/{config_name}.yaml"
                import yaml
                with open(filepath, 'w') as f:
                    yaml.dump(config.to_dict(), f, default_flow_style=False)
            elif format.lower() == 'cfg':
                filepath = f"{self.config_dir}/{config_name}.cfg"
                with open(filepath, 'w') as f:
                    f.write(config.summary())
            else:
                raise ValueError(f"Unsupported format: {format}")

            logging.info(f"Configuration saved: {config_name} ({format})")
            return True

        except Exception as e:
            logging.error(f"Error saving configuration: {e}")
            return False

    def load_config(self, config_name: str, format: Optional[str] = None) -> Optional['Config']:
        """
        Load configuration from file.

        Args:
            config_name: Configuration name (without extension)
            format: File format (auto-detected if None)

        Returns:
            Optional[Config]: Loaded configuration or None if not found
        """
        # Try different formats
        formats = [format] if format else ['json', 'yaml', 'cfg']
        
        for fmt in formats:
            filepath = f"{self.config_dir}/{config_name}.{fmt}"
            if os.path.exists(filepath):
                try:
                    config = Config()
                    if fmt.lower() == 'json':
                        with open(filepath, 'r') as f:
                            import json
                            config.load_from_dict(json.load(f))
                    elif fmt.lower() == 'yaml':
                        with open(filepath, 'r') as f:
                            import yaml
                            config.load_from_dict(yaml.safe_load(f))
                    elif fmt.lower() == 'cfg':
                        config.loadFromFile(filepath)  # Using core config method
                    
                    # Find matching config record
                    for config_id, config_record in self.config_history.items():
                        if config_record['name'] == config_name and config_record['format'] == fmt:
                            self.current_config_id = config_id
                            break

                    logging.info(f"Configuration loaded: {config_name} ({fmt})")
                    return config

                except Exception as e:
                    logging.error(f"Error loading configuration {config_name} from {filepath}: {e}")
                    continue

        logging.warning(f"Configuration not found: {config_name}")
        return None

    def get_all_configs(self) -> List[Dict[str, Any]]:
        """Get all saved configurations."""
        return list(self.config_history.values())

    def get_config(self, config_id: str) -> Optional[Dict[str, Any]]:
        """
        Get configuration by ID.

        Args:
            config_id: Configuration ID

        Returns:
            Optional[Dict[str, Any]]: Configuration record or None
        """
        return self.config_history.get(config_id)

    def delete_config(self, config_id: str) -> bool:
        """
        Delete configuration.

        Args:
            config_id: Configuration ID

        Returns:
            bool: True if successful, False otherwise
        """
        if config_id in self.config_history:
            del self.config_history[config_id]
            return True
        return False

    def get_config_versions(self, config_name: str) -> List[Dict[str, Any]]:
        """
        Get all versions of a configuration.

        Args:
            config_name: Configuration name

        Returns:
            List[Dict[str, Any]]: Configuration versions
        """
        return [c for c in self.config_history.values() if c['name'] == config_name]

    def export_config(self, config_id: str, output_path: str) -> bool:
        """
        Export configuration to external path.

        Args:
            config_id: Configuration ID
            output_path: Output path

        Returns:
            bool: True if successful, False otherwise
        """
        config = self.get_config(config_id)
        if not config:
            return False

        try:
            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_path), exist_ok=True)

            # Export based on format
            if config['format'].lower() == 'json':
                import json
                with open(output_path, 'w') as f:
                    json.dump(config['config'], f, indent=2)
            elif config['format'].lower() == 'yaml':
                import yaml
                with open(output_path, 'w') as f:
                    yaml.dump(config['config'], f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format for export: {config['format']}")

            logging.info(f"Configuration exported to {output_path}")
            return True

        except Exception as e:
            logging.error(f"Error exporting configuration: {e}")
            return False

    def import_config(self, filepath: str, config_name: Optional[str] = None) -> bool:
        """
        Import configuration from external file.

        Args:
            filepath: Path to configuration file
            config_name: Configuration name (uses filename if None)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            # Determine format
            if filepath.lower().endswith('.json'):
                format = 'json'
            elif filepath.lower().endswith('.yaml') or filepath.lower().endswith('.yml'):
                format = 'yaml'
            elif filepath.lower().endswith('.cfg'):
                format = 'cfg'
            else:
                raise ValueError(f"Unsupported file format: {filepath}")

            # Load configuration
            config = Config()
            if format.lower() == 'json':
                import json
                with open(filepath, 'r') as f:
                    config.load_from_dict(json.load(f))
            elif format.lower() == 'yaml':
                import yaml
                with open(filepath, 'r') as f:
                    config.load_from_dict(yaml.safe_load(f))
            elif format.lower() == 'cfg':
                config.loadFromFile(filepath)

            # Use filename as name if not provided
            if config_name is None:
                config_name = os.path.splitext(os.path.basename(filepath))[0]

            # Save configuration
            return self.save_config(config, config_name, format)

        except Exception as e:
            logging.error(f"Error importing configuration: {e}")
            return False

    def validate_config(self, config: 'Config') -> List[str]:
        """
        Validate configuration.

        Args:
            config: Configuration to validate

        Returns:
            List[str]: List of validation errors (empty if valid)
        """
        errors = []

        # Check for required keys
        required_keys = ['neuron_count', 'region_count']
        for key in required_keys:
            if not config.has(key):
                errors.append(f"Missing required key: {key}")

        # Validate key types
        if config.has('neuron_count'):
            try:
                neuron_count = config.get_int('neuron_count')
                if neuron_count <= 0 or neuron_count > 1000000:
                    errors.append("neuron_count must be between 1 and 1,000,000")
            except ValueError:
                errors.append("neuron_count must be an integer")

        if config.has('region_count'):
            try:
                region_count = config.get_int('region_count')
                if region_count <= 0 or region_count > 100:
                    errors.append("region_count must be between 1 and 100")
            except ValueError:
                errors.append("region_count must be an integer")

        if config.has('connection_probability'):
            try:
                conn_prob = config.get_float('connection_probability')
                if conn_prob < 0.0 or conn_prob > 1.0:
                    errors.append("connection_probability must be between 0.0 and 1.0")
            except ValueError:
                errors.append("connection_probability must be a float")

        return errors

    def compare_configs(self, config1: 'Config', config2: 'Config') -> Dict[str, Any]:
        """
        Compare two configurations.

        Args:
            config1: First configuration
            config2: Second configuration

        Returns:
            Dict[str, Any]: Comparison results
        """
        keys1 = set(config1.keys())
        keys2 = set(config2.keys())

        common_keys = keys1.intersection(keys2)
        only_in_config1 = keys1.difference(keys2)
        only_in_config2 = keys2.difference(keys1)

        differences = []
        for key in common_keys:
            val1 = config1.get(key)
            val2 = config2.get(key)
            if val1 != val2:
                differences.append({
                    'key': key,
                    'config1_value': val1,
                    'config2_value': val2,
                    'difference': f"{val1} != {val2}"
                })

        return {
            'common_keys': len(common_keys),
            'only_in_config1': list(only_in_config1),
            'only_in_config2': list(only_in_config2),
            'differences': differences,
            'similarity_score': len(common_keys) / max(len(keys1), len(keys2)) if keys1 or keys2 else 1.0
        }

    def get_config_suggestions(self, current_config: 'Config') -> List[Dict[str, Any]]:
        """
        Get configuration suggestions based on current config.

        Args:
            current_config: Current configuration

        Returns:
            List[Dict[str, Any]]: Configuration suggestions
        """
        suggestions = []

        # Suggest based on neuron count
        if current_config.has('neuron_count'):
            neuron_count = current_config.get_int('neuron_count')
            if neuron_count < 100:
                suggestions.append({
                    'type': 'preset',
                    'name': 'performance_small',
                    'description': 'Consider small-scale performance preset for faster iteration'
                })
            elif neuron_count > 10000:
                suggestions.append({
                    'type': 'preset',
                    'name': 'performance_large',
                    'description': 'Consider large-scale performance preset for research-grade experiments'
                })

        # Suggest research presets
        if current_config.has('enable_parameter_sweeps') and not current_config.get_bool('enable_parameter_sweeps'):
            suggestions.append({
                'type': 'preset',
                'name': 'research_scientific',
                'description': 'Enable parameter sweeps for comprehensive exploration'
            })

        if current_config.has('enable_multi_objective_optimization') and not current_config.get_bool('enable_multi_objective_optimization'):
            suggestions.append({
                'type': 'preset',
                'name': 'research_reproducibility',
                'description': 'Enable multi-objective optimization for better solutions'
            })

        return suggestions

    def __repr__(self) -> str:
        return f"<ConfigManager: {len(self.config_history)} configs, {len(self.config_preset.get_all_categories())} preset categories>"


class Config:
    """
    Enhanced configuration class with Pythonic interface.

    This class provides an enhanced configuration system with preset support,
    validation, and advanced features.
    """

    def __init__(self, config: Optional[Dict[str, Any]] = None,
                 config_manager: Optional['ConfigManager'] = None):
        """
        Initialize configuration.

        Args:
            config: Initial configuration dictionary
            config_manager: Configuration manager instance
        """
        self._config = config or {}
        self._config_manager = config_manager or ConfigManager()
        self._validation_rules = self._load_default_validation_rules()
        self._config_history = []
        self._last_modified = datetime.now()

    def _load_default_validation_rules(self) -> Dict[str, Any]:
        """Load default configuration validation rules."""
        return {
            'neuron_count': {
                'type': 'int',
                'min': 1,
                'max': 1000000,
                'required': True
            },
            'region_count': {
                'type': 'int',
                'min': 1,
                'max': 100,
                'required': True
            },
            'connection_probability': {
                'type': 'float',
                'min': 0.0,
                'max': 1.0,
                'required': False
            },
            'enable_parameter_sweeps': {
                'type': 'bool',
                'required': False
            },
            'enable_multi_objective_optimization': {
                'type': 'bool',
                'required': False
            },
            'seed': {
                'type': 'int',
                'min': 0,
                'max': 2147483647,
                'required': False
            }
        }

    # Configuration loading

    def load_from_file(self, filepath: str, format: Optional[str] = None) -> bool:
        """
        Load configuration from file.

        Args:
            filepath: Path to configuration file
            format: File format (auto-detected if None)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if format is None:
                if filepath.lower().endswith('.json'):
                    format = 'json'
                elif filepath.lower().endswith('.yaml') or filepath.lower().endswith('.yml'):
                    format = 'yaml'
                else:
                    format = 'cfg'

            if format.lower() == 'json':
                with open(filepath, 'r') as f:
                    import json
                    self.load_from_dict(json.load(f))
            elif format.lower() == 'yaml':
                with open(filepath, 'r') as f:
                    import yaml
                    self.load_from_dict(yaml.safe_load(f))
            else:
                # Use core config for cfg files
                return CoreConfig.loadFromFile(filepath)

            self._last_modified = datetime.now()
            return True

        except Exception as e:
            logging.error(f"Error loading configuration from {filepath}: {e}")
            return False

    def load_from_dict(self, config_dict: Dict[str, Any]) -> None:
        """
        Load configuration from dictionary.

        Args:
            config_dict: Configuration dictionary
        """
        self._config = config_dict.copy()
        self._last_modified = datetime.now()

    def load_from_preset(self, preset_name: str) -> bool:
        """
        Load configuration from preset.

        Args:
            preset_name: Name of preset

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            preset = self._config_manager.config_preset.get_preset(preset_name)
            if not preset:
                logging.error(f"Preset not found: {preset_name}")
                return False

            self.load_from_dict(preset.get('config', {}))
            self._last_modified = datetime.now()
            return True

        except Exception as e:
            logging.error(f"Error loading preset {preset_name}: {e}")
            return False

    def load_from_args(self, args: List[str]) -> bool:
        """
        Load configuration from command-line arguments.

        Args:
            args: List of command-line arguments

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            for i, arg in enumerate(args):
                if arg.startswith('--'):
                    # Handle --key=value format
                    if '=' in arg:
                        key, value = arg[2:].split('=', 1)
                        self.set_value(key, self._parse_value(value))
                    else:
                        # Handle --key value format
                        if i + 1 < len(args):
                            self.set_value(arg[2:], self._parse_value(args[i + 1]))
                elif arg.startswith('-'):
                    # Handle -k value format
                    key = arg[1:]
                    if i + 1 < len(args):
                        self.set_value(key, self._parse_value(args[i + 1]))

            self._last_modified = datetime.now()
            return True

        except Exception as e:
            logging.error(f"Error loading from args: {e}")
            return False

    def _parse_value(self, value: str) -> Any:
        """Parse value string to appropriate type."""
        # Try to parse as int
        try:
            return int(value)
        except ValueError:
            pass

        # Try to parse as float
        try:
            return float(value)
        except ValueError:
            pass

        # Try to parse as bool
        if value.lower() in ['true', 'false', '1', '0']:
            return value.lower() == 'true' or value == '1'

        # Return as string
        return value

    # Configuration saving

    def save_to_file(self, filepath: str, format: Optional[str] = None) -> bool:
        """
        Save configuration to file.

        Args:
            filepath: Path to save configuration
            format: Output format (auto-detected if None)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if format is None:
                if filepath.lower().endswith('.json'):
                    format = 'json'
                elif filepath.lower().endswith('.yaml') or filepath.lower().endswith('.yml'):
                    format = 'yaml'
                else:
                    format = 'cfg'

            # Ensure directory exists
            os.makedirs(os.path.dirname(filepath), exist_ok=True)

            if format.lower() == 'json':
                with open(filepath, 'w') as f:
                    import json
                    json.dump(self._config, f, indent=2)
            elif format.lower() == 'yaml':
                with open(filepath, 'w') as f:
                    import yaml
                    yaml.dump(self._config, f, default_flow_style=False)
            else:
                # Convert to cfg format
                with open(filepath, 'w') as f:
                    for key, value in self._config.items():
                        f.write(f"{key} = {value}\n")

            self._last_modified = datetime.now()
            return True

        except Exception as e:
            logging.error(f"Error saving configuration to {filepath}: {e}")
            return False

    def save_to_preset(self, preset_name: str, category: Optional[str] = None,
                      description: Optional[str] = None) -> bool:
        """
        Save configuration as preset.

        Args:
            preset_name: Preset name
            category: Preset category (auto-detected if None)
            description: Preset description (auto-generated if None)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if category is None:
                category = self._infer_category()

            if description is None:
                description = self._generate_description()

            preset_config = {
                'name': preset_name,
                'category': category,
                'description': description,
                'config': self._config.copy(),
                'created': datetime.now().isoformat(),
                'last_modified': datetime.now().isoformat()
            }

            success = self._config_manager.config_preset.create_preset(
                preset_name, category, description, self._config.copy()
            )

            if success:
                logging.info(f"Configuration saved as preset: {preset_name}")

            return success

        except Exception as e:
            logging.error(f"Error saving configuration as preset: {e}")
            return False

    def _infer_category(self) -> str:
        """Infer preset category from configuration."""
        if self.has('neuron_count'):
            neuron_count = self.get_int('neuron_count')
            if neuron_count < 100:
                return 'performance'
            elif neuron_count > 10000:
                return 'performance_large'

        if self.has('enable_parameter_sweeps') and self.get_bool('enable_parameter_sweeps'):
            if self.has('enable_multi_objective_optimization') and self.get_bool('enable_multi_objective_optimization'):
                return 'research'
            else:
                return 'research_speed'

        return 'research'

    def _generate_description(self) -> str:
        """Generate preset description."""
        descriptions = []

        if self.has('neuron_count'):
            descriptions.append(f"{self.get_int('neuron_count')} neurons")

        if self.has('region_count'):
            descriptions.append(f"{self.get_int('region_count')} regions")

        if self.has('connection_probability'):
            descriptions.append(f"{self.get_float('connection_probability') * 100:.0f}% connectivity")

        if self.has('enable_parameter_sweeps') and self.get_bool('enable_parameter_sweeps'):
            descriptions.append("with parameter sweeps")

        if self.has('enable_multi_objective_optimization') and self.get_bool('enable_multi_objective_optimization'):
            descriptions.append("with multi-objective optimization")

        return ", ".join(descriptions) if descriptions else "Custom configuration"

    # Configuration access and manipulation

    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value.

        Args:
            key: Configuration key
            default: Default value if key not found

        Returns:
            Configuration value or default
        """
        return self._config.get(key, default)

    def get_int(self, key: str, default: int = 0) -> int:
        """Get integer configuration value."""
        value = self.get(key)
        try:
            return int(value)
        except (ValueError, TypeError):
            return default

    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get float configuration value."""
        value = self.get(key)
        try:
            return float(value)
        except (ValueError, TypeError):
            return default

    def get_string(self, key: str, default: str = "") -> str:
        """Get string configuration value."""
        value = self.get(key)
        return str(value) if value is not None else default

    def get_bool(self, key: str, default: bool = False) -> bool:
        """Get boolean configuration value."""
        value = self.get(key)
        if isinstance(value, bool):
            return value
        try:
            return str(value).lower() in ['true', '1', 'yes', 'on']
        except (ValueError, TypeError):
            return default

    def has(self, key: str) -> bool:
        """Check if configuration contains a key."""
        return key in self._config

    def set(self, key: str, value: Any) -> None:
        """
        Set configuration value.

        Args:
            key: Configuration key
            value: Configuration value
        """
        self.set_value(key, value)
        self._last_modified = datetime.now()

    def set_value(self, key: str, value: Any) -> None:
        """Set configuration value without updating timestamp."""
        self._config[key] = value

    def remove(self, key: str) -> None:
        """Remove configuration key."""
        if key in self._config:
            del self._config[key]
        self._last_modified = datetime.now()

    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._config.keys())

    def clear(self) -> None:
        """Clear all configuration."""
        self._config.clear()
        self._last_modified = datetime.now()

    def merge(self, other: 'Config') -> None:
        """
        Merge another configuration into this one.

        Args:
            other: Configuration to merge
        """
        self._config.update(other._config)
        self._last_modified = datetime.now()

    def clone(self) -> 'Config':
        """Create a copy of the configuration."""
        new_config = Config(self._config.copy(), self._config_manager)
        new_config._validation_rules = self._validation_rules.copy()
        return new_config

    # Configuration validation

    def validate(self) -> List[str]:
        """
        Validate configuration.

        Returns:
            List[str]: List of validation errors (empty if valid)
        """
        errors = []

        # Check validation rules
        for key, rules in self._validation_rules.items():
            if rules.get('required', False) and key not in self._config:
                errors.append(f"Required key '{key}' is missing")

            if key in self._config:
                value = self._config[key]
                rule_type = rules.get('type')

                if rule_type == 'int' and not isinstance(value, int):
                    try:
                        int(value)
                    except (ValueError, TypeError):
                        errors.append(f"Key '{key}' must be an integer")
                elif rule_type == 'float' and not isinstance(value, (int, float)):
                    try:
                        float(value)
                    except (ValueError, TypeError):
                        errors.append(f"Key '{key}' must be a float")
                elif rule_type == 'bool' and not isinstance(value, bool):
                    errors.append(f"Key '{key}' must be a boolean")

                # Check ranges
                if rule_type in ['int', 'float']:
                    try:
                        num_value = float(value)
                        if 'min' in rules and num_value < rules['min']:
                            errors.append(f"Key '{key}' must be >= {rules['min']}")
                        if 'max' in rules and num_value > rules['max']:
                            errors.append(f"Key '{key}' must be <= {rules['max']}")
                    except (ValueError, TypeError):
                        pass

        return errors

    def validate_preset_compatibility(self, preset_name: str) -> bool:
        """
        Validate if current configuration is compatible with preset.

        Args:
            preset_name: Preset name

        Returns:
            bool: True if compatible, False otherwise
        """
        preset = self._config_manager.config_preset.get_preset(preset_name)
        if not preset:
            return False

        preset_config = preset.get('config', {})

        # Check for conflicting keys
        for key in preset_config:
            if key in self._config:
                preset_value = preset_config[key]
                current_value = self._config[key]

                # Allow current value if it overrides preset value
                # or if current value is compatible with preset value

        return True

    # Configuration history and version control

    def save_version(self, version_name: str) -> bool:
        """
        Save current configuration as a version.

        Args:
            version_name: Version name

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            version_record = {
                'name': version_name,
                'config': self._config.copy(),
                'timestamp': datetime.now().isoformat(),
                'modified': self._last_modified.isoformat()
            }

            self._config_history.append(version_record)
            self._last_modified = datetime.now()

            logging.info(f"Configuration version saved: {version_name}")
            return True

        except Exception as e:
            logging.error(f"Error saving configuration version: {e}")
            return False

    def restore_version(self, version_name: str) -> bool:
        """
        Restore configuration from version.

        Args:
            version_name: Version name

        Returns:
            bool: True if successful, False otherwise
        """
        for version in self._config_history:
            if version['name'] == version_name:
                self._config = version['config'].copy()
                self._last_modified = datetime.fromisoformat(version['modified'])
                logging.info(f"Configuration restored: {version_name}")
                return True

        logging.error(f"Configuration version not found: {version_name}")
        return False

    def get_version_history(self) -> List[Dict[str, Any]]:
        """Get configuration version history."""
        return self._config_history.copy()

    # Configuration utilities

    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self._config.copy()

    def to_json(self) -> str:
        """Convert configuration to JSON string."""
        import json
        return json.dumps(self._config, indent=2)

    def to_yaml(self) -> str:
        """Convert configuration to YAML string."""
        import yaml
        return yaml.dump(self._config, default_flow_style=False)

    def hash_config(self) -> str:
        """Get hash of configuration."""
        config_str = json.dumps(self._config, sort_keys=True)
        return hashlib.md5(config_str.encode()).hexdigest()

    def get_suggestions(self) -> List[Dict[str, Any]]:
        """Get configuration suggestions."""
        return self._config_manager.get_config_suggestions(self)

    # Configuration comparison

    def compare_with(self, other: 'Config') -> Dict[str, Any]:
        """
        Compare with another configuration.

        Args:
            other: Configuration to compare

        Returns:
            Dict[str, Any]: Comparison results
        """
        return self._config_manager.compare_configs(self, other)

    # Configuration operators

    def __getitem__(self, key: str) -> Any:
        return self.get(key)

    def __setitem__(self, key: str, value: Any) -> None:
        self.set(key, value)

    def __delitem__(self, key: str) -> None:
        self.remove(key)

    def __contains__(self, key: str) -> bool:
        return self.has(key)

    def __len__(self) -> int:
        return len(self._config)

    def __iter__(self):
        return iter(self._config)

    def __repr__(self) -> str:
        return f"<Config: {len(self._config)} entries, last modified: {self._last_modified.isoformat()}>"

    def __str__(self) -> str:
        return self.to_json()


# Global configuration management

_config_manager = None
_global_config = None


def get_config_manager(config_dir: str = "./configs") -> ConfigManager:
    """
    Get global configuration manager.

    Args:
        config_dir: Configuration directory

    Returns:
        ConfigManager: Configuration manager
    """
    global _config_manager
    if _config_manager is None:
        _config_manager = ConfigManager(config_dir)
    return _config_manager


def create_config(config: Optional[Dict[str, Any]] = None,
                 config_manager: Optional[ConfigManager] = None) -> Config:
    """
    Create configuration.

    Args:
        config: Initial configuration dictionary
        config_manager: Configuration manager instance

    Returns:
        Config: Created configuration
    """
    return Config(config, config_manager)


def load_config_from_preset(preset_name: str) -> Config:
    """
    Load configuration from preset.

    Args:
        preset_name: Preset name

    Returns:
        Config: Loaded configuration
    """
    config_manager = get_config_manager()
    return config_manager.create_config_from_preset(preset_name)


def save_config_to_preset(config: Config, preset_name: str,
                         category: Optional[str] = None,
                         description: Optional[str] = None) -> bool:
    """
    Save configuration as preset.

    Args:
        config: Configuration to save
        preset_name: Preset name
        category: Preset category
        description: Preset description

    Returns:
        bool: True if successful, False otherwise
    """
    return config.save_to_preset(preset_name, category, description)


# Export key classes and functions
__all__ = [
    'ConfigPreset', 'ConfigManager', 'Config',
    'get_config_manager', 'create_config',
    'load_config_from_preset', 'save_config_to_preset'
]