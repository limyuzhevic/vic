# NLM CLI - General Help & Documentation

## Overview

This document provides comprehensive help for the NLM (熙然) - Neural Learning Machine Command Line Interface (CLI). The NLM CLI unifies all experimental phases (2-6) and provides advanced features for both beginners and expert users.

## Command Structure

The NLM CLI follows a hierarchical command structure:

```
nlm [global_options] <command> [command_options]
```

### Global Options

- `--help` - Show help message
- `--version` - Show version information
- `--docs` - Show documentation
- `--interactive` - Enter interactive mode
- `--config <file>` - Load configuration from file
- `--verbose` - Enable verbose output
- `--quiet` - Suppress output

### Command Categories

1. **Phase Execution**: `run`, `phase2`, `phase3`, `phase4`, `phase5`, `phase6`
2. **Configuration Management**: `config`
3. **Checkpoint Management**: `checkpoint`
4. **Performance Monitoring**: `profile`
5. **Result Export**: `export`
6. **Plugin Management**: `plugin`

## Phase Execution Commands

### `run <phase> [options]`

Execute a specific NLM experimental phase with customizable parameters.

#### Usage Examples

```bash
# Run Phase 2 with default settings
nlm run phase2

# Run Phase 6 with custom parameters
nlm run phase6 --neuron_count 2000 --max_steps 5000 --working_memory_capacity 0.2

# Run Phase 4 with performance monitoring
nlm profile start
nlm run phase4 --simulated_trials 100 --performance_metric fps
nlm profile stop
```

#### Phase-Specific Options

| Phase | Description | Key Parameters |
|-------|-------------|----------------|
| phase2 | Real Neural Computation | `neuron_count`, `region_count`, `connection_probability` |
| phase3 | Cognition | `planning_depth`, `concept_threshold`, `attention_strength` |
| phase4 | Performance | `simd_vectorization`, `memory_pool_efficiency` |
| phase5 | Personality | `personality_learning_rate`, `preference_decay` |
| phase6 | Final Integration | `working_memory_capacity`, `episodic_memory_max_episodes` |

## Configuration Management

### `config <action> [key] [value]`

Manage NLM configuration settings.

#### Actions

1. **`config get <key>`**
   - Get configuration value
   - Example: `nlm config get neuron_count`

2. **`config set <key> <value>`**
   - Set configuration value
   - Example: `nlm config set neuron_count 1000`

3. **`config load <filepath>`**
   - Load configuration from file
   - Example: `nlm config load my_config.cfg`

4. **`config save <filepath>`**
   - Save current configuration to file
   - Example: `nlm config save my_config.cfg`

5. **`config validate`**
   - Validate current configuration
   - Example: `nlm config validate`

6. **`config suggestions <partial_key>`**
   - Get suggestions for configuration keys
   - Example: `nlm config suggestions neuron`

#### Configuration Parameters

| Parameter | Type | Default | Description | Valid Range |
|-----------|------|---------|-------------|--------------|
| `neuron_count` | int64 | 1000 | Number of neurons | 100-100000 |
| `region_count` | int64 | 1 | Number of brain regions | 1-100 |
| `connection_probability` | float | 0.1 | Synaptic connection probability | 0.0-1.0 |
| `random_seed` | int64 | 42 | Random number generator seed | Any |
| `simulation_timestep` | double | 0.001 | Simulation time step (s) | 0.0001-0.1 |
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight | 0.0-1.0 |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight | 0.0-1.0 |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) | 1.0-100.0 |
| `synaptogenesis_rate` | float | 0.0001 | Synapse formation rate | 0.0-0.01 |
| `pruning_rate` | float | 0.00001 | Synapse elimination rate | 0.0-0.001 |
| `planning_depth` | float | 5.0 | Planning depth | 1.0-20.0 |
| `concept_threshold` | float | 0.7 | Concept formation threshold | 0.0-1.0 |
| `attention_strength` | float | 0.5 | Attention strength | 0.0-2.0 |
| `simd_vectorization` | float | 1.0 | SIMD efficiency (0.0-1.0) | 0.0-1.0 |
| `memory_pool_efficiency` | float | 0.9 | Memory pool efficiency | 0.0-1.0 |
| `checkpoint_compression` | float | 0.8 | Checkpoint compression level | 0.0-1.0 |
| `personality_learning_rate` | float | 0.01 | Personality learning rate | 0.0-1.0 |
| `preference_decay` | float | 0.001 | Preference decay rate | 0.0-1.0 |
| `working_memory_capacity` | float | 0.1 | Working memory capacity | 0.0-1.0 |
| `episodic_memory_max_episodes` | float | 1000 | Maximum episodic memory episodes | 1-10000 |
| `neuromodulation_strength` | float | 1.0 | Neuromodulation strength | 0.0-2.0 |
| `development_maturation_rate` | float | 0.01 | Development maturation rate | 0.0-0.1 |

