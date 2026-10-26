# NLM (Neural Learning Machine) - Quick Start Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that simulates neural networks and brain-like systems. This version includes significant improvements to code quality, error handling, and user experience.

## New Features in This Version

### 1. Factory Pattern for Brain Creation

The Python bindings now include factory functions for creating brains with predefined configurations:

```python
import pynlm

# Create brains with different configurations
brain = pynlm.createBrain("minimal")      # Lightweight brain for basic operations
brain = pynlm.createBrain("research")     # Optimized for experimentation
brain = pynlm.createBrain("default")      # Default configuration
```

### 2. Enhanced Error Handling

Better error messages and exception handling throughout the codebase:

```python
try:
    brain = pynlm.createBrain("invalid")  # Will raise ValueError
except ValueError as e:
    print(f"Configuration error: {e}")
```

### 3. Configuration Validation

Configuration values are now validated before use:

```python
config = pynlm.createDefaultConfig()

# These settings are validated
config.set("brain.neuron_count", 1000)        # ✓ Valid
config.set("connection_probability", 0.1)     # ✓ Valid
config.set("simulation_timestep", 0.001)      # ✓ Valid

# These will raise exceptions
try:
    config.set("brain.neuron_count", 0)        # ✗ Invalid: must be > 0
except ValueError as e:
    print(f"Error: {e}")

try:
    config.set("connection_probability", 1.5) # ✗ Invalid: must be 0.0-1.0
except ValueError as e:
    print(f"Error: {e}")
```

### 4. Improved Documentation

Comprehensive examples and documentation in:
- `easy_usage.md` - Simplified usage guide
- `HOW_TO_USE.md` - Detailed documentation with examples
- `benchmark.py` - Performance benchmarks

## Installation

The NLM framework is built with CMake and Python bindings. To use the Python bindings:

```bash
# If not already installed
pip install -e .
```

## Basic Usage

### Create a Brain

```python
import pynlm

# Method 1: Using factory pattern
brain = pynlm.createBrain("research")
brain.initialize()

# Method 2: Using custom configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### Run a Simulation

```python
import pynlm

brain = pynlm.createBrain("research")
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Error Handling

```python
import pynlm

try:
    # This will fail and give a clear error message
    brain = pynlm.createBrain("invalid_config")
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
```

## Advanced Usage

### Using Agent and World

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Brain State Management

```python
import pynlm

brain = pynlm.createBrain("research")
brain.initialize()

# Save brain state
brain.save("brain_checkpoint.bin")
print("Brain state saved")

# Load brain state (later)
# brain.load("brain_checkpoint.bin")
```

## Error Messages

The enhanced error handling provides clear, actionable error messages:

- "Config key cannot be empty"
- "Invalid neuron count value"
- "Connection probability must be between 0.0 and 1.0"
- "Failed to load configuration from file"
- "Invalid configuration type"

## Configuration Options

### Common Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `simulation_timestep` | float | 0.001 | Simulation time step |
| `plasticity.stdp.enable` | bool | true | Enable STDP plasticity |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian plasticity |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

### Configuration Types

1. **Minimal**: Lightweight configuration for basic testing
2. **Research**: Optimized for experimentation and research
3. **Default**: Balanced configuration for general use

## Examples

### Example 1: Silent Brain

```python
import pynlm

brain = pynlm.createBrain("minimal")
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Performance

The improvements in this version include:

- **Better error handling**: Reduced runtime overhead from exception handling
- **Configuration validation**: Early detection of invalid configurations
- **Factory pattern**: Optimized creation paths for common use cases
- **Defensive programming**: Null pointer checks and parameter validation
- **Documentation**: Comprehensive examples and usage guides

## Further Reading

For more detailed information:

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [easy_usage.md](easy_usage.md) - Simplified usage guide
- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed documentation
- [benchmark.py](benchmark.py) - Performance benchmarks

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try using `createBrain("research")` for more active configurations

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

## Changes in This Version

### Code Quality Improvements

1. **Error Handling**: Added comprehensive error handling throughout the codebase
2. **Parameter Validation**: Added validation for configuration parameters
3. **Defensive Programming**: Added null pointer checks and defensive coding practices
4. **Factory Pattern**: Added factory methods for common creation scenarios
5. **Documentation**: Added comprehensive documentation and examples

### Backward Compatibility

This version maintains backward compatibility with existing code:
- All existing Python API methods work the same way
- Configuration system supports all existing parameters
- Brain API is unchanged
- World and Agent APIs are unchanged

### New Features

- Factory methods for brain creation
- Enhanced error messages
- Configuration validation
- Performance benchmarks
- Comprehensive documentation

## Contact

For questions or issues, please refer to the documentation or the NLM project repository.

---

**Version**: 0.1.0 with improvements
**Author**: NLM Authors
**License**: MIT
