# NLM Python API Documentation

**Comprehensive guide for Python users of the NLM (熙然) neural simulation framework**

## Overview

The NLM Python API (`pynlm`) provides Python-friendly interfaces for simulating artificial brains with spiking neurons, memory systems, neuromodulation, and cognitive functions.

### Key Features

- **Type hints and comprehensive docstrings**: Full Python documentation with type hints
- **Pythonic property access**: Intuitive attribute-based access to brain properties
- **Convenience functions**: High-level functions for common simulation patterns
- **Enhanced error handling**: Better error messages and exception handling
- **Built-in configuration presets**: Multiple configuration templates for different use cases
- **Complete simulation loop helpers**: Functions to run entire simulation episodes
- **Backward compatibility**: Existing API maintained while adding new features

## Installation

```bash
pip install pynlm
```

## Quick Start

### Example 1: Simplest Brain

```python
import pynlm

# Create and initialize a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
brain.step(0)

print(f"Your brain has {brain.neuron_count} neurons!")
print(f"It has fired {brain.total_spike_count} times.")
```

### Example 2: Complete Agent in a World

```python
import pynlm

def run_complete_simulation():
    # Setup components with convenience functions
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation using convenience function
    results = pynlm.run_episode(brain, agent, world, num_steps=100, verbose=True)
    
    avg_reward, max_firing = results
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Max firing neurons: {max_firing}")
    
    return brain, world, agent

brain, world, agent = run_complete_simulation()
```

## Configuration Templates

### 1. Default Configuration (`createDefaultConfig()`)

```python
config = pynlm.createDefaultConfig()
# Use for general purpose simulations
```

### 2. Minimal Configuration (`createMinimalConfig()`)

```python
config = pynlm.createMinimalConfig()
# Use for quick tests and debugging
```

### 3. Research Configuration (`createResearchConfig()`)

```python
config = pynlm.createResearchConfig()
# Use for advanced research experiments
```

## Core Classes

### 1. Brain Class

The main neural simulation brain class with real spiking neuron computation.

**Attributes:**
- `neuron_count`: Total number of neurons in the brain
- `synapse_count`: Total number of synapses in the brain  
- `firing_neuron_count`: Number of currently firing neurons
- `average_firing_rate`: Average firing rate across all neurons (Hz)
- `total_spike_count`: Total number of spikes generated
- `developmental_stage`: Current developmental stage

**Key Methods:**

```python
# Create and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation step
brain.step(0)                    # Step with default time
brain.step(step, time=0.001)     # Step with specific time

# Get statistics
print(f"Neurons: {brain.neuron_count}")
print(f"Firing rate: {brain.average_firing_rate:.2f} Hz")
print(f"Total spikes: {brain.total_spike_count}")

# Sensory and motor
brain.receiveSensoryInput(percept)
action = brain.produceAction()

# State management
brain.reset()
brain.save("brain_state.bin")
brain.load("brain_state.bin")
```

### 2. AgentBrain Class

Interface connecting NLM brain to world for complete perception-action loops.

**Attributes:**
- `curiosity_level`: Current curiosity (exploration motivation)
- `novelty_level`: Current novelty detection level
- `neuromodulation_level`: Current neuromodulation strength
- `prediction_error`: Current prediction error signal
- `developmental_stage`: Current developmental stage

**Key Methods:**

```python
# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Process sensory input
agent.processSensoryInput(percept)

# Decode action
action = agent.decodeMotorCommand()

# Apply reward learning
agent.applyRewardModulation(reward, predicted_reward)
```

### 3. SimpleWorld Class

2D environment for NLM simulation with physics, objects, and sensory processing.

**Key Methods:**

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Update simulation
world.update(0.1)

# Get sensory input
percept = world.getSensoryPercept()

# Get agent state
agent_body = world.getAgentBody()

# Apply actions
result = world.applyMotorCommand(action, current_time)
```

### 4. Configuration Class

Manages all simulation parameters including neural, plasticity, neuromodulation, and environmental settings.

**Key Methods:**

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Set parameters
config.set("neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Load from file/config
config.loadFromFile("config.json")
config.loadFromArgs()  # Command line

# Get parameters
if config.has("neuron_count"):
    count = config.getOr("neuron_count", 100)
```

## Convenience Functions

### 1. Quick Setup Functions

```python
# Create brain with sensible defaults
brain = pynlm.createQuickBrain()

# Create complete agent setup
brain, agent = pynlm.createAgentSetup(world)

# Run complete simulation episode
results = pynlm.run_episode(brain, agent, world, num_steps=100, verbose=True)
avg_reward, max_firing = results
```

