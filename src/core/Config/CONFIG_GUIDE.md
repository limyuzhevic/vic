# NLM Configuration System - Development Guide

This document provides comprehensive guidance for working with the NLM configuration system, including best practices, validation, and advanced usage.

## Overview

The NLM configuration system provides a flexible key-value configuration framework for neural simulation parameters. It supports multiple configuration sources, type validation, and comprehensive error handling.

## Configuration Schema

The configuration system supports the following parameter types and validation rules:

### Core Simulation Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `random_seed` | int64 | 42 | Range: [0, ∞) | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Range: [0.0001, 1.0] | Simulation timestep in seconds |
| `neuron_count` | int64 | 1000 | Range: [1, 1,000,000] | Number of neurons in the brain |
| `region_count` | int64 | 2 | Range: [1, 100] | Number of brain regions |
| `connection_probability` | float | 0.1 | Range: [0.0, 1.0] | Connection probability for random connectivity |

### Plasticity Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `plasticity_learning_rate` | float | 0.01 | Range: [0.0, 1.0] | Plasticity learning rate |
| `stdp_ltp_weight` | float | 0.01 | Range: [0.0, 1.0] | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | Range: [0.0, 1.0] | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | Range: [1.0, 100.0] | STDP time constant in milliseconds |

### Development Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `synaptogenesis_rate` | float | 0.0001 | Range: [0.0, 1.0] | Development synaptogenesis rate |
| `pruning_rate` | float | 0.00001 | Range: [0.0, 1.0] | Development pruning rate |

### Neuromodulation Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `dopamine_baseline` | float | 0.1 | Range: [0.0, 1.0] | Baseline dopamine level |

### Environment Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `environment_name` | string | "GridWorld" | Allowed: ["GridWorld", "RoomWorld", "MazeWorld"] | Name of the simulation environment |
| `environment_width` | int64 | 10 | Range: [5, 1000] | Width of the simulation environment |
| `environment_height` | int64 | 10 | Range: [5, 1000] | Height of the simulation environment |

### Logging Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `log_level` | string | "INFO" | Allowed: ["DEBUG", "INFO", "WARN", "ERROR"] | Log level |
| `log_to_file` | bool | false | - | Whether to log to file |
| `log_filename` | string | "nlm.log" | - | Log filename |

### Simulation Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `max_simulation_steps` | int64 | 10000 | Range: [1, 1,000,000] | Maximum number of simulation steps |
| `simulation_time_limit` | double | 0.0 | Range: [0.0, 1,000,000.0] | Simulation time limit in seconds |

### Visualization Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `visualization_enabled` | bool | false | - | Whether to enable visualization |
| `visualization_update_rate` | float | 30.0 | Range: [1.0, 1000.0] | Visualization update rate in Hz |

### Checkpoint Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `checkpoint_dir` | string | "./checkpoints" | - | Directory for checkpoint files |
| `checkpoint_save_interval` | int64 | 10000 | Range: [100, 1,000,000] | Interval between automatic checkpoints |
| `max_checkpoints` | size_t | 10 | Range: [1, 1000] | Maximum number of checkpoints to keep |
| `checkpoint_compress` | bool | true | - | Whether to compress checkpoint files |

### Memory Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `memory_working_memory_size` | int64 | 1000 | Range: [10, 1,000,000] | Size of working memory in traces |
| `memory_episodic_capacity` | int64 | 1000 | Range: [1, 1,000,000] | Capacity of episodic memory in episodes |

### Prediction Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `prediction_confidence_threshold` | float | 0.5 | Range: [0.0, 1.0] | Confidence threshold for predictions |

### Curiosity Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `curiosity_exploration_rate` | float | 0.01 | Range: [0.0, 1.0] | Rate of curiosity-driven exploration |

### Novelty Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `novelty_threshold` | float | 0.1 | Range: [0.0, 1.0] | Threshold for novelty detection |

### Development Parameters

| Parameter | Type | Default | Validation | Description |
|-----------|------|---------|------------|-------------|
| `development_enabled` | bool | true | - | Whether to enable developmental processes |
| `development_stage_duration` | double | 1000.0 | Range: [10.0, 1,000,000.0] | Duration of each developmental stage |
| `development_plasticity_decay_rate` | float | 0.01 | Range: [0.0, 1.0] | Rate of plasticity decay during development |

## Configuration Sources

