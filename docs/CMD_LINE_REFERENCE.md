# NLM - Command Line Interface

## Overview

The NLM (Neural Learning Machine) provides a comprehensive command-line interface for running neural simulations, testing, and exploring brain behaviors. This guide covers all available commands and options.

## Quick Start

### Basic Usage

```bash
# Run the main simulation with default settings
./nlm

# Load configuration from file
./nlm --config my_config.cfg

# Run with specific neuron count
./nlm --neuron-count 2000 --steps 5000

# Enable plasticity
./nlm --plasticity-on

# Show advanced options
./nlm --advanced-help
```

### Common Examples

```bash
# Run Phase 2 tests (neural computation demo)
./nlm

# Run with custom configuration
./nlm --config my_config.cfg --steps 10000

# Enable all neuromodulation systems
./nlm --enable-neuromodulation --enable-curiosity --enable-novelty

# Run development simulation
./nlm --development --plasticity-on

# Save checkpoint periodically
./nlm --check-point ./brain_state --checkpoint-interval 1000

# Minimal output
./nlm --quiet --summary-only
```

## Advanced Command Line Options

### Basic Operations

| Option | Description | Default |
|--------|-------------|---------|
| `--help`, `-h` | Show basic help message | - |
| `--version` | Show version information | - |
| `--config FILE` | Load configuration from file | `configs/default.cfg` |

### Execution Modes

| Option | Description | Default |
|--------|-------------|---------|
| `--run-tests` | Run all built-in tests | ✓ |
| `--demo-mode` | Run in demo mode with visualization | ✗ |
| `--benchmark` | Run performance benchmarks | ✗ |
| `--advanced-help` | Show detailed options | ✗ |

### Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `--neuron-count N` | Set number of neurons | 500 |
| `--region-count N` | Set number of brain regions | 1 |
| `--timestep T` | Simulation timestep in seconds | 0.001 |
| `--seed S` | Random seed for reproducibility | 42 |

### Plasticity Options

| Option | Description | Default |
|--------|-------------|---------|
| `--plasticity-on` | Enable plasticity (dynamic network) | ✓ |
| `--plasticity-off` | Disable plasticity (static network) | ✗ |
| `--stdp-ltp W` | STDP LTP weight | 0.02 |
| `--stdp-ltd W` | STDP LTD weight | 0.015 |
| `--stdp-tau T` | STDP time constant | 20.0 |

### Development Options

| Option | Description | Default |
|--------|-------------|---------|
| `--development` | Enable developmental stages | ✓ |
| `--no-development` | Disable developmental stages | ✗ |

### Neuromodulation Options

| Option | Description | Default |
|--------|-------------|---------|
| `--enable-neuromodulation` | Enable all neuromodulation systems | ✓ |
| `--disable-neuromodulation` | Disable neuromodulation systems | ✗ |
| `--enable-curiosity` | Enable curiosity-driven exploration | ✓ |
| `--disable-curiosity` | Disable curiosity | ✗ |
| `--enable-novelty` | Enable novelty detection | ✓ |
| `--disable-novelty` | Disable novelty detection | ✗ |
| `--enable-prediction` | Enable prediction system | ✓ |
| `--disable-prediction` | Disable prediction system | ✗ |
| `--enable-planning` | Enable neural planning | ✓ |
| `--disable-planning` | Disable neural planning | ✗ |
| `--enable-memory` | Enable memory systems | ✓ |
| `--disable-memory` | Disable memory systems | ✗ |
| `--enable-attention` | Enable attentional mechanisms | ✓ |
| `--disable-attention` | Disable attentional mechanisms | ✗ |

### Memory Options

| Option | Description | Default |
|--------|-------------|---------|
| `--checkpoint-dir DIR` | Checkpoint directory | `./checkpoints` |
| `--checkpoint-interval I` | Checkpoint interval in steps | 10000 |
| `--checkpoint-keep N` | Number of checkpoints to keep | 5 |

### Logging Options

| Option | Description | Default |
|--------|-------------|---------|
| `--log-level LEVEL` | Set log level | `INFO` |
| `--log-file FILE` | Log output to file | - |

### Output Options

| Option | Description | Default |
|--------|-------------|---------|
| `--quiet` | Minimal output (errors only) | ✗ |
| `--summary-only` | Only show final summary | ✗ |
| `--json-output FILE` | Export results to JSON | - |
| `--csv-output FILE` | Export results to CSV | - |

### Environment Options

| Option | Description | Default |
|--------|-------------|---------|
| `--environment NAME` | Environment type | `GridWorld` |
| `--width W` | Environment width | 10 |
| `--height H` | Environment height | 10 |

### Performance Options

