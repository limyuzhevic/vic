# NLM - Neural Learning Machine CLI Interface

This document describes the unified command-line interface for NLM (Neural Learning Machine), providing access to different demo modes, experiments, and configuration options.

## Overview

NLM provides several demo and experiment modes, each accessible through a unified command-line interface with consistent argument patterns and help documentation.

## Usage

```bash
# Show general help
./nlm --help

# Show demo-specific help
./nlm demo --help
./nlm experiment --help
./nlm benchmark --help

# Run specific demo with custom parameters
./nlm demo phase2 --neuron-count 1000 --steps 2000
./nlm demo phase3 3 300    # episodes, steps-per-episode
./nlm demo phase5 --max-resources 5
./nlm demo phase6 --quick

# Run experiment suite
./nlm experiment run --type phase6 --episodes 10
./nlm experiment compare phase5 phase6 --runs 3

# Run benchmark with custom settings
./nlm benchmark --neuron-sizes "100 500 1000 2000" --steps 1000
./nlm benchmark --quiet

# Configuration and output options
./nlm --config custom.cfg demo phase2 --log-level DEBUG
./nlm --output-dir ./results demo phase3 --save-results
```

## Global Options

### `--help, -h`
Show help information for the current command or global options.

### `--version, -v`
Show NLM version information.

### `--config FILE, -c FILE`
Load configuration from file. Defaults to `configs/default.cfg`.

### `--log-level LEVEL`
Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL). Default: INFO.

### `--output-dir DIR`
Set output directory for results and logs. Default: `./nlm_output`.

### `--verbose, -V`
Enable verbose output.

### `--quiet, -q`
Suppress non-essential output.

### `--seed SEED`
Set random seed for reproducible experiments. Default: 42.

### `--parallel N, -p N`
Run in parallel mode with N worker threads. Default: 1.

## Commands

### `demo MODE [ARGS...]`
Run a specific demo mode.

#### Available Modes

##### `phase2`
Run Phase 2: Real Neural Computation demo (basic connectivity, plasticity, STDP verification).

**Arguments:**
- `--neuron-count N`: Number of neurons (default: 500)
- `--steps N`: Simulation steps per test (default: 1000)
- `--plasticity`: Enable plasticity tests (default: true)
- `--stdp`: Enable STDP verification (default: true)
- `--connectivity`: Test basic connectivity (default: true)

**Example:**
```bash
./nlm demo phase2 --neuron-count 1000 --steps 2000 --plasticity
./nlm demo phase2 --stdp-only    # Only run STDP verification
```

##### `phase3`
Run Phase 3: World Interaction demo (experience -> action -> learning loop).

**Arguments:**
- `EPISODES`: Number of episodes (default: 5, positional)
- `STEPS_PER_EPISODE`: Steps per episode (default: 200, positional)
- `--world-size W`: World width/height in cells (default: 20)
- `--vision-range R`: Vision sensor range (default: 16)
- `--enable-curiosity`: Enable curiosity-driven exploration
- `--enable-development`: Enable developmental changes
- `--enable-reward`: Enable reward modulation
- `--enable-planner`: Enable action planning
- `--enable-concepts`: Enable concept formation

**Example:**
```bash
./nlm demo phase3 10 500 --world-size 30 --enable-curiosity
./nlm demo phase3 --enable-all    # Enable all optional features
```

##### `phase4`
Run Phase 4: Cognitive Integration demo (memory, prediction, attention).

**Arguments:**
- `--memory-size M`: Working memory capacity (default: 100)
- `--episodes N`: Number of episodes (default: 3)
- `--enable-associative`: Enable associative memory
- `--enable-planning`: Enable predictive planning
- `--enable-attention`: Enable attentional selection
- `--enable-concept`: Enable concept formation

**Example:**
```bash
./nlm demo phase4 --memory-size 200 --episodes 5 --enable-all
```

##### `phase5`
Run Phase 5: Personality & Sandbox demo (sandbox learning, personality development).

**Arguments:**
- `--sandbox-steps N`: Sandbox simulation steps (default: 100)
- `--enable-pattern-probe`: Run pattern discrimination tests
- `--enable-checkpoint`: Enable checkpoint/save functionality
- `--enable-replay`: Enable memory replay
- `--max-resources R`: Maximum resources to collect (default: 3)
- `--max-hazards H`: Maximum hazards to survive (default: 3)

**Example:**
```bash
./nlm demo phase5 --sandbox-steps 200 --enable-pattern-probe --max-resources 5
```

##### `phase6`
Run Phase 6: Final Integration demo (comprehensive system integration test).

**Arguments:**
- `--quick`: Run quick integration verification only
- `--full`: Run full integration test (default)
- `--verify-only`: Only verify system integration, don't run full experiment
- `--memory-test`: Focus on memory integration
- `--neuromod-test`: Focus on neuromodulation integration
- `--checkpoint-test`: Test checkpoint functionality
- `--replay-test`: Test replay functionality

**Example:**
```bash
./nlm demo phase6 --quick    # Fast integration check
./nlm demo phase6 --full     # Complete integration test
```

### `experiment COMMAND [ARGS...]`
Run experiment suites and comparisons.

#### Available Commands

##### `run`
Run experiment suite.

**Arguments:**
- `--type PHASE`: Phase to run (3, 4, 5, 6)
- `--episodes N`: Number of episodes per run (default: 5)
- `--steps N`: Steps per episode (default: 200)
- `--output FILE`: Save results to file
- `--compare`: Run multiple comparisons

