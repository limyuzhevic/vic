# NLM Configuration Templates

This document provides configuration templates for different use cases of the Neural Learning Machine (NLM) Phase 6 system.

## Overview

NLM supports multiple configuration file formats:
- **Default** (`configs/default.cfg`) - Simple key=value format
- **JSON** (`configs/nlm_config.json`) - Modern structured configuration
- **YAML** (`configs/nlm_config.yaml`) - Human-readable configuration

## Configuration Schema

The NLM configuration follows a hierarchical structure with the following main categories:

### 1. Core Simulation Parameters
```json
{
    "simulation": {
        "steps": 10000,
        "timestep": 0.001,
        "seed": 42,
        "random_seed": 42
    }
}
```

### 2. Brain Architecture
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "use_structural_plasticity": true,
        "development_enabled": true
    }
}
```

### 3. Memory Systems
```json
{
    "memory": {
        "working_memory_size": 1000,
        "episodic_memory_capacity": 100,
        "association_strength_threshold": 0.5,
        "replay_enabled": true,
        "consolidation_frequency": "medium"
    }
}
```

### 4. Neuromodulation
```json
{
    "neuromodulation": {
        "dopamine_baseline": 0.5,
        "curiosity_gain": 0.1,
        "novelty_sensitivity": 0.3,
        "prediction_error_weight": 0.8,
        "reward_modulation_enabled": true
    }
}
```

### 5. Plasticity Rules
```json
{
    "plasticity": {
        "stdp_ltp_strength": 0.02,
        "stdp_ltd_strength": 0.015,
        "stdp_time_constant": 20.0,
        "hebbian_learning_rate": 0.01,
        "structural_plasticity_rate": 0.0001,
        "pruning_rate": 0.00001
    }
}
```

### 6. Development System
```json
{
    "development": {
        "stage_duration": {
            "stage1": 1000,
            "stage2": 2000,
            "stage3": 3000,
            "stage4": 4000
        },
        "plasticity_decay_rate": 0.9,
        "excitatory_inhibition_balance": 1.0,
        "adaptation_rate": 0.05
    }
}
```

## Configuration Templates

### Beginner Configuration (`configs/templates/beginner.cfg`)

```ini
# Simple configuration for getting started
brain.neuron_count=100
brain.region_count=1
brain.connection_probability=0.15
simulation.steps=500
simulation.timestep=0.001
simulation.seed=42
memory.replay_enabled=false
neuromodulation.reward_modulation_enabled=true
neuromodulation.dopamine_baseline=0.5
plasticity.stdp_ltp_strength=0.02
plasticity.stdp_ltd_strength=0.015
```

**Use Case**: Learning NLM basics, quick testing, educational purposes
**Performance**: Low (100 neurons, short simulation)
**Best For**: Beginners, tutorials, simple demonstrations

### Research Configuration (`configs/templates/research.cfg`)

```json
{
    "simulation": {
        "steps": 50000,
        "timestep": 0.0005,
        "seed": 12345,
        "random_seed": 12345
    },
    "brain": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.08,
        "use_structural_plasticity": true,
        "development_enabled": true
    },
    "memory": {
        "working_memory_size": 5000,
        "episodic_memory_capacity": 500,
        "association_strength_threshold": 0.3,
        "replay_enabled": true,
        "consolidation_frequency": "high"
    },
    "neuromodulation": {
        "dopamine_baseline": 0.7,
        "curiosity_gain": 0.2,
        "novelty_sensitivity": 0.4,
        "prediction_error_weight": 0.9,
        "reward_modulation_enabled": true
    },
    "plasticity": {
        "stdp_ltp_strength": 0.025,
        "stdp_ltd_strength": 0.02,
        "stdp_time_constant": 25.0,
        "hebbian_learning_rate": 0.02,
        "structural_plasticity_rate": 0.0002,
        "pruning_rate": 0.00002
    },
    "development": {
        "stage_duration": {
            "stage1": 5000,
            "stage2": 10000,
            "stage3": 15000,
            "stage4": 20000
        },
        "plasticity_decay_rate": 0.8,
        "excitatory_inhibition_balance": 1.2,
        "adaptation_rate": 0.03
    },
    "output": {
        "enable_checkpointing": true,
        "checkpoint_frequency": 1000,
        "export_format": "json",
        "enable_visualization": true
    }
}
```

**Use Case**: Research-grade simulations, scientific studies
**Performance**: High (5000 neurons, long simulation)
**Best For**: Research papers, data analysis, scientific validation

### Production Configuration (`configs/templates/production.cfg`)

```json
{
    "simulation": {
        "steps": 20000,
        "timestep": 0.001,
        "seed": 999,
        "random_seed": 999
    },
    "brain": {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.12,
        "use_structural_plasticity": true,
        "development_enabled": true
    },
    "memory": {
        "working_memory_size": 2000,
        "episodic_memory_capacity": 200,
        "association_strength_threshold": 0.6,
        "replay_enabled": true,
        "consolidation_frequency": "medium"
    },
    "neuromodulation": {
        "dopamine_baseline": 0.6,
        "curiosity_gain": 0.15,
        "novelty_sensitivity": 0.35,
        "prediction_error_weight": 0.85,
        "reward_modulation_enabled": true
    },
    "plasticity": {
        "stdp_ltp_strength": 0.02,
        "stdp_ltd_strength": 0.015,
        "stdp_time_constant": 20.0,
        "hebbian_learning_rate": 0.015,
        "structural_plasticity_rate": 0.00015,
        "pruning_rate": 0.000015
    },
    "performance": {
        "enable_vectorization": true,
        "use_memory_pools": true,
        "enable_parallel_processing": true,
        "optimize_for_speed": true,
        "memory_optimization_level": "balanced"
    },
    "output": {
        "enable_checkpointing": true,
        "checkpoint_frequency": 500,
        "export_format": "compressed",
        "enable_visualization": false
    }
}
```

**Use Case**: Production environments, deployments, automated systems
**Performance**: Balanced (2000 neurons, medium simulation)
**Best For**: Deployments, automated testing, monitoring systems

## Configuration File Format Reference

### 1. Default Config (`configs/default.cfg`)

```ini
# Basic NLM configuration
brain.neuron_count=500
brain.region_count=1
brain.connection_probability=0.15
simulation.timestep=0.001
simulation.steps=10000
memory.replay_enabled=true
neuromodulation.dopamine_baseline=0.5
plasticity.stdp_ltp_strength=0.02
plasticity.stdp_ltd_strength=0.015
development.enabled=true
```

### 2. JSON Config (`configs/nlm_config.json`)

```json
{
    "brain": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15
    },
    "simulation": {
        "timestep": 0.001,
        "steps": 10000,
        "seed": 42
    },
    "memory": {
        "replay_enabled": true,
        "working_memory_size": 500
    },
    "neuromodulation": {
        "dopamine_baseline": 0.5,
        "reward_modulation_enabled": true
    },
    "plasticity": {
        "stdp_ltp_strength": 0.02,
        "stdp_ltd_strength": 0.015
    },
    "development": {
        "enabled": true
    }
}
```

### 3. YAML Config (`configs/nlm_config.yaml`)

```yaml
brain:
  neuron_count: 500
  region_count: 1
  connection_probability: 0.15

