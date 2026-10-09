# Configuration Guide

This document provides comprehensive documentation for NLM configuration parameters. Configuration controls all aspects of neural simulation, from basic neuron properties to advanced learning mechanisms.

## Overview

NLM uses a hierarchical configuration system with categories:
- **brain** - Core neural simulation parameters
- **plasticity** - Learning and plasticity mechanisms
- **neuromod** - Neuromodulation and motivation
- **development** - Developmental processes
- **memory** - Memory systems
- **prediction** - Prediction and cognition
- **performance** - Performance tuning

Configuration can be set through:
- Code using `Config` class methods
- Configuration files (JSON, YAML)
- Command-line arguments
- Default configuration

## Configuration Structure

### Root Configuration
```python
config = pynlm.createDefaultConfig()
```

### Hierarchical Keys
Configuration uses dot notation for nested parameters:
- `brain.neuron_count` - Total neurons
- `plasticity.stdp.enable` - STDP plasticity
- `neuromod.curiosity.scale` - Neuromodulator scale

## Configuration Categories

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity probability |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |
| `brain.lif_type` | string | "standard" | LIF variant (standard, adaptive, exponential) |

#### Brain Configuration Examples
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify brain parameters
config.set("brain.neuron_count", 5000)           # 5000 neurons
config.set("brain.synapse_density", 0.15)       # 15% connectivity
config.set("brain.v_thresh", -60.0)             # Lower threshold
config.set("brain.tau_mem", 15.0)               # Faster membrane
config.set("brain.tau_ref", 5.0)                # Longer refractory
```

### Plasticity Configuration

#### STDP (Spike-Timing-Dependent Plasticity)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for LTP |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for LTD |
| `plasticity.stdp.max_weight` | float | 1.0 | Maximum synaptic weight |
| `plasticity.stdp.min_weight` | float | 0.0 | Minimum synaptic weight |
| `plasticity.stdp.eligibility_window` | float | 50.0 | Eligibility trace window (ms) |

#### Hebbian Learning

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.hebbian.decay_rate` | float | 0.0001 | Weight decay rate |
| `plasticity.hebbian.correlation_window` | float | 10.0 | Correlation time window (ms) |

#### Structural Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synapse_prob` | float | 0.01 | Probability of forming new synapse |
| `plasticity.structural.prune_threshold` | float | 0.01 | Weight threshold for pruning |
| `plasticity.structural.max_synapses_per_neuron` | int | 100 | Maximum synapses per neuron |
| `plasticity.structural.min_synapses_per_neuron` | int | 1 | Minimum synapses per neuron |

#### Developmental Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.development.base_rate` | float | 0.1 | Base plasticity rate |
| `plasticity.development.maturation_rate` | float | 0.05 | Maturation plasticity rate |
| `plasticity.development.pruning_rate` | float | 0.02 | Synaptic pruning rate |
| `plasticity.development.synapse_grow_rate` | float | 0.03 | Synapse growth rate |

### Neuromodulation Configuration

#### Dopamine

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.phasicity` | bool | true | Enable phasic dopamine |
| `neuromod.dopamine.tone` | bool | false | Enable tonic dopamine |
| `neuromod.dopamine.prediction_error_weight` | float | 1.0 | Prediction error weighting |

#### Curiosity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.scale` | float | 1.0 | Curiosity intensity |
| `neuromod.curiosity.noise_level` | float | 0.1 | Exploration noise level |
| `neuromod.curiosity.adaptation_rate` | float | 0.01 | Curiosity adaptation rate |

#### Novelty Detection

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.5 | Novelty detection threshold |
| `neuromod.novelty.sensitivity` | float | 1.0 | Novelty sensitivity |
| `neuromod.novelty.memory_window` | float | 100.0 | Novelty memory window (ms) |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage` | string | "adult" | Current developmental stage |
| `development.plasticity_rate` | float | 0.1 | Overall plasticity rate |
| `development.critical_period` | bool | true | Enable critical period learning |
| `development.critical_period_strength` | float | 1.0 | Critical period learning strength |
| `development.maturation_rate` | float | 0.01 | Maturation rate |
| `development.synapse_grow_rate` | float | 0.02 | Synapse formation rate |
| `development.pruning_threshold` | float | 0.01 | Synapse pruning threshold |
| `development.pruning_rate` | float | 0.03 | Synapse pruning rate |

### Memory Configuration

#### Working Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.enable` | bool | true | Enable working memory |
| `memory.working.capacity` | int | 100 | Working memory capacity |
| `memory.working.decay_rate` | float | 0.1 | Working memory decay rate |
| `memory.working.stability_threshold` | float | 0.8 | Stability threshold |

