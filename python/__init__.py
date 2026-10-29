# NLM Python Module

# This file makes the python directory a proper Python package

__version__ = "0.1.0"

from .bindings import *

# Make all the main classes and functions available at module level
# This provides a clean and intuitive API for users

__all__ = [
    # Core classes
    'Config',
    'Brain', 
    'AgentBrain',
    'SimpleWorld',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Enumerations
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID', 
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Utility functions
    'validate_key',
    'validate_value',
    'parse_config_value',
    'safe_config_operation',
    
    # Version info
    '__version__',
    '__author__',
    '__description__',
    '__url__',
    '__license__',
]

# Module-level documentation
def __repr__():
    return "<pynlm: Python bindings for NLM (Neural Learning Machine)>"

def __str__():
    return "pynlm Python module for NLM neural simulation"

# Provide a helpful message when the module is imported
print("NLM Python Module loaded successfully!")
print("Import any of the following classes for use:")
print("  - Config")
print("  - Brain") 
print("  - AgentBrain")
print("  - SimpleWorld")
print("  - SensoryInput (and Vision, Audio, InternalSignals)")
print("  - Action")
print("  - WorldObject")
print("  - AgentBody")
print("  - And more...")
print("\nSee the documentation for examples and usage patterns.")