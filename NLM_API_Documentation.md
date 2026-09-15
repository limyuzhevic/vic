# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings.
The NLM is a brain-inspired spiking neural network simulator written in C++ with Python bindings
through pybind11, enabling Python developers to create artificial brains with learning capabilities.

## Module Structure

The main module is `pynlm` (import pynlm). It provides access to:

- **Core Classes**: Brain, Config, AgentBrain, SimpleWorld
- **Sensory Classes**: Vision, Audio, InternalSignals
- **Action Classes**: Action
- **Data Classes**: WorldObject, SensoryPercept, ActionResult
- **Type Enums**: NeuronType, SynapseType, ActionType, etc.
- **Factory Functions**: createBrain(), createSimpleWorld(), createAgentBrain(), createDefaultConfig()

## Core Classes

### Config

The `Config` class manages configuration for NLM systems using a key-value store with support for
JSON, YAML, command-line arguments, and runtime settings.

#### Constructor
```python
def __init__()
    """Create a new configuration object"""
```

#### Methods

**File I/O**
```python
def loadFromFile(self, filepath: str) -> bool
    """Load configuration from a file (auto-detects JSON/YAML format)
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        bool: True if successful, False otherwise
    """

def loadFromJSON(self, filepath: str) -> bool
    """Load configuration from a JSON file
    
    Args:
        filepath: Path to JSON configuration file
        
    Returns:
        bool: True if successful, False otherwise
    """

def loadFromYAML(self, filepath: str) -> bool
    """Load configuration from a YAML file
    
    Args:
        filepath: Path to YAML configuration file
        
    Returns:
        bool: True if successful, False otherwise
    """

def loadFromArgs(self, argc: int, argv: List[str]) -> bool
    """Load configuration from command line arguments
    
    Args:
        argc: Argument count
        argv: Argument list (including program name)
        
    Returns:
        bool: True if successful, False otherwise
    """

def saveToFile(self, filepath: str) -> bool
    """Save configuration to a file
    
    Args:
        filepath: Path to save configuration file
        
    Returns:
        bool: True if successful, False otherwise
    """
```

**Key/Value Operations**
```python
def has(self, key: str) -> bool
    """Check if a configuration key exists
    
    Args:
        key: Configuration key (dot notation supported, e.g., "brain.neuron_count")
        
    Returns:
        bool: True if key exists, False otherwise
    """

def getKeys(self) -> List[str]
    """Get all configuration keys
    
    Returns:
        List[str]: List of all configuration keys
    """

def get(self, key: str, default_value=None):
    """Get a configuration value
    
    Args:
        key: Configuration key
        default_value: Default value if key doesn't exist
        
    Returns:
        Configuration value or default_value if not found
    """

def getOr(self, key: str, default_value) -> Any
    """Get a configuration value with default
    
    Args:
        key: Configuration key
        default_value: Default value if key doesn't exist
        
    Returns:
        Configuration value or default_value
    """

def set(self, key: str, value, source=ConfigSource.Runtime)
    """Set a configuration value
    
    Args:
        key: Configuration key
        value: Configuration value
        source: Source of the configuration (Default, File, CommandLine, Runtime)
    """

def remove(self, key: str)
    """Remove a configuration key
    
    Args:
        key: Configuration key to remove
    """

def clear(self)
    """Clear all configuration entries"""

def summary(self) -> str
    """Get a summary string of the configuration
    
    Returns:
        str: Summary of all configuration entries
    """
```

### Brain

The `Brain` class represents a complete spiking neural network brain with neurons, synapses,
plasticity mechanisms, memory systems, and neuromodulation.

#### Constructor
```python
def __init__(self, config: Config) -> Brain
    """Create a new brain with configuration
    
    Args:
        config: Configuration for the brain
    """
```

#### Initialization
```python
def initialize(self) -> bool
    """Initialize the brain with configuration
    
    Returns:
        bool: True if successful, False otherwise
    """
```