The configuration system supports multiple sources with different precedence:

1. **File** (Lowest precedence) - Loaded from `configs/default.cfg`
2. **CommandLine** - Set via `--key=value` or `-key value` arguments
3. **Runtime** (Highest precedence) - Set via `Config::set()` calls

## Configuration Loading

### From File

The configuration system loads values from `configs/default.cfg` using a simple key=value format:

```
# Core parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Plasticity parameters
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Development parameters
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation parameters
dopamine_baseline = 0.1

# Environment parameters
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

### From Command Line

Command line arguments override file settings:

```bash
./nlm --neuron_count=500 --region_count=1 --connection_probability=0.15
./nlm -e 100 -w 20 -h 20
```

### Programmatic

Set configuration values programmatically:

```cpp
#include "core/Config/Config.hpp"

auto config = std::make_shared<Config>();
config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
config->set("random_seed", int64_t(123), ConfigSource::Runtime);
```

## Configuration Validation

The configuration system includes comprehensive validation:

### Type Validation

- Each configuration parameter has a specific expected type
- Automatic type conversion is provided for numeric types
- Type mismatches are reported with clear error messages

### Range Validation

- Numeric parameters are validated against defined ranges
- Out-of-range values are rejected with descriptive errors
- Example: `neuron_count` must be between 1 and 1,000,000

### Allowed Values Validation

- Some parameters accept only specific string values
- Example: `environment_name` must be one of ["GridWorld", "RoomWorld", "MazeWorld"]

### Custom Validators

Custom validation functions can be registered for specific parameters:

```cpp
void validatePositive(const ConfigValue& value) {
    if (std::holds_alternative<double>(value) && std::get<double>(value) <= 0.0) {
        throw std::invalid_argument("Value must be positive");
    }
}

// Register validator
config->registerValidator("learning_rate", validatePositive);
```

## Configuration Utilities

### Summary

Get a human-readable summary of all current configuration values:

```cpp
std::string summary = config->summary();
std::cout << summary;
```

### Get Values

Retrieve configuration values:

```cpp
// Get with type safety
auto value = config->get<int64_t>("neuron_count");
if (value.has_value()) {
    std::cout << "Neuron count: " << value.value() << std::endl;
} else {
    std::cout << "Neuron count not set" << std::endl;
}

// Get with default value
int64_t count = config->getOr<int64_t>("neuron_count", 1000);
```

### Configuration Operations

```cpp
// Check if key exists
if (config->has("neuron_count")) {
    std::cout << "Neuron count is set" << std::endl;
}

// Remove key
config->remove("temporary_parameter");

// Clear all configuration
config->clear();
```

## Best Practices

### 1. Use Appropriate Data Types

Choose the most specific data type for each parameter:

```cpp
// Use int64_t for large integers
config->set("neuron_count", int64_t(10000), ConfigSource::Default);

// Use float for single-precision values
config->set("learning_rate", 0.01f, ConfigSource::Default);

// Use double for high-precision values
config->set("simulation_timestep", 0.001, ConfigSource::Default);
```

### 2. Validate Configuration Early

Validate configuration as early as possible in the application:

```cpp
void validateConfig(const Config& config) {
    auto neuronCount = config.getOr<int64_t>("neuron_count", 1000);
    if (neuronCount < 10 || neuronCount > 100000) {
        throw std::invalid_argument("Invalid neuron count");
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0) {
        throw std::invalid_argument("Timestep must be positive");
    }
}
```

### 3. Use Descriptive Parameter Names

Use clear, descriptive parameter names:

```cpp
// Good
config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
config->set("development_synaptogenesis_rate", 0.0001, ConfigSource::Default);

// Avoid abbreviations or unclear names
config->set("lr", 0.01f, ConfigSource::Default); // Not recommended
```

### 4. Document Configuration Parameters

Include descriptions for all configuration parameters:

```cpp
config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default,
           "Plasticity learning rate for synaptic changes");
```

### 5. Handle Missing Parameters Gracefully

Provide sensible defaults for parameters that might be missing:

```cpp
int64_t neuronCount = config->getOr<int64_t>("neuron_count", 1000);
double timestep = config->getOr<double>("simulation_timestep", 0.001);
```

### 6. Use Configuration Sources Effectively

Leverage different configuration sources for different use cases:

```cpp
// File configuration for production
config->loadFromFile("configs/production.cfg");

