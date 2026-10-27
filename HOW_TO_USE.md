# NLM HOW TO USE - Detailed Instructions

## Table of Contents

1. [Installation and Setup](#installation-and-setup)
2. [Running Demos](#running-demos)
3. [Configuration Management](#configuration-management)
4. [Command Line Interface](#command-line-interface)
5. [Python Bindings](#python-bindings)
6. [Development and Extension](#development-and-extension)
7. [Troubleshooting](#troubleshooting)
8. [Advanced Features](#advanced-features)

## Installation and Setup

### System Requirements

NLM requires the following dependencies:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y cmake build-essential python3-dev python3-pip

# CentOS/RHEL
sudo yum install -y cmake gcc gcc-c++ python3-devel

# macOS (using Homebrew)
brew install cmake python
```

### Building from Source

#### Standard Build

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j4
```

#### Development Build (Debug)

```bash
# For development and debugging
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_DEBUG=ON
make -j4
```

#### Python Bindings Build

```bash
# Build Python bindings
cd python
pip install -e .

# Or build with CMake
cd python_build
cmake ../python -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Custom Configuration

You can customize the build with various options:

```bash
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DNLM_ENABLE_TESTS=ON \
  -DNLM_ENABLE_PYTHON=ON \
  -DNLM_PERF_PROFILE=ON \
  -DNLM_MAX_NEURONS=10000
```

## Running Demos

### Phase 6 Integration Demo

The Phase 6 demo is the most comprehensive test of NLM:

```bash
./build/nlm_phase6_demo
```

**What it tests:**
- Memory systems integration
- Neuromodulation integration
- Prediction system integration
- Development system integration
- Checkpoint and replay functionality

### Phase 2 Neural Computation Demo

Focuses on basic neural dynamics:

```bash
./build/nlm_phase2_demo
```

**What it demonstrates:**
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 Learning Demo

Shows world interaction and learning:

```bash
./build/nlm_phase3_demo
```

**What it demonstrates:**
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 Advanced Cognition Demo

Demonstrates advanced cognitive abilities:

```bash
./build/nlm_phase4_demo
```

**What it demonstrates:**
- Neural prediction systems
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention
- Predictive planning

### Main NLM Executable

The main executable allows custom simulation parameters:

```bash
./build/nlm [options]

Options:
  --config <file>           Load configuration from file
  --neurons <count>         Number of neurons (default: 1000)
  --steps <count>           Number of simulation steps (default: 10000)
  --regions <count>         Number of brain regions (default: 2)
  --connection-prob <float> Connection probability (0.0-1.0, default: 0.15)
  --help                    Show help message
```

## Configuration Management

### Configuration File Format

NLM uses a simple key=value format for configuration:

```ini
# Example configuration file (configs/experiment.cfg)

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.15

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Creating Custom Configurations

```bash
# Create a new configuration file
cp configs/default.cfg configs/custom.cfg

# Edit the configuration file
nano configs/custom.cfg
```

### Configuration Validation

You can validate configuration files using the NLM CLI:

```bash
./nlm-cli config configs/custom.cfg
```

### Saving Configurations

```bash
./nlm-cli save-config my_config.cfg
```

## Command Line Interface

The NLM CLI provides a user-friendly interface for common tasks:

### Available Commands

```bash
./nlm-cli [command] [options]

Commands:
  demo                    Run Phase 6 integration demo
  experiment              Run custom NLM experiment
  config <file>           Load and validate configuration
  test                    Run unit tests
  visualize               Start visualization
  save-config <file>      Save current configuration
```

### Demo Command

```bash
./nlm-cli demo
```

Runs the Phase 6 integration demo with default settings.

### Experiment Command

```bash
./nlm-cli experiment \
  --neurons 2000 \
  --steps 10000 \
  --regions 2 \
  --connection-prob 0.2 \
  --width 20 \
  --height 20
```

Runs a custom experiment with specified parameters.

### Config Command

```bash
./nlm-cli config configs/experiment.cfg
```

Loads and validates a configuration file.

### Test Command

```bash
./nlm-cli test
```

Runs the unit test suite.

### Visualize Command

```bash
./nlm-cli visualize
```

Starts the visualization module (currently a placeholder).

### Save-Config Command

```bash
./nlm-cli save-config my_config.cfg
```

Saves the current configuration to a file.

## Python Bindings

### Installation

```bash
cd python
pip install -e .
```

### Basic Usage

```python
from pynlm import NLM, BrainConfig

# Create brain configuration
config = BrainConfig()
config.random_seed = 42
config.neuron_count = 1000
config.simulation_timestep = 0.001

# Initialize brain
brain = NLM(config)

# Run simulation
for i in range(100):
    brain.step(i * config.simulation_timestep)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Python Usage

```python
from pynlm import NLM, BrainConfig
import json

# Create custom configuration
config = BrainConfig()
config.random_seed = 12345
config.neuron_count = 2000
config.simulation_timestep = 0.001

# Initialize brain
brain = NLM(config)

# Save brain state
brain.saveState("brain_state.json")

# Run some steps
for step in range(500):
    brain.step(step * config.simulation_timestep)

# Load saved state
brain.loadState("brain_state.json")

# Continue simulation
for step in range(100):
    brain.step(step * config.simulation_timestep)

print(f"Final total spikes: {brain.getTotalSpikeCount()}")
```

### Python API Reference

The Python bindings provide access to the following classes and functions:

- **NLM**: Main brain simulation class
- **BrainConfig**: Brain configuration class
- **saveState()**: Save brain state to file
- **loadState()**: Load brain state from file

## Development and Extension

### Adding New Components

To add new components to NLM:

1. **Create new source files** in the appropriate `src/` directory
2. **Update CMakeLists.txt** to include new files
3. **Add tests** in the `tests/` directory
4. **Update documentation** as needed

### Building with Tests

```bash
# Build with tests enabled
cd build
cmake .. -DNLM_ENABLE_TESTS=ON
make -j4

# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R test_neuron
```

### Adding Unit Tests

Example unit test structure:

```cpp
// tests/test_example.cpp
#include <gtest/gtest.h>
#include "path/to/your/component.h"

TEST(YourComponentTest, BasicFunctionality) {
    // Test basic functionality
    YourComponent component;
    EXPECT_TRUE(component.initialize());
    EXPECT_TRUE(component.run());
    EXPECT_TRUE(component.cleanup());
}
```

### Performance Testing

```bash
# Build with performance profiling
cd build
cmake .. -DNLM_PERF_PROFILE=ON
make -j4

# Run performance benchmarks
./build/benchmark --mode full

# Profile specific components
./build/benchmark --profile neuron
```

## Troubleshooting

### Common Build Issues

#### Missing Dependencies

```bash
# Install build tools
sudo apt-get install build-essential cmake

# Install Python development headers
sudo apt-get install python3-dev
```

#### CMake Configuration Errors

```bash
# Clean and rebuild
cd build
make clean
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Python Installation Issues

```bash
# Check if Python is available
python3 --version

# Install pip if not available
apt-get install python3-pip

# Install the package
cd python
pip install -e .
```

### Common Runtime Issues

#### Memory Issues

```bash
# Increase memory limit
ulimit -v 4096000  # 4GB

# Run with fewer neurons
./build/nlm --neurons 100 --steps 1000
```

#### Configuration Issues

```bash
# Check configuration file syntax
key = value
# Comments start with #

# Ensure required keys are present
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
```

#### File Permission Issues

```bash
# Check file permissions
chmod +x build/nlm
chmod +x nlm-cli

# Run with sudo if necessary (not recommended)
sudo ./build/nlm
```

### Debugging

#### Debugging with GDB

```bash
# Build with debugging symbols
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with GDB
gdb ./build/nlm

# Set breakpoints and run
break main
run --neurons 100 --steps 100
```

#### Running with Valgrind

```bash
# Build with Valgrind support
cd build
cmake .. -DNLM_VALGRIND=ON
make -j4

# Run with Valgrind
valgrind --leak-check=full ./build/nlm --neurons 100 --steps 100
```

## Advanced Features

### Checkpoint and Recovery

NLM supports checkpoint and recovery for long-running experiments:

```bash
# Run with checkpointing enabled
./build/nlm --neurons 5000 --steps 50000 --checkpoint-interval 1000

# The system will save checkpoint files at specified intervals
# You can interrupt and resume later
```

### Visualization

Visualization is supported through the `visualization/` module:

```bash
# Build with visualization
./nlm-cli visualize
```

### Performance Monitoring

Performance monitoring is available:

```bash
# Build with performance monitoring
cd build
cmake .. -DNLM_PERF_MONITOR=ON
make -j4

# Run with monitoring
./build/benchmark --monitor
```

### Advanced Configuration Options

Many advanced configuration options are available:

```ini
# Structural plasticity
structural_plasticity_enabled = true
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation
novelty_threshold = 0.5
curiosity_decay = 0.99

# Memory systems
memory_enabled = true
episodic_memory_size = 1000

# Development
developmental_stages = 4
plasticity_decay_rate = 0.01
```

### Custom Environments

You can create custom environments by extending the `Environment` class:

```cpp
class CustomEnvironment : public Environment {
public:
    std::unique_ptr<SensoryInput> reset() override {
        // Custom reset logic
    }
    
    std::unique_ptr<SensoryInput> step(const Action& action) override {
        // Custom step logic
    }
    
    // Implement other required methods
};
```

## Phase Information

NLM is developed in phases. Each phase builds on the previous one:

### Phase 1: Project Skeleton (COMPLETE)
- Project skeleton and core types
- Neural interfaces
- Basic configuration system

### Phase 2: Real Neural Computation (COMPLETE)
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 3: World Interaction (COMPLETE)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4: Advanced Cognition (COMPLETE)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5: Performance Optimizations (COMPLETE)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration (COMPLETE)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## License and Attribution

This project is licensed under the MIT License. Please see the `LICENSE` file for details.

## Support and Resources

### Documentation

- **Main README**: Complete project documentation
- **Quick Start**: `easy_usage.md` - Quick start guide
- **Detailed Instructions**: `HOW_TO_USE.md` - This file
- **API Documentation**: `docs/` - Technical API documentation

### Examples

```bash
# Find example programs
find . -name "*example*" -type f
find . -name "*demo*" -type f
```

### Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test categories
ctest -L "unit"
ctest -L "integration"
```

### Community

This is an active research project. For more information, please refer to the main README.md file and other documentation files.