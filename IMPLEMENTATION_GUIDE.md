# NLM Improvements - Documentation

This document describes the improvements made to the NLM (Neural Learning Machine) project, including bug fixes, new features, and enhanced user experience.

## Overview

The NLM project has been significantly improved with the following enhancements:

1. **Configuration System**: JSON/YAML support and improved file I/O
2. **Python API Wrapper**: Comprehensive, user-friendly Python interface
3. **Command-Line Interface**: Advanced CLI for power users
4. **Enhanced Documentation**: Complete examples and usage guides
5. **Error Handling**: Improved error messages and validation
6. **Performance Monitoring**: Basic performance tracking tools

## 1. Configuration System Improvements

### 1.1 Enhanced File Support

The configuration system now supports multiple file formats:

- **JSON files** (`.json`): For structured configuration
- **YAML files** (`.yaml`, `.yml`): For human-readable configuration
- **Simple key=value files** (`.ini`, `.cfg`): For backward compatibility

### 1.2 Improved File I/O

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Save to JSON file (formatted properly)
config.saveToFile("config.json")

# Load from various formats
config.loadFromFile("config.json")  # JSON
config.loadFromFile("config.yaml")  # YAML  
config.loadFromFile("config.ini")   # INI/INI-like
```

### 1.3 Configuration Key Examples

The configuration system now properly handles different data types:

```python
config.set("brain.neuron_count", 1000)  # int
config.set("brain.synapse_density", 0.1)  # float
config.set("plasticity.stdp.enable", True)  # bool
config.set("brain.name", "my_brain")  # string
```

## 2. Python API Wrapper

### 2.1 Overview

The Python API wrapper (`python.nlm` module) provides a high-level, user-friendly interface to NLM functionality, hiding the complexity of the underlying C++ implementation.

### 2.2 Main Classes

#### `NLMBrain`

```python
from python.nlm import NLMBrain

# Create brain with configuration
brain = NLMBrain({
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.0
}, name="my_brain")

# Initialize brain
brain.initialize()

# Run simulation
brain.step(100)

# Get statistics
stats = brain.get_stats()
print(f"Firing neurons: {stats['firing_neurons']}")
```

#### `NLMWorld`

```python
from python.nlm import NLMWorld

# Create world with default settings
world = NLMWorld(width=20, height=20, vision_width=8, vision_height=8)
world.setAgentStart(10.0, 10.0)

# Update world
world.update(0.1)

# Get sensory percept
percept = world.getSensoryPercept()
```

#### `NLMEnvironment`

```python
from python.nlm import NLMEnvironment

# Create complete agent-environment setup
env = NLMEnvironment(
    brain_config={"brain.neuron_count": 500},
    world_config={"width": 30, "height": 30},
    agent_config={"enable_reward_modulation": True}
)

# Run episode
stats = env.run_episode(steps=500)
print(f"Episode completed in {stats['episode_time']:.2f} seconds")
```

### 2.3 Convenience Functions

```python
from python.nlm import (
    create_simple_brain, 
    create_simple_world, 
    create_agent_environment,
    get_example_config,
    list_available_configs
)

# Create brain with preset configuration
brain = create_simple_brain(neuron_count=200, regions=1)

# Get example configuration
config = get_example_config("high_activity")
print(config)

# List available presets
presets = list_available_configs()
print(presets)
```

## 3. Command-Line Interface

### 3.1 Overview

The CLI (`python/cli.py`) provides advanced functionality for power users and automation.

### 3.2 Commands

#### Create Brain

```bash
nlm create-brain --name mybrain --neurons 1000 --regions 2 --auto-init
```

#### Create World

```bash
nlm create-world --width 30 --height 30 --vision-width 12 --vision-height 12
nlm create-world --agent-start-x 5.0 --agent-start-y 5.0
```

#### Run Simulation

```bash
nlm run-simulation --steps 500 --save simulation.bin --verbose
nlm run-simulation --steps 1000 --world-update --log-interval 100
```

#### Run Agent Episode

```bash
nlm create-agent --enable-reward --enable-curiosity --enable-development
nlm run-episode --steps 200 --timestep 0.05
```

#### State Management

```bash
nlm save-state --file brain_state.bin
nlm load-state --file brain_state.bin
nlm get-stats
```

#### Examples

```bash
nlm run-example --example silent
nlm run-example --example watching
nlm list-configs
```

### 3.3 Configuration Files

The CLI supports configuration files in JSON or INI format:

**JSON example (`config.json`)**:
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp.enable": true,
        "hebbian.enable": true
    }
}
```

**INI example (`config.ini`)**:
```ini
# Configuration for NLM simulation
brain.neuron_count = 1000
brain.region_count = 2
brain.connection_probability = 0.1
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
```

## 4. Enhanced Documentation

### 4.1 Usage Guides

The project now includes comprehensive documentation:

- `easy_usage.md`: Quick start guide for beginners
- `HOW_TO_USE.md`: Complete usage documentation with examples
- `python/nlm.py`: Python API reference
- `python/cli.py`: CLI documentation

