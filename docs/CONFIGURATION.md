# NLM Configuration System

## Overview

The NLM (Neural Learning Machine) project implements a comprehensive configuration system that supports multiple file formats and provides extensive flexibility for advanced users.

## Configuration Formats

### 1. JSON Configuration

JSON configuration is the preferred format for new configurations. It offers:

- **Structured data**: Proper hierarchical organization
- **Type safety**: Strong typing for configuration values
- **Easy parsing**: Standardized JSON format
- **Tooling support**: Extensive JSON tooling ecosystem

#### Example JSON Configuration

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "enable_development": true,
    "enable_curiosity": true,
    "enable_reward_modulation": true
}
```

#### JSON Configuration Benefits

1. **IntelliSense support**: Modern IDEs provide autocomplete for JSON keys
2. **Validation**: JSON schema validation possible
3. **Cross-platform**: Consistent behavior across platforms
4. **Version control**: Easier to diff and merge configuration changes

### 2. Simple Key-Value Configuration

The legacy key-value format is still supported for backward compatibility:

#### Example Simple Configuration

```
# NLM Configuration File
# Neuron configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Timing configuration
simulation_timestep = 0.001
random_seed = 42

# Plasticity configuration
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development configuration
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### 3. Command Line Arguments

Configuration can also be specified via command line arguments:

```bash
# Set individual parameters
./nlm --neuron-count 2000 --region-count 4 --simulation-timestep 0.005

# Use a JSON configuration file
./nlm configs/production.json

# Override specific parameters
./nlm --neuron-count 1500 --enable-development --enable-curiosity
```

## Configuration Parameters

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 500 | Number of neurons in the brain |
| `region_count` | int | 1 | Number of neural regions |
| `connection_probability` | float | 0.15 | Probability of connections between neurons |
| `random_seed` | int | 42 | Random seed for reproducible simulations |

### Timing Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP Long-Term Potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP Long-Term Depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse pruning |

### Feature Flags

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_development` | bool | true | Enable developmental processes |
| `enable_curiosity` | bool | true | Enable curiosity-driven exploration |
| `enable_reward_modulation` | bool | true | Enable reward-based learning |

## Configuration File Search Order

The configuration system searches for configuration files in the following order:

1. **Explicit file path**: Specified via `--config` argument
2. **Current directory**: `config.json`, `config.cfg`, `.nlm-config`
3. **User home directory**: `~/.nlm/config.json`
4. **Default file**: `configs/default.cfg`

## Configuration Validation

The configuration system performs automatic validation:

### Required Parameters

- `neuron_count`: Must be positive integer
- `simulation_timestep`: Must be positive float

### Range Validation

| Parameter | Valid Range | Warning |
|-----------|-------------|---------|
| `neuron_count` | 10-100,000 | Out of recommended range |
| `simulation_timestep` | 0.0-1.0 | Out of recommended range |
| `connection_probability` | 0.0-1.0 | Probability out of range |

### Consistency Checks

- `region_count` cannot exceed `neuron_count`
- Development parameters should be consistent with plasticity parameters

## Configuration Examples

### Development Environment Configuration

```json
{
    "neuron_count": 2000,
    "region_count": 4,
    "connection_probability": 0.1,
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0005,
    "pruning_rate": 0.00005,
    "enable_development": true,
    "enable_curiosity": true,
    "enable_reward_modulation": true
}
```

### Production Configuration

```json
{
    "neuron_count": 5000,
    "region_count": 8,
    "connection_probability": 0.08,
    "random_seed": 99999,
    "simulation_timestep": 0.002,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.008,
    "stdp_tau": 30.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002,
    "enable_development": true,
    "enable_curiosity": false,
    "enable_reward_modulation": true
}
```

### Testing Configuration

```json
{
    "neuron_count": 100,
    "region_count": 1,
    "connection_probability": 0.3,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.05,
    "stdp_ltd_weight": 0.04,
    "stdp_tau": 10.0,
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001,
    "enable_development": true,
    "enable_curiosity": true,
    "enable_reward_modulation": true
}
```

## Command Line Usage

### Basic Usage

```bash
# Run with default configuration
./nlm

# Run with specific configuration file
./nlm configs/myconfig.json

# Run with command line overrides
./nlm --neuron-count 1500 --region-count 3 --simulation-timestep 0.005

# Enable verbose logging
./nlm --verbose

# Enable benchmark mode
./nlm --benchmark

# Save brain state to file
./nlm --save-state --output-file=brain_state.bin
```

### Configuration File Examples

#### Using JSON Configuration

Create `configs/production.json`:

```json
{
    "neuron_count": 2000,
    "region_count": 4,
    "connection_probability": 0.12,
    "random_seed": 12345,
    "simulation_timestep": 0.001
}
```

Run the program:

```bash
./nlm configs/production.json
```

#### Using Simple Configuration

Create `configs/custom.cfg`:

```
# Custom NLM Configuration
neuron_count = 3000
region_count = 6
connection_probability = 0.08
simulation_timestep = 0.002
random_seed = 67890
```

Run the program:

```bash
./nlm configs/custom.cfg
```

#### Command Line Overrides

```bash
# Start with JSON configuration, override specific parameters
./nlm configs/production.json \
    --neuron-count 2500 \
    --enable-curiosity \
    --enable-reward-modulation
