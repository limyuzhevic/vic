# NLM Python API Reference

**Comprehensive documentation for the NLM Python bindings (pynlm)**

## Overview

The NLM Python API provides access to the full NLM neural simulation framework through a Python interface. This reference covers all available classes, methods, and functions for creating and simulating artificial neural brains.

## Module Structure

The pynlm module is organized into the following main components:

```
import pynlm

# Core classes
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enumerations
NeuronType, SynapseType, DevelopmentalStage, etc.

# Configuration
config = pynlm.createDefaultConfig()
```

## Type Definitions

### NeuronId

```python
class NeuronId:
    """Unique identifier for a neuron"""
    
    def __init__(self, value=0):
        """Create a neuron ID
        
        Args:
            value: Unique integer value for the neuron ID
        """
        
    @property
    def value(self) -> int:
        """Get the numeric value of the neuron ID"""
        
    def index(self) -> int:
        """Get the index of the neuron ID"""
        
    def __eq__(self, other) -> bool:
        """Check equality with another NeuronId"""
        
    def __ne__(self, other) -> bool:
        """Check inequality with another NeuronId"""
        
    def __hash__(self) -> int:
        """Get hash for use in dictionaries/sets"""
        
    def __repr__(self) -> str:
        """Get string representation"""
```

### SynapseId

```python
class SynapseId:
    """Unique identifier for a synapse"""
    
    def __init__(self, value=0):
        """Create a synapse ID
        
        Args:
            value: Unique integer value for the synapse ID
        """
        
    @property
    def value(self) -> int:
        """Get the numeric value of the synapse ID"""
        
    def index(self) -> int:
        """Get the index of the synapse ID"""
        
    def __eq__(self, other) -> bool:
        """Check equality with another SynapseId"""
        
    def __ne__(self, other) -> bool:
        """Check inequality with another SynapseId"""
        
    def __hash__(self) -> int:
        """Get hash for use in dictionaries/sets"""
        
    def __repr__(self) -> str:
        """Get string representation"""
```

### RegionId

```python
class RegionId:
    """Unique identifier for a brain region"""
    
    def __init__(self, value=0):
        """Create a region ID
        
        Args:
            value: Unique integer value for the region ID
        """
        
    @property
    def value(self) -> int:
        """Get the numeric value of the region ID"""
        
    def index(self) -> int:
        """Get the index of the region ID"""
        
    def __eq__(self, other) -> bool:
        """Check equality with another RegionId"""
        
    def __ne__(self, other) -> bool:
        """Check inequality with another RegionId"""
        
    def __hash__(self) -> int:
        """Get hash for use in dictionaries/sets"""
        
    def __repr__(self) -> str:
        """Get string representation"""
```

### PopulationId

```python
class PopulationId:
    """Unique identifier for a neuron population"""
    
    def __init__(self, value=0):
        """Create a population ID
        
        Args:
            value: Unique integer value for the population ID
        """
        
    @property
    def value(self) -> int:
        """Get the numeric value of the population ID"""
        
    def index(self) -> int:
        """Get the index of the population ID"""
        
    def __eq__(self, other) -> bool:
        """Check equality with another PopulationId"""
        
    def __ne__(self, other) -> bool:
        """Check inequality with another PopulationId"""
```

## Enumerations

### NeuronType

```python
class NeuronType:
    """Type enumeration for different neuron classes"""
    
    Excitatory: Spike-inducing neuron that promotes activity
    Inhibitory: Spike-inducing neuron that suppresses activity
    Modulatory: Spike-inducing neuron that affects other neurons' excitability
    Sensory: Neuron that receives external input
    Motor: Neuron that produces motor output
    Internal: Neuron for internal state representation
```

### SynapseType

```python
class SynapseType:
    """Type enumeration for different synapse connections"""
    
    Excitatory: Synapses that increase postsynaptic neuron activity
    Inhibitory: Synapses that decrease postsynaptic neuron activity
    Modulatory: Synapses that modulate postsynaptic neuron excitability
    Electrical: Direct electrical coupling between neurons
    GapJunction: Electrical synapse through gap junctions
```

### DevelopmentalStage

```python
class DevelopmentalStage:
    """Developmental stage affecting plasticity and learning"""
    
    Initial: Newborn stage with high plasticity
    CriticalPeriod: Sensitive learning period
    Maturation: Development toward adult capabilities
    Adult: Mature learning capabilities
    Aging: Age-related changes in plasticity
```

