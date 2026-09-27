# Command Reference

## Overview

NLM provides a comprehensive command-line interface for controlling brain simulation experiments. This reference documents all available commands, options, and usage patterns.

## Command Categories

### Core Commands

#### help or --help

**Display help information**

```bash
nlm help
nlm --help
nlm help command_name
```

**Description:** Shows help information. Without arguments, displays general help. With a command name, shows specific command help.

**Example:**
```bash
nlm help
nlm help run
```

#### version or --version

**Show NLM version information**

```bash
nlm version
nlm --version
```

**Description:** Displays NLM version, build information, and compilation details.

**Example:**
```bash
$ nlm --version
NLM 1.0.0
Build: Phase 6 Final Integration
Compiled: 2026-09-05 18:16:42
Platform: Linux x86_64
Features: SIMD, OpenMP, Memory Pools
```

#### list-experiments or --list-experiments

**List available experiments**

```bash
nlm list-experiments
nlm --list-experiments
```

**Description:** Displays all built-in experiments and their descriptions.

**Example:**
```bash
$ nlm list-experiments
Available Experiments:
  1. nlm_phase6_demo    - Comprehensive integration test
  2. simple_world       - Basic world interaction
  3. plasticity_study   - Synaptic plasticity analysis
  4. cognitive_development - Cognitive skill development
  5. memory_efficiency - Memory system optimization
```

#### status or --status

**Show current simulation status**

```bash
nlm status
nlm --status
```

**Description:** Displays current brain state, performance metrics, and system statistics.

**Example:**
```bash
$ nlm status
Brain Status: RUNNING
Simulation Steps: 1247/5000
Performance: 94.2%
Total Spikes: 3892
Memory Usage: 1.2 GB
Neuromodulation: ACTIVE
```

### Simulation Control Commands

#### run or --run

**Run simulation for specified steps**

```bash
nlm run [steps]
nlm --run [steps]
nlm run [steps] --config [file]
nlm run [steps] --output [file]
```

**Description:** Executes brain simulation. Steps default to configuration setting if not specified.

**Parameters:**

| Parameter | Short Form | Required | Default | Description |
|-----------|------------|----------|---------|-------------|
| steps | -s | No | Config value | Number of simulation steps |
| config | -c | No | current.json | Configuration file path |
| output | -o | No | None | Output file path |
| verbose | -v | No | false | Enable verbose logging |

**Examples:**
```bash
# Run with default configuration
nlm run 100

# Run with custom config and output
nlm run 500 --config=experiment.json --output=results.csv

# Run in verbose mode
nlm run 200 -v --config=debug_config.json
```

#### pause or --pause

**Pause current simulation**

```bash
nlm pause
nlm --pause
```

**Description:** Temporarily halts simulation execution while preserving current state.

**Example:**
```bash
$ nlm pause
Simulation paused at step 347
State saved to pause_state.json
```

#### resume or --resume

**Resume paused simulation**

```bash
nlm resume
nlm --resume
nlm resume --from [file]
```

**Description:** Resumes a previously paused simulation from saved state.

**Parameters:**

| Parameter | Short Form | Required | Default | Description |
|-----------|------------|----------|---------|-------------|
| from | -f | No | pause_state.json | State file to load |

**Example:**
```bash
nlm resume
nlm resume --from=experiment_state.json
```

#### stop or --stop

**Stop simulation**

```bash
nlm stop
nlm --stop
nlm stop --save-state [file]
```

**Description:** Terminates current simulation with optional state preservation.

**Parameters:**

| Parameter | Short Form | Required | Default | Description |
|-----------|------------|----------|---------|-------------|
| save-state | -s | No | None | Save final state to file |

**Example:**
```bash
# Stop without saving
nlm stop

# Stop with state save
nlm stop --save-state=final_state.json
```

### Configuration Management

#### load or --load

**Load configuration from file**

```bash
nlm load [file]
nlm --load [file]
```

