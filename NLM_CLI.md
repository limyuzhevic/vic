# NLM Command Line Interface
# Advanced user commands and options

## Overview

NLM provides a comprehensive command-line interface for running experiments, configuring the brain, and managing checkpoints.

## Available Commands

### `nlm` (Main executable)

**Purpose:** Phase 2 demonstration with learning experiments

**Usage:**
```bash
nlm [options]
```

**Options:**
- `--config <file>` - Load configuration from file
- `--neuron-count <n>` - Number of neurons (default: 500)
- `--max-steps <n>` - Maximum simulation steps (default: 1000)
- `--region-count <n>` - Number of brain regions (default: 1)
- `--connection-prob <p>` - Connection probability (default: 0.1)
- `--checkpoint <file>` - Enable checkpointing and save to file
- `--checkpoint-dir <dir>` - Checkpoint directory (default: ./checkpoints)
- `--log-level <level>` - Logging level (DEBUG, INFO, WARN, ERROR)
- `--help` - Show help message

### `nlm_phase3_demo` (Phase 3 executable)

**Purpose:** Agent-world interaction experiments

**Usage:**
```bash
nlm_phase3_demo [options]
```

**Options:**
- `--checkpoint <file>` - Load checkpoint
- `--steps <n>` - Number of simulation steps (default: 1000)
- `--log-level <level>` - Logging level

### `nlm_phase4_demo` (Phase 4 executable)

**Purpose:** Cognitive experiments

**Usage:**
```bash
nlm_phase4_demo [options]
```

**Options:**
- `--steps <n>` - Number of simulation steps (default: 1000)
- `--enable-memory` - Enable memory systems
- `--enable-prediction` - Enable prediction system
- `--enable-cognition` - Enable cognition systems
- `--log-level <level>` - Logging level

### `nlm_phase6_demo` (Phase 6 executable)

**Purpose:** Integration experiments (NEW)

**Usage:**
```bash
nlm_phase6_demo [options]
```

**Options:**
- `--steps <n>` - Number of simulation steps (default: 1000)
- `--neurons <n>` - Number of neurons (default: 500)
- `--enable-memory` - Enable memory systems
- `--enable-prediction` - Enable prediction system
- `--enable-cognition` - Enable cognition systems
- `--enable-neuromodulation` - Enable neuromodulation systems
- `--enable-development` - Enable development systems
- `--enable-replay` - Enable replay system
- `--checkpoint <file>` - Enable checkpointing
- `--log-level <level>` - Logging level
- `--output <file>` - Save results to file

## Configuration File Format

Configuration files use INI format:

```ini
# NLM Configuration File
# Phase 6 Integrated Artificial Brain

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 1

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1
dopamine_decay = 0.9
curiosity_threshold = 0.1
novelty_decay = 0.98

# Reward settings
reward_discount_factor = 0.99
reward_learning_rate = 0.01

# Memory settings
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
replay_interval = 100
consolidation_interval = 1000

# Prediction settings
prediction_history_size = 50
prediction_confidence_threshold = 0.8

# Cognition settings
planning_depth = 5
concept_formation_decay = 0.9
attention_inhibition_strength = 0.5
attention_excitation_strength = 1.5

# Environment settings
environment_name = SimpleWorld
environment_width = 16
environment_height = 16

# Checkpointing
checkpoint_dir = ./checkpoints
checkpoint_interval = 10000
max_checkpoints = 10
enable_checkpointing = true

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm.log

# Advanced options
enable_memory_systems = true
enable_prediction_system = true
enable_cognition_systems = true
enable_neuromodulation = true
enable_development_integration = true
enable_replay_learning = true
```

## Advanced Configuration Commands

NLM supports programmatic configuration through command line arguments:

### Memory System Configuration
```bash
# Enable and configure working memory
--working-memory-capacity 200
--episodic-memory-max-episodes 5000
--replay-interval 50
--consolidation-interval 500

# Enable memory integration in brain loop
--enable-memory-systems
```

### Prediction System Configuration
```bash
# Configure prediction parameters
--prediction-history-size 100
--prediction-confidence-threshold 0.9
--enable-prediction-system
```

### Cognition System Configuration
```bash
# Enable and configure cognition systems
--planning-depth 10
--enable-cognition-systems
--enable-neural-planner
--enable-concept-formation
--enable-attentional-selection
--enable-self-model
--enable-social-learning
```

