# NLM Python API Documentation

# NLM (熙然) - Neural Learning Machine Python Bindings

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project implemented in Python with C++ backend. This documentation provides comprehensive coverage of the Python API for NLM, including all classes, methods, parameters, and usage examples.

### What is NLM?

NLM is a **brain simulator** for computers that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

### What is This Documentation?

This is the complete Python API documentation for NLM's Python bindings. It covers:

1. **Core Classes**: Brain, Config, Action, WorldObject, etc.
2. **Sensory Systems**: Vision, Audio, InternalSignals
3. **Agent Interface**: AgentBrain for world interaction
4. **Factory Methods**: Convenient creation helpers
5. **Error Handling**: Comprehensive validation and error recovery
6. **Performance Features**: Benchmarking, profiling, and optimization
7. **Advanced Features**: Power user tools and utilities

### How to Use This Documentation

- **New Users**: Start with the "Quick Start" section and "Easy Usage" guide
- **Intermediate Users**: Read through the core classes and factory methods
- **Advanced Users**: Explore error handling, performance features, and advanced utilities
- **API Reference**: Use the "Class Reference" sections for detailed method documentation

### API Structure

The NLM Python API is organized into logical modules:

1. **Type Definitions**: NeuronId, SynapseId, RegionId, PopulationId
2. **Enumerations**: NeuronType, SynapseType, DevelopmentalStage, ActionType
3. **Configuration**: Config class for all simulation parameters
4. **Neural Components**: Brain class with all neural systems
5. **Sensory Systems**: Vision, Audio, InternalSignals for perception
6. **Action Systems**: Action class for motor output
7. **Environment**: SimpleWorld for simulation world
8. **Agent Interface**: AgentBrain for brain-world interaction

## Quick Start

### Installation

The NLM Python bindings are built as part of the main NLM project. To install:

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build and install Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
make install
```

Or for development (editable install):

```bash
pip install -e .
```

### Basic Usage

Here's a simple example to get started with NLM:

```python
import pynlm

# Step 1: Create a configuration
config = pynlm.createDefaultConfigWithDefaults()

# Step 2: Create a brain
brain = pynlm.createBrain(config)

# Step 3: Initialize the brain
brain.initialize()

# Step 4: Make the brain think for a few steps
for i in range(10):
    brain.step(i)

# Step 5: Check results
print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Example: Agent in a World

This example shows a complete agent simulation with world interaction:

```python
import pynlm
import numpy as np

# Setup configuration
config = pynlm.createWorldConfig(width=20, height=20, visionWidth=8, visionHeight=8)
brain = pynlm.createBrain(config)
brain.initialize()

# Setup world
world = pynlm.createWorld(20, 20, 8, 8)
world.reset()
world.setAgentStart(10, 10)

# Setup agent
agent = pynlm.createAgent(brain, world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)  # Update world
    
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)   # Brain sees it
    
    brain.step(step)                      # Brain thinks
    
    action = agent.decodeMotorCommand()   # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Optional: Add some rewards
    if step % 20 == 0:
        print(f"Step {step}: Firing neurons = {brain.getFiringNeuronCount()}")

print("Simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## Core Classes

### Config Class

The Config class manages all simulation parameters including neural network topology, simulation parameters, learning rules, and system behavior.

#### Creation Methods

```python
# Basic configuration
config = pynlm.createDefaultConfigWithDefaults()

# Configuration with sensible defaults
config = pynlm.createDefaultConfigWithDefaults()

# Simple configuration with custom parameters
config = pynlm.createSimpleConfig(neuronCount=100, connectionProb=0.2)

# Minimal configuration (only essential parameters)
config = pynlm.createMinimalConfig()

# World-optimized configuration
config = pynlm.createWorldConfig(width=15, height=15, visionWidth=8, visionHeight=8)

# Create from Python dictionary
config_dict = {"neuron_count": 250, "connection_probability": 0.12}
config = pynlm.createConfigFromDict(config_dict)
```

#### Property Access

```python
# Get configuration values
neuron_count = config.getInt("neuron_count", 500)
connection_prob = config.getFloat("connection_probability", 0.15)
learning_rate = config.getFloat("learning_rate", 0.01)
enabled = config.getBool("stdp_enabled", True)
name = config.getString("brain_name", "MyBrain")

# Item access syntax (like dictionary)
value = config["neuron_count"]
config["new_param"] = "value"

