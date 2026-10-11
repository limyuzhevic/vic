# NLM Python Module
# Python bindings for the Neural Learning Machine framework

__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "contact@nlm-project.org"
__license__ = "MIT"
__copyright__ = "Copyright (c) 2026 NLM Authors"

# Configuration
from .config import (
    Config,
    ConfigSchema,
    ConfigValidator,
    create_default_config,
    load_config_from_file,
    load_config_from_env,
)

from .advanced_config import (
    AdvancedConfigManager,
    ValidationSeverity,
    ValidationResult,
)

# Brain wrapper
from .brain import (
    Brain,
    ProgressCallback,
)

# World wrapper
from .world import (
    SimpleWorld,
)

# Agent wrapper
from .agent import (
    AgentBrain,
)

# Performance profiling
ProfileLevel,
ProfileSample,
ProfileResult,
AdvancedProfiler,
get_profiler,
start_profiling,
stop_profiling,
start_real_time_monitoring,
stop_real_time_monitoring,
profile_function,
profile_operation,

# Core creation functions
create_brain = Brain
create_simple_world = SimpleWorld
create_agent_brain = AgentBrain

# Constants from C++ bindings (simplified)
INVALID_NEURON_ID = None
INVALID_SYNAPSE_ID = None
INVALID_REGION_ID = None
INVALID_POPULATION_ID = None

# Type aliases
SimulationStep = int
Timestamp = float
TimestepDuration = float
NeuronIndex = int
SynapseIndex = int
PopulationIndex = int
RegionIndex = int
SynapticWeight = float
MembranePotential = float
FiringRate = float
Delay = int

# Basic C++ types (simplified)
class NeuronId:
    def __init__(self, value=0):
        self.value = value

class SynapseId:
    def __init__(self, value=0):
        self.value = value

class RegionId:
    def __init__(self, value=0):
        self.value = value

class PopulationId:
    def __init__(self, value=0):
        self.value = value

class NeuronType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

class SynapseType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

class DevelopmentalStage:
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

class FiringState:
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

class ActionType:
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
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class WorldObjectType:
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# Set up the module
__all__ = [
    # Core classes
    "Config",
    "Brain",
    "SimpleWorld",
    "AgentBrain",
    
    # Advanced configuration classes
    "AdvancedConfigManager",
    "ValidationSeverity",
    "ValidationResult",
    
    # Schema classes
    "ConfigSchema",
    "ConfigValidator",
    
    # Logger classes
    "LogLevel",
    "StructuredLogger",
    
    # Callback classes
    "ProgressCallback",
    
    # Advanced configuration classes
    "AdvancedConfigManager",
    "ValidationSeverity",
    "ValidationResult",
    
    # Performance profiling classes
    "ProfileLevel",
    "ProfileSample",
    "ProfileResult",
    "AdvancedProfiler",
    
    # Batch processing classes
    "BatchStatus",
    "ProcessingMode",
    "SimulationTask",
    "BatchResult",
    "BatchStatistics",
    "BatchProcessor",
    
    # Simulation control classes
    "SimulationState",
    "ControlMode",
    "Checkpoint",
    "ControlCommand",
    "AdvancedSimulationController",
    
    # Multi-language integration classes
    "Platform",
    "LanguageBinding",
    "IntegrationConfig",
    "IntegrationResult",
    "MultiLanguageIntegration",
    
    # Functions
    "create_default_config",
    "load_config_from_file",
    "load_config_from_env",
    
    # Convenience functions
    "create_brain",
    "create_simple_world",
    "create_agent_brain",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Type aliases
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
    
    # C++ types
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
    
    # Module info
    "__version__",
    "__author__",
    "__email__",
    "__license__",
    "__copyright__",
]