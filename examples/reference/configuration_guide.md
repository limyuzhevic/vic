# Configuration Guide

This document provides a comprehensive guide to all available configuration options in the NLM (Neural Learning Machine) system. It covers configuration categories, available options, their purposes, and usage examples.

## Overview

Configuration in NLM controls the behavior, structure, and properties of neural simulation components. The configuration system supports:

- **Basic settings** (neuron count, world dimensions)
- **Learning parameters** (plasticity, neuromodulation)
- **Developmental settings** (growth, pruning)
- **Neuromodulation parameters** (dopamine, curiosity, novelty)
- **Prediction and planning settings**
- **Memory system settings**
- **Logging and monitoring options**

## Configuration Categories

### 1. Basic Simulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |
| `max_simulation_steps` | int | 10000 | Maximum number of simulation steps |
| `simulation_time_limit` | float | 0.0 | Simulation time limit in seconds (0 = unlimited) |

### 2. Neural Structure Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Connection probability for random connectivity |

### 3. Plasticity Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity_learning_rate` | float | 0.01 | Base learning rate for plasticity |
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_time_constant` | float | 20.0 | STDP time constant |

### 4. Development Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `development_synaptogenesis_rate` | float | 0.001 | Rate of synapse formation during development |
| `development_pruning_rate` | float | 0.0001 | Rate of synapse pruning during development |

### 5. Neuromodulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `dopamine_baseline` | float | 0.0 | Baseline dopamine level |
| `reward_discount_factor` | float | 0.99 | Discount factor for reward calculation |

### 6. Environment Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `environment_name` | str | GridWorld | Name of the simulation environment |
| `environment_width` | int | 10 | Width of the environment |
| `environment_height` | int | 10 | Height of the environment |

### 7. World-Specific Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `vision_width` | int | 8 | Vision width in world coordinates |
| `vision_height` | int | 8 | Vision height in world coordinates |
| `world_width` | int | 10 | World width in cells |
| `world_height` | int | 10 | World height in cells |

### 8. Logging Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_level` | str | INFO | Logging level (DEBUG, INFO, WARN, ERROR) |
| `log_to_file` | bool | false | Whether to log to file |
| `log_filename` | str | nlm.log | Log file name |

### 9. Visualization Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `visualization_enabled` | bool | false | Whether visualization is enabled |
| `visualization_update_rate` | float | 30.0 | Visualization update rate in Hz |

## Configuration Files

### Default Configuration File

The default configuration file is located at:
```
/workspace/app/configs/default.cfg
```

**Content of default.cfg:**
```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.0
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

## Configuration Loading

### Loading from File

```python
import pynlm

# Load configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("configs/my_config.json")
```

### Loading from Command Line

```python
import pynlm
import sys

# Load configuration from command line
config = pynlm.createDefaultConfig()
config.loadFromArgs(len(sys.argv), sys.argv)
```

### Loading from Dictionary

```python
import pynlm
import json

# Load configuration from dictionary
config_dict = {
    "neuron_count": 2000,
    "region_count": 5,
    "plasticity_learning_rate": 0.02,
    "environment_width": 20,
    "environment_height": 20
}

config = pynlm.createDefaultConfig()
# Apply configuration values from dictionary
for key, value in config_dict.items():
    config.set(key, value)
```

## Configuration Validation

### Schema Validation

```python
import pynlm

# Enable schema validation
config = pynlm.createDefaultConfig()
config.setValidationEnabled(True)

# Validate configuration
validation_errors = config.validateConfiguration()

if validation_errors:
    print("Configuration validation errors:")
    for error in validation_errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### Custom Schema

```python
import pynlm

# Create custom schema
custom_schema = {
    "required_keys": ["neuron_count", "region_count", "plasticity_learning_rate"],
    "optional_keys": ["environment_width", "environment_height"],
    "value_ranges": {
        "neuron_count": (100, 10000),
        "region_count": (1, 10),
        "plasticity_learning_rate": (0.001, 0.1)
    }
}

# Create configuration with custom schema
config = pynlm.createDefaultConfig()
config.setValidationEnabled(True)
```

## Configuration Templates

### Creating Configuration Templates

```python
import pynlm

# Create configuration template
template_config = pynlm.createDefaultConfig()
template_config.set("neuron_count", 1000)
template_config.set("region_count", 2)
template_config.set("plasticity_learning_rate", 0.01)

# Save template to file
template_config.saveToFile("templates/baseline_config.cfg")
```

### Using Configuration Templates

```python
import pynlm

# Load configuration template
config = pynlm.createDefaultConfig()
config.loadFromFile("templates/baseline_config.cfg")

# Create brain with template configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Managing Templates

```python
import pynlm

