# NLM Command Line Interface Reference

## Overview

The NLM (Neural Learning Machine) Command Line Interface (CLI) provides comprehensive control over neural network simulations, configuration management, and debugging capabilities.

## General Usage

### Basic Commands

```bash
nlm --help                    # Display help information
nlm --version                 # Display version information
nlm --help                     # Show help for all commands
nlm <command> --help          # Show help for specific command
```

### Configuration Management

```bash
nlm --config-file config.json     # Load configuration from file
nlm --set neuron_count=2000       # Set configuration parameter
nlm --set region_count=4          # Set another parameter
nlm --set "synapse_probability=0.15"  # Set string with special characters
```

### Brain Control

```bash
nlm save --filepath brain_state.json     # Save current brain state
nlm load --filepath brain_state.json     # Load brain state from file
nlm checkpoint --filepath checkpoint.json # Create checkpoint
nlm reset                                # Reset brain to initial state
```

### Simulation Control

```bash
nlm run --steps 1000                    # Run simulation with 1000 steps
nlm run --steps 10000 --timestep 0.001   # Run with specific timestep
nlm run --steps 5000 --duration 100.0    # Run with specific duration
nlm --steps 500 --timestep 0.0005        # Set steps and timestep
```

### Visualization and Debugging

```bash
nlm --plot --output stats.png           # Generate plot
nlm --export-stats --filepath stats.json # Export statistics
nlm debug --verbose --profile            # Enable debugging
nlm --profile --output profile.json      # Enable profiling
```

### Advanced Operations

```bash
nlm --benchmark --duration 60.0          # Run performance benchmark
nlm --validate --config config.json      # Validate configuration
nlm batch --file simulations.txt         # Process batch simulations
```

## Command Reference

### Configuration Commands

#### --config-file
Load configuration from JSON file.

**Syntax:** `nlm --config-file <file>`

**Description:**
- Loads configuration from a JSON file
- Overwrites current configuration
- Required for any simulation setup

**Example:**
```bash
nlm --config-file configs/production.json run --steps 1000
```

#### --set
Set configuration key-value pairs.

**Syntax:** `nlm --set <key>=<value>`

**Description:**
- Sets configuration parameters at runtime
- Supports different value types (string, integer, float, boolean)
- Can be used multiple times to set multiple parameters

**Examples:**
```bash
nlm --set neuron_count=2000 --set region_count=4 --set learning_rate=0.001
nlm --set "synapse_probability=0.15" --set "plasticity_enabled=true"
```

### Brain Control Commands

#### save
Save current brain state to file.

**Syntax:** `nlm save --filepath <file>`

**Options:**
- `--filepath <file>`: Output file path (default: brain_state.json)
- `--format <format>`: Output format (default: json)

**Description:**
- Saves complete brain state including neurons, synapses, and internal state
- Useful for checkpointing and resuming simulations
- Supports JSON format for easy interoperability

**Example:**
```bash
nlm save --filepath my_brain_state.json
```

#### load
Load brain state from file.

**Syntax:** `nlm load --filepath <file>`

**Options:**
- `--filepath <file>`: Input file path (default: brain_state.json)
- `--format <format>`: Input format (default: json)

**Description:**
- Restores brain state from saved file
- Resets any runtime modifications
- Used to continue previously stopped simulations

**Example:**
```bash
nlm load --filepath my_brain_state.json --steps 5000
```

#### reset
Reset brain to initial state.

**Syntax:** `nlm reset [--preserve-config]`

**Options:**
- `--preserve-config`: Keep current configuration, only reset brain state

**Description:**
- Clears all neural state and activity
- Optional configuration preservation
- Fast reset without file I/O

**Examples:**
```bash
nlm reset                                      # Reset everything
nlm reset --preserve-config                   # Reset brain only
```

#### checkpoint
Create a checkpoint of current brain state.

**Syntax:** `nlm checkpoint --filepath <file> [--description <desc>]`

**Options:**
- `--filepath <file>`: Checkpoint file path (default: checkpoint.json)
- `--description <desc>`: Human-readable description

**Description:**
- Similar to save, but specifically for simulation checkpoints
- Useful for long-running simulations
- Can include metadata in description

**Example:**
```bash
nlm checkpoint --filepath checkpoint_1000.json --description "After 1000 steps"
```

### Simulation Control Commands

#### run
Execute simulation with current configuration.

**Syntax:** `nlm run [--steps <n>] [--timestep <t>] [--duration <d>] [--async]`

**Options:**
- `--steps <n>`: Number of simulation steps (default: 100)
- `--timestep <t>`: Time step duration in seconds (default: 0.001)
- `--duration <d>`: Total simulation duration in seconds
- `--async`: Run asynchronously (background processing)

