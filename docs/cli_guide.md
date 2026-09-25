# NLM CLI Guide

This guide provides comprehensive documentation for using the NLM (Neural Learning Machine) CLI.

## Overview

NLM is an experimental artificial developmental brain system that simulates neural computation with spiking neurons, plasticity rules, and memory systems.

## Quick Start

### Basic Usage

```bash
# Show help
nlm --help

# Run a basic simulation
nlm run

# Run an educational demo
nlm demo phase6

# Start interactive exploration mode
nlm repl

# View configuration
nlm config show

# Create sample configuration
nlm config init

# Save current brain state
nlm brain save mybrain.brain
```

## Commands

### Basic Operations

#### `nlm run`
Run a simulation with basic configuration.

**Options:**
- `--config FILE` - Load configuration from FILE
- `--verbose, -V` - Enable verbose output
- `--quiet, -q` - Quiet output (errors only)

**Example:**
```bash
nlm run --config myconfig.cfg --verbose
```

#### `nlm demo EXPERIMENT`
Run educational demonstrations.

**Available experiments:**
- `phase2` - Basic neural computation
- `phase4` - Memory systems with cognition
- `phase6` - Full integration demonstration

**Example:**
```bash
nlm demo phase6 --verbose
```

#### `nlm explore`
Interactive exploration mode with REPL interface.

**Available commands in REPL:**
- `help` or `?` - Show help
- `step N` - Advance N simulation steps
- `reset` - Reset brain to initial state
- `inspect` - Show brain state
- `plot` - Show neural activity plot
- `quit` or `q` - Exit exploration mode

**Example:**
```bash
nlm explore
```

### Configuration Management

#### `nlm config init`
Create a sample configuration file.

Creates `configs/sample.cfg` with default settings for Phase 2 experiments.

**Example:**
```bash
nlm config init
```

#### `nlm config validate`
Validate the current configuration.

Checks for:
- Required parameters
- Valid value ranges
- Consistency between settings

**Example:**
```bash
nlm config validate
```

#### `nlm config show`
Display the current configuration summary.

Shows all configuration values with their sources (file, default, command-line).

**Example:**
```bash
nlm config show
```

### Brain Management

#### `nlm brain save FILENAME`
Save the current brain state to a file.

The brain state includes:
- Neural connections (weights)
- Neuron membrane potentials
- Plasticity rule states
- Memory system contents

**Example:**
```bash
nlm brain save mybrain.brain
```

#### `nlm brain load FILENAME`
Load a saved brain state from a file.

Restores the brain to a previously saved state.

**Example:**
```bash
nlm brain load mybrain.brain
```

#### `nlm brain status`
Show current brain statistics and metrics.

Displays:
- Brain structure (neurons, synapses, regions)
- Performance metrics
- Activity statistics
- System status

**Example:**
```bash
nlm brain status
```

### Advanced Features

#### `nlm experiment TYPE NAME`
Run specific experiments with detailed control.

**Example:**
```bash
nlm experiment phase6 "Integration Test"
```

#### `nlm benchmark`
Run performance benchmarking.

Measures and reports:
- Brain initialization time
- Simulation performance
- Memory usage
- Steps per second

**Example:**
```bash
nlm benchmark
```

### Interactive Mode

#### `nlm repl`
Full-featured REPL for brain exploration.

**Key features:**
- Real-time brain state inspection
- Step-by-step simulation control
- Neural visualization
- Command history
- Built-in help system

**Example usage:**
```bash
nlm repl
> help
> step 100
> inspect
> plot
> quit
```

## Configuration Files

### Sample Configuration (`configs/sample.cfg`)

```ini
# Sample NLM Configuration
# This is a sample configuration file for NLM simulations

[simulation]
  simulation_timestep = 0.001
  max_steps = 1000

[brain]
  neuron_count = 500
  region_count = 1

[plasticity]
  stdp_ltp_weight = 0.02
  stdp_ltd_weight = 0.015
  stdp_tau = 20.0

[neuromodulation]
  dopamine_level = 0.5
  curiosity_strength = 0.3
  novelty_threshold = 0.1
```

### Configuration File Format

NLM configuration files use INI format:

```ini
[section_name]
  key = value
  another_key = another_value
```

