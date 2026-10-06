# NLM Configuration Examples

This directory contains example configuration files for different use cases.

## Example 1: Phase 2 Basic Configuration

**File: `configs/phase2.cfg`**

```
# NLM Phase 2 Configuration
# Basic neural computation setup

# Core parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Neuron parameters
neuron_threshold = -55.0
neuron_resting_potential = -70.0
neuron_reset_potential = -65.0
refractory_period = 5

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Development parameters
enable_development = false
development_rate = 0.001

# Logging
log_level = INFO
log_file = nlm.log
```

## Example 2: Phase 4 Advanced Configuration

**File: `configs/phase4.cfg`**

```
# NLM Phase 4 Configuration
# Advanced cognitive capabilities

# Core parameters
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.12

# Extended plasticity
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.012
stdp_tau = 25.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005
enable_reward_plasticity = true
enable_structural_plasticity = true

# Cognitive features
enable_prediction = true
prediction_horizon = 10
enable_attention = true
attention_span = 5
enable_planning = true
planning_horizon = 20

# Memory systems
enable_working_memory = true
working_memory_capacity = 50
enable_episodic_memory = true
enable_concept_formation = true

# Neuromodulation
enable_dopamine = true
dopamine_sensitivity = 0.8
enable_curiosity = true
curiosity_sensitivity = 0.6
enable_novelty = true
novelty_threshold = 0.1

# Development
enable_development = true
development_duration = 300.0
development_plasticity_factor = 0.3

# Logging
log_level = DEBUG
log_file = nlm_phase4.log
```

## Example 3: Phase 6 Full Integration Configuration

**File: `configs/phase6.cfg`**

```
# NLM Phase 6 Configuration
# Complete brain integration with all systems

# Core parameters
random_seed = 99999
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.1

# All plasticity types enabled
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.008
stdp_tau = 30.0
synaptogenesis_rate = 0.00001
pruning_rate = 0.000001
enable_hebbian_plasticity = true
enable_reward_plasticity = true
enable_structural_plasticity = true

# Full neuromodulation suite
enable_dopamine = true
dopamine_sensitivity = 1.0
enable_curiosity = true
curiosity_sensitivity = 0.9
enable_novelty = true
novelty_threshold = 0.05
novelty_decay = 0.95

# Complete memory architecture
enable_working_memory = true
working_memory_capacity = 100
enable_episodic_memory = true
episodic_memory_capacity = 1000
enable_concept_formation = true
concept_retention = 0.9

# Advanced cognition
enable_prediction = true
prediction_horizon = 20
prediction_confidence = 0.8
enable_attention = true
attention_span = 8
enable_planning = true
planning_horizon = 50
planning_confidence = 0.7

# Full development lifecycle
enable_development = true
development_duration = 600.0
development_plasticity_factor = 0.5
critical_period_end = 120.0

# Memory replay and consolidation
enable_replay = true
replay_frequency = 0.1
replay_strength = 0.8
enable_consolidation = true
consolidation_threshold = 0.7

# Performance settings
enable_checkpointing = true
checkpoint_interval = 1000
enable_sparse_connectivity = true
connection_threshold = 0.05

# Logging and monitoring
log_level = INFO
log_file = nlm_phase6.log
metrics_output = nlm_metrics.csv
performance_monitoring = true
```

## Configuration File Format

The configuration system uses a simple key=value format:

```
key = value
```

**Supported types:**
- Integers (e.g., `neuron_count = 500`)
- Floating-point numbers (e.g., `simulation_timestep = 0.001`)
- Strings (e.g., `log_file = nlm.log`)
- Booleans (e.g., `enable_development = true`)

**Special features:**
- Lines starting with `#` or `//` are comments
- Empty lines are ignored
- Values can be quoted with single or double quotes
- Command-line arguments override config file values

## Command Line Configuration Override

Configuration can be overridden using command-line arguments:

```bash
./nlm --config phase2.cfg --neuron_count 1000 --stdp_ltp_weight 0.025
```

This would load `phase2.cfg` but override `neuron_count` and `stdp_ltp_weight`.

## Configuration Validation

All configuration parameters are validated for reasonable ranges:

```bash
# Validate configuration
./nlm validate --config phase6.cfg
```

This checks for:
- Positive values for counts and rates
- Reasonable ranges for physiological parameters
- Consistency between related parameters
- Required dependencies between features
