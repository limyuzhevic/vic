# Advanced Configuration System for NLM

## Overview

This document provides comprehensive information about advanced configuration options and commands available in the NLM (Neural Learning Machine) configuration system.

## Advanced Configuration Features

### 1. JSON/YAML Configuration Support

NLM supports both JSON and YAML configuration formats:

#### JSON Configuration
```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 4,
        "connection_probability": 0.15,
        "plasticity": {
            "stdp": {
                "learning_rate": 0.02,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "learning_rate": 0.01
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.0,
                "baseline": 0.1
            },
            "curiosity": {
                "enable": true,
                "threshold": 0.5
            }
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "time_limit": 60.0
    },
    "environment": {
        "type": "SimpleWorld",
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    },
    "logging": {
        "level": "INFO",
        "to_file": true,
        "filename": "nlm_simulation.log",
        "max_size": 1048576
    },
    "checkpointing": {
        "enable": true,
        "path": "/tmp/nlm_checkpoint.bin",
        "interval": 1000
    }
}
```

#### YAML Configuration
```yaml
brain:
  neuron_count: 5000
  region_count: 4
  connection_probability: 0.15
  plasticity:
    stdp:
      learning_rate: 0.02
      tau_plus: 20.0
      tau_minus: 20.0
    hebbian:
      learning_rate: 0.01
  neuromodulation:
    dopamine:
      scale: 1.0
      baseline: 0.1
    curiosity:
      enable: true
      threshold: 0.5

simulation:
  timestep: 0.001
  max_steps: 10000
  time_limit: 60.0

environment:
  type: SimpleWorld
  width: 20
  height: 20
  vision_width: 8
  vision_height: 8

logging:
  level: INFO
  to_file: true
  filename: nlm_simulation.log
  max_size: 1048576

checkpointing:
  enable: true
  path: /tmp/nlm_checkpoint.bin
  interval: 1000
```

### 2. Configuration Commands

#### Configuration Management Commands

The NLM configuration system provides several command-line options for advanced configuration management:

##### View Configuration
```bash
# View current configuration
./nlm_phase6_demo --show_config

# View configuration in JSON format
./nlm_phase6_demo --show_config --format json

# View configuration in YAML format
./nlm_phase6_demo --show_config --format yaml
```

##### Merge Configuration
```bash
# Merge configuration files
./nlm_phase6_demo --merge_config config1.json config2.json > merged_config.json

# Merge command line arguments with file
./nlm_phase6_demo --config default.cfg --merge_args --neuron_count=10000
```

##### Validate Configuration
```bash
# Validate configuration file
./nlm_phase6_demo --validate_config config.json

# Validate all configurations
./nlm_phase6_demo --validate_all

# Get validation report
./nlm_phase6_demo --validate_config config.json --report
```

##### Export Configuration
```bash
# Export current configuration to file
./nlm_phase6_demo --export_config current_config.json

# Export with specific format
./nlm_phase6_demo --export_config experiment_config.yaml --format yaml
```

##### Import Configuration
```bash
# Import configuration from file
./nlm_phase6_demo --import_config new_config.json

# Import and merge with current
./nlm_phase6_demo --import_config new_config.json --merge
```

##### Configuration Templates
```bash
# Create configuration template
./nlm_phase6_demo --create_template brain_template.cfg

# List available templates
./nlm_phase6_demo --list_templates

# Apply template
./nlm_phase6_demo --apply_template brain_template.cfg
```

### 3. Advanced Configuration Options

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 2 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Connection probability |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (LTP) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (LTD) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.01 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.0 | Baseline dopamine level |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.threshold` | float | 0.5 | Novelty threshold for curiosity |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error signals |

#### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage` | string | "adult" | Developmental stage (initial, critical_period, maturation, adult, aging) |
| `development.plasticity_rate` | float | 1.0 | Plasticity rate multiplier |
| `development.synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `development.pruning_rate` | float | 0.0001 | Rate of synapse elimination |

#### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_capacity` | int | 100 | Working memory capacity |
| `memory.episodic_capacity` | int | 1000 | Episodic memory capacity |
| `memory.replay_interval` | int | 100 | Memory replay interval |
| `memory.consolidation_threshold` | float | 0.8 | Memory consolidation threshold |

#### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.type` | string | "SimpleWorld" | Environment type |
| `environment.width` | int | 10 | Environment width |
| `environment.height` | int | 10 | Environment height |
| `environment.vision_width` | int | 8 | Vision width |
| `environment.vision_height` | int | 8 | Vision height |

#### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep |
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Simulation time limit (seconds) |
| `simulation.random_seed` | int | 42 | Random seed for reproducibility |

#### Logging Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `logging.level` | string | "INFO" | Log level (DEBUG, INFO, WARNING, ERROR) |
| `logging.to_file` | bool | false | Write logs to file |
| `logging.filename` | string | "nlm.log" | Log file name |
| `logging.max_size` | int | 1048576 | Maximum log file size |
| `logging.rotate_count` | int | 5 | Number of log files to keep |

#### Checkpoint Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `checkpointing.enable` | bool | false | Enable checkpointing |
| `checkpointing.path` | string | "nlm_checkpoint.bin" | Checkpoint file path |
| `checkpointing.interval` | int | 1000 | Checkpoint save interval |
| `checkpointing.compress` | bool | true | Compress checkpoint files |

### 4. Configuration Validation

#### Validation Rules

1. **Parameter Bounds**: All parameters must be within valid ranges
2. **Type Consistency**: Parameter types must match expected types
3. **Format Validation**: Configuration files must be valid JSON/YAML
4. **Value Validation**: Some parameters have specific constraints

#### Validation Commands
```bash
# Validate configuration file
./nlm_phase6_demo --validate_config config.json

# Get detailed validation report
./nlm_phase6_demo --validate_config config.json --detailed

# Validate all current configurations
./nlm_phase6_demo --validate_all --detailed

# Fix common configuration issues
./nlm_phase6_demo --fix_config config.json > fixed_config.json
```

### 5. Configuration Templates

#### Available Templates

1. **Minimal Configuration** (`minimal.cfg`)
   - Just enough for basic simulation
   - Few neurons, fast execution

2. **Standard Configuration** (`standard.cfg`)
   - Balanced settings for typical use
   - Good learning capabilities

3. **High-Performance Configuration** (`high_perf.cfg`)
   - Optimized for speed
   - More neurons, higher resolution

4. **Research Configuration** (`research.cfg`)
   - Advanced settings for research
   - Custom plasticity, neuromodulation

#### Creating Custom Templates
```bash
# Create a custom template
./nlm_phase6_demo --create_template my_template.cfg \
    --neuron_count=2000 \
    --plasticity.stdp.learning_rate=0.05 \
    --neuromod.curiosity.enable=true \
    --environment.width=30 \
    --environment.height=30

# Apply template to simulation
./nlm_phase6_demo my_template.cfg ./nlm_phase6_demo
```

### 6. Configuration Migration

#### Migrate from Legacy Format
```bash
# Convert legacy configs/default.cfg to JSON
./nlm_phase6_demo --migrate_config configs/default.cfg > legacy_converted.json

# Convert all legacy configurations
./nlm_phase6_demo --migrate_legacy all_configs/ > migrated_all.json
```

#### Migrate from JSON to Legacy Format
```bash
# Convert JSON to legacy format
./nlm_phase6_demo --convert_legacy config.json > converted_legacy.cfg
```

### 7. Advanced Usage Examples

#### Example 1: Complex Learning Environment
```bash
# Create advanced configuration for learning
./nlm_phase6_demo --create_config learning_config.json \
    --brain.neuron_count=5000 \
    --brain.region_count=4 \
    --plasticity.stdp.enable=true \
    --plasticity.stdp.learning_rate=0.02 \
    --plasticity.hebbian.enable=true \
    --neuromod.dopamine.scale=1.5 \
    --neuromod.curiosity.enable=true \
    --neuromod.curiosity.threshold=0.3 \
    --environment.type=SimpleWorld \
    --environment.width=30 \
    --environment.height=30 \
    --environment.vision_width=12 \
    --environment.vision_height=12 \
    --simulation.max_steps=50000 \
    --simulation.time_limit=300.0 \
    --logging.level=INFO \
    --logging.to_file=true \
    --checkpointing.enable=true \
    --checkpointing.interval=5000
```

#### Example 2: Real-time Configuration Management
```bash
#!/bin/bash

# Configuration management script

CONFIG_FILE="simulation_config.json"
BACKUP_DIR="backups"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

# Create backup
mkdir -p "$BACKUP_DIR"
cp "$CONFIG_FILE" "$BACKUP_DIR/config_${TIMESTAMP}.json"

# Validate configuration
./nlm_phase6_demo --validate_config "$CONFIG_FILE"

# Update configuration dynamically
./nlm_phase6_demo --import_config "$CONFIG_FILE"
./nlm_phase6_demo --set brain.neuron_count 8000
./nlm_phase6_demo --set simulation.max_steps 20000

# Save updated configuration
./nlm_phase6_demo --export_config "$CONFIG_FILE"

# Run simulation with updated configuration
./nlm_phase6_demo
```

