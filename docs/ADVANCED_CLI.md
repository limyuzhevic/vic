# NLM Advanced Command Line Interface

## Overview

The NLM neural network provides enhanced command-line options for advanced users, researchers, and automated experimentation. These commands enable fine-grained control over simulation parameters, experimental configurations, and system behavior.

## Available Commands

### General Control Commands

#### `--mode [mode]`
Set simulation mode:
- `normal` - Standard neural computation (default)
- `replay` - Replay from checkpoint
- `development` - Focused on developmental plasticity
- `exploration` - Emphasis on curiosity-driven exploration

#### `--checkpoint [path]`
Load/save checkpoint:
- `--checkpoint path/to/checkpoint` - Load from checkpoint
- `--checkpoint path/to/checkpoint,save` - Save and load

#### `--seed [number]`
Set random seed for reproducible experiments:
- `--seed 42` - Fixed seed for reproducibility

### Simulation Parameters

#### `--timestep [ms]`
Set simulation timestep:
- `--timestep 0.001` - 1ms timestep
- `--timestep 0.0001` - 0.1ms timestep

#### `--neurons [count]`
Set number of neurons:
- `--neurons 1000` - Override configured neuron count

#### `--regions [count]`
Set number of neural regions:
- `--regions 2` - Multiple regions for modular processing

#### `--plasticity [mode]`
Set plasticity mode:
- `stdp` - Spike timing dependent plasticity only
- `hebbian` - Hebbian learning only
- `both` - Both plasticity rules (default)
- `none` - No plasticity

### Experimental Commands

#### `--experiment [name]`
Run specific experiment:
- `--experiment test1` - Run basic connectivity test
- `--experiment learning` - Run learning experiment
- `--experiment stdp` - Run STDP verification

#### `--duration [steps]`
Set experiment duration:
- `--duration 5000` - Run 5000 simulation steps

#### `--targets [count]`
Set target neuron count for experiments:
- `--targets 50` - Create 50 target neurons

### Output and Logging Commands

#### `--verbose [level]`
Set logging verbosity:
- `--verbose info` - Basic info (default)
- `--verbose debug` - Detailed debug information
- `--verbose trace` - All processing details

#### `--output [path]`
Set output directory:
- `--output results/exp1` - Save results to specified path

#### `--metrics [format]`
Set metrics export format:
- `--metrics json` - JSON format
- `--metrics csv` - CSV format
- `--metrics binary` - Binary format

#### `--profile [type]`
Enable performance profiling:
- `--profile cpu` - CPU profiling
- `--profile memory` - Memory profiling
- `--profile both` - Both CPU and memory profiling

### Advanced Neuroscience Commands

#### `--reward [type]`
Set reward computation method:
- `simple` - Simple reward function (default)
- `advanced` - Advanced reward with temporal difference
- `none` - No reward computation

#### `--neuromodulation [enable]`
Control neuromodulation:
- `--neuromodulation dopamine` - Only dopamine
- `--neuromodulation all` - All neuromodulators
- `--neuromodulation none` - Disable neuromodulation

#### `--development [stage]`
Set developmental stage:
- `--development 0.1` - Early development (high plasticity)
- `--development 0.5` - Mid development
- `--development 1.0` - Mature development

#### `--attention [mode]`
Set attention mode:
- `competitive` - Competitive attention (default)
- `sequential` - Sequential attention
- `parallel` - Parallel attention

### Utility Commands

#### `--help`
Show help information:
- Shows all available commands and their descriptions

#### `--list-experiments`
List available experiments:
- Lists all experiment types and descriptions

#### `--show-config`
Display current configuration:
- Shows effective configuration after parameter merging

#### `--validate-config`
Validate configuration:
- Checks for parameter consistency and validity

## Configuration File Format

Advanced configuration can be specified in files:

```ini
# Configuration file example
[nlm]
mode = normal
seed = 42
timestep = 0.001
neurons = 1000

[plasticity]
stdp_enabled = true
hebbian_enabled = true
reward_modulated = true

[neuromodulation]
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true

[output]
verbose_level = debug
metrics_format = json
profile_cpu = true
profile_memory = false
```

