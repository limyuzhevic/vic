# NLM CLI Improvements - Implementation Summary

This document summarizes the comprehensive CLI improvements implemented for the NLM (Neural Learning Machine) project.

## Overview

The NLM CLI has been completely enhanced to provide a professional-grade command-line interface with the following features:

## CLI Infrastructure

### Core Components

1. **CLI (cli.hpp, cli.cpp)** - Main CLI class that orchestrates all components
2. **ArgumentParser (cli/CLIParser.hpp, cli/CLIParser.cpp)** - Modern argument parsing with:
   - Support for both short and long options
   - Support for option values (with and without spaces)
   - Support for boolean flags
   - Support for value ranges and validation
   - Subcommand support

3. **ConfigManager (cli/ConfigManager.hpp, cli/ConfigManager.cpp)** - Advanced configuration management with:
   - Support for JSON, YAML, and XML config files
   - Configuration inheritance and merging
   - Configuration validation
   - Configuration commands (save, load, list, validate)

4. **SessionManager (cli/SessionManager.hpp, cli/SessionManager.cpp)** - Session management with:
   - Save/load session states via CLI
   - Session backup and restore
   - Session comparison and diffing
   - Session metadata management

5. **OutputFormatter (cli/OutputFormatter.hpp, cli/OutputFormatter.cpp)** - Output formatting with:
   - Multiple output formats (text, JSON, YAML, CSV)
   - Output filtering and redirection
   - Progress reporting options
   - Logging level control
   - Statistical output options

## Configuration Options

### Brain Configuration
- `brain.neuron_count` - Number of neurons
- `brain.region_count` - Number of brain regions
- `brain.connection_probability` - Connection probability
- `brain.random_seed` - Random seed
- `brain.simulation_timestep` - Simulation timestep

### Memory Configuration
- `memory.working_memory_capacity` - Working memory capacity
- `memory.episodic_memory_capacity` - Episodic memory capacity
- `memory.decay_rate` - Memory decay rate
- `memory.retrieval_threshold` - Retrieval threshold

### Plasticity Configuration
- `plasticity.learning_rate` - Plasticity learning rate
- `plasticity.stdp_ltp_weight` - STDP LTP weight
- `plasticity.stdp_ltd_weight` - STDP LTD weight
- `plasticity.stdp_time_constant` - STDP time constant
- `plasticity.hebbian_strength` - Hebbian strength

### Development Configuration
- `development.synaptogenesis_rate` - Synaptogenesis rate
- `development.pruning_rate` - Pruning rate
- `development.maturation_rate` - Maturation rate
- `development.critical_period` - Critical period enabled

### Neuromodulation Configuration
- `neuromodulation.dopamine_baseline` - Dopamine baseline
- `neuromodulation.dopamine_gain` - Dopamine gain
- `neuromodulation.curiosity_rate` - Curiosity rate
- `neuromodulation.novelty_sensitivity` - Novelty sensitivity
- `neuromodulation.reward_discount_factor` - Reward discount factor

### Performance Configuration
- `performance.optimization_level` - Optimization level
- `performance.thread_count` - Number of threads
- `performance.vectorization_enabled` - Vectorization enabled
- `performance.sparse_connectivity` - Sparse connectivity

### Output Configuration
- `output.format` - Output format (text, json, yaml, csv)
- `output.verbose` - Verbose output
- `output.quiet` - Quiet mode
- `output.progress_interval` - Progress interval
- `output.log_to_file` - Log to file
- `output.log_filename` - Log filename

### Debug Configuration
- `debug.mode` - Enable debug mode
- `debug.trace_level` - Trace level
- `debug.validate_every_step` - Validate every step

### Logging Configuration
- `logging.log_level` - Log level
- `logging.log_to_file` - Log to file
- `logging.log_filename` - Log filename
- `logging.log_append` - Append to log file

### Experiment Configuration
- `experiment.max_simulation_steps` - Maximum simulation steps
- `experiment.simulation_time_limit` - Simulation time limit
- `experiment.random_seed` - Random seed
- `experiment.checkpoint_interval` - Checkpoint interval

## CLI Commands

### Main Commands

