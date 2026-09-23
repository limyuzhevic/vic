# NLM Configuration Guide

## Overview

The NLM (Neural Learning Machine) configuration system provides a comprehensive framework for controlling brain simulation parameters. It supports multiple configuration sources, validation, and management of simulation parameters.

### Key Features

- **Multiple Configuration Sources**: Defaults, files, command line, and programmatic
- **Hierarchical Structure**: Support for nested configuration keys
- **Type Safety**: Strongly-typed configuration values
- **Validation**: Configuration validation and error handling
- **Environment-Specific**: Different configurations for different use cases
- **Persistence**: Save and load configuration to/from files

## Configuration Structure

### Configuration Sources

NLM supports configuration from multiple sources with a clear priority order:

1. **Programmatic**: Set at runtime via code
2. **File**: Loaded from JSON configuration files
3. **Command Line**: Override via command line arguments
4. **Default**: Built-in default values

### Configuration File Format

NLM uses JSON format for configuration files:

```json
{
    "nlm": {
        "simulation": {
            "timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
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
                "learning_rate": 0.01
            },
            "structural": {
                "enable": true,
                "synaptogenesis_rate": 0.001,
                "pruning_rate": 0.0001
            }
        },
        "neuromodulation": {
            "dopamine": {
                "baseline": 0.1,
                "scale": 1.0
            },
            "curiosity": {
                "enable": true,
                "baseline": 0.5
            },
            "novelty": {
                "enable": true,
                "threshold": 0.7
            }
        },
        "environment": {
            "name": "SimpleWorld",
            "width": 20,
            "height": 20,
            "vision_width": 8,
            "vision_height": 8
        },
        "logging": {
            "level": "INFO",
            "to_file": false,
            "filename": "nlm.log"
        },
        "development": {
            "enable": true,
            "stages": [
                "Initial",
                "CriticalPeriod",
                "Maturation",
                "Adult",
                "Aging"
            ],
            "plasticity_modulation": {
                "synaptogenesis": {
                    "rate": 0.001,
                    "modulation": {
                        "by_age": true,
                        "by_stage": true
                    }
                },
                "pruning": {
                    "rate": 0.0001,
                    "modulation": {
                        "by_age": true,
                        "by_stability": true
                    }
                }
            }
        }
    }
}
```

## Configuration Categories

### Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.simulation.timestep` | float | 0.001 | Simulation timestep in seconds |
| `nlm.simulation.neuron_count` | int | 1000 | Total number of neurons |
| `nlm.simulation.region_count` | int | 2 | Number of neural regions |
| `nlm.simulation.connection_probability` | float | 0.1 | Probability of connection between neurons |
| `nlm.simulation.max_simulation_steps` | int | 10000 | Maximum simulation steps |
| `nlm.simulation.simulation_time_limit` | float | 0.0 | Time limit in seconds (0 = unlimited) |

### Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `nlm.brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `nlm.brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `nlm.brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `nlm.brain.tau_ref` | float | 2.0 | Refractory period (ms) |
| `nlm.brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `nlm.brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `nlm.plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `nlm.plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for potentiation |
| `nlm.plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for depression |
| `nlm.plasticity.stdp.max_weight` | float | 1.0 | Maximum synaptic weight |
| `nlm.plasticity.stdp.min_weight` | float | 0.0 | Minimum synaptic weight |
| `nlm.plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `nlm.plasticity.hebbian.learning_rate` | float | 0.01 | Hebbian learning rate |
| `nlm.plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `nlm.plasticity.structural.synaptogenesis_rate` | float | 0.001 | Synapse formation rate |
| `nlm.plasticity.structural.pruning_rate` | float | 0.0001 | Synapse removal rate |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.neuromodulation.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `nlm.neuromodulation.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `nlm.neuromodulation.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `nlm.neuromodulation.curiosity.baseline` | float | 0.5 | Baseline curiosity level |
| `nlm.neuromodulation.novelty.enable` | bool | true | Enable novelty detection |
| `nlm.neuromodulation.novelty.threshold` | float | 0.7 | Novelty detection threshold |
| `nlm.neuromodulation.prediction_error.weight` | float | 1.0 | Prediction error modulation weight |

### Environment Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.environment.name` | string | "SimpleWorld" | Environment type |
| `nlm.environment.width` | int | 20 | World width |
| `nlm.environment.height` | int | 20 | World height |
| `nlm.environment.vision_width` | int | 8 | Vision sensor width |
| `nlm.environment.vision_height` | int | 8 | Vision sensor height |
| `nlm.environment.max_energy` | float | 100.0 | Maximum agent energy |
| `nlm.environment.energy_decay_rate` | float | 0.01 | Energy decay per timestep |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.development.enable` | bool | true | Enable developmental system |
| `nlm.development.initial_stage` | string | "Initial" | Initial developmental stage |
| `nlm.development.stage_progression` | array | ["CriticalPeriod", "Maturation", "Adult", "Aging"] | Development stages |
| `nlm.development.plasticity_by_age` | bool | true | Modulate plasticity by age |
| `nlm.development.plasticity_by_stage` | bool | true | Modulate plasticity by developmental stage |

### Logging Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nlm.logging.level` | string | "INFO" | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `nlm.logging.to_file` | bool | false | Enable file logging |
| `nlm.logging.filename` | string | "nlm.log" | Log file name |
| `nlm.logging.max_file_size` | int | 10485760 | Maximum log file size (bytes) |
| `nlm.logging.max_files` | int | 5 | Maximum number of log files |

## Configuration Management

### Creating Configuration

#### Programmatic Configuration

```cpp
// C++ example
auto config = std::make_shared<Config>();
config->set("nlm.simulation.neuron_count", 5000);
config->set("nlm.plasticity.stdp.enable", true);
config->set("nlm.plasticity.stdp.learning_rate", 0.0001);
config->set("nlm.environment.width", 50);
config->set("nlm.environment.height", 50);
config->set("nlm.logging.level", "DEBUG");
config->set("nlm.logging.to_file", true);
config->set("nlm.logging.filename", "my_simulation.log");
```

```python
# Python example
import pynlm

config = pynlm.createDefaultConfig()
config.set("nlm.simulation.neuron_count", 5000)
config.set("nlm.plasticity.stdp.enable", True)
config.set("nlm.plasticity.stdp.learning_rate", 0.0001)
config.set("nlm.environment.width", 50)
config.set("nlm.environment.height", 50)
config.set("nlm.logging.level", "DEBUG")
config.set("nlm.logging.to_file", True)
config.set("nlm.logging.filename", "my_simulation.log")
```

#### File-based Configuration

```cpp
// Load configuration from JSON file
auto config = std::make_shared<Config>();
if (config->loadFromFile("my_config.json")) {
    NLM_LOG_INFO("Configuration loaded from: my_config.json");
} else {
    NLM_LOG_WARNING("Failed to load configuration from file");
    // Use default configuration
}
```

```python
# Load configuration from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")  # If file doesn't exist, uses defaults
```

### Accessing Configuration Values

#### Safe Access with Defaults

```cpp
// C++ - Safe access with defaults
int neuron_count = config->getOr("nlm.simulation.neuron_count", 1000);
double timestep = config->getOr("nlm.simulation.timestep", 0.001);
float learning_rate = config->getOr("nlm.plasticity.stdp.learning_rate", 0.001);
bool enable_plasticity = config->getOr("nlm.plasticity.enable", true);
std::string log_level = config->getOr("nlm.logging.level", "INFO");
```

```python
# Python - Safe access with defaults
neuron_count = config.getOr("nlm.simulation.neuron_count", 1000)
timestep = config.getOr("nlm.simulation.timestep", 0.001)
learning_rate = config.getOr("nlm.plasticity.stdp.learning_rate", 0.001)
enable_plasticity = config.getOr("nlm.plasticity.enable", True)
log_level = config.getOr("nlm.logging.level", "INFO")
```

