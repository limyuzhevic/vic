# NLM Configuration Documentation

## Overview

The NLM configuration system provides comprehensive control over neural simulation parameters, plasticity rules, neuromodulation, development, and environmental settings. This documentation covers all available configuration options with practical examples for different use cases.

## Configuration System Architecture

The NLM configuration system uses a hierarchical key-value structure where:

- **Top-level categories** define major system components
- **Sub-keys** specify particular parameters within each component
- **Type conversion** automatically handles parameter types
- **Validation** ensures parameter values are within acceptable ranges
- **Presets** provide optimized configurations for common scenarios

```
CONFIGURATION HIERARCHY
├── brain                     # Neural system parameters
│   ├── neuron_count         # Number of neurons
│   ├── synapse_density      # Connection probability
│   └── learning_rate        # Plasticity strength
├── plasticity               # Learning mechanisms
│   ├── stdp                 # Spike-timing-dependent plasticity
│   └── hebbian             # Hebbian learning
├── neuromod                  # Neuromodulation signals
│   ├── dopamine           # Reward signals
│   └── curiosity          # Exploration drives
├── development              # Growth and maturation
│   ├── stage              # Current developmental stage
│   └── plasticity_rate    # Learning rate modulation
├── world                    # Environment parameters
│   ├── width              # World dimensions
│   └── max_energy         # Energy constraints
└── sensory                  # Sensory processing
    ├── vision             # Visual input
    └── audio             # Auditory input
```

## Configuration File Format

### JSON Configuration Files

NLM supports JSON configuration files with the following structure:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "learning_rate": 0.01
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.002
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        }
    },
    "world": {
        "width": 100.0,
        "height": 100.0,
        "max_energy": 1.0
    }
}
```

### Command Line Configuration

```bash
# Load configuration from file
cmake .. -DCONFIG_FILE=config.json

# Override specific parameters via command line
cmake .. -Dbrain.neuron_count=2000 -Dplasticity.stdp.learning_rate=0.005
```

## Configuration Categories

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the simulation |
| `brain.synapse_density` | float | 0.1 | Probability of connection between neurons (0.0-1.0) |
| `brain.connection_probability` | float | 0.05 | Alternative connection parameter |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Brain Configuration Examples

```python
import nlm

# Standard brain configuration
config = nlm.Config()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.v_thresh", -50.0)

# High-performance brain configuration
config = nlm.Config()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
config.set("brain.learning_rate", 0.005)

# Small brain for rapid prototyping
config = nlm.Config()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.2)
config.set("brain.v_thresh", -40.0)  # Higher threshold for faster firing
```

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable spike-timing-dependent plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for LTP |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for LTD |
| `plasticity.stdp.a_plus` | float | 0.1 | LTP magnitude |
| `plasticity.stdp.a_minus` | float | 0.12 | LTD magnitude |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.002 | Hebbian learning rate |
| `plasticity.hebbian.window_width` | float | 50.0 | Hebbian learning window width |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.formation_rate` | float | 0.001 | Rate of new synapse formation |
| `plasticity.structural.pruning_threshold` | float | 0.01 | Weight threshold for pruning |

#### Plasticity Configuration Examples

```python
import nlm

# Basic plasticity configuration
config = nlm.Config()
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.hebbian.learning_rate", 0.002)

# Advanced plasticity with structural changes
config = nlm.Config()
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.stdp.tau_plus", 15.0)
config.set("plasticity.stdp.tau_minus", 15.0)
config.set("plasticity.stdp.a_plus", 0.15)
config.set("plasticity.stdp.a_minus", 0.18)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.hebbian.learning_rate", 0.003)
config.set("plasticity.hebbian.window_width", 100.0)
config.set("plasticity.structural.enable", true)
config.set("plasticity.structural.formation_rate", 0.002)
config.set("plasticity.structural.pruning_threshold", 0.02)

# Production configuration (stable, no structural changes)
config = nlm.Config()
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.0005)
config.set("plasticity.hebbian.enable", false)
config.set("plasticity.structural.enable", false)
```

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.dopamine.learning_rate` | float | 0.05 | Dopamine learning rate |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.05 | Novelty detection threshold |
| `neuromod.novelty.decay_rate` | float | 0.95 | Novelty signal decay rate |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.exploration_bonus` | float | 0.5 | Exploration reward bonus |
| `neuromod.curiosity.exploitation_penalty` | float | -0.2 | Exploitation penalty |
| `neuromod.curiosity.learning_rate` | float | 0.03 | Curiosity learning rate |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error signals |
| `neuromod.prediction_error.sensitivity` | float | 0.8 | Prediction error sensitivity |