### FiringState

```python
class FiringState:
    """Current state of a neuron's firing behavior"""
    
    Resting: Neuron at rest, below firing threshold
    Active: Neuron actively firing action potentials
    Refractory: Neuron recovering from firing, unable to fire again
    Inhibited: Neuron actively suppressed by inhibitory inputs
```

### ActionType

```python
class ActionType:
    """Types of actions an agent can take"""
    
    MoveForward: Move forward in environment
    MoveBackward: Move backward in environment
    MoveLeft: Move left in environment
    MoveRight: Move right in environment
    TurnLeft: Rotate counterclockwise
    TurnRight: Rotate clockwise
    Look: Look in current direction
    LookUp: Look upward
    LookDown: Look downward
    Interact: Interact with nearby objects
    Eat: Consume resource
    Drink: Consume liquid
    Rest: Stay stationary
    Wait: Do nothing for duration
    Custom: Custom action with parameters
```

### MotorCommand

```python
class MotorCommand:
    """Low-level motor commands for precise control"""
    
    MoveForward: Move forward
    MoveBackward: Move backward
    TurnLeft: Turn left
    TurnRight: Turn right
    LookLeft: Look left
    LookRight: Look right
    Interact: Interact with environment
    Wait: Wait/do nothing
```

### WorldObjectType

```python
class WorldObjectType:
    """Types of objects in the world"""
    
    Empty: Empty space
    Resource: Consumable resource
    Hazard: Dangerous object
    Wall: Obstacle
    Marker: Reference point
```

## Configuration System

### Config

```python
class Config:
    """Configuration class for NLM system"""
    
    def __init__(self):
        """Create an empty configuration"""
    
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from a JSON file
        
        Args:
            filepath: Path to JSON configuration file
            
        Returns:
            True if file loaded successfully, False otherwise
        """
    
    def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
        """Load configuration from command line arguments
        
        Args:
            argc: Number of arguments
            argv: Argument list (typically sys.argv)
            
        Returns:
            True if arguments parsed successfully, False otherwise
        """
    
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to a JSON file
        
        Args:
            filepath: Path to save JSON configuration file
            
        Returns:
            True if saved successfully, False on write failure
        """
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists
        
        Args:
            key: Configuration key (e.g., "brain.neuron_count")
            
        Returns:
            True if key exists, False otherwise
        """
    
    def get(self, key: str, default_value: str = "") -> str:
        """Get a configuration value as string
        
        Args:
            key: Configuration key
            default_value: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if not found
        """
    
    def getInt(self, key: str, default_value: int = 0) -> int:
        """Get a configuration value as integer
        
        Args:
            key: Configuration key
            default_value: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if not found
        """
    
    def getFloat(self, key: str, default_value: float = 0.0) -> float:
        """Get a configuration value as float
        
        Args:
            key: Configuration key
            default_value: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if not found
        """
    
    def getBool(self, key: str, default_value: bool = False) -> bool:
        """Get a configuration value as boolean
        
        Args:
            key: Configuration key
            default_value: Default value if key doesn't exist
            
        Returns:
            Configuration value or default if not found
        """
    
    def set(self, key: str, value: str, source=None) -> bool:
        """Set a configuration value
        
        Args:
            key: Configuration key
            value: Configuration value
            source: Source indicating how the configuration was obtained
            
        Returns:
            True if set successfully, False on error
        """
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys as a list
        
        Returns:
            List of all configuration keys
        """
    
    def clear(self) -> None:
        """Clear all configuration entries"""
    
    def summary(self) -> str:
        """Get a human-readable summary of configuration
        
        Returns:
            Summary string with key-value pairs and sources
        """
    
    def exportToDict(self) -> Dict:
        """Export configuration to Python dictionary
        
        Returns:
            Dictionary representation of configuration
        """
    
    def importFromDict(self, dict: Dict) -> bool:
        """Import configuration from Python dictionary
        
        Args:
            dict: Dictionary with configuration data
            
        Returns:
            True on success, False on error
        """
    
    def merge(self, other) -> bool:
        """Merge another configuration into this one
        
        Args:
            other: Another Config object to merge
            
        Returns:
            True on success, False on error
        """
    
    def copy(self):
        """Create a copy of this configuration
        
        Returns:
            New Config instance with same values
        """
    
    def __repr__(self) -> str:
        """Get string representation"""
        
    def __str__(self) -> str:
        """Get string representation (same as summary)"""
```

