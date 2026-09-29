# NLM Configuration Guide

This document provides comprehensive guidance on configuring and using the NLM (Neural Learning Machine) framework.

## Configuration System

NLM uses a flexible configuration system that supports:
- File-based configuration (configs/default.cfg)
- Command-line overrides
- Runtime configuration
- Environment variable support

### Configuration File Format

Configuration files use a simple key=value format:
```
# Comments start with #
key = value
another_key = "quoted value"
numeric_value = 42
boolean_value = true
```

### Command-Line Options

```bash
--key=value    Set configuration parameter
-key value     Set configuration parameter (alternative format)
```

## Core Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Advanced Configuration

### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | GridWorld | Environment type |
| `environment.width` | int | 10 | World width |
| `environment.height` | int | 10 | World height |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |
| `simulation.time_limit` | float | 0.0 | Time limit in seconds (0 = unlimited) |
| `simulation.checkpoint_dir` | string | ./checkpoints | Directory for checkpoint files |

## Configuration Examples

### Minimal Configuration

```bash
# Save as config.cfg
echo 'brain.neuron_count = 500
environment.width = 20
environment.height = 20
plasticity.stdp.learning_rate = 0.005' > config.cfg

# Run with custom configuration
./nlm --config=config.cfg
```

### Command-Line Overrides

```bash
# Use file as base, override specific parameters
./nlm --config=configs/default.cfg \
     --brain.neuron_count=2000 \
     --plasticity.stdp.learning_rate=0.002
```

## Configuration Validation

The configuration system validates parameter types and ranges:

- Numeric parameters must be valid numbers
- Boolean parameters accept: true/false, 1/0, yes/no
- String parameters are quoted when saved to files
- Required parameters are checked before simulation starts

## Configuration Sources Priority

Configuration sources are applied in this order (later sources override earlier ones):

1. **Default values** - Built-in defaults
2. **Configuration file** - loaded from file (configs/default.cfg)
3. **Command-line arguments** - override file settings
4. **Runtime configuration** - programmatic setting via API

## Python API Configuration

The Python API provides the same configuration functionality:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1500)
config.set("environment.width", 30)
config.set("environment.height", 30)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

## Troubleshooting

### Configuration File Errors

**Error**: "File not found or cannot be read"
**Solution**: Ensure configuration file exists and is readable

**Error**: "Invalid key=value format"
**Solution**: Check file format, remove quotes if used inconsistently

### Command-Line Errors

**Error**: "Unknown parameter" or "Invalid parameter type"
**Solution**: Verify parameter name and type against documentation

### Missing Required Parameters

**Error**: "Required parameter not set"
**Solution**: Set required parameters or ensure configuration file includes them

## Configuration Logging

NLM logs configuration changes and warnings:

```
[INFO] Loaded configuration from: configs/custom.cfg
[INFO] Setting random_seed: 42 (source: File)
[WARN] Overriding parameter neuron_count: 500 (from 1000)
[INFO] Configuration summary: 12 parameters loaded
```

## Best Practices

1. **Use configuration files for persistent settings**
2. **Use command-line arguments for one-off experiments**
3. **Document custom configuration values**
4. **Version configuration files with your experiment**
5. **Use environment-specific configurations**

## Configuration Templates

### Development Configuration

```bash
# configs/dev.cfg
brain.neuron_count = 100
environment.width = 10
environment.height = 10
plasticity.stdp.learning_rate = 0.01
simulation.max_steps = 1000
simulation.checkpoint_dir = ./checkpoints_dev
```

### Production Configuration

```bash
# configs/prod.cfg
brain.neuron_count = 5000
environment.width = 50
environment.height = 50
plasticity.stdp.learning_rate = 0.001
simulation.max_steps = 100000
simulation.checkpoint_dir = ./checkpoints_prod
```

## Configuration Security

- Configuration files should not contain sensitive information
- Use environment variables for secrets
- Restrict file permissions for configuration files
- Consider using encrypted configuration for production systems

## Future Enhancements

The configuration system can be extended with:

- JSON/YAML file format support
- Schema validation
- Configuration inheritance
- Distributed configuration management
- Runtime configuration hot-swapping