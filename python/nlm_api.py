"""
Python API Documentation for NLM
=================================

Complete API reference for the NLM (Neural Learning Machine) Python bindings.
This document covers both the original bindings and the improved bindings.
"""

import pynlm

__version__ = "0.1.0"

# ==================== ORIGINAL BINDINGS ====================

# Neuron Types and Identifiers
# =============================

class NeuronId:
    """
    Unique identifier for a neuron.
    
    Attributes:
        value (int): The neuron ID value
    """
    
    def __init__(self, value=0):
        """
        Create a NeuronId.
        
        Args:
            value (int): The neuron ID value (default: 0)
        """
        self.value = value
    
    def index(self):
        """
        Get the index of the neuron ID.
        
        Returns:
            int: The index of the neuron ID
        """
        pass
    
    def __eq__(self, other):
        """Check equality with another NeuronId."""
        pass
    
    def __ne__(self, other):
        """Check inequality with another NeuronId."""
        pass
    
    def __hash__(self):
        """Get hash of the NeuronId."""
        pass
    
    def __repr__(self):
        """Get string representation of the NeuronId."""
        pass

class SynapseId:
    """
    Unique identifier for a synapse.
    
    Attributes:
        value (int): The synapse ID value
    """
    
    def __init__(self, value=0):
        """
        Create a SynapseId.
        
        Args:
            value (int): The synapse ID value (default: 0)
        """
        self.value = value
    
    def index(self):
        """
        Get the index of the synapse ID.
        
        Returns:
            int: The index of the synapse ID
        """
        pass
    
    def __eq__(self, other):
        """Check equality with another SynapseId."""
        pass
    
    def __ne__(self, other):
        """Check inequality with another SynapseId."""
        pass
    
    def __hash__(self):
        """Get hash of the SynapseId."""
        pass
    
    def __repr__(self):
        """Get string representation of the SynapseId."""
        pass

class RegionId:
    """
    Unique identifier for a brain region.
    
    Attributes:
        value (int): The region ID value
    """
    
    def __init__(self, value=0):
        """
        Create a RegionId.
        
        Args:
            value (int): The region ID value (default: 0)
        """
        self.value = value
    
    def index(self):
        """
        Get the index of the region ID.
        
        Returns:
            int: The index of the region ID
        """
        pass
    
    def __eq__(self, other):
        """Check equality with another RegionId."""
        pass
    
    def __ne__(self, other):
        """Check inequality with another RegionId."""
        pass
    
    def __hash__(self):
        """Get hash of the RegionId."""
        pass
    
    def __repr__(self):
        """Get string representation of the RegionId."""
        pass

class PopulationId:
    """
    Unique identifier for a neuron population.
    
    Attributes:
        value (int): The population ID value
    """
    
    def __init__(self, value=0):
        """
        Create a PopulationId.
        
        Args:
            value (int): The population ID value (default: 0)
        """
        self.value = value
    
    def index(self):
        """
        Get the index of the population ID.
        
        Returns:
            int: The index of the population ID
        """
        pass
    
    def __eq__(self, other):
        """Check equality with another PopulationId."""
        pass
    
    def __ne__(self, other):
        """Check inequality with another PopulationId."""
        pass

# Enumerations
# ============

class NeuronType:
    """
    Neuron type enumeration.
    
    Values:
        Excitatory: Excitatory neuron
        Inhibitory: Inhibitory neuron
        Modulatory: Modulatory neuron
        Sensory: Sensory neuron
        Motor: Motor neuron
        Internal: Internal neuron
    """
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

class SynapseType:
    """
    Synapse type enumeration.
    
    Values:
        Excitatory: Excitatory synapse
        Inhibitory: Inhibitory synapse
        Modulatory: Modulatory synapse
        Electrical: Electrical synapse
        GapJunction: Gap junction
    """
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

class DevelopmentalStage:
    """
    Developmental stage enumeration.
    
    Values:
        Initial: Initial developmental stage
        CriticalPeriod: Critical period
        Maturation: Maturation stage
        Adult: Adult stage
        Aging: Aging stage
    """
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class FiringState:
    """
    Neuron firing state enumeration.
    
    Values:
        Resting: Neuron is resting
        Active: Neuron is active
        Refractory: Neuron is refractory
        Inhibited: Neuron is inhibited
    """
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

