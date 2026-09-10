# NLM - Python Neural Learning Machine API

## Overview

The NLM Python API provides a high-level interface to the NLM (Neural Learning Machine) neural simulation framework. This API enables you to create and simulate spiking neural networks with advanced features like plasticity, development, and memory systems.

## Key Features

- **Easy-to-use Python interface** over complex C++ neural simulation core
- **Advanced neural dynamics** including real Leaky Integrate-and-Fire neurons
- **Multiple plasticity mechanisms**: STDP, Hebbian, and structural plasticity
- **Neuromodulation** systems (dopamine, curiosity, novelty)
- **Memory systems** for working, episodic, and associative memory
- **Developmental stages** from initial to adult
- **World interaction** for agent-environment simulation
- **High configurability** through Python dictionary-style configuration

## Quick Start

### Simple Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.get_total_spikes()} spikes")

print(f"Total neurons: {brain.get_neuron_count()}")
print(f"Total synapses: {brain.get_synapse_count()}")
```

### Complete Agent-World Simulation

```python
import pynlm

# Create brain, world, and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize world
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
agent.initialize(world)

# Enable advanced learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Run episode
for step in range(100):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.simulation_time)

print("Simulation complete!")
```

## Configuration Management

The NLM Python API supports flexible configuration through Python dictionaries.

### Basic Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set brain parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

### Advanced Configuration Patterns

```python
# Development-focused configuration
development_config = pynlm.create_advanced_config('development')

# Real-time configuration
realtime_config = pynlm.create_advanced_config('real_time')

# Research configuration
research_config = pynlm.create_advanced_config('research')
```

## Core Classes

### NeuralNetwork

The main class for neural network simulation.

**Key Methods:**
- `initialize()`: Initialize the neural network
- `step(step, time=None)`: Perform a simulation step
- `reset()`: Reset the network to initial state
- `get_neuron_count()`: Get the number of neurons
- `get_synapse_count()`: Get the number of synapses
- `get_firing_neurons()`: Get the number of firing neurons
- `get_total_spikes()`: Get the total number of spikes
- `get_average_firing_rate()`: Get the average firing rate
- `get_developmental_stage()`: Get the current developmental stage

**Properties:**
- `neuron_count`: Number of neurons in the network
- `synapse_count`: Number of synapses in the network
- `firing_neurons`: Number of currently firing neurons
- `total_spikes`: Total number of spikes generated
- `average_firing_rate`: Average firing rate across all neurons
- `developmental_stage`: Current developmental stage (initial, critical_period, maturation, adult)

### Synapse

Represents a synaptic connection between neurons with advanced plasticity.

**Key Features:**
- Short-term plasticity (facilitation and depression)
- Spike timing-dependent plasticity (STDP)
- Hebbian learning
- Reward-modulated learning via eligibility traces
- Synaptic efficacy modulation

### LIFNeuron

Leaky Integrate-and-Fire neuron implementation.

**Key Features:**
- Realistic membrane potential dynamics
- Refractory period handling
- Spike-frequency adaptation
- Current injection and synaptic input
- Random initialization with biological constraints

### SimpleWorld

Simple 2D world for agent interaction.

**Key Features:**
- Agent movement and positioning
- Vision sensory input
- Object interaction
- Energy and health tracking
- Time simulation

### AgentBrain

Interface connecting NLM brain to world.

**Key Features:**
- Sensory processing from world
- Motor command decoding from brain
- Reward-based neuromodulation
- Development system updates
- Curiosity and novelty integration

## Advanced Usage

### Saving and Loading

```python
# Save brain state
brain.save("brain_state.json")

# Load brain state
brain.load("brain_state.json")
```

### Configuration File Support

The NLM Python API supports multiple configuration file formats:

```python
# Load from JSON file
config.load_from_file("config.json")

# Load from YAML file
config.load_from_file("config.yaml")

# Load from INI file
config.load_from_file("config.ini")
```

### Experiment Templates

```python
import pynlm

# Create experiment template
experiment = pynlm.ExperimentTemplate("plasticity_experiment")

# Setup configuration
experiment.setup_config(
    brain.neuron_count=2000,
    plasticity.stdp.enable=True,
    plasticity.stdp.learning_rate=0.002,
    neuromod.curiosity.enable=True
)

