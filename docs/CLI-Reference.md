# NLM CLI Reference

## Overview

The NLM Command Line Interface (CLI) provides access to all simulation and analysis capabilities. This reference covers all available commands, their arguments, and usage examples.

## Global Options

All NLM commands share these global options:

| Option | Short | Description |
|--------|--------|-------------|
| `--help` | `-h` | Show help message and exit |
| `--version` | `-v` | Show version information and exit |
| `--config` | `-c` | Path to configuration file |
| `--log-level` | `-l` | Set logging level (DEBUG, INFO, WARNING, ERROR) |
| `--output` | `-o` | Output file path |
| `--verbose` | `-V` | Enable verbose output |
| `--quiet` | `-q` | Suppress non-essential output |

### Example Usage
```bash
nlm --config my_config.json --log-level INFO --output results.json run --steps 1000
```

## Core Commands

### `nlm brain` - Brain Operations

Brain-related commands for managing neural simulations:

#### `nlm brain create`
Create a new brain instance with specified configuration.

**Usage:**
```bash
nlm brain create [OPTIONS] NAME
```

**Options:**
| Option | Description |
|--------|-------------|
| `--neurons N` | Number of neurons (default: 1000) |
| `--regions R` | Number of brain regions (default: 2) |
| `--seed S` | Random seed (default: 42) |
| `--config FILE` | Configuration file path |

**Examples:**
```bash
nlm brain create my_brain --neurons 5000 --seed 123
nlm brain create research_brain -c configs/large_scale.json
```

#### `nlm brain load`
Load a saved brain state from checkpoint file.

**Usage:**
```bash
nlm brain load NAME FILE
```

**Arguments:**
- `NAME`: Brain name/identifier
- `FILE`: Checkpoint file path

**Examples:**
```bash
nlm brain load saved_brain checkpoint.bin
nlm brain load production brain_state.json
```

#### `nlm brain save`
Save current brain state to checkpoint file.

**Usage:**
```bash
nlm brain save NAME FILE
```

**Arguments:**
- `NAME`: Brain name/identifier
- `FILE`: Output checkpoint file path

**Examples:**
```bash
nlm brain save my_brain brain_checkpoint.bin
nlm brain save experiment1 results.json
```

#### `nlm brain reset`
Reset brain to initial state.

**Usage:**
```bash
nlm brain reset NAME
```

**Arguments:**
- `NAME`: Brain name/identifier

**Examples:**
```bash
nlm brain reset my_brain
```

### `nlm simulation` - Simulation Control

Commands for controlling neural simulation execution:

#### `nlm simulation run`
Run a complete simulation with specified parameters.

**Usage:**
```bash
nml simulation run NAME [OPTIONS]
```

**Options:**
| Option | Description |
|--------|-------------|
| `--steps N` | Number of simulation steps (default: 1000) |
| `--timestep T` | Time step duration in ms (default: 0.001) |
| `--epochs E` | Number of training epochs (default: 1) |
| `--random-seed S` | Random seed (default: current time) |
| `--world WORLD` | World type (default: GridWorld) |

**Examples:**
```bash
nlm simulation run my_brain --steps 5000 --timestep 0.001
nlm simulation run research_brain --epochs 10 --random-seed 42
```

#### `nlm simulation pause`
Pause current simulation.

**Usage:**
```bash
nlm simulation pause NAME
```

**Arguments:**
- `NAME`: Simulation name/identifier

#### `nlm simulation resume`
Resume paused simulation.

**Usage:**
```bash
nlm simulation resume NAME
```

#### `nlm simulation status`
Show current simulation status.

**Usage:**
```bash
nlm simulation status NAME
```

**Output:**
- Current step
- Simulation time
- Neuron activity
- Memory usage
- Performance metrics

### `nlm agent` - Agent Operations

Commands for managing agent brain interfaces:

#### `nlm agent create`
Create an agent brain interface.

**Usage:**
```bash
nlm agent create NAME BRAIN_NAME [OPTIONS]
```

**Arguments:**
- `NAME`: Agent name
- `BRAIN_NAME`: Brain name to connect

**Options:**
| Option | Description |
|--------|-------------|
| `--world WORLD` | World instance name |
| `--enable-reward` | Enable reward modulation |
| `--enable-curiosity` | Enable curiosity-driven exploration |

**Examples:**
```bash
nlm agent create my_agent my_brain --enable-reward --enable-curiosity
nlm agent create research_agent research_brain --world my_world
```

#### `nlm agent connect`
Connect agent to world and environment.

**Usage:**
```bash
nlm agent connect NAME WORLD
```

