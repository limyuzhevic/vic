# NLM Python Bindings

Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.

## Overview

This package provides enhanced Python bindings for the NLM C++ framework, featuring:

- **Pythonic API**: Methods designed to work naturally with Python conventions
- **Comprehensive testing**: 8 test files covering all major functionality
- **Error handling**: Robust error handling with descriptive messages
- **Convenience functions**: High-level utilities for common operations
- **Context managers**: Support for resource management and episode control
- **Performance benchmarking**: Built-in benchmarking capabilities

## Installation

The Python bindings are automatically built when installing the main package using scikit-build:

```bash
pip install .
```

For development with testing and linting:

```bash
pip install -e .[dev]
```

## Basic Usage

### Quick Start

```python
import pynlm as nlm

# Create a simulation configuration
config = nlm.create_simulation_config()
brain = nlm.Brain(config)
brain.initialize()

# Create a test environment
world = nlm.create_test_environment()

# Create agent brain
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)

# Run simulation
for step in range(10):
    brain.step(step)
```

### Pythonic Configuration

```python
# Create configuration with Pythonic API
config = nlm.Config()
config.update(
    neuron_count=1000,
    region_count=2,
    learning_rate=0.01,
    plasticity_enabled=True
)

# Load from file using convenience method
config.loadConfigFile("config.json")

# Validate configuration
config.validate("neuron_count:int,learning_rate:float")

# Save configuration
config.saveToFile("config.txt")
```

### Enhanced Brain Operations

```python
# Use context manager for controlled simulation
with brain.run(duration=10.0, timestep=0.01) as simulation:
    for step in range(100):
        simulation.step(step)

# Get neurons by type
excitatory_neurons = brain.getNeuronsByType(nlm.NeuronType.Excitatory)

# Get connections between regions
connections = brain.getConnections(source=region1, target=region2)

# Benchmark performance
results = brain.benchmark(steps=50, warmup=10)
```

### Enhanced World Operations

```python
# Quick world configuration
world.configure_simulation(width=100.0, height=100.0)

# Add object at specific position
world.add_object_at(50.0, 50.0, nlm.WorldObjectType.Resource, 10.0)

# Get nearby objects
nearby_objects = world.get_nearby_objects(50.0, 50.0, 20.0)
```

### Agent Brain Operations

```python
# Create episode context manager
with agent_brain.episode(max_duration=100.0) as episode:
    for step in range(100):
        # Process sensory input
        percept = world.getSensoryPercept()
        
        # Select action
        action = agent_brain.act(percept)
        
        # Observe percept
        agent_brain.observe(percept)
```

## API Reference

### Classes

#### Config
- `loadConfigFile(filepath)`: Load configuration from file with Pythonic error handling
- `update(**kwargs)`: Batch update using keyword arguments
- `validate(expectedTypes)`: Validate configuration with detailed error messages

#### Brain
- `run(duration, timestep)`: Context manager for controlled simulation
- `getNeuronsByType(type, **kwargs)`: Get neurons filtered by type
- `getConnections(**kwargs)`: Get connections between regions/neurons
- `benchmark(steps, warmup)`: Performance benchmarking

#### SimpleWorld
- `configure_simulation(width, height, vision_width, vision_height, max_energy, energy_decay_rate)`: Quick setup
- `add_object_at(x, y, type, value, radius)`: Pythonic object placement
- `get_nearby_objects(x, y, radius, **kwargs)`: Spatial queries with filters

#### AgentBrain
- `episode(**kwargs)`: Context manager for episode management
- `act(percept)`: Convenience method for action selection
- `observe(percept)`: Method for sensory processing

### Utility Functions

#### Environment Creation
- `create_simulation_config(filename)`: Create comprehensive simulation configuration
- `create_test_environment(width, height)`: Create test environment with sample objects
- `create_training_environment(width, height)`: Create training environment with rewards/hazards

## Testing

Run the comprehensive test suite:

```bash
pytest python/tests/
```

Individual test files:

