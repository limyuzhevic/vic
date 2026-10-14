# NLM Python API - Enhanced Documentation
# Complete guide for using NLM Phase 6 in Python

## Overview

This document provides comprehensive documentation for the NLM Python API, which wraps the C++ NLM (Neural Learning Machine) Phase 6 integration into a Python-friendly interface.

## What is NLM?

NLM (熙然) is an experimental artificial brain simulation that implements:
- **Spiking Neural Networks**: Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- **Multiple Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection for motivation and exploration
- **Prediction Systems**: Neural prediction and planning
- **Cognition**: Attention, concept formation, and self-model
- **Development**: Developmental stages and structural plasticity
- **Closed-loop Control**: Interaction with environment and learning from consequences

## Quick Start

### Installation

The NLM Python bindings are installed via pip. For development, clone this repository and build with:

```bash
# Standard installation
pip install .

# Development installation (editable mode)
pip install -e .
```

Or build with scikit-build:

```bash
pip install build
python -m build
pip install dist/*.whl
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run for some steps
for i in range(100):
    brain.step(i)

print(f"Brain thinking {brain.getTotalSpikeCount()} times")
```

## Core API Classes

### Brain

The central neural simulation brain class. It contains all the integrated systems:

**Methods:**
- `initialize()` - Initialize the brain with current configuration
- `step(step_number)` - Perform one simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron_id, current)` - Inject current into specific neuron
- `produceAction()` - Get motor command from brain activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Properties:**
- `getTotalNeuronCount()` - Total neuron count
- `getTotalSynapseCount()` - Total synapse count
- `getFiringNeuronCount()` - Currently firing neurons
- `getAverageFiringRate()` - Average firing rate (Hz)
- `getTotalSpikeCount()` - Total spike count
- `getDevelopmentalStage()` - Current developmental stage

**Example:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Config

Configuration class for NLM system settings:

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `summary()` - Get configuration summary

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
```

### AgentBrain

Interface connecting the NLM brain to the environment:

**Methods:**
- `initialize(world)` - Initialize with world instance
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode action from brain activity
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Update development system

**Convenience Methods:**
- `enableRewardModulation(True/False)` - Enable/disable reward learning
- `enableCuriosity(True/False)` - Enable/disable curiosity-driven exploration
- `enableDevelopment(True/False)` - Enable/disable brain development

**Example:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### SimpleWorld

2D simulation world for agent-environment interaction:

**Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world size
- `reset()` - Reset world state
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, current_time)` - Apply action
- `getSensoryPercept()` - Get what agent sees
- `getAgentBody()` - Get agent body state

**Properties:**
- `getWidth()`, `getHeight()` - World dimensions
- `getMaxEnergy()`, `setMaxEnergy(value)` - Energy management
- `getEnergyDecayRate()`, `setEnergyDecayRate(rate)` - Energy decay

**Example:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Agent sees its environment
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Brain decides what to do
brain.step(step)
action = agent.decodeMotorCommand()

# Action affects the world
world.applyMotorCommand(action, world.getSimulationTime())
```

## Complete Example: Agent in Environment

Here's a complete example of an agent learning to navigate in a world:

```python
import pynlm

def run_simulation(num_steps=1000):
    # Setup brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setAgentStart(15, 15)  # Start in middle
    world.reset()
    
    # Setup agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting simulation...")
    print(f"Initial position: {world.getAgentBody().x}, {world.getAgentBody().y}")
    
    # Run simulation
    for step in range(num_steps):
        # 1. World updates
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 3. Brain processes
        brain.step(step)
        
        # 4. Agent decides action
        action = agent.decodeMotorCommand()
        
        # 5. World reacts to action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Learn from experience
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: pos=({body.x:.1f}, {body.y:.1f}), "
                  f"action={action}, reward={result.reward:.2f}, "
                  f"neurons_firing={brain.getFiringNeuronCount()}")
    
    print("\nSimulation complete!")
    print(f"Final position: {world.getAgentBody().x}, {world.getAgentBody().y}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")

# Run the simulation
if __name__ == "__main__":
    run_simulation(500)
```

## Advanced API Usage

### Multiple Brains and Worlds

```python
# Create multiple agents with different configurations
configs = [
    pynlm.createDefaultConfig(),  # Default
    pynlm.createDefaultConfig(),  # Copy
]

configs[0].set("neuron_count", 500)   # Smaller brain
configs[1].set("neuron_count", 2000)  # Larger brain

agents = []
for i, config in enumerate(configs):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agents.append((brain, world, agent))
```

### Configuration Loading

```python
# Load configuration from file
config = pynlm.createDefaultConfig()
if config.loadFromFile("config.json"):
    print("Loaded configuration from config.json")
else:
    print("Using default configuration")

# Override with command line arguments
config.loadFromArgs(len(sys.argv), sys.argv)

# Save configuration
config.saveToFile("my_config.json")
```

### Working with Sensory Inputs

```python
# Get vision data
vision = percept.getVision()
data = vision.getData()
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()

# Process vision data
for x in range(width):
    for y in range(height):
        # Access pixel data
        pixel_data = data[x * height * channels + y * channels + 0]  # Red channel
        # ... process pixel ...
```

### Motor Actions

