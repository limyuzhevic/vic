# NLM Configuration Examples

This file provides examples of how to configure and use the Neural Learning Machine (NLM) with the enhanced features.

## Basic Configuration

### Command Line Usage

```bash
# Basic run with default settings
./nlm

# Run with specific parameters
./nlm --neurons 1000 --steps 5000 --seed 123

# Run with different experiment modes
./nlm --experiment-mode plasticity --verbose
./nlm --experiment-mode stdp
./nlm --experiment-mode all
```

### Configuration File

Create a file called `myconfig.cfg`:

```ini
# NLM Configuration File
# ====================

# Basic simulation parameters
neuron_count = 2000
region_count = 2
simulation_timestep = 0.001
random_seed = 12345

# Network connectivity
connection_probability = 0.1

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory systems
working_memory_capacity = 200
max_episodes = 1000

# Neuromodulation
enable_curiosity = true
enable_reward = true
enable_novelty = true

# Development
development_enabled = true
critical_period_length = 10000

# Performance
enable_multithreading = false
enable_simd = true
```

### Advanced Configuration with CLI

```bash
# Advanced configuration combining CLI and file
./nlm \
  --config-file myconfig.cfg \
  --neurons 5000 \
  --steps 20000 \
  --experiment-mode all \
  --verbose \
  --connection 0.2
```

## Phase 6 Integration Demo

Run the Phase 6 integration demo to test all brain systems:

```bash
# Basic Phase 6 demo
./nlm_phase6_demo

# Phase 6 with custom parameters
./nlm_phase6_demo \
  --neurons 3000 \
  --steps 15000 \
  --regions 3 \
  --connection 0.15 \
  --checkpoint ./my_checkpoint.bin

# Phase 6 with all features disabled
./nlm_phase6_demo \
  --no-checkpointing \
  --no-replay \
  --no-development
```

## Phase 3 World Interaction Demo

Run the Phase 3 world interaction demo to see the brain interacting with an environment:

```bash
# Phase 3 with default settings
./nlm_phase3_demo

# Phase 3 with custom parameters
./nlm_phase3_demo 10 500

# Parameters:
#   10 = number of episodes
#   500 = steps per episode
```

## Phase 4 Cognition Demo

Run the Phase 4 cognition demo to explore cognitive capabilities:

```bash
# Phase 4 with default settings
./nlm_phase4_demo

# Phase 4 with custom trial count
./nlm_phase4_demo 100

# Parameters:
#   100 = number of trials
```

## Configuration File Format

### Basic Types

- **integers**: `value = 123`
- **floating point numbers**: `value = 3.14`
- **strings**: `value = "my_string"`
- **booleans**: `value = true` or `value = false`

### Sectioned Configuration

You can use sections to organize related settings:

```ini
[Simulation]
neuron_count = 1000
timestep = 0.001
max_steps = 10000

[Network]
connection_probability = 0.1
initial_weight_mean = 0.2
initial_weight_std = 0.1

[Plasticity]
stdp_enabled = true
hebbian_enabled = true
structural_enabled = true

[Memory]
working_memory_size = 500
episodic_capacity = 1000

[Development]
enabled = true
stage_duration = 50000
```

## Environment Variables

You can also use environment variables:

```bash
export NLM_NEURON_COUNT=2000
export NLM_SEED=12345
export NLM_EXPERIMENT_MODE=plasticity
./nlm
```

## Output Files

The enhanced NLM will generate various output files:

### Configuration and Logs

- `nlm.log` - Main log file
- `experiment_YYYYMMDD_HHMMSS.log` - Experiment-specific logs

### Checkpoints

- `./checkpoints/` - Directory for saved brain states
- `checkpoint_YYYYMMDD_HHMMSS.bin` - Individual checkpoints
- `checkpoint_latest.bin` - Symbolic link to latest checkpoint

### Results and Analysis

- `./results/` - Directory for experiment results
- `experiment_summary_YYYYMMDD_HHMMSS.json` - JSON results
- `experiment_summary_YYYYMMDD_HHMMSS.txt` - Text summary
- `metrics_YYYYMMDD_HHMMSS.csv` - CSV metrics
- `./visualizations/` - Generated visualizations

## Performance Tuning

### Command Line Performance Options

```bash
# Enable SIMD and multithreading
./nlm --enable-simd --enable-multithreading --threads 4

# Use sparse connectivity for large networks
./nlm --sparse-connectivity --connection 0.05

# Event-driven simulation
./nlm --event-driven

# Memory pool optimization
./nlm --memory-pool-size 1000
```

### Configuration for Large Networks

```ini
[Performance]
enable_simd = true
enable_multithreading = true
thread_count = 4
enable_sparse_connectivity = true
event_driven = true
memory_pool_size = 2000

[Network]
neuron_count = 100000
connection_probability = 0.01  # Lower density for large networks

[Memory]
working_memory_capacity = 10000
episodic_capacity = 50000

[Development]
enabled = false  # Disable development for large-scale testing
```

## Debugging and Troubleshooting

### Verbose Logging

Enable verbose logging to see detailed information:

```bash
./nlm --verbose
./nlm_phase6_demo --verbose
```

### Quiet Mode

Run with minimal output to see only errors:

```bash
./nlm --quiet
```

### Checkpoint Debugging

Monitor checkpoint operations:

```bash
# Enable checkpoint debugging
./nlm --checkpoint-debug

# Check checkpoint status
ls -la ./checkpoints/
```

## Advanced Features

### Custom Config Command

You can specify multiple configuration files:

```bash
./nlm --config-file base.cfg --config-file overrides.cfg
```

### Scripted Experiments

Create a script to run multiple experiments:

```bash
#!/bin/bash
# run_experiments.sh

for seed in 42 123 456 789 999; do
    echo "Running experiment with seed $seed"
    ./nlm \
        --seed $seed \
        --experiment-mode plasticity \
        --steps 1000 \
        --output-dir ./results/seed_${seed}
    echo "Completed seed $seed"
done
```

### Automated Configuration

Generate configuration files programmatically:

```python
# generate_config.py
import json

config = {
    "neuron_count": 5000,
    "region_count": 2,
    "simulation_timestep": 0.001,
    "random_seed": 12345,
    "connection_probability": 0.1,
    "experiment_mode": "plasticity",
    "enable_verbose_logging": True,
    "checkpoint_enabled": True
}

with open('auto_config.cfg', 'w') as f:
    json.dump(config, f, indent=2)
```

## Summary

The enhanced NLM provides:

1. **Flexible configuration** through command line, config files, and environment variables
2. **Multiple experiment modes** for different testing scenarios
3. **Comprehensive logging** and progress tracking
4. **Checkpointing** for saving and resuming experiments
5. **Performance optimizations** for large-scale simulations
6. **Detailed output** for analysis and visualization
7. **Error handling** and validation for robust operation
8. **Backward compatibility** with existing configurations

These features make NLM easier to use, configure, and extend for both basic experiments and advanced research scenarios.