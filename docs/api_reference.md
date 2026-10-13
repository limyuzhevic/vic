# NLM Python API Reference

This document provides detailed reference for the NLM Python API, including all
classes, functions, and their usage.

## Configuration System

### Config Class

The `Config` class provides flexible configuration management for NLM systems,
supporting multiple input formats (JSON, YAML, simple key=value files) and
command-line arguments.

#### Initialization

```python
from nlm import Config

# Create default configuration
config = Config()
```

#### Loading Configuration

```python
# Load from JSON file
config.loadFromFile("config.json")

# Load from command line arguments
config.loadFromArgs(len(sys.argv), sys.argv)

# Save to file
config.saveToFile("output.json")
```

#### Setting Configuration Values

```python
# Basic types
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("plasticity_rate", 0.01)
config.set("enable_checkpointing", True)

# Configuration source management
config.set("debug_mode", True, ConfigSource.Runtime)  # Different from file/command line
```

#### Getting Configuration Values

```python
# Get values with type checking
neuron_count = config.get("neuron_count").value
if config.has("custom_setting"):
    value = config.get("custom_setting").value

# Get with defaults
max_steps = config.getOr("max_steps", 1000)
log_level = config.getOr("log_level", "INFO")
```

#### Configuration Management Methods

- **`loadFromFile(filepath)`**: Load configuration from JSON/YAML/CFG file
- **`loadFromArgs(argc, argv)`**: Parse command line arguments
- **`saveToFile(filepath)`**: Export configuration to file
- **`has(key)`**: Check if configuration key exists
- **`getKeys()`**: Get all configuration keys
- **`clear()`**: Remove all configuration entries
- **`summary()`**: Get formatted configuration summary

### Factory Functions

```python
# Create default configuration
config = nlm.createDefaultConfig()

# Create brain with configuration
brain = nlm.createBrain(config)

# Create simulation world
world = nlm.createSimpleWorld()

# Create agent interface
agent_brain = nlm.createAgentBrain(brain)
```

## Neural System

### Brain Class

The `Brain` class implements the core neural computation system, integrating
all brain modules into a coherent cognitive architecture.

#### Initialization

```python
from nlm import Brain, Config

config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)

brain = Brain(config)
brain.initialize()
```

#### Core Simulation Loop

```python
# Main simulation loop
for step in range(10000):
    # Receive sensory input
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    
    # Execute neural computation
    brain.step(step)
    
    # Generate motor output
    action = brain.produceAction()
    
    # Apply action in world
    result = world.applyMotorCommand(action, step)
    
    # Apply neuromodulation based on reward
    agent_brain.applyRewardModulation(result.reward, 0.0)
    
    # Update development
    agent_brain.updateDevelopment(world.getSimulationTime())
```

#### Brain Methods

- **`step(currentStep)`**: Perform one simulation step with implicit time
- **`step(currentStep, currentTime)`**: Perform simulation step with explicit time
- **`receiveSensoryInput(input)`**: Inject sensory data into neural system
- **`injectCurrent(neuron, current)`**: Direct current injection into specific neuron
- **`injectCurrentToNeurons(type, current)`**: Inject current into all neurons of type
- **`produceAction()`**: Generate motor action from neural activity
- **`reset()`**: Reset brain to initial state
- **`save(filepath)`**: Save brain state to file (checkpointing)
- **`load(filepath)`**: Load brain state from file
- **`addRegion(name)`**: Add new neural region
- **`getRegion(id)`**: Retrieve specific neural region
- **`getTotalNeuronCount()`**: Get total number of neurons
- **`getTotalSynapseCount()`**: Get total number of synapses
- **`getActiveNeuronCount()`**: Get count of currently active neurons
- **`getFiringNeuronCount()`**: Get count of firing neurons
- **`getAverageFiringRate()`**: Get average firing rate across all neurons
- **`getDevelopmentalStage()`**: Get current developmental stage
- **`setDevelopmentalStage(stage)`**: Set developmental stage

### Neuron and Synapse Types

#### NeuronType Enum

```python
from nlm import NeuronType

# Neuron types
excitatory = NeuronType.Excitatory      # Standard excitatory neuron
inhibitory = NeuronType.Inhibitory     # Standard inhibitory neuron
modulatory = NeuronType.Modulatory      # Neuromodulatory neuron

sensory = NeuronType.Sensory            # Sensory processing neuron
motor = NeuronType.Motor               # Motor control neuron
internal = NeuronType.Internal         # Internal state neuron
```

