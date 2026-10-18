# NLM Configuration Documentation

## Overview

The NLM configuration system provides a hierarchical, type-safe interface for controlling brain behavior, learning, memory, neuromodulation, and development. It supports multiple input formats and provides validation for configuration values.

## Configuration Structure

### Root Configuration Tree

The configuration system uses a hierarchical key-value structure with the following main scopes:

```
brain/              # Core brain parameters
plasticity/        # Learning and plasticity rules
neuromod/          # Neuromodulation systems
memory/            # Memory systems
prediction/        # Prediction and forecasting
environment/       # Environment interface
world/             # World simulation
performance/       # Performance optimization
logging/           # Logging and debugging
experiments/       # Experiment settings
```

Each scope contains specific parameters with their own data types and default values.

### Configuration Access Patterns

#### Dict-like Access

```python
config = pynlm.createDefaultConfig()

# Dot notation
config.set("brain.neuron_count", 1000)
value = config.get("brain.neuron_count")

# Direct key access
config["brain.v_thresh"] = -50.0
```

#### Sub-configuration Access

```python
# Get sub-configuration
brain_config = config.getSubConfig("brain")
print(f"Neuron count: {brain_config.get('neuron_count')}")

# Modify sub-configuration
brain_config.set("neuron_count", 2000)
```

#### Configuration Validation

```python
# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Current neuron count: {config.get('brain.neuron_count')}")

# Get all keys with prefix
keys = config.getKeys("brain.")
print(f"Brain parameters: {keys}")
```

## Brain Configuration

### Overview

Brain configuration controls the core neural dynamics including neuron properties, connectivity, and initial conditions.

### Parameters

#### Neuron Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `brain.neuron_count` | int | 1000 | 100-10000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | 0.01-0.5 | Probability of connection between neurons |
| `brain.connection_probability` | float | 0.05 | 0.01-0.2 | Target connectivity density |
| `brain.v_thresh` | float | -50.0 | -80 to 0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | -90 to -50 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | -85 to -65 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | 5-50 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | 1-10 | Refractory period (ms) |
| `brain.tau_syn` | float | 5.0 | 2-20 | Synaptic time constant (ms) |
| `brain.initial_weight_mean` | float | 0.5 | -1 to 1 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | 0-1 | Standard deviation of initial weights |

#### Population Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.excitatory_ratio` | float | 0.8 | Ratio of excitatory neurons |
| `brain.inhibitory_ratio` | float | 0.2 | Ratio of inhibitory neurons |
| `brain.sensory_ratio` | float | 0.0 | Ratio of sensory neurons |
| `brain.motor_ratio` | float | 0.0 | Ratio of motor neurons |
| `brain.modulatory_ratio` | float | 0.0 | Ratio of modulatory neurons |
| `brain.internal_ratio` | float | 0.0 | Ratio of internal neurons |

#### Region Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.regions.0.name` | string | "cortex" | Name of first region |
| `brain.regions.0.neuron_count` | int | 500 | Neurons in region |
| `brain.regions.0.excitatory_ratio` | float | 0.8 | Excitatory ratio for region |
| `brain.regions.0.inhibitory_ratio` | float | 0.2 | Inhibitory ratio for region |

### Example Configuration

```python
import pynlm

# Create minimal configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -55.0)

# Configure neuron types
config.set("brain.excitatory_ratio", 0.75)
config.set("brain.inhibitory_ratio", 0.25)

# Add regions
config.set("brain.regions.0.name", "cortex")
config.set("brain.regions.0.neuron_count", 1000)
config.set("brain.regions.1.name", "hippocampus")
config.set("brain.regions.1.neuron_count", 500)
```

## Plasticity Configuration

### Overview

Plasticity configuration controls learning mechanisms including STDP, Hebbian learning, and structural changes.

