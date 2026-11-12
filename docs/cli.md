# NLM - Command Line Interface for Brain Simulation

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain system that simulates spiking neural computation with plasticity, learning, and cognitive capabilities.

This CLI provides:
- Run brain simulations with configurable parameters
- Load/save brain checkpoints
- Configure simulation parameters from files or command line
- Run predefined experiments
- Monitor simulation progress and statistics

## Installation

Ensure NLM is built with:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
```

The `nlm` executable will be created in the build directory.

## Usage

### Basic Usage

```bash
nlm [OPTIONS]
```

### Common Options

```bash
# Run a simple simulation
nlm --neuron-count 500 --steps 1000

# Load a saved configuration
nlm --config configs/my_brain.cfg --steps 2000

# Save brain state to file
nlm --neuron-count 1000 --steps 1000 --save brain_state.bin

# Run with custom command-line parameters
nlm --neuron-count 500 --region-count 2 --connection-prob 0.1 --stdp-ltp-weight 0.01 --stdp-ltd-weight 0.12

# Run Phase 6 integration test
nlm --phase 6 --quick

# Run experiment with detailed logging
nlm --neuron-count 100 --steps 500 --log-level Info --enable-stats
```

### Configuration Options

```bash
--config FILE          Load configuration from file (JSON or key=value format)
--neuron-count N      Number of neurons (default: 500)
--region-count N      Number of brain regions (default: 1)
--connection-prob P   Connection probability (default: 0.15)
--steps S             Number of simulation steps (default: 1000)
--save FILE           Save brain state to file after simulation
--load FILE           Load brain state from file before simulation

# Plasticity parameters
--stdp-ltp-weight W    STDP long-term potentiation weight (default: 0.02)
--stdp-ltd-weight W    STDP long-term depression weight (default: 0.015)
--stdp-tau T          STDP time constant (default: 20.0)
--hebbian-ltp-weight W Hebbian LTP weight (default: 0.01)
--hebbian-ltd-weight W Hebbian LTD weight (default: 0.01)
--synaptogenesis-rate R Structural plasticity rate (default: 0.0001)
--pruning-rate R      Structural plasticity pruning rate (default: 0.00001)

# Development parameters
--development-stage S  Development stage (Initial, CriticalPeriod, Maturation, Adult, Aging)
--enable-development   Enable development plasticity (default: true)

# Neuromodulation
--enable-reward-modulation      Enable dopamine-based reward modulation
--enable-curiosity              Enable curiosity-based exploration
--enable-novelty                Enable novelty detection
--enable-prediction-error        Enable prediction error signals

# Logging and output
--log-level LEVEL        Logging level (Debug, Info, Warning, Error, Critical)
--log-file FILE          Log output to file in addition to console
--quiet                 Suppress informational messages
--silent                Only show errors and critical messages

# Performance and debugging
--enable-stats          Collect and display performance statistics
--enable-profiling      Enable performance profiling (adds overhead)
--benchmark            Run performance benchmark mode
--help                  Show this help message
--version               Show version information
```

### Configuration File Format

NLM supports two configuration file formats:

#### Simple key=value format (default)

```
# NLM Configuration File
# neuron_count = 500
# region_count = 2
# connection_probability = 0.1
# stdp_ltp_weight = 0.01
# stdp_ltd_weight = 0.012
# stdp_tau = 20.0
# synaptogenesis_rate = 0.0001
# pruning_rate = 0.00001
# enable_development = true
# enable_reward_modulation = true
# enable_curiosity = true
# enable_novelty = true
# enable_prediction_error = true
```

#### JSON format (experimental)

```json
{
    "nlm": {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.1,
        "stdp": {
            "ltp_weight": 0.01,
            "ltd_weight": 0.012,
            "tau": 20.0
        },
        "hebbian": {
            "ltp_weight": 0.01,
            "ltd_weight": 0.01
        },
        "structural_plasticity": {
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        },
        "development": {
            "enabled": true,
            "stage": "Adult"
        },
        "neuromodulation": {
            "reward_modulation": true,
            "curiosity": true,
            "novelty": true,
            "prediction_error": true
        },
        "simulation": {
            "timestep": 0.001,
            "random_seed": 42
        }
    }
}
```

### Examples

#### Example 1: Simple Brain Simulation

```bash
nlm --neuron-count 500 --steps 1000 --log-level Info --enable-stats
```

Run a basic brain simulation with 500 neurons for 1000 steps, showing progress and statistics.

#### Example 2: Learning Experiment

```bash
nlm --neuron-count 1000 --steps 5000 --stdp-ltp-weight 0.03 --stdp-ltd-weight 0.02 \
    --enable-reward-modulation --enable-curiosity --enable-development \
    --save learning_experiment.bin --log-level Info
