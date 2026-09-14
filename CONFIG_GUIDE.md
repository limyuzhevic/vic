# Configuration Management for NLM

This document provides a comprehensive guide to NLM's configuration management system, including configuration file formats, validation rules, and best practices for configuration management.

## Overview

NLM's configuration system provides a flexible and extensible way to control brain simulation parameters, learning behavior, and system performance. The configuration system supports multiple formats, validation, and provides sensible defaults for different use cases.

## Configuration File Formats

NLM supports multiple configuration file formats:

### 1. JSON Format (`.json`)
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "simulation_timestep": 0.001,
  "connection_probability": 0.1,
  "random_seed": 42,
  "enable_reward_modulation": true,
  "enable_curiosity": true,
  "development_enabled": true,
  "structural_plasticity": true,
  "log_level": "INFO",
  "memory_capacity": {
    "working_memory": 100,
    "episodic_memory": 1000,
    "associative_memory": 500
  },
  "neuromodulation": {
    "dopamine_baseline": 0.1,
    "curiosity_threshold": 0.3,
    "novelty_sensitivity": 0.5
  },
  "plasticity": {
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "hebbian_learning_rate": 0.1,
    "reward_modulation_factor": 1.0
  }
}
```

### 2. YAML Format (`.yaml`)
```yaml
neuron_count: 1000
region_count: 2
simulation_timestep: 0.001
connection_probability: 0.1
random_seed: 42

enable_reward_modulation: true
enable_curiosity: true
enable_development: true
enable_structural_plasticity: true

log_level: INFO

memory_capacity:
  working_memory: 100
  episodic_memory: 1000
  associative_memory: 500

neuromodulation:
  dopamine_baseline: 0.1
  curiosity_threshold: 0.3
  novelty_sensitivity: 0.5

plasticity:
  stdp_ltp_weight: 0.01
  stdp_ltd_weight: 0.012
  stdp_tau: 20.0
  hebbian_learning_rate: 0.1
  reward_modulation_factor: 1.0
```

### 3. INI Format (`.ini`)
```ini
[core]
neuron_count = 1000
region_count = 2
simulation_timestep = 0.001
connection_probability = 0.1
random_seed = 42

[learning]
enable_reward_modulation = true
enable_curiosity = true
enable_development = true
enable_structural_plasticity = true

[memory]
working_memory_capacity = 100
episodic_memory_capacity = 1000
associative_memory_capacity = 500

[neuromodulation]
dopamine_baseline = 0.1
curiosity_threshold = 0.3
novelty_sensitivity = 0.5

[plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
hebbian_learning_rate = 0.1
reward_modulation_factor = 1.0
```

## Configuration Categories

### Core Configuration
#### Neuron and Brain Parameters
- **`neuron_count`** (integer, default: 1000): Total number of neurons in the brain
- **`region_count`** (integer, default: 2): Number of brain regions
- **`connection_probability`** (float, default: 0.1): Probability of synaptic connections
- **`simulation_timestep`** (float, default: 0.001): Simulation timestep in seconds
- **`random_seed`** (integer, default: 42): Random seed for reproducible simulations

#### Development Parameters
- **`development_enabled`** (boolean, default: true): Whether development processes are active
- **`development_synaptogenesis_rate`** (float, default: 0.0001): Rate of new synapse formation
- **`development_pruning_rate`** (float, default: 0.00001): Rate of synapse elimination
- **`critical_period_duration`** (float, default: 300.0): Duration of critical developmental period

### Learning Configuration
#### Neuromodulation
- **`enable_reward_modulation`** (boolean, default: true): Enable reward-based learning
- **`enable_curiosity`** (boolean, default: true): Enable curiosity-driven exploration
- **`enable_structural_plasticity`** (boolean, default: true): Enable structural plasticity

#### Plasticity Parameters
- **`stdp_ltp_weight`** (float, default: 0.01): STDP Long-Term Potentiation weight
- **`stdp_ltd_weight`** (float, default: 0.012): STDP Long-Term Depression weight
- **`stdp_tau`** (float, default: 20.0): STDP time constant
- **`hebbian_learning_rate`** (float, default: 0.1): Hebbian learning rate
- **`reward_modulation_factor`** (float, default: 1.0): Reward modulation strength

#### Novelty and Prediction
- **`novelty_threshold`** (float, default: 0.5): Threshold for novelty detection
- **`prediction_error_sensitivity`** (float, default: 1.0): Sensitivity to prediction errors
- **`confidence_decay_rate`** (float, default: 0.95): Rate of confidence decay

### Memory Configuration
#### Working Memory
- **`working_memory_capacity`** (integer, default: 100): Maximum number of active traces
- **`working_memory_decay_rate`** (float, default: 0.9): Rate of trace decay
- **`working_memory_competition_strength`** (float, default: 1.0): Competition strength

#### Episodic Memory
- **`episodic_memory_capacity`** (integer, default: 1000): Maximum number of episodes
- **`episodic_memory_consolidation_threshold`** (float, default: 0.7): Consolidation threshold
- **`episodic_memory_replay_interval`** (integer, default: 100): Replay interval

#### Associative Memory
- **`associative_memory_capacity`** (integer, default: 500): Maximum number of associations
- **`associative_memory_strength`** (float, default: 1.0): Association strength
- **`pattern_matching_threshold`** (float, default: 0.6): Pattern matching threshold

### Performance Configuration
#### Resource Management
- **`memory_pool_size`** (integer, default: 1000): Size of memory pools
- **`event_queue_capacity`** (integer, default: 10000): Event queue capacity
- **`simd_optimization_level`** (integer, default: 2): SIMD optimization level

#### Parallel Processing
- **`parallel_processing_enabled`** (boolean, default: true): Enable parallel processing
- **`max_threads`** (integer, default: 4): Maximum number of threads
- **`load_balancing_strategy`** (string, default: "dynamic"): Load balancing strategy

#### Checkpointing
- **`checkpoint_enabled`** (boolean, default: true): Enable checkpointing
- **`checkpoint_interval`** (integer, default: 1000): Checkpoint interval
- **`checkpoint_compression`** (string, default: "balanced"): Compression level

### Logging Configuration
- **`log_level`** (string, default: "INFO"): Logging level (TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)
- **`log_to_file`** (boolean, default: false): Whether to log to file
- **`log_filename`** (string, default: "nlm.log"): Log file name
- **`log_max_size`** (integer, default: 10485760): Maximum log file size (10MB)
- **`log_rotation_count`** (integer, default: 5): Number of log rotations
- **`log_format`** (string, default: "detailed"): Log format (simple, detailed, json)

### Visualization Configuration
- **`visualization_enabled`** (boolean, default: false): Enable visualization
- **`visualization_update_rate`** (float, default: 30.0): Visualization update rate
- **`visualization_backend`** (string, default: "auto"): Visualization backend
- **`visualization_output_file`** (string, default: "nlm_viz.html"): Output file for visualization

## Configuration Validation

### Required Fields
The following fields must be present in the configuration:
- `neuron_count` (≥ 1)
- `region_count` (≥ 1)
- `simulation_timestep` (> 0)
- `connection_probability` [0, 1]
- `random_seed` (≥ 0)

### Type Validation
```python
def validate_config(config_dict):
    # Required integer fields
    required_ints = ['neuron_count', 'region_count', 'random_seed']
    for field in required_ints:
        if field in config_dict and not isinstance(config_dict[field], int):
            raise ValueError(f"{field} must be an integer")
    
    # Required float fields
    required_floats = ['simulation_timestep', 'connection_probability']
    for field in required_floats:
        if field in config_dict and not isinstance(config_dict[field], (int, float)):
            raise ValueError(f"{field} must be a number")
    
    # Required boolean fields
    required_bools = ['enable_reward_modulation', 'enable_curiosity', 
                     'enable_development', 'enable_structural_plasticity',
                     'log_to_file', 'visualization_enabled']
    for field in required_bools:
        if field in config_dict and not isinstance(config_dict[field], bool):
            raise ValueError(f"{field} must be a boolean")
    
    # Range validation
    if 'neuron_count' in config_dict and config_dict['neuron_count'] < 1:
        raise ValueError("neuron_count must be ≥ 1")
    
    if 'region_count' in config_dict and config_dict['region_count'] < 1:
        raise ValueError("region_count must be ≥ 1")
    
    if 'simulation_timestep' in config_dict and config_dict['simulation_timestep'] <= 0:
        raise ValueError("simulation_timestep must be > 0")
    
    if 'connection_probability' in config_dict and not (0 <= config_dict['connection_probability'] <= 1):
        raise ValueError("connection_probability must be in [0, 1]")
    
    # Log level validation
    if 'log_level' in config_dict and config_dict['log_level'] not in ['TRACE', 'DEBUG', 'INFO', 'WARNING', 'ERROR', 'FATAL']:
        raise ValueError("log_level must be one of: TRACE, DEBUG, INFO, WARNING, ERROR, FATAL")
```

### Configuration Inheritance
Configuration supports inheritance from base configurations:

```python
class ConfigLoader:
    def load_config(self, filepath, inherit_from=None):
        # Load the configuration file
        with open(filepath, 'r') as f:
            config = json.load(f)
        
        # Apply inheritance if specified
        if inherit_from:
            base_config = self.load_config(inherit_from)
            config = {**base_config, **config}  # Override base config
        
        # Validate the merged configuration
        self.validate_config(config)
        
        return config
```

## Configuration Management API

### Loading Configuration
```python
from nlm.config import ConfigLoader

# Load from file
loader = ConfigLoader()
config = loader.load_config("configs/my_config.json")

# Load with inheritance
config = loader.load_config("configs/experiment.json", inherit_from="configs/base.json")

# Load from command line
config = loader.load_from_args(sys.argv)
```

### Saving Configuration
```python
from nlm.config import ConfigSaver

saver = ConfigSaver()

# Save to file
saver.save_config(config, "configs/experiment.json")

# Save with compression
saver.save_config(config, "configs/experiment.json.gz", compression="gzip")

# Get configuration as string
summary = saver.get_config_summary(config)
```

### Configuration Comparison
```python
from nlm.config import ConfigComparator

comparator = ConfigComparator()

# Compare two configurations
config1 = loader.load_config("configs/config1.json")
config2 = loader.load_config("configs/config2.json")

differences = comparator.compare_configs(config1, config2)
for diff in differences:
    print(f"Key: {diff['key']}")
    print(f"  Value 1: {diff['value1']}")
    print(f"  Value 2: {diff['value2']}")
    print(f"  Type: {diff['type']}")
```

### Configuration Diff
```python
from nlm.config import ConfigDiff

diff = ConfigDiff()

# Generate diff between configurations
diff_text = diff.generate_diff(config1, config2)
print(diff_text)

# Apply diff to configuration
config2_updated = diff.apply_diff(config1, diff_text)
```

## Best Practices

### Configuration Organization

1. **Use Configuration Files for Different Environments**:
```bash
# Development configuration
configs/dev_config.json

# Production configuration
configs/prod_config.json

# Experiment configuration
configs/experiment_1.json
configs/experiment_2.json
```

2. **Configuration Templates**:
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "random_seed": 42,
  "enable_reward_modulation": true,
  "enable_curiosity": true,
  "enable_development": true,
  "enable_structural_plasticity": true,
  "log_level": "INFO"
}
```

3. **Version Control for Configuration**:
```bash
# Add configuration files to version control
git add configs/
git commit -m "Add experiment configuration"
```

### Configuration Validation Best Practices

1. **Validate Early**: Validate configuration as early as possible, before resource allocation
2. **Use Default Values**: Provide sensible defaults for all configuration options
3. **Document Configuration**: Document all configuration options with examples
4. **Test Configuration**: Test configuration with unit tests and integration tests
5. **Monitor Configuration**: Monitor configuration changes in production

### Configuration Usage Best Practices

1. **Use Environment Variables**: Override configuration values with environment variables
```bash
export NLM_NEURON_COUNT=2000
export NLM_RANDOM_SEED=123
./nlm simulate 1000
```

2. **Configuration Profiles**: Use configuration profiles for different scenarios
```bash
# Small experiment configuration
nlm config load configs/small_experiment.json

# Large scale configuration
nlm config load configs/large_scale.json

# Reproduction configuration
nlm config load configs/reproduction.json
```

3. **Configuration Backup**: Backup important configuration files
```bash
# Backup configuration
cp configs/experiment.json configs/experiment_$(date +%Y%m%d).json
```

## Configuration Examples

### Basic Configuration
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "simulation_timestep": 0.001,
  "connection_probability": 0.1,
  "random_seed": 42
}
```

### Learning Configuration
```json
{
  "neuron_count": 500,
  "enable_reward_modulation": true,
  "enable_curiosity": true,
  "enable_development": true,
  "enable_structural_plasticity": true,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012
}
```

### Memory-Intensive Configuration
```json
{
  "neuron_count": 5000,
  "region_count": 10,
  "enable_reward_modulation": true,
  "enable_curiosity": true,
  "working_memory_capacity": 500,
  "episodic_memory_capacity": 5000,
  "associative_memory_capacity": 2000,
  "log_level": "DEBUG"
}
```

### Fast Configuration
```json
{
  "neuron_count": 100,
  "region_count": 1,
  "simulation_timestep": 0.01,
  "connection_probability": 0.05,
  "enable_reward_modulation": false,
  "enable_curiosity": false,
  "enable_development": false,
  "enable_structural_plasticity": false,
  "log_level": "ERROR"
}
```

### Development Configuration
```json
{
  "neuron_count": 2000,
  "region_count": 5,
  "enable_reward_modulation": true,
  "enable_curiosity": true,
  "enable_development": true,
  "enable_structural_plasticity": true,
  "log_level": "DEBUG",
  "log_to_file": true,
  "visualization_enabled": true
}
```

## Configuration Migration

### Migrate from INI to JSON
```python
def migrate_ini_to_json(ini_content):
    # Parse INI content
    import configparser
    config = configparser.ConfigParser()
    config.read_string(ini_content)
    
    # Convert to JSON
    json_config = {}
    for section in config.sections():
        for key, value in config.items(section):
            # Convert string values to appropriate types
            if value.lower() in ('true', 'false'):
                json_config[f"{section}.{key}"] = value.lower() == 'true'
            elif value.isdigit():
                json_config[f"{section}.{key}"] = int(value)
            elif is_float(value):
                json_config[f"{section}.{key}"] = float(value)
            else:
                json_config[f"{section}.{key}"] = value
    
    return json_config
