# Installation Guide

This document provides instructions for installing and using the NLM (Neural Learning Machine) Python package.

## Prerequisites

### System Requirements
- **Operating System**: Linux, macOS, or Windows
- **Python**: 3.8 or higher
- **Compiler**: C++ compiler with C++20 support (for building C++ core)
- **CMake**: 3.16 or higher (for building)

### Python Dependencies
The NLM Python package has the following dependencies:

```python
# Core dependencies (automatically handled)
# numpy>=1.20 (optional, for advanced features)
# matplotlib>=3.5 (optional, for plotting)
```

## Installation Methods

### 1. From Source (Recommended)

```bash
# Clone the repository
# Note: This repository should already be cloned in /workspace/app

# Navigate to the project directory
cd /workspace/app

# Build the C++ core
mkdir -p build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Install the Python package
pip install .
```

### 2. Development Installation

```bash
cd /workspace/app
pip install -e .
```

### 3. Quick Test Installation

```bash
# For testing without building C++ core (limited functionality)
pip install --no-deps -e .
```

## Verification

### Basic Functionality Test

```bash
# Test that the package imports correctly
python -c "import nlm; print('NLM package version:', nlm.__version__)"

# Test brain functionality
python -c "
import nlm
brain = nlm.Brain()
brain.initialize()
print('Brain initialized successfully')
print('Brain status:')
brain.logStatus()
"
```

### Demo Test

```bash
# Run the built-in demo
python -m nlm demo
```

### Command-Line Interface Test

```bash
# Test command-line interface
python -m nlm help
```

## Configuration

### Environment Variables

```bash
# Set Python path (if needed)
export PYTHONPATH=/workspace/app:$PYTHONPATH

# Set working directory
cd /workspace/app
```

### Configuration Files

The NLM package uses configuration files for:

1. **Brain Configuration** (`configs/default.cfg`)
2. **Python Settings** (package configuration)
3. **Experiment Parameters**

### Configuration Template

```python
import nlm

# Create configuration
config = nlm.Config()

# Set parameters
config.set("random_seed", 42, None)
config.set("simulation_timestep", 0.001, None)
config.set("neuron_count", 1000, None)
config.set("region_count", 1, None)
config.set("connection_probability", 0.15, None)

# Save configuration
config.save_to_json_file("my_config.json")

# Create brain with configuration
brain = nlm.Brain(config)
```

## Usage Examples

### 1. Basic Neural Simulation

```python
import nlm

# Create simulation with default settings
simulation = nlm.NLMSimulation().quick_start(
    neurons=500,
    width=400,
    height=400,
    timestep=0.001,
    seed=42
)

# Run simulation
results = simulation.run_simulation(
    total_steps=1000,
    steps_per_episode=200,
    verbose=True
)

print(f"Simulation completed:")
print(f"  Total spikes: {results['final_spike_count']}")
print(f"  Final firing rate: {results['final_firing_rate']:.2f} Hz")
print(f"  Duration: {results['duration']:.2f}s")
```

### 2. Agent Simulation

```python
import nlm

# Create agent simulation
simulation = nlm.NLMSimulation().quick_start(
    neurons=1000,
    width=800,
    height=600,
    timestep=0.001,
    seed=42
)

# Configure world
simulation.world.configure(800, 600, 40, 40)
simulation.world.setAgentStart(100, 100)

# Add world objects
simulation.world.add_resource_object(200, 200, 1.0, 0.5)
simulation.world.add_hazard_object(600, 400, -1.0, 0.5)
simulation.world.add_wall_object(400, 300, 0.5)

# Run agent simulation
results = simulation.run_simulation(
    total_steps=2000,
    steps_per_episode=200,
    verbose=True
)

print(f"Agent simulation completed:")
print(f"  Final position: ({simulation.world.agent.x:.1f}, {simulation.world.agent.y:.1f})")
print(f"  Agent energy: {simulation.world.agent.energy:.1f}")
print(f"  Agent health: {simulation.world.agent.health:.1f}")
print(f"  Agent age: {simulation.world.agent.age}")
```

### 3. Learning Experiment

```python
import nlm

# Create learning simulation
simulation = nlm.NLMSimulation().quick_start(
    neurons=1000,
    width=800,
    height=600,
    timestep=0.001,
    seed=42
)

# Run learning experiment
results = simulation.run_learning_experiment(
    experiences=10,
    steps_per_experience=200,
    sensory_patterns=True
)

print(f"Learning experiment results:")
print(f"  Learning detected: {results['learning_detected']}")
print(f"  Total spikes: {results['total_spikes']}")
print(f"  Average final spikes: {results['average_final_spikes']:.1f}")

if results['learning_detected']:
    print("  ✓ Learning was successful!")
    print(f"  Weight change: {results['weight_change']:.4f}")
else:
    print("  ⚠ Limited learning detected - try different parameters")
```

