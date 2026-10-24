from .core import *
from .brain import *
from .agent import *
from .world import *
from .sensory import *
from .motor import *
from .memory import *
from .plasticity import *
from .development import *
from .neuromodulation import *
from .prediction import *
from .visualization import *
from .experiments import *

__all__ = [
    # Core
    'Config', 'RandomGenerator', 'SimulationClock', 'Logger',
    # Brain
    'Brain', 'Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
    # Agent
    'AgentBrain', 'AgentBody', 'SensoryPercept',
    # World
    'SimpleWorld', 'Environment', 'Observation',
    # Sensory
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    # Motor
    'MotorSystem', 'Action',
    # Memory
    'WorkingMemory', 'EpisodicMemory', 'SemanticMemory', 'ProceduralMemory', 'AssociativeMemory',
    # Plasticity
    'PlasticityRule', 'HebbianRule', 'STDP', 'StructuralPlasticity',
    # Development
    'DevelopmentSystem', 'Synaptogenesis', 'Pruning', 'Maturation',
    # Neuromodulation
    'Neuromodulator', 'Dopamine', 'Reward', 'Novelty', 'Curiosity',
    # Prediction
    'PredictionSystem',
    # Visualization
    'VisualizationInterface',
    # Experiments
    'Experiment', 'ExperimentRunner',
    # Types
    'NeuronId', 'SynapseId', 'NeuronType', 'RegionId', 'PopulationId',
    'MembranePotential', 'SynapticWeight', 'FiringRate', 'FiringState',
    'Timestamp', 'TimestepDuration', 'SimulationStep', 'PlasticityFlags',
    'ActionType', 'ObservationData',
    # Constants
    'DEFAULT_NEURON_COUNT', 'DEFAULT_REGION_COUNT',
    'PLASTICITY_LEARNING_RATE', 'STDP_LTP_WEIGHT', 'STDP_LTD_WEIGHT',
]

# Version information
__version__ = "6.0.0"
__description__ = "Neural Learning Machine - Python API"

# Quick start guide
def quick_start():
    """Print quick start guide for NLM Python API"""
    guide = """
NLM Python API Quick Start
==========================

1. Create a brain:
brain = Brain(neuron_count=100, region_count=2)

2. Configure:
brain.set('simulation_timestep', 0.001)
brain.set('plasticity_learning_rate', 0.01)

3. Create an environment:
env = SimpleWorld(width=10, height=10)

4. Run simulation:
for step in range(1000):
    brain.step(step)
    env.reset()
    # Add experiment recording if needed
    # brain.save_results('experiment_data.nvm')

5. Export data:
    # Access brain data
    neurons = brain.getNeurons()
    regions = brain.getRegions()
    data = brain.getCurrentState()
    
    # Use with agent's sensory system
    observation = env.reset()
    brain.receiveSensoryInput(observation)
    
    # Get actions
    action = brain.produceAction()
    env.step(action)

Benefits of this API:
- Object-oriented interface
- Type-safe parameter passing
- Integration with Python's scientific ecosystem
- Access to all NLM capabilities
- Easy to use for beginners
- Extensible for advanced users
"""
    print(guide)

# Simple example function
def create_sample_brain(neuron_count=100, region_count=2):
    """Create a sample brain with default configuration"""
    from .brain import Brain
    
    config = {
        'neuron_count': neuron_count,
        'region_count': region_count,
        'simulation_timestep': 0.001,
        'plasticity_learning_rate': 0.01,
        'plasticity_stdp_ltp_weight': 0.01,
        'plasticity_stdp_ltd_weight': 0.012,
        'development_synaptogenesis_rate': 0.001,
        'development_pruning_rate': 0.0001,
    }
    
    brain = Brain(config)
    return brain
