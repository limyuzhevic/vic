# NLM CLI Documentation
# =========================

## Overview

This document provides comprehensive documentation for the NLM Command Line Interface (nlmc), a suite of tools for managing, running, and analyzing NLM (Neural Learning Machine) simulations.

The nlmc command-line tool provides common operations for NLM users, including:

- Creating and initializing brains
- Running simulations with different configurations
- Comparing configuration files
- Benchmarking performance
- Analyzing brain statistics
- Creating experimental configurations
- Listing available configurations

## Installation

The NLM CLI is automatically installed with the NLM Python package. After installing pynlm, you should have the `nlmc` command available in your PATH.

```bash
pip install pynlm
```

## Basic Usage

```bash
# Show help
./nlmc

# Show help for a specific command
./nlmc create-brain

# Enable verbose logging
./nlmc -v create-brain --neuron-count 2000
```

## Commands

### create-brain

Create and initialize a new brain.

**Usage:**
```bash
./nlmc create-brain [OPTIONS] NEURON_COUNT
```

**Options:**
- `--config-file, -c TEXT` - Configuration file path
- `--output-file, -o TEXT` - Output file for brain configuration
- `--help` - Show help message

**Examples:**
```bash
# Create brain with default settings
./nlmc create-brain 1000

# Create brain with custom configuration
./nlmc create-brain 2000 --config-file configs/high_perf.cfg

# Save brain configuration to file
./nlmc create-brain 500 --output-file my_brain.json

# Create brain with verbose logging
./nlmc -v create-brain 1000
```

### run-sim

Run simulations with different configurations.

**Usage:**
```bash
./nlmc run-sim CONFIG_FILE [OPTIONS]
```

**Options:**
- `--steps, -s INTEGER` - Number of simulation steps (default: 1000)
- `--output-file, -o TEXT` - Output file for simulation results
- `--help` - Show help message

**Examples:**
```bash
# Run simulation with default config
./nlmc run-sim configs/default.cfg

# Run simulation for 5000 steps
./nlmc run-sim configs/development.cfg --steps 5000

# Save simulation results
./nlmc run-sim configs/experimental.cfg --steps 200 --output-file results.json
```

### compare-configs

Compare two configuration files.

**Usage:**
```bash
./nlmc compare-configs CONFIG_FILE1 CONFIG_FILE2
```

**Examples:**
```bash
# Compare two configuration files
./nlmc compare-configs configs/default.cfg configs/high_perf.cfg

# Compare different config types
./nlmc compare-configs configs/benchmark.cfg configs/lightweight.cfg
```

### benchmark

Benchmark performance of a configuration.

**Usage:**
```bash
./nlmc benchmark CONFIG_FILE [OPTIONS]
```

**Options:**
- `--steps, -s INTEGER` - Number of steps per iteration (default: 100)
- `--iterations, -i INTEGER` - Number of iterations (default: 5)
- `--help` - Show help message

**Examples:**
```bash
# Benchmark with default settings
./nlmc benchmark configs/benchmark.cfg

# Benchmark with more iterations for better accuracy
./nlmc benchmark configs/high_perf.cfg --iterations 10 --steps 200
```

### analyze-stats

Analyze brain statistics from a saved brain file.

**Usage:**
```bash
./nlmc analyze-stats BRAIN_FILE
```

**Examples:**
```bash
# Analyze brain statistics
./nlmc analyze-stats my_brain.json
```

### create-experiment

Create experimental configurations based on a base configuration.

**Usage:**
```bash
./nlmc create-experiment BASE_CONFIG NAME [OPTIONS]
```

**Options:**
- `--parameters TEXT` - Parameter modifications (format: key=value)
- `--output-dir, -o TEXT` - Output directory for experiment configs
- `--help` - Show help message

**Examples:**
```bash
# Create experiment with base config
./nlmc create-experiment configs/default.cfg my_experiment

# Create experiment with parameter modifications
./nlmc create-experiment configs/default.cfg test_exp --parameters neuron_count=1500 connection_probability=0.12

# Save experiment config to directory
./nlmc create-experiment configs/benchmark.cfg performance_test --output-dir ./experiments
```

### list-configs

List available configurations.

**Usage:**
```bash
./nlmc list-configs [OPTIONS]
```

**Options:**
- `--pattern, -p TEXT` - Pattern to filter configuration files
- `--help` - Show help message

**Examples:**
```bash
# List all configurations
./nlmc list-configs

# List only high-performance configurations
./nlmc list-configs --pattern high

# List only benchmark configurations
./nlmc list-configs --pattern benchmark
```

## Global Options

### Verbosity and Output Control

**Verbose Mode:**
```bash
./nlmc -v create-brain 1000
```

**Quiet Mode:**
```bash
./nlmc --quiet benchmark configs/default.cfg
```

**Both Verbose and Quiet (conflicting, quiet takes precedence):**
```bash
./nlmc -v --quiet benchmark configs/default.cfg
```

## Examples Directory

The examples directory contains several example scripts demonstrating different NLM use cases:

### example1_basic_brain.py
- Basic brain creation and initialization
- Simple simulation with periodic statistics

### example2_agent_simulation.py
- Complete agent simulation with brain and world
- Full integration test of all systems

