# Configuration Guide

## Overview

NLM uses JSON-based configuration files to control all aspects of brain simulation experiments. This guide provides comprehensive documentation of all configuration parameters, examples, and best practices.

## Configuration File Structure

NLM configuration files follow a hierarchical JSON structure with the following main sections:

```json
{
    "simulation": {
        "timestep": 0.001,
        "steps": 1000,
        "random_seed": 42
    },
    "brain": {
        "neuron_count": 1024,
        "region_count": 4,
        "plasticity_enabled": true
    },
    "memory": {
        "working_memory": {"capacity": 256},
        "episodic_memory": {"size": 1024}
    },
    "neuromodulation": {
        "dopamine": {"enabled": true, "strength": 0.8},
        "curiosity": {"enabled": true, "threshold": 0.3}
    },
    "environment": {
        "world_type": "SimpleWorld",
        "reward_structure": "delayed"
    }
}
```

## Configuration Categories

### 1. Simulation Parameters

#### simulation.timestep

**Type:** `float`  
**Default:** `0.001`  
**Range:** `(0.0, 1.0]`  
**Description:** Simulation time step in seconds. Controls the granularity of neural dynamics computation.

**Example:**
```json
"simulation": {
    "timestep": 0.0005,
    "steps": 5000,
    "random_seed": 12345
}
```

#### simulation.steps

**Type:** `integer`  
**Default:** `1000`  
**Range:** `[1, ∞)`  
**Description:** Number of simulation steps to execute.

**Example:**
```json
"simulation": {
    "timestep": 0.001,
    "steps": 10000,
    "random_seed": 42
}
```

#### simulation.random_seed

**Type:** `integer`  
**Default:** `42`  
**Range:** `[0, ∞)`  
**Description:** Random seed for reproducible experiments. Use same seed for identical results.

**Example:**
```json
"simulation": {
    "random_seed": 12345
}
```

#### simulation.parallel_enabled

**Type:** `boolean`  
**Default:** `false`  
**Description:** Enable parallel processing for performance optimization.

**Example:**
```json
"simulation": {
    "parallel_enabled": true,
    "simd_optimization": true
}
```

#### simulation.simd_optimization

**Type:** `boolean`  
**Default:** `false`  
**Description:** Enable SIMD vectorization for neural computations.

**Example:**
```json
"simulation": {
    "simd_optimization": true
}
```

### 2. Brain Configuration

#### brain.neuron_count

**Type:** `integer`  
**Default:** `1024`  
**Range:** `[16, ∞)`  
**Description:** Total number of neurons in the brain model. Higher values increase computational complexity.

**Example:**
```json
"brain": {
    "neuron_count": 4096,
    "region_count": 8,
    "plasticity_enabled": true
}
```

#### brain.region_count

**Type:** `integer`  
**Default:** `4`  
**Range:** `[1, 16]`  
**Description:** Number of brain regions. More regions enable complex topology.

**Example:**
```json
"brain": {
    "neuron_count": 1024,
    "region_count": 8,
    "plasticity_enabled": true
}
```

#### brain.plasticity_enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable/disable neural plasticity (learning capability).

**Example:**
```json
"brain": {
    "plasticity_enabled": true,
    "synaptic_density": 0.1
}
```

#### brain.synaptic_density

**Type:** `float`  
**Default:** `0.1`  
**Range:** `[0.0, 1.0]`  
**Description:** Proportion of potential connections that are actually established.

**Example:**
```json
"brain": {
    "synaptic_density": 0.2,
    "connection_range": 3
}
```

#### brain.connection_range

**Type:** `integer`  
**Default:** `1`  
**Range:** `[1, 5]`  
**Description:** Maximum distance for synaptic connections (in regions).

**Example:**
```json
"brain": {
    "connection_range": 3
}
```

### 3. Memory System Configuration

#### memory.working_memory.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable working memory system for temporary information maintenance.

**Example:**
```json
"memory": {
    "working_memory": {
        "enabled": true,
        "capacity": 256,
        "persistence_duration": 1000
    }
}
```

#### memory.working_memory.capacity

**Type:** `integer`  
**Default:** `256`  
**Range:** `[32, ∞)`  
**Description:** Maximum number of items working memory can hold simultaneously.

**Example:**
```json
"memory": {
    "working_memory": {
        "capacity": 512,
        "persistence_duration": 2000
    }
}
```

#### memory.working_memory.persistence_duration