## Checkpoint Management

### `checkpoint <action> [name]`

Manage simulation checkpoints for saving and restoring brain states.

#### Actions

1. **`checkpoint create <name> [<description>]`**
   - Create a new checkpoint
   - Example: `nlm checkpoint create "Before experiment" "State before neural tuning"`

2. **`checkpoint list`**
   - List all checkpoints
   - Example: `nlm checkpoint list`

3. **`checkpoint load <name>`**
   - Load a checkpoint
   - Example: `nlm checkpoint load checkpoint1.ckp`

4. **`checkpoint delete <name>`**
   - Delete a checkpoint
   - Example: `nlm checkpoint delete checkpoint1.ckp`

#### Checkpoint Format

Checkpoints are binary files containing:
- Brain state (neuron membrane potentials, synaptic weights)
- Working memory content
- Episodic memory episodes
- Neuromodulation state
- Development stage
- Performance metrics

## Performance Monitoring

### `profile <action>`

Monitor and analyze simulation performance.

#### Actions

1. **`profile start`**
   - Start performance monitoring
   - Example: `nlm profile start`

2. **`profile stop`**
   - Stop performance monitoring
   - Example: `nlm profile stop`

3. **`profile status`**
   - Show current performance status
   - Example: `nlm profile status`

#### Performance Metrics

- **CPU Usage**: Percentage of CPU utilization
- **Memory Usage**: Memory consumption in MB/GB
- **Simulation Time**: Real-time simulation speed (1.0x = real time)
- **FPS**: Frames per second (simulation steps per second)
- **Spike Count**: Total number of neural spikes
- **Neuron Count**: Number of active neurons
- **Synapse Count**: Number of active synapses

## Result Export

### `export <type> <format> [path]`

Export simulation results in various formats.

#### Types

| Type | Description |
|------|-------------|
| `results` | Complete simulation results |
| `neurons` | Neural activity data |
| `synapses` | Synaptic connection data |
| `simulation` | Full simulation state |
| `memory` | Memory system data |
| `config` | Configuration parameters |
| `performance` | Performance metrics |

#### Formats

| Format | Description |
|--------|-------------|
| `json` | JSON format (machine-readable) |
| `csv` | CSV format (spreadsheet) |
| `neural` | Neural network format |
| `visualization` | Visualization-ready format |
| `binary` | Binary format (compact) |
| `text` | Plain text format |
| `xml` | XML format (structured data) |

#### Usage Examples

```bash
# Export results as JSON
nlm export results json ./results.json

# Export neurons as CSV with custom options
nlm export neurons csv ./neurons.csv --include_spikes --time_window 1000

# Export simulation for visualization
nlm export simulation visualization ./simulation.viz

# Export performance metrics with formatting
nlm export performance json ./performance.json --pretty --include_detailed_stats
```

## Plugin Management

### `plugin <action> <name>`

Manage NLM plugins for extending functionality.

#### Actions

1. **`plugin list`**
   - List available plugins
   - Example: `nlm plugin list`

2. **`plugin enable <name>`**
   - Enable a plugin
   - Example: `nlm plugin enable export_manager`

3. **`plugin disable <name>`**
   - Disable a plugin
   - Example: `nlm plugin disable export_manager`

#### Available Plugins

- **config_manager**: Configuration management
- **checkpoint_manager**: Checkpoint operations
- **performance_monitor**: Performance monitoring
- **export_manager**: Result export
- **interactive_mode**: REPL interface
- **visualization**: Data visualization
- **analysis**: Statistical analysis

