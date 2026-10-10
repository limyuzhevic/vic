"""
NLM Python API - Main interface module

This module provides the main API for the NLM (熙然) neural simulation framework
with Python bindings. It includes factory functions, exception classes, and
convenience utilities for working with the neural simulation system.

Usage:
    >>> import pynlm
    >>> brain = pynlm.createBrain()
    >>> brain.initialize()
    >>> brain.step(0, 0.0)
    >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
"""

from .bindings import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    SensoryInput,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # ID types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Enum types
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Exceptions
    NLMException,
    ConfigurationError,
    SimulationError,
    MemoryError,
)

# Factory functions
from .bindings import (
    createDefaultConfig,
    createBrain,
    createWorld,
    createAgentBrain,
)

# Utility functions
from .bindings import (
    normalizeVector,
    vectorToString,
    validateConfig,
    isPositiveNumber,
    isPositiveInt,
    getVersion,
)

# Module constants
from .bindings import (
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    DEFAULT_SIMULATION_TIMESTEP,
    DEFAULT_RANDOM_SEED,
    DEFAULT_NEURON_COUNT,
    DEFAULT_REGION_COUNT,
)

__all__ = [
    # Core classes
    "Config",
    "Brain",
    "AgentBrain",
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryInput",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    "SensoryPercept",
    
    # ID types
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Enum types
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Exceptions
    "NLMException",
    "ConfigurationError",
    "SimulationError",
    "MemoryError",
    
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createWorld",
    "createAgentBrain",
    
    # Utility functions
    "normalizeVector",
    "vectorToString",
    "validateConfig",
    "isPositiveNumber",
    "isPositiveInt",
    "getVersion",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    "DEFAULT_SIMULATION_TIMESTEP",
    "DEFAULT_RANDOM_SEED",
    "DEFAULT_NEURON_COUNT",
    "DEFAULT_REGION_COUNT",
    
    # Module info
    "__version__",
    "__author__",
    "__description__",
]

# Set module metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for the NLM neural simulation framework"

# Convenience function for easy testing
def quick_test():
    """Run a quick test of the NLM Python bindings.
    
    This function creates a minimal brain and runs a few steps to verify
    that the Python bindings are working correctly.
    
    Returns:
        dict: Test results with key information
    """
    try:
        # Create and initialize brain
        brain = createBrain()
        brain.initialize()
        
        # Run a few steps
        for step in range(10):
            brain.step(step, step * 0.001)
        
        # Collect results
        results = {
            "success": True,
            "neuron_count": brain.getTotalNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "regions": brain.getRegionCount(),
            "version": getVersion(),
        }
        
        return results
        
    except Exception as e:
        return {
            "success": False,
            "error": str(e),
            "error_type": type(e).__name__,
        }

# Example usage function
def example_usage():
    """Print a quick usage example for the NLM Python bindings."""
    example = '''
    Quick NLM Usage Example:
    ===========================
    
    >>> import pynlm
    
    # Create a brain with default configuration
    >>> brain = pynlm.createBrain()
    >>> brain.initialize()
    
    # Run a few simulation steps
    >>> for step in range(5):
    ...     brain.step(step, step * 0.001)
    
    # Check results
    >>> print(f"Neurons: {brain.getTotalNeuronCount()}")
    >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
    >>> print(f"Firing rate: {brain.getAverageFiringRate():.4f}")
    
    # Create an embodied agent in a world
    >>> world = pynlm.createWorld(10, 10)
    >>> agent = pynlm.createAgentBrain(brain)
    >>> agent.initialize(world)
    >>> world.setAgentStart(5, 5)
    
    # Main simulation loop
    >>> for step in range(50):
    ...     percept = world.getSensoryPercept()
    ...     agent.processSensoryInput(percept)
    ...     brain.step(step, step * 0.001)
    ...     action = agent.decodeMotorCommand()
    ...     world.applyMotorCommand(action, step * 0.001)
    
    # Get statistics
    >>> body = world.getAgentBody()
    >>> print(f"Final position: ({body.x}, {body.y})")
    >>> print(f"Final energy: {body.energy}")
    '''
    
    print(example)

if __name__ == "__main__":
    # If run as a script, print usage example
    example_usage()
    
    # Run quick test
    print("\nRunning quick test...")
    results = quick_test()
    
    if results["success"]:
        print("Quick test PASSED!")
        print(f"  Version: {results['version']}")
        print(f"  Neurons: {results['neuron_count']}")
        print(f"  Total spikes: {results['total_spikes']}")
        print(f"  Firing rate: {results['firing_rate']:.4f}")
        print(f"  Regions: {results['regions']}")
    else:
        print("Quick test FAILED!")
        print(f"  Error: {results['error']}")
        print(f"  Type: {results['error_type']}")