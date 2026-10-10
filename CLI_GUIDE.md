# NLM CLI User Guide

This document provides comprehensive information about the NLM command-line interface (CLI).

## Overview

The NLM CLI provides a professional, extensible command-line interface for the Neural Learning Machine (NLM) project. It supports multiple subcommands for running simulations, experiments, analysis, benchmarking, and configuration management.

## Installation

The NLM CLI is included in the standard build. After building the project:

```bash
# Build the project
cd build
make nlm

# Run help
./nlm --help
```

## Basic Usage

### Getting Started

```bash
# Show help and all available commands
./nlm --help

# Show version information
./nlm version

# Run a quick benchmark
./nlm benchmark

# List configuration keys
./nlm config

# Show configuration summary
./nlm config show
```

### Common Patterns

```bash
# Run a 10-second benchmark with detailed output
./nlm benchmark --duration 10 --detail detailed

# Run 5 benchmark iterations
./nlm benchmark --repeats 5

# Configure custom settings
./nlm --config myconfig.cfg benchmark --duration 15
```

## Available Commands

### `version`

**Description:** Show version information and exit.

**Usage:**
```bash
./nlm version
```

**Output:** Displays detailed version information including:
- Version number and phase
- Feature list
- Build information

### `config`

**Description:** Manage NLM configuration files.

**Usage:**
```bash
./nlm config [ACTION] [OPTIONS]

Actions:
  list          List all configuration keys
  show          Show configuration summary
  save <file>   Save configuration to file
  load <file>   Load configuration from file

Options:
  --file <file>  Configuration file path

Examples:
  ./nlm config                   # List configuration keys
  ./nlm config show             # Show configuration summary
  ./nlm config save settings.cfg # Save current configuration
  ./nlm config load settings.cfg # Load configuration from file
```

### `benchmark`

**Description:** Run performance benchmarks for NLM.

**Usage:**
```bash
./nlm benchmark [OPTIONS]

Options:
  --duration <sec>    Benchmark duration in seconds (default: 5)
  --repeats <N>       Number of benchmark runs (default: 3)
  --detail <level>    Detail level (basic, detailed, advanced)

Examples:
  ./nlm benchmark                    # Default benchmark (5s, 3 runs)
  ./nlm benchmark --duration 10      # 10-second benchmark
  ./nlm benchmark --repeats 5       # 5 runs of benchmark
  ./nlm benchmark --duration 30 --repeats 10 --detail detailed
```

### Future Commands (Planned)

The following commands will be added in future versions:

- `simulation`: Run neural simulations
- `experiment`: Run scientific experiments
- `analyze`: Analyze brain states and performance
- `visualize`: Create visualizations of neural activity
- `profile`: Profile performance and resource usage

## Command Options

### Global Options

All commands support these global options:

- `-h, --help`: Show help message for the command
- `-v, --verbose`: Increase verbosity (can be repeated for more detail)
- `-q, --quiet`: Decrease verbosity
- `--verbosity <level>`: Set verbosity level (quiet, error, warning, info, debug)
- `-o, --output <file>`: Output file for results
- `--config <file>`: Configuration file path

### Verbosity Levels

1. **Quiet**: No output except errors
2. **Error**: Only error messages
3. **Warning**: Errors and warnings
4. **Info**: Errors, warnings, and informational messages
5. **Debug**: All messages including debug information

## Configuration Management

### Configuration File Format

NLM uses a custom configuration file format with the following structure:

```ini
# NLM Configuration File

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

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Configuration Management Commands

#### Save Configuration

```bash
# Save current configuration to a file
./nlm config save my_settings.cfg
```

#### Load Configuration

```bash
# Load configuration from a file
./nlm config load my_settings.cfg
```

#### List Configuration Keys

```bash
# List all available configuration keys
./nlm config
```

#### Show Configuration Summary

```bash
# Display current configuration summary
./nlm config show
```

## Configuration File Search Path

The CLI searches for configuration files in the following order:

1. Command-line `--config` option
2. Current directory (`nlm_config.cfg`)
3. Home directory (`~/.nlm/nlm_config.cfg`)
4. User configuration directory (`$XDG_CONFIG_HOME/nlm/nlm_config.cfg`)

## Error Handling

The CLI provides comprehensive error handling with user-friendly error messages:

### Common Error Messages

- **Configuration file not found**: "Could not load configuration from: <file> (using defaults)"
- **Invalid arguments**: "Unknown argument: <argument>"
- **Command execution failure**: "Command execution failed"
- **Exception**: "ERROR: <exception message>"

### Troubleshooting

#### Configuration File Issues

**Problem:** Configuration file not found or invalid.

**Solution:**
- Verify the file exists and is readable
- Check file format and syntax
- Use `nlm config save` to create a valid configuration file

#### Permission Issues

**Problem:** Permission denied when accessing files.

**Solution:**
- Check file and directory permissions
- Ensure you have write access to output files

#### Usage Issues

**Problem:** Command not found or incorrect syntax.

**Solution:**
- Use `--help` to see correct syntax
- Check available commands with `nlm --help`

## Examples

### Basic Usage Examples

```bash
# Run a quick performance test
./nlm benchmark

# Run a longer benchmark with more detail
./nlm benchmark --duration 30 --detail detailed

# Run multiple benchmark iterations
./nlm benchmark --repeats 10

# Use custom configuration
./nlm --config research.cfg benchmark --duration 15 --repeats 5
```

### Advanced Usage Examples

```bash
# Create a custom configuration file
nlm config save my_research_config.cfg

# Edit the configuration file
# (edit using your favorite text editor)

# Run benchmark with custom settings
./nlm --config my_research_config.cfg benchmark --duration 60 --repeats 3

# Save benchmark results to a file
./nlm benchmark --output benchmark_results.json
```

### Script Examples

```bash
#!/bin/bash

# Run a complete benchmark session
nlm config save benchmark_session.cfg
nlm benchmark --duration 120 --repeats 5 --output benchmark_results.txt
nlm benchmark --duration 30 --detail detailed --output detailed_benchmark.json

# Analyze results
python analyze_benchmark.py benchmark_results.txt
```

## Future Enhancements

The NLM CLI will be extended with the following features in future versions:

1. **Simulation Command**: Run neural network simulations with customizable parameters
2. **Experiment Command**: Run scientific experiments with data collection and analysis
3. **Analyze Command**: Analyze brain states, performance metrics, and learning patterns
4. **Visualize Command**: Create visualizations of neural activity and results
5. **Profile Command**: Profile performance and resource usage
6. **Interactive Mode**: Interactive REPL for exploring and controlling NLM
7. **Plugin System**: Extensible plugin architecture for custom commands
8. **Batch Processing**: Support for running multiple commands in batch files

## Help and Support

For additional help and support:

- **Documentation:** Visit the NLM documentation website
- **GitHub Issues:** Report bugs and request features at the NLM GitHub repository
- **Mailing Lists:** Subscribe to NLM development mailing lists
- **Community:** Join the NLM community forums

## License

This documentation is part of the NLM project, which is licensed under the MIT License.

---

*Generated by NLM CLI System*  
*Version 0.1.0 (Phase 6)*