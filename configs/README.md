# NLM Configuration File Examples

This directory contains example configuration files for different NLM (Neural Learning Machine) use cases.

## Overview

NLM uses JSON configuration files to control brain parameters, learning settings, and simulation behavior. Configuration files are loaded by the `nlm` executable and `pynlm` Python library.

## Available Examples

### 1. Basic Configuration (`basic.cfg`)

A minimal configuration for testing basic neural computation:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0,
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "tau": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        },
        "structural": {
            "enable": true,
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
            "scale": 0.5
        },
        "novelty": {
            "enable": true,
            "threshold": 0.1
        }
    },
    "development": {
        "stages": {
            "critical_period_start": 60.0,
            "maturation_start": 300.0,
            "aging_start": 900.0
        },
        "plasticity_modifiers": {
            "critical_period": 0.8,
            "maturation": 0.5,
            "adult": 0.2,
            "aging": 0.1
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000,
        "checkpoint_dir": "./checkpoints",
        "checkpoint_interval": 1000,
        "checkpoint_compression": "balanced"
    },
    "logging": {
        "level": "info",
        "file": "nlm.log",
        "console": true
    },
    "random": {
        "seed": 42,
        "generator_type": "mt19937"
    }
}
```

**Use for:** Testing basic brain functionality and learning

### 2. Development Configuration (`development.cfg`)

Optimized for brain development research:

```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 2,
        "connection_probability": 0.15,
        "initial_weight_mean": 0.3,
        "initial_weight_std": 0.15,
        "v_thresh": -45.0,
        "v_rest": -65.0,
        "v_reset": -70.0,
        "tau_mem": 25.0,
        "tau_ref": 1.5
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 25.0,
            "tau_minus": 25.0,
            "ltp_weight": 0.025,
            "ltd_weight": 0.02,
            "tau": 25.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0015
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0002,
            "pruning_rate": 0.00002,
            "growth_threshold": 0.5,
            "pruning_threshold": 0.1
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.5,
            "prediction_error_sensitivity": 1.2
        },
        "curiosity": {
            "enable": true,
            "scale": 0.8,
            "exploration_bonus": 0.3
        },
        "novelty": {
            "enable": true,
            "threshold": 0.08,
            "decay_rate": 0.95
        },
        "prediction_error": {
            "enable": true,
            "gain": 1.0
        }
    },
    "development": {
        "enabled": true,
        "stage_progression": "linear",
        "plasticity_decay": "exponential",
        "stages": {
            "initial_duration": 60.0,
            "critical_period_duration": 240.0,
            "maturation_duration": 540.0,
            "adult_duration": 3600.0
        },
        "structural_changes": {
            "synapse_addition_rate": 0.0002,
            "synapse_removal_rate": 0.00002,
            "reorganization_interval": 100.0
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "checkpoint_dir": "./development_checkpoints",
        "checkpoint_interval": 1000,
        "checkpoint_compression": "aggressive",
        "replay_interval": 50,
        "consolidation_interval": 500
    },
    "memory": {
        "working_memory": {
            "capacity": 500,
            "decay_rate": 0.99,
            "competition_strength": 0.5
        },
        "episodic_memory": {
            "max_episodes": 5000,
            "compression": "neural_patterns",
            "replay_on": true,
            "consolidation_threshold": 0.7
        },
        "associative_memory": {
            "pattern_storage": "incremental",
            "similarity_threshold": 0.6,
            "max_associations": 10000
        }
    },
    "logging": {
        "level": "debug",
        "file": "nlm_development.log",
        "console": true,
        "structured": true,
        "performance_tracking": true
    },
    "random": {
        "seed": 12345,
        "generator_type": "xoshiro256++",
        "state_file": "random_state.bin"
    }
}
```

**Use for:** Brain development research and long-running experiments

### 3. Performance Configuration (`performance.cfg`)

Optimized for speed and scalability:

```json
{
    "brain": {
        "neuron_count": 10000,
        "region_count": 4,
        "connection_probability": 0.05,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0,
            "ltp_weight": 0.01,
            "ltd_weight": 0.01,
            "tau": 20.0
        },
        "hebbian": {
            "enable": false,
            "learning_rate": 0.0
        },
        "structural": {
            "enable": false,
            "synaptogenesis_rate": 0.0,
            "pruning_rate": 0.0
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": false,
            "scale": 0.0
        },
        "novelty": {
            "enable": false,
            "threshold": 1.0
        }
    },
    "development": {
        "enabled": false,
        "stages": {
            "critical_period_start": 0.0,
            "maturation_start": 0.0,
            "aging_start": 0.0
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 5000,
        "checkpoint_dir": "./perf_checkpoints",
        "checkpoint_interval": 5000,
        "checkpoint_compression": "fast",
        "replay_interval": 0,
        "consolidation_interval": 0
    },
    "performance": {
        "optimization_level": "aggressive",
        "use_simd": true,
        "use_multithreading": true,
        "memory_pooling": true,
        "event_driven": true,
        "sparse_connectivity": true,
        "parallel_processing": true
    },
    "logging": {
        "level": "warning",
        "file": "",
        "console": false,
        "structured": false,
        "performance_tracking": true
    },
    "random": {
        "seed": 999,
        "generator_type": "mt19937",
        "fast_seeding": true
    }
}
```

**Use for:** Large-scale performance testing and benchmarking

## Configuration Schema

All NLM configuration files follow this JSON schema:

### Brain Configuration
- `neuron_count` (int): Total number of neurons (default: 1000)
- `region_count` (int): Number of brain regions (default: 1)
- `connection_probability` (float): Probability of connection between neurons (0.0-1.0, default: 0.1)
- `initial_weight_mean` (float): Mean initial synaptic weight (default: 0.5)
- `initial_weight_std` (float): Standard deviation of initial weights (default: 0.1)
- `v_thresh` (float): Neuron threshold potential in mV (default: -50.0)
- `v_rest` (float): Resting potential in mV (default: -70.0)
- `v_reset` (float): Reset potential after spike in mV (default: -75.0)
- `tau_mem` (float): Membrane time constant in ms (default: 20.0)
- `tau_ref` (float): Refractory period in ms (default: 2.0)

### Plasticity Configuration
- `stdp.enable` (bool): Enable spike-timing dependent plasticity (default: true)
- `stdp.learning_rate` (float): STDP learning rate (default: 0.001)
- `stdp.tau_plus` (float): STDP LTP time constant (default: 20.0)
- `stdp.tau_minus` (float): STDP LTD time constant (default: 20.0)
- `stdp.ltp_weight` (float): STDP LTP weight (default: 0.02)
- `stdp.ltd_weight` (float): STDP LTD weight (default: 0.015)
- `stdp.tau` (float): STDP time constant (default: 20.0)

- `hebbian.enable` (bool): Enable Hebbian learning (default: true)
- `hebbian.learning_rate` (float): Hebbian learning rate (default: 0.001)

- `structural.enable` (bool): Enable structural plasticity (default: true)
- `structural.synaptogenesis_rate` (float): Rate of new synapse formation (default: 0.0001)
- `structural.pruning_rate` (float): Rate of weak synapse removal (default: 0.00001)

### Neuromodulation Configuration
- `dopamine.scale` (float): Dopamine modulation scale (default: 1.0)
- `curiosity.enable` (bool): Enable curiosity-driven exploration (default: true)
- `curiosity.scale` (float): Curiosity modulation scale (default: 0.5)
- `novelty.enable` (bool): Enable novelty detection (default: true)
- `novelty.threshold` (float): Novelty detection threshold (default: 0.1)

### Development Configuration
- `development.enabled` (bool): Enable developmental processes (default: false)
- `development.stage_progression` (string): How stages progress ("linear" or "exponential")
- `development.plasticity_decay` (string): How plasticity changes ("linear" or "exponential")
- `development.stages.initial_duration` (float): Duration of initial stage in simulation time
- `development.stages.critical_period_duration` (float): Duration of critical period
- `development.stages.maturation_duration` (float): Duration of maturation stage
- `development.stages.adult_duration` (float): Duration of adult stage

### Simulation Configuration
- `simulation.timestep` (float): Simulation timestep in seconds (default: 0.001)
- `simulation.max_steps` (int): Maximum simulation steps (default: 1000)
- `simulation.checkpoint_dir` (string): Directory for checkpoints (default: "./checkpoints")
- `simulation.checkpoint_interval` (int): Checkpoint save interval (default: 1000)
- `simulation.checkpoint_compression` (string): Compression level ("fast", "balanced", "aggressive")
- `simulation.replay_interval` (int): Memory replay interval (default: 100)
- `simulation.consolidation_interval` (int): Memory consolidation interval (default: 1000)

### Memory Configuration
- `memory.working_memory.capacity` (int): Working memory capacity (default: 500)
- `memory.working_memory.decay_rate` (float): Memory decay rate (default: 0.99)
- `memory.working_memory.competition_strength` (float): Competition strength (default: 0.5)

- `memory.episodic_memory.max_episodes` (int): Maximum episodes to store (default: 1000)
- `memory.episodic_memory.compression` (string): Compression method
- `memory.episodic_memory.replay_on` (bool): Enable memory replay (default: true)
- `memory.episodic_memory.consolidation_threshold` (float): Consolidation threshold (default: 0.7)

### Logging Configuration
- `logging.level` (string): Log level ("debug", "info", "warning", "error")
- `logging.file` (string): Log file path (default: "") for console only
- `logging.console` (bool): Enable console logging (default: true)
- `logging.structured` (bool): Enable structured logging (default: false)
- `logging.performance_tracking` (bool): Track performance metrics (default: false)

### Random Configuration
- `random.seed` (int): Random seed for deterministic simulations
- `random.generator_type` (string): Random number generator type
- `random.state_file` (string): File to save/load random state

## Configuration Validation

NLM includes configuration validation that checks:

1. **Parameter ranges**: Ensures all values are within acceptable bounds
2. **Dependency consistency**: Validates that enabled features have required dependencies
3. **Memory constraints**: Checks that memory requirements are satisfied
4. **Performance constraints**: Ensures simulation parameters are feasible

Invalid configurations will cause the simulation to fail with descriptive error messages.

## Configuration Command Line Arguments

Configuration can also be specified via command line arguments:

```bash
# Override specific values
./nlm --config my_config.cfg \
     --brain.neuron_count=5000 \
     --simulation.timestep=0.0005 \
     --plasticity.stdp.learning_rate=0.002

# Enable specific features
./nlm --plasticity.hebbian.enable=true \
     --neuromodulation.curiosity.enable=true \
     --development.enabled=true

# Set development stage
./nlm --development.stage=Maturation
```

Command line arguments override configuration file values.

## Best Practices

1. **Use configuration files for reproducibility**: Save final working configurations for sharing
2. **Start with basic configurations**: Begin with `basic.cfg` and add features incrementally
3. **Validate before running**: Always check your configuration with `--verbose`
4. **Version your configurations**: Include version information in configuration comments
5. **Document your experiments**: Keep configuration history for each experimental run
6. **Use environment-specific configurations**: Have separate configs for development, testing, and production

## Creating Custom Configurations

To create a custom configuration:

1. Copy an existing example file
2. Modify values to match your experimental requirements
3. Validate with `nlm --help` for configuration options
4. Test with a brief demo: `nlm --brief --config your_config.cfg`
5. Run your full experiment: `nlm --config your_config.cfg`

Configuration files should be placed in the `configs/` directory or specified with absolute paths.