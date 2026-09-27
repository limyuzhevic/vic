"""
NLM Python API Reference
=======================

This file provides a complete reference for the NLM Python API,
including all classes, methods, and their parameters.

For detailed documentation, see:
- HOW_TO_USE.md: Comprehensive usage guide
- easy_usage.md: Quick start guide
- docs/: Scientific and technical documentation
"""

import pynlm

# ==============================================================================
# CLASS REFERENCES
# ==============================================================================

class ConfigReference:
    """
    Configuration class for NLM system.
    
    The Config class manages all configuration parameters for the NLM simulation,
    including neural properties, plasticity rules, neuromodulation, development,
    and world settings.
    
    Examples:
        >>> config = pynlm.createDefaultConfig()
        >>> config.setInt("neuron_count", 1000)
        >>> config.setFloat("connection_probability", 0.1)
        >>> config.saveToFile("config.json")
    """
    
    # Core methods
    def loadFromFile(self, filepath):
        """Load configuration from a JSON file.
        
        Args:
            filepath (str): Path to the configuration file
            
        Returns:
            bool: True if successful, False otherwise
        """
        pass
    
    def loadFromArgs(self, argc, argv):
        """Load configuration from command line arguments.
        
        Args:
            argc (int): Number of command line arguments
            argv (list): List of command line arguments
            
        Returns:
            bool: True if successful
        """
        pass
    
    def saveToFile(self, filepath):
        """Save configuration to a JSON file.
        
        Args:
            filepath (str): Path to save the configuration file
            
        Returns:
            bool: True if successful, False otherwise
        """
        pass
    
    def has(self, key):
        """Check if a configuration key exists.
        
        Args:
            key (str): Configuration key to check
            
        Returns:
            bool: True if key exists, False otherwise
        """
        pass
    
    def getKeys(self):
        """Get all configuration keys.
        
        Returns:
            list: List of all configuration keys
        """
        pass
    
    def clear(self):
        """Clear all configuration entries."""
        pass
    
    def summary(self):
        """Get a summary string of the configuration.
        
        Returns:
            str: Summary of configuration
        """
        pass
    
    # Value retrieval methods
    def get(self, key):
        """Get a configuration value as string.
        
        Args:
            key (str): Configuration key
            
        Returns:
            str: Configuration value, or empty string if not found
        """
        pass
    
    def getInt(self, key, defaultValue=0):
        """Get a configuration value as integer.
        
        Args:
            key (str): Configuration key
            defaultValue (int): Default value if key not found
            
        Returns:
            int: Configuration value, or defaultValue if not found
        """
        pass
    
    def getFloat(self, key, defaultValue=0.0):
        """Get a configuration value as float.
        
        Args:
            key (str): Configuration key
            defaultValue (float): Default value if key not found
            
        Returns:
            float: Configuration value, or defaultValue if not found
        """
        pass
    
    def getBool(self, key, defaultValue=False):
        """Get a configuration value as boolean.
        
        Args:
            key (str): Configuration key
            defaultValue (bool): Default value if key not found
            
        Returns:
            bool: Configuration value, or defaultValue if not found
        """
        pass
    
    # Value setting methods
    def set(self, key, value):
        """Set a configuration value (string).
        
        Args:
            key (str): Configuration key
            value (str): Configuration value
        """
        pass
    
    def setInt(self, key, value):
        """Set a configuration value (integer).
        
        Args:
            key (str): Configuration key
            value (int): Configuration value
        """
        pass
    
    def setFloat(self, key, value):
        """Set a configuration value (float).
        
        Args:
            key (str): Configuration key
            value (float): Configuration value
        """
        pass
    
    def setBool(self, key, value):
        """Set a configuration value (boolean).
        
        Args:
            key (str): Configuration key
            value (bool): Configuration value
        """
        pass
    
    def remove(self, key):
        """Remove a configuration key.
        
        Args:
            key (str): Configuration key to remove
        """
        pass

