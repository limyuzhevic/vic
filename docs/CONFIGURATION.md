# NLM Configuration Documentation

## Overview

This document describes all configuration parameters available for the Neural Learning Machine (NLM). Configuration parameters control various aspects of the neural simulation, including network topology, learning dynamics, neuromodulation, and simulation settings.

## Configuration File Format

NLM uses a simple key=value format for configuration files:

```
# Parameter description
parameter_name = value

# Another comment
another_param = "string_value"
```

- Parameters with `[type]` suffix in descriptions are data types
- String values should be quoted with double quotes
- Array values are represented as `[value1, value2, value3]`
- Comments start with `#` or `/`
- Empty lines are ignored

## Configuration Parameters

### Core Network Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int64 | 1000 | Number of neurons in the brain |
| `region_count` | int64 | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of connection between neurons (0.0-1.0) |
| `simulation_timestep` | double | 0.001 | Time step size in seconds |
| `max_simulation_steps` | int64 | 10000 | Maximum number of simulation steps |
| `simulation_time_limit` | double | 0.0 | Time limit in seconds (0.0 = unlimited) |

### Neuron Dynamics Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `lif_resting_potential` | double | -70.0 | Resting membrane potential in mV |
| `lif_threshold` | double | -55.0 | Spike threshold in mV |
| `lif_reset_potential` | double | -65.0 | Membrane potential after spike in mV |
| `lif_refractory_period` | double | 0.002 | Refractory period in seconds |
| `lif_leak_coefficient` | double | 0.1 | Membrane leak coefficient |
| `neuron_excitability` | float | 1.0 | Base excitability factor |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity_learning_rate` | float | 0.01 | Base learning rate |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight factor |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight factor |
| `stdp_time_constant` | float | 20.0 | STDP time constant in milliseconds |
| `stdp_trace_decay` | float | 0.01 | STDP trace decay rate |
| `hebbian_learning_rate` | float | 0.02 | Hebbian learning rate |
| `synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.0001 | Rate of synapse elimination |
| `structural_plasticity_enabled` | bool | true | Enable structural plasticity |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |
| `dopamine_sensitivity` | float | 1.0 | Dopamine sensitivity factor |
| `novelty_threshold` | float | 0.5 | Novelty detection threshold |
| `curiosity_strength` | float | 0.5 | Curiosity motivation strength |
| `prediction_error_lambda` | float | 0.1 | Prediction error scaling factor |
| `reward_discount_factor` | float | 0.99 | Reward discounting factor |

### Memory System Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | int64 | 100 | Maximum number of working memory items |
| `working_memory_decay` | float | 0.01 | Working memory decay rate |
| `episodic_memory_capacity` | int64 | 1000 | Maximum episodic memory capacity |
| `episodic_memory_decay` | float | 0.001 | Episodic memory decay rate |
| `associative_memory_strength` | float | 1.0 | Associative memory connection strength |

### Sensory Processing Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `vision_enabled` | bool | true | Enable vision processing |
| `vision_width` | int64 | 8 | Vision field width in neurons |
| `vision_height` | int64 | 8 | Vision field height in neurons |
| `vision_sensitivity` | float | 1.0 | Vision sensitivity factor |
| `audio_enabled` | bool | true | Enable audio processing |
| `audio_bins` | int64 | 8 | Number of frequency bins |
| `internal_signals_enabled` | bool | true | Enable internal signal processing |

### Motor System Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `motor_neurons_count` | int64 | 20 | Number of motor neurons |
| `action_selection_tau` | float | 0.1 | Action selection temperature |
| `motor_noise_level` | float | 0.1 | Motor output noise level |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_enabled` | bool | true | Enable developmental processes |
| `developmental_stage` | int64 | 1 | Initial developmental stage |
| `maturation_rate` | float | 1.0 | Maturation speed factor |
| `pruning_enabled` | bool | true | Enable synaptic pruning |
| `synaptogenesis_enabled` | bool | true | Enable synaptogenesis |

### Prediction System Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction_enabled` | bool | true | Enable prediction system |
| `prediction_horizon` | int64 | 10 | Prediction horizon in steps |
| `prediction_accuracy` | float | 0.8 | Target prediction accuracy |
| `prediction_error_weight` | float | 1.0 | Prediction error weighting |

