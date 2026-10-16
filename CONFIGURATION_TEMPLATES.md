# NLM Configuration Templates

This document provides example configuration files for different types of NLM experiments and use cases.

## Overview

Configuration files control the behavior of the NLM (Neural Learning Machine) brain simulation. They specify parameters for:

- **Brain properties**: neuron count, connectivity, initial conditions
- **Plasticity rules**: learning rates, STDP parameters
- **Development**: growth and maturation processes
- **Neuromodulation**: reward signals and motivation
- **Environment**: simulation world settings
- **Logging**: output verbosity and file locations

## Template Types

### 1. Basic Learning Experiment

**Purpose**: Simple neural network learning and plasticity demonstration

**File**: `configs/basic_learning.cfg`

```ini
# NLM Configuration - Basic Learning Experiment
# Demonstrates basic neural learning and synaptic plasticity

# Brain parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity settings (Hebbian learning)
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development
-synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation (simple reward)
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Environment
d environment_name = GridWorld
environment_width = 20
environment_height = 20
vision_width = 8
vision_height = 8

# Logging
log_level = INFO
log_to_file = false
log_filename = basic_learning.log

# Simulation
max_simulation_steps = 5000
simulation_time_limit = 0.0
```

### 2. Developmental Plasticity Experiment

**Purpose**: Study developmental stages and age-dependent plasticity

**File**: `configs/developmental.cfg`

```ini
# NLM Configuration - Developmental Plasticity
# Studies how learning changes across developmental stages

# Brain parameters
random_seed = 123
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.1

# Development parameters
development_critical_period_start = 100
development_critical_period_end = 1000
development_maturation_rate = 0.5
development_old_age_decline = 0.3

# Plasticity modulation by development
plasticity_learning_rate = 0.005  # Lower during critical period
stdp_ltp_weight = 0.005
stdp_ltd_weight = 0.006
stdp_time_constant = 30.0

# Synaptic plasticity
-synaptogenesis_rate = 0.0002  # Higher during development
pruning_rate = 0.00005

# Neuromodulation with development
dopamine_baseline = 0.2
reward_discount_factor = 0.95
curiosity_baseline = 0.5
novelty_threshold = 0.3

# Environment
environment_name = ComplexWorld
environment_width = 30
environment_height = 30
vision_width = 12
vision_height = 12

# Logging
log_level = INFO
log_to_file = true
log_filename = developmental.log

# Simulation
max_simulation_steps = 10000
simulation_time_limit = 0.0
enable_developmental_tracking = true
```

### 3. Advanced Cognitive Experiment

**Purpose**: Complex cognitive tasks with memory and planning

**File**: `configs/cognitive.cfg`

```ini
# NLM Configuration - Advanced Cognitive Experiment
# Demonstrates complex cognitive abilities and memory systems

# Brain parameters
random_seed = 999
simulation_timestep = 0.0005  # Finer timestep for accuracy
neuron_count = 5000
region_count = 4
connection_probability = 0.08

# Working memory configuration
working_memory_capacity = 100
working_memory_decay_rate = 0.001
working_memory_competition_strength = 0.8

# Episodic memory configuration
episodic_memory_capacity = 1000
episodic_memory_consolidation_threshold = 0.7
episodic_memory_replay_interval = 100

# Concept formation
concept_learning_rate = 0.02
concept_similarity_threshold = 0.6
concept_stability_threshold = 0.9

# Neural planning
planning_horizon = 10
planning_confidence_threshold = 0.7
planning_exploration_rate = 0.3

# Plasticity
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0

# Neuromodulation
reward_prediction_error_decay = 0.95
dopamine_modulation_strength = 1.2
curiosity_driven_exploration = true
novelty_sensitivity = 0.4

# Environment
environment_name = MazeWorld
environment_width = 40
environment_height = 40
vision_width = 16
vision_height = 16
enable_social_learning = true

# Logging
log_level = DEBUG
log_to_file = true
log_filename = cognitive.log
log_rotation_size = 10485760  # 10MB

# Simulation
max_simulation_steps = 50000
simulation_time_limit = 0.0
enable_checkpointing = true
checkpoint_interval = 1000
```

### 4. Performance Optimization Experiment

**Purpose**: High-performance neural simulation with optimizations

**File**: `configs/performance.cfg`

