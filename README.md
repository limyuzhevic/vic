# NLM CLI Interface

## Overview

This directory contains the Command Line Interface (CLI) for the Neural Learning Machine (NLM) system. The CLI provides comprehensive control over NLM simulations, configurations, experiments, and monitoring.

## Files

### Core CLI Interface

- **`cli/nlm_cli.cpp`** - Main CLI interface with argument parsing and command dispatch
- **`cli/nlm_cli.hpp`** - Header file for the main CLI interface

### Configuration Management

- **`cli/config_cli.cpp`** - Configuration management commands (show, set, load, save, validate)
- **`cli/config_cli.hpp`** - Header file for config CLI

### Experiment Management

- **`cli/experiment_cli.cpp`** - Experiment management commands (run, list, info, compare, track, results)
- **`cli/experiment_cli.hpp`** - Header file for experiment CLI

### Real-time Monitoring

- **`cli/monitor_cli.cpp`** - Monitoring commands (live, replay, metrics, log, export)
- **`cli/monitor_cli.hpp`** - Header file for monitor CLI

### Test Framework

- **`tests/test_cli_framework.cpp`** - Comprehensive test suite for CLI functionality

## CLI Structure

The NLM CLI is organized into three main categories of commands:

### 1. Basic Control Commands

- `nlm run` - Run simulations with configuration options
- `nlm save <file>` - Save brain state to file
- `nlm load <file>` - Load brain state from file

### 2. Advanced Features

- `nlm config` - Configuration management
  - `show` - Display current configuration
  - `set <key> <value>` - Set configuration values
  - `load <file>` - Load configuration from file
  - `save <file>` - Save configuration to file
  - `validate` - Validate configuration against schema
  - `export <format>` - Export configuration to various formats
  - `import <format>` - Import configuration from stdin
  - `list` - List all configuration keys

- `nlm experiment` - Experiment management
  - `run` - Run experiments (Phase 3, 4, 5, or 6)
  - `list` - List registered experiments
  - `info <name>` - Show experiment information
  - `compare <name1> <name2>` - Compare two experiments
  - `track` - Track experiment progress
  - `results` - Export experiment results

- `nlm monitor` - Real-time monitoring
  - `live` - Live monitoring with interactive controls
  - `replay <file>` - Replay monitoring data
  - `metrics` - Collect and analyze metrics
  - `log` - Display log files
  - `export <format>` - Export monitoring data

### 3. Utility Commands

- `nlm benchmark` - Performance benchmarking
- `nlm profile` - Performance profiling

## Usage Examples

### Basic Simulation

```bash
# Run a simple simulation
nlm run --steps 1000 --experiment phase3

# Run with custom configuration
nlm run --config my_config.cfg --steps 5000
```

### Configuration Management

```bash
# Show current configuration
nlm config show

# Set a configuration value
nlm config set neuron_count 2000

# Load configuration from file
nlm config load my_config.json

# Save configuration to file
nlm config save my_config.json

# Validate configuration
nlm config validate
```

### Experiment Management

```bash
# Run a Phase 4 experiment
nlm experiment run --experiment phase4 --steps 10000 --output-file results.json

# List all experiments
nlm experiment list

# Show experiment details
nlm experiment info phase4_demo

# Compare two experiments
nlm experiment compare phase3 phase4
```

### Monitoring

```bash
# Start live monitoring
nlm monitor live --interval 0.1 --duration 300

# Display log file
nlm monitor log --log-file nlm.log --level info

# Export monitoring data
nlm monitor export csv --output-file monitoring.csv
```

## Building the CLI

The CLI is built as part of the main NLM project. Ensure you have:

1. **CMake 3.16 or higher**
2. **C++20 compiler**
3. **pybind11** (for Python bindings)
4. **nlohmann/json** (for JSON configuration)

### Build Instructions

1. Create a build directory:

```bash
mkdir build && cd build
```

2. Configure with CMake:

```bash
cmake ..
```

3. Build the project:

```bash
make -j4
```

## Testing

Run the CLI tests to verify functionality:

```bash
g++ -std=c++20 -I../include -I. -c tests/test_cli_framework.cpp -o test_cli_framework.o
g++ test_cli_framework.o -o test_cli_framework
./test_cli_framework
```

## Integration with Python

The NLM CLI can be used from Python via the following methods:

1. **Direct Python Integration**: Use the `subprocess` module to call CLI commands

```python
import subprocess
import json

# Run a simulation via CLI
result = subprocess.run(
    ['nlm', 'run', '--steps', '1000', '--experiment', 'phase3'],
    capture_output=True,
    text=True
)

# Save brain state via CLI
brain.save('brain.chk')  # This is done via CLI:
subprocess.run(['nlm', 'save', 'brain.chk'])

# Load brain state via CLI
brain.load('brain.chk')  # This is done via CLI:
subprocess.run(['nlm', 'load', 'brain.chk'])
```

2. **Jupyter Notebook Integration**: Use the NLM Python bindings with CLI-like interface

```python
import pynlm
import subprocess

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation steps
for i in range(1000):
    brain.step(i)

# Save via CLI if needed
subprocess.run(['nlm', 'save', 'brain.chk'])
```

## Configuration File Formats

The NLM system supports multiple configuration file formats:

### JSON

```json
{
    "random_seed": 42,
    "neuron_count": 1000,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1,
    "enable_stdp": true,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

### Legacy Format (key=value)

```
# config.cfg
random_seed=42
neuron_count=1000
region_count=1
simulation_timestep=0.001
connection_probability=0.1
enable_stdp=true
stdp_ltp_weight=0.01
stdp_ltd_weight=0.012
stdp_tau=20.0
synaptogenesis_rate=0.0001
pruning_rate=0.00001
```

## Environment Variables

The NLM CLI respects the following environment variables:

- `NLM_LOG_LEVEL` - Set the logging level (trace, debug, info, warning, error, fatal)
- `NLM_CONFIG_DIR` - Directory for configuration files
- `NLM_DATA_DIR` - Directory for data files
- `NLM_LOG_FILE` - Log file path

## Troubleshooting

### Common Issues

1. **"Command not found" error**
   - Ensure the CLI is built and installed
   - Check that the build directory contains the executable

2. **"Configuration file not found"**
   - Check that the file path is correct
   - Ensure read permissions

3. **"Unknown command"**
   - Check command syntax
   - Use `--help` for command documentation

4. **"Failed to initialize brain"**
   - Check configuration validity
   - Ensure required parameters are set

### Getting Help

- `nlm --help` - Show main help
- `nlm <command> --help` - Show help for specific command
- `nlm config --help` - Show configuration command help
- `nlm experiment --help` - Show experiment command help
- `nlm monitor --help` - Show monitor command help

## License

This CLI is part of the NLM (Neural Learning Machine) project and is licensed under the MIT License.