#### Neuromodulation Configuration Examples

```python
import nlm

# Basic neuromodulation
config = nlm.Config()
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.novelty.enable", true)
config.set("neuromod.curiosity.enable", true)

# Development-focused neuromodulation (high learning)
config = nlm.Config()
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.dopamine.learning_rate", 0.1)
config.set("neuromod.novelty.enable", true)
config.set("neuromod.novelty.threshold", 0.1)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.curiosity.exploration_bonus", 1.0)

# Production neuromodulation (stable)
config = nlm.Config()
config.set("neuromod.dopamine.scale", 0.5)
config.set("neuromod.dopamine.learning_rate", 0.01)
config.set("neuromod.novelty.enable", false)  # No novelty in stable mode
config.set("neuromod.curiosity.enable", false)  # No exploration
```

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage` | int | 2 (Maturation) | Current developmental stage |
| `development.plasticity_rate` | float | 1.0 | Multiplier for plasticity rates |
| `development.critical_period` | bool | true | Enable critical period learning |
| `development.maturation_speed` | float | 0.1 | Speed of developmental progression |

#### Development Configuration Examples

```python
import nlm

# Development configuration (CriticalPeriod for rapid learning)
config = nlm.Config()
config.set("development.stage", nlm.DevelopmentalStage.CriticalPeriod)
config.set("development.plasticity_rate", 2.0)
config.set("development.critical_period", true)

# Standard development (Maturation phase)
config = nlm.Config()
config.set("development.stage", nlm.DevelopmentalStage.Maturation)
config.set("development.plasticity_rate", 1.0)

# Aging configuration (reduced plasticity)
config = nlm.Config()
config.set("development.stage", nlm.DevelopmentalStage.Aging)
config.set("development.plasticity_rate", 0.3)
```

### World Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.width` | float | 100.0 | World width in simulation units |
| `world.height` | float | 100.0 | World height in simulation units |
| `world.max_energy` | float | 1.0 | Maximum agent energy |
| `world.energy_decay` | float | 0.01 | Energy decay rate per step |
| `world.visionWidth` | int | 10 | Vision sensor width |
| `world.visionHeight` | int | 10 | Vision sensor height |
| `world.visionAngle` | float | 180.0 | Vision field of view (degrees) |
| `world.randomSeed` | int | 0 | Random seed for reproducibility |

#### World Configuration Examples

```python
import nlm

# Standard world configuration
config = nlm.Config()
config.set("world.width", 100.0)
config.set("world.height", 100.0)
config.set("world.max_energy", 1.0)
config.set("world.energy_decay", 0.01)

# Small world for quick prototyping
config = nlm.Config()
config.set("world.width", 20.0)
config.set("world.height", 20.0)
config.set("world.max_energy", 2.0)
config.set("world.energy_decay", 0.05)

# Large exploration world
config = nlm.Config()
config.set("world.width", 500.0)
config.set("world.height", 500.0)
config.set("world.max_energy", 10.0)
config.set("world.energy_decay", 0.001)
```

### Sensory Configuration

#### Vision Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `sensory.vision.width` | int | 10 | Vision sensor width |
| `sensory.vision.height` | int | 10 | Vision sensor height |
| `sensory.vision.channels` | int | 1 | Number of visual channels |

#### Audio Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `sensory.audio.sample_rate` | int | 22050 | Audio sampling rate |
| `sensory.audio.samples` | int | 1024 | Number of audio samples |

#### Sensory Configuration Examples

```python
import nlm

# Standard sensory configuration
config = nlm.Config()
config.set("sensory.vision.width", 8)
config.set("sensory.vision.height", 8)
config.set("sensory.vision.channels", 1)

# High-resolution vision
config = nlm.Config()
config.set("sensory.vision.width", 16)
config.set("sensory.vision.height", 16)
config.set("sensory.vision.channels", 3)  # RGB vision

# High-fidelity audio
config = nlm.Config()
config.set("sensory.audio.sample_rate", 44100)
config.set("sensory.audio.samples", 2048)
```

## Configuration Presets

NLM provides several pre-configured presets for common use cases:

### Default Preset

Balanced configuration suitable for most applications:

```python
import nlm

# Balanced configuration for general use
config = nlm.ConfigPresets.createDefault()

# Equivalent to manual configuration
config = nlm.Config()
config.set("brain.neuron_count", 1000)
config.set("brain.learning_rate", 0.01)
config.set("brain.decay_rate", 0.95)
config.set("world.width", 100.0)
config.set("world.height", 100.0)
config.set("world.max_energy", 1.0)
config.set("world.energy_decay", 0.01)
config.set("sensory.vision.width", 10)
config.set("sensory.vision.height", 10)
config.set("sensory.vision.channels", 1)
```

