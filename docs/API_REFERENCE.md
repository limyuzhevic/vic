# NLM Python API Reference

## Overview

This document provides comprehensive reference for the NLM (Neural Learning Machine) Python API bindings. The NLM library provides Python access to a sophisticated spiking neural network simulator with advanced brain-inspired computing capabilities.

**Version:** 0.1.0
**License:** MIT
**Documentation Date:** 2026-10-03

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Classes](#core-classes)
    - [Brain](#brain-class)
    - [Config](#config-class)
    - [AgentBrain](#agentbrain-class)
    - [SimpleWorld](#simpleworld-class)
    - [SimpleEnvironment](#simpleenvironment-class)
3. [Enumerations](#enumerations)
4. [Convenience Methods](#convenience-methods)
5. [Examples](#examples)
6. [Configuration Reference](#configuration-reference)
7. [Troubleshooting](#troubleshooting)

## Quick Start

```python
import pynlm

# Create a simple environment
env = pynlm.SimpleEnvironment()
env.initialize(width=20, height=20)

# Run a simulation
env.runEpisode(numSteps=1000, render=True)

# Get statistics
stats = env.getEpisodeStats()
print(f"Steps: {stats.stepsCompleted}")
print(f"Reward: {stats.totalReward}")
print(f"Firing Rate: {stats.avgFiringRate}")
```

## Core Classes

### Brain Class

The Brain class is the central neural computation engine. It implements real spiking neural dynamics with event-driven computation.

#### Creation and Initialization

```python
import pynlm

# Method 1: Create with default configuration (Recommended)
brain = pynlm.createBrain()
brain.initialize()

# Method 2: Create with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.1)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation Control

```python
# Run simulation steps
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Reset brain for new episode
brain.reset()
```

#### Input/Output Operations

```python
# Inject sensory input
from pynlm import Vision
vision_input = Vision(8, 8, 3)
brain.receiveSensoryInput(vision_input)

# Get motor action
action = brain.produceAction()
```

#### Statistics and Monitoring

```python
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

#### State Persistence

```python
# Save brain state
brain.save("brain_checkpoint.bin")

# Load brain state
brain.load("brain_checkpoint.bin")
```

### Config Class

The Config class manages brain configuration parameters using a key-value store with type safety.

#### Creation and Loading

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Load from file
config.loadFromFile("config.json")

# Load from command line arguments
# config.loadFromArgs(len(sys.argv), sys.argv)

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("neuromodulation.curiosity.enable", True)
```

#### Configuration Access

```python
# Get configuration values
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")
    
if config.has("plasticity.stdp.learning_rate"):
    stdp_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)

# Get all configuration keys
keys = config.getKeys()

# Get configuration summary
print(config.summary())
```

#### Configuration Saving

```python
# Save configuration to file
config.saveToFile("my_config.json")

# Clear configuration
config.clear()
```

### AgentBrain Class

The AgentBrain class connects the NLM brain to the environment, handling sensory transduction and motor decoding.

#### Creation and Initialization

```python
import pynlm

# Create brain first
brain = pynlm.createBrain()
brain.initialize()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
world = pynlm.createSimpleWorld()
world.configure(16, 16, 8, 8)
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensory input from world
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### Motor Decoding

```python
# Decode motor command from brain activity
motor_cmd = agent.decodeMotorCommand()
```

#### Neuromodulation

```python
# Apply reward-based modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Update development
agent.updateDevelopment(0.1)
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")
```

#### Agent Statistics

```python
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

### SimpleWorld Class

The SimpleWorld class simulates a 2D environment where the agent interacts.

#### Creation and Configuration

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### World Control

```python
# Reset world
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Update world simulation
world.update(timestep=0.1)
```

#### Object Management

```python
# Add world object
from pynlm import WorldObject, WorldObjectType
resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0)
world.addObject(resource)

# Check position validity
if world.isValidPosition(3.0, 4.0):
    print("Position is valid")
```

#### State Access

```python
# Get sensory perception
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")
```

### SimpleEnvironment Class

The SimpleEnvironment class provides a high-level interface for running complete simulations with minimal setup code.

#### Basic Usage

```python
import pynlm

# Create and initialize environment
env = pynlm.SimpleEnvironment()
env.initialize(width=16, height=16)

# Run complete episode
env.runEpisode(numSteps=1000, render=True)

# Reset for new episode
env.reset()
```

#### Configuration

```python
# Configure brain parameters
env.configureBrain(neuronCount=3000, regionCount=2, connectionProbability=0.1)

# Enable all subsystems
env.enableAllSubsystems()
```

#### Step-by-Step Simulation

```python
# Run simulation with progress tracking
for step in range(500):
    if not env.step(timestep=0.1):
        break
    
    # Get current statistics
    stats = env.getEpisodeStats()
    if step % 50 == 0:
        print(f"Step {step}: Reward={stats.totalReward:.2f}, " +
              f"Firing={stats.avgFiringRate:.2f}, " +
              f"Curiosity={stats.curiosityLevel:.2f}")
```

#### State and Statistics

```python
# Get comprehensive statistics
stats = env.getEpisodeStats()
print(f"Steps completed: {stats.stepsCompleted}")
print(f"Total reward: {stats.totalReward}")
print(f"Average firing rate: {stats.avgFiringRate}")
print(f"Active neurons: {stats.activeNeurons}")
print(f"Curiosity level: {stats.curiosityLevel}")
print(f"Novelty level: {stats.noveltyLevel}")
print(f"Development stage: {stats.developmentStage}")

# Get environment state description
print(env.getStateDescription())
```

## Enumerations

### NeuronType

```python
pynlm.NeuronType.Excitatory    # Excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons
pynlm.NeuronType.Modulatory     # Neuromodulatory neurons
pynlm.NeuronType.Sensory       # Sensory input neurons
pynlm.NeuronType.Motor         # Motor output neurons
pynlm.NeuronType.Internal      # Internal processing neurons
```

### SynapseType

```python
pynlm.SynapseType.Excitatory   # Excitatory synapses
pynlm.SynapseType.Inhibitory   # Inhibitory synapses
pynlm.SynapseType.Modulatory    # Neuromodulatory synapses
pynlm.SynapseType.Electrical   # Electrical synapses
pynlm.SynapseType.GapJunction  # Gap junction synapses
```

### ActionType

```python
pynlm.ActionType.MoveForward  # Move forward
pynlm.ActionType.MoveBackward # Move backward
pynlm.ActionType.TurnLeft     # Turn left
pynlm.ActionType.TurnRight    # Turn right
pynlm.ActionType.Look         # Look forward
pynlm.ActionType.Interact     # Interact with object
pynlm.ActionType.Eat          # Consume resource
pynlm.ActionType.Drink        # Drink water
pynlm.ActionType.Rest         # Rest/idle
pynlm.ActionType.Wait         # Wait without action
pynlm.ActionType.Custom       # Custom action
```

### MotorCommand

```python
pynlm.MotorCommand.MoveForward # Move forward
pynlm.MotorCommand.MoveBackward # Move backward
pynlm.MotorCommand.TurnLeft     # Turn left
pynlm.MotorCommand.TurnRight    # Turn right
pynlm.MotorCommand.LookLeft     # Look left
pynlm.MotorCommand.LookRight    # Look right
pynlm.MotorCommand.Interact     # Interact
pynlm.MotorCommand.Wait         # Wait
```

### WorldObjectType

```python
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Consumable resource
pynlm.WorldObjectType.Hazard    # Dangerous object
pynlm.WorldObjectType.Wall      # Solid barrier
pynlm.WorldObjectType.Marker    # Special marker
```

### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial     # Initial development
pynlm.DevelopmentalStage.CriticalPeriod # Critical learning period
pynlm.DevelopmentalStage.Maturation  # Maturation phase
pynlm.DevelopmentalStage.Adult      # Adult stage
pynlm.DevelopmentalStage.Aging      # Aging phase
```

## Convenience Methods

### Brain Factory Methods

```python
# Create brain with default configuration (Recommended)
brain = pynlm.createBrain()
brain.initialize()

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
brain = pynlm.createBrain(config)
```

### SimpleEnvironment Factory Methods

```python
# Create and initialize environment in one line
env = pynlm.SimpleEnvironment()
env.initialize(16, 16, 8, 8)

# Run complete simulation
env.runEpisode(1000)
```

### Common Patterns

```python
# Pattern 1: Simple simulation loop
def run_simple_simulation(steps=1000):
    env = pynlm.SimpleEnvironment()
    env.initialize(10, 10)
    env.runEpisode(steps)
    return env.getEpisodeStats()

# Pattern 2: Agent-based interaction
def run_agent_simulation(steps=500):
    brain = pynlm.createBrain()
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    agent.initialize(world)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    return brain.getTotalSpikeCount()
```

## Configuration Reference

### Brain Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Developmental Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_plasticity` | float | 1.0 | Initial plasticity factor |
| `development.critical_period_length` | float | 300.0 | Critical period duration |
| `development.maturation_length` | float | 900.0 | Maturation period duration |

## Examples

### Example 1: Silent Brain (Neurons Only)

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run silent simulation (no sensory input)
for i in range(100):
    brain.step(i)

print("Silent brain test completed!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
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
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate()}")
```

### Example 3: Complete Agent

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
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Example 4: Environment-based Simulation

```python
import pynlm

# Create environment (high-level interface)
env = pynlm.SimpleEnvironment()
env.initialize(width=12, height=12, visionWidth=6, visionHeight=6)

# Configure brain parameters
env.configureBrain(neuronCount=2000, connectionProbability=0.08)

# Enable all subsystems
env.enableAllSubsystems()

# Run episode with progress tracking
env.runEpisode(numSteps=500, render=True)

# Get final statistics
stats = env.getEpisodeStats()
print(f"Simulation completed: {stats.stepsCompleted} steps")
print(f"Total reward: {stats.totalReward:.2f}")
print(f"Average firing rate: {stats.avgFiringRate:.2f}")
print(f"Final curiosity level: {stats.curiosityLevel:.2f}")
```

## Troubleshooting

### "My brain isn't doing anything"

**Problem:** Brain appears inactive after initialization.

**Causes:**
- Did you call `brain.initialize()`?
- Are there enough neurons for activity?
- Is there sufficient sensory input?

**Solution:**
```python
# Ensure proper initialization
brain = pynlm.createBrain()
brain.initialize()

# Add sensory input to activate neurons
from pynlm import Vision
vision = Vision(8, 8, 3)
vision.setData([0.5] * 192)  # Add some activity
brain.receiveSensoryInput(vision)

# Run more steps to allow propagation
for i in range(100):
    brain.step(i)
```

### "The agent isn't moving"

**Problem:** Agent motor commands aren't producing movement.

**Causes:**
- Did you call `world.applyMotorCommand()`?
- Is `world.update()` being called?
- Are motor neurons receiving sufficient input?

**Solution:**
```python
# Ensure complete simulation loop
for step in range(100):
    # Update world (necessary for time advancement)
    world.update(0.1)
    
    # Process sensory input
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Run brain computation
    brain.step(step)
    
    # Get and apply motor command
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### "Everything is 0"

**Problem:** All activity values are zero.

**Causes:**
- Brains need time to "warm up"
- Neurons need input to fire
- Some configurations require specific setup

**Solution:**
```python
# Try with more steps and initialization
brain = pynlm.createBrain()
brain.initialize()

# Run warm-up period
for i in range(50):
    brain.step(i)

# Add initial activity
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 1.0)

# Continue simulation
for i in range(200):
    brain.step(i)
    
print(f"Final firing rate: {brain.getAverageFiringRate()}")
```

### "Performance is slow"

**Problem:** Simulation running slowly.

**Causes:**
- Too many neurons/synapses
- Inefficient configuration
- Missing performance optimizations

**Solution:**
```python
# Use optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Start smaller
config.set("brain.synapse_density", 0.05)
brain = pynlm.createBrain(config)
brain.initialize()

# Use SimpleEnvironment for optimized setup
env = pynlm.SimpleEnvironment()
env.initialize(8, 8)
env.runEpisode(200)  # Faster execution
```

## Error Handling

The NLM Python API includes comprehensive error handling:

```python
import pynlm

try:
    # Create brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
        
except Exception as e:
    print(f"Error: {e}")
    # Handle error appropriately
```

Common exceptions:
- `RuntimeError`: General runtime errors
- `ValueError`: Invalid parameter values
- `TypeError`: Type mismatches
- `AttributeError`: Missing attributes/methods

## Best Practices

### 1. Use SimpleEnvironment for Common Use Cases

```python
# Good practice
env = pynlm.SimpleEnvironment()
env.initialize(10, 10)
env.runEpisode(500)
```

### 2. Configure Brain Parameters Before Initialization

```python
# Good practice
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. Check Initialization Status

```python
# Good practice
if not env.isInitialized():
    print("Error: Environment not initialized")
    return
```

### 4. Use Appropriate Step Sizes

```python
# Good practice
for step in range(1000):
    # Smaller timestep for accuracy
    world.update(0.01)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### 5. Monitor Performance

```python
# Good practice
import time
start_time = time.time()

# Run simulation
env.runEpisode(1000)

end_time = time.time()
print(f"Simulation took {end_time - start_time:.2f} seconds")
```

## Future Enhancements

The NLM Python API will continue to evolve with:

1. **Advanced Visualization Tools**: Built-in plotting and graph visualization
2. **Interactive Jupyter Notebooks**: Live code examples and demonstrations
3. **Performance Profiling**: Built-in performance monitoring and optimization suggestions
4. **Experiment Comparison**: Tools for comparing different experimental results
5. **Custom Environment Builders**: More flexible environment creation APIs
6. **Real-time Visualization**: Live monitoring of brain activity during simulation

## Support and Community

For questions, issues, or feature requests:
- Check the official documentation
- Visit the NLM project website
- Submit issues on GitHub
- Join the community forum

---

*This API reference is continuously updated. For the latest version, visit the official NLM documentation website.*

## Licensing

MIT License - See LICENSE file for details.