### STDP Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `plasticity.stdp.enable` | bool | true | - | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | 0.0001-0.01 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | 5-50 | LTP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | 5-50 | LTD time constant (ms) |
| `plasticity.stdp.a_plus` | float | 0.01 | 0-0.1 | LTP amplitude |
| `plasticity.stdp.a_minus` | float | 0.01 | 0-0.1 | LTD amplitude |
| `plasticity.stdp.w_max` | float | 1.0 | 0.5-2.0 | Maximum synaptic weight |
| `plasticity.stdp.w_min` | float | 0.0 | 0-0.5 | Minimum synaptic weight |

### Hebbian Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `plasticity.hebbian.enable` | bool | true | - | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.0001 | 0.00001-0.001 | Hebbian learning rate |
| `plasticity.hebbian.threshold` | float | 0.5 | 0-1 | Weight change threshold |
| `plasticity.hebbian.decay_rate` | float | 0.99 | 0.9-0.999 | Weight decay rate |

### Structural Plasticity Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `plasticity.structural.enable` | bool | true | - | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.01 | 0.001-0.1 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.005 | 0.001-0.05 | Rate of synapse removal |
| `plasticity.structural.min_weight` | float | 0.1 | 0-0.5 | Minimum weight for pruning |
| `plasticity.structural.min_activity` | float | 0.01 | 0-0.1 | Minimum activity for retention |

### Plasticity Configuration Examples

```python
import pynlm

config = pynlm.createDefaultConfig()

# Enable STDP with custom parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 30.0)

# Configure Hebbian learning
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.0005)
config.set("plasticity.hebbian.decay_rate", 0.995)

# Configure structural plasticity
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.02)
config.set("plasticity.structural.pruning_rate", 0.008)

# Save configuration
config.save_to_file("plasticity_config.json")
```

## Neuromodulation Configuration

### Overview

Neuromodulation configuration controls the brain's modulatory systems including dopamine, curiosity, novelty, and prediction error signals.

### Dopamine Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | 0.1-5.0 | Dopamine modulation scale |
| `neuromod.dopamine.enable` | bool | true | - | Enable dopamine effects |
| `neuromod.dopamine.prediction_weight` | float | 0.5 | 0-1 | Weight of prediction error on dopamine |
| `neuromod.dopamine.reward_weight` | float | 0.5 | 0-1 | Weight of reward on dopamine |

### Curiosity Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.curiosity.enable` | bool | true | - | Enable curiosity system |
| `neuromod.curiosity.scale` | float | 1.0 | 0.1-5.0 | Curiosity signal scale |
| `neuromod.curiosity.noise` | float | 0.1 | 0-1 | Random noise level |
| `neuromod.curiosity.baseline` | float | 0.1 | 0-1 | Baseline curiosity level |

### Novelty Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.novelty.enable` | bool | true | - | Enable novelty detection |
| `neuromod.novelty.sensitivity` | float | 0.5 | 0-2 | Novelty detection sensitivity |
| `neuromod.novelty.decay` | float | 0.9 | 0.5-0.99 | Novelty decay rate |

### Prediction Error Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.prediction_error.enable` | bool | true | - | Enable prediction error signals |
| `neuromod.prediction_error.scale` | float | 1.0 | 0.1-5.0 | Prediction error scale |
| `neuromod.prediction_error.adaptation_rate` | float | 0.01 | 0.001-0.1 | Prediction error adaptation |

### Neuromodulation Configuration Examples

```python
import pynlm

config = pynlm.createDefaultConfig()

# Configure dopamine
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.dopamine.enable", True)
config.set("neuromod.dopamine.prediction_weight", 0.3)
config.set("neuromod.dopamine.reward_weight", 0.7)

# Configure curiosity
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.scale", 1.2)
config.set("neuromod.curiosity.noise", 0.05)

# Configure novelty
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 0.8)
config.set("neuromod.novelty.decay", 0.95)

# Disable prediction error
config.set("neuromod.prediction_error.enable", False)
```

## Memory Configuration

### Overview

Memory configuration controls working memory, episodic memory, and other memory systems.

### Working Memory Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.working.enable` | bool | true | - | Enable working memory |
| `memory.working.capacity` | float | 0.5 | 0.1-1.0 | Working memory capacity |
| `memory.working.decay_rate` | float | 0.95 | 0.9-0.99 | Decay rate of working memory |
| `memory.working.recurrent_strength` | float | 0.8 | 0.1-1.0 | Recurrent connection strength |

