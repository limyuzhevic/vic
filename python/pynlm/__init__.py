"""
NLM (Neural Learning Machine) Python Bindings
===============================================

A Python binding for the NLM C++ neural simulation framework. This package provides
access to a sophisticated artificial brain simulation with neural development, learning,
memory, and cognition capabilities.

Features:
- Neural network simulation with biological learning rules
- Neuromodulation (dopamine, curiosity, novelty detection)
- Working memory and episodic memory systems
- Developmental stages and synaptic plasticity
- Environment interaction and motor control
- Prediction and planning capabilities
- Checkpoint saving and loading for persistence
- Complete Python API for easy experimentation

Usage:
    import pynlm
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for i in range(100):
        brain.step(i)
    
Quick examples and tutorials are available in the documentation.
"""

import os
import sys
from pathlib import Path

# Add utils to path for advanced functionality
sys.path.insert(0, str(Path(__file__).parent / "utils"))

# Import main simulation components
from .core import (
    Config,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    Vision, Audio, InternalSignals, Action,
    WorldObject, AgentBody, ActionResult, SensoryPercept
)

# Import main classes
from .brain import Brain
from .agent import AgentBrain
from .world import SimpleWorld

# Import utilities
from .utils.demo import demo as _demo, run_with_world
from .utils.benchmark import main as _benchmark_main
from .utils.explore import main as _explore_main
from .utils import SimulationRunner, BrainAnalyzer, ConfigurationManager, ExplorationTool
from .simulation_utils import SimulationManager, PerformanceProfiler, StateExporter
from .error_handling import ErrorHandler, ErrorReporting, handle_error, validate_input

# Create convenience functions that will be populated by C++ bindings
# These will be overridden by actual C++ bindings if available
createBrain = None
createDefaultConfig = None
createSimpleWorld = None
createAgentBrain = None

# Constants (to be populated by C++ bindings)
INVALID_NEURON_ID = None
INVALID_SYNAPSE_ID = None
INVALID_REGION_ID = None
INVALID_POPULATION_ID = None

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Python module entry points
def demo():
    """Run demonstration of NLM capabilities."""
    _demo()

def benchmark():
    """Run benchmark suite."""
    _benchmark_main()

def explore():
    """Run exploration tool."""
    _explore_main()

# Import the actual C++ bindings
try:
    import pynlm as pynlm_cpp
    
    # Create a combined namespace
    for name in dir(pynlm_cpp):
        if not name.startswith('_'):
            __dict__[name] = getattr(pynlm_cpp, name)
    
    # Populate the convenience functions from C++ bindings
    global createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain
    global INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
    
    createBrain = pynlm_cpp.createBrain
    createDefaultConfig = pynlm_cpp.createDefaultConfig
    createSimpleWorld = pynlm_cpp.createSimpleWorld
    createAgentBrain = pynlm_cpp.createAgentBrain
    
    INVALID_NEURON_ID = pynlm_cpp.INVALID_NEURON_ID
    INVALID_SYNAPSE_ID = pynlm_cpp.INVALID_SYNAPSE_ID
    INVALID_REGION_ID = pynlm_cpp.INVALID_REGION_ID
    INVALID_POPULATION_ID = pynlm_cpp.INVALID_POPULATION_ID
    
except ImportError:
    pass

# Auto-setup function
def auto_setup():
    """Auto-setup dependencies and check environment."""
    print("NLM Python Package Auto-Setup")
    print(f"Version: {__version__}")
    
    # Check for essential dependencies
    try:
        import numpy
        print(f"✓ NumPy: {numpy.__version__}")
    except ImportError:
        print("⚠ NumPy not available - some features may be limited")
    
    # Check C++ bindings
    try:
        import pynlm as pynlm_cpp
        print("✓ C++ bindings available")
        
        # Populate the convenience functions from C++ bindings
        global createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain
        global INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
        
        createBrain = pynlm_cpp.createBrain
        createDefaultConfig = pynlm_cpp.createDefaultConfig
        createSimpleWorld = pynlm_cpp.createSimpleWorld
        createAgentBrain = pynlm_cpp.createAgentBrain
        
        INVALID_NEURON_ID = pynlm_cpp.INVALID_NEURON_ID
        INVALID_SYNAPSE_ID = pynlm_cpp.INVALID_SYNAPSE_ID
        INVALID_REGION_ID = pynlm_cpp.INVALID_REGION_ID
        INVALID_POPULATION_ID = pynlm_cpp.INVALID_POPULATION_ID
        
    except ImportError:
        print("✗ C++ bindings not available - package needs to be built")
    
    print("Auto-setup completed")

# Version check function
def version_check():
    """Check NLM version and dependencies."""
    print("=== NLM Python Package Information ===")
    print(f"Version: {__version__}")
    print(f"Author: {__author__}")
    print(f"Description: {__description__}")
    
    auto_setup()

if __name__ == "__main__":
    version_check()
