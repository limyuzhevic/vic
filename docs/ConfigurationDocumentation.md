# NLM Configuration Documentation

## Overview

This document provides comprehensive information about the Neural Learning Machine (NLM) configuration system, including configuration file formats, parameter descriptions, validation rules, and advanced configuration options.

## Configuration File Formats

### INI Format (Default)

The NLM configuration system supports multiple file formats:

#### Basic INI Format (`configs/default.cfg`)
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
```

#### JSON Format (`configs/default.json`)
```json
{
    "nlm": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "plasticity": {
            "learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012
        },
        "development": {
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0001
        },
        "neuromodulation": {
            "dopamine_baseline": 0.1
        },
        "reward": {
            "discount_factor": 0.99
        },
        "environment": {
            "name": "GridWorld",
            "width": 10,
            "height": 10
        },
        "logging": {
            "level": "INFO",
            "to_file": false,
            "filename": "nlm.log"
        },
        "simulation": {
            "max_steps": 10000,
            "time_limit": 0.0
        },
        "visualization": {
            "enabled": false,
            "update_rate": 30.0
        }
    }
}
```

#### YAML Format (`configs/default.yaml`)
```yaml
nlm:
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  
  plasticity:
    learning_rate: 0.01
    stdp_ltp_weight: 0.01
    stdp_ltd_weight: 0.012
  
  development:
    synaptogenesis_rate: 0.001
    pruning_rate: 0.0001
    
  neuromodulation:
    dopamine_baseline: 0.1
    
  reward:
    discount_factor: 0.99
    
  environment:
    name: GridWorld
    width: 10
    height: 10
    
  logging:
    level: INFO
    to_file: false
    filename: nlm.log
    
  simulation:
    max_steps: 10000
    time_limit: 0.0
    
  visualization:
    enabled: false
    update_rate: 30.0
```

## Configuration Parameters

### Basic Simulation Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments | Positive integer |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds | > 0, < 1.0 |
| `neuron_count` | int | 1000 | Number of neurons in the brain | 100 ≤ value ≤ 100,000 |
| `region_count` | int | 2 | Number of brain regions | 1 ≤ value ≤ 10 |
| `connection_probability` | double | 0.1 | Connection probability for random connectivity | 0.0 ≤ value ≤ 1.0 |

### Plasticity Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `plasticity_learning_rate` | double | 0.01 | Learning rate for synaptic plasticity | 0.0 ≤ value ≤ 1.0 |
| `stdp_ltp_weight` | double | 0.01 | Spike-Timing-Dependent Plasticity Long-Term Potentiation weight | 0.0 ≤ value ≤ 1.0 |
| `stdp_ltd_weight` | double | 0.012 | Spike-Timing-Dependent Plasticity Long-Term Depression weight | 0.0 ≤ value ≤ 1.0 |
| `stdp_time_constant` | double | 20.0 | STDP time constant in milliseconds | > 0 |

### Development Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `development_synaptogenesis_rate` | double | 0.001 | Rate of new synapse formation during development | 0.0 ≤ value ≤ 0.1 |
| `development_pruning_rate` | double | 0.0001 | Rate of synapse elimination during development | 0.0 ≤ value ≤ 0.01 |

### Neuromodulation Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `dopamine_baseline` | double | 0.1 | Baseline dopamine level | 0.0 ≤ value ≤ 1.0 |

### Reward Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `reward_discount_factor` | double | 0.99 | Discount factor for future rewards | 0.0 ≤ value ≤ 1.0 |

### Environment Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `environment_name` | string | "GridWorld" | Name of the simulation environment | Non-empty |
| `environment_width` | int | 10 | Width of the environment in cells | > 0 |
| `environment_height` | int | 10 | Height of the environment in cells | > 0 |

### Logging Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARN, ERROR) | Valid log level |
| `log_to_file` | bool | false | Whether to write logs to file | Boolean |
| `log_filename` | string | "nlm.log" | Name of the log file | Valid filename |

### Simulation Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `max_simulation_steps` | int | 10000 | Maximum number of simulation steps | > 0 |
| `simulation_time_limit` | double | 0.0 | Time limit for simulation in seconds | ≥ 0 |

### Visualization Parameters

| Parameter | Type | Default | Description | Validation |
|-----------|------|---------|-------------|------------|
| `visualization_enabled` | bool | false | Whether visualization is enabled | Boolean |
| `visualization_update_rate` | double | 30.0 | Visualization update rate in Hz | > 0 |

## Configuration Profiles

### Available Profiles

The NLM system supports configuration profiles for different use cases:

1. **Phase 1: Basic Skeleton** (`configs/profiles/phase1.cfg`)
2. **Phase 2: Neural Computation** (`configs/profiles/phase2.cfg`)
3. **Phase 3: Basic Agent** (`configs/profiles/phase3.cfg`)
4. **Phase 4: Advanced Agent** (`configs/profiles/phase4.cfg`)
5. **Phase 5: Cognitive Agent** (`configs/profiles/phase5.cfg`)
6. **Phase 6: Integrated System** (`configs/profiles/phase6.cfg`)

### Profile Creation

To create a new configuration profile:

```bash
# Copy an existing profile as a template
cp configs/profiles/phase1.cfg configs/profiles/my_profile.cfg

