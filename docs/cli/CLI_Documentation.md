# NLM CLI Documentation
# Comprehensive Guide for NLM Command Line Interface

## Overview

The NLM (Neural Learning Machine) Command Line Interface (CLI) provides advanced users with comprehensive control over neural simulation workflows. This enhanced CLI replaces the basic interface in main.cpp with a sophisticated command structure supporting multiple execution modes, advanced options, and extensive customization capabilities.

## CLI Architecture

### Command Categories

The NLM CLI organizes commands into logical categories:

#### Simulation Commands
- **simulation**: Run neural simulations with configurable parameters
- **benchmark**: Performance benchmarking of neural networks
- **profile**: Performance profiling and analysis

#### Configuration Commands
- **config**: Manage configuration files and settings
- **validate**: System validation and testing

#### Debugging Commands
- **debug**: Debug and profiling tools for neural simulations

#### Plugin Commands
- **plugin**: Plugin management commands

#### Analysis Commands
- **analyze**: System analysis and reporting

#### Utility Commands
- **utility**: Utility commands and tools

## Getting Started

### Basic Usage

```bash
# Run a basic simulation
nlm simulation --neuron-count 1000 --max-steps 500

# Simulation with detailed output
nlm simulation --neuron-count 2000 --max-steps 1000 --output-file results.json --output-format json

# Configuration management
nlm config --list
nlm config --config-file myconfig.cfg --validate
```

### Help System

The NLM CLI features an extensive help system:

```bash
# Show general help
nlm help

# Show command-specific help
nlm help simulation
nlm simulation --help

# Search for commands
nlm help --search "simulation"

# List commands by category
nlm help --category simulation
```

## Command Reference

### simulation Command

**Description**: Run neural simulations with configurable parameters

**Usage**:
```bash
nlm simulation [OPTIONS]
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--neuron-count N` | Number of neurons | 500 |
| `--max-steps N` | Maximum simulation steps | 1000 |
| `--timestep T` | Time step duration | 0.001 |
| `--config-file F` | Configuration file path | configs/default.cfg |
| `--output-file F` | Output file path | (none) |
| `--output-format FMT` | Output format: text, json, csv | text |
| `--quiet, -q` | Suppress non-essential output | false |
| `--verbose, -v` | Enable verbose output | false |
| `--enable-progress` | Show progress indicators | true |
| `--parallel N` | Number of parallel threads | 1 |
| `--memory-limit M` | Memory limit in MB | 0 (unlimited) |
| `--plugin-path P` | Plugin directory path | (none) |
| `--enable-profiling` | Enable performance profiling | false |
| `--enable-benchmarking` | Enable benchmarking mode | false |
| `--debug-level L` | Debug level: error, warning, info, debug | info |
| `--enable-validation` | Enable system validation | false |
| `--enable-logging` | Enable detailed logging | false |

**Examples**:

```bash
# Basic simulation
nlm simulation --neuron-count 1000 --max-steps 500

# Advanced simulation with profiling
nlm simulation --neuron-count 2000 --max-steps 1000 --enable-profiling --enable-validation

# Simulation with custom configuration
nlm simulation --config-file advanced.cfg --output-file results.json --output-format json

# High-performance simulation
nlm simulation --neuron-count 1000 --max-steps 2000 --parallel 4 --memory-limit 2048
```

### benchmark Command

**Description**: Performance benchmarking of neural simulations

**Usage**:
```bash
nlm benchmark [OPTIONS]
```

**Options**:

| Option | Description | Default |
|--------|-------------|---------|
| `--neuron-count N` | Number of neurons | 1000 |
| `--steps N` | Number of simulation steps | 1000 |
| `--threads N` | Number of threads | 1 |
| `--output-file FILE` | Output results to file | (none) |

**Examples**:

```bash
nlm benchmark --neuron-count 2000 --steps 500
nlm benchmark --threads 4 --output-file results.csv
```

### config Command

**Description**: Manage configuration files and settings

**Usage**:
```bash
nlm config [OPTIONS]
```

**Options**:

| Option | Description |
|--------|-------------|
| `--list` | List current configuration |
| `--config-file FILE` | Configuration file to load |
| `--output-file FILE` | Configuration file to export to |
| `--validate` | Validate current configuration |

**Examples**:

```bash
nlm config --list
nlm config --config-file myconfig.cfg --validate
nlm config --output-file exported.cfg
```

### debug Command

**Description**: Debug and profiling tools for neural simulations

**Usage**:
```bash
nlm debug [OPTIONS]
```

**Options**:

| Option | Description |
|--------|-------------|
| `--attach` | Attach debugger to running process |
| `--profile, -p` | Enable performance profiling |
| `--validate, -v` | Validate system state |
| `--memory` | Enable memory debugging |
| `--threads` | Enable thread debugging |
| `--log-level LEVEL` | Set log level for debugging |

**Examples**:

```bash
nlm debug --attach
nlm debug --profile --log-level debug
nlm debug --validate --memory
```

### validate Command

**Description**: System validation and testing commands

**Usage**:
```bash
nlm validate [OPTIONS]
```

**Options**:

