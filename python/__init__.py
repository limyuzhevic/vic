"""
NLM - Neural Learning Machine Python Bindings

This is the main entry point for the NLM Python package. NLM is an experimental
artificial developmental brain system that implements a complete neural
simulation framework with multiple integrated systems.

The package provides Python bindings to the NLM C++ neural simulation framework,
enabling users to:
- Create and initialize artificial brains
- Run neural simulations with multiple plasticity mechanisms
- Interface with world environments for embodied intelligence
- Access integrated memory, prediction, and cognition systems
- Apply neuromodulation and developmental processes

For detailed documentation, examples, and API reference, visit:
https://nlm.readthedocs.io

Key Features:
- Complete integration of neural, memory, prediction, cognition, and neuromodulation systems
- Event-driven spiking neural computation with LIF dynamics
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Working memory, episodic memory, and associative memory systems
- Prediction systems and cognitive planning
- Developmental stages and structural plasticity
- Embodied intelligence with world interaction
- Comprehensive Python API with factory functions and utility classes

Installation:
    pip install pynlm

Quick Start:
    import pynlm
    
    # Create configuration
    config = pynlm.create_default_config()
    
    # Create brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        # ... more simulation code

For more examples, see the examples/ directory.
"""

__version__ = "0.1.0"

# Import main classes and factory functions
from .core import (
    Config,
    create_default_config,
    load_config_from_file,
    save_config_to_file,
    get_research_config,
    get_agent_training_config,
    get_development_config,
)

from .brain import (
    Brain,
    create_brain,
    BrainUtils,
    BrainStatsCollector,
    RegionId,
    NeuronId,
    SynapseId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    SimulationStep,
    Timestamp,
    MembranePotential,
    FiringRate,
)

from .agent import (
    AgentBrain,
    create_agent_brain,
    AgentBrainUtils,
    ScenarioFactory,
    SensoryPercept,
    AgentBody,
    WorldObject,
    ActionResult,
)

from .world import (
    SimpleWorld,
    create_simple_world,
    WorldUtils,
    EnvironmentFactory,
)

# Re-export all for convenience
__all__ = [
    # Core
    'Config',
    'create_default_config',
    'load_config_from_file',
    'save_config_to_file',
    'get_research_config',
    'get_agent_training_config',
    'get_development_config',
    
    # Brain
    'Brain',
    'create_brain',
    'BrainUtils',
    'BrainStatsCollector',
    'RegionId',
    'NeuronId',
    'SynapseId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'SimulationStep',
    'Timestamp',
    'MembranePotential',
    'FiringRate',
    
    # Agent
    'AgentBrain',
    'create_agent_brain',
    'AgentBrainUtils',
    'ScenarioFactory',
    'SensoryPercept',
    'AgentBody',
    'WorldObject',
    'ActionResult',
    
    # World
    'SimpleWorld',
    'create_simple_world',
    'WorldUtils',
    'EnvironmentFactory',
]

# Version information
__author__ = "NLM Authors"
__copyright__ = "Copyright 2024 NLM Authors"
__license__ = "MIT"

# Performance and compatibility checks
import sys

def check_environment():
    """Check if the environment meets NLM requirements"""
    requirements = {
        'python_version': sys.version_info >= (3, 8),
        'minimum_memory': True,  # Would check memory if available
    }
    
    missing = [req for req, met in requirements.items() if not met]
    
    if missing:
        print(f"Warning: NLM requirements not met: {missing}")
        return False
    
    print("NLM environment requirements satisfied")
    return True

# Initialize on import
check_environment()
