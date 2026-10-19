# NLM Configuration Reference Guide

## Overview

NLM uses a hierarchical key-value configuration system to control all aspects of the neural simulation. Configuration files are JSON-based and can be loaded from files or command line arguments.

**Configuration File Formats:**
- `.cfg` (custom NLM format)
- `JSON` (via file loading)
- Command line arguments

**Key Formats:**
- **`brain.neuron_count`**: Integer, e.g., `"brain.neuron_count": 1000`
- **`plasticity.stdp.learning_rate`**: Float, e.g., `"plasticity.stdp.learning_rate": 0.01`
- **`neuromod.dopamine.scale`**: Float, e.g., `"neuromod.dopamine.scale": 1.0`

---

## Configuration Categories

### 1. Core Brain Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `brain.neuron_count` | Integer | 1000 | Total number of neurons in the brain | 10 - 100,000 |
| `brain.region_count` | Integer | 2 | Number of neural regions | 1 - 20 |
| `brain.synapse_density` | Float | 0.1 | Connection probability | 0.01 - 0.5 |
| `brain.initial_weight_mean` | Float | 0.5 | Mean initial synaptic weight | 0.0 - 2.0 |
| `brain.initial_weight_std` | Float | 0.1 | Standard deviation of initial weights | 0.0 - 1.0 |

#### Neuron Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.v_thresh` | Float | -50.0 | Neuron threshold potential (mV) | -70.0 to -40.0 |
| `brain.v_rest` | Float | -70.0 | Resting potential (mV) | -80.0 to -60.0 |
| `brain.v_reset` | Float | -75.0 | Reset potential after spike (mV) | -80.0 to -70.0 |
| `brain.tau_mem` | Float | 20.0 | Membrane time constant (ms) | 10.0 - 50.0 |
| `brain.tau_ref` | Float | 2.0 | Refractory period (ms) | 1.0 - 10.0 |

### 2. Plasticity Configuration

#### STDP (Spike-Timing-Dependent Plasticity)

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `plasticity.stdp.enable` | Boolean | true | Enable STDP learning | true/false |
| `plasticity.stdp.learning_rate` | Float | 0.001 | STDP learning rate | 0.0001 - 0.1 |
| `plasticity.stdp.ltp_weight` | Float | 0.02 | Long-Term Potentiation weight | 0.0 - 0.1 |
| `plasticity.stdp.ltd_weight` | Float | 0.015 | Long-Term Depression weight | 0.0 - 0.1 |
| `plasticity.stdp.tau_plus` | Float | 20.0 | STDP time constant (ms) | 10.0 - 50.0 |
| `plasticity.stdp.tau_minus` | Float | 20.0 | STDP time constant (ms) | 10.0 - 50.0 |

#### Hebbian Learning

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `plasticity.hebbian.enable` | Boolean | true | Enable Hebbian learning | true/false |
| `plasticity.hebbian.learning_rate` | Float | 0.01 | Hebbian learning rate | 0.001 - 0.1 |

#### Structural Plasticity

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `plasticity.structural.enable` | Boolean | true | Enable structural plasticity | true/false |
| `plasticity.structural.synaptogenesis_rate` | Float | 0.0001 | Rate of new synapse formation | 0.00001 - 0.001 |
| `plasticity.structural.pruning_rate` | Float | 0.00001 | Rate of synapse elimination | 0.000001 - 0.0001 |

### 3. Neuromodulation Configuration

#### Dopamine System

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `neuromod.dopamine.scale` | Float | 1.0 | Dopamine modulation scale | 0.1 - 5.0 |
| `neuromod.dopamine.baseline` | Float | 0.0 | Baseline dopamine level | 0.0 - 1.0 |

#### Curiosity System

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `neuromod.curiosity.enable` | Boolean | true | Enable curiosity-driven exploration | true/false |
| `neuromod.curiosity.scale` | Float | 1.0 | Curiosity sensitivity | 0.1 - 5.0 |

#### Novelty Detection

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `neuromod.novelty.enable` | Boolean | true | Enable novelty detection | true/false |
| `neuromod.novelty.threshold` | Float | 0.5 | Novelty detection threshold | 0.0 - 1.0 |
| `neuromod.novelty.sensitivity` | Float | 1.0 | Novelty sensitivity | 0.1 - 5.0 |

### 4. Memory Configuration

#### Working Memory

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `memory.working.capacity` | Integer | 100 | Working memory capacity | 10 - 1000 |
| `memory.working.decay_rate` | Float | 0.01 | Working memory decay rate | 0.001 - 0.1 |