### 2. Complete Simulation Example

```python
def run_research_simulation():
    # Use research configuration
    config = pynlm.createResearchConfig()
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run long simulation
    print("Running research simulation...")
    for step in range(1000):
        world.update(0.1)
        
        # Get observation
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Make decision
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.simulation_time)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.firing_neuron_count}, "
                  f"Curiosity: {agent.curiosity_level:.3f}")
    
    return brain, world, agent

brain, world, agent = run_research_simulation()
```

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Silent brain test done!")
print(f"Your brain has {brain.neuron_count} neurons!")
```

### Project 2: Brain Watching a World

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
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print(f"Firing rate: {brain.average_firing_rate:.2f} Hz")
```

### Project 3: Complete Learning Agent

```python
import pynlm

# Setup
config = pynlm.createResearchConfig()
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
agent.enableDevelopment(True)

# Run agent with learning
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.simulation_time)
    
    # Apply reward based on world outcome
    reward = world.computeReward(brain.getRegions()[0].get())
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 100 == 0:
        print(f"Step {step}: Firing: {brain.firing_neuron_count}, "
              f"Curiosity: {agent.curiosity_level:.3f}, "
              f"Reward: {reward:.3f}")

print("Agent simulation complete!")
```

## Type Hints and IDE Support

The API includes type hints for better IDE support and code completion:

```python
# Type hints are automatically available
brain: pynlm.Brain = pynlm.createBrain(config)
world: pynlm.SimpleWorld = pynlm.createSimpleWorld()
agent: pynlm.AgentBrain = pynlm.createAgentBrain(brain)

# IntelliSense will show available attributes and methods
brain.neuron_count          # int
brain.step(0, 0.001)        # method
brain.average_firing_rate   # float
```

## Error Handling

The API provides improved error handling with descriptive messages:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except pynlm.RuntimeError as e:
    print(f"Brain error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

### 1. Use Configuration Presets

```python
# Research simulations
config = pynlm.createResearchConfig()

# Quick tests  
brain = pynlm.createQuickBrain()
```

### 2. Monitor Performance

```python
# Track key metrics
print(f"Neurons: {brain.neuron_count}")
print(f"Firing rate: {brain.average_firing_rate:.2f} Hz")
print(f"Total spikes: {brain.total_spike_count}")
print(f"Development: {brain.developmental_stage}")
```

### 3. Efficient Simulation

```python
# Batch process steps
for step in range(100):
    brain.step(step, step * 0.001)

# Group sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

## Backward Compatibility

The Python API maintains full backward compatibility with existing code:

```python
# Old code still works
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# New code (recommended)
brain = pynlm.createQuickBrain()
brain.initialize()
```

## Advanced Usage

### 1. Custom Configurations

```python
custom_config = pynlm.createDefaultConfig()
custom_config.set("neuron_count", 5000)
custom_config.set("region_count", 4)
custom_config.set("connection_probability", 0.2)
```

### 2. Multi-Agent Systems

```python
# Multiple agents sharing the same brain
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)

agent1 = pynlm.createAgentBrain(brain)
agent1.initialize(world)
agent1.setAgentPosition(10, 10)

agent2 = pynlm.createAgentBrain(brain)
agent2.initialize(world)
agent2.setAgentPosition(40, 40)
```

### 3. Save/Load States

```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state
new_brain = pynlm.createQuickBrain()
new_brain.load("my_brain.bin")
```

## Further Reading

- **NLM Documentation**: `docs/PHASE6_ARCHITECTURE.md` for technical details
- **C++ API**: Source code for advanced customization
- **Research Papers**: Scientific background in `docs/SCIENCE.md`

## Troubleshooting

### "My brain isn't doing anything"

```python
# Solution 1: Ensure initialization
brain = pynlm.createQuickBrain()
brain.initialize()  # Always call this!

# Solution 2: Check sensory input
agent.processSensoryInput(world.getSensoryPercept())
```

### "The agent isn't moving"

```python
# Solution 1: Ensure world update
world.update(0.1)  # Call update before getting percept

# Solution 2: Check action decoding
action = agent.decodeMotorCommand()
```

### "Everything is 0"

```python
# Solution: Brains need time to "warm up"
for step in range(100):
    brain.step(step)  # Let neurons fire and connect
```

## Conclusion

The NLM Python API provides a comprehensive, Pythonic interface to the NLM neural simulation framework. Whether you're a beginner exploring spiking neural networks or a researcher running advanced experiments, this API offers the tools you need while remaining intuitive and efficient.

The improvements include better documentation, type hints, convenience functions, and enhanced error handling, making it easier than ever to create, simulate, and experiment with artificial brains.
