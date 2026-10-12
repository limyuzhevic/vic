# NLM (熙然) - Neural Learning Machine CLI Usage Examples
# Comprehensive Guide to Using the Enhanced CLI

This document provides comprehensive examples and usage patterns for the enhanced NLM CLI interface.

## Table of Contents

1. [Getting Started with CLI](#getting-started-with-cli)
2. [Basic Simulation Commands](#basic-simulation-commands)
3. [Experiment Commands](#experiment-commands)
4. [Configuration Management](#configuration-management)
5. [Analysis and Benchmarking](#analysis-and-benchmarking)
6. [Scripting and Automation](#scripting-and-automation)
7. [Advanced Usage Patterns](#advanced-usage-patterns)
8. [Troubleshooting](#troubleshooting)

## Getting Started with CLI

### Quick Start

The enhanced NLM CLI provides a powerful command-line interface for running neural network simulations. Here are the essential commands to get started:

```bash
# Show help information
nlm --help

# List available presets
nlm --list-presets

# Run a basic simulation
nlm run --type basic --steps 100

# Run a plasticity experiment
nlm experiment --type plasticity --steps 1000
```

### Basic Command Structure

All NLM CLI commands follow the pattern:

```bash
nlm [global-options] <command> [command-options] [arguments]
```

**Global Options:**
- `--help, -h`: Show help information
- `--version, -v`: Show version information
- `--list-presets`: List available configuration presets
- `--verbose=<level>`: Set verbosity level (quiet, error, warning, info, debug, trace)
- `--config=<file>`: Load configuration from file
- `--output=<file>`: Save output to file
- `--log=<file>`: Save log to file

**Commands:**
- `run`: Run simulation
- `configure`: Manage configuration
- `experiment`: Run experiment
- `analyze`: Analyze results
- `benchmark`: Performance benchmarking

## Basic Simulation Commands

### Running Basic Simulations

The `run` command executes neural network simulations with different types:

```bash
# Basic neuron dynamics
nlm run --type basic --steps 100 --neurons 500

# Plasticity learning simulation
nlm run --type plasticity --steps 1000 --neurons 1000

# Spike-timing-dependent plasticity (STDP) verification
nlm run --type stdp --steps 500

# Developmental simulation
nlm run --type development --steps 5000 --neurons 800

# Cognitive simulation with attention
nlm run --type cognitive --steps 10000 --neurons 2000

# Social interaction simulation
nlm run --type social --steps 3000 --regions 3

# Memory consolidation simulation
nlm run --type memory --steps 20000 --neurons 3000

# Attention focus simulation
nlm run --type attention --steps 2000 --neurons 1200
```

### Run Command Options

```bash
nlm run [options]

Options:
  --type <type>              Simulation type: basic, plasticity, stdp, development, cognitive, social, memory, attention
  --steps <n>                Number of simulation steps
  --neurons <n>              Number of neurons
  --regions <n>              Number of brain regions
  --preset <preset>          Configuration preset (basic, plasticity, development, cognitive, social, memory, attention)
  --output <file>            Output file for results
  --format <format>          Output format: text, json, csv
  --verbose <level>          Verbosity level: quiet, error, warning, info, debug, trace
```

## Experiment Commands

### Running Experiments

The `experiment` command runs specialized neural network experiments:

```bash
# Plasticity learning experiment
nlm experiment --type plasticity --steps 5000 --neurons 1000

# Development experiment with maturation
nlm experiment --type development --steps 10000 --neurons 800

# Social learning experiment
nlm experiment --type social --steps 8000 --regions 3

# Memory formation experiment
nlm experiment --type memory --steps 15000 --neurons 2000

# Attention training experiment
nlm experiment --type attention --steps 3000 --neurons 1500
```

### Experiment Command Options

```bash
nlm experiment [options]

Options:
  --type <type>              Experiment type: plasticity, development, cognitive, social, memory, attention
  --steps <n>                Number of experiment steps
  --neurons <n>              Number of neurons
  --regions <n>              Number of brain regions
  --preset <preset>          Configuration preset
  --output <file>            Output file for experimental results
  --verbose <level>          Verbosity level
```

## Configuration Management

### Managing Configuration

The `configure` command manages configuration files and settings:

```bash
# Show current configuration
nlm configure --show

# Save current configuration to file
nlm configure --save my_config.json

# Load configuration from file
nlm run --config my_config.json --type basic

# Reset to default configuration
nlm configure --reset

# Show available presets
nlm --list-presets

# Apply a preset and save
nlm configure --preset cognitive --save cognitive_config.json
```

### Configuration Management Options

```bash
nlm configure [options]

Options:
  --show                     Show current configuration
  --save <file>              Save configuration to file
  --reset                    Reset to default configuration
  --load <file>              Load configuration from file
```

## Analysis and Benchmarking

### Analyzing Results

The `analyze` command analyzes simulation results and statistics:

```bash
# Analyze simulation results in JSON format
nlm analyze --format json --output analysis_results.json

# Analyze with text output
nlm analyze --format text --output analysis.txt

# Analyze with CSV output for spreadsheet import
nlm analyze --format csv --output analysis.csv

# Analyze specific experiment results
nlm analyze --format json --output exp_analysis.json
```

### Benchmarking

The `benchmark` command performs performance benchmarking:

```bash
# Benchmark simulation performance for 60 seconds
nlm benchmark --duration 60 --neurons 500

# Benchmark with different neuron counts
nlm benchmark --duration 30 --neurons 1000

# Benchmark development simulation
nlm benchmark --duration 120 --type development

# Benchmark with specific configuration
nlm benchmark --duration 60 --preset cognitive --neurons 2000
```

### Analysis and Benchmarking Options

```bash
nlm analyze [options]

Options:
  --format <format>          Output format: text, json, csv
  --output <file>            Output file for results
  --filter <category>        Filter results by category
  --verbose <level>          Verbosity level

nlm benchmark [options]

Options:
  --duration <sec>           Benchmark duration in seconds
  --steps <n>                Number of steps to benchmark
  --neurons <n>              Number of neurons
  --type <type>              Simulation type
  --output <file>            Output file for benchmark results
```

## Scripting and Automation

### Python Batch Runner

The NLM CLI includes Python scripts for automation and batch processing:

```bash
# Run batch of simulations
python scripts/batch_run.py --config-dir configs/ --output-dir results/ --preset basic

# Run parameter sweep
python scripts/parameter_sweep.py --param neurons:500,1000,1500 --output sweep_results.csv

# Run experiment suite
python scripts/experiment_runner.py --experiments plasticity,development,cognitive --output-dir exp_results/
```

### Example Batch Script

```python
#!/usr/bin/env python3

"""
NLM Batch Runner Script
Runs multiple simulations with different configurations.
"""

import argparse
import subprocess
import os
import json
from pathlib import Path

def run_simulation(preset, neurons, steps, output_dir):
    """Run a single simulation with specified parameters."""
    output_file = os.path.join(output_dir, f"{preset}_{neurons}_{steps}.json")
    
    cmd = [
        "nlm", "run",
        "--type", preset,
        "--steps", str(steps),
        "--neurons", str(neurons),
        "--output", output_file,
        "--verbose", "info"
    ]
    
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        print(f"✓ Completed: {output_file}")
        return True
    else:
        print(f"✗ Failed: {output_file}")
        print(f"Error: {result.stderr}")
        return False

def main():
    parser = argparse.ArgumentParser(description="Run batch of NLM simulations")
    parser.add_argument("--presets", nargs="+", default=["basic", "plasticity"], 
                       help="Presets to run")
    parser.add_argument("--neurons", nargs="+", type=int, default=[500, 1000],
                       help="Neuron counts to test")
    parser.add_argument("--steps", nargs="+", type=int, default=[100, 500],
                       help="Step counts to test")
    parser.add_argument("--output-dir", default="batch_results",
                       help="Output directory for results")
    
    args = parser.parse_args()
    
    # Create output directory
    os.makedirs(args.output_dir, exist_ok=True)
    
    # Run simulations
    total = len(args.presets) * len(args.neurons) * len(args.steps)
    completed = 0
    
    for preset in args.presets:
        for neurons in args.neurons:
            for steps in args.steps:
                success = run_simulation(preset, neurons, steps, args.output_dir)
                if success:
                    completed += 1
    
    print(f"\nBatch completed: {completed}/{total} simulations successful")

if __name__ == "__main__":
    main()
```

### Example Parameter Sweep Script

```python
#!/usr/bin/env python3

"""
NLM Parameter Sweep Script
Runs simulations with varying parameters to find optimal configurations.
"""

import argparse
import subprocess
import json
import csv
import os
from pathlib import Path

def run_parameter_sweep(base_config, param_values, output_file):
    """Run parameter sweep with specified parameter values."""
    results = []
    
    with open(output_file, 'w', newline='') as csvfile:
        fieldnames = ['parameters', 'neurons', 'steps', 'spikes', 'firing_rate', 'success']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        
        for param_value in param_values:
            config = base_config.copy()
            config.update(param_value)
            
            # Run simulation
            cmd = [
                "nlm", "run",
                "--type", config['type'],
                "--steps", str(config['steps']),
                "--neurons", str(config['neurons']),
                "--verbose", "quiet"  # Suppress output for automated processing
            ]
            
            print(f"Running with parameters: {param_value}")
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            # Parse results (simplified - actual parsing depends on output format)
            result_data = {
                'parameters': json.dumps(param_value),
                'neurons': config['neurons'],
                'steps': config['steps'],
                'spikes': 0,  # This would be extracted from actual output
                'firing_rate': 0.0,
                'success': result.returncode == 0
            }
            
            writer.writerow(result_data)
            results.append(result_data)
    
    return results

def main():
    parser = argparse.ArgumentParser(description="Run parameter sweep for NLM")
    parser.add_argument("--param", nargs="+", action='append',
                       help="Parameter values to test (format: key=value)")
    parser.add_argument("--output", default="parameter_sweep.csv",
                       help="Output CSV file for sweep results")
    parser.add_argument("--config-file", default="configs/default.cfg",
                       help="Base configuration file")
    
    args = parser.parse_args()
    
    # Parse parameter values
    param_values = []
    if args.param:
        for param_list in args.param:
            param_dict = {}
            for item in param_list:
                if '=' in item:
                    key, value = item.split('=', 1)
                    param_dict[key] = value
            if param_dict:
                param_values.append(param_dict)
    
    # Default base configuration
    base_config = {
        'type': 'plasticity',
        'steps': 1000,
        'neurons': 1000
    }
    
    # Run parameter sweep
    results = run_parameter_sweep(base_config, param_values, args.output)
    
    print(f"\nParameter sweep completed: {len(results)} configurations tested")
    print(f"Results saved to: {args.output}")

if __name__ == "__main__":
    main()
```

## Advanced Usage Patterns

### Using Configuration Files

Create custom configuration files to save and reuse simulation settings:

```bash
# Create a custom configuration file
cat > my_simulation.cfg << EOF
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1500
region_count = 3
connection_probability = 0.12
plasticity_learning_rate = 0.02
plasticity_stdp_enable = true
plasticity_hebbian_enable = true
max_simulation_steps = 5000
EOF

# Use the custom configuration
nlm run --config my_simulation.cfg --type plasticity
```

### Combining Commands

Chain multiple commands for complex workflows:

```bash
# Configure, run, and analyze in one pipeline
nlm configure --preset cognitive --save cognitive_config.json && \
nlm run --config cognitive_config.json --type cognitive --steps 5000 && \
nlm analyze --format json --output cognitive_analysis.json
```

### Verbosity Levels

Control output detail with different verbosity levels:

```bash
# Quiet - only errors and important messages
nlm run --type basic --verbose quiet

# Error - only errors
nlm run --type basic --verbose error

# Warning - warnings and errors
nlm run --type basic --verbose warning

# Info - information messages (default)
nlm run --type basic --verbose info

# Debug - detailed debug information
nlm run --type basic --verbose debug

# Trace - all available information
nlm run --type basic --verbose trace
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Command not found**
```
Solution: Ensure NLM is installed and in your PATH
```

**Issue: Configuration file not found**
```
Solution: Use full path or ensure file is in working directory
```

**Issue: Simulation takes too long**
```
Solution: Reduce neuron count or steps, or use less complex preset
```

**Issue: Memory usage too high**
```
Solution: Use smaller presets or increase available memory
```

**Issue: Output file permissions**
```
Solution: Check file permissions or specify different output location
```

### Getting More Help

- `nlm --help`: Show general help information
- `nlm help run`: Get help for run command
- `nlm help experiment`: Get help for experiment command
- `nlm --list-presets`: List all available presets
- `nlm help preset`: Get information about a specific preset

## Conclusion

The enhanced NLM CLI provides a comprehensive set of tools for running neural network simulations, experiments, and analysis. With its flexible command structure, configuration management, and scripting support, it enables users to efficiently explore and utilize NLM's capabilities for a wide range of applications.

For more advanced usage, refer to the Python scripts in the scripts/ directory and the example configuration files in the configs/ directory. The CLI system is designed to be both powerful for complex workflows and simple for basic use cases.