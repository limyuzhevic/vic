# Enhanced NLM Python API Documentation
# This document provides comprehensive information about the enhanced Python bindings for NLM

## Overview

The NLM (Neural Learning Machine) Python bindings have been enhanced to provide a more intuitive, powerful, and user-friendly interface for working with neural simulations. The enhanced API includes:

- High-level abstractions for common use cases
- Better error handling and validation
- Convenience methods for configuration
- Training and testing utilities
- Visualization and monitoring tools
- Checkpoint management
- Episode-based agent interaction

## Installation and Setup

### Prerequisites

```bash
# Install the NLM package
pip install .

# For development, install in editable mode
pip install -e .

# Install additional visualization dependencies (optional)
pip install matplotlib seaborn pandas
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Enhanced Python API Reference

### 1. Configuration (PyConfig)

The enhanced `Config` class provides better error handling and convenience methods.

#### Basic Operations

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)

# Get configuration values
neuron_count = config.get("brain.neuron_count")
learning_rate = config.get("plasticity.stdp.learning_rate")

# Get with default value (Pythonic)
default_rate = config.get("plasticity.stdp.learning_rate", 0.01)

# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Convenience methods for common configurations
config.configure_brain(neuron_count=500, synapse_density=0.1)
config.configure_plasticity(enable_stdp=True, learning_rate=0.002)
config.configure_neuromodulation(enable_curiosity=True, enable_novelty=True)
```

#### Error Handling

```python
config = pynlm.createDefaultConfig()

try:
    value = config.get("nonexistent.key")
    print(f"Found value: {value}")
except pynlm.NLMError as e:
    print(f"Configuration error: {e}")
```

### 2. Brain (PyBrain)

The enhanced `Brain` class provides high-level simulation APIs.

#### Basic Simulation

```python
import pynlm
import time

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Run simulation using high-level API
results = brain.run_simulation(100, timestep=0.001)

# Process results
for result in results[:10]:  # First 10 steps
    print(f"Step {result['step']}: Active={result['active_neurons']}, "
          f"Firing={result['firing_neurons']}, Rate={result['average_firing_rate']:.4f}")
```

#### Simulation Control

```python
# Reset and reinitialize
brain.reset_and_initialize()

# Save and load brain state
brain.save("brain_checkpoint.bin")
# ... later ...
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("brain_checkpoint.bin")

# Get comprehensive statistics
stats = brain.get_stats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Average firing rate: {stats['average_firing_rate']:.4f} Hz")
print(f"E/I ratio: {stats['e_i_ratio']:.3f}")
```

#### Sensory and Motor Operations

```python
# Inject current into sensory/motor neurons
brain.inject_current_to_sensory(5.0)  # 5.0 units of current
brain.inject_current_to_motor(2.0)   # 2.0 units of current

# Direct neuron injection (advanced)
# brain.inject_current_to_neuron(neuron_id, current)
```

### 3. AgentBrain (PyAgentBrain)

The enhanced `AgentBrain` class provides high-level interaction APIs.

#### Basic Agent Interaction

```python
import pynlm

# Create agent environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enable_subsystems(reward_modulation=True, curiosity=True, development=True)

# Run single interaction step
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    print(f"Step {step}: {agent.getThoughtTrace()}")
```

#### Episode-based Interaction

```python
# Run complete episode with reward function
def simple_reward_function(step, state):
    """Simple heuristic reward function"""
    if step % 10 == 0:
        return 0.1  # Small exploration bonus
    elif step > 50 and step % 20 == 0:
        return 1.0  # Milestone reward
    return 0.0

results = agent.run_episode(world, num_steps=100, reward_function=simple_reward_function)

# Get interaction statistics
status = agent.get_status()
print(f"Curiosity: {status['curiosity']:.3f}")
print(f"Novelty: {status['novelty']:.3f}")
print(f"Learning preference: {status['learned_preference']:.3f}")
```

### 4. Visualization Utilities (PyVisualization)

The `PyVisualization` class provides tools for analyzing and visualizing neural activity.

#### Activity Visualization

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Visualize neural activity
PyVisualization.plot_neural_activity(brain, num_steps=100)

# Analyze spike patterns
PyVisualization.plot_spike_patterns(brain, num_steps=1000)

# Generate comprehensive summary
PyVisualization.generate_activity_summary(brain)
```

#### World Interaction Visualization

```python
PyVisualization.plot_world_interaction(agent, world, num_steps=50)
```

### 5. Training Utilities (PyTraining)

The `PyTraining` class provides tools for training and evaluating agents.

#### Simple Training Loop

```python
def reward_function(step, state):
    """Custom reward function based on state"""
    curiosity = state.get('curiosity', 0.0)
    
    # Reward novelty/exploration
    if curiosity > 0.5:
        return 0.5
    
    # Small reward for survival
    return 0.1

# Train agent
results = PyTraining.train_agent(
    agent=agent,
    world=world,
    num_episodes=100,
    steps_per_episode=200,
    reward_function=reward_function
)

# Analyze training results
print("Training Results:")
for result in results:
    print(f"Episode {result['episode']}: "
          f"Reward={result['total_reward']:.2f}, "
          f"Avg={result['average_reward_per_step']:.3f}, "
          f"Final Curiosity={result['final_curiosity']:.3f}")
```

#### Built-in Reward Functions

```python
# Use built-in heuristic reward function
def heuristic_reward(step, state):
    return PyTraining.heuristic_reward_function(step, state)