### Development Preset

Aggressive learning configuration for developmental studies:

```python
import nlm

# Development-focused configuration (high plasticity)
config = nlm.ConfigPresets.createDevelopment()

# Key characteristics:
# - Higher learning rate (0.05)
# - All plasticity mechanisms enabled
# - Higher energy capacity (2.0)
# - Sensitive to novelty and curiosity
```

### Production Preset

Stable configuration for production environments:

```python
import nlm

# Production configuration (stable, conservative)
config = nlm.ConfigPresets.createProduction()

# Key characteristics:
# - Conservative learning rate (0.001)
# - STDP enabled, Hebbian and structural plasticity disabled
# - Fixed development (adult stage)
# - No curiosity-driven exploration
# - Standard energy constraints
```

### Exploration Preset

High plasticity configuration for exploration studies:

```python
import nlm

# Exploration-focused configuration (high plasticity)
config = nlm.ConfigPresets.createExploration()

# Key characteristics:
# - Very high learning rate (0.1)
# - All plasticity mechanisms enabled
# - High energy capacity (5.0)
# - High curiosity and novelty sensitivity
# - Slow energy decay (0.05)
```

## Configuration Validation

### Parameter Validation

NLM validates configuration parameters to ensure they're within acceptable ranges:

```python
import nlm

# Valid configurations
config = nlm.Config()
config.set("brain.neuron_count", 1000)  # Valid
config.set("brain.synapse_density", 0.5)  # Valid
config.set("world.max_energy", 10.0)  # Valid

# Invalid configurations (will raise exceptions)
# config.set("brain.neuron_count", 0)  # Too small
# config.set("brain.synapse_density", 1.5)  # > 1.0
# config.set("world.max_energy", 1000.0)  # Unrealistically high
```

### File Validation

```python
import nlm

# Load and validate configuration from file
config = nlm.Config()
if config.load_from_file("my_config.json"):
    print("Configuration loaded successfully")
    # Configuration is now validated
else:
    print("Failed to load configuration")
```

## Configuration Best Practices

### 1. Use Presets as Starting Point

```python
# Start with a preset and modify as needed
config = nlm.ConfigPresets.createProduction()
config.set("brain.neuron_count", 2000)  # Scale up
config.set("neuromod.curiosity.enable", true)  # Add curiosity
```

### 2. Maintain Reproducibility

```python
import nlm

# Set random seed for reproducibility
config = nlm.Config()
config.set("world.randomSeed", 42)

# Create brain with reproducible configuration
brain = nlm.Brain(config)
brain.initialize()

# Same configuration will produce same results
brain2 = nlm.Brain(config)
brain2.initialize()
```

### 3. Scale Appropriately

```python
# Scale configuration parameters appropriately
base_config = nlm.ConfigPresets.createDefault()

# Scale to larger network
base_config.set("brain.neuron_count", base_config.get("brain.neuron_count") * 5)
base_config.set("world.width", base_config.get("world.width") * 2)
base_config.set("world.height", base_config.get("world.height") * 2)
```

### 4. Balance Performance and Complexity

```python
# Trade-off between performance and biological realism
# Fast simulation (small brain)
config = nlm.ConfigPresets.createProduction()
config.set("brain.neuron_count", 500)
config.set("world.width", 50.0)

# Detailed simulation (larger brain)
config = nlm.ConfigPresets.createDevelopment()
config.set("brain.neuron_count", 5000)
config.set("world.width", 200.0)
config.set("world.height", 200.0)
```

## Configuration Tools

### Configuration Builder

```python
import nlm
from nlm import ConfigBuilder

# Use configuration builder for complex setup
builder = ConfigBuilder()
builder.brain(neurons=2000, density=0.05)
builder.plasticity(stdp=True, hebbian=True, structural=True)
builder.neuromod(dopamine_scale=1.5, curiosity=True)
builder.world(width=150, height=150, energy=2.0)
builder.sensory(vision_width=16, vision_height=16)

config = builder.build()
```

### Configuration Validation Tool

```python
import nlm

# Validate configuration
config = nlm.Config()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", true)

# Check for required parameters
required_params = ["brain.neuron_count", "plasticity.stdp.enable"]
missing_params = config.validate_required_params(required_params)

if missing_params:
    print(f"Missing required parameters: {missing_params}")
else:
    print("Configuration is valid")
```

## Configuration Migration

