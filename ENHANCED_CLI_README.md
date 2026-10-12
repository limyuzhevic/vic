# Enhanced NLM Command-Line Interface

## Overview

This document provides a comprehensive overview of the enhanced NLM (Neural Learning Machine) Command-Line Interface (CLI), which was implemented as part of Phase 6 (Emerging Cognition) to address the limitations of the basic CLI and provide advanced functionality for complex use cases.

## What's New

The enhanced NLM CLI provides:

- **Rich command-line interface with subcommands**
- **Configuration presets for common use cases**
- **Scripting support for batch operations**
- **Help system with detailed documentation**
- **Logging configuration and verbosity control**
- **Progress monitoring and status reporting**

## CLI Architecture

The CLI system is organized into the following components:

### Core CLI Implementation (`src/cli/`)

The main CLI implementation includes:

- `CLIParser.hpp/cpp`: Command-line argument parsing
- `CommandHandler.hpp/cpp`: Command execution handlers
- `ConfigManager.hpp/cpp`: Configuration management
- `LoggerConfig.hpp/cpp`: Logging configuration
- `HelpSystem.hpp/cpp`: Help system implementation
- `ProgressMonitor.hpp/cpp`: Progress monitoring
- `CLIApp.hpp/cpp`: Main application orchestrator

### Configuration Management (`configs/`)

The system includes built-in configuration presets:

- `basic.cfg`: Simple neuron dynamics (500 neurons, 1 region)
- `plasticity.cfg`: Learning and adaptation (1000 neurons, 2 regions)
- `development.cfg`: Developmental stages (800 neurons, 2 regions)
- `cognitive.cfg`: Full cognitive capabilities (2000 neurons, 4 regions)
- `social.cfg`: Social interaction simulation (1500 neurons, 3 regions)
- `memory.cfg`: Memory-intensive scenarios (3000 neurons, 2 regions)
- `attention.cfg`: Attention and focus simulation (1200 neurons, 3 regions)

### Scripting and Automation (`scripts/`)

Three Python scripts for automation and batch processing:

- `batch_run.py`: Run batch of simulations with different parameters
- `parameter_sweep.py`: Parameter sweep for optimization
- `experiment_runner.py`: Run complex experimental workflows

### Documentation

Comprehensive documentation includes:

- `cli_examples.md`: CLI usage examples and workflows
- Updated `docs/ARCHITECTURE.md`: CLI architecture documentation
- Updated `HOW_TO_USE.md`: Enhanced CLI usage guide
- Updated `easy_usage.md`: Simple CLI usage guide

## CLI Command Structure

All NLM CLI commands follow the pattern:

```bash
nlm [options] <command> [arguments]
```

### Global Options

| Option | Description | Example |
|--------|-------------|---------|
| `--help, -h` | Show help information | `nlm --help` |
| `--version, -v` | Show version information | `nlm --version` |
| `--list-presets` | List available presets | `nlm --list-presets` |
| `--verbose=<level>` | Verbosity level | `nlm --verbose debug` |
| `--config=<file>` | Configuration file | `nlm --config config.json` |
| `--output=<file>` | Output file | `nlm --output results.json` |
| `--log=<file>` | Log file | `nlm --log simulation.log` |

### Commands

| Command | Description | Example |
|---------|-------------|---------|
| `run` | Run simulation | `nlm run --type basic --steps 100` |
| `configure` | Manage configuration | `nlm configure --show` |
| `experiment` | Run experiment | `nlm experiment --type plasticity` |
| `analyze` | Analyze results | `nlm analyze --format json` |
| `benchmark` | Performance benchmarking | `nlm benchmark --duration 60` |

## Usage Examples

### Basic Simulation Commands

```bash
# Run a basic simulation
nlm run --type basic --steps 100 --neurons 500

# Run a plasticity experiment
nlm experiment --type plasticity --steps 1000 --neurons 1000

# Run a development simulation
nlm run --type development --steps 5000 --neurons 800

# Run a cognitive simulation
nlm run --type cognitive --steps 10000 --neurons 2000
```

### Using Presets

```bash
# Use built-in preset configurations
nlm run --preset basic --steps 100
nlm run --preset plasticity --steps 1000
nlm run --preset cognitive --steps 5000
```

### Configuration Management

```bash
# Show current configuration
nlm configure --show

# Save configuration to file
nlm configure --save my_config.json

# Load configuration from file
nlm run --config my_config.json --type basic

# Reset to default configuration
nlm configure --reset
```

### Analysis and Benchmarking

```bash
# Analyze simulation results
nlm analyze --format json --output analysis.json

# Performance benchmarking
nlm benchmark --duration 60 --neurons 500

# Benchmark with specific configuration
nlm benchmark --duration 30 --type development
```

