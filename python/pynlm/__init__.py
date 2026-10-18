"""
NLM (Neural Learning Machine) Python Bindings
A Python interface to the NLM C++ neural simulation framework.

This module provides classes and functions for creating and simulating virtual brains
using spiking neural networks with learning capabilities.

Key Features:
- Virtual brain creation and initialization
- Sensory-motor integration
- Learning through plasticity mechanisms
- Developmental stage progression
- World simulation and interaction
- Performance monitoring and benchmarking
- Configuration management
- Brain registry system
- Visualization tools
"""

import warnings
from typing import Optional, List, Dict, Any, Union
import numpy as np

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "nlm-project@nlm.org"

# Configure warnings
warnings.filterwarnings("always", category=UserWarning, module="pynlm")

# Import core classes from C++ bindings
try:
    from pynlm_core import (
        Brain,
        Config,
        AgentBrain,
        SimpleWorld,
        Vision,
        Audio,
        InternalSignals,
        SensoryInput,
        SensoryPercept,
        Action,
        WorldObject,
        AgentBody,
        ActionResult,
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
    )
    
    # Import factory functions
    from pynlm_core import (
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
    # Core module is successfully imported
    _core_available = True
    
except ImportError as e:
    # Core module not available (C++ bindings not built)
    _core_available = False
    
    # Define placeholder classes and functions
    class Brain:
        def __init__(self, *args, **kwargs):
            raise ImportError("pynlm_core module not available. "
                           "Please build the C++ bindings first.")
    
    class Config:
        def __init__(self, *args, **kwargs):
            raise ImportError("pynlm_core module not available. "
                           "Please build the C++ bindings first.")
    
    # Create placeholder functions that raise informative errors
    def _create_placeholder(func_name):
        def placeholder(*args, **kwargs):
            raise ImportError(f"{func_name} requires the C++ bindings to be built. "
                           f"Please build the pynlm module first.")
        return placeholder
    
    # Create placeholder factory functions
    createDefaultConfig = _create_placeholder("createDefaultConfig")
    createBrain = _create_placeholder("createBrain")
    createSimpleWorld = _create_placeholder("createSimpleWorld")
    createAgentBrain = _create_placeholder("createAgentBrain")
    
    # Create placeholder constants
    INVALID_NEURON_ID = None
    INVALID_SYNAPSE_ID = None
    INVALID_REGION_ID = None
    INVALID_POPULATION_ID = None
    
    # Create placeholder classes for enums
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
    
    # Create placeholder other classes
    for class_name in ['Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'SensoryPercept',
                      'Action', 'WorldObject', 'AgentBody', 'ActionResult',
                      'NeuronId', 'SynapseId', 'RegionId', 'PopulationId']:
        globals()[class_name] = type(class_name, (), {})

# Import helper classes
from .brain_config import BrainConfig
from .simulation_metrics import SimulationMetrics
from .performance_monitor import PerformanceMonitor
from .nlm_exception import NLMException
from .registration_system import (
    register_brain, get_brain, unregister_brain, list_brains
)
from .experiment_helper import run_experiment
from .benchmark_helpers import benchmark_brain_creation, benchmark_simulation
from .examples import examples

# Create module-level constants from C++ constants (or placeholders)
# These will be set to actual values when core is available
# For now, create placeholder objects that will work at import time
if _core_available:
    # Core is available, use actual values
    _C_FN = lambda name: getattr(sys.modules.get('pynlm_core'), name, None)
    INVALID_NEURON_ID = _C_FN('INVALID_NEURON_ID')
    INVALID_SYNAPSE_ID = _C_FN('INVALID_SYNAPSE_ID')
    INVALID_REGION_ID = _C_FN('INVALID_REGION_ID')
    INVALID_POPULATION_ID = _C_FN('INVALID_POPULATION_ID')
else:
    # Core not available, create placeholder objects
    class _PlaceholderID:
        def __init__(self, value):
            self.value = value
        def __str__(self):
            return f"<PlaceholderID: {self.value}>"
    
    INVALID_NEURON_ID = _PlaceholderID(0)
    INVALID_SYNAPSE_ID = _PlaceholderID(0)
    INVALID_REGION_ID = _PlaceholderID(0)
    INVALID_POPULATION_ID = _PlaceholderID(0)

# Global configuration
_default_config: Optional['Config'] = None
_brain_registry: Dict[str, 'Brain'] = {}
_world_registry: Dict[str, 'SimpleWorld'] = {}
_agent_registry: Dict[str, 'AgentBrain'] = {}

# Re-export everything for easy access
__all__ = [
    # Main classes
    "Brain",
    "Config", 
    "AgentBrain",
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryInput",
    "SensoryPercept",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
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
    # Helper classes and utilities
    "NLMException",
    "BrainConfig",
    "SimulationMetrics",
    "PerformanceMonitor",
    # Global functions
    "register_brain",
    "get_brain",
    "unregister_brain",
    "list_brains",
    "run_experiment",
    "benchmark_brain_creation",
    "benchmark_simulation",
    # Examples
    "examples",
]

# Set up default config if needed
def _get_default_config() -> 'Config':
    """Get or create the default configuration."""
    global _default_config
    if _default_config is None:
        _default_config = createDefaultConfig()
    return _default_config

# Module cleanup function
def cleanup() -> None:
    """Clean up module resources."""
    global _brain_registry, _world_registry, _agent_registry
    
    # Reset all registered brains
    for name, brain in _brain_registry.items():
        try:
            brain.reset()
        except Exception:
            pass
    
    _brain_registry.clear()
    _world_registry.clear()
    _agent_registry.clear()
    _default_config = None

# Register cleanup with Python exit
import atexit
atexit.register(cleanup)

# Print module info when imported in debug mode
if __name__ == "__main__":
    print(f"NLM Python module version {__version__}")
    print("Type 'help(pynlm)' for more information.")
    
    # Test core availability
    if _core_available:
        print("Core C++ bindings: Available")
    else:
        print("Core C++ bindings: Not available")
        print("Please build the pynlm module first.")

# Add module metadata
__doc__ = f"""
NLM (Neural Learning Machine) Python Bindings
Version: {__version__}
Author: {__author__}

This module provides a comprehensive Python interface for the NLM neural simulation framework.

Installation:
    pip install pynlm

Basic Usage:
    import pynlm
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)

Advanced Usage:
    # Use helper classes
    config = pynlm.BrainConfig.with_parameters(neuron_count=2000)
    brain = pynlm.createBrain(config)
    
    # Monitor performance
    monitor = pynlm.PerformanceMonitor(brain)
    monitor.start()
    
    # Run experiment
    world = pynlm.createSimpleWorld()
    results = pynlm.run_experiment(config, world, num_steps=500)

Documentation:
    See pynlm.__doc__ for module documentation.
    Check the examples dictionary: pynlm.examples
"""
