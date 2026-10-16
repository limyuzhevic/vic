# NLM Command Line Tool

The NLM command line tool (`nlm_tool`) provides a comprehensive interface for running neural simulations with various modes and configurations.

## Overview

The tool was created to improve the user experience by:

1. **Providing an intuitive command-line interface** for running different types of simulations
2. **Supporting multiple presets** (demo, research, realtime, competitive) for common use cases
3. **Enabling configuration import/export** in both JSON and legacy formats
4. **Adding benchmarking capabilities** for performance analysis
5. **Supporting both interactive and batch processing modes**

## Installation and Building

The tool is built as part of the main NLM project. Ensure the main project is built first:

```bash
# Build the main NLM project
mkdir build
cd build
cmake ..
make -j4
```

The `nlm_tool` executable will be created in the build directory.

## Usage

### Basic Commands

```bash
# Show help information
./nlm_tool --help

# Show version information
./nlm_tool --version
```

### Mode Options

The tool supports several simulation modes:

#### Demo Mode (Default)
```bash
./nlm_tool --mode demo
```

Quick demonstration of NLM with default settings.

#### Training Mode
```bash
./nlm_tool --mode training --neurons 1000 --steps 10000 --verbose
```

Extended training session with reward-based learning.

#### Research Mode
```bash
./nlm_tool --mode research --preset research --steps 10000
```

Research-grade simulation with comprehensive plasticity and neuromodulation.

#### Real-Time Mode
```bash
./nlm_tool --mode realtime --preset realtime
```

Real-time optimized simulation for interactive applications.

#### Competitive Mode
```bash
./nlm_tool --mode competitive --preset competitive
```

Multi-agent competition where different brains compete for resources.

### Presets

The tool includes several pre-configured presets:

#### Demo Preset
- Neurons: 100
- Regions: 1
- Simulation steps: 500
- Focused on: Quick demonstration and user experience

#### Research Preset
- Neurons: 10000
- Regions: 4
- Simulation steps: 10000
- Focused on: Comprehensive analysis and research

#### Real-Time Preset
- Neurons: 50
- Regions: 1
- Simulation steps: 100
- Focused on: Fast, interactive response

#### Competitive Preset
- Neurons: 200
- Regions: 3
- Simulation steps: 5000
- Focused on: Multi-agent competition and evolution

#### Custom Preset
Create your own configurations by combining parameters:

```bash
./nlm_tool --preset demo --neurons 300 --steps 200 --enable-curiosity --enable-development
```

### Configuration Management

#### Export Configuration
Save your current configuration to a file:

```bash
./nlm_tool --mode export --output my_settings.json
```

#### Import Configuration
Load a saved configuration:

```bash
./nlm_tool --mode import --config my_settings.json
```

#### JSON Support
The tool supports JSON configuration files with enhanced features:

```bash
# Export in JSON format
./nlm_tool --mode export --output my_config.json

# Import from JSON format
./nlm_tool --mode import --config my_config.json
```

### Parameters

Common parameters you can customize:

#### Network Parameters
```bash
--neurons N          Number of neurons (default: preset-dependent)
--regions N          Number of brain regions (default: 1)
--connection-prob P  Connection probability (default: 0.15)
```

#### Simulation Parameters
```bash
--steps N            Number of simulation steps (default: 1000)
--timestep T         Simulation timestep in seconds (default: 0.001)
```

#### Feature Flags
```bash
--verbose, -V        Enable verbose output and progress reporting
--no-plasticity      Disable all plasticity mechanisms
--enable-development Enable developmental plasticity
--enable-curiosity   Enable curiosity-driven exploration
--no-reward          Disable reward modulation
```

### Examples

#### 1. Quick Demo with Custom Settings
```bash
./nlm_tool --mode demo --preset realtime --steps 100 --verbose
```

#### 2. Research-Grade Simulation
```bash
./nlm_tool --mode research --neurons 5000 --steps 5000 --verbose --enable-curiosity
```

#### 3. Real-Time Application
```bash
./nlm_tool --mode realtime --preset realtime
```

#### 4. Benchmarking
```bash
./nlm_tool --benchmark 1000
```
Benchmarks brain creation with 1000 neurons.

#### 5. Configuration Workflow
```bash
# Create and save custom configuration
./nlm_tool --mode demo --neurons 200 --steps 300 --output custom_config.json

# Load and run with saved configuration
./nlm_tool --mode import --config custom_config.json --mode demo --steps 500
```

#### 6. Extended Training
```bash
./nlm_tool --mode training --preset research --steps 20000 --enable-curiosity --enable-development
```

### Configuration File Format

The tool supports two configuration file formats:

#### JSON Format (Enhanced)
```json
{
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "enable_hebbian": true,
    "enable_stdp": true,
    "enable_structural_plasticity": true,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_development": true,
    "_description": "Custom research configuration"
}
```