### Episodic Memory Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.episodic.enable` | bool | true | - | Enable episodic memory |
| `memory.episodic.capacity` | int | 100 | 10-1000 | Maximum number of episodes |
| `memory.episodic.consolidation_rate` | float | 0.01 | 0.001-0.1 | Consolidation rate |
| `memory.episodic.replay_frequency` | float | 0.1 | 0.01-1.0 | Replay frequency (per step) |

### Semantic Memory Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.semantic.enable` | bool | false | - | Enable semantic memory |
| `memory.semantic.capacity` | int | 50 | 5-200 | Maximum number of concepts |
| `memory.semantic.learning_rate` | float | 0.001 | 0.0001-0.01 | Concept learning rate |

### Associative Memory Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.associative.enable` | bool | false | - | Enable associative memory |
| `memory.associative.capacity` | int | 50 | 5-200 | Maximum associations |
| `memory.associative.strength_decay` | float | 0.99 | 0.9-0.999 | Association strength decay |

## Prediction Configuration

### Overview

Prediction configuration controls temporal sequence prediction and forecasting.

### Prediction System Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `prediction.enable` | bool | true | - | Enable prediction system |
| `prediction.horizon` | int | 10 | 1-100 | Prediction horizon (steps) |
| `prediction.learning_rate` | float | 0.01 | 0.001-0.1 | Prediction learning rate |
| `prediction.accuracy_threshold` | float | 0.5 | 0-1 | Accuracy threshold for learning |
| `prediction.error_decay` | float | 0.95 | 0.9-0.99 | Prediction error decay |

## Development Configuration

### Overview

Development configuration controls brain development across different stages.

### Development System Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `development.enable` | bool | true | - | Enable development |
| `development.stage_duration` | float | 100.0 | 10-1000 | Duration of each stage (steps) |
| `development.initial_plasticity` | float | 1.0 | 0.1-5.0 | Initial plasticity rate |
| `development.final_plasticity` | float | 0.1 | 0.01-0.5 | Final plasticity rate |
| `development.adaptation_rate` | float | 0.01 | 0.001-0.1 | Development adaptation rate |

### Plasticity Development Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `development.plasticity.enable` | bool | true | - | Enable plasticity development |
| `development.plasticity.rate_by_stage` | array | [1.0, 0.8, 0.6, 0.3, 0.1] | - | Plasticity rates by stage |
| `development.plasticity.initial_LTP` | float | 0.005 | 0-0.01 | Initial LTP rate |
| `development.plasticity.initial_LTD` | float | 0.005 | 0-0.01 | Initial LTD rate |

## Environment Configuration

### Overview

Environment configuration controls the simulation environment and sensory processing.

### Sensory Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `environment.sensory.vision.enable` | bool | true | - | Enable visual processing |
| `environment.sensory.vision.field_of_view` | float | 90.0 | 45-180 | Vision field of view (degrees) |
| `environment.sensory.vision.resolution` | int | 8 | 4-16 | Vision resolution |
| `environment.sensory.proprioception.enable` | bool | true | - | Enable proprioception |
| `environment.sensory.internal.enable` | bool | true | - | Enable internal state |

### Reward Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `environment.reward.function` | string | "external" | "external", "internal", "mixed" | Reward function type |
| `environment.reward.scale` | float | 1.0 | 0.1-5.0 | Reward scale factor |
| `environment.reward.discount` | float | 0.95 | 0.9-0.99 | Reward discount rate |

## World Configuration

### Overview

World configuration controls the simple world simulation environment.

### World Simulation Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `world.width` | int | 20 | 5-100 | World width (cells) |
| `world.height` | int | 20 | 5-100 | World height (cells) |
| `world.vision_width` | int | 8 | 2-16 | Agent vision width |
| `world.vision_height` | int | 8 | 2-16 | Agent vision height |
| `world.timestep` | float | 0.1 | 0.01-1.0 | Physics timestep (seconds) |
| `world.max_energy` | float | 100.0 | 10-1000 | Maximum agent energy |
| `world.random_seed` | int | None | - | Random seed (None for random) |