**Type:** `integer`  
**Default:** `1000`  
**Range:** `[100, ∞)`  
**Description:** Duration in simulation steps that working memory items persist without reinforcement.

**Example:**
```json
"memory": {
    "working_memory": {
        "persistence_duration": 2000
    }
}
```

#### memory.episodic_memory.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable episodic memory for experience storage and replay.

**Example:**
```json
"memory": {
    "episodic_memory": {
        "enabled": true,
        "storage_capacity": 1024,
        "replay_frequency": 100
    }
}
```

#### memory.episodic_memory.storage_capacity

**Type:** `integer`  
**Default:** `1024`  
**Range:** `[256, ∞)`  
**Description:** Maximum number of experiences episodic memory can store.

**Example:**
```json
"memory": {
    "episodic_memory": {
        "storage_capacity": 2048
    }
}
```

#### memory.episodic_memory.replay_frequency

**Type:** `integer`  
**Default:** `100`  
**Range:** `[10, ∞)`  
**Description:** Number of steps between episodic memory replay events.

**Example:**
```json
"memory": {
    "episodic_memory": {
        "replay_frequency": 50
    }
}
```

#### memory.consolidation_interval

**Type:** `integer`  
**Default:** `1000`   
**Range:** `[100, ∞)`  
**Description:** Interval for memory consolidation operations.

**Example:**
```json
"memory": {
    "consolidation_interval": 500
}
```

### 4. Neuromodulation Configuration

#### neuromodulation.dopamine.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable dopamine neuromodulation system.

**Example:**
```json
"neuromodulation": {
    "dopamine": {
        "enabled": true,
        "strength": 0.8,
        "reward_delay": 10
    }
}
```

#### neuromodulation.dopamine.strength

**Type:** `float`  
**Default:** `0.8`  
**Range:** `[0.0, 1.0]`  
**Description:** Dopamine signal strength affecting plasticity magnitude.

**Example:**
```json
"neuromodulation": {
    "dopamine": {
        "strength": 0.9,
        "reward_delay": 5
    }
}
```

#### neuromodulation.dopamine.reward_delay

**Type:** `integer`  
**Default:** `10`  
**Range:** `[0, ∞)`  
**Description:** Delay between action and reward signal processing.

**Example:**
```json
"neuromodulation": {
    "dopamine": {
        "reward_delay": 5
    }
}
```

#### neuromodulation.curiosity.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable curiosity-driven exploration.

**Example:**
```json
"neuromodulation": {
    "curiosity": {
        "enabled": true,
        "threshold": 0.3,
        "exploration_bonus": 0.5
    }
}
```

#### neuromodulation.curiosity.threshold

**Type:** `float`  
**Default:** `0.3`  
**Range:** `[0.0, 1.0]`  
**Description:** Novelty threshold for curiosity activation.

**Example:**
```json
"neuromodulation": {
    "curiosity": {
        "threshold": 0.5
    }
}
```

#### neuromodulation.novelty.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable novelty detection system.

**Example:**
```json
"neuromodulation": {
    "novelty": {
        "enabled": true,
        "sensitivity": 0.7,
        "decay_rate": 0.95
    }
}
```

### 5. Plasticity Configuration

#### plasticity.stdp.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable Spike-Timing-Dependent Plasticity.

**Example:**
```json
"plasticity": {
    "stdp": {
        "enabled": true,
        "ltp_weight": 0.01,
        "ltd_weight": 0.012,
        "tau": 20.0
    }
}
```

#### plasticity.stdp.ltp_weight

**Type:** `float`  
**Default:** `0.01`  
**Range:** `[0.0, 1.0]`  
**Description:** Long-Term Potentiation weight change magnitude.

**Example:**
```json
"plasticity": {
    "stdp": {
        "ltp_weight": 0.015,
        "ltd_weight": 0.018
    }
}
```

#### plasticity.stdp.ltd_weight

**Type:** `float`  
**Default:** `0.012`  
**Range:** `[0.0, 1.0]`  
**Description:** Long-Term Depression weight change magnitude.

**Example:**
```json
"plasticity": {
    "stdp": {
        "ltd_weight": 0.014
    }
}
```

#### plasticity.stdp.tau

**Type:** `float`  
**Default:** `20.0`   
**Range:** `[1.0, 100.0]`  
**Description:** STDP time constant in milliseconds.

**Example:**
```json
"plasticity": {
    "stdp": {
        "tau": 15.0
    }
}
```

#### plasticity.hebbian.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable Hebbian learning.