```ini
# NLM Configuration - Performance Optimized
# Optimized for maximum performance and efficiency

# Brain parameters
random_seed = 777
simulation_timestep = 0.002  # Larger timestep for speed
neuron_count = 10000
region_count = 5
connection_probability = 0.05

# Memory pool configuration
memory_pool_enabled = true
neuron_memory_pool_size = 50000
synapse_memory_pool_size = 100000
region_memory_pool_size = 1000

# Event system
enable_event_queue = true
event_queue_batch_size = 100
event_processing_threads = 4

# SIMD and vectorization
enable_simd = true
simd_instruction_set = AVX2
enable_vectorization = true

# Plasticity optimization
optimize_plasticity = true
plasticity_batch_processing = true
plasticity_thread_pool_size = 2

# Memory optimization
enable_memory_pools = true
enable_cache_friendly_layout = true
optimize_data_access_patterns = true

# Neuromodulation optimization
 neuromodulation_batch_processing = true
 neuromodulation_thread_pool_size = 2

# Environment
environment_name = LargeWorld
environment_width = 50
environment_height = 50
vision_width = 20
vision_height = 20

# Logging (minimal for performance)
log_level = WARNING
log_to_file = false  # Disable file logging for performance
console_output = true

# Simulation
max_simulation_steps = 20000
simulation_time_limit = 0.0
enable_perf_monitoring = true
performance_monitoring_interval = 1000
```

### 5. Debugging and Testing Template

**Purpose**: Configuration for development and debugging

**File**: `configs/debug.cfg`

```ini
# NLM Configuration - Debug and Testing
# Configuration for development, debugging, and testing

# Brain parameters
random_seed = 0  # Reproducible for testing
random_seed_random = true
simulation_timestep = 0.001
neuron_count = 100  # Small for fast testing
region_count = 1
connection_probability = 0.3  # High connectivity for testing

# Debug flags
debug_neuron = true
debug_synapse = true
debug_plasticity = true
debug_memory = true
debug_prediction = true

# Extended logging
log_level = DEBUG
log_to_file = true
log_filename = debug.log
log_format = verbose
log_include_timestamps = true
log_include_thread_ids = true

# Validation
enable_config_validation = true
strict_parameter_validation = true
validate_config_ranges = true

# Testing specific
run_unit_tests = true
run_integration_tests = true
enable_memory_leak_detection = true
enable_performance_benchmarks = true

# Simulation
max_simulation_steps = 1000
simulation_time_limit = 0.0
enable_step_debugging = true
enable_detailed_statistics = true

# Environment
environment_name = TestWorld
environment_width = 10
environment_height = 10
vision_width = 5
vision_height = 5
enable_test_mode = true
```

## Configuration File Guidelines

### 1. Section Organization

Configuration files can be organized with comments and sections:

```ini
# ===== BRAIN CONFIGURATION =====
random_seed = 42
neuron_count = 1000

# ===== PLASTICITY CONFIGURATION =====
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01

# ===== NEUROMODULATION CONFIGURATION =====
dopamine_baseline = 0.1
```

### 2. Parameter Validation

Most parameters should follow these constraints:

- **Counts** (neuron_count, region_count): Positive integers
- **Probabilities** (connection_probability): 0.0 to 1.0
- **Rates** (synaptogenesis_rate, pruning_rate): Non-negative
- **Timesteps** (simulation_timestep): Positive
- **Weights** (stdp_ltp_weight, stdp_ltd_weight): Non-negative

### 3. Inheritance and Overrides

Configuration can be inherited and overridden:

```ini
# base.cfg
neuron_count = 1000
connection_probability = 0.1

# experiment.cfg (overrides)
neuron_count = 2000
```

### 4. Variable Substitution

Some configuration systems support variable substitution:

```ini
# config.cfg
base_neurons = 1000
neuron_count = ${base_neurons} * 2
```

## Configuration Command-Line Interface

The NLM system supports command-line overrides of configuration values:

```bash
# Override individual parameters
./nlm --random_seed=123 --neuron_count=500 --connection_probability=0.2

# Use custom configuration file
./nlm --config myconfig.cfg

# Enable debug mode with custom settings
./nlm --debug --verbose --config debug.cfg

# Run specific demo with custom configuration
./nlm --demo phase6 --config cognitive.cfg
```

## Configuration Validation

### 1. Required Parameters

- `random_seed`: Random seed for reproducibility
- `simulation_timestep`: Simulation timestep in seconds
- `neuron_count`: Number of neurons
- `region_count`: Number of brain regions

