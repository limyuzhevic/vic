# NLM Configuration Schema (Phase 6)

This schema defines configuration options for the NLM Phase 6 integrated brain system.

## Configuration Structure

### Core Simulation
```
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 10000
simulation_time_limit = 0.0
```

### Neural Network
```
neuron_count = 1000
region_count = 2
connection_probability = 0.1
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0
```

### Plasticity
```
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
```

### Neuromodulation (Phase 6 additions)
```
dopamine_baseline = 0.1
curiosity_baseline = 0.5
novelty_threshold = 0.3
reward_decay = 0.95
prediction_error_weight = 0.7
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
```

### Memory Systems
```
memory.working.enable = true
memory.episodic.enable = true
memory.associative.enable = true
memory.replay.enable = true
memory.consolidation.enable = true
```

### Cognition
```
cognition.planner.enable = true
cognition.concept_formation.enable = true
cognition.attention.enable = true
cognition.planning.enable = true
```

### Environment
```
environment_name = GridWorld
environment_width = 10
environment_height = 10
visualization_enabled = false
```

### Development
```
development.enabled = true
development.critical_period = 100
development.maturation_stage = 500
```

## Validation Rules

### Required Fields
- `random_seed` (int): Must be positive integer
- `simulation_timestep` (float): Must be > 0
- `neuron_count` (int): Must be > 0
- `region_count` (int): Must be >= 1

### Constraint Checks
- `max_simulation_steps` >= 100
- `simulation_time_limit` >= 0 or -1 for unlimited
- `connection_probability` in [0, 1]
- `plasticity_learning_rate` >= 0
- `reward_discount_factor` in [0, 1]

## Example Configuration File

```ini
# NLM Configuration File - Phase 6 Schema
# This configuration follows the NLM configuration schema

# Core Simulation
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 10000
simulation_time_limit = 0.0
checkpoint_interval = 1000

# Neural Network Configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.1
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation Configuration
dopamine_baseline = 0.1
curiosity_baseline = 0.5
novelty_threshold = 0.3
reward_decay = 0.95
prediction_error_weight = 0.7
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Memory System Configuration
memory.working.enable = true
memory.episodic.enable = true
memory.associative.enable = true
memory.replay.enable = true
memory.consolidation.enable = true

# Cognition System Configuration
cognition.planner.enable = true
cognition.concept_formation.enable = true
cognition.attention.enable = true
cognition.planning.enable = true

# Environment Configuration
environment_name = GridWorld
environment_width = 10
environment_height = 10
visualization_enabled = false
visualization_update_rate = 30.0

# Development Configuration
development.enabled = true
development.critical_period = 100
development.maturation_stage = 500
```

## Configuration Source Priority

1. **Command Line Arguments** (`--key=value` or `-key value`) - Highest priority
2. **File Configuration** (`config.cfg`) - Medium priority
3. **Defaults** (`configs/default.cfg`) - Base defaults
4. **Runtime Setters** - Lowest priority

## Schema Versioning

- **Version**: 1.0
- **Last Updated**: 2026-09-05 (Phase 6)
- **Compatible Phases**: 4, 5, 6

## Migration Notes

- The schema includes Phase 4 and Phase 5 parameters for backward compatibility
- New parameters specific to Phase 6 integration are marked with Phase 6 prefix
- Parameters without prefix use standard names for consistency