class BrainReference:
    """
    Central neural simulation brain class.
    
    The Brain class is the main component of NLM - it contains neurons, synapses,
    populations, and regions, along with all the integrated systems that make up
the artificial brain.
    
    The Brain implements real spiking neural computation with event-driven dynamics
    and integrated memory, prediction, cognition, and neuromodulation systems.
    
    Examples:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> brain.step(0)
    """
    
    # Core methods
    def initialize(self):
        """Initialize the brain with configuration.
        
        Returns:
            bool: True on success
        """
        pass
    
    def step(self, currentStep):
        """Perform a simulation step with current step number.
        
        Args:
            currentStep (int): Current simulation step number
        """
        pass
    
    def step_with_time(self, currentStep, currentTime):
        """Perform a simulation step with current step number and time.
        
        Args:
            currentStep (int): Current simulation step number
            currentTime (float): Current simulation time
        """
        pass
    
    def receiveSensoryInput(self, input):
        """Inject sensory input into the brain.
        
        Args:
            input (SensoryInput): Sensory input to inject
        """
        pass
    
    def injectCurrent(self, neuron, current):
        """Inject current directly into a specific neuron.
        
        Args:
            neuron (NeuronId): Neuron to inject current into
            current (float): Current value to inject (nA)
        """
        pass
    
    def injectCurrentToNeurons(self, type, current):
        """Inject current into all neurons of a specific type.
        
        Args:
            type (NeuronType): Type of neurons to inject current into
            current (float): Current value to inject (nA)
        """
        pass
    
    def produceAction(self):
        """Produce motor action based on neural activity.
        
        Returns:
            Action: Motor action produced by the brain
        """
        pass
    
    def reset(self):
        """Reset brain state to initial configuration."""
        pass
    
    def save(self, filepath):
        """Save brain state to file (checkpointing).
        
        Args:
            filepath (str): Path to save the checkpoint file
            
        Returns:
            bool: True on success, False otherwise
        """
        pass
    
    def load(self, filepath):
        """Load brain state from file (checkpointing).
        
        Args:
            filepath (str): Path to load the checkpoint file
            
        Returns:
            bool: True on success, False otherwise
        """
        pass
    
    def addRegion(self, name=""):
        """Add a new neural region.
        
        Args:
            name (str): Name for the new region
            
        Returns:
            RegionId: ID of the newly created region
        """
        pass
    
    def getRegion(self, id):
        """Get a region by ID.
        
        Args:
            id (RegionId): ID of the region to retrieve
            
        Returns:
            NeuralRegion: Region object, or None if ID doesn't exist
        """
        pass
    
    # Statistics methods
    def getTotalNeuronCount(self):
        """Get total neuron count across all regions.
        
        Returns:
            int: Total number of neurons
        """
        pass
    
    def getTotalSynapseCount(self):
        """Get total synapse count across all regions.
        
        Returns:
            int: Total number of synapses
        """
        pass
    
    def getActiveNeuronCount(self):
        """Get count of active neurons.
        
        Returns:
            int: Number of active neurons
        """
        pass
    
    def getFiringNeuronCount(self):
        """Get count of currently firing neurons.
        
        Returns:
            int: Number of firing neurons
        """
        pass
    
    def getPendingSpikeEventCount(self):
        """Get count of pending spike events.
        
        Returns:
            int: Number of pending spike events
        """
        pass
    
    def getAverageFiringRate(self):
        """Get average firing rate across all neurons.
        
        Returns:
            float: Average firing rate (spikes per second)
        """
        pass
    
    def getExcitationInhibitionRatio(self):
        """Get excitation/inhibition balance ratio.
        
        Returns:
            float: E/I ratio
        """
        pass
    
    def getTotalSpikeCount(self):
        """Get total spike count since initialization.
        
        Returns:
            int: Total number of spikes
        """
        pass
    
    def getDevelopmentalStage(self):
        """Get current developmental stage.
        
        Returns:
            DevelopmentalStage: Current developmental stage
        """
        pass
    
    def setDevelopmentalStage(self, stage):
        """Set developmental stage.
        
        Args:
            stage (DevelopmentalStage): Developmental stage to set
        """
        pass
    
    def getConfig(self):
        """Get the configuration object.
        
        Returns:
            Config: Configuration object
        """
        pass
    
    def logStatus(self):
        """Log brain status to console/log file."""
        pass

    # Convenience methods
    def getStats(self):
        """Get brain statistics as a tuple.
        
        Returns:
            tuple: (neuron_count, synapse_count, firing_count, 
                   avg_firing_rate, total_spikes)
        """
        pass

    def isInitialized(self):
        """Check if brain has been initialized.
        
        Returns:
            bool: True if brain is initialized
        """
        pass

