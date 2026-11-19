# NLM Python API Reference

## Overview

This document provides comprehensive API reference for the NLM Python bindings, covering all available classes, functions, and their usage patterns.

## Module Overview

The main module is `pynlm` which provides access to the NLM neural simulation framework.

```python
import pynlm
```

## Core Classes

### Brain

The main class representing a neural brain simulation.

#### Creation

```python
# Create brain with default configuration
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
```

#### Initialization

```python
# Initialize brain (must be called before use)
brain.initialize()

# Initialize with custom timestep
config = pynlm.createDefaultConfig()
config.set("simulation_timestep", 0.01)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation

```python
# Run a single simulation step
brain.step(0)                    # With step number
brain.step(0, 0.1)               # With step number and time
brain.step(0, time=0.1)          # With keyword arguments

# Run multiple steps
for step in range(1000):
    brain.step(step)

# Reset brain
brain.reset()
brain.initialize()  # Initialize again

# Save and load brain state
brain.save("checkpoint.bin")
# ... later ...
brain.load("checkpoint.bin")
```

#### Sensory Processing

```python
# Inject sensory input
import pynlm

# Create vision input (8x8 with 3 channels)
vision = pynlm.Vision(8, atoshi8, 3)
# Set pixel data
vision.setData([0.1, 0.2, ...])  # flattened array
brain.receiveSensoryInput(vision)
```

#### Motor Production

```python
# Get motor action from brain
action = brain.produceAction()
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")
```

#### Statistics and Properties

```python
# Brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Region management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
print(f"Regions count: {brain.getRegionCount()}")
```

#### Configuration

```python
# Get brain configuration
config = brain.getConfig()
print(f"Config keys: {config.getKeys()}")
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")
```

### Config

Configuration management class for NLM system parameters.

#### Creation

```python
import pynlm
config = pynlm.createDefaultConfig()
```

#### Configuration Operations

```python
# Check if key exists
if config.has("brain.neuron_count"):
    print("Neuron count is configured")

# Get all keys
keys = config.getKeys()
for key in keys:
    print(f"{key}: {config.get(key)}")

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("simulation_timestep", 0.001)

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("config.json")

# Clear configuration
config.clear()

# Get configuration summary
print(config.summary())
```

### AgentBrain

Bridge between NLM brain and world environment.

#### Creation

```python
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
```

#### Initialization

```python
import pynlm

# Create brain, world, and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Sensory Processing

```python
# Get sensory percept from world
percept = world.getSensoryPercept()

# Process sensory input
agent.processSensoryInput(percept)

# Access different sensory modalities
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
```

#### Motor Decoding

```python
# Decode brain activity into motor command
motor_cmd = agent.decodeMotorCommand()
print(f"Action type: {motor_cmd.getType()}")
print(f"Action parameters: {motor_cmd.getParameters()}")
```

#### Neuromodulation

```python
# Apply reward modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Get neuromodulation levels
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")
```

#### Development

```python
# Update development system
agent.updateDevelopment(timestep=0.1)
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
```

### SimpleWorld

Simple 2D world for NLM simulation.

#### Creation

```python
import pynlm
world = pynlm.createSimpleWorld()
```

#### Configuration

```python
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

#### Control

```python
# Reset world to initial state
world.reset()

# Update world simulation
world.update(timestep=0.1)