**Supported types:**
- Integer64: `value = 42`
- Double: `value = 3.14`
- Float: `value = 2.5`
- Boolean: `value = true` or `value = false`
- String: `value = "string with spaces"`

## Brain File Format

Brain state files are binary files containing:
- Neural connectivity matrices
- Neuron states (membrance potentials, thresholds)
- Plasticity rule parameters
- Memory system contents
- Development stage information

Brain files are platform-independent and can be loaded on different machines.

## Educational Features

### Built-in Tutorials

The CLI includes several educational tutorials:

1. **Phase 2 Tutorial** (`nlm demo phase2`)
   - Introduces LIF neuron dynamics
   - Demonstrates STDP and Hebbian plasticity
   - Shows structural plasticity

2. **Phase 4 Tutorial** (`nlm demo phase4`)
   - Covers memory systems (working, episodic, associative)
   - Demonstrates prediction and planning
   - Shows cognitive mechanisms (attention, self-model)

3. **Phase 6 Tutorial** (`nlm demo phase6`)
   - Full system integration demonstration
   - Tests all brain components working together
   - Shows real-world applications

### Interactive Exercises

The REPL mode supports interactive exercises:

1. **Neural Dynamics Exploration**
   - Step through simulation manually
   - Inject currents into specific neurons
   - Observe spike propagation

2. **Plasticity Investigation**
   - Watch synaptic weights change
   - Test different input patterns
   - Analyze learning curves

3. **Memory Analysis**
   - Examine working memory states
   - Track episodic memory encoding
   - Analyze concept formation

## Example Configurations

See the `examples/` directory for:
- Advanced simulation setups
- Different brain architectures
- Experiment configurations
- Visualization settings

## Command Line Options

### Global Options

- `--help, -h` - Show help message
- `--version, -v` - Show version information
- `--quiet, -q` - Quiet output (errors only)
- `--verbose, -V` - Verbose output
- `--debug` - Debug output (maximum verbosity)

### Command-specific Options

Each command may have additional options specific to its functionality.

## Troubleshooting

### Common Issues

1. **Configuration file not found**
   ```bash
   nlm run --config configs/default.cfg
   ```

2. **Brain initialization fails**
   ```bash
   nlm brain status
   nlm run --verbose
   ```

3. **REPL doesn't start**
   ```bash
   nlm explore --help
   ```

4. **Memory issues**
   ```bash
   nlm benchmark
   ```

### Error Messages

- "Unknown command: X" - Command not recognized
- "Failed to load configuration" - Configuration file error
- "Brain initialization failed" - Simulation setup issue
- "Permission denied" - File access issue

## Advanced Usage

### Scripted Simulations

For batch processing, use configuration files:

```bash
#!/bin/bash
nlm run --config config1.cfg > output1.log
nlm brain save brain1.brain
nlm explore --load brain1.brain --steps 1000
nlm brain save brain2.brain
```

### Monitoring and Logging

Enable maximum verbosity for debugging:

```bash
nlm run --debug --output simulation.log
```

### Performance Optimization

Use the benchmark command to optimize:

```bash
nlm benchmark
```

## Keybindings (REPL Mode)

| Command | Description |
|---------|-------------|
| `help` | Show help |
| `step N` | Advance N steps |
| `reset` | Reset brain |
| `inspect` | Show brain status |
| `plot` | Show neural activity |
| `quit` | Exit REPL |

## Related Tools

The NLM ecosystem includes:
- **NLM Visualizer** - Advanced visualization tools
- **NLM Simulator** - Specialized simulation modes
- **NLM Analyzer** - Post-simulation analysis

These can be accessed via the CLI for enhanced functionality.

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Integration Guide](docs/PHASE6_FINAL_AUDIT.md)
- [Scientific Background](docs/SCIENCE.md)
- [Experimental Methods](docs/EXPERIMENTS.md)

## Getting Help

For additional assistance:
1. Use `nlm help` for command-specific documentation
2. Check the `docs/` directory for detailed guides
3. Visit the NLM documentation website
4. Join the NLM community forums

## Version Information

NLM Neural Learning Machine v1.0.0
Phase 6: Final Integration
Built: $(date)