**Description:** Loads new configuration, replacing current settings.

**Parameters:**

| Parameter | Short Form | Required | Default | Description |
|-----------|------------|----------|---------|-------------|
| file | -f | Yes | None | Configuration file path |

**Example:**
```bash
nlm load configs/experiment.json
```

#### save or --save

**Save current configuration**

```bash
nlm save [file]
nlm --save [file]
```

**Description:** Saves current configuration to file for later use.

**Parameters:**

| Parameter | Short Form | Required | Default | Description |
|-----------|------------|----------|---------|-------------|
| file | -f | No | current_config.json | Output file path |

**Example:**
```bash
nlm save my_experiment.json
```

#### export or --export

**Export results in various formats**

```bash
nlm export [format] [options]
nlm --export [format] [options]
```

**Description:** Exports simulation results in specified formats for analysis.

**Parameters:**

| Parameter | Short Form | Required | Default | Description |
|-----------|------------|----------|---------|-------------|
| format | -f | Yes | None | Export format (csv, json, matlab, pickle, graphs) |
| output | -o | No | results.[format] | Output file path |

**Formats:**

- `csv`: Comma-separated values for spreadsheet analysis
- `json`: JavaScript Object Notation for programmatic access
- `matlab`: MATLAB/Octave workspace format
- `pickle`: Python pickle format
- `graphs`: PNG image files for visualization

**Examples:**
```bash
# Export results as CSV
nlm export csv --output=neuron_data.csv

# Export as MATLAB format
nlm export matlab --output=brain_data.mat

# Export graphs
nlm export graphs --output=neural_plots
```

### Analysis and Monitoring

#### analyze or --analyze

**Analyze simulation results**

```bash
nlm analyze [options]
nlm --analyze [options]
```

**Description:** Performs comprehensive analysis of simulation results.

**Options:**

- `--statistics`: Calculate statistical summaries
- `--patterns`: Detect behavioral patterns
- `--correlations`: Compute correlation matrices
- `--performance`: Performance benchmarking
- `--output [file]`: Save analysis results

**Example:**
```bash
nlm analyze --statistics --patterns --output=analysis_report.txt
```

#### monitor or --monitor

**Monitor simulation in real-time**

```bash
nlm monitor [metrics] [interval]
nlm --monitor [metrics] [interval]
```

**Description:** Displays real-time monitoring of specified metrics.

**Parameters:**

| Parameter | Required | Default | Description |
|-----------|----------|---------|-------------|
| metrics | No | all | Metrics to monitor (spikes, memory, performance, plasticity, neuromodulation) |
| interval | No | 1.0 | Update interval in seconds |

**Example:**
```bash
# Monitor all metrics every 2 seconds
nlm monitor all 2.0

# Monitor only performance and memory
nlm monitor performance,memory 0.5
```

### Development and Debugging

#### validate or --validate

**Validate configuration**

```bash
nlm validate
nlm --validate
```

**Description:** Validates current configuration for errors and inconsistencies.

**Example:**
```bash
$ nlm validate
Configuration is valid!
2 warnings:
  - "random_seed" not specified, using default: 42
  - "simulation.timestep" below recommended minimum: 0.001
```

#### profile or --profile

**Profile performance**

```bash
nlm profile [type] [output]
nlm --profile [type] [output]
```

**Description:** Runs simulation with performance profiling enabled.

**Profile Types:**

- `memory`: Memory usage profiling
- `cpu`: CPU performance profiling
- `detailed`: Comprehensive profiling
- `report`: Generate performance report

**Example:**
```bash
nlm profile detailed performance_report.txt
```

#### debug or --debug

**Enable debug mode**

```bash
nlm debug [level]
nlm --debug [level]
```

**Description:** Enables debug mode with enhanced logging and error reporting.

**Debug Levels:**

