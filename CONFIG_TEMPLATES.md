# NLM Enhanced CLI - Configuration Templates

This document contains configuration templates for the NLM Enhanced CLI, designed to optimize different use cases and testing scenarios.

## Overview

Configuration files in NLM follow a simple key=value format and support multiple data types. This guide provides templates for common use cases and advanced configurations.

## Configuration File Format

### Basic Syntax
```
key = value
# Comments start with # or /
key "value with spaces" = "string value"
key = 42
key = 3.14
key = true
```

### Data Types
- **Integer**: `neuron_count = 1000`
- **Float**: `simulation_timestep = 0.001`
- **Boolean**: `log_to_file = true`
- **String**: `environment_name = "GridWorld"`
- **Lists**: `supported_actions = ["move", "turn", "wait"]`

## Configuration Templates

### 1. Basic Learning Configuration

**File**: `configs/learning.cfg`

```
# NLM Basic Learning Configuration
# Designed for learning experiments and plasticity testing

# Random seed for reproducible experiments
random_seed = 42

# Simulation settings
simulation_timestep = 0.001
simulation_time_limit = 0.0
max_simulation_steps = 10000

# Brain architecture
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity configuration
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Structural plasticity
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation
 dopamine_baseline = 0.0
dopamine_learning_rate = 0.1

curiosity_baseline = 0.0
curiosity_sensitivity = 0.5

novelty_baseline = 0.0
novelty_threshold = 0.3

# Reward system
reward_discount_factor = 0.99
reward_prediction_error_learning_rate = 0.01

# Environment configuration
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging
log_level = INFO
log_to_file = true
log_filename = nlm_learning.log

# Memory systems
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
associative_memory_pattern_count = 500

# Visualization
visualization_enabled = false
visualization_update_rate = 30.0
```

### 2. Performance Optimization Configuration

**File**: `configs/performance.cfg`

```
# NLM Performance Optimization Configuration
# Designed for high-performance simulation and large networks

# Random seed for reproducibility
random_seed = 12345

# High-performance simulation settings
simulation_timestep = 0.0001  # Smaller timestep for accuracy
max_simulation_steps = 100000  # More steps for long experiments

# Large-scale brain architecture
neuron_count = 10000
region_count = 4
connection_probability = 0.05  # Lower density for larger networks

# Optimized plasticity
plasticity_learning_rate = 0.005  # Slower learning for stability
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 15.0  # Faster decay

# Reduced structural plasticity for performance
development_synaptogenesis_rate = 0.00001
development_pruning_rate = 0.000001

# Neuromodulation with faster dynamics
dopamine_baseline = 0.1
dopamine_learning_rate = 0.05

curiosity_baseline = 0.05
curiosity_sensitivity = 0.3

novelty_baseline = 0.05
novelty_threshold = 0.2

# Efficient reward processing
reward_discount_factor = 0.95
reward_prediction_error_learning_rate = 0.02

# Optimized environment
environment_name = SimpleWorld
environment_width = 20
environment_height = 20

# Balanced logging for performance
log_level = WARNING  # Reduce log verbosity
log_to_file = true
log_filename = nlm_performance.log

# Memory optimization
working_memory_capacity = 500
episodic_memory_max_episodes = 5000
associative_memory_pattern_count = 2000

# Disabled visualization for performance
visualization_enabled = false

# Parallel processing settings (if supported)
enable_multithreading = true
thread_count = 4
enable_sparse_connectivity = true
enable_simd_optimization = true
```

### 3. Debug and Testing Configuration

**File**: `configs/debug.cfg`

```
# NLM Debug and Testing Configuration
# Designed for debugging, testing, and development

# Deterministic testing
random_seed = 0  # Fixed for reproducible test results

# Detailed simulation settings
simulation_timestep = 0.001
simulation_time_limit = 0.0
max_simulation_steps = 1000

# Small network for focused testing
neuron_count = 100
region_count = 1
connection_probability = 0.2  # Higher density for easier observation

# Detailed plasticity settings for debugging
plasticity_learning_rate = 0.1  # Large changes for observable effects
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.06
stdp_time_constant = 50.0  # Slower for observing effects

# Enhanced structural plasticity for debugging
development_synaptogenesis_rate = 0.01
development_pruning_rate = 0.001

# Detailed neuromodulation
 dopamine_baseline = 0.0
dopamine_learning_rate = 0.5  # Large changes

curiosity_baseline = 0.0
curiosity_sensitivity = 1.0

novelty_baseline = 0.0
novelty_threshold = 0.1  # Low threshold for frequent novel events

# Detailed reward system
reward_discount_factor = 0.999
reward_prediction_error_learning_rate = 0.1

# Maximum verbosity logging
log_level = DEBUG
log_to_file = true
log_filename = nlm_debug.log

# Large memory capacity for debugging
working_memory_capacity = 1000
episodic_memory_max_episodes = 5000
associative_memory_pattern_count = 1000

# Visualization enabled for debugging
visualization_enabled = true
visualization_update_rate = 10.0

# Enable all debugging features
enable_debug_mode = true
enable_validation_checks = true
enable_trace_logging = true
enable_performance_profiling = true
```