#### Legacy Format
```
# neuron_count = 500
# region_count = 2
# connection_probability = 0.1
# simulation_timestep = 0.001
# enable_hebbian = true
# enable_stdp = true
```

### Quick Start Examples

#### Example 1: First Time Setup
```bash
# Quick demo to understand NLM
./nlm_tool --mode demo --preset realtime --steps 50

# View advanced options
./nlm_tool --help
```

#### Example 2: Research Setup
```bash
# Setup research-grade simulation
./nlm_tool --mode research --preset research --steps 5000 --verbose

# Export settings for reproducibility
./nlm_tool --mode export --output research_settings.json
```

#### Example 3: Development and Testing
```bash
# Test different configurations
./nlm_tool --preset demo --neurons 100 --steps 100
./nlm_tool --preset demo --neurons 200 --steps 100
./nlm_tool --preset demo --neurons 300 --steps 100

# Benchmark performance
./nlm_tool --benchmark 500
./nlm_tool --benchmark 1000
```

### Python Integration Examples

While the command-line tool is powerful, you can also integrate NLM with Python:

```python
# Create a brain for learning
from pynlm import create_brain_for_learning
brain = create_brain_for_learning(neuron_count=100)

# Run simulation
brain.runSteps(1000)

# Get statistics
print(f"Firing rate: {brain.getAverageFiringRate()}")
print(f"Active neurons: {brain.getFiringNeuronCount()}")

# Create agent and train
from pynlm import createSimpleWorld, createAgentBrain
world = createSimpleWorld()
world.configure(16, 16, 8, 8)
agent = createAgentBrain(brain)
agent.initialize(world)
agent.trainEpisode(world, steps=500)
```

### Troubleshooting

#### Common Issues

1. **Command not found**
   ```bash
   # Ensure the tool is in your PATH
   export PATH=/path/to/build:$PATH
   ```

2. **Configuration file errors**
   ```bash
   # Check file format and syntax
   cat my_config.json | python -m json.tool
   ```

3. **Memory or performance issues**
   ```bash
   # Use a preset instead of custom values
   ./nlm_tool --mode realtime --preset realtime

   # Or reduce simulation steps
   ./nlm_tool --steps 100
   ```

4. **Help not showing**
   ```bash
   # Try with help flag
   ./nlm_tool --help

   # Or check if the tool was built
   ls -la ./nlm_tool
   ```

#### Getting More Information

For more detailed information:

```bash
# View all available options
./nlm_tool --help

# Check version information
./nlm_tool --version

# View example configurations
./nlm_tool --mode demo --verbose
```

### Integration with Other Tools

The NLM tool integrates well with:

#### Shell Scripts
```bash
#!/bin/bash
# Run a sequence of experiments
./nlm_tool --preset demo --neurons 100 --steps 100
./nlm_tool --preset demo --neurons 200 --steps 100
./nlm_tool --preset demo --neurons 300 --steps 100
```

#### Python Scripts
```python
import subprocess
import json

# Run a simulation and capture output
result = subprocess.run(['./nlm_tool', '--mode', 'demo', '--steps', '100'], 
                       capture_output=True, text=True)
print(result.stdout)

# Export configuration for Python analysis
with open('config.json', 'w') as f:
    json.dump({'neurons': 100, 'steps': 100}, f)
```

## Features Summary

| Feature | Description |
|---------|-------------|
| **Multiple Modes** | Demo, Training, Research, Real-Time, Competitive |
| **Configuration Presets** | Demo, Research, Real-Time, Competitive |
| **JSON Support** | Enhanced configuration with JSON format |
| **Import/Export** | Save and load configurations |
| **Benchmarking** | Performance measurement and comparison |
| **Verbose Output** | Detailed progress reporting |
| **Feature Flags** | Enable/disable specific mechanisms |
| **Help System** | Comprehensive documentation and examples |

## Future Enhancements

Potential future improvements include:

1. **Web Interface**: HTTP API for remote control
2. **Visualization**: Real-time plotting and monitoring
3. **Parallel Processing**: Multi-GPU support
4. **Distributed Computing**: Cluster-based simulations
5. **Plugin Architecture**: Extend with custom components
6. **Machine Learning**: Integration with modern ML frameworks

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Quick Reference

```bash
# Quick help
./nlm_tool --help

# Version info
./nlm_tool --version

# Quick demo
./nlm_tool --mode demo --preset realtime --steps 50

# Research simulation
./nlm_tool --mode research --preset research --steps 5000

# Real-time interaction
./nlm_tool --mode realtime --preset realtime

# Competitive training
./nlm_tool --mode competitive --preset competitive

# Export configuration
./nlm_tool --mode export --output my_settings.json

# Import configuration
./nlm_tool --mode import --config my_settings.json

# Benchmark performance
./nlm_tool --benchmark 1000
```