```

### Configuration Upgrade
```python
def upgrade_config(config_dict, from_version, to_version):
    # Handle configuration upgrades between versions
    if from_version < "1.0" and to_version >= "1.0":
        # Upgrade logic for 1.0
        if 'enable_stdp' in config_dict:
            config_dict['stdp_ltp_weight'] = config_dict.pop('enable_stdp') * 0.01
        if 'enable_hebbian' in config_dict:
            config_dict['hebbian_learning_rate'] = config_dict.pop('enable_hebbian') * 0.1
    
    return config_dict
```

## Configuration Security

### Sensitive Configuration
```json
{
  "database_url": "postgresql://user:password@localhost/db",
  "api_key": "secret-api-key",
  "encryption_key": "base64-encoded-key"
}
```

### Configuration Access Control
```python
class ConfigAccessControl:
    def __init__(self):
        self.permissions = {}
    
    def can_access_config(self, user, config_key):
        # Check user permissions
        if user.is_admin:
            return True
        if user in self.permissions and config_key in self.permissions[user]:
            return self.permissions[user][config_key]
        return False
    
    def set_permission(self, user, config_key, can_access):
        if user not in self.permissions:
            self.permissions[user] = {}
        self.permissions[user][config_key] = can_access
```

## Configuration Monitoring

### Configuration Monitoring API
```python
from nlm.config import ConfigMonitor