### 4. Brain Functionality Test

```python
import nlm

# Test brain functionality
brain = nlm.Brain()
brain.initialize()

print("Initial brain status:")
brain.logStatus()

# Run basic tests
print("\nRunning connectivity test...")
brain.inject_sensory_current(nlm.NeuronType.Sensory, 50.0, 100)

print("Running plasticity test...")
results = brain.simulate_plasticity_experiment(500)

print(f"\nPlasticity results:")
print(f"  Initial mean weight: {results['initial_mean_weight']:.4f}")
print(f"  Final mean weight: {results['final_mean_weight']:.4f}")
print(f"  Weight change: {results['weight_change']:.4f}")
print(f"  Learning percent: {results['learning_percent']:.2f}%")
print(f"  Total spikes: {results['total_spikes']}")
print(f"  Firing rate: {results['firing_rate']:.2f} Hz")

# Test developmental stages
print("\nTesting developmental stages...")
for stage in nlm.DevelopmentalStage:
    brain.setDevelopmentalStage(stage)
    print(f"  Stage {stage.name}: {brain.getTotalSpikeCount()} spikes")
```

### 5. Configuration Testing

```python
import nlm

# Test configuration system
config = nlm.Config()

# Test basic configuration
config.set("random_seed", 42, None)
config.set("simulation_timestep", 0.001, None)
config.set("neuron_count", 500, None)
config.set("region_count", 1, None)

print("Configuration keys:", config.getKeys())

# Test validation
errors = config.validate()
if errors:
    print("Validation errors:", errors)
else:
    print("Configuration is valid")

# Test save/load
config.save_to_json_file("test_config.json")
print("Configuration saved")

new_config = nlm.Config()
new_config.load_from_json_file("test_config.json")
print("Configuration loaded")

# Clean up
import os
if os.path.exists("test_config.json"):
    os.remove("test_config.json")
    print("Test file cleaned up")
```

## Command-Line Interface

### Installation

The NLM package includes a command-line interface. After installation, you can run:

```bash
nlm --help
```

### Available Commands

1. **`nlm demo`** - Run demo simulation
2. **`nlm agent`** - Run agent simulation
3. **`nlm learning`** - Run learning experiment
4. **`nlm brain-test`** - Test brain functionality
5. **`nlm config-test`** - Test configuration system
6. **`nlm experiment`** - Run experiment
7. **`nlm plotting`** - Demonstrate plotting
8. **`nlm developer`** - Show developer tools

### Command Examples

#### Demo
```bash
nlm demo
```

#### Agent Simulation
```bash
nlm agent --steps 1000 --seed 42 --width 800 --height 600 --start-x 100 --start-y 100
```

#### Learning Experiment
```bash
nlm learning --experiences 5 --steps-per-experience 200 --seed 42
```

#### Brain Test
```bash
nlm brain-test
```

#### Configuration Test
```bash
nlm config-test
```

#### Experiment
```bash
nlm experiment --steps 500
```

### Command-Line Options

#### Common Options
- `--verbose, -v` - Enable verbose output

#### Agent Command Options
- `--steps N` - Total steps to simulate (default: 1000)
- `--seed N` - Random seed (default: 42)
- `--width N` - World width (default: 800)
- `--height N` - World height (default: 600)
- `--start-x N` - Agent start X position (default: 0)
- `--start-y N` - Agent start Y position (default: 0)
- `--episode-length N` - Steps per episode (default: 200)

#### Learning Command Options
- `--experiences N` - Number of learning experiences (default: 10)
- `--steps-per-experience N` - Steps per experience (default: 200)
- `--seed N` - Random seed (default: 42)
- `--vary-patterns` - Use varied sensory patterns

#### Experiment Command Options
- `--steps N` - Steps per experiment (default: 500)

## Advanced Usage

### 1. Custom Configuration

```python
import nlm
from nlm.types import DevelopmentalStage, NeuronType

# Create custom configuration
config = nlm.Config()

# Set custom parameters
config.set("random_seed", 123, None)
config.set("simulation_timestep", 0.0005, None)  # Faster simulation
config.set("neuron_count", 2000, None)  # More neurons
config.set("region_count", 3, None)  # Multiple regions
config.set("connection_probability", 0.2, None)  # More connections

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Run custom simulation
for step in range(1000):
    # Custom input pattern
    current = 20.0 + 10.0 * math.sin(step * 0.01)
    brain.injectCurrentToNeurons(NeuronType.Sensory, current)
    brain.step(step, step * 0.0005)

# Get results
print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
```

### 2. Experiment Management

