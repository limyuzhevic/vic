# NLM Configuration Schema

This document describes the configuration schema used throughout NLM, providing comprehensive documentation for all configuration parameters.

## Configuration System Overview

NLM uses a unified configuration system that supports:
- File-based configuration (key=value format)
- Command-line argument overrides
- Default values for all parameters
- Type safety and validation
- Environment variable support (future)

## Core Configuration Parameters

### Simulation Core

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `random_seed` | int64 | 42 | Seed for random number generation | [0, 2^63-1] |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds | (0, 1.0] |
| `neuron_count` | int64 | 500 | Total number of neurons | [1, 1000000] |
| `region_count` | int64 | 1 | Number of brain regions | [1, 100] |
| `connection_probability` | float | 0.15 | Probability of connection formation | [0.0, 1.0] |

### Neuron Dynamics

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `neuron_threshold` | float | -55.0 | Membrane potential threshold for spiking | [-100, 0] |
| `neuron_resting_potential` | float | -70.0 | Resting membrane potential | [-85, -40] |
| `neuron_reset_potential` | float | -65.0 | Membrane potential after spike | [-80, -50] |
| `refractory_period` | uint32 | 5 | Refractory period in timesteps | [1, 100] |

### Synaptic Plasticity

#### STDP (Spike-Timing-Dependent Plasticity)

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `stdp_ltp_weight` | float | 0.02 | Long-Term Potentiation weight | [0.001, 0.1] |
| `stdp_ltd_weight` | float | 0.015 | Long-Term Depression weight | [0.001, 0.1] |
| `stdp_tau` | float | 20.0 | STDP timescale in ms | [1, 100] |

#### Structural Plasticity

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation | [0.0, 0.01] |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination | [0.0, 0.001] |

#### Hebbian Plasticity

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `hebbian_learning_rate` | float | 0.01 | Hebbian learning rate | [0.001, 0.1] |

### Neuromodulation

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `dopamine_sensitivity` | float | 1.0 | Dopamine effect on plasticity | [0.0, 2.0] |
| `curiosity_sensitivity` | float | 0.8 | Curiosity effect on learning | [0.0, 2.0] |
| `novelty_threshold` | float | 0.1 | Novelty detection threshold | [0.0, 1.0] |
| `novelty_decay` | float | 0.95 | Novelty signal decay rate | [0.0, 1.0] |

### Memory Systems

#### Working Memory

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `working_memory_capacity` | uint32 | 50 | Number of items in working memory | [1, 1000] |
| `working_memory_decay` | float | 0.9 | Decay rate of working memory | [0.0, 1.0] |

#### Episodic Memory

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `episodic_memory_capacity` | uint32 | 1000 | Maximum number of episodes | [1, 10000] |
| `episodic_memory_retention` | float | 0.9 | Retention probability per timestep | [0.0, 1.0] |

#### Concept Formation

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `concept_formation_threshold` | float | 0.7 | Threshold for concept formation | [0.0, 1.0] |
| `concept_retention` | float | 0.8 | Concept retention rate | [0.0, 1.0] |

### Cognitive Functions

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `prediction_horizon` | uint32 | 10 | Number of steps for prediction | [1, 100] |
| `prediction_confidence` | float | 0.8 | Confidence threshold for predictions | [0.0, 1.0] |
| `attention_span` | uint32 | 5 | Duration of attentional focus | [1, 20] |
| `planning_horizon` | uint32 | 20 | Number of steps for planning | [1, 100] |
| `planning_confidence` | float | 0.7 | Confidence threshold for plans | [0.0, 1.0] |

### Development

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `enable_development` | bool | true | Enable developmental processes | [true, false] |
| `development_duration` | double | 60.0 | Total developmental period in seconds | [0, 1000] |
| `development_plasticity_factor` | float | 0.5 | Multiplier for developmental plasticity | [0.0, 2.0] |
| `critical_period_end` | double | 120.0 | End of critical period in seconds | [0, 1000] |

