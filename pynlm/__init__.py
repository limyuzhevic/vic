"""Main package module for NLM Python bindings."""

# Package metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import core functionality from bindings
from .bindings import (
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    
    # Debug and analysis tools
    getBrainSystemInfo,
    runIntegrationTest,
    compareBrains,
    validateConfig,
    isValidBrainConfig,
    
    # Exception types
    RuntimeError,
    InvalidArgumentError,
    OutOfRangeError,
    
    # Enum classes
    NeuronType,
    DevelopmentalStage,
    ActionType,
    WorldObjectType,
    MotorCommand,
    SynapseType,
    FiringState,
    
    # ID types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Import CLI
from .cli import cli

# Package-level utilities
def __all__():
    """Return list of all public exports."""
    return [
        # Factory functions
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain',
        
        # Debug tools
        'getBrainSystemInfo',
        'runIntegrationTest',
        'compareBrains',
        'validateConfig',
        'isValidBrainConfig',
        
        # Exceptions
        'RuntimeError',
        'InvalidArgumentError',
        'OutOfRangeError',
        
        # Enums
        'NeuronType',
        'DevelopmentalStage',
        'ActionType',
        'WorldObjectType',
        'MotorCommand',
        'SynapseType',
        'FiringState',
        
        # ID types
        'NeuronId',
        'SynapseId',
        'RegionId',
        'PopulationId',
        
        # Constants
        'INVALID_NEURON_ID',
        'INVALID_SYNAPSE_ID',
        'INVALID_REGION_ID',
        'INVALID_POPULATION_ID',
        
        # CLI
        'cli',
    ]

# Utility functions
def get_version():
    """Get the version of the NLM Python bindings."""
    return __version__

def get_author():
    """Get the author of the NLM Python bindings."""
    return __author__

def get_license():
    """Get the license of the NLM Python bindings."""
    return __license__

# Example usage documentation
example_usage = '''
Example usage of NLM Python bindings:

>>> import pynlm

>>> # Create a default configuration
>>> config = pynlm.createDefaultConfig()

>>> # Create a brain
>>> brain = pynlm.createBrain(config)
>>> brain.initialize()

>>> # Run simulation
>>> for step in range(100):
...     brain.step(step)

>>> # Get statistics
>>> print(f"Total neurons: {brain.getTotalNeuronCount()}")
>>> print(f"Total spikes: {brain.getTotalSpikeCount()}")

>>> # Create world and agent
>>> world = pynlm.createSimpleWorld()
>>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
>>> world.reset()

>>> agent = pynlm.createAgentBrain(brain)
>>> agent.initialize(world)

>>> # Run complete agent simulation
>>> for step in range(1000):
...     world.update(0.1)
...     agent.processSensoryInput(world.getSensoryPercept())
...     brain.step(step)
...     world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

>>> # Access advanced features
>>> info = pynlm.getBrainSystemInfo(brain)
>>> results = pynlm.runIntegrationTest()
>>> comparison = pynlm.compareBrains(brain1, brain2)

Advanced Features:
- Memory system integration (working memory, episodic memory, etc.)
- Prediction and cognition systems
- Neuromodulation (dopamine, curiosity, novelty, etc.)
- Development system
- Debugging and analysis tools
- Experiment utilities
- Configuration validation
'''

# Help function
def help():
    """Show help for the NLM Python bindings."""
    return example_usage

# Initialize package
print(f"NLM Python bindings {__version__} loaded successfully.")
print("Type 'import pynlm' and 'pynlm.help()' for examples.")