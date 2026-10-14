# NLM CLI - Command Line Interface

This document describes the NLM command-line interface (CLI) for controlling the Neural Learning Machine system.

## Overview

The NLM CLI provides a user-friendly interface for controlling the neural simulation with various commands:

- **run**: Execute simulation with specified parameters
- **save**: Save brain state to file
- **load**: Load brain state from file
- **info**: Display system and brain information
- **help**: Show help information and usage

## Commands

### run

**Usage:**
```bash
./nlm [--config FILE] run [OPTIONS]
```

**Description:** Run a neural simulation with specified parameters.

**Options:**
- `--neurons N`: Number of neurons (default: 500)
- `--timestep T`: Simulation timestep in seconds (default: 0.001)
- `--seed S`: Random seed for reproducibility (default: 42)
- `--steps N`: Number of simulation steps (default: 100)
- `--region-count N`: Number of brain regions (default: 1)
- `--conn-prob P`: Connection probability (default: 0.15)
- `--max-steps N`: Maximum simulation steps (default: 10000)

**Examples:**
```bash
# Run a basic simulation
./nlm run --neurons 1000 --steps 1000

# Run with custom configuration
./nlm --config myconfig.cfg run --neurons 500 --timestep 0.01

# Run with seed for reproducibility
./nlm run --seed 12345 --neurons 200
```

### save

**Usage:**
```bash
./nlm save [FILE]
```

**Description:** Save the current brain state to a checkpoint file.

**Arguments:**
- `FILE`: Output file path (default: brain_state.checkpoint)

**Examples:**
```bash
# Save brain state to default file
./nlm save

# Save brain state to custom file
./nlm save my_brain.checkpoint
```

### load

**Usage:**
```bash
./nlm load FILE
```

**Description:** Load a brain state from a checkpoint file.

**Arguments:**
- `FILE`: Input file path

**Examples:**
```bash
./nlm load saved_brain.checkpoint
```

### info

**Usage:**
```bash
./nlm info
```

**Description:** Display information about the current brain system and configuration.

**Examples:**
```bash
./nlm info
```

### help

**Usage:**
```bash
./nlm help [COMMAND]
```

**Description:** Show help information.

**Arguments:**
- `COMMAND`: Specific command to get help for (optional)

**Examples:**
```bash
# Show general help
./nlm help

# Show help for run command
./nlm help run
```

## Configuration File Support

### Using Configuration Files

The NLM CLI supports loading configuration from external files using the `--config` option:

**Example config file (`myconfig.cfg`):**
```
# NLM Configuration File
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

**Usage:**
```bash
# Load configuration from file and run simulation
./nlm --config myconfig.cfg run

# Override configuration with command-line arguments
./nlm --config myconfig.cfg run --neurons 2000 --timestep 0.005
```

## Command Line Argument Parsing

### Configuration File
```bash
--config FILE    Load configuration from FILE (default: configs/default.cfg)
```

### Command-Specific Options

#### run command
- `--neurons N`: Set number of neurons
- `--timestep T`: Set simulation timestep
- `--seed S`: Set random seed
- `--steps N`: Set number of simulation steps
- `--region-count N`: Set number of regions
- `--conn-prob P`: Set connection probability
- `--max-steps N`: Set maximum simulation steps

#### save command
- `FILE`: Output file path (optional, defaults to brain_state.checkpoint)

#### load command
- `FILE`: Input file path (required)

### Common Options
- `--help`: Show help information

## Configuration File Format

The NLM configuration file uses a simple key=value format:

```
# Key-value pairs with optional comments
key = value

# Values can be:
# - Integers: 42
# - Floats: 3.14
# - Strings: "hello world"
# - Booleans: true/false
# - Lists: [1, 2, 3]
```

## Brain State Serialization

The NLM system supports saving and loading brain states in checkpoint format:

### Save Command
The `save` command saves the current brain state, including:
- Neuron states (membrane potential, firing thresholds)
- Synapse weights and connections
- Brain metadata (neuron count, region count, etc.)
- Simulation progress (current step, current time)

### Load Command
The `load` command restores a previously saved brain state, allowing you to:
- Resume simulations from saved checkpoints
- Load trained neural networks
- Share brain states between instances

## Examples

### Basic Usage
```bash
# Run a simple simulation
./nlm run

# Save the brain state
./nlm save my_brain.checkpoint

# Load the brain state and continue
./nlm load my_brain.checkpoint
```

### Advanced Usage
```bash
# Run with custom configuration and parameters
./nlm --config config.advanced.cfg run \
    --neurons 2000 \
    --timestep 0.005 \
    --seed 12345 \
    --steps 5000

# Save detailed state information
./nlm save --format detailed --compress fast

# Load and run analysis
./nlm load trained_brain.checkpoint \
    --neurons 1000 \
    --steps 1000
```

### Configuration Override
```bash
# Load from config file but override specific parameters
./nlm --config base_config.cfg run \
    --neurons 500 \
    --region-count 2 \
    --conn-prob 0.2
```

## Error Handling

The CLI provides informative error messages for:

- Invalid commands or options
- Missing required arguments
- Configuration file not found or malformed
- Invalid parameter values
- File I/O errors during save/load operations

## Logging

The NLM CLI uses the same logging system as the rest of the NLM project. Log messages are written to:

- Console (by default)
- Log file (if configured in the config file)

Log levels can be controlled through the configuration file using the `log_level` setting.

## Integration

The NLM CLI integrates with the existing NLM systems:

- Uses the `nlm::Config` class for configuration management
- Integrates with `nlm::Brain` for brain state management
- Supports all NLM configuration options
- Maintains compatibility with existing NLM workflows

## Building

To build the NLM CLI, follow the existing build system:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

The CLI executable will be named `nlm` and will be located in the build directory.

## Testing

Test the CLI functionality with:

```bash
# Run the integration tests
./test_cli_integration

# Run the CLI tests
./test_cli
```

Or run the existing test suite:
```bash
./nlm_phase6_demo    # Phase 6 integration test
```

## Compatibility

The NLM CLI is designed to be compatible with:

- Existing NLM configuration files
- Existing brain state checkpoint files
- Existing NLM project structure
- Existing NLM test infrastructure

All existing NLM functionality remains intact; the CLI provides additional control options.
