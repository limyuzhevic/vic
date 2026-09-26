# NLM Advanced CLI

## Overview

The NLM Advanced CLI provides comprehensive command-line interfaces for NLM power users, offering advanced configuration management, enhanced simulation control, power user features, and brain state management.

## Features

### 1. Advanced Configuration Management
- **Import/Export**: JSON, YAML, and table format support
- **Validation**: Configuration schema validation
- **Comparison**: Configuration diffing between versions
- **Listing**: Display all configuration keys and values

### 2. Enhanced Simulation Control
- **Custom Simulation**: Parameterized simulation control
- **Real-time Monitoring**: Live simulation monitoring with adjustable speed
- **Checkpoint System**: Automatic and manual checkpoint creation/restart
- **Learning Rate Scheduling**: Multiple scheduling algorithms

### 3. Power User Features
- **Advanced Debugging**: Detailed neuron and synapse debugging
- **Performance Profiling**: Comprehensive performance metrics and analysis
- **Neuron-by-Neuron**: Individual neuron state inspection and control
- **Synapse Monitoring**: Real-time synaptic weight and plasticity tracking
- **Network Statistics**: Global network-wide metrics and analysis

### 4. Brain State Management
- **Manual Manipulation**: Direct control over neuron states and synaptic weights
- **Pattern Injection**: Custom input patterns for testing and experimentation
- **Topology Exploration**: Network connectivity analysis and measurements
- **Visualization**: Weight distribution and network topology visualization
- **Analysis**: Computational complexity and information bottleneck analysis

## Installation

### Build from Source

```bash
mkdir build
cd build
cmake ..
cmake --build . -- -j$(nproc)
```

### Install

```bash
cd build
sudo make install
```

## Usage

### Basic Commands

```bash
# Show help information
nlm-cli --help

# Show version information
nlm-cli --version

# Enable verbose output
nlm-cli --verbose

# Suppress output
nlm-cli --quiet
```

### Configuration Management

```bash
# Export configuration
nlm-cli config export --file config.json --format json
nlm-cli config export --file config.yaml --format yaml

# Import configuration
nlm-cli config import --file config.json --format json

# Validate configuration
nlm-cli config validate

# Compare configurations
nlm-cli config diff --source current --target default

# List configuration
nlm-cli config list
```

### Simulation Control

```bash
# Custom simulation
nlm-cli simulate custom --timestep 0.001 --neurons 1000 --regions 2

# Real-time monitoring
nlm-cli simulate realtime --speed 2.0 --monitoring

# Checkpoint management
nlm-cli simulate checkpoint --directory checkpoints
nlm-cli simulate restart --checkpoint checkpoints/brain_checkpoint.bin
```

### Power User Features

```bash
# Advanced debugging
nlm-cli debug advanced --neuron-debug --synapse-monitor

# Performance profiling
nlm-cli profiler start --output profile.json --duration 60

# Neuron debugging
nlm-cli neuron debug --id 1 --show-potential --show-spikes

# Network statistics
nlm-cli network stats --summary --detailed
```

### Brain State Management

```bash
# Manual state manipulation
nlm-cli state manual --command inject --neuron-id 1 --current 50.0

# Pattern injection
nlm-cli state pattern --type pre-post --pre-neuron 1 --post-neuron 2

# Network topology
nlm-cli state topology --measure clustering --identify hubs

# Visualization
nlm-cli state visualize --type network --output network.png

# Analysis
nlm-cli state analysis --complexity --generalization
```

## Configuration File Formats

### JSON Format

```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "random_seed": 42
}
```

### YAML Format

```yaml
neuron_count: 1000
region_count: 1
simulation_timestep: 0.001
random_seed: 42
```

### Table Format

```text
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
random_seed = 42
```

## Error Handling

The CLI uses standard error codes:
- `0`: Success
- `1`: General error
- `2`: Configuration error
- `3`: Brain initialization error
- `4`: Simulation error
- `5`: Debug error
- `6`: Profiling error
- `7`: State management error
- `8`: I/O error

## Files Created

### Core Implementation
- `src/cli/AdvancedCLI.hpp` - CLI interface header
- `src/cli/AdvancedCLI.cpp` - CLI implementation

### Documentation
- `AdvancedCLI.md` - User documentation and examples
- `IMPLEMENTATION_SUMMARY.md` - Implementation details

### Testing
- `test_cli.sh` - Test script for basic functionality

## Build System Integration

### CMake Configuration
The CLI is integrated into the NLM build system with:
- `nlm_cli` library for CLI functionality
- Proper dependency management with core NLM libraries
- Support for all existing NLM experiments (Phase 3-6)

### Build Commands

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build -- -j$(nproc)

# Install
cmake --install build
```

## Testing

Run the test script to verify basic functionality:

```bash
chmod +x test_cli.sh
./test_cli.sh
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Additional Notes

The Advanced CLI provides a comprehensive interface for NLM power users, combining advanced configuration management, enhanced simulation control, power user features, and brain state management into a single, unified command-line interface.
