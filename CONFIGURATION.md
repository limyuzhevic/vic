# Configuration Schema for NLM Brain

## Overview
This document defines the complete configuration schema for the NLM (Neural Learning Machine) brain simulation framework. The configuration system provides typed configuration values with validation, dependency checking, and improved key management.

## Key Structure

### Legacy Dot Notation (Still Supported)
```
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05
```

### New Flat Notation (Recommended)
```
neuron_count = 1000
synapse_density = 0.1
connection_probability = 0.05
```

### Deprecated Notation (For backward compatibility)
```
-n = 1000
-p = 0.1
```

## Configuration Categories

### Brain Configuration
| Key | Type | Default | Description | Example |
|-----|------|---------|-------------|---------|
| `neuron_count` | int | 1000 | Total number of neurons | 500, 2000 |
| `synapse_density` | float | 0.1 | Synapse creation density (0-1) | 0.15, 0.05 |
| `connection_probability` | float | 0.05 | Probability of connection between neurons | 0.1, 0.01 |
| `initial_weight_mean` | float | 0.5 | Mean initial synaptic weight | 0.3, 0.7 |
| `initial_weight_std` | float | 0.1 | Standard deviation of initial weights | 0.05, 0.2 |
| `v_thresh` | float | -50.0 | Neuron threshold potential (mV) | -55.0, -45.0 |
| `v_rest` | float | -70.0 | Resting potential (mV) | -65.0, -75.0 |
| `v_reset` | float | -75.0 | Reset potential after spike (mV) | -80.0, -70.0 |
| `tau_mem` | float | 20.0 | Membrane time constant (ms) | 15.0, 25.0 |
| `tau_ref` | float | 2.0 | Refractory period (ms) | 1.0, 3.0 |

### Plasticity Configuration
| Key | Type | Default | Description | Example |
|-----|------|---------|-------------|---------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity | true, false |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate | 0.002, 0.0005 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for potentiation (ms) | 15.0, 25.0 |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for depression (ms) | 15.0, 25.0 |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | true, false |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | true, false |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation | 0.0005, 0.00001 |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination | 0.00005, 0.000001 |

### Neuromodulation Configuration
| Key | Type | Default | Description | Example |
|-----|------|---------|-------------|---------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | 0.5, 2.0 |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | true, false |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | true, false |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error computation | true, false |

### Memory Configuration
| Key | Type | Default | Description | Example |
|-----|------|---------|-------------|---------|
| `memory.working_capacity` | int | 100 | Working memory capacity (neurons) | 50, 200 |
| `memory.episodic.max_episodes` | int | 1000 | Maximum episodic memory episodes | 500, 2000 |
| `memory.associative.pheromone_decay` | float | 0.1 | Pheromone decay rate (0-1) | 0.05, 0.2 |

### Development Configuration
| Key | Type | Default | Description | Example |
|-----|------|---------|-------------|---------|
| `development.stage` | int | 4 | Number of developmental stages | 3, 5 |
| `development.initial_plasticity` | float | 1.0 | Initial plasticity level (0-1) | 0.8, 0.5 |
| `development.critical_period_start` | int | 100 | Step when critical period begins | 50, 200 |
| `development.critical_period_end` | int | 500 | Step when critical period ends | 300, 800 |

### Simulation Configuration
| Key | Type | Default | Description | Example |
|-----|------|---------|-------------|---------|
| `simulation.timestep` | float | 0.001 | Simulation timestep (seconds) | 0.0005, 0.002 |
| `simulation.max_steps` | int | 10000 | Maximum simulation steps | 5000, 20000 |
| `simulation.replay_interval` | int | 100 | Replay interval (steps) | 50, 200 |
| `simulation.consolidation_interval` | int | 1000 | Memory consolidation interval | 500, 2000 |

### World Configuration
| Key | Type | Default | Description | Example |
|-----|------|---------|-------------|---------|
| `world.width` | int | 20 | World width (cells) | 10, 30 |
| `world.height` | int | 20 | World height (cells) | 10, 30 |
| `world.vision_width` | int | 8 | Agent vision width | 6, 10 |
| `world.vision_height` | int | 8 | Agent vision height | 6, 10 |
| `world.max_energy` | float | 100.0 | Maximum energy per agent | 50.0, 200.0 |

## Configuration File Format

### Basic Example
```ini
# NLM Configuration File
# Generated: 2026-11-19

# Brain Configuration
neuron_count = 1000
connection_probability = 0.15
synaptogenesis_rate = 0.001

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true

# World Configuration
world.width = 30
world.height = 30
world.vision_width = 8
world.vision_height = 8

# Development Configuration
development.stage = 4
development.critical_period_start = 100
development.critical_period_end = 500
```

### Command Line Interface
```bash
# Using flat keys
nlm --neuron_count=2000 --connection_probability=0.2 --world.width=40

# Using legacy keys (for compatibility)
nlm --brain.neuron_count=2000 --brain.connection_probability=0.2

# Using short keys (legacy)
nlm -n 2000 -p 0.2
```

## Configuration Validation

### Type Validation
- All numeric values must be valid numbers
- Boolean values must be true/false
- String values must not contain illegal characters

