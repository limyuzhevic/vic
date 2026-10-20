# Configuration Validation System

## Overview

This document describes the comprehensive configuration validation system implemented for the NLM (Neural Learning Machine) project.

## Key Features

### 1. Configuration Schema Definitions

The system includes detailed configuration schemas for all major configuration categories:

- **Neuromodulation**: dopamine_baseline, reward_discount_factor, curiosity, novelty_sensitivity, prediction_error_baseline
- **Simulation**: simulation_timestep, max_simulation_steps, simulation_time_limit, random_seed
- **Development**: synaptogenesis_rate, pruning_rate, development_threshold, critical_period_end
- **Memory**: working_memory_capacity, working_memory_decay_rate, episodic_memory_capacity, associative_memory_capacity
- **Environment**: width, height, environment_name, wall_penalty
- **Logging**: log_level, log_to_file, log_filename
- **Visualization**: visualization_enabled, visualization_update_rate, visualization_size
- **Reward**: reward_discount_factor, reward_scaling

### 2. Validation Functions

The system provides comprehensive validation including:

- **Type checking**: Ensures values match expected types (integer, float, boolean, string)
- **Value ranges**: Validates that values are within acceptable ranges
- **Required fields**: Checks that mandatory configuration values are present
- **Consistency validation**: Ensures related fields have valid relationships
- **Custom validation**: Supports custom validation functions for complex validation rules

### 3. Error Handling

- **Clear error messages**: Provides detailed descriptions of validation failures
- **Multiple error reporting**: Reports all validation errors at once rather than stopping at the first error
- **File location information**: Tracks line numbers and file locations for file-based configurations
- **Graceful degradation**: Allows configuration loading even with some invalid values in lenient mode

### 4. Configuration Validation API

The Config class now includes these new validation methods:

```cpp
// Validate configuration for a specific category
bool validate(const std::string& category = "default") const;

// Get all validation errors for a specific category
std::vector<ValidationError> getValidationErrors(const std::string& category = "default") const;

// Set validation mode
void setValidationMode(ValidationMode mode);
ValidationMode getValidationMode() const;

// Detect configuration category based on key
static ConfigCategory detectCategory(const std::string& key);
static std::string categoryToString(ConfigCategory category);
```

### 5. Validation Modes

Three validation modes are supported:

- **Strict Mode**: Rejects any invalid configuration (default mode)
- **Lenient Mode**: Allows invalid values but reports them as warnings
- **Permissive Mode**: Allows any configuration with minimal validation

### 6. Configuration Categories

Each configuration key is mapped to a specific category for targeted validation:

- **Neuromodulation**: Related to neuromodulatory signals and reward processing
- **Simulation**: Core simulation parameters like timestep and duration
- **Development**: Structural plasticity and developmental processes
- **Memory**: Working and episodic memory system parameters
- **Environment**: World simulation and sensory environment settings
- **Logging**: Output and debugging configuration
- **Visualization**: Visual output and rendering settings
- **Reward**: Reward processing and reinforcement learning parameters

## Usage Examples

### Basic Validation

```cpp
nlm::Config config;
config->set("dopamine_baseline", 0.5);
config->set("simulation_timestep", 0.001);

// Enable validation
config->setValidationMode(nlm::ValidationMode::Strict);

// Check if configuration is valid
if (config->validate()) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    auto errors = config->getValidationErrors();
    for (const auto& error : errors) {
        std::cout << "Error: " << error.field << " - " << error.message << std::endl;
    }
}
```

### Category-Specific Validation

```cpp
// Validate only neuromodulation settings
config->validate("neuromodulation");

// Validate only simulation settings
config->validate("simulation");
```

### File-Based Configuration

```cpp
// Load from file with validation
config->loadFromFile("config.json");
config->setValidationMode(nlm::ValidationMode::Strict);

// All errors are reported at once
auto errors = config->getValidationErrors();
```

## Validation Rules Examples

### Neuromodulation Settings

- `dopamine_baseline`: Must be between 0.0 and 1.0
- `reward_discount_factor`: Must be between 0.0 and 1.0
- `curiosity`: Must be between 0.0 and 1.0

### Simulation Settings

- `simulation_timestep`: Must be positive and typically small (1e-6 to 0.1)
- `max_simulation_steps`: Must be positive
- `random_seed`: Must be non-negative

### Development Settings

- `synaptogenesis_rate`: Must be between 0.0 and 0.1
- `pruning_rate`: Must be between 0.0 and 0.01
- `development_threshold`: Must be between 0.0 and 1.0

### Memory Settings

- `working_memory_capacity`: Must be positive
- `working_memory_decay_rate`: Must be between 0.0 and 1.0
- `episodic_memory_capacity`: Must be positive

### Environment Settings

- `width`: Must be positive
- `height`: Must be positive
- `environment_name`: Must not be empty

## Testing

Comprehensive tests are available in the `tests/` directory:

- `test_config_validation.cpp`: Full validation test suite
- `test_config.cpp`: Basic configuration tests
- `test_main.cpp`: Main test runner

## Migration Guide

For projects currently using the old configuration system:

1. **Update Includes**: Change `#include "Config.hpp"` to include the new validation headers if needed
2. **New Validation**: Call `validate()` after setting configuration values to check for errors
3. **Error Handling**: Use `getValidationErrors()` to retrieve detailed error information
4. **File Loading**: The `loadFromFile()` method now includes validation support

## Future Enhancements

1. **JSON/YAML Support**: Full JSON and YAML parsing (Phase 2)
2. **Configuration Templates**: Support for configuration templates and presets
3. **Configuration Profiling**: Analysis of configuration impact on performance
4. **Hot-Reloading**: Support for configuration changes during runtime
5. **Validation Plugins**: Extensible validation plugin system

## Configuration File Examples

### Simple Configuration File (Phase 1)

```
# NLM Configuration File
dopamine_baseline = 0.1
simulation_timestep = 0.001
synaptogenesis_rate = 0.001
width = 10
height = 10
log_level = INFO
log_to_file = false
```

### Validated Configuration (Future Phase 2)

```json
{
    "neuromodulation": {
        "dopamine_baseline": 0.1,
        "reward_discount_factor": 0.99,
        "curiosity": 0.5
    },
    "simulation": {
        "simulation_timestep": 0.001,
        "max_simulation_steps": 10000,
        "random_seed": 42
    },
    "environment": {
        "width": 10,
        "height": 10,
        "environment_name": "GridWorld"
    }
}
```

## Performance Considerations

- **Validation Overhead**: Validation adds O(n) overhead where n is the number of configuration values
- **Caching**: Validation results can be cached for frequently used configurations
- **Lazy Validation**: Validation can be deferred until configuration is actually used
- **Batch Validation**: Multiple validation operations can be batched for efficiency

## Error Handling Best Practices

1. **Always Validate**: Validate configuration before use in production
2. **Graceful Degradation**: Use lenient mode for non-critical configurations
3. **Inform Users**: Provide clear error messages to help users fix configuration issues
4. **Log Errors**: Log validation errors for debugging and monitoring
5. **Test Thoroughly**: Test all validation scenarios including edge cases