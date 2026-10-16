# NLM Configuration Guide

# NLM Configuration Guide

## Overview

The NLM (Neural Learning Machine) project uses configuration files to control neural simulation parameters, learning mechanisms, and system behavior. Configuration files allow users to customize simulation behavior without modifying source code.

## Configuration File Format

NLM configuration files use a simple `key = value` format with optional comments:

```ini
# Example configuration file
# Version: 1.0
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
plasticity.stdp.enable = true
log.level = INFO
```

### Key-Value Rules
- Keys use dot notation for hierarchical organization (e.g., `plasticity.stdp.enable`)
- Values can be integers (`int64_t`), floating-point numbers (`double`), or booleans (`true`/`false`)
- Comments start with `#` and continue to end of line
- Blank lines are ignored
- Keys are case-sensitive

## Configuration Sections

### Core Simulation Parameters
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `random_seed` | int64_t | 42 | Any integer | Random seed for reproducible experiments. Use 0 for system time-based seed |
| `simulation_timestep` | double | 0.001 | 0.0001 - 1.0 | Simulation timestep in seconds. Smaller values increase accuracy |
| `neuron_count` | int64_t | 1000 | 100 - 10000 | Total number of neurons in the brain |
| `region_count` | int64_t | 2 | 1 - 100 | Number of brain regions |
| `connection_probability` | float | 0.1 | 0.0 - 1.0 | Probability that any two neurons form a connection |

### Plasticity Learning Mechanisms
#### STDP (Spike-Timing Dependent Plasticity)
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `plasticity.stdp.enable` | bool | true | true/false | Enable or disable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | 0.0001 - 0.1 | STDP learning rate (magnitude of weight changes) |
| `plasticity.stdp.tau_plus` | float | 20.0 | 1.0 - 100.0 | Time constant for LTP (Long-Term Potentiation) in ms |
| `plasticity.stdp.tau_minus` | float | 20.0 | 1.0 - 100.0 | Time constant for LTD (Long-Term Depression) in ms |
| `plasticity.stdp.max_weight` | float | 1.0 | 0.1 - 10.0 | Maximum synaptic weight value |
| `plasticity.stdp.min_weight` | float | 0.0 | 0.0 - 1.0 | Minimum synaptic weight value |

#### Hebbian Learning
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `plasticity.hebbian.enable` | bool | true | true/false | Enable or disable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.01 | 0.001 - 0.1 | Hebbian learning rate |
| `plasticity.hebbian.time_constant` | float | 500.0 | 100.0 - 5000.0 | Memory retention time constant for Hebbian connections |
| `plasticity.hebbian.similarity_threshold` | float | 0.5 | 0.0 - 1.0 | Similarity threshold for Hebbian association |

#### Structural Plasticity
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `plasticity.structural.enable` | bool | true | true/false | Enable or disable structural plasticity (synaptogenesis/pruning) |
| `plasticity.structural.synaptogenesis_rate` | float | 0.001 | 0.0001 - 0.01 | Rate of new synapse formation during development |
| `plasticity.structural.pruning_rate` | float | 0.0001 | 0.00001 - 0.001 | Rate of synapse elimination of weak connections |

### Neuromodulation System
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `neuromod.dopamine.scale` | float | 0.5 | 0.1 - 10.0 | Dopamine modulation scale. Affects reward prediction and motivation |
| `neuromod.serotonin.scale` | float | 0.5 | 0.1 - 10.0 | Serotonin modulation scale. Influences mood and behavioral flexibility |
| `neuromod.norepinephrine.scale` | float | 0.5 | 0.1 - 10.0 | Norepinephrine modulation scale. Affects arousal and attention |
| `neuromod.acetylcholine.scale` | float | 0.5 | 0.1 - 10.0 | Acetylcholine modulation scale. Influences memory and attention |
| `neuromod.curiosity.enable` | bool | true | true/false | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | true/false | Enable novelty detection for exploration |
| `neuromod.novelty.threshold` | float | 0.1 | 0.01 - 0.5 | Novelty threshold for triggering exploration behavior |

### Reinforcement Learning
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `rl.curiosity_weight` | float | 0.1 | 0.0 - 1.0 | Weight for curiosity-driven intrinsic motivation |
| `rl.novelty_threshold` | float | 0.1 | 0.01 - 0.5 | Threshold for considering an experience novel |
| `rl.reward_discount_factor` | float | 0.99 | 0.5 - 1.0 | Discount factor for future rewards (temporal discounting) |
| `rl.prediction_error_weight` | float | 1.0 | 0.5 - 2.0 | Weight for prediction error in learning |
| `rl.exploration_rate` | float | 0.1 | 0.0 - 1.0 | Rate of random exploration actions |