```python
# Create different actions
action_forward = pynlm.Action(pynlm.MotorCommand.MoveForward)
action_turn_left = pynlm.Action(pynlm.MotorCommand.TurnLeft)
action_look = pynlm.Action(pynlm.ActionType.Look, [0.5, 0.0, 0.0])  # Look right

# Set action parameters
action_look.setParameters([0.5, 0.0, 0.0])  # Look direction vector
```

## Configuration Reference

Common configuration parameters for NLM Phase 6:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | size_t | 1000 | Total number of neurons |
| `region_count` | size_t | 1 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of random connections |
| `simulation_timestep` | double | 0.001 | Simulation time step (seconds) |
| `random_seed` | uint64_t | 42 | Random seed for reproducibility |
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse removal |
| `replay_interval` | size_t | 100 | Steps between memory replay |
| `consolidation_interval` | size_t | 1000 | Steps between memory consolidation |

**Example configuration loading:**

```python
config = pynlm.createDefaultConfig()

# Set parameters
config.set("neuron_count", 500)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("random_seed", 123)

# Get parameters
neuron_count = config.getOr("neuron_count", 1000)
simulation_timestep = config.getOr("simulation_timestep", 0.001)
```

## Error Handling

NLM uses standard Python exceptions for error handling:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Error initializing brain: {e}")
    # Handle error...
```

Common errors:
- `RuntimeError` for initialization failures
- `ValueError` for invalid parameters
- `FileNotFoundError` for missing files

## Performance Tips

### For Large Simulations

1. **Batch Processing**: Run multiple steps at once when possible
2. **Memory Management**: Monitor memory usage for long simulations
3. **Parallel Processing**: Use multiple CPU cores if available

2. **Configuration Optimization**: Use appropriate neuron counts for your use case

### For Development and Testing

1. **Small Test Configurations**: Start with smaller brain sizes for testing
2. **Checkpointing**: Use checkpointing to save progress and resume later
3. **Logging**: Enable verbose logging for debugging

## Advanced Features

### Working Memory

Access working memory traces for maintaining information across steps:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# ... run simulation ...

working_memory = brain.getWorkingMemory()
if working_memory:
    traces = working_memory.getActiveTraces()
    print(f"Working memory has {traces} active traces")
```

### Episodic Memory

Store and recall experiences:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# ... run simulation ...

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    episode_count = episodic_memory.getEpisodeCount()
    print(f"Episodic memory has {episode_count} episodes")
    
    # Get episodes for replay
    episodes = episodic_memory.getEpisodesForReplay(5)
    print(f"Retrieved {len(episodes)} episodes for replay")
```

### Neuromodulation

Monitor neuromodulation systems:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# ... run simulation ...

# Get neuromodulation levels
agent = pynlm.createAgentBrain(brain)
novalty_level = agent.getNoveltyLevel()
curiosity_level = agent.getCuriosityLevel()
dopamine_level = agent.getNeuromodulationLevel()
prediction_error = agent.getPredictionError()

print(f"Novelty: {novalty_level:.3f}, Curiosity: {curiosity_level:.3f}")
print(f"Dopamine: {dopamine_level:.3f}, Prediction Error: {prediction_error:.3f}")
```

## Examples and Tutorials

### Tutorial 1: Simple Agent

See `examples/agent_basics.py` for a complete example of a simple agent learning to navigate.

### Tutorial 2: Memory Systems

See `examples/memory_systems.py` for examples of working with different memory types.

### Tutorial 3: Development

See `examples/development.py` for examples of brain development over time.

## API Reference

For complete API documentation, see the `pybind11` documentation generated when building the Python bindings.

## Migration Guide

### From Phase 3/4 to Phase 6

Phase 6 adds many new features while maintaining backward compatibility:

- **New Memory Systems**: Working memory, episodic memory added
- **Enhanced Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction Systems**: Neural prediction and planning
- **Cognition**: Attention, concept formation, self-model
- **Development**: Developmental stages and structural plasticity

Most existing Phase 3/4 code will work with Phase 6 with minimal changes.

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure the package is installed: `pip install pynlm`
   - Try `pip install -e .` for development installation

2. **RuntimeError: Failed to initialize brain**
   - Check that configuration is valid
   - Ensure required parameters are set
   - Try with a minimal configuration

3. **Performance Issues**
   - Use smaller brain sizes for development/testing
   - Monitor memory usage
   - Consider checkpointing for long simulations

4. **Python/C++ Compatibility**
   - Ensure compatible Python and C++ versions
   - Check for compiler compatibility issues

## Future Enhancements

The NLM Python API is continuously evolving. Future enhancements may include:

- **Enhanced Visualization**: Built-in plotting and visualization tools
- **Multi-Agent Support**: Multiple agents interacting with each other
- **GPU Acceleration**: CUDA support for faster computation
- **Web API**: REST API for remote brain control
- **Integration with ML Frameworks**: TensorFlow/PyTorch compatibility

## Support

For support, issues, or questions:

1. **GitHub Issues**: Report bugs or request features
2. **Documentation**: Check the latest documentation
3. **Examples**: Use the provided examples as starting points

## License

NLM is licensed under the MIT License. See `LICENSE` file for details.

---

This document covers NLM Phase 6. For other phases, see phase-specific documentation.