## Sensory Processing

### SensoryInput

```python
class SensoryInput:
    """Base class for sensory input"""
    
    def getType(self) -> str:
        """Get the type of sensory input
        
        Returns:
            String indicating sensory type
        """
    
    def getData(self) -> List[float]:
        """Get the raw data as a list
        
        Returns:
            List of float values representing sensory data
        """
    
    def getDimensions(self) -> int:
        """Get the dimensionality of sensory data
        
        Returns:
            Number of dimensions
        """
    
    def getTimestamp(self) -> float:
        """Get the timestamp of the sensory data
        
        Returns:
            Timestamp in simulation time
        """
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set the timestamp of the sensory data
        
        Args:
            timestamp: Timestamp in simulation time
        """
```

### Vision

```python
class Vision(SensoryInput):
    """Visual sensory input"""
    
    def __init__(self, width: int = 0, height: int = 0, channels: int = 3):
        """Create visual sensory input
        
        Args:
            width: Image width
            height: Image height
            channels: Number of color channels
        """
    
    def setData(self, data: List[float]) -> None:
        """Set visual data
        
        Args:
            data: List of pixel values (width × height × channels)
        """
    
    def getWidth(self) -> int:
        """Get image width
        
        Returns:
            Image width in pixels
        """
    
    def getHeight(self) -> int:
        """Get image height
        
        Returns:
            Image height in pixels
        """
    
    def getChannels(self) -> int:
        """Get number of color channels
        
        Returns:
            Number of color channels
        """
```

### Audio

```python
class Audio(SensoryInput):
    """Audio sensory input"""
    
    def __init__(self, sampleRate: int = 0, numSamples: int = 0):
        """Create audio sensory input
        
        Args:
            sampleRate: Samples per second
            numSamples: Number of audio samples
        """
    
    def setData(self, data: List[float]) -> None:
        """Set audio data
        
        Args:
            data: List of audio sample values
        """
    
    def setSampleRate(self, sampleRate: int) -> None:
        """Set audio sample rate
        
        Args:
            sampleRate: Samples per second
        """
    
    def getSampleRate(self) -> int:
        """Get audio sample rate
        
        Returns:
            Samples per second
        """
    
    def getNumSamples(self) -> int:
        """Get number of audio samples
        
        Returns:
            Number of samples in audio data
        """
```

### InternalSignals

```python
class InternalSignals(SensoryInput):
    """Internal signals sensory input"""
    
    def __init__(self):
        """Create internal signals input"""
    
    def addSignal(self, value: float) -> None:
        """Add a signal value
        
        Args:
            value: Signal value to add
        """
    
    def clearSignals(self) -> None:
        """Clear all signals"""
```

## Motor Processing

### Action

```python
class Action:
    """Action representation for motor output"""
    
    def __init__(self, action_type=None):
        """Create an action
        
        Args:
            action_type: Type of action (ActionType)
        """
    
    def __init__(self, action_type, parameters: List[float]):
        """Create an action with parameters
        
        Args:
            action_type: Type of action (ActionType)
            parameters: Action parameters (e.g., power, duration)
        """
    
    def getType(self):
        """Get the action type
        
        Returns:
            Action type
        """
    
    def setType(self, action_type) -> None:
        """Set the action type
        
        Args:
            action_type: New action type
        """
    
    def getParameters(self) -> List[float]:
        """Get action parameters
        
        Returns:
            List of action parameters
        """
    
    def setParameters(self, parameters: List[float]) -> None:
        """Set action parameters
        
        Args:
            parameters: New action parameters
        """
    
    def getName(self) -> str:
        """Get human-readable action name
        
        Returns:
            Action name
        """
    
    def clone(self):
        """Create a copy of this action
        
        Returns:
            New Action instance
        """
```

### AgentBody

