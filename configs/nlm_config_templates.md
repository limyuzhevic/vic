# NLM Configuration Templates

## Overview

This document provides configuration templates and examples for setting up NLM simulations with different use cases and performance requirements.

## Configuration Structure

NLM uses JSON configuration files with a hierarchical structure. Here's the basic format:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "region_count": 1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": true,
            "exploration_threshold": 0.5
        },
        "novelty": {
            "enable": true
        },
        "dopamine": {
            "scale": 1.0
        }
    },
    "development": {
        "stage_transitions": {
            "enabled": true
        },
        "critical_period": {
            "duration": 100
        }
    },
    "simulation": {
        "checkpoint_dir": "./checkpoints",
        "replay_interval": 100,
        "consolidation_interval": 1000
    }
}
```

## Configuration Templates

### Template 1: Minimal Working Brain (Phase 2)

`minimal_config.json`

```json
{
    "brain": {
        "neuron_count": 500,
        "synapse_density": 0.05,
        "connection_probability": 0.1,
        "region_count": 1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true
        },
        "structural": {
            "enable": false
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": false
        },
        "novelty": {
            "enable": false
        }
    },
    "simulation": {
        "timestep": 0.001
    }
}
```

**Use case:** Basic testing and development of core neural dynamics.

### Template 2: Complete Brain with Learning (Phase 3)

`learning_config.json`

```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.08,
        "connection_probability": 0.12,
        "region_count": 2
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.005,
            "tau_plus": 15.0,
            "tau_minus": 15.0
        },
        "hebbian": {
            "enable": true
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0005,
            "pruning_rate": 0.00005
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": true,
            "exploration_threshold": 0.3,
            "novelty_weight": 0.7,
            "prediction_error_weight": 0.3
        },
        "novelty": {
            "enable": true,
            "sensitivity": 0.5
        },
        "dopamine": {
            "scale": 1.2
        }
    },
    "development": {
        "stage_transitions": {
            "enabled": true
        },
        "critical_period": {
            "duration": 200
        },
        "plasticity_modulation": {
            "enabled": true,
            "rate_multiplier": 2.0
        }
    },
    "simulation": {
        "timestep": 0.001,
        "checkpoint_dir": "./checkpoints/learning_run",
        "replay_interval": 50,
        "consolidation_interval": 500
    }
}
```

**Use case:** Full learning environment with curiosity-driven exploration.

### Template 3: Phase 6 Integration Test

`integration_config.json`

```json
{
    "brain": {
        "neuron_count": 3000,
        "synapse_density": 0.06,
        "connection_probability": 0.08,
        "region_count": 3
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0,
            "ltp_weight": 0.02,
            "ltd_weight": 0.015
        },
        "hebbian": {
            "enable": true,
            "association_window": 50
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0002,
            "pruning_rate": 0.00002,
            "max_synapses_per_neuron": 100
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": true,
            "exploration_threshold": 0.2,
            "novelty_weight": 0.6,
            "prediction_error_weight": 0.4,
            "min_curiosity": 0.1,
            "max_curiosity": 5.0
        },
        "novelty": {
            "enable": true,
            "sensitivity": 0.3,
            "adaptation_rate": 0.95
        },
        "dopamine": {
            "scale": 1.0,
            "plasticity_factor": 1.5,
            "excitability_modulator": 0.8
        }
    },
    "memory": {
        "working": {
            "capacity_multiplier": 0.1,
            "decay_rate": 0.99,
            "competition_strength": 2.0
        },
        "episodic": {
            "max_episodes": 5000,
            "replay_frequency": 0.1,
            "encoding_strength": 0.8
        },
        "associative": {
            "learning_rate": 0.001,
            "similarity_threshold": 0.7
        }
    },
    "cognition": {
        "planning": {
            "depth": 5,
            "lookahead_steps": 3
        },
        "concept_formation": {
            "category_threshold": 0.6,
            "similarity_metric": "euclidean"
        },
        "attention": {
            "inhibition_strength": 0.5,
            "excitation_strength": 1.5,
            "focus_duration": 20
        }
    },
    "development": {
        "stage_transitions": {
            "enabled": true,
            "progression_rate": 0.01
        },
        "critical_period": {
            "duration": 300,
            "plasticity_boost": 2.0
        },
        "structural_plasticity": {
            "enabled": true,
            "growth_rate": 0.001,
            "pruning_rate": 0.0001
        }
    },
    "simulation": {
        "timestep": 0.001,
        "checkpoint_dir": "./checkpoints/integration",
        "checkpoint_interval": 1000,
        "max_checkpoints": 5,
        "replay_interval": 100,
        "consolidation_interval": 2000
    }
}
```

**Use case:** Complete Phase 6 integration testing with all systems connected.

### Template 4: Performance-Optimized (High Neuron Count)

`performance_config.json`

```json
{
    "brain": {
        "neuron_count": 10000,
        "synapse_density": 0.02,
        "connection_probability": 0.04,
        "region_count": 4
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.0005,
            "tau_plus": 25.0,
            "tau_minus": 25.0
        },
        "hebbian": {
            "enable": false
        },
        "structural": {
            "enable": false
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": false
        },
        "novelty": {
            "enable": false
        }
    },
    "performance": {
        "sparse_connectivity": true,
        "event_driven": true,
        "parallel_processing": true,
        "memory_pools": true,
        "simd_optimization": true
    },
    "simulation": {
        "timestep": 0.002,
        "checkpoint_dir": "./checkpoints/performance",
        "checkpoint_interval": 5000,
        "min_checkpoint_size_mb": 50
    }
}
```

**Use case:** Large-scale simulations with performance optimizations.

## Configuration Parameter Reference

### Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.region_count` | int | 1 | Number of neural regions |
| `brain.v_thresh` | float | -50.0 | Neuron threshold (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant |
| `plasticity.stdp.ltp_weight` | float | 0.02 | LTP weight magnitude |
| `plasticity.stdp.ltd_weight` | float | 0.015 | LTD weight magnitude |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.association_window` | float | 50.0 | Time window for Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination |
| `plasticity.structural.max_synapses_per_neuron` | int | 100 | Maximum synapses per neuron |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.exploration_threshold` | float | 0.5 | Novelty threshold for exploration |
| `neuromod.curiosity.novelty_weight` | float | 0.7 | Weight of novelty in curiosity |
| `neuromod.curiosity.prediction_error_weight` | float | 0.3 | Weight of prediction error in curiosity |
| `neuromod.curiosity.min_curiosity` | float | 0.1 | Minimum curiosity level |
| `neuromod.curiosity.max_curiosity` | float | 5.0 | Maximum curiosity level |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.sensitivity` | float | 0.5 | Sensitivity to novel patterns |
| `neuromod.novelty.adaptation_rate` | float | 0.95 | Rate of novelty adaptation |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.plasticity_factor` | float | 1.0 | Dopamine effect on plasticity |
| `neuromod.dopamine.excitability_modulator` | float | 1.0 | Dopamine effect on excitability |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage_transitions.enabled` | bool | true | Enable developmental stages |
| `development.stage_transitions.progression_rate` | float | 0.01 | Rate of stage progression |
| `development.critical_period.duration` | float | 100.0 | Critical period length (steps) |
| `development.critical_period.plasticity_boost` | float | 2.0 | Plasticity enhancement during critical period |
| `development.plasticity_modulation.enabled` | bool | true | Enable development effects on plasticity |
| `development.plasticity_modulation.rate_multiplier` | float | 1.0 | Multiplier for plasticity rate |
| `development.structural_plasticity.enabled` | bool | false | Enable development effects on structure |
| `development.structural_plasticity.growth_rate` | float | 0.001 | Rate of structural growth |
| `development.structural_plasticity.pruning_rate` | float | 0.0001 | Rate of structural pruning |

### Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.capacity_multiplier` | float | 0.1 | Working memory capacity as fraction of neurons |
| `memory.working.decay_rate` | float | 0.99 | Rate of memory decay |
| `memory.working.competition_strength` | float | 2.0 | Strength of memory competition |
| `memory.episodic.max_episodes` | int | 1000 | Maximum number of episodic memories |
| `memory.episodic.replay_frequency` | float | 0.1 | Frequency of memory replay (0-1) |
| `memory.episodic.encoding_strength` | float | 0.8 | Strength of memory encoding |
| `memory.associative.learning_rate` | float | 0.001 | Learning rate for associative memory |
| `memory.associative.similarity_threshold` | float | 0.7 | Similarity threshold for associations |

## Configuration Validation

### Command Line Validation

```bash
# Validate configuration before running
./validate_config.py --config my_config.json

# Check for errors
python -c "import json; config = json.load(open('my_config.json')); print('Configuration is valid JSON')"

# Run with configuration
cd build
./nlm --config my_config.json
```

### Python Validation

```python
import pynlm
import json

def validate_config(config_path):
    """Validate configuration file before use."""
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        if not config.loadFromFile(config_path):
            print(f"Failed to load configuration from {config_path}")
            return False
        
        # Validate essential parameters
        if not config.has("brain.neuron_count"):
            print("Missing brain.neuron_count parameter")
            return False
        
        neuron_count = config.get("brain.neuron_count")
        if neuron_count <= 0:
            print(f"Invalid brain.neuron_count: {neuron_count}")
            return False
        
        print(f"Configuration validated successfully with {neuron_count} neurons")
        return True
        
    except Exception as e:
        print(f"Configuration validation failed: {e}")
        return False

# Usage
if validate_config("my_config.json"):
    # Safe to use configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
```

## Configuration Examples

### 1. Brain Development Experiment

```bash
# Create a development tracking configuration
ec2 config=create_development_config.json \
    --neuron_count=2000 \
    --stages=5 \
    --critical_period=200 \
    --plasticity_boost=2.0 \
    --output=development_experiment.json
```

**Result:** `development_experiment.json` with configuration optimized for tracking brain development over time.

### 2. Learning Efficiency Test

```bash
# Create a configuration for testing learning efficiency
ec2 config=create_efficiency_test.json \
    --neuron_count=1000 \
    --stdp_lr=0.01 \
    --hebbian=true \
    --structural=false \
    --curiosity=true \
    --novelty=true \
    --output=efficiency_test.json
```

**Result:** `efficiency_test.json` optimized for measuring learning speed and efficiency.

### 3. Memory Consolidation Study

```bash
# Create a configuration for studying memory consolidation
ec2 config=create_memory_consolidation.json \
    --neuron_count=3000 \
    --working_memory_capacity=0.2 \
    --episodic_max_episodes=5000 \
    --replay_interval=50 \
    --consolidation_interval=200 \
    --output=memory_consolidation.json
```

**Result:** `memory_consolidation.json` configured for studying memory replay and consolidation.

## Configuration Best Practices

### 1. Start with Templates

- Always start with an existing template
- Modify specific parameters as needed
- Keep default values for unfamiliar parameters

### 2. Validate Before Use

```bash
# Validate configuration before running
./validate_config.py config.json
```

### 3. Monitor Configuration Changes

```python
import pynlm

def log_config_changes(old_config, new_config):
    """Log changes between two configurations."""
    all_keys = set(old_config.getKeys()) | set(new_config.getKeys())
    
    for key in sorted(all_keys):
        old_val = old_config.get(key) if old_config.has(key) else None
        new_val = new_config.get(key) if new_config.has(key) else None
        
        if old_val != new_val:
            print(f"Config change: {key}")
            print(f"  Old: {old_val}")
            print(f"  New: {new_val}")
```

### 4. Use Configuration Comments

```json
{
    "brain": {
        "neuron_count": 1000,  // 1000 neurons for basic testing
        "synapse_density": 0.05, // Moderate connectivity
        "region_count": 1       // Single region for simplicity
    },
    // "neuromod.curiosity": {
    //     "enable": false,       // Disable curiosity for baseline
    //     "exploration_threshold": 0.5
    // }
}
```

### 5. Keep Configuration Organized

```bash
# Organize configuration files
mkdir -p configs/{development,learning,integration,performance}

# Development configs
configs/development/basic_config.json
configs/development/development_config.json
configs/development/memory_config.json

# Learning configs  
configs/learning/exploration_config.json
configs/learning/optimization_config.json
configs/learning/reward_config.json

# Integration configs
configs/integration/phase6_config.json
configs/integration/benchmark_config.json
configs/integration/stress_config.json

# Performance configs
configs/performance/large_scale_config.json
configs/performance/real_time_config.json
configs/performance/cloud_config.json
```

## Configuration Tools

### Configuration Generator

```bash
# Generate configuration programmatically
python generate_config.py --template learning --output learning_config.json --neurons 2000

# Available templates:
- minimal
- development
- learning
- integration
- performance
```

### Configuration Comparator

```bash
# Compare two configurations
python compare_configs.py config1.json config2.json > diff_report.txt

# Generate unified diff
diff -u config1.json config2.json > unified_diff.txt
```

### Configuration Validator

```bash
# Validate configuration schema
python validate_schema.py --schema config_schema.json --config config.json

# Check for consistency
python check_consistency.py config.json
```

## Conclusion

Configuration templates provide a structured approach to setting up NLM simulations. They:

1. **Standardize setup** - Consistent configuration across experiments
2. **Reduce errors** - Pre-validated parameter ranges
3. **Speed up development** - Quick setup of common scenarios
4. **Improve documentation** - Self-documenting parameter values
5. **Enable comparison** - Easy comparison between experimental conditions

Use configuration templates as your starting point, then customize specific parameters as needed for your experimental requirements.

---
*This document provides configuration templates and best practices for NLM simulations. Use these templates as starting points for your experiments, then customize parameters as needed.*