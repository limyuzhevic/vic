# NLM API Documentation
# Version 0.1.0

This is the complete API documentation for the NLM (Neural Learning Machine) framework. It provides comprehensive reference documentation for all classes, methods, and functionality available in the NLM Python bindings.

## Table of Contents

1. [Introduction](#introduction)
2. [Core Classes](#core-classes)
3. [Configuration Management](#configuration-management)
4. [Brain System](#brain-system)
5. [Agent System](#agent-system)
6. [World System](#world-system)
7. [Sensory and Motor Systems](#sensory-and-motor-systems)
8. [Memory Systems](#memory-systems)
9. [Neuromodulation](#neuromodulation)
10. [Development](#development)
11. [Plasticity](#plasticity)
12. [Prediction System](#prediction-system)
13. [Cognition](#cognition)
14. [Visualization and Debugging](#visualization-and-debugging)
15. [Performance and Profiling](#performance-and-profiling)
16. [Export and Analysis](#export-and-analysis)
17. [Advanced Topics](#advanced-topics)
18. [Examples](#examples)
19. [Troubleshooting](#troubleshooting)
20. [Migration Guide](#migration-guide)

---

# Introduction

NLM (Neural Learning Machine) is a biologically inspired spiking neural network simulator that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, multiple plasticity mechanisms, and integrated memory, prediction, cognition, and neuromodulation systems.

## Key Features

- **Biologically Plausible Neuroscience**: LIF neurons with refractory periods, synaptic delays, and realistic membrane dynamics
- **Event-Driven Architecture**: Efficient spike propagation and processing
- **Multiple Learning Mechanisms**: STDP, Hebbian learning, structural plasticity, reward-modulated learning
- **Integrated Memory Systems**: Working memory, episodic memory, semantic memory, procedural memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error signals
- **Prediction System**: Forward models and prediction error signaling
- **Cognition**: Planning, concept formation, attention, decision making
- **Development**: Synaptogenesis, pruning, maturation stages
- **Agent-World Interaction**: Embodied cognition with sensory perception and motor output
- **Performance Optimizations**: SIMD vectorization, parallel processing, memory pools
- **Checkpointing and Replay**: State persistence and memory consolidation
- **Comprehensive Testing**: Unit tests, integration tests, and performance benchmarks

## Architecture Overview

### Core Components

1. **Brain**: Central neural simulation engine managing neurons, synapses, and all integrated systems
2. **Agent**: Interface connecting brain to world, handling sensory transduction and motor decoding
3. **World**: 2D simulation environment with objects, physics, and sensory input
4. **Memory Systems**: Multiple memory architectures for different types of information storage
5. **Plasticity Systems**: Learning rules for synaptic weight modification
6. **Neuromodulation**: Chemical signals regulating neural excitability and plasticity

### Data Flow

```
World (Environment) → Sensory Input → Brain → Motor Output → World
         ↑                                    ↓
   Sensory Perception ←- Motor Command ←- Action Selection
```

### Key Concepts

- **Neurons**: LIF (Leaky Integrate-and-Fire) units with spiking dynamics
- **Synapses**: Connections with weights, delays, and plasticity properties
- **Populations**: Groups of neurons with shared characteristics
- **Regions**: Anatomical divisions of the brain
- **Neuronal States**: Membrane potential, firing rate, adaptation variables
- **Spike Events**: Temporal signals representing neuronal activity

---

# Core Classes

## Config
Configuration management for NLM system.

### Attributes

- `keys`: List of all configuration keys
- `values`: Dictionary of all configuration values

### Methods

#### loadFromFile(filepath)
```python
def loadFromFile(self, filepath: str) -> bool:
    """Load configuration from a JSON file.
    
    Args:
        filepath: Path to JSON configuration file
        
    Returns:
        True if successful, False if file not found or invalid
    """
```

#### loadFromArgs(argc, argv)
```python
def loadFromArgs(self, argc: int, argv: list) -> bool:
    """Load configuration from command line arguments.
    
    Args:
        argc: Argument count
        argv: Argument vector
        
    Returns:
        True if successful, False if invalid arguments
    """
```

#### saveToFile(filepath)
```python
def saveToFile(self, filepath: str) -> bool:
    """Save configuration to a JSON file.
    
    Args:
        filepath: Path where to save configuration
        
    Returns:
        True if successful, False if write failed
    """
```

#### has(key)
```python
def has(self, key: str) -> bool:
    """Check if a configuration key exists.
    
    Args:
        key: Configuration key to check
        
    Returns:
        True if key exists, False otherwise
    """
```

#### getKeys()
```python
def getKeys(self) -> list:
    """Get all configuration keys.
    
    Returns:
        List of all configuration keys
    """
```

#### clear()
```python
def clear(self) -> None:
    """Clear all configuration entries."""
```

#### summary()
```python
def summary(self) -> str:
    """Get a summary string of the configuration.
    
    Returns:
        Summary string describing current configuration
    """
```

#### __repr__()
```python
def __repr__(self) -> str:
    """String representation of configuration."""
```

### Usage Example

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query configuration
print(config.has("brain.neuron_count"))  # True
print(config.getKeys())  # ['brain.neuron_count', 'brain.region_count', ...]

# Set configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

## Brain
Central neural simulation brain class implementing complete artificial brain system.

### Attributes

- `total_neurons`: Total number of neurons in brain
- `total_synapses`: Total number of synapses in brain
- `regions`: List of all neural regions

### Methods

#### __init__(config)
```python
def __init__(self, config: Config) -> None:
    """Create brain with configuration.
    
    Args:
        config: Configuration object for brain parameters
    """
```

#### initialize()
```python
def initialize(self) -> bool:
    """Initialize the brain with configuration.
    
    Returns:
        True if initialization successful, False otherwise
    """
```

#### step(step_number)
```python
def step(self, step_number: int) -> None:
    """Perform a simulation step with given step number.
    
    Args:
        step_number: Current simulation step (used for timing)
    """
```

#### step(step_number, time)
```python
def step(self, step_number: int, time: float) -> None:
    """Perform a simulation step with timestamp.
    
    Args:
        step_number: Current simulation step
        time: Current simulation time (seconds)
    """
```

#### reset()
```python
def reset(self) -> None:
    """Reset brain state to initial conditions."""
```

#### receiveSensoryInput(input)
```python
def receiveSensoryInput(self, input: SensoryInput) -> None:
    """Inject sensory input into the brain.
    
    Args:
        input: Sensory input data (Vision, Audio, etc.)
    """
```

#### injectCurrent(neuron, current)
```python
def injectCurrent(self, neuron: NeuronId, current: float) -> None:
    """Inject current into a specific neuron.
    
    Args:
        neuron: ID of neuron to receive current
        current: Current amount (in nA or appropriate units)
    """
```

#### injectCurrentToNeurons(type, current)
```python
def injectCurrentToNeurons(self, type: NeuronType, current: float) -> None:
    """Inject current into all neurons of specified type.
    
    Args:
        type: Neuron type (Sensory, Motor, Internal, etc.)
        current: Current amount
    """
```

#### produceAction()
```python
def produceAction(self) -> Action:
    """Produce motor action based on neural activity.
    
    Returns:
        Motor action based on current brain state
    """
```

#### save(filepath)
```python
def save(self, filepath: str) -> bool:
    """Save brain state to file.
    
    Args:
        filepath: Path where to save brain state
        
    Returns:
        True if successful, False otherwise
    """
```

#### load(filepath)
```python
def load(self, filepath: str) -> bool:
    """Load brain state from file.
    
    Args:
        filepath: Path to load brain state from
        
    Returns:
        True if successful, False otherwise
    """
```

#### addRegion(name)
```python
def addRegion(self, name: str) -> RegionId:
    """Add a new neural region.
    
    Args:
        name: Name for the new region
        
    Returns:
        ID of newly created region
    """
```

#### getRegion(id)
```python
def getRegion(self, id: RegionId) -> NeuralRegion:
    """Get a region by ID.
    
    Args:
        id: Region ID to retrieve
        
    Returns:
        NeuralRegion object or None if not found
    """
```

#### getRegionCount()
```python
def getRegionCount(self) -> int:
    """Get the number of regions.
    
    Returns:
        Number of regions in brain
    """
```

#### getRegionIds()
```python
def getRegionIds(self) -> list:
    """Get all region IDs.
    
    Returns:
        List of all region IDs
    """
```

#### getRegions()
```python
def getRegions(self) -> list:
    """Get all regions.
    
    Returns:
        List of all NeuralRegion objects
    """
```

#### getTotalNeuronCount()
```python
def getTotalNeuronCount(self) -> int:
    """Get total neuron count across all regions.
    
    Returns:
        Total number of neurons
    """
```

#### getTotalSynapseCount()
```python
def getTotalSynapseCount(self) -> int:
    """Get total synapse count across all regions.
    
    Returns:
        Total number of synapses
    """
```

#### getActiveNeuronCount()
```python
def getActiveNeuronCount(self) -> int:
    """Get count of active neurons.
    
    Returns:
        Number of neurons with membrane potential > threshold
    """
```

#### getFiringNeuronCount()
```python
def getFiringNeuronCount(self) -> int:
    """Get count of currently firing neurons.
    
    Returns:
        Number of neurons in Active firing state
    """
```

#### getAverageFiringRate()
```python
def getAverageFiringRate(self) -> float:
    """Get average firing rate across all neurons.
    
    Returns:
        Average firing rate (Hz)
    """
```

#### getExcitationInhibitionRatio()
```python
def getExcitationInhibitionRatio(self) -> float:
    """Get excitation/inhibition balance ratio.
    
    Returns:
        Ratio of excitatory to inhibitory synaptic weights
    """
```

#### getTotalSpikeCount()
```python
def getTotalSpikeCount(self) -> int:
    """Get total spike count.
    
    Returns:
        Total number of spikes emitted by all neurons
    """
```

#### getDevelopmentalStage()
```python
def getDevelopmentalStage(self) -> DevelopmentalStage:
    """Get current developmental stage.
    
    Returns:
        Current developmental stage
    """
```

#### setDevelopmentalStage(stage)
```python
def setDevelopmentalStage(self, stage: DevelopmentalStage) -> None:
    """Set developmental stage.
    
    Args:
        stage: New developmental stage
    """
```

#### getConfig()
```python
def getConfig(self) -> Config:
    """Get the configuration.
    
    Returns:
        Configuration object used to create brain
    """
```

#### logStatus()
```python
def logStatus(self) -> None:
    """Log brain status information."""
```

### Usage Example

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Has {brain.getRegionCount()} regions")

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Save brain state
brain.save("brain_state.bin")

# Load brain state
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("brain_state.bin")
```

## AgentBrain
Connects NLM brain to the world, handling sensory transduction and motor decoding.

### Attributes

- `brain`: Underlying brain object
- `world`: World object for interaction
- `sensory_input_size`: Size of expected sensory input
- `motor_output_size`: Size of expected motor output

### Methods

#### __init__(brain)
```python
def __init__(self, brain: Brain) -> None:
    """Create agent brain interface.
    
    Args:
        brain: Underlying NLM brain object
    """
```

#### initialize(world)
```python
def initialize(self, world: SimpleWorld) -> None:
    """Initialize with world.
    
    Args:
        world: World object to interact with
    """
```

#### getSensoryInputSize()
```python
def getSensoryInputSize(self) -> int:
    """Get expected sensory input size.
    
    Returns:
        Number of sensory input features expected
    """
```

#### getMotorOutputSize()
```python
def getMotorOutputSize(self) -> int:
    """Get expected motor output size.
    
    Returns:
        Number of motor output commands available
    """
```

#### processSensoryInput(percept)
```python
def processSensoryInput(self, percept: SensoryPercept) -> None:
    """Process sensory percept and inject into brain.
    
    Args:
        percept: Sensory data from world (vision, touch, etc.)
    """
```

#### decodeMotorCommand()
```python
def decodeMotorCommand(self) -> MotorCommand:
    """Decode brain motor activity into motor command.
    
    Returns:
        Motor command based on current neural activity
    """
```

#### applyRewardModulation(reward, predicted_reward)
```python
def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
    """Apply neuromodulation based on reward.
    
    Args:
        reward: Actual reward received
        predicted_reward: Expected reward
    """
```

#### updateDevelopment(timestep)
```python
def updateDevelopment(self, timestep: float) -> None:
    """Update development system.
    
    Args:
        timestep: Time step duration
    """
```

#### getDevelopmentalStage()
```python
def getDevelopmentalStage(self) -> DevelopmentalStage:
    """Get current developmental stage.
    
    Returns:
        Current developmental stage
    """
```

#### getNeuromodulationLevel()
```python
def getNeuromodulationLevel(self) -> float:
    """Get current neuromodulation level.
    
    Returns:
        Current neuromodulation signal level
    """
```

#### getCuriosityLevel()
```python
def getCuriosityLevel(self) -> float:
    """Get curiosity level.
    
    Returns:
        Current curiosity signal level
    """
```

#### getNoveltyLevel()
```python
def getNoveltyLevel(self) -> float:
    """Get novelty level.
    
    Returns:
        Current novelty detection signal level
    """
```

#### getPredictionError()
```python
def getPredictionError(self) -> float:
    """Get prediction error.
    
    Returns:
        Current prediction error signal
    """
```

#### reset()
```python
def reset(self) -> None:
    """Reset agent for new episode."""
```

#### getBrain()
```python
def getBrain(self) -> Brain:
    """Get the underlying brain.
    
    Returns:
        Brain object
    """
```

#### enableRewardModulation(enable)
```python
def enableRewardModulation(self, enable: bool) -> None:
    """Enable or disable reward modulation.
    
    Args:
        enable: Whether to enable reward modulation
    """
```

#### enableStructuralPlasticity(enable)
```python
def enableStructuralPlasticity(self, enable: bool) -> None:
    """Enable or disable structural plasticity.
    
    Args:
        enable: Whether to enable structural plasticity
    """
```

#### enableDevelopment(enable)
```python
def enableDevelopment(self, enable: bool) -> None:
    """Enable or disable development.
    
    Args:
        enable: Whether to enable development
    """
```

#### enableCuriosity(enable)
```python
def enableCuriosity(self, enable: bool) -> None:
    """Enable or disable curiosity.
    
    Args:
        enable: Whether to enable curiosity-driven exploration
    """
```

#### isRewardModulationEnabled()
```python
def isRewardModulationEnabled(self) -> bool:
    """Check if reward modulation is enabled.
    
    Returns:
        True if reward modulation enabled
    """
```

#### isStructuralPlasticityEnabled()
```python
def isStructuralPlasticityEnabled(self) -> bool:
    """Check if structural plasticity is enabled.
    
    Returns:
        True if structural plasticity enabled
    """
```

#### isDevelopmentEnabled()
```python
def isDevelopmentEnabled(self) -> bool:
    """Check if development is enabled.
    
    Returns:
        True if development enabled
    """
```

#### isCuriosityEnabled()
```python
def isCuriosityEnabled(self) -> bool:
    """Check if curiosity is enabled.
    
    Returns:
        True if curiosity enabled
    """
```

### Usage Example

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    agent.applyRewardModulation(0.5, 0.3)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## SimpleWorld
Simple 2D world for NLM simulation.

### Attributes

- `width`: World width in meters
- `height`: World height in meters
- `vision_width`: Vision sensor width
- `vision_height`: Vision sensor height

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty world."""
```

#### configure(width, height, visionWidth, visionHeight)
```python
def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> None:
    """Configure world dimensions and vision.
    
    Args:
        width: World width in meters
        height: World height in meters
        visionWidth: Vision sensor width in pixels
        visionHeight: Vision sensor height in pixels
    """
```

#### reset()
```python
def reset(self) -> None:
    """Reset world to initial state."""
```

#### setAgentStart(x, y)
```python
def setAgentStart(self, x: float, y: float) -> None:
    """Set agent starting position.
    
    Args:
        x: Starting x coordinate
        y: Starting y coordinate
    """
```

#### update(timestep)
```python
def update(self, timestep: float) -> None:
    """Update world by timestep seconds.
    
    Args:
        timestep: Time duration to update
    """
```

#### applyMotorCommand(cmd, currentTime)
```python
def applyMotorCommand(self, cmd: MotorCommand, currentTime: float) -> None:
    """Apply motor command to world.
    
    Args:
        cmd: Motor command to execute
        currentTime: Current simulation time
    """
```

#### getSensoryPercept()
```python
def getSensoryPercept(self) -> SensoryPercept:
    """Get sensory percept from world.
    
    Returns:
        Sensory percept data including vision, touch, etc.
    """
```

#### getAgentBody()
```python
def getAgentBody(self) -> AgentBody:
    """Get agent body state.
    
    Returns:
        AgentBody object with position, velocity, etc.
    """
```

#### addObject(obj)
```python
def addObject(self, obj: WorldObject) -> None:
    """Add world object.
    
    Args:
        obj: WorldObject to add
    """
```

#### removeObject(x, y)
```python
def removeObject(self, x: float, y: float) -> None:
    """Remove object at position.
    
    Args:
        x: X coordinate
        y: Y coordinate
    """
```

#### isValidPosition(x, y)
```python
def isValidPosition(self, x: float, y: float) -> bool:
    """Check if position is valid.
    
    Args:
        x: X coordinate
        y: Y coordinate
        
    Returns:
        True if position is valid (within bounds and not occupied)
    """
```

#### getWidth()
```python
def getWidth(self) -> int:
    """Get world width.
    
    Returns:
        World width in meters
    """
```

#### getHeight()
```python
def getHeight(self) -> int:
    """Get world height.
    
    Returns:
        World height in meters
    """
```

#### getSimulationTime()
```python
def getSimulationTime(self) -> float:
    """Get current simulation time.
    
    Returns:
        Current simulation time in seconds
    """
```

#### setRandomSeed(seed)
```python
def setRandomSeed(self, seed: int) -> None:
    """Set random seed.
    
    Args:
        seed: Random seed for reproducible simulations
    """
```

#### getRandomSeed()
```python
def getRandomSeed(self) -> int:
    """Get random seed.
    
    Returns:
        Current random seed
    """
```

### Usage Example

```python
import pynlm

# Create simple world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Reset world
world.reset()

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Check valid position
print(world.isValidPosition(5.0, 5.0))  # True
print(world.isValidPosition(25.0, 5.0))  # False

# Update world
world.update(0.1)

# Get agent position
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")

# Get sensory input
percept = world.getSensoryPercept()
vision = percept.getVision()
print(f"Vision data shape: {len(vision)} pixels")
```

## Vision
Vision sensory input class.

### Attributes

- `width`: Image width in pixels
- `height`: Image height in pixels
- `channels`: Number of color channels

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty vision object."""
```

#### __init__(width, height, channels)
```python
def __init__(self, width: int, height: int, channels: int = 3) -> None:
    """Create vision object.
    
    Args:
        width: Image width in pixels
        height: Image height in pixels
        channels: Number of color channels (default: 3)
    """
```

#### setData(data)
```python
def setData(self, data: list) -> None:
    """Set vision data.
    
    Args:
        data: List of pixel values (flattened)
    """
```

#### getWidth()
```python
def getWidth(self) -> int:
    """Get vision width.
    
    Returns:
        Image width in pixels
    """
```

#### getHeight()
```python
def getHeight(self) -> int:
    """Get vision height.
    
    Returns:
        Image height in pixels
    """
```

#### getChannels()
```python
def getChannels(self) -> int:
    """Get number of channels.
    
    Returns:
        Number of color channels
    """
```

### Usage Example

```python
import pynlm

# Create vision object
vision = pynlm.Vision(8, 8, 3)  # 8x8 RGB image

# Set vision data
import random
vision_data = [random.random() for _ in range(8 * 8 * 3)]
vision.setData(vision_data)

print(f"Vision shape: {vision.getWidth()} x {vision.getHeight()} x {vision.getChannels()}")
```

## Audio
Audio sensory input class.

### Attributes

- `sample_rate`: Audio sample rate in Hz
- `num_samples`: Number of audio samples

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty audio object."""
```

#### __init__(sample_rate, num_samples)
```python
def __init__(self, sample_rate: int, num_samples: int) -> None:
    """Create audio object.
    
    Args:
        sample_rate: Audio sample rate (Hz)
        num_samples: Number of audio samples
    """
```

#### setData(data)
```python
def setData(self, data: list) -> None:
    """Set audio data.
    
    Args:
        data: List of audio sample values
    """
```

#### setSampleRate(sample_rate)
```python
def setSampleRate(self, sample_rate: int) -> None:
    """Set audio sample rate.
    
    Args:
        sample_rate: New sample rate (Hz)
    """
```

#### getSampleRate()
```python
def getSampleRate(self) -> int:
    """Get audio sample rate.
    
    Returns:
        Current sample rate (Hz)
    """
```

#### getNumSamples()
```python
def getNumSamples(self) -> int:
    """Get number of audio samples.
    
    Returns:
        Number of audio samples
    """
```

### Usage Example

```python
import pynlm

# Create audio object
audio = pynlm.Audio(44100, 1024)  # 44.1 kHz, 1024 samples

# Set audio data
audio_data = [0.5 * (i % 100) / 100.0 for i in range(1024)]
audio.setData(audio_data)

print(f"Audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
```

## InternalSignals
Internal signals sensory input class.

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty internal signals object."""
```

#### addSignal(value)
```python
def addSignal(self, value: float) -> None:
    """Add internal signal value.
    
    Args:
        value: Signal value (-1 to 1 range typical)
    """
```

#### clearSignals()
```python
def clearSignals(self) -> None:
    """Clear all signals."""
```

### Usage Example

```python
import pynlm

# Create internal signals
internal = pynlm.InternalSignals()

# Add signal values
internal.addSignal(0.5)  # Hopefulness
internal.addSignal(-0.3)  # Fear
internal.addSignal(0.9)  # Curiosity

print("Internal signals collected")
```

## Action
Motor action representation for motor output.

### Attributes

- `type`: Action type
- `parameters`: Action parameters

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty action."""
```

#### __init__(type)
```python
def __init__(self, type: ActionType) -> None:
    """Create action with type.
    
    Args:
        type: Action type (MoveForward, TurnLeft, etc.)
    """
```

#### __init__(type, parameters)
```python
def __init__(self, type: ActionType, parameters: list) -> None:
    """Create action with type and parameters.
    
    Args:
        type: Action type
        parameters: Action parameters (speed, angle, etc.)
    """
```

#### getType()
```python
def getType(self) -> ActionType:
    """Get action type.
    
    Returns:
        Action type
    """
```

#### setType(type)
```python
def setType(self, type: ActionType) -> None:
    """Set action type.
    
    Args:
        type: New action type
    """
```

#### getParameters()
```python
def getParameters(self) -> list:
    """Get action parameters.
    
    Returns:
        List of action parameters
    """
```

#### setParameters(params)
```python
def setParameters(self, params: list) -> None:
    """Set action parameters.
    
    Args:
        params: New action parameters
    """
```

#### getName()
```python
def getName(self) -> str:
    """Get action name.
    
    Returns:
        Human-readable action name
    """
```

#### clone()
```python
def clone(self) -> Action:
    """Create copy of action.
    
    Returns:
        Copy of this action
    """
```

### Usage Example

```python
import pynlm

# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.TurnLeft, [45.0])  # Turn 45 degrees

print(f"Action 1: {action1.getName()}")
print(f"Action 2: {action2.getName()}, angle: {action2.getParameters()[0]}")
```

## WorldObject
World object representation.

### Attributes

- `x`: X coordinate
- `y`: Y coordinate
- `radius`: Object radius
- `type`: Object type
- `value`: Object value (for resources)
- `active`: Object active status

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty world object."""
```

#### __init__(x, y, type, value, radius)
```python
def __init__(self, x: float, y: float, type: WorldObjectType, value: float = 0.0, radius: float = 0.5) -> None:
    """Create world object.
    
    Args:
        x: X coordinate
        y: Y coordinate
        type: Object type
        value: Object value (for resources)
        radius: Object radius
    """
```

### Usage Example

```python
import pynlm

# Create world objects
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
hazard = pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, value=5.0)
wall = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Wall)
```

## AgentBody
Agent body state for representing agent in world.

### Attributes

- `x`: X coordinate
- `y`: Y coordinate
- `orientation`: Orientation angle in degrees
- `velocityX`: X velocity
- `velocityY`: Y velocity
- `angularVelocity`: Angular velocity
- `energy`: Energy level
- `health`: Health level
- `age`: Age in simulation steps
- `isMoving`: Moving state
- `isTurning`: Turning state
- `lastActionTime`: Time since last action

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty agent body."""
```

#### reset()
```python
def reset(self) -> None:
    """Reset agent to initial state."""
```

### Usage Example

```python
import pynlm

# Create agent body
body = pynlm.AgentBody()

# Set initial state
body.x = 10.0
body.y = 10.0
body.orientation = 0.0
body.velocityX = 0.0
body.velocityY = 0.0
body.energy = 100.0
body.health = 100.0
body.age = 0

print(f"Agent at ({body.x}, {body.y}), orientation: {body.orientation}°")
```

## ActionResult
Action result from world interaction.

### Attributes

- `reward`: Reward value
- `success`: Whether action succeeded
- `message`: Optional message

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty action result."""
```

#### __init__(reward, success, message)
```python
def __init__(self, reward: float, success: bool, message: str = "") -> None:
    """Create action result.
    
    Args:
        reward: Reward value
        success: Whether action succeeded
        message: Optional message
    """
```

### Usage Example

```python
import pynlm

# Create action result
success = world.applyMotorCommand(action, time)
result = pynlm.ActionResult(reward=10.0, success=success, message="Moved forward")

print(f"Action result: {result.reward} reward, success: {result.success}")
```

## SensoryPercept
Sensory percept data combining all sensory modalities.

### Methods

#### __init__()
```python
def __init__(self) -> None:
    """Create empty sensory percept."""
```

#### getVision()
```python
def getVision(self) -> Vision:
    """Get vision data.
    
    Returns:
        Vision data object
    """
```

#### setVision(vision)
```python
def setVision(self, vision: Vision) -> None:
    """Set vision data.
    
    Args:
        vision: Vision data to set
    """
```

#### getVisionWidth()
```python
def getVisionWidth(self) -> int:
    """Get vision width.
    
    Returns:
        Vision width in pixels
    """
```

#### getVisionHeight()
```python
def getVisionHeight(self) -> int:
    """Get vision height.
    
    Returns:
        Vision height in pixels
    """
```

#### getTouch()
```python
def getTouch(self) -> list:
    """Get touch data.
    
    Returns:
        Touch data (collisions with objects)
    """
```

#### setTouch(touch)
```python
def setTouch(self, touch: list) -> None:
    """Set touch data.
    
    Args:
        touch: Touch data to set
    """
```

#### getInternal()
```python
def getInternal(self) -> list:
    """Get internal signals.
    
    Returns:
        Internal signal values
    """
```

#### setInternal(internal)
```python
def setInternal(self, internal: list) -> None:
    """Set internal signals.
    
    Args:
        internal: Internal signal values to set
    """
```

#### getProprioception()
```python
def getProprioception(self) -> list:
    """Get proprioception data.
    
    Returns:
        Proprioception data (body position feedback)
    """
```

#### setProprioception(proprioception)
```python
def setProprioception(self, proprioception: list) -> None:
    """Set proprioception data.
    
    Args:
        proprioception: Proprioception data to set
    """
```

#### getAudio()
```python
def getAudio(self) -> Audio:
    """Get audio data.
    
    Returns:
        Audio data object
    """
```

#### setAudio(audio)
```python
def setAudio(self, audio: Audio) -> None:
    """Set audio data.
    
    Args:
        audio: Audio data to set
    """
```

#### getAllSignals()
```python
def getAllSignals(self) -> list:
    """Get all sensory signals.
    
    Returns:
        Combined list of all sensory signals
    """
```

#### getTimestamp()
```python
def getTimestamp(self) -> float:
    """Get sensory timestamp.
    
    Returns:
        Timestamp of sensory data
    """
```

#### setTimestamp(timestamp)
```python
def setTimestamp(self, timestamp: float) -> None:
    """Set sensory timestamp.
    
    Args:
        timestamp: New timestamp
    """
```

### Usage Example

```python
import pynlm

# Create sensory percept
percept = pynlm.SensoryPercept()

# Set sensory data
vision = pynlm.Vision(8, 8, 3)
percept.setVision(vision)

internal = percept.getInternal()
internal.append(0.5)  # Hopefulness
percept.setInternal(internal)

# Get combined signals
all_signals = percept.getAllSignals()
print(f"Total sensory signals: {len(all_signals)}")
```

## Enumerations

### NeuronType
Neuron type enumeration:

- `Excitatory`: Excitatory neuron (positive current)
- `Inhibitory`: Inhibitory neuron (negative current)
- `Modulatory`: Modulatory neuron (neuromodulation)
- `Sensory`: Sensory neuron (receives external input)
- `Motor`: Motor neuron (produces output)
- `Internal`: Internal neuron (internal processing)

### SynapseType
Synapse type enumeration:

- `Excitatory`: Excitatory synapse (positive weight)
- `Inhibitory`: Inhibitory synapse (negative weight)
- `Modulatory`: Modulatory synapse (neuromodulation)
- `Electrical`: Electrical synapse (direct coupling)
- `GapJunction`: Gap junction (electrical coupling)

### DevelopmentalStage
Developmental stage enumeration:

- `Initial`: Initial development stage (high plasticity)
- `CriticalPeriod`: Critical period of plasticity (language, sensory)
- `Maturation`: Maturation stage (refining connections)
- `Adult`: Adult stage (stable performance)
- `Aging`: Aging stage (declining plasticity)

### ActionType
Action type enumeration:

- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `MoveLeft`: Move left
- `MoveRight`: Move right
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look straight
- `LookUp`: Look up
- `LookDown`: Look down
- `Interact`: Interact with object
- `Eat`: Eat food
- `Drink`: Drink water
- `Rest`: Rest (recharge energy)
- `Wait`: Wait (do nothing)
- `Custom`: Custom action

### MotorCommand
Motor command enumeration for low-level control:

- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `LookLeft`: Look left
- `LookRight`: Look right
- `Interact`: Interact with object
- `Wait`: Wait (do nothing)

### WorldObjectType
World object type enumeration:

- `Empty`: Empty space
- `Resource`: Valuable resource
- `Hazard`: Dangerous object
- `Wall`: Impassable wall
- `Marker`: Landmark/target

### FiringState
Neuron firing state enumeration:

- `Resting`: Resting state (no activity)
- `Active`: Active state (potential to fire)
- `Refractory`: Refractory state (just fired, cannot fire again)
- `Inhibited`: Inhibited state (actively prevented from firing)

## Factory Functions

### createDefaultConfig()
```python
def createDefaultConfig() -> Config:
    """Create a default configuration.
    
    Returns:
        Default configuration object with recommended settings
    """
```

### createBrain(config)
```python
def createBrain(config: Config) -> Brain:
    """Create a new brain with configuration.
    
    Args:
        config: Configuration for brain parameters
        
    Returns:
        New Brain object
    """
```

### createSimpleWorld()
```python
def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world.
    
    Returns:
        New SimpleWorld object
    """
```

### createAgentBrain(brain)
```python
def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface.
    
    Args:
        brain: Underlying brain object
        
    Returns:
        New AgentBrain object
    """
```

### Invalid ID Constants
```python
INVALID_NEURON_ID: NeuronId  # Invalid neuron identifier
INVALID_SYNAPSE_ID: SynapseId  # Invalid synapse identifier
INVALID_REGION_ID: RegionId    # Invalid region identifier
INVALID_POPULATION_ID: PopulationId  # Invalid population identifier
```

## Type Aliases

- `NeuronId`: Unique identifier for neurons
- `SynapseId`: Unique identifier for synapses
- `RegionId`: Unique identifier for regions
- `PopulationId`: Unique identifier for populations
- `SimulationStep`: Simulation step counter (uint64_t)
- `Timestamp`: Simulation time in seconds (double)
- `TimestepDuration`: Time step duration in seconds (double)
- `NeuronIndex`: Index for neuron arrays (uint64_t)
- `SynapseIndex`: Index for synapse arrays (uint64_t)
- `PopulationIndex`: Index for population arrays (uint64_t)
- `RegionIndex`: Index for region arrays (uint64_t)
- `SynapticWeight`: Synaptic weight type (float)
- `MembranePotential`: Membrane potential type (float)
- `FiringRate`: Firing rate type (float)
- `Delay`: Connection delay in simulation steps (uint32_t)

## Version Information

**Current Version:** 0.1.0

**Release Date:** October 2026

**License:** MIT

**Author:** NLM Research Team

**Contact:** nlm-project@nlm.org

---

*This API documentation is continuously updated.*
*For the latest information, visit the NLM documentation portal.*
"""

# Write to a temporary file first to avoid Git LFS issues with binary files
import os
import tempfile

# Create the file content
temp_file = tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.md')
temp_file.write(__doc__)
temp_file.close()

# Copy to the target location
os.makedirs('/workspace/app/docs', exist_ok=True)
os.replace(temp_file.name, '/workspace/app/docs/API_REFERENCE.md')

print("API reference documentation created successfully!")
