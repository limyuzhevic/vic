# NLM Configuration Schema Documentation

## Overview

This document describes the configuration system for NLM (Neural Learning Machine), an experimental artificial brain simulation framework. The configuration system uses JSON format to specify all parameters for neural network simulation, memory systems, plasticity rules, neuromodulation, and cognitive functions.

## Configuration File Format

Configuration files should be saved in JSON format. Each file represents a single configuration object with key-value pairs where keys are configuration parameter names and values are their corresponding settings.

### Example Configuration File

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
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
      "learning_rate": 0.001
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001f,
      "pruning_rate": 0.00001f
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "decay_rate": 0.99
    },
    "novelty": {
      "enable": true,
      "threshold": 0.1
    }
  },
  "development": {
    "enable": true,
    "stages": {
      "initial": 0.0,
      "critical_period": 60.0,
      "maturation": 300.0
    }
  },
  "memory": {
    "working": {
      "capacity": 100,
      "decay_rate": 0.95
    },
    "episodic": {
      "max_episodes": 1000,
      "replay_interval": 100,
      "consolidation_interval": 1000
    }
  },
  "prediction": {
    "enable": true,
    "horizon": 10,
    "error_decay": 0.95
  },
  "cognition": {
    "planning": {
      "depth": 5,
      "confidence_threshold": 0.5
    },
    "concept": {
      "min_instances": 3,
      "similarity_threshold": 0.8
    }
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000,
    "random_seed": 42
  }
}
```

## Configuration Categories

### Brain Parameters

| Key | Type | Default | Description | Validation |
|-----|------ hierzu |---------|-------------|-------------|
| `brain.neuron_count` | int64 | 1000 | Total number of neurons | ≥1, ≤1,000,000 |
| `brain.region_count` | int64 | 1 | Number of brain regions | ≥1 |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons | 0.0-1.0 |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight | Any |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights | Any |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) | Any |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) | Any |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) | Any |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) | Any |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) | Any |

### Plasticity Parameters

| Key | Type | Default | Description | Validation |
|-----|------|--------|-------------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP plasticity | Any |
| `plasticity.stdp.learning_rate` | float |  ultrasonic | STDP learning rate | 0.0-1.0 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant | ≥0 |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant | ≥0 |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | Any |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate | 0.0-1.0 |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | Any |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Synaptogenesis rate | 0.0-1.0 |
| `plasticity.structural.pruning_rate` семь | float | 0.00001 | Pruning rate | 0.0-1.0 |

### Neuromodulation Parameters

| Key | Type | Default | Description | Validation |
|-----|------|--------|-------------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | 0.0-10.0 |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | Any |
| `neuromod.curiosity.decay_rate` | float | 0.99 | Curiosity decay rate | 0.0-1.0 |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | Any |
| `neuromod.novelty.threshold` | float | 0.1 | Novelty detection threshold | 0.0-1.0 |

### Development Parameters

| Key | Type | Default | Description | Validation |
|-----|------|--------|-------------|-------------|
| `development.enable` | bool | true | Enable development system | Any |
| `development.initial_duration` | float | 60.0 | Initial period duration (s) | ≥0 |
| `development.critical_period_duration` | float | 300.0 | Critical period duration (s) | ≥0 |
| `development.maturation_duration` | float | 900.0 | Maturation period duration (s) | ≥0 |

### Memory Parameters

| Key | Type | Default | Description | Validation |
|-----|------|--------|-------------|-------------|
| `memory.working.capacity` | int64 | 100 | Working memory capacity | ≥1 |
| `memory.working.decay_rate` | float | 0.95 | Working memory decay rate | 0.0-1.0 |
| `memory.episodic.max_episodes` | int64 | 1000 | Maximum episodic memory episodes | ≥1 |
| `memory.episodic.replay_interval` | int64 | 100 | Replay interval (steps) | ≥1 |
| `memory.episodic.consolidation_interval` | int64 | 1000 | Consolidation interval (steps) | ≥1 |

### Prediction Parameters

| Key | Type | Default | Description | Validation |
|-----|------|--------|-------------|-------------|
| `prediction.enable` | bool | true | Enable prediction system | Any |
| `prediction.horizon` | int64 | 10 | Prediction horizon (steps) | ≥1 |
| `prediction.error_decay` | float | 0.95 | Prediction error decay rate | 0.0-1.0 |

### Cognition Parameters

| Key | Type | Default | Description | Validation |
|-----|------|--------|-------------|-------------|
| `cognition.planning.depth` | int64 | 5 | Planning depth | ≥1 |
| `cognition.planning.confidence_threshold` | float | 0.5 | Planner confidence threshold | 0.0-1.0 |
| `cognition.concept.min_instances` | int64 | 3 | Minimum instances for concept formation | ≥1 |
| `cognition.concept.similarity_threshold` | float | 0.8 | Concept similarity threshold | 0.0-1.0 |

### Simulation Parameters

| Key | Type | Default | Description | Validation |
|-----|------|--------|-------------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep (s) | ≥0.0001 |
| `simulation.max_steps` | int64 | 10000 | Maximum simulation steps | ≥1 |
| `simulation.random_seed` | int64 | 42 | Random seed for reproducibility | ≥0 |

## Configuration Validation

### Validation Rules

The NLM configuration system validates configuration values to ensure they are within acceptable ranges. Invalid configurations are rejected with error messages.

#### Data Type Validation

Configuration values must match their expected data types:

- **Numeric values**: Integers and floats are automatically converted
- **Strings**: Must be quoted in JSON
- **Arrays**: Must contain homogeneous elements (all integers, all floats, or all strings)
- **Booleans**: Use JSON `true`/`false`

#### Range Validation

| Configuration Key | Valid Range |
|------------------|-------------|
| `brain.neuron_count` | [1, 1,000,000] |
| `brain.connection_probability` | [0.0, 1.0] |
| `plasticity.stdp.learning_rate` | [0.0, 1.0] |
| `plasticity.stdp.tau_plus` | [0.0, ∞) |
| `plasticity.stdp.tau_minus` | [0.0, ∞) |
| `neuromod.dopamine.scale` | [0.0, 10.0] |
| `neuromod.curiosity.decay_rate` | [0.0, 1.0] |
| `neuromod.novelty.threshold` | [0.0, 1.0] |

### Configuration Validation API

The configuration system provides validation capabilities through the Python API:

#### Python Example

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Load from file
config.loadFromFile("config.json")

# Validate configuration
errors = config.validate()
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("Configuration is valid")

# Validate specific key
config.set("brain.neuron_count", 2000)
if config.validate("brain.neuron_count", 2000):
    print("Neuron count is valid")
else:
    print("Neuron count is invalid")
```

