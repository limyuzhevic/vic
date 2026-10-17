# NLM Command Line Interface Documentation

## Overview

The NLM command line interface provides comprehensive control over neural network simulations through a rich set of options and arguments. It supports configuration files, presets, type validation, and extensive help documentation.

## Command Line Syntax

### Basic Usage

```bash
./build/nlm [options] [arguments]
```

### Options

Options can be specified in several ways:

1. **Short options**: `-h`, `-n`, `-p`, `-v`
2. **Long options**: `--help`, `--neuron-count`, `--enable-plasticity`
3. **Configuration file**: `--config=filename`
4. **Preset**: `--preset=name`

## Command Line Structure

```
Usage: ./build/nlm [options]

Options:
  -h, --help                    Show help information and exit
  -v, --verbose                 Enable verbose output
  -n, --neuron-count=N          Number of neurons (default: 1000)
  -p, --enable-plasticity       Enable synaptic plasticity
  -s, --disable-plasticity      Disable synaptic plasticity

  Core Configuration:
    --random-seed=N              Random seed for reproducibility
    --simulation-timestep=N      Simulation time step (default: 0.001)
    --neuron-count=N             Number of neurons (default: 1000)
    --region-count=N             Number of regions (default: 10)
    --connection-probability=N   Connection probability (default: 0.1)

  Brain Configuration:
    --brain-type=TYPE             Brain architecture type (default: "standard")
    --plasticity-rule=RULE        Plasticity rule (stdp, hebbian)

  Plasticity Settings:
    --stdp-ltp-weight=N           STDP LTP weight (default: 0.01)
    --stdp-ltd-weight=N           STDP LTD weight (default: 0.01)
    --stdp-tau=N                  STDP time constant (default: 20.0)
    --synaptogenesis-rate=N       Synaptogenesis rate (default: 0.001)
    --pruning-rate=N              Pruning rate (default: 0.0001)

  Neuromodulation Settings:
    --enable-neuromodulation       Enable neuromodulation system
    --disable-neuromodulation      Disable neuromodulation system
    --curiosity-strength=N        Curiosity strength (default: 1.0)
    --novelty-threshold=N         Novelty threshold (default: 0.5)
    --reward-decay=N              Reward decay (default: 0.99)

  Simulation Parameters:
    --max-steps=N                 Maximum simulation steps (default: 10000)
    --enable-checkpointing        Enable checkpoint saving
    --checkpoint-interval=N       Checkpoint interval (default: 1000)
    --enable-profiling            Enable performance profiling

  Output Configuration:
    --output-dir=DIR              Output directory (default: "./results")
    --save-weights                Save neural network weights
    --log-level=LEVEL             Log level (debug, info, warning, error)
    --enable-visualization        Enable visualization output

  Debugging Options:
    --debug-neurons=LIST          Comma-separated list of neuron IDs for debugging
    --trace-spikes                Trace spike events
    --trace-plasticity           Trace plasticity events

  Preset Options:
    --preset=NAME                 Apply predefined preset (standard, fast, high_precision, debug)

  Configuration File:
    --config=FILE                 Load configuration from file

  Additional Options:
    -c, --complete                Show command completion suggestions
    --version                     Show version information
    --license                     Show license information

  Examples:
    ./build/nlm --neuron-count=1000 --simulation-timestep=0.01
    ./build/nlm --config=my_config.cfg
    ./build/nlm --preset=debug
    ./build/nlm --neuron-count=2000 --enable-neuromodulation
```

## Option Categories

### Core Options

#### Basic Controls
- `-h, --help`: Show help information and exit
- `-v, --verbose`: Enable verbose output
- `--version`: Show version information
- `--license`: Show license information

#### Random Seed
- `--random-seed=N`: Set random seed for reproducible simulations

#### Simulation Settings
- `-n, --neuron-count=N`: Number of neurons (default: 1000)
- `--simulation-timestep=N`: Time step for simulation (default: 0.001)
- `--region-count=N`: Number of neural regions (default: 10)
- `--connection-probability=N`: Probability of synaptic connections (default: 0.1)

### Brain Configuration Options

#### Brain Architecture
- `--brain-type=TYPE`: Type of brain architecture (default: "standard")
  - Available types: "standard", "cortical", "subcortical", "hybrid"
- `--plasticity-rule=RULE`: Plasticity rule (stdp, hebbian) (default: "stdp")

### Plasticity Options

#### STDP Configuration
- `--stdp-ltp-weight=N`: STDP long-term potentiation weight (default: 0.01)
- `--stdp-ltd-weight=N`: STDP long-term depression weight (default: 0.01)
- `--stdp-tau=N`: STDP time constant (default: 20.0)

#### Structural Plasticity
- `--synaptogenesis-rate=N`: Rate of new synapse formation (default: 0.001)
- `--pruning-rate=N`: Rate of synapse pruning (default: 0.0001)