class ActionType:
    """
    Action type enumeration.
    
    Values:
        MoveForward: Move forward
        MoveBackward: Move backward
        MoveLeft: Move left
        MoveRight: Move right
        TurnLeft: Turn left
        TurnRight: Turn right
        Look: Look
        LookUp: Look up
        LookDown: Look down
        Interact: Interact
        Eat: Eat
        Drink: Drink
        Rest: Rest
        Wait: Wait
        Custom: Custom action
    """
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14

class MotorCommand:
    """
    Low-level motor command enumeration.
    
    Values:
        MoveForward: Move forward
        MoveBackward: Move backward
        TurnLeft: Turn left
        TurnRight: Turn right
        LookLeft: Look left
        LookRight: Look right
        Interact: Interact
        Wait: Wait
    """
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class WorldObjectType:
    """
    World object type enumeration.
    
    Values:
        Empty: Empty space
        Resource: Resource object
        Hazard: Hazard object
        Wall: Wall object
        Marker: Marker object
    """
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# Core Classes
# ============

class Config:
    """
    Configuration class for NLM system.
    
    This class manages the NLM system configuration parameters.
    """
    
    def __init__(self):
        """Create a default configuration."""
        pass
    
    def loadFromFile(self, filepath):
        """
        Load configuration from a JSON file.
        
        Args:
            filepath (str): Path to the JSON configuration file
            
        Returns:
            bool: True if configuration was loaded successfully
        """
        pass
    
    def loadFromArgs(self, argc, argv):
        """
        Load configuration from command line arguments.
        
        Args:
            argc (int): Number of command line arguments
            argv (list): Command line arguments
            
        Returns:
            bool: True if configuration was loaded successfully
        """
        pass
    
    def saveToFile(self, filepath):
        """
        Save configuration to a JSON file.
        
        Args:
            filepath (str): Path to the output JSON file
            
        Returns:
            bool: True if configuration was saved successfully
        """
        pass
    
    def has(self, key):
        """
        Check if a configuration key exists.
        
        Args:
            key (str): Configuration key to check
            
        Returns:
            bool: True if the key exists
        """
        pass
    
    def getKeys(self):
        """
        Get all configuration keys.
        
        Returns:
            list: List of configuration keys
        """
        pass
    
    def clear(self):
        """Clear all configuration entries."""
        pass
    
    def summary(self):
        """
        Get a summary string of the configuration.
        
        Returns:
            str: Configuration summary
        """
        pass
    
    def __repr__(self):
        """Get string representation of the configuration."""
        pass

class SensoryInput:
    """
    Base class for sensory input.
    
    This is the base class for all sensory input types.
    """
    
    def getType(self):
        """
        Get the type of sensory input.
        
        Returns:
            int: Sensory input type
        """
        pass
    
    def getData(self):
        """
        Get the raw data as a vector.
        
        Returns:
            list: Raw sensory data
        """
        pass
    
    def getDimensions(self):
        """
        Get the dimensionality.
        
        Returns:
            int: Number of dimensions
        """
        pass
    
    def getTimestamp(self):
        """
        Get the timestamp.
        
        Returns:
            float: Timestamp
        """
        pass
    
    def setTimestamp(self, timestamp):
        """
        Set the timestamp.
        
        Args:
            timestamp (float): Timestamp
        """
        pass

class Vision(SensoryInput):
    """
    Vision sensory input.
    
    Attributes:
        width (int): Vision width
        height (int): Vision height
        channels (int): Number of channels (default: 3)
    """
    
    def __init__(self, width=0, height=0, channels=3):
        """
        Create a vision input.
        
        Args:
            width (int): Vision width (default: 0)
            height (int): Vision height (default: 0)
            channels (int): Number of channels (default: 3)
        """
        pass
    
    def setData(self, data):
        """
        Set vision data.
        
        Args:
            data (list): Vision data
        """
        pass
    
    def getWidth(self):
        """
        Get vision width.
        
        Returns:
            int: Vision width
        """
        pass
    
    def getHeight(self):
        """
        Get vision height.
        
        Returns:
            int: Vision height
        """
        pass
    
    def getChannels(self):
        """
        Get number of channels.
        
        Returns:
            int: Number of channels
        """
        pass