```python
class AgentBody:
    """Agent body state"""
    
    def __init__(self):
        """Create an agent body state"""
    
    @property
    def x(self) -> float:
        """Get x position"""
    
    @x.setter
    def x(self, value: float) -> None:
        """Set x position"""
    
    @property
    def y(self) -> float:
        """Get y position"""
    
    @y.setter
    def y(self, value: float) -> None:
        """Set y position"""
    
    @property
    def orientation(self) -> float:
        """Get orientation (degrees)"""
    
    @orientation.setter
    def orientation(self, value: float) -> None:
        """Set orientation (degrees)"""
    
    @property
    def velocityX(self) -> float:
        """Get x velocity"""
    
    @velocityX.setter
    def velocityX(self, value: float) -> None:
        """Set x velocity"""
    
    @property
    def velocityY(self) -> float:
        """Get y velocity"""
    
    @velocityY.setter
    def velocityY(self, value: float) -> None:
        """Set y velocity"""
    
    @property
    def angularVelocity(self) -> float:
        """Get angular velocity"""
    
    @angularVelocity.setter
    def angularVelocity(self, value: float) -> None:
        """Set angular velocity"""
    
    @property
    def energy(self) -> float:
        """Get energy level"""
    
    @energy.setter
    def energy(self, value: float) -> None:
        """Set energy level"""
    
    @property
    def health(self) -> float:
        """Get health level"""
    
    @health.setter
    def health(self, value: float) -> None:
        """Set health level"""
    
    @property
    def age(self) -> float:
        """Get age"""
    
    @age.setter
    def age(self, value: float) -> None:
        """Set age"""
    
    @property
    def isMoving(self) -> bool:
        """Get whether agent is moving"""
    
    @property
    def isTurning(self) -> bool:
        """Get whether agent is turning"""
    
    @property
    def lastActionTime(self) -> float:
        """Get time of last action"""
    
    @lastActionTime.setter
    def lastActionTime(self, value: float) -> None:
        """Set time of last action"""
    
    def reset(self) -> None:
        """Reset agent body state"""
```

### ActionResult

```python
class ActionResult:
    """Action result from world"""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        """Create an action result
        
        Args:
            reward: Reward value from action
            success: Whether action was successful
            message: Descriptive message about action
        """
    
    @property
    def reward(self) -> float:
        """Get reward value"""
    
    @reward.setter
    def reward(self, value: float) -> None:
        """Set reward value"""
    
    @property
    def success(self) -> bool:
        """Get whether action was successful"""
    
    @success.setter
    def success(self, value: bool) -> None:
        """Set whether action was successful"""
    
    @property
    def message(self) -> str:
        """Get result message"""
    
    @message.setter
    def message(self, value: str) -> None:
        """Set result message"""
```

### WorldObject

```python
class WorldObject:
    """World object representation"""
    
    def __init__(self):
        """Create an empty world object"""
    
    def __init__(self, x: float, y: float, object_type, value: float = 0.0, radius: float = 0.5):
        """Create a world object
        
        Args:
            x: X coordinate
            y: Y coordinate
            object_type: Type of object (WorldObjectType)
            value: Object value (e.g., resource amount)
            radius: Object radius
        """
    
    @property
    def x(self) -> float:
        """Get x coordinate"""
    
    @x.setter
    def x(self, value: float) -> None:
        """Set x coordinate"""
    
    @property
    def y(self) -> float:
        """Get y coordinate"""
    
    @y.setter
    def y(self, value: float) -> None:
        """Set y coordinate"""
    
    @property
    def radius(self) -> float:
        """Get object radius"""
    
    @radius.setter
    def radius(self, value: float) -> None:
        """Set object radius"""
    
    @property
    def type(self):
        """Get object type"""
    
    @type.setter
    def type(self, value) -> None:
        """Set object type"""
    
    @property
    def value(self) -> float:
        """Get object value"""
    
    @value.setter
    def value(self, value: float) -> None:
        """Set object value"""
    
    @property
    def active(self) -> bool:
        """Get whether object is active"""
    
    @active.setter
    def active(self, value: bool) -> None:
        """Set whether object is active"""
```

### SensoryPercept