**Arguments:**
- `NAME`: Agent name
- `WORLD`: World name

### `nlm world` - World Management

Commands for creating and managing simulation worlds:

#### `nlm world create`
Create a new simulation world.

**Usage:**
```bash
nlm world create NAME [OPTIONS]
```

**Options:**
| Option | Description |
|--------|-------------|
| `--type TYPE` | World type (GridWorld, ContinuousWorld, etc.) (default: GridWorld) |
| `--width W` | World width (default: 20) |
| `--height H` | World height (default: 20) |
| `--vision-width VW` | Vision field width (default: 8) |
| `--vision-height VH` | Vision field height (default: 8) |

**Examples:**
```bash
nlm world create my_world --type GridWorld --width 30 --height 30
nlm world create continuous --type ContinuousWorld --vision-width 16
```

#### `nlm world reset`
Reset world to initial state.

**Usage:**
```bash
nlm world reset NAME
```

#### `nlm world update`
Update world state for one timestep.

**Usage:**
```bash
nlm world update NAME [TIMESTEP]
```

**Arguments:**
- `NAME`: World name
- `TIMESTEP`: Optional timestep (default: 0.1)

### `nlm experiment` - Experiment Management

Commands for running and managing experiments:

#### `nlm experiment run`
Run a predefined experiment.

**Usage:**
```bash
nlm experiment run NAME [OPTIONS]
```

**Arguments:**
- `NAME`: Experiment name (e.g., phase3_demo, phase4_cognition)

**Options:**
| Option | Description |
|--------|-------------|
| `--trials N` | Number of trials (default: 10) |
| `--config FILE` | Experiment configuration file |
| `--output DIR` | Output directory for results |

**Examples:**
```bash
nlm experiment run phase3_demo --trials 50
nlm experiment run phase4_cognition --config exp_config.json
```

#### `nlm experiment list`
List available experiments.

**Usage:**
```bash
nlm experiment list
```

#### `nlm experiment info`
Show detailed information about an experiment.

**Usage:**
```bash
nlm experiment info NAME
```

### `nlm config` - Configuration Management

Commands for managing configuration files:

#### `nlm config show`
Display current configuration.

**Usage:**
```bash
nlm config show [SECTION]
```

**Arguments:**
- `SECTION`: Optional section to display (brain, plasticity, memory, etc.)

**Examples:**
```bash
nlm config show
nlm config show brain
nlm config show plasticity
```

#### `nlm config set`
Set a configuration value.

**Usage:**
```bash
nlm config set KEY VALUE
```

**Examples:**
```bash
nlm config set brain.neuron_count 5000
nlm config set plasticity.stdp.learning_rate 0.01
```

#### `nlm config load`
Load configuration from file.

**Usage:**
```bash
nlm config load FILE
```

**Examples:**
```bash
nlm config load config.json
nlm config load /path/to/experiment_config.yaml
```

### `nlm analysis` - Analysis and Reporting

Commands for analyzing simulation results:

#### `nlm analysis metrics`
Calculate performance metrics from simulation data.

**Usage:**
```bash
nlm analysis metrics FILE [OPTIONS]
```

**Arguments:**
- `FILE`: Simulation data file (JSON, binary, etc.)

**Options:**
| Option | Description |
|--------|-------------|
| `--metrics M` | Comma-separated metric list (spike_count,learning_rate, etc.) |
| `--output FILE` | Output file for metrics (default: stdout) |

**Examples:**
```bash
nlm analysis metrics simulation_data.json --metrics spike_count,learning_rate,accuracy
nlm analysis metrics results.bin --output analysis_results.csv
```

#### `nlm analysis visualize`
Generate visualizations from simulation data.

**Usage:**
```bash
nlm analysis visualize FILE [OPTIONS]
```

**Arguments:**
- `FILE`: Simulation data file

**Options:**
| Option | Description |
|--------|-------------|
| `--plot TYPE` | Plot type (spike_raster, activity_heatmap, etc.) |
| `--output FILE` | Output image file (PNG, SVG, PDF) |
| `--width W` | Plot width in pixels (default: 800) |
| `--height H` | Plot height in pixels (default: 600) |

**Examples:**
```bash
nlm analysis visualize simulation_data.json --plot spike_raster --output spike_raster.png
nlm analysis visualize results.bin --plot activity_heatmap --output heatmap.svg
```

### `nlm benchmark` - Performance Benchmarking

Commands for benchmarking NLM performance:

#### `nlm benchmark run`
Run performance benchmark with specified configuration.

