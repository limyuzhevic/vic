# Configuration Schema Documentation

## Overview

This document provides comprehensive documentation for the NLM configuration schema system, including all available configuration keys, their types, validation rules, default values, and usage examples.

## Configuration Schema Overview

The NLM configuration system features a comprehensive schema validation system that ensures configuration integrity, provides type safety, and offers detailed error messages. The schema system includes:

- **Schema Registry**: Centralized configuration schema storage
- **Type Validation**: Strict type checking for configuration values
- **Range Validation**: Value range enforcement
- **Pattern Validation**: Regular expression-based validation
- **Allowed Values**: Restricted value sets
- **Error Reporting**: Detailed validation error messages

## Configuration Categories

### Core Simulation Parameters

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `random_seed` | int | 42 | Yes | Random seed for reproducible experiments | Range: [0, 2147483647], Allowed: ["42"] |
| `simulation_timestep` | double | 0.001 | Yes | Simulation timestep in seconds | Range: (0.0001, 1.0] |
| `neuron_count` | int | 1000 | Yes | Number of neurons in the brain | Range: [1, 100000] |
| `region_count` | int | 2 | Yes | Number of brain regions | Range: [1, 100] |
| `connection_probability` | double | 0.1 | Yes | Connection probability for random connectivity | Range: [0.0, 1.0] |

### Plasticity Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `plasticity_learning_rate` | double | 0.01 | No | Base learning rate for plasticity | Range: [0.0, 1.0] |
| `stdp_ltp_weight` | double | 0.01 | No | STDP Long-Term Potentiation weight | Range: [0.0, 1.0] |
| `stdp_ltd_weight` | double | 0.012 | No | STDP Long-Term Depression weight | Range: [0.0, 1.0] |
| `stdp_time_constant` | double | 20.0 | No | STDP time constant in milliseconds | Range: [1.0, 1000.0] |

### Development Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `development_synaptogenesis_rate` | double | 0.001 | No | Development synaptogenesis rate | Range: [0.0, 1.0] |
| `development_pruning_rate` | double | 0.0001 | No | Development pruning rate | Range: [0.0, 1.0] |

### Neuromodulation Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `dopamine_baseline` | double | 0.1 | No | Dopamine baseline level | Range: [0.0, 10.0] |

### Reward Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `reward_discount_factor` | double | 0.99 | No | Reward discount factor for temporal difference learning | Range: [0.0, 1.0] |

### Environment Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `environment_name` | string | "GridWorld" | No | Name of the environment | None |
| `environment_width` | int | 10 | No | Environment width in cells | Range: [1, 10000] |
| `environment_height` | int | 10 | No | Environment height in cells | Range: [1, 10000] |
| `vision_width` | int | 16 | No | Vision sensor width | Range: [1, 10000] |
| `vision_height` | int | 16 | No | Vision sensor height | Range: [1, 10000] |

### Logging Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `log_level` | string | "INFO" | No | Log level (DEBUG, INFO, WARNING, ERROR) | Allowed: ["DEBUG", "INFO", "WARNING", "ERROR"] |
| `log_to_file` | bool | false | No | Whether to log to file in addition to console | None |
| `log_filename` | string | "nlm.log" | No | Log file name | None |

### Simulation Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `max_simulation_steps` | int | 10000 | No | Maximum number of simulation steps | Range: [1, 1000000] |
| `simulation_time_limit` | double | 0.0 | No | Simulation time limit in seconds (0 = no limit) | Range: [0.0, 1000000.0] |

### Visualization Settings

| Key | Type | Default | Required | Description | Validation |
|-----|------|---------|----------|-------------|------------|
| `visualization_enabled` | bool | false | No | Whether to enable visualization | None |
| `visualization_update_rate` | double | 30.0 | No | Visualization update rate in Hz | Range: [1.0, 1000.0] |

## Configuration File Formats

### JSON Format

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "dopamine_baseline": 0.1,
  "log_level": "INFO",
  "log_to_file": true,
  "log_filename": "nlm_simulation.log"
}
```

### YAML Format

```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
dopamine_baseline: 0.1
log_level: INFO
log_to_file: true
log_filename: "nlm_simulation.log"
```

### Legacy .cfg Format

```
# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Neuromodulation settings
dopamine_baseline = 0.1

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm_simulation.log
```

## Command Line Configuration

The configuration system supports command line arguments for overriding configuration values:

### Argument Formats

#### `--key=value` format
```bash
./nlm --random_seed=123 --neuron_count=500 --log_level=DEBUG --log_to_file=true
```

#### `-key value` format
```bash
./nlm -random_seed 123 -neuron_count 500 -log_level DEBUG -log_to_file true
```

#### Configuration file specification
```bash
./nlm --config configs/production.cfg --config custom.json
```

### Command Line Examples

```bash
# Basic configuration with command line overrides
./nlm --neuron_count=2000 --log_level=INFO --log_to_file=true