| Command | Description | Examples |
|---------|-------------|----------|
| `--help` | Show help information | `nlm --help` |
| `--version` | Show version information | `nlm --version` |
| `--config=FILE` | Load configuration | `nlm --config=config.json` |
| `--run` | Run simulation | `nlm --run` |

### Session Commands

| Command | Description | Examples |
|---------|-------------|----------|
| `session save` | Save current state | `nlm session save --path=session.json` |
| `session load` | Load saved state | `nlm session load --path=session.json` |
| `session list` | List sessions | `nlm session list` |
| `session compare` | Compare sessions | `nlm session compare --session1=s1.json --session2=s2.json` |
| `session delete` | Delete session | `nlm session delete --path=session.json` |

### Configuration Commands

| Command | Description | Examples |
|---------|-------------|----------|
| `config save` | Save configuration | `nlm config save --path=config.json` |
| `config load` | Load configuration | `nlm config load --path=config.json` |
| `config validate` | Validate configuration | `nlm config validate` |
| `config list` | List configuration | `nlm config list` |

### Output Commands

| Command | Description | Examples |
|---------|-------------|----------|
| `--output=format=FORMAT` | Set output format | `nlm --output=format:json` |
| `--output=quiet` | Enable quiet mode | `nlm --output=quiet` |
| `--output=verbose` | Enable verbose mode | `nlm --output=verbose` |
| `--output=progress` | Enable progress reporting | `nlm --output=progress` |

## Configuration File Support

The CLI supports multiple configuration file formats:

### JSON Configuration
```json
{
  "nlm_version": "2.0",
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
  }
}
```

### YAML Configuration
```yaml
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
```

### XML Configuration
```xml
<nlm-config version="2.0">
  <brain>
    <neuron_count>1000</neuron_count>
    <region_count>2</region_count>
  </brain>
</nlm-config>
```

## Usage Examples

### Running a Simple Simulation
```bash
nlm --brain.neuron_count=500 \
    --brain.region_count=2 \
    --plasticity.learning_rate=0.01 \
    --performance.optimization_level=2 \
    --output.format=text \
    --output.verbose
```

### Running with Configuration File
```bash
nlm --config=brain_config.json \
    --run \
    --output=session.json \
    --output.format=json
```

### Session-Based Experiment
```bash
# Save initial state
nlm session save --path=initial_state.json

# Run simulation
nlm --run --brain.neurons=1000 --plasticity.learning_rate=0.01

# Save final state
nlm session save --path=final_state.json

# Compare sessions
nlm session compare --session1=initial_state.json --session2=final_state.json
```

### Batch Processing
```bash
#!/bin/bash
for config in configs/*.json; do
    echo "Running simulation with $config"
    nlm --config=$config --output=results/${config##*/}.json
    echo "Completed"
done
```

## Build Instructions

The CLI is included in the standard NLM build. After building NLM from source, you can use the `nlm` and `nlm_cli` commands.

```bash
# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run the main NLM executable
./nlm --help

# Run the CLI executable
./nlm_cli --help
```

## Key Features

1. **Backward Compatibility** - Maintains compatibility with existing CLI usage
2. **Advanced Argument Parsing** - Modern C++ argument parsing with comprehensive validation
3. **Configuration Management** - Full support for JSON, YAML, and XML configuration files
4. **Session Management** - Complete session save/load/backup/restore functionality
5. **Output Formatting** - Multiple output formats with filtering and progress reporting
6. **Performance Control** - Profiling, debugging, and performance tuning options
7. **Validation** - Comprehensive argument and configuration validation
8. **Scripting Support** - Script mode for automated execution
9. **Error Handling** - Graceful error recovery with detailed error messages
10. **Platform Support** - Cross-platform with optimizations

## Files Modified

1. **src/main.cpp** - Integrated CLI infrastructure
2. **CMakeLists.txt** - Updated build system
3. **src/CLI/** - Created new CLI components
4. **examples/** - Created example configuration files
5. **CLI.md** - Created comprehensive CLI documentation

## Testing

The CLI includes comprehensive validation:
- Argument validation
- Configuration validation
- Session comparison
- Output format validation
- Error handling

## License

The NLM CLI is part of the NLM project under the MIT License.