# List all templates
config = pynlm.createDefaultConfig()
template_names = config.getTemplateNames()
print("Available templates:", template_names)

# Load specific template
if "baseline" in template_names:
    template_config = config.getTemplate("baseline")
```

## Configuration Monitoring

### Enabling Configuration Monitoring

```python
import pynlm

# Create configuration with monitoring
config = pynlm.createDefaultConfig()

# Set up monitoring
config_monitor = ConfigMonitor()
config.setMonitor(config_monitor)
```

### Tracking Configuration Changes

```python
import pynlm

# Create configuration with monitoring
config = pynlm.createDefaultConfig()
config_monitor = ConfigMonitor()
config.setMonitor(config_monitor)

# Modify configuration
config.set("neuron_count", 1500)
config.set("region_count", 3)

# Get change history
change_history = config_monitor.getConfigurationHistory()
print("Configuration changes:")
for event in change_history:
    print(f"  - {event['timestamp']}: {event['event']}")
    if 'changes' in event:
        for change in event['changes']:
            print(f"    * {change['type']}: {change['key']}")
```

## Configuration Comparison

### Comparing Configurations

```python
import pynlm

# Create two configurations
config1 = pynlm.createDefaultConfig()
config1.set("neuron_count", 1000)
config1.set("region_count", 2)

config2 = pynlm.createDefaultConfig()
config2.set("neuron_count", 2000)
config2.set("region_count", 3)

# Compare configurations
differences = config1.compareConfigurations(config2, "Config 1", "Config 2")
print("Differences between configurations:")
for diff in differences:
    print(f"  - {diff['key']}: {diff['value1']} -> {diff['value2']}")
```

### Merging Configurations

```python
import pynlm

# Create two configurations
config1 = pynlm.createDefaultConfig()
config1.set("neuron_count", 1000)
config1.set("region_count", 2)

config2 = pynlm.createDefaultConfig()
config2.set("plasticity_learning_rate", 0.05)
config2.set("environment_width", 30)

# Merge configurations
merged_config = config1.mergeConfigurations(config2, "Merged")
print("Merged configuration:")
for key in merged_config.getKeys():
    print(f"  {key}: {merged_config.get(key)}")
```

## Configuration Export and Import

### Exporting Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("region_count", 3)

# Export configuration to JSON
config.exportConfiguration("config.json", "json")

# Export configuration to INI
config.exportConfiguration("config.ini", "ini")

# Export configuration to text
config.exportConfiguration("config.txt", "txt")
```

### Importing Configuration

```python
import pynlm

# Import configuration from JSON
config = pynlm.createDefaultConfig()
imported_config = config.importConfiguration("config.json", "json")

# Import configuration from INI
config = pynlm.createDefaultConfig()
imported_config = config.importConfiguration("config.ini", "ini")

# Import configuration from text
config = pynlm.createDefaultConfig()
imported_config = config.importConfiguration("config.txt", "txt")
```

## Configuration Best Practices

### 1. Validation

Always validate your configuration before use:

```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()
config.setValidationEnabled(True)

validation_errors = config.validateConfiguration()
if validation_errors:
    print("Configuration has errors:", validation_errors)
    # Fix errors or exit
```

### 2. Templates

Use templates for consistent configurations:

```python
import pynlm

# Create template for common configuration
template_config = pynlm.createDefaultConfig()
template_config.set("neuron_count", 1000)
template_config.set("region_count", 2)
template_config.set("plasticity_learning_rate", 0.01)

# Save template
template_config.saveToFile("templates/common.cfg")
```

### 3. Monitoring

Monitor configuration changes for debugging:

```python
import pynlm

# Set up monitoring
config = pynlm.createDefaultConfig()
config_monitor = ConfigMonitor()
config.setMonitor(config_monitor)

# Modify configuration
config.set("neuron_count", 1500)

# Check for changes
change_history = config_monitor.getConfigurationHistory()
```

### 4. Export and Import

Use export/import for configuration management:

```python
import pynlm

# Export configuration for backup
config.exportConfiguration("backup_config.json", "json")

# Import configuration for testing
config = pynlm.createDefaultConfig()
imported_config = config.importConfiguration("test_config.json", "json")
```

### 5. Incremental Configuration

Build configuration incrementally:

```python
import pynlm

# Start with default configuration
config = pynlm.createDefaultConfig()

# Apply base settings
config.set("neuron_count", 1000)
config.set("region_count", 2)

# Apply experiment-specific settings
config.set("plasticity_learning_rate", 0.02)

# Apply environment-specific settings
config.set("environment_width", 20)
config.set("environment_height", 20)
```

