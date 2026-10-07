# NLM Python Package - Quick Start Guide

# Overview

The NLM Python Package provides a Python interface to the Neural Learning Machine (NLM) brain simulator. This improved Python API offers:

- **Pythonic API design** with intuitive method names and type hints
- **Comprehensive error handling** with specific exception types
- **Flexible configuration** with both programmatic and file-based options
- **Extensive examples** and documentation
- **Advanced features** for experienced users

# Installation

## Requirements

- Python 3.8 or higher
- pybind11
- C++ compiler with C++20 support

## Installation Steps

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. **Build the project**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Install Python bindings**
   ```bash
   pip install -e .
   ```

# Quick Start

## Basic Usage

### Create a Simple Brain

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make brain think
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Brain has emitted {brain.getTotalSpikeCount()} spikes")
```

### Create a Complete Agent

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config["brain.regionCount"] = 5
config["brain.plasticityHebbianRate"] = 0.01

brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Use Sensory Inputs

```python
import pynlm

# Create different sensory inputs
vision = pynlm.createVision(width=64, height=64, channels=3)
# Set vision data (e.g., from camera or image processing)

internal = pynlm.createInternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.8)
internal.addSignal(0.3)

# Access sensory data
print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
print(f"Internal signals: {internal.getSignals()}")
```

### Create and Use Actions

```python
import pynlm

# Use predefined actions
action = pynlm.move_forward()
action = pynlm.turn_left()
action = pynlm.interact()
action = pynlm.wait()

# Create custom action with parameters
parameters = [0.5, 0.8, 0.3]
custom_action = pynlm.createCustomAction(parameters)

# Check action properties
print(f"Action: {action.getName()}")
print(f"Is movement: {action.isMovement()}")
print(f"Is interaction: {action.isInteraction()}")
```

### World Objects

```python
import pynlm

# Create world objects
resource = pynlm.createResource(x=10, y=10, value=5.0)
wall = pynlm.createWall(x=15, y=15, radius=0.8)
hazard = pynlm.createHazard(x=25, y=25, value=2.0)

# Add to world
world.add_object(resource)
world.add_object(wall)
world.add_object(hazard)

# Check object properties
print(f"Resource passable: {resource.isPassable()}")
print(f"Wall solid: {wall.isSolid()}")
print(f"Hazard collideable: {hazard.isCollideable()}")
```

# Configuration Management

## Programmatic Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config["brain.regionCount"] = 8
config["brain.plasticityHebbianRate"] = 0.02
config["brain.developmentPlasticityMultiplier"] = 1.5
config["world.width"] = 50
config["world.height"] = 50

# Get configuration values
region_count = config["brain.regionCount"]
plasticity_rate = config["brain.plasticityHebbianRate"]

print(f"Region count: {region_count}")
print(f"Plasticity rate: {plasticity_rate}")
```

## File-based Configuration

```python
import pynlm

# Create and save configuration
config = pynlm.createDefaultConfig()
config["brain.regionCount"] = 5
config.save_to_file("my_config.json")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.load_from_file("my_config.json")

# Or load from command line arguments
loaded_config.load_from_args()
```

# Brain Control and Monitoring

## Basic Statistics

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"EI ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

## Advanced Brain Features

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

# Get brain systems
spike_system = brain.getSpikeSystem()
stdp = brain.getSTDP()
working_memory = brain.getWorkingMemory()

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage.name}")

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)

# Save and load brain state
brain.save("checkpoint.json")
# ... later ...
# brain.load("checkpoint.json")
# brain.initialize()  # Need to reinitialize after load
```

# Agent Features

## Neuromodulation

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Check neuromodulation levels
print(f"Reward mod level: {agent.getNeuromodulationLevel():.2f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"Novelty level: {agent.getNoveltyLevel():.2f}")
print(f"Prediction error: {agent.getPredictionError():.2f}")
```

## Development

```python
import pynlm

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Check development
stage = agent.getDevelopmentalStage()
print(f"Developmental stage: {stage.name}")

# Update development
agent.updateDevelopment(0.1)  # Update by 0.1 time units

# Development feature flags
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")
```

# Action Results

```python
import pynlm

# Action results contain reward, success status, and message
action = pynlm.move_forward()
result = world.applyMotorCommand(action, current_time)

print(f"Action result: {result.message}")
print(f"Reward: {result.reward}")
print(f"Success: {result.success}")
print(f"Confidence: {result.confidence}")

# Check if result is good
if result.is_good(minReward=0.5):
    print("Result is good!")
elif result.is_bad():
    print("Result is bad!")
```

# Examples and Testing

Run the examples to see the full functionality:

```bash
python -m pynlm.examples
```

Or run individual examples:

```bash
# Basic brain usage
python -c "import pynlm; brain = pynlm.createBrain(); brain.initialize(); print(f'Neurons: {brain.getTotalNeuronCount()}')"

# Complete agent example
python -c "
import pynlm
brain = pynlm.createBrain()
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print('Agent created successfully')
"
```

# API Reference

## Core Classes

### Brain
- `create_brain(config=None)` - Create a new brain
- `brain.initialize()` - Initialize the brain
- `brain.step(step, time=None)` - Perform a simulation step
- `brain.produce_action()` - Produce motor action
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state

