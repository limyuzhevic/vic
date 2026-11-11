# NLM Python API Comprehensive Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API. NLM is an experimental artificial developmental brain simulator that implements real spiking neural computation with event-driven dynamics, synaptic plasticity, memory systems, and neuromodulation.

## Quick Start

### Installation

```bash
# Install the NLM package
pip install pynlm

# For development
pip install -e .
```

### Basic Usage Example

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Quick simulation
for i in range(1000):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core API Components

### 1. Config Class

The `Config` class manages brain configuration parameters.

#### Creation and Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("random_seed", 123)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `plasticity.stdp.enable` | bool | true | Enable STDP |

#### Configuration File I/O

```python
# Load from file
config.loadFromFile("my_config.cfg")

# Save to file
config.saveToFile("config.json")

# Check existence
if config.has("brain.neuron_count"):
    count = config.getOr("brain.neuron_count", 1000)
```

### 2. Brain Class

The `Brain` class represents the central neural simulation system with millions of spiking neurons.

#### Basic Usage

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### Advanced Usage

```python
# Context manager for automatic initialization/reset
import pynlm

config = pynlm.createDefaultConfig()
with pynlm.BrainContext(config) as brain:
    for step in range(500):
        brain.step(step)
        # Brain automatically initializes on enter, reset on exit
        
    print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")

# Batch current injection
currents = [
    (pynlm.INVALID_NEURON_ID, 30.0),
    (pynlm.INVALID_NEURON_ID, 15. afs()),
]
brain.batchInjectCurrents(currents)

# Quick simulation helper
pynlm.quickBrainSimulation(brain, 1000)

# Comprehensive statistics
stats = brain.getBrainStats()
print(f"E/I ratio: {stats['e_i_ratio']:.2f}")
print(f"Total synapses: {stats['total_synapses']}")
```

#### Brain Methods

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the brain with configuration |
| `step(step, time=None)` | Perform one simulation step |
| `reset()` | Reset brain state to initial condition |
| `injectCurrent(neuron_id, current)` | Inject current into specific neuron |
| `injectCurrentToNeurons(type, current)` | Inject current to all neurons of type |
| `receiveSensoryInput(input)` | Process sensory input |
| `produceAction()` | Generate motor action from brain activity |
| `save(filepath)` | Save brain state to checkpoint file |
| `load(filepath)` | Load brain state from checkpoint file |
| `addRegion(name)` | Add a new neural region |
| `getRegion(region_id)` | Get region by ID |
| `getTotalNeuronCount()` | Get total neuron count |
| `getTotalSynapseCount()` | Get total synapse count |
| `getFiringNeuronCount()` | Get count of firing neurons |
| `getTotalSpikeCount()` | Get total spike count |
| `getAverageFiringRate()` | Get average firing rate |
| `getExcitationInhibitionRatio()` | Get E/I balance ratio |
| `getDevelopmentalStage()` | Get current developmental stage |
| `setDevelopmentalStage(stage)` | Set developmental stage |
| `logStatus()` | Log detailed brain status |

### 3. AgentBrain Class

The `AgentBrain` class connects brain activity to world interaction through sensory processing and motor output.

#### Basic Usage

```python
import pynlm

# Setup complete agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### Agent Methods

| Method | Description |
|--------|-------------|
| `initialize(world)` | Initialize agent with world interface |
| `processSensoryInput(percept)` | Process sensory input from world |
| `decodeMotorCommand()` | Convert brain activity to action |
| `applyRewardModulation(reward, predicted_reward)` | Learn from rewards |
| `updateDevelopment(timestep)` | Update developmental stage |
| `getCuriosityLevel()` | Get curiosity-driven exploration level |
| `getNoveltyLevel()` | Get novelty detection level |
| `getPredictionError()` | Get prediction error signal |
| `getNeuromodulationLevel()` | Get overall neuromodulation level |

#### Complete Agent Example

```python
def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
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
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
```

### 4. SimpleWorld Class

The `SimpleWorld` class represents the simulated environment with sensory input and motor output.

#### Basic Usage

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
time_step = 0.1
world.update(time_step)

# Get sensory input (what the agent sees)
percept = world.getSensoryPercept()
vision = percept.getVision()

# Get agent body state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
```

#### World Methods

| Method | Description |
|--------|-------------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `reset()` | Reset world to initial state |
| `setAgentStart(x, y)` | Set agent starting position |
| `update(timestep)` | Update world dynamics |
| `applyMotorCommand(cmd, time)` | Apply motor action to world |
| `getSensoryPercept()` | Get sensory perception data |
| `getAgentBody()` | Get agent physical state |
| `addObject(obj)` | Add object to world |
| `removeObject(x, y)` | Remove object at position |
| `isValidPosition(x, y)` | Check if position is valid |
| `getWidth()`, `getHeight()` | Get world dimensions |
| `getSimulationTime()` | Get current simulation time |

### 5. SensoryInput Classes

#### Vision

```python
# Create vision sensory input
vision = pynlm.Vision(width=8, height=8, channels=3)

# Set vision data (8x8 grayscale image)
data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8] * 8
vision.setData(data)

# Get dimensions
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

#### Audio

```python
# Create audio sensory input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set audio data
audio_data = [0.0] * 1024  # Silence
audio.setData(audio_data)
audio.setSampleRate(44100)

