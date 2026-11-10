# NLM Command Line Interface
# Comprehensive CLI for advanced users

## Overview

The NLM (Neural Learning Machine) command-line interface provides advanced users with powerful tools for:
- Running simulations with various configurations
- Performance analysis and benchmarking
- Experiment management and batch processing
- Configuration management
- Debugging and diagnostics
- Advanced analysis and visualization

## Installation

```bash
# Build the project first (see README.md for instructions)
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Available Commands

### Core Commands

`nlm help [topic]` - Display help information
`nlm version` - Show version information
`nlm config <command> [options]` - Configuration management
`nlm experiment <command> [options]` - Experiment management
`nlm benchmark <options>` - Performance benchmarking
`nlm simulate <options>` - Run a single simulation
`nlm monitor <options>` - Real-time monitoring
`nlm analyze <options>` - Analysis and diagnostics

### Advanced Commands

`nlm batch <options>` - Run batch experiments
`nlm replay <options>` - Replay saved simulations
`nlm checkpoint <command> [options]` - Checkpoint management
`nlm profile <options>` - Performance profiling
`nlm validate <options>` - Configuration validation
`nlm visualize <options>` - Visualization and analysis

## Command: nlm help

### Usage
```bash
nlm help [topic]
nlm --help [topic]
nlm -h [topic]
```

### Description
Display help information about NLM commands, configuration options, or advanced features.

### Arguments

`[topic]` - Specific topic to get help for (e.g., "config", "benchmark", "experiment")

### Examples

```bash
# General help
nlm help

# Help for config commands
nlm help config

# Help for benchmark options
nlm help benchmark
```

## Command: nlm version

### Usage
```bash
nlm version
```

### Description
Display NLM version information and build configuration.

### Examples

```bash
nlm version
```

## Command: nlm config

### Usage
```bash
nlm config <command> [options]
```

### Subcommands

`init [template]` - Initialize a configuration template
`validate <file>` - Validate a configuration file
`compare <file1> <file2>` - Compare two configuration files
`export <file> [format]` - Export configuration to different formats
`merge <file1> <file2> <output>` - Merge configuration files
`stats <file>` - Show configuration statistics

### Configuration Management Examples

```bash
# Initialize a basic configuration template
nlm config init basic.cfg

# Validate a configuration file
nlm config validate my_config.cfg

# Compare two configuration files
nlm config compare old_config.cfg new_config.cfg

# Export configuration to JSON
nlm config export config.cfg --format json

# Merge two configuration files
nlm config merge base.cfg overrides.cfg merged.cfg
```

## Command: nlm experiment

### Usage
```bash
nlm experiment <command> [options]
```

### Subcommands

`list` - List all available experiments
`run <name> [options]` - Run a specific experiment
`create <name> [template]` - Create a new experiment
`clone <name> [new_name]` - Clone an existing experiment
`stats <name>` - Show experiment statistics
`delete <name>` - Delete an experiment

### Experiment Management Examples

```bash
# List all available experiments
nlm experiment list

# Run the Phase 6 integration experiment
nlm experiment run phase6 --neurons 1000 --steps 5000

# Create a new experiment from a template
nlm experiment create my_experiment basic_template

# Clone an existing experiment
nlm experiment clone phase6 custom_phase6
```

## Command: nlm benchmark

### Usage
```bash
nlm benchmark [options]
```

### Options

`-n, --neurons N` - Number of neurons (default: 1000)
`-s, --steps S` - Number of simulation steps (default: 1000)
`-r, --runs R` - Number of benchmark runs (default: 5)
`-o, --output FILE` - Output file for results (default: benchmark_results.json)
`--verbose` - Enable verbose output
`--no-plots` - Skip plotting results

### Benchmark Examples

```bash
# Basic benchmark
nlm benchmark --neurons 1000 --steps 1000

# Comprehensive benchmark with multiple configurations
nlm benchmark --neurons 500,1000,2000 --steps 1000,2000,5000 --runs 3

