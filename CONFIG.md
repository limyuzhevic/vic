# NLM Configuration Schema

This document describes all available configuration options for the NLM (Neural Learning Machine) project.

## Overview

NLM uses a flexible configuration system that supports multiple file formats:
- **Simple text format**: `key = value` (backward compatible)
- **JSON format**: `{ "key": value }`
- **YAML format**: `key: value`

Configuration can be loaded from:
1. Configuration files
2. Command-line arguments (`--key=value` or `-key value`)
3. Environment variables
4. Runtime settings

## Configuration Source Priority

The following sources are checked in order of priority (later sources override earlier ones):
1. Command-line arguments
2. Environment variables
3. Configuration files
4. Default values

## Available Configuration Options

### Neural Network Configuration

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `neuron_count` | int64_t | 1000 | Number of neurons in the network | `--neuron_count 5000` |
| `region_count` | int64_t | 1 | Number of neural regions | `--region_count 4` |
| `connection_probability` | float | 0.1 | Probability of synapse formation | `--connection_probability 0.15` |

### Simulation Configuration

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds | `--simulation_timestep 0.0005` |
| `random_seed` | int64_t | 42 | Random seed for reproducible results | `--random_seed 12345` |

### Learning and Plasticity

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight | `--stdp_ltp_weight 0.02` |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight | `--stdp_ltd_weight 0.015` |
| `stdp_tau` | float | 20.0 | STDP time constant in ms | `--stdp_tau 25.0` |

### Structural Plasticity

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `synaptogenesis_rate` | float | 0.0001 | Structural synaptogenesis rate | `--synaptogenesis_rate 0.0002` |
| `pruning_rate` | float | 0.00001 | Structural pruning rate | `--pruning_rate 0.000005` |

### Memory Systems

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `replay_interval` | size_t | 100 | Episodic memory replay interval (steps) | `--replay_interval 50` |
| `consolidation_interval` | size_t | 1000 | Memory consolidation interval (steps) | `--consolidation_interval 2000` |

### Checkpoint and Persistence

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `checkpoint_dir` | string | "./checkpoints" | Directory for checkpoint saves | `--checkpoint_dir ./my_checkpoints` |
| `max_checkpoints` | int | 100 | Maximum number of checkpoints to keep | `--max_checkpoints 50` |
| `enable_checkpointing` | bool | true | Enable checkpoint saving | `--enable_checkpointing false` |
| `enable_replay` | bool | true | Enable episodic memory replay | `--enable_replay false` |
| `enable_development` | bool | true | Enable developmental changes | `--enable_development false` |

### Logging and Output

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `log_level` | string | "info" | Log level: debug, info, warning, error, critical | `--log_level debug` |
| `output_format` | string | "text" | Output format: text, json | `--output_format json` |
| `verbose` | bool | false | Enable verbose logging | `--verbose` |

### Advanced Options

| Option | Type | Default | Description | Example |
|--------|------|---------|-------------|---------|
| `dry_run` | bool | false | Validate configuration without running simulation | `--dry_run` |

## File Format Examples

### Simple Text Format

```
# NLM Configuration File
# Neural network configuration
neuron_count = 2000
region_count = 2
connection_probability = 0.15

# Learning configuration
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Simulation configuration
simulation_timestep = 0.001
random_seed = 12345

# Memory configuration
replay_interval = 200
consolidation_interval = 5000

# Checkpoint configuration
checkpoint_dir = ./checkpoints
enable_checkpointing = true
```

### JSON Format

```json
{
  "nlm_config": {
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "simulation_timestep": 0.001,
    "random_seed": 12345,
    "replay_interval": 200,
    "consolidation_interval": 5000,
    "checkpoint_dir": "./checkpoints",
    "enable_checkpointing": true,
    "log_level": "debug",
    "output_format": "json"
  }
}
```

### YAML Format

```yaml
# NLM Configuration
nlm_config:
  neuron_count: 2000
  region_count: 2
  connection_probability: 0.15
  stdp_ltp_weight: 0.02
  stdp_ltd_weight: 0.015
  simulation_timestep: 0.001
  random_seed: 12345
  replay_interval: 200
  consolidation_interval: 5000
  checkpoint_dir: ./checkpoints
  enable_checkpointing: true
  log_level: debug
  output_format: json
```

## Environment Variables

NLM supports the following environment variables:

| Variable | Description | Example |
|----------|-------------|---------|
| `NLM_CONFIG_FILE` | Path to configuration file | `export NLM_CONFIG_FILE=./my_config.json` |
| `NLM_VERBOSE` | Enable verbose logging (true/false) | `export NLM_VERBOSE=true` |
| `NLM_RANDOM_SEED` | Random seed for reproducible results | `export NLM_RANDOM_SEED=99999` |

Environment variables are overridden by command-line arguments and configuration files.

## Command-Line Usage Examples

### Basic Usage

```bash
# Run with default configuration
./nlm

# Load configuration from file
./nlm --config my_config.json

# Load configuration from YAML file
./nlm --config my_config.yaml

# Load configuration from text file
./nlm --config my_config.cfg
```

### Configuration Override

```bash
# Override configuration options
./nlm --neuron_count 5000 --region_count 4 --verbose

# Run with custom configuration and verbose output
./nlm --config config.json --neuron_count 3000 --log_level debug --output_format json
```

### Dry Run Mode

```bash
# Validate configuration without running simulation
./nlm --dry_run --config test_config.json
```

### Environment Variable Usage

```bash
# Set environment variables
export NLM_CONFIG_FILE=./my_config.json
export NLM_VERBOSE=true
export NLM_RANDOM_SEED=12345

# Run with environment configuration
./nlm
```

## Error Handling

The configuration system provides comprehensive error handling:

1. **File Loading Errors**: Proper error messages for missing or invalid configuration files
2. **Type Validation**: Ensures configuration values have correct types
3. **Range Validation**: Validates numeric ranges for critical parameters
4. **Format Validation**: Validates JSON and YAML syntax
5. **Override Conflicts**: Provides warnings when configuration sources conflict

## Validation

The configuration system validates the following:

1. **Network Consistency**: Ensures neuron_count >= region_count
2. **Parameter Ranges**: Validates that simulation parameters are within reasonable bounds
3. **File Paths**: Validates that checkpoint directories can be created
4. **Format Compatibility**: Ensures JSON/YAML files are properly formatted

## Default Values

When configuration options are not specified, the following defaults are used:

```
neuron_count: 1000
region_count: 1
connection_probability: 0.1
simulation_timestep: 0.001
random_seed: 42
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
stdp_tau: 20.0
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001
replay_interval: 100
consolidation_interval: 1000
checkpoint_dir: ./checkpoints
max_checkpoints: 100
enable_checkpointing: true
enable_replay: true
enable_development: true
log_level: info
output_format: text
verbose: false
dry_run: false
```

## Integration with NLM

The enhanced configuration system integrates with all NLM components:

1. **Brain Initialization**: Configuration is used to initialize neural regions, neurons, and connectivity
2. **Learning Systems**: Plasticity parameters control STDP, Hebbian, and structural learning
3. **Memory Systems**: Configuration controls memory formation, consolidation, and replay
4. **Neuromodulation**: Configuration affects reward prediction, curiosity, and novelty detection
5. **Development**: Configuration controls developmental stages and plasticity modulation
6. **Checkpointing**: Configuration determines persistence and recovery mechanisms

## Migration Guide

### From Phase 2 to Phase 6

The configuration system has evolved from Phase 2 to support comprehensive integration:

**Old Phase 2 Configuration**:
```
neuron_count = 500
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
```

**New Phase 6 Configuration**:
```json
{
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_tau": 20.0,
  "replay_interval": 100,
  "consolidation_interval": 1000,
  "checkpoint_dir": "./checkpoints",
  "enable_checkpointing": true,
  "enable_replay": true,
  "enable_development": true,
  "log_level": "info",
  "output_format": "text"
}
```

## Troubleshooting

### Common Configuration Issues

1. **File Not Found**: Ensure the configuration file path is correct
2. **Invalid JSON/YAML**: Use a JSON validator or YAML linter
3. **Type Mismatch**: Ensure configuration values match the expected type
4. **Permission Denied**: Check that the application has permission to read/write configuration files
5. **Memory Issues**: Increase `neuron_count` and `region_count` if the system is running out of memory

### Configuration Validation Commands

```bash
# Validate JSON configuration
python -m json.tool config.json

# Validate YAML configuration
python -c "import yaml; yaml.safe_load(open('config.yaml'))"

# Test NLM configuration loading
./nlm --config config.json --dry_run
```

## See Also

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [NLM API Reference](https://nlm.readthedocs.io/en/latest/api/)

---

*Last updated: 2026-10-03*
*NLM Configuration System - Phase 6 Integration*