#### Simulation Loop
```python
def step(self, currentStep: int) -> None
    """Perform a simulation step
    
    Args:
        currentStep: Current simulation step number
    """

def step(self, currentStep: int, currentTime: float) -> None
    """Perform a simulation step with timestamp
    
    Args:
        currentStep: Current simulation step number
        currentTime: Current simulation time in seconds
    """
```

#### Sensory Input
```python
def receiveSensoryInput(self, input: SensoryInput) -> None
    """Inject sensory input into the brain
    
    Args:
        input: Sensory input object containing perception data
    """

def injectCurrent(self, neuron: NeuronId, current: float) -> None
    """Inject current directly into a specific neuron
    
    Args:
        neuron: Neuron ID
        current: Current to inject (in nA)
    """

def injectCurrentToNeurons(self, type: NeuronType, current: float) -> None
    """Inject current into all neurons of a specific type
    
    Args:
        type: Neuron type to target
        current: Current to inject (in nA)
    """
```

#### Motor Output
```python
def produceAction(self) -> Action
    """Produce motor action based on neural activity
    
    Returns:
        Action: Motor action determined by brain activity
    """
```

#### State Management
```python
def reset(self) -> None
    """Reset brain state to initial conditions"""

def save(self, filepath: str) -> bool
    """Save brain state to file (checkpointing)
    
    Args:
        filepath: Path to save checkpoint file
        
    Returns:
        bool: True if successful, False otherwise
    """

def load(self, filepath: str) -> bool
    """Load brain state from file
    
    Args:
        filepath: Path to checkpoint file
        
    Returns:
        bool: True if successful, False otherwise
    """
```

#### Region Management
```python
def addRegion(self, name: str = "") -> RegionId
    """Add a new neural region
    
    Args:
        name: Optional name for the region
        
    Returns:
        RegionId: ID of the created region
    """

def getRegion(self, id: RegionId) -> NeuralRegion
    """Get a region by ID
    
    Args:
        id: Region ID
        
    Returns:
        NeuralRegion: Region object
    """
```

#### Statistics
```python
def getTotalNeuronCount(self) -> int
    """Get total neuron count across all regions
    
    Returns:
        int: Total number of neurons
    """

def getTotalSynapseCount(self) -> int
    """Get total synapse count across all regions
    
    Returns:
        int: Total number of synapses
    """

def getActiveNeuronCount(self) -> int
    """Get count of active neurons
    
    Returns:
        int: Number of active neurons
    """

def getFiringNeuronCount(self) -> int
    """Get count of currently firing neurons
    
    Returns:
        int: Number of firing neurons
    """

def getAverageFiringRate(self) -> float
    """Get average firing rate across all neurons
    
    Returns:
        float: Average firing rate (Hz)
    """

def getExcitationInhibitionRatio(self) -> float
    """Get excitation/inhibition balance ratio
    
    Returns:
        float: E/I ratio (higher = more excitation)
    """

def getTotalSpikeCount(self) -> int
    """Get total spike count
    
    Returns:
        int: Total number of spikes
    """

def getPendingSpikeEventCount(self) -> int
    """Get count of pending spike events
    
    Returns:
        int: Number of pending spike events
    """

def getDevelopmentalStage(self) -> DevelopmentalStage
    """Get current developmental stage
    
    Returns:
        DevelopmentalStage: Current developmental stage
    """
```

#### Neuromodulation
```python
def applyNeuromodulation(self, signal: Neuromodulator) -> None
    """Apply neuromodulatory signals
    
    Args:
        signal: Neuromodulator signal (dopamine, curiosity, novelty, etc.)
    """
```

#### Access to Subsystems
```python
def getSpikeSystem(self) -> SpikeSystem
    """Get the spike system
    
    Returns:
        SpikeSystem: Spike system object
    """

def getSTDP(self) -> STDP
    """Get the STDP plasticity system
    
    Returns:
        STDP: STDP plasticity system
    """

def getHebbian(self) -> Hebbian
    """Get the Hebbian plasticity system
    
    Returns:
        Hebbian: Hebbian plasticity system
    """

def getStructuralPlasticity(self) -> StructuralPlasticity
    """Get the structural plasticity system
    
    Returns:
        StructuralPlasticity: Structural plasticity system
    """
```

