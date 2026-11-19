# Configuration Files for NLM Brain

This document describes the example configuration files included in the NLM project to help users get started with the new flat key configuration system.

## Overview

The NLM project includes several configuration file templates to help users:

1. **Basic Configuration** (`configs/default.cfg`) - Simple configuration for getting started
2. **Development Configuration** (`configs/development.cfg`) - Configuration optimized for development and testing
3. **Performance Configuration** (`configs/performance.cfg`) - High-performance configuration for production
4. **Debug Configuration** (`configs/debug.cfg`) - Debug configuration with verbose logging

## Configuration File Format

The configuration system supports two key formats:

### Flat Keys (Recommended)
```ini
neuron_count = 1000
connection_probability = 0.15
synaptogenesis_rate = 0.001
world.width = 30
world.height = 30
```

### Legacy Keys (Still Supported)
```ini
brain.neuron_count = 1000
brain.connection_probability = 0.15
brain.synaptogenesis_rate = 0.001
```

### Mixed Format
```ini
# Combination of both formats
neuron_count = 1000
brain.connection_probability = 0.15  # Use legacy for testing
world.width = 40
world.height = 40
```

## Example Configuration Files

### 1. Default Configuration (`configs/default.cfg`)

```ini
# NLM Default Configuration
# This configuration provides balanced settings for most use cases

# Brain Configuration
neuron_count = 1000
connection_probability = 0.15
synaptogenesis_rate = 0.001
pruning_rate = 0.00001

# Plasticity Configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Memory Configuration
memory.working_capacity = 100
memory.episodic.max_episodes = 1000
memory.associative.pheromone_decay = 0.1

# Development Configuration
development.stage = 4
development.initial_plasticity = 1.0
development.critical_period_start = 100
development.critical_period_end = 500

# World Configuration
world.width = 20
world.height = 20
world.vision_width = 8
world.vision_height = 8
world.max_energy = 100.0

# Simulation Configuration
simulation.timestep = 0.001
simulation.max_steps = 10000
simulation.replay_interval = 100
simulation.consolidation_interval = 1000
```

### 2. Development Configuration (`configs/development.cfg`)

```ini
# NLM Development Configuration
# Optimized for development, testing, and experimentation

# Brain Configuration - Smaller for faster iteration
debug_neuron_count = 500
connection_probability = 0.1

# Development Focus
development.stage = 5
development.initial_plasticity = 0.8
development.critical_period_start = 50
development.critical_period_end = 300

# Enhanced Monitoring
debug.level = "verbose"
log.all_systems = true
enable.performance_metrics = true

# Memory - More frequent consolidation
memory.working_capacity = 200
memory.episodic.max_episodes = 2000
memory.associative.pheromone_decay = 0.2

# Plasticity - Higher learning rates for faster learning
plasticity.stdp.learning_rate = 0.02
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation - Strong motivation
neuromod.dopamine.scale = 2.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# World - Smaller environment for testing
world.width = 15
world.height = 15
world.vision_width = 6
world.vision_height = 6
world.max_energy = 50.0

# Simulation - Faster iteration
simulation.timestep = 0.001
simulation.max_steps = 5000
simulation.replay_interval = 50
simulation.consolidation_interval = 500

# Checkpoint Configuration
checkpoint.enabled = true
checkpoint.interval = 100
checkpoint.directory = "./dev_checkpoints"
```

### 3. Performance Configuration (`configs/performance.cfg`)