simulation:
  timestep: 0.001
  steps: 10000
  seed: 42

memory:
  replay_enabled: true
  working_memory_size: 500

neuromodulation:
  dopamine_baseline: 0.5
  reward_modulation_enabled: true

plasticity:
  stdp_ltp_strength: 0.02
  stdp_ltd_strength: 0.015

development:
  enabled: true
```

## Command Line Configuration Override

Configuration can be overridden using command line arguments:

```bash
# Override specific parameters
nlm --simulate \
    --config configs/research.cfg \
    --steps 20000 \
    --neurons 1000 \
    --checkpoint my_experiment.bin

# Enable advanced features
nlm --simulate --neurons 2000 --steps 5000 --enable-replay --enable-development

# Load JSON configuration
nlm --simulate --config experiment_config.json

# Use YAML configuration
nlm --simulate --config experiment_config.yaml
```

## Configuration Validation

### Valid Parameter Ranges

| Parameter | Minimum | Maximum | Default | Description |
|-----------|---------|---------|---------|-------------|
| `brain.neuron_count` | 10 | 100000 | 1000 | Number of neurons in the brain |
| `brain.region_count` | 1 | 10 | 1 | Number of brain regions |
| `brain.connection_probability` | 0.01 | 0.5 | 0.1 | Probability of synapse creation |
| `simulation.timestep` | 0.0001 | 0.1 | 0.001 | Simulation time step (seconds) |
| `simulation.steps` | 100 | 1000000 | 10000 | Total simulation steps |
| `memory.replay_enabled` | false | true | true | Enable memory replay system |
| `plasticity.stdp_ltp_strength` | 0.001 | 0.1 | 0.02 | STDP LTP weight change |
| `plasticity.stdp_ltd_strength` | 0.001 | 0.1 | 0.015 | STDP LTD weight change |

### Configuration Inheritance

Configuration parameters follow these inheritance rules:

1. **Default Configuration**: Base configuration with sensible defaults
2. **File Configuration**: Overrides defaults based on configuration file
3. **Command Line**: Overrides both defaults and file configuration
4. **Runtime**: Can be modified during simulation (limited parameters)

## Configuration Examples

### 1. Quick Test Configuration

```json
{
    "brain": {"neuron_count": 50, "region_count": 1},
    "simulation": {"timestep": 0.001, "steps": 200},
    "memory": {"replay_enabled": false},
    "development": {"enabled": false}
}
```

### 2. High-Performance Research Configuration

```json
{
    "brain": {"neuron_count": 10000, "region_count": 5},
    "simulation": {"timestep": 0.0005, "steps": 50000},
    "performance": {"enable_vectorization": true, "use_memory_pools": true},
    "memory": {"replay_enabled": true, "working_memory_size": 10000},
    "development": {"enabled": true, "stage_duration": {"stage1": 10000}},
    "output": {"enable_checkpointing": true, "checkpoint_frequency": 1000}
}
```

### 3. Minimal Configuration

```json
{
    "brain": {"neuron_count": 100},
    "simulation": {"timestep": 0.001, "steps": 1000}
}
```

## Configuration Tips

### 1. Choosing the Right Configuration

- **Start Simple**: Begin with `configs/templates/beginner.cfg`
- **Scale Up**: Increase neuron_count and steps for research
- **Optimize**: Use `configs/templates/production.cfg` for deployments

### 2. Configuration Validation

Always validate your configuration before running:

```bash
# Validate JSON configuration
python -m json.tool configs/nlm_config.json