#### Memory Systems
```python
def getWorkingMemory(self) -> NeuralWorkingMemory
    """Get the working memory system
    
    Returns:
        NeuralWorkingMemory: Working memory system
    """

def getEpisodicMemory(self) -> NeuralEpisodicMemory
    """Get the episodic memory system
    
    Returns:
        NeuralEpisodicMemory: Episodic memory system
    """

def getAssociativeMemory(self) -> NeuralAssociativeMemory
    """Get the associative memory system
    
    Returns:
        NeuralAssociativeMemory: Associative memory system
    """
```

#### Prediction System
```python
def getPredictionSystem(self) -> PredictionSystem
    """Get the prediction system
    
    Returns:
        PredictionSystem: Prediction system
    """
```

#### Cognitive Systems
```python
def getPlanner(self) -> NeuralPlanner
    """Get the neural planner
    
    Returns:
        NeuralPlanner: Neural planning system
    """

def getConceptFormation(self) -> ConceptFormation
    """Get the concept formation system
    
    Returns:
        ConceptFormation: Concept formation system
    """

def getAttention(self) -> AttentionalSelection
    """Get the attentional selection system
    
    Returns:
        AttentionalSelection: Attention system
    """
```

#### Development System
```python
def getDevelopmentSystem(self) -> DevelopmentSystem
    """Get the development system
    
    Returns:
        DevelopmentSystem: Development system
    """

def setDevelopmentalStage(self, stage: DevelopmentalStage) -> None
    """Set developmental stage
    
    Args:
        stage: Developmental stage to set
    """
```

#### Neuromodulation Systems
```python
def getDopamine(self) -> Dopamine
    """Get the dopamine neuromodulator
    
    Returns:
        Dopamine: Dopamine system
    """

def getCuriosity(self) -> Curiosity
    """Get the curiosity neuromodulator
    
    Returns:
        Curiosity: Curiosity system
    """

def getNovelty(self) -> Novelty
    """Get the novelty detection system
    
    Returns:
        Novelty: Novelty system
    """

def getPredictionErrorSignal(self) -> PredictionError
    """Get the prediction error signal
    
    Returns:
        PredictionError: Prediction error system
    """
```

### AgentBrain

The `AgentBrain` class connects the NLM brain to the world, handling sensory transduction
and motor decoding.

#### Constructor
```python
def __init__(self, brain: Brain) -> AgentBrain
    """Create a new agent brain interface
    
    Args:
        brain: Underlying brain object
    """
```

#### Initialization
```python
def initialize(self, world: SimpleWorld) -> None
    """Initialize with world
    
    Args:
        world: World object for interaction
    """
```

#### Sensory Processing
```python
def getSensoryInputSize(self) -> int
    """Get expected sensory input size
    
    Returns:
        int: Expected size of sensory input vector
    """

def processSensoryInput(self, percept: SensoryPercept) -> None
    """Process sensory percept and inject into brain
    
    Args:
        percept: Sensory percept containing perception data
    """
```

#### Motor Decoding
```python
def decodeMotorCommand(self) -> MotorCommand
    """Decode brain motor activity into motor command
    
    Returns:
        MotorCommand: Motor command determined by brain activity
    """
```

#### Neuromodulation
```python
def applyRewardModulation(self, reward: float, predictedReward: float) -> None
    """Apply reward-based neuromodulation
    
    Args:
        reward: Actual reward received
        predictedReward: Expected reward
    """
```

#### Development
```python
def updateDevelopment(self, timestep: float) -> None
    """Update development system
    
    Args:
        timestep: Time step for development updates
    """

def getDevelopmentalStage(self) -> DevelopmentalStage
    """Get current developmental stage
    
    Returns:
        DevelopmentalStage: Current developmental stage
    """
```

#### Statistics
```python
def getNeuromodulationLevel(self) -> float
    """Get current neuromodulation level
    
    Returns:
        float: Neuromodulation level (0-1)
    """

def getCuriosityLevel(self) -> float
    """Get curiosity level
    
    Returns:
        float: Curiosity level (0-1)
    """

def getNoveltyLevel(self) -> float
    """Get novelty level
    
    Returns:
        float: Novelty level (0-1)
    """

def getPredictionError(self) -> float
    """Get prediction error
    
    Returns:
        float: Prediction error (0-1)
    """
```