#### SynapseType Enum

```python
from nlm import SynapseType

# Synapse types
excitatory = SynapseType.Excitatory    # Standard chemical excitatory synapse
electrical = SynapseType.Electrical     # Direct electrical coupling

inhibitory = SynapseType.Inhibitory   # Chemical inhibitory synapse
gap_junction = SynapseType.GapJunction  # Electrical gap junction

modulatory = SynapseType.Modulatory    # Neuromodulatory synapse
```

## Agent Interface

### AgentBrain Class

The `AgentBrain` class provides the interface between the NLM brain and the
external world, handling sensory transduction and motor decoding.

#### Initialization

```python
from nlm import AgentBrain, Brain

brain = Brain(config)
brain.initialize()

agent_brain = AgentBrain(brain)
agent_brain.initialize(world)
```

#### Sensory Processing

```python
# Process sensory percept from world
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)
```

#### Motor Decoding

```python
# Convert neural activity to motor command
motor_command = agent_brain.decodeMotorCommand()

# Get agent state
position = (agent_brain.getBrain().getAgentBody().x,
           agent_brain.getBrain().getAgentBody().y)
orientation = agent_brain.getBrain().getAgentBody().orientation
```

#### Neuromodulation and Development

```python
# Apply reward-based learning
agent_brain.applyRewardModulation(reward, predicted_reward)

# Update developmental state
agent_brain.updateDevelopment(world.getSimulationTime())

# Get agent state information
dopamine_level = agent_brain.getNeuromodulationLevel()
curiosity_level = agent_brain.getCuriosityLevel()
novelty_level = agent_brain.getNoveltyLevel()
prediction_error = agent_brain.getPredictionError()
developmental_stage = agent_brain.getDevelopmentalStage()
```

#### Configuration Options

```python
# Enable/disable subsystems
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Check system status
if agent_brain.isRewardModulationEnabled():
    print("Reward modulation is active")
```

## Environment Interface

### SimpleWorld Class

The `SimpleWorld` class provides a 2D simulation environment for NLM agents.

#### Configuration

```python
from nlm import SimpleWorld

world = SimpleWorld()
world.configure(width=20, height=20, vision_width=16, vision_height=16)
```

#### Agent Management

```python
# Set agent starting position
world.setAgentStart(x=10.0, y=5.0)

# Get agent body state
body = world.getAgentBody()
position = (body.x, body.y)
orientation = body.orientation
velocity = (body.velocityX, body.velocityY)
```

#### World Simulation

```python
# Main simulation loop
for step in range(1000):
    # Get current sensory percept
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    
    # Decode motor command
    command = agent_brain.decodeMotorCommand()
    
    # Apply action in world
    result = world.applyMotorCommand(command, step)
    
    # Get reward signal
    reward = result.reward
    success = result.success
    message = result.message
    
    # Update with world state
    world.update(step * 0.001)
```

#### World Objects

```python
from nlm import WorldObject, WorldObjectType

# Create world object
resource = WorldObject(x=5.0, y=5.0, type=WorldObjectType.Resource, value=10.0)
wall = WorldObject(x=15.0, y=15.0, type=WorldObjectType.Wall, radius=1.0)

# Add to world
world.addObject(resource)

# Check position validity
is_valid = world.isValidPosition(10.0, 10.0)

# Remove object
world.removeObject(5.0, 5.0)
```

#### World Configuration

```python
# Energy and decay settings
world.setMaxEnergy(2.0)
world.setEnergyDecayRate(0.1)

# Random seed for reproducibility
world.setRandomSeed(42)

# Simulation time tracking
time = world.getSimulationTime()
```

## Sensory Input

### SensoryInput Base Class

The `SensoryInput` class is the base class for all sensory modalities.

#### Basic Properties

```python
from nlm import SensoryInput

# Generic sensory input
sensory = SensoryInput()
sensory.setTimestamp(0.0)

# Get properties
type = sensory.getType()
data = sensory.getData()
dimensions = sensory.getDimensions()
timestamp = sensory.getTimestamp()
```

### Vision Class

```python
from nlm import Vision

# Create vision with specified resolution
vision = Vision(width=16, height=16, channels=3)

# Set vision data
vision_data = [0.1, 0.2, 0.3, 0.4]  # Example 2x2 grayscale image
vision.setData(vision_data)

# Get properties
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

### Audio Class

```python
from nlm import Audio

