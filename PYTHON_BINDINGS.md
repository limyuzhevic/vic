# NLM Python Bindings - User Guide

## Overview

The NLM (Neural Learning Machine) Python bindings provide a high-level interface to the NLM neural simulation framework. This guide covers the improved API for easy and effective use.

## Installation

The Python bindings are automatically installed with NLM. You can import the package using:

```python
import pynlm
```

## Core Classes

### NeuronId

A unique identifier for neurons.

```python
# Create from integer
neuron_id = pynlm.NeuronId(42)

# Create from string
neuron_id = pynlm.NeuronId.from_string("123")

# Access value
print(neuron_id.value)  # 123

# Get index
print(neuron_id.index())  # 123
```

### Config

Configuration object for NLM systems.

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create with custom parameters
config = pynlm.createConfigWithDefaults(
    neuronCount=1000,
    regionCount=2,
    seed="12345"
)

# Save configuration
config.saveToFile("my_config.cfg")

# Check if key exists
if config.has("neuron_count"):
    value = config.get("neuron_count")
```

### Brain

The central neural simulation brain.

```python
# Create brain with simple parameters
brain = pynlm.createBrain(
    neuronCount=500,
    regionCount=1,
    seed="42"
)

# Brain properties
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Run simulation
brain.step(0)
brain.step(100)
brain.step(100, 0.1)  # with timestamp

# Inject sensory input
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)

# Reset brain
brain.reset()
```

### SimpleWorld

A simple 2D simulation world.

```python
# Create world with basic dimensions
world = pynlm.createSimpleWorld(64, 64)

# Create world with specific vision parameters
world = pynlm.createSimpleWorld(
    width=128,
    height=128,
    visionWidth=32,
    visionHeight=32
)

# World properties
print(f"Width: {world.getWidth()}")
print(f"Height: {world.getHeight()}")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
```

### AgentBrain

Interface connecting NLM brain to world.

```python
# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)

# Initialize with world
agent_brain.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)

# Get motor command
command = agent_brain.decodeMotorCommand()

# Apply reward
agent_brain.applyRewardModulation(reward=1.0, predictedReward=0.8)

# Update development
agent_brain.updateDevelopment(timestep=0.001)
```

## Neuron Factory Functions

### create_sensory_neuron

Creates a sensory neuron (can be used with individual neuron management):

```python
sensory_neuron = pynlm.create_sensory_neuron(pynlm.NeuronId(1))
```

### create_motor_neuron

Creates a motor neuron (can be used with individual neuron management):

```python
motor_neuron = pynlm.create_motor_neuron(pynlm.NeuronId(2))
```

## Convenience Functions

### runSimpleBrainDemo

Runs a complete demonstration of the NLM brain:

```python
pynlm.runSimpleBrainDemo()
```

This function:
- Creates a brain with realistic parameters
- Runs 100 simulation steps
- Injects sensory input periodically
- Prints statistics about the simulation

## AgentBrain Features

### Configuration Methods

```python
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)
```

### State Accessors

```python
# Get neuromodulation levels
print(agent_brain.getNeuromodulationLevel())
print(agent_brain.getCuriosityLevel())
print(agent_brain.getNoveltyLevel())
print(agent_brain.getPredictionError())

# Get developmental stage
stage = agent_brain.getDevelopmentalStage()
print(f"Development stage: {stage}")

# Get brain reference
brain = agent_brain.getBrain()
```

### Status Checks

```python
if agent_brain.isRewardModulationEnabled():
    print("Reward modulation is active")

if agent_brain.isDevelopmentEnabled():
    print("Development is enabled")
```

## Advanced Usage Examples

### Complete Agent Simulation

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(1000, 2, "agent_seed")
world = pynlm.createSimpleWorld(100, 100, 25, 25)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Main simulation loop
for episode in range(10):
    # Reset agent for new episode
    agent.reset()
    world.reset()
    
    # Run episode
    for step in range(200):
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Get motor command
        command = agent.decodeMotorCommand()
        
        # Apply action to world
        result = world.applyMotorCommand(command, step * 0.001)
        
        # Get reward from world
        reward = result.reward
        predicted_reward = 0.0  # Could use agent's prediction system
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.001)
    
    print(f"Episode {episode + 1} completed")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Reward: {reward}")
```

### Configuration File Management

```python
import pynlm

# Save custom configuration
config = pynlm.createConfigWithDefaults(
    neuronCount=2000,
    regionCount=3,
    seed="simulation_1"
)

config.set("connection_probability", 0.05)
config.set("stdp_ltp_weight", 0.02)

config.saveToFile("my_simulation.cfg")

# Load configuration
loaded_config = pynlm.createDefaultConfig("my_simulation.cfg")

if loaded_config:
    print("Configuration loaded successfully")
    print(f"Neurons: {loaded_config.get('neuron_count')}")
    print(f"Regions: {loaded_config.get('region_count')}")
```

### Brain Comparison

```python
import pynlm

# Create two brains with different configurations
brain1 = pynlm.createBrain(500, 1, "brain1")
brain2 = pynlm.createBrain(1000, 2, "brain2")

# Compare properties
print("Brain 1:", brain1.getTotalNeuronCount(), "neurons")
print("Brain 2:", brain2.getTotalNeuronCount(), "neurons")

# Run comparison simulation
for step in [0, 50, 100]:
    brain1.step(step, step * 0.001)
    brain2.step(step, step * 0.001)
    
    print(f"Step {step}:")
    print(f"  Brain 1 - Spikes: {brain1.getTotalSpikeCount()}")
    print(f"  Brain 2 - Spikes: {brain2.getTotalSpikeCount()}")
    print(f"  Brain 1 - Firing: {brain1.getFiringNeuronCount()}")
    print(f"  Brain 2 - Firing: {brain2.getFiringNeuronCount()}")
```

## Error Handling

The Python bindings raise standard Python exceptions for errors:

```python
try:
    brain = pynlm.createBrain(1000, 1, "invalid_seed")
except Exception as e:
    print(f"Error creating brain: {e}")
```

## Performance Tips

1. **Reuse Objects**: Create Brain and World objects once and reuse them across multiple episodes or simulations.

2. **Batch Operations**: When possible, perform multiple operations together rather than individually.

3. **Configuration Caching**: Save configurations to disk and reload them for repeated simulations with the same parameters.

4. **Memory Management**: The bindings use reference counting for memory management, so there's no need to manually delete objects.

## Changelog

### Recent Improvements

- Added convenience factory functions for neurons
- Enhanced brain creation with multiple parameter sets
- Added comprehensive AgentBrain configuration methods
- Improved world creation with vision parameters
- Added detailed documentation and examples
- Added configuration file operations
- Added brain demo function

### API Changes

- `createSimpleWorld()` now accepts vision dimensions as optional parameters
- Multiple overloads for `createBrain()` to support different parameter combinations
- Added `createConfigWithDefaults()` for pre-configured settings
- Enhanced `AgentBrain` with more configuration methods

## Support

For questions, issues, or feature requests, please refer to the NLM documentation or GitHub repository.

## License

This software is licensed under the MIT License.

```
MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

For more information, visit the NLM project documentation.