# Check if key exists
if "neuron_count" in config:
    print("Configuration has neuron_count")

# Iterate over all keys
for key in config:
    print(f"{key}: {config[key]}")
```

#### Setting Values

```python
# Set individual values
config.setInt("neuron_count", 1000)
config.setFloat("learning_rate", 0.05)
config.setBool("stdp_enabled", True)
config.setString("brain_name", "MyAgent")

# Automatic type conversion
config.set("neuron_count", 500)
config.set("learning_rate", 0.01)
config.set("stdp_enabled", False)
config.set("name", "TestBrain")

# Remove keys
config.remove("temp_param")
```

#### File Operations

```python
# Save configuration to file
success = config.saveToFile("my_config.json")

# Load configuration from file
if config.loadFromFile("my_config.json"):
    print("Configuration loaded successfully")

# Load from command line arguments
import sys
config.loadFromArgs(len(sys.argv), sys.argv)
```

#### Summary and Validation

```python
# Get configuration summary
summary = config.summary()
print(summary)

# Create default config and validate
brain = pynlm.createBrainWithDefaults()
if pynlm.validateBrainState(brain):
    print("Brain configuration is valid!")
```

### Brain Class

The Brain class implements the core neural simulation with real spiking neural computation using Leaky Integrate-and-Fire neurons and event-driven dynamics. It integrates multiple memory systems, prediction mechanisms, cognitive architectures, and neuromodulation systems.

#### Creation Methods

```python
# Create brain with default configuration
brain = pynlm.createBrainWithDefaults()

# Create brain with custom configuration
config = pynlm.createSimpleConfig(100, 0.2)
brain = pynlm.createBrain(config)

# Create minimal brain for testing
brain = pynlm.createMinimalBrain()
```

#### Basic Operations

```python
# Initialize brain (ALWAYS call this!)
brain.initialize()

# Perform simulation steps
for step in range(100):
    brain.step(step, step * 0.001)  # Include time
    brain.step(step)               # Default timestep (1ms)

# Receive sensory input
vision = pynlm.Vision(8, 8, 3)
audio = pynlm.Audio(44100, 1000)
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setAudio(audio)
brain.receiveSensoryInput(percept)

# Inject current (for testing/debugging)
brain.injectCurrent(neuron_id, 5.0)
brain.injectCurrentToNeurons(neuron_type, 3.0)

# Get action
action = brain.produceAction()
print(f"Action: {action.getName()}")
print(f"Parameters: {action.getParameters()}")
```

#### Brain Information

```python
# Get neural network statistics
stats = pynlm.getNeuralNetworkStats(brain)
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Excitatory/Inhibition ratio: {stats['excitatory_inhibition_ratio']:.3f}")

# Get developmental information
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Check brain state
if pynlm.validateBrainState(brain):
    print("Brain state is valid!")
```

#### Plasticity Control

```python
# Enable/disable all plasticity
brain.enableAllPlasticity()
brain.disableAllPlasticity()

# Get current plasticity status
status = brain.getPlasticityStatus()
print(f"STDP enabled: {status['stdp']}")
print(f"Hebbian enabled: {status['hebbian']}")
print(f"Structural enabled: {status['structural']}")
```

#### Error Handling and Recovery

```python
# Safe brain stepping with error handling
success = pynlm.safeBrainStep(brain, 0, 0.0)
if success:
    print("Brain step successful!")

# Safe reset with recovery
if pynlm.safeReset(brain):
    print("Brain reset and reinitialized successfully")
else:
    print("Reset failed, brain may be in unstable state")
```

#### Performance Monitoring

```python
# Get brain performance profile
profile = pynlm.createPerformanceProfile(brain)
print(f"Utilization: {profile['utilization']:.2f}")
print(f"Efficiency: {profile['efficiency']:.4f}")
print(f"Network complexity: {profile['network_complexity']:.2f}")
print(f"Activity level: {profile['activity_level']:.2f}")
```

### AgentBrain Class

The AgentBrain class provides an interface for connecting the neural brain to the simulation world, handling sensory processing, motor decoding, and neuromodulation based on agent-environment interactions.

#### Creation Methods

```python
# Create agent brain with given brain
agent = pynlm.createAgentBrain(brain)

# Create complete agent with brain and world
agent = pynlm.createAgent(brain, world)
```

#### Basic Operations

```python
# Initialize with world
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Decode motor command
action = agent.decodeMotorCommand()