# Save results to file
nlm benchmark --neurons 2000 --steps 2000 --output my_benchmark.json
```

## Command: nlm simulate

### Usage
```bash
nlm simulate [options]
```

### Options

`-c, --config FILE` - Configuration file (default: configs/default.cfg)
`-n, --neurons N` - Override neuron count
`-s, --steps S` --Simulation steps
`-t, --timestep T` - Simulation timestep (default: 0.001)
`--checkpoint FILE` - Save checkpoint to file
`--no-plots` - Skip plotting
`--verbose` - Enable verbose output

### Simulation Examples

```bash
# Run a simple simulation
nlm simulate --steps 1000

# Run with custom configuration
nlm simulate --config my_config.cfg --steps 5000

# Run and save checkpoint
nlm simulate --steps 2000 --checkpoint simulation.cp
```

## Command: nlm monitor

### Usage
```bash
nlm monitor [options]
```

### Options

`-c, --config FILE` - Configuration file
`-p, --port P` - Port for web interface (default: 8080)
`--web` - Enable web interface
`--metrics M` - Metrics to monitor (comma-separated list)
`--interval I` - Monitoring interval in seconds (default: 1.0)

### Monitoring Examples

```bash
# Start real-time monitoring
nlm monitor

# Start web interface
nlm monitor --web --port 8080

# Monitor specific metrics
nlm monitor --metrics neurons,spikes,firing_rate
```

## Command: nlm analyze

### Usage
```bash
nlm analyze <file> [options]
```

### Options

`-o, --output FILE` - Output file for analysis results
`--plot` - Generate plots
`--json` - Output in JSON format
`--verbose` - Enable verbose output

### Analysis Examples

```bash
# Analyze simulation results
nlm analyze simulation_results.dat --plot --output analysis.png

# Analyze checkpoint
nlm analyze checkpoint.cp --json --verbose
```

## Command: nlm batch

### Usage
```bash
nlm batch <command> [options]
```

### Subcommands

`run <experiment> [options]` - Run a batch of experiments
`grid <experiment> [options]` - Run a parameter grid search
`monte-carlo <experiment> [options]` - Run Monte Carlo simulations

### Batch Examples

```bash
# Run batch of experiments
nlm batch run phase6 --neurons 500,1000,2000 --steps 1000,2000

# Parameter grid search
nlm batch grid my_experiment --parameters "neurons=500,1000,2000;plasticity_rate=0.001,0.01,0.1"

# Monte Carlo simulation
nlm batch monte-carlo learning_exp --runs 10 --seed-range 1-100
```

## Command: nlm replay

### Usage
```bash
nlm replay <file> [options]
```

### Options

`--speed S` - Replay speed multiplier (default: 1.0)
`--skip FRAME` - Skip every Nth frame
`--format F` - Output format (default: text)
`--output FILE` - Output file for replay

### Replay Examples

```bash
# Replay a simulation
nlm replay simulation.cp

# Replay with modified speed
nlm replay simulation.cp --speed 2.0

# Replay and save as video
nlm replay simulation.cp --format video --output replay.mp4
```

## Command: nlm checkpoint

### Usage
```bash
nlm checkpoint <command> [options]
```

### Subcommands

`list` - List all checkpoints
`create <file>` - Create a new checkpoint
`restore <file>` - Restore from checkpoint
`delete <file>` - Delete a checkpoint
`compare <file1> <file2>` - Compare two checkpoints

### Checkpoint Examples

```bash
# List all checkpoints
nlm checkpoint list

# Create a new checkpoint
nlm checkpoint create brain_state.cp

# Restore from checkpoint
nlm checkpoint restore backup.cp
```

## Command: nlm profile

### Usage
```bash
nlm profile [options]
```

### Options

`-c, --config FILE` - Configuration file
`-o, --output FILE` - Output file for profiling results
`--memory` - Profile memory usage
`--time` - Profile execution time
`--cpu` - Profile CPU usage
`--gpu` - Profile GPU usage (if available)

### Profiling Examples

```bash
# Profile execution time
nlm profile --time --output profile.txt

# Profile memory usage
nlm profile --memory --output memory_profile.json
```

## Command: nlm validate

### Usage
```bash
nlm validate <file> [options]
```

### Options

`--strict` - Enable strict validation
`--json` - Output in JSON format
`--verbose` - Enable verbose output

### Validation Examples

```bash
# Validate configuration file
nlm validate config.cfg