### example3_development_learning.py
- Development and learning simulation
- Tracking developmental stages over time

### example4_experiment_management.py
- Configuration management and experiment tracking
- Creating and running experiments systematically

### example5_benchmarking.py
- Benchmarking and performance analysis
- Finding optimal configurations
- Visualizing benchmark results

To run an example:
```bash
cd examples
python example2_agent_simulation.py
```

## Configuration Templates

The configs directory contains example configuration files for different use cases:

### high_perf.cfg
- Optimized for speed and efficiency
- Large brain (2000 neurons)
- High connectivity and plasticity

### development.cfg
- Optimized for learning and development
- Aggressive plasticity settings
- Development stage tracking

### lightweight.cfg
- Optimized for minimal resource usage
- Small brain (300 neurons)
- Minimal feature set

### experimental.cfg
- Designed for testing specific hypotheses
- Extreme parameter values
- Detailed activity recording

### benchmark.cfg
- Designed for performance testing
- Standard brain size for comparison
- Benchmark-specific settings

## Error Handling and Validation

The nlmc tool provides comprehensive error handling and validation:

### Common Error Types

1. **File Not Found Errors:** When configuration or brain files are not found
2. **Invalid Parameters:** When numerical parameters are out of range
3. **Configuration Parsing Errors:** When configuration files cannot be parsed
4. **Simulation Errors:** When simulation fails to complete

### Error Messages

Error messages are designed to be:

- Clear and descriptive
- Include relevant context
- Suggest possible solutions

**Example Error Message:**
```
ERROR: Configuration file 'nonexistent.cfg' not found
   - File: /path/to/configs/nonexistent.cfg
   - Please ensure the file exists and is accessible
   - Use 'nlmc list-configs' to see available configurations
```

### Validation

The tool validates parameters before execution:

- Neuron count must be a positive integer
- Simulation steps must be positive
- Configuration files must be accessible
- Output directories must be writable

## Logging

The nlmc tool uses Python's standard logging framework with the following log levels:

- **DEBUG:** Detailed debug information (enabled with -v)
- **INFO:** General information (default)
- **WARNING:** Warning messages
- **ERROR:** Error messages

Log output includes:

- Timestamp
- Log level
- Message source
- Message content

**Example Log Output:**
```
2026-10-01 06:57:37 - nlm_cli - INFO - Creating brain with 1000 neurons
2026-10-01 06:57:37 - nlm_cli - INFO - Brain creation completed successfully
```

## Scripting and Automation

The nlmc tool is designed to be easily scripted for automation:

### Basic Script Example

```bash
#!/bin/bash

# Create and initialize a brain
./nlmc create-brain 1000 --output-file brain.json

# Run a simulation
./nlmc run-sim brain.json --steps 500 --output-file results.json

# Benchmark the results
./nlmc benchmark brain.json --iterations 3

# Analyze the results
./nlmc analyze-stats results.json

# Create an experiment for further testing
./nlmc create-experiment brain.json test_exp --parameters neuron_count=1500
```

### Python Integration

You can also integrate nlmc functionality with Python scripts:

```python
import subprocess
import json

def run_nlm_command(command, args):
    """Run an nlmc command and return results."""
    cmd = [command] + args
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode, result.stdout, result.stderr

# Create a brain
returncode, stdout, stderr = run_nlm_command('./nlmc', ['create-brain', '1000'])
if returncode == 0:
    print("Brain created successfully")
```

## Performance Tips

1. **Use Lightweight Configurations:** For quick testing, use `lightweight.cfg`
2. **Benchmark Before Scaling:** Use `benchmark.cfg` to find optimal settings
3. **Use Quiet Mode:** For scripts, use `--quiet` to suppress output
4. **Parallel Processing:** Run multiple simulations in parallel using `&`
5. **Cache Results:** Use `--output-file` to save and reuse results

## Troubleshooting

### Common Issues and Solutions

1. **"Command not found: nlmc"**
   ```bash
   # Add nlmc to your PATH
   export PATH="$PATH:/path/to/nlmc"
   ```

2. **Python import errors**
   ```bash
   pip install pynlm
   ```

3. **Configuration parsing errors**
   ```bash
   # Check file format
   cat configs/default.cfg
   ```

4. **Permission denied**
   ```bash
   chmod +x nlmc
   ```

## Advanced Features

### Custom Configuration Templates

You can create your own configuration templates by copying and modifying the existing ones:

```bash
cp configs/benchmark.cfg configs/my_custom_config.cfg
# Edit my_custom_config.cfg with your parameters
```

### Extensibility

The nlmc tool is designed to be extensible. You can:

1. Add custom commands by modifying the NLMCLI class
2. Create custom configuration parsers
3. Add new file formats
4. Integrate with external tools

### API Integration

The nlmc tool can be used in conjunction with:

- Python scripts using subprocess
- Jupyter notebooks
- CI/CD pipelines
- Automated testing frameworks

## Contact and Support

For questions, issues, or feature requests:

- Check the NLM documentation
- Review the examples in the examples directory
- Consult the configuration templates in the configs directory
- Join the NLM community forums

## Version Information

This documentation applies to NLM CLI version 1.0

Created as part of the NLM Command Line Tools project.
All rights reserved under MIT License.