#### Checking Configuration

```cpp
// Check if configuration key exists
bool has_neurons = config->has("nlm.simulation.neuron_count");
bool has_plasticity = config->has("nlm.plasticity.stdp.enable");

// Get all configuration keys
auto keys = config->getKeys();
for (const auto& key : keys) {
    std::cout << "Key: " << key << std::endl;
}
```

```python
# Check if configuration key exists
has_neurons = config.has("nlm.simulation.neuron_count")
has_plasticity = config.has("nlm.plasticity.stdp.enable")

# Get all configuration keys
keys = config.getKeys()
for key in keys:
    print(f"Key: {key}")
```

### Saving Configuration

```cpp
// Save configuration to file
config->saveToFile("my_config.json");
NLM_LOG_INFO("Configuration saved to: my_config.json");
```

```python
# Save configuration to file
config.saveToFile("my_config.json")
```

## Command Line Configuration

### Command Line Arguments

NLM supports command line arguments to override configuration:

```bash
./nlm [OPTIONS]

Options:
  --config=file           Load configuration from file
  --nlm.simulation.neuron_count=1000  Override neuron count
  --nlm.plasticity.stdp.enable=true   Enable STDP
  --nlm.logging.level=DEBUG           Set logging level
  --test=basic_connectivity,plasticity  Run specific tests
  --list-tests                      List available tests
  --help                         Show help message
```

### Example Command Lines

```bash
# Run with custom configuration
./nlm --config=my_config.json --test=all

# Run with specific test selection
./nlm --test=basic_connectivity,plasticity,stdp_verification

# Override configuration parameters
./nlm --nlm.simulation.neuron_count=2000 --nlm.plasticity.stdp.learning_rate=0.0005

# List available tests
./nlm --list-tests

# Show help
./nlm --help
```

### Configuration Priority

The order of configuration sources is (highest priority to lowest):

1. **Command Line Arguments**: Explicitly override all other sources
2. **File Configuration**: Load from JSON file
3. **Programmatic Configuration**: Set at runtime
4. **Default Configuration**: Built-in defaults

If the same parameter is set in multiple sources, the command line arguments take precedence.

## Configuration Examples

### Example 1: Research Configuration

```json
{
    "nlm": {
        "simulation": {
            "timestep": 0.0001,
            "neuron_count": 10000,
            "region_count": 10,
            "connection_probability": 0.05
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.0001,
                "tau_plus": 30.0,
                "tau_minus": 30.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.001
            },
            "structural": {
                "enable": true,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        },
        "neuromodulation": {
            "dopamine": {
                "baseline": 0.05,
                "scale": 2.0
            },
            "curiosity": {
                "enable": true,
                "baseline": 0.3
            },
            "novelty": {
                "enable": true,
                "threshold": 0.6
            }
        },
        "environment": {
            "name": "GridWorld",
            "width": 50,
            "height": 50,
            "vision_width": 16,
            "vision_height": 16
        },
        "logging": {
            "level": "DEBUG",
            "to_file": true,
            "filename": "research_simulation.log",
            "max_file_size": 52428800,
            "max_files": 10
        },
        "development": {
            "enable": true,
            "plasticity_by_age": true,
            "plasticity_by_stage": true
        }
    }
}
```

### Example 2: Fast Testing Configuration

```json
{
    "nlm": {
        "simulation": {
            "timestep": 0.01,
            "neuron_count": 100,
            "region_count": 2,
            "connection_probability": 0.2
        },
        "plasticity": {
            "stdp": {
                "enable": false,
                "learning_rate": 0.0
            },
            "hebbian": {
                "enable": false,
                "learning_rate": 0.0
            },
            "structural": {
                "enable": false,
                "synaptogenesis_rate": 0.0,
                "pruning_rate": 0.0
            }
        },
        "neuromodulation": {
            "dopamine": {
                "baseline": 0.0,
                "scale": 0.0
            },
            "curiosity": {
                "enable": false,
                "baseline": 0.0
            },
            "novelty": {
                "enable": false,
                "threshold": 1.0
            }
        },
        "environment": {
            "name": "SimpleWorld",
            "width": 10,
            "height": 10,
            "vision_width": 4,
            "vision_height": 4
        },
        "logging": {
            "level": "WARNING",
            "to_file": false
        },
        "development": {
            "enable": false
        }
    }
}
```