```python
class SensoryPercept:
    """Sensory percept data"""
    
    def __init__(self):
        """Create an empty sensory percept"""
    
    def getVision(self):
        """Get vision sensory data
        
        Returns:
            Vision sensory input or None if not available
        """
    
    def setVision(self, vision) -> None:
        """Set vision sensory data
        
        Args:
            vision: Vision sensory input
        """
    
    def getVisionWidth(self) -> int:
        """Get vision width
        
        Returns:
            Vision width in pixels
        """
    
    def getVisionHeight(self) -> int:
        """Get vision height
        
        Returns:
            Vision height in pixels
        """
    
    def getTouch(self):
        """Get touch sensory data
        
        Returns:
            Touch sensory input or None if not available
        """
    
    def setTouch(self, touch) -> None:
        """Set touch sensory data
        
        Args:
            touch: Touch sensory input
        """
    
    def getInternal(self):
        """Get internal sensory data
        
        Returns:
            Internal signals or None if not available
        """
    
    def setInternal(self, internal) -> None:
        """Set internal sensory data
        
        Args:
            internal: Internal signals
        """
    
    def getProprioception(self):
        """Get proprioception sensory data
        
        Returns:
            Proprioception data or None if not available
        """
    
    def setProprioception(self, proprioception) -> None:
        """Set proprioception sensory data
        
        Args:
            proprioception: Proprioception data
        """
    
    def getAudio(self):
        """Get audio sensory data
        
        Returns:
            Audio sensory input or None if not available
        """
    
    def setAudio(self, audio) -> None:
        """Set audio sensory data
        
        Args:
            audio: Audio sensory input
        """
    
    def getAllSignals(self) -> List:
        """Get all sensory signals
        
        Returns:
            List of all sensory signals
        """
    
    def getTimestamp(self) -> float:
        """Get percept timestamp
        
        Returns:
            Timestamp in simulation time
        """
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set percept timestamp
        
        Args:
            timestamp: Timestamp in simulation time
        """
```

## Simulation Environment

### SimpleWorld

```python
class SimpleWorld:
    """Simple 2D world for NLM simulation"""
    
    def __init__(self):
        """Create an empty world"""
    
    def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> None:
        """Configure the world
        
        Args:
            width: World width in cells
            height: World height in cells
            visionWidth: Agent vision width
            visionHeight: Agent vision height
        """
    
    def reset(self) -> None:
        """Reset the world to initial state"""
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position
        
        Args:
            x: Starting x coordinate
            y: Starting y coordinate
        """
    
    def update(self, timestep: float) -> None:
        """Update world state
        
        Args:
            timestep: Time step for update
        """
    
    def applyMotorCommand(self, cmd, currentTime: float):
        """Apply motor command to world
        
        Args:
            cmd: Motor command to apply
            currentTime: Current simulation time
            
        Returns:
            Action result from world
        """
    
    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept from world
        
        Returns:
            Current sensory percept
        """
    
    def getAgentBody(self) -> AgentBody:
        """Get agent body state
        
        Returns:
            Current agent body state
        """
    
    def addObject(self, obj) -> None:
        """Add object to world
        
        Args:
            obj: World object to add
        """
    
    def removeObject(self, x: float, y: float) -> bool:
        """Remove object from world
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if object removed, False otherwise
        """
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid, False otherwise
        """
    
    def getWidth(self) -> int:
        """Get world width
        
        Returns:
            World width in cells
        """
    
    def getHeight(self) -> int:
        """Get world height
        
        Returns:
            World height in cells
        """
    
    def getMaxEnergy(self) -> float:
        """Get maximum energy
        
        Returns:
            Maximum energy level
        """
    
    def setMaxEnergy(self, e: float) -> None:
        """Set maximum energy
        
        Args:
            e: Maximum energy level
        """
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate
        
        Returns:
            Energy decay rate per timestep
        """
    
    def setEnergyDecayRate(self, r: float) -> None:
        """Set energy decay rate
        
        Args:
            r: Energy decay rate per timestep
        """
    
    def getSimulationTime(self) -> float:
        """Get current simulation time
        
        Returns:
            Current simulation time
        """
    
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed for reproducible simulations
        
        Args:
            seed: Random seed
        """
    
    def getRandomSeed(self) -> int:
        """Get random seed
        
        Returns:
            Current random seed
        """
```

## Brain System

### Brain

