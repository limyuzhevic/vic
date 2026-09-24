# NLM Python Package
# This is the __init__.py file for the pynlm package
# Exports the main API functions and classes

from . import _pynlm

# Re-export the main classes and functions from the C++ extension
__version__ = _pynlm.__version__
__author__ = _pynlm.__author__
__description__ = _pynlm.__description__

# Core types and IDs
NeuronId = _pynlm.NeuronId
SynapseId = _pynlm.SynapseId
RegionId = _pynlm.RegionId
PopulationId = _pynlm.PopulationId

# Enums
NeuronType = _pynlm.NeuronType
SynapseType = _pynlm.SynapseType
DevelopmentalStage = _pynlm.DevelopmentalStage
FiringState = _pynlm.FiringState
ActionType = _pynlm.ActionType
MotorCommand = _pynlm.MotorCommand
WorldObjectType = _pynlm.WorldObjectType

# Type constants
SIMULATION_STEP = _pynlm.SIMULATION_STEP
TIMESTAMP = _pynlm.TIMESTAMP
SYNAPTIC_WEIGHT = _pynlm.SYNAPTIC_WEIGHT
MEMBRANE_POTENTIAL = _pynlm.MEMBRANE_POTENTIAL
FIRING_RATE = _pynlm.FIRING_RATE
TIMESTEP_DURATION = _pynlm.TIMESTEP_DURATION

# Memory system types
MEMORY_TYPE_WORKING = _pynlm.MEMORY_TYPE_WORKING
MEMORY_TYPE_EPISODIC = _pynlm.MEMORY_TYPE_EPISODIC
MEMORY_TYPE_ASSOCIATIVE = _pynlm.MEMORY_TYPE_ASSOCIATIVE

# Development stage constants
DEVELOPMENT_INITIAL = _pynlm.DEVELOPMENT_INITIAL
DEVELOPMENT_CRITICAL_PERIOD = _pynlm.DEVELOPMENT_CRITICAL_PERIOD
DEVELOPMENT_MATURATION = _pynlm.DEVELOPMENT_MATURATION
DEVELOPMENT_ADULT = _pynlm.DEVELOPMENT_ADULT
DEVELOPMENT_AGING = _pynlm.DEVELOPMENT_AGING

# Neuron type constants
NEURON_TYPE_EXCITATORY = _pynlm.NEURON_TYPE_EXCITATORY
NEURON_TYPE_INHIBITORY = _pynlm.NEURON_TYPE_INHIBITORY
NEURON_TYPE_MODULATORY = _pynlm.NEURON_TYPE_MODULATORY
NEURON_TYPE_SENSORY = _pynlm.NEURON_TYPE_SENSORY
NEURON_TYPE_MOTOR = _pynlm.NEURON_TYPE_MOTOR
NEURON_TYPE_INTERNAL = _pynlm.NEURON_TYPE_INTERNAL

# Invalid ID sentinels
INVALID_NEURON_ID = _pynlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = _pynlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = _pynlm.INVALID_REGION_ID
INVALID_POPULATION_ID = _pynlm.INVALID_POPULATION_ID

# Core classes
class Config(_pynlm.Config):
    """Configuration class for NLM system"""
    pass

class SensoryInput(_pynlm.SensoryInput):
    """Base class for sensory input"""
    pass

class Vision(_pynlm.Vision):
    """Vision sensory input representing visual input"""
    pass

class Audio(_pynlm.Audio):
    """Audio sensory input representing sound data"""
    pass

class InternalSignals(_pynlm.InternalSignals):
    """Internal signals sensory input for neural state"""
    pass

class Action(_pynlm.Action):
    """Action representation for motor output"""
    pass

class WorldObject(_pynlm.WorldObject):
    """World object representation"""
    pass

class AgentBody(_pynlm.AgentBody):
    """Agent body state and kinematics"""
    pass

class ActionResult(_pynlm.ActionResult):
    """Action result from world interaction"""
    pass

class SensoryPercept(_pynlm.SensoryPercept):
    """Sensory percept data combining all input modalities"""
    pass

class SimpleWorld(_pynlm.SimpleWorld):
    """Simple 2D world for NLM simulation"""
    pass

class Brain(_pynlm.Brain):
    """Central neural simulation brain class"""
    pass