class Audio(SensoryInput):
    """
    Audio sensory input.
    """
    
    def __init__(self, sampleRate=0, numSamples=0):
        """
        Create audio input.
        
        Args:
            sampleRate (int): Sample rate
            numSamples (int): Number of samples
        """
        pass
    
    def setData(self, data):
        """
        Set audio data.
        
        Args:
            data (list): Audio data
        """
        pass
    
    def setSampleRate(self, sampleRate):
        """
        Set sample rate.
        
        Args:
            sampleRate (int): Sample rate
        """
        pass
    
    def getSampleRate(self):
        """
        Get sample rate.
        
        Returns:
            int: Sample rate
        """
        pass
    
    def getNumSamples(self):
        """
        Get number of samples.
        
        Returns:
            int: Number of samples
        """
        pass

class InternalSignals(SensoryInput):
    """
    Internal signals sensory input.
    """
    
    def __init__(self):
        """Create internal signals input."""
        pass
    
    def addSignal(self, value):
        """
        Add a signal.
        
        Args:
            value (float): Signal value
        """
        pass
    
    def clearSignals(self):
        """Clear all signals."""
        pass

class Action:
    """
    Action representation for motor output.
    """
    
    def __init__(self):
        """Create an empty action."""
        pass
    
    def __init__(self, action_type):
        """
        Create an action with type.
        
        Args:
            action_type (ActionType): Action type
        """
        pass
    
    def __init__(self, action_type, parameters):
        """
        Create an action with type and parameters.
        
        Args:
            action_type (ActionType): Action type
            parameters (list): Action parameters
        """
        pass
    
    def getType(self):
        """
        Get action type.
        
        Returns:
            ActionType: Action type
        """
        pass
    
    def setType(self, action_type):
        """
        Set action type.
        
        Args:
            action_type (ActionType): Action type
        """
        pass
    
    def getParameters(self):
        """
        Get action parameters.
        
        Returns:
            list: Action parameters
        """
        pass
    
    def setParameters(self, params):
        """
        Set action parameters.
        
        Args:
            params (list): Action parameters
        """
        pass
    
    def getName(self):
        """
        Get action name.
        
        Returns:
            str: Action name
        """
        pass
    
    def clone(self):
        """
        Clone the action.
        
        Returns:
            Action: Cloned action
        """
        pass

class WorldObject:
    """
    World object representation.
    
    Attributes:
        x (float): X position
        y (float): Y position
        radius (float): Radius
        type (WorldObjectType): Object type
        value (float): Object value
        active (bool): Whether object is active
    """
    
    def __init__(self):
        """Create an empty world object."""
        pass
    
    def __init__(self, x, y, obj_type, value=0.0, radius=0.5):
        """
        Create a world object.
        
        Args:
            x (float): X position
            y (float): Y position
            obj_type (WorldObjectType): Object type
            value (float): Object value (default: 0.0)
            radius (float): Radius (default: 0.5)
        """
        pass

class AgentBody:
    """
    Agent body state.
    
    Attributes:
        x (float): X position
        y (float): Y position
        orientation (float): Orientation (in degrees)
        velocityX (float): X velocity
        velocityY (float): Y velocity
        angularVelocity (float): Angular velocity
        energy (float): Energy
        health (float): Health
        age (float): Age
        isMoving (bool): Whether agent is moving
        isTurning (bool): Whether agent is turning
        lastActionTime (float): Last action time
    """
    
    def __init__(self):
        """Create an empty agent body."""
        pass
    
    def reset(self):
        """Reset agent body to initial state."""
        pass

class ActionResult:
    """
    Action result from world.
    
    Attributes:
        reward (float): Reward
        success (bool): Whether action succeeded
        message (str): Result message
    """
    
    def __init__(self):
        """Create an empty action result."""
        pass
    
    def __init__(self, reward, success, message=""):
        """
        Create an action result.
        
        Args:
            reward (float): Reward
            success (bool): Whether action succeeded
            message (str): Result message (default: "")
        """
        pass

