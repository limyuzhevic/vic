# NLM Python Bindings Documentation

## Overview

The NLM (Neural Learning Machine) Python bindings provide a modular interface to the NLM neural simulation framework, which implements a brain-inspired architecture with neurons and synapses that learn from experience through interaction with an environment.

This documentation covers the improved Python bindings with enhanced error handling, validation, and convenience functions.

## Table of Contents

1. [Getting Started](#getting-started)
2. [API Reference](#api-reference)
   - [Config Class](#config-class)
   - [Brain Class](#brain-class)
   - [AgentBrain Class](#agentbrain-class)
   - [SimpleWorld Class](#simpleworld-class)
   - [SensoryInput Classes](#sensoryinput-classes)
   - [Action Classes](#action-classes)
   - [Exception Hierarchy](#exception-hierarchy)
3. [Best Practices](#best-practices)
4. [Migration Guide](#migration-guide)
5. [Examples](#examples)
6. [Troubleshooting](#troubleshooting)

## Getting Started

### Prerequisites

- Python 3.7 or higher
- NumPy (optional, for data processing)
- pybind11 (already included in the build)

### Installation

The NLM Python bindings are built as part of the main NLM project:

```bash
# Build the project with Python bindings support
cd /path/to/nlm/project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
```

After building, the Python module `pynlm` will be available in the `python` directory.

```bash
# Install the module (optional but recommended)
pip install ./python
```

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.create_config()

# Create a brain with the configuration
brain = pynlm.create_brain(config)

# Initialize the brain
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

# Log brain status
brain.log_status()
```

## API Reference

### Exception Hierarchy

The NLM Python bindings include a comprehensive exception hierarchy for better error handling:

```python
# Base exception for all NLM errors
class NLMError(Exception):
    """Base class for all NLM exceptions"""

# Configuration errors
class ConfigError(NLMError):
    """Raised for configuration-related errors"""

# Validation errors
class ValidationError(NLMError):
    """Raised for validation errors (invalid parameters, ranges, etc.)"""

# Creation errors
class CreationError(NLMError):
    """Raised when objects cannot be created"""

# Brain-specific errors
class BrainError(NLMError):
    """Raised for brain-specific errors"""

# Agent-specific errors
class AgentError(NLMError):
    """Raised for agent-specific errors"""

# World-specific errors
class WorldError(NLMError):
    """Raised for world/environment errors"""

# Sensory-specific errors
class SensoryError(NLMError):
    """Raised for sensory processing errors"""

# Motor-specific errors
class MotorError(NLMError):
    """Raised for motor command errors"""
```

### Config Class

The `Config` class provides a flexible key-value storage system for NLM configuration with improved error handling.

```python
import pynlm

# Create a configuration
config = pynlm.create_config()

# Create a new config instance
config = pynlm.Config()

# Set configuration values
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)
config.set("neuron_count", 500)
config.set("connection_probability", 0.15)

# Get configuration values
seed = config.get("random_seed")  # Returns 42 as int
timestep = config.get_or("simulation_timestep", 0.001)  # Returns timestep with default

# Check if key exists
if config.has("neuron_count"):
    count = config.get("neuron_count")

# Get all configuration keys
keys = config.getKeys()

# Save configuration to JSON file
try:
    config.saveToFile("config.json")
except pynlm.ConfigError as e:
    print(f"Error saving config: {e}")

# Load configuration from JSON file
try:
    config.loadFromFile("config.json")
except pynlm.ConfigError as e:
    print(f"Error loading config: {e}")

# Load from command line arguments (ignores unsupported args)
config.loadFromArgs(3, ["--neuron_count", "1000", "--connection_probability", "0.2"])

# Clear all configuration entries
config.clear()

# Get a summary of the configuration
summary = config.summary()

# String representation
print(repr(config))  # <Config: random_seed=42, simulation_timestep=0.001, ...>
```

#### Config Class Methods

- `__init__()` - Creates a new empty configuration
- `loadFromFile(filepath: str) -> bool` - Loads configuration from JSON file, returns success status
- `loadFromArgs(argc: int, argv: list) -> bool` - Loads configuration from command line arguments
- `saveToFile(filepath: str) -> bool` - Saves configuration to JSON file, returns success status
- `has(key: str) -> bool` - Checks if a configuration key exists
- `get(key: str, type: type) -> Optional[type]` - Gets a configuration value with type checking
- `getOr(key: str, default_value: Any) -> Any` - Gets a value with a default if key doesn't exist
- `set(key: str, value: Any, source: ConfigSource = ConfigSource.Runtime) -> None` - Sets a configuration value
- `remove(key: str) -> None` - Removes a configuration key
- `getKeys() -> list` - Gets all configuration keys
- `clear() -> None` - Clears all configuration entries
- `summary() -> str` - Returns a summary string of the configuration

### Brain Class

The `Brain` class implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

```python
import pynlm

# Create configuration
config = pynlm.create_config()
config.set("neuron_count", 500)
config.set("region_count", 1)
config.set("simulation_timestep", 0.001)

# Create brain
brain = pynlm.create_brain(config)

# Initialize the brain
brain.initialize()

# Get basic statistics
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Synapse count: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Run simulation steps
for step in range(100):
    brain.step(step)

# Inject current into neurons
brain.injectCurrent(pynlm.INVALID_NEURON_ID, 50.0)
brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 30.0)

# Get regions
region_ids = brain.getRegionIds()
for region_id in region_ids:
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getNeuronCount()} neurons")

# Produce motor output
action = brain.produceAction()
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")

# Save brain state
try:
    brain.save("brain_checkpoint.json")
    print("Brain state saved successfully")
except pynlm.BrainError as e:
    print(f"Error saving brain: {e}")

# Load brain state
try:
    brain.load("brain_checkpoint.json")
    print("Brain state loaded successfully")
except pynlm.BrainError as e:
    print(f"Error loading brain: {e}")

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Get brain status
brain.logStatus()
```

#### Brain Class Methods

- `__init__(config: Config) -> Brain` - Creates a new brain with configuration
- `initialize() -> bool` - Initializes the brain with configuration
- `step(current_step: int) -> None` - Performs a simulation step
- `step(current_step: int, current_time: float) -> None` - Performs a simulation step with timestamp
- `receiveSensoryInput(input: SensoryInput) -> None` - Injects sensory input into the brain
- `injectCurrent(neuron: NeuronId, current: float) -> None` - Injects current into a specific neuron
- `injectCurrentToNeurons(neuron_type: NeuronType, current: float) -> None` - Injects current into all neurons of a specific type
- `produceAction() -> Action` - Produces motor action based on neural activity
- `reset() -> None` - Resets brain state
- `save(filepath: str) -> bool` - Saves brain state to file
- `load(filepath: str) -> bool` - Loads brain state from file
- `addRegion(name: str = "") -> RegionId` - Adds a new neural region
- `getRegion(id: RegionId) -> NeuralRegion` - Gets a region by ID
- `getRegionCount() -> int` - Gets the number of regions
- `getRegionIds() -> list` - Gets all region IDs
- `getRegions() -> list` - Gets all regions
- `getTotalNeuronCount() -> int` - Gets total neuron count across all regions
- `getTotalSynapseCount() -> int` - Gets total synapse count across all regions
- `getActiveNeuronCount() -> int` - Gets count of active neurons
- `getFiringNeuronCount() -> int` - Gets count of currently firing neurons
- `getAverageFiringRate() -> float` - Gets average firing rate across all neurons
- `getExcitationInhibitionRatio() -> float` - Gets excitation/inhibition balance ratio
- `getTotalSpikeCount() -> int` - Gets total spike count
- `getDevelopmentalStage() -> DevelopmentalStage` - Gets current developmental stage
- `setDevelopmentalStage(stage: DevelopmentalStage) -> None` - Sets developmental stage
- `getConfig() -> Config` - Gets the configuration
- `logStatus() -> None` - Logs brain status

### AgentBrain Class

The `AgentBrain` class provides a convenient interface that connects the NLM brain to the world, handling sensory transduction and motor decoding.

```python
import pynlm

# Create brain and agent brain
config = pynlm.create_config()
brain = pynlm.create_brain(config)
agent_brain = pynlm.create_agent_brain(brain)

# Create and configure world
world = pynlm.create_simple_world()
world.configure(100.0, 100.0, 10, 10)
world.setAgentStart(50.0, 50.0)

# Initialize agent brain with world
agent_brain.initialize(world)

# Get expected sensory input/output sizes
print(f"Sensory input size: {agent_brain.getSensoryInputSize()}")
print(f"Motor output size: {agent_brain.getMotorOutputSize()}")

# Get agent's current state
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
print(f"Agent energy: {agent_body.energy}")

# Get sensory perception
sensory_percept = world.getSensoryPercept()

# Process sensory input (convert perception to sensory input)
# This would depend on your specific implementation
try:
    agent_brain.processSensoryInput(sensory_percept)
except pynlm.SensoryError as e:
    print(f"Error processing sensory input: {e}")

# Get motor command
try:
    motor_cmd = agent_brain.decodeMotorCommand()
    print(f"Decoded motor command: {motor_cmd}")
    
    # Apply motor command to world
    result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    print(f"Action result - Reward: {result.reward}, Success: {result.success}")
    
    # Apply reward modulation
    agent_brain.applyRewardModulation(result.reward, 0.5)  # Assuming predicted reward of 0.5
except pynlm.MotorError as e:
    print(f"Error decoding motor command: {e}")

# Update development system
agent_brain.updateDevelopment(world.getSimulationTime())

# Get neuromodulation levels
print(f"Dopamine level: {agent_brain.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent_brain.getCuriosityLevel()}")
print(f"Novelty level: {agent_brain.getNoveltyLevel()}")
print(f"Prediction error: {agent_brain.getPredictionError()}")

# Check system status
print(f"Reward modulation enabled: {agent_brain.isRewardModulationEnabled()}")
print(f"Development enabled: {agent_brain.isDevelopmentEnabled()}")
print(f"Curiosity enabled: {agent_brain.isCuriosityEnabled()}")

# Reset agent for new episode
agent_brain.reset()

# Get underlying brain (for advanced operations)
brain = agent_brain.getBrain()
```

#### AgentBrain Class Methods

- `__init__(brain: Brain) -> AgentBrain` - Creates a new agent brain interface
- `initialize(world: SimpleWorld) -> None` - Initializes with world
- `getSensoryInputSize() -> int` - Gets expected sensory input size
- `getMotorOutputSize() -> int` - Gets expected motor output size
- `processSensoryInput(percept: SensoryPercept) -> None` - Processes sensory percept and injects into brain
- `decodeMotorCommand() -> MotorCommand` - Decodes brain motor activity into motor command
- `applyRewardModulation(reward: float, predicted_reward: float) -> None` - Applies reward-based neuromodulation
- `updateDevelopment(timestep: float) -> None` - Updates development system
- `getDevelopmentalStage() -> DevelopmentalStage` - Gets current developmental stage
- `getNeuromodulationLevel() -> float` - Gets current neuromodulation level
- `getCuriosityLevel() -> float` - Gets curiosity level
- `getNoveltyLevel() -> float` - Gets novelty level
- `getPredictionError() -> float` - Gets prediction error
- `reset() -> None` - Resets agent for new episode
- `getBrain() -> Brain` - Gets the underlying brain
- `enableRewardModulation(enable: bool) -> None` - Enables/disables reward modulation
- `enableStructuralPlasticity(enable: bool) -> None` - Enables/disables structural plasticity
- `enableDevelopment(enable: bool) -> None` - Enables/disables development
- `enableCuriosity(enable: bool) -> None` - Enables/disables curiosity
- `isRewardModulationEnabled() -> bool` - Checks if reward modulation is enabled
- `isStructuralPlasticityEnabled() -> bool` - Checks if structural plasticity is enabled
- `isDevelopmentEnabled() -> bool` - Checks if development is enabled
- `isCuriosityEnabled() -> bool` - Checks if curiosity is enabled

### SimpleWorld Class

The `SimpleWorld` class provides a simulated 2D environment for NLM development with enhanced features.

```python
import pynlm

# Create simple world
world = pynlm.create_simple_world()

# Configure world
world.configure(width=100.0, height=100.0, visionWidth=10, visionHeight=10)

# Set agent starting position
world.setAgentStart(50.0, 50.0)

# Add objects to the world
# Create a resource object
resource = pynlm.WorldObject(x=20.0, y=20.0, type=pynlm.WorldObjectType.Resource, value=10.0)
world.addObject(resource)

# Create a wall
wall = pynlm.WorldObject(x=70.0, y=70.0, type=pynlm.WorldObjectType.Wall, radius=2.0)
world.addObject(wall)

# Create a hazard
hazard = pynlm.WorldObject(x=80.0, y=80.0, type=pynlm.WorldObjectType.Hazard, value=-5.0)
world.addObject(hazard)

# Check position validity
position = world.isValidPosition(30.0, 30.0)
print(f"Position (30, 30) valid: {position}")

# Get object at position
obj = world.getObjectAt(20.0, 20.0)
if obj:
    print(f"Object at (20, 20): type={obj.type}, value={obj.value}")

# Remove object
world.removeObject(20.0, 20.0)

# Update world state
for step in range(100):
    world.update(step * 0.001)  # timestep = 0.001
    
    # Get current perception
    percept = world.getSensoryPercept()
    
    # Update agent brain (simplified example)
    # agent_brain.processSensoryInput(percept)
    
    # Get motor command and apply
    # motor_cmd = agent_brain.decodeMotorCommand()
    # result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())

# Get agent body state
agent = world.getAgentBody()
print(f"Agent position: ({agent.x}, {agent.y})")
print(f"Agent orientation: {agent.orientation}")
print(f"Agent energy: {agent.energy}")
print(f"Agent health: {agent.health}")
print(f"Agent age: {agent.age}")

# Get world dimensions
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")

# Configure energy settings
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Energy decay rate: {world.getEnergyDecayRate()}")

# Set random seed for reproducibility
world.setRandomSeed(42)

# Reset world
world.reset()

# World statistics
print(f"Simulation time: {world.getSimulationTime()}")
print(f"Objects in world: {len(world.objects_)}")  # Note: objects_ is internal, may change
```

#### SimpleWorld Class Methods

- `__init__() -> SimpleWorld` - Creates a new empty world
- `configure(width: float, height: float, visionWidth: int, visionHeight: int) -> None` - Configures world dimensions and vision
- `reset() -> None` - Resets world to initial state
- `setAgentStart(x: float, y: float) -> None` - Sets agent starting position
- `update(timestep: float) -> None` - Updates world state
- `applyMotorCommand(cmd: MotorCommand, currentTime: float) -> ActionResult` - Applies motor command to agent
- `getSensoryPercept() -> SensoryPercept` - Gets current sensory percept for the agent
- `getAgentBody() -> AgentBody` - Gets agent body state
- `addObject(obj: WorldObject) -> None` - Adds object to world
- `removeObject(x: float, y: float) -> None` - Removes object at position
- `isValidPosition(x: float, y: float) -> bool` - Checks if position is valid (not wall)
- `getObjectAt(x: float, y: float) -> WorldObject` - Gets object at position (if any)
- `getWidth() -> float` - Gets world width
- `getHeight() -> float` - Gets world height
- `getMaxEnergy() -> float` - Gets maximum energy
- `setMaxEnergy(e: float) -> None` - Sets maximum energy
- `getEnergyDecayRate() -> float` - Gets energy decay rate
- `setEnergyDecayRate(r: float) -> None` - Sets energy decay rate
- `getSimulationTime() -> float` - Gets simulation time
- `setRandomSeed(seed: uint64_t) -> None` - Sets random seed
- `getRandomSeed() -> uint64_t` - Gets random seed

### SensoryInput Classes

#### Vision

Vision sensory input representing visual information with enhanced data handling.

```python
import pynlm

# Create vision input
vision = pynlm.Vision()

# Create with dimensions
vision = pynlm.Vision(width=10, height=10, channels=3)

# Set vision data
# Example: 10x10 RGB image (300 values)
import numpy as np
data = np.random.rand(10, 10, 3).flatten().tolist()
vision.setData(data)

# Alternative data setting
vision.setData(data_array)  # Accepts array or vector of floats

# Get vision properties
print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision channels: {vision.getChannels()}")

# Get base class properties
print(f"Vision type: {vision.getType()}")
print(f"Vision dimensions: {vision.getDimensions()}")
print(f"Vision data size: {len(vision.getData())}")

# Set timestamp
vision.setTimestamp(world.getSimulationTime())
print(f"Vision timestamp: {vision.getTimestamp()}")

# Clone vision
vision_clone = vision.clone()
```

#### Audio

Audio sensory input representing sound with enhanced sampling capabilities.

```python
import pynlm

# Create audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set audio data
samples = [0.0] * 1024  # 1 second of silence
# Or generate audio
import numpy as np
import math
t = np.linspace(0, 1, 1024)
samples = np.sin(2 * np.pi * 440 * t).tolist()  # 440 Hz sine wave
audio.setData(samples)

# Set sample rate
audio.setSampleRate(48000)

# Get audio properties
print(f"Sample rate: {audio.getSampleRate()}")
print(f"Number of samples: {audio.getNumSamples()}")
print(f"Audio dimensions: {audio.getDimensions()}")

# Get raw data
sound_data = audio.getData()

# Clone audio
audio_clone = audio.clone()
```

#### InternalSignals

Internal signals representing the brain's internal state and homeostasis.

```python
import pynlm

# Create internal signals
internal = pynlm.InternalSignals()

# Add various internal state signals
internal.addSignal(0.5)  # Hemispheric asymmetry
internal.addSignal(0.8)  # Activity level
internal.addSignal(0.3)  # Homeostatic pressure
internal.addSignal(-0.1)  # Stress level
internal.addSignal(1.2)  # Excitatory drive

# Clear all signals
internal.clearSignals()

# Get base class properties
print(f"Internal signals type: {internal.getType()}")
print(f"Internal signals dimensions: {internal.getDimensions()}")
print(f"Internal signals count: {len(internal.getData())}")

# Set timestamp
internal.setTimestamp(world.getSimulationTime())
print(f"Internal signals timestamp: {internal.getTimestamp()}")

# Clone internal signals
internal_clone = internal.clone()
```

### Action Classes

Action representation for motor output with enhanced parameter handling.

```python
import pynlm

# Create action with default type
action = pynlm.Action()

# Create action with specific type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with type and parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.7])

# Get and set action type
print(f"Action type: {action.getType()}")
action.setType(pynlm.ActionType.MoveLeft)

# Get and set parameters
print(f"Action parameters: {action.getParameters()}
action.setParameters([1.0, 0.5, 0.0])

# Get action name
print(f"Action name: {action.getName()}")

# Get action type as enum value
if action.getType() == pynlm.ActionType.MoveForward:
    print("Action: Move forward")
elif action.getType() == pynlm.ActionType.TurnLeft:
    print("Action: Turn left")

# Clone action
action_clone = action.clone()

# Common action types and their meanings
action_types = {
    pynlm.ActionType.MoveForward: "Move forward",
    pynlm.ActionType.MoveBackward: "Move backward",
    pynlm.ActionType.MoveLeft: "Move left",
    pynlm.ActionType.MoveRight: "Move right",
    pynlm.ActionType.TurnLeft: "Turn left",
    pynlm.ActionType.TurnRight: "Turn right",
    pynlm.ActionType.Look: "Look",
    pynlm.ActionType.LookUp: "Look up",
    pynlm.ActionType.LookDown: "Look down",
    pynlm.ActionType.Interact: "Interact with object",
    pynlm.ActionType.Eat: "Eat food",
    pynlm.ActionType.Drink: "Drink water",
    pynlm.ActionType.Rest: "Rest",
    pynlm.ActionType.Wait: "Wait",
    pynlm.ActionType.Custom: "Custom action"
}

for action_type, description in action_types.items():
    test_action = pynlm.Action(action_type)
    print(f"{action_type}: {description}")
```

### New Utility Functions and Convenience Methods

#### Mathematical Functions

Enhanced mathematical utilities for NLM simulations.

```python
import pynlm

# Clamp a value between min and max
clamped = pynlm.clamp(1.5, 0.0, 1.0)
print(f"Clamped value: {clamped}")

# Linear interpolation between two values
interpolated = pynlm.lerp(0.0, 10.0, 0.5)
print(f"Linear interpolation: {interpolated}")

# Ease-in-out interpolation (smooth animation)
eased = pynlm.ease_in_out(0.5)
print(f"Ease-in-out at 0.5: {eased}")

# Create default configuration
config = pynlm.create_config()
print(f"Created config with {len(config.getKeys())} default keys")
```

#### Factory Functions

Convenience factory functions for creating NLM components.

```python
import pynlm

# Create brain with default configuration
brain = pynlm.create_brain()

# Create brain with specific configuration
config = pynlm.Config()
config.set("neuron_count", 500)
brain = pynlm.create_brain(config)

# Create world
world = pynlm.create_simple_world()

# Create agent brain
agent_brain = pynlm.create_agent_brain(brain)

# Get invalid ID sentinels
invalid_neuron = pynlm.INVALID_NEURON_ID
invalid_synapse = pynlm.INVALID_SYNAPSE_ID
invalid_region = pynlm.INVALID_REGION_ID
invalid_population = pynlm.INVALID_POPULATION_ID
```

## Best Practices

### 1. Error Handling

Always use try-except blocks for operations that can raise exceptions:

```python
try:
    brain.step(step)
except pynlm.BrainError as e:
    print(f"Brain simulation error: {e}")
    # Handle error (retry, fallback, etc.)
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
    # Fix validation issues
```

### 2. Configuration Management

Use configuration files for different environments:

```python
# Development configuration
dev_config = pynlm.Config()
dev_config.set("neuron_count", 100)
dev_config.set("simulation_timestep", 0.001)

# Production configuration
prod_config = pynlm.Config()
prod_config.loadFromFile("prod_config.json")

# Use appropriate configuration based on environment
if is_production:
    brain = pynlm.create_brain(prod_config)
else:
    brain = pynlm.create_brain(dev_config)
```

### 3. State Management

Save and load brain state for resuming simulations:

```python
# Save checkpoint
save_success = brain.save("checkpoint.json")
if save_success:
    print("Brain state saved successfully")

# Load checkpoint
load_success = brain.load("checkpoint.json")
if load_success:
    print("Brain state loaded successfully")
```

### 4. Performance Optimization

Use appropriate timestep and batch operations:

```python
# Use smaller timestep for stability, larger for performance
config.set("simulation_timestep", 0.001)

# Batch sensory inputs
sensory_inputs = [get_vision(), get_audio(), get_internal()]
for input_data in sensory_inputs:
    brain.receiveSensoryInput(input_data)
```

### 5. Development Debugging

Use logging and status methods:

```python
# Enable verbose logging
brain.logStatus()

# Check brain health
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Spike count: {brain.getTotalSpikeCount()}")
print(f"Memory usage: {brain.getTotalSynapseCount()}")
```

## Migration Guide

### Breaking Changes Summary

The improved Python bindings maintain backward compatibility while adding new features:

1. **Exception Hierarchy**: New exception types added (ConfigError, ValidationError, etc.)
2. **Enhanced Validation**: Configuration and parameter validation now throws exceptions
3. **New Convenience Methods**: Factory functions and utility methods added
4. **Improved Error Messages**: More descriptive error messages with context

### Migration Steps

#### 1. Update Error Handling

**Before**: Generic error handling
```python
try:
    config.loadFromFile("config.json")
except Exception as e:
    print(f"Error: {e}")
```

**After**: Specific exception handling
```python
try:
    config.loadFromFile("config.json")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
```

#### 2. Update Configuration Access

**Before**: Basic configuration access
```python
if config.has("random_seed"):
    seed = config.get("random_seed")
else:
    seed = 42
```

**After**: Improved configuration access with better defaults
```python
seed = config.get_or("random_seed", 42)
```

#### 3. Add New Optional Features

**Before**: Basic functionality
```python
brain = pynlm.create_brain(config)
brain.initialize()
```

**After**: With enhanced features and validation
```python
try:
    brain = pynlm.create_brain(config)
    if not brain.initialize():
        raise pynlm.CreationError("Failed to initialize brain")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.CreationError as e:
    print(f"Brain creation error: {e}")
```

#### 4. Update AgentBrain Usage

**Before**: Basic agent brain setup
```python
agent_brain = pynlm.AgentBrain(brain)
agent_brain.initialize(world)
```

**After**: With enhanced initialization and validation
```python
agent_brain = pynlm.create_agent_brain(brain)
agent_brain.initialize(world)

# Check system health
if not agent_brain.isDevelopmentEnabled():
    agent_brain.enableDevelopment(True)
```

## Examples

### 1. Simple Agent Learning

```python
import pynlm
import time

# Create agent
config = pynlm.create_config()
config.set("neuron_count", 200)
config.set("simulation_timestep", 0.001)

brain = pynlm.create_brain(config)
agent_brain = pynlm.create_agent_brain(brain)

world = pynlm.create_simple_world()
world.configure(100.0, 100.0, 8, 8)

# Place agent and resources
world.setAgentStart(50.0, 50.0)

# Add resources for the agent
for i in range(5):
    resource = pynlm.WorldObject(
        x=20.0 + i * 15.0,
        y=20.0 + i * 10.0,
        type=pynlm.WorldObjectType.Resource,
        value=10.0
    )
    world.addObject(resource)

agent_brain.initialize(world)

# Learning simulation
for episode in range(10):
    print(f"\n=== Episode {episode + 1} ===")
    
    # Reset for new episode
    agent_brain.reset()
    world.reset()
    
    # Run episode
    for step in range(200):
        # Get current perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Get motor command
        motor_cmd = agent_brain.decodeMotorCommand()
        
        # Apply to world
        result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(result.reward, 0.5)
        
        # Update development
        agent_brain.updateDevelopment(0.001)
        
        world.update(0.001)
    
    # Print episode summary
    agent = world.getAgentBody()
    print(f"Episode summary:")
    print(f"  Final position: ({agent.x:.1f}, {agent.y:.1f})")
    print(f"  Final energy: {agent.energy:.1f}")
    print(f"  Final health: {agent.health:.1f}")
    print(f"  Developmental stage: {agent_brain.getDevelopmentalStage()}")
    print(f"  Neuromodulation level: {agent_brain.getNeuromodulationLevel():.2f}")
    print(f"  Curiosity level: {agent_brain.getCuriosityLevel():.2f}")

print("\nLearning complete!")
```

### 2. Brain Checkpoint and Resume

```python
import pynlm

# Create brain
config = pynlm.create_config()
config.set("neuron_count", 500)

brain = pynlm.create_brain(config)
brain.initialize()

# Run some simulation
for step in range(100):
    brain.step(step)

# Save checkpoint
if brain.save("my_brain_checkpoint.json"):
    print("Brain checkpoint saved successfully")
    
    # Later... load checkpoint
    brain.reset()  # Clear current state
    if brain.load("my_brain_checkpoint.json"):
        print("Brain checkpoint loaded successfully")
        print("Brain is ready for continued learning")
    else:
        print("Failed to load checkpoint")
else:
    print("Failed to save checkpoint")
```

### 3. Complex Sensory Processing

```python
import pynlm
import numpy as np

# Create brain
brain = pynlm.create_brain()
brain.initialize()

# Create world
world = pynlm.create_simple_world()
world.configure(100.0, 100.0, 16, 16)  # High resolution vision
world.setAgentStart(50.0, 50.0)

# Create agent brain
agent_brain = pynlm.create_agent_brain(brain)
agent_brain.initialize(world)

# Create a simple object detector using internal signals
def detect_objects(percept):
    vision = percept.getVision()
    if vision.getDimensions() == 0:
        return []
    
    # Simple center-surround detection (placeholder for real implementation)
    data = vision.getData()
    if len(data) == 0:
        return []
    
    # Process vision data (example implementation)
    # In real implementation, this would use the brain to process vision
    return []

# Simulation loop with complex processing
for step in range(500):
    # Get sensory perception
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    
    # Get motor command
    motor_cmd = agent_brain.decodeMotorCommand()
    
    # Apply to world
    result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Complex reward calculation
    reward = calculate_complex_reward(result, percept, agent_brain)
    agent_brain.applyRewardModulation(reward, agent_brain.getPredictionError())
    
    # Update development
    agent_brain.updateDevelopment(0.001)
    
    world.update(0.001)

def calculate_complex_reward(action_result, percept, agent_brain):
    """Calculate complex reward based on multiple factors"""
    reward = action_result.reward
    
    # Add novelty-based curiosity reward
    novelty_reward = agent_brain.getNoveltyLevel() * 0.1
    reward += novelty_reward
    
    # Add prediction error reward (for learning)
    pred_error_reward = -abs(agent_brain.getPredictionError()) * 0.05
    reward += pred_error_reward
    
    # Add developmental stage reward
    dev_stage = agent_brain.getDevelopmentalStage()
    dev_reward = float(dev_stage) * 0.01  # Higher stage = higher potential reward
    reward += dev_reward
    
    return reward
```

## Troubleshooting

### 1. "Failed to initialize brain" Error

**Cause**: Configuration validation errors or invalid parameters.

**Solution**: Check configuration values:

```python
try:
    brain = pynlm.create_brain(config)
    if not brain.initialize():
        print("Brain initialization failed")
        # Check configuration issues
        print(f"Neuron count: {config.get_or('neuron_count', 'not set')}")
        print(f"Region count: {config.get_or('region_count', 'not set')}")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
```

### 2. "Invalid parameters" Error

**Cause**: Out-of-range parameter values.

**Solution**: Validate configuration before use:

```python
def validate_config(config):
    errors = []
    
    # Check neuron count
    neuron_count = config.get_or("neuron_count", 0)
    if neuron_count < 10 or neuron_count > 10000:
        errors.append(f"Invalid neuron_count: {neuron_count} (valid range: 10-10000)")
    
    # Check timestep
    timestep = config.get_or("simulation_timestep", 0.0)
    if timestep <= 0 or timestep > 1.0:
        errors.append(f"Invalid simulation_timestep: {timestep} (valid range: 0-1.0)")
    
    # Check connection probability
    prob = config.get_or("connection_probability", 0.0)
    if prob < 0 or prob > 1:
        errors.append(f"Invalid connection_probability: {prob} (valid range: 0-1)")
    
    return errors

# Validate configuration
validation_errors = validate_config(config)
if validation_errors:
    for error in validation_errors:
        print(f"Validation error: {error}")
else:
    print("Configuration is valid")
```

### 3. Performance Issues

**Cause**: Small timestep causing many simulation steps, or large network size.

**Solution**: Optimize simulation parameters:

```python
# For better performance, increase timestep
config.set("simulation_timestep", 0.01)  # 10x fewer steps

# For smaller simulations, reduce neuron count
config.set("neuron_count", 100)  # Smaller network

# Use checkpointing for long simulations
brain.save("checkpoint.json")
```

### 4. Memory Issues

**Cause**: Large brain state consuming excessive memory.

**Solution**: Use checkpointing and smaller configurations:

```python
# Save brain state periodically to free memory
if step % 100 == 0:
    brain.save(f"checkpoint_{step}.json")
    # Optionally clear memory-intensive components
    # brain.clearSomeCaches()  # If available

# Use smaller configuration for testing
config.set("neuron_count", 100)
```

### 5. Sensory Input Errors

**Cause**: Invalid sensory data or processing errors.

**Solution**: Validate sensory input before processing:

```python
# Validate vision input
vision = get_vision_input()
if vision.getDimensions() > 0:
    if vision.getWidth() > 0 and vision.getHeight() > 0:
        agent_brain.processSensoryInput(percept)
    else:
        print("Warning: Invalid vision dimensions")
else:
    print("Warning: No vision data")
```

### 6. Motor Command Errors

**Cause**: Invalid motor commands or decoding errors.

**Solution**: Validate motor commands:

```python
# Validate motor command
try:
    motor_cmd = agent_brain.decodeMotorCommand()
    
    # Check if motor command is valid
    if motor_cmd is None or motor_cmd == pynlm.MotorCommand.Wait:
        print("Warning: Motor command is wait/idle")
    else:
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
except pynlm.MotorError as e:
    print(f"Motor command error: {e}")
    # Fallback to wait command
    world.applyMotorCommand(pynlm.MotorCommand.Wait, world.getSimulationTime())
```

## Conclusion

The NLM Python bindings provide a comprehensive interface to the NLM neural simulation framework with enhanced error handling, validation, and convenience functions. This documentation covers all the improved features and provides guidance for getting the most out of the library.

Key improvements in the latest version:

- Comprehensive exception hierarchy for better error handling
- Enhanced configuration validation
- Convenience factory functions
- Improved mathematical utilities
- Better integration between brain, agent, and world components

The bindings are designed to be both beginner-friendly and powerful enough for advanced research and development work.