# Load configuration file and override specific values
./nlm --config configs/default.cfg --random_seed=999 --connection_probability=0.2

# Minimal configuration with all defaults
./nlm --neuron_count=100

# Development configuration
./nlm --log_level=DEBUG --log_to_file=true --simulation_timestep=0.0001
```

## Configuration Validation

### Validation Process

1. **Schema Check**: Verify that all configuration keys have defined schemas
2. **Type Validation**: Ensure values match the expected types
3. **Range Validation**: Check that values fall within specified ranges
4. **Pattern Validation**: Validate against regular expressions if specified
5. **Allowed Values Validation**: Check that values are in the allowed set
6. **Dependency Validation**: Validate inter-configuration dependencies

### Validation Error Messages

#### Type Mismatch
```
Error: Configuration validation failed for key: random_seed
  Error: Type mismatch: expected int
```

#### Range Violation
```
Error: Configuration validation failed for key: neuron_count
  Error: Value 0 outside range [1, 100000]
```

#### Pattern Violation
```
Error: Configuration validation failed for key: environment_name
  Error: Value does not match pattern: ^[A-Z][a-zA-Z0-9_]*$
```

#### Allowed Values Violation
```
Error: Configuration validation failed for key: log_level
  Error: Value not in allowed values list
```

### Validation Output Examples

#### Successful Validation
```
[INFO] Configuration validation successful
[INFO] Loaded configuration from: configs/default.cfg
[INFO] Applied command line configuration overrides
[INFO] ✓ All configuration entries are valid

Configuration Schema:
  random_seed (int) [REQUIRED]: Random seed for reproducible experiments [Default: 42]
  simulation_timestep (double) [REQUIRED]: Simulation timestep in seconds [Default: 0.001]
  neuron_count (int) [REQUIRED]: Number of neurons in the brain [Default: 1000]
  region_count (int) [REQUIRED]: Number of brain regions [Default: 2]
  connection_probability (double) [REQUIRED]: Connection probability for random connectivity [Default: 0.1]
```

#### Failed Validation
```
[WARNING] Configuration validation failed for key: neuron_count
  [ERROR] Error: Value -5 outside range [1, 100000]
[WARNING] Configuration validation failed for key: log_level
  [ERROR] Error: Value not in allowed values list
[WARNING] Configuration validation failed for key: simulation_timestep
  [ERROR] Error: Value 0.0 outside range (0.0001, 1.0]

Configuration validation failed. Using defaults.
```

## Schema Registry Management

### Adding New Configuration Keys

#### Programmatic Registration

```cpp
#include "core/Config/Config.hpp"

// Register a new configuration key
ConfigSchema schema;
schema.key = "custom_parameter";
schema.type = "double";
schema.defaultValue = "1.0";
schema.description = "Custom parameter for testing";
schema.required = false;
schema.minValue = 0.0;
schema.maxValue = 100.0;
schema.validationPattern = "^\\d+(\\.\\d+)?$";

ConfigRegistry& registry = ConfigRegistry::getInstance();
registry.registerConfig("custom_parameter", schema);
```

#### Adding Multiple Configuration Keys

```cpp
void registerConfigurationSchema() {
    ConfigRegistry& registry = ConfigRegistry::getInstance();
    
    // Core parameters
    registry.registerConfig("random_seed", ConfigSchema{
        "random_seed", "int", "42",
        "Random seed for reproducible experiments", true, "",
        0, 2147483647, {"42"}
    });
    
    registry.registerConfig("simulation_timestep", ConfigSchema{
        "simulation_timestep", "double", "0.001",
        "Simulation timestep in seconds", true, "",
        0.0001, 1.0
    });
    
    // Add more configurations...
}
```

### Schema Validation API

#### Validation Methods

```cpp
ConfigRegistry& registry = ConfigRegistry::getInstance();

// Check if a schema exists
bool hasSchema = registry.hasSchema("random_seed");

// Get schema for a specific key
const ConfigSchema* schema = registry.getSchema("random_seed");

// Get all registered keys
std::vector<std::string> keys = registry.getAllKeys();

// Get schema description
std::string description = registry.getSchemaDescription();
```

#### Comprehensive Validation

```cpp
// Create configuration map
std::map<std::string, ConfigValue> configMap;
configMap["random_seed"] = ConfigValue(42);
configMap["neuron_count"] = ConfigValue(1000);

// Get validation errors
std::vector<std::string> errors = registry.getValidationErrors(configMap);
for (const auto& error : errors) {
    std::cout << "Error: " << error << std::endl;
}