class NeuralRegion(_pynlm.NeuralRegion):
    """Neural region containing multiple populations and local connectivity"""
    pass

class NeuralPopulation(_pynlm.NeuralPopulation):
    """Population of homogeneous neurons"""
    pass

class Neuron(_pynlm.Neuron):
    """Individual neuron implementing LIF dynamics"""
    pass

class Synapse(_pynlm.Synapse):
    """Synaptic connection between neurons with plasticity"""
    pass

class AgentBrain(_pynlm.AgentBrain):
    """Agent brain interface connecting NLM brain to world"""
    pass

# Convenience functions and factories
createDefaultConfig = _pynlm.createDefaultConfig
createBrain = _pynlm.createBrain
createSimpleWorld = _pynlm.createSimpleWorld
createAgentBrain = _pynlm.createAgentBrain

createDefaultBrainConfig = _pynlm.createDefaultBrainConfig
createSimpleAgentConfig = _pynlm.createSimpleAgentConfig
createWorldConfig = _pynlm.createWorldConfig

run_simulation = _pynlm.run_simulation
create_agent_system = _pynlm.create_agent_system

# System aliases for backward compatibility
SimulationStep = _pynlm.SimulationStep
Timestamp = _pynlm.Timestamp
TimestepDuration = _pynlm.TimestepDuration
SynapticWeight = _pynlm.SynapticWeight
MembranePotential = _pynlm.MembranePotential
FiringRate = _pynlm.FiringRate

# Utility functions
def get_version():
    """Get the version of pynlm"""
    return __version__

def get_author():
    """Get the author of pynlm"""
    return __author__

def get_description():
    """Get the description of pynlm"""
    return __description__

def list_api_features():
    """List all available features in the NLM Python bindings"""
    features = [
        "Core type IDs (NeuronId, SynapseId, RegionId, PopulationId)",
        "Enumeration types (NeuronType, SynapseType, DevelopmentalStage, etc.)",
        "Configuration management (Config class)",
        "Sensory processing (Vision, Audio, InternalSignals)",
        "Motor control (Action, MotorCommand)",
        "World simulation (WorldObject, AgentBody, SimpleWorld)",
        "Neural simulation (Brain, Neuron, Synapse, NeuralRegion)",
        "Agent interface (AgentBrain, SensoryPercept)",
        "Memory systems (WorkingMemory, EpisodicMemory)",
        "Prediction and cognition (NeuralPlanner, ConceptFormation)",
        "Development and plasticity systems",
        "Convenience factory functions and high-level APIs",
        "Serialization/deserialization utilities",
        "Comprehensive documentation and examples",
    ]
    return features

# Example usage
example_usage = """
# Basic usage example:
import pynlm

# Create a brain with configuration
config = pynlm.createDefaultBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Create an agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a simulation
simulation_result = pynlm.run_simulation(brain, world, steps=1000, verbose=True)
spikes, avg_rate = simulation_result
print(f"Total spikes: {spikes}, Average firing rate: {avg_rate:.2f} Hz")
"""

__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'SIMULATION_STEP', 'TIMESTAMP', 'SYNAPTIC_WEIGHT',
    'MEMBRANE_POTENTIAL', 'FIRING_RATE', 'TIMESTEP_DURATION',
    'MEMORY_TYPE_WORKING', 'MEMORY_TYPE_EPISODIC', 'MEMORY_TYPE_ASSOCIATIVE',
    'DEVELOPMENT_INITIAL', 'DEVELOPMENT_CRITICAL_PERIOD', 'DEVELOPMENT_MATURATION',
    'DEVELOPMENT_ADULT', 'DEVELOPMENT_AGING',
    'NEURON_TYPE_EXCITATORY', 'NEURON_TYPE_INHIBITORY', 'NEURON_TYPE_MODULATORY',
    'NEURON_TYPE_SENSORY', 'NEURON_TYPE_MOTOR', 'NEURON_TYPE_INTERNAL',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Classes
    'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld', 'Brain', 'NeuralRegion', 'NeuralPopulation', 'Neuron',
    'Synapse', 'AgentBrain',
    
    # Functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'createDefaultBrainConfig', 'createSimpleAgentConfig', 'createWorldConfig',
    'run_simulation', 'create_agent_system',
    
    # Utilities
    'get_version', 'get_author', 'get_description', 'list_api_features',
    'example_usage',
]