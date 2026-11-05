# NLM Configuration Examples

## Overview

Practical configuration examples for the NLM neural learning machine. These examples demonstrate common configuration patterns and advanced settings for different use cases.

## Basic Configuration

### Minimal Configuration
```
# Minimal NLM configuration for basic testing
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.1
log_level = INFO
```

### Research Configuration
```
# Configuration optimized for research experiments
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.15

# Plasticity settings
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 25.0

# Development settings
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002

# Neuromodulation settings
dopamine_baseline = 0.2
neuromodulation_strength = 1.5

# Environment settings
environment_name = GridWorld
environment_width = 20
environment_height = 20
vision_range = 10
vision_resolution = 32

# Memory settings
working_memory_capacity = 200.0
episodic_memory_max_episodes = 500

# Logging
log_level = DEBUG
log_to_file = true
log_filename = nlm_research.log
```

## Advanced Configuration

### Production Configuration
```
# Production-ready configuration for stable long-running simulations
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Checkpoint settings for production
checkpoint_save_interval = 1000
checkpoint_compression = true
checkpoint_format_version = 1

# Performance settings
max_simulation_steps = 100000
simulation_time_limit = 3600. Leyour (1 hour max)

# Logging for production
log_level = INFO
log_to_file = true
log_filename = nlm_production.log

# Disable expensive features for stability
visualization_enabled = false
experiment_reward_function = exponential
```

### Experimental Configuration
```
# Configuration for scientific experiments
random_seed = 42
simulation_timestep = 0.0005  # Smaller timestep for accuracy
neuron_count = 5000
region_count = 4
connection_probability = 0.05  # Lower connectivity for control

# Precise plasticity settings for scientific control
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development with controlled stages
development_synaptogenesis_rate = 0.0005
development_pruning_rate = 0.00005
development_stage_transition_age = 2000.0

# Advanced neuromodulation
reward_discount_factor = 0.999
dopamine_sensitivity = 2.0
novelty_threshold = 0.05
curiosity_decay_rate = 0.99

# Enhanced memory systems
working_memory_capacity = 500.0
episodic_memory_max_episodes = 10000
episodic_memory_memory_strength = 0.8

# High precision prediction
prediction_window_size = 20
prediction_confidence_threshold = 0.9

# Cognitive development
concept_formation_threshold = 0.9
self_model_confidence_decay = 0.98
social_learning_weight = 0.3

# Performance optimizations
max_pending_spikes = 50000
event_queue_size = 5000
memory_pool_size = 50000

# Logging for experiment analysis
log_level = DEBUG
log_to_file = true
log_filename = nlm_experiment.log

# Experimental parameters
experiment_seed = 42
experiment_max_trials = 200
experiment_reward_function = exponential
```

### Development Configuration
```
# Configuration optimized for development and testing
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.2

# Fast development settings
development_synaptogenesis_rate = 0.01
development_pruning_rate = 0.001
development_plasticity_decay_rate = 0.95

# Liberal plasticity for exploration
plasticity_learning_rate = 0.1
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.06

# Rich neuromodulation for learning
reward_discount_factor = 0.95
dopamine_baseline = 0.3
neuromodulation_strength = 2.%