monitor = ConfigMonitor()

# Monitor configuration changes
def on_config_change(config_key, old_value, new_value):
    print(f"Configuration changed: {config_key} {old_value} -> {new_value}")

monitor.add_change_listener(on_config_change)

# Start monitoring
monitor.start_monitoring()

# Stop monitoring
monitor.stop_monitoring()
```

### Configuration Metrics
```python
class ConfigMetrics:
    def __init__(self):
        self.config_access_count = 0
        self.config_change_count = 0
        self.config_validation_count = 0
    
    def record_config_access(self, config_key):
        self.config_access_count += 1
    
    def record_config_change(self, config_key):
        self.config_change_count += 1
    
    def record_config_validation(self, config_key, is_valid):
        self.config_validation_count += 1
        if not is_valid:
            self.validation_failure_count += 1
    
    def get_metrics(self):
        return {
            'config_access_count': self.config_access_count,
            'config_change_count': self.config_change_count,
            'config_validation_count': self.config_validation_count,
            'validation_failure_rate': (self.validation_failure_count / self.config_validation_count) if self.config_validation_count > 0 else 0
        }
```

## Configuration Testing

### Unit Tests
```python
import unittest
from nlm.config import ConfigLoader

class TestConfigValidation(unittest.TestCase):
    def test_required_fields(self):
        loader = ConfigLoader()
        config = {"neuron_count": 100, "region_count": 2}
        # Should raise ValueError for missing required fields
        with self.assertRaises(ValueError):
            loader.validate_config(config)
    
    def test_type_validation(self):
        loader = ConfigLoader()
        config = {"neuron_count": "invalid", "region_count": 2}
        # Should raise ValueError for wrong type
        with self.assertRaises(ValueError):
            loader.validate_config(config)
    
    def test_range_validation(self):
        loader = ConfigLoader()
        config = {"neuron_count": -1, "region_count": 2}
        # Should raise ValueError for out-of-range value
        with self.assertRaises(ValueError):
            loader.validate_config(config)