### 4.2 Example Categories

#### 4.2.1 Basic Examples

- **Silent Brain**: Neurons without sensory input
- **World Watching**: Brain observing environment without acting
- **Complete Agent**: Full agent-environment interaction

#### 4.2.2 Advanced Examples

- **Development Simulation**: Brain development over time
- **Learning with Rewards**: Reward-based learning
- **Multiple Regions**: Multi-region brain architecture

#### 4.2.3 Configuration Examples

- **Default Configuration**: Balanced settings for most use cases
- **Minimal Configuration**: Smallest working brain
- **High Activity**: Large, active brain with many connections

## 5. Error Handling Improvements

### 5.1 Better Error Messages

The improved error handling provides clear, actionable error messages:

```python
brain = NLMBrain()
brain.initialize()

# Clear error message
brain.step(1)  # "Brain must be initialized before stepping. Call initialize() first."
```

### 5.2 Validation

Input validation prevents common mistakes:

- Invalid configuration values
- Missing required parameters
- Type mismatches

### 5.3 Exception Types

The wrapper uses appropriate exception types:

- `RuntimeError`: For operational errors
- `ValueError`: For invalid input values
- `FileNotFoundError`: For missing files

## 6. Performance Monitoring

### 6.1 Built-in Monitoring

The Python API includes basic performance monitoring:

```python
import time
from python.nlm import create_simple_brain

brain = create_simple_brain(500, 2)

start_time = time.time()
brain.initialize()
init_time = time.time() - start_time
print(f"Initialization time: {init_time:.3f}s")

start_time = time.time()
brain.step(100)
step_time = time.time() - start_time
print(f"Step time: {step_time:.3f}s per 100 steps")
```

### 6.2 Performance Statistics

The `get_stats()` method provides performance metrics:

```python
stats = brain.get_stats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f}")
print(f"E/I ratio: {stats['excitatory_inhibition_ratio']:.2f}")
```

## 7. Migration Guide

### 7.1 From Old to New API

#### 7.1.1 Configuration Loading

**Old:**
```python
config = pynlm.createDefaultConfig()
# Only simple key=value files supported
```

**New:**
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")  # JSON, YAML, or INI files
```

#### 7.1.2 Brain Operations

**Old:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(10)
```

**New:**
```python
from python.nlm import NLMBrain
brain = NLMBrain(config)
brain.initialize()
brain.step(10)
```

### 7.2 Configuration Presets

The new system includes predefined configuration presets:

- **default**: Balanced settings for most use cases
- **minimal**: Smallest working brain (100 neurons)
- **high_activity**: Large, active brain with many connections
- **exploratory**: Brain focused on exploration and novelty detection
- **developmental**: Brain with enhanced developmental stages

```python
from python.nlm import get_example_config

config = get_example_config("high_activity")
brain = NLMBrain(config)
```

## 8. Testing and Validation

### 8.1 Test Suite

A comprehensive test suite (`tests/test_improvements.py`) validates all improvements:

```bash
python tests/test_improvements.py
```

### 8.2 Test Coverage

The test suite covers:

- Configuration system (file I/O, type handling)
- Python API wrapper (brain, world, environment)
- Convenience functions and examples
- Error handling and validation
- Performance characteristics

## 9. Installation and Setup

### 9.1 Building from Source

The project continues to support the original build system:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 9.2 Python Installation

```bash
# Install Python bindings
pip install -e .

# Install development dependencies
pip install pytest numpy
```

### 9.3 Running Tests

```bash
# Run unit tests
cd tests
python test_improvements.py

# Run all tests
pytest
```

## 10. Best Practices

### 10.1 Configuration Management

- Use JSON/YAML files for complex configurations
- Use environment variables for sensitive data
- Version your configuration files

### 10.2 Simulation Management

- Always call `initialize()` before using a brain
- Use `save()` to checkpoint state regularly
- Monitor performance for large simulations

### 10.3 Error Recovery

- Always handle exceptions from NLM operations
- Use checkpoints to recover from failures
- Validate inputs before simulation

## 11. Future Enhancements

The improvements provide a foundation for future enhancements:

1. **Advanced JSON/YAML Parser**: Full support for nested structures
2. **Visualization Tools**: Built-in brain and world visualization
3. **Distributed Computing**: Multi-machine simulation support
4. **Advanced Analytics**: Performance analysis and reporting
5. **Plugin System**: Extensible architecture for new features

## 12. License and Attribution

All improvements are released under the MIT License, consistent with the original NLM project.

## 13. Acknowledgments

These improvements build upon the original NLM project by:

- Preserving all existing C++ functionality
- Adding user-friendly Python interfaces
- Enhancing configuration management
- Improving documentation and examples
- Adding comprehensive testing

The original NLM project continues to serve as the foundation for all new features and improvements.

---

*For more information, visit the [NLM documentation](https://nlm.readthedocs.io) or [GitHub repository](https://github.com/nlm-project/nlm).*