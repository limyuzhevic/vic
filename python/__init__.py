# NLM Python Package - Enhanced Pythonic API
# 
# This package provides a Pythonic interface to the NLM (Neural Learning Machine) framework.
# It includes enhanced features, better error handling, and advanced utilities for both beginners and experts.

from pynlm.core import *
from pynlm.brain import *
from pynlm.world import *
from pynlm.agent import *
from pynlm.config import *
from pynlm.utils import *

__version__ = "1.0.0-enhanced"
__author__ = "NLM Project"
__description__ = "Enhanced Python bindings for NLM neural simulation framework with Pythonic API and advanced features"

# Version information
import sys
print(f"NLM Python Package v{__version__}")
print(f"Python {sys.version}")

# Performance optimizations
import numpy as np
print("NumPy available:", np.__version__ if hasattr(np, '__version__') else "Available")

# Backward compatibility
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Classes
    'Config', 'Brain', 'PythonicBrain', 'SimpleWorld', 'AgentBrain',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    
    # Enums
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 
    'createAgentBrain', 'createOptimizedConfig',
    
    # Advanced utilities
    'BrainSession', 'formatNeuronStats', 'runAgentSimulation',
    'validateConfig', 'exportConfigToJson', 'importConfigFromJson',
    
    # Version and info
    '__version__', '__author__', '__description__'
]