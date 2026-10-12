# NLM CLI Enhancement - Usage Guide

## Overview

This document provides comprehensive guidance for using the enhanced NLM (Neural Learning Machine) command-line interface.

## Quick Start

### Basic Usage
```bash
# Run basic neural computation (backward compatible)
nlm run

# Phase 6 integration demo with verbose output
nlm demo --config configs/config.json -v

# Validate configuration
nlm validate configs/default.cfg

# Performance benchmarking
nlm benchmark --profile
```

### Environment Variables
```bash
# Set environment variables
export NLM_NEURON_COUNT=2000
export NLM_SIMULATION_TIMESTEP=0.002
export NLM_LOG_LEVEL=DEBUG

# Run with environment variables
nlm demo
```

## Available Commands

### 1. Basic Commands

#### `nlm run`
- **Description**: Run basic neural computation demonstration (Phase 2 compatible)
- **Use Case**: Backward compatibility, quick testing
- **Arguments**: `--config <file>`, `--verbose`, `--debug`, `--profile`
- **Example**: `nlm run --verbose`

#### `nlm demo`
- **Description**: Phase 6 integration demo with comprehensive verification
- **Use Case**: Integration testing, system validation
- **Arguments**: `--config <file>`, `--verbose`, `--debug`, `--profile`
- **Example**: `nlm demo --config configs/config.json -v`

#### `nlm validate`
- **Description**: Validate configuration and system integration
- **Use Case**: Configuration testing, system health checks
- **Arguments**: `--config <file>`, `--verbose`, `--debug`
- **Example**: `nlm validate configs/default.cfg`

### 2. Advanced Commands

#### `nlm experiment`
- **Description**: Run Phase 6 lifetime learning experiment
- **Use Case**: Comprehensive lifecycle testing, learning analysis
- **Arguments**: `--verbose`, `--debug`
- **Example**: `nlm experiment --verbose`

#### `nlm performance`
- **Description**: Run performance benchmarks
- **Use Case**: Performance testing, optimization analysis
- **Arguments**: `--verbose`, `--debug`, `--profile`
- **Example**: `nlm performance --profile`

#### `nlm benchmark`
- **Description**: Detailed performance profiling
- **Use Case**: Micro-benchmarking, performance analysis
- **Arguments**: `--verbose`, `--debug`
- **Example**: `nlm benchmark`

### 3. Research and Interactive Commands

#### `nlm interactive`
- **Description**: Interactive configuration and control
- **Use Case**: Live experimentation, real-time control
- **Arguments**: `--verbose`, `--debug`
- **Example**: `nlm interactive`

#### `nlm research`
- **Description**: Research mode with system control
- **Use Case**: Scientific experiments, system ablation studies
- **Arguments**: `--verbose`, `--debug`
- **Example**: `nlm research`

#### `nlm help`
- **Description**: Show help information for all commands
- **Use Case**: Command discovery, usage reference
- **Arguments**: None
- **Example**: `nlm help`

## Configuration Files

### Default Configuration
**File**: `configs/default.cfg`
- **Purpose**: Default configuration for backward compatibility
- **Format**: Simple key=value pairs
- **Contents**: Basic neural computation settings

### JSON Configuration
**File**: `configs/config.json`
- **Purpose**: Modern JSON configuration
- **Format**: JSON
- **Features**: Complete Phase 6 configuration with all settings

### YAML Configuration
**File**: `configs/config.yaml`
- **Purpose**: YAML configuration format
- **Format**: YAML
- **Features**: Human-readable configuration with all settings

### Configuration Template
**File**: `configs/config_template.cfg`
- **Purpose**: Configuration template for users
- **Format**: Simple key=value pairs
- **Features**: Example of all configuration options

## Environment Variables

