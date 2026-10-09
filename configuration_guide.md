# configuration_guide.md

# NLM Configuration Guide

## Overview

NLM uses a hierarchical configuration system based on key-value pairs with dotted keys for nesting. Configuration controls all aspects of the brain including neural properties, learning mechanisms, memory systems, and neuromodulation.

## Configuration Structure

The configuration system uses dotted keys to represent nested structures:

```cpp
config->set("brain.neuron_count", size_t(1000));
config->set("plasticity.stdp.learning_rate", 0.001f);
config->set("neuromod.dopamine.scale", 1.0f);
```

## Configuration Categories

### 1. Brain Configuration (src/core/Config.hpp)

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

### 2. Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.0001 | Rate of synapse removal |

### 3. Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.threshold` | float | 0.5 | Surprise threshold |

### 4. Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory.capacity` | int | 100 | Working memory capacity |
| `memory.working_memory.decay_rate` | float | 0.01 | Decay rate for working memory |
| `memory.episodic_memory.max_episodes` | int | 1000 | Maximum episodes in episodic memory |
| `memory.episodic_memory.replay_interval` | int | 100 | Steps between replays |
| `memory.episodic_memory.consolidation_interval` | int | 1000 | Steps between consolidation |
| `memory.associative_memory.association_strength` | float | 0.5 | Strength of associations |

### 5. Prediction Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.horizon` | int | 3 | Prediction horizon in steps |
| `prediction.sequence_memory_size` | int | 50 | Size of sequence memory |
| `prediction.enable_temporal_prediction` | bool | true | Enable temporal prediction |
| `prediction.enable_action_consequence` | bool | true | Enable action-consequence prediction |

### 6. Cognitive Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.planning.depth` | int | 2 | Planning depth (steps ahead) |
| `cognition.attention.competition_strength` | float | 1.0 | Strength of attentional competition |
| `cognition.concept.formation_threshold` | float | 0.7 | Threshold for concept formation |
| `cognition.concept.stability_window` | int | 10 | Window for stability measurement |

### 7. Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.plasticity_rate` | float | 1.0 | Overall plasticity rate |
| `development.excitability_baseline` | float | 0.5 | Baseline neural excitability |
| `development.attention_maturation_rate` | float | 0.01 | Rate of attention improvement |
| `development.memory_maturation_rate` | float | 0.01 | Rate of memory improvement |
| `development.neuromodulation_rate` | float | 0.01 | Rate of neuromodulation development |

### 8. Performance Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.checkpoint.interval` | int | 1000 | Steps between checkpoints |
| `performance.enable_multithreading` | bool | false | Enable parallel processing |
| `performance.simulation_step_time` | float | 0.1 | Simulation step time (ms) |

## Configuration Methods

### Creating Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 2.0)

# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Get all keys
all_keys = config.getKeys()
print(f"Configuration keys: {all_keys}")
```

### Loading/Saving Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set some values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save to file
config.saveToFile("my_config.json")

# Load from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Load from command line arguments
import sys
config3 = pynlm.createDefaultConfig()
config3.loadFromArgs(len(sys.argv), sys.argv)
```

### Configuration Templates

#### Basic Configuration

```cpp
// Basic simulation configuration
config->set("brain.neuron_count", size_t(1000));
config->set("brain.synapse_density", 0.1f);
config->set("brain.v_thresh", -50.0f);
config->set("brain.v_rest", -70.0f);
config->set("brain.tau_mem", 20.0f);
```

#### Learning-Focused Configuration

```cpp
// Emphasize learning and plasticity
config->set("brain.neuron_count", size_t(2000));
config->set("plasticity.stdp.enable", true);
config->set("plasticity.stdp.learning_rate", 0.01f);
config->set("plasticity.hebbian.enable", true);
config->set("plasticity.hebbian.learning_rate", 0.005f);
config->set("plasticity.structural.enable", true);
config->set("neuromod.dopamine.scale", 1.5f);
```

#### Memory-Intensive Configuration

```cpp
// Emphasize memory systems
config->set("brain.neuron_count", size_t(3000));
config->set("memory.working_memory.capacity", 200);
config->set("memory.working_memory.decay_rate", 0.005f);
config->set("memory.episodic_memory.max_episodes", 5000);
config->set("memory.episodic_memory.replay_interval", 50);
config->set("memory.episodic_memory.consolidation_interval", 500);
```

#### Prediction-Focused Configuration

```cpp
// Emphasize prediction systems
config->set("brain.neuron_count", size_t(2500));
config->set("prediction.horizon", 5);
config->set("prediction.sequence_memory_size", 100);
config->set("prediction.enable_temporal_prediction", true);
config->set("prediction.enable_action_consequence", true);
config->set("cognition.planning.depth", 3);
```

#### Developmental Configuration

