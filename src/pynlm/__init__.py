# NLM Python Module
# Main entry point for the NLM Python API

__version__ = "0.1.0"

# Import submodules
import importlib

# Create submodule references
_core = importlib.import_module('.core')
_brain = importlib.import_module('.brain')
_agent = importlib.import_module('.agent')
_world = importlib.import_module('.world')
_memory = importlib.import_module('.memory')
_prediction = importlib.import_module('.prediction')
_cognition = importlib.import_module('.cognition')
_neuromodulation = importlib.import_module('.neuromodulation')
_development = importlib.import_module('.development')
_sensory = importlib.import_module('.sensory')
_motor = importlib.import_module('.motor')
_environment = importlib.import_module('.environment')

# Import everything from submodules
from .core import *
from .brain import *
from .agent import *
from .world import *
from .memory import *
from .prediction import *
from .cognition import *
from .neuromodulation import *
from .development import *
from .sensory import *
from .motor import *
from .environment import *

# Convenience functions
def createDefaultConfig():
    """Create a default NLM configuration"""
    return Config.create_default()

def createBrain(config=None):
    """Create an NLM brain"""
    if config is None:
        config = createDefaultConfig()
    return Brain(config)

def createSimpleWorld():
    """Create a simple NLM world"""
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface"""
    return AgentBrain(brain)

# Make submodules available
__all__ = [
    # Core classes (from core.py)
    'Config',
    
    # Types (from core.py)
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Main classes (from brain.py)
    'Brain', 'Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
    
    # Agent and world classes (from agent.py and world.py)
    'AgentBrain', 'AgentBody', 'SensoryPercept', 'SimpleWorld',
    'WorldObject', 'Action', 'SensoryInput', 'Vision', 'Audio',
    'InternalSignals',
    
    # Memory classes (from memory.py)
    'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    
    # Prediction classes (from prediction.py)
    'PredictionSystem', 'NeuralPrediction', 'ActionConsequencePredictor',
    'PredictionErrorSignal',
    
    # Cognition classes (from cognition.py)
    'ConceptFormation', 'NeuralPlanner', 'AttentionalSelection',
    
    # Development classes (from development.py)
    'DevelopmentSystem', 'Synaptogenesis', 'Pruning', 'Maturation',
    
    # Neuromodulation classes (from neuromodulation.py)
    'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    'Neuromodulator', 'HebbianRule', 'STDP', 'StructuralPlasticity',
    
    # Convenience functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain',
    
    # Submodules
    'core', 'brain', 'agent', 'world', 'memory', 'prediction',
    'cognition', 'neuromodulation', 'development', 'sensory',
    'motor', 'environment'
]