class TestConfigLoading(unittest.TestCase):
    def test_load_from_file(self):
        loader = ConfigLoader()
        config = loader.load_config("configs/test_config.json")
        self.assertIn("neuron_count", config)
        self.assertEqual(config["neuron_count"], 1000)
    
    def test_load_from_args(self):
        # Test command line argument parsing
        pass
```

### Integration Tests
```python
import pytest
from nlm.config import ConfigLoader
from nlm.brain import Brain

@pytest.mark.integration
def test_config_brain_integration():
    loader = ConfigLoader()
    config = loader.load_config("configs/integration_test.json")
    
    brain = Brain(config)
    assert brain.initialize()
    
    # Test that configuration affects brain behavior
    assert brain.getTotalNeuronCount() == config["neuron_count"]
    assert brain.getRegionCount() == config["region_count"]
```

## Configuration Documentation

### Generated Documentation
```python
def generate_config_docs(config_dict):
    """Generate configuration documentation"""
    docs = "# NLM Configuration Documentation\n\n"
    
    docs += "## Configuration Options\n\n"
    
    for key, value in sorted(config_dict.items()):
        docs += f"### {key}\n"
        docs += f"Type: {type(value).__name__}\n"
        docs += f"Value: {value}\n"
        docs += f"Description: {get_config_description(key)}\n\n"
    
    return docs
```

### Configuration Help
```python
class ConfigHelp:
    HELP_TEXT = {
        "neuron_count": "Total number of neurons in the brain",
        "region_count": "Number of brain regions",
        "simulation_timestep": "Simulation timestep in seconds",
        "connection_probability": "Probability of synaptic connections",
        "random_seed": "Random seed for reproducible simulations",
        "enable_reward_modulation": "Enable reward-based learning",
        "enable_curiosity": "Enable curiosity-driven exploration",
        "enable_development": "Enable developmental processes",
        "enable_structural_plasticity": "Enable structural plasticity",
        "log_level": "Logging level (TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)",
        "working_memory_capacity": "Capacity of working memory",
        "episodic_memory_capacity": "Capacity of episodic memory",
        "associative_memory_capacity": "Capacity of associative memory"
    }
    
    def get_help(self, config_key):
        return self.HELP_TEXT.get(config_key, "No help available")
```

## Configuration Performance

### Optimized Configuration Access
```python
class OptimizedConfig:
    def __init__(self, config_dict):
        self.config = config_dict
        self._cache = {}
    
    def get(self, key, default=None):
        if key not in self._cache:
            self._cache[key] = self.config.get(key, default)
        return self._cache[key]
    
    def set(self, key, value):
        self.config[key] = value
        # Clear cache for this key
        self._cache.pop(key, None)
    
    def get_all(self):
        return self.config.copy()
```

### Configuration Serialization
```python
class ConfigSerializer:
    def serialize(self, config_dict, format='json'):
        if format == 'json':
            return json.dumps(config_dict, indent=2)
        elif format == 'yaml':
            import yaml
            return yaml.dump(config_dict)
        elif format == 'ini':
            return self._serialize_ini(config_dict)
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    def deserialize(self, data, format='json'):
        if format == 'json':
            return json.loads(data)
        elif format == 'yaml':
            import yaml
            return yaml.safe_load(data)
        elif format == 'ini':
            return self._deserialize_ini(data)
        else:
            raise ValueError(f"Unsupported format: {format}")
```

## Conclusion

NLM's configuration management system provides a robust, flexible, and extensible way to control brain simulation parameters. The system supports multiple file formats, comprehensive validation, and provides best practices for configuration management in different environments.

Key features include:

1. **Multiple File Formats**: JSON, YAML, and INI support
2. **Comprehensive Validation**: Type checking, range validation, and required field validation
3. **Configuration Inheritance**: Support for configuration inheritance and overriding
4. **Configuration Management**: Loading, saving, comparing, and merging configurations
5. **Configuration Testing**: Unit tests and integration tests for configuration
6. **Configuration Documentation**: Generated documentation and help system
7. **Configuration Monitoring**: Change tracking and metrics collection
8. **Configuration Security**: Access control and sensitive data handling

This configuration system enables NLM to support diverse simulation scenarios while maintaining consistency, reliability, and ease of use.
