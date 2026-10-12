# NLM Configuration Guide

## Overview

NLM configuration is central to customizing and optimizing brain simulations. The configuration system allows you to control everything from neuron properties to learning mechanisms, development stages, and environmental interactions.

Configuration is managed through the `Config` class, which provides a hierarchical key-value system for setting parameters at different levels (global, brain, plasticity, neuromodulation, etc.).

---

## Configuration System Architecture

### Configuration Hierarchy

NLM configuration follows a hierarchical structure with the following levels:

```
Global Settings
├── brain.*
├── plasticity.*
├── neuromodulation.*
├── development.*
├── memory.*
├── prediction.*
├── cognition.*
└── world.*
```

### Configuration Keys

Configuration keys use dot notation to navigate the hierarchy:

- `brain.neuron_count` - Total number of neurons
- `brain.synapse_density` - Synapse connectivity density
- `plasticity.stdp.learning_rate` - STDP learning rate
- `neuromodulation.dopamine.scale` - Dopamine modulation scale
- `development.synaptogenesis_rate` - Synaptogenesis rate

---

## Brain Configuration

### Core Neuron Parameters

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `brain.neuron_count` | int | 1000 | Total number of neurons | 500, 2000 |
| `brain.region_count` | int | 1 | Number of neural regions | 1, 5, 10 |
| `brain.connection_probability` | float | 0.05 | Connection probability | 0.01, 0.1, 0.2 |
| `brain.initial_weight_mean` | float | 0.5 | Initial synaptic weight mean | 0.1, 0.5, 1.0 |
| `brain.initial_weight_std` | float | 0.1 | Initial synaptic weight std | 0.05, 0.1, 0.2 |

### Neuron Dynamics

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `brain.v_thresh` | float | -50.0 | Threshold potential (mV) | -45.0, -50.0, -55.0 |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) | -65.0, -70.0, -75.0 |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) | -70.0, -75.0, -80.0 |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) | 10.0, 20.0, 30.0 |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) | 1.0, 2.0, 5.0 |

### Population Parameters

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `brain.excitatory_ratio` | float | 0.8 | Excitatory neuron ratio | 0.7, 0.8, 0.9 |
| `brain.inhibitory_ratio` | float | 0.2 | Inhibitory neuron ratio | 0.1, 0.2, 0.3 |
| `brain.sensory_ratio` | float | 0.05 | Sensory neuron ratio | 0.01, 0.05, 0.1 |

---

## Plasticity Configuration

### STDP (Spike-Timing-Dependent Plasticity)

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `plasticity.stdp.enable` | bool | true | Enable STDP | true, false |
| `plasticity.stdp.learning_rate` | float | 0.001 | Learning rate | 0.0005, 0.001, 0.002 |
| `plasticity.stdp.tau_plus` | float | 20.0 | LTP time constant (ms) | 15.0, 20.0, 30.0 |
| `plasticity.stdp.tau_minus` | float | 20.0 | LTD time constant (ms) | 15.0, 20.0, 30.0 |
| `plasticity.stdp. LTP_threshold` | float | 10.0 | LTP timing threshold (ms) | 5.0, 10.0, 20.0 |
| `plasticity.stdp.LTD_threshold` | float | 20.0 | LTD timing threshold (ms) | 15.0, 20.0, 30.0 |

### Hebbian Plasticity

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | true, false |
| `plasticity.hebbian.learning_rate` | float | 0.01 | Learning rate | 0.005, 0.01, 0.02 |
| `plasticity.hebbian.max_weight` | float | 1.0 | Maximum synaptic weight | 0.5, 1.0, 2.0 |
| `plasticity.hebbian.min_weight` | float | 0.0 | Minimum synaptic weight | 0.0, -0.5, -1.0 |

### Structural Plasticity

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | true, false |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | New synapse formation rate | 0.00005, 0.0001, 0.0002 |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Synapse removal rate | 0.000005, 0.00001, 0.00002 |
| `plasticity.structural.max_synapses` | int | 50000 | Maximum synapses | 10000, 50000, 100000 |

---

## Neuromodulation Configuration

### Dopamine System

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | 0.5, 1.0, 2.0 |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level | 0.05, 0.1, 0.2 |
| `neuromod.dopamine.response_strength` | float | 0.5 | Response strength | 0.1, 0.5, 1.0 |

### Curiosity System

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity | true, false |
| `neuromod.curiosity.baseline` | float | 0.1 | Baseline curiosity | 0.05, 0.1, 0.2 |
| `neuromod.curiosity.exploration_bonus` | float | 0.2 | Exploration bonus | 0.1, 0.2, 0.5 |
| `neuromod.curiosity.noise_level` | float | 0.1 | Noise level | 0.05, 0.1, 0.2 |

### Novelty Detection

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | true, false |
| `neuromod.novelty.threshold` | float | 0.3 | Novelty threshold | 0.1, 0.3, 0.5 |
| `neuromod.novelty.sensitivity` | float | 1.0 | Novelty sensitivity | 0.5, 1.0, 2.0 |