#### Episodic Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.episodic.enable` | bool | true | Enable episodic memory |
| `memory.episodic.capacity` | int | 1000 | Episodic memory capacity |
| `memory.episodic.consolidation_interval` | float | 10.0 | Consolidation interval (ms) |
| `memory.episodic.consolidation_strength` | float | 0.5 | Consolidation strength |
| `memory.episodic.replay_probability` | float | 0.1 | Replay probability |

#### Concept Memory

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.concept.enable` | bool | true | Enable concept formation |
| `memory.concept.min_samples` | int | 10 | Minimum samples for concept |
| `memory.concept.distance_threshold` | float | 0.3 | Concept distance threshold |
| `memory.concept.stability_threshold` | float | 0.9 | Concept stability threshold |

### Prediction Configuration

#### Temporal Prediction

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.temporal.enable` | bool | true | Enable temporal prediction |
| `prediction.temporal.horizon` | int | 10 | Prediction horizon (steps) |
| `prediction.temporal.accuracy_threshold` | float | 0.7 | Prediction accuracy threshold |
| `prediction.temporal.memory_span` | int | 50 | Memory span for predictions |

#### Action Prediction

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.action.enable` | bool | true | Enable action prediction |
| `prediction.action.horizon` | int | 5 | Action prediction horizon |
| `prediction.action.accuracy_weight` | float | 1.0 | Action prediction accuracy weight |

### Performance Configuration

#### Simulation Performance

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.event_queue_size` | int | 16384 | Event queue size |
| `performance.time_buckets` | int | 1024 | Time bucket count for delays |
| `performance.max_delay` | int | 100 | Maximum spike delay (steps) |
| `performance.parallel_threads` | int | 0 | Number of parallel threads (0 = auto) |

#### Memory Performance

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.memory_pool_size` | int | 0 | Memory pool size in bytes (0 = auto) |
| `performance.cache_line_size` | int | 64 | Cache line size (bytes) |
| `performance.align_neuron_data` | bool | true | Align neuron data to cache lines |

## Configuration Files

### JSON Configuration
```json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.15,
        "v_thresh": -60.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 15.0,
        "tau_ref": 5.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0,
            "phasicity": true
        },
        "curiosity": {
            "enable": true,
            "scale": 1.0
        }
    },
    "development": {
        "stage": "adult",
        "plasticity_rate": 0.1,
        "critical_period": true
    }
}
```

### Loading Configuration from File
```python
import pynlm

# Create configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Or create configuration and apply file settings
config = pynlm.createDefaultConfig()
config.loadFromFile("experiment_config.json")

# Override with command line or runtime settings
config.set("brain.neuron_count", 10000)
```

### Saving Configuration
```python
import pynlm
import json

# Save configuration to file
config = pynlm.createDefaultConfig()
config.saveToFile("my_config.json")

# Get configuration as dictionary
config_dict = {}
def add_config_to_dict(prefix, section, target_dict):
    for key, value in section.items():
        full_key = f"{prefix}.{key}" if prefix else key
        if isinstance(value, dict):
            add_config_to_dict(full_key, value, target_dict)
        else:
            target_dict[full_key] = value

add_config_to_dict("", config, config_dict)

# Pretty print configuration
print(json.dumps(config_dict, indent=2, default=str))
```

## Configuration Presets

### Preset: Micro
```python
# Small-scale configuration for development
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", true)
config.set("neuromod.curiosity.enable", false)
config.set("development.stage", "initial")
```

### Preset: Small
```python
# Small-scale configuration for testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.curiosity.scale", 0.5)
config.set("development.stage", "critical_period")
```

### Preset: Medium
```python
# Medium-scale configuration for research
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.structural.enable", true)
config.set("neuromod.curiosity.scale", 1.0)
config.set("neuromod.novelty.enable", true)
config.set("development.stage", "adult")
```

### Preset: Large
```python
# Large-scale configuration for production
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100000)
config.set("brain.synapse_density", 0.2)
config.set("performance.event_queue_size", 65536)
config.set("performance.time_buckets", 2048)
config.set("performance.parallel_threads", 8)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)
config.set("neuromod.curiosity.scale", 1.5)
config.set("neuromod.novelty.enable", true)
config.set("development.stage", "adult")
```

## Configuration Validation

### Validate Configuration
```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()

# Check if configuration is valid
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration has errors:")
    for error in config.getErrors():
        print(f"  - {error}")
```

### Required Parameters
```python
# Check if required parameters are set
required_keys = [
    "brain.neuron_count",
    "brain.v_thresh",
    "brain.v_rest",
    "brain.v_reset",
    "plasticity.stdp.enable"
]

