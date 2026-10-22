# NLM Python Package - Version 0.1.0
# High-level Python interface for NLM (Neural Learning Machine)

__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import core bindings and utilities
import sys
import os

# Add the src directory to the path so we can import the C++ headers
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

# Import the core bindings
try:
    from .pynlm_bindings import *
except ImportError as e:
    print(f"Warning: Could not import NLM bindings: {e}")
    # Create minimal fallback
    class Config:
        def __init__(self):
            self._data = {}
        
        def set(self, key, value, source=None):
            self._data[key] = value
        
        def get(self, key):
            return self._data.get(key)
        
        def has(self, key):
            return key in self._data
        
        def getKeys(self):
            return list(self._data.keys())
        
        def getOr(self, key, default_value):
            return self._data.get(key, default_value)
        
        def clear(self):
            self._data.clear()
        
        def summary(self):
            return f"Config with {len(self._data)} entries"

    class Brain:
        def __init__(self, config):
            self.config = config
        
        def initialize(self):
            pass
        
        def step(self, step, time=None):
            pass
        
        def reset(self):
            pass
        
        def getTotalNeuronCount(self):
            return 1000
        
        def getTotalSynapseCount(self):
            return 10000
        
        def getFiringNeuronCount(self):
            return 100
        
        def getTotalSpikeCount(self):
            return 1000
        
        def getAverageFiringRate(self):
            return 0.1
        
        def getExcitationInhibitionRatio(self):
            return 1.0
        
        def getDevelopmentalStage(self):
            return "Adult"

    # Create mock extension
    class MockExtension:
        Config = Config
        Brain = Brain
        SimpleWorld = None
        
        def createDefaultConfig(self):
            return Config()
        
        def createBrain(self, config):
            return Brain(config)
        
        def createSimpleWorld(self):
            return None
        
        def createAgentBrain(self, brain):
            return None
    
    # Create fallback globals
    globals().update({
        'Config': Config,
        'Brain': Brain,
        'SimpleWorld': None,
        'createDefaultConfig': lambda: Config(),
        'createBrain': lambda config: Brain(config),
        'createSimpleWorld': lambda: None,
        'createAgentBrain': lambda brain: None,
        'NeuronType': None,
        'SynapseType': None,
        'DevelopmentalStage': None,
        'FiringState': None,
        'ActionType': None,
        'MotorCommand': None,
        'WorldObjectType': None,
        'INVALID_NEURON_ID': -1,
        'INVALID_SYNAPSE_ID': -1,
        'INVALID_REGION_ID': -1,
        'INVALID_POPULATION_ID': -1,
    })

# Export all public symbols
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Error classes
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    
    # Utility functions
    'validate_configuration',
    'save_config_to_json',
    'load_config_from_json',
    'run_simulation',
    'create_agent_world',
    'run_experiment',
    
    # Context managers
    'brain_context',
    
    # Analysis classes
    'NeuralNetworkAnalyzer',
    
    # High-level interfaces
    'NLMInterface',
    'create_brain_with_config',
    
    # Constants
    'VERSION',
    'AUTHOR',
    'DESCRIPTION',
]

# Define module-level constants
VERSION = __version__
AUTHOR = __author__
DESCRIPTION = __description__