---

## Development Configuration

### Synaptogenesis

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `development.synaptogenesis.enable` | bool | true | Enable synaptogenesis | true, false |
| `development.synaptogenesis.rate` | float | 0.0001 | Synaptogenesis rate | 0.00005, 0.0001, 0.0002 |
| `development.synaptogenesis.depression_rate` | float | 0.00005 | Synaptogenesis depression | 0.00002, 0.00005, 0.0001 |

### Pruning

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `development.pruning.enable` | bool | true | Enable pruning | true, false |
| `development.pruning.rate` | float | 0.00001 | Pruning rate | 0.000005, 0.00001, 0.00002 |
| `development.pruning.weak_threshold` | float | 0.2 | Weak synapse threshold | 0.1, 0.2, 0.3 |

### Maturation

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `development.maturation.enable` | bool | true | Enable maturation | true, false |
| `development.maturation.plasticity_rate` | float | 1.0 | Plasticity rate modifier | 0.5, 1.0, 2.0 |
| `development.maturation.excitability_rate` | float | 1.0 | Excitability rate modifier | 0.5, 1.0, 2.0 |

### Critical Period

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `development.critical_period.enable` | bool | true | Enable critical period | true, false |
| `development.critical_period.start_time` | float | 10.0 | Critical period start (ms) | 5.0, 10.0, 20.0 |
| `development.critical_period.end_time` | float | 100.0 | Critical period end (ms) | 50.0, 100.0, 200.0 |
| `development.critical_period.intensity` | float | 1.0 | Critical period intensity | 0.5, 1.0, 2.0 |

---

## Memory System Configuration

### Working Memory

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `memory.working.enable` | bool | true | Enable working memory | true, false |
| `memory.working.trace_decay` | float | 0.01 | Trace decay rate | 0.005, 0.01, 0.02 |
| `memory.working.capacity` | int | 1000 | Working memory capacity | 500, 1000, 2000 |

### Episodic Memory

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `memory.episodic.enable` | bool | true | Enable episodic memory | true, false |
| `memory.episodic.encoding_rate` | float | 0.05 | Encoding rate | 0.01, 0.05, 0.1 |
| `memory.episodic.consolidation_rate` | float | 0.01 | Consolidation rate | 0.005, 0.01, 0.02 |
| `memory.episodic.max_episodes` | int | 1000 | Maximum episodes | 500, 1000, 2000 |

### Associative Memory

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `memory.associative.enable` | bool | true | Enable associative memory | true, false |
| `memory.associative.association_strength` | float | 0.5 | Association strength | 0.1, 0.5, 1.0 |
| `memory.associative.forget_rate` | float | 0.001 | Forget rate | 0.0005, 0.001, 0.002 |

---

## Prediction System Configuration

### Neural Prediction

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `prediction.neural.enable` | bool | true | Enable neural prediction | true, false |
| `prediction.neural.prediction_horizon` | int | 10 | Prediction horizon (steps) | 5, 10, 20 |
| `prediction.neural.error_weight` | float | 0.1 | Prediction error weight | 0.05, 0.1, 0.2 |

---

## Cognition System Configuration

### Concept Formation

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `cognition.concept.enable` | bool | true | Enable concept formation | true, false |
| `cognition.concept.stability_threshold` | float | 0.8 | Stability threshold | 0.5, 0.8, 0.9 |
| `cognition.concept.min_examples` | int | 5 | Minimum examples per concept | 1, 5, 10 |

### Planning

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `cognition.planning.enable` | bool | true | Enable planning | true, false |
| `cognition.planning.horizon` | int | 5 | Planning horizon (steps) | 3, 5, 10 |
| `cognition.planning.discount_factor` | float | 0.9 | Discount factor | 0.8, 0.9, 0.95 |

---

## World Configuration

### SimpleWorld Configuration

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `world.width` | int | 20 | World width | 10, 20, 50 |
| `world.height` | int | 20 | World height | 10, 20, 50 |
| `world.vision_width` | int | 8 | Vision width | 4, 8, 16 |
| `world.vision_height` | int | 8 | Vision height | 4, 8, 16 |

### Resource Configuration

| Parameter | Type | Default | Description | Example |
|-----------|------|---------|-------------|----------|
| `world.resource.max_energy` | float | 100.0 | Maximum energy | 50.0, 100.0, 500.0 |
| `world.resource.spawn_rate` | float | 0.1 | Resource spawn rate | 0.05, 0.1, 0.2 |

---

## Configuration Files

### JSON Configuration

NLM supports JSON configuration files for complex simulations:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 5,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "baseline": 0.1
        }
    }
}
```

### Configuration Loading

```cpp
// Load from JSON file
std::shared_ptr<Config> config = std::make_shared<Config>();
if (!config->loadFromFile("config.json")) {
    std::cerr << "Failed to load configuration" << std::endl;
    return 1;
}

// Create brain with loaded configuration
Brain brain(config);
```

```python
import pynlm

