# NLM Command Line Tools

This directory contains the NLM command line tools that provide enhanced control over the NLM framework.

## Overview

The NLM project now includes several command-line tools to make it easier to:

1. **nlm-cli** - A versatile command-line interface for controlling NLM brains
2. **config-cli** - Configuration management and validation tool
3. **benchmark** - Performance benchmarking tool
4. Various experiment runners for different phases

## Installation

After building with CMake, the tools will be available in the build directory:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Tools will be available in ./bin or build directory
ls -la bin/
```

## Available Tools

### 1. nlm-cli

The main command-line interface for NLM control.

**Usage:**
```bash
nlm-cli <command> [arguments]
```

**Available commands:**
- `create` - Create a new brain with configuration
- `run` - Run brain simulation
- `config` - Get/set configuration values
- `experiment` - Run predefined experiments (phase3, phase4, phase6)
- `agent` - Run agent in world environment
- `validate` - Validate configuration
- `benchmark` - Benchmark brain performance
- `status` - Display brain status
- `checkpoint` - Save/load brain state
- `help` - Show help for commands
- `version` - Show version information
- `list` - List all available commands

**Examples:**
```bash
# Create a brain with custom settings
nlm-cli create --neurons 1000 --regions 2 --config configs/default.cfg

# Run a simulation
nlm-cli run --steps 500 --output brain_state.bin

# Run an agent experiment
nlm-cli agent --world-size 30,30 --episodes 10

# Validate a configuration
nlm-cli validate --config my_config.cfg --strict

# Benchmark performance
nlm-cli benchmark --duration 30

# Check status
nlm-cli status
```

### 2. config-cli

Configuration management and validation tool.

**Usage:**
```bash
config-cli <command> [arguments]
```

**Available commands:**
- `get` - Get configuration value
- `set` - Set configuration value
- `load` - Load configuration from file
- `save` - Save configuration to file
- `validate` - Validate configuration
- `merge` - Merge configuration from file
- `export` - Export configuration to different formats (JSON, INI, YAML)
- `import` - Import configuration from different formats
- `diff` - Show differences between configs
- `history` - Show configuration change history
- `reset` - Reset configuration to defaults
- `profile` - Profile configuration and suggest optimizations
- `template` - Create configuration template
- `search` - Search for configuration keys
- `list` - List all configuration keys
- `count` - Count configuration entries
- `clear` - Clear configuration
- `backup` - Create backup of current configuration

**Examples:**
```bash
# Get a configuration value
config-cli get neuron_count

# Set multiple configuration values
config-cli set neuron_count 1000 --type int
config-cli set connection_probability 0.15 --type double
config-cli set enable_curiosity true --type bool

# Load and validate a configuration
config-cli load configs/production.cfg --validate

# Export to JSON
config-cli export json brain_config.json

# Search for keys
config-cli search "neuron"

# Validate with strict checking
config-cli validate --strict
```

## Configuration File Format

The configuration system supports the following file formats:

### Text Format (Default)
```
# Configuration file example
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
enable_curiosity = true
enable_reward_modulation = true
enable_structural_plasticity = false
enable_development = true
```

### JSON Format
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.15,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "enable_curiosity": true,
  "enable_reward_modulation": true,
  "enable_structural_plasticity": false,
  "enable_development": true
}
```

### INI Format
```ini
[config]
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
enable_curiosity = true
enable_reward_modulation = true
enable_structural_plasticity = false
enable_development = true
```

## Configuration Validation

The configuration system includes validation for critical parameters:

### Required Parameters
- `neuron_count`: Must be > 0 and <= 1,000,000
- `region_count`: Must be > 0 and <= 100

### Strict Validation (enabled with `--strict`)
- `simulation_timestep`: Must be > 0 and <= 1.0
- `connection_probability`: Must be 0.0-1.0

## Command Line Argument Formats

### nlm-cli
```bash
# --key=value format
nlm-cli create --neurons=500 --regions=2

# -key value format
nlm-cli run -steps 100 -output brain_state.bin

# Global options
nlm-cli --quiet --help
```

### config-cli
```bash
# Type-specific setting
config-cli set neuron_count 1000 --type int
config-cli set connection_probability 0.15 --type double
config-cli set enable_curiosity true --type bool
```

## Advanced Features

### Configuration Templates
```bash
# Create a template for common configurations
config-cli template phase3 > configs/phase3_template.cfg

# Use template as base
config-cli merge configs/phase3_template.cfg configs/custom.cfg
```

### Configuration Profiling
```bash
# Profile current configuration and get optimization suggestions
config-cli profile

# Get suggestions for parameter tuning
config-cli profile --strict
```

### Configuration Search
```bash
# Find all keys containing "neuron"
config-cli search neuron

# Find keys containing "curiosity" or "reward"
config-cli search "curiosity|reward"
```

## Troubleshooting

### Common Issues

1. **Configuration validation failed**
   - Check that all required parameters are present
   - Use `--strict` for more detailed validation
   - Check parameter ranges

2. **File not found**
   - Ensure file paths are correct
   - Use absolute paths for better reliability

3. **Permission denied**
   - Check file permissions
   - Use appropriate output directory

4. **Memory issues**
   - Large configurations may require more memory
   - Consider reducing `neuron_count` for testing

### Error Messages

- `[ERROR] Key not found: <key>` - Configuration key doesn't exist
- `[ERROR] Invalid <type> value: <value>` - Value conversion failed
- `[ERROR] Failed to load config from: <file>` - File loading failed
- `[ERROR] Failed to save config to: <file>` - File saving failed

## Integration with Existing NLM

These command-line tools are designed to work with the existing NLM infrastructure:

1. **Python Bindings**: The `pynlm` package provides Python access to NLM
2. **Experiments**: All existing experiments (Phase 3, 4, 6) are accessible via `nlm-cli`
3. **Configuration**: Existing configuration files in `configs/` directory are supported
4. **Backward Compatibility**: Existing C++ executables continue to work as before

## Development

To add new commands or modify existing ones:

1. Add new command handlers in the appropriate source file
2. Update the command registration function
3. Add documentation for the new command
4. Update CMakeLists.txt if new source files are added

For more details on development, see the documentation in the `docs/` directory.
