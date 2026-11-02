# NLM Command Line Interface Documentation

## Overview

The NLM (Neural Learning Machine) unified command-line interface provides advanced access to all NLM phases through a single, comprehensive interface. This document covers the CLI's capabilities, commands, and usage patterns for both beginners and advanced users.

## Quick Start

### Basic Usage

```bash
# Interactive mode (default)
./nlm

# Execute single command
./nlm --command="brain status"

# Batch mode with multiple commands
./nlm --batch
# Commands:
>>> brain status
>>> config get neuron_count
>>> run 1000

# Help and version information
./nlm --help
./nlm --version
```

## Available Commands

### General Commands

| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show help message | `help` |
| `exit` | Exit the NLM interface | `exit` |
| `version` | Show version information | `version` |

### Brain Commands

| Command | Description | Example |
|---------|-------------|---------|
| `brain` | Brain system commands | `brain` |
| `brain reset` | Reset brain state | `brain reset` |
| `brain step [count]` | Advance brain by steps | `brain step 100` |
| `brain inject [type] [amount]` | Inject current into neuron type | `brain inject sensory 50.0` |
| `brain enable [type]` | Enable specific brain system | `brain enable memory` |
| `brain disable [type]` | Disable specific brain system | `brain disable prediction` |
| `brain status` | Show brain status | `brain status` |

### Configuration Commands

| Command | Description | Example |
|---------|-------------|---------|
| `config` | Configuration commands | `config` |
| `config get <key>` | Get configuration value | `config get neuron_count` |
| `config set <key> <value>` | Set configuration value | `config set neuron_count 1000` |
| `config list` | List all configuration keys | `config list` |
| `config save [file]` | Save config to file | `config save myconfig.cfg` |
| `config load [file]` | Load config from file | `config load default.cfg` |
| `config print` | Show current configuration | `config print` |

### Experiment Commands

| Command | Description | Example |
|---------|-------------|---------|
| `experiment` | Run experiments | `experiment` |
| `experiment list` | List available experiments | `experiment list` |
| `experiment run <name> [steps]` | Run specific experiment | `experiment run phase6_integration 2000` |

### File Commands

| Command | Description | Example |
|---------|-------------|---------|
| `save [file]` | Save brain state to file | `save checkpoint.bin` |
| `load [file]` | Load brain state from file | `load checkpoint.bin` |

### Execution Commands

| Command | Description | Example |
|---------|-------------|---------|
| `run [steps]` | Run simulation for steps | `run 5000` |
| `status` | Show brain system status | `status` |
| `stats` | Show performance statistics | `stats` |
| `plot [output]` | Generate visualization plots | `plot activity.png` |
| `benchmark` | Run performance benchmark | `benchmark` |

## Command Syntax Reference

### Brain Commands

```bash
# Basic brain status
brain status

# Reset brain to initial state
brain reset

# Run simulation for N steps
brain step 100

# Inject current into sensory neurons
brain inject sensory 25.0

# Inject current into motor neurons
brain inject motor 10.0

# Inject current into interneurons
brain inject interneuron 15.0

# Enable specific brain system
brain enable working_memory

# Disable specific brain system
brain disable prediction
```

### Configuration Commands

```bash
# Get current value
config get neuron_count

# Set integer value
config set neuron_count 500

# Set floating-point value
config set connection_probability 0.2

# Set boolean value
config set enable_checkpointing true

# List all configuration keys
config list

# Save current configuration
config save myconfig.cfg

# Load configuration
config load default.cfg

# Print current configuration
config print
```

### Experiment Commands

```bash
# List available experiments
experiment list

# Run specific experiment with custom steps
experiment run phase6_integration 3000

# Run experiment with default steps
experiment run phase2_basic
```

### File Operations

```bash
# Save brain checkpoint (uses timestamp if no filename provided)
save
save my_checkpoint.bin

# Load brain checkpoint
load my_checkpoint.bin
```

### Simulation Control

```bash
# Run simulation for specified steps
run 1000

# Show current brain status
status

# Show performance statistics
stats

# Generate neural activity plot
plot neural_activity.png

# Run performance benchmark
benchmark
```

## Advanced Usage Examples

### Example 1: Complete Workflow