### Cognitive Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `attention_enabled` | bool | true | Enable attentional selection |
| `attention_strength` | float | 1.0 | Attention modulation strength |
| `concept_formation_enabled` | bool | true | Enable concept formation |
| `neural_planner_enabled` | bool | true | Enable neural planning |
| `planner_exploration` | float | 0.1 | Planner exploration rate |

### Environment Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment_name` | string | "GridWorld" | Name of the environment |
| `environment_width` | int64 | 10 | World width |
| `environment_height` | int64 | 10 | World height |
| `world_boundary_enabled` | bool | true | Enable world boundaries |

### Visualization Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization_enabled` | bool | false | Enable visualization |
| `visualization_update_rate` | float | 30.0 | Visualization update rate (Hz) |
| `visualization_neurons` | bool | true | Show neuron visualization |
| `visualization_synapses` | bool | true | Show synapse visualization |
| `visualization_spikes` | bool | true | Show spike visualization |

### Logging Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `log_to_file` | bool | false | Enable file logging |
| `log_filename` | string | "nlm.log" | Log file name |
| `log_console_enabled` | bool | true | Enable console logging |

## Example Configuration Files

### Minimal Configuration

```
# Minimal NLM configuration
random_seed = 42
neuron_count = 100
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
```

### Advanced Configuration

```
# Advanced NLM configuration with all systems enabled
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.15

# Plasticity settings
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 25.0

# Neuromodulation
dopamine_baseline = 0.2
dopamine_sensitivity = 1.5
novelty_threshold = 0.3
curiosity_strength = 0.8

# Memory systems
working_memory_capacity = 200
episodic_memory_capacity = 2000

# Development
development_enabled = true
maturation_rate = 1.2
pruning_enabled = true

# Sensory processing
vision_enabled = true
vision_width = 16
vision_height = 16
audio_enabled = true

# Logging
log_level = DEBUG
log_to_file = true
log_filename = nlm_advanced.log
```

## Command Line Configuration

Command line arguments can override configuration file settings:

### Basic Override

```bash
./nlm --neuron_count=200 --connection_probability=0.2 --config=my_config.cfg
```

### Short Form Arguments

```bash
./nlm -n 300 -c 0.25
```

### Common Command Line Options

| Option | Description |
|--------|-------------|
| `--config=<file>` | Load configuration from file |
| `--neuron_count=<n>` | Set number of neurons |
| `--region_count=<n>` | Set number of regions |
| `--connection_probability=<p>` | Set connection probability |
| `--simulation_timestep=<t>` | Set time step size |
| `--random_seed=<s>` | Set random seed for reproducibility |
| `--log_level=<level>` | Set logging level |

## Configuration Validation

The configuration system validates:

1. **Parameter Types**: Ensures values match expected types
2. **Range Checking**: Validates values are within acceptable ranges
3. **Uniqueness**: Prevents duplicate parameter keys
4. **File Format**: Validates configuration file syntax
5. **Source Priority**: Command line arguments override file settings, which override defaults

## Error Handling

Configuration errors are reported with:

- Clear error messages indicating the problem
- File and line number for file-based errors
- Suggestion of valid alternatives where possible
- Graceful fallback to default values for critical parameters

## Best Practices

1. **Version Control**: Configuration files should be tracked in version control
2. **Parameter Comments**: Use comments to document parameter choices
3. **Testing**: Test configuration combinations thoroughly
4. **Documentation**: Keep documentation updated with new parameters
5. **Validation**: Implement application-specific validation beyond basic type checking

## Additional Resources

- `easy_usage.md`: Beginner-friendly usage guide
- `docs/ARCHITECTURE.md`: System architecture documentation
- `HOW_TO_USE.md`: Detailed usage instructions

## Configuration Command Line Reference

| Command | Description |
|---------|-------------|
| `--help` | Show help message |
| `--version` | Show version information |
| `--config=<file>` | Load configuration from file |
| `--validate-only` | Validate configuration without running |
| `--list-params` | List all available configuration parameters |
| `--show-config` | Show current configuration with descriptions |
