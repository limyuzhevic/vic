# NLM Python Examples and Tutorials

This directory contains comprehensive examples demonstrating the NLM (Neural Learning Machine) Python API.
These examples showcase the full range of NLM capabilities from basic usage to advanced features.

## Overview

The NLM Python API provides Python bindings for the C++ NLM brain simulation framework.
It allows you to:
- Create and simulate virtual brains with spiking neural networks
- Interact with the world through agents
- Use advanced memory, prediction, and cognition systems
- Configure simulations with JSON/YAML configuration files
- Run research experiments with comprehensive monitoring

## Prerequisites

- Install NLM: `pip install pynlm` or build from source
- Ensure all dependencies are installed (C++17 compatible compiler)

## Examples

### 1. Minimal Working Example

**File:** `examples/minimal_example.py`

This is the simplest possible example that demonstrates basic NLM functionality.
It creates a brain and runs a basic simulation for 10 steps.

**What you'll learn:**
- How to create a brain with default configuration
- Basic simulation loop (step execution)
- How to access brain statistics and properties

**Run:** `python examples/minimal_example.py`

### 2. Agent Simulation Example

**File:** `examples/agent_example.py`

This example demonstrates how to create an agent that interacts with a simulated world.
It shows the complete NLM agent loop: sense, think, act.

**What you'll learn:**
- How to create and configure a world
- Agent brain interface creation
- Sensory processing and motor decoding
- Complete agent-environment interaction
- Neuromodulation (curiosity, novelty)

**Run:** `python examples/agent_example.py`

### 3. Advanced Features Example

**File:** `examples/advanced_example.py`

This advanced example demonstrates all NLM systems working together including:
- Memory systems (working, episodic, associative)
- Prediction system with accuracy tracking
- Cognition systems (planning, concept formation, attention)
- Neuromodulation (curiosity, novelty, dopamine)
- Development system with age-based learning
- Advanced configuration

**What you'll learn:**
- How to create advanced configurations
- Comprehensive system integration
- Monitoring and statistics collection
- Brain state validation
- Memory system usage and tracking

**Run:** `python examples/advanced_example.py`

### 4. Configuration Management Example

**File:** `examples/config_example.py`

This example demonstrates advanced configuration management including:
- JSON configuration files
- YAML configuration files
- Configuration validation
- Environment variable substitution
- Configuration merging
- Schema validation

**What you'll learn:**
- How to use JSON and YAML configuration formats
- Configuration validation and error handling
- File I/O operations
- Environment variable substitution
- Configuration inheritance and merging

**Run:** `python examples/config_example.py`

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

### Complete Agent Example

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Advanced Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 5000)
config.setFloat("brain.synapse_density", 0.15)
config.setBool("neuromod.curiosity.enable", True)
config.setBool("plasticity.stdp.enable", True)

# Save to JSON
config.saveToFile("my_config.json")

# Load from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

## Key Features

### Brain System
- **Real Spiking Neural Networks**: Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven Dynamics**: Spike propagation and synaptic delays
- **Multiple Plasticity Mechanisms**: STDP, Hebbian, structural plasticity
- **Integrated Memory Systems**: Working, episodic, associative memory
- **Prediction System**: Forward models and prediction error
- **Cognition Systems**: Planning, concept formation, attention
- **Development**: Age-based plasticity and maturation
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error

### Python API Features
- **Pythonic Interface**: Intuitive, high-level API designed for ease of use
- **Type Safety**: Strong typing with automatic conversions
- **Error Handling**: Comprehensive error messages and validation
- **Configuration Management**: JSON/YAML support with environment variables
- **Statistics Collection**: Real-time monitoring and analysis
- **Property Access**: Easy access to system properties and states

### Configuration Options
- **Brain Configuration**: Neuron count, connectivity, biophysics
- **Plasticity Configuration**: Learning rates, time constants, mechanisms
- **Neuromodulation Configuration**: Reward prediction, curiosity, novelty
- **Memory Configuration**: Capacity, retention, consolidation
- **Development Configuration**: Maturation rates, critical periods
- **Simulation Configuration**: Time steps, integration intervals

## Performance Tips

1. **Use Appropriate Neuron Counts**: Start with 1000-5000 neurons for testing
2. **Enable/Disable Subsystems**: Turn off unused systems for better performance
3. **Use Efficient Data Types**: Leverage the configuration system for optimization
4. **Monitor Resource Usage**: Use the statistics API to track performance
5. **Batch Operations**: Group similar operations when possible

## Common Patterns

### Running Multiple Simulations
```python
for seed in [42, 123, 456]:
    config = pynlm.createDefaultConfig()
    config.setInt("random.seed", seed)
    brain = pynlm.createBrain(config)
    brain.initialize()
    # Run simulation...
```

### Comparing Configurations
```python
configs = []
for density in [0.05, 0.1, 0.2]:
    config = pynlm.createDefaultConfig()
    config.setFloat("brain.synapse_density", density)
    configs.append(config)

# Run each configuration and compare results
```

### Monitoring Brain State
```python
stats = brain.getMemoryStats()
print(f"Working memory usage: {stats['working_memory_usage']}/{stats['working_memory_capacity']}")
print(f"Prediction accuracy: {stats['prediction_accuracy']:.3f}")
print(f"Curiosity level: {stats['curiosity_level']:.3f}")
```

## Troubleshooting

### Common Issues

**ImportError: No module named 'pynlm'**
- Install the package: `pip install pynlm`
- Ensure build dependencies are installed

**RuntimeError: Brain not initialized**
- Always call `brain.initialize()` before `brain.step()`
- Use `brain.reset()` to reset brain state

**Configuration errors**
- Use the validation API: `config.validate()`
- Check error messages: `config.getValidationErrors()`

**Performance issues**
- Reduce neuron count for testing
- Disable unused subsystems
- Monitor statistics to identify bottlenecks

### Getting Help

1. **Check examples**: Run the example scripts to understand the API
2. **Read documentation**: Review this file and the NLM documentation
3. **Configuration validation**: Use the config validation API
4. **Statistics monitoring**: Use the brain statistics API

## File Structure

```
examples/
├── minimal_example.py          # Basic usage example
├── agent_example.py            # Complete agent simulation
├── advanced_example.py         # All features example
├── config_example.py           # Advanced configuration

README.md                      # This file
```

## License

MIT License - See LICENSE file for details.

## Authors

NLM Project Contributors - Research project on artificial developmental brains.

---

**Note:** These examples are designed to work with the NLM Phase 6 (FINAL INTEGRATION) system.
All features should be fully functional, though performance may vary based on system resources.