class SensoryPercept:
    """
    Sensory percept data.
    """
    
    def __init__(self):
        """Create an empty sensory percept."""
        pass
    
    def getVision(self):
        """
        Get vision data.
        
        Returns:
            Vision: Vision data
        """
        pass
    
    def setVision(self, vision):
        """
        Set vision data.
        
        Args:
            vision (Vision): Vision data
        """
        pass
    
    def getVisionWidth(self):
        """
        Get vision width.
        
        Returns:
            int: Vision width
        """
        pass
    
    def getVisionHeight(self):
        """
        Get vision height.
        
        Returns:
            int: Vision height
        """
        pass
    
    def getTouch(self):
        """
        Get touch data.
        
        Returns:
            list: Touch data
        """
        pass
    
    def setTouch(self, touch):
        """
        Set touch data.
        
        Args:
            touch (list): Touch data
        """
        pass
    
    def getInternal(self):
        """
        Get internal signals.
        
        Returns:
            list: Internal signals
        """
        pass
    
    def setInternal(self, internal):
        """
        Set internal signals.
        
        Args:
            internal (list): Internal signals
        """
        pass
    
    def getProprioception(self):
        """
        Get proprioception data.
        
        Returns:
            list: Proprioception data
        """
        pass
    
    def setProprioception(self, proprioception):
        """
        Set proprioception data.
        
        Args:
            proprioception (list): Proprioception data
        """
        pass
    
    def getAudio(self):
        """
        Get audio data.
        
        Returns:
            list: Audio data
        """
        pass
    
    def setAudio(self, audio):
        """
        Set audio data.
        
        Args:
            audio (list): Audio data
        """
        pass
    
    def getAllSignals(self):
        """
        Get all signals.
        
        Returns:
            dict: All signals
        """
        pass
    
    def getTimestamp(self):
        """
        Get timestamp.
        
        Returns:
            float: Timestamp
        """
        pass
    
    def setTimestamp(self, timestamp):
        """
        Set timestamp.
        
        Args:
            timestamp (float): Timestamp
        """
        pass

class SimpleWorld:
    """
    Simple 2D world for NLM simulation.
    
    Attributes:
        width (int): World width
        height (int): World height
        maxEnergy (float): Maximum energy
        energyDecayRate (float): Energy decay rate
    """
    
    def __init__(self):
        """Create an empty world."""
        pass
    
    def configure(self, width, height, visionWidth, visionHeight):
        """
        Configure world dimensions.
        
        Args:
            width (int): World width
            height (int): World height
            visionWidth (int): Vision width
            visionHeight (int): Vision height
        """
        pass
    
    def reset(self):
        """Reset world to initial state."""
        pass
    
    def setAgentStart(self, x, y):
        """
        Set agent start position.
        
        Args:
            x (float): X position
            y (float): Y position
        """
        pass
    
    def update(self, timestep):
        """
        Update world state.
        
        Args:
            timestep (float): Time step
        """
        pass
    
    def applyMotorCommand(self, cmd, currentTime):
        """
        Apply motor command to world.
        
        Args:
            cmd (MotorCommand): Motor command
            currentTime (float): Current time
            
        Returns:
            ActionResult: Action result
        """
        pass
    
    def getSensoryPercept(self):
        """
        Get sensory percept.
        
        Returns:
            SensoryPercept: Sensory percept
        """
        pass
    
    def getAgentBody(self):
        """
        Get agent body.
        
        Returns:
            AgentBody: Agent body
        """
        pass
    
    def addObject(self, obj):
        """
        Add world object.
        
        Args:
            obj (WorldObject): World object
        """
        pass
    
    def removeObject(self, x, y):
        """
        Remove world object at position.
        
        Args:
            x (float): X position
            y (float): Y position
        """
        pass
    
    def isValidPosition(self, x, y):
        """
        Check if position is valid.
        
        Args:
            x (float): X position
            y (float): Y position
            
        Returns:
            bool: True if position is valid
        """
        pass
    
    def getWidth(self):
        """
        Get world width.
        
        Returns:
            int: World width
        """
        pass
    
    def getHeight(self):
        """
        Get world height.
        
        Returns:
            int: World height
        """
        pass
    
    def getMaxEnergy(self):
        """
        Get maximum energy.
        
        Returns:
            float: Maximum energy
        """
        pass
    
    def setMaxEnergy(self, e):
        """
        Set maximum energy.
        
        Args:
            e (float): Maximum energy
        """
        pass
    
    def getEnergyDecayRate(self):
        """
        Get energy decay rate.
        
        Returns:
            float: Energy decay rate
        """
        pass
    
    def setEnergyDecayRate(self, r):
        """
        Set energy decay rate.
        
        Args:
            r (float): Energy decay rate
        """
        pass
    
    def getSimulationTime(self):
        """
        Get simulation time.
        
        Returns:
            float: Simulation time
        """
        pass
    
    def setRandomSeed(self, seed):
        """
        Set random seed.
        
        Args:
            seed (int): Random seed
        """
        pass
    
    def getRandomSeed(self):
        """
        Get random seed.
        
        Returns:
            int: Random seed
        """
        pass

