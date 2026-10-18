"""
NLM (Neural Learning Machine) Python Bindings
A Python binding for the NLM C++ neural simulation framework.

This module provides access to a brain-inspired spiking neural network simulator
that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics,
synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning,
and structural plasticity.

The brain operates as a closed-loop system with sensory processing, neural computation,
memory, prediction, neuromodulation, and action selection.
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__copyright__ = "Copyright (c) 2024 NLM Authors"

# Import core C++ bindings
from .core import (
    Brain,
    AgentBrain,
    SimpleWorld,
    Config,
    Action,
    SensoryPercept,
    WorldObject,
    Vision,
    Audio,
    InternalSignals,
    # Identifier types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    # Utility functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Configuration
__all__ = [
    # Version
    "__version__",
    "__author__",
    "__license__",
    "__copyright__",
    
    # Core classes
    "Brain",
    "AgentBrain", 
    "SimpleWorld",
    "Config",
    "Action",
    "SensoryPercept",
    "WorldObject",
    "Vision",
    "Audio",
    "InternalSignals",
    
    # Identifier types
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Enumerations
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
]

# Pythonic utilities
try:
    from .core import run_simulation
    from .core import brain_stats
    from .core import enable_features
    from .core import run_complete_agent_simulation
    
    __all__.extend([
        "run_simulation",
        "brain_stats", 
        "enable_features",
        "run_complete_agent_simulation",
    ])
except ImportError:
    # Fall back if enhanced modules aren't available yet
    pass

# Try to import enhanced modules
import sys
import importlib

# Optional enhanced modules
_optional_modules = [
    "stats",
    "experiments", 
    "testing",
    "config_helpers",
]

_forbidden_modules = set(_optional_modules)

if "PYNLm_ADVANCED_FEATURES" in sys.modules or \
   "PYNLm_ADVANCED_FEATURES" in os.environ.get("PYNLm_ADVANCED_FEATURES", ""):
    # Enable advanced features if explicitly requested
    for module_name in _optional_modules:
        try:
            module = importlib.import_module(f".{module_name}", __name__)
            globals().update({name: getattr(module, name) for name in dir(module) 
                            if not name.startswith('_')})
            
            # Add to __all__ if module has __all__
            if hasattr(module, "__all__"):
                __all__.extend(module.__all__)
                
        except ImportError:
            pass

# Error handling
class NLMError(Exception):
    """Base exception class for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration or setup."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class MemoryError(NLMError):
    """Error related to memory operations."""
    pass

class ValidationError(NLMError):
    """Error in input validation."""
    pass

# Performance monitoring
def _get_performance_stats(obj):
    """Get performance statistics for an object."""
    stats = {}
    if hasattr(obj, '__dict__'):
        stats['attributes'] = list(obj.__dict__.keys())
    return stats

# Common utility functions
def get_version_info():
    """Get detailed version and information about the NLM installation."""
    return {
        'version': __version__,
        'author': __author__,
        'license': __license__,
        'backend': 'C++ with pybind11',
        'python_support': '>=3.8',
        'features': [
            'spiking neural networks',
            'LIF neurons',
            'event-driven dynamics',
            'multiple plasticity mechanisms',
            'memory systems',
            'prediction systems',
            'cognition systems',
            'neuromodulation',
            'development system',
        ]
    }

# Context manager utilities
class BrainSimulator:
    """
    Context manager for managing brain simulations.
    
    Example:
        with BrainSimulator() as simulator:
            brain = simulator.create_brain()
            brain.initialize()
            for i in range(100):
                brain.step(i)
    """
    def __init__(self, config=None):
        self.config = config
        self.brain = None
        self.world = None
        self.agent = None
        
    def __enter__(self):
        self.config = self.config or createDefaultConfig()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset()
        
    def create_brain(self, config=None):
        """Create a brain with the given configuration."""
        if config is not None:
            self.config = config
        self.brain = createBrain(self.config)
        return self.brain
        
    def create_world(self, width=20, height=20, vision_width=8, vision_height=8):
        """Create a simple world."""
        self.world = createSimpleWorld()
        self.world.configure(width, height, vision_width, vision_height)
        return self.world
        
    def create_agent(self):
        """Create an agent brain interface."""
        if self.brain is None:
            self.brain = createBrain(createDefaultConfig())
            self.brain.initialize()
        self.agent = createAgentBrain(self.brain)
        return self.agent
        
    def run_episode(self, num_steps=1000, enable_features=None):
        """Run a complete episode with brain and world interaction."""
        if self.agent is None:
            self.create_agent()
        if self.world is None:
            self.create_world()
            
        if enable_features:
            enable_features(self.agent, enable_features)
            
        for step in range(num_steps):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
        return brain_stats(self.brain)
        
    def reset(self):
        """Reset all components."""
        if self.brain:
            self.brain.reset()
        if self.world:
            self.world.reset()
        if self.agent:
            self.agent.reset()
        self.brain = None
        self.world = None
        self.agent = None