## Performance Configuration

### Overview

Performance configuration optimizes simulation speed and memory usage.

### Performance Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `performance.threads` | int | 1 | 1-32 | Number of processing threads |
| `performance.memory_pool_size` | int | 1024 | 256-8192 | Memory pool size |
| `performance.event_queue_size` | int | 2048 | 512-16384 | Event queue size |
| `performance.sparse_connectivity` | bool | true | - | Use sparse connectivity |
| `performance.simd_optimization` | bool | true | - | Enable SIMD optimizations |
| `performance.parallel_plasticity` | bool | false | - | Parallelize plasticity updates |

## Logging Configuration

### Overview

Logging configuration controls debug and diagnostic output.

### Logging Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `logging.level` | string | "info" | "debug", "info", "warn", "error" | Logging level |
| `logging.file` | string | None | - | Log file path (None for stdout) |
| `logging.max_size` | int | 10485760 | 1048576-104857600 | Maximum log file size (bytes) |
| `logging.max_files` | int | 5 | 1-10 | Maximum number of log files |
| `logging.format` | string | "%(asctime)s - %(name)s - %(levelname)s - %(message)s" | - | Log message format |

## Experiment Configuration

### Overview

Experiment configuration controls experiment setup and data collection.

### Experiment Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `experiment.name` | string | "default" | - | Experiment name |
| `experiment.description` | string | "" | - | Experiment description |
| `experiment.version` | string | "1.0" | - | Experiment version |
| `experiment.author` | string | "" | - | Experiment author |
| `experiment.record_metrics` | bool | true | - | Record metrics |
| `experiment.save_brain` | bool | true | - | Save brain state |
| `experiment.metrics.frequency` | int | 100 | 1-1000 | Metric recording frequency |
| `experiment.metrics.threshold` | float | 0.0 | 0-1 | Metric threshold for alerts |

## Configuration Validation

### Type Validation

```python
config.set("brain.neuron_count", 1000)  # int
config.set("brain.synapse_density", 0.1)  # float
config.set("plasticity.stdp.enable", True)  # bool
config.set("brain.region_name", "cortex")  # string
```

### Range Validation

```python
# These will raise exceptions if values are out of range
config.set("brain.neuron_count", 50)    # OK (min: 100)
config.set("brain.neuron_count", 10000) # OK (max: 10000)
config.set("brain.neuron_count", 50000) # Raises ValueError
```

### Format Validation

```python
# Configuration keys must use dot notation
config.set("brain.neuron_count", 1000)  # OK
config.set("brain/neuron_count", 1000)  # Raises ValueError

# Values must match expected types
config.set("brain.neuron_count", "1000")  # Raises TypeError
```

## Configuration Inheritance

### Configuration Hierarchy

Configuration supports inheritance through nested scopes:

```python
# Base configuration
base_config = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    },
    "plasticity": {
        "stdp": {"enable": True}
    }
}

# Child configuration (inherits base)
child_config = {
    "brain": {
        "neuron_count": 2000  # Override
    }
}

# Merged configuration
merged = {**base_config, **child_config}
```

### Configuration Scopes

| Scope | Priority | Description |
|-------|----------|-------------|
| Default | Low | Built-in defaults |
| File | Medium | Configuration file |
| Programmatic | High | Code-level settings |
| Runtime | Highest | Runtime modifications |

## Configuration Examples

### Minimal Configuration

```python
import pynlm

# Create minimal configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)

# Save to file
config.save_to_file("minimal_config.json")
```

### Learning Configuration

```python
import pynlm

# Create learning-focused configuration
config = pynlm.createDefaultConfig()

# Brain settings for learning
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -55.0)

# Enable all plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation for learning
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Memory for experience
config.set("memory.working.enable", True)
config.set("memory.episodic.enable", True)

# Save configuration
config.save_to_file("learning_config.json")
```

### World Simulation Configuration