### Example 3: Interactive Development Configuration

```json
{
    "nlm": {
        "simulation": {
            "timestep": 0.001,
            "neuron_count": 500,
            "region_count": 3,
            "connection_probability": 0.1
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.01,
                "tau_plus": 10.0,
                "tau_minus": 10.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.02
            },
            "structural": {
                "enable": true,
                "synaptogenesis_rate": 0.01,
                "pruning_rate": 0.001
            }
        },
        "neuromodulation": {
            "dopamine": {
                "baseline": 0.2,
                "scale": 1.5
            },
            "curiosity": {
                "enable": true,
                "baseline": 0.7
            },
            "novelty": {
                "enable": true,
                "threshold": 0.8
            }
        },
        "environment": {
            "name": "SimpleWorld",
            "width": 15,
            "height": 15,
            "vision_width": 6,
            "vision_height": 6
        },
        "logging": {
            "level": "INFO",
            "to_file": true,
            "filename": "interactive_simulation.log"
        },
        "development": {
            "enable": true,
            "initial_stage": "CriticalPeriod",
            "plasticity_by_age": true
        }
    }
}
```

## Configuration Validation

### Validation Rules

NLM configuration includes validation rules:

1. **Range Validation**: Parameter values must be within valid ranges
2. **Type Validation**: Parameter types must match expectations
3. **Dependency Validation**: Some parameters require others to be enabled
4. **Resource Validation**: Configuration must fit within available resources

### Validation Examples

```cpp
// Configuration validation function
bool validateConfiguration(const std::shared_ptr<Config>& config) {
    // Check neuron count range
    int neuron_count = config->getOr("nlm.simulation.neuron_count", 1000);
    if (neuron_count < 10 || neuron_count > 100000) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuron_count));
        return false;
    }
    
    // Check timestep range
    double timestep = config->getOr("nlm.simulation.timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_ERROR("Invalid timestep: " + std::to_string(timestep));
        return false;
    }
    
    // Check plasticity dependency
    bool stdp_enabled = config->getOr("nlm.plasticity.stdp.enable", false);
    bool hebbian_enabled = config->getOr("nlm.plasticity.hebbian.enable", false);
    bool structural_enabled = config->getOr("nlm.plasticity.structural.enable", false);
    
    if (stdp_enabled || hebbian_enabled || structural_enabled) {
        // At least one plasticity mechanism should be enabled
        if (!stdp_enabled && !hebbian_enabled && !structural_enabled) {
            NLM_LOG_ERROR("Plasticity mechanisms enabled but none selected");
            return false;
        }
    }
    
    return true;
}
```

### Configuration Diagnostics

```cpp
// Diagnostic function to check configuration consistency
void diagnoseConfiguration(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== Configuration Diagnostics ===");
    
    // Check for common issues
    int neuron_count = config->getOr("nlm.simulation.neuron_count", 1000);
    NLM_LOG_INFO("Neuron count: " + std::to_string(neuron_count));
    
    bool plasticity_enabled = config->getOr("nlm.plasticity.stdp.enable", false) ||
                             config->getOr("nlm.plasticity.hebbian.enable", false) ||
                             config->getOr("nlm.plasticity.structural.enable", false);
    NLM_LOG_INFO("Plasticity enabled: " + std::to_string(plasticity_enabled));
    
    std::string log_level = config->getOr("nlm.logging.level", "INFO");
    NLM_LOG_INFO("Log level: " + log_level);
    
    bool logging_to_file = config->getOr("nlm.logging.to_file", false);
    if (logging_to_file) {
        std::string log_filename = config->getOr("nlm.logging.filename", "nlm.log");
        NLM_LOG_INFO("Log file: " + log_filename);
    }
    
    // Check environment compatibility
    int env_width = config->getOr("nlm.environment.width", 20);
    int env_height = config->getOr("nlm.environment.height", 20);
    int neuron_per_cell = neuron_count / (env_width * env_height);
    NLM_LOG_INFO("Neurons per environment cell: " + std::to_string(neuron_per_cell));
    
    if (neuron_per_cell > 100) {
        NLM_LOG_WARNING("High neuron density may impact performance");
    }
}
```