## Configuration File Format Examples

### Simple Configuration

```json
{
  "brain": {
    "neuron_count": 1000,
    "connection_probability": 0.1
  },
  "simulation": {
    "timestep": 0.001,
    "random_seed": 123
  }
}
```

### Advanced Configuration

```json
{
  "brain": {
    "neuron_count": 5000,
    "region_count": 2,
    "connection_probability": 0.15
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.002,
      "tau_plus": 25.0,
      "tau_minus": 25.0
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.00005,
      "pruning_rate": 0.000005
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 2.0
    },
    "curiosity": {
      "enable": true,
      "decay_rate": 0.98
    }
  },
  "development": {
    "enable": true,
    "stages": {
      "initial": 60.0,
      "critical_period": 300.0,
      "maturation": 900.0
    }
  },
  "simulation": {
    "timestep": 0.0005,
    "max_steps": 50000,
    "random_seed": 42
  }
}
```

## Best Practices

### Configuration Management

1. **Use relative paths**: Store configuration files relative to the working directory
2. **Validation before use**: Always validate configuration before running simulations
3. **Version control**: Keep configuration files in version control for reproducibility
4. **Environment variables**: Use environment variables for sensitive or environment-specific settings

### Configuration Organization

1. **Group related parameters**: Organize configuration by functional areas
2. **Use descriptive keys**: Use descriptive key names for better readability
3. **Document defaults**: Document default values for all parameters
4. **Validation rules**: Define and document validation rules

### Configuration Testing

1. **Configuration validation**: Test configuration validation before simulation
2. **Configuration comparison**: Compare different configurations to identify differences
3. **Configuration export/import**: Test configuration export and import functionality
4./mlbtv Configuration files for reproducibility

## Configuration Comparison

The NLM configuration system provides configuration comparison functionality to identify differences between two configuration files:

### Python Example