# Create network
brain = experiment.create_network()

# Run experiment
results = experiment.run(1000)

# Save results
experiment.save_results("experiment_results.json")

# Print summary
print(experiment.get_summary())
```

### Visualization

```python
import pynlm

# Create visualizer
visualizer = pynlm.Visualizer()

# Plot neural activity
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation and visualize
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        visualizer.plot_neural_activity(brain, f"plot_{i}.txt")

# Create animation data
animation_data = visualizer.create_animation_data(brain, 500)
```

## Error Handling

The NLM Python API includes comprehensive error handling with specific exception classes:

```python
import pynlm

# Configuration errors
if config_file:
    if not config.load_from_file(config_file):
        raise pynlm.ConfigurationError(f"Failed to load configuration from {config_file}")

# Simulation errors
if step < 0:
    raise pynlm.SimulationError("Step count cannot be negative")

# Memory errors
if memory_usage > limit:
    raise pynlm.MemoryError(f"Memory usage exceeded limit: {memory_usage}")
```

## Performance Tips

### For High Performance
- Use `realtime_config` for real-time applications
- Disable unnecessary plasticity mechanisms
- Configure appropriate simulation timestep
- Use efficient network sizes for your use case

### For Development and Testing
- Use `development_config` for exploring neural behavior
- Enable all plasticity mechanisms to observe learning
- Use smaller networks for rapid iteration
- Monitor developmental stages

## Examples

### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.process_sensory_input(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Complete Agent with Learning

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
    world.applyMotorCommand(action, world.simulationTime)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.get_firing_neurons()} neurons firing")

print("Agent simulation complete!")
```

## Installation

The NLM Python API requires the NLM C++ libraries to be installed first. Please refer to the main NLM documentation for installation instructions.

Once NLM is installed, the Python API can be installed via pip:

```bash
pip install pynlm
```

For development, install in editable mode:

```bash
pip install -e .
```

## Version Information

- **Python API Version**: 0.1.0
- **NLM Version**: 0.1.0
- **License**: MIT
- **Author**: NLM Research Team

## Troubleshooting

### Common Issues

1. **"My brain isn't doing anything"**
   - Did you call `brain.initialize()`?
   - Try increasing the number of steps

2. **"The agent isn't moving"**
   - Did you call `world.applyMotorCommand()`?
   - Check that `world.update()` is being called

3. **"Everything is 0"**
   - Brains need time to "warm up" - try more steps
   - Some neurons need input to fire - make sure sensory input is connected

### Performance Issues

1. **Slow simulation**
   - Use `realtime_config` for better performance
   - Consider reducing network size
   - Disable unnecessary plasticity mechanisms

2. **Memory issues**
   - Monitor network size and synapse count
   - Use `benchmark_simulation()` to check performance
   - Consider saving and loading brain states periodically

## Further Reading

- [NLM Documentation](https://github.com/nlmbrain/nlm/blob/main/README.md) - Main NLM documentation
- [NLM Architecture](https://github.com/nlmbrain/nlm/blob/main/docs/ARCHITECTURE.md) - System architecture overview
- [NLM Science](https://github.com/nlmbrain/nlm/blob/main/docs/SCIENCE.md) - Scientific background
- [NLM Experiments](https://github.com/nlmbrain/nlm/blob/main/docs/EXPERIMENTS.md) - Experiment descriptions

## API Reference

For complete API documentation, see the individual class documentation or use Python's help() function:

```python
import pynlm
help(pynlm.NeuralNetwork)
help(pynlm.Synapse)
help(pynlm.LIFNeuron)
help(pynlm.AgentBrain)
```

## Conclusion

The NLM Python API provides a powerful, user-friendly interface to the complex NLM neural simulation framework. Whether you're a beginner looking to explore neural networks or an advanced researcher needing sophisticated simulation capabilities, the NLM Python API offers the tools you need to create, simulate, and analyze neural systems.

The API balances ease of use with advanced functionality, making it suitable for both educational purposes and serious research applications. With comprehensive configuration management, error handling, and example scripts, the NLM Python API is ready for immediate use and extensible for your specific needs.