| Option | Description |
|--------|-------------|
| `--memory` | Check memory allocation |
| `--threads` | Check thread safety |
| `--config` | Validate configuration |
| `--plugins` | Check plugins |

**Examples**:

```bash
nlm validate --memory --threads
nlm validate --config --plugins
```

### plugin Command

**Description**: Plugin management commands

**Usage**:
```bash
nlm plugin [OPTIONS]
```

**Options**:

| Option | Description |
|--------|-------------|
| `--list` | List available plugins |
| `--load PATH` | Load plugins from directory |
| `--unload NAME` | Unload specific plugin |
| `--enable NAME` | Enable plugin |
| `--disable NAME` | Disable plugin |
| `--info NAME` | Show plugin information |

**Examples**:

```bash
nlm plugin --list
nlm plugin --load ./plugins
nlm plugin --info MyPlugin
```

### profile Command

**Description**: Performance profiling and benchmarking

**Usage**:
```bash
nlm profile [OPTIONS]
```

**Options**:

| Option | Description |
|--------|-------------|
| `--benchmark, -b` | Run benchmark |
| `--profile, -p` | Run profiler |
| `--analyze, -a` | Run analysis |
| `--output-file FILE` | Output results to file |
| `--output-format FMT` | Output format: text, json |

**Examples**:

```bash
nlm profile --benchmark
nlm profile --profile --output-file perf.json
```

### analyze Command

**Description**: System analysis and reporting

**Usage**:
```bash
nlm analyze [OPTIONS]
```

**Options**:

| Option | Description |
|--------|-------------|
| `--type TYPE` | Analysis type: performance, memory, network |
| `--output-file FILE` | Output results to file |
| `--output-format FMT` | Output format: text, json, csv |

**Examples**:

```bash
nlm analyze --type performance
nlm analyze --type memory --output-format json
```

### utility Command

**Description**: Utility commands and tools

**Usage**:
```bash
nlm utility [OPTIONS]
```

**Options**:

| Option | Description |
|--------|-------------|
| `--convert` | Run conversion utility |
| `--validate` | Run validation utility |
| `--format-info` | Show supported formats |

**Examples**:

```bash
nlm utility --convert
nlm utility --validate
```

## Advanced Features

### Configuration Files

NLM supports configuration files with the following format:

```ini
# Configuration file example
neuron_count = 500
max_steps = 1000
timestep = 0.001
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
output_format = "json"
enable_profiling = true
```

### Environment Variables

NLM can be configured using environment variables:

```bash
export NLM_LOG_LEVEL=info
export NLM_VERBOSE=true
export NLM_CONFIG_FILE=configs/myconfig.cfg
export NLM_THREADS=4
export NLM_OUTPUT_FORMAT=json
```

### Global Options

All NLM commands support the following global options:

```bash
--help, -h              Show help message
--version               Show version information
--verbose, -v           Enable verbose output
--quiet, -q             Suppress non-essential output
--config FILE           Configuration file path
--output FILE           Output file path
--output-format FMT      Output format (text, json, csv)
--profile               Enable profiling
--benchmark             Enable benchmarking
--threads N             Number of threads to use
```

### Interactive Help

The NLM CLI supports interactive help mode:

```bash
nlm help --interactive
```

In interactive mode, you can:
- Get help for specific commands
- List all commands
- Search for commands
- Get suggestions for options
- Access command examples

## Configuration

### Default Configuration File

The default configuration file is `configs/default.cfg`. You can specify a different configuration file using the `--config-file` option.

### Configuration File Locations

NLM looks for configuration files in the following order:
1. Command line arguments
2. Environment variables
3. `configs/` directory
4. Current directory

### Configuration Validation

You can validate your configuration file using:

```bash
nlm config --config-file myconfig.cfg --validate
```

## Performance Tips

### Optimize Simulation Performance

```bash
# Use multiple threads for better performance
nlm simulation --parallel 4 --neuron-count 2000

# Enable profiling to identify bottlenecks
nlm simulation --enable-profiling --enable-validation

# Use appropriate memory limits
nlm simulation --memory-limit 2048
```

### Batch Processing

Create batch scripts for running multiple simulations:

```bash
#!/bin/bash
for i in {1..10}; do
    echo "Running simulation $i"
    nlm simulation --neuron-count 1000 --max-steps 500 --output-file "results_${i}.json"
done
```

## Troubleshooting

### Common Issues

#### Command not found

Make sure the NLM executable is in your PATH:

```bash
export PATH=$PATH:/path/to/nlm/bin
```

#### Configuration file not found

Specify the full path to the configuration file:

```bash
nlm simulation --config-file /path/to/configs/myconfig.cfg
```

#### Memory limit exceeded

Increase the memory limit or reduce the simulation size:

```bash
nlm simulation --neuron-count 1000 --memory-limit 4096
```

#### Plugin loading errors

Check that plugin files are in the correct format and location:

```bash
nlm plugin --list
nlm plugin --load ./plugins
```

### Getting More Help

For more detailed help:

1. Check the documentation in `docs/cli/`
2. Run `nlm help --interactive` for interactive help
3. Visit the NLM documentation website
4. Check the examples in `examples/cli/`

## License

This documentation is part of the NLM project. For license information, see the LICENSE file.
