"""
NLM (Neural Learning Machine) Python Package

This package provides enhanced Python bindings for the NLM C++ neural simulation framework.
It includes both direct C++ bindings and enhanced Python utilities for easier usage.

Main Features:
- Direct access to NLM C++ classes and functionality
- Enhanced memory systems (working, episodic, associative)
- Neuromodulation support
- Prediction systems
- Experiment running utilities
- Developmental stage tracking
- Comprehensive error handling and validation

Example Usage:
    import pynlm as nlm
    
    # Create a brain with default configuration
    brain = nlm.create_brain()
    
    # Run a simple experiment
    results = brain.run_tutorial(100)
    
    # Access memory systems
    memory = brain.memory_integrator
    memory.store_working_memory("Learning progress", 0.8)
    
    # Run advanced experiments
    advanced_results = brain.run_advanced_experiment({
        'neuron_count': 2000,
        'neuromod_curiosity': 0.8
    })
"""

# Import and expose key classes and functions from C++ bindings
import sys
import os

# Add the directory to sys.path to allow relative imports
this_dir = os.path.dirname(os.path.abspath(__file__))
if this_dir not in sys.path:
    sys.path.insert(0, this_dir)

# Try to import from the compiled C++ module
try:
    # First, try to import the compiled module directly
    import pynlm as _pynlm
    HAS_PYNLM = True
except ImportError:
    # If that fails, try to find the module elsewhere
    # This is a fallback for development scenarios
    print("Warning: Could not find compiled pynlm module")
    HAS_PYNLM = False

# Core C++ classes (will be populated after successful import)
Config = None
Brain = None
SimpleWorld = None
AgentBrain = None

# Sensory classes
SensoryInput = None
Vision = None
Audio = None
InternalSignals = None

# Motor classes
Action = None
WorldObject = None
AgentBody = None
ActionResult = None

# Agent classes
SensoryPercept = None

# C++ type definitions
NeuronId = None
SynapseId = None
RegionId = None
PopulationId = None
NeuronType = None
SynapseType = None
DevelopmentalStage = None
FiringState = None
ActionType = None
MotorCommand = None
WorldObjectType = None
SimulationStep = None
Timestamp = None

# Invalid ID constants
INVALID_NEURON_ID = None
INVALID_SYNAPSE_ID = None
INVALID_REGION_ID = None
INVALID_POPULATION_ID = None

# Import enhanced Python functionality
from .nlm_core import (
    WorkingMemoryEntry,
    EpisodicEntry,
    AssociativeMemoryEntry,
    NeuromodulationState,
    PredictionEntry,
    DevelopmentalStageInfo,
    NLMUtils,
    MemoryIntegrator,
    ExperimentRunner,
    NLM,
    print_banner,
    create_default_config,
    create_brain,
    create_simple_world,
)

# Package metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"

# System initialization
print(f"NLM Python Package v{__version__} initialized")
print(f"Author: {__author__}")
print(f"License: {__license__}")
print("Enhanced Python bindings for Neural Learning Machine (NLM)")
print("For documentation and examples, see documentation/ directory")
print("-" * 60)

# Initialize core classes from C++ bindings if available
def _init_core_classes():
    """Initialize core classes from C++ bindings if module is available"""
    global Config, Brain, SimpleWorld, AgentBrain
    global SensoryInput, Vision, Audio, InternalSignals
    global Action, WorldObject, AgentBody, ActionResult
    global SensoryPercept
    global NeuronId, SynapseId, RegionId, PopulationId
    global NeuronType, SynapseType, DevelopmentalStage
    global FiringState, ActionType, MotorCommand, WorldObjectType
    global SimulationStep, Timestamp
    global INVALID_NEURON_ID, INVALID_SYNAPSE_ID
    global INVALID_REGION_ID, INVALID_POPULATION_ID
    
    if HAS_PYNLM and _pynlm:
        # Map C++ classes to Python
        Config = _pynlm.Config
        Brain = _pynlm.Brain
        SimpleWorld = _pynlm.SimpleWorld
        AgentBrain = _pynlm.AgentBrain
        
        SensoryInput = _pynlm.SensoryInput
        Vision = _pynlm.Vision
        Audio = _pynlm.Audio
        InternalSignals = _pynlm.InternalSignals
        
        Action = _pynlm.Action
        WorldObject = _pynlm.WorldObject
        AgentBody = _pynlm.AgentBody
        ActionResult = _pynlm.ActionResult
        
        SensoryPercept = _pynlm.SensoryPercept
        
        NeuronId = _pynlm.NeuronId
        SynapseId = _pynlm.SynapseId
        RegionId = _pynlm.RegionId
        PopulationId = _pynlm.PopulationId
        
        NeuronType = _pynlm.NeuronType
        SynapseType = _pynlm.SynapseType
        DevelopmentalStage = _pynlm.DevelopmentalStage
        
        FiringState = _pynlm.FiringState
        ActionType = _pynlm.ActionType
        MotorCommand = _pynlm.MotorCommand
        WorldObjectType = _pynlm.WorldObjectType
        
        SimulationStep = _pynlm.SimulationStep
        Timestamp = _pynlm.Timestamp
        
        INVALID_NEURON_ID = _pynlm.INVALID_NEURON_ID
        INVALID_SYNAPSE_ID = _pynlm.INVALID_SYNAPSE_ID
        INVALID_REGION_ID = _pynlm.INVALID_REGION_ID
        INVALID_POPULATION_ID = _pynlm.INVALID_POPULATION_ID
        
        print("✓ Successfully imported NLM C++ bindings")
    else:
        print("⚠ Warning: Could not import NLM C++ bindings. Some functionality will be limited.")
        print("  Make sure the package is built: pip install .")

# Initialize core classes
_init_core_classes()

# Module exports
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'SimpleWorld',
    'AgentBrain',
    
    # Sensory classes
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    
    # Motor classes
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    
    # Agent classes
    'SensoryPercept',
    
    # Enhanced classes
    'WorkingMemoryEntry',
    'EpisodicEntry',
    'AssociativeMemoryEntry',
    'NeuromodulationState',
    'PredictionEntry',
    'DevelopmentalStageInfo',
    
    # Utility classes
    'NLMUtils',
    'MemoryIntegrator',
    'ExperimentRunner',
    'NLM',
    
    # Convenience functions
    'print_banner',
    'create_default_config',
    'create_brain',
    'create_simple_world',
    
    # C++ type definitions
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
    'SimulationStep',
    'Timestamp',
    
    # Invalid ID constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
]
