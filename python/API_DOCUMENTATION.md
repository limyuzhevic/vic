# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides bindings to the C++ neural simulation framework, allowing users to create and interact with artificial brains in Python. This documentation provides comprehensive examples and explanations for getting started with NLM.

## Installation

The NLM Python library is installed as `pynlm` and includes all the core NLM functionality. After installation, you can use it directly in your Python scripts:

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

## Core Classes

### 1. Config

The `Config` class manages brain configuration parameters, allowing you to customize neuron count, connectivity, and various system behaviors.

**Key Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Parse command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if a configuration key exists
- `getKeys()` - Get all configuration keys

**Common Configuration Keys:**
- `brain.neuron_count` - Number of neurons (default: 1000)
- `brain.synapse_density` - Connection probability (default: 0.1)
- `plasticity.stdp.enable` - Enable STDP plasticity (default: true)
- `neuromod.dopamine.scale` - Dopamine modulation scale (default: 1.0)

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()

# Customize configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("neuromod.dopamine.scale", 1.5)

# Save to file
config.saveToFile("my_brain_config.json")
```

### 2. Brain

The `Brain` class is the central neural simulation component, containing neurons, synapses, and all brain systems.

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(step_number)` - Run one simulation step
- `receiveSensoryInput(input)` - Inject sensory data
- `produceAction()` - Get motor action from brain
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `getTotalNeuronCount()` - Get total neuron count
- `getFiringNeuronCount()` - Get currently firing neurons

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simulation
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

### 3. AgentBrain

The `AgentBrain` class bridges the NLM brain with a simulated world, handling sensory input and motor output.

**Key Methods:**
- `initialize(world)` - Initialize with a world
- `processSensoryInput(percept)` - Process sensory data
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted)` - Apply reward signals
- `updateDevelopment(timestep)` - Update development system

**Example:**
```python
import pynlm

# Create all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
```

### 4. SimpleWorld

The `SimpleWorld` class simulates a 2D environment where agents interact with objects and resources.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Set world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world simulation
- `applyMotorCommand(cmd, currentTime)` - Apply action to world
- `getSensoryPercept()` - Get agent's sensory perception
- `getAgentBody()` - Get agent body state

**Example:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)  # Center of world

# Add resources to the world
agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
world.addObject(pynlm.WorldObject(agent_x, agent_y, pynlm.WorldObjectType.Resource, value=10.0))

# Run world updates
for step in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    # Process percept and decide action
    # ...
```

## Complete Example: Agent in a Resource-Rich Environment

This example demonstrates creating a complete agent that learns to find and consume resources:

```python
import pynlm

def create_and_run_agent(world_width=20, world_height=20, num_steps=1000):
    """Create and run a learning agent in a resource-rich environment."""
    
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_width, height=world_height, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(world_width/2, world_height/2)
    
    # 4. Add resources throughout the world
    for x in range(5, world_width-5, 6):
        for y in range(5, world_height-5, 6):
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=5.0))
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable advanced learning features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore novel situations
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return brain, agent, world

# Run the complete example
if __name__ == "__main__":
    print("Starting NLM agent simulation...")
    brain, agent, world = create_and_run_agent(num_steps=1000)
    print("\nSimulation complete!")
    print(f"Final statistics:")
    print(f"  Total neurons created: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses created: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final development stage: {brain.getDevelopmentalStage()}")
```

## Common Patterns

### Pattern 1: Silent Brain Simulation

```python
import pynlm

def silent_brain_simulation(num_steps=1000):
    """Run brain simulation without world interaction."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
    
    print(f"Silent brain test complete. Neurons: {brain.getTotalNeuronCount()}, "
          f"Spikes: {brain.getTotalSpikeCount()}, "
          f"Firing: {brain.getFiringNeuronCount()}")
    return brain
```

### Pattern 2: Brain Watching a World

```python
import pynlm

def watching_agent_simulation(num_steps=300):
    """Agent observes world but doesn't take actions."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
    
    print(f"Watched world for {num_steps} steps")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    return brain, world
```

### Pattern 3: Complete Learning Agent

```python
import pynlm

def learning_agent_simulation(num_steps=1000):
    """Full learning agent with all subsystems enabled."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add some resources
    for i in range(10):
        world.addObject(pynlm.WorldObject(
            (i * 3) % 15, (i * 3) % 15,
            pynlm.WorldObjectType.Resource,
            value=3.0
        ))
    
    # Run simulation with detailed logging
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    return brain, agent, world
```

## Sensory Input Types

### Vision
```python
import pynlm

# Create vision sensory input
vision = pynlm.Vision(8, 8, 3)  # 8x8 pixels, 3 color channels

# Set data (flattened array)
data = [0.1] * 192  # 8*8*3 = 192 elements
vision.setData(data)

print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}, "
      f"{vision.getChannels()} channels")
```

### Audio
```python
import pynlm

# Create audio sensory input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set audio data
audio_data = [0.5] * 1024
audio.setData(audio_data)

print(f"Audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
```

### Internal Signals
```python
import pynlm

# Create internal signals
internal = pynlm.InternalSignals()

# Add various internal signals
internal.addSignal(0.8)  # High arousal
internal.addSignal(-0.3)  # Low motivation
internal.addSignal(0.1)  # Slightly hungry

print(f"Internal signals count: {len(internal.getAllSignals())}")
```

## Action Types

The NLM supports many action types for agent behavior:

```python
import pynlm

# Common action types
actions = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.MoveBackward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.TurnRight,
    pynlm.ActionType.Interact,
    pynlm.ActionType.Eat,
    pynlm.ActionType.Drink,
    pynlm.ActionType.Look,
    pynlm.ActionType.Rest,
    pynlm.ActionType.Wait
]

# Create and use actions
for action_type in actions:
    action = pynlm.Action(action_type)
    print(f"Action: {action.getName()} (type: {action_type})")
```

## Configuration Examples

### Custom Configuration JSON

```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.2,
            "baseline": 0.1
        },
        "curiosity": {
            "enable": true,
            "threshold": 0.3
        }
    },
    "world": {
        "environment_name": "GridWorld",
        "width": 20,
        "height": 20,
        "max_energy": 100.0,
        "energy_decay_rate": 0.1
    }
}
```

### Loading from Command Line

```python
import pynlm
import sys

# Simulate command line arguments
config = pynlm.createDefaultConfig()
config.loadFromArgs([
    "--brain.neuron_count", "3000",
    "--brain.synapse_density", "0.2",
    "-log_level", "DEBUG"
])

print(f"Loaded config with {config.getOr('brain.neuron_count', 1000)} neurons")
```

## Performance Monitoring

The NLM provides comprehensive monitoring of brain state:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few steps
for i in range(100):
    brain.step(i)

# Log detailed status
brain.logStatus()
```