- `test_bindings_basic.py`: Basic functionality and API essentials
- `test_bindings_config.py`: Configuration loading and management
- `test_bindings_brain.py`: Brain operations and simulation
- `test_bindings_agent.py`: Agent brain interface and control
- `test_bindings_world.py`: World simulation and spatial queries
- `test_bindings_sensory.py`: Sensory input handling
- `test_bindings_error_handling.py`: Error handling and validation
- `test_bindings_performance.py`: Performance characteristics and benchmarking

## Examples

### Basic Example

`quick_start.py` - Minimal working example to get started.

### Pythonic API Example

`example_pythonic_api.py` - Comprehensive demonstration of all Pythonic features.

### Training Example

```python
# Create training environment
world = nlm.create_training_environment(300.0, 200.0)

# Configure for training
world.configure_simulation(
    width=300.0,
    height=200.0,
    vision_width=128,
    vision_height=128,
    max_energy=200.0,
    energy_decay_rate=0.02
)

# Create brain and agent
config = nlm.create_simulation_config()
brain = nlm.Brain(config)
brain.initialize()

agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)

# Training loop
for episode in range(100):
    # Reset for new episode
    agent_brain.reset()
    
    # Run episode
    for step in range(50):
        percept = world.getSensoryPercept()
        action = agent_brain.act(percept)
        agent_brain.observe(percept)
        world.update(0.1)
```

## Error Handling

The bindings provide comprehensive error handling:

```python
config = nlm.Config()

try:
    config.loadConfigFile("nonexistent.json")
    print("File not found")
except Exception as e:
    print(f"Error loading config: {e}")

config.update(valid_field=1)
config.validate("invalid_field:string")  # May raise validation error
```

## Performance

### Benchmarking

```python
results = brain.benchmark(steps=100, warmup=20)
print(f"Steps per second: {results['steps_per_second']:.1f}")
print(f"Average time per step: {results['average_time_per_step_seconds']:.4f}s")
```

### Large-Scale Operations

```python
# Handle large configurations efficiently
config = nlm.Config()
for i in range(10000):
    config.update(**{f"param_{i}": i})

# Large-scale spatial queries
nearby = world.get_nearby_objects(50.0, 50.0, 100.0)
```

## Contributing

### Development Setup

1. Install development dependencies:

```bash
pip install -e .[dev,lint]
```

2. Run tests:

```bash
pytest python/tests/
```

3. Run linting:

```bash
ruff check python/
black --check python/
isort --check python/
```

### Code Style

- Use black for code formatting
- Use ruff for linting
- Use isort for import sorting
- Follow PEP 8 guidelines

### Testing

- Write comprehensive test coverage
- Use pytest fixtures for setup/teardown
- Test both happy path and error cases
- Include performance benchmarks

## Documentation

- This README provides basic usage and API reference
- `example_pythonic_api.py` provides comprehensive examples
- Each test file includes detailed documentation of functionality

## Migration Guide

### From C++ to Python API

**Old (C++ style):**

```cpp
Config config;
config.loadFromFile("file.json");
brain->initialize();
brain->step(0);
```

**New (Pythonic style):**

```python
config = nlm.Config()
config.loadConfigFile("file.json")

brain = nlm.Brain(config)
brain.initialize()

brain.step(0)
```

### From Basic to Enhanced API

**Basic methods:**

```python
config.set("key", "value")
brain.getTotalNeuronCount()
```

**Enhanced methods:**

```python
config.update(key="value")
brain.getNeuronsByType(nlm.NeuronType.Excitatory)
```

## Troubleshooting

### Common Issues

**Module not found:**

```bash
pip install -e .[dev]
```

**Import errors:**

```bash
pip install numpy pytest
```

**Binding compilation errors:**

```bash
pip install --no-build-isolation -e .
```

**Test failures:**

```bash
pytest python/tests/ -v --tb=short
```

## License

MIT License. See LICENSE file for details.

## Authors

NLM Authors

## Acknowledgements

- Based on NLM C++ framework
- Uses pybind11 for Python bindings
- Built with scikit-build for C++/Python integration

## Version

0.1.0

---

*For more information, visit the [NLM GitHub repository](https://github.com/nlm-project/nlm)*