## Script Mode

For automated experimentation, NLM supports script mode:

### Basic Script Mode
```bash
nlm --script experiment.json
```

### Script with Parameters
```bash
nlm --script experiment.json --param learning_rate 0.01 --param population_size 100
```

### Batch Processing
```bash
nlm --batch --script-list scripts/ --output-dir results/
```

## Python API Integration

### Command Line Execution
```python
import subprocess
import json

# Run NLM with specific parameters
result = subprocess.run([
    'nlm',
    '--mode', 'exploration',
    '--duration', '10000',
    '--output', 'results/test_run',
    '--metrics', 'json'
], capture_output=True, text=True)

# Parse results
results = json.loads(result.stdout)
```

### Programmatic API
```cpp
#include <nlm/AdvancedCLI.hpp>

// Create advanced CLI configuration
AdvancedCLI cli;
cli.setMode(Mode::EXPLORATION);
cli.setSeed(42);
cli.setDuration(10000);
cli.setOutputDirectory("results/test_run");
cli.enableCPUProfiling();

// Run simulation
NLMResult result = cli.run();
```

## Configuration Command Examples

### Learning Experiment
```bash
nlm \
  --mode normal \
  --duration 5000 \
  --plasticity both \
  --reward advanced \
  --neuromodulation all \
  --output results/learning_test \
  --metrics json \
  --profile both
```

### Developmental Study
```bash
nlm \
  --mode development \
  --development 0.3 \
  --neurons 2000 \
  --plasticity stdp \
  --neuromodulation dopamine \
  --checkpoint dev_checkpoint.bin,save \
  --output results/development \
  --metrics csv
```

### Replay and Analysis
```bash
nlm \
  --mode replay \
  --checkpoint important_run.bin \
  --verbose trace \
  --profile memory \
  --output results/replay_analysis \
  --show-config
```

### Batch Experiment
```bash
nlm \
  --batch \
  --script-list experiments/ \
  --param learning_rate 0.01 0.02 0.05 \
  --param population_size 100 200 500 \
  --output-dir results/batch_test
```

## Configuration Parameter Merging

When multiple configuration sources are used (file, command line, runtime), NLM follows this precedence:

1. **Runtime API** (highest) - Programmatically set parameters
2. **Command Line** - Explicitly provided options
3. **Configuration File** - Loaded from files
4. **Default Configuration** (lowest) - Built-in defaults

Parameters set with higher precedence override those from lower precedence sources.

## Error Handling

### Common Error Messages

- `"Error: Invalid mode '--mode invalid'"`
- `"Error: Checkpoint file not found: path/to/checkpoint"`
- `"Error: Invalid parameter value for '--neurons': must be positive integer"`
- `"Error: Configuration validation failed: missing required parameters"`

### Validation Rules

- Mode must be one of: `normal`, `replay`, `development`, `exploration`
- Timestep must be positive and less than 1.0
- Neuron count must be positive integer
- Region count must be positive integer
- Configuration files must be valid JSON/INI format

## Integration with Phase 6

The advanced CLI is fully integrated with Phase 6 capabilities:

- Checkpoints support full brain state serialization
- All Phase 6 systems respect CLI settings
- Metrics export includes Phase 6-specific data
- Replay mode works with Phase 6 checkpoints

## Performance Considerations

### Profiling Commands

#### `--profile cpu`
Enables CPU performance measurements:
- Simulation step timing
- Plasticity computation time
- Memory allocation tracking

#### `--profile memory`
Enables memory usage tracking:
- Neural state memory
- Synapse weight arrays
- Checkpoint file sizes

### Output Size Optimization

```bash
# Lightweight output for batch processing
nlm --metrics binary --output results/light

# Compressed output for storage
nlm --metrics compressed --output results/compressed
```

## Script Examples