```python
class Brain:
    """Central neural simulation brain class"""
    
    def __init__(self, config):
        """Create a brain with configuration
        
        Args:
            config: Brain configuration (Config object)
        """
    
    def initialize(self) -> bool:
        """Initialize the brain with configuration
        
        Returns:
            True on success, False on failure
        """
    
    def step(self, currentStep: int) -> None:
        """Perform a simulation step
        
        Args:
            currentStep: Current simulation step number
        """
    
    def step(self, currentStep: int, currentTime: float) -> None:
        """Perform a simulation step with timestamp
        
        Args:
            currentStep: Current simulation step number
            currentTime: Current simulation time
        """
    
    def reset(self) -> None:
        """Reset brain state"""
    
    def receiveSensoryInput(self, input) -> None:
        """Inject sensory input into brain
        
        Args:
            input: Sensory input data
        """
    
    def injectCurrent(self, neuron, current: float) -> None:
        """Inject current into a specific neuron
        
        Args:
            neuron: Target neuron
            current: Current amount to inject
        """
    
    def injectCurrentToNeurons(self, neuron_type, current: float) -> None:
        """Inject current into all neurons of a type
        
        Args:
            neuron_type: Type of neurons to target
            current: Current amount to inject
        """
    
    def produceAction(self):
        """Produce motor action based on neural activity
        
        Returns:
            Action object with motor commands
        """
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file
        
        Args:
            filepath: Path to save brain state
            
        Returns:
            True on success, False on failure
        """
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file
        
        Args:
            filepath: Path to load brain state from
            
        Returns:
            True on success, False on failure
        """
    
    def addRegion(self, name: str = ""):
        """Add a new neural region to the brain
        
        Args:
            name: Optional region name
            
        Returns:
            Region ID for the new region
        """
    
    def getRegion(self, region_id):
        """Get a region by ID
        
        Args:
            region_id: Region ID
            
        Returns:
            Region object or None if not found
        """
    
    def getRegionCount(self) -> int:
        """Get the number of neural regions
        
        Returns:
            Number of regions
        """
    
    def getRegionIds(self) -> List:
        """Get all region IDs
        
        Returns:
            List of all region IDs
        """
    
    def getRegions(self) -> List:
        """Get all regions
        
        Returns:
            List of all region objects
        """
    
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count across all regions
        
        Returns:
            Total number of neurons
        """
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count across all regions
        
        Returns:
            Total number of synapses
        """
    
    def getActiveNeuronCount(self) -> int:
        """Get count of currently active neurons
        
        Returns:
            Number of active neurons
        """
    
    def getFiringNeuronCount(self) -> int:
        """Get count of neurons currently firing spikes
        
        Returns:
            Number of firing neurons
        """
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons
        
        Returns:
            Average firing rate (spikes per millisecond)
        """
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio
        
        Returns:
            Ratio (>1.0 excitation bias, <1.0 inhibition bias)
        """
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count since initialization
        
        Returns:
            Total number of spikes
        """
    
    def getMostActiveNeurons(self, count: int) -> List:
        """Get most active neuron IDs
        
        Args:
            count: Number of top neurons to return
            
        Returns:
            List of most active neuron IDs
        """
    
    def getDevelopmentalStage(self):
        """Get current developmental stage
        
        Returns:
            Current developmental stage
        """
    
    def setDevelopmentalStage(self, stage) -> None:
        """Set developmental stage
        
        Args:
            stage: New developmental stage
        """
    
    def getConfig(self):
        """Get the configuration object
        
        Returns:
            Config object used to initialize this brain
        """
    
    def getFiringNeurons(self) -> List:
        """Get list of currently firing neuron IDs
        
        Returns:
            List of firing neuron IDs
        """
    
    def getNeuron(self, neuron_id) -> Neuron:
        """Get a specific neuron by ID
        
        Args:
            neuron_id: Target neuron ID
            
        Returns:
            Neuron object or None if not found
        """
    
    def getSynapse(self, synapse_id) -> Synapse:
        """Get a specific synapse by ID
        
        Args:
            synapse_id: Target synapse ID
            
        Returns:
            Synapse object or None if not found
        """
    
    def getStatistics(self) -> Dict:
        """Get comprehensive brain statistics
        
        Returns:
            Dictionary with brain statistics
        """
    
    def logStatus(self) -> None:
        """Log brain status including statistics and configuration"""
    
    def __repr__(self) -> str:
        """Get string representation"""
        
    def __str__(self) -> str:
        """Get formatted string representation"""
```

## Agent Interface

### AgentBrain