### 4. Minimal Configuration

**File**: `configs/minimal.cfg`

```
# NLM Minimal Configuration
# Bare-bones settings for testing basic functionality

# Simple random seed
random_seed = 42

# Minimal simulation settings
simulation_timestep = 0.001
max_simulation_steps = 100

# Small network
neuron_count = 10
region_count = 1
connection_probability = 0.5

# Basic plasticity
plasticity_learning_rate = 0.01

# No neuromodulation
 dopamine_baseline = 0.0
curiosity_baseline = 0.0
novelty_baseline = 0.0

# Simple environment
environment_name = SimpleWorld

# Minimal logging
log_level = INFO
log_to_file = false

# Disabled features for minimal test
visualization_enabled = false
```

### 5. Research Configuration

**File**: `configs/research.cfg`

```
# NLM Research Configuration
# Designed for scientific experiments and publications

# Controlled random seed for reproducibility
random_seed = 2024

# Precise simulation settings for research
# Use even smaller timesteps for precise measurements
simulation_timestep = 0.0001
simulation_time_limit = 0.0
max_simulation_steps = 50000  # Long experiments

# Medium-sized network for detailed analysis
neuron_count = 2000
region_count = 2
connection_probability = 0.08

# Carefully tuned plasticity for research
plasticity_learning_rate = 0.001  # Precise learning rate
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0  # Standard biological value

# Research-grade structural plasticity
development_synaptogenesis_rate = 0.00005
development_pruning_rate = 0.000005

# Neuromodulation with research values
 dopamine_baseline = 0.05
dopamine_learning_rate = 0.05

curiosity_baseline = 0.1
curiosity_sensitivity = 0.7

novelty_baseline = 0.1
novelty_threshold = 0.25

# Standard reward processing
reward_discount_factor = 0.95
reward_prediction_error_learning_rate = 0.01

# Professional logging for research
log_level = INFO
log_to_file = true
log_filename = nlm_research_$(date +\%Y\%m\%d).log

# Comprehensive memory for research
working_memory_capacity = 2000
episodic_memory_max_episodes = 10000
associative_memory_pattern_count = 5000

# Optional visualization for presentations
visualization_enabled = true
visualization_update_rate = 20.0

# Research-grade validation and checkpoints
enable_checkpointing = true
checkpoint_interval = 1000
checkpoint_path = ./research_checkpoints/

# Enable all scientific measurements
enable_science_mode = true
record_all_metrics = true
enable_biological_validation = true
```

## Configuration Command Line Reference

### Global Options

```bash
./nlm --help                    # Show all help messages
./nlm --version                 # Show version information
./nlm --interactive             # Start interactive mode
./nlm --config config.cfg        # Load specific configuration file
./nlm --verbose                 # Enable verbose logging
```

### Configuration Commands

```bash
# Show current configuration
./nlm config show
./nlm config show random_seed

# Set configuration values
./nlm config set neuron_count 2000
./nlm config set simulation_timestep 0.0005
./nlm config set log_level DEBUG

# Load configuration from file
./nlm config load custom_config.cfg

# Save configuration to file
./nlm config save current_config.cfg
```

### Brain Configuration

```bash
# Initialize brain with configuration file
./nlm brain init configs/learning.cfg

# Initialize brain with command-line overrides
./nlm brain init --neuron_count=2000 --region_count=4 --connection_probability=0.15

# Reset brain to factory defaults
./nlm brain reset

# Show current brain configuration
./nlm brain status
```

## Advanced Configuration Features

### Configuration Variable Substitution

NLM supports variable substitution in configuration files:

```
# In configs/advanced.cfg
base_neuron_count = 100
neuron_count = ${base_neuron_count}0  # Expands to 1000

simulation_timestep = 0.001
time_multiplier = 2.0
effective_timestep = ${simulation_timestep} * ${time_multiplier}
```

### Configuration Validation

The enhanced CLI validates configuration values:

```bash
# Validate current configuration
./nlm config validate

# Show configuration warnings
./nlm config warnings
```

### Configuration Inheritance

Configuration files can inherit from base templates:

```bash
# configs/project.cfg (inherits from default.cfg)
base_config = configs/default.cfg
include ${base_config}

# Additional project-specific settings
neuron_count = 500
simulation_timestep = 0.0005
```

## Configuration Best Practices

### 1. Start Simple

For new projects, start with `configs/minimal.cfg` and gradually add complexity:

```bash
./nlm brain init configs/minimal.cfg
./nlm simulation run 10
./nlm stats

# Add features incrementally
./nlm config set visualization_enabled true
./nlm simulation run 100
```

### 2. Use Descriptive Filenames

Use meaningful configuration filenames:

- `configs/debug.cfg` - For debugging and development
- `configs/performance.cfg` - For production use
- `configs/learning.cfg` - For learning experiments
- `configs/research.cfg` - For scientific research

### 3. Version Control Configurations

Track configuration changes:

```bash
# Create backup before major changes
cp configs/current.cfg configs/current_$(date +%Y%m%d).cfg

# Test configurations before use
./nlm brain init configs/test.cfg
```

### 4. Configuration Templates for Teams

Create team-specific configuration templates:

```
# configs/team_defaults.cfg
# Settings shared across team projects

# Common team parameters
log_level = INFO
log_filename = team_nlm_$(whoami).log
working_memory_capacity = 500
episodic_memory_max_episodes = 2000

# Environment team settings
environment_name = TeamWorld
environment_width = 15
environment_height = 15
```

### 5. Configuration Monitoring

Monitor configuration usage:

```bash
# Run with logging to track configuration usage
./nlm --config configs/learning.cfg --verbose

# Check configuration summaries
./nlm brain init configs/learning.cfg
./nlm brain status
```

## Configuration Troubleshooting

### Common Configuration Issues

#### Issue: Configuration file not found
**Solution**: Use absolute path or place file in working directory
```bash
./nlm brain init /absolute/path/to/config.cfg
```

#### Issue: Invalid configuration value
**Solution**: Check data types and valid ranges
```bash
# neuron_count must be positive integer
# simulation_timestep must be positive float
```

#### Issue: Permission denied
**Solution**: Check file permissions
```bash
chmod 644 configs/*.cfg
```

#### Issue: Configuration validation errors
**Solution**: Fix configuration issues and reload
```bash
# Fix configuration
nano configs/learning.cfg

# Reinitialize brain
./nlm brain init configs/learning.cfg
```

### Configuration Debug Commands

```bash
# Show all configuration warnings
./nlm config warnings

# Validate configuration syntax
./nlm config validate

# Show configuration differences
./nlm config diff old.cfg new.cfg

# Generate configuration report
./nlm config report
```

## Configuration Examples

### Example 1: Quick Start

```bash
# One-line initialization with defaults
./nlm brain init
```

### Example 2: Custom Learning Environment

```bash
# Create learning configuration
cat > configs/learning_env.cfg << EOF
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
log_level = INFO
log_to_file = true
log_filename = learning_env.log
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
visualization_enabled = false
EOF

# Initialize and run learning experiment
./nlm brain init configs/learning_env.cfg
./nlm plasticity enable
./nlm simulation run 1000 record
./nlm analyze learning
./nlm stats
```

### Example 3: Performance Testing

```bash
# Create performance configuration
cat > configs/performance_test.cfg << EOF
random_seed = 12345
simulation_timestep = 0.0001
neuron_count = 2000
region_count = 4
connection_probability = 0.05
enable_multithreading = true
thread_count = 4
enable_sparse_connectivity = true
log_level = WARNING
log_to_file = true
log_filename = performance_test.log
visualization_enabled = false
EOF

# Run performance benchmark
./nlm brain init configs/performance_test.cfg
./nlm benchmark 1000
```

### Example 4: Interactive Development

```bash
# Start interactive session with development configuration
./nlm --interactive --config configs/debug.cfg

# Inside interactive mode:
NLM> config set neuron_count 200
NLM> config set log_level DEBUG
NLM> brain init
NLM> neurons list
NLM> simulation run 50
NLM> config set visualization_enabled true
NLM> neurons inject 42 50.0
NLM> stats
NLM> save development_state.checkpoint
NLM> quit
```

## Conclusion

The NLM enhanced configuration system provides flexible, powerful, and easy-to-use configuration management for brain simulation projects. By using the configuration templates and best practices outlined in this guide, you can:

1. **Quickly start** new projects with appropriate templates
2. **Optimize performance** with specialized configurations
3. **Debug effectively** with detailed logging and validation
4. **Maintain consistency** across team projects
5. **Scale efficiently** from minimal to production configurations

This configuration system, combined with the enhanced CLI, makes NLM more accessible, powerful, and professional for researchers, developers, and users at all levels.
