# NLM User Guide

## Overview

This guide provides comprehensive instructions for using the NLM (Neural Learning Machine) system. It covers everything from basic setup to advanced usage, including tutorials, examples, and troubleshooting.

## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Basic Operations](#basic-operations)
4. [Brain Operations](#brain-operations)
5. [World Operations](#world-operations)
6. [Agent Operations](#agent-operations)
7. [Memory Systems](#memory-systems)
8. [Neuromodulation](#neuromodulation)
9. [Development](#development)
10. [Configuration](#configuration)
11. [Examples and Tutorials](#examples-and-tutorials)
12. [Troubleshooting](#troubleshooting)
13. [Advanced Topics](#advanced-topics)
14. [References](#references)

## Introduction

NLM is a brain-inspired spiking neural network simulator that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation and multiple plasticity mechanisms.

### Key Concepts

- **Brain**: The central neural processing system
- **World**: The environment containing sensory and motor spaces
- **Agent**: The interface connecting brain and world
- **Neuromodulation**: Reward, curiosity, and novelty signals
- **Development**: Age-based plasticity and maturation
- **Memory**: Working memory, episodic memory, and other cognitive systems

## Getting Started

### Installation Requirements

Before starting, ensure you have:

1. **NLM installed** (see Installation Guide)
2. **Python 3.8+** for Python bindings
3. **Basic understanding** of neural networks and brain concepts

### First Steps

#### Step 1: Import the Module

```python
import pynlm
```

#### Step 2: Create a Brain

```python
# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

#### Step 3: Initialize the Brain

```python
brain.initialize()
```

#### Step 4: Run a Simple Simulation

```python
# Run 100 simulation steps
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.firing_neuron_count()} neurons firing")
```

## Basic Operations

### Creating Brains

#### Basic Brain Creation

```python
import pynlm

# Method 1: Use defaults
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Method 2: Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Configuration Options

| Configuration Key | Type | Default | Description |
|------------------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Probability of connection |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |

### Brain Operations

#### Initialization

```python
brain.initialize()
```

#### Running Simulation Steps

```python
for step in range(100):
    brain.step(step)
```

#### Reset Brain

```python
brain.reset()
```

#### State Management

```python
# Save brain state
brain.save("brain_state.bin")

# Load brain state
brain.load("brain_state.bin")
```

#### Statistics

```python
# Get basic statistics
print(f"Total neurons: {brain.total_neuron_count}")
print(f"Total synapses: {brain.total_synapse_count}")
print(f"Current firing: {brain.firing_neuron_count}")
print(f"Average firing rate: {brain.average_firing_rate:.2f} Hz")
print(f"E/I ratio: {brain.excitation_inhibition_ratio:.2f}")
```

### World Operations

#### Creating a World

```python
world = pynlm.createSimpleWorld()
```

#### Configuring World

```python
world.configure(
    width=30,
    height=30,
    vision_width=8,
    vision_height=8,
    timestep=0.1,
    max_energy=200.0
)
```

#### World Control

```python
world.reset()
world.update(0.1)  # Update by timestep
world.set_agent_start(15.0, 15.0)
```

#### World Interaction

```python
# Apply motor command
action = agent.decode_motor_command()
world.apply_motor_command(action, world.simulation_time)

# Get sensory input
percept = world.get_sensory_percept()

# Get agent body
body = world.get_agent_body()

# Add/remove objects
world.add_object(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
```

### Agent Operations

#### Creating Agent Interface

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Sensory Processing

```python
percept = world.get_sensory_percept()
agent.process_sensory_input(percept)
```

#### Motor Decoding

```python
action = agent.decode_motor_command()
```

#### Neuromodulation

```python
# Apply reward modulation
reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
agent.apply_reward_modulation(reward, 0.0)

# Update development
agent.update_development(0.1)
```

#### Subsystem Control

```python
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)
```

#### Agent Statistics

```python
print(f"Curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Novelty level: {agent.get_novelty_level():.3f}")
print(f"Prediction error: {agent.get_prediction_error():.3f}")
print(f"Neuromodulation level: {agent.get_neuromodulation_level():.3f}")
```

## Memory Systems

### Working Memory

```python
# Check if working memory is available
wm = brain.get_working_memory()
if wm:
    # Store pattern
    wm.store(pattern, priority=1.0)
    
    # Retrieve pattern
    retrieved = wm.retrieve()
    
    # Get size
    print(f"Working memory size: {wm.size()}")
    
    # Clear memory
    wm.clear()
else:
    print("Working memory not available")
```

### Episodic Memory

```python
# Check if episodic memory is available
em = brain.get_episodic_memory()
if em:
    # Store experience
    em.store(experience)
    
    # Retrieve experience
    em.retrieve(time_window)
    
    # Trigger consolidation
    em.consolidate()
    
    # Replay memories
    em.replay(consolidation_strength=1.0)
```

### Semantic Memory

```python
# Check if semantic memory is available
sm = brain.get_semantic_memory()
if sm:
    # Add concept
    sm.add_concept(concept)
    
    # Retrieve concept
    retrieved = sm.retrieve("concept_name")
```

### Procedural Memory

```python
# Check if procedural memory is available
pm = brain.get_procedural_memory()
if pm:
    # Learn procedure
    pm.learn(procedure, outcome)
    
    # Retrieve procedure
    retrieved = pm.retrieve("procedure_name")
```

## Neuromodulation

### Reward Modulation

```python
def run_with_reward():
    # Get reward from world
    reward = world.get_sensory_percept().get_internal()[0]
    if reward is None:
        reward = 0.0
    
    # Apply reward modulation
    agent.apply_reward_modulation(reward, 0.0)
```

### Curiosity System

```python
# Monitor curiosity
curiosity = agent.get_curiosity_level()
print(f"Curiosity level: {curiosity:.3f}")

# Enable/disable curiosity
agent.enable_curiosity(True)
```

### Novelty Detection

```python
# Monitor novelty
novelty = agent.get_novelty_level()
print(f"Novelty level: {novelty:.3f}")

# Enable/disable novelty
agent.enable_curiosity(True)
```

### Prediction Error

```python
# Monitor prediction error
error = agent.get_prediction_error()
print(f"Prediction error: {error:.3f}")
```

## Development

### Development Stages

```python
# Get current developmental stage
stage = brain.developmental_stage
print(f"Developmental stage: {stage:.2f}")

# Update development
brain.update_development(0.1)  # Advance by 0.1 time units
```

### Developmental Configuration

```python
# Configure developmental stages
config.set("development.enable", True)
config.set("development.stage_duration", 100.0)
config.set("development.initial_plasticity", 1.0)
config.set("development.final_plasticity", 0.1)
```

## Configuration

### Creating Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
```

### Modifying Configuration

```python
# Set values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)

# Query values
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")
```

### Configuration Files

```python
# Save configuration
config.save_to_file("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.load_from_file("my_config.json")
```

## Examples and Tutorials

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
print("Basic Brain Simulation")
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.firing_neuron_count()} neurons firing")

print(f"\nFinal statistics:")
print(f"Total neurons: {brain.total_neuron_count}")
print(f"Average firing rate: {brain.average_firing_rate():.2f} Hz")
```

### Example 2: Agent in World

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
world.set_agent_start(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.simulation_time)
```

### Example 3: Complete Learning Simulation

```python
import pynlm

def run_learning_simulation():
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, vision_width=6, vision_height=6)
    world.reset()
    world.set_agent_start(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    for step in range(200):
        world.update(0.1)
        
        # Get reward and process
        percept = world.get_sensory_percept()
        reward = percept.get_internal()[0] if percept.get_internal() else 0.0
        agent.process_sensory_input(percept)
        brain.step(step)
        agent.apply_reward_modulation(reward, 0.0)
        
        # Act
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
        
        # Monitor learning
        if step % 20 == 0:
            print(f"Step {step}: Reward={reward:.2f}, "
                  f"Curiosity={agent.get_curiosity_level():.3f}, "
                  f"Novelty={agent.get_novelty_level():.3f}")

run_learning_simulation()
```

## Troubleshooting

### Common Issues

#### "Brain Not Responding"

**Problem:** Brain simulation seems stuck or not processing.

**Solution:**

```python
# Check if brain was initialized
brain = pynlm.createBrain(config)
brain.initialize()  # Always call initialize!

# Add progress monitoring
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: Brain processing normally")
```

#### "Configuration Not Applied"

**Problem:** Configuration changes not taking effect.

**Solution:**

```python
# Configure before creating brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)  # Use configured brain
```

#### "Python Bindings Not Found"

**Problem:** Cannot import pynlm module.

**Solution:**

```bash
# Install Python bindings
pip install -e .

# Check Python path
python3 -c "import sys; print(sys.path)"
```

#### "Memory Issues"

**Problem:** System runs out of memory.

**Solution:**

```python
# Use smaller brain for testing
config.set("brain.neuron_count", 500)
brain = pynlm.createBrain(config)
```

### Debugging Tips

#### Basic Debugging

```python
# Add print statements for debugging
print(f"Step {step}: Firing neurons: {brain.firing_neuron_count()}")
print(f"Configuration: Neuron count = {config.get('brain.neuron_count')}")

# Check memory systems
if brain.get_working_memory():
    print("Working memory: Available")
else:
    print("Working memory: Not available")
```

#### Advanced Debugging

```python
# Use Python's debugger
import pdb

# Set breakpoints
for step in range(100):
    brain.step(step)
    # Add breakpoints for specific conditions
    if brain.firing_neuron_count() > 100:
        pdb.set_trace()  # Enter debugger
```

## Advanced Topics

### Extending NLM

#### Adding New Configuration Parameters

```python
# In configuration file
# Add new parameters
config.set("custom.new_parameter", 1.0)
config.set("custom.learning_rate", 0.01)
```

#### Creating Custom Worlds

```python
# Subclass SimpleWorld for custom behavior
class CustomWorld(pynlm.SimpleWorld):
    def custom_behavior(self):
        # Implement custom logic
        pass
```

### Performance Optimization

#### Optimizing Configuration

```python
# Cache frequently accessed values
neuron_count = config.get("brain.neuron_count")
synapse_density = config.get("brain.synapse_density")

# Use cached values in loops
for step in range(100):
    brain.step(step)
    # Use neuron_count and synapse_density
```

#### Memory Management

```python
# Reuse brain instances
brain = pynlm.createBrain(config)
brain.initialize()

# Reset instead of recreating
for episode in range(10):
    brain.reset()
    for step in range(50):
        brain.step(step)
```

### Multi-Agent Systems

```python
# Create multiple brains and agents
brains = []
agents = []
worlds = []

for i in range(3):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30)
    world.reset()
    world.set_agent_start(i * 10, i * 10)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    brains.append(brain)
    agents.append(agent)
    worlds.append(world)

# Run multi-agent simulation
for step in range(100):
    for i, world in enumerate(worlds):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brains[i].step(step)
        action = agents[i].decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
```

## References

### Related Documentation

- **Installation Guide**: `/workspace/app/docs/guides/installation.md`
- **Getting Started**: `/workspace/app/docs/guides/getting_started.md`
- **API References**: `/workspace/app/docs/api/`
- **Configuration Guide**: `/workspace/app/docs/configuration/config_reference.md`

### Examples

- **Basic Examples**: `/workspace/app/examples/`
- **Agent Examples**: `/workspace/app/examples/agent_simulation.md`
- **Learning Examples**: `/workspace/app/examples/learning_example.md`

### Code References

- **Source Code**: `/workspace/app/src/`
- **Python Bindings**: `/workspace/app/python/`
- **Tests**: `/workspace/app/tests/`

### External Resources

- **NLM Documentation**: (Link to external documentation)
- **Pybind11 Documentation**: (Link to pybind11 docs)
- **CMake Documentation**: (Link to CMake docs)

## Index

- Introduction: 1-8
- Getting Started: 9-16
- Basic Operations: 17-40
  - Creating Brains: 19-23
  - Brain Operations: 25-33
  - World Operations: 35-43
  - Agent Operations: 45-53
- Memory Systems: 55-67
- Neuromodulation: 69-87
- Development: 89-101
- Configuration: 103-115
- Examples and Tutorials: 117-200
- Troubleshooting: 202-220
- Advanced Topics: 222-280
- References: 282-320

---

*NLM User Guide Version 1.0*
*Phase 6 - Final Integration*
*Last Updated: 2026-09-05*