for key in required_keys:
    if config.has(key):
        print(f"✓ {key} is set")
    else:
        print(f"✗ {key} is required but not set")
```

### Configuration Constraints
```python
# Check configuration constraints
if config.has("brain.neuron_count"):
    neuron_count = config.getInt("brain.neuron_count")
    if neuron_count < 10:
        print("Warning: Very small neuron count may not be representative")
    elif neuron_count > 1000000:
        print("Warning: Very large neuron count may cause memory issues")

if config.has("plasticity.stdp.learning_rate"):
    stdp_rate = config.getFloat("plasticity.stdp.learning_rate")
    if stdp_rate < 0.0 or stdp_rate > 1.0:
        print("Error: STDP learning rate must be between 0.0 and 1.0")
```

## Configuration Optimization

### Optimize for Performance
```python
# Performance optimization
config = pynlm.createDefaultConfig()

# Enable performance features
config.set("performance.event_queue_size", 65536)
config.set("performance.time_buckets", 2048)
config.set("performance.max_delay", 100)
config.set("performance.parallel_threads", 0)  # Auto-detect

# Set neuron parameters for performance
config.set("brain.lif_type", "adaptive")
config.set("brain.tau_mem", 20.0)
config.set("brain.tau_ref", 2.0)

# Enable memory pools for large simulations
config.set("performance.memory_pool_size", 100 * 1024 * 1024)  # 100MB
config.set("performance.align_neuron_data", true)
```

### Optimize for Accuracy
```python
# Accuracy optimization
config = pynlm.createDefaultConfig()

# Enable all learning mechanisms
config.set("plasticity.stdp.enable", true)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)

# Set parameters for accurate learning
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.hebbian.learning_rate", 0.001)
config.set("plasticity.hebbian.decay_rate", 0.0001)

# Enable developmental learning
config.set("development.stage", "critical_period")
config.set("development.plasticity_rate", 0.2)
config.set("development.critical_period", true)
```

### Optimize for Memory
```python
# Memory optimization
config = pynlm.createDefaultConfig()

# Reduce memory usage
config.set("brain.synapse_density", 0.05)  # Lower connectivity
config.set("performance.event_queue_size", 8192)  # Smaller event queue
config.set("performance.time_buckets", 512)  # Fewer time buckets

# Enable efficient neuron types
config.set("brain.lif_type", "standard")  # Simpler than adaptive

# Disable unnecessary features
config.set("memory.episodic.enable", false)  # Disable if not needed
config.set("memory.concept.enable", false)   # Disable if not needed
config.set("prediction.temporal.enable", false)  # Disable if not needed
```

## Configuration Best Practices

### Naming Conventions
```python
# Use consistent naming
config.set("brain.neuron_count", 1000)           # snake_case
config.set("brain.synapse.density", 0.1)        # nested parameters
config.set("performance.event_queue.size", 1024) # descriptive names
```

### Documentation
```python
# Document configuration changes
config.set("brain.neuron_count", 5000)  # Changed from 1000 for this experiment
config.set("plasticity.stdp.learning_rate", 0.001)  # Standard STDP learning rate
```

### Version Control
```python
# Include configuration in experiments
import json
import datetime

experiment_config = {
    "timestamp": datetime.datetime.now().isoformat(),
    "config": config.to_dict(),  # If available
    "experiment_id": "exp_001",
    "description": "Test with 5000 neurons and STDP learning"
}

with open("experiment_config.json", "w") as f:
    json.dump(experiment_config, f, indent=2)
```

### Reproducibility
```python
# Ensure reproducible experiments
import random
import numpy as np

# Set random seeds for reproducibility
random.seed(42)
np.random.seed(42)

# Save configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.random_seed", 42)
config.saveToFile("reproducible_config.json")
```

## Configuration API Reference

### Config Class Methods

#### Creation
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create from file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
```

#### Query Methods
```python
# Check if key exists
if config.has("brain.neuron_count"):
    value = config.getInt("brain.neuron_count")

# Get keys
keys = config.getKeys()

# Check validation
if config.validate():
    print("Configuration is valid")
else:
    errors = config.getErrors()
    for error in errors:
        print(f"Error: {error}")
```

#### Modification Methods
```python
# Set values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", true)

# Get values
neuron_count = config.getInt("brain.neuron_count")
synapse_density = config.getFloat("brain.synapse_density")
stdp_enabled = config.getBool("plasticity.stdp.enable")
```

#### File I/O
```python
# Save configuration
config.saveToFile("config.json")

# Load configuration
config.loadFromFile("config.json")

# Get configuration as dictionary
config_dict = config.to_dict()
```