```ini
# NLM Performance Configuration
# Optimized for maximum performance and throughput

# Brain Configuration - Larger for more computation
neuron_count = 10000
connection_probability = 0.01
synaptogenesis_rate = 0.0005
pruning_rate = 0.000001

# Performance Optimizations
performance.enable_parallel = true
performance.enable_vectorization = true
performance.use_memory_pools = true
performance.optimization_level = "aggressive"

# Plasticity - Optimized for performance
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.005
plasticity.hebbian.enable = false  # Disable for speed
plasticity.structural.enable = false  # Disable for speed

# Neuromodulation - Light weighting
neuromod.dopamine.scale = 0.5
neuromod.curiosity.enable = false  # Disable for speed
neuromod.novelty.enable = false  # Disable for speed

# Memory - Optimized for performance
memory.working_capacity = 500
memory.episodic.max_episodes = 100
memory.associative.pheromone_decay = 0.05

# Development - Fixed for consistent performance
development.stage = 4
development.initial_plasticity = 1.0
development.critical_period_start = 100
development.critical_period_end = 500

# World - Larger for complex behavior
world.width = 100
world.height = 100
world.vision_width = 16
world.vision_height = 16
world.max_energy = 1000.0

# Simulation - Optimized for performance
simulation.timestep = 0.0005
simulation.max_steps = 50000
simulation.replay_interval = 1000
simulation.consolidation_interval = 10000

# Checkpoint Configuration
checkpoint.enabled = true
checkpoint.interval = 5000
checkpoint.directory = "./perf_checkpoints"
checkpoint.compression = "high"
```

### 4. Debug Configuration (`configs/debug.cfg`)

```ini
# NLM Debug Configuration
# Optimized for debugging and troubleshooting

# Brain Configuration - Standard size
debug_neuron_count = 200
connection_probability = 0.05

# Debug Mode
debug.level = "debug"
log.all_systems = true
enable.break_on_error = true
enable.detailed_timing = true
enable.memory_tracking = true

# Memory Configuration - Detailed tracking
memory.working_capacity = 50
memory.episodic.max_episodes = 100
memory.associative.pheromone_decay = 0.1

# Plasticity Configuration - Detailed logging
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development Configuration - Detailed tracking
development.stage = 4
development.initial_plasticity = 1.0
development.critical_period_start = 100
development.critical_period_end = 500

# World Configuration - Smaller for easier debugging
world.width = 10
world.height = 10
world.vision_width = 4
world.vision_height = 4
world.max_energy = 10.0

# Simulation Configuration - Frequent checkpoints
simulation.timestep = 0.001
simulation.max_steps = 1000
simulation.replay_interval = 10
simulation.consolidation_interval = 100

# Debug Outputs
debug.output_file = "debug_output.txt"
debug.include_stack_traces = true
debug.include_memory_dumps = true
```

## Configuration File Templates

### Simple Brain Experiment

```ini
# For quick experiments with a simple brain
simple = neuron_count=100,connection_probability=0.1

# Usage in command line
nlm --config=simple.cfg
```

### Learning Experiment

```ini
# Configuration optimized for learning experiments
learning = 
    neuron_count=1000,
    development.stage=4,
    development.critical_period_start=50,
    development.critical_period_end=500,
    neuromod.curiosity.enable=true,
    neuromod.novelty.enable=true,
    plasticity.stdp.learning_rate=0.01
```

### Social Learning Experiment

```ini
# Configuration for social learning studies
social = 
    neuron_count=2000,
    world.width=30,
    world.height=30,
    development.stage=5,
    memory.episodic.max_episodes=500,
    neuromod.curiosity.enable=true
```

### Development Study

```ini
# Configuration for developmental studies
development_study =
    neuron_count=1500,
    development.stage=6,
    development.critical_period_start=25,
    development.critical_period_end=750,
    development.initial_plasticity=0.9,
    plasticity.stdp.learning_rate=0.005
```

## Configuration File Extensions

### Template File (`.template.cfg`)

Configuration files can be created as templates that specify which parameters are required:

```ini
# template.cfg - Template for required parameters
required = neuron_count, connection_probability, development.stage

# Template validation requires these parameters
description = Template for basic NLM configuration
```

### Validation Configuration (`.validate.cfg`)

Configuration validation rules:

```ini
# validate.cfg - Validation rules
min.neuron_count = 10
max.neuron_count = 100000

min.connection_probability = 0.001
max.connection_probability = 1.0

required.development.stage = true
required.world.width = true
```

## Configuration File Naming Convention

### Standard Filenames

| Purpose | Filename | Example |
|---------|----------|---------|
| Default | `default.cfg` | No additional arguments |
| Development | `development.cfg` | `--config=development.cfg` |
| Performance | `performance.cfg` | `--config=performance.cfg` |
| Debug | `debug.cfg` | `--config=debug.cfg` |
| Custom | `custom.cfg` | `--config=custom.cfg` |

