# NLM Python Bindings Documentation

This document provides comprehensive information about the Python bindings for NLM (Neural Learning Machine).

## Overview

The NLM Python bindings provide a high-level interface to the C++ neural simulation framework, making it accessible to Python developers while maintaining the power and performance of the underlying implementation.

## Installation

### From Pre-built Package

```bash
pip install pynlm
```

### From Source

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install -e .

# Or install in development mode
pip install -e .
```

## Basic Usage Examples

### 1. Minimal Example

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### 2. Agent-based Simulation

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: Apply reward modulation
    if step % 100 == 0:
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Print statistics
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

### 3. Configuration Management

```python
import pynlm
import json

# Create and configure
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)

# Save to JSON file
config.saveToFile("my_config.json")

# Load from JSON file
config.loadFromFile("my_config.json")

# Get configuration summary
print(config.summary())

# Get specific values
if config.has("brain.neuron_count"):
    neurons = config.getOr("brain.neuron_count", 1000)
    print(f"Neuron count: {neurons}")
```

## API Reference

### Core Classes

#### `pynlm.Config`
Configuration management for NLM parameters.

**Methods:**
- `loadFromFile(filepath)`: Load configuration from JSON file
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save to JSON file
- `has(key)`: Check if key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all entries
- `summary()`: Get configuration summary

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
```

#### `pynlm.Brain`
Central neural simulation brain.

**Main Methods:**
- `initialize()`: Initialize with configuration
- `step(currentStep, [currentTime])`: Run simulation step
- `receiveSensoryInput(input)`: Inject sensory data
- `injectCurrent(neuron, current)`: Inject current into neuron
- `produceAction()`: Get motor action
- `reset()`: Reset state
- `save(filepath)`: Save checkpoint
- `load(filepath)`: Load checkpoint

**Properties:**
- `getTotalNeuronCount()`: Total neurons
- `getTotalSynapseCount()`: Total synapses
- `getFiringNeuronCount()`: Currently firing neurons
- `getAverageFiringRate()`: Average firing rate
- `getTotalSpikeCount()`: Total spikes
- `getDevelopmentalStage()`: Current developmental stage

#### `pynlm.AgentBrain`
Agent interface for brain-world interaction.

**Main Methods:**
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Get motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward learning
- `updateDevelopment(timestep)`: Update development

**Subsystems:**
- `enableRewardModulation(True/False)`
- `enableStructuralPlasticity(True/False)`
- `enableDevelopment(True/False)`
- `enableCuriosity(True/False)`

#### `pynlm.SimpleWorld`
2D simulation world for agent interaction.

**Main Methods:**
- `configure(width, height, visionWidth, visionHeight)`: Setup world
- `reset()`: Reset state
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world
- `applyMotorCommand(cmd, currentTime)`: Apply action

**Properties:**
- `getSensoryPercept()`: Get agent's perception
- `getAgentBody()`: Get agent state
- `addObject(obj)`: Add world object
- `removeObject(x, y)`: Remove object

### Enumerations

#### `NeuronType`
- `Excitatory`: Excitatory neuron
- `Inhibitory`: Inhibitory neuron
- `Modulatory`: Neuromodulatory neuron
- `Sensory`: Sensory neuron
- `Motor`: Motor neuron
- `Internal`: Internal neuron

#### `ActionType`
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `MoveLeft`: Move left
- `MoveRight`: Move right
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look in direction
- `Interact`: Interact with object
- `Eat`: Eat resource
- `Rest`: Rest/stay still
- `Wait`: Wait/do nothing
- `Custom`: Custom action

#### `DevelopmentalStage`
- `Initial`: Initial stage
- `CriticalPeriod`: Critical period
- `Maturation`: Maturation stage
- `Adult`: Adult stage
- `Aging`: Aging stage

### Utility Functions

#### `createDefaultConfig()`
Create a default configuration.

#### `createBrain(config)`
Create a brain with configuration.

#### `createSimpleWorld()`
Create a simple world.

#### `createAgentBrain(brain)`
Create an agent brain interface.

### Key Identifiers

#### `NeuronId`, `SynapseId`, `RegionId`, `PopulationId`
Strongly-typed identifiers for neural elements.

## Configuration Schema

### Brain Configuration
```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "connection_probability": 0.05,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
  }
}
```

### Plasticity Configuration
```json
{
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  }
}
```

### Neuromodulation Configuration
```json
{
  "neuromod": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "exploration_rate": 0.5
    },
    "novelty": {
      "enable": true,
      "detection_threshold": 0.1
    },
    "prediction_error": {
      "enable": true,
      "prediction_weight": 1.0
    }
  }
}
```

## Advanced Features

### Learning and Development

```python
# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Monitor development
for step in range(1000):
    agent.updateDevelopment(0.1)
    stage = agent.getDevelopmentalStage()
    
    # Check for developmental stage changes
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        # Special learning during critical period
        pass
    elif stage == pynlm.DevelopmentalStage.Adult:
        # Stable adult learning
        pass
```

### Memory Integration

```python
# Access memory systems through brain
brain = pynlm.createBrain(config)
brain.initialize()

# Memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Use memory for learning
working_memory.store(sensory_input, motor_command)
episodic_memory.record(experience)
```

### Prediction and Planning

```python
# Access prediction system
prediction_system = brain.getPredictionSystem()
neural_planner = brain.getPlanner()

# Make predictions
prediction = prediction_system.predict(sensory_input)
error = prediction_system.computeError(prediction, actual_result)

# Use planning
planner = brain.getPlanner()
action_sequence = planner.plan(desired_outcome, constraints)
```

## Troubleshooting

### Common Issues

1. **"My brain isn't doing anything"**
   ```python
   # Did you call initialize()?
   brain.initialize()
   ```

2. **"The agent isn't moving"**
   ```python
   # Did you call applyMotorCommand()?
   action = agent.decodeMotorCommand()
   world.applyMotorCommand(action, world.getSimulationTime())
   ```

3. **"Everything is 0"**
   ```python
   # Brains need time to "warm up"
   # Try more steps or add initial input
   brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
   ```

4. **"ImportError: No module named 'pynlm'"**
   ```bash
   pip install --force-reinstall .
   ```

### Performance Tips

1. **Use appropriate neuron count**: Start with 100-500 neurons for testing
2. **Adjust timestep**: Larger timesteps for faster simulation, smaller for accuracy
3. **Enable/disable subsystems**: Disable learning for faster testing
4. **Use checkpointing**: Save frequently for long simulations

## Extending the Library

### Adding Custom Neurons

```python
# Custom neuron types can be added by extending the C++ code
# This requires modifying the core C++ classes and rebuilding
```

### Custom Actions

```python
# Define custom action types
# Requires extending ActionType enum in C++ and adding Python bindings
```

### Specialized Worlds

```python
# Create custom world implementations
# Inherits from SimpleWorld or implements the world interface
```

## Migration Guide

### From Version 0.1 to 0.2

1. Configuration files are now JSON format (backward compatible)
2. New developmental stages added
3. Enhanced memory systems
4. Improved Python bindings documentation

### Breaking Changes

- None in current version (0.2+)

## Research and References

For more information about NLM and its scientific basis:

- Documentation in `docs/` directory
- Research papers on brain-inspired computing
- NLM project website

## License

MIT License

## Contributing

Contributions are welcome! See the CONTRIBUTING.md file for guidelines.

---

*This documentation is auto-generated from the NLM source code.*
