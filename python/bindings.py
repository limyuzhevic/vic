"""
C++ bindings wrapper for NLM Python module

This module provides Python wrappers around the C++ bindings defined in bindings.cpp.
It ensures proper Python module initialization and provides additional
convenience functions.
"""

import sys
import importlib.util
import os

# Try to import the compiled module
try:
    # Try to import pynlm from the current directory
    spec = importlib.util.spec_from_file_location(
        "_pynlm_c", 
        os.path.join(os.path.dirname(__file__), "_pynlm_c.so")
    )
    
    if spec is not None:
        _pynlm_c = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(_pynlm_c)
        
        # Copy attributes from C++ module
        __all__ = list(getattr(_pynlm_c, "__all__", []))
        
        # Copy all public attributes
        for name in dir(_pynlm_c):
            if not name.startswith("_"):
                setattr(sys.modules[__name__], name, getattr(_pynlm_c, name))
                
    else:
        raise ImportError("Could not load compiled pynlm module")
        
except ImportError as e:
    # If compiled module is not available, provide helpful error message
    print(f"Warning: Could not load compiled NLM Python bindings: {e}")
    print("Please install the package properly using: pip install -e .")
    
    # Create placeholder classes for development/testing
    class MockModule:
        def __getattr__(self, name):
            raise ImportError(
                f"NLM Python bindings not available. "
                f"Please install the package: pip install -e .\n"
                f"Original error: {name}"
            )
    
    # Create placeholder classes for documentation purposes
    class Config:
        def __init__(self):
            pass
        def loadFromFile(self, filepath):
            raise NotImplementedError("Please install pynlm properly")
        def loadFromArgs(self, *args):
            raise NotImplementedError("Please install pynlm properly")
        def saveToFile(self, filepath):
            raise NotImplementedError("Please install pynlm properly")
        def has(self, key):
            return False
        def getKeys(self):
            return []
        def clear(self):
            pass
        def summary(self):
            return "<Mock Config>"
        def set(self, key, value, source=None):
            pass
        def getOr(self, key, default):
            return default
    
    # Add placeholder classes to module
    for name in [
        "Config", "Brain", "AgentBrain", "SimpleWorld", "Vision", 
        "Audio", "InternalSignals", "SensoryInput", "Action", "WorldObject",
        "AgentBody", "ActionResult", "SensoryPercept", "NeuronId", 
        "SynapseId", "RegionId", "PopulationId", "NeuronType", "SynapseType",
        "DevelopmentalStage", "FiringState", "ActionType", "MotorCommand",
        "WorldObjectType", "NLMException", "ConfigurationError", 
        "SimulationError", "MemoryError"
    ]:
        setattr(sys.modules[__name__], name, locals()[name])
    
    # Add factory functions
    def createDefaultConfig():
        return Config()
    
    def createBrain(configPath=""):
        brain = Brain()
        brain.initialize()
        return brain
    
    def createWorld(*args, **kwargs):
        return SimpleWorld()
    
    def createAgentBrain(brain):
        return AgentBrain(brain)
    
    # Add utility functions
    def normalizeVector(data, min_val=0.0, max_val=1.0):
        return data
    
    def vectorToString(data, delimiter=", "):
        return str(data)
    
    def validateConfig(config):
        pass
    
    def isPositiveNumber(value):
        return value >= 0
    
    def isPositiveInt(value):
        return value > 0
    
    def getVersion():
        return "0.1.0-mock"
    
    # Add constants
    INVALID_NEURON_ID = 0
    INVALID_SYNAPSE_ID = 0
    INVALID_REGION_ID = 0
    INVALID_POPULATION_ID = 0
    DEFAULT_SIMULATION_TIMESTEP = 0.001
    DEFAULT_RANDOM_SEED = 42
    DEFAULT_NEURON_COUNT = 100
    DEFAULT_REGION_COUNT = 1
    
    # Add to locals for __all__
    for name in [
        "createDefaultConfig", "createBrain", "createWorld", "createAgentBrain",
        "normalizeVector", "vectorToString", "validateConfig", "isPositiveNumber",
        "isPositiveInt", "getVersion", "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID",
        "INVALID_REGION_ID", "INVALID_POPULATION_ID", "DEFAULT_SIMULATION_TIMESTEP",
        "DEFAULT_RANDOM_SEED", "DEFAULT_NEURON_COUNT", "DEFAULT_REGION_COUNT"
    ]:
        setattr(sys.modules[__name__], name, locals()[name])
    
    print("Using mock module for development. Install with: pip install -e .")