class Brain:
    """
    Central neural simulation brain class.
    
    This class represents the central neural simulation brain.
    """
    
    def __init__(self, config):
        """
        Create a brain with configuration.
        
        Args:
            config (Config): Configuration
        """
        pass
    
    def initialize(self):
        """
        Initialize the brain with configuration.
        
        Returns:
            bool: True if initialization was successful
        """
        pass
    
    def step(self, currentStep):
        """
        Perform a simulation step.
        
        Args:
            currentStep (int): Current step number
        """
        pass
    
    def step(self, currentStep, currentTime):
        """
        Perform a simulation step with timestamp.
        
        Args:
            currentStep (int): Current step number
            currentTime (float): Current time
        """
        pass
    
    def receiveSensoryInput(self, input):
        """
        Inject sensory input into the brain.
        
        Args:
            input (SensoryInput): Sensory input
        """
        pass
    
    def injectCurrent(self, neuron, current):
        """
        Inject current into a specific neuron.
        
        Args:
            neuron (NeuronId): Neuron ID
            current (float): Current to inject
        """
        pass
    
    def injectCurrentToNeurons(self, neuron_type, current):
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type (NeuronType): Neuron type
            current (float): Current to inject
        """
        pass
    
    def produceAction(self):
        """
        Produce motor action based on neural activity.
        
        Returns:
            Action: Motor action
        """
        pass
    
    def reset(self):
        """Reset brain state."""
        pass
    
    def save(self, filepath):
        """
        Save brain state to file.
        
        Args:
            filepath (str): Path to output file
            
        Returns:
            bool: True if state was saved successfully
        """
        pass
    
    def load(self, filepath):
        """
        Load brain state from file.
        
        Args:
            filepath (str): Path to input file
            
        Returns:
            bool: True if state was loaded successfully
        """
        pass
    
    def addRegion(self, name=""):
        """
        Add a new neural region.
        
        Args:
            name (str): Region name (default: "")
            
        Returns:
            RegionId: Region ID
        """
        pass
    
    def getRegion(self, id):
        """
        Get a region by ID.
        
        Args:
            id (RegionId): Region ID
            
        Returns:
            Region: Region
        """
        pass
    
    def getRegionCount(self):
        """
        Get the number of regions.
        
        Returns:
            int: Number of regions
        """
        pass
    
    def getRegionIds(self):
        """
        Get all region IDs.
        
        Returns:
            list: List of region IDs
        """
        pass
    
    def getRegions(self):
        """
        Get all regions.
        
        Returns:
            list: List of regions
        """
        pass
    
    def getTotalNeuronCount(self):
        """
        Get total neuron count across all regions.
        
        Returns:
            int: Total neuron count
        """
        pass
    
    def getTotalSynapseCount(self):
        """
        Get total synapse count across all regions.
        
        Returns:
            int: Total synapse count
        """
        pass
    
    def getActiveNeuronCount(self):
        """
        Get count of active neurons.
        
        Returns:
            int: Count of active neurons
        """
        pass
    
    def getFiringNeuronCount(self):
        """
        Get count of currently firing neurons.
        
        Returns:
            int: Count of firing neurons
        """
        pass
    
    def getAverageFiringRate(self):
        """
        Get average firing rate across all neurons.
        
        Returns:
            float: Average firing rate (Hz)
        """
        pass
    
    def getExcitationInhibitionRatio(self):
        """
        Get excitation/inhibition balance ratio.
        
        Returns:
            float: E/I ratio
        """
        pass
    
    def getTotalSpikeCount(self):
        """
        Get total spike count.
        
        Returns:
            int: Total spike count
        """
        pass
    
    def getDevelopmentalStage(self):
        """
        Get current developmental stage.
        
        Returns:
            DevelopmentalStage: Current developmental stage
        """
        pass
    
    def setDevelopmentalStage(self, stage):
        """
        Set developmental stage.
        
        Args:
            stage (DevelopmentalStage): Developmental stage
        """
        pass
    
    def getConfig(self):
        """
        Get the configuration.
        
        Returns:
            Config: Configuration
        """
        pass
    
    def logStatus(self):
        """Log brain status."""
        pass

class AgentBrain:
    """
    Agent brain interface connecting NLM brain to world.
    
    This class provides the interface between the NLM brain and the environment.
    """
    
    def __init__(self, brain):
        """
        Create an agent brain interface.
        
        Args:
            brain (Brain): Underlying brain
        """
        pass
    
    def initialize(self, world):
        """
        Initialize with world.
        
        Args:
            world (SimpleWorld): World
        """
        pass
    
    def getSensoryInputSize(self):
        """
        Get expected sensory input size.
        
        Returns:
            int: Expected sensory input size
        """
        pass
    
    def getMotorOutputSize(self):
        """
        Get expected motor output size.
        
        Returns:
            int: Expected motor output size
        """
        pass
    
    def processSensoryInput(self, percept):
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept (SensoryPercept): Sensory percept
        """
        pass
    
    def decodeMotorCommand(self):
        """
        Decode brain motor activity into motor command.
        
        Returns:
            MotorCommand: Motor command
        """
        pass
    
    def applyRewardModulation(self, reward, predictedReward):
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward (float): Reward
            predictedReward (float): Predicted reward
        """
        pass
    
    def updateDevelopment(self, timestep):
        """
        Update development system.
        
        Args:
            timestep (float): Time step
        """
        pass
    
    def getDevelopmentalStage(self):
        """
        Get current developmental stage.
        
        Returns:
            DevelopmentalStage: Current developmental stage
        """
        pass
    
    def getNeuromodulationLevel(self):
        """
        Get current neuromodulation level.
        
        Returns:
            float: Neuromodulation level
        """
        pass
    
    def getCuriosityLevel(self):
        """
        Get curiosity level.
        
        Returns:
            float: Curiosity level
        """
        pass
    
    def getNoveltyLevel(self):
        """
        Get novelty level.
        
        Returns:
            float: Novelty level
        """
        pass
    
    def getPredictionError(self):
        """
        Get prediction error.
        
        Returns:
            float: Prediction error
        """
        pass
    
    def reset(self):
        """Reset agent for new episode."""
        pass
    
    def getBrain(self):
        """
        Get the underlying brain.
        
        Returns:
            Brain: Underlying brain
        """
        pass
    
    def enableRewardModulation(self, enable):
        """
        Enable or disable reward modulation.
        
        Args:
            enable (bool): Whether to enable reward modulation
        """
        pass
    
    def enableStructuralPlasticity(self, enable):
        """
        Enable or disable structural plasticity.
        
        Args:
            enable (bool): Whether to enable structural plasticity
        """
        pass
    
    def enableDevelopment(self, enable):
        """
        Enable or disable development.
        
        Args:
            enable (bool): Whether to enable development
        """
        pass
    
    def enableCuriosity(self, enable):
        """
        Enable or disable curiosity.
        
        Args:
            enable (bool): Whether to enable curiosity
        """
        pass
    
    def isRewardModulationEnabled(self):
        """
        Check if reward modulation is enabled.
        
        Returns:
            bool: True if reward modulation is enabled
        """
        pass
    
    def isStructuralPlasticityEnabled(self):
        """
        Check if structural plasticity is enabled.
        
        Returns:
            bool: True if structural plasticity is enabled
        """
        pass
    
    def isDevelopmentEnabled(self):
        """
        Check if development is enabled.
        
        Returns:
            bool: True if development is enabled
        """
        pass
    
    def isCuriosityEnabled(self):
        """
        Check if curiosity is enabled.
        
        Returns:
            bool: True if curiosity is enabled
        """
        pass
# ==================== IMPROVED BINDINGS ====================

# High-level Classes
# =================

class ConfigBuilder:
    """
    High-level configuration builder for Python.
    
    This class provides a fluent interface for configuring NLM parameters.
    """
    
    def __init__(self):
        """Create a configuration builder."""
        pass
    
    def setNeuronCount(self, count):
        """
        Set neuron count.
        
        Args:
            count (int): Number of neurons
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def setRegionCount(self, count):
        """
        Set region count.
        
        Args:
            count (int): Number of regions
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def setConnectionProbability(self, prob):
        """
        Set connection probability.
        
        Args:
            prob (float): Connection probability
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def setSTDPParameters(self, ltp_weight, ltd_weight, tau):
        """
        Set STDP parameters.
        
        Args:
            ltp_weight (float): STDP LTP weight
            ltd_weight (float): STDP LTD weight
            tau (float): STDP time constant
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def setStructuralPlasticity(self, synaptogenesis, pruning):
        """
        Set structural plasticity parameters.
        
        Args:
            synaptogenesis (float): Synaptogenesis rate
            pruning (float): Pruning rate
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def setMemoryCapacity(self, working_memory, episodic_memory):
        """
        Set memory capacity.
        
        Args:
            working_memory (int): Working memory capacity
            episodic_memory (int): Max episodic memory episodes
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def setSimulation(self, timestep, max_steps):
        """
        Set simulation parameters.
        
        Args:
            timestep (float): Simulation timestep
            max_steps (int): Maximum simulation steps
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def enableNeuromodulation(self, enable):
        """
        Enable or disable neuromodulation.
        
        Args:
            enable (bool): Whether to enable neuromodulation
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def setDopamineLevel(self, level):
        """
        Set dopamine level.
        
        Args:
            level (float): Dopamine level
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def enableDevelopment(self, enable):
        """
        Enable or disable development.
        
        Args:
            enable (bool): Whether to enable development
            
        Returns:
            ConfigBuilder: Self for method chaining
        """
        pass
    
    def build(self):
        """
        Build the configuration.
        
        Returns:
            Config: Built configuration
        """
        pass
    
    @staticmethod
    def createDefaultConfig():
        """
        Create a default configuration.
        
        Returns:
            Config: Default configuration
        """
        pass
    
    @staticmethod
    def createPerformanceConfig(neuron_count):
        """
        Create a performance configuration.
        
        Args:
            neuron_count (int): Number of neurons
            
        Returns:
            Config: Performance configuration
        """
        pass
    
    @staticmethod
    def createResearchConfig():
        """
        Create a research configuration.
        
        Returns:
            Config: Research configuration
        """
        pass

