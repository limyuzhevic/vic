# NLM Configuration Guide

## Overview

NLM uses a flexible JSON-based configuration system to control brain initialization, simulation parameters, and learning behavior. The configuration system allows you to customize every aspect of your neural network simulation.

## Configuration Structure

NLM configurations are JSON files with key-value pairs. Configuration keys use dot notation for hierarchical organization:

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1
  },
  "plasticity": {
    "stdp": {
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "learning_rate": 0.01
    },
    "structural": {
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "exploration_rate": 0.5
    },
    "novelty": {
      "enable": true,
      "threshold": 0.1
    }
  },
  "development": {
    "stage_duration": [100, 200, 300, 400],
    "plasticity_decay_rate": 0.01
  }
}
```

## Configuration Categories

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons |
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
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.01 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.exploration_rate` | float | 0.5 | Curiosity-driven exploration probability |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.1 | Novelty detection threshold |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage_duration` | array | [100, 200, 300, 400] | Duration of each developmental stage |
| `development.plasticity_decay_rate` | float | 0.01 | Rate of plasticity decay during development |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation time step (seconds) |
| `simulation.max_steps` | int | 10000 | Maximum number of simulation steps |
| `simulation.checkpoint_interval` | int | 1000 | Steps between checkpoints |

## Python Configuration API

### Creating Configurations

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.2)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.curiosity.enable", True)
```

### Loading and Saving Configurations

```python
# Load from file
config.loadFromFile("my_config.json")

# Save to file
config.saveToFile("my_config.json")

# Load from command line
config.loadFromArgs(len(sys.argv), sys.argv)
```

### Configuration Validation

```python
# Check if configuration key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Get all configuration keys
keys = config.getKeys()
print(f"Available keys: {keys}")

# Clear configuration
config.clear()
```

## Example Configurations

### Small Brain for Testing

```json
{
  "brain": {
    "neuron_count": 100,
    "region_count": 1,
    "connection_probability": 0.05,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1
  },
  "plasticity": {
    "stdp": {"learning_rate": 0.001},
    "hebbian": {"learning_rate": 0.01}
  }
}
```

### Large Brain for Research

```json
{
  "brain": {
    "neuron_count": 10000,
    "region_count": 5,
    "connection_probability": 0.05,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.15
  },
  "plasticity": {
    "stdp": {"learning_rate": 0.001, "tau_plus": 25.0, "tau_minus": 25.0},
    "hebbian": {"learning_rate": 0.005},
    "structural": {
      "synaptogenesis_rate": 0.0005,
      "pruning_rate": 0.00005
    }
  },
  "neuromodulation": {
    "curiosity": {"enable": true, "exploration_rate": 0.8},
    "novelty": {"enable": true, "threshold": 0.05}
  },
  "development": {
    "stage_duration": [500, 1000, 1500, 2000],
    "plasticity_decay_rate": 0.02
  }
}
```

### Development Brain

```json
{
  "brain": {
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15
  },
  "development": {
    "stage_duration": [50, 100, 150, 200],
    "plasticity_decay_rate": 0.05
  },
  "plasticity": {
    "stdp": {"learning_rate": 0.002},
    "structural": {
      "synaptogenesis_rate": 0.001,
      "pruning_rate": 0.0001
    }
  }
}
```

## Configuration Best Practices

### 1. Start with Defaults

```python
# Always start with default configuration and modify only what you need
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Customize only this
```

### 2. Use Consistent Naming

```python
# Use consistent dot notation for related parameters
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
```

### 3. Validate Before Running

```python
# Validate configuration before simulation
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")
    if neuron_count < 10:
        print("Warning: Very small brain may not exhibit interesting behavior")
```

### 4. Test Configuration Changes

```python
# Test different configurations systematically
base_config = pynlm.createDefaultConfig()

# Configuration A
config_a = pynlm.createDefaultConfig()
config_a.set("brain.neuron_count", 500)
config_a.set("plasticity.stdp.learning_rate", 0.001)

# Configuration B
config_b = pynlm.createDefaultConfig()
config_b.set("brain.neuron_count", 2000)
config_b.set("plasticity.stdp.learning_rate", 0.01)

# Run experiments with each configuration
```

### 5. Save Working Configurations