- `minimal`: Basic error reporting
- `standard`: Detailed logging
- `full`: Maximum debugging information
- `symbols`: Symbol information for crash reports

**Example:**
```bash
nlm debug full --output=debug.log
```

### Plugin Commands

#### plugins or --plugins

**List available plugins**

```bash
nlm plugins
nlm --plugins
```

**Description:** Lists all registered plugins and their versions.

**Example:**
```bash
$ nlm plugins
Available Plugins:
  1. NeuralVisualizer v2.1.0
  2. MemoryOptimizer v1.5.2
  3. ExportHandler v3.0.0
```

#### plugin or --plugin

**Plugin management**

```bash
nlm plugin [action] [plugin_name] [options]
nlm --plugin [action] [plugin_name] [options]
```

**Actions:**

- `load [file]`: Load plugin from file
- `unload [name]`: Unload plugin
- `enable [name]`: Enable plugin
- `disable [name]`: Disable plugin
- `info [name]`: Show plugin information

**Example:**
```bash
nlm plugin load plugins/custom_visualizer.so
nlm plugin enable NeuralVisualizer
```

## Command Line Syntax

### General Syntax

```
nlm [global_options] command [command_options]
```

### Global Options

| Option | Description |
|--------|-------------|
| `--help` | Show general help |
| `--version` | Show version information |
| `--verbose` | Enable verbose output |
| `--quiet` | Suppress output |

### Command Options

Each command has its own specific options. Use `nlm help [command]` for detailed information about a specific command.

## Command Examples

### Complete Experiment Workflow

```bash
# 1. List available experiments
nlm list-experiments

# 2. Load configuration
nlm load configs/cognitive_development.json

# 3. Run experiment
nlm run 1000 --verbose

# 4. Monitor progress
nlm monitor performance,memory 2.0

# 5. Analyze results
nlm analyze --statistics --patterns

# 6. Export data
nlm export csv --output=results.csv

# 7. Save final configuration
nlm save final_config.json

# 8. Stop simulation
nlm stop --save-state=final_brain_state.json
```

### Quick Experiment Setup

```bash
# One-line experiment execution
./nlm --config=quick_experiment.json --steps=100 --output=quick_results.json --verbose
```

### Debugging Session

```bash
# Start debugging session
nlm debug full --output=debug_session.log

# Load and validate configuration
nlm load configs/debug_config.json
nlm validate

# Run with profiling
nlm profile detailed debug_profile.txt

# Monitor during execution
nlm monitor all 0.5

# Analyze and export
nlm analyze --statistics --output=debug_analysis.txt
nlm export csv --output=debug_results.csv

# Clean up
nlm stop
```

## Command Line Tips

### Efficiency

- **Use short options** where possible: `-s 100 -c config.json` instead of `--steps 100 --config config.json`
- **Batch operations**: Combine multiple commands in scripts
- **Redirect output**: Use `>` to save output to files

### Troubleshooting

- **Invalid commands**: Use `nlm help` for command list
- **Configuration issues**: Use `nlm validate` to check problems
- **Performance issues**: Use `nlm profile` to identify bottlenecks
- **Debug issues**: Use `nlm debug full` for detailed information

### Scripting

```bash
#!/bin/bash
# NLM experiment script

# Configuration
CONFIG="configs/my_experiment.json"
STEPS=500
OUTPUT="results/experiment_$(date +%Y%m%d).json"

# Run experiment
./nlm --config="$CONFIG" --steps="$STEPS" --output="$OUTPUT"

# Analyze results
./nlm analyze --statistics --output="analysis_$(date +%Y%m%d).txt"

# Export for further analysis
./nlm export csv --output="data_$(date +%Y%m%d).csv"

# Save final state
./nlm stop --save-state="final_state_$(date +%Y%m%d).json"

echo "Experiment completed successfully!"
```

## License

NLM Command Line Interface is licensed under the MIT License.

## Copyright

© 2026 NLM Development Team. All rights reserved.