### Environment Parameters
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `environment.name` | string | "GridWorld" | See Environment class | Name of the simulation environment |
| `environment.width` | float | 10.0 | > 0.0 | Width of the environment in simulation units |
| `environment.height` | float | 10.0 | > 0.0 | Height of the environment in simulation units |
| `environment.vision_range` | float | 20.0 | 5.0 - 50.0 | Maximum distance for sensory detection |
| `environment.auditory_range` | float | 20.0 | 5.0 - 50.0 | Maximum distance for sound detection |

### Logging System
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `log.level` | string | "INFO" | "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" | Minimum logging level |
| `log.to_file` | bool | false | true/false | Enable or disable file logging |
| `log.filename` | string | "nlm.log" | Valid filename | Log file name for file-based logging |
| `log.max_size` | int64_t | 100 | 1 - 1000 | Maximum log file size in MB before rotation |
| `log.max_files` | int64_t | 5 | 1 - 100 | Maximum number of log files to keep |

### Simulation Control
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `simulation.max_steps` | int64_t | 10000 | 0 - 1000000 | Maximum number of simulation steps. 0 for unlimited |
| `simulation.time_limit` | double | 0.0 | >= 0.0 | Absolute time limit in seconds. 0 for unlimited |
| `simulation.checkpoint_interval` | int64_t | 1000 | 0 - 10000 | Steps between automatic checkpoints. 0 to disable |

### Visualization System
| Parameter | Type | Default | Valid Range | Description |
|-----------|------|---------|-------------|-------------|
| `visualization.enabled` | bool | false | true/false | Enable or disable visualization |
| `visualization.fps` | int64_t | 30 | 10 - 60 | Frames per second for visualization updates |
| `visualization.width` | int64_t | 1920 | 800 - 3840 | Visualization window width in pixels |
| `visualization.height` | int64_t | 1080 | 600 - 2160 | Visualization window height in pixels |

## Configuration Validation

### Validation Rules
1. **Neuron Count**: Must be at least 100 neurons for meaningful simulation
2. **Connection Probability**: Must be between 0.0 and 1.0
3. **Learning Rates**: Must be positive values
4. **Weight Ranges**: LTP weight should be greater than LTD weight for stability
5. **Timestep**: Must be positive and small enough for numerical stability
6. **Memory Limits**: Simulation cannot exceed available memory

### Common Configuration Mistakes
| Mistake | Symptom | Solution |
|---------|---------|----------|
| `neuron_count` too low | Unstable simulation | Increase to at least 100 |
| `connection_probability` too high | Over-connected network | Reduce to 0.05-0.2 |
| `stdp_ltp_weight` < `stdp_ltd_weight` | Synaptic instability | Ensure LTP > LTD weight |
| `simulation_timestep` too large | Numerical errors | Use 0.001 or smaller |
| `plasticity.stdp.enable` false and no other learning | No learning occurs | Enable at least one plasticity mechanism |

## Configuration File Examples

### Minimal Configuration
```ini
# Minimal working configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
plasticity.stdp.enable = true
log.level = INFO
```

### High-Performance Configuration
```ini
# Optimized for performance
random_seed = 12345
simulation_timestep = 0.01
neuron_count = 5000
region_count = 5
connection_probability = 0.05
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.005
plasticity.hebbian.enable = true
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
log.level = WARN
log.to_file = true
log.filename = nlm_highperf.log
log.max_size = 500
log.max_files = 10
simulation.max_steps = 50000
simulation.checkpoint_interval = 5000
```

### Research Configuration
```ini
# Focused on exploration and learning
random_seed = 0  # Use system time for variety
simulation_timestep = 0.001
neuron_count = 1000
region_count = 3
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true
plasticity.hebbian.time_constant = 1000.0
plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.005
plasticity.structural.pruning_rate = 0.0005
neuromod.dopamine.scale = 1.0
neuromod.serotonin.scale = 1.0
neuromod.norepinephrine.scale = 1.0
neuromod.acetylcholine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
neuromod.novelty.threshold = 0.05
rl.curiosity_weight = 0.3
rl.novelty_threshold = 0.05
rl.reward_discount_factor = 0.95
rl.prediction_error_weight = 1.5
environment.name = GridWorld
environment.width = 20.0
environment.height = 20.0
vision_range = 30.0
auditory_range = 30.0
log.level = DEBUG
log.to_file = true
simulation.max_steps = 100000
```