# Complete simulation step
action = agent.simulateStep(world, 0.1)  # Returns action
```

#### Learning Features

```python
# Enable various learning mechanisms
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check if features are enabled
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")
```

#### Episode Simulation

```python
# Run complete episode
agent.simulateEpisode(world, 100)  # 100 steps

# Get episode statistics
stats = agent.getEpisodeStats()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Curiosity level: {stats['curiosity_level']:.2f}")
print(f"Prediction error: {stats['prediction_error']:.4f}")
```

### Sensory Input Classes

#### Vision Class

```python
# Create vision sensor
vision = pynlm.Vision(8, 8, 3)  # 8x8 pixels, 3 color channels
vision = pynlm.Vision(10, 10, 1)  # Grayscale

# Set vision data (flattened array)
image_data = np.random.randn(8 * 8 * 3).astype(np.float32)
vision.setData(list(image_data))

# Get vision properties
print(f"Width: {vision.getWidth()}")
print(f"Height: {vision.getHeight()}")
print(f"Channels: {vision.getChannels()}")
print(f"Image size: {vision.getImageSize()} pixels")

# Convert to NumPy array (requires numpy)
import numpy as np
array = vision.to_array()
print(f"Array shape: {array.shape}")  # (height, width, channels)
```

#### Audio Class

```python
# Create audio sensor
audio = pynlm.Audio(44100, 1000)  # sample_rate, num_samples

# Set audio data
audio_data = np.random.randn(1000).astype(np.float32)
audio.setData(list(audio_data))
audio.setSampleRate(22050)  # Change sample rate

# Get audio properties
print(f"Sample rate: {audio.getSampleRate()}")
print(f"Number of samples: {audio.getNumSamples()}")
print(f"Duration: {audio.getDuration():.2f} seconds")

# Convert to NumPy array
array = audio.to_array()
print(f"Array shape: {array.shape}")  # (num_samples,)
```

#### InternalSignals Class

```python
# Create internal signals sensor
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.5)
internal.addSignal(1.0)
internal.addSignal(-0.3)

# Get statistics
print(f"Signal count: {internal.getSignalCount()}")
print(f"Mean: {internal.getMean():.3f}")
print(f"Max: {internal.getMax():.3f}")
print(f"Min: {internal.getMin():.3f}")
```

### Action Class

```python
# Create default action
action = pynlm.Action()
print(f"Type: {action.getType()}")  # ActionType.Wait
print(f"Name: {action.getName()}")

# Create action with specific type
action = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Type: {action.getType()}")  # ActionType.MoveForward
print(f"Name: {action.getName()}")  # "MoveForward"

# Create action with type and parameters
action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
print(f"Type: {action.getType()}")
print(f"Parameters: {action.getParameters()}")

# Modify action
action.setType(pynlm.ActionType.TurnLeft)
action.setParameters([4.0, 5.0])

# Clone action
action2 = action.clone()
print(f"Cloned type: {action2.getType()}")  # Same type
print(f"Cloned parameters: {action2.getParameters()}")  # Same parameters
```

### WorldObject Class

```python
# Create world object
obj = pynlm.WorldObject(5.0, 10.0, pynlm.WorldObjectType.Resource, 15.0, 1.0)
print(f"Position: ({obj.x}, {obj.y})")
print(f"Type: {obj.type}")
print(f"Value: {obj.value}")
print(f"Radius: {obj.radius}")
print(f"Active: {obj.active}")

# Calculate distance to point
distance = obj.distanceTo(5.0, 10.0)
print(f"Distance to (5,10): {distance:.2f}")

# Check if at position
if obj.isAtPosition(5.0, 10.0):
    print("Object is at position (5,10)")

# Convert to string
print(repr(obj))
```

### AgentBody Class

```python
# Create agent body
body = pynlm.AgentBody()
body.x = 10.0
body.y = 5.0
body.orientation = 1.57  # π/2 radians
body.velocityX = 2.0
body.velocityY = -1.0
body.angularVelocity = 0.5
body.energy = 75.0
body.health = 90.0
body.age = 10
body.isMoving = True
body.isTurning = False

print(f"Position: ({body.x}, {body.y})")
print(f"Orientation: {body.getFacingAngle():.3f} radians")
print(f"Speed: {body.getSpeed():.2f}")
print(f"Energy: {body.energy}/100")
print(f"Health: {body.health}/100")
print(f"Healthy: {body.isHealthy()}")