// Command line overrides for development
config->loadFromArgs(argc, argv);

// Runtime configuration for experiments
config->set("random_seed", int64_t(42), ConfigSource::Runtime);
```

## Advanced Usage

### Custom Configuration Validators

Create custom validators for complex validation rules:

```cpp
namespace config_validation {

bool validateConnectionProbability(const ConfigValue& value) {
    if (std::holds_alternative<float>(value)) {
        float prob = std::get<float>(value);
        if (prob < 0.0f || prob > 1.0f) {
            return false;
        }
        return true;
    }
    return false;
}

bool validateNeuronCount(const ConfigValue& value) {
    if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value) >= 10 && std::get<int64_t>(value) <= 100000;
    }
    return false;
}

} // namespace config_validation

// Register custom validators
config->registerValidator("connection_probability", 
                         config_validation::validateConnectionProbability);
config->registerValidator("neuron_count", 
                         config_validation::validateNeuronCount);
```

### Configuration Post-Processing

Implement custom post-processing for complex configuration transformations:

```cpp
void normalizeLearningRates(ConfigValue& value) {
    if (std::holds_alternative<float>(value)) {
        float rate = std::get<float>(value);
        if (rate > 1.0f) {
            std::get<float>(value) = 1.0f;
        }
    }
}

config->registerPostProcessor("plasticity_learning_rate", normalizeLearningRates);
```

### Configuration Serialization

Save configuration to files for reproducibility:

```cpp
// Save configuration to a file
bool saved = config->saveToFile("experiment_config.cfg");
if (saved) {
    std::cout << "Configuration saved to experiment_config.cfg" << std::endl;
} else {
    std::cerr << "Failed to save configuration" << std::endl;
}
```

## Migration Guide

### From Phase 1 Configuration

The configuration system has evolved from Phase 1. Key changes:

1. **New Parameters**: Many new parameters were added for Phase 6 integration
2. **Improved Validation**: Comprehensive validation with clear error messages
3. **Better Documentation**: Detailed parameter descriptions and validation rules
4. **Enhanced Flexibility**: Multiple configuration sources and custom validators

### Migration Example

```cpp
// OLD (Phase 1 - limited parameters)
config->set("neuron_count", 1000);
config->set("region_count", 2);

// NEW (Phase 6 - comprehensive parameters)
config->set("neuron_count", int64_t(1000), ConfigSource::Default,
           "Number of neurons in the brain");
config->set("region_count", int64_t(2), ConfigSource::Default,
           "Number of brain regions");
config->set("connection_probability", 0.1f, ConfigSource::Default,
           "Connection probability for random connectivity");
```

## Performance Considerations

### Large Configurations

For configurations with many parameters, consider:

1. **Lazy Loading**: Load parameters only when needed
2. **Caching**: Cache frequently accessed values
3. **Validation**: Validate configuration once at startup

### Thread Safety

The configuration system is thread-safe for read operations. For write operations:

- Use appropriate synchronization if multiple threads modify configuration
- Consider using `ConfigSource::Runtime` for thread-specific configuration

## Troubleshooting

### Common Issues

1. **Configuration not loading**
   - Check file path and permissions
   - Verify file format (key=value)
   - Check for syntax errors in the configuration file

2. **Type conversion errors**
   - Ensure the correct type is requested
   - Check for type mismatches between stored and retrieved values

3. **Validation failures**
   - Use the summary() method to see current configuration values
   - Check the configuration file for invalid values
   - Verify parameter names are correct

4. **Missing parameters**
   - Use getOr() with default values
   - Check if the configuration file includes the required parameters

### Error Messages

Common error messages and their meanings:

- `"Configuration file not found"`: Check file path and existence
- `"Invalid configuration value"`: Check parameter name and value format
- `"Configuration parameter validation failed"`: Check parameter name and validation rules

## References

- [Config.hpp](src/core/Config/Config.hpp) - Configuration class interface
- [Config.cpp](src/core/Config/Config.cpp) - Configuration implementation
- [configs/default.cfg](configs/default.cfg) - Default configuration file
- NLM Documentation: https://nlm.readthedocs.io

## See Also

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [NLM Science Documentation](docs/SCIENCE.md)
- [Phase 6 Integration](docs/PHASE6_FINAL_AUDIT.md)