# Create audio with specified parameters
audio = Audio(sampleRate=44100, numSamples=1024)

# Set audio data
audio_data = [0.5, 0.7, 0.3, 0.9]
audio.setData(audio_data)

# Configure audio
audio.setSampleRate(48000)
sample_rate = audio.getSampleRate()
num_samples = audio.getNumSamples()
```

### InternalSignals Class

```python
from nlm import InternalSignals

internal = InternalSignals()

# Add internal signals
internal.addSignal(0.5)
internal.addSignal(-0.3)

# Clear all signals
internal.clearSignals()
```

## Motor Output

### Action Class

The `Action` class represents motor commands and their parameters.

#### Initialization

```python
from nlm import Action, ActionType

# Create basic action
action = Action(ActionType.MoveForward)

# Create action with parameters
action = Action(ActionType.Custom, [0.1, 0.2, 0.3, 0.4])
```

#### Action Properties

```python
# Action type
action_type = action.getType()
action.setType(ActionType.Interact)

# Action parameters
parameters = action.getParameters()
action.setParameters([0.0, 1.0, 0.5])

# Action metadata
name = action.getName()
```

#### Cloning

```python
# Clone action
action_copy = action.clone()
```

## World Objects

### WorldObject Class

Represents physical objects in the simulation world.

#### Creation and Properties

```python
from nml import WorldObject, WorldObjectType

# Create resource object
resource = WorldObject(x=10.0, y=10.0, type=WorldObjectType.Resource)

# Set properties
resource.x = 15.0
resource.y = 15.0
resource.radius = 0.5
resource.type = WorldObjectType.Wall
resource.value = 100.0
resource.active = True
```

## Agent Body

### AgentBody Class

Represents the physical state of an agent in the simulation world.

#### Properties

```python
from nlm import AgentBody

body = AgentBody()

# Position and orientation
body.x = 10.0
body.y = 5.0
body.orientation = 90.0  # degrees

# Movement
body.velocityX = 0.5
body.velocityY = 0.0
body.angularVelocity = 0.1

# Physical properties
body.energy = 1.0
body.health = 1.0
body.age = 0.0

# State flags
body.isMoving = True
body.isTurning = False
body.lastActionTime = 0.0
```

#### Operations

```python
# Reset agent to initial state
body.reset()
```

## Sensory Percept

### SensoryPercept Class

Represents a complete sensory experience from the world.

#### Structure

```python
from nlm import SensoryPercept

percept = SensoryPercept()

# Vision component
vision_data = percept.getVision()
vision_width = percept.getVisionWidth()
vision_height = percept.getVisionHeight()
percept.setVision(vision_data)

# Touch component
touch_data = percept.getTouch()
percept.setTouch(touch_data)

# Internal component
internal_data = percept.getInternal()
percept.setInternal(internal_data)

# Proprioception component
proprioception_data = percept.getProprioception()
percept.setProprioception(proprioception_data)

# Audio component
audio_data = percept.getAudio()
percept.setAudio(audio_data)

# Timestamp
percept.setTimestamp(time)
```

#### Access Methods

```python
# Get all signals at once
all_signals = percept.getAllSignals()

# Get individual components
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
proprioception = percept.getProprioception()
audio = percept.getAudio()

# Timestamp
timestamp = percept.getTimestamp()
```

## Action Results

### ActionResult Class

Represents the outcome of an action in the world.

#### Structure

```python
from nlm import ActionResult

result = ActionResult()

# Reward signal
reward = result.reward  # Positive for good outcomes, negative for bad

# Success flag
success = result.success  # True if action succeeded

# Descriptive message
message = result.message  # Human-readable description of outcome
```

## Identifiers

### NeuronId, SynapseId, RegionId, PopulationId

Each class represents a unique identifier with value, index, and comparison methods:

```python
from nlm import NeuronId, SynapseId, RegionId, PopulationId

# Create identifiers
neuron_id = NeuronId(42)
synapse_id = SynapseId(100)
region_id = RegionId(1)
population_id = PopulationId(0)

# Access value
print(neuron_id.value)  # 42

# Get index
print(neuron_id.index())  # Returns index representation

# Comparison
id1 = NeuronId(1)
id2 = NeuronId(1)
assert id1 == id2  # True
assert id1 != neuron_id  # True