### Replay and Consolidation

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `enable_replay` | bool | true | Enable memory replay during sleep | [true, false] |
| `replay_frequency` | float | 0.1 | Frequency of replay events | [0.0, 1.0] |
| `replay_strength` | float | 0.8 | Strength of replay effects | [0.0, 1.0] |
| `enable_consolidation` | bool | true | Enable memory consolidation | [true, false] |
| `consolidation_threshold` | float | 0.7 | Threshold for consolidation | [0.0, 1.0] |

### Performance and Optimization

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `enable_checkpointing` | bool | true | Enable checkpoint saving | [true, false] |
| `checkpoint_interval` | uint32 | 1000 | Steps between checkpoints | [1, 10000] |
| `enable_sparse_connectivity` | bool | false | Use sparse connectivity | [true, false] |
| `connection_threshold` | float | 0.05 | Threshold for connection formation | [0.0, 1.0] |
| `enable_simd` | bool | true | Use SIMD optimizations | [true, false] |
| `enable_parallel` | bool | true | Use parallel processing | [true, false] |

### Logging

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `log_level` | string | INFO | Logging level (DEBUG, INFO, WARNING, ERROR) | [DEBUG, INFO, WARNING, ERROR] |
| `log_file` | string | (empty) | File path for log output | (any valid path) |
| `metrics_output` | string | (empty) | File path for metrics output | (any valid path) |
| `performance_monitoring` | bool | false | Enable performance monitoring | [true, false] |

## Configuration Validation Rules

### Type Consistency
- All numeric parameters must be of the correct type
- String parameters must be properly quoted if they contain spaces

### Range Validation
- Parameters outside valid ranges trigger warnings but don't fail
- Extreme values are clamped to reasonable ranges

### Dependency Validation
- `enable_development = true` requires `development_duration > 0`
- `enable_prediction = true` requires `prediction_horizon > 0`
- `enable_attention = true` requires `attention_span > 0`
- `enable_planning = true` requires `planning_horizon > 0`

### Resource Constraints
- `neuron_count` must be consistent with available memory
- `region_count` must be compatible with parallel processing settings
- `connection_probability` affects simulation runtime

## Configuration File Format

### Basic Syntax
```
# This is a comment
key = value
another_key = "value with spaces"
third_key = 42
fourth_key = true
```

### Special Characters
- Escape quotes in strings: `"escaped quote"`
- Use scientific notation: `1.23e-4`
- Support for comments: `# comment` or `// comment`

### Environment Variable Expansion
```
# Environment variables are expanded
some_path = ${HOME}/.nlm
seed = ${RANDOM_SEED:-42}
```

## Command Line Override

Command-line arguments override configuration file values:

```bash
./nlm run --config phase2.cfg --neuron_count 1000 --log_level DEBUG
```

Available override options:
- `--key value` : Override configuration parameter
- `--key=value` : Override configuration parameter (shorthand)
- `--config file` : Load configuration file
- `--help` : Show help message
- `--version` : Show version information

## Configuration Examples

### Minimal Configuration
```
random_seed = 42
neuron_count = 100
region_count = 1
```

### Full Configuration
```
# Core settings
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3

# All features enabled
enable_development = true
enable_prediction = true
enable_attention = true
enable_planning = true
enable_replay = true

# Performance settings
enable_checkpointing = true
enable_sparse_connectivity = true
```

## Configuration Migration Guide

When upgrading from older versions:

### Version 0.1.0 to 0.2.0
- New parameters: `prediction_horizon`, `attention_span`, `planning_horizon`
- Changed parameter names: `hebbian_learning_rate` -> `hebbian_weight`
- New validation rules added for developmental processes

### Configuration Migration Script
A Python script can migrate old configurations to new format:

```python
#!/usr/bin/env python3
import json
import re

def migrate_config(old_config_path, new_config_path):
    # Load old config
    with open(old_config_path, 'r') as f:
        content = f.read()
    
    # Parse old format
    old_config = parse_old_format(content)
    
    # Apply migration rules
    new_config = migrate_to_new_format(old_config)
    
    # Write new config
    with open(new_config_path, 'w') as f:
        f.write(format_new_config(new_config))

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: migrate_config.py <old_config> <new_config>")
        sys.exit(1)
    migrate_config(sys.argv[1], sys.argv[2])
```