### Range Validation
```
0 < neuron_count <= 100000
0.0 <= connection_probability <= 1.0
0.0 <= synaptogenesis_rate <= 0.1
0.0 <= pruning_rate <= 0.01
```

### Dependency Validation
- `connection_probability` must be less than `synapse_density` if both set
- `critical_period_start` < `critical_period_end`
- `world.vision_width` <= `world.width`
- `world.vision_height` <= `world.height`

## Configuration Sources (Priority Order)

1. **Command Line Arguments** (highest priority)
2. **Configuration File** (medium priority)
3. **Defaults** (lowest priority)

## Configuration Command Line Examples

### Basic Configuration
```bash
# Minimal configuration
nlm --neuron_count=500

# Full configuration with world
nlm \
  --neuron_count=2000 \
  --connection_probability=0.2 \
  --world.width=40 \
  --world.height=40 \
  --world.vision_width=10 \
  --world.vision_height=10
```

### Development Experiment
```bash
#Developmental configuration for learning studies
nlm \
  --neuron_count=1000 \
  --development.stage=5 \
  --development.critical_period_start=50 \
  --development.critical_period_end=500 \
  --plasticity.stdp.enable=true \
  --plasticity.stdp.learning_rate=0.005 \
  --neuromod.curiosity.enable=true \
  --neuromod.novelty.enable=true
```

### Performance Testing
```bash
# High-performance configuration
nlm \
  --neuron_count=5000 \
  --connection_probability=0.05 \
  --simulation.timestep=0.0005 \
  --simulation.max_steps=5000 \
  --plasticity.stdp.enable=false \
  --plasticity.hebbian.enable=false \
  --neuromod.dopamine.scale=0.5
```

### Checkpoint and Resume
```bash
# Save and restore experiment
nlm --save=checkpoint1.bin --neuron_count=1000 --world.width=20
nlm --load=checkpoint1.bin --continue=true
```

## Advanced Configuration Features

### Configuration Templates
```ini
# Fast template for simple experiments
template.simple = neuron_count=500,connection_probability=0.1,plasticity.stdp.enable=true

# Learning template for developmental studies
template.learning = neuron_count=1000,development.stage=4,neuromod.curiosity.enable=true

# Performance template for speed testing
template.performance = neuron_count=10000,connection_probability=0.01,simulation.timestep=0.0001
```

### Configuration Inheritance
```ini
# Base configuration
base = neuron_count=1000,connection_probability=0.1

# Extended configuration (inherits from base)
extended = base,development.stage=4,world.width=30
```

### Configuration Macros
```ini
# Define common configuration patterns
macro.small_brain = neuron_count=100,connection_probability=0.2
macro.large_brain = neuron_count=10000,connection_probability=0.01
macro.high_plasticity = plasticity.stdp.learning_rate=0.01,plasticity.hebbian.enable=true
```

## Configuration Files in Repository

The NLM project includes example configuration files:

- `configs/default.cfg` - Default configuration
- `configs/development.cfg` - Configuration optimized for development
- `configs/performance.cfg` - High-performance configuration
- `configs/debug.cfg` - Debug configuration with verbose logging

## Configuration File Format Details

### Comments
- `#` for inline comments
- `//` for inline comments (alternative syntax)
- `#` for section headers

### Data Types
```ini
string_values = "hello world"
int_values = 42
double_values = 3.14159
bool_values = true
list_values = [1,2,3,4,5]
```

### Special Values
```ini
# Null/Unset value
null_value = 

# Empty list
empty_list = []

# Scientific notation
time_constant = 1.5e-3
```

## Configuration Best Practices

1. **Keep configurations simple**: Use flat keys when possible
2. **Validate early**: Validate configuration at load time
3. **Document everything**: Document the meaning and units of each parameter
4. **Use templates**: Create reusable configuration patterns
5. **Version configuration**: Track configuration versions for experiments
6. **Backup configurations**: Keep backups of successful configurations

## Configuration Error Handling

### Common Errors
1. **Invalid file path**: Configuration file not found
2. **Syntax errors**: Malformed key=value pairs
3. **Type mismatches**: Setting boolean as string
4. **Validation failures**: Values outside allowed ranges

### Error Recovery
- Graceful fallback to defaults
- Logging of configuration errors
- Partial application of valid configuration
- Clear error messages for debugging

## Configuration Migration Guide

### From Legacy to New System
```bash
# Legacy way (still supported but discouraged)
nlm --brain.neuron_count=1000 --brain.synapse_density=0.1

# New way (recommended)
nlm --neuron_count=1000 --synapse_density=0.1
```

### Configuration Evolution
The NLM configuration system evolves over time. To keep compatibility:

1. **Always use flat keys when possible**
2. **Support legacy dot notation for backward compatibility**
3. **Provide clear migration guides**
4. **Maintain backward compatibility for existing configurations**

## Conclusion

The improved configuration system provides:

- **Better usability**: Flat keys are easier to use and understand
- **Stronger validation**: Prevents configuration errors early
- **Better documentation**: Comprehensive documentation for all parameters
- **Flexible configuration**: Multiple configuration sources and formats
- **Backward compatibility**: Support for legacy configurations
- **Advanced features**: Templates, macros, and inheritance

This configuration system enables users to easily configure NLM experiments for various research scenarios while maintaining ease of use and flexibility.