class SimulationManager:
    """
    High-level simulation manager.
    
    This class provides high-level control over simulations.
    """
    
    def __init__(self, config):
        """
        Create a simulation manager.
        
        Args:
            config (Config): Configuration
        """
        pass
    
    def runSteps(self, steps):
        """
        Run simulation steps.
        
        Args:
            steps (int): Number of steps to run
            
        Returns:
            SimulationManager: Self for method chaining
        """
        pass
    
    def runEpisode(self, sensory_inputs, action_indices):
        """
        Run simulation episode with sensory input and actions.
        
        Args:
            sensory_inputs (list): List of sensory input vectors
            action_indices (list): List of action indices
            
        Returns:
            SimulationManager: Self for method chaining
        """
        pass
    
    def getBrain(self):
        """
        Get the underlying brain.
        
        Returns:
            Brain: Underlying brain
        """
        pass
    
    def getStatistics(self):
        """
        Get simulation statistics.
        
        Returns:
            dict: Statistics dictionary
        """
        pass
    
    def saveState(self, filepath):
        """
        Save brain state.
        
        Args:
            filepath (str): Path to output file
            
        Returns:
            bool: True if state was saved successfully
        """
        pass
    
    def loadState(self, filepath):
        """
        Load brain state.
        
        Args:
            filepath (str): Path to input file
            
        Returns:
            bool: True if state was loaded successfully
        """
        pass
    
    def enableLearning(self, enable):
        """
        Enable or disable learning.
        
        Args:
            enable (bool): Whether to enable learning
        """
        pass
    
    def setLearningRate(self, rate):
        """
        Set learning rate.
        
        Args:
            rate (float): Learning rate
        """
        pass

