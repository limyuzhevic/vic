# NLM Configuration Examples

This directory contains example configuration files for different use cases and experiment scenarios.

## Overview

These configuration files demonstrate the new NLM configuration system with various options for different experiment modes, performance levels, and research scenarios.

## Configuration Structure

The NLM configuration system uses key-value pairs with support for:

- Basic types: int, float, string, bool
- Arrays: lists of values
- Source tracking: configuration can come from file, command line, or defaults

## Configuration Categories

### 1. Basic Configuration
Configuration files for basic simulation setup.

#### `configs/basic.cfg`
```ini
# Basic simulation configuration
# For simple testing and demonstration

random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development settings
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Logging
log_level = INFO
verbosity = 1
```

### 2. Research Configuration
Configuration files for research experiments and parameter sweeps.

#### `configs/research_basic.cfg`
```ini
# Research configuration - fine timestep for accuracy
random_seed = 42
simulation_timestep = 0.001
neuron_count = 5000
region_count = 5
connection_probability = 0.3

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development parameters
initial_stage_age = 60.0
critical_period_age = 300.0
maturation_stage_age = 900.0

# Neuromodulation parameters
dopamine_baseline = 0.1

# Performance settings
log_level = INFO
checkpoint_enabled = true
checkpoint_interval = 1000
```

#### `configs/research_performance.cfg`
```ini
# High-performance research configuration
random_seed = 42
simulation_timestep = 0.01  # Larger timestep for speed
neuron_count = 2000          # Fewer neurons for faster simulation
region_count = 3             # Fewer regions
connection_probability = 0.2  # Moderate connectivity

# Performance optimization
log_level = WARN              # Less logging overhead
verbosity = 0               # Minimal output
checkpoint_enabled = false    # Skip checkpoints for speed
enable_profiling = true       # Enable performance profiling

# Research parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
```

### 3. Debugging Configuration
Configuration files for development and debugging.

#### `configs/debug.cfg`
```ini
# Development and debugging configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100           # Small for quick testing
region_count = 1              # Simple structure
connection_probability = 0.5  # Higher connectivity for debugging

# Debugging settings
log_level = DEBUG             # Maximum verbosity
verbosity = 3               # Full debug output
log_to_file = true            # Log to file for analysis
log_filename = debug_nlm.log
checkpoint_enabled = true     # Save progress frequently
checkpoint_interval = 100     # Save every 100 steps

# Agent debugging
validation_enabled = true     # Validate all operations
enable_profiling = true       # Profile all operations
```

### 4. Experiment-Specific Configurations

#### `configs/experiment_learning.cfg`
```ini
# Configuration for learning experiments
mode = learning
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.3

# STDP parameters for learning
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development for learning
initial_stage_age = 60.0
critical_period_age = 300.0
maturation_stage_age = 900.0

# Neuromodulation for learning
dopamine_baseline = 0.1

# Experiment settings
epoch_count = 10
train_steps_per_epoch = 1000
validation_enabled = true
export_results = true
result_format = json
```

#### `configs/experiment_plasticity.cfg`
```ini
# Configuration for plasticity experiments
mode = plasticity
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.2

# Enhanced plasticity settings
stdp_ltp_weight = 0.03  # Stronger potentiation
stdp_ltd_weight = 0.02  # Stronger depression
stdp_tau = 15.0         # Faster decay

# Structural plasticity
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

# Development for plasticity
initial_stage_age = 60.0
critical_period_age = 300.0
maturation_stage_age = 900.0

# Novelty detection for exploration
novelty_enabled = true
validation_enabled = true

# Experiment settings
epoch_count = 20
train_steps_per_epoch = 2000
export_results = true
result_format = json
```

#### `configs/experiment_stdp.cfg`
```ini
# Configuration for STDP verification experiments
mode = stdp
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.4

# STDP-specific parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Enable only STDP plasticity (disable others)
plasticity_hebbian = false
plasticity_structural = false
plasticity_development = false

# Novelty for exploration
novelty_enabled = true
curiosity_enabled = true

# Experiment settings
epoch_count = 5
train_steps_per_epoch = 500
validation_enabled = true
export_results = true
result_format = json
```

### 5. Advanced Configuration
Configuration files for advanced users and power users.

