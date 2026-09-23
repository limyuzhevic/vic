# NLM CLI User Guide

# NLM Brain Simulation CLI

The NLM CLI provides a comprehensive command-line interface for the NLM (Neural Learning Machine) brain simulation system. It enables users to run simulations, manage configurations, export/import brain states, and monitor performance.

## Overview

The NLM CLI is a separate executable (`nlm_cli`) that provides all the functionality requested in the Phase 2 real neural computation system. It follows modern command-line conventions with clear help messages, subcommands, and options.

## Installation

The CLI is built as part of the NLM project. To build and install:

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
make install
```

The executable will be installed to `bin/nlm_cli`.

## Basic Usage

### Run a Simple Brain Simulation

```bash
nlm_cli run --neuron-count 100 --max-steps 500
```

This runs a basic brain simulation with 100 neurons for 500 steps.

### With Configuration File

```bash
nlm_cli run --config my_config.cfg
```

Load a custom configuration file.

### Configuration Overrides

```bash
nlm_cli run --set neuron_count=200 --set timestep=0.01
```

Override configuration values from command line.

## Available Commands

### `nlm_cli run`
Run a simple brain simulation with configurable parameters.

**Options:**
- `--neuron-count, -n <n>`: Number of neurons (default: 1000)
- `--timestep, -t <t>`: Simulation timestep (default: 0.001)
- `--max-steps <n>`: Maximum simulation steps (default: 1000)
- `--output <file>`: Output file for brain state
- `--verbose, -v`: Enable verbose output
- `--log-level <level>`: Set log level (DEBUG, INFO, WARN, ERROR)

**Examples:**
```bash
nlm_cli run --neuron-count 500 --max-steps 2000
nlm_cli run -v -t 0.0001
```

### `nlm_cli experiment <name>`
Run predefined experiments.

**Available experiments:**
- `connectivity`: Test basic neural connectivity
- `plasticity`: Plasticity learning experiment
- `stdp`: STDP verification experiment
- `phase2`: Complete Phase 2 demonstration

**Options:**
- `--output <file>`: Output file for experiment results

**Examples:**
```bash
nlm_cli experiment connectivity
nlm_cli experiment plasticity --output plasticity_results.json
```

### `nlm_cli config`
Configuration management.

**Subcommands:**
- `list`: List current configuration
- `generate`: Generate default configuration file
- `validate <file>`: Validate a configuration file

**Options:**
- `--config <file>`: Configuration file to use (default: configs/default.cfg)
- `--output <file>`: Output file for generated config

**Examples:**
```bash
nlm_cli config list
nlm_cli config generate --output my_config.cfg
nlm_cli config validate custom_config.cfg
```

### `nlm_cli export <format>`
Export brain state to various formats.

**Supported formats:**
- `json`: JSON format (default)
- `xml`: XML format
- `csv`: CSV format for neural data
- `binary`: Binary format

**Options:**
- `--output <file>`: Output file for exported data
- `--brain <id>`: Specific brain region to export

**Examples:**
```bash
nlm_cli export json
nlm_cli export csv --output neurons.csv
```

### `nlm_cli import <file>`
Import brain state from a file.

**Options:**
- `--format <format>`: Format of the input file (JSON, XML, etc.)
- `--region <id>`: Region to import into

**Examples:**
```bash
nlm_cli import saved_brain.json
nlm_cli import neural_data.xml --format xml
```

### `nlm_cli profile`
Profile performance metrics.

**Options:**
- `--output <file>`: Output file for profile data (JSON)
- `--duration <seconds>`: Duration of profiling (default: 60)

**Examples:**
```bash
nlm_cli profile --output profile.json
nlm_cli profile --duration 300
```

### `nlm_cli test`
Run unit tests and integration tests.

**Options:**
- `--unit`: Run only unit tests
- `--integration`: Run only integration tests
- `--output <file>`: Output file for test results
- `--format <format>`: Output format (text, json, xml)

**Examples:**
```bash
nlm_cli test
nlm_cli test --unit --format json
```

### `nlm_cli monitor`
Monitor brain status in real-time.

**Options:**
- `--interval <seconds>`: Update interval (default: 1)
- `--output <file>`: Log file for monitoring
- `--metrics <list>`: Metrics to monitor (spikes, firing_rate, memory, etc.)

**Examples:**
```bash
nlm_cli monitor --interval 0.5
nlm_cli monitor --metrics spikes,firing_rate --output monitor.log
```

### `nlm_cli benchmark`
Run performance benchmarks.

**Options:**
- `--output <file>`: Output file for benchmark results
- `--scale <factor>`: Scale factor for benchmark (1=small, 2=medium, 3=large)
- `--iterations <n>`: Number of iterations (default: 10)

**Examples:**
```bash
nlm_cli benchmark
nlm_cli benchmark --scale 2 --iterations 20
```

## Configuration File Format

NLM uses a simple configuration file format:

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
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0
```