#### Subsystem Control
```python
def enableRewardModulation(self, enable: bool) -> None
    """Enable/disable reward modulation
    
    Args:
        enable: Whether to enable reward modulation
    """

def enableStructuralPlasticity(self, enable: bool) -> None
    """Enable/disable structural plasticity
    
    Args:
        enable: Whether to enable structural plasticity
    """

def enableDevelopment(self, enable: bool) -> None
    """Enable/disable development
    
    Args:
        enable: Whether to enable development
    """

def enableCuriosity(self, enable: bool) -> None
    """Enable/disable curiosity
    
    Args:
        enable: Whether to enable curiosity
    """

def isRewardModulationEnabled(self) -> bool
    """Check if reward modulation is enabled
    
    Returns:
        bool: True if enabled, False otherwise
    """

def isStructuralPlasticityEnabled(self) -> bool
    """Check if structural plasticity is enabled
    
    Returns:
        bool: True if enabled, False otherwise
    """

def isDevelopmentEnabled(self) -> bool
    """Check if development is enabled
    
    Returns:
        bool: True if enabled, False otherwise
    """

def isCuriosityEnabled(self) -> bool
    """Check if curiosity is enabled
    
    Returns:
        bool: True if enabled, False otherwise
    """
```

### SimpleWorld

The `SimpleWorld` class represents a 2D simulation world for NLM agents.

#### Constructor
```python
def __init__(self) -> SimpleWorld
    """Create a new simple world"""
```

#### Configuration
```python
def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> None
    """Configure world dimensions
    
    Args:
        width: World width in meters
        height: World height in meters
        visionWidth: Agent vision width in cells
        visionHeight: Agent vision height in cells
    """
```

#### Control
```python
def reset(self) -> None
    """Reset world to initial state"""

def update(self, timestep: float) -> None
    """Update world by timestep
    
    Args:
        timestep: Time step to update (in seconds)
    """

def setRandomSeed(self, seed: int) -> None
    """Set random seed for reproducible simulations
    
    Args:
        seed: Random seed value
    """
```

#### Agent Interaction
```python
def setAgentStart(self, x: float, y: float) -> None
    """Set agent starting position
    
    Args:
        x: X coordinate
        y: Y coordinate
    """

def applyMotorCommand(self, cmd: MotorCommand, currentTime: float) -> None
    """Apply motor command to world
    
    Args:
        cmd: Motor command to execute
        currentTime: Current simulation time
    """
```

#### State Access
```python
def getSensoryPercept(self) -> SensoryPercept
    """Get current sensory percept
    
    Returns:
        SensoryPercept: Current sensory percept
    """

def getAgentBody(self) -> AgentBody
    """Get agent body state
    
    Returns:
        AgentBody: Current agent body state
    """
```

#### World Objects
```python
def addObject(self, obj: WorldObject) -> None
    """Add a world object
    
    Args:
        obj: World object to add
    """

def removeObject(self, x: float, y: float) -> None
    """Remove world object at position
    
    Args:
        x: X coordinate
        y: Y coordinate
    """

def isValidPosition(self, x: float, y: float) -> bool
    """Check if position is valid (not in wall)
    
    Args:
        x: X coordinate
        y: Y coordinate
        
    Returns:
        bool: True if position is valid, False otherwise
    """
```

#### Properties
```python
def getWidth(self) -> int
    """Get world width
    
    Returns:
        int: World width in meters
    """

def getHeight(self) -> int
    """Get world height
    
    Returns:
        int: World height in meters
    """

def getMaxEnergy(self) -> float
    """Get maximum agent energy
    
    Returns:
        float: Maximum energy
    """

def setMaxEnergy(self, e: float) -> None
    """Set maximum agent energy
    
    Args:
        e: Maximum energy value
    """

def getEnergyDecayRate(self) -> float
    """Get energy decay rate
    
    Returns:
        float: Energy decay rate per second
    """

def setEnergyDecayRate(self, r: float) -> None
    """Set energy decay rate
    
    Args:
        r: Energy decay rate per second
    """

def getSimulationTime(self) -> float
    """Get current simulation time
    
    Returns:
        float: Current simulation time
    """
```