class AgentManager:
    """
    High-level agent manager.
    
    This class provides high-level control over agents.
    """
    
    def __init__(self):
        """Create an agent manager."""
        pass
    
    def configureWorld(self, width, height):
        """
        Configure world.
        
        Args:
            width (int): World width
            height (int): World height
            
        Returns:
            AgentManager: Self for method chaining
        """
        pass
    
    def setAgentPosition(self, x, y):
        """
        Set agent position.
        
        Args:
            x (float): X position
            y (float): Y position
            
        Returns:
            AgentManager: Self for method chaining
        """
        pass
    
    def setAgentConfig(self, config_file):
        """
        Set agent configuration from file.
        
        Args:
            config_file (str): Path to configuration file
            
        Returns:
            AgentManager: Self for method chaining
        """
        pass
    
    def runEpisode(self, max_steps):
        """
        Run agent episode.
        
        Args:
            max_steps (int): Maximum number of steps
            
        Returns:
            AgentManager: Self for method chaining
        """
        pass
    
    def getEpisodeResults(self):
        """
        Get episode results.
        
        Returns:
            dict: Episode results
        """
        pass
    
    def reset(self):
        """Reset agent for new episode."""
        pass
    
    def getWorldState(self):
        """
        Get world state.
        
        Returns:
            dict: World state
        """
        pass