| Option | Description | Default |
|--------|-------------|---------|
| `--threads N` | Number of CPU threads | 0 (auto) |
| `--vectorize` | Enable SIMD vectorization | ✓ |
| `--optimize` | Enable performance optimizations | ✓ |
| `--profile` | Enable performance profiling | ✗ |

## Configuration File Format

NLM uses a simple key=value format for configuration files:

```
# Example configuration file
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 2
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Configuration File Comments

- Lines starting with `#` are comments
- Empty lines are ignored
- Quoted values are supported
- Boolean values: `true`, `false`
- Floating point values support scientific notation

## Running Different Phases

### Phase 2: Neural Computation

```bash
# Standard Phase 2 test (this is what ./nlm runs by default)
./nlm

# Phase 2 with custom configuration
./nlm --config phase2.cfg

# Phase 2 with enhanced logging
./nlm --log-level DEBUG --log-file phase2.log
```

### Phase 3: World Interaction

```bash
# Run Phase 3 demo (if available)
./nlm_phase3_demo

# Phase 3 with agent interface
./nlm --agent-mode --development

# Phase 3 with custom world
./nlm --environment OpenSpace --width 20 --height 20
```

### Phase 4: Emerging Cognition

```bash
# Run Phase 4 demo (if available)
./nlm_phase4_demo

# Phase 4 with advanced cognition features
./nlm --enable-planning --enable-attention --enable-prediction
```

### Phase 6: Final Integration

```bash
# Run Phase 6 integration test (if available)
./nlm_phase6_demo

# Phase 6 with all systems enabled
./nlm --enable-neuromodulation --enable-curiosity --enable-novelty --enable-prediction --enable-planning --enable-memory --enable-attention
```

## Common Issues and Solutions

### "My brain isn't doing anything"

**Problem**: The brain appears inactive during simulation.

**Solutions**:
1. Ensure `--plasticity-on` is set (default is on)
2. Check that `--steps` is large enough (default: 2000)
3. Verify `--neuron-count` is appropriate (default: 500)
4. Enable proper neuromodulation: `--enable-neuromodulation`
5. Check log output with `--log-level DEBUG`

### "The agent isn't moving"

**Problem**: The agent in the world doesn't respond to stimuli.

**Solutions**:
1. Ensure world is being updated: `--world-update-rate` (default: 30 Hz)
2. Check that sensory input is being processed
3. Verify motor decoding is working: `--enable-memory`
4. Debug with: `--log-level INFO --log-file debug.log`

### "Everything is 0"

**Problem**: All outputs are zero.

**Solutions**:
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - ensure sensory input is connected
3. Enable proper neuromodulation: `--enable-curiosity --enable-novelty`
4. Check development is working: `--development`

### "Performance issues"

**Problem**: Simulation is too slow.

**Solutions**:
1. Enable vectorization: `--vectorize`
2. Optimize for CPU: `--threads 4`
3. Reduce neuron count: `--neuron-count 1000`
4. Use checkpoints: `--checkpoint-interval 5000`
5. Profile performance: `--profile`

## Environment Setup

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Installing Python Library

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

### Running Different Builds

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Release build (optimized, no debug symbols)
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
```

## Configuration Examples

### Fast Debug Test

```bash
# Quick test to verify everything works
./nlm --neuron-count 100 --steps 100 --quiet --summary-only
```

### Realistic Simulation

```bash
# More realistic brain with longer simulation
./nlm \
  --neuron-count 5000 \
  --region-count 4 \
  --timestep 0.0001 \
  --steps 50000 \
  --plasticity-on \
  --development \
  --enable-neuromodulation \
  --enable-curiosity \
  --enable-novelty \
  --enable-prediction \
  --enable-planning \
  --enable-memory \
  --enable-attention \
  --checkpoint-dir ./brain_states \
  --checkpoint-interval 20000
```

### Scientific Research

```bash
# Experiment for research paper
./nlm \
  --config research.cfg \
  --log-level INFO \
  --log-file research.log \
  --json-output results.json \
  --csv-output data.csv \
  --checkpoint-dir ./checkpoints \
  --checkpoint-keep 10 \
  --validate-only
```

### Performance Testing

```bash
# Benchmark the system
./nlm --benchmark --neuron-count 1000 --steps 10000 --vectorize

# Profile with detailed output
./nlm --profile --log-level DEBUG --log-file profile.log
```

## Troubleshooting

### "CMake cannot find Python"

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

### "Compilation errors"

```bash
# Ensure C++20 compiler support
# Check CMake version >= 3.16
# Verify all source files are present
```

### "ImportError: No module named 'pynlm'"

```bash
pip install --force-reinstall .
```

### "Segmentation faults"

```bash
# Check that `initialize()` is called before `step()`
# Ensure `reset()` is called before re-running simulation
# Verify configuration values are within expected ranges
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Final Phase 6 audit results
