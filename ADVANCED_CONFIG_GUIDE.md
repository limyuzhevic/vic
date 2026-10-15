# NLM Advanced Configuration Guide

## Overview

This guide provides comprehensive documentation for NLM's advanced configuration system. It covers configuration file formats, command-line arguments, and advanced usage patterns for experienced users.

## Table of Contents

1. [Introduction](#introduction)
2. [Configuration File Formats](#configuration-file-formats)
3. [Command Line Arguments](#command-line-arguments)
4. [Environment Variables](#environment-variables)
5. [Configuration Validation](#configuration-validation)
6. [Performance Tuning](#performance-tuning)
7. [Advanced Features](#advanced-features)
8. [Troubleshooting](#troubleshooting)

## Introduction

NLM's configuration system provides flexible options for customizing simulation parameters. It supports multiple configuration sources:

- **Default Configuration**: Built-in defaults for all parameters
- **Configuration Files**: External `.cfg`, `.json`, and other formats
- **Command Line Arguments**: Override settings via CLI
- **Environment Variables**: Set configuration via environment variables
- **Runtime Configuration**: Modify settings during simulation

The configuration system is designed to be intuitive while providing advanced capabilities for experienced users.

## Configuration File Formats

### Native NLM Format (.cfg)

The native NLM configuration file uses a simple section-based format:

```ini
[GENERAL]
neuron_count = 1000
region_count = 1
connection_probability = 0.1

[ADVANCED]
enable_checkpointing = true
checkpoint_dir = ./checkpoints
enable_development = true

[ENVIRONMENT]
world_width = 20.0
world_height = 20.0
vision_width = 16
vision_height = 16
```

### JSON Format (.json)

JSON format is useful for programmatic configuration:

```json
{
    "general": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "advanced": {
        "enable_checkpointing": true,
        "enable_development": true
    },
    "environment": {
        "world_width": 20.0,
        "world_height": 20.0,
        "vision_width": 16,
        "vision_height": 16
    }
}
```

### Supported Formats

- **Native NLM (.cfg)**: Simple INI-style format
- **JSON (.json)**: Standard JSON format
- **INI (.ini)**: Standard INI format
- **TOML (.toml)**: Modern TOML format

## Command Line Arguments

NLM supports extensive command-line configuration:

### Basic Usage

```bash
# Basic configuration
nlm --neuron_count=1000 --region_count=1 --random_seed=42

# Advanced configuration
nlm --neuron_count=2000 --region_count=2 --connection_probability=0.2 \
    --stdp_ltp_weight=0.02 --stdp_ltd_weight=0.018 --enable_development
```

### Common Options

- `--neuron_count=1000`: Set number of neurons
- `--region_count=1`: Set number of brain regions
- `--connection_probability=0.1`: Set synapse connection probability
- `--random_seed=42`: Set random seed for reproducibility
- `--simulation_timestep=0.001`: Set simulation time step
- `--enable_checkpointing`: Enable checkpoint saving
- `--checkpoint_dir=./checkpoints`: Set checkpoint directory
- `--enable_development`: Enable developmental stages
- `--enable_curiosity`: Enable curiosity system
- `--enable_dopamine`: Enable dopamine system

### Complex Examples

```bash
# High-performance configuration
nlm --neuron_count=5000 --region_count=4 --enable_vectorization \
    --enable_parallel_processing --memory_pool_size=4194304

# Learning-focused configuration
nlm --neuron_count=3000 --region_count=2 --enable_development \
    --enable_curiosity --enable_novelty --planning_depth=8

# Research configuration
nlm --neuron_count=10000 --region_count=8 --connection_probability=0.05 \
    --stdp_ltp_weight=0.005 --stdp_ltd_weight=0.006 --stdp_tau=50.0 \
    --synaptogenesis_rate=0.00005 --pruning_rate=0.000005
```

## Environment Variables

Environment variables provide an alternative way to set configuration:

```bash
# Set multiple configuration options
export NLM_NEURON_COUNT=1500
export NLM_REGION_COUNT=3
export NLM_RANDOM_SEED=999
export NLM_SIMULATION_TIMESTEP=0.0005
export NLM_ENABLE_DEVELOPMENT=true

# Run NLM
nlm
```

### Supported Environment Variables

All configuration options can be set via environment variables using the prefix `NLM_` followed by the parameter name in uppercase with underscores:

- `NLM_NEURON_COUNT`: Number of neurons
- `NLM_REGION_COUNT`: Number of regions
- `NLM_CONNECTION_PROBABILITY`: Connection probability
- `NLM_RANDOM_SEED`: Random seed
- `NLM_SIMULATION_TIMESTEP`: Simulation time step
- `NLM_ENABLE_CHECKPOINTING`: Enable checkpointing
- `NLM_CHECKPOINT_DIR`: Checkpoint directory
- And many more...

## Configuration Validation

### Validation Rules

Configuration validation ensures that parameter values are within acceptable ranges:

```bash
# Validate configuration file
nlm --config_file=my_config.cfg --validate

# Validate configuration values
nlm --validate_config
```

### Common Validation Issues

- **Neuron Count**: Must be a positive integer
- **Region Count**: Must be a positive integer
- **Connection Probability**: Must be between 0.0 and 1.0
- **Random Seed**: Must be a positive integer
- **Time Step**: Must be positive
- **Memory Parameters**: Must be positive values

## Performance Tuning

### Performance Configuration

Advanced users can tune performance parameters:

```ini
[PERFORMANCE]
enable_vectorization = true
enable_parallel_processing = true
memory_pool_size = 4194304
event_queue_size = 10000
enable_profiling = true
profiler_output = profile_report.json
```

### Profiling Options

Performance profiling provides detailed performance metrics:

```bash
# Enable profiling
nlm --enable_profiling --profiler_output=profile.json

# Profiling output formats
# JSON: Structured profiling data
# TXT: Human-readable profiling summary
# CSV: Tabular profiling data
```

## Advanced Features

### Configuration File Merging

Multiple configuration files can be merged (later files override earlier ones):

```bash
nlm --config_file=base_config.cfg --config_file=experiment_config.cfg
```

### Checkpoint Management

Advanced checkpoint management features:

```bash
# Save checkpoints at specific intervals
nlm --checkpoint_dir=./checkpoints --checkpoint_interval=500

# Enable automatic checkpoint restoration
nlm --checkpoint_file=./checkpoints/latest.bin

# Checkpoint compression options
# none: No compression
# fast: Fast compression
# balanced: Balanced compression
# maximum: Maximum compression
```

### Development Features

Advanced development and learning features:

```bash
# Enable developmental stages with specific timing
nlm --enable_development --development_interval=1000

# Configure neuromodulation systems
nlm --enable_dopamine --enable_curiosity --enable_novelty

# Set neuromodulation parameters
nlm --dopamine_sensitivity=1.5 --curiosity_threshold=0.3
```

## Troubleshooting

### Common Issues and Solutions

**Issue**: Configuration file not found
```bash
# Solution: Ensure the file exists
nlm --config_file=./my_config.cfg

# Solution: Use absolute path
nlm --config_file=$(pwd)/my_config.cfg
```

**Issue**: Invalid parameter values
```bash
# Solution: Check configuration file syntax
nlm --config_file=./my_config.cfg --validate

# Solution: Use valid parameter ranges
nlm --neuron_count=1000  # Instead of neuron_count=0
nlm --connection_probability=0.1  # Instead of 1.5
```

**Issue**: Performance issues
```bash
# Solution: Optimize performance configuration
nlm --enable_vectorization --enable_parallel_processing

# Solution: Monitor performance
nlm --enable_profiling --profiler_output=profile.json
```

### Getting Help

```bash
# Show help for configuration options
nlm --help

# Show version information
nlm --version

# Show configuration examples
nlm --config_examples
```

## Best Practices

### Configuration Organization

1. **Use configuration files for complex setups**
2. **Use command line arguments for quick overrides**
3. **Use environment variables for deployment-specific settings**
4. **Keep configuration files well-documented**
5. **Validate configurations before running experiments**

### Performance Recommendations

1. **Use configuration files for frequently used settings**
2. **Enable profiling for performance analysis**
3. **Monitor memory usage for large configurations**
4. **Use appropriate checkpoint intervals for your use case**
5. **Balance performance with accuracy based on your needs**

### Experiment Best Practices

1. **Document your configuration choices**
2. **Use consistent naming conventions**
3. **Validate configurations before running experiments**
4. **Monitor performance during experiments**
5. **Save configurations for reproducibility**

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [Configuration API Reference](https://nlm.readthedocs.io/configuration)
- [Performance Tuning Guide](https://nlm.readthedocs.io/performance)
- [Advanced Usage Examples](https://nlm.readthedocs.io/examples)

This guide provides comprehensive documentation for NLM's advanced configuration system. For additional information, refer to the official NLM documentation and API references.