// Get detailed validation report
std::string report = registry.getValidationReport(configMap);
std::cout << report << std::endl;
```

## Configuration Migration

### Version Management

The configuration system supports version-based migration to handle schema changes:

```cpp
struct ConfigMigration {
    int fromVersion;
    int toVersion;
    std::function<void(Config&)> migrate;
    std::string description;
};
```

### Migration Example

```cpp
void migrateFromV1ToV2(Config& config) {
    // Migrate configuration from version 1 to version 2
    // This might involve renaming keys, changing types, or updating defaults
    
    // Example: Rename old key to new key
    if (config.has("old_parameter")) {
        auto value = config.getOr<int>("old_parameter", 0);
        config.set("new_parameter", value, ConfigSource::Runtime);
        config.remove("old_parameter");
    }
}

// Register migration
ConfigMigration migration;
migration.fromVersion = 1;
migration.toVersion = 2;
migration.migrate = migrateFromV1ToV2;
migration.description = "Migrate configuration from version 1 to 2";
```

## Configuration Statistics

### Statistics Collection

The configuration system automatically collects statistics:

```cpp
struct ConfigStats {
    size_t totalEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    double averageMemoryUsage;
    time_t lastModified;
};
```

### Statistics Output

```
Configuration Statistics (Version 2):
  Total entries: 15
  File entries: 8
  Command line entries: 3
  Runtime entries: 4
  Last modified: 1234567890
  Memory usage: 2.4 KB

Configuration Entries:
  random_seed = [42] (1)
  simulation_timestep = [0.001] (1)
  neuron_count = [1000] (1)
  ...
```

## Testing Configuration System

### Unit Tests

```bash
# Run configuration validation tests
./test_config --validation

# Test schema registration
./test_config --schema_registration

# Test configuration loading
./test_config --loading

# Test configuration validation
./test_config --validation

# Run all tests
./test_config --all
```

### Test Configuration Files

#### Test Configuration File 1

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "region_count": 2,
  "invalid_key": "should_fail"
}
```

#### Test Configuration File 2

```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
log_level: INFO
log_to_file: true
```

### Performance Testing

```bash
# Test configuration loading performance
./perf_test --config_file=config.json --iterations=1000

# Test validation performance
./perf_test --validation --iterations=10000

# Test schema lookup performance
./perf_test --schema_lookup --iterations=50000
```

## Error Handling and Recovery

### Error Recovery Strategies

1. **Fallback to Defaults**: When validation fails, use default values
2. **Graceful Degradation**: Disable features that require invalid configuration
3. **User Notification**: Provide clear error messages and recovery options
4. **Configuration Repair**: Automatically attempt to fix common configuration errors

### Recovery Examples

```cpp
bool loadConfigurationSafely(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(filepath)) {
        NLM_LOG_WARNING("Failed to load configuration from " + filepath);
        NLM_LOG_INFO("Using default configuration");
        return true;  // Continue with defaults
    }
    
    // Validate configuration
    if (!validateConfiguration(config)) {
        NLM_LOG_ERROR("Configuration validation failed");
        NLM_LOG_INFO("Attempting recovery...");
        
        // Apply recovery strategies
        applyConfigurationRecovery(config);
        
        if (!validateConfiguration(config)) {
            NLM_LOG_ERROR("Recovery failed. Using minimal configuration.");
            config->clear();
            // Set minimal valid configuration
            config->set("random_seed", 42, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", 100, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
        }
    }
    
    return true;
}
```

## Migration Guide

### From Legacy Configuration System

1. **Migrate Configuration Files**
   - Convert .cfg files to JSON or YAML format
   - Update deprecated configuration keys
   - Add missing configuration keys with default values

2. **Update Code**
   - Replace direct configuration access with Config class methods
   - Add validation where missing
   - Update error handling to use new validation API

3. **Testing**
   - Run validation tests on migrated configurations
   - Test edge cases and error conditions
   - Verify backward compatibility

### Best Practices

1. **Schema Validation**: Always validate configuration before use
2. **Default Values**: Provide sensible defaults for all configuration keys
3. **Documentation**: Keep configuration documentation up to date
4. **Testing**: Include configuration validation in automated tests
5. **Logging**: Log configuration loading and validation for debugging

## Conclusion

The NLM configuration system provides a robust, validated, and well-documented configuration management framework. It supports multiple file formats, comprehensive validation, detailed error reporting, and extensive configuration management capabilities.

Key features include:

- **Multi-format Support**: JSON, YAML, and legacy .cfg formats
- **Schema Validation**: Type checking, range validation, and pattern validation
- **Command Line Overrides**: Flexible command line configuration
- **Error Recovery**: Robust error handling and recovery mechanisms
- **Statistics Tracking**: Detailed configuration statistics and performance metrics
- **Documentation**: Comprehensive schema documentation and usage examples
- **Testing**: Extensive testing framework for configuration validation

This system ensures configuration integrity, provides clear error messages, and offers extensive configuration management capabilities for the NLM project.