**Description:**
- Core simulation command
- Supports both step count and duration specifications
- Can be interrupted with Ctrl+C
- Shows progress for every 100 steps

**Examples:**
```bash
nlm run --steps 1000                    # Run 1000 steps
nlm run --steps 10000 --timestep 0.0005 # Run with custom timestep
nlm run --duration 10.0                # Run for 10 seconds
```

#### --steps
Set number of simulation steps (alias for run --steps).

**Syntax:** `nlm --steps <n>`

**Description:**
- Convenience wrapper for setting steps parameter
- Can be combined with other commands

**Example:**
```bash
nlm --steps 1000 run
```

#### --timestep
Set simulation time step (alias for run --timestep).

**Syntax:** `nlm --timestep <t>`

**Description:**
- Convenience wrapper for setting timestep parameter
- Affects simulation dynamics

**Example:**
```bash
nlm --timestep 0.0001 run --steps 5000
```

#### --duration
Set simulation duration (alias for run --duration).

**Syntax:** `nlm --duration <d>`

**Description:**
- Convenience wrapper for setting duration parameter
- Alternative to --steps specification

**Example:**
```bash
nlm --duration 5.0 run
```

### Visualization Commands

#### --plot
Generate visualization of simulation statistics.

**Syntax:** `nlm --plot [--output <file>] [--format <format>]`

**Options:**
- `--output <file>`: Output file for plot (default: stats.png)
- `--format <format>`: Plot format (png, svg, pdf, default: png)

**Description:**
- Generates various plots of simulation statistics
- Can be used after simulation run
- Supports multiple output formats

**Example:**
```bash
nlm --plot --output firing_rate.png --format png
```

#### --export-stats
Export simulation statistics to file.

**Syntax:** `nlm --export-stats [--filepath <file>] [--format <format>]`

**Options:**
- `--filepath <file>`: Output file path (default: stats.json)
- `--format <format>`: Export format (json, csv, txt, default: json)

**Description:**
- Exports comprehensive simulation statistics
- Supports multiple output formats for different use cases
- Includes neural activity, plasticity statistics, and performance metrics

**Examples:**
```bash
nlm --export-stats --filepath metrics.json --format json
nlm --export-stats --filepath metrics.csv --format csv
```

#### debug
Enable debug output and diagnostic tools.

**Syntax:** `nlm debug [--verbose] [--profile] [--log-file <file>]`

**Options:**
- `--verbose`: Enable verbose output
- `--profile`: Enable profiling mode
- `--log-file <file>`: Output log file

**Description:**
- Enables detailed debug information
- Useful for troubleshooting and analysis
- Can redirect output to log file

**Example:**
```bash
nlm debug --verbose --profile --log-file debug.log
```

### Advanced Commands

#### --profile
Enable profiling mode.

**Syntax:** `nlm --profile [--output <file>] [--format <format>] [--stats]`

**Options:**
- `--output <file>`: Profile output file (default: profile.json)
- `--format <format>`: Profile format (json, default: json)
- `--stats`: Include statistics in profile

**Description:**
- Enables detailed performance profiling
- Records execution times and resource usage
- Useful for performance analysis and optimization

**Example:**
```bash
nlm --profile --output profile.json --stats
```

#### --benchmark
Run performance benchmark.

**Syntax:** `nlm --benchmark [--output <file>] [--duration <d>] [--warmup <w>]`

**Options:**
- `--output <file>`: Benchmark output file (default: benchmark.json)
- `--duration <d>`: Benchmark duration in seconds (default: 60.0)
- `--warmup <w>`: Warmup duration in seconds (default: 10.0)

**Description:**
- Runs comprehensive performance benchmark
- Measures simulation throughput and efficiency
- Provides comparative performance metrics

**Example:**
```bash
nlm --benchmark --output benchmark.json --duration 120.0 --warmup 20.0
```

#### --validate
Validate configuration and brain state.

**Syntax:** `nlm --validate [--config <file>] [--brain <file>] [--strict]`

**Options:**
- `--config <file>`: Configuration file to validate
- `--brain <file>`: Brain file to validate
- `--strict`: Enable strict validation mode

**Description:**
- Validates configuration files for syntax and semantics
- Checks brain state files for consistency
- Can validate both configuration and brain files

**Examples:**
```bash
nlm --validate --config config.json
nlm --validate --config config.json --brain brain_state.json --strict
```

#### batch
Process multiple simulations from file.

**Syntax:** `nlm batch --file <file> [--parallel <n>] [--output-dir <dir>]`

**Options:**
- `--file <file>`: Batch file containing simulations
- `--parallel <n>`: Number of parallel simulations (default: 1)
- `--output-dir <dir>`: Output directory for results (default: .)