##### `compare`
Compare results between different configurations or phases.

**Arguments:**
- `PHASE1`: First phase to compare
- `PHASE2`: Second phase to compare
- `--runs N`: Number of runs for comparison (default: 3)
- `--metrics M1,M2,...`: Metrics to compare (default: reward,performance,learning)

##### `ablation`
Run ablation studies (component removal tests).

**Arguments:**
- `--component COMP`: Component to remove (working_memory, episodic_memory, curiosity, development, etc.)
- `--baseline`: Compare against baseline without component
- `--iterations N`: Number of iterations (default: 5)

### `benchmark`
Run performance benchmarks.

#### Arguments:
- `--neuron-sizes N1,N2,...`: Neuron counts for benchmarking (default: "100,500,1000,2000")
- `--steps N`: Steps per benchmark (default: 1000)
- `--output FILE`: Save benchmark results to file
- `--format FORMAT`: Output format (table, json, csv) (default: table)
- `--warmup`: Include warmup runs

**Example:**
```bash
./nlm benchmark --neuron-sizes "500,1000,2000" --steps 500 --format json
./nlm benchmark --warmup --output /tmp/benchmark.json
```

### `config`
Manage configuration files.

#### Subcommands:

##### `validate FILE`
Validate configuration file against schema.

##### `schema` 
Print configuration schema.

##### `example` 
Print example configuration file.

##### `set KEY VALUE`
Set configuration value.

##### `get KEY`
Get configuration value.

### `replay FILE`
Replay a saved brain checkpoint.

#### Arguments:
- `FILE`: Checkpoint file to replay
- `--steps N`: Number of steps to replay (default: 1000)
- `--world`: Run in sandbox world (default: false)
- `--visualize`: Enable visualization (requires GUI support)

## Advanced Options

### `--enable-all`
Enable all optional features for demos.

### `--disable-all`
Disable all optional features for demos (faster execution).

### `--preserve-state`
Preserve brain state across demo runs for continuity.

### `--export-state FILE`
Export brain state to file for later import.

### `--import-state FILE`
Import brain state from file for continuation.

## Configuration File

### `configs/default.cfg`

The default configuration file can be customized with:

- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `simulation_timestep`: Time step for simulation
- `connection_probability`: Initial connectivity
- `plasticity_learning_rate`: Plasticity learning rate
- And many more settings...

You can create custom configuration files and use them with `--config FILE`.

## Output Options

### `--output FILE, -o FILE`
Save demo results to file.

### `--output-format FORMAT`
Output format for results (json, txt, csv). Default: txt.

### `--output-append`
Append to existing output file instead of overwriting.

## Interactive Mode

### `./nlm interactive`
Launch interactive shell for manual brain control and exploration.

### `./nlm script FILE`
Run NLM commands from script file.

## Examples

### Run Phase 2 demo with custom settings:
```bash
./nlm demo phase2 --neuron-count 1000 --steps 2000 --log-level DEBUG
```

### Run Phase 3 demo with curiosity and development:
```bash
./nlm demo phase3 8 300 --enable-curiosity --enable-development --verbose
```

### Run benchmark with custom settings:
```bash
./nlm benchmark --neuron-sizes "100,500,1000" --steps 500 --format json
```

### Compare Phase 5 and Phase 6:
```bash
./nlm experiment compare phase5 phase6 --runs 5 --metrics reward,performance,learning
```

### Validate configuration file:
```bash
./nlm config validate configs/my_config.cfg
```

### Run with custom configuration:
```bash
./nlm --config configs/performance.cfg demo phase2 --quiet
```

## Scripting Examples

### Bash script to run all demos:
```bash
#!/bin/bash

echo "Running NLM Phase 2 Demo"
./nlm demo phase2 --steps 1000 > phase2_results.txt 2>&1

echo "Running NLM Phase 3 Demo"
./nlm demo phase3 5 200 > phase3_results.txt 2>&1

echo "Running NLM Phase 6 Demo"
./nlm demo phase6 --quick > phase6_results.txt 2>&1

echo "All demos completed!"
```

### Python script for experiment automation:
```python
import subprocess
import json
import os

def run_nlm_demo(phase, **kwargs):
    cmd = ["nlm", "demo", phase]
    for key, value in kwargs.items():
        cmd.extend([f"--{key.replace('_', '-')}={value}"])
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    return {
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "success": result.returncode == 0
    }

# Run experiments
results = []
for phase in ["phase2", "phase3", "phase6"]:
    result = run_nlm_demo(phase, steps=500, log_level="ERROR")
    results.append({"phase": phase, "result": result})

# Save results
with open("experiment_results.json", "w") as f:
    json.dump(results, f, indent=2)
```

## Troubleshooting

### Common Issues:

1. **Configuration errors**: Check your config file syntax and path.
2. **Missing dependencies**: Ensure all NLM dependencies are installed.
3. **Memory issues**: Try running with smaller neuron counts or disable optional features.
4. **Performance issues**: Consider using `--quiet` mode or reducing simulation steps.

### Getting Help:

```bash
# General help
./nlm --help

# Command help
./nlm demo --help
./nlm experiment --help
./nlm benchmark --help

# Configuration help
./nlm config --help

# Example configurations
./nlm config example > example.cfg
./nlm config schema
```

## Contact

For issues, questions, or feature requests, please refer to the NLM documentation and community resources.

---
*This documentation is part of the NLM (Neural Learning Machine) project.*
*Version: 0.1.0 Phase 6*
*Built with C++20 and modern software engineering practices.*