### SensoryInput Classes

Base class for all sensory inputs:

```python
def getType(self) -> str
    """Get the type of sensory input
    
    Returns:
        str: Type string
    """

def getData(self) -> List[float]
    """Get the raw data as a vector
    
    Returns:
        List[float]: Raw sensory data
    """

def getDimensions(self) -> int
    """Get the dimensionality
    
    Returns:
        int: Number of dimensions
    """

def getTimestamp(self) -> float
    """Get the timestamp
    
    Returns:
        float: Timestamp
    """

def setTimestamp(self, timestamp: float) -> None
    """Set the timestamp
    
    Args:
        timestamp: Timestamp value
    """
```

#### Vision
```python
def __init__(self, width: int, height: int, channels: int = 3) -> Vision
    """Create vision sensory input
    
    Args:
        width: Vision width
        height: Vision height
        channels: Number of color channels (default: 3 for RGB)
    """

def setData(self, data: List[float]) -> None
    """Set vision data
    
    Args:
        data: Vision data as flattened array (width*height*channels)
    """

def getWidth(self) -> int
    """Get vision width
    
    Returns:
        int: Vision width
    """

def getHeight(self) -> int
    """Get vision height
    
    Returns:
        int: Vision height
    """

def getChannels(self) -> int
    """Get number of channels
    
    Returns:
        int: Number of channels
    """
```

#### Audio
```python
def __init__(self, sampleRate: int, numSamples: int) -> Audio
    """Create audio sensory input
    
    Args:
        sampleRate: Sample rate (Hz)
        numSamples: Number of samples
    """

def setData(self, data: List[float]) -> None
    """Set audio data
    
    Args:
        data: Audio samples
    """

def setSampleRate(self, sampleRate: int) -> None
    """Set sample rate
    
    Args:
        sampleRate: Sample rate (Hz)
    """

def getSampleRate(self) -> int
    """Get sample rate
    
    Returns:
        int: Sample rate (Hz)
    """

def getNumSamples(self) -> int
    """Get number of samples
    
    Returns:
        int: Number of samples
    """
```

#### InternalSignals
```python
def __init__(self) -> InternalSignals
    """Create internal signals sensory input"""

def addSignal(self, value: float) -> None
    """Add a signal value
    
    Args:
        value: Signal value
    """

def clearSignals(self) -> None
    """Clear all signals"""
```

### Action

Represents an action that can be executed in the world:

```python
def __init__(self, type: ActionType) -> Action
    """Create an action
    
    Args:
        type: Action type
    """

def __init__(self, type: ActionType, parameters: List[float]) -> Action
    """Create an action with parameters
    
    Args:
        type: Action type
        parameters: Action parameters
    """

def getType(self) -> ActionType
    """Get action type
    
    Returns:
        ActionType: Action type
    """

def setType(self, type: ActionType) -> None
    """Set action type
    
    Args:
        type: Action type
    """

def getParameters(self) -> List[float]
    """Get action parameters
    
    Returns:
        List[float]: Action parameters
    """

def setParameters(self, params: List[float]) -> None
    """Set action parameters
    
    Args:
        params: Action parameters
    """

def getName(self) -> str
    """Get action name
    
    Returns:
        str: Human-readable action name
    """

def clone(self) -> Action
    """Clone the action
    
    Returns:
        Action: Cloned action
    """
```

### WorldObject

Represents an object in the world:

