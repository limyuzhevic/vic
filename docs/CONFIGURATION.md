# NLM Configuration Documentation

## Overview

This document provides comprehensive information about the NLM (Neural Learning Machine) configuration system, including basic configuration, advanced features, and examples for different use cases.

## Table of Contents

1. [Basic Configuration](#basic-configuration)
2. [Advanced Configuration Features](#advanced-configuration-features)
3. [Configuration File Formats](#configuration-file-formats)
4. [Command-Line Interface](#command-line-interface)
5. [Configuration Validation](#configuration-validation)
6. [Configuration Tools](#configuration-tools)
7. [Examples](#examples)
8. [Migration Guide](#migration-guide)

## Basic Configuration

### Configuration Sources

NLM supports multiple configuration sources with the following precedence (highest to lowest):

1. **Runtime Configuration** - Set programmatically via `Config::set()`
2. **Command Line Arguments** - Via `--key=value` or `-key value`
3. **Configuration File** - Via `configs/` directory
4. **Default Configuration** - Built into the application

### Basic Configuration File Format

The simple key=value format is the original NLM configuration format:

```ini
# Basic configuration example
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

### Configuration Value Types

NLM supports the following configuration value types:

| Type | Format | Example |
|------|--------|---------|
| Integer | `key = 42` | `neuron_count = 1000` |
| Float | `key = 3.14` | `simulation_timestep = 0.001` |
| String | `key = "value"` | `environment_name = "GridWorld"` |
| Boolean | `key = true` or `key = false` | `log_to_file = false` |
| Integer Array | `key = [1,2,3]` | `some_list = [1,2,3,4,5]` |
| Float Array | `key = [1.5,2.5]` | `some_floats = [0.1,0.5,1.0]` |
| String Array | `key = ["a","b","c"]` | `some_strings = ["a","b","c"]` |

## Advanced Configuration Features

### JSON Configuration Support

NLM now supports JSON configuration files for more advanced use cases:

```json
{
  "nlm": {
    "version": "1.0",
    "description": "Neural Learning Machine Configuration",
    "configuration": {
      "random_seed": 42,
      "simulation_timestep": 0.001,
      "neuron_count": 1000,
      "region_count": 2,
      "connection_probability": 0.1,
      "plasticity": {
        "learning_rate": 0.01,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
      },
      "development": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
      }
    }
  }
}
```

### YAML Configuration Support

NLM also supports YAML configuration files:

```yaml
nlm:
  version: "1.0"
  description: "Neural Learning Machine Configuration"
  configuration:
    random_seed: 42
    simulation_timestep: 0.001
    neuron_count: 1000
    region_count: 2
    connection_probability: 0.1
    plasticity:
      learning_rate: 0.01
      stdp_ltp_weight: 0.02
      stdp_ltd_weight: 0.015
    development:
      synaptogenesis_rate: 0.0001
      pruning_rate: 0.00001
```

## Command-Line Interface

### Basic Usage

```bash
# Run with default configuration
./nlm

# Run with custom configuration file
./nlm --config=my_config.cfg

# Override specific configuration values
./nlm --neuron_count=500 --region_count=1 --connection_probability=0.2

# Using short form (if supported)
./nlm -c my_config.cfg -n 500 -r 1 -p 0.2
```

### Supported Command-Line Options

| Short Form | Long Form | Description | Type | Default |
|------------|-----------|-------------|------|---------|
| `-h`, `--help` | `--help` | Show help information | Flag | N/A |
| `-v`, `--version` | `--version` | Show version information | Flag | N/A |
| `-c`, `--config` | `--config` | Load configuration from file | File | `configs/default.cfg` |
| `-n`, `--neurons` | `--neuron_count` | Set number of neurons | Integer | 1000 |
| `-r`, `--regions` | `--region_count` | Set number of regions | Integer | 2 |
| `-p`, `--prob` | `--connection_probability` | Set connection probability | Float | 0.1 |
| `--seed` | `--random_seed` | Set random seed | Integer | 42 |
| `--timestep` | `--simulation_timestep` | Set simulation timestep | Float | 0.001 |
| `--log-level` | `--log_level` | Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL) | String | INFO |
| `--log-file` | `--log_filename` | Set log filename | String | nlm.log |
| `--enable-plasticity` | `--enable_plasticity` | Enable plasticity | Flag | true |
| `--disable-plasticity` | `--disable_plasticity` | Disable plasticity | Flag | false |

### Advanced Command-Line Features

#### Multiple Values

```bash
# Set multiple neuron counts (creates multiple configurations)
./nlm --neurons=500,800,1000

# Set multiple region configurations
./nlm --regions=1,2,3,4,5
```

#### Ranges

```bash
# Set simulation timestep range (creates multiple configurations)
./nlm --timestep=0.001,0.01,0.1
```

#### Boolean Flags

```bash
# Enable specific features
./nlm --enable-plasticity --enable-development --enable-curiosity

# Disable specific features
./nlm --disable-plasticity --disable-visualization
```

#### Configuration Validation

```bash
# Validate configuration without running
./nlm --validate-config

# Validate and show configuration summary
./nlm --validate-config --show-summary

# Validate with custom configuration file
./nlm --validate-config --config=my_config.cfg
```

## Configuration Validation

### Validation Rules

NLM validates configuration values based on the following rules:

1. **Type Validation**: Values must match expected types
2. **Range Validation**: Numeric values must be within valid ranges
3. **Dependency Validation**: Some options require others
4. **Uniqueness Validation**: Required keys must be unique

### Example Validation Errors

```
Error: Invalid type for 'neuron_count'. Expected integer, got 'abc'
Error: Value for 'neuron_count' must be between 1 and 10000
Error: 'connection_probability' is required when 'plasticity' is enabled
Error: Duplicate configuration key 'random_seed'
```

### Validation Output

```bash
$ ./nlm --validate-config --config=my_config.cfg
Configuration Validation Results:
===========================

Status: PASS

Validated Keys: 15
Errors: 0
Warnings: 2

Configuration Summary:
---------------------
random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.1
... (continuing)

Validation Details:
------------------
[WARNING] Configuration key 'log_to_file' is deprecated
[WARNING] Configuration key 'visualization_update_rate' should be set to 30.0 for optimal performance
```

## Configuration Tools

### NLM Configuration Tool (`nlm-config`)

The NLM configuration tool provides comprehensive configuration management capabilities:

#### Installation

```bash
# Install configuration tools
./scripts/install-config-tools.sh

# Or install system-wide
sudo ./scripts/install-config-tools.sh
```

#### Available Commands

| Command | Description |
|---------|-------------|
| `nlm-config validate [file]` | Validate configuration file |
| `nlm-config compare [file1] [file2]` | Compare two configuration files |
| `nlm-config merge [file1] [file2] [output]` | Merge two configuration files |
| `nlm-config export [file] [format]` | Export configuration to different formats |
| `nlm-config import [file] [format]` | Import configuration from different formats |
| `nlm-config list` | List available configuration presets |
| `nlm-config create [name]` | Create a new configuration preset |
| `nlm-config template [name]` | Generate configuration template |
| `nlm-config info [file]` | Show configuration information |

#### Examples

```bash
# Validate a configuration file
nlm-config validate my_config.cfg

# Compare two configuration files
nlm-config compare config_v1.cfg config_v2.cfg

# Merge configurations
nlm-config merge base.cfg overrides.cfg merged.cfg

# Export to JSON format
nlm-config export my_config.cfg my_config.json

# Import from JSON format
nlm-config import my_config.json my_config.cfg

# List available presets
nlm-config list

# Create a new preset
nlm-config create my_preset

# Generate template for a preset
nlm-config template my_preset > template.cfg

# Show configuration information
nlm-config info my_config.cfg
```

### Configuration Backup and Restore

```bash
# Create configuration backup
nlm-config backup > config_backup_2024_01_18.cfg

# Restore configuration from backup
nlm-config restore config_backup_2024_01_18.cfg

# List backup files
nlm-config backup list

# Delete old backup files
nlm-config backup clean --older-than 30d
```

## Examples

### Example 1: Simple Configuration

This example shows how to create a simple configuration for basic testing:

```bash
# Create a simple configuration file
cat > simple_config.cfg << EOF
# Simple configuration for testing
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.05
log_level = INFO
log_to_file = true
log_filename = simple_test.log
EOF

# Run with the simple configuration
./nlm --config=simple_config.cfg
```

### Example 2: JSON Configuration

This example shows how to use JSON configuration:

```bash
# Create a JSON configuration file
cat > advanced_config.json << EOF
{
  "nlm": {
    "version": "1.0",
    "configuration": {
      "random_seed": 42,
      "simulation_timestep": 0.001,
      "neuron_count": 500,
      "region_count": 2,
      "connection_probability": 0.1,
      "plasticity": {
        "learning_rate": 0.01,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
      },
      "development": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
      },
      "log_level": "INFO",
      "log_to_file": true,
      "log_filename": "advanced_test.log"
    }
  }
}
EOF

# Run with the JSON configuration
./nlm --config=advanced_config.json
```

### Example 3: Advanced Command-Line Configuration

This example shows advanced command-line usage:

```bash
# Run with multiple configuration overrides
./nlm \
  --neurons=1000,2000,3000 \
  --regions=1,2,3 \
  --timestep=0.001,0.01,0.1 \
  --log-level=DEBUG \
  --enable-plasticity \
  --enable-development \
  --enable-curiosity \
  --enable-visualization
```

### Example 4: Configuration Validation Workflow

This example shows a complete configuration validation workflow:

```bash
# Step 1: Create a new configuration
cat > experimental_config.cfg << EOF
# Experimental configuration for testing new features
random_seed = 123
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.15
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002
log_level = DEBUG
log_to_file = true
log_filename = experimental.log
EOF

# Step 2: Validate the configuration
nlm-config validate experimental_config.cfg

# Step 3: Show configuration information
nlm-config info experimental_config.cfg

# Step 4: Compare with default configuration
nlm-config compare configs/default.cfg experimental_config.cfg

# Step 5: Export to JSON for sharing
nlm-config export experimental_config.cfg experimental_config.json

# Step 6: Validate the exported JSON
nlm-config validate experimental_config.json

# Step 7: Run the application
./nlm --config=experimental_config.cfg
```

### Example 5: Configuration Preset Management

This example shows how to manage configuration presets:

```bash
# List available presets
nlm-config list

# Create a development preset
development_config=$(cat << EOF
simulation_timestep = 0.0001
neuron_count = 5000
region_count = 5
connection_probability = 0.2
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
log_level = INFO
log_to_file = true
log_filename = development.log
EOF
echo "$development_config" > presets/development.cfg
nlm-config create development

# Create a production preset
production_config=$(cat << EOF
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001
log_level = WARNING
log_to_file = true
log_filename = production.log
EOF
echo "$production_config" > presets/production.cfg
nlm-config create production

# Use the development preset
./nlm --config=presets/development.cfg

# Use the production preset
./nlm --config=presets/production.cfg
```

## Migration Guide

### From Old Configuration Format to New

If you're migrating from the old key=value format to the new JSON/YAML format, follow these steps:

#### Step 1: Convert Your Configuration File

```bash
# Convert from key=value to JSON format
./scripts/convert-config.py old_config.cfg > new_config.json

# Or convert to YAML format
./scripts/convert-config.py old_config.cfg > new_config.yaml
```

#### Step 2: Validate the New Configuration

```bash
nlm-config validate new_config.json
nlm-config validate new_config.yaml
```

#### Step 3: Update Your Application

```bash
# Update your application's configuration loading
// Old way
config->loadFromFile("old_config.cfg");

// New way (supports both formats)
config->loadFromFile("new_config.json"); // or .yaml
```

#### Step 4: Test Your Application

```bash
# Test with the new configuration
./nlm --config=new_config.json

# Compare outputs with the old configuration
./nlm --config=old_config.cfg
```

### Configuration File Naming Conventions

| File Type | Extension | Description | Example |
|-----------|-----------|-------------|---------|
| Basic Configuration | `.cfg`, `.config` | Simple key=value format | `configs/default.cfg` |
| JSON Configuration | `.json` | Structured JSON format | `configs/settings.json` |
| YAML Configuration | `.yaml`, `.yml` | Structured YAML format | `configs/settings.yaml` |

### Environment-Specific Configurations

Create environment-specific configuration files for different deployment scenarios:

```bash
# Development configuration
development.cfg:
  log_level = DEBUG
  log_to_file = true
  neuron_count = 500

# Testing configuration  
testing.cfg:
  log_level = INFO
  log_to_file = true
  neuron_count = 1000
  random_seed = 42

# Production configuration
production.cfg:
  log_level = WARNING
  log_to_file = true
  log_filename = production.log
  neuron_count = 1000
  random_seed = 12345
  enable_curiosity = true
```

### Configuration Best Practices

1. **Use Descriptive Names**: Use descriptive names for configuration keys
2. **Document Configuration**: Include comments explaining each configuration option
3. **Validate Configuration**: Always validate configuration before use
4. **Backup Configuration**: Regularly backup configuration files
5. **Version Control**: Store configuration files in version control
6. **Environment Separation**: Use separate configuration files for different environments
7. **Configuration Templates**: Create templates for common configuration patterns
8. **Configuration Validation**: Implement configuration validation in your application

## Configuration Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration File Not Found

**Symptom**: Application fails to start with "Configuration file not found" error

**Solution**: 
```bash
# Check if file exists
ls -la my_config.cfg

# Create the file if it doesn't exist
cat > my_config.cfg << EOF
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
EOF

# Try again
./nlm --config=my_config.cfg
```

#### Issue: Invalid Configuration Values

**Symptom**: Application fails to start with validation errors

**Solution**:
```bash
# Validate the configuration
nlm-config validate my_config.cfg

# Check for specific errors
nlm-config validate my_config.cfg --show-errors

# Fix the configuration based on error messages
```

#### Issue: Configuration Values Not Being Applied

**Symptom**: Configuration changes are not taking effect

**Solution**:
```bash
# Check configuration source precedence
# Runtime > Command Line > File > Default

# Verify command-line overrides
./nlm --neuron_count=500 --config=my_config.cfg

# Verify runtime configuration
config->set("neuron_count", 1000, ConfigSource::Runtime);
```

#### Issue: Performance Issues with Large Configurations

**Symptom**: Application startup is slow with large configuration files

**Solution**:
```bash
# Use a more efficient configuration format
# JSON or YAML are generally faster than key=value

# Split configuration into multiple files
# Create separate files for different sections

# Use configuration presets for common scenarios
./nlm --config=presets/light.cfg
```

#### Issue: Memory Usage with Complex Configurations

**Symptom**: Application uses excessive memory with complex configuration

**Solution**:
```bash
# Remove unnecessary configuration options
# Use minimal configuration for production

# Use configuration validation to remove unused options
nlm-config validate my_config.cfg --remove-unused
```

### Getting Help

If you encounter issues with configuration, try these commands:

```bash
# Get help information
./nlm --help
nlm-config --help

# Get detailed help information
./nlm --help-detailed
nlm-config --help-detailed

# Get version information
./nlm --version
nlm-config --version

# Report bugs
# Submit issues to the NLM project repository
```

## Conclusion

The NLM configuration system provides a comprehensive and flexible approach to configuring neural learning machines. From simple key=value files to complex JSON and YAML configurations, NLM supports a wide range of configuration needs.

Key improvements include:

1. **Enhanced File Format Support**: JSON and YAML configuration files
2. **Advanced CLI Argument Parsing**: Rich command-line interface with type conversion
3. **Configuration Validation**: Comprehensive validation with detailed error messages
4. **Configuration Tools**: Powerful configuration management utilities
5. **Environment-Specific Configurations**: Easy configuration for different environments
6. **Backward Compatibility**: Existing configurations continue to work

These improvements make NLM more accessible to users, easier to configure, and more maintainable for developers.