```bash
# Initialize with custom configuration
./nlm --config=high_precision.cfg

# Enable advanced features
./nlm --command="config set enable_checkpointing true"
./nlm --command="config set enable_development true"
./nlm --command="config set enable_prediction true"

# Run integration experiment with checkpointing
./nlm --command="experiment run phase6_integration 5000"

# Save final state
./nlm --command="save final_checkpoint.bin"

# Generate statistics report
./nlm --command="stats"
```

### Example 2: Batch Processing

```bash
# Create batch processing script
#!/bin/bash
nlm --batch << EOF
config set neuron_count 200
config set region_count 2
brain reset
run 2000
stats
plot batch_results.png
save batch_checkpoint.bin
EOF

# Execute batch processing
./nlm --batch
```

### Example 3: Automated Testing

```bash
# Automated test script
#!/bin/bash

# Test 1: Baseline performance
./nlm --command="brain reset"
./nlm --command="run 100"
./nlm --command="stats"

# Test 2: Learning experiment
./nlm --command="config set enable_plasticity true"
./nlm --command="experiment run phase2_basic 500"
./nlm --command="stats"

# Test 3: Memory integration
./nlm --command="config set enable_memory true"
./nlm --command="experiment run phase4_cognitive 1000"
./nlm --command="stats"

# Test 4: Full integration
./nlm --command="experiment run phase6_integration 2000"
./nlm --command="stats"
```

## Configuration Management

### Available Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | integer | 42 | Random number generator seed |
| `simulation_timestep` | double | 0.001 | Simulation time step (seconds) |
| `neuron_count` | integer | 500 | Number of neurons in brain |
| `region_count` | integer | 1 | Number of neural regions |
| `connection_probability` | float | 0.15 | Probability of synaptic connections |
| `enable_checkpointing` | boolean | true | Enable checkpoint save/load |
| `enable_development` | boolean | true | Enable developmental stages |
| `enable_replay` | boolean | true | Enable episodic memory replay |
| `enable_prediction` | boolean | true | Enable predictive processing |
| `enable_neuromodulation` | boolean | true | Enable neuromodulatory systems |
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Structural plasticity rate |
| `pruning_rate` | float | 0.00001 | Structural pruning rate |
| `maturation_rate` | float | 0.001 | Developmental maturation rate |
| `initial_development_stage` | integer | 0 | Starting developmental stage |
| `dopamine_sensitivity` | float | 1.0 | Neuromodulation sensitivity |
| `curiosity_strength` | float | 0.5 | Exploration motivation strength |

### Configuration File Format

NLM configuration files are JSON format:

```json
{
    "random_seed": 123,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "enable_checkpointing": true,
    "enable_development": true,
    "enable_replay": true,
    "enable_prediction": true,
    "enable_neuromodulation": true,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "maturation_rate": 0.001,
    "initial_development_stage": 0,
    "dopamine_sensitivity": 1.0,
    "curiosity_strength": 0.5
}
```

## Brain System Architecture

### Neural Regions

NLM organizes neurons into functional regions:

1. **Sensory Region**: Processes external input
2. **Cognitive Region**: Handles memory, prediction, and planning
3. **Motor Region**: Generates action outputs
4. **Inter-regional Connections**: Long-range connectivity between regions

### Memory Systems

NLM implements multiple memory architectures:

1. **Working Memory**: Transient active information (capped at ~100 items)
2. **Episodic Memory**: Experience storage with replay capability
3. **Associative Memory**: Pattern associations and Hebbian learning

### Neuromodulation

Key neuromodulatory systems:

1. **Dopamine**: Reward prediction and reinforcement learning
2. **Curiosity**: Exploration motivation and intrinsic reward
3. **Novelty**: Surprise detection and novelty processing
4. **Prediction Error**: Mismatch between predicted and actual outcomes

### Cognitive Systems

Integrated cognitive architecture:

1. **Neural Planner**: Action sequence evaluation and planning
2. **Concept Formation**: Pattern discovery and abstraction
3. **Attentional Selection**: Competitive focus and prioritization
4. **Self-Model**: Internal representation of agent state

## Performance and Monitoring

### Statistics Commands

The `stats` command provides comprehensive performance metrics:

```bash
>>> stats

Neural Statistics:
  Total Spikes: 15432
  Active Neurons: 127
  Firing Neurons: 89
  Pending Spikes: 3
  Average Firing Rate: 18.7 Hz
  Excitation/Inhibition Ratio: 1.23

Command History:
  >>> brain reset
  >>> run 100
  >>> stats
```

### Benchmarking

The `benchmark` command runs comprehensive performance tests:

```bash
>>> benchmark

Running NLM Benchmark...
Phase 2: Basic Neural Computation - 15.2 ms
Phase 3: World Interaction - 28.7 ms
Phase 4: Cognitive Architecture - 45.3 ms
Phase 6: Integration Test - 89.1 ms

Average Processing Time: 44.8 ms
```

## Scripting and Automation

### Command History

All commands are automatically logged in the command history, accessible via the `stats` command.

### Batch Mode

Use `--batch` flag for scriptable execution:

```bash
# Execute commands from command line
./nlm --batch --command="brain reset" --command="run 1000" --command="stats"
```

### Integration with External Tools

NLM supports integration with various external tools:

1. **Python Bindings**: Access NLM functionality from Python
2. **Visualization Tools**: Export neural activity data for external visualization
3. **Logging Systems**: Integration with external logging and monitoring
4. **Testing Frameworks**: Automated testing and validation

## Troubleshooting

### Common Issues

#### "Unknown command" Error

If you receive an "Unknown command" error, ensure:
1. The command is spelled correctly
2. The command syntax is correct (including required arguments)
3. Type `help` to see all available commands

#### "Configuration not found" Error

If configuration loading fails:
1. Verify the file path is correct
2. Check that the file is valid JSON format
3. Use `config save` to create a valid configuration file

#### Performance Issues

For slow simulation performance:
1. Reduce `neuron_count` and `region_count`
2. Increase `simulation_timestep`
3. Disable optional features: `--command="config set enable_replay false"`

### Help Resources

- **Documentation**: See `docs/` directory for comprehensive guides
- **Examples**: Review `examples/` directory for usage examples
- **API Reference**: Generated documentation in `docs/doxygen/`
- **Community**: GitHub repository and issue tracker

## Advanced Features

### Command Completion

Tab completion is supported in interactive mode for:
- Command names
- Configuration keys
- Neuron types
- Experiment names

### Persistent Sessions

Sessions can be saved and restored using checkpoint files:

```bash
# Save current session
save my_session.bin

# Restore session
load my_session.bin
```

### Multi-Platform Support

NLM provides consistent interface across:
- Linux (native)
- macOS (native)
- Windows (via WSL or native build)

## Migration from Previous Versions

### Breaking Changes

Version 0.1.0 introduces the unified CLI interface, replacing the previous separate demo binaries:

- `./nlm` now includes all previous demo functionality
- `./nlm_phase3_demo`, `./nlm_phase4_demo`, etc. are preserved for backward compatibility
- Configuration file format updated to include new unified settings

### Migration Guide

For users migrating from previous versions:

1. Use `./nlm --command="help"` to see available commands
2. Explore `./nlm_phase3_demo` for Phase 3 specific functionality
3. Update configuration files to include new unified settings
4. Review new command syntax for advanced features

## Future Enhancements

### Planned Improvements

1. **Real-time Visualization**: Interactive neural activity monitoring
2. **Web Interface**: Browser-based control and visualization
3. **Plugin Architecture**: Extensible command system
4. **Distributed Computing**: Multi-machine simulation support
5. **Advanced Analytics**: Performance profiling and optimization
6. **Machine Learning Integration**: Enhanced learning capabilities
7. **Robotics Integration**: Real-world embodiment support

## License and Attribution

This unified command-line interface is part of the NLM (Neural Learning Machine) project, licensed under the MIT License. All improvements and additions are intended to make the system more accessible to researchers, developers, and advanced users while maintaining backward compatibility.

## Acknowledgments

Special thanks to all contributors who have enhanced the NLM system with:
- Advanced neural computation algorithms
- Comprehensive testing frameworks
- Performance optimizations
- Documentation improvements

The unified CLI interface represents a significant step forward in making advanced neural simulation accessible to both experts and newcomers to the field.
