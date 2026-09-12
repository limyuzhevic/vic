"""
NLM (Neural Learning Machine) Python Bindings

A Python binding for the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
"""

from .pynlm import (
    # Version
    __version__,
    
    # Core classes
    Config,
    createDefaultConfig,
    Brain,
    createBrain,
    SimpleWorld,
    createSimpleWorld,
    AgentBrain,
    createAgentBrain,
    
    # Data types
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
    
    # Sensory and Action
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    SensoryPercept,
    
    # Agent components
    AgentBody,
    ActionResult,
)

# Import advanced API for power users
try:
    from .advanced_api import (
        AdvancedBrainManager,
        ExperimentRunner,
        VisualizationHelper,
        quick_test_simulation,
        benchmark_comparison,
    )
    
    # Add advanced classes to module namespace
    __all__ = [
        # Version
        '__version__',
        
        # Core classes
        'Config',
        'createDefaultConfig',
        'Brain',
        'createBrain',
        'SimpleWorld',
        'createSimpleWorld',
        'AgentBrain',
        'createAgentBrain',
        
        # Data types
        'NeuronId',
        'SynapseId',
        'RegionId',
        'PopulationId',
        'NeuronType',
        'SynapseType',
        'DevelopmentalStage',
        'FiringState',
        'ActionType',
        'MotorCommand',
        'WorldObjectType',
        
        # Sensory and Action
        'SensoryInput',
        'Vision',
        'Audio',
        'InternalSignals',
        'Action',
        'WorldObject',
        'SensoryPercept',
        
        # Agent components
        'AgentBody',
        'ActionResult',
        
        # Advanced API
        'AdvancedBrainManager',
        'ExperimentRunner',
        'VisualizationHelper',
        'quick_test_simulation',
        'benchmark_comparison',
    ]
    
    ADVANCED_API_AVAILABLE = True
    
except ImportError:
    ADVANCED_API_AVAILABLE = False
    __all__ = [
        # Version
        '__version__',
        
        # Core classes
        'Config',
        'createDefaultConfig',
        'Brain',
        'createBrain',
        'SimpleWorld',
        'createSimpleWorld',
        'AgentBrain',
        'createAgentBrain',
        
        # Data types
        'NeuronId',
        'SynapseId',
        'RegionId',
        'PopulationId',
        'NeuronType',
        'SynapseType',
        'DevelopmentalStage',
        'FiringState',
        'ActionType',
        'MotorCommand',
        'WorldObjectType',
        
        # Sensory and Action
        'SensoryInput',
        'Vision',
        'Audio',
        'InternalSignals',
        'Action',
        'WorldObject',
        'SensoryPercept',
        
        # Agent components
        'AgentBody',
        'ActionResult',
    ]

# Convenience functions
def print_version():
    """Print version information."""
    print(f"NLM Python Bindings v{__version__}")
    if ADVANCED_API_AVAILABLE:
        print("Advanced API is available")
    else:
        print("Advanced API is not available (import error)")
    
    # Print basic system info
    print(f"Python version: {sys.version}")
    print(f"Platform: {sys.platform}")

# Initialize the bindings when module is imported
# The actual initialization happens in the C++ extension module

__all__ += ['print_version']