### Configuration File Path Examples

```bash
# Current directory
nlm --config=./myconfig.cfg

# Subdirectory
nlm --config=./configs/experiment.cfg

# Home directory
nlm --config=~/nlm/config.cfg

# Relative path
nlm --config=configs/debug.cfg
```

## Configuration File Migration Guide

### From Legacy to New System

If you have existing configurations in the legacy format, you can migrate them:

```ini
# Legacy format (still supported)
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05

# Convert to new flat format
neuron_count = 1000
synapse_density = 0.1
connection_probability = 0.05
```

### Configuration File Migration Script

```bash
#!/bin/bash
# migrate_config.sh - Migrate legacy configuration to new format

INPUT_FILE="legacy_config.cfg"
OUTPUT_FILE="migrated_config.cfg"

# Convert legacy dot notation to flat keys
sed 's/brain\.neuron_count = /neuron_count = /g' "$INPUT_FILE" | \
sed 's/brain\.synapse_density = /synapse_density = /g' | \
sed 's/brain\.connection_probability = /connection_probability = /g' | \
sed 's/brain\.v_thresh = /v_thresh = /g' | \
sed 's/brain\.v_rest = /v_rest = /g' | \
sed 's/brain\.v_reset = /v_reset = /g' | \
sed 's/brain\.tau_mem = /tau_mem = /g' | \
sed 's/brain\.tau_ref = /tau_ref = /g' > "$OUTPUT_FILE"

 echo "Migration complete: $INPUT_FILE → $OUTPUT_FILE"
```

## Configuration File Best Practices

### For Users

1. **Use provided templates**: Start with one of the provided configuration files
2. **Document configurations**: Keep comments explaining what each parameter does
3. **Backup configurations**: Keep backup copies of important configurations
4. **Test configurations**: Test configurations before running simulations
5. **Version configurations**: Keep track of configuration versions for experiments

### For Developers

1. **Document configuration parameters**: Provide clear documentation for each parameter
2. **Validate configurations**: Always validate configurations at load time
3. **Provide templates**: Create reusable configuration templates
4. **Test configurations**: Test configurations with unit tests
5. **Maintain backward compatibility**: Legacy configurations should still work

## Configuration File Examples in Command Line

### Using Configuration Files

```bash
# Use default configuration
nlm

# Use custom configuration
nlm --config=myexperiment.cfg

# Use development configuration
nlm --config=development.cfg --verbose

# Use performance configuration
nlm --config=performance.cfg --profile

# Use debug configuration
nlm --config=debug.cfg --quiet
```

### Combining Configuration Files

```bash
# Merge configuration files (file 2 overrides file 1)
nlm --config=development.cfg --neuron_count=2000 --world.width=40

# Override specific parameters
nlm --config=performance.cfg --plasticity.stdp.learning_rate=0.02
```

## Configuration File Troubleshooting

### Common Issues and Solutions

1. **File not found**: Use absolute paths or relative paths
2. **Invalid key format**: Use flat keys or legacy keys
3. **Invalid value format**: Check data types and ranges
4. **Missing required parameters**: Add missing required parameters
5. **Parameter conflicts**: Resolve conflicting parameter values

### Troubleshooting Commands

```bash
# Check configuration file syntax
nlm --config=myconfig.cfg --verbose

# Validate configuration
nlm --config=myconfig.cfg --validate

# Show configuration summary
nlm --config=myconfig.cfg --config-output

# Test configuration
nlm --config=myconfig.cfg --test=1
```

## Conclusion

The NLM configuration system provides:

- **Multiple configuration files**: Different configurations for different use cases
- **Template system**: Reusable configuration patterns
- **Migration tools**: Tools to migrate from legacy configurations
- **Best practices**: Guidelines for using configurations effectively
- **Documentation**: Comprehensive documentation for all configurations

These configuration files make it easier to configure NLM experiments for various research scenarios while maintaining ease of use and flexibility.