# Set facing angle with normalization
body.setFacingAngle(7.0)  # > 2π
print(f"Normalized angle: {body.getFacingAngle():.3f} radians")

body.setFacingAngle(-1.0)  # < 0
print(f"Normalized angle: {body.getFacingAngle():.3f} radians")
```

### ActionResult Class

```python
# Create action result
result = pynlm.ActionResult(5.0, True, "Successfully ate food")
print(f"Reward: {result.reward}")
print(f"Success: {result.success}")
print(f"Message: {result.message}")

# Check result quality
if result.isGood():
    print("Great! This was a good outcome!")
elif result.isBad():
    print("This was a bad outcome...")
```

### SensoryPercept Class

```python
# Create sensory percept
percept = pynlm.SensoryPercept()

# Set various sensory inputs
vision = pynlm.Vision(8, 8, 3)
percept.setVision(vision)

audio = pynlm.Audio(44100, 1000)
percept.setAudio(audio)

internal = pynlm.InternalSignals()
percept.setInternal(internal)

# Check what modalities are available
print(f"Has vision: {percept.hasVision()}")
print(f"Has audio: {percept.hasAudio()}")
print(f"Has internal: {percept.hasInternal()}")

# Get total signal dimensions
total_dims = percept.getTotalSignalCount()
print(f"Total signal dimensions: {total_dims}")
```

### SimpleWorld Class

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent starting position
world.setAgentStart(5, 5)

# Reset world
world.reset()

# Update world
world.update(0.1)  # 0.1 seconds

# Get agent state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")

# Get sensory percept
percept = world.getSensoryPercept()

# Apply motor command
action = pynlm.Action(pynlm.ActionType.MoveForward)
world.applyMotorCommand(action, world.getSimulationTime())

# Check object at position
if world.isValidPosition(10, 10):
    print("Position (10,10) is valid")
else:
    print("Position (10,10) is invalid (wall or obstacle)")

# Get world statistics
print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Current time: {world.getSimulationTime():.2f} seconds")
```

## Factory Methods and Pythonic Features

### Configuration Factories

```python
# Different configuration factories
config1 = pynlm.createDefaultConfigWithDefaults()
config2 = pynlm.createSimpleConfig(100, 0.2)  # Custom neuron count and connection prob
config3 = pynlm.createMinimalConfig()       # Only essential params
config4 = pynlm.createWorldConfig(15, 15)   # Optimized for world simulation

# Create from dictionary
config_dict = {"neuron_count": 250, "learning_rate": 0.05}
config = pynlm.createConfigFromDict(config_dict)
```

### Brain Factories

```python
# Different brain creation methods
brain1 = pynlm.createBrainWithDefaults()    # Standard brain
brain2 = pynlm.createMinimalBrain()         # Small brain for testing
brain3 = pynlm.createBrain(config)           # Custom configuration

# Learning-optimized brain (all features enabled)
learning_brain = pynlm.createLearningBrain()

# Social learning brain
social_brain = pynlm.createSocialBrain()
```

### World Factories

```python
# Different world creation methods
world1 = pynlm.createSimpleWorld()
world2 = pynlm.createWorld(30, 25, 12, 12)  # Custom dimensions
world3 = pynlm.createWorldWithObjects(10, 10)  # With resources
```

### Agent Factories

```python
# Different agent creation methods
agent1 = pynlm.createAgentBrain(brain)
agent2 = pynlm.createAgent(brain, world)
```

### Pythonic Features

#### Dictionary Configuration

```python
# Create configuration from Python dictionary
config_dict = {
    "neuron_count": 250,
    "region_count": 2,
    "connection_probability": 0.12,
    "stdp_enabled": True,
    "reward_modulation_enabled": True
}

config = pynlm.createConfigFromDict(config_dict)

# Item access syntax (like Python dictionaries)
config["neuron_count"] = 500
if "stdp_enabled" in config:
    enabled = config["stdp_enabled"]
```

#### Context Managers

```python
# Use brain step context manager
brain = pynlm.createBrainWithDefaults()
with pynlm.withBrainStep(brain, 0, 0.0) as brain_ptr:
    # Context manager handles initialization
    brain_ptr.step(0, 0.0)
    print("Brain step completed successfully")
```

#### Data Conversion

