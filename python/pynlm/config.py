# Enhanced NLM Configuration Parser
# Supports JSON, YAML, and other configuration formats for advanced users

import json
import os
from typing import Dict, Any, List, Optional, Union
from pathlib import Path

class AdvancedConfigParser:
    """Enhanced configuration parser with format detection and validation."""
    
    def __init__(self):
        self.supported_formats = {
            '.json': self._parse_json,
            '.yaml': self._parse_yaml,
            '.yml': self._parse_yaml,
            '.cfg': self._parse_cfg,
            '.ini': self._parse_ini,
            '.txt': self._parse_txt,
            '.xml': self._parse_xml
        }
        self.config_cache = {}
    
    def detect_format(self, filepath: str) -> str:
        """Detect configuration file format based on extension."""
        ext = Path(filepath).suffix.lower()
        if ext in self.supported_formats:
            return ext
        
        # Try to detect format by content
        try:
            with open(filepath, 'r') as f:
                content = f.read(1024)  # Read first 1KB
                content = content.strip()
                
                if content.startswith('{') and content.endswith('}'):
                    return '.json'
                elif 'array' in content.lower() or '[' in content:
                    return '.yaml'
                elif '=' in content and '\n' in content:
                    return '.cfg'
                elif '<?xml' in content.lower():
                    return '.xml'
        except Exception:
            pass
        
        # Default to JSON
        return '.json'
    
    def load_config(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Load configuration from file with automatic format detection."""
        filepath = str(Path(filepath).resolve())
        
        # Check cache first
        if filepath in self.config_cache:
            return self.config_cache[filepath]
        
        # Try to load with detected format
        format_ext = self.detect_format(filepath)
        parser = self.supported_formats.get(format_ext)
        
        if not parser:
            return None
        
        try:
            config = parser(filepath)
            if config:
                self.config_cache[filepath] = config
                return config
        except Exception as e:
            print(f"Error loading {filepath} with {format_ext} format: {e}")
        
        return None
    
    def _parse_json(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Parse JSON configuration file."""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        return self._validate_config_structure(data)
    
    def _parse_yaml(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Parse YAML configuration file."""
        try:
            import yaml
            with open(filepath, 'r') as f:
                data = yaml.safe_load(f)
            
            if not isinstance(data, dict):
                raise ValueError("YAML file must contain a dictionary at top level")
            
            return self._validate_config_structure(data)
        except ImportError:
            print("PyYAML not installed. Install with: pip install PyYAML")
            return None
        except Exception as e:
            print(f"Error parsing YAML file: {e}")
            return None
    
    def _parse_cfg(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Parse traditional config file (key=value format)."""
        config = {}
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#') or line.startswith('//'):
                    continue
                
                if '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Try to parse value as different types
                    config[key] = self._parse_cfg_value(value)
        
        return self._validate_config_structure(config)
    
    def _parse_ini(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Parse INI-style configuration file."""
        config = {}
        try:
            import configparser
            parser = configparser.ConfigParser()
            parser.read(filepath)
            
            for section in parser.sections():
                config[section] = {}
                for key, value in parser.items(section):
                    config[section][key] = self._parse_ini_value(value)
            
            return self._validate_config_structure(config)
        except ImportError:
            print("configparser not available. Install with: pip install configparser")
            return None
        except Exception as e:
            print(f"Error parsing INI file: {e}")
            return None
    
    def _parse_txt(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Parse simple text configuration file."""
        config = {}
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#') or line.startswith('//'):
                    continue
                
                # Parse key: value or key value formats
                if ':' in line:
                    key, value = line.split(':', 1)
                elif '=' in line:
                    key, value = line.split('=', 1)
                else:
                    continue
                
                key = key.strip()
                value = value.strip()
                config[key] = self._parse_txt_value(value)
        
        return self._validate_config_structure(config)
    
    def _parse_xml(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Parse XML configuration file."""
        try:
            import xml.etree.ElementTree as ET
            tree = ET.parse(filepath)
            root = tree.getroot()
            
            config = self._xml_to_dict(root)
            return self._validate_config_structure(config)
        except ImportError:
            print("xml module not available. Install with: pip install xml")
            return None
        except Exception as e:
            print(f"Error parsing XML file: {e}")
            return None
    
    def _xml_to_dict(self, element) -> Dict[str, Any]:
        """Convert XML element to dictionary."""
        result = {}
        
        if element.attrib:
            result['@attributes'] = element.attrib
        
        children = list(element)
        if children:
            for child in children:
                child_data = self._xml_to_dict(child)
                if child.tag in result:
                    if not isinstance(result[child.tag], list):
                        result[child.tag] = [result[child.tag]]
                    result[child.tag].append(child_data)
                else:
                    result[child.tag] = child_data
        else:
            text = element.text.strip() if element.text else ''
            if text:
                result['#text'] = text
        
        return result
    
    def _parse_cfg_value(self, value: str) -> Any:
        """Parse configuration value as appropriate type."""
        value = value.strip()
        
        # Remove quotes
        if value.startswith('"') and value.endswith('"'):
            return value[1:-1]
        elif value.startswith("'") and value.endswith("'"):
            return value[1:-1]
        
        # Try to parse as number
        try:
            if '.' in value:
                return float(value)
            else:
                return int(value)
        except ValueError:
            pass
        
        # Try to parse as boolean
        if value.lower() == 'true':
            return True
        elif value.lower() == 'false':
            return False
        
        # Try to parse as list
        if value.startswith('[') and value.endswith(']'):
            try:
                return json.loads(value)
            except json.JSONDecodeError:
                pass
        
        return value
    
    def _parse_ini_value(self, value: str) -> Any:
        """Parse INI-style value."""
        value = value.strip()
        
        # Remove quotes
        if value.startswith('"') and value.endswith('"'):
            return value[1:-1]
        elif value.startswith("'") and value.endswith("'"):
            return value[1:-1]
        
        # Try to parse as number
        try:
            if '.' in value:
                return float(value)
            else:
                return int(value)
        except ValueError:
            pass
        
        # Try to parse as boolean
        if value.lower() == 'true':
            return True
        elif value.lower() == 'false':
            return False
        
        return value
    
    def _parse_txt_value(self, value: str) -> Any:
        """Parse text configuration value."""
        return self._parse_cfg_value(value)
    
    def _validate_config_structure(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate and normalize configuration structure."""
        # Add NLM-specific validation
        validated_config = {}
        
        for key, value in config.items():
            normalized_key = self._normalize_key(key)
            validated_config[normalized_key] = self._validate_value(key, value)
        
        return validated_config
    
    def _normalize_key(self, key: str) -> str:
        """Normalize configuration key to standard format."""
        # Convert to snake_case if it's in camelCase or kebab-case
        import re
        
        # Convert camelCase to snake_case
        key = re.sub(r'(?<!^)(?=[A-Z])', '_', key).lower()
        
        # Convert kebab-case to snake_case
        key = key.replace('-', '_')
        
        # Remove common prefixes
        key = re.sub(r'^(config_|nlm_|brain_|world_|agent_|simulation_)', '', key)
        
        return key
    
    def _validate_value(self, key: str, value: Any) -> Any:
        """Validate configuration value based on key."""
        # Type validation based on key
        key_lower = key.lower()
        
        if 'count' in key_lower or 'size' in key_lower or 'steps' in key_lower:
            return int(value)
        elif 'rate' in key_lower or 'ratio' in key_lower or 'probability' in key_lower or 'weight' in key_lower:
            return float(value)
        elif 'enable' in key_lower or 'active' in key_lower or 'flag' in key_lower:
            return bool(value)
        elif 'time' in key_lower or 'duration' in key_lower or 'delay' in key_lower:
            return float(value)
        elif 'seed' in key_lower:
            return int(value)
        
        return value
    
    def load_multiple_configs(self, filepaths: List[str]) -> Dict[str, Dict[str, Any]]:
        """Load multiple configuration files, merging them."""
        merged_config = {}
        
        for filepath in filepaths:
            config = self.load_config(filepath)
            if config:
                # Deep merge configuration
                merged_config = self._deep_merge(merged_config, config)
        
        return merged_config
    
    def _deep_merge(self, base: Dict[str, Any], override: Dict[str, Any]) -> Dict[str, Any]:
        """Deep merge two dictionaries."""
        result = base.copy()
        
        for key, value in override.items():
            if key in result and isinstance(result[key], dict) and isinstance(value, dict):
                result[key] = self._deep_merge(result[key], value)
            else:
                result[key] = value
        
        return result
    
    def save_config(self, config: Dict[str, Any], filepath: str) -> bool:
        """Save configuration to file in specified format."""
        filepath = str(Path(filepath).resolve())
        ext = Path(filepath).suffix.lower()
        
        try:
            if ext == '.json':
                with open(filepath, 'w') as f:
                    json.dump(config, f, indent=2)
            elif ext in ['.yaml', '.yml']:
                try:
                    import yaml
                    with open(filepath, 'w') as f:
                        yaml.dump(config, f, default_flow_style=False)
                except ImportError:
                    print("PyYAML not installed. Cannot save YAML file.")
                    return False
            else:
                # For other formats, save as Python-like format
                with open(filepath, 'w') as f:
                    f.write(self._config_to_text_format(config))
            
            return True
        except Exception as e:
            print(f"Error saving configuration to {filepath}: {e}")
            return False
    
    def _config_to_text_format(self, config: Dict[str, Any]) -> str:
        """Convert configuration to text format for simple config files."""
        lines = []
        
        for key, value in config.items():
            if isinstance(value, dict):
                lines.append(f"[{key}]")
                for subkey, subvalue in value.items():
                    lines.append(f"  {subkey} = {subvalue}")
                lines.append("")
            elif isinstance(value, list):
                lines.append(f"{key} = {json.dumps(value)}")
            else:
                lines.append(f"{key} = {value}")
        
        return "\n".join(lines)
    
    def validate_config_schema(self, config: Dict[str, Any], schema: Optional[Dict[str, Any]] = None) -> List[str]:
        """Validate configuration against schema."""
        errors = []
        
        if not schema:
            return errors
        
        # Basic schema validation
        for key, expected_type in schema.items():
            if key not in config:
                errors.append(f"Missing required key: {key}")
                continue
            
            value = config[key]
            if not isinstance(value, expected_type):
                errors.append(f"Invalid type for key '{key}': expected {expected_type}, got {type(value)}")
        
        return errors


class ConfigValidator:
    """Configuration validation and analysis utilities."""
    
    @staticmethod
    def analyze_config(config: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze configuration for potential issues and optimizations."""
        analysis = {
            'total_keys': len(config),
            'type_distribution': {},
            'warnings': [],
            'suggestions': [],
            'complexity_score': 0.0
        }
        
        # Count types
        type_counts = {}
        for value in config.values():
            type_name = type(value).__name__
            type_counts[type_name] = type_counts.get(type_name, 0) + 1
        
        analysis['type_distribution'] = type_counts
        
        # Check for potential issues
        for key, value in config.items():
            key_lower = key.lower()
            
            # Check for extremely large values
            if isinstance(value, (int, float)):
                if abs(value) > 1e9:
                    analysis['warnings'].append(f"Very large value for key '{key}': {value}")
                elif abs(value) < 1e-9 and value != 0:
                    analysis['warnings'].append(f"Very small value for key '{key}': {value}")
            
            # Check for string keys that might need normalization
            if isinstance(key, str) and ' ' in key:
                analysis['suggestions'].append(f"Consider normalizing key '{key}' to remove spaces")
        
        # Calculate complexity score (simplified)
        complexity = len(type_counts) * 0.1 + len(analysis['warnings']) * 0.2
        analysis['complexity_score'] = min(complexity, 10.0)
        
        return analysis
    
    @staticmethod
    def merge_configs(*configs: Dict[str, Any]) -> Dict[str, Any]:
        """Merge multiple configuration dictionaries."""
        result = {}
        
        for config in configs:
            result.update(config)
        
        return result
    
    @staticmethod
    def diff_configs(config1: Dict[str, Any], config2: Dict[str, Any]) -> Dict[str, Any]:
        """Find differences between two configurations."""
        diff = {
            'added': {},
            'removed': {},
            'changed': {}
        }
        
        keys1 = set(config1.keys())
        keys2 = set(config2.keys())
        
        # Added keys
        for key in keys2 - keys1:
            diff['added'][key] = config2[key]
        
        # Removed keys
        for key in keys1 - keys2:
            diff['removed'][key] = config1[key]
        
        # Changed keys
        for key in keys1 & keys2:
            if config1[key] != config2[key]:
                diff['changed'][key] = {
                    'from': config1[key],
                    'to': config2[key]
                }
        
        return diff


# Default NLM configuration schemas
NLM_SCHEMAS = {
    'default': {
        'brain.neuron_count': int,
        'brain.synapse_density': (float, 0.0, 1.0),
        'brain.connection_probability': (float, 0.0, 1.0),
        'simulation_timestep': float,
        'random_seed': int,
        'plasticity.stdp.enable': bool,
        'plasticity.stdp.learning_rate': (float, 0.0, 1.0),
        'plasticity.hebbian.enable': bool,
        'plasticity.structural.enable': bool,
        'neuromod.dopamine.scale': float,
        'neuromod.curiosity.enable': bool,
        'neuromod.novelty.enable': bool,
    },
    'experimental': {
        'brain.neuron_count': (int, 100, 10000),
        'brain.synapse_density': (float, 0.0, 1.0),
        'simulation_timestep': (float, 0.0, 0.1),
        'random_seed': int,
        'plasticity.stdp.enable': bool,
        'plasticity.stdp.learning_rate': (float, 0.0, 1.0),
        'plasticity.hebbian.enable': bool,
        'neuromod.dopamine.scale': (float, 0.0, 5.0),
        'development.enable': bool,
        'memory.enable': bool,
        'prediction.enable': bool,
    }
}

# Factory functions for common configuration patterns
CONFIG_PATTERNS = {
    'development': {
        'brain.neuron_count': 1000,
        'brain.synapse_density': 0.1,
        'plasticity.stdp.enable': True,
        'plasticity.stdp.learning_rate': 0.001,
        'plasticity.hebbian.enable': True,
        'plasticity.structural.enable': True,
        'neuromod.dopamine.scale': 1.0,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'development.enable': True,
        'memory.enable': True,
    },
    'real_time': {
        'brain.neuron_count': 100,
        'brain.synapse_density': 0.05,
        'simulation_timestep': 0.01,
        'random_seed': 42,
        'plasticity.stdp.enable': False,  # Disable for speed
        'plasticity.hebbian.enable': True,
        'neuromod.dopamine.scale': 0.5,
        'neuromod.curiosity.enable': False,  # Disable for speed
    },
    'research': {
        'brain.neuron_count': 5000,
        'brain.synapse_density': 0.15,
        'simulation_timestep': 0.001,
        'random_seed': 12345,
        'plasticity.stdp.enable': True,
        'plasticity.stdp.learning_rate': 0.002,
        'plasticity.hebbian.enable': True,
        'plasticity.structural.enable': True,
        'neuromod.dopamine.scale': 2.0,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'development.enable': True,
        'memory.enable': True,
        'prediction.enable': True,
    }
}

# Utility functions for advanced users
def create_advanced_config(config_type: str = 'default', **overrides) -> Dict[str, Any]:
    """Create advanced configuration with validation."""
    if config_type not in CONFIG_PATTERNS:
        raise ValueError(f"Unknown config_type: {config_type}")
    
    # Start with base pattern
    config = CONFIG_PATTERNS[config_type].copy()
    
    # Apply overrides
    config.update(overrides)
    
    # Validate against schema
    schema = NLM_SCHEMAS.get(config_type, NLM_SCHEMAS['default'])
    validator = ConfigValidator()
    errors = validator.validate_config_schema(config, schema)
    
    if errors:
        raise ValueError(f"Configuration validation errors: {'; '.join(errors)}")
    
    return config
def export_config_for_reproduction(config: Dict[str, Any], filepath: str) -> bool:
    """Export configuration for reproducibility."""
    import datetime
    
    export_data = {
        'config': config,
        'metadata': {
            'exported_at': datetime.datetime.now().isoformat(),
            'exported_by': 'NLM Advanced Config',
            'version': '1.0'
        }
    }
    
    parser = AdvancedConfigParser()
    return parser.save_config(export_data, filepath)
def import_config_from_reproduction(filepath: str) -> Optional[Dict[str, Any]]:
    """Import configuration from reproduction export."""
    parser = AdvancedConfigParser()
    data = parser.load_config(filepath)
    
    if data and 'config' in data:
        return data['config']
    
    return None
# Example advanced configuration files
ADVANCED_EXAMPLES = {
    'json_example': '{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.12,
        "connection_probability": 0.08
    },
    "simulation": {
        "timestep": 0.001,
        "duration": 1000.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0005
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.5,
            "baseline": 0.1,
            "burst_strength": 2.0
        },
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.3,
            "novelty_threshold": 0.05
        },
        "novelty": {
            "enable": true,
            "detection_threshold": 0.01,
            "adaptation_rate": 0.1
        }
    },
    "development": {
        "enable": true,
        "stages": [
            {"name": "initial", "duration": 100, "plasticity": 1.0},
            {"name": "critical_period", "duration": 500, "plasticity": 0.5},
            {"name": "maturation", "duration": 1000, "plasticity": 0.2},
            {"name": "adult", "duration": "unlimited", "plasticity": 0.1}
        ]
    }
}',
    'yaml_example': '''# Advanced NLM Configuration (YAML)
brain:
  neuron_count: 3000
  synapse_density: 0.15
  connection_probability: 0.1
  neuron_types:
    excitatory: 0.8
    inhibitory: 0.15
    modulatory: 0.05

simulation:
  timestep: 0.001
  duration: 500
  random_seed: 42

plasticity:
  stdp:
    enable: true
    learning_rate: 0.002
    tau_plus: 15.0
    tau_minus: 15.0
    a_plus: 0.01
    a_minus: 0.01
  hebbian:
    enable: true
    learning_rate: 0.001
    trace_decay: 0.1
  structural:
    enable: true
    synaptogenesis_rate: 0.0002
    pruning_rate: 0.00002
    min_synapses: 10
    max_synapses: 1000

neuromodulation:
  dopamine:
    scale: 2.0
    baseline: 0.2
    burst_strength: 3.0
    adaptation_time: 100
  curiosity:
    enable: true
    exploration_rate: 0.4
    novelty_threshold: 0.03
    reward_scaling: 1.5
  novelty:
    enable: true
    detection_threshold: 0.02
    adaptation_rate: 0.15
    history_size: 100

development:
  enable: true
  stages:
    - name: initial
      duration: 200
      plasticity: 1.0
      neurogenesis: true
    - name: critical_period
      duration: 1000
      plasticity: 0.3
      neurogenesis: false
    - name: maturation
      duration: 2000
      plasticity: 0.1
      neurogenesis: false
    - name: adult
      duration: unlimited
      plasticity: 0.05
      neurogenesis: false

memory:
  enable: true
  working_memory:
    capacity: 50
    decay_rate: 0.01
    interference: true
  episodic_memory:
    storage_limit: 1000
    consolidation_interval: 100
    replay_probability: 0.1
  associative_memory:
    pattern_size: 20
    similarity_threshold: 0.7
    forgetting_rate: 0.001

prediction:
  enable: true
  prediction_horizon: 10
  error_sensitivity: 0.5
  confidence_threshold: 0.8

cognition:
  enable: true
  planning_horizon: 5
  attention_span: 50
  working_memory_capacity: 20
''',
    'cfg_example': '''# Traditional Config File Example
# NLM Configuration for Research

# Brain Configuration
brain.neuron_count = 5000
brain.synapse_density = 0.12
brain.connection_probability = 0.08
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0

# Simulation Configuration
simulation.timestep = 0.001
simulation.duration = 1000.0
simulation.random_seed = 12345

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.0005
plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.0001
plasticity.structural.pruning_rate = 0.00001

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.dopamine.baseline = 0.1
neuromod.curiosity.enable = true
neuromod.curiosity.exploration_rate = 0.3
neuromod.novelty.enable = true
neuromod.novelty.threshold = 0.05

# Development Configuration
development.enable = true
development.stages.initial.duration = 100
development.stages.initial.plasticity = 1.0
development.stages.critical_period.duration = 500
development.stages.critical_period.plasticity = 0.5

# Performance Configuration
performance.max_neurons = 10000
performance.memory_pool_size = 1024
performance.event_queue_size = 8192

# Logging Configuration
logging.level = "INFO"
logging.file = "nlm_simulation.log"
logging.max_size = 10485760
'''
}

__all__ = [
    'AdvancedConfigParser',
    'ConfigValidator',
    'NLM_SCHEMAS',
    'CONFIG_PATTERNS',
    'create_advanced_config',
    'export_config_for_reproduction',
    'import_config_from_reproduction',
    'ADVANCED_EXAMPLES'
]