**Usage:**
```bash
nlm benchmark run NAME [OPTIONS]
```

**Arguments:**
- `NAME`: Benchmark name

**Options:**
| Option | Description |
|--------|-------------|
| `--neurons N` | Number of neurons for benchmark (default: 1000) |
| `--timesteps T` | Number of timesteps (default: 1000) |
| `--regions R` | Number of regions (default: 2) |

**Examples:**
```bash
nlm benchmark run memory_benchmark --neurons 10000 --timesteps 5000
nlm benchmark run plasticity_benchmark --regions 5
```

## Command Examples

### Complete Agent Simulation
```bash
# Create brain with custom configuration
nlm brain create experimental_brain -c configs/experiment.json

# Create world
nlm world create test_world --width 25 --height 25 --vision-width 12

# Create agent and connect to world
nlm agent create experimental_agent experimental_brain --world test_world

# Run simulation
nlm simulation run experimental_brain --steps 10000 --timestep 0.001

# Save results
nlm brain save experimental_brain simulation_results.bin

# Analyze results
nlm analysis metrics simulation_results.bin --metrics spike_count,learning_rate,accuracy --output metrics.csv
```

### Phase 4 Experiment
```bash
# Run cognition experiment with specific configuration
nlm experiment run phase4_cognition --trials 100 --config exp_config.yaml

# Monitor progress
nlm simulation status phase4_cognition

# Save intermediate results
nlm brain save phase4_brain intermediate_checkpoint.bin
```

### Batch Processing
```bash
# Process multiple configurations
for config in configs/*.json; do
    name=$(basename "$config" .json)
    echo "Running experiment with $name..."
    nlm brain create "$name" -c "$config"
    nlm simulation run "$name" --steps 1000
    nlm brain save "$name" "results_$name.bin"
done
```

## Command Line Completion

Tab completion is available for all NLM commands. To enable:

### Bash
```bash
nlm completion bash > ~/.bash_completion.d/nlm
```

### Zsh
```bash
nlm completion zsh > ~/.zsh_completion.d/nlm
```

### Fish
```bash
nlm completion fish > ~/.config/fish/completions/nlm.fish
```

## Error Codes

NLM uses the following exit codes:

| Code | Description |
|------|-------------|
| `0` | Success |
| `1` | General error |
| `2` | Invalid command syntax |
| `3` | Configuration file not found |
| `4` | Simulation failed |
| `5` | Invalid arguments |
| `6` | Resource limit exceeded |

## Logging Output Format

### Standard Output
```
2026-01-15 10:30:45.123 INFO  [nlm] Simulation started
2026-01-15 10:30:46.234 DEBUG [brain] 1000 neurons initialized
2026-01-15 10:30:47.456 INFO  [agent] Agent connected to world
2026-01-15 10:31:00.789 INFO  [simulation] Completed 100 steps
```

### Error Output
```
ERROR: Failed to load configuration file "config.json"
  File: /home/user/project/configs/missing.json
  Reason: File not found
  Suggestions: Use --config to specify correct path
```

## Cross-Platform Usage

### Windows
```cmd
nlm.exe --help
nlm.exe brain create windows_brain --neurons 2000
```

### macOS/Linux
```bash
./nlm --help
./nlm brain create mac_brain --neurons 2000
```

### WSL (Windows Subsystem for Linux)
```bash
wsl$ nlm --help
wsl$ nlm brain create wsl_brain --neurons 2000
```

## Configuration Override

Command line options override configuration file values:

```bash
# Configuration file has: neuron_count = 1000
# But command line specifies:
nlm brain create my_brain --neurons 5000

# Result: 5000 neurons (command line takes precedence)
```

## Environment Variables

NLM supports the following environment variables:

| Variable | Default | Description |
|----------|---------|-------------|
| `NLM_CONFIG_PATH` | `./config.json` | Default configuration file path |
| `NLM_LOG_LEVEL` | `INFO` | Default log level |
| `NLM_TEMP_DIR` | `/tmp/nlm` | Temporary directory path |
| `NLM_DATA_DIR` | `./data` | Data directory path |

## Related Documentation

- See [Configuration.md](Configuration.md) for configuration file options
- See [API-Guide.md](API-Guide.md) for programmatic access
- See [Development-Guide.md](Development-Guide.md) for adding new commands
- See [Troubleshooting.md](Troubleshooting.md) for common issues

## Command Help

For detailed help on any command, use:

```bash
nlm <command> --help
nlm <command> <subcommand> --help
```

For example:
```bash
nlm simulation run --help
nlm brain create --help
nlm analysis metrics --help
```