```python
def __init__(self) -> WorldObject
    """Create a world object"""

def __init__(self, x: float, y: float, type: WorldObjectType, 
             value: float = 0.0, radius: float = 0.5) -> WorldObject
    """Create a world object
    
    Args:
        x: X coordinate
        y: Y coordinate
        type: Object type
        value: Object value
        radius: Object radius
    """

# Properties (read-write access)
@property
def x(self) -> float
    """X coordinate"""

@property  
def y(self) -> float
    """Y coordinate"""

@property
def radius(self) -> float
    """Object radius"""

@property
def type(self) -> WorldObjectType
    """Object type"""

@property
def value(self) -> float
    """Object value"""

@property
def active(self) -> bool
    """Object active state"""
```

### AgentBody

Represents the physical state of an agent:

```python
def __init__(self) -> AgentBody
    """Create agent body state"""

# Properties (read-write access)
@property
def x(self) -> float
    """X coordinate"""

@property
def y(self) -> float
    """Y coordinate"""

@property
def orientation(self) -> float
    """Orientation angle (radians)"""

@property
def velocityX(self) -> float
    """X velocity"""

@property
def velocityY(self) -> float
    """Y velocity"""

@property
def angularVelocity(self) -> float
    """Angular velocity (rad/s)"""

@property
def energy(self) -> float
    """Energy level"""

@property
def health(self) -> float
    """Health level"""

@property
def age(self) -> float
    """Age (simulation steps)"""

@property
def isMoving(self) -> bool
    """Whether agent is moving"""

@property
def isTurning(self) -> bool
    """Whether agent is turning"""

@property
def lastActionTime(self) -> float
    """Time since last action"""

def reset(self) -> None
    """Reset agent body to initial state"""
```

### ActionResult

Represents the result of an action executed in the world:

```python
def __init__(self) -> ActionResult
    """Create action result"""

def __init__(self, reward: float, success: bool, message: str = "") -> ActionResult
    """Create action result
    
    Args:
        reward: Reward received
        success: Whether action succeeded
        message: Optional message
    """

# Properties (read-write access)
@property
def reward(self) -> float
    """Reward received"""

@property
def success(self) -> bool
    """Whether action succeeded"""

@property
def message(self) -> str
    """Result message"""
```

### SensoryPercept

Sensory data packet containing all sensory modalities:

```python
def __init__(self) -> SensoryPercept
    """Create sensory percept"""

# Vision (read-write access)
@property
def vision(self) -> List[float]
    """Vision data (flattened grid)"""

@property
def visionWidth(self) -> int
    """Vision width"""

@property
def visionHeight(self) -> int
    """Vision height"""

# Touch (read-write access)
@property
def touch(self) -> List[float]
    """Touch data"""

# Internal signals (read-write access)
@property
def internal(self) -> List[float]
    """Internal signals"""

# Proprioception (read-write access)
@property
def proprioception(self) -> List[float]
    """Proprioception data"""

# Audio (read-write access)
@property
def audio(self) -> List[float]
    """Audio data"""

# Methods
def getAllSignals(self) -> List[float]
    """Get all signals concatenated
    
    Returns:
        List[float]: All sensory signals concatenated
    """

def getTimestamp(self) -> float
    """Get timestamp
    
    Returns:
        float: Timestamp
    """

def setTimestamp(self, timestamp: float) -> None
    """Set timestamp
    
    Args:
        timestamp: Timestamp value
    """
```

### Factory Functions

#### Config
```python
def createDefaultConfig() -> Config
    """Create a default configuration
    
    Returns:
        Config: Default configuration
    """
```

#### Brain
```python
def createBrain(config: Config) -> Brain
    """Create a new brain with configuration
    
    Args:
        config: Configuration for the brain
        
    Returns:
        Brain: Created brain
    """
```

#### World
```python
def createSimpleWorld() -> SimpleWorld
    """Create a new simple world
    
    Returns:
        SimpleWorld: Created world
    """
```

#### Agent
```python
def createAgentBrain(brain: Brain) -> AgentBrain
    """Create a new agent brain interface
    
    Args:
        brain: Underlying brain
        
    Returns:
        AgentBrain: Agent brain interface
    """
```

#### Type Constants
```python
# Invalid IDs
INVALID_NEURON_ID: NeuronId
INVALID_SYNAPSE_ID: SynapseId
INVALID_REGION_ID: RegionId
INVALID_POPULATION_ID: PopulationId
```