# Convenience Functions
# ====================

def createVisionInput(width, height, data):
    """
    Create vision sensory input.
    
    Args:
        width (int): Vision width
        height (int): Vision height
        data (list): Vision data
        
    Returns:
        SensoryInput: Vision input
    """
    pass

def createAudioInput(sample_rate, samples):
    """
    Create audio sensory input.
    
    Args:
        sample_rate (int): Sample rate
        samples (list): Audio samples
        
    Returns:
        SensoryInput: Audio input
    """
    pass

def createAction(action_type, parameters=None):
    """
    Create action.
    
    Args:
        action_type (ActionType): Action type
        parameters (list, optional): Action parameters
        
    Returns:
        Action: Created action
    """
    pass

def neuralSimilarity(pattern1, pattern2):
    """
    Calculate similarity between two neural activity patterns.
    
    Args:
        pattern1 (list): First pattern
        pattern2 (list): Second pattern
        
    Returns:
        float: Similarity score (0-1)
    """
    pass

def getPresetConfig(name):
    """
    Get preset configuration.
    
    Args:
        name (str): Configuration name ("default", "performance", "research")
        
    Returns:
        Config: Preset configuration
        
    Raises:
        Exception: If configuration name is unknown
    """
    pass

def validateConfig(config):
    """
    Validate configuration.
    
    Args:
        config (Config): Configuration to validate
        
    Returns:
        bool: True if configuration is valid
    """
    pass

# Module Metadata
# ============

__all__ = [
    # Neuron types and identifiers
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Enumerations
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Core classes
    "Config",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    "SensoryPercept",
    "SimpleWorld",
    "Brain",
    "AgentBrain",
    
    # Improved bindings
    "ConfigBuilder",
    "SimulationManager",
    "AgentManager",
    
    # Convenience functions
    "createVisionInput",
    "createAudioInput",
    "createAction",
    "neuralSimilarity",
    "getPresetConfig",
    "validateConfig",
    
    # Original bindings (for backward compatibility)
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Helper functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain"
]

# Version
# =======

__version__ = "0.1.0"

# Documentation
# =============

if __name__ == "__main__":
    print("NLM Python API Documentation")
    print("=" * 40)
    print(f"Version: {__version__}")
    print("\nFor detailed documentation, refer to:")
    print("- HOW_TO_USE.md")
    print("- Python examples directory")
    print("- Python API documentation")
    print("\nFor API reference, use:")
    print("  help(pynlm)")
    print("  help(pynlm.Config)")
    print("  help(pynlm.Brain)")
    print("\nFor examples, run:")
    print("  python examples/tutorial_basic.py")
    print("  python examples/advanced_examples.py")
    print("  python examples/complete_examples.py")