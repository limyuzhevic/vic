# Advanced Command-Line Options and Configuration Presets

This document describes the advanced command-line options and configuration presets added to NLM Phase 6.

## Advanced Command-Line Options

### Core Options
- `--config=file` - Load configuration from file (JSON or key=value format)
- `--preset=name` - Apply preset configuration (debug, production, research)
- `--help` - Show help message
- `--version` - Show version information

### New Advanced Options

#### Network Configuration Options
- `--neuron-count=N` - Set number of neurons (default: 1000)
- `--region-count=N` - Set number of regions (default: 1)
- `--connection-probability=P` - Set connection probability (0.0-1.0, default: 0.1)
- `--synapse-delay=RANGE` - Set synapse delay range (e.g., "1-5")

#### Plasticity Options
- `--plasticity-mode=mode` - Set plasticity mode (stdp, hebbian, both, none)
- `--stdp-learning-rate=rate` - Set STDP learning rate
- `--hebbian-learning-rate=rate` - Set Hebbian learning rate
- `--synaptogenesis-rate=rate` - Set synaptogenesis rate
- `--pruning-rate=rate` - Set pruning rate

#### Neuromodulation Options
- `--dopamine-level=level` - Set initial dopamine level
- `--curiosity-level=level` - Set initial curiosity level
- `--novelty-threshold=threshold` - Set novelty detection threshold
- `--reward-scale=scale` - Scale reward prediction error

#### Memory Options
- `--working-memory-capacity=size` - Set working memory capacity
- `--episodic-memory-max-episodes=count` - Set maximum episodic memory episodes
- `--memory-consolidation-interval=interval` - Set memory consolidation interval

#### Development Options
- `--development-enabled=true/false` - Enable/disable development system
- `--developmental-stage=stage` - Set developmental stage (Initial, CriticalPeriod, Maturation, Adult)
- `--plasticity-modulation=level` - Set developmental plasticity modulation (0.0-1.0)

#### Simulation Options
- `--simulation-timestep=delta` - Set simulation timestep in seconds
- `--max-steps=N` - Set maximum simulation steps
- `--checkpoint-interval=N` - Set checkpoint save interval
- `--replay-interval=N` - Set memory replay interval
- `--dry-run` - Initialize brain but don't run experiments

#### Performance Options
- `--profile` - Enable performance profiling
- `--log-level=LEVEL` - Set log level (Debug, Info, Warning, Error, Critical)
- `--verbose` - Enable verbose output
- `--quiet` - Suppress informational messages

#### Advanced Debugging Options
- `--debug-mode=level` - Set debug mode (basic, detailed, comprehensive)
- `--enable-spike-tracing` - Enable detailed spike tracing
- `--enable-plasticity-tracking` - Enable plasticity tracking
- `--enable-memory-profiling` - Enable memory profiling
- `--enable-performance-monitoring` - Enable performance monitoring

### Preset Configurations

#### debug
```
--preset=debug
# Configuration:
# neuron_count: 100
# region_count: 1
# connection_probability: 0.2
# stdp_ltp_weight: 0.05
# stdp_ltd_weight: 0.03
# synaptogenesis_rate: 0.001
# pruning_rate: 0.0001
# replay_interval: 50
# consolidation_interval: 500
```

#### production
```
--preset=production
# Configuration:
# neuron_count: 1000
# region_count: 1
# connection_probability: 0.1
# stdp_ltp_weight: 0.01
# stdp_ltd_weight: 0.012
# synaptogenesis_rate: 0.0001
# pruning_rate: 0.00001
# replay_interval: 100
# consolidation_interval: 1000
```

#### research
```
--preset=research
# Configuration:
# neuron_count: 5000
# region_count: 4
# connection_probability: 0.15
# stdp_ltp_weight: 0.02
# stdp_ltd_weight: 0.018
# synaptogenesis_rate: 0.0005
# pruning_rate: 0.00005
# replay_interval: 200
# consolidation_interval: 5000
```

### Compound Options
```
--preset=debug --neuron-count=200 --region-count=2 --plasticity-mode=both \
  --dopamine-level=0.5 --curiosity-level=0.3 --enable-spike-tracing \
  --log-level=Debug --verbose --checkpoint-interval=100
```

### Configuration File Examples

#### JSON Configuration
```json
{
    "config": {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.15,
        "random_seed": 12345,
        "simulation_timestep": 0.001,
        "plasticity": {
            "stdp": {
                "learning_rate": 0.02,
                "learning_window": 20.0
            },
            "hebbian": {
                "learning_rate": 0.01,
                "correlation_window": 50
            }
        },
        "neuromodulation": {
            "dopamine": {
                "initial_level": 0.5,
                "decay_rate": 0.01
            },
            "curiosity": {
                "exploration_bonus": 0.3,
                "novelty_sensitivity": 0.8
            }
        },
        "memory": {
            "working_memory": {
                "capacity": 500,
                "decay_rate": 0.95
            },
            "episodic_memory": {
                "max_episodes": 2000,
                "consolidation_interval": 1000
            }
        },
        "development": {
            "enabled": true,
            "plasticity_modulation": {
                "Initial": 1.0,
                "CriticalPeriod": 0.8,
                "Maturation": 0.5,
                "Adult": 0.2
            }
        }
    }
}
```