## Enumerations

### NeuronType
```python
NeuronType.Excitatory   # Excitatory neuron
NeuronType.Inhibitory   # Inhibitory neuron
NeuronType.Modulatory    # Modulatory neuron
NeuronType.Sensory       # Sensory neuron
NeuronType.Motor         # Motor neuron
NeuronType.Internal      # Internal neuron
```

### SynapseType
```python
SynapseType.Excitatory    # Excitatory synapse
SynapseType.Inhibitory    # Inhibitory synapse
SynapseType.Modulatory     # Modulatory synapse
SynapseType.Electrical    # Electrical synapse
SynapseType.GapJunction   # Gap junction synapse
```

### DevelopmentalStage
```python
DevelopmentalStage.Initial         # Initial development stage
DevelopmentalStage.CriticalPeriod # Critical period (high plasticity)
DevelopmentalStage.Maturation      # Maturation phase
DevelopmentalStage.Adult           # Adult stage
DevelopmentalStage.Aging          # Aging phase
```

### FiringState
```python
FiringState.Resting      # Not firing
FiringState.Active      # Currently firing
FiringState.Refractory   # In refractory period
FiringState.Inhibited    # Inhibited from firing
```

### ActionType
```python
ActionType.MoveForward    # Move forward
ActionType.MoveBackward   # Move backward
ActionType.TurnLeft       # Turn left
ActionType.TurnRight      # Turn right
ActionType.Look           # Look forward
ActionType.LookUp         # Look up
ActionType.LookDown       # Look down
ActionType.Interact       # Interact with object
ActionType.Eat            # Eat from resource
ActionType.Drink          # Drink from water source
ActionType.Rest           # Rest
ActionType.Wait           # Wait
ActionType.Custom         # Custom action
```

### MotorCommand
```python
MotorCommand.MoveForward  # Move forward
MotorCommand.MoveBackward # Move backward
MotorCommand.TurnLeft      # Turn left
MotorCommand.TurnRight     # Turn right
MotorCommand.LookLeft      # Look left
MotorCommand.LookRight     # Look right
MotorCommand.Interact     # Interact
MotorCommand.Wait         # Wait
```

### WorldObjectType
```python
WorldObjectType.Empty     # Empty cell
WorldObjectType.Resource # Resource (food, water, etc.)
WorldObjectType.Hazard   # Hazardous object
WorldObjectType.Wall     # Wall
WorldObjectType.Marker   # Marker point
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

## Example Usage

### Minimal Example
```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

### Environment Interaction
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print("Agent simulation complete!")
```

### Complete Agent Simulation
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
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
    
    # 6. Enable subsystems
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

# Run the simulation
run_agent_simulation(1000)
```

## Error Handling

### Common Errors

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### Exception Handling
```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
    import traceback
    traceback.print_exc()
```

## Performance Tips

1. **Reuse Objects**: Reuse Brain, Config, and other objects instead of creating new ones
2. **Batch Operations**: Process multiple sensory inputs before stepping the brain
3. **Memory Management**: Monitor memory usage in long simulations
4. **Parallel Processing**: Use the C++ library's parallel processing capabilities
5. **Configuration Optimization**: Use optimal configuration parameters for your use case

## Migration Guide

### From Version 5.x to 6.0

#### Changes
1. **API Simplification**: Removed deprecated methods and simplified API
2. **Memory Integration**: Memory systems are now integrated by default
3. **Development System**: Enhanced development stages and plasticity modulation
4. **Neuromodulation**: Added curiosity and novelty detection

#### Migration Steps
1. Update import statements (if any changes)
2. Verify configuration loading (new config file format)
3. Test basic functionality with minimal examples
4. Gradually add advanced features

## See Also

- `HOW_TO_USE.md` - Detailed installation and usage instructions
- `easy_usage.md` - Beginner-friendly usage guide
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background and theory
- `docs/EXPERIMENTS.md` - Experiment descriptions and results

---

*This documentation covers NLM version 6.0. For specific API details of individual classes and methods,
refer to the source code headers in the C++ source tree.*