### Config
- `create_default_config()` - Create default configuration
- `config.set(key, value)` - Set configuration value
- `config.get(key, default=None)` - Get configuration value
- `config[key]` - Dictionary-style access

### SimpleWorld
- `create_simple_world()` - Create new world
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.reset()` - Reset world
- `world.update(timestep)` - Update world state
- `world.apply_motor_command(action, time)` - Execute action

### AgentBrain
- `create_agent_brain(brain)` - Create agent interface
- `agent.initialize(world)` - Initialize agent
- `agent.process_sensory_input(percept)` - Process sensory input
- `agent.decode_motor_command()` - Get motor command

## Sensory Systems

### Vision
- `create_vision(width, height, channels=3)` - Create vision sensor
- `vision.set_data(data)` - Set vision data
- `vision.get_width()` - Get vision width
- `vision.get_height()` - Get vision height
- `vision.get_grayscale()` - Get grayscale data

### Audio
- `create_audio(sample_rate, num_samples)` - Create audio sensor
- `audio.set_data(data)` - Set audio samples
- `audio.get_sample_rate()` - Get sample rate
- `audio.get_num_samples()` - Get sample count

### InternalSignals
- `create_internal_signals()` - Create internal signals
- `internal.add_signal(value)` - Add signal
- `internal.get_signals()` - Get all signals
- `internal.get_signal_mean()` - Get signal mean

## Motor System

### ActionType
- `move_forward()` - Movement action
- `turn_left()` - Turn action
- `interact()` - Interaction action
- `wait()` - Wait action

### Action
- `create_action(action_type, parameters=None)` - Create action
- `action.get_type()` - Get action type
- `action.set_parameters(parameters)` - Set parameters
- `action.is_movement()` - Check if movement
- `action.is_interaction()` - Check if interaction

## Supporting Classes

### AgentBody
- `create_agent_body()` - Create agent body
- `body.x, body.y` - Position coordinates
- `body.orientation` - Facing direction
- `body.get_position()` - Get position as tuple
- `body.move_to(x, y, speed=1.0, duration=1.0)` - Move to position

### ActionResult
- `create_action_result()` - Create action result
- `result.reward` - Reward value
- `result.success` - Success status
- `result.message` - Result message
- `result.is_good()` - Check if result is good
- `result.is_bad()` - Check if result is bad

### WorldObject
- `create_world_object(x, y, obj_type, value=0, radius=0.5)` - Create world object
- `create_wall(x, y, radius)` - Create wall
- `create_resource(x, y, value, radius)` - Create resource
- `create_hazard(x, y, value, radius)` - Create hazard

## Enumerations

### DevelopmentalStage
- `DevelopmentalStage.Initial` - Initial state
- `DevelopmentalStage.CriticalPeriod` - Critical learning
- `DevelopmentalStage.Maturation` - Maturation phase
- `DevelopmentalStage.Adult` - Adult state
- `DevelopmentalStage.Aging` - Aging phase

### ActionType
- `ActionType.MoveForward`, `ActionType.MoveBackward`
- `ActionType.TurnLeft`, `ActionType.TurnRight`
- `ActionType.Look`, `ActionType.Interact`
- `ActionType.Eat`, `ActionType.Drink`
- `ActionType.Rest`, `ActionType.Wait`
- `ActionType.Custom`

### WorldObjectType
- `WorldObjectType.Empty`, `WorldObjectType.Resource`
- `WorldObjectType.Hazard`, `WorldObjectType.Wall`
- `WorldObjectType.Marker`

# Error Handling

The NLM Python API provides specific exception types for error handling:

```python
import pynlm

from pynlm import NLMError, ConfigLoadError, BrainInitializationError, WorldError

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except BrainInitializationError as e:
    print(f"Failed to initialize brain: {e}")
except ConfigLoadError as e:
    print(f"Failed to load configuration: {e}")
except WorldError as e:
    print(f"World operation failed: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

# Migration Guide

## From Old API to New API

### Configuration

**Old:**
```python
config = {"brain.regionCount": 5}
```

**New:**
```python
config = pynlm.createDefaultConfig()
config["brain.regionCount"] = 5
```

### Brain Creation

**Old:**
```python
from pynlm import Brain
brain = Brain()
```

**New:**
```python
import pynlm
brain = pynlm.createBrain()
```

### Agent Creation

**Old:**
```python
from pynlm import AgentBrain
agent = AgentBrain(brain)
```

**New:**
```python
import pynlm
agent = pynlm.createAgentBrain(brain)
```

### Sensory Input

**Old:**
```python
from pynlm import Vision
vision = Vision(64, 64, 3)
```

**New:**
```python
import pynlm
vision = pynlm.createVision(64, 64, 3)
```

# Troubleshooting

## Common Issues

### "Brain is not initialized"
Ensure you call `brain.initialize()` before using the brain.

### "Configuration key not found"
Check that the configuration key exists, or provide a default value:
```python
value = config.get("brain.regionCount", 1)
```

### "Action type not supported"
Use valid action types: `move_forward()`, `turn_left()`, etc.

### "World not configured"
Call `world.configure()` before using the world.

## Getting Help

- Check the examples in `pynlm/examples.py`
- Read the documentation in `docs/`
- Use the Python interpreter for interactive testing
- Check error messages for specific guidance

# Version Information

```python
import pynlm
print(f"NLM Python Package version: {pynlm.__version__}")
```

# License

This project is licensed under the MIT License.