**Description:**
- Processes multiple simulations from a batch file
- Supports parallel execution for faster processing
- Useful for parameter sweeps and hyperparameter optimization

**Batch file format:**
```ini
[simulation_1]
steps = 1000
timestep = 0.001
neuron_count = 1000

[simulation_2]
steps = 2000
timestep = 0.001
neuron_count = 2000
```

**Example:**
```bash
nlm batch --file simulations.ini --parallel 4 --output-dir results/
```

## Configuration File Format

### Basic JSON Configuration

```json
{
    "neuron_count": 1000,
    "region_count": 4,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "learning_rate": 0.01,
    "plasticity_enabled": true,
    "stdp_enabled": true,
    "hebbian_enabled": true,
    "connection_probability": 0.15,
    "synapse_weight_range": [0.0, 1.0],
    "neurite_length": 10.0,
    "max_firing_rate": 100.0
}
```

### Configuration File Examples

#### Simple Configuration
```json
{
    "neuron_count": 500,
    "region_count": 2,
    "simulation_timestep": 0.001
}
```

#### Advanced Configuration
```json
{
    "neuron_count": 2000,
    "region_count": 4,
    "connection_probability": 0.2,
    "stdp_enabled": true,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "hebbian_enabled": true,
    "learning_rate": 0.01,
    "plasticity_enabled": true,
    "synapse_weight_range": [0.0, 2.0],
    "neurite_length": 15.0,
    "max_firing_rate": 200.0,
    "random_seed": 12345,
    "simulation_timestep": 0.0005
}
```

## Command Line Options

### Global Options

- `--help`, `-h`: Show help information
- `--version`: Show version information
- `--verbose`, `-v`: Enable verbose output
- `--debug`: Enable debug mode
- `--config-file <file>`, `-f <file>`: Load configuration from file

### Command-Specific Options

Each command has its own set of options. Use `nlm <command> --help` for specific command options.

## Integration with Existing Code

### Command-Line Argument Compatibility

The new CLI system is backward compatible with the existing `Config::loadFromArgs()` implementation:

```cpp
// Existing code continues to work
config->loadFromArgs(argc, argv);

// New CLI system provides enhanced functionality
auto parser = std::make_unique<CommandLineParser>();
parser->setCommandHandler(CLIFactory::createCommandHandler(config, brain));
CommandResult result = parser->parseAndExecute(argc, argv);
```

### Configuration File Support

JSON configuration files are now fully supported:

```cpp
// Load configuration from file
config->loadFromFile("config.json");

// Save configuration to file
config->saveToFile("output_config.json");
```

## Examples

### Complete Workflow

```bash
# Create and run a simulation
nlm \
  --set neuron_count=1000 \
  --set region_count=4 \
  --set learning_rate=0.01 \
  --set "plasticity_enabled=true" \
  run --steps 5000

# Save and reload simulation
nlm save --filepath my_brain.json
nlm load --filepath my_brain.json --steps 10000

# Run with profiling
nlm run --steps 2000 --profile --output profile.json

# Validate configuration
nlm --validate --config config.json

# Run batch of simulations
nlm batch --file parameter_sweep.ini --parallel 8 --output-dir results/
```

### Interactive Development

```bash
# Start with configuration file
nlm --config-file config.json debug --verbose

# Experiment with different parameters
nlm --set neuron_count=500 --set learning_rate=0.005 run --steps 1000
nlm --set neuron_count=1000 --set learning_rate=0.01 run --steps 1000

# Compare results
nlm --export-stats --filepath stats_500.json
nlm --export-stats --filepath stats_1000.json
```

## Troubleshooting

### Common Issues

1. **Configuration file not found:**
   ```bash
   nlm --config-file configs/my_config.json
   # Ensure the file exists
   ```

2. **Invalid command syntax:**
   ```bash
   nlm <command> --help
   # Get help for specific command
   ```

3. **Brain initialization failures:**
   ```bash
   nlm reset --preserve-config run --steps 100
   # Reset brain while preserving configuration
   ```

4. **Memory usage too high:**
   ```bash
   nlm --set neuron_count=500 run --steps 1000
   # Reduce neuron count for smaller simulations
   ```

### Debug Information

Enable debug mode for detailed troubleshooting:

```bash
nlm --config-file config.json debug --verbose --profile
```

## See Also

- `Config::loadFromFile()` - Load configuration from JSON file
- `Config::loadFromArgs()` - Load configuration from command line
- `Brain::save()` - Save brain state to file
- `Brain::load()` - Load brain state from file
- `Brain::reset()` - Reset brain to initial state

## Version

NLM Command Line Interface v2.0.0 (Phase 2 - Real Neural Computation)