# Set agent starting position
world.setAgentStart(x=10.0, y=10.0)
```

#### Agent Interaction

```python
# Apply motor command from agent
world.applyMotorCommand(motor_cmd, current_time)

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent orientation: {body.orientation}")
```

#### World Objects

```python
# Add world objects
resource = pynlm.WorldObject(x=5.0, y=5.0, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(resource)

# Remove objects
world.removeObject(x=5.0, y=5.0)

# Check valid positions
if world.isValidPosition(x=10.0, y=10.0):
    print("Position is valid")
```

#### Properties

```python
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Simulation time: {world.getSimulationTime()}")
```

### SensoryInput Classes

#### Vision

```python
import pynlm

# Create vision input
vision = pynlm.Vision(width=8, height=8, channels=3)

# Set/get data
data = [0.1, 0.2, 0.3, 0.4, ...]  # flattened
vision.setData(data)
print(f"Vision size: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
```

#### Audio

```python
import pynlm

# Create audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
audio.setSampleRate(44100)
samples = [0.1, 0.2, ...]  # audio samples
audio.setData(samples)
```

#### InternalSignals

```python
import pynlm

# Create internal signals
signals = pynlm.InternalSignals()
signals.addSignal(0.5)  # Reward signal
signals.addSignal(-0.1) # Prediction error
signals.addSignal(1.0)  # Curiosity signal
```

### Action Classes

#### Action

```python
import pynlm

# Create action from type
action = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Action name: {action.getName()}")

# Create action with parameters
action = pynlm.Action(
    pynlm.ActionType.Custom,
    parameters=[0.5, 1.2, 0.8]  # Custom parameters
)

# Clone action
cloned = action.clone()
```

#### WorldObject

```python
import pynlm

# Create world object
obj = pynlm.WorldObject(
    x=5.0,
    y=5.0,
    pynlm.WorldObjectType.Resource,
    value=1.0,
    radius=0.5
)

# Modify properties
obj.x = 10.0
obj.y = 10.0
obj.type = pynlm.WorldObjectType.Hazard
obj.value = 2.0
obj.active = True
```

### AgentBody

```python
import pynlm

body = pynlm.AgentBody()
body.x = 10.0
body.y = 10.0
body.orientation = 0.0
body.velocityX = 0.0
body.velocityY = 0.0
body.energy = 100.0
body.health = 1.0
body.age = 0.0
body.isMoving = False
body.isTurning = False
body.lastActionTime = 0.0

print(f"Position: ({body.x}, {body.y})")
print(f"Energy: {body.energy}")
print(f"Health: {body.health}")
```

### Enumerations

#### NeuronType

```python
import pynlm

print(pynlm.NeuronType.Excitatory)
print(pynlm.NeuronType.Inhibitory)
print(pynlm.NeuronType.Sensory)
print(pynlm.NeuronType.Motor)
print(pynlm.NeuronType.Modulatory)
print(pynlm.NeuronType.Internal)
```

#### SynapseType

```python
import pynlm

print(pynlm.SynapseType.Excitatory)
print(pynlm.SynapseType.Inhibitory)
print(pynlm.SynapseType.Modulatory)
print(pynlm.SynapseType.Electrical)
print(pynlm.SynapseType.GapJunction)
```

#### DevelopmentalStage

```python
import pynlm

print(pynlm.DevelopmentalStage.Initial)
print(pynlm.DevelopmentalStage.CriticalPeriod)
print(pynlm.DevelopmentalStage.Maturation)
print(pynlm.DevelopmentalStage.Adult)
print(pynlm.DevelopmentalStage.Aging)
```

#### ActionType

```python
import pynlm

print(pynlm.ActionType.MoveForward)
print(pynlm.ActionType.MoveBackward)
print(pynlm.ActionType.TurnLeft)
print(pynlm.ActionType.TurnRight)
print(pynlm.ActionType.Interact)
print(pynlm.ActionType.Eat)
print(pynlm.ActionType.Drink)
print(pynlm.ActionType.Rest)
print(pynlm.ActionType.Wait)
print(pynlm.ActionType.Custom)
```

#### WorldObjectType

```python
import pynlm

print(pynlm.WorldObjectType.Empty)
print(pynlm.WorldObjectType.Resource)
print(pynlm.WorldObjectType.Hazard)
print(pynlm.WorldObjectType.Wall)
print(pynlm.WorldObjectType.Marker)
```

#### MotorCommand

```python
import pynlm

print(pynlm.MotorCommand.MoveForward)
print(pynlm.MotorCommand.MoveBackward)
print(pynlm.MotorCommand.TurnLeft)
print(pynlm.MotorCommand.TurnRight)
print(pynlm.MotorCommand.LookLeft)
print(pynlm.MotorCommand.LookRight)
print(pynlm.MotorCommand.Interact)
print(pynlm.MotorCommand.Wait)
```

## Module Attributes

```python
import pynlm

# Invalid identifiers for error handling
pynlm.INVALID_NEURON_ID
pynlm.INVALID_SYNAPSE_ID
pynlm.INVALID_REGION_ID
pynlm.INVALID_POPULATION_ID

# Version information
print(pynlm.__version__)
```

## Configuration Parameters

### Brain Configuration

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

## Advanced Usage Patterns

### 1. Agent-Based Simulation

```python
import pynlm
import time

class NLMAgent:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Initialize brain
        self.brain.initialize()
    
    def run_episode(self, steps):
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
        
        return self.brain.getTotalSpikeCount()

# Usage
agent = NLMAgent()
spikes = agent.run_episode(1000)
print(f"Episode complete. Total spikes: {spikes}")
```

### 2. Configuration Management

```python
import pynlm
import json

class BrainConfig:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def save_to_file(self, filename):
        self.config.saveToFile(filename)
    
    def load_from_file(self, filename):
        self.config.loadFromFile(filename)
    
    def set_custom_config(self, neuron_count=1000, learning_rate=0.001):
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("plasticity.stdp.learning_rate", learning_rate)
        self.config.set("neuromod.dopamine.scale", 1.0)
    
    def get_summary(self):
        return self.config.summary()

# Usage
config = BrainConfig()
config.set_custom_config(neuron_count=2000, learning_rate=0.01)
print(config.get_summary())
config.save_to_file("custom_config.json")
```

### 3. Batch Processing

```python
import pynlm
import time

def run_multiple_simulations(num_simulations=10, steps_per_sim=100):
    results = []
    
    for sim in range(num_simulations):
        # Create fresh brain for each simulation
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for step in range(steps_per_sim):
            brain.step(step)
        end_time = time.time()
        
        # Collect results
        result = {
            'simulation': sim,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'duration': end_time - start_time
        }
        results.append(result)
    
    return results

# Usage
results = run_multiple_simulations(5, 500)
for result in results:
    print(f"Sim {result['simulation']}: {result['total_spikes']} spikes, "
          f"{result['duration']:.2f}s")
```

### 4. Error Handling and Validation

```python
import pynlm

def safe_simulation_run(config, steps=1000):
    """Safely run a simulation with error handling."""
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
            
            # Check for errors
            if brain.getTotalSpikeCount() > 1000000:
                print("Warning: High spike count detected")
        
        return True, brain
        
    except Exception as e:
        print(f"Simulation error: {e}")
        return False, None

# Usage
config = pynlm.createDefaultConfig()
success, brain = safe_simulation_run(config)
if success:
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

## Migration Guide

### From Old to New API

#### Old API Pattern

```python
# Old way (requires more setup)
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
```

#### New API Pattern

```python
# New simplified way
import pynlm

# For beginners - simple one-liner
brain = pynlm.simpleBrain()  # New simplified API
brain.start()  # Simplified initialization

# Or traditional way
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

### Configuration Migration

#### Old Config File (default.cfg)

```ini
# Old format (has duplicates)
[brain]
neuron_count = 1000
plasticity_learning_rate = 0.01

[neuromodulation]
dopamine_baseline = 0.1
dopamine_baseline = 0.1  # Duplicate - remove this line
```

#### New Config Format (JSON)

```json
{
  "brain": {
    "neuron_count": 1000,
    "plasticity_learning_rate": intra0.001
  },
  "neuromodulation": {
    "dopamine_baseline": 0.1
  }
}
```

## Performance Tips

### Optimizing Brain Configuration

```python
# Performance-optimized configuration
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)      # Smaller for faster testing
config.set("brain.synapse_density", 0.05)  # Lower density
config.set("simulation_timestep", 0.001)   # Smaller timestep

brain = pynlm.createBrain(config)
brain.initialize()
```

### Memory Management

```python
# Reusing objects instead of creating new ones
import pynlm

# Create once, reuse multiple times
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleCity()

for episode in range(10):
    brain = pynlm.createBrain(config)  # New brain each episode
    agent = pynlm.createAgentBrain(brain)
    # ... run episode
```

### Parallel Processing

```python
import pynlm
import threading

def run_in_thread(config, steps):
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(steps):
        brain.step(step)
    return brain

# Run multiple simulations in parallel
simulations = []
for i indnl range(5):
    config = pynlm.createDefaultConfig()
    thread = threading.Thread(target=run_in_thread, args=(config, 1000))
    simulations.append(thread)
    thread.start()
```

## Troubleshooting

### Common Issues

#### Import Errors

```bash
# Python module not found
pip install -e .  # Reinstall Python bindings
pip install --force-reinstall .  # Force reinstall
```

#### Runtime Errors

```python
# Segmentation fault - usually means initialize() not called
brain.initialize()  # Always call this before use

# Memory issues - check for memory leaks
brain.reset()  # Reset state periodically
```

#### Configuration Errors

```python
# Configuration file issues
config = pynlm.createDefaultConfig()
if not config.has("brain.neuron_count"):
    config.set("brain.neuron_count", 1000)  # Set default
```

### Debugging Techniques

```python
# Add debug prints
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 50 == áže:
        print(f"Step {step}: neurons firing {brain.getFiringNeuronCount()}")
```

## Further Reading

- [NLM Documentation](https://docs.nualm.org) - Comprehensive documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture details
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and theory
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [Phase Documentation](README.md#phase-summary) - Phase-by-phase implementation details

## Conclusion

The NLM Python API provides a comprehensive interface for neural simulation with options for both simple use and advanced customization. Whether you're creating a simple brain simulation or implementing complex agent behaviors, the API offers the tools you need.

For questions not covered here, refer to the development documentation or reach out through the project's issue tracker.
