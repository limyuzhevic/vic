# NLM (熙然) - Neural Learning Machine
Phase 2: Real Neural Computation

## Overview

This phase implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

## Command-Line Interface

NLM now features an enhanced command-line interface for running and managing neural experiments:

### Basic Usage

```bash
# Run all tests with default configuration
nlm

# Show help information
nlm --help

# Run specific tests
nlm --test basic --test plasticity --test stdp

# Run tests with verbose logging
nlm --verbose --test all

# Use custom configuration file
nlm --config my_config.cfg --test basic

# Enable performance profiling
nlm --profile --test plasticity
```

### Available Commands and Options

#### `--config <file>`
Load configuration from a file (default: `configs/default.cfg`)

#### `--test <name>`
Run specific test(s). Can be repeated to run multiple tests:
- `basic`: Run basic neural connectivity test
- `plasticity`: Run plasticity learning experiment
- `stdp`: Run STDP verification test
- `all`: Run all tests (default if no tests specified)

#### `--verbose`
Enable verbose logging for detailed output

#### `--profile`
Enable performance profiling

#### `--help` or `-h`
Show this help message and exit

### Configuration File Format

Configuration files use a simple `key = value` format:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
dopamine_baseline = 0.1
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
```

### Configuration Presets

For different experimental scenarios, consider using these configuration presets:

#### `configs/experiment.cfg` - Development/Testing
```ini
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500  # Smaller for faster testing
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001f
pruning_rate = 0.00001f
dopamine_baseline = 0.1
log_level = INFO
log_to_file = false
max_simulation_steps = 10000
```

#### `configs/production.cfg` - Production/Simulation
```ini
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000  # Full-scale simulation
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
dopamine_baseline = 0.0
log_level = INFO
log_to_file = true
log_filename = nlm_production.log
max_simulation_steps = 100000
```

### Test Descriptions

#### Basic Neural Connectivity Test
Tests that neurons can spike and propagate signals through the network. Injects current into sensory neurons and monitors for spikes.

#### Plasticity Learning Experiment
Demonstrates measurable synaptic changes through Hebbian and STDP learning. Records initial and final weights, then computes statistics to verify learning occurred.

#### STDP Verification
Specifically tests the Spike-Timing-Dependent Plasticity rule by creating correlated pre-before-post activity patterns to verify LTP (potentiation).

### Configuration Options

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `random_seed` | Random seed for reproducible experiments | 42 | Any integer |
| `simulation_timestep` | Simulation timestep in seconds | 0.001 | 0.0001 - 0.01 |
| `neuron_count` | Number of neurons in the brain | 1000 | 100 - 10000 |
| `region_count` | Number of brain regions | 1 | 1 - 10 |
| `connection_probability` | Connection probability for random connectivity | 0.1 | 0.01 - 0.5 |
| `stdp_ltp_weight` | STDP LTP learning rate | 0.01 | 0.001 - 0.1 |
| `stdp_ltd_weight` | STDP LTD learning rate | 0.012 | 0.001 - 0.1 |
| `stdp_tau` | STDP time constant (ms) | 20.0 | 1.0 - 100.0 |
| `synaptogenesis_rate` | Structural plasticity synaptogenesis rate | 0.001 | 0.00001 - 0.01 |
| `pruning_rate` | Structural plasticity pruning rate | 0.0001 | 0.00001 - 0.01 |
| `dopamine_baseline` | Baseline dopamine level | 0.0 | 0.0 - 1.0 |
| `log_level` | Logging verbosity level | INFO | DEBUG, INFO, WARNING, ERROR |
| `log_to_file` | Enable/disable logging to file | false | true, false |
| `log_filename` | Output log filename | nlm.log | Any valid filename |

### Building and Running

```bash
# Build with CMake (typical setup)
cmake -B build
cmake --build build

# Run the executable
./build/nlm

# Or with custom options
./build/nlm --test basic --verbose
```

### Advanced Usage

#### Custom Configuration

Create your own configuration file:

```bash
# Create minimal config
./build/nlm --config custom.cfg

# Custom config example
cat > custom.cfg << EOF
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.05
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.014
stdp_tau = 25.0
dopamine_baseline = 0.05
log_level = DEBUG
log_to_file = true
log_filename = debug_nlm.log
EOF

# Run with custom config
./build/nlm --config custom.cfg --test all --verbose
```

#### Running Specific Test Combinations

```bash
# Fast testing - only basic connectivity
./build/nlm --test basic

# Learning-focused - plasticity and STDP
./build/nlm --test plasticity --test stdp --verbose

# Comprehensive testing - all tests with profiling
./build/nlm --test all --profile
```

### Troubleshooting

#### No spikes generated
- Check neuron parameters (threshold, resting potential)
- Verify current injection values
- Ensure configuration has sufficient neurons

#### Configuration file not found
- Use absolute path or ensure file is in `configs/` directory
- Create `configs/default.cfg` if it doesn't exist

#### Memory issues
- Reduce `neuron_count` in configuration
- Monitor system resources during long simulations

#### Performance issues
- Use `--profile` flag to identify bottlenecks
- Consider reducing `max_simulation_steps`
- Use production configuration for steady-state operation

### Files and Directories

- `src/main.cpp` - Main entry point with test runner
- `configs/default.cfg` - Default configuration
- `src/core/Config/Config.cpp` - Configuration management (JSON/YAML parser TODO)
- `src/neuromodulation/Neuromodulator.cpp` - Neuromodulation TODOs to be implemented

### Future Enhancements

Planned improvements for later phases:
- Real-time visualization interface
- Advanced action selection algorithms
- Complex environmental interactions
- Advanced memory consolidation mechanisms

## License

Copyright (c) 2026 NLM Project