#### Key-Value Configuration
```
# NLM Configuration File
# Generated on 2025-09-30 16:52:01

# Random seed
random_seed = 42

# Network topology
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Simulation parameters
simulation_timestep = 0.001
max_steps = 10000

# Plasticity rules
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation
dopamine_initial_level = 0.1
curiosity_initial_level = 0.2
novelty_threshold = 0.5

# Memory systems
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
replay_interval = 100
consolidation_interval = 1000

# Development
development_enabled = true
developmental_stage = Adult
plasticity_modulation = 0.2

# Debugging and profiling
debug_mode = basic
enable_spike_tracing = false
enable_plasticity_tracking = false
log_level = Info

# Performance
checkpoint_interval = 1000
profile = false
verbose = true
```

### Command-Line Option Combinations

#### For Development and Testing
```bash
# Debug mode with detailed logging
./nlm --preset=debug --log-level=Debug --verbose --enable-spike-tracing

# Performance testing with profiling
./nlm --preset=production --profile --checkpoint-interval=500 --max-steps=5000

# Custom configuration for specific research question
./nlm --neuron-count=2000 --region-count=4 --plasticity-mode=both \
  --dopamine-level=0.8 --curiosity-level=0.6 --enable-memory-profiling \
  --log-level=Info --save-config=research_config.json
```

#### For Production Use
```bash
# Minimal configuration for production
./nlm --preset=production --quiet --checkpoint-interval=10000

# Production with monitoring
./nlm --preset=production --enable-performance-monitoring \
  --log-level=Info --checkpoint-interval=5000 --save-config=prod_config.json
```

#### For Research and Experimentation
```bash
# Large-scale research experiment
./nlm --preset=research --development-enabled=true --plasticity-mode=both \
  --replay-interval=500 --consolidation-interval=10000 --max-steps=50000 \
  --enable-spike-tracing --enable-plasticity-tracking --verbose

# Comparative experiment
./nlm --preset=debug --neuron-count=100 --region-count=2 --plasticity-mode=stdp
./nlm --preset=production --neuron-count=1000 --region-count=1 --plasticity-mode=stdp
./nlm --preset=research --neuron-count=5000 --region-count=4 --plasticity-mode=stdp
```

## Configuration Preset Documentation

### debug Preset
**Purpose**: Quick development, testing, and debugging
- Smaller network (100 neurons) for faster iteration
- Higher plasticity rates for observing changes
- Detailed logging enabled
- Spike tracing available

### production Preset
**Purpose**: Standard research and production use
- Balanced configuration for most research scenarios
- Optimized for performance and stability
- Default settings for scientific experiments
- Minimal debugging output

### research Preset
**Purpose**: Large-scale research and experimentation
- Large network (5000 neurons) for complex behaviors
- Enhanced development and plasticity
- Advanced monitoring and analysis
- Designed for long-running experiments

## Configuration File Format Reference

### JSON Configuration
The JSON configuration file format uses a hierarchical structure:

```json
{
    "config": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "plasticity": {
            "stdp": { "learning_rate": 0.02 },
            "hebbian": { "learning_rate": 0.01 }
        },
        "neuromodulation": {
            "dopamine": { "initial_level": 0.5 },
            "curiosity": { "exploration_bonus": 0.3 }
        },
        "memory": {
            "working_memory": { "capacity": 500 },
            "episodic_memory": { "max_episodes": 2000 }
        }
    }
}
```

### Key-Value Configuration
The simple key=value format is used for backward compatibility:

```
# Comments start with #
key = value
another_key = "quoted value"

# Empty lines are ignored
# Lines with # or / are comments
```

### Configuration Sources Priority
1. Command-line arguments (highest priority)
2. Configuration presets
3. Configuration files
4. Default values (lowest priority)

### Configuration Validation
The configuration system validates:
- Parameter ranges (e.g., neuron_count > 0)
- Parameter consistency (e.g., region_count <= neuron_count)
- Data type compatibility
- Biological plausibility (e.g., probability values between 0.0 and 1.0)

### Configuration Save/Load
Configurations can be saved to files and reloaded:
- JSON format for structured data
- Simple key=value format for compatibility
- Automatic backup and versioning support
- Validation on load

## Summary

The advanced command-line options and configuration presets provide:

1. **Flexibility**: Wide range of options for different use cases
2. **Convenience**: Preset configurations for common scenarios
3. **Powerful**: Advanced options for research and experimentation
4. **Extensible**: Easy to add new options and presets
5. **Robust**: Comprehensive validation and error handling
6. **Documented**: Clear documentation for all options and presets

These improvements make NLM more accessible to beginners while providing powerful tools for advanced researchers.