```python
import pynlm

# Create world-optimized configuration
config = pynlm.createDefaultConfig()

# Large brain for complex perception
config.set("brain.neuron_count", 5000)
config.set("brain.excitatory_ratio", 0.85)

# World settings
world_config = {
    "world": {
        "width": 50,
        "height": 50,
        "vision_width": 16,
        "vision_height": 16,
        "timestep": 0.05,
        "max_energy": 500
    }
}

# Performance settings
performance_config = {
    "performance": {
        "threads": 4,
        "memory_pool_size": 4096,
        "event_queue_size": 8192
    }
}

# Save comprehensive configuration
import json
comprehensive_config = {**config.to_dict(), **world_config, **performance_config}
with open("world_config.json", "w") as f:
    json.dump(comprehensive_config, f, indent=2)
```

## Configuration File Format

### JSON Format

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "excitatory_ratio": 0.8
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        }
    }
}
```

### YAML Format

```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  excitatory_ratio: 0.8

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001

neuromod:
  dopamine:
    scale: 1.0
```

## Configuration API Reference

### Config Class Methods

#### Construction and Parsing

**`createDefaultConfig()`**

Creates a configuration with all default values.

**`createConfigFromFile(filepath)`**

Creates configuration from file (JSON, YAML, or TOML).

**`createConfigFromDict(config_dict)`**

Creates configuration from dictionary.

**`createEmptyConfig()`**

Creates empty configuration.

#### Query Operations

**`has(key)`**

Check if configuration key exists.

**`getKeys([prefix])`**

Get all configuration keys, optionally with prefix.

**`get(key, default)`**

Get configuration value with default.

**`getInt(key)`**
**`getFloat(key)`**
**`getString(key)`**
**`getBool(key)`**

Type-specific getters.

**`getSubConfig(prefix)`**

Get sub-configuration with prefix.

#### Modification Operations

**`set(key, value)`**

Set configuration value with validation.

**`delete(key)`**

Delete configuration key.

**`merge(other_config)`**

Merge another configuration.

#### File Operations

**`loadFromFile(filepath)`**

Load configuration from file.

**`saveToFile(filepath)`**

Save configuration to file.

**`toDict()`**

Convert configuration to dictionary.

**`fromDict(dict)`**

Create configuration from dictionary.

#### Validation Operations

**`validate()`**

Validate entire configuration.

**`validateKey(key)`**

Validate specific key.

**`getValidationErrors()`**

Get all validation errors.

**`getValidationWarnings()`**

Get all validation warnings.

#### Special Operations

**`copy()`**

Create copy of configuration.

**`clear()`**

Clear all configuration values.

**`reset()`**

Reset to defaults.

**`getConflictingKeys(other_config)`**

Find conflicting keys with another configuration.

**`getOverriddenKeys(other_config)`**

Find overridden keys from another configuration.

## Configuration Tips

### Configuration Best Practices

1. **Use Constants for Named Parameters**

```python
# Good - named constants
NEURON_COUNT = "brain.neuron_count"
SYNAPSE_DENSITY = "brain.synapse_density"

# Set once, use repeatedly
config.set(NEURON_COUNT, 1000)
```

2. **Validate Before Setting**

```python
# Validate before modification
if config.has("brain.neuron_count"):
    old_value = config.get("brain.neuron_count")
    new_value = 2000
    
    # Check if modification is valid
    if is_valid_neuron_count(new_value):
        config.set("brain.neuron_count", new_value)
    else:
        raise ValueError(f"Invalid neuron count: {new_value}")
```

3. **Use Configuration Templates**

```python
# Create reusable templates
DEFAULT_BRAIN_CONFIG = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0
    }
}

# Apply template to create configuration
brain_config = {**DEFAULT_BRAIN_CONFIG, **custom_changes}
```

4. **Configuration Serialization**

```python
# Save configuration for reproducibility
config.save_to_file("experiment_1.json")

# Load configuration later
new_config = pynlm.createConfigFromFile("experiment_1.json")
```

### Performance Optimization

1. **Batch Configuration Updates**

```python
# Update multiple values at once
updates = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True
}

for key, value in updates.items():
    config.set(key, value)
