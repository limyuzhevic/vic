# NLM User Guide
# Neural Learning Machine - User Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain project that investigates computational brain-like systems through real neural computation. This guide provides comprehensive instructions for using NLM, from basic setup to advanced experimentation.

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Command Line Interface Reference](#command-line-interface-reference)
3. [Configuration Management](#configuration-management)
4. [Running Experiments](#running-experiments)
5. [Performance Tips](#performance-tips)
6. [Troubleshooting](#troubleshooting)
7. [Advanced Features](#advanced-features)

## Quick Start Guide

### Installation and Setup

The NLM project comes pre-installed with all necessary dependencies. To get started:

```bash
# Navigate to the project root
cd /path/to/nlm

# Build the project (first time only)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### First Run

Run the enhanced NLM application with the Phase 6 integration demo:

```bash
./nlm
```

This will run a comprehensive integration test that demonstrates all major brain systems working together.

### Running Specific Phases

You can run specific experimental phases using the command line:

```bash
# Phase 3: World interaction demo
./nlm --mode phase3

# Phase 4: Cognition demo
./nlm --mode phase4

# Phase 5: Performance demo
./nlm --mode phase5

# Phase 6: Full integration demo
./nlm --mode phase6
```

### Basic Configuration

Create a basic configuration file at `configs/myconfig.cfg`:

```cfg
# NLM Configuration File
# This file sets basic parameters for neural network simulation

random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
mode = phase6
max_steps = 1000
```

Load this configuration:

```bash
./nlm --config configs/myconfig.cfg
```

## Command Line Interface Reference

### Core Options

| Option | Description | Example |
|--------|-------------|---------|
| `--mode <phase>` | Run specific experimental phase | `--mode phase3` |
| `--config <file>` | Load configuration from file | `--config configs/myconfig.cfg` |
| `--help` | Show help message | `--help` |
| `--version` | Show version information | `--version` |

### Advanced Options

| Option | Description | Example |
|--------|-------------|---------|
| `--set <key>=<value>` | Override configuration values | `--set neuron_count=1000` |
| `--validate-config` | Validate configuration and exit | `--validate-config` |
| `--print-config` | Print current configuration and exit | `--print-config` |
| `--quiet` | Suppress progress output | `--quiet` |
| `--verbose` | Enable verbose logging | `--verbose` |
| `--monitor` | Enable real-time performance monitoring | `--monitor` |
| `--benchmark` | Run performance benchmarks | `--benchmark` |
| `--max-steps <n>` | Set maximum simulation steps | `--max-steps 5000` |
| `--timestep <t>` | Set simulation timestep | `--timestep 0.001` |
| `--neuron-count <n>` | Set neuron count | `--neuron-count 1000` |
| `--seed <n>` | Set random seed | `--seed 12345` |
| `--json-output <file>` | Save results to JSON file | `--json-output results.json` |
| `--csv-output <file>` | Save metrics to CSV file | `--csv-output metrics.csv` |

### Environment Variables

NLM supports the following environment variables for configuration:

| Variable | Description | Default |
|----------|-------------|---------|
| `NLM_RANDOM_SEED` | Random seed for reproducible results | `42` |
| `NLM_SIMULATION_TIMESTEP` | Simulation timestep in seconds | `0.001` |
| `NLM_NEURON_COUNT` | Number of neurons in the network | `1000` |
| `NLM_REGION_COUNT` | Number of neural regions | `1` |
| `NLM_CONNECTION_PROBABILITY` | Probability of synaptic connections | `0.15` |
| `NLM_CONFIG_PATH` | Configuration directory path | System default |

### Configuration File Format

NLM configuration files use a simple key=value format with optional comments:

```cfg
# This is a configuration file
# Comments start with # or /

# Basic parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Additional settings
mode = phase6
max_steps = 1000
quiet = false
verbose = false
```

## Configuration Management

### Default Configuration

The enhanced configuration system provides presets for different experimental phases:

- **Phase 3**: 500 neurons, 2 regions, 2000 steps
- **Phase 4**: 800 neurons, 1 region, 3000 steps
- **Phase 5**: 1500 neurons, 3 regions, 5000 steps
- **Phase 6**: 1000 neurons, 1 region, 10000 steps

### Schema Validation

All configuration values are validated against predefined schemas. The enhanced configuration system ensures:

- Type checking (int, double, string, boolean)
- Range validation where applicable
- Required field checking
- Allowed value enumeration

### Configuration Precedence

Configuration values follow this precedence (highest to lowest):
1. Command line arguments (`--set`)
2. Environment variables
3. Configuration file
4. Defaults

## Running Experiments

### Phase 3: World Interaction Demo

The Phase 3 demo demonstrates basic world interaction capabilities:

```bash
./nlm --mode phase3 --max-steps 2000 --verbose
```

**Expected Behavior:**
- Brain initializes with 500 neurons and 2 regions
- Agents interact with a simulated environment
- Reward prediction error drives learning
- Memory systems integrate with neural processing

### Phase 4: Cognition Demo

The Phase 4 demo showcases advanced cognitive capabilities:

```bash
./nlm --mode phase4 --neuron-count 800 --auto-checkpoint 300
```

**Features Demonstrated:**
- Neural prediction systems
- Working memory with persistent activity
- Concept formation from experience
- Attention and competitive selection
- Predictive planning

### Phase 5: Performance Demo

The Phase 5 demo emphasizes performance optimizations:

```bash
./nlm --mode phase5 --benchmark --max-steps 5000
```

**Optimizations Demonstrated:**
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Integration Demo

The Phase 6 demo provides the complete integration test:

```bash
./nlm --mode phase6 --monitor --json-output integration_results.json
```

**Integration Features:**
- All memory systems connected to neural processing
- Neuromodulation affecting plasticity and dynamics
- Prediction integrated with learning
- Development affecting plasticity rates
- Checkpoint save/load functionality
- Replay and consolidation mechanisms

## Performance Tips

### Memory Management

For large-scale simulations, consider these optimizations:

```bash
# Use memory pools for reduced allocation overhead
./nlm --mode phase5 --neuron-count 2000

# Enable checkpointing for long runs
./nlm --mode phase6 --auto-checkpoint 600

# Use sparse connectivity for efficiency
./nlm --set connection_probability=0.05
```

### Simulation Parameters

- **Neuron Count**: Trade-off between realism and performance
  - Small networks (500 neurons): Fast, suitable for testing
  - Medium networks (1000 neurons): Good balance
  - Large networks (2000+ neurons): More realistic, slower

- **Timestep**: Smaller timesteps increase accuracy but decrease speed
  - 0.001s: Standard for LIF neurons
  - 0.0001s: Higher precision, slower
  - 0.01s: Faster, less accurate

### Hardware Utilization

- **CPU**: Multiple threads for parallel processing
- **Memory**: Monitor with `--monitor` flag
- **Storage**: Use SSD for checkpoint files

## Troubleshooting

### Common Issues

#### "Failed to initialize brain!"

**Cause**: Configuration validation failed or incompatible settings.

**Solution**:
```bash
# Validate your configuration
./nlm --validate-config

# Use defaults
./nlm

# Check configuration
./nlm --print-config
```

#### Slow Performance

**Cause**: Large neuron count or timestep too small.

**Solution**:
```bash
# Reduce neuron count
./nlm --neuron-count 500

# Increase timestep
./nlm --timestep 0.001

# Disable unnecessary features
./nlm --quiet --mode phase3
```

#### Missing Dependencies

**Cause**: nlohmann/json library not found.

**Solution**:
```bash
# Install dependencies
# Ubuntu/Debian:
apt-get install nlohmann-json3-dev

# Compile with JSON support
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Getting More Information

- **Help**: `./nlm --help`
- **Verbose output**: `./nlm --verbose`
- **Logging**: Check application logs in `logs/` directory
- **Documentation**: Visit the NLM documentation website

## Advanced Features

### Custom Configuration

Create custom configuration files in the `configs/` directory:

```bash
# Create custom config
cat > configs/custom.cfg << EOF
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
mode = phase6
EOF

# Use custom config
./nlm --config configs/custom.cfg
```

### Scripted Experiments

Automate experiment runs with shell scripts:

```bash
#!/bin/bash
# run_experiments.sh

for seed in 42 123 456; do
    echo "Running experiment with seed $seed"
    ./nlm --seed $seed --mode phase3 --max-steps 1000 --json-output results_seed_$seed.json
    echo "Completed seed $seed"
done
```

### Performance Analysis

Analyze performance with benchmark mode:

```bash
./nlm --mode phase5 --benchmark --neuron-count 500 --neuron-count 1000 --neuron-count 2000
```

This will compare performance across different scales.

## Examples Directory

The `examples/` directory contains additional scripts and configurations:

- `simple_demo.sh`: Basic usage example
- `parameter_study.sh`: Systematic parameter exploration
- `comparison_demo.sh`: Compare different phases
- `visualization_script.py`: Generate output visualizations

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Authors

NLM Research Team

For questions, bug reports, or feature requests, please visit the NLM GitHub repository.

---

*Last updated: 2026-09-30*
*NLM Version: 0.1.0 (Enhanced)*