```cpp
// Emphasize development
config->set("brain.neuron_count", size_t(2000));
config->set("development.plasticity_rate", 1.2f);
config->set("development.excitability_baseline", 0.6f);
config->set("development.attention_maturation_rate", 0.02f);
config->set("development.memory_maturation_rate", 0.02f);
config->set("development.neuromodulation_rate", 0.02f);
```

## Configuration for Different Use Cases

### 1. Cognitive Research

```cpp
// Research on cognitive development
config->set("brain.neuron_count", size_t(3000));
config->set("development.plasticity_rate", 2.0f);  // Enhanced plasticity
config->set("development.attention_maturation_rate", 0.05f);
config->set("development.memory_maturation_rate", 0.05f);
config->set("memory.working_memory.capacity", 150);
config->set("memory.episodic_memory.max_episodes", 3000);
config->set("cognition.planning.depth", 4);
config->set("cognition.concept.formation_threshold", 0.6f);
```

### 2. Memory Studies

```cpp
// Research on memory systems
config->set("brain.neuron_count", size_t(2500));
config->set("memory.working_memory.capacity", 300);
config->set("memory.working_memory.decay_rate", 0.001f);
config->set("memory.episodic_memory.max_episodes", 10000);
config->set("memory.episodic_memory.replay_interval", 20);
config->set("memory.episodic_memory.consolidation_interval", 200);
config->set("prediction.horizon", 10);
```

### 3. Neuromodulation Research

```cpp
// Research on neuromodulation
config->set("brain.neuron_count", size_t(2000));
config->set("neuromod.dopamine.scale", 2.0f);
config->set("neuromod.curiosity.enable", true);
config->set("neuromod.novelty.enable", true);
config->set("neuromod.prediction_error.threshold", 0.3f);
config->set("development.neuromodulation_rate", 0.05f);
```

### 4. Prediction Systems

```cpp
// Research on prediction
config->set("brain.neuron_count", size_t(3000));
config->set("prediction.horizon", 10);
config->set("prediction.sequence_memory_size", 200);
config->set("cognition.planning.depth", 5);
config->set("cognition.attention.competition_strength", 1.5f);
```

### 5. Real-Time Simulation

```cpp
// Real-time or large-scale simulation
config->set("brain.neuron_count", size_t(5000));
config->set("performance.enable_multithreading", true);
config->set("performance.simulation_step_time", 0.05f);
config->set("performance.checkpoint.interval", 5000);
config->set("plasticity.stdp.learning_rate", 0.0001f);  // Slower learning
```

## Dynamic Configuration

### Runtime Configuration Changes

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Change configuration during simulation
# (Note: changing core parameters during simulation may cause issues)
config.set("neuromod.dopamine.scale", 2.0)

# Configuration for learning rates
config.set("plasticity.stdp.learning_rate", 0.01)

# Memory system tuning
config.set("memory.working_memory.capacity", 200)
config.set("memory.episodic_memory.max_episodes", 2000)
```

## Configuration Validation

### Common Configuration Issues

1. **Inconsistent Configuration**: Conflicting settings
   - Example: High learning rate with short simulation time
   - Solution: Check parameter interactions

2. **Memory Conflicts**: Insufficient memory for configured systems
   - Example: Large neuron count with high memory capacity
   - Solution: Check system resource constraints

3. **Development Timing**: Development too fast/slow for simulation
   - Example: Short simulation with long developmental stages
   - Solution: Match simulation time to development time scales

4. **Scale Mismatches**: Different systems operating on different time scales
   - Example: Fast STDP with slow development
   - Solution: Ensure consistent time scales

### Configuration Checking

```python
import pynlm

def validate_config(config):
    """Validate configuration for consistency."""
    errors = []
    
    # Check neuron count reasonable
    neuron_count = config.get("brain.neuron_count", 1000)
    if neuron_count < 100 or neuron_count > 10000:
        errors.append(f"Unreasonable neuron count: {neuron_count}")
    
    # Check plasticity rate reasonable
    stdp_rate = config.get("plasticity.stdp.learning_rate", 0.001)
    if stdp_rate < 0.000001 or stdp_rate > 0.1:
        errors.append(f"Extreme STDP rate: {stdp_rate}")
    
    # Check memory capacity
    wm_capacity = config.get("memory.working_memory.capacity", 100)
    if wm_capacity < 10 or wm_capacity > 1000:
        errors.append(f"Unreasonable working memory capacity: {wm_capacity}")
    
    # Check configuration consistency
    if config.get("development.plasticity_rate", 1.0) > 2.0 and stdp_rate < 0.000001:
        errors.append("High development plasticity with very low STDP rate")
    
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    else:
        print("Configuration validation passed")
        return True

# Example usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("development.plasticity_rate", 1.5)
validate_config(config)
```

## Configuration Files

### Example Configuration Files

#### config_basic.json

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
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
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    }
}
```

#### config_advanced.json

