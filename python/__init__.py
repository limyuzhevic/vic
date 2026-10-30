"""
NLM (熙然) - Neural Learning Machine Python Package

A Python binding for the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

The NLM brain simulates biological learning and development through:
- Spiking neural networks with LIF dynamics
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction and cognitive systems
- Developmental processes
- Reward-based learning

Examples:
    >>> import pynlm
    >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
    >>> brain.initialize()
    >>> for i in range(100):
    ...     brain.step(i)
    >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
"""

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "nlm-project@example.com"
__license__ = "MIT"

# Export all public APIs
__all__ = [
    # Core classes
    "Config",
    "Brain",
    "AgentBrain",
    "SimpleWorld",
    "AgentBody",
    "SensoryPercept",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "WorldObject",
    "ActionResult",
    
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createAgentBrain",
    "createSimpleWorld",
    
    # Enumerations
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Identifiers
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Invalid sentinel values
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Type aliases for convenience
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    "ConfigValue",
    "SimulationStep",
    "Timestamp",
    "TimestepDuration",
    "NeuronIndex",
    "SynapseIndex",
    "PopulationIndex",
    "RegionIndex",
    "SynapticWeight",
    "MembranePotential",
    "FiringRate",
    "Delay",
    
    # Constants
    "CHECKPOINT_VERSION_MAJOR",
    "CHECKPOINT_VERSION_MINOR",
    "CHECKPOINT_VERSION_PATCH",
    "CHECKPOINT_MAGIC",
    "COMPRESSION_NONE",
    "COMPRESSION_FAST",
    "COMPRESSION_BALANCED",
    "COMPRESSION_BEST",
]

# Import core binding modules
try:
    from pynlm.core import (
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        AgentBody,
        SensoryPercept,
        Vision,
        Audio,
        InternalSignals,
        Action,
        WorldObject,
        ActionResult,
        createDefaultConfig,
        createBrain,
        createAgentBrain,
        createSimpleWorld,
    )
    from pynlm.enums import (
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        CompressionLevel,
        CheckpointSection,
    )
    from pynlm.types import (
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        SimulationStep,
        Timestamp,
        TimestepDuration,
        NeuronIndex,
        SynapseIndex,
        PopulationIndex,
        RegionIndex,
        SynapticWeight,
        MembranePotential,
        FiringRate,
        Delay,
        ConfigValue,
    )
    from pynlm.constants import (
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
        CHECKPOINT_VERSION_MAJOR,
        CHECKPOINT_VERSION_MINOR,
        CHECKPOINT_VERSION_PATCH,
        CHECKPOINT_MAGIC,
    )
    
    # Create shortcuts for common usage
    Config = Config
    Brain = Brain
    AgentBrain = AgentBrain
    SimpleWorld = SimpleWorld
    AgentBody = AgentBody
    SensoryPercept = SensoryPercept
    Vision = Vision
    Audio = Audio
    InternalSignals = InternalSignals
    Action = Action
    WorldObject = WorldObject
    ActionResult = ActionResult
    
except ImportError as e:
    # Fallback to direct C++ bindings if module structure not ready
    import sys
    import warnings
    warnings.warn(
        f"Failed to import pynlm submodules: {e}. "
        "Falling back to direct C++ bindings. "
        "Some features may not be available.",
        ImportWarning,
        stacklevel=2,
    )
    
    # Import directly from C++ bindings
    from .core import (
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        AgentBody,
        SensoryPercept,
        Vision,
        Audio,
        InternalSignals,
        Action,
        WorldObject,
        ActionResult,
        createDefaultConfig,
        createBrain,
        createAgentBrain,
        createSimpleWorld,
    )
    from .enums import (
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
    )
    from .types import (
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        SimulationStep,
        Timestamp,
        TimestepDuration,
        NeuronIndex,
        SynapseIndex,
        PopulationIndex,
        RegionIndex,
        SynapticWeight,
        MembranePotential,
        FiringRate,
        Delay,
        ConfigValue,
    )
    from .constants import (
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )

# Add module documentation
__doc__ += """

Quick Start:
============

Basic Brain Simulation:
----------------------
>>> import pynlm
>>> 
>>> # Create a brain with default configuration
>>> config = pynlm.createDefaultConfig()
>>> brain = pynlm.createBrain(config)
>>> 
>>> # Initialize the brain
>>> brain.initialize()
>>> 
>>> # Run simulation
>>> for step in range(100):
...     brain.step(step)
>>> 
>>> # Get statistics
>>> print(f"Neurons: {brain.getTotalNeuronCount()}")
>>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
>>> print(f"Firing neurons: {brain.getFiringNeuronCount()}")

Agent-World Simulation:
----------------------
>>> import pynlm
>>> 
>>> # Create brain, world, and agent
>>> config = pynlm.createDefaultConfig()
>>> brain = pynlm.createBrain(config)
>>> brain.initialize()
>>> 
>>> world = pynlm.createSimpleWorld()
>>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
>>> world.reset()
>>> 
>>> agent = pynlm.createAgentBrain(brain)
>>> agent.initialize(world)
>>> 
>>> # Enable learning subsystems
>>> agent.enableRewardModulation(True)
>>> agent.enableCuriosity(True)
>>> agent.enableDevelopment(True)
>>> 
>>> # Run simulation
>>> for step in range(500):
...     world.update(0.1)
...     agent.processSensoryInput(world.getSensoryPercept())
...     brain.step(step)
...     action = agent.decodeMotorCommand()
...     world.applyMotorCommand(action, world.getSimulationTime())
...     
>>> print(f"Simulation complete!")

Error Handling:
--------------
NLM provides several Python-specific exceptions:

>>> try:
...     config.loadFromFile("nonexistent.json")
... except pynlm.RuntimeError as e:
...     print(f"Configuration error: {e}")

Type Checking:
-------------
The Python API includes type checking for safety:

>>> config = pynlm.createDefaultConfig()
>>> config.set("brain.neuron_count", 1000)  # OK
>>> config.set("brain.neuron_count", "not_a_number")  # Raises ValueError

Advanced Usage:
--------------
See the documentation for advanced usage patterns including:
- Checkpoint management
- Visualization APIs
- Performance profiling
- Custom configuration
- Event-driven architectures
"""

# Python 3.7+ type hints support
try:
    from typing import Dict, List, Optional, Union, Any, Callable
    import sys
    
    if sys.version_info >= (3, 8):
        from typing import TypedDict
        
        class ConfigDict(TypedDict, total=False):
            """Type definition for configuration dictionaries"""
            neuron_count: int
            connection_probability: float
            simulation_timestep: float
            random_seed: int
        
    # Add type hints for better IDE support
    ConfigDict = Dict[str, Any]
    
except ImportError:
    pass

# Performance optimizations
import sys
if hasattr(sys, 'setdlopenflags'):
    # Optimize module loading
    import os
    os.environ.setdefault('PYNLM_OPTIMIZED', '1')

# Version compatibility check
import pkg_resources
try:
    version = pkg_resources.get_distribution("pynlm").version
    if version != __version__:
        import warnings
        warnings.warn(
            f"Version mismatch: Package version {version}, "
            f"module version {__version__}. "
            "This may indicate installation issues.",
            UserWarning,
            stacklevel=2,
        )
except Exception:
    pass

# Final documentation
__all__ += [
    "ConfigDict",  # Only available if typing is available
]

print(f"NLM Python bindings loaded (version {__version__})")