### Scripting and Automation

```bash
# Run batch of simulations
python scripts/batch_run.py --presets basic plasticity --neurons 500 1000 --steps 100 500

# Run parameter sweep
python scripts/parameter_sweep.py --param neurons=500,1000 --param steps=100,500

# Run experiment suite
python scripts/experiment_runner.py --type plasticity,development --neurons 1000 1500
```

### Help System

```bash
# Show general help
nlm --help

# Show help for specific command
nlm help run

# Show help for experiment command
nlm help experiment

# Get information about presets
nlm --list-presets
```

## Configuration Presets

### Basic Preset

**Description**: Simple neuron dynamics with basic connectivity
**Parameters**: 500 neurons, 1 region, moderate plasticity

```bash
nlm run --preset basic --steps 100
```

### Plasticity Preset

**Description**: Learning and adaptation with STDP and Hebbian plasticity
**Parameters**: 1000 neurons, 2 regions, high plasticity rates

```bash
nlm run --preset plasticity --steps 1000
```

### Development Preset

**Description**: Developmental stages with synaptogenesis and pruning
**Parameters**: 800 neurons, 2 regions, developmental plasticity

```bash
nlm run --preset development --steps 5000
```

### Cognitive Preset

**Description**: Full cognitive capabilities including memory and attention
**Parameters**: 2000 neurons, 4 regions, complex interactions

```bash
nlm run --preset cognitive --steps 10000
```

### Social Preset

**Description**: Social interaction simulation with reward-based learning
**Parameters**: 1500 neurons, 3 regions, social reward modulation

```bash
nlm run --preset social --steps 3000
```

### Memory Preset

**Description**: Memory-intensive scenarios with high synaptic stability
**Parameters**: 3000 neurons, 2 regions, strong memory consolidation

```bash
nlm run --preset memory --steps 20000
```

### Attention Preset

**Description**: Attention and focus simulation with competitive dynamics
**Parameters**: 1200 neurons, 3 regions, attention competition

```bash
nlm run --preset attention --steps 2000
```

## Advanced Features

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

### Environment Variables

Set environment variables to configure NLM:

```bash
export NLM_LOG_LEVEL=info
export NLM_OUTPUT_DIR=results/
export NLM_DEFAULT_PRESET=plasticity
```

### Shell Integration

Create aliases for common commands:

```bash
# Add NLM to PATH if not already there
export PATH="$PATH:/path/to/nlm/bin"

# Create aliases
alias nlm-run="nlm run --type basic --steps 100"
alias nlm-explore="nlm experiment --type plasticity --steps 1000"
alias nlm-config="nlm configure --show"
```

### Integration with Python API

The CLI can be integrated with the NLM Python API for complex workflows:

```python
import pynlm
import subprocess
import json

# Create and save configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.saveToFile("simulation_config.json")

# Run simulation using CLI
result = subprocess.run([
    "nlm", "run", 
    "--config", "simulation_config.json",
    "--type", "plasticity",
    "--steps", "1000"
], capture_output=True, text=True)

print(f"Simulation completed with exit code: {result.returncode}")

# Use Python API for analysis
if result.returncode == 0:
    print("Simulation completed successfully")
```

## Scripting Examples

### Batch Processing Script

```python
#!/usr/bin/env python3
import subprocess
import os

def run_batch(presets, neurons, steps, output_dir):
    """Run batch of NLM simulations."""
    os.makedirs(output_dir, exist_ok=True)
    
    total_jobs = len(presets) * len(neurons) * len(steps)
    completed = 0
    
    for preset in presets:
        for neuron_count in neurons:
            for step_count in steps:
                cmd = [
                    "nlm", "run",
                    "--type", preset,
                    "--steps", str(step_count),
                    "--neurons", str(neuron_count),
                    "--output", f"{output_dir}/{preset}_{neuron_count}_{step_count}.json"
                ]
                
                print(f"Running: {' '.join(cmd)}")
                result = subprocess.run(cmd)
                
                if result.returncode == 0:
                    completed += 1
    
    print(f"Batch completed: {completed}/{total_jobs} simulations")

# Run batch
run_batch(
    presets=["basic", "plasticity"],
    neurons=[500, 1000],
    steps=[100, 500],
    output_dir="batch_results"
)
```

### Parameter Sweep Script