### Simple Experiment Script
```python
#!/usr/bin/env python3

import subprocess
import json
import os

def run_experiment(learning_rate, population_size, duration):
    """Run a single NLM experiment with specified parameters."""
    
    # Prepare command
    cmd = [
        'nlm',
        '--mode', 'normal',
        '--plasticity', 'both',
        '--reward', 'advanced',
        '--duration', str(duration),
        '--neurons', str(population_size),
        '--output', f'results/exp_lr{learning_rate}_size{population_size}',
        '--metrics', 'json'
    ]
    
    # Run experiment
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"Experiment failed: {result.stderr}")
        return None
    
    # Parse and return results
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        print(f"Failed to parse results: {result.stdout}")
        return None

# Run multiple experiments
experiments = [
    (0.01, 100, 5000),
    (0.02, 100, 5000),
    (0.05, 100, 5000)
]

for learning_rate, pop_size, dur in experiments:
    print(f"Running experiment: lr={learning_rate}, pop={pop_size}, dur={dur}")
    results = run_experiment(learning_rate, pop_size, dur)
    if results:
        print(f"Completed: {results}")
```

### Configuration Generator
```python
#!/usr/bin/env python3

import yaml
import argparse

def generate_config(args):
    """Generate NLM configuration from command line arguments."""
    
    config = {
        'nlm': {
            'mode': args.mode,
            'seed': args.seed,
            'timestep': args.timestep,
            'neurons': args.neurons,
            'regions': args.regions
        },
        'plasticity': {
            'stdp_enabled': args.plasticity in ['both', 'stdp'],
            'hebbian_enabled': args.plasticity in ['both', 'hebbian'],
            'reward_modulated': args.reward != 'none'
        },
        'neuromodulation': {
            'dopamine_enabled': 'dopamine' in args.neuromodulation,
            'curiosity_enabled': 'curiosity' in args.neuromodulation,
            'novelty_enabled': 'novelty' in args.neuromodulation
        },
        'development': {
            'stage': args.development
        },
        'attention': {
            'mode': args.attention
        },
        'output': {
            'verbose_level': args.verbose,
            'metrics_format': args.metrics,
            'directory': args.output
        }
    }
    
    # Save configuration
    with open(f'{args.output}/config.yaml', 'w') as f:
        yaml.dump(config, f, default_flow_style=False)
    
    print(f"Configuration saved to {args.output}/config.yaml")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate NLM configuration')
    parser.add_argument('--mode', choices=['normal', 'replay', 'development', 'exploration'], default='normal')
    parser.add_argument('--seed', type=int, default=42)
    parser.add_argument('--timestep', type=float, default=0.001)
    parser.add_argument('--neurons', type=int, default=500)
    parser.add_argument('--regions', type=int, default=1)
    parser.add_argument('--plasticity', choices=['stdp', 'hebbian', 'both', 'none'], default='both')
    parser.add_argument('--reward', choices=['simple', 'advanced', 'none'], default='simple')
    parser.add_argument('--neuromodulation', nargs='+', default=['all'], choices=['dopamine', 'curiosity', 'novelty', 'all', 'none'])
    parser.add_argument('--development', type=float, default=1.0)
    parser.add_argument('--attention', choices=['competitive', 'sequential', 'parallel'], default='competitive')
    parser.add_argument('--verbose', choices=['info', 'debug', 'trace'], default='info')
    parser.add_argument('--metrics', choices=['json', 'csv', 'binary', 'compressed'], default='json')
    parser.add_argument('--output', default='config')
    
    args = parser.parse_args()
    generate_config(args)
```

## Summary

The NLM advanced CLI provides comprehensive command-line control over the neural network simulation, enabling:

- **Fine-grained parameter control** for experimental precision
- **Flexible output formats** for different analysis needs
- **Performance profiling** for optimization
- **Batch processing** capabilities for automated experimentation
- **Configuration management** for reproducibility
- **Integration with Phase 6** systems

These capabilities make NLM suitable for:
- Research experiments requiring precise control
- Automated benchmark testing
- Machine learning hyperparameter optimization
- Educational demonstrations
- Production neural network deployment
