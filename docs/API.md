# NLM Python API Documentation

This document provides comprehensive documentation for the NLM Python API,
covering basic usage, advanced features, and best practices.

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start](#quick-start)
3. [Core Classes](#core-classes)
4. [Advanced Features](#advanced-features)
5. [API Reference](#api-reference)
6. [Examples](#examples)
7. [Troubleshooting](#troubleshooting)
8. [Performance Considerations](#performance-considerations)

## Introduction

NLM (Neural Learning Machine) is a computational brain simulation framework
that provides Python bindings for creating artificial neural systems with
self-organizing capabilities.

The Python API (`pynlm`) exposes the full power of the NLM framework through
a high-level, Pythonic interface while maintaining access to all underlying
C++ performance and functionality.

## Quick Start

### Installation

Install NLM using pip:

```bash
pip install pynlm
```

### Basic Example

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

## Core Classes

### Config

The `Config` class manages configuration parameters for the NLM system.

#### Methods

- `config.loadFromFile(filepath)` - Load configuration from JSON file
- `config.loadFromArgs(argc, argv)` - Load from command line arguments
- `config.saveToFile(filepath)` - Save to JSON file
- `config.has(key)` - Check if key exists
- `config.get(key)` - Get value
- `config.getOr(key, default)` - Get with default value
- `config.set(key, value)` - Set value
- `config.getKeys()` - Get all keys
- `config.clear()` - Clear all entries
- `config.summary()` - Get configuration summary

#### Example

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
print(f"Neuron count: {config.get('brain.neuron_count')}")
print(f"Synapse density: {config.get('brain.synapse_density')}")
```

### Brain

The `Brain` class represents the central neural simulation system.

#### Methods

- `brain.initialize()` - Initialize the brain
- `brain.step(current_step)` - Take a simulation step
- `brain.step(current_step, current_time)` - Take step with timestamp
- `brain.reset()` - Reset brain state
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.addRegion(name)` - Add a neural region
- `brain.getRegion(id)` - Get region by ID
- `brain.getRegions()` - Get all regions
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSynapseCount()` - Get total synapse count
- `brain.getActiveNeuronCount()` - Get active neuron count
- `brain.getFiringNeuronCount()` - Get firing neuron count
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getDevelopmentalStage()` - Get developmental stage
- `brain.setDevelopmentalStage(stage)` - Set developmental stage

#### Example

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

### SimpleWorld

The `SimpleWorld` class represents the simulation environment.

#### Methods

- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.reset()` - Reset world
- `world.setAgentStart(x, y)` - Set agent starting position
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(cmd, currentTime)` - Apply action
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentBody()` - Get agent body state
- `world.addObject(obj)` - Add world object
- `world.removeObject(x, y)` - Remove object
- `world.isValidPosition(x, y)` - Check position validity
- `world.getWidth()` - Get world width
- `world.getHeight()` - Get world height

#### Example

```python
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(15.0, 15.0)

# Run world simulation
for i in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    # Process percept...
```

### AgentBrain

The `AgentBrain` class connects the brain to the world for sensory-motor processing.

#### Methods

- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.applyRewardModulation(reward, predicted)` - Apply reward modulation
- `agent.updateDevelopment(timestep)` - Update development
- `agent.reset()` - Reset agent
- `agent.getBrain()` - Get underlying brain
- `agent.enableRewardModulation(enable)` - Enable reward modulation
- `agent.enableStructuralPlasticity(enable)` - Enable structural plasticity
- `agent.enableDevelopment(enable)` - Enable development
- `agent.enableCuriosity(enable)` - Enable curiosity
- `agent.getNeuromodulationLevel()` - Get neuromodulation level
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error

#### Example

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent loop
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Advanced Features

### Complete Agent with Learning

```python
import pynlm
import numpy as np

def create_training_agent(brain, world, enable_learning=True):
    """Create a complete training agent."""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return agent

def run_episode(brain, world, agent, num_steps, with_reward=True):
    """Run a complete episode."""
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if with_reward and step % 10 == 0:
            agent.applyRewardModulation(0.5, 0.3)
    
    return brain.getAverageFiringRate()

# Create and train agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = create_training_agent(brain, world, enable_learning=True)

# Run training episodes
for episode in range(10):
    firing_rate = run_episode(brain, world, agent, 50)
    print(f"Episode {episode}: {firing_rate:.3f} firing rate")
    
    # Reset for next episode
    brain.reset()
    world.reset()
    agent.reset()
```

### Memory Systems

```python
# Access memory systems
brain = pynlm.createBrain(config)
brain.initialize()

working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

print(f"Working memory capacity: {working_memory.getCapacity() if working_memory else 'N/A'}")
```

### Development Stages

```python
# Test different developmental stages
brain = pynlm.createBrain(config)
brain.initialize()

stages = [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
    pynlm.DevelopmentalStage.Aging
]

for stage in stages:
    brain.setDevelopmentalStage(stage)
    print(f"Stage {stage}: {brain.getDevelopmentalStage()}")
```

### Visualization and Analysis

```python
# Visualize neural network
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Generate network visualization
brain.visualizeNetwork("brain_network.png")

# Analyze statistics
stats = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'firing_rate': brain.getAverageFiringRate(),
    'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
    'development_stage': brain.getDevelopmentalStage()
}

print("Brain statistics:", stats)
```

## API Reference

### Enumerations

#### NeuronType
- `NeuronType.Excitatory` - Excitatory neuron
- `NeuronType.Inhibitory` - Inhibitory neuron
- `NeuronType.Modulatory` - Modulatory neuron
- `NeuronType.Sensory` - Sensory neuron
- `NeuronType.Motor` - Motor neuron
- `NeuronType.Internal` - Internal neuron

#### SynapseType
- `SynapseType.Excitatory` - Excitatory synapse
- `SynapseType.Inhibitory` - Inhibitory synapse
- `SynapseType.Modulatory` - Modulatory synapse
- `SynapseType.Electrical` - Electrical synapse
- `SynapseType.GapJunction` - Gap junction

#### DevelopmentalStage
- `DevelopmentalStage.Initial` - Initial development stage
- `DevelopmentalStage.CriticalPeriod` - Critical period
- `DevelopmentalStage.Maturation` - Maturation stage
- `DevelopmentalStage.Adult` - Adult stage
- `DevelopmentalStage.Aging` - Aging stage

#### ActionType
- `ActionType.MoveForward` - Move forward
- `ActionType.MoveBackward` - Move backward
- `ActionType.MoveLeft` - Move left
- `ActionType.MoveRight` - Move right
- `ActionType.TurnLeft` - Turn left
- `ActionType.TurnRight` - Turn right
- `ActionType.Look` - Look
- `ActionType.Interact` - Interact with object
- `ActionType.Eat` - Eat object

#### MotorCommand
- `MotorCommand.MoveForward` - Move forward
- `MotorCommand.MoveBackward` - Move backward
- `MotorCommand.TurnLeft` - Turn left
- `MotorCommand.TurnRight` - Turn right
- `MotorCommand.LookLeft` - Look left
- `MotorCommand.LookRight` - Look right
- `MotorCommand.Interact` - Interact
- `MotorCommand.Wait` - Wait

#### WorldObjectType
- `WorldObjectType.Empty` - Empty space
- `WorldObjectType.Resource` - Resource object
- `WorldObjectType.Hazard` - Hazard object
- `WorldObjectType.Wall` - Wall
- `WorldObjectType.Marker` - Marker object

#### FiringState
- `FiringState.Resting` - Resting state
- `FiringState.Active` - Active state
- `FiringState.Refractory` - Refractory state
- `FiringState.Inhibited` - Inhibited state

### Data Classes

#### Action
- `Action(type, parameters)` - Create action
- `action.getType()` - Get action type
- `action.setType(type)` - Set action type
- `action.getParameters()` - Get parameters
- `action.setParameters(params)` - Set parameters
- `action.getName()` - Get action name
- `action.clone()` - Clone action

#### WorldObject
- `WorldObject(x, y, type, value, radius)` - Create world object
- `object.x` - X position
- `object.y` - Y position
- `object.type` - Object type
- `object.value` - Object value
- `object.radius` - Object radius
- `object.active` - Object active status

#### AgentBody
- `AgentBody()` - Create agent body
- `body.x` - X position
- `body.y` - Y position
- `body.orientation` - Orientation
- `body.velocityX` - X velocity
- `body.velocityY` - Y velocity
- `body.angularVelocity` - Angular velocity
- `body.energy` - Energy
- `body.health` - Health
- `body.age` - Age
- `body.isMoving` - Moving state
- `body.isTurning` - Turning state

#### ActionResult
- `ActionResult(reward, success, message)` - Create action result
- `result.reward` - Reward
- `result.success` - Success flag
- `result.message` - Message

#### SensoryPercept
- `SensoryPercept()` - Create sensory percept
- `percept.getVision()` - Get vision
- `percept.getTouch()` - Get touch
- `percept.getInternal()` - Get internal signals
- `percept.getProprioception()` - Get proprioception
- `percept.getAudio()` - Get audio
- `percept.getAllSignals()` - Get all signals

### Factory Functions

- `pynlm.createDefaultConfig()` - Create default configuration
- `pynlm.createBrain(config)` - Create brain
- `pynlm.createSimpleWorld()` - Create simple world
- `pynlm.createAgentBrain(brain)` - Create agent brain
- `pynlm.createAction(type, parameters)` - Create action
- `pynlm.createWorldObject(x, y, type, value, radius)` - Create world object

## Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

### Example 2: Complete Agent with Learning

```python
import pynlm

def train_agent(episodes=10):
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Training loop
    for episode in range(episodes):
        print(f"Episode {episode + 1}/{episodes}")
        
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:
                agent.applyRewardModulation(1.0, 0.5)
        
        # Print statistics
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Neuormodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Reset for next episode
        brain.reset()
        world.reset()
        agent.reset()
    
    return brain, agent

# Run training
trained_brain, trained_agent = train_agent(5)
```

### Example 3: Advanced Agent with Multiple Systems

```python
import pynlm
import numpy as np

def create_advanced_agent(config_overrides=None):
    """Create an advanced agent with multiple systems."""
    config = pynlm.createDefaultConfig()
    
    # Apply configuration overrides
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Create brain and enable advanced features
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Create agent with advanced features
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, world, agent

def run_advanced_simulation(brain, world, agent, num_steps=100):
    """Run advanced simulation with detailed logging."""
    simulation_data = {
        'steps': [],
        'firing_rates': [],
        'rewards': [],
        'curiosity_levels': [],
        'actions_taken': 0
    }
    
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.5 if action != pynlm.MotorCommand.Wait else 0.1
        predicted = 0.3
        agent.applyRewardModulation(reward, predicted)
        
        # Log data
        simulation_data['steps'].append(step)
        simulation_data['firing_rates'].append(brain.getAverageFiringRate())
        simulation_data['rewards'].append(reward)
        simulation_data['curiosity_levels'].append(agent.getCuriosityLevel())
        
        if action != pynlm.MotorCommand.Wait:
            simulation_data['actions_taken'] += 1
    
    return simulation_data

# Create and run advanced simulation
brain, world, agent = create_advanced_agent({
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15
})

data = run_advanced_simulation(brain, world, agent, 200)

print(f"Simulation completed with {data['actions_taken']} actions taken")
print(f"Average firing rate: {np.mean(data['firing_rates']):.3f}")
print(f"Average curiosity: {np.mean(data['curiosity_levels']):.3f}")
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Import Error: No module named 'pynlm'

**Solution:** Ensure the NLM Python bindings are installed:

```bash
pip install --force-reinstall .
```

Or build from source:

```bash
mkdir build
cd build
cmake ..
make -j4
```

#### 2. Segmentation Fault

**Causes:**
- Invalid configuration parameters
- Memory corruption
- Invalid state transitions

**Solutions:**
- Check configuration parameters
- Validate inputs before use
- Use proper error handling
- Report bugs to developers

#### 3. Agent Not Moving

**Causes:**
- Motor commands not being applied
- World not being updated
- Agent not receiving sensory input

**Solutions:**
- Verify world update is called
- Check that motor commands are applied
- Ensure agent has sensory input processing

#### 4. Brain Not Firing

**Causes:**
- No neurons configured
- No synaptic connections
- No input currents

**Solutions:**
- Check neuron count configuration
- Verify synaptic connections
- Ensure sensory input is injected

### Debug Tips

#### Enable Debug Output

```python
import pynlm

# Create brain with debug configuration
config = pynlm.createDefaultConfig()
config.set("debug.level", "verbose")

brain = pynlm.createBrain(config)
brain.initialize()
brain.logStatus()  # Log brain status
```

#### Check Configuration

```python
config = pynlm.createDefaultConfig()
print("Available keys:", config.getKeys())
print("Brain neuron count:", config.getOr("brain.neuron_count", "not set"))
```

#### Validate State

```python
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

## Performance Considerations

### Optimization Tips

1. **Brain Configuration**
   - Adjust neuron and synapse counts based on requirements
   - Use appropriate region counts for complex behaviors
   - Configure synaptic density appropriately

2. **Simulation Parameters**
   - Use appropriate time steps for stability
   - Consider batch processing for multiple steps
   - Monitor computational complexity

3. **Memory Management**
   - Regularly call `brain.reset()` for long-running simulations
   - Monitor memory usage with large brain configurations
   - Consider checkpointing for long training sessions

### Performance Monitoring

```python
import time
import pynlm

# Performance benchmark
def benchmark_brain_performance(brain, steps=100):
    start_time = time.time()
    
    for i in range(steps):
        brain.step(i)
    
    end_time = time.time()
    
    total_time = end_time - start_time
    avg_step_time = total_time / steps
    
    print(f"Total time for {steps} steps: {total_time:.3f}s")
    print(f"Average step time: {avg_step_time:.6f}s")
    print(f"Steps per second: {steps/total_time:.1f}")
    
    return avg_step_time

# Run benchmark
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

benchmark_time = benchmark_brain_performance(brain, 1000)
```

### Memory Usage

```python
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Monitor during simulation
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

## Best Practices

### 1. Configuration Management

- Use configuration files for complex setups
- Validate configuration parameters before use
- Use environment variables for runtime configuration

### 2. Simulation Design

- Design simulation loops carefully
- Use checkpointing for long-running simulations
- Implement proper cleanup and resource management

### 3. Testing and Validation

- Write comprehensive tests for your simulation logic
- Use the provided test suite for the Python bindings
- Monitor simulation statistics regularly

### 4. Error Handling

- Implement proper error handling
- Validate inputs and states
- Use meaningful error messages

### 5. Performance Optimization

- Profile simulation performance
- Optimize configuration for your use case
- Consider parallelization for large simulations

## Conclusion

The NLM Python API provides a powerful, Pythonic interface to the NLM
neural simulation framework. This documentation covers the core
functionality and provides examples for getting started with NLM.

For more advanced usage and custom integrations, refer to the examples
and API reference documentation. The NLM framework is actively developed
and new features are continuously added to support advanced neural
simulation research and applications.
