# NLM (Neural Learning Machine) - Quick Start Guide

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. This document provides a quick start guide for getting up and running with the NLM Phase 2 Real Neural Computation system.

## Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (optional, for Python bindings)

## Quick Installation

### Option 1: Build from Source (Recommended for Development)

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Option 2: Using Python Package

```bash
# Install dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python package
pip install .
```

## Quick Start with NLM Executable

### Basic Usage

```bash
# Run all tests (default)
nlm

# Show help and available options
nlm --help

# Show version information
nlm --version
```

### Running Specific Tests

```bash
# Run only basic connectivity test
nlm --mode basic

# Run plasticity experiment
nlm --mode plasticity

# Run STDP verification only
nlm --mode stdp

# Run learning experiment only
nlm --mode learning

# Run all tests (default)
nlm --mode all
```

### Configuration Options

```bash
# Load configuration from file
nlm --config configs/default.cfg

# Use custom configuration with different mode
nlm --config configs/learning.cfg --mode learning

# Override neuron count (faster testing)
nlm --mode basic --neuron-count 500

# Use different simulation timestep
nlm --mode all --timestep 0.001

# Run with quiet output (faster)
nlm --mode learning --quiet

# Enable benchmarking to measure performance
nlm --mode all --benchmark

# Validate configuration without running simulation
nlm --validate-only --config configs/experiment.cfg
```

### Configuration Files

NLM includes several example configuration files in the `configs/` directory:

- `configs/default.cfg` - Basic testing configuration (500 neurons)
- `configs/learning.cfg` - Optimized for learning experiments (1000 neurons)
- `configs/large.cfg` - Large-scale simulation (5000 neurons)
- `configs/debug.cfg` - Debug configuration with verbose logging
- `configs/performance.cfg` - Performance-optimized configuration
- `configs/experiment.cfg` - Scientific experiment configuration

### Python API Quick Start

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Running Tests

After building, you can run the unit tests:

```bash
# From the build directory
cd build
ctest

# Run with detailed output
ctest --output-on-failure

# Specific test
./nlm_test
```

## Configuration File Format

NLM uses a simple key-value format for configuration files:

```
brain.neuron_count = 1000
brain.region_count = 2
brain.connection_probability = 0.1

brain.neurons.v_thresh = -50.0
brain.neurons.v_rest = -70.0
brain.neurons.v_reset = -75.0

plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
```

## Advanced Usage

### Custom Command Line Options

```bash
# Enable specific plasticity rules
nlm --enable-plasticity     # Enable STDP and Hebbian
nlm --enable-stdp           # Enable STDP only
nlm --enable-hebbian        # Enable Hebbian only
nlm --disable-plasticity    # Disable all plasticity

# Set random seed for reproducible results
nlm --random-seed 12345

# Combine multiple options
nlm --mode learning --neuron-count 2000 --benchmark --verbose
```

### Output Levels

```bash
# Quiet mode (minimal output)
nlm --quiet

# Normal mode (default)
nlm

# Verbose mode (maximum detail)
nlm --verbose
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Development Mode

For development and debugging, use the debug configuration:

```bash
nlm --config configs/debug.cfg --verbose
```

This provides maximum logging output and uses a small network for quick iteration.

## Performance Tips

1. **For faster testing:** Use smaller configurations
   ```bash
   nlm --mode basic --neuron-count 100
   ```

2. **For production:** Use the performance configuration
   ```bash
   nlm --config configs/performance.cfg
   ```

3. **For detailed analysis:** Use the experiment configuration
   ```bash
   nlm --config configs/experiment.cfg --benchmark
   ```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 implementation details

## Support

For issues or questions, please refer to the documentation or the NLM repository issues.

---

**NLM Version 2.0.0 (Phase 2)**  
Copyright (c) 2024-2026 NLM Development Team