# Get properties
sample_rate = audio.getSampleRate()
num_samples = audio.getNumSamples()
```

#### InternalSignals

```python
# Create internal signals (e.g., hunger, energy, etc.)
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.8)  # hunger level
internal.addSignal(0.3)  # energy level
internal.addSignal(0.1)  # stress level

# Get all signals
all_signals = internal.getAllSignals()
```

### 6. Action Classes

#### Action

```python
# Create action
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Set with parameters
action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])

# Get properties
type = action.getType()
params = action.getParameters()
name = action.getName()

# Clone action
action2 = action.clone()
```

#### MotorCommand

```python
# Motor command enums
motor_cmd = pynlm.MotorCommand.MoveForward
motor_cmd = pynlm.MotorCommand.TurnLeft
motor_cmd = pynlm.MotorCommand.Interact
motor_cmd = pynlm.MotorCommand.Wait
```

## Advanced Features

### 1. Context Managers

The NLM Python API provides context managers for automatic resource management:

```python
import pynlm

# Brain context manager - auto-initializes and resets
config = pynlm.createDefaultConfig()
with pynlm.BrainContext(config) as brain:
    for step in range(500):
        brain.step(step)
    print("Simulation completed automatically!")

# World context manager
with pynlm.WorldContext() as world:
    world.configure(10, 10, 8, 8)
    world.reset()
    # Automatic cleanup on exit
```

### 2. Batch Operations

Efficient batch operations for performance-critical applications:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Batch current injection - more efficient than individual calls
currents = [
    (pynlm.INVALID_NEURON_ID, 50.0),
    (pynlm.INVALID_NEURON_ID, 30.0),
    (pynlm.INVALID_NEURON_ID, 20.0),
]
brain.batchInjectCurrents(currents)

# Batch configuration from file
config = pynlm.createBrainFromConfigFile("my_config.cfg")

# Quick simulation helper
pynlm.quickBrainSimulation(brain, 1000)
```

### 3. Utility Functions

High-level utility functions for common tasks:

```python
import pynlm

# Create brain with common settings
def createQuickBrain(neuron_count=1000):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("random_seed", 42)
    return pynlm.createBrain(config)

# Comprehensive statistics
def analyzeBrain(brain):
    stats = brain.getBrainStats()
    print(f"Brain Analysis:")
    print(f"  Total Neurons: {stats['total_neurons']}")
    print(f"  Total Synapses: {stats['total_synapses']}")
    print(f"  Firing Neurons: {stats['firing_neurons']}")
    print(f"  Total Spikes: {stats['total_spikes']}")
    print(f"  E/I Ratio: {stats['e_i_ratio']:.2f}")

# Simulation runner
def runSimulation(brain, world, agent, steps):
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## Configuration Files

### Example Config File

```
# NLM configuration file
brain.neuron_count = 2000
brain.synapse_density = 0.15
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.tau_mem = 20.0

random_seed = 42
simulation_timestep = 0.001

# Plasticity settings
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true

# Neuromodulation
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
```

## Example Scripts

### Minimal Working Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Silent Brain Example

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Brain Watching a World Example

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth= Yale, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Branch the world for observation
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Complete Agent Example

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
    agent.processSensoryInput(world.getSensoryPerlect())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 ==  Yeni:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Performance Tips

### 1. For Large Simulations

```python
# Use context managers for automatic resource management
with pynlm.BrainContext(config) as brain:
    # Run large simulation
    for step in range(100000):
        brain.step(step)
```

### 2. For Real-time Applications

```python
# Use batch operations for efficiency
injections = [(neuron_id, current) for neuron_id, current in ...]
brain.batchInjectCurrents(injections)

# Use quick simulation for repeated runs
pynlm.quickBrainSimulation(brain, 1000)
```

### 3. For Memory Efficiency

```python
# Periodically save checkpoints to disk
if step % 1000 == 0:
    brain.save(f"checkpoint_step_{step}.bin")

# Free memory periodically
if step % 5000 == 0:
    brain.reset()  # Clears working memory
```

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
```python
# Ensure initialization is called
brain.initialize()

# Try increasing simulation steps
for i in range(1000):
    brain.step(i)
```

**"The agent isn't moving"
```python
# Ensure world.update() is called
world.update(0.1)

# Check that motor commands are being generated
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

**"Everything is 0"
```python
# Brains need time to "warm up" - try more steps
for i in range(1000):
    brain.step(i)

# Some neurons need input to fire
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
```

## Frequently Asked Questions

### Q: How do I save and load brain states?
```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state later
brain.load("my_brain.bin")
```

### Q: How do I configure the brain?
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

brain = pynlm.createBrain(config)
```

### Q: How do I enable learning?
```python
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

### Q: How do I get detailed brain statistics?
```python
stats = brain.getBrainStats()
print(f"E/I Ratio: {stats['e_i_ratio']:.2f}")
print(f"Total Spikes: {stats['total_spikes']}")
```

## Further Reading

- **ARCHITECTURE.md**: System architecture overview
- **SCIENCE.md**: Scientific background
- **EXPERIMENTS.md**: Experiment descriptions
- **HOW_TO_USE.md**: Detailed usage instructions

## Version Information

This documentation covers NLM Phase 6 (Final Integration) with complete Python API bindings.

---

*Last updated: 2026-10-11*
*NLM Version 0.1.0*