# Load configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Create brain with loaded configuration
brain = pynlm.createBrain(config)
```

---

## Configuration Presets

### Preset: Small Scale

```json
{
    "brain": {
        "neuron_count": 100,
        "region_count": 1
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.002
        }
    }
}
```

### Preset: Medium Scale

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 5
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.001
        }
    }
}
```

### Preset: Large Scale

```json
{
    "brain": {
        "neuron_count": 10000,
        "region_count": 10
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.0005
        }
    }
}
```

---

## Configuration Validation

### Valid Configuration Check

```cpp
// Validate configuration
bool validateConfiguration(const Config& config) {
    // Check brain parameters
    if (config.get("brain.neuron_count", 0) < 10) {
        return false;
    }
    
    // Check plasticity parameters
    if (config.get("plasticity.stdp.learning_rate", 0.0) < 0.0) {
        return false;
    }
    
    // Check neuromodulation parameters
    if (config.get("neuromod.dopamine.scale", 0.0) < 0.0) {
        return false;
    }
    
    return true;
}
```

### Configuration Warnings

```python
# Check for potential configuration issues
def warn_configuration_issues(config):
    issues = []
    
    # Warn if neuron count is very high
    neuron_count = config.get("brain.neuron_count", 0)
    if neuron_count > 50000:
        issues.append("Neuron count is very high - may impact performance")
    
    # Warn if connection probability is too high
    conn_prob = config.get("brain.connection_probability", 0.0)
    if conn_prob > 0.5:
        issues.append("Connection probability is very high - may cause instability")
    
    # Warn if learning rates are very high
    stdp_rate = config.get("plasticity.stdp.learning_rate", 0.0)
    hebb_rate = config.get("plasticity.hebbian.learning_rate", 0.0)
    if stdp_rate > 0.01 or hebb_rate > 0.1:
        issues.append("Learning rates are very high - may cause instability")
    
    return issues
```

---

## Configuration Examples

### Example 1: Learning-focused Configuration

```python
import pynlm

# Create learning-focused configuration
config = pynlm.createDefaultConfig()

# Enhance learning
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.learning_rate", 0.02)
config.set("neuromod.dopamine.scale", 2.0)

# Reduce noise for more stable learning
config.set("neuromod.curiosity.noise_level", 0.05)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 2: Exploration-focused Configuration

```python
import pynlm

# Create exploration-focused configuration
config = pynlm.createDefaultConfig()

# Enhance exploration
config.set("neuromod.curiosity.baseline", 0.3)
config.set("neuromod.curiosity.exploration_bonus", 0.5)
config.set("neuromod.novelty.sensitivity", 2.0)

# Reduce learning stability for more exploration
config.set("plasticity.stdp.learning_rate", 0.0005)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 3: Development-focused Configuration

```python
import pynlm

# Create development-focused configuration
config = pynlm.createDefaultConfig()

# Enhance development
config.set("development.maturation.plasticity_rate", 2.0)
config.set("development.maturation.excitability_rate", 1.5)
config.set("development.critical_period.intensity", 1.5)

# Reduce structural plasticity for more stable connections
config.set("plasticity.structural.synaptogenesis_rate", 0.00005)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()
```

---

## Configuration Performance Tips

### Performance Optimization

1. **Reuse Configurations**: Create and reuse `Config` objects for multiple simulations
2. **Minimize File I/O**: Load configuration once and reuse
3. **Use Appropriate Neuron Counts**: Match neuron count to simulation requirements
4. **Tune Learning Rates**: Use appropriate learning rates for your task
5. **Monitor Memory Usage**: Monitor memory usage with different configurations

### Profiling with Different Configurations

```python
import pynlm
import time

# Test different configurations
configs = [
    {"neuron_count": 100, "stdp_rate": 0.001},
    {"neuron_count": 500, "stdp_rate": 0.001},
    {"neuron_count": 1000, "stdp_rate": 0.001},
    {"neuron_count": 2000, "stdp_rate": 0.001}
]

for config_params in configs:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", config_params["neuron_count"])
    config.set("plasticity.stdp.learning_rate", config_params["stdp_rate"])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    end_time = time.time()
    
    print(f"Neuron count: {config_params['neuron_count']}, "
          f"Time: {end_time - start_time:.2f}s")
```

---

## Configuration Documentation

### Configuration Reference

The complete configuration reference is available in:

- `docs/API_REFERENCE.md` - Full API documentation
- `pyproject.toml` - Python configuration
- `CMakeLists.txt` - Build configuration

### Configuration Validation Scripts

Validation scripts are available in:

- `scripts/validate_config.py` - Python validation
- `scripts/config_comparison.py` - Configuration comparison

---

## Summary

NLM configuration is a powerful tool for customizing brain simulations. The hierarchical configuration system allows for precise control over all aspects of the simulation, from low-level neuron dynamics to high-level cognitive functions. This guide provides comprehensive documentation for configuring NLM for various use cases and requirements.