#### `configs/advanced_performance.cfg`
```ini
# Advanced performance configuration
random_seed = 42
simulation_timestep = 0.005  # Trade accuracy for speed
neuron_count = 3000           # Medium scale
region_count = 4              # Multiple regions
connection_probability = 0.25  # Sparse connectivity

# Performance optimization
log_level = INFO
verbosity = 1
checkpoint_enabled = true
checkpoint_interval = 500
enable_profiling = true

# Advanced agent features
agent_brain_enabled = true
reward_modulation_enabled = true
structural_plasticity_enabled = true
development_enabled = true
curiosity_enabled = true
novelty_enabled = true
validation_enabled = true

# Advanced development
initial_stage_age = 60.0
critical_period_age = 300.0
maturation_stage_age = 900.0

# Advanced neuromodulation
dopamine_baseline = 0.1
expected_reward_decay = 0.95
reward_update_rate = 0.05

# Integration features
visualization_enabled = false  # Disable for performance
max_simulation_steps = 50000   # Longer simulation
simulation_time_limit = 0.0    # No time limit
```

#### `configs/advanced_research.cfg`
```ini
# Advanced research configuration
random_seed = 42
simulation_timestep = 0.001  # Fine timestep for accuracy
neuron_count = 10000          # Large scale
region_count = 10             # Many regions
connection_probability = 0.15  # Sparse connectivity

# Research-grade settings
log_level = INFO
verbosity = 2
checkpoint_enabled = true
checkpoint_interval = 1000
enable_profiling = true

# Full feature set
agent_brain_enabled = true
reward_modulation_enabled = true
structural_plasticity_enabled = true
development_enabled = true
curiosity_enabled = true
novelty_enabled = true
validation_enabled = true

# Advanced development
initial_stage_age = 60.0
critical_period_age = 300.0
maturation_stage_age = 900.0

# Advanced neuromodulation
dopamine_baseline = 0.1
expected_reward_decay = 0.95
reward_update_rate = 0.05

# Research features
max_simulation_steps = 200000  # Very long simulation
simulation_time_limit = 3600.0  # 1 hour limit
export_results = true           # Always export results
result_format = detailed_json  # Full result format
```

### 6. Environment-Specific Configurations

#### `configs/development.cfg`
```ini
# Development environment configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.5

# Development optimizations
log_level = DEBUG
verbosity = 3
log_to_file = true
log_filename = development_nlm.log
checkpoint_enabled = true
checkpoint_interval = 100
enable_profiling = true

# Full validation
validation_enabled = true
enable_strict_validation = true

# Quick iterations
max_simulation_steps = 1000
epoch_count = 3
train_steps_per_epoch = 100
```

#### `configs/production.cfg`
```ini
# Production environment configuration
random_seed = 42
simulation_timestep = 0.01
neuron_count = 5000
region_count = 3
connection_probability = 0.2

# Production optimizations
log_level = INFO
verbosity = 1
checkpoint_enabled = true
checkpoint_interval = 1000
enable_profiling = false
log_to_file = true
log_filename = production_nlm.log

# Reliable operation
validation_enabled = true
max_simulation_steps = 100000
simulation_time_limit = 3600.0

# Standard parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
```

### 7. Command Line Override Examples

#### `configs/with_command_line.cfg`
```ini
# This configuration can be overridden via command line
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# These can be overridden with --key=value
# stdp_ltp_weight = 0.02
# neuron_count = 1000
# log_level = INFO
```

### Command Line Usage Examples

#### Override Configuration
```bash
# Override specific parameters
./nlm --config=configs/with_command_line.cfg \
     --stdp_ltp_weight=0.03 \
     --neuron_count=2000 \
     --log_level=DEBUG
```

#### Experiment Mode
```bash
# Run with specific experiment mode
./nlm --config=configs/experiment_learning.cfg --mode=learning
```

#### Verbosity Levels
```bash
# Different verbosity levels
./nlm -v           # Basic info
./nlm -vv          # Detailed info
./nlm -vvv         # Debug info
```

#### Configuration File Discovery
```bash
# NLM will search for configs in this order:
# 1. --config parameter (highest priority)
# 2. configs/default.cfg
# 3. configs/config.cfg
# 4. nlm.cfg (project root)
# 5. .nlm.cfg (project root) (lowest priority)

# Example with auto-discovery
./nlm  # Uses default.cfg
```

## Configuration Validation

### Common Issues and Solutions

#### 1. Configuration File Not Found
**Problem**: "Using default configuration."

**Solution**:
```bash
# Use absolute path
./nlm --config=/full/path/to/config.cfg

# Copy to standard location
cp custom.cfg configs/default.cfg
```

#### 2. Invalid Configuration Values
**Problem**: Crashes or unexpected behavior

**Solution**:
```bash
# Use valid configuration files
# Check available examples in configs/ directory
./nlm --config=configs/experiment_learning.cfg

# Use --verbose for error messages
./nlm --config=bad_config.cfg --verbose
```

