# Configuration Schema Implementation for NLM

This document describes the improved configuration system for NLM, including the flat key system, validation, and backward compatibility.

## Overview

The NLM configuration system has been improved with:

1. **Flat Key System**: New simplified key notation for easier configuration
2. **Configuration Schema**: Complete schema with validation and documentation
3. **Backward Compatibility**: Support for legacy dot notation
4. **Type Safety**: Improved type checking and validation
5. **Command Line Integration**: Enhanced CLI with flat key support

## Key Improvements

### 1. Flat Key System

**New Simple Keys (Recommended):**
- `neuron_count` instead of `brain.neuron_count`
- `synapse_density` instead of `brain.synapse_density`
- `connection_probability` instead of `brain.connection_probability`

**Legacy Keys (Still Supported):**
- `brain.neuron_count`
- `brain.synapse_density`
- `brain.connection_probability`

**Deprecated Short Keys:**
- `-n` for `--neuron_count`
- `-p` for `--connection_probability`

### 2. Configuration Schema

The configuration system now includes a complete schema with:

- **Type definitions**: Each parameter has a defined type (int, float, bool, string)
- **Default values**: Every parameter has a sensible default
- **Range validation**: Numeric parameters validated against allowed ranges
- **Dependency validation**: Parameters validated against other parameters
- **Documentation**: Detailed documentation for each parameter

### 3. Enhanced CLI Support

The command-line interface now supports:

```bash
# Flat keys (recommended)
nlm --neuron_count=1000 --connection_probability=0.2 --world.width=40

# Legacy keys (for compatibility)
nlm --brain.neuron_count=1000 --brain.connection_probability=0.2

# Short keys (legacy)
nlm -n 1000 -p 0.2
```

### 4. Configuration File Improvements

Configuration files now support:

- **Flat key notation** (primary)
- **Legacy dot notation** (backward compatibility)
- **Comment support** with `#` and `//`
- **Section headers** with `[section]` syntax
- **Type inference** from values

## Configuration Schema Details

### Brain Configuration

| Parameter | Type | Default | Legacy | Description |
|-----------|------|---------|--------|-------------|
| `neuron_count` | int | 1000 | `brain.neuron_count` | Total number of neurons |
| `synapse_density` | float | 0.1 | `brain.synapse_density` | Synapse creation density (0-1) |
| `connection_probability` | float | 0.05 | `brain.connection_probability` | Probability of connection between neurons |
| `initial_weight_mean` | float | 0.5 | (legacy) | Mean initial synaptic weight |
| `initial_weight_std` | float | 0.1 | (legacy) | Standard deviation of initial weights |
| `v_thresh` | float | -50.0 | (legacy) | Neuron threshold potential (mV) |
| `v_rest` | float | -70.0 | (legacy) | Resting potential (mV) |
| `v_reset` | float | -75.0 | (legacy) | Reset potential after spike (mV) |
| `tau_mem` | float | 20.0 | (legacy) | Membrane time constant (ms) |
| `tau_ref` | float | 2.0 | (legacy) | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Legacy | Description |
|-----------|------|---------|--------|-------------|
| `plasticity.stdp.enable` | bool | true | (legacy) | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | (legacy) | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | (legacy) | STDP time constant for potentiation |
| `plasticity.stdp.tau_minus` | float | 20.0 | (legacy) | STDP time constant for depression |
| `plasticity.hebbian.enable` | bool | true | (legacy) | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | (legacy) | Enable structural plasticity |
| `synaptogenesis_rate` | float | 0.0001 | (legacy) | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | (legacy) | Rate of synapse elimination |

### Neuromodulation Configuration

| Parameter | Type | Default | Legacy | Description |
|-----------|------|---------|--------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | (legacy) | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | (legacy) | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | (legacy) | Enable novelty detection |
| `neuromod.prediction_error.enable` | bool | true | (legacy) | Enable prediction error computation |

### Memory Configuration

| Parameter | Type | Default | Legacy | Description |
|-----------|------|---------|--------|-------------|
| `memory.working_capacity` | int | 100 | (legacy) | Working memory capacity (neurons) |
| `memory.episodic.max_episodes` | int | 1000 | (legacy) | Maximum episodic memory episodes |
| `memory.associative.pheromone_decay` | float | 0.1 | (legacy) | Pheromone decay rate (0-1) |

### Development Configuration

| Parameter | Type | Default | Legacy | Description |
|-----------|------|---------|--------|-------------|
| `development.stage` | int | 4 | (legacy) | Number of developmental stages |
| `development.initial_plasticity` | float | 1.0 | (legacy) | Initial plasticity level (0-1) |
| `development.critical_period_start` | int | 100 | (legacy) | Step when critical period begins |
| `development.critical_period_end` | int | 500 | (legacy) | Step when critical period ends |

### Simulation Configuration

| Parameter | Type | Default | Legacy | Description |
|-----------|------|---------|--------|-------------|
| `simulation.timestep` | float | 0.001 | (legacy) | Simulation timestep (seconds) |
| `simulation.max_steps` | int | 10000 | (legacy) | Maximum simulation steps |
| `simulation.replay_interval` | int | 100 | (legacy) | Replay interval (steps) |
| `simulation.consolidation_interval` | int | 1000 | (legacy) | Memory consolidation interval |

### World Configuration

| Parameter | Type | Default | Legacy | Description |
|-----------|------|---------|--------|-------------|
| `world.width` | int | 20 | (legacy) | World width (cells) |
| `world.height` | int | 20 | (legacy) | World height (cells) |
| `world.vision_width` | int | 8 | (legacy) | Agent vision width |
| `world.vision_height` | int | 8 | (legacy) | Agent vision height |
| `world.max_energy` | float | 100.0 | (legacy) | Maximum energy per agent |