```

Run a learning experiment with enhanced plasticity and neuromodulation.

#### Example 3: Quick Integration Test

```bash
nlm --phase 6 --quick --log-level Warning
```

Run the Phase 6 integration test in quick mode (reduced settings).

#### Example 4: Interactive Session

```bash
nlm --neuron-count 200 --steps 2000 --enable-development --enable-curiosity \
    --log-level Info --log-file simulation.log
```

Run an interactive simulation session with logging to file.

### Configuration Examples

#### Create a custom configuration file

```bash
cat > my_config.cfg << EOF
# Custom NLM Configuration for Learning Experiment
neuron_count = 800
region_count = 3
connection_probability = 0.12
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_tau = 25.0
hebbian_ltp_weight = 0.015
hebbian_ltd_weight = 0.012
synaptogenesis_rate = 0.00015
pruning_rate = 0.000012
enable_development = true
enable_reward_modulation = true
enable_curiosity = true
enable_novelty = true
enable_prediction_error = true
random_seed = 12345
EOF

nlm --config my_config.cfg --steps 3000 --save my_brain.bin --enable-stats
```

#### Command line overrides

```bash
# Use config file as base but override specific parameters
nlm --config my_config.cfg --neuron-count 1000 --steps 5000 --log-level Debug

# Disable certain features
nlm --enable-development --enable-curiosity \
    --disable-reward-modulation --disable-novelty
```

### Advanced Usage

#### Batch Processing

```bash
#!/bin/bash
# Run multiple simulations with different parameters

for neurons in 500 1000 2000; do
    for steps in 1000 5000 10000; do
        echo "Running simulation with $neurons neurons for $steps steps"
        nlm --neuron-count $neurons --steps $steps \
            --save "batch_${neurons}_${steps}.bin" \
            --quiet 2>&1 | tail -n 5
    done
done
```

#### Experiment Automation

```bash
# Run a complete experiment with automated analysis
nlm --phase 6 --quick --enable-stats --save experiment_results.bin \
    | tee experiment_output.txt

# Extract key metrics
python3 -c "
import json
import re

# Parse output file for key metrics
with open('experiment_output.txt', 'r') as f:
    content = f.read()

# Extract statistics
metrics = {}
for line in content.split('\n'):
    if 'firing rate:' in line:
        match = re.search(r'firing rate:\s+(\d+\.?\d*)', line)
        if match:
            metrics['avg_firing_rate'] = float(match.group(1))
    elif 'total spikes:' in line:
        match = re.search(r'total spikes:\s+(\d+)', line)
        if match:
            metrics['total_spikes'] = int(match.group(1))

print('Experiment Results:', json.dumps(metrics, indent=2))
"
```

### Troubleshooting

#### Common Issues and Solutions

1. **Configuration file not found**
   ```bash
   nlm --config non_existent.cfg
   ```
   Solution: Ensure the configuration file exists and is readable.

2. **Permission denied**
   ```bash
   nlm --save /root/brain_state.bin
   ```
   Solution: Run with appropriate permissions or save to a writable location.

3. **Memory usage too high**
   ```bash
   nlm --neuron-count 10000 --steps 1000
   ```
   Solution: Try reducing neuron count or simulation steps.

4. **Python bindings not working**
   ```bash
   python3 -c "import pynlm; print('OK')"
   ```
   Solution: Reinstall Python bindings or check build configuration.

#### Getting More Information

- Use `--log-level Info` to see progress messages
- Use `--log-file simulation.log` to save logs to file
- Check the NLM documentation in the `docs/` directory
- Visit the NLM project website for examples and tutorials

## Exit Codes

- `0`: Success
- `1`: Error (invalid arguments, file not found, etc.)
- `2`: Simulation terminated by user (Ctrl+C)
- `3`: Configuration error
- `4`: Simulation error

## Environment Variables

- `NLM_CONFIG_PATH`: Default path for configuration files
- `NLM_LOG_LEVEL`: Default log level (Debug, Info, Warning, Error, Critical)
- `NLM_PYTHON_PATH`: Path to Python bindings (if not installed system-wide)

## Technical Details

### Configuration Loading Priority

1. Command-line arguments (highest priority)
2. Configuration file
3. Default values (lowest priority)

### Memory Management

- Brain state is saved to file using compression
- Checkpoint files can be loaded to resume simulations
- Memory usage scales with neuron count and connectivity

### Performance

- Simulation uses event-driven computation
- Parallel processing for spike propagation
- Configurable timestep for accuracy vs. performance tradeoff

## Related Commands

The NLM project provides several related executables:

- `nlm_phase3_demo`: Phase 3 world interaction demo
- `nlm_phase4_demo`: Phase 4 cognitive capabilities demo
- `nlm_phase5_demo`: Phase 5 performance and sandbox demo
- `nlm_phase6_demo`: Phase 6 integration test
- `nlm_benchmark`: Performance benchmark

These can be run directly or through this CLI with the `--phase` option.

## License

MIT License - See LICENSE file for details.

---

*This documentation is automatically generated from the NLM source code.*
*For the latest information, see the NLM GitHub repository.*
