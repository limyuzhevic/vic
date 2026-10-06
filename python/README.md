# NLM Python Bindings

## Overview

This repository provides Python bindings for the NLM (Neural Learning Machine) neural simulation framework. The bindings enable Python users to create, simulate, and interact with artificial neural networks using the comprehensive NLM brain simulation system.

## Quick Start

### Installation

```bash
# Install from pip (if available)
pip install pynlm

# Or install in development mode
pip install -e .
```

### Basic Usage

```python
import pynlm as nlm

# Create a brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get results
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Features

- **Full Brain Simulation**: Complete access to NLM neural simulation capabilities
- **Config Management**: Load/save configuration from files
- **Real-time Control**: Step-by-step simulation control
- **Agent Interface**: Connect brain to world environment
- **Memory Systems**: Access to working memory and episodic memory
- **Neuromodulation**: Reward, curiosity, novelty, and prediction error systems
- **Development**: Brain development stages and plasticity
- **Error Handling**: Comprehensive error handling with descriptive messages
- **Type Safety**: Strong typing with Python-friendly API

## Quick Examples

### Basic Brain Simulation

```python
import pynlm as nlm

# Configure brain
config = nlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 2)

# Create and initialize brain
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getActiveNeuronCount()} active neurons")

print("Simulation completed successfully!")
```

### Agent with World

```python
import pynlm as nlm

# Create components
brain = nlm.createBrain(nlm.createDefaultConfig())
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Setup world
world.configure(20, 20, 10, 10)
world.setAgentStart(10, 10)

# Initialize
brain.initialize()
agent.initialize(world)

# Run simulation
for step in range(200):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process and act
    agent.processSensoryInput(percept)
    cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(cmd, step)
    
    # Apply learning
    reward = world.computeReward(world.getAgentBody())
    predicted_reward = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted_reward)

print("Agent simulation completed!")
```

## Installation from Source

### Prerequisites

- Python 3.8+
- CMake 3.16+
- C++ compiler with C++20 support

### Build Instructions

```bash
# Clone the repository
# (if not already cloned)

# Build with CMake
mkdir build
cd build
cmake ..
make -j4

# Install to current environment
pip install -e .
```

## Python API Reference

### Core Classes

- **Brain**: Main neural simulation engine
- **AgentBrain**: Interface between brain and environment
- **SimpleWorld**: 2D simulation environment
- **Config**: Configuration management
- **Vision, Audio, InternalSignals**: Sensory input types

### Key Methods

#### Brain
- `initialize()` - Initialize the brain
- `step(step, time=None)` - Perform simulation step
- `produceAction()` - Generate motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `getTotalNeuronCount()` - Get neuron count
- `getAverageFiringRate()` - Get average firing rate

#### AgentBrain
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted)` - Apply reward learning

#### SimpleWorld
- `configure(width, height, visionW, visionH)` - Configure world
- `update(timestep)` - Update simulation
- `applyMotorCommand(cmd, time)` - Apply action
- `getSensoryPercept()` - Get sensory input

### Enumeration Types

- **NeuronType**: Excitatory, Inhibitory, Modulatory, etc.
- **ActionType**: Movement and interaction types
- **WorldObjectType**: Environment object types
- **DevelopmentalStage**: Brain development stages

## Error Handling

The bindings include comprehensive error handling:

```python
try:
    # Create invalid ID (will raise exception)
    neuron_id = NeuronId(-1)
except nlm.InvalidNeuronIdError as e:
    print(f"Error: {e}")
```

## Advanced Features

### Development Stages

```python
brain = nlm.createBrain(config)
brain.initialize()

# Simulate development
for step in range(1000):
    brain.step(step)
    
    # Check development stage
    stage = brain.getDevelopmentalStage()
    if stage == nlm.DevelopmentalStage.CriticalPeriod:
        print("Critical period - high plasticity")
```

### Memory Systems

```python
brain = nlm.createBrain(config)
brain.initialize()

# Access memory
working_memory = brain.getWorkingMemory()
if working_memory:
    # Store experience
    percept = nlm.SensoryPercept()
    working_memory.storeExperience(percept)
```

### Neuromodulation

```python
agent = nlm.createAgentBrain(brain)

# Simulate neuromodulation
for step in range(100):
    reward = random.random() * 10
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
    
    # Check neuromodulation levels
    print(f"Dopamine: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
```

## Performance Tips

1. **Reuse Objects**: Create and reuse Brain/AgentBrain objects
2. **Batch Processing**: Process multiple steps before accessing properties
3. **Configuration**: Load config from file once
4. **Memory**: Clean up references when finished
5. **Logging**: Use logging for debugging, disable in production

## Troubleshooting

### Common Issues

- **Module not found**: Run `pip install -e .`
- **Shared library error**: Reinstall the module
- **Simulation crashes**: Validate input parameters
- **Performance issues**: Check for memory leaks

## Documentation

For comprehensive documentation, see:
- `PythonBindingsDocumentation.md` - Full API reference
- `HOW_TO_USE.md` - Usage instructions
- `easy_usage.md` - Easy setup guide

## License

MIT License

## Support

For issues and questions, please refer to the main NLM documentation and issue tracker.