class AgentBrainReference:
    """
    Agent brain interface connecting NLM brain to world.
    
    The AgentBrain class handles sensory transduction and motor decoding,
    acting as the bridge between the neural processing of the Brain and
    the external world.
    
    Examples:
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.enableCuriosity(True)
    """
    
    def initialize(self, world):
        """Initialize with world.
        
        Args:
            world (SimpleWorld): World object to connect to
        """
        pass
    
    def getSensoryInputSize(self):
        """Get expected sensory input size.
        
        Returns:
            int: Expected number of sensory input features
        """
        pass
    
    def getMotorOutputSize(self):
        """Get expected motor output size.
        
        Returns:
            int: Expected number of motor output actions
        """
        pass
    
    def processSensoryInput(self, percept):
        """Process sensory percept and inject into brain.
        
        Args:
            percept (SensoryPercept): Sensory percept to process
        """
        pass
    
    def decodeMotorCommand(self):
        """Decode brain motor activity into motor command.
        
        Returns:
            MotorCommand: Motor command decoded from brain activity
        """
        pass
    
    def applyRewardModulation(self, reward, predictedReward):
        """Apply reward-based neuromodulation.
        
        Args:
            reward (float): Actual reward received
            predictedReward (float): Predicted reward
        """
        pass
    
    def updateDevelopment(self, timestep):
        """Update development system.
        
        Args:
            timestep (float): Time step for development update
        """
        pass
    
    def getDevelopmentalStage(self):
        """Get current developmental stage.
        
        Returns:
            DevelopmentalStage: Current developmental stage
        """
        pass
    
    def getNeuromodulationLevel(self):
        """Get current neuromodulation level.
        
        Returns:
            float: Current neuromodulation level (dopamine)
        """
        pass
    
    def getCuriosityLevel(self):
        """Get curiosity level.
        
        Returns:
            float: Current curiosity level (0.0-1.0)
        """
        pass
    
    def getNoveltyLevel(self):
        """Get novelty level.
        
        Returns:
            float: Current novelty level (0.0-1.0)
        """
        pass
    
    def getPredictionError(self):
        """Get prediction error.
        
        Returns:
            float: Current prediction error signal
        """
        pass
    
    def reset(self):
        """Reset agent for new episode."""
        pass
    
    def getBrain(self):
        """Get the underlying brain object.
        
        Returns:
            Brain: Underlying brain object
        """
        pass
    
    # Subsystem control methods
    def enableRewardModulation(self, enable):
        """Enable/disable reward modulation system.
        
        Args:
            enable (bool): True to enable, False to disable
        """
        pass
    
    def enableStructuralPlasticity(self, enable):
        """Enable/disable structural plasticity.
        
        Args:
            enable (bool): True to enable, False to disable
        """
        pass
    
    def enableDevelopment(self, enable):
        """Enable/disable developmental changes.
        
        Args:
            enable (bool): True to enable, False to disable
        """
        pass
    
    def enableCuriosity(self, enable):
        """Enable/disable curiosity-driven exploration.
        
        Args:
            enable (bool): True to enable, False to disable
        """
        pass
    
    # Status methods
    def isRewardModulationEnabled(self):
        """Check if reward modulation is enabled.
        
        Returns:
            bool: True if reward modulation is enabled
        """
        pass
    
    def isStructuralPlasticityEnabled(self):
        """Check if structural plasticity is enabled.
        
        Returns:
            bool: True if structural plasticity is enabled
        """
        pass
    
    def isDevelopmentEnabled(self):
        """Check if development is enabled.
        
        Returns:
            bool: True if development is enabled
        """
        pass
    
    def isCuriosityEnabled(self):
        """Check if curiosity is enabled.
        
        Returns:
            bool: True if curiosity is enabled
        """
        pass
    
    def getStats(self):
        """Get agent statistics as a tuple.
        
        Returns:
            tuple: (curiosity, novelty, prediction_error, 
                   neuromodulation, developmental_stage)
        """
        pass

    def getSystemStatus(self):
        """Get human-readable system status string.
        
        Returns:
            str: Human-readable status string
        """
        pass

# ==============================================================================
# TYPE DEFINITIONS
# ==============================================================================

# Neuron and Cell Types
NeuronTypeReference = NeuronType
SynapseTypeReference = SynapseType
FiringStateReference = FiringState

# Identifiers
NeuronIdReference = NeuronId
SynapseIdReference = SynapseId
RegionIdReference = RegionId
PopulationIdReference = PopulationId

# Developmental Stages
DevelopmentalStageReference = DevelopmentalStage

# Action and Command Types
ActionTypeReference = ActionType
MotorCommandReference = MotorCommand
WorldObjectTypeReference = WorldObjectType

# ==============================================================================
# CONSTANTS
# ==============================================================================

# Invalid ID constants
INVALID_NEURON_ID = pynlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = pynlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = pynlm.INVALID_REGION_ID
INVALID_POPULATION_ID = pynlm.INVALID_POPULATION_ID

# ==============================================================================
# FACTORY FUNCTIONS
# ==============================================================================

# See __init__.py for detailed documentation

# ==============================================================================
# MODULE METADATA
# ==============================================================================

__all__ = [
    "Config",
    "Brain", 
    "AgentBrain",
    "SimpleWorld",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryPercept",
    "Action",
    "WorldObject",
    "ActionResult",
    "AgentBody",
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID", 
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
]

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"