## Configuration Examples

### Example 1: Basic Configuration

```python
import pynlm

# Create basic configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 2)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 2: Advanced Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("region_count", 5)
config.set("plasticity_learning_rate", 0.05)
config.set("development_synaptogenesis_rate", 0.002)
config.set("development_pruning_rate", 0.0002)
config.set("environment_width", 30)
config.set("environment_height", 30)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 3: Developmental Configuration

```python
import pynlm

# Create developmental configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("plasticity_learning_rate", 0.1)
config.set("environment_width", 20)
config.set("environment_height", 20)

brain = pynlm.createBrain(config)
brain.initialize()

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
```

### Example 4: Neuromodulation Configuration

```python
import pynlm

# Create neuromodulation configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("region_count", 2)
config.set("dopamine_baseline", 0.1)
config.set("reward_discount_factor", 0.95)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable neuromodulation
dopamine = pynlm.createDopamine()
dopamine.setLevel(0.2)
```

## Configuration Troubleshooting

### Common Issues and Solutions

#### Problem 1: Configuration Validation Fails
**Solution:** Ensure configuration keys are valid and values are within expected ranges.

```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()
config.setValidationEnabled(True)

validation_errors = config.validateConfiguration()
if validation_errors:
    print("Fix configuration errors:", validation_errors)
```

#### Problem 2: Configuration Too Large
**Solution:** Reduce neuron count or simplify configuration.

```python
import pynlm

# Create configuration with reasonable size
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # Reduced from 10000
config.set("region_count", 3)
```

#### Problem 3: Configuration Not Serializable
**Solution:** Ensure all values are JSON-serializable.

```python
import pynlm

# Ensure values are serializable
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)  # Integer
config.set("plasticity_learning_rate", 0.01)  # Float
config.set("environment_name", "GridWorld")  # String
config.set("log_to_file", True)  # Boolean
```

#### Problem 4: Configuration Monitoring Not Working
**Solution:** Ensure monitoring is properly configured.

```python
import pynlm

# Set up monitoring
config = pynlm.createDefaultConfig()
config_monitor = ConfigMonitor()
config.setMonitor(config_monitor)

# Verify monitoring is working
if config.hasMonitor():
    print("Monitoring is enabled")
else:
    print("Monitoring is not enabled")
```

### Configuration Debugging

```python
import pynlm

# Create configuration with debugging
config = pynlm.createDefaultConfig()

# Set debug logging level
config.set("log_level", "DEBUG")
config.set("log_to_file", True)

# Create brain with debugging
brain = pynlm.createBrain(config)
brain.initialize()

# Log brain status
brain.logStatus()
```

## Configuration Files

### Configuration File Formats

| Format | Extension | Description |
|--------|-----------|-------------|
| JSON | .json | Human-readable, structured data |
| INI | .ini | Key-value pairs |
| TXT | .txt | Simple key-value pairs |

### Example Configuration Files

#### JSON Configuration
```json
{
  "configuration": {
    "neuron_count": 1000,
    "region_count": 2,
    "plasticity_learning_rate": 0.01,
    "environment_width": 10,
    "environment_height": 10
  },
  "timestamp": "2026-10-19"
}
```

#### INI Configuration
```ini
[configuration]
neuron_count=1000
region_count=2
plasticity_learning_rate=0.01
environment_width=10
environment_height=10

[metadata]
timestamp=2026-10-19
version=1.0
```

#### TXT Configuration
```txt
# Configuration file
# neuron count
neuron_count=1000
# region count
region_count=2
# learning rate
plasticity_learning_rate=0.01
# environment dimensions
environment_width=10
environment_height=10
```

## Configuration Security

### Encryption

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Set encryption key
encryption_key = "my-secret-key"
config.setEncryptionKey(encryption_key)

# Encrypt configuration
encrypted_data = config.encryptConfiguration(encryption_key)

# Decrypt configuration
decrypted_config = config.decryptConfiguration(encrypted_data, encryption_key)
```

### Digital Signatures

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Generate signature (simplified)
import hashlib
config_data = json.dumps(dict(config.getKeys()), sort_keys=True)
signature = hashlib.sha256(config_data.encode()).hexdigest()

# Validate signature
is_valid = config.validateSignature(signature, config_data)
```

## Configuration Performance

### Performance Optimization

```python
import pynlm

# Create performance-optimized configuration
config = pynlm.createDefaultConfig()

# Optimize for memory
config.set("neuron_count", 1000)  # Reduced from 10000

# Optimize for speed
config.set("plasticity_learning_rate", 0.01)  # Reduced from 0.05

# Optimize for learning
config.set("environment_width", 20)
config.set("environment_height", 20)