## Configuration Troubleshooting

### Common Issues

**Configuration Validation Errors**
```python
# Problem: Configuration validation fails
# Solution: Check for required parameters
required_keys = ["brain.neuron_count", "brain.v_thresh"]
for key in required_keys:
    if not config.has(key):
        print(f"Missing required key: {key}")
```

**Invalid Parameter Values**
```python
# Problem: Invalid parameter values cause errors
# Solution: Validate parameter ranges
if config.has("brain.v_thresh"):
    v_thresh = config.getFloat("brain.v_thresh")
    if v_thresh < -100.0 or v_thresh > 0.0:
        print(f"Warning: v_thresh {v_thresh} may be unrealistic")
```

**File I/O Errors**
```python
# Problem: Configuration file not found or invalid
# Solution: Handle file I/O gracefully
try:
    config.loadFromFile("config.json")
except FileNotFoundError:
    print("Configuration file not found, using defaults")
except json.JSONDecodeError:
    print("Invalid JSON in configuration file")
```

**Memory Issues**
```python
# Problem: Memory configuration causes issues
# Solution: Check memory constraints
if config.has("performance.memory_pool_size"):
    pool_size = config.getInt("performance.memory_pool_size")
    if pool_size > 1024 * 1024 * 1024:  # > 1GB
        print("Warning: Large memory pool may cause memory issues")
```

## Configuration Migration

### Migrate from Older Versions
```python
# Migration example for configuration changes between versions
old_config = pynlm.createDefaultConfig()

# Apply migration rules
new_config = pynlm.createDefaultConfig()

# Migrate brain configuration
if old_config.has("neuron_count"):
    new_config.set("brain.neuron_count", old_config.getInt("neuron_count"))

# Migrate plasticity configuration  
if old_config.has("stdp_enabled"):
    new_config.set("plasticity.stdp.enable", old_config.getBool("stdp_enabled"))

# Apply other migrations...
```

## Configuration Monitoring

### Configuration Monitoring
```python
# Monitor configuration changes
class ConfigMonitor:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.changes = []
    
    def set_parameter(self, key, value):
        old_value = self.config.get(key) if self.config.has(key) else None
        self.config.set(key, value)
        
        if old_value != value:
            self.changes.append({
                "key": key,
                "old_value": old_value,
                "new_value": value,
                "timestamp": datetime.datetime.now()
            })
    
    def get_changes(self):
        return self.changes
```

## Configuration Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create simple configuration for basic simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", false)  # Disable learning
config.set("neuromod.curiosity.enable", false)  # Disable motivation

brain = pynlm.createBrain(config)
brain.initialize()

# Run simple simulation
for step in range(100):
    brain.step(step)
```

### Example 2: Learning Brain
```python
import pynlm

# Create configuration for learning simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", true)
config.set("development.stage", "critical_period")
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.curiosity.scale", 1.0)

brain = pynlm.createBrain(config)
brain.initialize()

# Run learning simulation
for step in range(1000):
    # Simulate environment interaction
    # ... apply learning...
    brain.step(step)
```

### Example 3: Large-Scale Simulation
```python
import pynlm

# Create configuration for large-scale simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 50000)
config.set("brain.synapse_density", 0.15)
config.set("performance.event_queue_size", 32768)
config.set("performance.time_buckets", 1024)
config.set("performance.parallel_threads", 4)
config.set("performance.memory_pool_size", 200 * 1024 * 1024)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.structural.enable", true)
config.set("memory.episodic.enable", false)  # Save memory
config.set("memory.concept.enable", false)   # Save memory
config.set("prediction.temporal.enable", false)  # Save memory

brain = pynlm.createBrain(config)
brain.initialize()

# Run large-scale simulation
for step in range(5000):
    brain.step(step)
```

## Future Configuration Features

### Planned Enhancements

1. **YAML Support**: Add YAML configuration file support
2. **Configuration Schema**: Add JSON schema validation
3. **Dynamic Configuration**: Support for runtime configuration changes
4. **Configuration Templates**: Preset configuration templates
5. **Configuration Comparison**: Compare different configurations
6. **Configuration Sharing**: Share configurations between experiments

### Configuration API Evolution

The configuration API will continue to evolve with:

1. **New Categories**: Additional configuration categories
2. **Parameter Types**: New parameter types (arrays, objects)
3. **Validation Rules**: Advanced configuration validation
4. **Inheritance**: Configuration inheritance and overriding
5. **Variables**: Support for configuration variables

---

*Last updated: 2026-10-09*
*Documentation version: 0.1.0*