```

## Advanced Features

### 1. Configuration Presets

```python
def create_exploratory_config():
    """Create configuration optimized for exploration"""
    config = pynlm.createDefaultConfig()
    config.configure_brain(neuron_count=2000, synapse_density=0.2)
    config.configure_plasticity(enable_stdp=True, learning_rate=0.001)
    config.configure_neuromodulation(enable_curiosity=True, enable_novelty=True)
    return config

def create_expert_config():
    """Create configuration optimized for expert performance"""
    config = pynlm.createDefaultConfig()
    config.configure_brain(neuron_count=1000, synapse_density=0.15)
    config.configure_plasticity(enable_stdp=True, learning_rate=0.01)
    config.configure_neuromodulation(enable_curiosity=False, enable_novelty=False)
    return config
```

### 2. Custom Agent Classes

```python
class CustomAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.agent.enable_subsystems(reward_modulation=True, curiosity=True)
    
    def act(self, percept, step):
        self.agent.processSensoryInput(percept)
        self.agent.getBrain().step(step)
        action = self.agent.decodeMotorCommand()
        return action
    
    def learn(self, reward, predicted_reward):
        self.agent.applyRewardModulation(reward, predicted_reward)
        self.agent.updateDevelopment(0.1)
```

### 3. Multi-Agent Support

```python
# Create multiple agents for multi-agent simulations
agents = []
for i in range(3):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agents.append(agent)

# Multi-agent interaction loop
for step in range(100):
    # Get actions from all agents
    actions = [agent.decodeMotorCommand() for agent in agents]
    
    # Apply actions to world
    for i, agent in enumerate(agents):
        # Process sensory input (agents may share world state)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Learn from shared outcome
        reward = 0.1  # Shared reward
        agent.applyRewardModulation(reward, 0.0)
```

## Error Handling

The enhanced API includes comprehensive error handling:

### Custom Exceptions

```python
import pynlm

try:
    # This will raise an NLMError if key doesn't exist
    value = config.get("nonexistent.key")
except pynlm.NLMError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Validation

The API includes built-in validation for:
- Configuration keys
- Neuron IDs
- Brain initialization state
- World configuration validity
- Agent initialization requirements

## Performance Considerations

### Memory Management

```python
# Use context managers for resource management
with brain_session as session:
    # Your simulation code here
    pass

# Explicit cleanup
brain.save("checkpoint.bin")
del brain
import gc
gc.collect()
```

### Simulation Optimization

```python
# Use efficient simulation patterns
def efficient_simulation(brain, num_steps):
    """Optimized simulation loop"""
    results = []
    for i in range(num_steps):
        # Batch operations when possible
        brain.step(i)
        
        # Periodic checkpointing
        if i % 1000 == 0:
            brain.save(f"checkpoint_{i}.bin")
        
        # Log statistics periodically
        if i % 100 == 0:
            stats = brain.get_stats()
            print(f"Step {i}: {stats['total_spikes']} total spikes")
    
    return results
```

## Examples and Tutorials

### 1. Getting Started Tutorial

```python
# This tutorial walks through basic NLM usage
# It covers:
# - Creating a brain
# - Running a simple simulation
# - Interacting with a world
# - Training an agent

# See examples/getting_started.py for the complete tutorial
```

### 2. Advanced Tutorial

```python
# This tutorial covers advanced features:
# - Custom reward functions
# - Multi-agent simulations
# - Advanced configuration
# - Performance optimization

# See examples/advanced_tutorial.py for the complete tutorial
```

### 3. API Reference

```python
# Complete API documentation
# See docs/api_reference.md for detailed documentation
# of all classes, methods, and parameters
```

## Troubleshooting

### Common Issues

#### 1. "Brain not initialized"

```python
# Solution: Always call initialize() after creating a brain
brain = pynlm.createBrain(config)
brain.initialize()  # <- This is required!
```

#### 2. Configuration key not found

```python
# Solution: Use .get() with default value
value = config.get("some.key", default_value)
```

#### 3. Agent not working with world

```python
# Solution: Ensure both agent and world are properly initialized
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### 4. Simulation running slowly

```python
# Solution: Optimize simulation parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller brain
brain = pynlm.createBrain(config)
```

### Error Messages

- `NLMError: Configuration key "xxx" not found` - Check configuration key spelling
- `NLMError: Brain not initialized` - Call brain.initialize() after creation
- `NLMError: Invalid neuron ID` - Use valid neuron IDs from brain regions
- `NLMError: World configuration invalid` - Check world dimensions and settings

## Migration Guide

### From Old API to New API

#### Configuration

**Old:**
```python
config = {"brain.neuron_count": 1000, "brain.synapse_density": 0.1}
# Manual key-value access
if "brain.neuron_count" in config:
    value = config["brain.neuron_count"]
```

**New:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
# Better error handling
value = config.get("brain.neuron_count")
```

#### Brain Simulation

**Old:**
```python
# Manual step management
brain.step(step)
```

** New:**
```python
# High-level simulation with statistics
results = brain.run_simulation(100)
```

#### Agent Interaction

**Old:**
```python
# Manual agent control
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, time)
```

**New:**
```python
# High-level interaction loops
results = agent.run_episode(world, num_steps=100)
```

## Acknowledgments

The enhanced Python bindings build upon the work of:
- The NLM research team
- PyBind11 for Python bindings
- The scientific community for neural simulation research

## License

This enhanced documentation is provided under the MIT License, consistent with the NLM project.
