# NLM CLI — Command Line Interface for Neural Learning Machine

The NLM CLI provides a comprehensive command-line interface for accessing NLM (Neural Learning Machine) functionality,
allowing users to run simulations, manage configurations, explore brain states, and analyze results programmatically.

## Overview

The NLM CLI is built on the existing NLM C++ neural simulation framework and provides access to all NLM functionality
through an intuitive command-line interface. It's designed for power users, researchers, and automation scripts that
need programmatic access to brain simulation and experiment capabilities.

## Installation

### From pip

```bash
pip install nlm
```

### From source

```bash
pip install -e .
```

## Quick Start

### Run a simple simulation

```bash
nlm simulate --steps 500 --neuron-count 1000 --output simulation_results.json
```

### Create a brain configuration

```bash
nlm create --neurons 2000 --output mybrain.cfg
```

### Explore brain statistics

```bash
nlm explore --stats --neurons --synapses
```

### Run Phase 6 integration test

```bash
nlm experiment --phase6 --output phase6_results.json
```

### Manage configuration

```bash
nlm config list
nlm config set neuron_count 2000
nlm config get environment_name
```

### Batch processing

```bash
# Create a file list
nlm batch --file config_files.txt --parallel 4
```

### Performance benchmarking

```bash
nlm benchmark --steps 1000 --neurons 1500 --repeats 5 --output benchmark_results.json
```

## Commands

### `nlm simulate`

Run brain simulations with configurable parameters.

**Arguments:**
- `--config FILE`: Configuration file (JSON, YAML, or .cfg format)
- `--steps N`: Number of simulation steps (default: 1000)
- `--output FILE`: Output file path
- `--format TEXT|JSON|YAML`: Output format (default: text, overrides --output-format)
- `--neuron-count N`: Override neuron count
- `--verbose`: Enable verbose output

**Examples:**
```bash
# Run a 500-step simulation
nlm simulate --steps 500 --output results.json

# Run with custom configuration
nlm simulate --config myconfig.cfg --steps 1000
```

### `nlm create`

Create and initialize brain configurations.

**Arguments:**
- `--config FILE`: Configuration template file
- `--output FILE`: Save configuration to file
- `--neurons N`: Number of neurons to create

**Examples:**
```bash
# Create default configuration
nlm create --output default.cfg

# Create configuration with custom neuron count
nlm create --neurons 5000 --output research.cfg
```

### `nlm explore`

Explore brain states and statistics.

**Arguments:**
- `--file FILE`: Checkpoint file path
- `--stats`: Display statistics
- `--neurons`: List neurons
- `--synapses`: List synapses
- `--format TEXT|JSON|YAML`: Output format

**Examples:**
```bash
# Explore brain state with statistics
nlm explore --stats --neurons --synapses

# Explore from checkpoint file
nlm explore --file checkpoint.bin --stats
```

### `nlm experiment`

Run integration experiments.

**Arguments:**
- `--phase6`: Run Phase 6 integration test
- `--config FILE`: Experiment configuration file
- `--output FILE`: Results output file

**Examples:**
```bash
# Run Phase 6 integration test
nlm experiment --phase6 --output phase6_results.json
```

### `nlm config`

Configuration management.

**Subcommands:**
- `nlm config set KEY VALUE`: Set configuration value
- `nlm config get KEY`: Get configuration value
- `nlm config list`: List all configuration keys
- `nlm config save FILE`: Save configuration
- `nlm config load FILE`: Load configuration

**Examples:**
```bash
# Set configuration value
nlm config set neuron_count 2000

# Get configuration value
nlm config get environment_name

# List all configuration keys
nlm config list

# Save configuration
nlm config save myconfig.cfg --format cfg

# Load configuration
nlm config load myconfig.cfg
```

### `nlm batch`

Batch processing for large-scale simulations.

**Arguments:**
- `--file FILE`: List of configuration files (one per line)
- `--parallel N`: Number of parallel processes (default: 1)

**Examples:**
```bash
# Process configurations in parallel
nlm batch --file configs.txt --parallel 4
```

### `nlm benchmark`

Performance benchmarking.

**Arguments:**
- `--steps N`: Number of steps
- `--neurons N`: Neuron count
- `--repeats N`: Number of repetitions
- `--output FILE`: Results output file

**Examples:**
```bash
# Run benchmark
nlm benchmark --steps 1000 --neurons 1500 --repeats 5
```

## Configuration Files

The NLM CLI supports multiple configuration file formats:

### JSON (.json)
```json
{
  "random_seed": 42,
  "neuron_count": 1000,
  "simulation_timestep": 0.001
}
```

### YAML (.yaml, .yml)
```yaml
random_seed: 42
neuron_count: 1000
simulation_timestep: 0.001
```

### Simple (.cfg)
```
random_seed = 42
neuron_count = 1000
simulation_timestep = 0.001
```

## Output Formats

The NLM CLI supports three output formats:

### Text (default)
Human-readable output for console display.

### JSON
Structured output for programmatic consumption.

### YAML
Structured output for data processing.

## Configuration Management

The NLM CLI includes comprehensive configuration management:

### Environment Variables
Use environment variables to override configuration:
- `NLM_RANDOM_SEED`
- `NLM_NEURON_COUNT`
- `NLM_REGION_COUNT`
- `NLM_CONNECTION_PROBABILITY`
- `NLM_SIMULATION_STEPS`
- `NLM_LOG_LEVEL`

### Configuration Commands
```bash
# Set configuration
nlm config set KEY VALUE

# Get configuration
nlm config get KEY

# List all keys
nlm config list

# Save to file
nlm config save FILE [--format json|yaml|cfg]

# Load from file
nlm config load FILE
```

## Configuration Presets

The NLM CLI includes built-in configuration presets:

### Available Presets
- `quick_test`: Small configuration for rapid testing
- `research`: Full research configuration with advanced settings
- `minimal`: Minimal configuration for testing basic functionality
- `development`: Enhanced configuration with development features
- `performance`: Performance-optimized configuration for benchmarking

### Using Presets
```bash
# View available presets
nlm config list-presets

# Apply a preset
nlm config apply preset_name
```

## Error Handling

The NLM CLI provides comprehensive error handling:

- File validation and existence checks
- Configuration validation
- Argument validation
- Graceful error messages with suggestions
- Verbose mode for debugging

## Scripting

The NLM CLI is designed for scripting and automation:

### Using JSON Output
```bash
#!/bin/bash

# Run simulation and capture JSON output
results=$(nlm simulate --steps 100 --format json)

# Extract specific values
echo "Neurons: $results" | jq '.brain_status.total_neurons'
```

### Batch Processing Script
```bash
#!/bin/bash

# Process multiple configuration files in parallel
nlm batch --file config_list.txt --parallel 4

# Export results
nlm export-results --batch --output batch_results.json
```

## Extensions

The NLM CLI includes extensible architecture for adding custom commands:

- Plugin system for custom commands
- Extension framework for new functionality
- Module system for additional tools

## Support and Documentation

- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [API Reference](https://nlm.readthedocs.io/en/latest/api/)

## License

MIT License - See LICENSE file for details.

## Contributing

Contributions to the NLM CLI are welcome. Please follow the project's coding standards and contribution guidelines.

## Changelog

See CHANGELOG.md for a detailed history of changes and updates.
