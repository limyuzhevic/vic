# NLM User Guide

**Neural Learning Machine (熙然) - User Documentation**

This guide documents the NLM command-line interface, configuration system, and available functionality.

## Table of Contents

1. [Overview](#overview)
2. [Command-Line Interface](#command-line-interface)
   - [Available Executables](#available-executables)
   - [Common Options](#common-options)
   - [Mode Selection](#mode-selection)
3. [Configuration Files](#configuration-files)
   - [Default Configuration](#default-configuration)
   - [Configuration Schema](#configuration-schema)
   - [Validation](#validation)
4. [Quick Start Examples](#quick-start-examples)
5. [Advanced Usage](#advanced-usage)
   - [Custom Configuration](#custom-configuration)
   - [Performance Tuning](#performance-tuning)
   - [Debugging](#debugging)
6. [Troubleshooting](#troubleshooting)
7. [References](#references)

## Overview

NLM is an experimental artificial developmental brain system that implements neural computation, learning, and development. It provides multiple experimental modes ranging from basic neural computation to full cognitive integration.

### Available Executables

After building with CMake, you will have the following executables available:

- **`nlm`** - Phase 2 basic neural computation demo
- **`nlm_phase3_demo`** - Phase 3 world interaction demo
- **`nlm_phase4_demo`** - Phase 4 cognitive integration demo
- **`nlm_phase6_demo`** - Phase 6 final integration test

Each executable demonstrates different aspects of the NLM architecture:

| Executable | Phase | Focus | Key Features |
|------------|-------|-------|--------------|
| `nlm` | 2 | Basic Neural Computation | LIF neurons, STDP, Hebbian plasticity |
| `nlm_phase3_demo` | 3 | World Interaction | Sensory-motor loop, reward learning |
| `nlm_phase4_demo` | 4 | Cognitive Integration | Working memory, prediction, planning |
| `nlm_phase6_demo` | 6 | Full Integration | All systems working together |

## Command-Line Interface

### Available Executables

The NLM suite provides executables for different experimental phases:

#### Basic Neural Computation (`nlm`)

```bash
# Default - run all Phase 2 tests
./nlm

# Run only basic connectivity test
./nlm --mode --basic

# Run only plasticity learning experiment
./nlm --mode --plasticity

# Run only STDP verification
./nlm --mode --stdp

# Run all tests sequentially
./nlm --mode --all
```

**Description**: Demonstrates real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons, event-driven spike propagation, STDP and Hebbian plasticity rules, and structural plasticity.

#### Phase 3 World Interaction (`nlm_phase3_demo`)

```bash
# Run Phase 3 demo
./nlm_phase3_demo
```

**Description**: Tests world interaction with sensory processing, motor output, and basic reward learning in a simulated environment.

#### Phase 4 Cognitive Integration (`nlm_phase4_demo`)

```bash
# Run Phase 4 demo
./nlm_phase4_demo
```

**Description**: Demonstrates emergent cognitive capabilities including working memory, episodic memory, prediction, and planning.

#### Phase 6 Integration Test (`nlm_phase6_demo`)

```bash
# Run comprehensive Phase 6 integration test
./nlm_phase6_demo
```

**Description**: Verifies that all brain systems work together as a coherent artificial brain. This is the most comprehensive test of the NLM architecture.

### Common Options

All NLM executables support these common options:

| Option | Description | Example |
|--------|-------------|---------|
| `--config FILE` | Load configuration from FILE | `--config myconfig.cfg` |
| `--help` | Show help message | `--help` |
| `--verbose LEVEL` | Set verbosity level (0-3) | `--verbose 2` |
| `--neuron-count N` | Override neuron count | `--neuron-count 2000` |
| `--steps S` | Override simulation steps | `--steps 5000` |
| `--timestep T` | Override simulation timestep | `--timestep 0.0005` |

### Mode Selection (nlm only)

The `nlm` executable (Phase 2) allows selecting which tests to run:

| Option | Short Form | Description |
|--------|------------|-------------|
| `--basic` | `-b` | Run basic connectivity test only |
| `--plasticity` | `-p` | Run plasticity learning experiment only |
| `--stdp` | `-s` | Run STDP verification only |
| `--all` | `-a` | Run all tests sequentially (default) |

**Examples**:

```bash
# Run only STDP verification
./nlm --mode --stdp

# Run plasticity experiment with custom neurons
./nlm --mode --plasticity --neuron-count 1000

# Run all tests with increased verbosity
./nlm --mode --all --verbose 3
```

## Configuration Files

### Default Configuration

The default configuration file is located at `configs/default.cfg`. Here's the current structure:

```ini
# NLM Configuration File
# This is the default configuration for Phase 6

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Memory system settings
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
replay_interval = 100
consolidation_interval = 1000

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Configuration Schema

The NLM configuration system includes schema validation to ensure required fields are present and have correct types. The default schema includes:

#### Required Fields (Phase 2+)

| Key | Type | Description | Default |
|-----|------|-------------|---------|
| `random_seed` | int64 | Random seed for reproducible experiments | 42 |
| `simulation_timestep` | double | Simulation timestep in seconds | 0.001 |
| `neuron_count` | int64 | Number of neurons in the brain | 1000 |
| `region_count` | int64 | Number of brain regions | 2 |
| `connection_probability` | float | Connection probability | 0.1 |

#### Optional Fields

| Key | Type | Description |
|-----|------|-------------|
| `plasticity_learning_rate` | float | Learning rate for plasticity |
| `stdp_ltp_weight` | float | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | STDP long-term depression weight |
| `stdp_time_constant` | float | STDP time constant |
| `development_synaptogenesis_rate` | float | Synaptogenesis rate |
| `development_pruning_rate` | float | Pruning rate |
| `dopamine_baseline` | float | Dopamine baseline level |
| `reward_discount_factor` | float | Reward discount factor |
| `environment_name` | string | Environment name |
| `environment_width` | int | Environment width |
| `environment_height` | int | Environment height |
| `log_level` | string | Logging level (DEBUG, INFO, WARN, ERROR) |
| `log_to_file` | bool | Whether to log to file |
| `log_filename` | string | Log filename |
| `max_simulation_steps` | int64 | Maximum simulation steps |
| `simulation_time_limit` | double | Simulation time limit |

### Validation

The configuration system validates that required fields are present and have appropriate types. If validation fails, warnings are logged and defaults are used.

You can enable validation by setting the `NLM_VALIDATE_CONFIG` environment variable:

```bash
export NLM_VALIDATE_CONFIG=1
./nlm
```

## Quick Start Examples

### Example 1: Basic Usage

Run the default Phase 2 neural computation demo:

```bash
./nlm
```

### Example 2: Custom Configuration

Create a custom configuration file and run the system:

```bash
# Create custom config
cat > myconfig.cfg << EOF
random_seed = 12345
neuron_count = 500
connection_probability = 0.2
visualization_enabled = true
EOF

# Run with custom config
./nlm --config myconfig.cfg
```

### Example 3: Performance Testing

Run a larger simulation with custom parameters:

```bash
# Run with 2000 neurons for 10000 steps
./nlm --neuron-count 2000 --steps 10000 --verbose 2
```

### Example 4: Specific Test Mode

Run only the STDP verification test:

```bash
./nlm --mode --stdp
```

### Example 5: Full Integration Test

Run the comprehensive Phase 6 integration test:

```bash
./nlm_phase6_demo
```

## Advanced Usage

### Custom Configuration

Create advanced configurations by editing the config file. Here are some examples:

#### High Plasticity Configuration

```ini
# High plasticity configuration for rapid learning
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.04
development_synaptogenesis_rate = 0.01
development_pruning_rate = 0.002
reward_discount_factor = 0.95
```

#### Fast Simulation Configuration

```ini
# Fast simulation for prototyping
simulation_timestep = 0.0005
max_simulation_steps = 5000
neuron_count = 200
region_count = 1
```

#### Memory-Intensive Configuration

```ini
# Configuration optimized for memory testing
working_memory_capacity = 500
episodic_memory_max_episodes = 5000
replay_interval = 50
consolidation_interval = 500
```

### Performance Tuning

For better performance in production:

1. **Increase timestep**: Use larger timesteps for faster simulation
2. **Reduce neurons**: Start with smaller networks for prototyping
3. **Disable visualization**: Set `visualization_enabled = false`
4. **Enable logging to file**: Set `log_to_file = true` for detailed analysis

### Debugging

Enable debug logging for troubleshooting:

```bash
# Enable debug logging
./nlm --verbose 3

# Or set environment variable
NLM_VERBOSE=3 ./nlm
```

### Common Issues and Solutions

#### Issue: Configuration file not found
**Solution**: Use absolute path or ensure file is in the working directory:

```bash
./nlm --config ./configs/myconfig.cfg
```

#### Issue: Insufficient memory
**Solution**: Reduce neuron count or increase system memory:

```bash
./nlm --neuron-count 200
```

#### Issue: Long simulation times
**Solution**: Reduce steps or increase timestep:

```bash
./nlm --steps 1000 --timestep 0.001
```

#### Issue: No spikes detected
**Solution**: Check configuration and increase input current:

```bash
# In config file
cfg->set("plasticity_learning_rate", 0.1);
```

## Troubleshooting

### Symptom: No output or crashes
**Cause**: Configuration file issues
**Solution**: 
1. Check if configuration file exists
2. Validate syntax (no typos, correct types)
3. Ensure required fields are present

### Symptom: Memory allocation errors
**Cause**: System resource limits
**Solution**:
1. Reduce `neuron_count` 
2. Increase system memory
3. Use larger timesteps for faster simulation

### Symptom: No learning detected
**Cause**: Plasticity settings too low
**Solution**:
1. Increase `stdp_ltp_weight` and `stdp_ltd_weight`
2. Check if plasticity is enabled
3. Verify neuron inputs are sufficient

### Symptom: Excessive logging
**Cause**: Verbosity level too high
**Solution**:
1. Reduce verbosity level: `--verbose 1`
2. Or redirect to file: `./nlm --verbose 2 2> nlm.log`

## References

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Scientific Background](docs/SCIENCE.md)
- [Roadmap](docs/ROADMAP.md)

## License

MIT License. See LICENSE file for details.

---

*Last updated: $(date -I)*