#### Example 3: Configuration for Research Experiments
```bash
# Research-grade configuration
./nlm_phase6_demo --create_config research_experiment.json \
    --brain.neuron_count=10000 \
    --brain.region_count=8 \
    --connection_probability=0.12 \
    --plasticity.stdp.enable=true \
    --plasticity.stdp.learning_rate=0.025 \
    --plasticity.hebbian.enable=true \
    --plasticity.structural.enable=true \
    --neuromod.dopamine.scale=1.0 \
    --neuromod.dopamine.baseline=0.2 \
    --neuromod.curiosity.enable=true \
    --neuromod.curiosity.threshold=0.4 \
    --neuromod.novelty.enable=true \
    --neuromod.prediction_error.enable=true \
    --development.stage=maturation \
    --development.plasticity_rate=1.2 \
    --memory.working_capacity=200 \
    --memory.episodic_capacity=5000 \
    --environment.type=SimpleWorld \
    --environment.width=50 \
    --environment.height=50 \
    --environment.vision_width=16 \
    --environment.vision_height=16 \
    --simulation.timestep=0.0005 \
    --simulation.max_steps=100000 \
    --simulation.time_limit=600.0 \
    --logging.level=DEBUG \
    --logging.to_file=true \
    --logging.filename=research_simulation.log \
    --checkpointing.enable=true \
    --checkpointing.path=/research/checkpoints/experiment_1.bin \
    --checkpointing.interval=10000
```

## Configuration File Formats

### JSON Configuration File
```json
{
    "version": "2.0",
    "description": "NLM Configuration",
    "author": "Researcher",
    "created": "2024-01-16T00:00:00Z",
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000
    }
}
```

### YAML Configuration File
```yaml
---
version: '2.0'
description: NLM Configuration
author: Researcher
created: 2024-01-16T00:00:00Z
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
simulation:
  timestep: 0.001
  max_steps: 10000
```

## Error Handling and Recovery

### Common Configuration Errors

1. **Invalid Key Format**: Keys must contain only alphanumeric characters, underscores, dots, or hyphens
2. **Missing Required Parameters**: Some parameters are required for proper operation
3. **Invalid Value Ranges**: Parameters must be within valid ranges
4. **File Format Errors**: Configuration files must be valid JSON or YAML

### Error Recovery

```bash
# Fix common configuration issues
./nlm_phase6_demo --fix_config broken_config.json > fixed_config.json

# Validate after fixing
./nlm_phase6_demo --validate_config fixed_config.json
```

## Performance Considerations

### Large Configurations

For configurations with many neurons or complex settings:

1. **Memory Usage**: Monitor memory usage with large neuron counts
2. **Simulation Time**: Increase simulation timestep for faster execution
3. **Checkpointing**: Enable checkpointing for long simulations
4. **Logging**: Use appropriate log levels to reduce overhead

### Recommended Configuration Patterns

#### For Development and Testing
```json
{
    "brain": { "neuron_count": 100 },
    "simulation": { "max_steps": 1000 },
    "logging": { "level": "DEBUG" }
}
```

#### For Production
```json
{
    "brain": { "neuron_count": 5000 },
    "simulation": { "max_steps": 10000 },
    "logging": { "level": "INFO", "to_file": true },
    "checkpointing": { "enable": true }
}
```

#### For Research
```json
{
    "brain": { "neuron_count": 10000 },
    "simulation": { "max_steps": 50000, "time_limit": 300.0 },
    "logging": { "level": "DEBUG", "to_file": true },
    "checkpointing": { "enable": true, "interval": 5000 }
}
```

## Configuration Summary

The advanced configuration system in NLM provides:

1. **Multiple Formats**: JSON and YAML support
2. **Validation**: Comprehensive parameter validation
3. **Templates**: Pre-defined configuration templates
4. **Commands**: Advanced configuration management commands
5. **Migration**: Easy migration from legacy formats
6. **Error Handling**: Robust error detection and recovery
7. **Performance**: Optimized for different use cases
8. **Extensibility**: Easy to add new parameters and features

This system enables users to create, manage, and validate complex configurations for advanced NLM simulations while maintaining simplicity for basic use cases.