```python
# Save successful configurations for later use
if successful_simulation:
    config.saveToFile("successful_config.json")
```

## Configuration Troubleshooting

### Common Issues and Solutions

#### Issue: Brain has no neurons

**Cause**: `brain.neuron_count` is 0 or missing

**Solution**:
```python
if not config.has("brain.neuron_count") or config.get("brain.neuron_count") == 0:
    config.set("brain.neuron_count", 1000)
```

#### Issue: Brain has no connections

**Cause**: `brain.connection_probability` is 0

**Solution**:
```python
if config.has("brain.connection_probability"):
    prob = config.get("brain.connection_probability")
    if prob == 0:
        config.set("brain.connection_probability", 0.1)
```

#### Issue: Simulation runs too slowly

**Cause**: Too many neurons or too small timestep

**Solution**:
```python
# Reduce neuron count or increase timestep
config.set("brain.neuron_count", 1000)  # Reduce from 10000
config.set("simulation.timestep", 0.01)  # Increase from 0.001
```

#### Issue: Learning doesn't occur

**Cause**: Plasticity is disabled

**Solution**:
```python
# Enable plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

### Validation Script

```python
def validate_config(config):
    """Validate configuration and return list of warnings."""
    warnings = []
    
    if not config.has("brain.neuron_count") or config.get("brain.neuron_count") == 0:
        warnings.append("brain.neuron_count is 0 or missing - brain will be empty")
    
    if config.has("brain.connection_probability"):
        prob = config.get("brain.connection_probability")
        if prob < 0 or prob > 1:
            warnings.append("brain.connection_probability must be between 0 and 1")
    
    if config.has("simulation.timestep"):
        timestep = config.get("simulation.timestep")
        if timestep <= 0 or timestep > 0.1:
            warnings.append("simulation.timestep should be between 0 and 0.1 seconds")
    
    if not config.has("plasticity.stdp.enable") or not config.get("plasticity.stdp.enable"):
        warnings.append("STDP is disabled - learning may not occur")
    
    return warnings

# Usage
warnings = validate_config(config)
if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

## Configuration File Format

### JSON Configuration Files

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 1
  },
  "simulation": {
    "timestep": 0.001
  }
}
```

### INI Configuration Files

For applications that prefer INI format, NLM supports:

```ini
[brain]
neuron_count = 1000
region_count = 1

[simulation]
timestep = 0.001

[plasticity.stdp]
learning_rate = 0.001
```

### Environment Variable Configuration

NLM can also read configuration from environment variables:

```bash
export NLM_BRAIN_NEURON_COUNT=1000
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_PLASTICITY_STDP_LEARNING_RATE=0.001
```

## Advanced Configuration

### Custom Neuron Parameters

```python
# Custom neuron parameters for specialized experiments
config.set("brain.v_thresh", -45.0)  # Higher threshold
config.set("brain.tau_mem", 10.0)   # Faster membrane time constant
config.set("brain.tau_ref", 1.0)    # Shorter refractory period
```

### Custom Plasticity Rules

```python
# Custom STDP parameters for research
config.set("plasticity.stdp.tau_plus", 50.0)    # Longer potentiation window
config.set("plasticity.stdp.tau_minus", 10.0)   # Shorter depression window
config.set("plasticity.stdp.learning_rate", 0.0001)  # Slower learning
```

### Development Schedules

```python
# Custom developmental schedule for research
stages = [100, 300, 500, 800, 1000]  # Longer critical period
config.set("development.stage_duration", stages)
```

## Configuration File Templates

The repository includes configuration templates in the `configs/` directory:

- `configs/default.cfg` - Default configuration
- `configs/minimal.cfg` - Minimal working configuration
- `configs/research.cfg` - Configuration for research experiments
- `configs/development.cfg` - Configuration for development studies

Use these as starting points for your experiments.

## Configuration Best Practices Summary

1. **Start Simple**: Use default configuration and modify only what's necessary
2. **Validate**: Always validate configuration before running simulations
3. **Document**: Save successful configurations for reproduction
4. **Test**: Systematically test different configuration combinations
5. **Document**: Keep notes on configuration changes and their effects

The configuration system is designed to be flexible while preventing common mistakes. Use it to explore the vast parameter space of neural network simulation!
