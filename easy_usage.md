# NLM Quick Start Guide

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project. This guide provides quick access to the most common tasks and workflows.

## Quick Usage

### 1. Building

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Running Demos

```bash
# Phase 6 Integration Demo (recommended for first run)
./build/nlm_phase6_demo

# Phase 2 Neural Computation Demo (basic neural dynamics)
./build/nlm_phase2_demo

# Phase 3 Learning Demo (world interaction)
./build/nlm_phase3_demo

# Phase 4 Cognition Demo (advanced abilities)
./build/nlm_phase4_demo
```

### 3. Using the Command Line Interface

```bash
# Run the NLM CLI
./nlm-cli demo              # Run Phase 6 integration demo
./nlm-cli experiment         # Run custom experiment
./nlm-cli config <file>      # Load and validate config
./nlm-cli test              # Run unit tests
./nlm-cli visualize          # Start visualization
./nlm-cli save-config <file> # Save current config
```

### 4. Python Bindings

```bash
# Install Python bindings
cd python
pip install -e .

# Run a simple Python example
python -c "
from pynlm import NLM, BrainConfig

config = BrainConfig()
config.random_seed = 42
config.neuron_count = 1000

brain = NLM(config)
for i in range(100):
    brain.step(i * config.simulation_timestep)

print(f'Total spikes: {brain.getTotalSpikeCount()}')
"
```

## Configuration

### Basic Configuration

Create a configuration file (`configs/myconfig.cfg`):

```ini
# NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
platomacy_learning_rate = 0.01
```

### Command Line Configuration

```bash
./nlm --config configs/myconfig.cfg --neurons 2000 --steps 5000
```

## Common Workflows

### 1. Testing Your Installation

```bash
# Run unit tests
./build/nlm_test

# Run integration demo
./build/nlm_phase6_demo
```

### 2. Running a Custom Experiment

```bash
# Run with default parameters
./build/nlm

# Run with custom parameters
./build/nlm --neurons 5000 --steps 20000 --regions 4

# Run with custom configuration
./build/nlm --config configs/large_network.cfg
```

### 3. Debugging and Development

```bash
# Build with debugging symbols
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with debugging output
./nlm --neurons 100 --steps 1000
```

### 4. Performance Testing

```bash
# Build with performance profiling
cd build
cmake .. -DNLM_PERF_PROFILE=ON
make -j4

# Run performance benchmarks
./build/benchmark --mode full
```

## Troubleshooting

### Build Issues

```bash
# Ensure you have required dependencies
sudo apt-get install cmake build-essential python3-dev

# Clean and rebuild
cd build
make clean
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Runtime Issues

```bash
# Check for memory issues
ulimit -v 4096000  # 4GB limit

# Check for file descriptor limits
ulimit -n 1024

# Run with fewer neurons for testing
./build/nlm --neurons 100 --steps 1000
```

### Configuration Issues

```bash
# Check configuration syntax
# Configuration format: key = value
# Comments start with #

# Example config file
configs/example.cfg
```

## Getting Help

### Documentation

- **Main README**: `README.md` - Complete project documentation
- **Quick Guide**: `easy_usage.md` - This file
- **Detailed Instructions**: `HOW_TO_USE.md` - Advanced usage
- **API Documentation**: `docs/` - Technical API documentation

### Command Line Help

```bash
# NLM CLI help
./nlm-cli --help

# NLM executable help
./build/nlm --help
```

### Examples

```bash
# Find example programs
find . -name "*example*" -type f

# Look at test programs for patterns
test/test_neuron.cpp
```

## Advanced Usage

### Python Scripting

```python
from pynlm import NLM, BrainConfig
import json

# Create a custom brain
config = BrainConfig()
config.random_seed = 12345
config.neuron_count = 2000
config.simulation_timestep = 0.001

brain = NLM(config)

# Save brain state
brain.saveState("my_brain.json")

# Run simulation
for step in range(1000):
    brain.step(step * config.simulation_timestep)

# Load saved state
brain.loadState("saved_brain.json")
```

### Batch Processing

```bash
#!/bin/bash
# batch_run.sh - Run multiple experiments

for neurons in 100 500 1000 2000; do
    echo "Running experiment with $neurons neurons..."
    ./build/nlm --neurons $neurons --steps 1000 --output results_$neurons.txt
    echo "Results saved to results_$neurons.txt"
done
```

## Files and Directories

```
.
├── build/                    # Build output directory
├── configs/                  # Configuration files
│   ├── default.cfg          # Default configuration
│   └── example.cfg          # Example configuration
├── docs/                     # Technical documentation
├── easy_usage.md             # This quick start guide
├── HOW_TO_USE.md             # Detailed instructions
├── README.md                 # Complete documentation
├── src/                      # Source code
│   ├── ...
├── tests/                    # Unit tests
├── python/                   # Python bindings
├── nlm-cli                  # Command line interface
└── examples/                 # Example programs
```

## Phase Information

NLM is developed in phases. Each phase adds new capabilities:

- **Phase 1**: Project skeleton and core interfaces
- **Phase 2**: Real neural computation with LIF neurons
- **Phase 3**: World interaction and learning
- **Phase 4**: Advanced cognitive abilities
- **Phase 5**: Performance optimizations
- **Phase 6**: Final integration of all systems

Each demo corresponds to a specific phase. Start with Phase 6 demo for the most comprehensive test.

## License

MIT License

## Support

For issues and support, please refer to the main README.md file for contact information and project resources.