#### Episodic Memory

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `memory.episodic.capacity` | Integer | 50 | Episodic memory capacity | 5 - 500 |
| `memory.episodic.consolidation_threshold` | Float | 0.7 | Memory consolidation threshold | 0.1 - 1.0 |

### 5. Prediction System Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `prediction.model.order` | Integer | 5 | Prediction model order | 1 - 20 |
| `prediction.model.learning_rate` | Float | 0.01 | Prediction learning rate | 0.001 - 0.1 |
| `prediction.accuracy_threshold` | Float | 0.8 | Prediction accuracy threshold | 0.5 - 1.0 |

### 6. Development Configuration

#### Developmental Stages

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `development.stage.duration` | Float | 100.0 | Duration of each developmental stage | 10.0 - 1000.0 |
| `development.plasticity.initial_rate` | Float | 1.0 | Initial plasticity rate | 0.1 - 5.0 |
| `development.plasticity.final_rate` | Float | 0.1 | Final plasticity rate | 0.01 - 1.0 |

#### Synaptogenesis

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `development.synaptogenesis.rate` | Float | 0.001 | Synaptogenesis rate | 0.0001 - 0.01 |
| `development.synaptogenesis.threshold` | Float | 0.5 | Synaptogenesis threshold | 0.1 - 1.0 |

#### Pruning

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `development.pruning.rate` | Float | 0.0001 | Pruning rate | 0.00001 - 0.001 |
| `development.pruning.strength` | Float | 0.5 | Pruning strength | 0.1 - 1.0 |

### 7. Environment Configuration

#### World Settings

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `environment.world.width` | Integer | 10 | World width | 5 - 100 |
| `environment.world.height` | Integer | 10 | World height | 5 - 100 |
| `environment.world.vision_width` | Integer | 8 | Vision width | 4 - 32 |
| `environment.world.vision_height` | Integer | 8 | Vision height | 4 - 32 |
| `environment.world.max_energy` | Float | 100.0 | Maximum agent energy | 10.0 - 1000.0 |
| `environment.world.energy_decay_rate` | Float | 0.01 | Energy decay rate | 0.001 - 0.1 |

#### Object Types

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `environment.object.resource.value` | Float | 1.0 | Resource value | 0.1 - 10.0 |
| `environment.object.hazard.damage` | Float | 10.0 | Hazard damage | 1.0 - 50.0 |
| `environment.object.wall.resistance` | Float | 1.0 | Wall resistance | 0.1 - 10.0 |

### 8. Performance Configuration

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `performance.optimization.enable_simd` | Boolean | true | Enable SIMD optimizations | true/false |
| `performance.optimization.enable_parallel` | Boolean | true | Enable parallel processing | true/false |
| `performance.optimization.memory_pool_size` | Integer | 10000 | Memory pool size | 1000 - 100000 |
| `performance.optimization.event_queue_size` | Integer | 5000 | Event queue size | 1000 - 50000 |

### 9. Logging and Monitoring

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `logging.level` | String | INFO | Log level (DEBUG, INFO, WARNING, ERROR) | DEBUG, INFO, WARNING, ERROR |
| `logging.to_file` | Boolean | false | Log to file | true/false |
| `logging.filename` | String | nlm.log | Log filename | - |
| `logging.max_file_size` | Integer | 10485760 | Maximum log file size (bytes) | 1048576 - 100MB |

### 10. Simulation Control

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|-------------|
| `simulation.max_steps` | Integer | 10000 | Maximum simulation steps | 100 - 1000000 |
| `simulation.time_limit` | Float | 0.0 | Time limit in seconds (0 = unlimited) | 0.0 - 3600.0 |
| `simulation.checkpoint_interval` | Integer | 1000 | Checkpoint save interval | 100 - 100000 |
| `simulation.random_seed` | Integer | 42 | Random seed for reproducibility | 0 - 2^31-1 |

---

## Configuration File Examples

