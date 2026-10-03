"""
NLM (Neural Learning Machine) Python Bindings

A Python interface to the NLM (Neural Learning Machine) C++ neural simulation framework.
This module provides Python bindings for creating and simulating artificial brains
with spiking neurons, learning, memory, and adaptive behavior.

The NLM brain operates as a closed-loop system that:
- Receives sensory input from the environment
- Processes information through neural networks
- Makes decisions and produces actions
- Learns from rewards and prediction errors
- Adapts and develops over time

Example usage:

    >>> import pynlm
    >>> 
    >>> # Create a default brain configuration
    >>> config = pynlm.createDefaultConfig()
    >>> 
    >>> # Create and initialize a brain
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> 
    >>> # Run some simulation steps
    >>> for step in range(100):
    ...     brain.step(step)
    ...     print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    >>> 
    >>> # Create a world and agent for interaction
    >>> world = pynlm.createSimpleWorld()
    >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    >>> agent = pynlm.createAgentBrain(brain)
    >>> agent.initialize(world)
    >>> 
    >>> # Run complete agent simulation
    >>> for step in range(500):
    ...     world.update(0.1)
    ...     agent.processSensoryInput(world.getSensoryPercept())
    ...     brain.step(step)
    ...     action = agent.decodeMotorCommand()
    ...     world.applyMotorCommand(action, world.getSimulationTime())

The NLM framework supports:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Neuromodulation (dopamine, curiosity, novelty)
- Working, episodic, and associative memory
- Developmental stages and plasticity modulation
- Sensorimotor loops and environmental interaction
- Reinforcement learning and prediction error
- Concept formation and neural planning
- Checkpoint persistence and replay

For more information, visit: https://github.com/nlm-project/nlm
"""

import sys
from typing import Dict, Any, Optional, Union, List, Tuple

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "nlm-authors@example.com"
__license__ = "MIT"

# Python language version requirement
if sys.version_info < (3, 8):
    raise ImportError(
        "pynlm requires Python 3.8 or higher. "
        f"You are running Python {sys.version_info.major}.{sys.version_info.minor}"
    )

# Forward declarations for type hints
class Config:
    pass

class Brain:
    pass

class AgentBrain:
    pass

class SimpleWorld:
    pass

class SensoryInput:
    pass

class Action:
    pass

class NeuronId:
    pass

class SynapseId:
    pass

class RegionId:
    pass

class PopulationId:
    pass

# Global constants
INVALID_NEURON_ID = None
INVALID_SYNAPSE_ID = None
INVALID_REGION_ID = None
INVALID_POPULATION_ID = None

# Type aliases for better documentation
NeuronType = Union[int, str]
SynapseType = Union[int, str]
ActionType = Union[int, str]
MotorCommand = Union[int, str]
WorldObjectType = Union[int, str]

# Export public API
__all__ = [
    # Core classes
    "Config",
    "Brain", 
    "AgentBrain",
    "SimpleWorld",
    "SensoryInput",
    "Action",
    
    # ID types
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Enumerations
    "NeuronType",
    "SynapseType", 
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    "DevelopmentalStage",
    "FiringState",
    "WorldObjectType",
    
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createAgentBrain", 
    "createSimpleWorld",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Version and metadata
    "__version__",
    "__author__",
    "__license__",
    
    # Utility functions
    "run_brain_simulation",
    "run_agent_simulation",
    "create_complete_brain",
]