```

## Configuration File Format Reference

### JSON Syntax

The JSON configuration files must follow standard JSON syntax:

```json
{
    "key1": value1,
    "key2": "string value",
    "key3": 42,
    "key4": 3.14,
    "key5": true,
    "key6": ["item1", "item2"],
    "key7": {"nested": "object"}
}
```

### Simple Syntax

The simple key-value format follows:

```
key = value
# This is a comment
```

### Allowed Value Types

| Type | Example | Description |
|------|---------|-------------|
| String | `"value"` | Quoted string values |
| Integer | `42` | Whole numbers |
| Float | `3.14` | Decimal numbers |
| Boolean | `true` or `false` | Boolean flags |
| Array | `[1, 2, 3]` | Array values (limited support) |

## Configuration Validation Rules

### Mandatory Parameters

1. **neuron_count**: Must be specified and positive
2. **simulation_timestep**: Must be specified and positive

### Recommended Ranges

- **neuron_count**: 10-100,000
- **simulation_timestep**: 0.0-1.0
- **connection_probability**: 0.0-1.0
- **stdp_ltp_weight**: 0.0-1.0
- **stdp_ltd_weight**: 0.0-1.0

### Compatibility Rules

- `region_count` ≤ `neuron_count`
- `synaptogenesis_rate` ≤ `pruning_rate`
- `stdp_ltp_weight` ≥ `stdp_ltd_weight`

## Advanced Configuration Features

### Configuration Inheritance

Configuration can inherit from base configurations:

```json
{
    "base": "configs/base.json",
    "overrides": {
        "neuron_count": 2000,
        "random_seed": 12345
    }
}
```

### Environment-Specific Configurations

Different configurations for different environments:

- **Development**: Small networks, frequent logging, detailed output
- **Testing**: Deterministic behavior, isolated components
- **Production**: Optimized performance, error handling, monitoring

### Configuration Templates

Predefined configuration templates:

```json
{
    "template": "development",
    "neuron_count": 100,
    "enable_curiosity": true,
    "enable_reward_modulation": false
}
```

## Configuration Best Practices

### 1. Version Control

- Store configuration files in version control
- Use environment-specific configuration files
- Document configuration changes

### 2. Configuration Management

- Use separate configuration for different environments
- Validate configuration before use
- Log configuration changes

### 3. Configuration Security

- Avoid committing sensitive configuration to version control
- Use environment variables for sensitive data
- Validate configuration file permissions

### 4. Configuration Documentation

- Document all configuration parameters
- Provide example configuration files
- Document configuration validation rules

## Troubleshooting

### Common Configuration Errors

1. **File not found**: Ensure configuration files are in the correct location
2. **Invalid JSON syntax**: Use a JSON validator
3. **Missing required parameters**: Check configuration file contents
4. **Invalid parameter values**: Verify parameter ranges

### Configuration Debugging

```bash
# Enable verbose logging to see configuration loading
./nlm --verbose

# Use the command line parser to test configuration
./nlm --help
```

## Configuration Migration

### From Old to New Format

To migrate from the old key-value format to JSON:

1. Convert old configuration files to JSON format
2. Update any tooling to use the new format
3. Test the migrated configuration

### Example Migration

Old `config.cfg`:
```
neuron_count = 500
region_count = 1
simulation_timestep = 0.001
```

New `config.json`:
```json
{
    "neuron_count": 500,
    "region_count": 1,
    "simulation_timestep": 0.001
}
```

## Configuration Performance Considerations

### Large Configurations

For configurations with many neurons:

1. **Memory usage**: Monitor memory consumption
2. **Processing time**: Profile simulation performance
3. **I/O operations**: Cache frequently used configuration values

### Configuration Optimization

1. **Use simple configurations for development**: Smaller files, faster loading
2. **Use JSON configurations for production**: Better tooling support
3. **Compress configuration files**: Reduce disk usage

## Configuration Security

### Sensitive Data

Avoid storing sensitive information in configuration files:

```json
{
    "database": {
        "host": "localhost",
        "port": 5432,
        "username": "admin",
        "password": "secret"  // Use environment variables instead
    }
}
```

### File Permissions

Set appropriate file permissions:

```bash
chmod 600 configs/production.json
```

## Configuration Testing

### Unit Tests

Test configuration parsing:

```cpp
// Test JSON parsing
auto config = std::make_shared<Config>();
config->loadFromFile("test_config.json");
assert(config->getOr<int64_t>("neuron_count", 0) == 100);

// Test command line parsing
config->loadFromArgs(3, (char*[]){"nlm", "--neuron-count", "500"});
assert(config->getOr<int64_t>("neuron_count", 0) == 500);
```

### Integration Tests

Test configuration with the brain system:

```cpp
// Test configuration with brain initialization
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("simulation_timestep", 0.001);