```python
import nlm

# Create experiment
experiment = nlm.NLMExperiment("Custom Experiment")

# Add parameter sets
param_sets = [
    {"random_seed": 42, "neuron_count": 500, "connection_probability": 0.15},
    {"random_seed": 43, "neuron_count": 1000, "connection_probability": 0.2},
    {"random_seed": 44, "neuron_count": 200, "connection_probability": 0.1},
]

# Run parameter sweep
results = experiment.run_parameter_sweep(param_sets, steps=500)

# Analyze results
for i, result in enumerate(results):
    print(f"Parameter set {i + 1}:")
    print(f"  Spikes: {result['standard_results']['final_spike_count']}")
    print(f"  Firing rate: {result['standard_results']['final_firing_rate']:.2f} Hz")
```

### 3. World Customization

```python
import nlm

# Create world with custom settings
world = nlm.SimpleWorld()
world.configure(1200, 800, 60, 60)  # Larger world

# Add custom objects
world.add_resource_object(100, 100, 2.0, 1.0)  # Larger resource
world.add_hazard_object(1100, 700, -2.0, 1.0)  # Larger hazard

# Add walls
for x in [200, 400, 600, 800]:
    world.add_wall_object(x, 400, 2.0)

# Set agent start
world.setAgentStart(50, 50)

print(f"World configured: {world.getWidth()} x {world.getHeight()}")
print(f"Resources: {len(world.get_resource_objects())}")
print(f"Hazards: {len(world.get_hazard_objects())}")
print(f"Walls: {len(world.get_wall_objects())}")
```

### 4. Performance Monitoring

```python
import nlm
import time

# Create brain for benchmarking
brain = nlm.Brain()
brain.initialize()

# Benchmark performance
start_time = time.time()
for step in range(1000):
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0 + step % 50)
    brain.step(step, step * 0.001)
end_time = time.time()

print(f"Benchmark results:")
print(f"  Steps: 1000")
print(f"  Time: {end_time - start_time:.2f}s")
print(f"  Steps per second: {1000 / (end_time - start_time):.1f}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

## Troubleshooting

### 1. Import Errors

**Problem**: `ModuleNotFoundError: No module named 'nlm'`

**Solution**:
```bash
# Install the package
pip install -e .

# Or ensure the path is set
cd /workspace/app
export PYTHONPATH=/workspace/app:$PYTHONPATH
```

### 2. Build Errors

**Problem**: CMake build failures

**Solution**:
```bash
# Build with verbose output
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
cmake --build build --config Release
```

### 3. Configuration Errors

**Problem**: Invalid configuration

**Solution**:
```python
import nlm

# Create configuration with validation
config = nlm.Config()
config.set("random_seed", 42, None)
config.set("simulation_timestep", 0.001, None)

# Validate configuration
errors = config.validate()
if errors:
    print("Configuration errors:", errors)
    # Fix configuration
    # ...
else:
    print("Configuration is valid")
```

### 4. Performance Issues

**Problem**: Slow simulation

**Solution**:
```python
import nlm

# Create brain with optimized settings
brain = nlm.Brain()
brain.initialize()

# Use larger timesteps for faster simulation
config = brain.getConfig()
config.set("simulation_timestep", 0.01, None)  # 10x faster

# Run with fewer neurons for testing
config.set("neuron_count", 100, None)
```

### 5. Memory Issues

**Problem**: High memory usage

**Solution**:
```python
import nlm

# Create brain with memory-efficient settings
brain = nlm.Brain()
brain.initialize()

# Use smaller configuration
config = brain.getConfig()
config.set("neuron_count", 100, None)  # Fewer neurons
config.set("region_count", 1, None)   # Single region
```

## Uninstallation

### Uninstall NLM Package

```bash
# Uninstall the Python package
pip uninstall nlm

# Remove build directory (optional)
rm -rf build
```

### Remove Configuration Files

```bash
# Remove generated configuration files
rm -f *.json
rm -f test_*.json
```

## Frequently Asked Questions

### Q: What Python versions are supported?
A: Python 3.8 and higher

### Q: Do I need to install the C++ core?
A: Yes, but it's automatically built during installation

### Q: Can I use NLM without building the C++ core?
A: Partially - Python bindings provide basic functionality, but full neural simulation requires the C++ core

### Q: What are the system requirements?
A: C++ compiler with C++20 support, CMake 3.16+, Python 3.8+

### Q: How do I get help?
A: Use the command-line interface:
```bash
python -m nlm help
```

### Q: Where can I find examples?
A: In the documentation and examples in the NLM package

## Support

### Documentation
- Comprehensive documentation: `NLM.md`
- API references: See individual class docstrings
- Examples: Included in documentation

### Support
- GitHub issues: Check for existing issues
- Documentation: README and NLM.md files
- Examples: Command-line examples and Python examples

## License

This package is licensed under the MIT License. See LICENSE file for details.