### Upgrading from Configuration Version 1.0 to 2.0

1. **Parameter Renames**
   - Old: `neurons` → New: `brain.neuron_count`
   - Old: `learning_rate` → New: `brain.learning_rate`
   - Old: `world_size` → New: `world.width`, `world.height`

2. **New Parameters**
   - `plasticity.stdp.tau_plus` (STDP time constant)
   - `neuromod.curiosity.exploration_bonus` (curiosity reward)
   - `development.stage` (developmental stage)

3. **Example Migration**

```python
# Old configuration (Version 1.0)
old_config = {
    "neurons": 1000,
    "learning_rate": 0.01,
    "world_size": 100.0,
    "enable_stdp": true,
    "enable_curiosity": true
}

# New configuration (Version 2.0)
new_config = nlm.Config()
new_config.set("brain.neuron_count", old_config["neurons"])
new_config.set("brain.learning_rate", old_config["learning_rate"])
new_config.set("world.width", old_config["world_size"])
new_config.set("world.height", old_config["world_size"])
new_config.set("plasticity.stdp.enable", old_config["enable_stdp"])
new_config.set("neuromod.curiosity.enable", old_config["enable_curiosity"])
```

## Configuration Testing

### Unit Testing Configuration

```python
import nlm
import unittest

class TestConfiguration(unittest.TestCase):
    def test_brain_configuration(self):
        config = nlm.Config()
        config.set("brain.neuron_count", 1000)
        self.assertEqual(config.get("brain.neuron_count"), 1000)
    
    def test_plasticity_configuration(self):
        config = nlm.Config()
        config.set("plasticity.stdp.enable", true)
        self.assertTrue(config.get("plasticity.stdp.enable"))
    
    def test_world_configuration(self):
        config = nlm.Config()
        config.set("world.width", 100.0)
        self.assertEqual(config.get("world.width"), 100.0)
    
    def test_configuration_validation(self):
        config = nlm.Config()
        config.set("brain.neuron_count", 1000)
        self.assertTrue(config.has("brain.neuron_count"))

# Run tests
if __name__ == "__main__":
    unittest.main()
```

### Integration Testing with Brain Creation

```python
import nlm

def test_configuration_with_brain():
    # Create configuration
    config = nlm.ConfigPresets.createDevelopment()
    config.set("brain.neuron_count", 500)
    
    # Create brain with configuration
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Verify configuration was applied
    assert brain.get_total_neuron_count() == 500
    print("Configuration successfully applied to brain")

def test_configuration_with_world():
    # Create configuration
    config = nlm.Config()
    config.set("world.width", 50.0)
    config.set("world.height", 50.0)
    
    # Create world with configuration
    world = nlm.create_simple_world()
    world.configure(**config.get_world_config())
    
    # Verify configuration was applied
    assert world.getWidth() == 50.0
    assert world.getHeight() == 50.0
    print("Configuration successfully applied to world")

if __name__ == "__main__":
    test_configuration_with_brain()
    test_configuration_with_world()
```

## Configuration Performance Tips

### Optimizing Configuration Loading

```python
import nlm

# Load configuration once and reuse
config = nlm.Config()
config.load_from_file("production_config.json")

# Create multiple brains with same configuration
brains = [nlm.Brain(config) for _ in range(10)]
for brain in brains:
    brain.initialize()
```

### Configuration Memory Efficiency

```python
import nlm
import gc

# Clean up configuration objects to free memory
config = nlm.Config()
config.set("brain.neuron_count", 1000)

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Delete configuration reference to free memory
# (Note: Brain still holds reference to config)
del config
gc.collect()
```

## Configuration Reference Summary

### Core Configuration Categories

| Category | Key Pattern | Required | Default |
|----------|-------------|----------|---------|
| `brain` | `brain.neuron_count` | Yes | 1000 |
| `plasticity` | `plasticity.stdp.enable` | No | true |
| `neuromod` | `neuromod.dopamine.scale` | No | 1.0 |
| `development` | `development.stage` | No | 2 (Maturation) |
| `world` | `world.width`, `world.height` | Yes | 100.0, 100.0 |
| `sensory` | `sensory.vision.width` | No | 10 |

### Configuration Validation Rules

- **brain.neuron_count**: 1-100000
- **brain.synapse_density**: 0.0-1.0
- **world.max_energy**: 0.1-1000.0
- **world.width**, **world.height**: 1.0-10000.0
- **development.stage**: 0-4 (DevelopmentalStage enum)

This configuration documentation provides comprehensive coverage of NLM's configuration system, enabling users to create optimized simulations for their specific research or application needs.