**Example:**
```json
"plasticity": {
    "hebbian": {
        "enabled": true,
        "strength": 0.001
    }
}
```

#### plasticity.structural.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable structural plasticity (synaptogenesis/pruning).

**Example:**
```json
"plasticity": {
    "structural": {
        "enabled": true,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    }
}
```

#### plasticity.structural.synaptogenesis_rate

**Type:** `float`  
**Default:** `0.0001`  
**Range:** `[0.0, 0.001]`  
**Description:** Rate of new synapse formation.

**Example:**
```json
"plasticity": {
    "structural": {
        "synaptogenesis_rate": 0.00005
    }
}
```

#### plasticity.structural.pruning_rate

**Type:** `float`  
**Default:** `0.00001`  
**Range:** `[0.0, 0.0001]`  
**Description:** Rate of synapse removal.

**Example:**
```json
"plasticity": {
    "structural": {
        "pruning_rate": 0.000005
    }
}
```

### 6. Development Configuration

#### development.enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable developmental stages and maturation processes.

**Example:**
```json
"development": {
    "enabled": true,
    "stages": {
        "initial": {"duration": 100, "plasticity": 1.0},
        "critical_period": {"duration": 500, "plasticity": 0.5},
        "adult": {"duration": "infinite", "plasticity": 0.1}
    }
}
```

#### development.stages.initial.duration

**Type:** `integer`   
**Default:** `100`  
**Range:** `[10, ∞)`  
**Description:** Duration of initial developmental stage.

**Example:**
```json
"development": {
    "stages": {
        "initial": {"duration": 200}
    }
}
```

### 7. Environment Configuration

#### environment.world_type

**Type:** `string`  
**Default:** `"SimpleWorld"`  
**Values:** `SimpleWorld`, `ComplexWorld`, `CustomWorld`  
**Description:** Type of environment to simulate.

**Example:**
```json
"environment": {
    "world_type": "ComplexWorld",
    "reward_structure": "delayed",
    "visual_field": 120
}
```

#### environment.reward_structure

**Type:** `string`  
**Default:** `"immediate"`  
**Values:** `immediate`, `delayed`  
**Description:** Timing of reward signals relative to actions.

**Example:**
```json
"environment": {
    "reward_structure": "delayed"
}
```

#### environment.visual_field

**Type:** `integer`  
**Default:** `90`  
**Range:** `[0, 360]`  
**Description:** Visual field angle in degrees for sensory processing.

**Example:**
```json
"environment": {
    "visual_field": 120
}
```

### 8. Advanced Configuration

#### advanced.checkpoint_enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable checkpoint saving and loading.

**Example:**
```json
"advanced": {
    "checkpoint_enabled": true,
    "checkpoint_interval": 100,
    "checkpoint_compression": "gzip"
}
```

#### advanced.replay_enabled

**Type:** `boolean`  
**Default:** `true`  
**Description:** Enable episodic memory replay during rest/sleep phases.

**Example:**
```json
"advanced": {
    "replay_enabled": true,
    "replay_interval": 100,
    "consolidation_enabled": true
}
```

## Configuration Validation

### Required Parameters

| Category | Parameter | Type | Description |
|----------|-----------|------|-------------|
| simulation | timestep | float | Time step (0.0, 1.0] |
| simulation | steps | integer | Number of steps [1, ∞) |
| brain | neuron_count | integer | Neuron count [16, ∞) |
| memory | working_memory.capacity | integer | Working memory capacity [32, ∞) |

### Dependencies

- If `neuromodulation.dopamine.enabled` is true, `brain.plasticity_enabled` should also be true
- If `memory.episodic_memory.enabled` is true, `memory.consolidation_interval` must be specified
- If `development.enabled` is true, `development.stages.initial.duration` must be specified

## Configuration Examples

### Minimal Configuration

```json
{
    "simulation": {
        "timestep": 0.001,
        "steps": 100,
        "random_seed": 42
    },
    "brain": {
        "neuron_count": 256,
        "region_count": 2
    }
}
```

### Performance Optimized

```json
{
    "simulation": {
        "timestep": 0.0005,
        "steps": 5000,
        "random_seed": 12345,
        "parallel_enabled": true,
        "simd_optimization": true
    },
    "brain": {
        "neuron_count": 4096,
        "region_count": 8,
        "plasticity_enabled": true,
        "synaptic_density": 0.2
    },
    "memory": {
        "working_memory": {
            "capacity": 512,
            "persistence_duration": 2000
        },
        "episodic_memory": {
            "storage_capacity": 2048,
            "replay_frequency": 50
        }
    },
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "strength": 0.9,
            "reward_delay": 5
        },
        "curiosity": {
            "enabled": true,
            "threshold": 0.5
        }
    }
}
```