# Validate YAML configuration
python -c "import yaml; yaml.safe_load(open('configs/nlm_config.yaml'))"
```

### 3. Configuration Backup

Create backup configurations for different experiment types:

```bash
# Backup current configuration
mkdir -p backups
cp configs/default.cfg backups/config_$(date +%Y%m%d_%H%M%S).cfg
```

## Configuration File Locations

NLM searches for configuration files in this order:

1. Command line `--config` argument
2. Current directory: `configs/default.cfg`
3. `configs/research.cfg` (if exists)
4. `configs/production.cfg` (if exists)
5. Built-in defaults

## Configuration Best Practices

### 1. Parameter Naming Conventions

- Use snake_case for parameter names
- Group related parameters (e.g., `brain.*`, `simulation.*`)
- Prefix with category for clarity

### 2. Memory Management

```json
{
    "memory": {
        "working_memory_size": "equal_to_neuron_count",
        "episodic_memory_capacity": "appropriate_for_experiment",
        "replay_enabled": "true_for_long_experiments"
    }
}
```

### 3. Performance Optimization

```json
{
    "performance": {
        "enable_vectorization": true,
        "use_memory_pools": true,
        "enable_parallel_processing": true,
        "optimize_for_speed": true
    }
}
```

This configuration system provides the foundation for flexible, scalable, and user-friendly NLM experiments across different use cases from education to research to production.