```python
# Convert brain state to dictionary
brain_dict = pynlm.brainToDict(brain)
print(brain_dict["total_neurons"])
print(brain_dict["average_firing_rate"])

# Convert world state to dictionary
world_dict = pynlm.worldToDict(world)
print(f"World size: {world_dict['width']} x {world_dict['height']}")
```

## Error Handling and Validation

### Exception Types

NLM Python bindings define several custom exception types:

```python
# Import exception types
from pynlm import RuntimeError, InvalidArgumentError, OutOfRangeError

# These are raised by pybind11 when appropriate
try:
    # This might raise InvalidArgumentError
    config.set("invalid_key", None)
except InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except RuntimeError as e:
    print(f"Runtime error: {e}")
```

### Validation Functions

```python
# Comprehensive brain state validation
if pynlm.validateBrainState(brain):
    print("Brain state is valid!")
else:
    print("Brain state has issues")

# Configuration validation
config = pynlm.createDefaultConfigWithDefaults()
config.setInt("neuron_count", 1000)
if config.getInt("neuron_count") > 0:
    print("Configuration is valid")
```

### Error Recovery

```python
# Safe brain stepping with error handling
success = pynlm.safeBrainStep(brain, 0, 0.0)
if not success:
    print("Brain step failed, attempting recovery...")
    if pynlm.safeReset(brain):
        print("Recovery successful!")
    else:
        print("Recovery failed, brain may be in unstable state")
```

### Input Validation

```python
# Validate configuration before use
def validate_config(config):
    required_keys = ["neuron_count", "connection_probability"]
    for key in required_keys:
        if not config.has(key):
            raise InvalidArgumentError(f"Missing required key: {key}")
    
    neuron_count = config.getInt("neuron_count")
    if neuron_count <= 0:
        raise InvalidArgumentError("neuron_count must be positive")
    
    conn_prob = config.getFloat("connection_probability")
    if conn_prob < 0 or conn_prob > 1:
        raise InvalidArgumentError("connection_probability must be between 0 and 1")

validate_config(config)
```

## Performance Monitoring and Benchmarking

### Performance Profiling

```python
# Create detailed performance profile
profile = pynlm.createPerformanceProfile(brain)

print("=== Performance Profile ===")
print(f"Neurons: {profile['neuron_count']}")
print(f"Synapses: {profile['synapse_count']}")
print(f"Utilization: {profile['utilization']:.2%}")
print(f"Efficiency: {profile['efficiency']:.4f}")
print(f"Network complexity: {profile['network_complexity']:.2f}")
print(f"Activity level: {profile['activity_level']:.2f}")
```

### Benchmarking

```python
# Benchmark brain stepping operations
benchmark = pynlm.benchmarkBrainOperations(brain, 100)

print("=== Brain Stepping Benchmark ===")
print(f"Steps: {benchmark['total_steps']}")
print(f"Duration: {benchmark['duration_seconds']:.3f} seconds")
print(f"Steps per second: {benchmark['steps_per_second']:.0f}")
print(f"Average step time: {benchmark['avg_step_time_ms']:.3f} ms")
print(f"Memory operations: {benchmark['memory_ops_per_second']:.0f} ops/sec")
```

### Performance Visualization

```python
# Get ASCII status visualization
status = pynlm.asciiBrainStatus(brain)
print(status)

# Get brain statistics as dictionary
stats = pynlm.brainToDict(brain)
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
```

## Advanced Features for Power Users

### Simulation Control

```python
# Run complete simulation episode
def run_simulation(agent, world, num_steps):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinking
        agent.getBrain()->step(step, world.getSimulationTime())
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Log progress every 10 steps
        if step % 10 == 0:
            stats = agent.getEpisodeStats()
            print(f"Step {step}: {stats['total_spikes']} spikes, {stats['firing_neurons']} firing neurons")

run_simulation(agent, world, 500)
```

### Multiple Agents

```python
# Create multiple agents sharing a brain
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Create different worlds for each agent
world1 = pynlm.createWorld(10, 10)
world1.setAgentStart(2, 2)

world2 = pynlm.createWorld(10, 10)
world2.setAgentStart(7, 7)

# Create agent brains
agent1 = pynlm.createAgentBrain(brain)
agent2 = pynlm.createAgentBrain(brain)

# Initialize with different worlds
agent1.initialize(world1)
agent2.initialize(world2)

# Run simulations in parallel (conceptually)
for step in range(100):
    # Agent 1 simulation
    world1.update(0.1)
    agent1.processSensoryInput(world1.getSensoryPercept())
    brain.step(step, world1.getSimulationTime())
    action1 = agent1.decodeMotorCommand()
    world1.applyMotorCommand(action1, world1.getSimulationTime())
    
    # Agent 2 simulation (same brain, different time)
    world2.update(0.1)
    agent2.processSensoryInput(world2.getSensoryPercept())
    brain.step(step + 100, world2.getSimulationTime())  # Different time for same brain
    action2 = agent2.decodeMotorCommand()
    world2.applyMotorCommand(action2, world2.getSimulationTime())
```

