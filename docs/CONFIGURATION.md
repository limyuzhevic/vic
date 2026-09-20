# NLM Configuration Documentation

This document describes all configuration parameters available in NLM.

## Overview

NLM uses a flexible key-value configuration system. Configuration can be loaded from:
- Default configuration file
- Command line arguments
- Runtime configuration

Configuration values can be:
- Integers (`int`, `int64_t`)
- Floating point numbers (`double`, `float`)
- Booleans (`bool`)
- Strings
- Lists of numbers or strings

## Configuration File Format

The configuration file uses a simple key=value format:
```
# This is a comment
key = value
another_key = "string with quotes"
number_list = [1, 2, 3]
boolean_list = [true, false]
```

## Configuration Parameters

### Core Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |

### Neuron Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total neurons |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Connection probability |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage.duration` | float | 1000.0 | Duration of each developmental stage |
| `development.synaptogenesis.rate` | float | 0.0001 | Rate of new synapse formation |
| `development.pruning.rate` | float | 0.00001 | Rate of synapse pruning |

### Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.capacity` | int | 100 | Working memory capacity |
| `memory.episodic.capacity` | int | 1000 | Episodic memory capacity |
| `memory.associative.capacity` | int | 500 | Associative memory capacity |

### Prediction Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.horizon` | int | 10 | Prediction time horizon |
| `prediction.error.threshold` | float | 0.5 | Prediction error threshold |

### Cognition Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.attention.competition` | float | 1.0 | Attention competition strength |
| `cognition.planning.horizon` | int | 5 | Planning horizon |

### Environment Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.width` | int | 20 | Environment width |
| `environment.height` | int | 20 | Environment height |
| `environment.vision_width` | int | 8 | Vision width |
| `environment.vision_height` | int | 8 | Vision height |

### Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep (seconds) |
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Maximum simulation time (seconds) |

### Logging Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `logging.level` | string | INFO | Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL) |
| `logging.to_file` | bool | false | Write logs to file |
| `logging.filename` | string | nlm.log | Log file name |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enabled` | bool | true | Enable development system |
| `development.curiosity.enabled` | bool | true | Enable curiosity-driven exploration |
| `development.novelty.enabled` | bool | true | Enable novelty detection |

## Command Line Arguments

NLM supports several command line options:

### Basic Options

```bash
nlm [options]

Options:
  --config <file>          Configuration file (default: configs/default.cfg)
  --help                   Show help message
  --version                Show version information
  --log-level <level>      Set log level
  --neuron-count <n>       Number of neurons (overrides config)
  --region-count <r>       Number of regions (overrides config)
  --simulation-steps <s>   Maximum simulation steps (overrides config)
  --random-seed <seed>     Random seed (overrides config)
```

### Example Usage

```bash
# Run with default configuration
nlm

# Run with custom configuration
nlm --config my_config.cfg

# Override neuron count
nlm --neuron-count 500

# Run with debug logging
nlm --log-level DEBUG

# Complete example
nlm --config experiment.cfg --neuron-count 1000 \
    --region-count 4 --random-seed 12345
```

## Configuration Examples

### Minimal Configuration

```
# File: minimal.cfg
neuron_count = 100
region_count = 1
simulation_timestep = 0.001
log_level = INFO
```

### High-Performance Configuration

```
# File: performance.cfg
neuron_count = 5000
region_count = 8
connection_probability = 0.2
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
plasticity.structural.enable = true
environment.width = 50
environment.height = 50
log_level = WARNING
```

### Research Configuration

```
# File: research.cfg
neuron_count = 1000
region_count = 4
connection_probability = 0.05
plasticity.stdp.learning_rate = 0.0005
plasticity.stdp.tau_plus = 30.0
plasticity.stdp.tau_minus = 30.0
development.synaptogenesis.rate = 0.00005
development.pruning.rate = 0.000001
memory.working.capacity = 200
memory.episodic.capacity = 2000
prediction.horizon = 20
cognition.attention.competition = 2.0
log_level = DEBUG
```

## Configuration Override Priority

1. **Command line arguments** (highest priority)
2. **Configuration file** (medium priority)
3. **Default values** (lowest priority)

If a parameter is set in multiple places, command line arguments take precedence over configuration files, which take precedence over defaults.

## Configuration Validation

The configuration system validates that:
- Positive integers are positive
- Probabilities are between 0 and 1
- Potentials are within biologically plausible ranges
- File paths are valid

Invalid configurations will be logged and the program may terminate with an error message.

## Runtime Configuration

Parameters can also be set at runtime using the Config API:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("environment.width", 100)
config.set("logging.level", "DEBUG")

brain = pynlm.createBrain(config)
brain.initialize()
```

## Configuration Templates

### Experimental Templates

The following configuration templates are commonly used in NLM experiments:

1. **Exploration**: Small brain, high plasticity
2. **Exploitation**: Large brain, low plasticity
3. **Development**: Full developmental stages
4. **Real-time**: Optimized for real-time performance

See the `configs/` directory for specific configuration files.

## Best Practices

1. **Always specify a random seed** for reproducible experiments
2. **Use descriptive names** for custom configuration parameters
3. **Document your configuration** in comments
4. **Test configurations** before running large simulations
5. **Monitor log files** for configuration warnings
6. **Use environment variables** for common configuration values

## Troubleshooting

### Common Issues

**Issue**: Brain initialization fails
**Solution**: Check that `neuron_count` > 0 and `region_count` <= `neuron_count`

**Issue**: Plasticity not working
**Solution**: Ensure `plasticity.*` parameters are enabled and have appropriate values

**Issue**: Simulation too slow
**Solution**: Reduce `neuron_count`, increase timestep, or disable certain features

**Issue**: Memory usage too high
**Solution**: Reduce `memory.*` capacities or disable memory systems

### Configuration File Errors

**Symptom**: Parser error or unrecognized parameter
**Solution**: Check syntax and parameter names against this documentation

**Symptom**: Default values used instead of file values
**Solution**: Verify file path and permissions

### Command Line Issues

**Symptom**: Option not recognized
**Solution**: Use `--help` to see all available options

**Symptom**: Override not working
**Solution**: Check option spelling and format

## Advanced Configuration

### Dynamic Configuration

NLM supports dynamic configuration changes during simulation:

```python
# During simulation
config.set("plasticity.learning_rate", 0.01)
config.set("environment.width", 50)
```

### Configuration Monitoring

The configuration system can monitor for changes and log them:

```python
config.monitorChanges(lambda key, old_val, new_val: 
    print(f"Config changed: {key} = {old_val} -> {new_val}"))
```

## Configuration Migration

When upgrading NLM, some parameters may have changed names or moved between sections. The configuration loader attempts to be backward compatible, but check this documentation for the latest parameter names.