## Data Export Formats

### JSON Format
The JSON format includes all neural state information in a structured format:
- Neuron properties (ID, type, membrane potential, threshold)
- Synapse connections (pre-synaptic, post-synaptic, weight)
- Spike times and activity
- Regional metadata

### CSV Format
The CSV format is optimized for analysis:
- `neuron_id,neuron_type,membrane_potential,threshold,region_id`
- `synapse_id,pre_neuron_id,post_neuron_id,weight,plasticity_enabled`
- `spike_times,neuron_id,step,neuron_type`

### XML Format
The XML format provides hierarchical data:
- `<brain>` root element
- `<region>` elements with `<neuron>` and `<synapse>` children
- `<spike>` elements for spike recording

## Configuration Management

### Generate Default Configuration

```bash
nlm_cli config generate --output custom.cfg
```

Generates a default configuration file with all necessary parameters.

### Validate Configuration

```bash
nlm_cli config validate invalid_config.cfg
```

Validates a configuration file and reports any issues.

## Development and Testing

### Running Tests

```bash
nlm_cli test
```

Runs all unit and integration tests. For more options:

```bash
nlm_cli test --help
```

### Profiling

To profile memory usage:

```bash
nlm_cli profile --output memory_profile.json
```

To profile performance:

```bash
nlm_cli profile --output performance_profile.json
```

## Examples

### Simple Usage Pattern for Beginners

```bash
# Create a simple brain with default settings
nlm_cli run --neuron-count 50 --max-steps 100

# Monitor the brain
nlm_cli monitor --interval 0.5

# Export results
nlm_cli export json --output results.json
```

### Advanced Usage for Developers

```bash
# Run a complex experiment with custom configuration
nlm_cli experiment phase2 --config configs/complex.cfg

# Profile performance for a specific workload
nlm_cli benchmark --scale 2 --iterations 5

# Import and continue a previous session
nlm_cli import saved_session.json
nlm_cli run --set max_simulation_steps=1000
```

### Script Integration Examples

**Bash Script Example:**
```bash
#!/bin/bash

# Run a simulation and export results
simulation_id=$(date +%s)

echo "Starting NLM simulation $simulation_id"
nlm_cli run --neuron-count 100 --max-steps 500

if [ $? -eq 0 ]; then
    echo "Simulation successful, exporting results..."
    nlm_cli export json --output simulation_${simulation_id}.json
    echo "Simulation ${simulation_id} completed successfully"
else
    echo "Simulation failed"
    exit 1
fi
```

**Python Script Example:**
```python
#!/usr/bin/env python3
import subprocess
import json
import os

def run_nlm_simulation(neuron_count=100, max_steps=500, config_file=None):
    """Run an NLM simulation via CLI"""
    cmd = ["nlm_cli", "run", f"--neuron-count={neuron_count}", f"--max-steps={max_steps}"]
    
    if config_file:
        cmd.extend(["--config", config_file])
    
    print(f"Running command: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        print("Simulation completed successfully")
        return result.stdout
    else:
        print(f"Simulation failed: {result.stderr}")
        return None

if __name__ == "__main__":
    output = run_nlm_simulation()
    if output:
        print("Simulation output:")
        print(output)
```

## Common Issues and Solutions

### "Command not found: nlm_cli"

**Solution:** Install the NLM package or build from source:
```bash
make install
# or
cmake --install build
```

### "Failed to load configuration file"

**Solution:** Check that the configuration file exists and is readable:
```bash
ls -la configs/default.cfg
```

### "Configuration validation failed"

**Solution:** Fix configuration errors and retry:
```bash
nlm_cli config validate your_config.cfg
```

### Performance Issues

**Solution:** Reduce the number of neurons or increase timestep:
```bash
nlm_cli run --neuron-count 50 --timestep 0.01
```

## See Also

- `how_to_use.md`: Python API documentation
- `docs/`: Additional documentation
- `README.md`: Project overview

## License

This CLI is part of the NLM (Neural Learning Machine) project. See LICENSE for details.

EOF