## Implementation Details

### Key Normalization

The configuration system automatically normalizes keys:

```cpp
// Legacy key normalization
brain.neuron_count    →   neuron_count
brain.synapse_density →   synapse_density
brain.connection_probability → connection_probability
```

### Configuration File Format

```ini
# Configuration file with mixed key formats

# Flat keys (preferred)
neuron_count = 1000
connection_probability = 0.15
synaptogenesis_rate = 0.001

# Legacy keys (backward compatibility)
brain.neuron_count = 1000
brain.connection_probability = 0.15

# Section headers
[plasticity]
stdp.enable = true
hebbian.enable = true

[neuromod]
dopamine.scale = 1.0
curiosity.enable = true
```

### Command Line Argument Parsing

```bash
# Flat keys
nlm --neuron_count=1000 --connection_probability=0.2

# Legacy keys  
nlm --brain.neuron_count=1000 --brain.connection_probability=0.2

# Short keys (legacy)
nlm -n 1000 -p 0.2
```

### Configuration Validation

The configuration system validates:

1. **Type validation**: Ensures values match expected types
2. **Range validation**: Checks numeric ranges
3. **Dependency validation**: Validates parameter relationships
4. **Conflict detection**: Detects conflicting parameter values

## Configuration Migration Guide

### For Existing Users

If you're using NLM with existing configurations, you can:

1. **Continue using legacy keys** (they still work)
2. **Gradually migrate to flat keys** (recommended for new configurations)
3. **Use both formats simultaneously** in the same configuration

### Migration Examples

```bash
# Legacy way (still supported)
nlm --brain.neuron_count=1000 --brain.connection_probability=0.1

# New way (recommended)
nlm --neuron_count=1000 --connection_probability=0.1
```

### Configuration Templates

Create reusable configuration templates:

```ini
# Template for simple experiments
template.simple = neuron_count=500,connection_probability=0.1,plasticity.stdp.enable=true

# Template for development studies
template.development = neuron_count=1000,development.stage=4,neuromod.curiosity.enable=true

# Template for performance testing
template.performance = neuron_count=10000,connection_probability=0.01,simulation.timestep=0.0001
```

## Configuration File Examples

### Basic Configuration File
```ini
# Basic NLM configuration
neuron_count = 1000
connection_probability = 0.15
synaptogenesis_rate =  manipulaton_rate = 0.0001_00001_development_015_05_plasticity_stdp_enable_true_hebbian_enable_true_neuromod_dopamine_scale_1.0_curiosity_enable_true_novelty_enable_true_memory_working_capacity_100_memory_episodic_max_episodes_1000_memory_associative_pheromone_decay_0.1_development_stage_4_development_initial_plasticity_1.0_development_critical_period_start_100_development_critical_period_end_500_simulation_timestep_0.001_simulation_max_steps_10000_simulation_replay_interval_100_simulation_consolidation_interval_1000_world_width_20_world_height_20_world_vision_width_8_world_vision_height_8_world_max_energy_100.0
```

### Advanced Configuration File
```ini
# Advanced NLM configuration for research

# Brain configuration
neuron_count = 2000
connection_probability = 0.2
synaptogenesis_rate = 0.001
pruning_rate = 0.00001

# Plasticity configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation configuration
neuromod.dopamine.scale = 1.5
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Memory configuration
memory.working_capacity = 200
memory.episodic.max_episodes = 2000
memory.associative.pheromone_decay = 0.2

# Development configuration
development.stage = 5
development.initial_plasticity = 0.8
development.critical_period_start = 100
development.critical_period_end = 500

# Simulation configuration
simulation.timestep = 0.001
simulation.max_steps = 20000
simulation.replay_interval = 200
simulation.consolidation_interval = 2000

# World configuration
world.width = 40
world.height = 40
world.vision_width = 10
world.vision_height = 10
world.max_energy = 200.0
```

## Configuration Error Handling

### Common Errors and Solutions

1. **Key normalization errors**: Use flat keys when possible
2. **Type conversion errors**: Ensure values match expected types
3. **Range validation errors**: Check parameter ranges
4. **Dependency errors**: Validate parameter relationships

### Error Messages

The configuration system provides helpful error messages:

```
Error: Invalid configuration value
Key: neuron_count
Value: -1000
Error: Value must be positive and less than 100,000

Error: Configuration conflict
Key: development.stage
Value: 5
Conflict: critical_period_start must be less than critical_period_end
```

## Configuration Best Practices

### For Users

1. **Use flat keys**: They are easier to read and understand
2. **Validate configurations**: Always validate before running simulations
3. **Document parameters**: Document the meaning and units of each parameter
4. **Use templates**: Create reusable configuration patterns
5. **Backup configurations**: Keep backups of successful configurations

### For Developers

1. **Maintain backward compatibility**: Legacy configurations should still work
2. **Provide migration guides**: Help users transition to new system
3. **Validate early**: Validate configuration at load time
4. **Document everything**: Document the schema and validation rules
5. **Test configurations: Always test configurations before running simulations

## Conclusion

The improved configuration system provides:

- **Better usability**: Flat keys are easier to use and understand
- **Stronger validation**: Prevents configuration errors early
- **Better documentation**: Comprehensive documentation for all parameters
- **Flexible configuration**: Multiple configuration sources and formats
- **Backward compatibility**: Support for legacy configurations
- **Advanced features**: Templates, macros, and inheritance

This configuration system makes it easier to configure NLM experiments for various research scenarios while maintaining ease of use and flexibility.