auto brain = std::make_shared<Brain>(config);
assert(brain->initialize());
```

## Configuration References

### Function Reference

- `Config::loadFromFile()`: Load configuration from file
- `Config::loadFromArgs()`: Load configuration from command line arguments
- `Config::saveToFile()`: Save configuration to file
- `Config::set()`: Set configuration value
- `Config::get()`: Get configuration value
- `Config::getOr()`: Get configuration value with default
- `Config::has()`: Check if configuration has key
- `Config::remove()`: Remove configuration key
- `Config::clear()`: Clear all configuration
- `Config::summary()`: Get configuration summary

### Configuration Sources

- `ConfigSource::Default`: Default configuration
- `ConfigSource::File`: Configuration from file
- `ConfigSource::CommandLine`: Configuration from command line
- `ConfigSource::Runtime`: Configuration at runtime

### Configuration Value Types

- `ConfigValue`: Variant type for configuration values
- `int`: Integer values
- `int64_t`: 64-bit integer values
- `double`: Floating point values
- `bool`: Boolean values
- `std::string`: String values
- `std::vector<int>`: Integer arrays
- `std::vector<double>`: Float arrays
- `std::vector<std::string>`: String arrays

## Configuration Migration Guide

### Version 0.1 to 0.2

The configuration system has been enhanced in version 0.2:

1. **New file formats**: Added JSON support
2. **New parameters**: Added development and curiosity parameters
3. **Improved validation**: Enhanced configuration validation
4. **Better logging**: Improved configuration logging

To migrate from version 0.1:

1. Update configuration files to use JSON format
2. Add new parameters as needed
3. Update any tooling that uses configuration
4. Test the migrated configuration

### Configuration Backward Compatibility

The configuration system maintains backward compatibility:

1. **Old file formats**: Simple key-value format still supported
2. **Old parameters**: All old parameters still work
3. **Old API**: Configuration API unchanged

Migration path:

1. Keep existing configuration files for backward compatibility
2. Gradually migrate to new format
3. Update tooling as needed
4. Test throughout the migration process

## Configuration Performance Optimization

### Configuration Loading

Optimize configuration loading:

1. **Cache configurations**: Cache frequently used configurations
2. **Use efficient parsers**: Use fast JSON parsers
3. **Minimize I/O**: Reduce disk I/O operations
4. **Preprocess configurations**: Preprocess configurations for common use cases

### Configuration Storage

Optimize configuration storage:

1. **Use efficient formats**: Use JSON for production, simple format for development
2. **Compress configurations**: Compress large configuration files
3. **Use versioning**: Version configuration changes
4. **Use checksums**: Verify configuration file integrity

### Configuration Validation

Optimize configuration validation:

1. **Lazy validation**: Validate configurations when needed
2. **Use caching**: Cache validation results
3. **Use efficient algorithms**: Use efficient validation algorithms
4. **Use parallel processing**: Validate configurations in parallel

## Configuration Documentation

### Generated Documentation

The configuration system generates documentation:

1. **Configuration summary**: Print configuration summary
2. **Configuration validation**: Validate configuration
3. **Configuration comparison**: Compare configurations
4. **Configuration migration**: Migrate configurations

### Documentation Examples

Example configuration documentation:

```
Configuration Summary:
  Total entries: 15
  Parameters: neuron_count, region_count, connection_probability, ...
  Sources: Default (5), File (3), CommandLine (2), Runtime (1)

Configuration Validation:
  ✓ neuron_count is valid (100)
  ✓ simulation_timestep is valid (0.001)
  ✓ All required parameters present
```

### Configuration Help

Configuration help:

```
Usage: nlm [options] [arguments]

Options:
  --neuron-count INT        Number of neurons (default: 500)
  --region-count INT        Number of regions (default: 1)
  --connection-prob FLOAT   Connection probability (default: 0.15)
  --simulation-timestep FLOAT Simulation timestep (default: 0.001)
  --random-seed INT          Random seed (default: 42)
  --verbose                 Enable verbose logging
  --benchmark               Enable benchmark mode
  --help                    Show this help message
  --version                 Show version information

Arguments:
  [config-file]             Configuration file (JSON or .cfg format)

Examples:
  nlm                                   Run with default configuration
  nlm configs/production.json           Run with JSON configuration
  nlm --neuron-count 2000 --verbose    Run with overrides and verbose logging
```

## Configuration Conclusion

The NLM configuration system provides:

1. **Flexibility**: Supports multiple configuration formats
2. **Performance**: Efficient configuration loading and parsing
3. **Validation**: Comprehensive configuration validation
4. **Documentation**: Extensive documentation and examples
5. **Security**: Secure configuration handling
6. **Testing**: Comprehensive configuration testing
7. **Performance optimization**: Optimized configuration performance
8. **Backward compatibility**: Backward compatibility with existing configurations

This configuration system enables users to configure NLM effectively for various use cases while maintaining flexibility, performance, and ease of use.