### Basic Configuration

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "synapse_density": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": true,
            "scale": 1.0
        }
    }
}
```

### Advanced Configuration

```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 5,
        "synapse_density": 0.15,
        "v_thresh": -55.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01,
            "ltp_weight": 0.03,
            "ltd_weight": 0.02
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.05
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.002,
            "pruning_rate": 0.0005
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 2.0,
            "baseline": 0.1
        },
        "curiosity": {
            "enable": true,
            "scale": 1.5
        },
        "novelty": {
            "enable": true,
            "threshold": 0.3,
            "sensitivity": 2.0
        }
    },
    "development": {
        "stage": {
            "duration": 200.0
        },
        "plasticity": {
            "initial_rate": 2.0,
            "final_rate": 0.5
        }
    },
    "environment": {
        "world": {
            "width": 20,
            "height": 20,
            "vision_width": 12,
            "vision_height": 12
        }
    },
    "performance": {
        "optimization": {
            "enable_simd": true,
            "enable_parallel": true
        }
    },
    "simulation": {
        "max_steps": 50000,
        "checkpoint_interval": 5000,
        "random_seed": 12345
    }
}
```

---

## Configuration Modification

### Programmatic Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify values
config.setInt("brain.neuron_count", 2000)
config.setFloat("plasticity.stdp.learning_rate", 0.01)
config.setString("neuromod.curiosity.scale", "1.5")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

### Command Line Configuration

```bash
# Using --config option
./nlm --config brain.neuron_count=5000 --config plasticity.stdp.learning_rate=0.01

# Using config file
./nlm --config=my_config.cfg
```

---

## Configuration Validation

### Validating Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 2000)

# Validate configuration
try:
    validation_result = config.validate()
    if validation_result.is_valid:
        print("Configuration is valid")
    else:
        print("Configuration errors:", validation_result.errors)
except Exception as e:
    print("Validation failed:", str(e))
```

### Common Configuration Issues

1. **Missing Required Parameters**: Some parameters are required for brain initialization
2. **Invalid Ranges**: Values outside acceptable ranges
3. **Type Mismatches**: String values where numbers expected
4. **Incompatible Combinations**: Conflicting configuration settings

---

## Configuration Best Practices

### For New Users

1. **Start with Default Configuration**: Use `createDefaultConfig()`
2. **Incremental Modifications**: Change one parameter at a time
3. **Test with Small Simulations**: Start with fewer neurons and shorter runs
4. **Save Working Configurations**: Save successful configurations for reproduction

### For Advanced Users

1. **Use Configuration Inheritance**: Load base configuration, then override specific values
2. **Batch Configuration Changes**: Use configuration templates for experiments
3. **Performance Tuning**: Optimize for specific hardware requirements
4. **Experiment Documentation**: Document parameter sweeps and results

### For Production Use

1. **Reproducible Experiments**: Set consistent random seeds
2. **Memory Management**: Monitor memory usage with large configurations
3. **Error Handling**: Implement robust configuration validation
4. **Backup and Restore**: Regular configuration backups

---

## Configuration Dependencies

### Required Parameters

The following parameters must be set for brain initialization:

- `brain.neuron_count`
- `brain.region_count`
- `brain.synapse_density`
- `plasticity.stdp.learning_rate`
- `neuromod.curiosity.enable`

### Optional Parameters

Most parameters have reasonable defaults, but can be overridden for specific experiments.

---

## Further Reading

- `docs/ARCHITECTURE.md` - System architecture and configuration
- `docs/SCIENCE.md` - Scientific background and parameter justification
- `examples/config_examples.py` - Practical configuration examples
- `python/bindings.cpp` - Configuration API implementation details

---

## Configuration Tools

### Configuration Validation Tool

```python
# In your project, add this validation utility
from typing import Dict, Any

class ConfigValidator:
    @staticmethod
    def validate_config(config: Dict[str, Any]) -> Dict[str, Any]:
        errors = []
        
        # Check required parameters
        if "brain.neuron_count" not in config:
            errors.append("Missing required parameter: brain.neuron_count")
            
        # Check parameter ranges
        if "brain.neuron_count" in config:
            neuron_count = config["brain.neuron_count"]
            if not (10 <= neuron_count <= 100000):
                errors.append(f"Invalid brain.neuron_count: {neuron_count}")
                
        return {
            "is_valid": len(errors) == 0,
            "errors": errors,
            "warnings": []
        }
```

### Configuration Generator

```python
import json
import random

def generate_random_config():
    config = {
        "brain": {
            "neuron_count": random.randint(500, 5000),
            "region_count": random.randint(2, 8),
            "synapse_density": random.uniform(0.05, 0.2)
        },
        "plasticity": {
            "stdp": {
                "learning_rate": random.uniform(0.0001, 0.01),
                "ltp_weight": random.uniform(0.01, 0.05),
                "ltd_weight": random.uniform(0.01, 0.05)
            }
        }
    }
    return config

# Generate and save random configuration
random_config = generate_random_config()
with open("random_config.json", "w") as f:
    json.dump(random_config, f, indent=2)
```

---

This comprehensive configuration reference guide provides everything you need to understand, create, and modify NLM configurations for any use case, from simple demonstrations to complex research experiments.