```python
import pynlm

# Load two configurations
config1 = pynlm.createDefaultConfig()
config2 = pynlm.createDefaultConfig()

config1.loadFromFile("config1.json")
config2.loadFromFile("config2.json")

# Compare configurations
differences = config1.compare(config2)
if differences:
    print(f"Differences found: {differences}")
else:
    print("Configurations are identical")
```

## Configuration Export and Import

The configuration system supports exporting and importing configurations in JSON format, making it easy to:

1. **Save working configurations**: Save simulation configurations for later use
2. **Share configurations**: Export configurations for sharing with others
3. **Configuration recovery**: Import configurations after system updates
4. **Batch processing**: Process multiple configurations in batch

### Python Examples

```python
import pynlm
import json

# Create and save configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("example.json")
config.saveToFile("saved_config.json")

# Export as JSON string
config_json = json.loads(config.export())
print(json.dumps(config_json, indent=2))

# Import from JSON string
config_data = json.loads('{"brain": {"neuron_count": 1000}}')
config.import(config_data)
```

## Troubleshooting

### Common Configuration Issues

1. **File not found**: Ensure configuration file paths are correct
2. **Invalid JSON**: Validate JSON format using a JSON validator
3. **Missing required keys**: Check for all required configuration parameters
4. **Type mismatches**: Ensure configuration values match expected data types
5. **Range violations**: Check configuration values against validation rules

### Error Messages

Common error messages and their meanings:

- **"Failed to load config from file: filename.json"**: File not found or invalid JSON
- **"Failed to save config to file: filename.json"**: File not writable or disk full
- **"Error loading config: error message"**: Configuration parsing error
- **"Invalid configuration value for key: parameter_name"**: Configuration validation failure
- **"Error exporting config: error message"**: Configuration export error
- **"Error importing config: error message"**: Configuration import error

## Advanced Usage

### Configuration Validation Scripts

```python
import pynlm

# Configuration validation script
def validate_config_file(filepath):
    config = pynlm.createDefaultConfig()
    if not config.loadFromFile(filepath):
        print(f"Failed to load: {filepath}")
        return False
    
    errors = config.validate()
    if errors:
        print(f"Validation errors in {filepath}: {errors}")
        return False
    
    print(f"Config {filepath} is valid")
    return True

# Batch validation
config_files = ["config1.json", "config2.json", "config3.json"]
valid_configs = [f for f in config_files if validate_config_file(f)]
print(f"Valid configurations: {len(valid_configs)}/{len(config_files)}")
```

### Configuration Comparison Scripts

```python
import pynlm
import json

# Configuration comparison script
def compare_configs(filepath1, filepath2):
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.createDefaultConfig()
    
    config1.loadFromFile(filepath1)
    config2.loadFromFile(filepath2)
    
    differences = config1.compare(config2)
    
    if differences:
        print(f"Differences between {filepath1} and {filepath2}:")
        print(differences)
        return False
    else:
        print(f"{filepath1} and {filepath2} are identical")
        return True

# Compare multiple configurations
configs = [("baseline", "baseline.json"), ("experiment", "experiment.json")]
for name1, filepath1 in configs:
    for name2, filepath2 in configs:
        if name1 < name2:  # Avoid duplicate comparisons
            compare_configs(filepath1, filepath2)
```

## Conclusion

The NLM configuration system provides a robust, flexible, and user-friendly way to specify and manage neural network simulation parameters. With JSON format, validation, comparison, export/import capabilities, and comprehensive error handling, the configuration system supports everything from simple configurations to complex, multi-parameter setups.

Key features of the NLM configuration system:

1. **JSON format**: Standard, human-readable configuration format
2. **Validation**: Comprehensive validation of configuration values
3. **Error handling**: Detailed error messages for configuration issues
4. **Validation API**: Python API for configuration validation
5. **Comparison**: Configuration comparison for identifying differences
6. **Export/import**: JSON-based configuration export and import
7. **Documentation**: Comprehensive documentation and examples
8. **Best practices**: Clear guidelines for configuration management

The configuration system is designed to be intuitive for users while providing powerful capabilities for advanced users. It supports everything from simple configurations to complex, multi-parameter setups, making it suitable for a wide range of use cases from prototyping to production deployments.

## References

- JSON Specification: https://www.json.org/
- Modern C++ Best Practices: https://isocpp.github.io/CppCoreGuidelines/
- Python Pybind11 Documentation: https://pybind11.readthedocs.io/
- NLM Documentation: https://nlm.readthedocs.io