# Hashing for use in dictionaries
ids = {neuron_id: "some data"}
```

## Developmental Stages

### DevelopmentalStage Enum

Represents the developmental phase of the brain:

```python
from nlm import DevelopmentalStage

# Developmental stages
initial = DevelopmentalStage.Initial
# High plasticity, early learning phase

critical_period = DevelopmentalStage.CriticalPeriod
# Enhanced plasticity for skill acquisition

maturation = DevelopmentalStage.Maturation
# Structural development and specialization

adult = DevelopmentalStage.Adult
# Stable adult cognitive capabilities

aging = DevelopmentalStage.Aging
# Gradual decline in plasticity and function
```

## Motor Commands

### MotorCommand Enum

Low-level motor commands for agent control:

```python
from nlm import MotorCommand

# Basic movement commands
move_forward = MotorCommand.MoveForward
move_backward = MotorCommand.MoveBackward
turn_left = MotorCommand.TurnLeft
turn_right = MotorCommand.TurnRight

# Specialized commands
look_left = MotorCommand.LookLeft
look_right = MotorCommand.LookRight
interact = MotorCommand.Interact
wait = MotorCommand.Wait
```

## World Object Types

### WorldObjectType Enum

Types of objects that can exist in the world:

```python
from nlm import WorldObjectType

# Object types
empty = WorldObjectType.Empty      # No object
resource = WorldObjectType.Resource # Valuable item
hazard = WorldObjectType.Hazard    # Dangerous object
wall = WorldObjectType.Wall        # Impassable barrier
marker = WorldObjectType.Marker    # Location marker
```

## Error Handling

NLM provides exception classes for error handling:

```python
import nlm

try:
    config = nlm.Config()
    config.loadFromFile("missing_file.json")
except Exception as e:
    print(f"Error: {e}")
```

## Performance Considerations

### Memory Management

- Objects are reference-counted to minimize memory overhead
- Large arrays (vision, audio) use efficient data structures
- Automatic cleanup prevents memory leaks

### Performance Tips

1. **Reuse objects**: Create and reuse sensory inputs, actions, and results
2. **Batch operations**: Process multiple time steps when possible
3. **Configure appropriately**: Use smaller simulations for debugging
4. **Monitor performance**: Track simulation time and memory usage

## Example: Complete Simulation

```python
#!/usr/bin/env python3
"""
Complete NLM simulation example demonstrating all major components.
"""

import sys
import time
import nlm

def main():
    print("=== NLM Complete Simulation Example ===\n")
    
    # 1. Configuration
    print("1. Setting up configuration...")
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("simulation_timestep", 0.001)
    config.set("log_level", "INFO")
    
    # 2. Create brain and agent
    print("2. Creating brain and agent interface...")
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, vision_width=16, vision_height=16)
    world.setAgentStart(x=10.0, y=10.0)
    
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # 3. Simulation loop
    print("3. Running simulation...")
    start_time = time.time()
    
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent brain
        agent_brain.processSensoryInput(percept)
        
        # Generate action
        command = agent_brain.decodeMotorCommand()
        
        # Apply action in world
        result = world.applyMotorCommand(command, step)
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent_brain.updateDevelopment(world.getSimulationTime())
        
        # Get brain status periodically
        if step % 20 == 0:
            brain.logStatus()
    
    end_time = time.time()
    
    # 4. Results
    print("\n4. Simulation Results:")
    print(f"   Duration: {end_time - start_time:.2f} seconds")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Developmental stage: {agent_brain.getDevelopmentalStage()}")
    print(f"   Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
    print(f"   Curiosity level: {agent_brain.getCuriosityLevel():.3f}")
    
    print("\n=== Simulation Complete ===")

if __name__ == "__main__":
    main()
```

## Further Reading

For more advanced usage and research applications, see:

- **Phase 6 Integration**: Demonstrates full system integration
- **Phase 3 Neural Computation**: Focuses on basic neural dynamics
- **Phase 4 Cognitive Architecture**: Explores higher cognitive functions
- **Phase 5 Performance Optimizations**: Advanced performance techniques

## API Changes

See CHANGELOG.md for detailed information about API changes between versions.

## Support

For questions, issues, or collaboration:

- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: https://github.com/nlm-project/nlm/issues

---

*This API reference is automatically generated from the source code.
For the most up-to-date information, see the source code and inline documentation.*