### Research Configuration

```json
{
    "experiment": {
        "name": "cognitive_development_study",
        "objectives": ["concept_formation", "planning_ability", "social_learning"],
        "duration": "long_term"
    },
    "development": {
        "enabled": true,
        "plasticity_schedule": {
            "initial": 1.0,
            "critical_period": 0.3,
            "adult": 0.1
        }
    },
    "cognition": {
        "attention_enabled": true,
        "planning_enabled": true,
        "self_model_enabled": true,
        "social_learning_enabled": true
    },
    "advanced": {
        "replay_enabled": true,
        "consolidation_enabled": true,
        "checkpoint_interval": 200
    }
}
```

## Configuration File Loading

### Command Line Loading

```bash
# Load specific configuration
nlm run 100 --config=../configs/experiment.json

# Merge with command line options
nlm run 1000 --config=config.json --verbose --output=results.json
```

### Configuration Inheritance

NLM supports configuration inheritance for modular experiment design:

```json
{
    "extends": "configs/base_config.json",
    "simulation": {
        "steps": 5000
    },
    "brain": {
        "neuron_count": 2048
    },
    "experiment": {
        "name": "my_experiment"
    }
}
```

## Configuration Best Practices

### 1. Performance Considerations

- Use smaller configurations for testing
- Enable parallel and SIMD optimizations for large simulations
- Balance neuron count with available memory
- Consider using sparse connectivity for large networks

### 2. Reproducibility

- Always specify random_seed for reproducible results
- Save final configuration after experiments
- Use version control for configuration files

### 3. Validation

- Validate configuration before running experiments
- Check for parameter conflicts
- Verify resource requirements before execution

### 4. Documentation

- Document experiment parameters and results
- Keep configuration files organized in config/ directory
- Use descriptive file names for different experiment types

## Configuration Files in Repository

### Default Configurations

- `configs/default.json`: Basic configuration for quick testing
- `configs/simple_world.json`: Simple environment setup
- `configs/advanced.json`: Full-featured configuration

### Experiment Configurations

- `configs/plasticity_study.json`: Plasticity dynamics research
- `configs/cognitive_development.json`: Cognitive skill development
- `configs/memory_efficiency.json`: Memory system optimization

### Development Configurations

- `configs/debug_config.json`: Configuration for debugging
- `configs/performance_config.json`: Performance optimization settings
- `configs/research_config.json`: Research-grade configurations

## Configuration Migration Guide

### From Version 1.x to 2.0

Configuration changes between versions:

| Parameter | Old Name | New Name | Notes |
|-----------|----------|----------|-------|
| simulation.step_size | simulation.timestep | simulation.timestep | Renamed for clarity |
| brain.neurons | brain.neuron_count | brain.neuron_count | Singular form |
| memory.wm_capacity | memory.working_memory.capacity | memory.working_memory.capacity | No change |

### Migration Steps

1. **Backup**: Copy existing configs to backup directory
2. **Convert**: Use configuration converter tool
3. **Validate**: Test converted configurations
4. **Test**: Run experiments with new configurations
5. **Monitor**: Ensure no regression in results

```bash
# Backup existing configs
cp -r configs configs_backup_$(date +%Y%m%d)

# Convert configurations
./convert_configs.py --from-version 1 --to-version 2

# Validate
./nlm --config=configs/new_config.json --validate
```

## Configuration Reference Summary

This guide covers configuration parameters organized by functional categories:

- **Core Simulation**: timestep, steps, random_seed, performance options
- **Brain Model**: neuron_count, region_count, plasticity, connectivity
- **Memory Systems**: working_memory, episodic_memory, consolidation
- **Neuromodulation**: dopamine, curiosity, novelty systems
- **Plasticity Rules**: STDP, Hebbian, structural plasticity
- **Development**: developmental stages and maturation processes
- **Environment**: world types and interaction models
- **Advanced**: checkpointing, replay, and optimization settings

For detailed parameter descriptions, examples, and validation rules, refer to the specific parameter sections above.

## License

Configuration files and documentation are licensed under the MIT License.

## Updates and Changes

Configuration documentation is updated with each major release. Check for updates when upgrading NLM versions.