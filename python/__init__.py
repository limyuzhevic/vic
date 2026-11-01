"""
NLM (Neural Learning Machine) - Python Bindings and Tools
A neural system that learns from experience through spiking neural networks.
"""

__version__ = "0.1.0"

from .pynlm import (
    # Core types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Core classes
    Config,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # Main classes
    SimpleWorld,
    Brain,
    AgentBrain,
    
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    
    # Invalid ID constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

__all__ = [
    # Core types
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
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
    
    # Main classes
    "SimpleWorld",
    "Brain",
    "AgentBrain",
    
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    
    # Invalid ID constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
]

# Import convenience functions
from . import tools
from . import visualization
from . import experiments

# Export major tool classes
from .tools import (
    NLMCli,
    ConfigGenerator,
    BrainRunner,
    Visualizer,
    BenchmarkTool,
    Explorer,
)

# Export major visualization modules
from .visualization import (
    BrainVisualizer,
    SimplePlotter,
    GraphRenderer,
)

# Export major experiment modules
from .experiments import (
    ExperimentRunner,
    Phase6Experiment,
)

# Setup logging
import logging

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

logger = logging.getLogger(__name__)