```

2. **Use Local Variables for Frequent Access**

```python
# Cache frequently accessed values
neuron_count = config.get("brain.neuron_count")
synapse_density = config.get("brain.synapse_density")
learning_rate = config.get("plasticity.stdp.learning_rate")

# Use cached values in loops
for i in range(100):
    # Use neuron_count, synapse_density, learning_rate
```

3. **Configuration Profiles**

```python
# Define configuration profiles for different use cases
PROFILES = {
    "fast": {
        "performance.threads": 1,
        "performance.sparse_connectivity": True,
        "brain.neuron_count": 500
    },
    "balanced": {
        "performance.threads": 2,
        "performance.sparse_connectivity": True,
        "brain.neuron_count": 1000
    },
    "detailed": {
        "performance.threads": 4,
        "performance.sparse_connectivity": False,
        "brain.neuron_count": 5000
    }
}

# Use profile
def get_config_for_profile(profile_name):
    base_config = pynlm.createDefaultConfig()
    profile = PROFILES.get(profile_name, {})
    
    for key, value in profile.items():
        base_config.set(key, value)
        
    return base_config
```

## Configuration Troubleshooting

### Common Issues and Solutions

1. **Configuration Not Applied**

**Problem:** Configuration changes not taking effect.

**Solution:**

```python
# Issue: Configuration not applied before brain creation
brain = pynlm.createBrain(config)  # Created with old config
config.set("brain.neuron_count", 2000)  # Change too late

# Solution: Configure before creation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)  # Correct order
```

2. **Invalid Configuration Values**

**Problem:** Invalid configuration values causing runtime errors.

**Solution:**

```python
# Issue: Out of range values
config.set("brain.neuron_count", 50000)  # Too high

# Solution: Validate before setting
try:
    config.set("brain.neuron_count", 2000)
except ValueError as e:
    print(f"Configuration error: {e}")
```

3. **File I/O Errors**

**Problem:** Configuration file read/write failures.

**Solution:**

```python
# Issue: File permissions or path issues
try:
    config.save_to_file("/readonly/config.json")
except IOError as e:
    print(f"File error: {e}")
    # Use relative path or check permissions
    config.save_to_file("./config.json")
```

4. **Memory Issues**

**Problem:** Configuration memory usage too high.

**Solution:**

```python
# Issue: Too many configuration parameters
config = pynlm.createDefaultConfig()

# Solution: Use only necessary parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
# Only set essential parameters
```

### Debugging Configuration

```python
# Debug current configuration
print("Current configuration:")
print(config.toDict())

# Check specific keys
print(f"Brain neuron count: {config.get('brain.neuron_count')}")
print(f"Has STDP: {config.get('plasticity.stdp.enable')}")

# Validate configuration
errors = config.validate()
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
```

## Configuration Future Development

### Planned Enhancements

1. **Configuration Schema Validation**

2. **Dynamic Configuration**

3. **Configuration Templates**

4. **Advanced Validation Rules**

5. **Configuration Versioning**

### Configuration Research Directions

- **Configuration-driven Learning**: Automatically discover optimal configurations
- **Hierarchical Configuration**: Multi-level configuration inheritance
- **Configuration Optimization**: Automated configuration tuning
- **Configuration Visualization**: Graphical configuration editors

## Index

- Configuration Structure: 6-11
- Brain Configuration: 13-73
- Plasticity Configuration: 75-140
- Neuromodulation Configuration: 142-177
- Memory Configuration: 179-218
- Prediction Configuration: 220-234
- Development Configuration: 236-254
- Environment Configuration: 256-270
- World Configuration: 272-282
- Performance Configuration: 284-291
- Logging Configuration: 293-301
- Experiment Configuration: 303-311
- Configuration Validation: 313-340
- Configuration Inheritance: 342-352
- Configuration Examples: 354-410
- Configuration File Format: 412-421
- Configuration API Reference: 423-450
- Configuration Tips: 452-480
- Configuration Troubleshooting: 482-506
- Configuration Future Development: 508-520

---

*Configuration Documentation Version 1.0*
*Phase 6 - Final Integration*
*Last Updated: 2026-09-05*