### Neuromodulation Options

#### General Controls
- `--enable-neuromodulation`: Enable neuromodulation system
- `--disable-neuromodulation`: Disable neuromodulation system

#### Neuromodulator Parameters
- `--curiosity-strength=N`: Strength of curiosity-driven learning (default: 1.0)
- `--novelty-threshold=N`: Threshold for novelty detection (default: 0.5)
- `--reward-decay=N`: Reward decay factor (default: 0.99)
- `--dopamine-strength=N`: Dopamine signal strength (default: 1.0)

### Simulation Options

#### Execution Control
- `--max-steps=N`: Maximum simulation steps (default: 10000)
- `--enable-checkpointing`: Enable checkpoint saving
- `--checkpoint-interval=N`: Checkpoint saving interval (default: 1000)
- `--enable-profiling`: Enable performance profiling

#### Debugging
- `--debug-neurons=LIST`: Comma-separated list of neuron IDs for debugging
- `--trace-spikes`: Trace spike events
- `--trace-plasticity`: Trace plasticity events

### Output Options

#### Directory and File Control
- `--output-dir=DIR`: Output directory (default: "./results")
- `--save-weights`: Save neural network weights
- `--log-level=LEVEL`: Log level (debug, info, warning, error) (default: "info")
- `--enable-visualization`: Enable visualization output

### Preset Options

#### Available Presets
- `--preset=NAME`: Apply predefined preset
  - `standard`: Balanced configuration for general use
  - `fast`: Optimized for speed, reduced accuracy
  - `high_precision`: Optimized for accuracy, enhanced learning
  - `debug`: Enhanced debugging output, detailed logging

## Configuration File Format

The configuration system supports multiple file formats:

### Plain Text (.cfg)
```
# Comment
key = value
```

### JSON (.json)
```json
{
    "key": "value"
}
```

### YAML (.yaml)
```yaml
key: value
```

### TOML (.toml)
```toml
key = "value"
```

### INI (.ini)
```ini
[section]
key = value
```

### Properties (.properties)
```properties
key = value
```

## Command Line Examples

### Basic Usage

```bash
# Simple simulation
./build/nlm --neuron-count=1000 --simulation-timestep=0.01

# With plasticity enabled
./build/nlm --neuron-count=1000 --enable-plasticity

# With verbose output
./build/nlm --neuron-count=500 --verbose
```

### Configuration File Usage

```bash
# Use configuration file
./build/nlm --config=my_config.cfg

# Override parameters from configuration file
./build/nlm --config=my_config.cfg --neuron-count=2000 --enable-plasticity=true
```

### Preset Usage

```bash
# Use standard preset
./build/nlm --preset=standard

# Use debug preset
./build/nlm --preset=debug

# Use preset and override
./build/nlm --preset=debug --neuron-count=100
```

### Advanced Configuration

```bash
./build/nlm \
    --neuron-count=5000 \
    --simulation-timestep=0.001 \
    --enable-plasticity \
    --plasticity-rule=stdp \
    --stdp-ltp-weight=0.02 \
    --stdp-ltd-weight=0.01 \
    --enable-neuromodulation \
    --curiosity-strength=2.0 \
    --enable-novelty \
    --enable-prediction-error \
    --enable-checkpointing \
    --checkpoint-interval=5000 \
    --max-steps=50000 \
    --output-dir=./results \
    --log-level=info
```

## Command Completion

The command line interface supports completion for common options:

```bash
# Show completion suggestions
./build/nlm --complete

# Use completion in shell
./build/nlm --complete "--"
```

## Error Handling

### Common Errors and Solutions

#### Error: Invalid Option
```
Usage: ./build/nlm [options]
Error: Invalid option '--invalid-option'
Run './build/nlm --help' for more information.
```

**Solution**: Check spelling or use `--help` for valid options.

#### Error: Invalid Value
```
Usage: ./build/nlm [options]
Error: Invalid value for '--neuron-count': 'invalid'
Error: Value must be an integer.
```

**Solution**: Provide a valid integer value.

#### Error: Missing Required Value
```
Usage: ./build/nlm [options]
Error: Missing required value for '--neuron-count'.
```

**Solution**: Provide a value after the option (e.g., `--neuron-count=1000`).

#### Error: File Not Found
```
Usage: ./build/nlm [options]
Error: Configuration file not found: 'nonexistent.cfg'
```

**Solution**: Check the file path or ensure the file exists.

#### Error: Invalid Preset
```
Usage: ./build/nlm [options]
Error: Invalid preset: 'invalid_preset'
Available presets: standard, fast, high_precision, debug
```

**Solution**: Use one of the available preset names.

### Error Messages

The command line interface provides detailed error messages:

1. **Validation Errors**: Invalid values are caught and reported
2. **Missing Dependencies**: Missing required parameters are flagged
3. **File Errors**: Invalid or unreadable configuration files
4. **Format Errors**: Unsupported configuration file formats
5. **Type Errors**: Type conversion failures

## Environment Variables

The command line interface also supports environment variables for configuration:

```bash
export NLM_NEURON_COUNT=1000
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_ENABLE_PLASTICITY=true
export NLM_OUTPUT_DIR=./results

# These can be used as fallback or defaults
./build/nlm --neuron-count=$NLM_NEURON_COUNT
```

## Configuration File Command Line Integration

The command line can load and merge configuration files:

```bash
# Load base configuration
./build/nlm --config=base.cfg

# Override specific parameters
./build/nlm --config=base.cfg --neuron-count=2000

# Use multiple configuration files (later files override earlier ones)
./build/nlm --config=base.cfg --config=overrides.cfg
```

## Help Information

### General Help

```bash
./build/nlm --help
```

Shows all available options with descriptions.

### Option-Specific Help

```bash
./build/nlm --help --neuron-count
```

Shows detailed information about the `--neuron-count` option.

### Configuration File Help

```bash
./build/nlm --help --config
```

Shows information about configuration file loading and format.

### Preset Help

```bash
./build/nlm --help --preset
```

Shows information about available presets.

## Completion Examples

### Complete Options

```bash
# Show all options
./build/nlm --help

# Show core options
./build/nlm --help --core

# Show brain options
./build/nlm --help --brain

# Show plasticity options
./build/nlm --help --plasticity

# Show neuromodulation options
./build/nlm --help --neuromodulation
```

### Complete Values

```bash
# Complete neuron count values
./build/nlm --complete "--neuron-count="

# Complete brain types
./build/nlm --complete "--brain-type="

# Complete log levels
./build/nlm --complete "--log-level="
```

## Integration with Python

The command line interface can be used with Python scripts:

```python
import subprocess
import sys

# Run NLM with specific configuration
result = subprocess.run([
    './build/nlm',
    '--neuron-count=1000',
    '--simulation-timestep=0.01',
    '--enable-plasticity',
    '--enable-neuromodulation'
], capture_output=True, text=True)

if result.returncode == 0:
    print("Simulation completed successfully")
    print(result.stdout)
else:
    print(f"Simulation failed: {result.stderr}")
```

## Scripting and Automation

### Shell Script Example

```bash
#!/bin/bash

# NLM simulation script
set -e

NEURON_COUNT=${1:-1000}
TIMESTEP=${2:-0.01}
MAX_STEPS=${3:-10000}

./build/nlm \
    --neuron-count=$NEURON_COUNT \
    --simulation-timestep=$TIMESTEP \
    --max-steps=$MAX_STEPS \
    --enable-plasticity \
    --enable-neuromodulation \
    --output-dir=./results_${NEURON_COUNT} \
    --log-level=info

# Check if simulation was successful
if [ -f "./results_${NEURON_COUNT}/weights.npz" ]; then
    echo "Simulation completed successfully"
else
    echo "Simulation failed"
    exit 1
fi
```

### Python Script Example

```python
import pynlm
import os
import subprocess
import json

def run_nlm_simulation(config):
    """Run NLM simulation with given configuration."""
    # Convert configuration to command line arguments
    args = ['./build/nlm']
    
    for key, value in config.items():
        if isinstance(value, bool):
            if value:
                args.append(f'--{key.replace("_", "-")}')
        else:
            args.append(f'--{key.replace("_", "-")}={value}')
    
    # Run simulation
    result = subprocess.run(args, capture_output=True, text=True)
    
    if result.returncode != 0:
        raise RuntimeError(f"Simulation failed: {result.stderr}")
    
    return result.stdout

# Configuration
config = {
    'neuron_count': 1000,
    'simulation_timestep': 0.01,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'max_steps': 10000,
    'output_dir': './python_results',
    'log_level': 'info'
}

# Run simulation
output = run_nlm_simulation(config)
print("Simulation completed successfully")
```

## Conclusion

The NLM command line interface provides comprehensive control over neural network simulations through:

1. **Rich Option Set**: Over 20 command line options for fine-grained control
2. **Configuration Files**: Multiple file formats for complex configurations
3. **Presets**: Predefined configurations for common use cases
4. **Type Safety**: Automatic type conversion and validation
5. **Help System**: Comprehensive help documentation and completion
6. **Error Handling**: Detailed error messages and helpful suggestions
7. **Scripting Support**: Integration with shell scripts and Python
8. **Environment Variables**: Fallback configuration through environment

The command line interface is designed to support both simple interactive use and complex automation scenarios, providing flexibility for researchers, developers, and users at all skill levels.

For more information, refer to the specific sections of this documentation or consult the configuration system documentation for detailed implementation details.