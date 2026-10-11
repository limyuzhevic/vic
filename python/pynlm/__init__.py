from pynlm.core.config import Config, createDefaultConfig
from pynlm.core.types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, WorldObjectType
)
from pynlm.brain import Brain, createBrain
from pynlm.agent import AgentBrain, createAgentBrain
from pynlm.world import SimpleWorld, createSimpleWorld
from pynlm.sensory import Vision, Audio, InternalSignals, SensoryInput, createVision, createAudio, createInternalSignals
from pynlm.motor import Action, createAction, createActionFromType, createActionWithParameters, MotorCommand
from pynlm.objects import WorldObject, ActionResult, SensoryPercept, createWorldObject, createActionResult
from pynlm.advanced import (
    PerformanceMonitor, AdvancedBrainFeatures, AdvancedWorldFeatures, 
    ExperimentManager, ProfilingLevel, create_performance_monitor,
    create_advanced_brain_features, create_advanced_world_features,
    create_experiment_manager
)
from pynlm.performance import (
    ProfilingLevel, PerformanceMetrics, TimingProfiler, SystemProfiler,
    NLMAnalytics, OptimizationTools, create_performance_monitor as create_analytics,
    create_timing_profiler, create_system_profiler, profile_operation,
    profile_step, profile_simulation
)

__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    
    # Types and enumerations
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'WorldObjectType',
    
    # Motor system
    'MotorCommand', 'Action',
    
    # Sensory systems
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    
    # Objects
    'WorldObject', 'ActionResult', 'SensoryPercept',
    
    # Advanced features
    'PerformanceMonitor', 'AdvancedBrainFeatures', 'AdvancedWorldFeatures',
    'ExperimentManager', 'ProfilingLevel',
    
    # Performance and analytics
    'PerformanceMetrics', 'TimingProfiler', 'SystemProfiler',
    'NLMAnalytics', 'OptimizationTools',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createAgentBrain',
    'createSimpleWorld', 'createVision', 'createAudio', 'createInternalSignals',
    'createAction', 'createActionFromType', 'createActionWithParameters',
    'createWorldObject', 'createActionResult',
    'create_performance_monitor', 'create_advanced_brain_features',
    'create_advanced_world_features', 'create_experiment_manager',
    'create_analytics', 'create_timing_profiler', 'create_system_profiler',
    'profile_operation', 'profile_step', 'profile_simulation'
]

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# For backward compatibility
import warnings
warnings.filterwarnings('deprecated', message="Direct import of pynlm is deprecated, use specific submodules instead")

# Special constants (to be defined by C++ bindings)
INVALID_NEURON_ID = 0
INVALID_SYNAPSE_ID = 0
INVALID_REGION_ID = 0
INVALID_POPULATION_ID = 0

# Export special constants
__all__.extend([
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
])