```python
class AgentBrain:
    """Agent brain interface connecting NLM brain to world"""
    
    def __init__(self, brain):
        """Initialize agent brain interface
        
        Args:
            brain: Underlying brain object
        """
    
    def initialize(self, world) -> None:
        """Initialize agent with world
        
        Args:
            world: World object to interact with
        """
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size
        
        Returns:
            Number of sensory channels
        """
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size
        
        Returns:
            Number of motor commands
        """
    
    def processSensoryInput(self, percept) -> None:
        """Process sensory percept from world
        
        Args:
            percept: Sensory percept from world
        """
    
    def decodeMotorCommand(self):
        """Decode brain motor activity into motor command
        
        Returns:
            Motor command object
        """
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update developmental system
        
        Args:
            timestep: Current timestep
        """
    
    def getDevelopmentalStage(self):
        """Get current developmental stage
        
        Returns:
            Current developmental stage
        """
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level
        
        Returns:
            Sum of all neuromodulator effects
        """
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level
        
        Returns:
            Curiosity level driving exploration
        """
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level
        
        Returns:
            Novelty level indicating unexpected input
        """
    
    def getPredictionError(self) -> float:
        """Get prediction error signal
        
        Returns:
            Prediction error value
        """
    
    def reset(self) -> None:
        """Reset agent for new episode"""
    
    def getBrain(self):
        """Get the underlying brain object
        
        Returns:
            Underlying brain object
        """
    
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable or disable reward-based neuromodulation
        
        Args:
            enable: Whether to enable reward modulation
        """
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity
        
        Args:
            enable: Whether to enable structural plasticity
        """
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable or disable developmental system
        
        Args:
            enable: Whether to enable development
        """
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable or disable curiosity-driven exploration
        
        Args:
            enable: Whether to enable curiosity
        """
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled
        
        Returns:
            True if reward modulation is enabled
        """
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled
        
        Returns:
            True if structural plasticity is enabled
        """
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development system is enabled
        
        Returns:
            True if development is enabled
        """
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity system is enabled
        
        Returns:
            True if curiosity is enabled
        """
    
    def getAgentStats(self) -> Dict:
        """Get comprehensive agent statistics
        
        Returns:
            Dictionary with agent statistics
        """
    
    def getSensoryConfig(self) -> int:
        """Get sensory input configuration
        
        Returns:
            Expected sensory input size
        """
    
    def getMotorConfig(self) -> int:
        """Get motor output configuration
        
        Returns:
            Expected motor output size
        """
    
    def getEnablements(self) -> Dict:
        """Get current enablement status of all subsystems
        
        Returns:
            Dictionary with subsystem enablement status
        """
    
    def setEnablements(self, enablements: Dict) -> bool:
        """Enable/disable agent subsystems
        
        Args:
            enablements: Dictionary with subsystem enablements
            
        Returns:
            True on success, False on error
        """
    
    def __repr__(self) -> str:
        """Get string representation"""
        
    def __str__(self) -> str:
        """Get formatted string representation"""
```

## Factory Functions

### createDefaultConfig

```python
def createDefaultConfig() -> Config:
    """Create a default configuration
    
    Returns:
        Default configuration object
    """
```

### createBrain

```python
def createBrain(config) -> Brain:
    """Create a new brain with configuration
    
    Args:
        config: Configuration object
        
    Returns:
        New Brain instance
    """
```

### createSimpleWorld

```python
def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world
    
    Returns:
        New SimpleWorld instance
    """
```

### createAgentBrain

```python
def createAgentBrain(brain) -> AgentBrain:
    """Create a new agent brain interface
    
    Args:
        brain: Brain object to wrap
        
    Returns:
        New AgentBrain instance
    """
```

## Global Constants

```python
INVALID_NEURON_ID: NeuronId
INVALID_SYNAPSE_ID: SynapseId
INVALID_REGION_ID: RegionId
INVALID_POPULATION_ID: PopulationId
```

## Example Usage

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    # Access statistics
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Save brain state
brain.save("brain_checkpoint.bin")

# Load brain state
brain.load("brain_checkpoint.bin")
```

## Advanced Usage

```python
import pynlm

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete agent simulation
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply reward modulation (example)
    reward = 0.0  # Get reward from world logic
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

## Version Information

```python
print(pynlm.__version__)
```