# Create brain with optimized configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Configuration Caching

```python
import pynlm

class ConfigurationCache:
    def __init__(self):
        self.cache = {}
        
    def get_config(self, filename):
        """Get configuration from cache or load from file"""
        if filename in self.cache:
            return self.cache[filename]
            
        # Load from file
        config = self.importer.import_configuration(filename, "json")
        self.cache[filename] = config
        return config
```

## Configuration Reference

### Available Configuration Keys

| Category | Keys | Description |
|----------|------|-------------|
| Basic | `random_seed`, `simulation_timestep`, `max_simulation_steps`, `simulation_time_limit` | Basic simulation settings |
| Neural | `neuron_count`, `region_count`, `connection_probability` | Neural network structure |
| Plasticity | `plasticity_learning_rate`, `stdp_ltp_weight`, `stdp_ltd_weight`, `stdp_time_constant` | Plasticity parameters |
| Development | `development_synaptogenesis_rate`, `development_pruning_rate` | Developmental processes |
| Neuromodulation | `dopamine_baseline`, `reward_discount_factor` | Neuromodulation parameters |
| Environment | `environment_name`, `environment_width`, `environment_height` | Environment settings |
| World | `vision_width`, `vision_height`, `world_width`, `world_height` | World-specific settings |
| Logging | `log_level`, `log_to_file`, `log_filename` | Logging configuration |
| Visualization | `visualization_enabled`, `visualization_update_rate` | Visualization settings |

### Configuration Key Descriptions

#### random_seed
Random seed for reproducible experiments. Default: 42.

#### simulation_timestep
Time step duration in seconds. Default: 0.001.

#### max_simulation_steps
Maximum number of simulation steps. Default: 10000.

#### simulation_time_limit
Simulation time limit in seconds (0 = unlimited). Default: 0.0.

#### neuron_count
Number of neurons in the brain. Default: 1000.

#### region_count
Number of brain regions. Default: 2.

#### connection_probability
Connection probability for random connectivity. Default: 0.1.

#### plasticity_learning_rate
Base learning rate for plasticity. Default: 0.01.

#### stdp_ltp_weight
STDP long-term potentiation weight. Default: 0.01.

#### stdp_ltd_weight
STDP long-term depression weight. Default: 0.012.

#### stdp_time_constant
STDP time constant. Default: 20.0.

#### development_synaptogenesis_rate
Rate of synapse formation during development. Default: 0.001.

#### development_pruning_rate
Rate of synapse pruning during development. Default: 0.0001.

#### dopamine_baseline
Baseline dopamine level. Default: 0.0.

#### reward_discount_factor
Discount factor for reward calculation. Default: 0.99.

#### environment_name
Name of the simulation environment. Default: GridWorld.

#### environment_width
Width of the environment. Default: 10.

#### environment_height
Height of the environment. Default: 10.

#### vision_width
Vision width in world coordinates. Default: 8.

#### vision_height
Vision height in world coordinates. Default: 8.

#### world_width
World width in cells. Default: 10.

#### world_height
World height in cells. Default: 10.

#### log_level
Logging level (DEBUG, INFO, WARN, ERROR). Default: INFO.

#### log_to_file
Whether to log to file. Default: false.

#### log_filename
Log file name. Default: nlm.log.

#### visualization_enabled
Whether visualization is enabled. Default: false.

#### visualization_update_rate
Visualization update rate in Hz. Default: 30.0.

## Configuration Validation Rules

### Required Keys

The following keys are required for basic functionality:

- `neuron_count`: Number of neurons (integer, 1-100000)
- `region_count`: Number of regions (integer, 1-10)

### Value Ranges

| Key | Min | Max | Type |
|-----|-----|-----|------|
| `neuron_count` | 1 | 100000 | int |
| `region_count` | 1 | 10 | int |
| `plasticity_learning_rate` | 0.001 | 0.1 | float |
| `environment_width` | 5 | 100 | int |
| `environment_height` | 5 | 100 | int |

### Data Types

All configuration values must be one of the following types:

- `int`: Integer values
- `float`: Floating-point values
- `str`: String values
- `bool`: Boolean values

## Configuration Best Practices Summary

1. **Always validate configurations** before use.
2. **Use templates** for consistent configurations.
3. **Monitor configuration changes** for debugging.
4. **Export configurations** for backup and sharing.
5. **Optimize configurations** for performance.
6. **Use incremental configuration** for flexibility.
7. **Follow naming conventions** for consistency.
8. **Document configurations** for understanding.

---

*Configuration Guide provides comprehensive information about NLM configuration options, best practices, and troubleshooting.*