### 2. Optional Parameters

Most parameters have sensible defaults, but can be overridden:

- `connection_probability`: Synapse formation probability
- `plasticity_learning_rate`: Hebbian learning rate
- `stdp_ltp_weight`: STDP LTP weight
- `stdp_ltd_weight`: STDP LTD weight
- `synaptogenesis_rate`: New synapse formation rate
- `pruning_rate`: Synapse elimination rate

### 3. Configuration Sources (in order of precedence)

1. **Default values** (hardcoded)
2. **Configuration file** (`configs/default.cfg`)
3. **Command-line arguments**
4. **Runtime settings** (via C++ API)

## Advanced Configuration Topics

### 1. Sparse Connectivity

For large-scale simulations, use sparse connectivity:

```ini
connection_probability = 0.01
neuron_count = 100000
```

This creates approximately 100 neurons per region on average.

### 2. Heterogeneous Networks

Create different neuron populations:

```ini
# Different neuron types
neuron_count = 3000
# Excitatory neurons
region_count = 3
# Sensory, Motor, and Association regions
```

### 3. Development Stages

Configure developmental processes:

```ini
development_critical_period_start = 100
development_critical_period_end = 1000
development_maturation_rate = 0.5
```

### 4. Memory System Configuration

Configure memory systems for specific tasks:

```ini
# Working memory
working_memory_capacity = 200
working_memory_decay_rate = 0.001

# Episodic memory
episodic_memory_capacity = 5000
episodic_memory_consolidation_threshold = 0.8
```

## Configuration Troubleshooting

### 1. Common Issues

**Error**: `Invalid neuron_count: must be positive`
**Solution**: Set `neuron_count` to a positive integer, e.g., `neuron_count = 100`

**Error**: `Invalid connection_probability: must be between 0 and 1`
**Solution**: Set `connection_probability` to a value between 0 and 1, e.g., `connection_probability = 0.15`

### 2. Performance Issues

**Symptom**: Slow simulation with many neurons
**Solutions**:
- Increase `simulation_timestep` (e.g., `simulation_timestep = 0.002`)
- Decrease `neuron_count`
- Use sparse connectivity (`connection_probability = 0.05`)

**Symptom**: Memory usage too high
**Solutions**:
- Use memory-efficient configurations
- Disable unnecessary features
- Use performance-optimized template

### 3. Debugging Issues

**Symptom**: No output or logging
**Solutions**:
- Set `log_level = DEBUG`
- Enable `log_to_file = true`
- Check file permissions

## Configuration Examples

### Minimal Working Configuration

```ini
# Minimal configuration that works
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
```

### High-Performance Configuration

```ini
# For research and production use
random_seed = 777
simulation_timestep = 0.002
neuron_count = 10000
region_count = 5
connection_probability = 0.05
log_level = WARNING
```

### Research Configuration

```ini
# For detailed research and analysis
random_seed = 999
simulation_timestep = 0.0005
neuron_count = 5000
region_count = 4
connection_probability = 0.08
log_level = DEBUG
log_to_file = true
enable_perf_monitoring = true
```

## Configuration Best Practices

### 1. For Reproducibility

- Always specify `random_seed`
- Use configuration files for experiment parameters
- Document configuration settings

### 2. For Performance

- Use appropriate `simulation_timestep` values
- Consider memory-efficient configurations for large networks
- Profile and optimize configuration

### 3. For Research

- Use descriptive configuration names
- Document experimental parameters
- Create configuration templates for different experiments

### 4. For Production

- Use performance-optimized configurations
- Monitor system resources
- Set appropriate log levels for production

## Conclusion

Configuration files are central to the NLM system's flexibility and extensibility. The templates provided above cover common use cases:

- **Basic learning experiments**: Simple, educational demonstrations
- **Developmental studies**: Age-dependent processes and critical periods
- **Advanced cognition**: Complex memory and planning tasks
- **Performance optimization**: High-efficiency simulations
- **Debugging and testing**: Development and validation tools

By using appropriate configuration files, researchers and developers can:

1. **Reproduce experiments** with consistent parameters
2. **Compare different approaches** using standardized configurations
3. **Optimize performance** with tailored settings
4. **Debug issues** with detailed configuration logging
5. **Extend the system** with custom configurations

The configuration system provides the foundation for exploring the full potential of the NLM neural simulation framework.