## Interactive Mode

### `interactive`

Enter the NLM Interactive REPL (Read-Eval-Print Loop) mode for experimentation.

#### Interactive Commands

| Command | Description |
|---------|-------------|
| `help` | Show help information |
| `run <phase> [options]` | Run a phase |
| `config <action> [key] [value]` | Manage configuration |
| `checkpoint <action> [name]` | Manage checkpoints |
| `profile <action>` | Performance monitoring |
| `export <type> <format> [path]` | Export results |
| `phase2`, `phase3`, `phase4`, `phase5`, `phase6` | Run specific phases |
| `exit`, `quit` | Exit interactive mode |

#### Usage Example

```
nlm interactive
nlm:phase6 --neuron_count 1000 --max_steps 2000
nlm:config set neuron_count 2000
nlm:checkpoint create "Snapshot 1"
nlm:profile start
nlm:run phase6 --max_steps 500
nlm:profile stop
nlm:export results json ./results.json
exit
```

## Configuration File Format

### File Format

NLM configuration files use a simple key-value format:

```ini
# NLM Configuration File
# Generated by NLM CLI

# Basic settings
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Neural parameters
random_seed = 42
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Comments are indicated with #
# Empty lines are ignored
```

### Loading Configuration

```bash
# Load configuration from file
nlm config load my_config.cfg

# Run with configuration file
nlm --config my_config.cfg run phase6

# Save current configuration
nlm config save current_config.cfg
```

## Error Handling

The NLM CLI provides detailed error messages with suggestions:

### Error Message Format

```
Error <code>: <message>
  Context: <context>
  Suggestion: <suggestion>
```

### Common Errors and Solutions

| Error Code | Description | Solution |
|------------|-------------|----------|
| 1 | Invalid command | Use `nlm help` for available commands |
| 2 | Missing required argument | Check command syntax |
| 3 | Invalid configuration value | Validate configuration with `nlm config validate` |
| 4 | File not found | Check file path and permissions |
| 5 | Permission denied | Run with appropriate privileges |
| 6 | Out of memory | Reduce simulation size or increase system resources |
| 7 | Timeout | Reduce simulation duration or increase timeout |

## Examples

### Beginner Examples

```bash
# Quick start
nlm help
nlm version
nlm run phase2

# Run with configuration
nlm --config configs/example.cfg run phase6

# Check simulation status
nlm profile status
```

### Intermediate Examples

```bash
# Run with monitoring
nlm profile start
nlm run phase3 --episodes 5 --steps 200
nlm profile stop

# Use checkpoints
nlm checkpoint create "Before experiment"
nlm run phase5 --steps 100
nlm checkpoint load checkpoint.ckp

# Export results
nlm export results phase3_results.json json
nlm export neurons neurons.csv csv
```

### Advanced Examples

```bash
# Custom configuration
nlm config load custom.cfg
nlm run phase6 --neuron_count 2000 --max_steps 5000

# Interactive experimentation
nlm interactive
>>> run phase6 --max_steps 1000
>>> config set neuron_count 3000
>>> export results temp_results.json json
>>> checkpoint create "Experimental snapshot"

# Performance optimization
nlm profile start
nlm run phase4 --trials 100
nlm profile stop

# Plugin development
nlm plugin enable analysis
nlm run phase6 --enable_analysis
```

## Troubleshooting

### Common Issues

1. **"Command not found"**
   - Solution: Use `nlm help` to see available commands

2. **"Configuration validation error"**
   - Solution: Use `nlm config validate` to see specific errors

3. **"File not found"**
   - Solution: Check file path and ensure file exists

4. **"Permission denied"**
   - Solution: Run with appropriate privileges or check file permissions

5. **"Out of memory"**
   - Solution: Reduce simulation size or increase system resources

### Getting Help

- **Command Help**: `nlm help <command>`
- **Phase Help**: `nlm help run <phase>`
- **Documentation**: `nlm docs`
- **Examples**: Check the `docs/` directory
- **Community**: Visit the NLM project website

## License

This NLM CLI implementation is part of the NLM (熙然) Neural Learning Machine project. It is licensed under the MIT License.

## Copyright

© 2024 NLM Project. All rights reserved.