### Neuromodulation Configuration
```bash
# Configure neuromodulation parameters
--dopamine-baseline 0.2
--dopamine-decay 0.8
--curiosity-threshold 0.05
--enable-neuromodulation
--enable-curiosity
--enable-novelty-detection
--enable-prediction-error
```

### Development System Configuration
```bash
# Configure developmental stages
--enable-development-integration
--development-stage-initial 60.0
--development-stage-critical 300.0
--development-stage-maturation 600.0
--development-plasticity-modulation
```

## Checkpoint Management Commands

NLM provides advanced checkpoint management:

### Basic Checkpoint Commands
```bash
# Save current brain state
nlm --checkpoint brain_state_1000.bin

# Load previous brain state
nlm_phase3_demo --checkpoint brain_state_1000.bin

# List available checkpoints
nlm_phase4_demo --list-checkpoints

# Delete checkpoints
nlm_phase6_demo --prune-checkpoints 5
```

### Advanced Checkpoint Management
```bash
# Automatic checkpointing
nlm --enable-checkpointing --checkpoint-dir ./auto_checkpoints --checkpoint-interval 1000

# Multi-session checkpointing
nlm_phase3_demo --checkpoint-session session1
nlm_phase4_demo --checkpoint-session session2
nlm_phase6_demo --checkpoint-session integrated

# Checkpoint comparison
nlm_phase6_demo --compare-checkpoint checkpoint1.bin checkpoint2.bin
```

## Experiment Configuration Commands

Configure experiments through command line:

### Phase-Specific Experiments
```bash
# Phase 2: Real Neural Computation
nlm --neuron-count 2000 --max-steps 5000 --enable-plasticity

# Phase 3: Agent-World Interaction
nlm_phase3_demo --steps 2000 --enable-reward-modulation --enable-curiosity

# Phase 4: Cognitive Experiments
nlm_phase4_demo --steps 3000 --enable-memory --enable-prediction --enable-cognition

# Phase 6: Integrated Artificial Brain
nlm_phase6_demo --steps 1000 --neurons 500 --enable-all-systems
```

### Custom Experiments
```bash
# Create custom experiment configuration
nlm_phase6_demo \
  --steps 2000 \
  --neurons 1000 \
  --working-memory-capacity 200 \
  --episodic-memory-max-episodes 2000 \
  --enable-prediction-system \
  --enable-cognition-systems \
  --enable-neuromodulation \
  --enable-development-integration \
  --enable-replay-learning \
  --checkpoint brain_2k_steps.bin
```

## Environment and Simulation Commands

Configure the simulation environment:

### Environment Configuration
```bash
# Create custom environments
nlm_phase3_demo --world-width 32 --world-height 32 --vision-width 16 --vision-height 16
nlm_phase4_demo --environment-type ComplexWorld
nlm_phase6_demo --environment-type IntegratedSimulation
```

### Simulation Control
```bash
# Simulation timing
nlm --simulation-timestep 0.0005 --max-simulation-steps 20000
nlm_phase3_demo --simulation-time-limit 300.0
nlm_phase4_demo --update-rate 60.0
nlm_phase6_demo --real-time-mode
```

## Logging and Output Commands

Configure logging and output:

### Logging Configuration
```bash
# Log levels
nlm --log-level DEBUG
nlm_phase3_demo --log-level INFO
nlm_phase4_demo --log-level WARN
nlm_phase6_demo --log-level ERROR

# Log output
nlm --log-to-file --log-filename nlm_detailed.log
nlm_phase3_demo --log-to-console --log-prefix [AGENT] 
nlm_phase4_demo --log-to-file --log-to-console
nlm_phase6_demo --log-format json
```

### Output Configuration
```bash
# Save experiment results
nlm_phase3_demo --output results.json
nlm_phase4_demo --output results.csv --append
nlm_phase6_demo --output integration_results.json --format pretty

# Performance monitoring
nlm --profile-performance --performance-output profile.json
nlm_phase3_demo --measure-metrics --metrics-output metrics.txt
nlm_phase4_demo --benchmark --benchmark-output benchmark.csv
nlm_phase6_demo --monitor-system --monitor-output monitor.log
```

## Configuration File Examples

### Basic Configuration
```bash
# configs/basic.cfg
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.1
log_level = INFO
```