# Validate with strict mode
nlm validate config.cfg --strict

# Validate and output JSON
nlm validate config.cfg --json --output validation_report.json
```

## Command: nlm visualize

### Usage
```bash
nlm visualize <file> [options]
```

### Options

`-o, --output FILE` - Output file for visualization
`--type T` - Visualization type (neurons, spikes, weights, activity)
`--range MIN,MAX` - Data range for visualization
`--interactive` - Enable interactive visualization
`--save-gif` - Save as GIF animation

### Visualization Examples

```bash
# Visualize neuron activity
nlm visualize simulation_results.dat --type activity --output activity.png

# Create interactive visualization
nlm visualize data.dat --interactive

# Save as GIF animation
nlm visualize simulation.cp --save-gif --output animation.gif
```

## Configuration Files

### Default Configuration Template

```ini
# Default configuration template for NLM simulations

[simulation]
neurons = 1000
regions = 1
timestep = 0.001
max_steps = 1000
connection_probability = 0.1

[plasticity]
stdp_enabled = true
hebbian_enabled = true
structural_plasticity_enabled = true

[neuromodulation]
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true

[memory]
working_memory_enabled = true
episodic_memory_enabled = true
associative_memory_enabled = true

[development]
enabled = true
stage = "initial"

[experiment]
name = "default_experiment"
seed = 42
enable_checkpointing = false
enable_replay = false
```

### JSON Configuration Format

```json
{
  "simulation": {
    "neurons": 1000,
    "regions": 1,
    "timestep": 0.001,
    "max_steps": 1000
  },
  "plasticity": {
    "stdp_enabled": true,
    "hebbian_enabled": true,
    "structural_plasticity_enabled": true
  }
}
```

## Configuration Management

### Environment Variables

- `NLM_HOME` - NLM installation directory
- `NLM_CONFIG` - Default configuration file
- `NLM_LOG_LEVEL` - Logging level (debug, info, warning, error)
- `NLM_DATA_DIR` - Directory for data files

### Configuration File Locations

1. User configuration: `~/.config/nlm/config.cfg`
2. System configuration: `/etc/nlm/config.cfg`
3. Project configuration: `./configs/default.cfg`
4. Command line overrides

## Advanced Usage

### Creating Custom Experiments

```bash
# Create a custom experiment configuration
nlm experiment create custom_learning learning_template \
  --neurons 500 \
  --plasticity_rate 0.05 \
  --enable_curiosity \
  --enable_reward_modulation
```

### Performance Optimization

```bash
# Optimize for specific hardware
nlm benchmark --neurons 2000 --steps 5000 --runs 3 --verbose

# Monitor system resources during simulation
nlm simulate --steps 10000 --monitor-system
```

### Debugging and Development

```bash
# Run with debug logging
nlm simulate --verbose --debug

# Validate configuration before running
nlm validate config.cfg --strict

# Profile memory usage
nlm profile --memory --output memory_analysis.json
```

## Troubleshooting

### Common Issues

1. **Configuration file not found**
   ```bash
   nlm config validate my_config.cfg
   ```

2. **Memory issues**
   ```bash
   nlm config set neurons 500
   nlm simulate --steps 1000
   ```

3. **Simulation too slow**
   ```bash
   nlm benchmark --neurons 500 --steps 1000
   nlm profile --time
   ```

4. **Web interface not working**
   ```bash
   nlm monitor --web --port 8080
   ```

### Getting Help

```bash
# General help
nlm help

# Command-specific help
nlm help config
nlm help benchmark
nlm help experiment

# Quick start guide
nlm help quickstart
```

## Contributing

This CLI is designed to be extensible. To add new commands or features:

1. Create a new source file in `cli/` directory
2. Add the command to the main CLI dispatcher
3. Follow the existing patterns and conventions
4. Update documentation accordingly

## License

MIT License - See LICENSE file for details

## Author

NLM Development Team

---

For more information, visit the NLM documentation at https://nlm.readthedocs.io or join the NLM community on GitHub.