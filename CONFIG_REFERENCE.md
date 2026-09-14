# NLM Configuration Reference

This document provides a complete reference for all NLM configuration parameters.

## Configuration Overview

NLM uses a hierarchical configuration system organized into sections:
- Brain: Core brain parameters
- Plasticity: Learning and synaptic modification
- Neuromodulation: Reward and motivation systems
- Development: Growth and maturation parameters
- Environment: Simulation world settings
- Logging: Output and debugging settings
- Simulation: Runtime behavior
- Visualization: Rendering and display options

## Configuration File Format

Configuration files use a simple key-value format:
```
# Comment
parameter_name = value
```

## Detailed Parameters

### Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons in the brain |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of connection between neurons |
| `plasticity_learning_rate` | float | 0.01 | Base learning rate for plasticity |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | Long-Term Potentiation weight change |
| `stdp_ltd_weight` | float | 0.012 | Long-Term Depression weight change |
| `stdp_time_constant` | float | 20.0 | STDP time constant (ms) |
| `development_synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `development_pruning_rate` | float | 0.0001 | Rate of synapse elimination |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level (was duplicate in default.cfg) |
| `reward_discount_factor` | float | 0.99 | Discount factor for reward prediction error |

### Environment Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment_name` | string | GridWorld | Environment type to use |
| `environment_width` | int | 10 | Width of the environment grid |
| `environment_height` | int | 10 | Height of the environment grid |

### Logging Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log_level` | string | INFO | Logging verbosity level |
| `log_to_file` | bool | false | Write logs to file |
| `log_filename` | string | nlm.log | Log file name when logging to file |

### Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `max_simulation_steps` | int | 10000 | Maximum number of simulation steps |
| `simulation_time_limit` | float | 0.0 | Absolute time limit for simulation (0 = unlimited) |
| `simulation_timestep` | float | 0.001 | Time step between simulation updates |
| `random_seed` | int | 42 | Random seed for reproducible results |

### Visualization Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization_enabled` | bool | false | Enable visualization output |
| `visualization_update_rate` | float | 30.0 | Visualization update frequency (Hz) |

## Configuration Loading

### Python API

The NLM Python bindings provide a convenient way to work with configurations:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Access configuration values
neuron_count = config.get("brain.neuron_count")
config.set("brain.neuron_count", 2000)

# Check if parameter exists
if config.has("neuromod.dopamine_baseline"):
    value = config.get("neuromod.dopamine_baseline")

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### Available Configuration Sections

```
Brain:
  - neuron_count
  - region_count
  - connection_probability

Plasticity:
  - plasticity_learning_rate
  - stdp_ltp_weight
  - stdp_ltd_weight
  - stdp_time_constant
  - development_synaptogenesis_rate
  - development_pruning_rate

Neuromodulation:
  - dopamine_baseline
  - reward_discount_factor

Environment:
  - environment_name
  - environment_width
  - environment_height

Logging:
  - log_level
  - log_to_file
  - log_filename

Simulation:
  - max_simulation_steps
  - simulation_time_limit
  - simulation_timestep
  - random_seed

Visualization:
  - visualization_enabled
  - visualization_update_rate
```

## Configuration Validation

### Parameter Types

- **Integer**: Parameters expecting integer values
- **Float**: Parameters expecting floating-point values
- **String**: Parameters expecting string values
- **Boolean**: Parameters accepting true/false values

### Valid Ranges

| Parameter | Valid Range |
|-----------|-------------|
| `neuron_count` | 1-100000 |
| `region_count` | 1-100 |
| `connection_probability` | 0.0-1.0 |
| `plasticity_learning_rate` | 0.0-1.0 |
| `stdp_ltp_weight` | -1.0-1.0 |
| `stdp_ltd_weight` | -1.0-1.0 |
| `stdp_time_constant` | 1.0-100.0 |
| `development_synaptogenesis_rate` | 0.0-1.0 |
| `development_pruning_rate` | 0.0-1.0 |
| `dopamine_baseline` | 0.0-1.0 |
| `reward_discount_factor` | 0.0-1.0 |
| `environment_width` | 1-1000 |
| `environment_height` | 1-1000 |

## Best Practices

### Configuration Management

1. **Use defaults for new simulations**: Start with default.cfg and modify only what's necessary
2. **Save configurations**: Save your working configuration for reproducibility
3. **Parameter validation**: Always check parameter values are within valid ranges
4. **Documentation**: Document configuration changes for your experiment

### Common Configuration Scenarios

#### High-Frequency Trading Brain
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 50000)
config.set("plasticity_learning_rate", 0.001)
config.set("stdp_time_constant", 5.0)
config.set("log_level", "DEBUG")
```

#### Developmental Learning Brain
```python
config = pynlm.createDefaultConfig()
config.set("development_synaptogenesis_rate", 0.01)
config.set("development_pruning_rate", 0.001)
config.set("dopamine_baseline", 0.5)
config.set("visualization_enabled", True)
```

#### Simple Learning Brain
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("plasticity_learning_rate", 0.05)
config.set("log_to_file", True)
config.set("log_filename", "simple_brain.log")
```

## Configuration Changes

### From Default to Custom

When modifying default configuration, remember that:
- Changing `neuron_count` affects brain size and simulation performance
- Increasing plasticity rates leads to faster learning but potential instability
- Neuromodulation parameters influence exploration vs. exploitation behavior
- Development parameters control brain maturation and lifespan

### Migration Guide

If upgrading from an older configuration format:
1. New parameter names follow dot notation (e.g., `brain.neuron_count`)
2. Some deprecated parameters have been consolidated
3. New validation rules apply to ensure simulation stability

## Troubleshooting

### Common Issues

**Issue**: Configuration values not applying correctly
**Solution**: Verify parameter paths are correct and values are of the right type

**Issue**: Simulation crashes due to invalid configuration
**Solution**: Check parameter ranges and ensure no conflicting values

**Issue**: Configuration not saving/loading
**Solution**: Verify file permissions and valid JSON/CFG format

## See Also

- [README.md](README.md) - High-level project overview
- [HOW_TO_USE.md](HOW_TO_USE.md) - Practical usage examples
- [TUTORIAL.md](TUTORIAL.md) - Step-by-step tutorials
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - Technical architecture