### Advanced Configuration
```bash
# configs/advanced.cfg
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 2
connection_probability = 0.15

# Memory
working_memory_capacity = 500
episodic_memory_max_episodes = 5000
replay_interval = 100
consolidation_interval = 1000

# Prediction
prediction_history_size = 100
prediction_confidence_threshold = 0.9

# Cognition
planning_depth = 10
attention_inhibition_strength = 0.5

# Neuromodulation
dopamine_baseline = 0.2
curiosity_threshold = 0.05

# Development
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Environment
environment_name = SimpleWorld
environment_width = 32
environment_height = 32

# Checkpointing
checkpoint_dir = ./checkpoints
checkpoint_interval = 5000
max_checkpoints = 20
enable_checkpointing = true

# Advanced systems
enable_memory_systems = true
enable_prediction_system = true
enable_cognition_systems = true
enable_neuromodulation = true
enable_development_integration = true
enable_replay_learning = true
```

### Performance Configuration
```bash
# configs/performance.cfg
random_seed = 999
simulation_timestep = 0.0005
neuron_count = 5000
region_count = 4
connection_probability = 0.05

# Optimized for performance
working_memory_capacity = 1000
episodic_memory_max_episodes = 10000
replay_interval = 200

# High-performance prediction
prediction_history_size = 200
prediction_confidence_threshold = 0.95

# Advanced cognition
planning_depth = 20
attention_inhibition_strength = 0.3

# Simulation settings
log_level = WARN
log_to_file = true
log_filename = nlm_performance.log
enable_checkpointing = false  # Disable for performance
max_simulation_steps = 50000
simulation_time_limit = 600.0
```

## Command Line Help

For detailed help on any command, use:
```bash
nlm --help
nlm_phase3_demo --help
nlm_phase4_demo --help
nlm_phase6_demo --help
```

## Environment Variables

NLM also supports environment variables:

```bash
export NLM_LOG_LEVEL=DEBUG
export NLM_CHECKPOINT_DIR=/data/checkpoints
export NLM_DEFAULT_CONFIG=/path/to/config.cfg
export NLM_PERFORMANCE_MODE=true
```

## Troubleshooting

### Common Issues and Solutions

1. **Memory usage too high**
```bash
# Reduce memory usage
nlm --neuron-count 1000 --working-memory-capacity 100
```

2. **Experiment runs too slow**
```bash
# Increase timestep for faster simulation
nlm --simulation-timestep 0.005
```

3. **Checkpointing fails**
```bash
# Check disk space and permissions
ls -la ./checkpoints
chmod 755 ./checkpoints
```

4. **Logging issues**
```bash
# Force console logging
nlm --log-to-console --log-level INFO
```

5. **Configuration file not found**
```bash
# Use default configuration
nlm --config ./configs/default.cfg
```

## Scripting and Automation

NLM provides scripts for common tasks:

### Quick Run Script
```bash
#!/bin/bash
# run_nlm.sh - Quick NLM experiment runner

NLM_HOME="/path/to/nlm"

# Run Phase 2 demo
"$NLM_HOME"/bin/nlm --steps 1000 --neuron-count 500

# Run Phase 3 demo
"$NLM_HOME"/bin/nlm_phase3_demo --steps 2000 --checkpoint brain_state.bin

# Run Phase 4 demo
"$NLM_HOME"/bin/nlm_phase4_demo --steps 3000 --enable-memory --enable-prediction

# Run Phase 6 demo
"$NLM_HOME"/bin/nlm_phase6_demo --steps 1000 --neurons 1000 --enable-all-systems
```

### Experiment Comparison Script
```bash
#!/bin/bash
# compare_experiments.sh - Compare experiment results

python3 << EOF
import json
import sys
import numpy as np

def compare_experiments(file1, file2):
    with open(file1, 'r') as f:
        exp1 = json.load(f)
    with open(file2, 'r') as f:
        exp2 = json.load(f)
    
    # Compare key metrics
    metrics = ['total_reward', 'avg_firing_rate', 'memory_episodes_stored', 'dopamine_level']
    
    print("Experiment Comparison:")
    for metric in metrics:
        if metric in exp1 and metric in exp2:
            diff = abs(exp1[metric] - exp2[metric])
            rel_diff = diff / max(abs(exp1[metric]), abs(exp2[metric])) * 100
            print(f"  {metric}: {exp1[metric]:.3f} vs {exp2[metric]:.3f} (diff: {rel_diff:.1f}%)")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: compare_experiments.sh file1.json file2.json")
        sys.exit(1)
    compare_experiments(sys.argv[1], sys.argv[2])
EOF
```

This advanced command-line interface provides comprehensive control over NLM experiments, making it suitable for both beginners and advanced users.