### Supported Variables
```bash
NLM_RANDOM_SEED          Random seed for reproducibility
NLM_NEURON_COUNT         Number of neurons (default: 500)
NLM_REGION_COUNT        Number of brain regions (default: 1)
NLM_CONNECTION_PROBABILITY Connection probability (default: 0.15)
NLM_SIMULATION_TIMESTEP  Simulation timestep (default: 0.001)
NLM_STDP_LTP_WEIGHT      STDP LTP weight (default: 0.02)
NLM_STDP_LTD_WEIGHT      STDP LTD weight (default: 0.015)
NLM_SYNAPTOGENESIS_RATE  Synaptogenesis rate (default: 0.0001)
NLM_PRUNING_RATE         Pruning rate (default: 0.00001)
NLM_DOPAMINE_BASELINE    Dopamine baseline (default: 0.1)
NLM_LOG_LEVEL           Log level (DEBUG, INFO, WARN, ERROR)
NLM_MAX_SIMULATION_STEPS Maximum simulation steps (default: 10000)
NLM_VISUALIZATION_ENABLED Enable visualization (default: false)
```

### Environment Variable Usage
```bash
# Set multiple environment variables
export NLM_NEURON_COUNT=2000
export NLM_SIMULATION_TIMESTEP=0.002
export NLM_LOG_LEVEL=DEBUG

# Run command (environment variables applied)
nlm demo

# Command line arguments override environment variables
nlm demo --config neuron_count=3000
```

## Command Line Arguments

### General Options
```bash
-h, --help           Show help information
-v, --verbose        Enable verbose output
-d, --debug          Enable debug output
-p, --profile        Enable performance profiling
--version            Show version information
```

### Configuration Options
```bash
--config <file>      Load configuration from file (can be repeated)
--help               Show help for specific command
--verbose            Enable verbose output
--debug              Enable debug output
--profile            Enable profiling
```

### Key-Value Arguments
```bash
--key=value          Set configuration value
-key value           Alternative format for key-value arguments
```

## Configuration Loading Priority

1. **Command Line Arguments** (highest priority)
2. **Environment Variables**
3. **Config Files** (in order specified, or default)
4. **Default Values** (lowest priority)

### Example Configuration Loading
```bash
# Command line overrides everything
nlm demo --config neuron_count=3000

# Environment variables + config file
export NLM_NEURON_COUNT=2000
nlm demo --config configs/config.json

# Only config file
nlm demo configs/config.json

# Only defaults
nlm demo
```

## Progress Reporting

### Task Progress
The CLI provides real-time progress reporting for long-running tasks:

- **Task Start**: "Starting task: <description>"
- **Progress Updates**: "Progress: <percentage>% - <message>"
- **Task Completion**: "Task completed: <description> (<duration>s)"

### Example Progress Output
```
Starting task: Phase 6 Integration Demo
  Progress: 10% - Initializing demo environment
  Progress: 20% - Creating Phase 6 Integrated Experiment
  Progress: 30% - Verifying system integration
  Task completed: Phase 6 Integration Demo (45s)
```

## Error Handling

### Configuration Errors
- **Invalid File**: "Warning: Could not load configuration from: <file>"
- **Missing Keys**: "Warning: Missing configuration: <key>, using default"
- **Validation Failure**: "Error: Configuration validation failed"

### Command Errors
- **Initialization Failure**: "Error: Failed to initialize brain"
- **Execution Failure**: "Error: Exception in <command> command"
- **System Errors**: Detailed error messages with stack traces

### Logging Levels
- **DEBUG**: Detailed debugging information
- **INFO**: General information messages
- **WARN**: Warning messages
- **ERROR**: Error messages
- **CRITICAL**: Critical error messages

## Performance Features

### Profiling
- **Performance Profiling**: Detailed timing information
- **Benchmarking**: Performance metrics collection
- **Resource Monitoring**: Memory and CPU usage tracking

### Optimization
- **Efficient Configuration Loading**: Fast parsing of multiple formats
- **Memory Management**: Optimized configuration handling
- **Parallel Processing**: Multi-threaded operation support

## File Formats

### JSON Configuration
```json
{
  "nlm_config": {
    "version": "6.0",
    "phase": "FINAL INTEGRATION",
    "simulation": {
      "timestep": 0.001,
      "max_steps": 10000,
      "neuron_count": 1000
    }
  }
}
```