# Edit the profile using your preferred text editor
# Or use the configuration management tools:
mkdir -p configs/profiles
python3 -c "
from src.core.Config.ConfigurationManager import ConfigurationManager
config = ConfigurationManager()
config.loadProfile('my_profile')
config.set('neuron_count', 2000)
config.saveProfile('my_profile')
"
```

## Configuration Loading Priority

When loading configuration, the NLM system follows this priority order (higher priority first):

1. **Command Line Arguments** (highest priority)
   - `--key=value` format
   - `-key value` format

2. **Environment Variables**
   - `NLM_RANDOM_SEED`
   - `NLM_NEURON_COUNT`
   - `NLM_CONNECTION_PROBABILITY`
   - `NLM_ENABLE_DEVELOPMENT`
   - `NLM_LOG_LEVEL`

3. **Configuration File** (specified or default)
   - `configs/default.cfg`
   - `configs/my_config.cfg`

4. **Default Values** (lowest priority)

## Command Line Usage

### Basic Usage

```bash
# Run with custom configuration
./nlm_demo --neuron_count=2000 --log_level=DEBUG

# Run with configuration file
./nlm_demo --config_file=configs/custom.cfg

# Run with environment variables
export NLM_RANDOM_SEED=12345
export NLM_NEURON_COUNT=5000
./nlm_demo
```

### Advanced Options

```bash
# Multiple configuration options
./nlm_demo \
    --neuron_count=3000 \
    --connection_probability=0.15 \
    --environment_width=20 \
    --environment_height=15 \
    --visualization_enabled=true \
    --log_level=INFO

# Override specific settings
./nlm_demo --log_level=WARN --max_simulation_steps=5000
```

## Schema Validation

### Configuration Validation Rules

The NLM configuration system validates all configuration values against a predefined schema. Validation includes:

1. **Type Validation**: Ensures values match expected types (int, double, string, bool)
2. **Range Validation**: Checks if values fall within acceptable ranges
3. **Format Validation**: Validates string formats (e.g., log levels)
4. **Dependency Validation**: Ensures required parameter combinations are valid
5. **Uniqueness Validation**: Detects duplicate keys

### Validation Error Messages

When configuration validation fails, error messages include:

```
Configuration validation errors:
- neuron_count: Out of range (100,000 < value ≤ 100,000)
- connection_probability: Invalid type (expected double, got string)
- log_level: Invalid value "VERBOSE" (allowed: DEBUG, INFO, WARN, ERROR)
- environment_name: Missing required field
```

## Advanced Configuration Features

### Dynamic Configuration Updates

The NLM system supports dynamic configuration updates during runtime:

```cpp
// Set up dynamic configuration updates
ConfigurationManager::getInstance()
    .enableDynamicUpdates(true)
    .setUpdateCallback([](const std::string& key, const ConfigValue& value) {
        NLM_LOG_INFO("Configuration updated: " + key + " = " + 
                    ConfigUtils::toString(value), "ConfigDynamicUpdate");
        // Apply configuration change immediately
        applyConfigurationChange(key, value);
    });
```

### Configuration Templates

Create reusable configuration templates:

```bash
# Create a template directory
templates/
├── basic.cfg
├── high_performance.cfg
├── research.cfg
└── production.cfg

# Use a template
./nlm_demo --template=high_performance.cfg --neuron_count=5000
```

## Troubleshooting

### Common Configuration Issues

1. **Invalid File Format**: Ensure the configuration file is in the correct format (INI, JSON, or YAML)
2. **Missing Dependencies**: All required parameters must be present
3. **Invalid Values**: Check that values are within acceptable ranges
4. **Permission Issues**: Ensure the application has read/write permissions to configuration files

### Error Resolution

| Problem | Solution |
|---------|----------|
| "Configuration validation failed" | Check configuration file format and values |
| "Failed to load configuration" | Verify file exists and has correct permissions |
| "Configuration file not found" | Specify correct file path or use default |
| "Invalid parameter type" | Ensure parameter value matches expected type |

## API Documentation

### ConfigurationManager API

#### Key Methods

- `loadConfiguration(filepath)`: Load configuration from file
- `loadFromEnvironment()`: Load from environment variables
- `loadFromCommandLine(argc, argv)`: Load from command line arguments
- `get<T>(key, defaultValue)`: Get value with type safety
- `has(key)`: Check if key exists
- `validate()`: Validate configuration against schema
- `saveToFile(filepath)`: Save configuration to file
- `saveToJSON(filepath)`: Save configuration as JSON
- `saveToYAML(filepath)`: Save configuration as YAML

#### Configuration Profiles

- `loadProfile(name)`: Load a named profile
- `saveProfile(name)`: Save current configuration as profile
- `getAvailableProfiles()`: Get list of available profiles

#### Dynamic Updates

- `enableDynamicUpdates(bool)`: Enable/disable dynamic updates
- `setUpdateCallback(callback)`: Set callback for configuration changes

## Migration Guide

### From Phase 1 to Phase 2+

Configuration changes needed when upgrading:

1. **New Configuration Format**: Phase 2+ uses JSON/YAML support
2. **Enhanced Validation**: All configuration values are now validated
3. **Dynamic Configuration**: Configuration can be updated at runtime
4. **Environment Variables**: Additional environment variable support

### Example Migration

```bash
# Phase 1 configuration (old format)
# configs/default.cfg (INI format)

# Phase 2+ configuration (new format)
# configs/default.json (JSON format with validation)
```

The system automatically detects and converts configuration formats, maintaining backward compatibility.

## License and Usage

This configuration system is part of the Neural Learning Machine (NLM) project. Configuration files follow the project's coding standards and best practices. All configuration changes should be made through the ConfigurationManager API or configuration files, not by directly editing internal data structures.

## Contact

For configuration-related questions or issues, please refer to the NLM documentation or contact the development team.