```json
{
    "brain": {
        "neuron_count": 3000,
        "synapse_density": 0.15,
        "v_thresh": -45.0,
        "v_rest": -65.0,
        "tau_mem": 15.0,
        "tau_ref": 3.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.005,
            "tau_plus": 25.0,
            "tau_minus": 25.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.002
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.002,
            "pruning_rate": 0.0002
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.5
        },
        "serotonin": {
            "enable": true,
            "scale": 1.0
        },
        "norepinephrine": {
            "enable": true,
            "scale": 1.2
        },
        "curiosity": {
            "enable": true,
            "novelty_weight": 0.7,
            "prediction_error_weight": 0.3
        },
        "novelty": {
            "enable": true,
            "decay_rate": 0.1
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 200,
            "decay_rate": 0.005
        },
        "episodic_memory": {
            "max_episodes": 5000,
            "replay_interval": 50,
            "consolidation_interval": 500
        }
    },
    "prediction": {
        "horizon": 5,
        "sequence_memory_size": 100,
        "enable_temporal_prediction": true,
        "enable_action_consequence": true
    },
    "cognition": {
        "planning": {
            "depth": 3
        },
        "concept": {
            "formation_threshold": 0.6,
            "stability_window": 20
        }
    }
}
```

## Configuration Performance Tips

### 1. Optimize for Memory

```cpp
// For memory-constrained systems
config->set("brain.neuron_count", size_t(500));
config->set("memory.working_memory.capacity", 50);
config->set("memory.episodic_memory.max_episodes", 100);
config->set("prediction.sequence_memory_size", 20);
```

### 2. Optimize for Speed

```cpp
// For fast simulation
config->set("brain.tau_mem", 10.0f);  // Faster membrane dynamics
config->set("plasticity.stdp.learning_rate", 0.0001f);  // Slower learning
config->set("performance.enable_multithreading", true);
config->set("performance.checkpoint.interval", 10000);
```

### 3. Optimize for Learning

```cpp
// For maximum learning
config->set("brain.neuron_count", size_t(5000));
config->set("plasticity.stdp.learning_rate", 0.01f);
config->set("plasticity.hebbian.learning_rate", 0.01f);
config->set("development.plasticity_rate", 2.0f);
config->set("neuromod.dopamine.scale", 2.0f);
```

### 4. Optimize for Memory

```cpp
// For memory research
config->set("brain.neuron_count", size_t(2000));
config->set("memory.working_memory.capacity", 300);
config->set("memory.working_memory.decay_rate", 0.001f);
config->set("memory.episodic_memory.max_episodes", 10000);
config->set("memory.episodic_memory.replay_interval", 20);
```

## Configuration Best Practices

1. **Start Simple**: Begin with default configuration and add modifications
2. **Test Thoroughly**: Validate configuration with intended use case
3. **Document Configuration**: Keep track of configuration changes
4. **Version Control**: Store configuration files in version control
5. **Parameter Ranges**: Use reasonable parameter ranges
6. **Consistency Check**: Ensure configuration parameters work together
7. **Performance Monitoring**: Monitor system performance with configuration

## Configuration Automation

### Configuration Generators

```python
import pynlm
import json

def create_research_config(research_type, parameters=None):
    """Create configuration for specific research types."""
    
    config = pynlm.createDefaultConfig()
    
    if research_type == "cognitive_development":
        config.set("brain.neuron_count", 3000)
        config.set("development.plasticity_rate", 2.0)
        config.set("development.attention_maturation_rate", 0.05)
        config.set("development.memory_maturation_rate", 0.05)
        config.set("memory.working_memory.capacity", 200)
        config.set("cognition.planning.depth", 4)
        config.set("cognition.concept.formation_threshold", 0.6)
        
    elif research_type == "memory_systems":
        config.set("brain.neuron_count", 2500)
        config.set("memory.working_memory.capacity", 300)
        config.set("memory.working_memory.decay_rate", 0.001)
        config.set("memory.episodic_memory.max_episodes", 5000)
        config.set("memory.episodic_memory.replay_interval", 50)
        
    elif research_type == "neuromodulation":
        config.set("brain.neuron_count", 2000)
        config.set("development.neuromodulation_rate", 0.05)
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
    elif research_type == "prediction":
        config.set("brain.neuron_count", 3000)
        config.set("prediction.horizon", 10)
        config.set("prediction.sequence_memory_size", 200)
        config.set("cognition.planning.depth", 5)
    
    return config

# Example usage
research_config = create_research_config("cognitive_development")
research_config.saveToFile("cognitive_research_config.json")
```

## Conclusion

Configuration is a critical aspect of NLM that determines system behavior, performance, and research capabilities. Proper configuration management enables:

1. **Reproducible Experiments**: Consistent configuration across runs
2. **System Optimization**: Tailored configuration for specific use cases
3. **Research Control**: Precise control over experimental parameters
4. **Performance Management**: Balanced configuration for resource constraints
5. **Development Studies**: Configuration that emphasizes developmental processes

The configuration system provides flexible, hierarchical management of all NLM parameters while maintaining simplicity for basic use and complexity for advanced research.