# C extension module
try:
    from pynlm._pynlm import (
        # Core classes
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        SensoryInput,
        Action,
        
        # ID types
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        
        # Enumerations
        NeuronType,
        SynapseType,
        ActionType,
        MotorCommand,
        WorldObjectType,
        DevelopmentalStage,
        FiringState,
        
        # Factory functions
        createDefaultConfig,
        createBrain,
        createAgentBrain,
        createSimpleWorld,
        
        # Constants
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
    # Re-export all symbols from C extension
    for symbol in __all__:
        if hasattr(sys.modules[__name__], symbol):
            # Already imported by C extension
            pass
        elif hasattr(sys.modules['pynlm._pynlm'], symbol):
            globals()[symbol] = getattr(sys.modules['pynlm._pynlm'], symbol)
    
except ImportError:
    # C extension not available - provide helpful error message
    class _MissingCExtension(Exception):
        """Raised when the C extension module is not available."""
        pass
    
    def _raise_extension_error(*args, **kwargs):
        raise _MissingCExtension(
            "pynlm C extension not available. "
            "Please install pynlm with: pip install pynlm"
        )
    
    # Create placeholder classes that raise helpful errors
    for symbol in ["Config", "Brain", "AgentBrain", "SimpleWorld", "SensoryInput", "Action",
                  "NeuronId", "SynapseId", "RegionId", "PopulationId", "NeuronType",
                  "SynapseType", "ActionType", "MotorCommand", "WorldObjectType",
                  "DevelopmentalStage", "FiringState"]:
        globals()[symbol] = _raise_extension_error
    
    for symbol in ["createDefaultConfig", "createBrain", "createAgentBrain", 
                   "createSimpleWorld"]:
        globals()[symbol] = _raise_extension_error
    
    for symbol in ["INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID", "INVALID_POPULATION_ID"]:
        globals()[symbol] = None

# Version check
def check_version(version: str) -> bool:
    """
    Check if the current version meets the minimum required version.
    
    Args:
        version: Version string to check against (e.g., "0.1.0")
        
    Returns:
        True if current version >= required version, False otherwise
    """
    from packaging import version as pkg_version
    
    current = pkg_version.parse(__version__)
    required = pkg_version.parse(version)
    return current >= required

# Utility functions
def run_brain_simulation(brain: Brain, num_steps: int, initialize: bool = True) -> Dict[str, Any]:
    """
    Run a simple brain simulation and return statistics.
    
    Args:
        brain: The brain instance to simulate
        num_steps: Number of simulation steps to run
        initialize: Whether to initialize the brain before simulation
        
    Returns:
        Dictionary containing simulation statistics
    """
    if initialize:
        brain.initialize()
    
    stats = {
        'steps': num_steps,
        'final_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'ei_ratio': brain.getExcitationInhibitionRatio(),
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
    }
    
    for step in range(num_steps):
        brain.step(step)
    
    return stats


def run_agent_simulation(brain: Brain, world: SimpleWorld, agent: AgentBrain, 
                        num_steps: int, enable_learning: bool = True) -> Dict[str, Any]:
    """
    Run a complete agent simulation with brain-world interaction.
    
    Args:
        brain: The brain instance
        world: The world instance
        agent: The agent brain interface
        num_steps: Number of simulation steps
        enable_learning: Whether to enable learning subsystems
        
    Returns:
        Dictionary containing simulation statistics
    """
    brain.initialize()
    agent.initialize(world)
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    stats = {
        'steps': num_steps,
        'final_firing': brain.getFiringNeuronCount(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
        'dev_stage': brain.getDevelopmentalStage(),
    }
    
    return stats


def create_complete_brain(config_dict: Optional[Dict[str, Any]] = None) -> Tuple[Brain, AgentBrain, SimpleWorld]:
    """
    Create a complete brain-world-agent setup with default configuration.
    
    Args:
        config_dict: Optional dictionary of configuration parameters
        
    Returns:
        Tuple of (brain, agent, world)
    """
    config = createDefaultConfig()
    
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value)
    
    brain = createBrain(config)
    world = createSimpleWorld()
    agent = createAgentBrain(brain)
    
    return brain, agent, world


def save_simulation_results(filename: str, results: Dict[str, Any]) -> None:
    """
    Save simulation results to a JSON file.
    
    Args:
        filename: Path to save the results file
        results: Dictionary containing simulation results
    """
    import json
    
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2, default=str)


def load_simulation_results(filename: str) -> Dict[str, Any]:
    """
    Load simulation results from a JSON file.
    
    Args:
        filename: Path to the results file
        
    Returns:
        Dictionary containing simulation results
    """
    import json
    
    with open(filename, 'r') as f:
        return json.load(f)


# Progress reporting for long simulations
def simulate_with_progress(brain: Brain, num_steps: int, report_interval: int = 100) -> Dict[str, Any]:
    """
    Run simulation with periodic progress reporting.
    
    Args:
        brain: Brain instance to simulate
        num_steps: Number of steps to run
        report_interval: How often to report progress (in steps)
        
    Returns:
        Dictionary with final statistics
    """
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
        
        if report_interval and step % report_interval == 0:
            print(f"Step {step}/{num_steps}: "
                  f"{brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    return {
        'steps': num_steps,
        'final_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
    }
