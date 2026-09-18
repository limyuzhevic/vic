# NLM Advanced Command Line Interface

## Overview
The NLM (Neural Learning Machine) now supports advanced command line options to improve usability, debugging, and experimentation capabilities.

## Available Commands

### Basic Mode
- **Default**: `./nlm` - Run with default Phase 2 experiments

### Advanced Options

#### `--help`
Show help information and available options

#### `--version`  
Display NLM version information

#### `--save-config <filename>`
Save the current configuration to a JSON file for reuse

#### `--load-config <filename>`
Load configuration from a JSON file before running

#### `--debug` 
Enable detailed debug logging for troubleshooting

#### `--benchmark <neuron_count> <steps>`
Run performance benchmark
- Default: `--benchmark 500 1000`
- Example: `--benchmark 1000 500`

#### `--benchmark-file <filename>`
Save benchmark results to specified file (default: `benchmark_results.txt`)

#### `--checkpoint <filename>`
Save brain state to checkpoint file after completion

#### `--multi-phase` 
Run all available demo phases (Phase 3, Phase 4, Phase 6)

#### `--develop` 
Enable developmental mode for longer learning simulations

#### `--curiosity` 
Enable curiosity-driven exploration behavior

#### `--plasticity` 
Enable all plasticity mechanisms (STDP, Hebbian, structural)

### Complex Examples

#### Save and reload configuration
```bash
./nlm --save-config my_config.json
# Later:
./nlm --load-config my_config.json
```

#### Run custom benchmark
```bash
./nlm --benchmark 2000 300 --benchmark-file perf_results.txt
```

#### Development mode with debugging
```bash
./nlm --develop --debug --checkpoint dev_checkpoint.bin
```

#### Multi-phase demo with all features
```bash
./nlm --multi-phase --curiosity --plasticity --save-config demo_config.json
```

### Configuration File Format (JSON)

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.05,
        "random_seed": 42,
        "simulation_timestep": 0.001
    },
    "plasticity": {
        "stdp": {"enable": true, "learning_rate": 0.001},
        "hebbian": {"enable": true, "learning_rate": 0.01},
        "structural": {"enable": true, "synaptogenesis_rate": 0.0001}
    },
    "neuromodulation": {
        "dopamine": {"scale": 1.0},
        "curiosity": {"enable": true, "threshold": 0.3},
        "novelty": {"enable": true, "decay_rate": 0.99}
    },
    "development": {
        "enable": true,
        "plasticity_modifier": [1.0, 0.8, 0.5, 0.2],
        "stages": [60.0, 300.0, 900.0]
    }
}
```

## Configuration Override

Command line arguments override configuration file values. Use command line syntax:
- `--key value` (for simple values)
- `--complex.key value` (for nested values)

Example:
```bash
./nlm --load-config config.json --brain.neuron_count 2000 --plasticity.stdp.learning_rate 0.002
```

## Benchmark Results Format

Benchmark output includes:
- Neuron count
- Synapse count  
- Steps per second
- Spikes per step
- Synapses per neuron

Sample output:
```
╔═══════════════════════════════════════════════════════════════════════════════════════════╗
║                     NLM Performance Benchmark Results                                   ║
╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣
║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║
╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣
║     500      ║     1500     ║    42000  ║     0.85    ║      3.00           ║
║    1000      ║     4500     ║    38000  ║     0.82    ║      4.50           ║
║    2000      ║     9500     ║    35000  ║     0.78    ║      4.75           ║
╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝
```

## Development Mode Features

When using `--develop`, NLM simulates:
- **Extended learning periods** (up to 15 minutes simulation time)
- **Developmental stages** (Initial → Critical Period → Maturation → Adult)
- **Progressive plasticity changes** (high initial learning, decreasing with age)
- **Experience-dependent maturation** (synaptogenesis and pruning)
- **Memory consolidation** (replay and checkpointing)

## Debug Mode

Debug mode enables:
- **Detailed logging** of all system components
- **Neuron state tracking** (membrane potential, firing rates)
- **Synapse connection monitoring** (weight changes, eligibility traces)
- **Event timeline logging** (spike times, sensory inputs)
- **Performance metrics** (memory usage, execution time)

## Error Handling

Common error codes:
- `0`: Success
- `1`: Brain initialization failed
- `2`: Configuration file not found
- `3`: Invalid command line arguments
- `4`: Benchmark execution failed

## Scripts and Automation

### Run Script (for bash)

```bash
#!/bin/bash
# nlm_runner.sh - NLM execution script with common configurations

NLM_EXEC="./nlm"

function run_with_config {
    local name="$1"
    shift
    echo "=== Running $name ==="
    $NLM_EXEC $*
    echo "=== $name complete ==="
}

# Quick test
run_with_config "Quick Test" --neuron_count 100 --steps 500

# Full learning
run_with_config "Full Learning" --develop --curiosity --plasticity --checkpoint learning.bin

# Performance test
run_with_config "Performance" --benchmark 1000 500 --benchmark-file results.txt

# Debug session
run_with_config "Debug Session" --debug --develop
```

### Python Wrapper (for automation)

```python
def run_nlm_experiment(config_file=None, args=None):
    """Python interface for running NLM experiments"""
    import subprocess
    import json
    
    cmd = ["./nlm"]
    
    if config_file:
        cmd.extend(["--load-config", config_file])
    
    if args:
        cmd.extend(args)
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        print("Experiment completed successfully!")
        print(result.stdout)
        return True
    else:
        print(f"Experiment failed with code {result.returncode}")
        print(f"Error: {result.stderr}")
        return False

# Example usage
if run_nlm_experiment("configs/large_config.json", [
    "--benchmark", "5000", "1000",
    "--benchmark-file", "experiments/performance.txt"
]):
    print("Benchmark completed!")
```

## Advanced User Features

1. **Custom configurations**: Save and load experiment settings
2. **Performance optimization**: Benchmark and tune parameters
3. **Development debugging**: Detailed logging for brain development
4. **Integration testing**: Multi-phase demo runs
5. **State management**: Checkpoint and resume experiments
6. **Scripting support**: Bash and Python interfaces
7. **Experiment tracking**: Save benchmark results for comparison

These enhancements make NLM more accessible for beginners while providing powerful tools for advanced users to experiment with brain simulation, learning, and development.