### YAML Configuration
```yaml
nlm_config:
  version: "6.0"
  phase: "FINAL INTEGRATION"
  simulation:
    timestep: 0.001
    max_steps: 10000
    neuron_count: 1000
```

### Key=Value Configuration
```
# Key=Value format (backward compatible)
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
```

## Example Command Sequences

### Complete Workflow
```bash
# 1. Set environment for research
export NLM_NEURON_COUNT=2000
export NLM_LOG_LEVEL=DEBUG

# 2. Validate configuration
nlm validate configs/config.json

# 3. Run integration demo
nlm demo --config configs/config.json -v

# 4. Run performance benchmarks
nlm benchmark --profile

# 5. Run lifetime experiment
nlm experiment --verbose

# 6. Interactive research session
nlm interactive

# 7. Research mode
nlm research
```

### Research Workflow
```bash
# Enable research mode
export NLM_RESEARCH_MODE=true

# Run controlled experiment
nlm demo --config configs/config.json --research-mode

# Analyze results
nlm benchmark --profile
```

### Production Workflow
```bash
# Standard production configuration
nlm demo --config configs/config.json

# With monitoring
nlm demo --config configs/config.json -v

# Performance optimized
nlm benchmark

# Validation and health check
nlm validate
```

## Installation and Setup

### Building from Source
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j4
```

### Installation
```bash
cd build
make install
```

### Testing
```bash
# Run unit tests
./tests/test_main
./tests/test_config
./tests/test_neuron

# Run integration tests
./experiments/Phase6Demo
./experiments/Phase5Experiment
```

### Configuration Setup
```bash
# Create custom configuration
cp configs/config_template.cfg myconfig.cfg

# Edit configuration with your settings
# (using your preferred text editor)

# Run with custom configuration
nlm demo --config myconfig.cfg
```

## Troubleshooting

### Common Issues

#### "Could not load configuration from file"
- **Cause**: File not found or invalid format
- **Solution**: Check file path, verify file format

#### "Configuration validation failed"
- **Cause**: Missing required keys or invalid values
- **Solution**: Check configuration file, use defaults

#### "Failed to initialize brain"
- **Cause**: Invalid configuration, missing dependencies
- **Solution**: Check configuration, verify system requirements

#### Performance Issues
- **Cause**: Large network size, inefficient configuration
- **Solution**: Use appropriate configuration, check system resources

### Debug Commands
```bash
# Run with debug output
nlm demo --debug

# Use verbose logging
export NLM_LOG_LEVEL=DEBUG
nlm demo

# Check system resources
free -h
df -h
```

## Advanced Usage

### Custom Configuration Templates
Create your own configuration templates:

```bash
# Create project-specific configuration
cat > myproject.json << EOF
{
  "nlm_config": {
    "simulation": {
      "neuron_count": 5000,
      "max_steps": 20000
    },
    "research": {
      "mode": true,
      "data_export": true
    }
  }
}
EOF

# Use custom configuration
nlm demo --config myproject.json
```

### Configuration Comparison
Compare different configurations:

```bash
# Create test configurations
cp configs/config.json config1.json
cp configs/config.yaml config2.yaml

# Run tests with different configurations
nlm demo --config config1.json
nlm demo --config config2.yaml

# Compare results using output analysis
```

### Batch Processing
Process multiple configurations:

```bash
# Batch processing script
for config in configs/*.json; do
    echo "Testing $config"
    nlm demo --config "$config" --verbose
    echo "---"
done
```

## Summary

The enhanced NLM CLI provides:

1. **Comprehensive Command Set**: 8 different command modes
2. **Multiple Configuration Formats**: JSON, YAML, key=value
3. **Environment Variable Support**: NLM_* prefix variables
4. **Progress Reporting**: Real-time task tracking
5. **Error Handling**: Robust error reporting and recovery
6. **Performance Features**: Profiling and benchmarking
7. **Backward Compatibility**: Existing functionality preserved
8. **Research Support**: Tools for scientific experimentation

This implementation transitions NLM from Phase 2 to Phase 6 FINAL INTEGRATION, providing a production-ready, professional CLI suitable for both basic usage and advanced research applications.