#### 3. Invalid Configuration Format
**Problem**: Parsing errors

**Solution**:
```ini
# Correct format (key = value)
random_seed = 42
simulation_timestep = 0.001
log_level = INFO

# Avoid these formats (key:value)
# random_seed: 42
# simulation_timestep: 0.001
```

### Configuration Validation Commands

```bash
# Validate configuration file
./nlm --config=configs/experiment_learning.cfg --validate

# Check configuration syntax
python3 -c "
import configparser
config = configparser.ConfigParser()
config.read('configs/experiment_learning.cfg')
for section in config.sections():
    for key, value in config.items(section):
        print(f'{key} = {value}')
"
```

## Configuration File Templates

### Template 1: Simple Test
```ini
# Simple test configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.5
log_level = INFO
```

### Template 2: Learning Experiment
```ini
# Learning experiment configuration
mode = learning
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.3

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development
initial_stage_age = 60.0
critical_period_age = 300.0
maturation_stage_age = 900.0

# Neuromodulation
dopamine_baseline = 0.1

# Experiment settings
epoch_count = 10
train_steps_per_epoch = 1000
```

### Template 3: Performance Test
```ini
# Performance test configuration
random_seed = 42
simulation_timestep = 0.01
neuron_count = 5000
region_count = 5
connection_probability = 0.2

# Performance settings
log_level = WARN
verbosity = 0
checkpoint_enabled = false
enable_profiling = true

# Standard plasticity
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
```

## Configuration Best Practices

### 1. Use Descriptive Names
```ini
# Good: Clear parameter names
random_seed = 42
simulation_timestep = 0.001

# Avoid: Ambiguous or unclear names
unknown_param = 42
```

### 2. Add Comments
```ini
# Configuration for learning experiments
random_seed = 42  # For reproducible results
simulation_timestep = 0.001  # Fine timestep for accuracy
neuron_count = 2000  # Enough neurons for interesting behavior
region_count = 3  # Multiple regions for diversity
```

### 3. Validate Values
```ini
# Validate numerical ranges
random_seed = 42  # Should be positive
assert simulation_timestep > 0.0  # Should be positive
assert neuron_count > 0  # Should be positive
```

### 4. Use Consistent Naming
```ini
# Consistent naming convention
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.3
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
```

### 5. Document Configuration
```ini
# Document important settings with comments

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds (smaller = more accurate, but slower)
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 2000

# Number of brain regions (more regions = more complexity)
region_count = 3

# Probability of synaptic connections (higher = more connected)
connection_probability = 0.3
```

## Configuration Integration Examples

### Example: Integration Test Configuration
```ini
# Configuration for comprehensive integration test
mode = integration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.2

# Enable all features
agent_brain_enabled = true
reward_modulation_enabled = true
structural_plasticity_enabled = true
development_enabled = true
curiosity_enabled = true
novelty_enabled = true
validation_enabled = true

# Integration test settings
epoch_count = 5
max_simulation_steps = 10000
export_results = true
result_format = json
```

### Example: Parameter Sweep Configuration
```ini
# Configuration for parameter sweep experiment
mode = custom
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.2

# Parameters to sweep
# These will be overridden by command line
# stdp_ltp_weight = 0.02
# stdp_ltd_weight = 0.015
# initial_stage_age = 60.0

# Experiment settings
epoch_count = 3
train_steps_per_epoch = 1000
export_results = true
result_format = json
```

### Command Line for Parameter Sweep
```bash
# Run parameter sweep
./nlm --config=configs/experiment_learning.cfg \
     --epoch_count=5 \
     --train_steps_per_epoch=2000 \
     --stdp_ltp_weight=0.025 \
     --initial_stage_age=90.0
```

## Conclusion

The NLM configuration system provides:

- **Flexibility**: Wide range of configuration options
- **Extensibility**: Easy to add new parameters
- **Validation**: Comprehensive configuration validation
- **Performance**: Optimized for different use cases
- **Documentation**: Clear examples and templates

These configuration examples help users:
- **Get started quickly**: Use basic configuration
- **Conduct research**: Use research-specific configurations
- **Debug issues**: Use debugging configurations
- **Optimize performance**: Use performance configurations
- **Test experiments**: Use experiment-specific configurations
- **Customize behavior**: Use advanced configurations

The configuration system is designed to be flexible yet powerful, supporting everything from simple testing to large-scale research experiments.

---

*Configuration Examples Version: 2.0*
*Last Updated: October 2026*
*Target Audience: Users, Researchers, Developers*