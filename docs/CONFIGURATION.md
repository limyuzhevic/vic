# NLM Configuration Documentation

## Overview

This document provides comprehensive information about the NLM configuration system, including:
- Configuration file formats
- Command-line options
- Advanced configuration features
- Logging configuration
- Configuration validation
- Best practices

## Table of Contents

1. [Introduction](#introduction)
2. [Configuration File Formats](#configuration-file-formats)
   - [Default Configuration](#default-configuration)
   - [JSON Configuration](#json-configuration)
   - [YAML Configuration](#yaml-configuration)
3. [Command-Line Interface](#command-line-interface)
   - [Basic Options](#basic-options)
   - [Configuration Options](#configuration-options)
   - [Examples](#examples)
4. [Advanced Configuration Features](#advanced-configuration-features)
   - [Configuration Templates](#configuration-templates)
   - [Configuration Validation](#configuration-validation)
   - [Configuration Comparison](#configuration-comparison)
   - [Configuration Merging](#configuration-merging)
5. [Logging Configuration](#logging-configuration)
   - [Log Levels](#log-levels)
   - [File Logging](#file-logging)
   - [Log File Rotation](#log-file-rotation)
   - [Console Output](#console-output)
6. [Configuration Validation](#configuration-validation)
   - [Required Values](#required-values)
   - [Range Validation](#range-validation)
   - [Format Validation](#format-validation)
7. [Best Practices](#best-practices)
   - [Configuration Management](#configuration-management)
   - [Logging Configuration](#logging-configuration-1)
   - [Testing](#testing)
8. [Troubleshooting](#troubleshooting)
   - [Common Issues](#common-issues)
   - [Error Messages](#error-messages)
   - [Solutions](#solutions)
9. [References](#references)

## Introduction

The NLM (Neural Learning Machine) configuration system provides comprehensive configuration management for neural network simulations. It supports multiple configuration sources (files, command-line, runtime), advanced validation, and detailed logging.

### Key Features

- **Multiple configuration sources**: Load configurations from files, command-line arguments, or set them at runtime
- **Advanced validation**: Validate configuration values against custom rules
- **Configuration templates**: Define and apply reusable configuration sets
- **Detailed logging**: Comprehensive logging with file rotation support
- **Progress reporting**: Real-time progress reporting for long-running operations
- **Error handling**: Comprehensive error handling and recovery mechanisms

## Configuration File Formats

### Default Configuration

The default configuration file format is a simple key-value format:

```
# Configuration parameter key = value
# Comments start with # or /
# Values can be quoted for strings

# Basic parameters
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development parameters
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Logging parameters
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### JSON Configuration

For more complex configurations, JSON format is supported:

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0
    },
    "development": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    "logging": {
        "log_level": "INFO",
        "log_to_file": false,
        "log_filename": "nlm.log"
    }
}
```

### YAML Configuration

YAML provides a more readable configuration format:

```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.15

plasticity:
    stdp_ltp_weight: 0.02
    stdp_ltd_weight: 0.015
    stdp_tau: 20.0

development:
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

logging:
    log_level: INFO
    log_to_file: false
    log_filename: nlm.log
```

## Command-Line Interface

The NLM executable provides extensive command-line options for configuration:

### Basic Options

| Option | Description | Default |
|--------|-------------|---------|
| `-h`, `--help` | Show help message | N/A |
| `-v`, `--verbose` | Enable verbose logging | Disabled |
| `-q`, `--quick` | Run quick test (reduced neuron count) | Disabled |

### Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `--config FILE` | Load configuration from FILE | `configs/default.cfg` |
| `--output-log FILE` | Save log output to FILE | None |
| `--steps N` | Set maximum number of steps | 1000 |
| `--key=value` | Set configuration value | None |
| `-key value` | Set configuration value (alternative format) | None |

### Examples

```bash
# Basic usage with default configuration
./nlm

# Quick test with logging to file
./nlm --quick --output-log nlm.log

# Custom configuration with verbose logging
./nlm --verbose --config myconfig.cfg

# Configuration overrides
./nlm --neuron_count=500 --connection_probability=0.05

# JSON configuration
./nlm --config config.json

# YAML configuration
./nlm --config config.yaml
```

## Advanced Configuration Features

### Configuration Templates

Configuration templates allow you to define reusable configuration sets:

```cpp
struct ConfigTemplate {
    std::string name;
    std::map<std::string, ConfigValue> defaultValues;
    std::map<std::string, std::function<ValidationResult(const ConfigValue&)>> validators;
    std::unordered_set<std::string> requiredKeys;
};
```

#### Template Validation

Templates can include validation rules:

```cpp
// Template validation function
ValidationResult validateNeuronCount(const ConfigValue& value) {
    auto val = std::get<int64_t>(value);
    if (val <= 0) {
        return ValidationResult::Error("neuron_count must be positive");
    }
    return ValidationResult::Success();
}

// Create template
ConfigTemplate basicTemplate;
basicTemplate.name = "basic_config";
basicTemplate.defaultValues["neuron_count"] = 1000;
basicTemplate.defaultValues["connection_probability"] = 0.15;
basicTemplate.addValidation("neuron_count", validateNeuronCount);
basicTemplate.addRequired("neuron_count");
```

### Configuration Validation

#### Required Values

Some configuration values must be present:

```cpp
// Template requires neuron_count
void applyBasicConfig() {
    ConfigTemplate template;
    template.addRequired("neuron_count");
    template.apply(configData);
}
```

#### Range Validation

Validate that values are within acceptable ranges:

```cpp
// Range validation for neuron count
ValidationResult validateNeuronCount(const ConfigValue& value) {
    auto val = std::get<int64_t>(value);
    if (val < 1 || val > 10000) {
        return ValidationResult::Error("neuron_count must be between 1 and 10000");
    }
    return ValidationResult::Success();
}
```

#### Format Validation

Validate the format of string values:

```cpp
// Format validation for configuration keys
ValidationResult validateConfigKey(const ConfigValue& value) {
    auto val = std::get<std::string>(value);
    // Check if key follows expected pattern
    if (val.find(' ') != std::string::npos) {
        return ValidationResult::Error("Configuration key cannot contain spaces");
    }
    return ValidationResult::Success();
}
```

### Configuration Comparison

Compare different configuration states:

```cpp
// Compare two configurations
bool configsEqual(const Config& config1, const Config& config2) {
    return config1.configsEqual(config2);
}

// Get differences between configurations
std::vector<std::string> getConfigDifferences(const Config& config1, const Config& config2) {
    std::vector<std::string> differences;
    // Compare key-value pairs
    return differences;
}
```

### Configuration Merging

Merge multiple configuration sources:

```cpp
// Merge base and override configurations
std::map<std::string, ConfigValue> mergedConfig = 
    configMerger.mergeConfigs(baseConfig, overrideConfig);

// Priority: Runtime > CommandLine > File > Default
```

## Logging Configuration

### Log Levels

The NLM logging system supports the following levels:

| Level | Description | Usage |
|-------|-------------|-------|
| `Debug` | Detailed debugging information | Verbose mode only |
| `Info` | General information about execution | Normal operation |
| `Warning` | Potential issues that don't prevent execution | Warnings |
| `Error` | Errors that prevent completion of operations | Errors |
| `Critical` | Critical errors that prevent program execution | Fatal errors |

### File Logging

Configure logging to files:

```cpp
// Enable file logging
consoleLogger->setOutputFile("nlm.log");

// Configure log level from configuration
if (config->has("log_level")) {
    std::string logLevel = config->getOr<std::string>("log_level", "INFO");
    if (logLevel == "DEBUG") {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (logLevel == "WARNING") {
        consoleLogger->setLevel(LogLevel::Warning);
    } // etc.
}
```

### Log File Rotation

Automatic log file rotation prevents excessive disk usage:

```cpp
// Configure file rotation (10MB max, 5 backup files)
consoleLogger->setFileRotation(true, 10 * 1024 * 1024, 5);
```

### Console Output

Configure console output:

```cpp
// Enable colors in console output
consoleLogger->setUseColors(true);

// Set log level for console
consoleLogger->setLevel(LogLevel::Info);
```

## Configuration Validation

### Required Values

Some configuration values are required for proper operation:

```cpp
// Check for required configuration
bool validateRequiredConfig(const Config& config) {
    if (!config.has("neuron_count")) {
        NLM_LOG_ERROR("Missing required configuration: neuron_count");
        return false;
    }
    if (!config.has("simulation_timestep")) {
        NLM_LOG_ERROR("Missing required configuration: simulation_timestep");
        return false;
    }
    return true;
}
```

### Range Validation

Ensure values are within acceptable ranges:

```cpp
// Validate neuron count range
bool validateNeuronCount(const Config& config) {
    auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (neuronCount < 1 || neuronCount > 10000) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount) +
                     " (must be between 1 and 10000)");
        return false;
    }
    return true;
}
```

### Format Validation

Validate the format of string values:

```cpp
// Validate configuration keys
bool validateConfigKeys(const Config& config) {
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        if (key.find(' ') != std::string::npos) {
            NLM_LOG_ERROR("Invalid configuration key: '" + key + "' (cannot contain spaces)");
            return false;
        }
    }
    return true;
}
```

## Best Practices

### Configuration Management

1. **Use configuration templates** for common configuration sets
2. **Validate configuration** before running simulations
3. **Log configuration changes** for debugging purposes
4. **Use quick test mode** for development and testing
5. **Document configuration** for team collaboration

### Logging Configuration

1. **Configure appropriate log levels** for your use case
2. **Enable file logging** for persistent logs
3. **Use log rotation** to prevent excessive disk usage
4. **Monitor log files** for issues and performance
5. **Configure colors** for better readability in console

### Testing

1. **Use quick test mode** for rapid iteration
2. **Validate configuration** before running tests
3. **Check logs** for debugging information
4. **Use verbose mode** for detailed debugging
5. **Document test configurations** for reproducibility

## Troubleshooting

### Common Issues

#### Configuration File Not Found

**Error Message:**
```
Failed to load configuration file: configs/default.cfg
```

**Solution:**
1. Check if the configuration file exists
2. Verify file permissions
3. Use absolute path for configuration file
4. Create default configuration file if missing

#### Invalid Configuration Values

**Error Message:**
```
Invalid line in config file: 42 - invalid_format
Critical configuration error: neuron_count must be positive
```

**Solution:**
1. Check configuration file syntax
2. Verify value types (integer, float, string, boolean)
3. Use correct format for arrays and objects
4. Remove comments or escape special characters

#### Log File Issues

**Error Message:**
```
Failed to open log file: nlm.log
```

**Solution:**
1. Check write permissions for log directory
2. Create log directory if it doesn't exist
3. Use absolute path for log file
4. Configure file rotation to prevent disk full

### Error Messages

| Error Message | Cause | Solution |
|--------------|-------|----------|
| "Failed to load configuration file" | Configuration file not found | Check file path and permissions |
| "Invalid line in config file" | Syntax error in configuration | Check file syntax and format |
| "Failed to open log file" | Log directory not writable | Check permissions or create directory |
| "Invalid neuron_count" | Value out of range | Set value between 1 and 10000 |
| "Failed to initialize brain" | Brain initialization failed | Check configuration or system resources |

### Solutions

1. **Configuration Issues:**
   - Use `--help` to see available options
   - Check configuration file format
   - Validate configuration values
   - Use `--verbose` for detailed error messages

2. **Logging Issues:**
   - Check log file permissions
   - Configure log directory
   - Use `--output-log` to specify log file
   - Enable log rotation

3. **System Issues:**
   - Check available memory
   - Verify system requirements
   - Use `--quick` for reduced resource usage
   - Check system logs for more information

## References

### Related Documentation

- **HOW_TO_USE.md**: Comprehensive usage guide
- **easy_usage.md**: Quick start guide
- **docs/ARCHITECTURE.md**: System architecture overview
- **docs/SCIENCE.md**: Scientific background

### API Documentation

- **Config.hpp**: Configuration class reference
- **Config.cpp**: Configuration implementation
- **Logger.hpp**: Logger class reference
- **Logger.cpp**: Logger implementation

### Examples

- **Example configuration files**: `configs/default.cfg`
- **Test configuration**: `tests/test_config.cpp`
- **Configuration validation examples**: In `tests/` directory

## Conclusion

The NLM configuration system provides comprehensive configuration management for neural network simulations. It supports multiple configuration sources, advanced validation, detailed logging, and extensive command-line options. By following the best practices outlined in this document, you can effectively configure and run NLM simulations for a wide range of applications.

For more information about specific features or to report issues, please refer to the NLM documentation and support channels.