```python
#!/usr/bin/env python3
import subprocess
import json

def run_parameter_sweep(params, base_config):
    """Run parameter sweep with specified parameters."""
    results = []
    
    for param_name, param_values in params.items():
        for param_value in param_values:
            # Create modified config
            config = base_config.copy()
            config[param_name] = param_value
            
            # Run simulation
            cmd = ["nlm", "run"]
            for key, value in config.items():
                cmd.extend([f"--{key}", str(value)])
            
            print(f"Running: {cmd}")
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            results.append({
                'parameters': config,
                'success': result.returncode == 0,
                'output': result.stdout
            })
    
    return results

# Define parameter sweep
params = {
    'neurons': [500, 1000, 1500],
    'steps': [100, 500, 1000]
}

base_config = {
    'type': 'plasticity',
    'preset': 'plasticity'
}

# Run sweep
results = run_parameter_sweep(params, base_config)

# Save results
with open('sweep_results.json', 'w') as f:
    json.dump(results, f, indent=2)
```

### Experiment Runner Script

```python
#!/usr/bin/env python3
import subprocess
import json
from pathlib import Path

def create_experiment_suite(experiment_type, neurons_list, steps_list):
    """Create a suite of experiments with varying parameters."""
    experiments = []
    
    for neurons in neurons_list:
        for steps in steps_list:
            experiments.append({
                'type': experiment_type,
                'neurons': neurons,
                'steps': steps,
                'name': f"{experiment_type}_{neurons}_{steps}"
            })
    
    return experiments

def run_experiment_suite(experiments, output_dir):
    """Run a suite of experiments."""
    Path(output_dir).mkdir(exist_ok=True)
    
    results = []
    
    for exp in experiments:
        cmd = [
            "nlm", "experiment",
            "--type", exp['type'],
            "--neurons", str(exp['neurons']),
            "--steps", str(exp['steps']),
            "--output", f"{output_dir}/{exp['name']}.json"
        ]
        
        print(f"Running experiment: {exp['name']}")
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        results.append({
            'name': exp['name'],
            'success': result.returncode == 0,
            'output': result.stdout,
            'error': result.stderr if result.returncode != 0 else None
        })
    
    return results

# Create and run experiment suite
experiments = create_experiment_suite(
    experiment_type='plasticity',
    neurons_list=[1000, 1500, 2000],
    steps_list=[500, 1000, 2000]
)

results = run_experiment_suite(
    experiments,
    output_dir='experiment_results'
)

# Save results
with open('experiment_results.json', 'w') as f:
    json.dump(results, f, indent=2)
```

## Integration with Development Workflow

### CI/CD Integration

```yaml
# Example GitHub Actions workflow
name: NLM Tests

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Run NLM Simulation
      run: |
        nlm experiment --type plasticity --steps 1000 --neurons 1000
        nlm analyze --format json --output results.json
    
    - name: Upload results
      uses: actions/upload-artifact@v2
      with:
        name: nlm-results
        path: results.json
```

### Testing

```bash
# Create test configuration
nlm run --type basic --steps 10 --neurons 100 --output test_results.json

# Verify output
python -c "
import json
data = json.load(open('test_results.json'))
print(f'Test completed: {data.get(\"spikes\", 0)} spikes')
"
```

## Troubleshooting

### Common CLI Issues

**Issue: "Command not found: nlm"**
```
Solution: Ensure NLM is installed and in your PATH
```

**Issue: Unknown command**
```
Solution: Check available commands with 'nlm --help'
```

**Issue: Invalid parameter**
```
Solution: Check parameter names and values with 'nlm help run'
```

**Issue: Configuration file not found**
```
Solution: Use absolute path or ensure file exists in current directory
```

**Issue: Simulation timeout**
```
Solution: Increase timeout or reduce simulation parameters
```

### Performance Tips

1. **Use presets for common configurations**
   ```bash
   nlm run --preset basic --steps 100
   ```

2. **Batch processing with Python scripts**
   ```bash
   python scripts/batch_run.py --presets basic plasticity
   ```

3. **Limit verbosity for production runs**
   ```bash
   nlm run --type basic --steps 100 --verbose quiet
   ```

4. **Save configurations for reproducibility**
   ```bash
   nlm configure --save production_config.json
   ```

### System Requirements

- **Memory**: For simulations with >2000 neurons, ensure sufficient RAM
- **CPU**: Multithreaded builds may use multiple cores
- **Disk space**: Large simulations may require significant disk space
- **Python**: For scripting, Python 3.8+ is required

## Conclusion

The enhanced NLM CLI provides a comprehensive set of tools for running neural network simulations, experiments, and analysis. With its flexible command structure, configuration management, and scripting support, it enables users to efficiently configure, run, and analyze complex neural network simulations.

The CLI system integrates seamlessly with the NLM Python API, allowing users to leverage the strengths of both approaches for optimal results. Whether you're running simple simulations or complex experimental workflows, the enhanced NLM CLI provides the tools you need to succeed.

For more information, refer to the documentation in this directory and the examples in the scripts/ directory. The CLI system is designed to be both powerful for complex workflows and simple for basic use cases.

Enjoy experimenting with neural network simulations using NLM!