### Serialization

```python
# Save brain state to file
brain.save("my_brain.nlm")

# Load brain state from file
new_brain = pynlm.createBrainWithDefaults()
if new_brain.load("my_brain.nlm"):
    print("Brain state loaded successfully!")
else:
    print("Failed to load brain state")
```

### Configuration Management

```python
# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
if config.loadFromFile("my_config.json"):
    print("Configuration loaded!")
else:
    print("Using default configuration")

# Create configuration from command line arguments
import sys
config.loadFromArgs(len(sys.argv), sys.argv)
```

## Help and Documentation

### Module Information

```python
import pynlm

# Get module version
print(f"NLM Version: {pynlm.getVersion()}")

# Get module constants
constants = pynlm.getConstants()
print(f"Invalid neuron ID: {constants['invalid_neuron_id']}")
print(f"Default timestep: {constants['default_timestep']}")

# Print welcome message
pynlm.printHello()
```

### Interactive Help

```python
# Get help for specific classes
help(pynlm.Brain)
help(pynlm.Config)
help(pynlm.Action)
help(pynlm.WorldObject)

# Get help for functions
help(pynlm.createBrain)
help(pynlm.createDefaultConfig)
help(pynlm.simulateEpisode)

# Get help for enumerations
help(pynlm.ActionType)
help(pynlm.NeuronType)
```

## Example Gallery

### Example 1: Silent Brain

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Silent brain test done!")
print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
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
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
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

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"ImportError: No module named pynlm"**
- Make sure pynlm is installed: `pip install pynlm`
- If building from source, run `python setup.py build_ext --inplace`

**"RuntimeError: Failed to initialize brain"**
- Check that configuration is valid
- Try with default configuration: `brain = pynlm.createBrainWithDefaults()`

### Debugging Tips

```python
# Enable debug output in configuration
config = pynlm.createDefaultConfigWithDefaults()
config.setBool("debug_output", True)

# Create brain with debug config
brain = pynlm.createBrain(config)
brain.initialize()

# Add print statements to your code
for step in range(10):
    print(f"Step {step}: total spikes = {brain.getTotalSpikeCount()}")
    brain.step(step, step * 0.001)

# Use the ASCII status function
status = pynlm.asciiBrainStatus(brain)
print(status)
```

## References

### Core Concepts

1. **Spiking Neural Networks**: NLM implements Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
2. **Plasticity Rules**: STDP (Spike-Timing-Dependent Plasticity) and Hebbian learning
3. **Neuromodulation**: Dopamine, curiosity, and novelty systems
4. **Memory Systems**: Working memory, episodic memory, and associative memory
5. **Development**: Structural plasticity and developmental stages
6. **Predictive Processing**: Forward models and prediction errors

### Neural Architecture

The NLM brain follows a closed-loop architecture:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Performance Considerations

1. **Memory Management**: NLM uses memory pools for efficient allocation
2. **Event-Driven Computation**: Spike events are processed asynchronously
3. **Sparse Connectivity**: Optional sparse connectivity for large networks
4. **SIMD Vectorization**: Optional SIMD acceleration for performance

### Limitations

1. **Simplified Neuron Model**: NLM uses LIF neurons, not detailed biophysical models
2. **Limited Scale**: Current implementation supports up to ~10,000 neurons
3. **Developmental Scope**: Developmental stages are simplified for simulation
4. **Memory Capacity**: Memory systems have finite capacity

## License

NLM is licensed under the MIT License. See the LICENSE file for details.

## Copyright

Copyright © 2024 NLM Research Team. All rights reserved.

## Authors

NLM is a research project maintained by:
- NLM Research Team
- Department of Computational Neuroscience
- Institute for Brain and Mind Studies

For more information, visit the NLM project website or documentation.

---

*This documentation was generated automatically from the NLM Python bindings source code.*
*Last updated: $(date)*
*Version: 0.1.0*