## Configuration Best Practices

### Performance Optimization

1. **Use Appropriate Timesteps**: Smaller timesteps increase accuracy but decrease performance
2. **Balance Neuron Count**: Too many neurons can impact performance
3. **Enable Only Required Features**: Disable unused subsystems to save resources
4. **Configure Logging Wisely**: Use appropriate log levels for production

### Research Reproducibility

1. **Save Configuration Files**: Always save configuration for reproducibility
2. **Use Descriptive Filenames**: Include parameter settings in filenames
3. **Document Configuration Choices**: Keep comments about configuration decisions
4. **Version Control**: Keep configuration files in version control

### Development Best Practices

1. **Use Default Configuration**: Start with defaults and modify as needed
2. **Incremental Testing**: Test with small configurations before scaling up
3. **Configuration Validation**: Validate configuration before use
4. **Error Handling**: Gracefully handle configuration errors

## Configuration Command Line Interface

### Configuration Commands

```bash
# Get current configuration
./nlm --config-info

# Compare configurations
cat config1.json > config1
./nlm --config=config2.json --config-info > config2
# Compare config1 vs config2

# Merge configurations
./nlm --config=base.json --config=override.json --merge > merged.json

# Validate configuration
./nlm --validate --config=my_config.json

# Find configuration issues
./nlm --check --config=my_config.json
```

## Configuration API Reference

### Config Class Methods

| Method | Description | Parameters | Return Type |
|--------|-------------|------------|-------------|
| `loadFromFile(filepath)` | Load configuration from JSON file | `filepath: string` | `bool` |
| `saveToFile(filepath)` | Save configuration to JSON file | `filepath: string` | `void` |
| `has(key)` | Check if configuration key exists | `key: string` | `bool` |
| `getKeys()` | Get all configuration keys | - | `std::vector<std::string>` |
| `set(key, value, source)` | Set configuration value | `key: string, value: T, source: ConfigSource` | `void` |
| `getOr(key, default)` | Get configuration value with default | `key: string, default: T` | `T` |
| `clear()` | Clear all configuration | - | `void` |
| `summary()` | Get configuration summary | - | `std::string` |

### Configuration Sources

| Source | Description |
|--------|-------------|
| `Default` | Built-in default configuration |
| `File` | Loaded from JSON file |
| `CommandLine` | Set via command line arguments |
| `Programmatic` | Set at runtime via code |

### Common Configuration Patterns

1. **Minimal Configuration**: Use defaults with minimal overrides
2. **Standard Research**: Use standard research configuration
3. **Performance Testing**: Use optimized configuration for testing
4. **Production**: Use stable, well-tested configuration
5. **Custom Research**: Use custom configuration for specific research needs

## Conclusion

The NLM configuration system provides comprehensive control over brain simulation parameters. It supports multiple configuration sources, validation, and management tools to ensure reproducible and efficient simulations.

Key takeaways:

1. **Understand Configuration Sources**: Know the priority order of configuration sources
2. **Use Appropriate Configuration**: Choose configuration based on use case
3. **Validate Configuration**: Always validate configuration before use
4. **Save Configuration**: Save configuration for reproducibility
5. **Monitor Configuration**: Monitor configuration during simulation
6. **Follow Best Practices**: Use configuration best practices for optimal results

The NLM configuration system enables flexible and reproducible brain simulations across research, development, and production environments.